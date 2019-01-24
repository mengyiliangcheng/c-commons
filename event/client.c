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
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/syscall.h>

#include "osadapter.h"
#include "commons_log.h"
#define LOG(...) COMMONS_LOG("CLIENT",__VA_ARGS__);


void* client_read_socket_hander(void* args)
{
    int ret;
    char buf[1024];    
    int connect_fd = *(int*)args;
    if(NULL == args)
    {   
        LOG("args is null");
        return -1;
    }

    while(1)
    {
        bzero(buf,sizeof(buf));
        ret = read(connect_fd,buf,sizeof(buf));
        if(ret < 0)
        {
            LOG("read from server failed,err:%s",strerror(errno));
            close(connect_fd);
            return -1;
        }
        LOG("read from server:[%s]",buf);
    }
}


int Client(char* unixSocket)
{
    int connect_fd;
    int ret;
    char buf[256];
    int i;
    struct sockaddr_un svr_addr;

    //LOG("current pid:%d",getpid());
    LOG("thread_one:int %d main process, the tid=%lu,pid=%ld\n",getpid(),pthread_self(),syscall(SYS_gettid));
    LOG("unixSocket:%s\n",unixSocket);

    connect_fd = socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd < 0)
    {
        LOG("create socket failed,err:%s",strerror(errno));
        return -1;
    }

    /* connect to server*/
    svr_addr.sun_family = AF_UNIX;
    strcpy(svr_addr.sun_path,unixSocket);

    ret = connect(connect_fd,(struct sockaddr*)&svr_addr,sizeof(svr_addr));
    if(-1 == ret)
    {
        LOG("connect to server failed,err:%s",strerror(errno));
        close(connect_fd);
        return -1;
    }
    LOG("connect to server succ");

    pthread_t pth;
    ret = pthread_create(&pth,NULL,client_read_socket_hander,&connect_fd);
    if(ret < 0)
    {
        LOG("create thread failed,err:%s",strerror(errno));
        return;
    }

    for(i = 0;i < 100;i ++)
    {
        bzero(buf,sizeof(buf));
        strcpy(buf,"hello from client");
        ret = write(connect_fd,buf,sizeof(buf));
        if(ret < 0)
        {
            LOG("write to server failed,err:%s",strerror(errno));
            close(connect_fd);
            return -1;
        }
        LOG("write to server succ");

        sleep(4);
    }
    close(connect_fd);

    return 0;
}


