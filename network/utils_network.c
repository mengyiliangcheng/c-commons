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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include "commons_log.h"

#define LOG(...) COMMONS_LOG("NETWORK",__VA_ARGS__);
#define UNIX_DOMAIN "/tmp/unix_socket"
#define MAX_CONNECTION_NUMBER 10
#define MAX_SELECT_SOCKET 10

int utils_network_create_server()
{
    fd_set fset;
    fd_set fset_bak;
    int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    int ret;
    int reuse = 1;

    //create socket
    server_socket = socket(AF_UNIX,SOCK_STREAM,0);
    if(server_socket < 0)
    {
        LOG("can not create socket,err:%s",strerror(errno));
        return -1;
    }

    //allow reuse
    if((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse))) < 0)
    {
        LOG("setsockopt failed,err:%s",strerror(errno));
        close(server_socket);
        return -1;
    }

    //bind
    ret = unlink(UNIX_DOMAIN);
    if(ret < 0)
    {
        LOG("delete file error,err:%s",strerror(errno));
    }

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path,UNIX_DOMAIN,sizeof(server_addr.sun_path));
    ret = bind(server_socket,(struct sockaddr*)&server_addr,SUN_LEN(&server_addr));
    if(-1 == ret)
    {
        LOG("can not bind,err:%s",strerror(errno));
        close(server_socket);
        unlink(UNIX_DOMAIN);
        return -1;
    }

    ret = listen(server_socket,MAX_SELECT_SOCKET);
    if(-1 == ret)
    {
        LOG("can not listen,err:%s",strerror(errno));
        close(server_socket);
        unlink(UNIX_DOMAIN);
        return -1;
    }

    //add server socket to set
    FD_ZERO(&fset);
    FD_SET(server_socket,&fset);

    while(1)
    {
        int fd;
        char recv_data[1024];
        int read_len;
        fset_bak = fset;   //back up set,select would change the set
        ret = select(MAX_SELECT_SOCKET+1,&fset,&fset,&fset,NULL);
        if(0 == ret)
        {
            LOG("select timeout,err:%s",strerror(errno));
            continue;
        }else if(ret < 0)
        {
            LOG("select timeout,err:%s",strerror(errno));
            continue;
        }

        for(fd = 0;fd < MAX_SELECT_SOCKET;fd ++)
        {
            if(FD_ISSET(fd,&fset_bak))
            {
                //connection request
                if(fd == server_socket)
                {
                    client_socket = accept(server_socket,(struct sockaddr*)&server_addr,sizeof(struct sockaddr));
                    if(client_socket < 0)
                    {
                        LOG("cannot accept client connect request,err:%s",strerror(errno));
                        continue;
                    }
                    LOG("add client fd:%d",client_socket);
                }
                //client data request
                else
                {
                   
                   ioctl(fd,FIONREAD,&read_len);

                    //request data finish
                    if(0 == read_len)
                    {
                        close(fd);
                        FD_CLR(fd,&fset);
                        LOG("remove client fd:%d",fd);
                    }
                    //deal client request
                    else
                    {
                        ret = read(fd,recv_data,sizeof(recv_data));
                        LOG("read len:%d data:%s",ret,recv_data);
                        write(fd,recv_data,ret);
                    }
                }
            }
        }
    }

}


int utils_network_create_socket_server(void)
{
    socklen_t ctl_addr_len;
    int listen_fd;
    int com_fd;
    int ret;
    int i;
    char buf[1024];
    int len;
    struct sockaddr_un clt_addr;
    struct sockaddr_un srv_addr;
    
    listen_fd = socket(AF_UNIX,SOCK_STREAM,0);
    if(listen_fd < 0)
    {
        LOG("can not create socket,err:%s",strerror(errno));
        return -1;
    }
    srv_addr.sun_family = AF_UNIX;
    strncpy(srv_addr.sun_path,UNIX_DOMAIN,sizeof(srv_addr.sun_path)-1);
    unlink(UNIX_DOMAIN);

    /* bind server address  */
    ret = bind(listen_fd,(struct socketaddr*)&srv_addr,sizeof(srv_addr));
    if(-1 == ret)
    {
        LOG("can not bind,err:%s",strerror(errno));
        close(listen_fd);
        unlink(UNIX_DOMAIN);
        return -1;
    }

    //set max connection number
    ret = listen(listen_fd,MAX_CONNECTION_NUMBER);
    if(-1 == ret)
    {
        LOG("can not listen,err:%s",strerror(errno));
        close(listen_fd);
        unlink(UNIX_DOMAIN);
        return -1;
    }

    LOG("start listen...");
    while(1)
    {
        len = sizeof(clt_addr);
        LOG("wait task...");
        com_fd = accept(listen_fd,(struct sockaddr*)&clt_addr,&len);
        if(com_fd < 0)
        {
            LOG("cannot accept client connect request,err:%s",strerror(errno));
            close(listen_fd);
            unlink(UNIX_DOMAIN);
            return -1;
        }

        //read and printf sent client info
        bzero(buf,sizeof(buf));
        ret = read(com_fd,buf,sizeof(buf));
        if(ret < 0)
        {
           LOG("cannot read client message,err:%s",strerror(errno));
           continue;
        }
        LOG("Message from client (%d)) :%s/n",ret,buf);
        sleep(20);

        bzero(buf,sizeof(buf));
        strcpy(buf,"send from server");
        LOG("Write Message To client  :%s/n",buf);
        ret = write(com_fd,buf,sizeof(buf));
        if(ret < 0)
        {
            LOG("cannot write message to client,err:%s",strerror(errno));
        }
    }
    close(com_fd);
    close(listen_fd);
    
    LOG("create socket succ");
    return 0;
}



s32 utils_network_create_client(void)
{
    int connect_fd;
    int ret;
    char buf[256];
    struct sockaddr_un svr_addr;

    //LOG("current pid:%d",getpid());
    LOG("thread_one:int %d main process, the tid=%lu,pid=%ld\n",getpid(),pthread_self(),syscall(SYS_gettid));

    connect_fd = socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd < 0)
    {
        LOG("create socket failed,err:%d",strerror(errno));
        return -1;
    }

    /* connect to server*/
    svr_addr.sun_family = AF_UNIX;
    strcpy(svr_addr.sun_path,UNIX_DOMAIN);

    ret = connect(connect_fd,(struct sockaddr*)&svr_addr,sizeof(svr_addr));
    if(-1 == ret)
    {
        LOG("connect to server failed,err:%d",strerror(errno));
        close(connect_fd);
        return -1;
    }

    bzero(buf,sizeof(buf));
    strcpy(buf,"hello from client");
    ret = write(connect_fd,buf,sizeof(buf));
    if(ret < 0)
    {
        LOG("write to server failed,err:%d",strerror(errno));
        close(connect_fd);
        return -1;
    }

    bzero(buf,sizeof(buf));
    ret = read(connect_fd,buf,sizeof(buf));
    if(ret < 0)
    {
        LOG("read from server failed,err:%d",strerror(errno));
        close(connect_fd);
        return -1;
    }
    LOG("read from server:[%s]",buf);
    close(connect_fd);

    return 0;
}




