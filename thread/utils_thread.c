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

#include "osadapter.h"
#include "utils_thread.h"
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "commons_log.h"
#include "osadapter.h"
#include "utils_string.h"

#define BUF_SIZE        1024
#define MAX_ARGS_NUM    32

extern char **environ;  /* 环境变量 */


//#define LOG(...) COMMONS_LOG("THREAD",__VA_ARGS__);
#define LOG commons_println

void getNameByPid(pid_t pid, char *task_name) {
    char proc_pid_path[BUF_SIZE];
    char buf[BUF_SIZE];

    sprintf(proc_pid_path, "/proc/%d/status", pid);
    FILE* fp = fopen(proc_pid_path, "r");
    if(NULL != fp){
        if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
            fclose(fp);
        }
        fclose(fp);
        sscanf(buf, "%*s %s", task_name);
    }
}



int getPidByName(char* task_name)
{
#if 0  //待重构
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];
    char cur_task_name[50] = {0};
    char buf[256];

    if(NULL == task_name)
    {
        return -1;
    }

    LOG("task name:%s",task_name);
    dir = opendir("/proc");
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))             
                continue;
            if (DT_DIR != ptr->d_type) 
                continue;

            sprintf(filepath, "/proc/%s/status", ptr->d_name);
            fp = fopen(filepath, "r");
            if (NULL != fp)
            {
                bzero(buf,sizeof(buf));
                if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%*s %s", cur_task_name);
                if (0 == strncmp(task_name, cur_task_name,MAX(strlen(task_name),strlen(cur_task_name))))
                {   
                    LOG("PID:  %s,%d\n", ptr->d_name,atoi(ptr->d_name));
                    fclose(fp);
                    return atoi(ptr->d_name);
                }
            }

        }
        closedir(dir);//鍏抽棴璺緞
    }
    LOG("get pid error");
    return -1;
#endif
}

void utils_thread_sigchld_handler(int signum)
{
    pid_t pid;
    int status;

    while((pid = waitpid(-1,&status,0)) > 0)
    {
        if (WIFEXITED(status))
        {
           commons_println("pid %d exited, status=%d",pid,WEXITSTATUS(status));
        } else if (WIFSIGNALED(status))
        {
           commons_println("pid %d killed by signal %d",pid,WTERMSIG(status));
        } else if (WIFSTOPPED(status))
        {
           commons_println("pid %d stopped by signal %d",pid,WSTOPSIG(status));
        } else if (WIFCONTINUED(status))
        {
           commons_println("pid %d continued");
        }
    }
}

s32 utils_thread_create_process(char* file,char* cmdline)
{
    pid_t pid;
    pid_t gpid;
    char* argv[MAX_ARGS_NUM];
    char** p;
    int count;
    sigset_t mask,prev_mask;
    struct sigaction action;

    /* 检查入参合法性 */
    CHECK_PARAM(file,NULL,-1);

    /* 解析命令行 */
    count = 0;
    argv[count++] = file;
    if(cmdline != NULL)
    {
        p = strings_split(cmdline,' ');
        while(p != NULL && *p != NULL && count < (MAX_ARGS_NUM-1))
        {
            argv[count++] = *p;
            p++;
        }
    }
    argv[count] = NULL;

    //commons_print_args(argv);

    gpid = getpid();

    /* 屏蔽信号 */
    sigemptyset(&mask);
    sigaddset(&mask,SIGCHLD);
    sigaddset(&mask,SIGINT);
    sigprocmask(SIG_BLOCK,&mask,&prev_mask);

    /* 执行fork */
    pid = fork();
    if(pid == 0)
    {
        LOG("[%d]",getpid());

        /* 取消信号阻塞，初始化信号 */
        sigprocmask(SIG_SETMASK,&prev_mask,NULL);
        action.sa_handler = SIG_DFL;
        action.sa_flags = SA_RESTART;
        sigemptyset(&action.sa_mask);
        sigaction(SIGINT,&action, NULL);
        sigaction(SIGCHLD,&action,NULL);
        sigaction(SIGPIPE,&action,NULL);

        /* 设置子进程组id */
        setpgid(0,gpid);

        /* 替换进程 */
        execve(file,argv,environ);
        LOG("this is child process execl failed,err:%d",errno);
    }else if(pid < 0) 
    {
        LOG("fork failed,err:%d",errno);
    }

    /* 恢复信号 */
    sigprocmask(SIG_SETMASK,&prev_mask,NULL);

    /* 注册子进程消亡处理函数，以免产生僵尸进程 */
    Signal(SIGCHLD,utils_thread_sigchld_handler);

    return pid;
}

s32 utils_thread_create_process_block(char* file,char* cmdline)
{
    int pid;
    int ret;
    int status;
    pid = utils_thread_create_process(file,cmdline);
    if(pid < 0)
    {
        LOG("create new process failed,err:%d!!",pid);
        return -1;
    }

    do
    {
        ret = waitpid(pid, &status, WUNTRACED | WCONTINUED);
        if (ret == -1)
        {
           LOG("waitpid failed,err:%d",errno);
           return -2;
        }

        if (WIFEXITED(status))
        {
           LOG("exited, status=%d", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status))
        {
           LOG("killed by signal %d", WTERMSIG(status));
        } else if (WIFSTOPPED(status))
        {
           LOG("stopped by signal %d", WSTOPSIG(status));
        } else if (WIFCONTINUED(status))
        {
           LOG("continued");
        }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 0;
}

pthread_t utils_thread_create_thread(void* (*func)(void *),void* args)
{
    pthread_t pth;
    
    if(pthread_create(&pth,NULL,func,args) < 0)
    {
       LOG("create thread failed,err:%d",strerror(errno));
       return -1;
    }
    LOG("create thread succ");
    return pth;
}

pthread_mutex_t count_lock;
pthread_cond_t count_nonzero;
int count;

s32 utils_thread_hander(void)
{
    pthread_mutex_lock(&count_lock);
    LOG("decrement_count get count_lock");
    LOG("decrement_count count == 0 ");
    LOG("decrement_count before cond_wait");
    pthread_cond_wait(&count_nonzero, &count_lock);
    LOG("decrement_count after cond_wait");
    pthread_mutex_unlock(&count_lock);
}

void* consumer(void* args)
{
    for(;;)
    {
        pthread_mutex_lock(&count_lock);
        while(count <= 0)
        {
            pthread_cond_wait(&count_nonzero,&count_lock);
        }
        count--;
        pthread_mutex_unlock(&count_lock);
        LOG("consumer:%d\n",count);
        //sleep(10);
    }

}

void* producter(void* args)
{
    for(;;)
    {
        pthread_mutex_lock(&count_lock);
        count ++;
        pthread_mutex_unlock(&count_lock);
        LOG("producter:%d\n",count);
        pthread_cond_signal(&count_nonzero);
        sleep(1);
    }
}


u32 utils_thread_cond(void)
{
    pthread_cond_t cond=PTHREAD_COND_INITIALIZER; //初始化条件变量

    pthread_t thread1,thread2;

    pthread_mutex_init(&count_lock,NULL);
    pthread_cond_init(&count_nonzero,NULL);

    pthread_create(&thread1,NULL,*consumer,NULL);
    pthread_create(&thread2,NULL,*producter,NULL);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_cond_destroy(&count_nonzero);

    pthread_exit(0);
    
}


