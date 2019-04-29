/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/04/21  automatic
*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#define LOG(format, ...) do { \
                printf("%s:%s/%s|%d::%d::"format,"fork",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                                }while(0)

void child1_callback(int num)
{
    LOG("Child process 1 is killed by parent");
    exit(0);
}

void child1_process()
{
    signal(SIGQUIT,child1_callback);  /* 注册信号SIGQUIT */
    while(1);   /* 阻塞等待信号 */
}

void child2_callback(int num)
{
    LOG("Child process 2 is killed by parent");
    sleep(1);
    exit(0);
}

void child2_process()
{
    signal(SIGQUIT,child2_callback); 
    while(1);
}


int main()
{

    pid_t pid_child1;
    pid_t pid_child2;

    /* 创建进程1 */
    pid_child1 = fork();
    if(pid_child1 < 0)  /* 创建失败 */
    {
        LOG("fork failed,err:%d",errno);
        exit(-1);
    }
    else if(pid_child1 == 0) /* 返回值为0，代表是子进程本身 */
    {
        LOG("this is child process");
        child1_process();
    }
    else  /* 返回值大于0，代表子进程的pid */
    {
        /* 创建进程2 */
        LOG("this is parent process");
        pid_child2 = fork();
        if(pid_child2 < 0)
        {
            LOG("fork failed,err:%d",errno);
            exit(-1);
        }
        else if (pid_child2 == 0)
        {
            LOG("this is child 2 process");
            child2_process();
        }
        else
        {
            LOG("this is parent process");
            sleep(3);
            /* 给子进程发信号 */
            kill(pid_child1,SIGQUIT);
            kill(pid_child2,SIGQUIT);
/*            int status1,status2;
            int ret;
            ret = waitpid(pid_child1,&status1,0);
            if(ret < 0)
            {
                LOG("waitpid failed,err:%d",errno);
            }
            ret = waitpid(pid_child2,&status2,0);
            if(ret < 0)
            {
                LOG("waitpid failed,err:%d",errno);
            }
*/
            /* 阻塞等待子进程退出,waitpid(-1,NULL,0) 
			-1：代表进程组，NULL处子进程退出状态，我们不需要获取该值，所以填NULL，0代表阻塞等待 */
            pid_t pid;
            while((pid = waitpid(-1,NULL,0)) != -1)
            {
                LOG("process %d is exited",pid);
            }
            LOG("Parent process is killed");
            exit(0);
        }
    }

    return 0;
}


