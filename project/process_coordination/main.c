/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/29  automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

//#define DEBUG
#ifdef DEBUG
#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)
#else
#define LOG(...) 
#endif
/* 任务线程堆栈大小 */
#define THREAD_STACK_SIZE               (10*1024)

/* 线程guard堆栈大小 */
#define THREAD_GUARDER_STACK_SIZE       (1*1000)

typedef enum
{
    E_THREAD_SUCC = 0,                    /* 成功 */
    E_THREAD_ERR = -1,                    /* 失败 */
    E_THREAD_PARAM_ERR = -2,              /* 参数错误 */
    E_THREAD_AGAIN = -3,                  /* 稍后重试 */
}E_THREADPOOL_ERRCODE;

int thread_create(pthread_t *thread, void *(*start_routine) (void *), void *arg)
{
    pthread_attr_t attr;
    int ret = E_THREAD_SUCC;

    if((NULL == thread) || (NULL == start_routine))
    {
        return E_THREAD_ERR;
    }

    do
    {
        if(pthread_attr_init(&attr))
        {
            LOG("pthread_attr_init failed,err:%d",errno);
            return E_THREAD_ERR;
        }

        if(pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE) < 0)
        {
            LOG("pthread_attr_setstacksize failed,err:%d",errno);
            ret = E_THREAD_ERR;
            break;
        }

        if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) < 0)
        {
            LOG("pthread_attr_setdetachstate failed,err:%d",errno);
            ret = E_THREAD_ERR;
            break;
        }

        if(pthread_attr_setguardsize(&attr,THREAD_GUARDER_STACK_SIZE))
        {
            LOG("pthread_attr_setguardsize failed,err:%d",errno);
            ret = E_THREAD_ERR;
            break;
        }

        if(pthread_create (thread, &attr, start_routine, arg))
        {
            LOG("pthread_create failed,err:%d",errno);
            ret = E_THREAD_ERR;
            break;
        }
    }while(0);

    pthread_attr_destroy(&attr);

    return ret;
}

long gettime(void)
{
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (tv.tv_sec*1000 + tv.tv_usec / 1000);
}

typedef struct LIST
{
    struct LIST *next;
    void* args;
    int num;
}LIST,NODE;

LIST list_head;

static pthread_mutex_t list_lock;

typedef struct
{
    long start_time;
    pthread_mutex_t prof_lock;
    pthread_mutex_t ta_lock;
    int prof_busy;
    int ta_busy;
    int wait_num;
    int total_num;
    pthread_mutex_t context_lock;
    pthread_mutex_t over_lock;
}ST_CONTEXT;

static ST_CONTEXT context;

#define LOGI(fmt,...) do { \
                        printf("%d ms--"fmt,gettime()-context.start_time,##__VA_ARGS__); \
                        printf("\n"); \
                      }while(0);

void* profTY_thread(void* args)
{
    LOG("this is prof TY thread");
    NODE* p = NULL;
    pthread_mutex_init(&context.prof_lock,NULL);

    while(1)
    {
        LOG("Prof begin");
        if(context.wait_num <= 0)
        {
            pthread_mutex_lock(&context.prof_lock);
            LOGI("Prof.TY:rest");
            pthread_mutex_lock(&context.context_lock);
            context.prof_busy = 0;
            pthread_mutex_unlock(&context.context_lock);
            pthread_mutex_lock(&context.prof_lock);
        }
        pthread_mutex_lock(&context.context_lock);
        context.prof_busy = 1;
        pthread_mutex_unlock(&context.context_lock);
        LOG("Prof.TY start work");
        p = list_head.next;
        if(NULL == p)
        {
            LOG("ERROR!!!!");
            exit(1);
        }

        int s = rand() % 50;
        LOGI("Prof.TY:discuss with Student %02d %d ms",p->num,s+50);
        usleep((s+50)*1000);
        LOG("start unlock student %d",p->num);
        LOGI("Student %02d:leave",p->num);

        /* 通知同学 */
        pthread_mutex_unlock(p->args);
        pthread_mutex_lock(&context.context_lock);
        context.wait_num --;
        LOG("wait:%d ta_busy:%d",context.wait_num,context.ta_busy);
        if(context.wait_num >= 4 && 0 == context.ta_busy)
            pthread_mutex_unlock(&context.ta_lock); /* 通知TA */
        pthread_mutex_unlock(&context.context_lock);

        /* 和下一个同学交互 */
        LOG("wait num:%d",context.wait_num);
        list_head.next = p->next;
        //if(context.total_num == 0) break;
    }

    return NULL;
}

void* TA_thread(void* args)
{
    LOG("this is TA thread");
    NODE* p  = NULL;
    pthread_mutex_init(&context.ta_lock,NULL);

    p = &list_head;
    while(1)
    {
        if(context.total_num == context.wait_num || context.wait_num >= 5)
        {
            LOG("TA to wait,total:%d,wait:%d",context.total_num,context.wait_num);
            pthread_mutex_lock(&context.ta_lock);
            if(context.wait_num <= 5)
            {
                LOGI("TA Y:rest");
            }
            context.ta_busy = 0;
            pthread_mutex_lock(&context.ta_lock);
            pthread_mutex_unlock(&context.ta_lock);
        }

        pthread_mutex_lock(&context.context_lock);
        context.ta_busy = 1;
        pthread_mutex_unlock(&context.context_lock);

        pthread_mutex_lock(&context.context_lock);
        pthread_mutex_unlock(&context.context_lock);

        pthread_mutex_lock(&context.context_lock);
        if(context.wait_num >= 5)
        {
            LOG("wait too many:%d",context.wait_num);
            pthread_mutex_unlock(&context.context_lock);
            continue;
        }
        pthread_mutex_unlock(&context.context_lock);

        pthread_mutex_lock(&list_lock);
        p = p->next;
        if(NULL == p)
        {
            LOG("NULL point");
            pthread_mutex_unlock(&list_lock);
            break;
        }

        int s = rand() % 20;
        LOGI("TA Y:discuss with Student %02d %dms",p->num,s+10);
        pthread_mutex_unlock(&list_lock);

        usleep((s+10) * 1000);
        LOG("add wait num");
        pthread_mutex_lock(&context.context_lock);
        context.wait_num ++;
        LOG("release prof lock,prof busy:%d",context.prof_busy); 
        if(context.prof_busy == 0)
        {
            pthread_mutex_unlock(&context.prof_lock); 
        }

        pthread_mutex_unlock(&context.context_lock);
        LOG("release prof lock");
    }

    while(1);



    return NULL;
}

void* student_thread(void * args)
{
    LIST *node = NULL;
    LIST *list = NULL;
    pthread_mutex_t lock;
    int num = (int)args;

    pthread_mutex_init(&lock,NULL);

    node = (LIST*)calloc(sizeof(LIST),1);
    if(NULL == node)
    {
        return ;
    }

    node->next = NULL;
    node->args = &lock;
    node->num = num;

    pthread_mutex_lock(&list_lock);
    list = &list_head;
    while(NULL != list->next) list = list->next;

    list->next = node;
    pthread_mutex_unlock(&list_lock);
    LOG("student init over");

    pthread_mutex_lock(&lock);
    LOG("student is wait...");
    pthread_mutex_lock(&context.context_lock);
    context.total_num ++;
    if(0 == context.ta_busy && context.wait_num < 5)
    {
        LOGI("Student %02d:enter",num);
        pthread_mutex_unlock(&context.ta_lock);
    }else
    {
        LOGI("Student %02d:enter & wait",num);
    }
    pthread_mutex_unlock(&context.context_lock);
    pthread_mutex_lock(&lock);
//    LOGI("Student %02d:leave",num);
#if 0
    pthread_mutex_lock(&list_lock);
    NODE* p = &list_head;
    while(NULL != p)
    {
        if(p->next->num == num) break;
    }
    p->next = p->next->next;
#endif
    free(node);
    pthread_mutex_destroy(&lock);
    
    pthread_mutex_lock(&context.context_lock);
    context.total_num --;
    pthread_mutex_unlock(&context.context_lock);

    if(num == 49)
    {
        pthread_mutex_unlock(&context.over_lock);
    }

    return NULL;
}


int main()
{
    pthread_t prof_pth;
    pthread_t TA_pth;
    int wait_time = 0;
    int i;
    
    list_head.next = NULL;
    list_head.args = NULL;
    pthread_mutex_init(&list_lock,NULL);

    context.start_time = gettime();
    pthread_mutex_init(&context.context_lock,NULL);

    thread_create(&prof_pth,profTY_thread,NULL);    
    thread_create(&TA_pth,TA_thread,NULL);
    srand(time(NULL));
    

    for(i = 0;i < 50;i ++)
    {
//        LOG("create %d student thread",i);
        pthread_t spth;
        wait_time = rand() % 5;
        usleep((wait_time + 5) * 1000);
        thread_create(&spth,student_thread,i);
    }

    pthread_mutex_init(&context.over_lock,NULL);   
    pthread_mutex_lock(&context.over_lock); 
    pthread_mutex_lock(&context.over_lock);

    return 0;
}


