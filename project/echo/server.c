/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/17  automatic
*/
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"SERVER",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)
#define SOCK_PORT   23643

static void client_handler(void* socketfd);

int setopt(int fd)
{
    int on = 1;
    int keep_alive = 1;
    int keep_interval = 5;
    /* 允许地址重用 */
    if((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))<0)
    {
        LOG("Setsockopt Failed,err:%d",errno);
        return -1;
    }
    //setsockopt(fd_server, SOL_SOCKET, SO_KEEPALIVE, (void *)&keep_alive, sizeof(keep_alive));
    //setsockopt(fd_server, IPPROTO_TCP ,TCP_KEEPINTVL,(void*)keep_interval,sizeof(keep_interval));
    return 0;
}

int main()
{
    int fd_server;
    int fd_tmp;
    int ret;
    int on = 1;
    struct sockaddr_in addr_in;
    struct sockaddr_in addr_client;

    /* 创建socket */
    fd_server = socket(AF_INET,SOCK_STREAM,0);
    if(fd_server < 0)
    {
        LOG("create socket failed! err:%d",errno);
        return -1;
    }
    setopt(fd_server);    

    memset(&addr_in,0,sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_in.sin_port = htons(SOCK_PORT);
    
    /* 将fd和地址绑定 */
    fd_tmp = bind(fd_server,(struct sockaddr*)(&addr_in),sizeof(addr_in));
    if(fd_tmp < 0)
    {
        LOG("bind failed,err:%d",errno);
        return -1;
    }

    /* 监听 */
    fd_tmp = listen(fd_server,100);
    if(fd_tmp < 0)
    {
        LOG("listen failed,err:%d",errno);
        return -1;
    }

    /* 等待客户端连接 */
    while(1)
    {
        LOG("wait for new connection on port:%d....",SOCK_PORT);
        int client_len = sizeof(addr_client);
        int fd = accept(
                        fd_server,
                        (struct sockaddr *)&addr_client,
                        (socklen_t*)&client_len
                        );
        if(fd < 0)
        {
            LOG("accept failed,err:%d",errno);
            return -1;
        }
        
        char* client_ip = inet_ntoa(addr_client.sin_addr);
        int client_port = ntohs(addr_client.sin_port);
        LOG("client ip:%s port:%d",client_ip,client_port);
#if ENABLE_THREAD
        pthread_t thread_id;
        ret = pthread_create(
                            &thread_id,
                            NULL,
                            (void*)client_handler,
                            (void*)fd
                            );
        if(ret < 0)
        {   
            LOG("create thread failed,ret:%d err:%d",ret,errno);
            break;
        }
#else
        client_handler(fd);
#endif
    }
    ret = shutdown(fd_server,SHUT_WR);
    if(ret < 0)
    {
        LOG("shutdown failed,err:%d",errno);
        return -1;
    }
    
    LOG("Server is shutdowning");
    return 0;

}


static void client_handler(void* socketfd)
{
    int fd = (int*)socketfd;
    int read_bytes;
    int write_bytes;
    char buffer[1024];
    char writebuf[1024];
    const char* echo = "ok";
    
    while(1)
    {
        memset(buffer,0,sizeof(buffer));
        memset(writebuf,0,sizeof(writebuf));

        read_bytes = read(fd,buffer,1024);
        if(read_bytes == 0)
        {
            LOG("Maybe the connection is closed,err:%d",errno);
            break;
        }

        if(read_bytes == -1)
        {
            LOG("read failed,err:%d",errno);
            break;
        }

        printf("CLIENT >%s\n",buffer);
        printf("INPUT  >");
        scanf("%s",writebuf);
        write_bytes = write(fd,writebuf,strlen(writebuf));
        if(write_bytes < 0)
        {
            LOG("write failed,err:%d",errno);
            break;
        }
         
    }

    LOG("close the connection....");
    close(fd);
#ifdef ENABLE_THREAD
    pthread_detach(pthread_self());
    pthread_exit(NULL);
#endif
    return 0;
}


