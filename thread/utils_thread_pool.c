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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include "osadapter.h"
#include "commons_log.h"
#include "utils_thread_pool.h"

#define LOG(...) COMMONS_LOG("EVENT",__VA_ARGS__);

#define THREAD_EXIT(cmd)  do{ \
                       pthread_detach(pthread_self()); \
                       pthread_exit (cmd); \
}while(0)

#define THREAD_STACK_SIZE (1024*1024)
#define THREAD_ADMIN_STACK_SIZE (10*1024)


void* thread_routine(void* args);
void* thread_admin(void* args);
static int pool_free(ST_THREAD_POOL** __pool);


ST_THREAD_POOL* pool_init(int maxThreadNum,int minThreadNum,int maxQueueSize,int nonBlock)
{
    int i;
    pthread_rwlockattr_t attr;
    ST_THREAD_POOL* thread_pool = NULL;
    thread_pool = (ST_THREAD_POOL*)calloc(1,sizeof(ST_THREAD_POOL));
    if(NULL == thread_pool)
    {
        LOG("malloc failed");
        return NULL;
    }

    /* initial mutex lock & condition */
    if(pthread_mutex_init(&(thread_pool->queueLock),NULL) < 0) goto err;
    if(pthread_cond_init(&(thread_pool->queueReady),NULL) < 0) goto err;
    if(pthread_cond_init(&(thread_pool->queueNotFull),NULL) < 0) goto err;
    if(pthread_rwlockattr_init(&attr) < 0) goto err;
    if(pthread_rwlockattr_setpshared(&attr,PTHREAD_PROCESS_PRIVATE) < 0) goto err;
    if(pthread_rwlock_init(&(thread_pool->queueCounterLock),&attr) < 0) goto err;

    /* initial thread info*/
    thread_pool->maxThrdNum = maxThreadNum;

    thread_pool->minThrdNum = minThreadNum;

    thread_pool->liveThrdNum = 0;

    thread_pool->busyThrdNum = 0;

    thread_pool->waitExitThrdNum = 0;

    thread_pool->historyThrdNum = 1;

    if(thread_create(&(thread_pool->adminThrdId),thread_admin,(void*)thread_pool,THREAD_ADMIN_STACK_SIZE) < 0)
    {
        LOG("create thread failed,err:%s",strerror(errno));
        goto err;
    }

    thread_pool->threadId = (pthread_t*)calloc(maxThreadNum,sizeof(pthread_t));
    if(NULL == thread_pool->threadId)
    {
        LOG("malloc failed");
        goto err;
    }
    /* initial queue info */
    thread_pool->curQueueSize = 0;

    thread_pool->maxQueueSize = maxQueueSize;
    
    thread_pool->queueHead = NULL;

    /* initial control info */
    thread_pool->shutdown = 0;
    thread_pool->nonBlock = nonBlock;
    return thread_pool;

err:
    pool_free(thread_pool);
    return NULL;
}


int pool_add_worker(ST_THREAD_POOL* pool,void* (*process_handler)(void* args),void* args)
{
    ST_THREAD_WORKER* st_new_worker = NULL;

    LOG("add worker");
    pthread_mutex_lock(&(pool->queueLock));
    if(0 == pool->nonBlock)
    {
        while((pool->curQueueSize >= pool->maxQueueSize)
            && (0 == pool->shutdown))
        {
            pthread_cond_wait(&(pool->queueNotFull),&(pool->queueLock));
        }
    }else
    {
        if((pool->curQueueSize >= pool->maxQueueSize)
            && (0 == pool->shutdown))
        {
            LOG("queue is full,pls try later");
            pthread_mutex_unlock(&(pool->queueLock));
            return -2;
        }
    }

    st_new_worker = (ST_THREAD_WORKER*) malloc(sizeof(ST_THREAD_WORKER));
    if(NULL == st_new_worker)
    {
        return -1;
    }

    st_new_worker->process_handler = process_handler;
    st_new_worker->args = args;
    st_new_worker->next = NULL;

    ST_THREAD_WORKER* member = pool->queueHead;
    if(NULL != member)
    {
        while(NULL != member->next)
        {
            member = member->next;
        }
        member->next = st_new_worker;
    }else
    {
        pool->queueHead = st_new_worker;
    }

    pool->curQueueSize ++;

    pthread_mutex_unlock(&(pool->queueLock));

    pthread_cond_signal(&(pool->queueReady));

    return 0;
}

int pool_destory(ST_THREAD_POOL* pool,int flags)
{
    int i = 0;
    if(NULL == pool)
    {
        return -1;
    }

    /* wait task to finish */
    if(flags)
    {
        while(pool->curQueueSize > 0);
    }

    pool->shutdown = 1;

    /* broadcast wakeup singal*/
    pthread_cond_broadcast(&(pool->queueReady));

    /* wait thread exit */
    for(i = 0;i < pool->maxThrdNum;i ++)
    {
        while(thread_judge_alive(pool->threadId[i]))
        {
            usleep(100*1000);
        }
    }
    
    pool_free(&pool);
    return 0;
}

static int pool_free(ST_THREAD_POOL** __pool)
{
    ST_THREAD_POOL* pool = *__pool;
    if(NULL == pool)
        return -1;

    if(pool->threadId)
    {
        free(pool->threadId);
    }

    /* destroy lock and conditon */
    pthread_rwlock_destroy(&(pool->queueCounterLock));
    pthread_mutex_destroy(&(pool->queueLock));
    pthread_cond_destroy(&(pool->queueReady));
    pthread_cond_destroy(&(pool->queueNotFull));

    if(pool->queueHead)
    {
        free(pool->queueHead);
    }

    free(pool);
    *__pool = NULL;

    return 0;
}

int thread_judge_alive(pthread_t pth)
{
    if(NULL != pth)
    {
        int kill_rc = pthread_kill(pth,0);
        if(ESRCH == kill_rc)
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

int thread_create(pthread_t *thread, void *(*start_routine) (void *), void *arg, int stacksize)
{
    pthread_attr_t attr;

    if((NULL == thread) || (NULL == start_routine)){
        return -1;
    }

    if(pthread_attr_init(&attr)){
        return-1;
    }

    if(pthread_attr_setstacksize(&attr, stacksize) < 0){
        return -1;
    }

    if(pthread_create (thread, &attr, start_routine, arg)){
        return -1;
    }

    return 0;
}

int thread_admin_process(ST_THREAD_POOL* pool)
{
    int i = 0;
    int add = 0;
        
    if(NULL == pool)
    {
       LOG("invalid args");
       return -4;
    }
    if(1 == pool->shutdown)
    {
        THREAD_EXIT(NULL);
    }

    pthread_mutex_lock(&(pool->queueLock));

    LOG("current thread live:[%d] busy:[%d] max:[%d] min:[%d] task:[%d] his:[%d]",
        pool->liveThrdNum,
        pool->busyThrdNum,
        pool->maxThrdNum,
        pool->minThrdNum,
        pool->curQueueSize,
        pool->historyThrdNum
    );
    /* create new thread when task is too many to deal */
    if(((pool->curQueueSize > MIN_TASK_WAIT_NUM) && (pool->liveThrdNum < pool->maxThrdNum))
        || (pool->liveThrdNum < pool->minThrdNum && pool->curQueueSize > 0))/* create initial thread when needed */
    {
        LOG("admin create new thread");

        for(i = 0;i < pool->maxThrdNum;i ++)
        {
            if(add >= DEFAULT_THREAD_OP_NUM)
            {
                break;
            }
            /* create new thread*/
            if(!thread_judge_alive(pool->threadId[i]))
            {
                LOG("create thread id:%d",i);
                if(thread_create(&(pool->threadId[i]),thread_routine,(void*)pool,THREAD_STACK_SIZE) < 0)
                {
                    LOG("create thread failed,err:%s",strerror(errno));
                    break;
                }
                add ++;
                pool->liveThrdNum ++;
                pool->historyThrdNum ++;
            }
        }
    }

    if(((pool->busyThrdNum * 2) < pool->liveThrdNum) 
        && (pool->liveThrdNum > pool->minThrdNum))
    {
        pool->waitExitThrdNum = DEFAULT_THREAD_OP_NUM;
        for(i = 0;i < pool->waitExitThrdNum;i ++)
        {
            pthread_cond_signal(&(pool->queueReady));
            LOG("admin clear");
        }
    }
    pthread_mutex_unlock(&(pool->queueLock));
    return 0;
}


void* thread_admin(void* args)
{
    if(NULL == args)
    {
        LOG("invalid args");
        THREAD_EXIT(NULL);
    }
    ST_THREAD_POOL* pool = (ST_THREAD_POOL*)args;

    while(1)
    {
        LOG("admin-----------");
        sleep(1);
        if(thread_admin_process(pool) < 0)
        {
            LOG("admin process failed");
        }
        
    }
}


void* thread_routine(void* args)
{
    if(NULL == args)
    {
        LOG("invalid args");
        THREAD_EXIT(NULL);
    }
    ST_THREAD_POOL* pool = (ST_THREAD_POOL*)args;
    LOG("start thread %x",pthread_self());
    while(1)
    {
        pthread_mutex_lock(&(pool->queueLock));

        while(0 == pool->curQueueSize && 0 == pool->shutdown)
        {
            LOG("thread %x is waiting",pthread_self());
            pthread_cond_wait(&(pool->queueReady),&(pool->queueLock));

            if(pool->waitExitThrdNum > 0)
            {
                pool->waitExitThrdNum --;
                if(pool->liveThrdNum > pool->minThrdNum)
                {
                    LOG("thread %x is exiting",pthread_self());
                    pool->liveThrdNum --;
                    pthread_mutex_unlock(&(pool->queueLock));
                    THREAD_EXIT(NULL);
                }
            }
        }

        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->queueLock));
            LOG("thread %x will exit",pthread_self());
            THREAD_EXIT(NULL);
        }

        pool->curQueueSize --;
        ST_THREAD_WORKER* worker = pool->queueHead;
        pool->queueHead = worker->next;

        pthread_cond_broadcast(&(pool->queueNotFull));
        pthread_mutex_unlock(&(pool->queueLock));

        LOG("thread %x is starting to work",pthread_self());
        pthread_rwlock_wrlock(&(pool->queueCounterLock));
        pool->busyThrdNum ++;
        pthread_rwlock_unlock(&(pool->queueCounterLock));
        
        (*(worker->process_handler))(worker->args);
        free(worker);
        worker = NULL;

        pthread_rwlock_wrlock(&(pool->queueCounterLock));
        pool->busyThrdNum --;
        pthread_rwlock_unlock(&(pool->queueCounterLock));
    }
    THREAD_EXIT(NULL);
}

void* myprocess(void* args)
{
    LOG("thread %x,args is %d",pthread_self(),*(int*)args);
    sleep(rand()%20);
    LOG("thread %x,process over",pthread_self());
    return NULL;
}

int thread_pool_main()
{
    int i = 0,j;
    ST_THREAD_POOL* my_pool = NULL;
    my_pool = pool_init(5,2,50,0);
    if(NULL == my_pool)
    {
        return -1;
    }

    i = 1;
    pool_add_worker(my_pool,myprocess,&i);
    sleep(5);

    for(j = 0;j < 3;j ++)
    {
        int m = (rand()%5);
        for(i = 0;i < m;i ++)
        {
            pool_add_worker(my_pool,myprocess,&i);
            //sleep(1);
        }
        //sleep(5);
    }
    LOG("start destroy-------------");
    pool_destory(my_pool,FLAGS_FORCE_TASK_EXIT);

    return 0;
}





