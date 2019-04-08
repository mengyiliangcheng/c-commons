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

/* �����̶߳�ջ��С */
#define THREAD_STACK_SIZE               (1024*1024)

/* �����̶߳�ջ��С */
#define THREAD_ADMIN_STACK_SIZE         (10*1024)

/* �߳�guard��ջ��С */
#define THREAD_GUARDER_STACK_SIZE       (1*1000)

/* �߳�����Ԥ��ʱ�� */
#define THREAD_STARTUP_TIME             (100 * 1000)

/* �߳�����Ԥ��ʱ�� */
#define THREAD_ADMIN_ROUTINE_CIRCLE     (30)

/* �����߳�ÿ�β����̸߳��� */
#define DEFAULT_THREAD_OP_NUM           (1)

/*  ��С�ȴ���������������������ܻ�ȥ�������߳� */
#define MIN_TASK_WAIT_NUM               (5)

/* ǿ���߳��˳� */
#define FLAGS_FORCE_TASK_EXIT           (0)

/* �ȴ�����ִ��������˳� */
#define FLAGS_WAIT_TASK_EXIT            (1)

/* ������� */
typedef struct ST_THREAD_WORKER
{
    void* (*process_handler)(void* args);  /* ����Ĵ����� */
    void* args;                            /* ����Ĵ������ */
    struct ST_THREAD_WORKER* next;         /* ��һ������ڵ� */
}ST_THREAD_WORKER;

/* �̳߳����Խṹ�� */
typedef struct 
{
    pthread_mutex_t queueLock;            /* ������еĻ����� */
    pthread_rwlock_t queueCounterLock;    /* ������еĶ�д�� */
    pthread_cond_t queueReady;            /* ���������ӵ��������� */
    pthread_cond_t queueNotFull;          /* ������п�����ӵ��������� */

    sem_t sem_add_work;                   /* ���������ź��� */

    /* thread pool info */
    pthread_t adminThrdId;                /* �����̵߳�id */
    pthread_t* threadId;                  /* �̳߳ص�id */
    int maxThrdNum;                       /* ����̸߳��� */
    int minThrdNum;                       /* ��С�̸߳��� */
    int liveThrdNum;                      /* �����̸߳��� */
    int busyThrdNum;                      /* æµ���̸߳��� */
    int waitExitThrdNum;                  /* �ȴ��˳����̸߳��� */
    int historyThrdNum;                   /* ��ʷ�������̸߳��� */

    /* task queue info */
    ST_THREAD_WORKER* queueHead;          /* ������е�ͷ  */
    int curQueueSize;                     /* ������еĴ�С */
    int maxQueueSize;                     /* ������е������ */

    /* thread pool status */
    int shutdown;                         /* �����̳߳ر�־λ */
    int nonBlock;                         /* �������ʱ�Ƿ����� */
}ST_THREAD_POOL;

typedef enum
{
    E_THREAD_SUCC = 0,                    /* �ɹ� */
    E_THREAD_ERR = -1,                    /* ʧ�� */
    E_THREAD_PARAM_ERR = -2,              /* �������� */
    E_THREAD_AGAIN = -3,                  /* �Ժ����� */
}E_THREADPOOL_ERRCODE;



/****************************************************************
* Function: threadpool_init
* Description: �̳߳س�ʼ��
* Param:
* Input:    maxThreadNum���̳߳�����߳���
            minThreadNum���̳߳�ά�ֵ���С�߳�����ע�⣺�̳߳ظ�����ʱ�߳���Ϊ0���͸ò����޹ء��ò���ֻ���жϵ�ǰ�߳����Ƿ���࣬��Ҫ����
            maxQueueSize�����ȴ������������̶߳�����æµ״̬ʱ�����������񶼽��ŵ��ȴ�������
            nonBlock��     ���������ȴ�������ʱ�����������������������ִ���ʽ��1.���� 2.��������
                          ��ѡ����TRUEʱ��Ϊ��������ʽ��������������ʧ��
                          ��ѡ����FALSEʱ��Ϊ������ʽ��ֻ�еȶ�����С����������ʱ�Ż᷵�ء�
* Output: 
* Return: ST_THREAD_POOL* �����̳߳ض���
* Others:                 ע�⣺����̳߳س�ʼ��ʧ�ܣ������exit�����½����˳�
* Author: pengyicheng
****************************************************************/
extern ST_THREAD_POOL* threadpool_init(int maxThreadNum,int minThreadNum,int maxQueueSize,int nonBlock);
/****************************************************************
* Function: threadpool_add_worker
* Description: �̳߳��������
* Param:
* Input:    ST_THREAD_POOL* pool���̳߳ض���
            void* (*process_handler)(void* args)������ִ�еĻص�����
            void* args������ִ������
* Output: 
* Return: E_THREAD_AGAIN�����Ժ�����
          E_THREAD_SUCC���ɹ�
* Others: 
* Author: pengyicheng
****************************************************************/
extern int threadpool_add_worker(ST_THREAD_POOL* pool,void* (*process_handler)(void* args),void* args);
/****************************************************************
* Function: threadpool_destory
* Description: �����̳߳�
* Param:
* Input:    ST_THREAD_POOL* pool���̳߳ض���
            int flags��FLAGS_WAIT_TASK_EXIT���ȴ�ȫ��ִ������˳�
                       FLAGS_FORCE_TASK_EXIT��ǿ���˳�������߳̿��У��������˳�������߳�æµ��ȴ���ǰ����ִ�����˳�
* Output: 
* Return:   E_THREAD_PARAM_ERR����������
            E_THREAD_SUCC���ɹ�
* Others:                 
* Author: pengyicheng
****************************************************************/
extern int threadpool_destory(ST_THREAD_POOL* pool,int flags);


extern int thread_pool_main();

#endif

