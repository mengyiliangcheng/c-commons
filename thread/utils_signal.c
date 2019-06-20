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
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "osadapter.h"
#include "commons_log.h"
#include "utils_signal.h"

#define LOG(...) COMMONS_LOG("SIGNAL",__VA_ARGS__);

typedef void (*signal_handler)(void* args);
typedef void (*sighandler_t)(int);


void signal_none(void)
{
    sigset_t mask;
    /* block all signal except SIGKILL & SIGSTOP,SIGKILL & SIGSTOP can not be blocked */
    sigfillset(&mask);

    int ret = pthread_sigmask(SIG_BLOCK,&mask,NULL);
    if(ret < 0)
    {
        LOG("pthread sigmask failed,err:%s",strerror(errno));
    }
}

void signal_add()
{
    
}

signal_handler signal_set(int sig,signal_handler func,int mask)
{
    /* ignore */
    if(NULL == func)
    {
        func = SIG_IGN;
    }
    /* default */
    else if(-1 == (long)func)
    {
        func = SIG_DFL;
    }

    struct sigaction act,oldact;
    
    act.sa_handler = func;

    if(mask)
    {
        /* set all signal to 1 */
        /* block all signal */
        sigfillset(&act.sa_mask);
    }
    else
    {
        /* set all signal to 0 */
        /*  do not block any signal */
        sigemptyset(&act.sa_mask);
    }

    act.sa_flags = 0;

    if(sigaction(sig,&act,&oldact) < 0)
    {
        LOG("sigaction failed,err:%s",strerror(errno));
        return NULL;
    }

    return oldact.sa_handler;
}

sighandler_t Signal(int signum,sighandler_t handler)
{
    struct sigaction action,old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* sa_mask：中断执行过程中的信号阻塞集合 */
                                 /* 如果全为空，则说明不阻塞信号，则在中断期间会处理其他信号,但不会处理相同信号 */
                                 /* 如果全为1，则说明阻塞所有信号，只有等该信号处理完以后才会处理其他信号 */
    action.sa_flags = SA_RESTART; /* 在中断执行完之后，尽量恢复系统调用 */

    if(sigaction(signum,&action,&old_action) < 0)
    {
        fprintf(stderr,"Signal error,%d",errno);
    }
    return old_action.sa_handler;
}


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
sem_t sem;


static void handler(int sig)
{
    LOG("sem_post() from handler");
}


int sem_wait_main()
{
    struct sigaction sa;
    int s;
    //signal();
    if (sem_init(&sem, 0, 0) == -1)
        LOG("sem_init failed");

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        printf("sigaction");
    alarm(1);
    
    LOG("main() to call sem_wait()");
    s = sem_wait(&sem);
    LOG("main() to call sem_wait finish,ret:%d,err:%d",s,errno);
}


int signal_main(void)
{
    
}



