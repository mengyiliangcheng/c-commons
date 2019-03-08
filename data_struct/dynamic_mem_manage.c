/*
* Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <stdlib.h>
#include "osadapter.h"
#include "commons_log.h"
#include "dynamic_mem_manage.h"

#define LOG(...) COMMONS_LOG("MEM",__VA_ARGS__);

typedef enum
{
    TRUNCK_FREE,
    TRUNCK_OCCUPY,
}e_trunck_tag;

typedef struct WORD
{
    union
    {
        struct WORD *llink;  //head domain,previous node
        struct WORD *uplink; //tail domain,pointing to the head of node
    };
    int tag;                 //trunck flag,0:free,1:occupy
    int size;                //head domain,size of trunck
    struct WORD *rlink;      //head domain,next node
    
}WORD,head,foot,*Space;

#define FootLoc(p)                  (p + p->size -1)
#define MIN_MEMORY_MARGIN           (5)
#define MAX_MEMORY_SIZE             (10000)

Space AllocBoundTag(Space* pav,int n)
{
    Space p = NULL;
    Space f = NULL;
    if(NULL == (*pav))
    {
        LOG("param is null");
        return NULL;
    }

    for(p = *pav;p && (p->size < n) && (p->rlink != *pav);p = p->rlink);
    //LOG("find [%x]!",p);
    SpacePrintWord(p);
    
    if(!p || p->size < n)
    {
        LOG("error");
        return NULL;
    }
    else
    {
        f = FootLoc(p);
        *pav = p->rlink;
        if(p->size - n <= MIN_MEMORY_MARGIN)
        {
            //LOG("alloc full mem");
            if(*pav == p)
            {
                *pav = NULL;
            }
            else
            {
                (*pav)->llink = p->llink;
                p->llink->rlink = *pav;
            }
            p->tag = f->tag = 1;
        }
        else
        {
            //LOG("alloc part mem");
            f->tag = 1;
            p->size -= n;
            f = FootLoc(p);
            p->tag = 0;
            f->uplink = p;
            p = f + 1;
            p->tag = 1;
            p->size = n;
        }
        //LOG("alloc succ");
        return p;
    }
}

void ReclaimBoundTag(Space *pav,Space p)
{
    if(NULL == p)
    {
        return ;
    }
    if(TRUNCK_FREE == p->tag)
    {
        LOG("ReclaimBoundTag double free [%x] l:%d r:%d",p,(p-1)->tag,(p + p->size)->tag);
        return ;
    }

    LOG("ReclaimBoundTag [%x] l:%d r:%d",p,(p-1)->tag,(p + p->size)->tag);
    if(TRUNCK_OCCUPY == (p - 1)->tag && TRUNCK_OCCUPY == (p + p->size)->tag)
    {
        p->tag = 0;
        FootLoc(p)->uplink = p;
        FootLoc(p)->tag = 0;
        if(NULL == (*pav))
        {
            pav = p->llink = p->rlink = p;
        }
        else
        {
            p->rlink = *pav;
            p->llink = (*pav)->llink;
            (*pav)->llink->rlink = p;            
            (*pav)->llink = p;
            (*pav) = p;
        }
    }
    else if(TRUNCK_OCCUPY == (p - 1)->tag && TRUNCK_FREE == (p + p->size)->tag)
    {
        Space q = p + p->size;
        q->llink->rlink = p;
        p->llink = q->llink;
        p->rlink = q->rlink;
        q->rlink->llink = p;
        p->tag = TRUNCK_FREE;
        p->size += q->size;
        FootLoc(p)->tag = TRUNCK_FREE;
        FootLoc(p)->uplink = p;
    }
    else if(TRUNCK_FREE == (p - 1)->tag && TRUNCK_OCCUPY == (p + p->size)->tag)
    {
        Space q = (p - 1)->uplink;
        q->size += p->size;
        FootLoc(q)->tag = TRUNCK_FREE;
        FootLoc(q)->uplink = q;
    }
    else
    {
        //Space q = (p - 1)->uplink;
        Space q = (p + p->size);
        q->llink->rlink = q->rlink;
        q->rlink->llink = q->llink;

        q = (p - 1)->uplink;
        q->size += p->size;
        q->size += (p + p->size)->size;
        FootLoc(q)->tag = TRUNCK_FREE;
        FootLoc(q)->uplink = q;
        q->tag = TRUNCK_FREE;
    }
}

void SpaceInit(Space* freeSpace,Space* pav)
{
    *freeSpace = (Space)calloc(sizeof(struct WORD),MAX_MEMORY_SIZE + 2);
    if(NULL == *freeSpace)
    {
        return ;
    }

    Space head = *freeSpace;
    head->tag = 1;
    head ++;
    head->tag = 0;
    head->llink = head->rlink = head;
    head->size = MAX_MEMORY_SIZE;
    *pav = head;
    Space foot = FootLoc(head);
    foot->tag = 0;
    foot->uplink = head;
    foot++;
    foot->tag = 1;
}

void SpacePrintWord(Space s)
{
    LOG("The word head is [%0x] SIZE: [%d]  pre is [%0x] ,next is [%0x] ,tag is [%d] ,size is [%d]",
        s,s->size,s->llink,s->rlink,s->tag,s->size);
}

void SpacePrintStruct(Space pav)
{
    if(NULL == pav)
    {
        LOG("param is null");
        return ;
    }

    LOG("________start_________");
    Space pos = pav;
    LOG("free list pav:[%0x] ,size: [%5d] ,pre is [%0x], next is [%0x] ,tag is [%d]",pav,pav->size,pav->llink,pav->rlink,pav->tag);
    for(pos = pos->rlink;pos != pav;pos = pos->rlink)
    {
        LOG("free list pos:[%0x] ,size: [%5d] ,pre is [%0x], next is [%0x] ,tag is [%d]",pos,pos->size,pos->llink,pos->rlink,pos->tag);
    }
    LOG("________end_________");
}

void dynamic_mem_manage_main(void)
{
    Space pav = NULL;
    Space freeSpace = NULL;
    Space avaible_data = NULL;
    
    SpaceInit(&freeSpace,&pav);
    //LOG("word size:%d,%d,%d,%d,%d",sizeof(head),sizeof(pav->tag),sizeof(pav->size),sizeof(int *),sizeof(pav->llink));
    SpacePrintWord(pav);
    SpacePrintStruct(pav);

    avaible_data = AllocBoundTag(&pav,10);
    Space avaible_data1 = AllocBoundTag(&pav,10);
    avaible_data = AllocBoundTag(&pav,10);
    avaible_data = AllocBoundTag(&pav,10);
    Space avaible_data2 = AllocBoundTag(&pav,10);
    avaible_data = AllocBoundTag(&pav,10);
    avaible_data = AllocBoundTag(&pav,20);

    ReclaimBoundTag(&pav,avaible_data1);
    ReclaimBoundTag(&pav,avaible_data2);
    SpacePrintStruct(pav);
    ReclaimBoundTag(&pav,avaible_data2);
    SpacePrintStruct(pav);
    avaible_data = AllocBoundTag(&pav,20);
    Space avaible_data3 = AllocBoundTag(&pav,90);
    avaible_data = AllocBoundTag(&pav,20);
    avaible_data = AllocBoundTag(&pav,20);
    ReclaimBoundTag(&pav,avaible_data3);
    SpacePrintStruct(pav);
    
}


