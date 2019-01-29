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

#define DEFAULT_THREAD_OP_NUM     (2)
#define MIN_TASK_WAIT_NUM         (5)
#define FLAGS_FORCE_TASK_EXIT 0
#define FLAGS_WAIT_TASK_EXIT 1


typedef struct ST_THREAD_WORKER
{
    void* (*process_handler)(void* args);
    void* args;
    struct ST_THREAD_WORKER* next;
}ST_THREAD_WORKER;

typedef struct 
{
    pthread_mutex_t queueLock;
    pthread_rwlock_t queueCounterLock;
    pthread_cond_t queueReady;
    pthread_cond_t queueNotFull;

    /* thread pool info */
    pthread_t adminThrdId;
    pthread_t* threadId;
    int maxThrdNum;
    int minThrdNum;
    int liveThrdNum;
    int busyThrdNum;
    int waitExitThrdNum;
    int historyThrdNum;

    /* task queue info */
    ST_THREAD_WORKER* queueHead;
    int curQueueSize;
    int maxQueueSize;

    /* thread pool status */
    int shutdown;
    int nonBlock;
}ST_THREAD_POOL;


extern ST_THREAD_POOL* pool_init(int maxThreadNum,int minThreadNum,int maxQueueSize,int nonBlock);
extern int pool_add_worker(ST_THREAD_POOL* pool,void* (*process_handler)(void* args),void* args);
extern int pool_destory(ST_THREAD_POOL* pool,int flags);

extern int thread_pool_main();

