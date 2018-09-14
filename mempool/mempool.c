/*
 * Copyright (c) 2018-2018 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <pthread.h>
#include "osadapter.h"
#include "mempool.h"

/**********************************************/
/*          HEAD                              */
/* |--------------------|                     */
/* | number of nodes    |      _____________  */
/* |--------------------|      |  node 1   |  */
/* | node1 head ptr     |----->|-----------|  */
/* |--------------------|      |check sum  |  */
/* | node1 tail ptr     |---|  |           |  */
/* |--------------------|   |  |-----^-----|  */
/* | node2 head ptr     |   |        |        */
/* |--------------------|   |  |-----------|  */
/* | node2 tail ptr     |   |  |   memory  |  */
/* |--------------------|   |->|-----------|  */
/*                                            */
/**********************************************/
static ST_MEMPOOL_HEAD _gstMempoolHead;
static pthread_mutex_t _gst_mutex_lock = PTHREAD_MUTEX_INITIALIZER;

void* mempool_malloc(s32 size)
{
    s32 i;
    s32 ret;
    ST_MEMPOOL_HEAD* head = getMempoolHead();
    ST_MEMPOOL_BODY* body = NULL;
    u8* ptr = NULL;

    if(size > COMMONS_MEMPOOL_MAX_POOL_SIZE){
        COMMONS_MEM_LOG("add failed,too large size");
        return -1;
    }

    pthread_mutex_lock(&_gst_mutex_lock);

    if(0 == head->iTotalBodyNum)
    {
        if(0 != mempool_init()){
            COMMONS_MEM_LOG("init failed");
            pthread_mutex_unlock(&_gst_mutex_lock);
            return -1;
        }
    }

    for(i = 0;i < head->iTotalBodyNum;i ++)
    {
        COMMONS_MEM_LOG("i:%d",i);
        if(head->iBodySize[i] + size <  COMMONS_MEMPOOL_MAX_POOL_SIZE)
        {
            if(NULL == head->ptrBodyHead[i])
            {
                ret = mempool_add();
                if(ret < 0){
                    COMMONS_MEM_LOG("add failed");
                    pthread_mutex_unlock(&_gst_mutex_lock);
                    return NULL;
                }
                i = 0;
            }
            head->iBodySize[i] += size;
            ptr = head->ptrBodyTail[i];
            head->ptrBodyTail[i] += size;
            COMMONS_MEM_LOG("add failed");
            return ptr;
        }
    }

    pthread_mutex_unlock(&_gst_mutex_lock);

    COMMONS_MEM_LOG("not enough memory");
    return NULL;
}

s32 mempool_free(void* ptr)
{
    return 0;
}

ST_MEMPOOL_HEAD* getMempoolHead()
{
    return &_gstMempoolHead;
}

s32 mempool_checksum(const u8* src,s32 src_len,u8* checksum,s32 type)
{
    //u8* ptr = NULL;

    if(NULL == src || NULL == checksum)
    {
        return -1;
    }

    return 0;
}

BOOL mempool_body_check(ST_MEMPOOL_BODY* body)
{
    return TRUE;
}

BOOL mempool_check()
{
    ST_MEMPOOL_BODY* body = NULL;
    ST_MEMPOOL_HEAD* head = getMempoolHead();
    s32 num;

    for(num = 0;num < COMMONS_MEMPOOL_MAX_POOL_NUM;num ++)
    {
        body = head->ptrBodyHead[num];
        if(mempool_body_check(body)){
            mempool_delete(body);
        }
        head->ptrBodyHead[num] = NULL;
    }
	return TRUE;
}

ST_MEMPOOL_BODY* mempool_init_body()
{
    ST_MEMPOOL_BODY* body = NULL;

    body = (ST_MEMPOOL_BODY*)commons_malloc(sizeof(ST_MEMPOOL_BODY));
    if(NULL == body)
    {
        return NULL;
    }
    BZERO(body,sizeof(ST_MEMPOOL_BODY));

    return body;
}

s32 mempool_init()
{
    ST_MEMPOOL_BODY* body = NULL;
    ST_MEMPOOL_HEAD* head = getMempoolHead();
    s32 num;

    pthread_mutex_lock(&_gst_mutex_lock);

    /*  check the validity of body,if valid,free the memory of body  */
    for(num = 0;num < COMMONS_MEMPOOL_MAX_POOL_NUM;num ++)
    {
        body = head->ptrBodyHead[num];
        if(mempool_body_check(body)){
            mempool_delete(body);
        }
        head->ptrBodyHead[num] = NULL;
    }

    BZERO(head,sizeof(ST_MEMPOOL_HEAD));

    /*  create first body for head  */
    body= mempool_init_body();
    if(NULL == body)
    {
        pthread_mutex_unlock(&_gst_mutex_lock);
        return -1;
    }
    head->iBodySize[0] = 0;
    head->ptrBodyHead[0] = body;
    head->ptrBodyTail[0] = body->ptrBody;
    head->iTotalBodyNum = 1;

    pthread_mutex_unlock(&_gst_mutex_lock);

    return 0;
}

s32 mempool_delete()
{
    return 0;
}

s32 mempool_add()
{
    ST_MEMPOOL_BODY* body = NULL;
    ST_MEMPOOL_HEAD* head = getMempoolHead();
    s32 i;
    s32 ret = -1;
    
    COMMONS_MEM_LOG("add mempool");

    pthread_mutex_lock(&_gst_mutex_lock);

    for(i = 0;i < COMMONS_MEMPOOL_MAX_POOL_SIZE;i ++)
    {
        if(NULL == head->ptrBodyHead[i]){
            body = mempool_init_body();
            if(NULL == body){
                ret = -1;
                break;
            }
            head->ptrBodyHead[i] = body;
            head->ptrBodyTail[i] = body->ptrBody;
            head->iBodySize[i] = 0;
            head->iTotalBodyNum ++;
            ret = 0;
            break;
        }
    }
    
    pthread_mutex_unlock(&_gst_mutex_lock);
    return ret;
}

