/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/23  automatic
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

/* 对传统system进行封装 */
void mysystem(char* cmd)
{
    int status;
    if(!cmd)
        return -1;

    LOG("Qt Manager:  System Command:%s", cmd);

    status = system(cmd);

    if(-1 == status)
    {
        LOG("System Error");
        return -1;
    }

    LOG("System Return Value:0x%04X",status);
    if(WIFEXITED(status))
    {
        if (0 == WEXITSTATUS(status))
        {
            LOG("Run Shell Script Successfully.");
        }
        else
        {
            LOG("Run Shell Script Fail, Script Exit Code: %d", WEXITSTATUS(status));
        }
    }
    else
    {
        LOG("Exit Status = [%d]\n", WEXITSTATUS(status));
    }
}

/* 使用popen实现system */
int mysystem1(char *cmd)
{
    FILE* fp = NULL;
    char buf[128] = {0};
    int read_bytes = 0;
    int ret = 0;

    if(!cmd)
    {
        LOG("invalid param");
        return -1;
    }

    fp = popen(cmd,"r");
    if(NULL == fp)
    {
        LOG("popen failed,err:%d",errno);
        return -1;
    }

    while(NULL != fgets(buf,sizeof(buf),fp))
    {
        printf("%s",buf);
    }

    ret = pclose(fp);
    if(ret < 0)
    {
        LOG("pclose failed,err:%d",errno);
        return -1;
    }
    else
    {
        LOG("cmd:%s exit status:%d return code:%d",cmd,ret,WEXITSTATUS(ret));
    }

    return 0;
}

/* 实现简化版system */
int myshell(char* path,int argc,char *argv[])
{
    FILE* fp = NULL;
    char buf[128] = {0};
    int read_bytes = 0;
    int ret = 0;
    pid_t parent_pid;

    pid_t pid = fork();
    if(pid < 0)
    {
        LOG("fork failed,err:%d",errno);
        return -1;
    }

    parent_pid = getpid();
    //LOG("parent pgid:%d",getpgrp());
    if(pid == 0)
    {
        //LOG("child pgid:%d",getpgrp());
        setpgid(0,parent_pid); /* 将子进程的组id改成父进程的pid */
        //LOG("child pgid:%d",getpgrp());
        if(execv(path,argv) < 0)
        LOG("execv failed,err:%d",errno);
        exit(1);
    }

    //LOG("child pid:%d",pid);
    int status;
    if(waitpid(pid,&status,0) < 0)
    {
        LOG("waitpid failed,err:%d",errno);
        return -1;
    }

    if(WIFEXITED(status))
    {
        LOG("normal exit,exit status:%d",WEXITSTATUS(status));
    }else if(WIFSIGNALED(status))
    {
        LOG("signal interp,signal num:%d",WTERMSIG(status));
    }

    return 0;
}
void signal_handler(int signum)
{
    LOG("interp by %d",signum);
}

void exit_handler(void)
{
    LOG("exit handler...");
}

int main(int argc,char *argv[])
{
    //mysystem("ls");
    int i = 0;
    //for(i = 0;i < 100;i ++)
//    while(1)
    LOG("%d",getpid());
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGCHLD);
    sigaddset(&mask,SIGINT);
    //sigprocmask(SIG_BLOCK,&mask,NULL);

    atexit(exit_handler);

    signal(SIGCHLD,signal_handler);
    //myshell("/bin/sh",argc,argv); /* 创建一个shell */
    myshell("./hello",argc,argv); /* 执行hello */
    //mysystem("sleep 10");    
    //mysystem1("date");
    //mysystem1("ls -l");
    //signal(SIGCHLD,signal_handler);
    {
        //mysystem1("sleep 3");
        //mysystem1("sleep 2");
    }
    printf("hello world\n");
    exit(10);
    return 0;
}


