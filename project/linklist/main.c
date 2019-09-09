/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/07/22  automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <time.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

#define LIST_ADD(a,b)       list_add((stLinklist*)a,(stLinklist*)b)
#define LIST_DEL(a,b,c)     list_delete((stLinklist*)a,b,(stLinklist**)c)
#define LIST_FIND(a,b)      list_find((stLinklist*)a,b)
#define LIST_COUNT(a)       list_count((stLinklist*)a)

typedef struct _stLinklist
{
    struct _stLinklist* next;
    int id;
}stLinklist;

typedef struct _stTest
{
    struct _stTest* next;
    int id;
    int dirty;
    int value;
}stTest;

stLinklist* list_add(stLinklist* head,stLinklist* node)
{
    stLinklist* target;
    if(NULL == head)
    {
        return node;
    }

    if(NULL == node)
        return head;

    target = head;
    if(target == NULL || target->id > node->id)
    {
        node->next = target;
        return node;
    }

    for(;target->next != NULL && target->next->id <= node->id ;target = target->next)
    {}

    node->next = target->next;
    target->next = node;

    return head;
}

stLinklist* list_delete(stLinklist* head,int id,stLinklist** delete)
{
    stLinklist* target;
    if(NULL == head || NULL == delete)
    {
        return head;
    }

    *delete = NULL;

    if(head->id == id)
    {
        *delete = head;
        return head->next;
    }

    target = head;
    while(target->next != NULL)
    {
        if(target->next->id == id)
        {
            *delete = target->next;
            target->next = target->next->next;
        }
        target = target->next;
    }

    return head;
}

stLinklist* list_find(stLinklist* head,int id)
{
    stLinklist* target;
    if(NULL == head)
    {
        return NULL;
    }
    if(head->id == id)
    {
        return head;
    }

    target = head->next;
    while(target != NULL)
    {
        if(target->id == id)
            return target;
        target = target->next;
    }
    
    return NULL;
}


stLinklist* list_get(void)
{
    stLinklist* node = (stLinklist*)malloc(sizeof(stLinklist));
    memset(node,0,sizeof(stLinklist));
    node->id = rand() % 1000;

    return node;
}

int list_count(stLinklist* head)
{
   int num = 0;
   while(NULL != head)
   {
        num ++;
        head = head->next;
   }

   return num;
}

void list_print(stLinklist* head)
{
    srand(time(NULL));
    while(head != NULL)
    {
        printf("%d ",head->id);
        head = head->next;
    }
}

void test_print(stTest* head)
{
    srand(time(NULL));
    while(head != NULL)
    {
        printf("[%d %d] ",head->id,head->value);
        head = head->next;
    }
    printf("\n");
}

stTest* test_get(void)
{
    static int cnt = 0;
    stTest* node = (stTest*)malloc(sizeof(stTest));
    memset(node,0,sizeof(stTest));
    node->id = rand() % 1000;
    node->value = cnt++;
    return node;
}


int main()
{
    stTest* head = NULL;
    stTest* tmp = NULL;
    int i;
    printf("hello world\n");

    for(i = 0;i < 100;i ++)
    {
        tmp = test_get();
        head = LIST_ADD(head,tmp);
    }
    printf("time:%d\n",time(NULL));

    test_print(head);

    int num = 0;
    printf("DELETE:pls input:\n");
    scanf("%d",&num);

    
    head = LIST_DEL(head,num,&tmp);
    if(NULL != tmp)
        printf("\ndelete :%d\n",tmp->id);
    else
        printf("\n%d not exist\n",num);
    test_print(head);

    printf("FIND:pls input:\n");
    scanf("%d",&num);
    tmp = LIST_FIND(head,num);
    if(NULL != tmp)
        printf("find %d value:%d\n",tmp->id,tmp->value);
    else
        printf("can not find %d\n",num);

    printf("list count:%d\n",LIST_COUNT(head));
    
    printf("TAG:pls input:\n");
    scanf("%d",&num);

    tmp = head;
    while(NULL != tmp->next)
    {
        if(num == tmp->next->id)
        {
            tmp->next->dirty = true;
        }
        tmp = tmp->next;
    }

    tmp = head;
    stTest* delete;
    while(NULL != tmp->next)
    {
        if(true == tmp->next->dirty)
        {
            delete = tmp->next;
            tmp->next = tmp->next->next;
            free(delete);
            continue;
        }
        tmp = tmp->next;
    }
    test_print(head);

    return 0;
}


