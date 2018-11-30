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
#include "commons_log.h"

#define LOG(...) COMMONS_LOG("NETWORK",__VA_ARGS__);
#define UNIX_DOMAIN "/tmp/UNIX.domain"
#define MAX_CONNECTION_NUMBER 10


int utils_network_create_server(void)
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

    connect_fd = socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd < 0)
    {
        LOG("create socket failed,err:%d",strerror(errno));
        return -1;
    }

    /*    */
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




