/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <wait.h>

typedef void (*sighandler_t)(int);

void process_exit(int num)
{
    printf("child is disconnect\n");
}

int sio_puts(char* s)
{
    if(!s) return 0;
    return write(STDOUT_FILENO,s,strlen(s));
}

void signal_handler(int num)
{
    char buf[128];

    snprintf(buf,sizeof(buf),"Catch Sigal %d\n",num);
    sio_puts(buf);
    sleep(10);
    _exit(0);
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

int main()
{
    int num;
    sigset_t mask;
    sigset_t prev;
    printf("pid:%d\n",getpid());
    Signal(SIGINT,signal_handler);
    Signal(SIGCHLD,signal_handler);

    while(1)
    {
        printf("block:      0-SIGINT 1-SIGCHLD  :\n");
        printf("unblock:    2-SIGINT 3-SIGCHLD  :\n");
        printf("setmask:    4:\n");
        printf("sigsuspend  5:\n");
        if(scanf("%d",&num) == 0)
        {
            getchar();
            continue;
        }
        switch(num)
        {
            case 0:
                sigemptyset(&mask);
                sigaddset(&mask,SIGINT);
                sigprocmask(SIG_BLOCK,&mask,&prev);
                printf("block signal SIGINT\n");
                break;
            case 1:
                sigemptyset(&mask);
                sigaddset(&mask,SIGCHLD);
                sigprocmask(SIG_BLOCK,&mask,&prev);
                printf("block signal SIGCHLD\n");
                break;
            case 2:
                sigemptyset(&mask);
                sigaddset(&mask,SIGINT);
                sigprocmask(SIG_UNBLOCK,&mask,&prev);
                printf("block signal SIGINT\n");
                break;
            case 3:
                sigemptyset(&mask);
                sigaddset(&mask,SIGCHLD);
                sigprocmask(SIG_UNBLOCK,&mask,&prev);
                printf("block signal SIGCHLD\n");
                break;
            case 4:
                sigemptyset(&mask);
                sigprocmask(SIG_SETMASK,&prev,NULL);
                printf("restore mask\n");
                break;
            case 5:
                printf("signal suspend\n");
                sigsuspend(&prev);
                printf("signal suspend over\n");
                break;
            default:
                break;
        }
    }
}

int main_1()
{
    
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        printf("error\n");
        exit(-1);
    }
    else if(pid == 0)
    {
        printf("child process\n");
        sleep(3);
        exit(0);
    }
    else
    {
        signal(SIGCHLD,process_exit);
        printf("parent process\n");
        int status;
        int ret = wait(&status);
        printf("wait ret:%d,status:%d",ret,status);
    }

    printf("hello world");
    return 0;
}


