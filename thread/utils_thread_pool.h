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

#ifndef __UTILS_THREAD_POOL_H__
#define __UTILS_THREAD_POOL_H__

/* 任务线程堆栈大小 */
#define THREAD_STACK_SIZE               (1024*1024)

/* 管理线程堆栈大小 */
#define THREAD_ADMIN_STACK_SIZE         (10*1024)

/* 线程guard堆栈大小 */
#define THREAD_GUARDER_STACK_SIZE       (1*1000)

/* 线程启动预留时间 */
#define THREAD_STARTUP_TIME             (100 * 1000)

/* 线程启动预留时间 */
#define THREAD_ADMIN_ROUTINE_CIRCLE     (30)

/* 管理线程每次操作线程个数 */
#define DEFAULT_THREAD_OP_NUM           (1)

/*  最小等待任务数，超过这个数可能会去创建新线程 */
#define MIN_TASK_WAIT_NUM               (5)

/* 强制线程退出 */
#define FLAGS_FORCE_TASK_EXIT           (0)

/* 等待任务执行完毕再退出 */
#define FLAGS_WAIT_TASK_EXIT            (1)

/* 任务队列 */
typedef struct ST_THREAD_WORKER
{
    void* (*process_handler)(void* args);  /* 任务的处理函数 */
    void* args;                            /* 任务的处理参数 */
    struct ST_THREAD_WORKER* next;         /* 下一个任务节点 */
}ST_THREAD_WORKER;

/* 线程池属性结构体 */
typedef struct 
{
    pthread_mutex_t queueLock;            /* 任务队列的互斥锁 */
    pthread_rwlock_t queueCounterLock;    /* 任务队列的读写锁 */
    pthread_cond_t queueReady;            /* 任务队列添加的条件变量 */
    pthread_cond_t queueNotFull;          /* 任务队列可以添加的条件变量 */

    sem_t sem_add_work;                   /* 添加任务的信号量 */

    /* thread pool info */
    pthread_t adminThrdId;                /* 管理线程的id */
    pthread_t* threadId;                  /* 线程池的id */
    int maxThrdNum;                       /* 最大线程个数 */
    int minThrdNum;                       /* 最小线程个数 */
    int liveThrdNum;                      /* 存活的线程个数 */
    int busyThrdNum;                      /* 忙碌的线程个数 */
    int waitExitThrdNum;                  /* 等待退出的线程个数 */
    int historyThrdNum;                   /* 历史创建的线程个数 */

    /* task queue info */
    ST_THREAD_WORKER* queueHead;          /* 任务队列的头  */
    int curQueueSize;                     /* 任务队列的大小 */
    int maxQueueSize;                     /* 任务队列的最大数 */

    /* thread pool status */
    int shutdown;                         /* 销毁线程池标志位 */
    int nonBlock;                         /* 添加任务时是否阻塞 */
}ST_THREAD_POOL;

typedef enum
{
    E_THREAD_SUCC = 0,                    /* 成功 */
    E_THREAD_ERR = -1,                    /* 失败 */
    E_THREAD_PARAM_ERR = -2,              /* 参数错误 */
    E_THREAD_AGAIN = -3,                  /* 稍后重试 */
}E_THREADPOOL_ERRCODE;



/****************************************************************
* Function: threadpool_init
* Description: 线程池初始化
* Param:
* Input:    maxThreadNum：线程池最大线程数
            minThreadNum：线程池维持的最小线程数。注意：线程池刚启动时线程数为0，和该参数无关。该参数只用判断当前线程数是否过多，需要销毁
            maxQueueSize：最大等待队列数。当线程都处于忙碌状态时，后续的任务都将放到等待队列中
            nonBlock：     当到达最大等待队列数时，后续如果再添加任务，有两种处理方式：1.阻塞 2.非阻塞。
                          当选择传入TRUE时，为非阻塞方式，函数立即返回失败
                          当选择传入FALSE时，为阻塞方式，只有等队列数小于最大队列数时才会返回。
* Output: 
* Return: ST_THREAD_POOL* 返回线程池对象
* Others:                 注意：如果线程池初始化失败，会调用exit，导致进程退出
* Author: pengyicheng
****************************************************************/
extern ST_THREAD_POOL* threadpool_init(int maxThreadNum,int minThreadNum,int maxQueueSize,int nonBlock);
/****************************************************************
* Function: threadpool_add_worker
* Description: 线程池添加任务
* Param:
* Input:    ST_THREAD_POOL* pool：线程池对象
            void* (*process_handler)(void* args)：任务执行的回调函数
            void* args：任务执行内容
* Output: 
* Return: E_THREAD_AGAIN：请稍后重试
          E_THREAD_SUCC：成功
* Others: 
* Author: pengyicheng
****************************************************************/
extern int threadpool_add_worker(ST_THREAD_POOL* pool,void* (*process_handler)(void* args),void* args);
/****************************************************************
* Function: threadpool_destory
* Description: 销毁线程池
* Param:
* Input:    ST_THREAD_POOL* pool：线程池对象
            int flags：FLAGS_WAIT_TASK_EXIT：等待全部执行完才退出
                       FLAGS_FORCE_TASK_EXIT：强制退出，如果线程空闲，则立即退出；如果线程忙碌则等待当前任务执行完退出
* Output: 
* Return:   E_THREAD_PARAM_ERR：参数错误
            E_THREAD_SUCC：成功
* Others:                 
* Author: pengyicheng
****************************************************************/
extern int threadpool_destory(ST_THREAD_POOL* pool,int flags);


extern int thread_pool_main();

#endif

