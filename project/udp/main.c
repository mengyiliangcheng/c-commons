/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/05  automatic
*/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

#define SERV_PORT 8000
#define DEST_PORT 8000
#define DEST_IP_ADDRESS "192.168.32.240"

void* client(void* args)
{
    while(1) 
    {
    sleep(1);
    LOG("client start");
    int sock_fd = socket(AF_INET,SOCK_DGRAM,0);
    if(sock_fd < 0)
    {
        LOG("create socket failed,err:%d",errno);
        return -1;
    }
    LOG("client fd:%d",sock_fd);

    int on=1;
    setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR | SO_BROADCAST,&on,sizeof(on));

    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv,0,sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(DEST_IP_ADDRESS);
    addr_serv.sin_port = htons(DEST_PORT);
    len = sizeof(addr_serv);

    int send_num;
    int recv_num;
    char send_buf[20] = "hi,who are you?";
    char recv_buf[100];

    LOG("client send to :%s",send_buf);
    send_num = sendto(sock_fd,send_buf,strlen(send_buf),0,
                (struct sockaddr*)&addr_serv,len);
    
    if(send_num < 0)
    {
        LOG("send err:%s",strerror(errno));
        return -1;
    }

    LOG("send %d bytes",send_num);
#if 1
    recv_num = recvfrom(sock_fd,recv_buf,sizeof(recv_buf),
                0,(struct sockaddr*)&addr_serv,(socklen_t*)&len);
    if(recv_num < 0)
    {
        LOG("recv err:%d",errno);
        return -1;
    }

    recv_buf[recv_num] = '\n';
    LOG("client receive %d bytes :%s",recv_num,recv_buf);
#endif
    close(sock_fd);
}
}

void print_ipaddr(struct sockaddr* addr)
{
    char buf[INET6_ADDRSTRLEN] = {0};
    if(NULL == addr)
    {
        printf("Invalid Parameter\n");
        return;
    }

    if(AF_INET == addr->sa_family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)addr;
        printf("IPV4 [%s]:[%hu]\n",
            inet_ntop(AF_INET,&saddr->sin_addr,buf,INET6_ADDRSTRLEN),
            ntohs(saddr->sin_port));
    }
    else if(AF_INET6 == addr->sa_family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)addr;
        printf("IPV6 [%s]:[%hu]\n",
            inet_ntop(AF_INET6,&saddr->sin6_addr,buf,INET6_ADDRSTRLEN),
            ntohs(saddr->sin6_port));
    }
    else
    {
        printf("Unknown IP Protcol\n");
    }
}

typedef struct
{
    int sock;
    struct sockaddr_in6 addr;
    size_t addrLen;
}ST_CONNECTION;

ST_CONNECTION* create_connection(char* host,char* port,int addrFamily)
{
    int ret;
    int s;
    struct addrinfo infos;
    struct addrinfo *serverinfo;
    struct addrinfo *p;
    //struct addrinfo *conn = NULL;
    ST_CONNECTION *conn = NULL;

    memset(&infos,0,sizeof(struct addrinfo));
    infos.ai_family = addrFamily;
    infos.ai_socktype = SOCK_DGRAM;

    ret = getaddrinfo(host,port,&infos,&serverinfo);
    if(0 != ret)
    {
        LOG("get addr info failed,err:%s",gai_strerror(ret));
        return NULL;
    }

    // we test the various addresses
    s = -1;
    for(p = serverinfo ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        LOG("create socket %d",s);
        if (s >= 0)
        {
            print_ipaddr(p->ai_addr);
            printf("addrlen:%d sizeof addr:%d\n",p->ai_addrlen,sizeof(struct sockaddr));
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                LOG("can not connect,str:%s",strerror(errno));
            }
            else
            {
                conn = (ST_CONNECTION*)malloc(sizeof(ST_CONNECTION));
                if(NULL == conn)
                {
                    LOG("malloc failed,err:%s",strerror(errno));
                }
                memset(conn,0,sizeof(ST_CONNECTION));
                memcpy(&(conn->addr),p->ai_addr,p->ai_addrlen);
                conn->addrLen = p->ai_addrlen;
            }
            
            close(s);
        }
        else
        {
            LOG("can not create socket,err:%s",strerror(errno));
        }
    }

    if (NULL != serverinfo)
    {
        freeaddrinfo(serverinfo);
    }

    return conn;
}

void* ipv6_client(void* args)
{
    //char* host = "fe80::329c:23ff:fe53:3aeb";
    char* host = "::1";
    char* port = "8888";

    struct addrinfo hints;
    struct addrinfo *serverinfo = NULL;
    struct addrinfo *p = NULL;
    struct sockaddr *sa = NULL;

    int ret;
    int s;
    int sl;

#if 0
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;

    ret = getaddrinfo(host,port,&hints,&serverinfo);
    if(0 != ret)
    {
        LOG("get addr info failed,err:%s",gai_strerror(ret));
        return ;
    }

    // we test the various addresses
    s = -1;
    for(p = serverinfo ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        LOG("create socket %d",s);
        if (s >= 0)
        {
            print_ipaddr(p->ai_addr);
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                LOG("can not connect,str:%s",strerror(errno));
                close(s);
                s = -1;
            }
        }
    }
    if(s <= 0)
    {
        LOG("create socket failed");
        return;
    }

    LOG("create socket succ");
#endif
    //struct sockaddr *sa = NULL;
    sleep(1);
    ST_CONNECTION * conn;
    conn = create_connection(host,port,AF_INET6);
    if(NULL == conn)
    {
        LOG("connection failed");
        return ;
    }
    LOG("create connection succ");

    s = socket(AF_INET6,SOCK_DGRAM,0);
    if(s < 0)
    {
        LOG("create socket failed,err:%s",strerror(errno));
        return ;
    }
    LOG("create socket succ");

    int len;

    int send_num;
    int recv_num;
    char send_buf[20] = "hi,who are you?";
    char recv_buf[100];
    int sock_fd = s;
    struct sockaddr_storage addr_server;
    int server_len;
    sl = 28;
    print_ipaddr(&(conn->addr));

    LOG("client send to :%s",send_buf);
    send_num = sendto(sock_fd,send_buf,strlen(send_buf),0,
                (struct sockaddr*)&(conn->addr),conn->addrLen);

    if(send_num < 0)
    {
        LOG("send err:%s",strerror(errno));
        return -1;
    }

    LOG("send %d bytes",send_num);

    server_len = sizeof(addr_server);
    recv_num = recvfrom(sock_fd,recv_buf,sizeof(recv_buf),0,&addr_server,&server_len);
    if(recv_num < 0)
    {
        LOG("recv err:%s",strerror(errno));
        return -1;
    }

    LOG("recv succ");

    close(s);
    if (NULL != serverinfo) {
        freeaddrinfo(serverinfo);
    }

    return;
}


int create_socket(const char * portStr, int addressFamily)
{
    int s = -1;
    int ret;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (0 != (ret =getaddrinfo(NULL, portStr, &hints, &res)))
    {
        LOG("getaddrinfo: %s",gai_strerror(ret));
        return -1;
    }

    for(p = res ; p != NULL && s == -1 ; p = p->ai_next)
    {
        LOG("111\n");
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }

    freeaddrinfo(res);

    return s;
}

void* ipv6_server(void* args)
{
    int sock_fd = create_socket("8888",AF_INET6);
    
    if(sock_fd < 0)
    {
        LOG("create socket failed,err:%d",errno);
        return -1;
    }
    LOG("server fd:%d",sock_fd);

    int len;
    int recv_num;
    int send_num;
    char send_buf[100] = "this is server";
    char recv_buf[100];
    struct sockaddr_storage addr_client;

    LOG("server start");
    while(1)
    {
        LOG("server wait");
        memset(recv_buf,0,sizeof(recv_buf));
        len = sizeof(struct sockaddr_storage); /* len要初始化，否则接收的客户端地址会有问题 */
        recv_num = recvfrom(
                sock_fd,
                recv_buf,
                sizeof(recv_buf),
                0,
                (struct sockaddr*)&addr_client,
                (socklen_t*)&len
                );
        if(recv_num < 0)
        {
            LOG("recvfrom err:%d",errno);
            return -1;
        }

        recv_buf[recv_num] = '\0';
        LOG("server receive(%d)[%s]\n",recv_num,recv_buf);
        printf("client addr len %d addr:\n",len);
        print_ipaddr(&addr_client);
        send_num = sendto(sock_fd,send_buf,recv_num,0,
                    (struct sockaddr*)&addr_client,len);
        if(send_num < 0)
        {
            LOG("send err:%s",strerror(errno));
            return -1;
        }
    }
    close(sock_fd);
}


void* server(void* args)
{
    int sock_fd = create_socket("8000",AF_INET);
    
    int sock_fd1 = socket(AF_INET,SOCK_DGRAM,0);
    if(sock_fd < 0)
    {
        LOG("create socket failed,err:%d",errno);
        return -1;
    }
    LOG("server fd:%d",sock_fd);

    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv,0,sizeof(struct sockaddr_in));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = SERV_PORT;

    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    len = sizeof(addr_serv);

    if(bind(sock_fd1,(struct sockaddr *)&addr_serv,sizeof(addr_serv)) < 0)
    {
        LOG("bind err:%d",errno);
        return -1;
    }

    LOG("bind on port:%d",addr_serv.sin_port);

    int recv_num;
    int send_num;
    char send_buf[100] = "this is server";
    char recv_buf[100];
    struct sockaddr_in addr_client;

    LOG("server start");
    while(1)
    {
        LOG("server wait");
        recv_num = recvfrom(
                sock_fd,
                recv_buf,
                sizeof(recv_buf),
                0,
                (struct sockaddr*)&addr_client,
                (socklen_t*)&len
                );
        if(recv_num < 0)
        {
            LOG("recvfrom err:%d",errno);
            return -1;
        }

        recv_buf[recv_num] = '\0';
        LOG("server receive %d bytes:%s\n",recv_num,recv_buf);

        send_num = sendto(sock_fd,send_buf,recv_num,0,
                    (struct sockaddr*)&addr_client,len);
        if(send_num < 0)
        {
            LOG("send err:%d",errno);
            return -1;
        }
    }
    close(sock_fd);
}



int main()
{
    pthread_t cli;
    pthread_t ser;
    int ret;
    
    ret = pthread_create(&cli,NULL,ipv6_client,NULL);
    if(ret < 0)
        printf("create thread failed");
#if 1
    ret = pthread_create(&ser,NULL,ipv6_server,NULL);
    if(ret < 0)
        printf("create thread failed");
#endif
    while(1)
        sleep(1);

    printf("hello world");
    return 0;
}


