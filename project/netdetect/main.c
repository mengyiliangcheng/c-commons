/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/19  automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
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

#define MAX_OPT_DATA_META_LEN   64

typedef struct
{
    int data_type;
    union
    {
        int asInt;
        char asChar;
        char asString[MAX_OPT_DATA_META_LEN];
    }data;
}ST_OPT_DATA_META;

typedef struct
{
    ST_OPT_DATA_META ip_family;
    ST_OPT_DATA_META port;
    ST_OPT_DATA_META sock_type;
    ST_OPT_DATA_META ip_addr;
    ST_OPT_DATA_META payload;
}ST_CLIENT_CONF;

ST_CLIENT_CONF stClientCfg;

#define MAX_OPT_LEN 16
#define MAX_OPT_INFO_LEN 128

typedef enum
{
    OPT_TYPE_NONE,
    OPT_TYPE_INT,
    OPT_TYPE_CHAR,
    OPT_TYPE_STRING,
}E_OPT_TYPE;

typedef struct 
{
    char *opts;
    ST_OPT_DATA_META* data_meta;
    int data_type;
    bool args_num;
    char *info;
}ST_CLIENT_OPT;

ST_CLIENT_OPT stClientOpt[] = 
{
    {"-h",          NULL,                     OPT_TYPE_NONE,     1,     "Print Help Menu"},
    {"-f",          &(stClientCfg.ip_family), OPT_TYPE_INT,     1,     "IP Family Request,[4/6] ipv4/ipv6"},
    {"-protocol",   &(stClientCfg.sock_type), OPT_TYPE_CHAR,    1,     "Protocol Requeset,[u/t] udp/tcp"},
    {"-port",       &(stClientCfg.port),      OPT_TYPE_INT,     1,     "Destination Port,[0~65535]"},
    {"-a",          &(stClientCfg.ip_addr),   OPT_TYPE_STRING,  1,     "Destination IP Address"},
    {"-payload",    &(stClientCfg.payload),   OPT_TYPE_STRING,  1,     "Request Message Payload"},
};

void showHelp(void)
{
    int i = 0;
    int num = sizeof(stClientOpt) / sizeof(ST_CLIENT_OPT);
    printf("*******************command***********************************\n");
    for(i = 0;i < num;i ++)
    {
        printf("*%-10s [%s]\n",stClientOpt[i].opts,stClientOpt[i].info);
    }
    printf("*******************command***********************************\n");
}

void printConf(void)
{
    printf("*******************configure*********************************\n");
    printf("*ip family:         %d\n",stClientCfg.ip_family.data.asInt);
    printf("*destnation port:   %d\n",stClientCfg.port.data.asInt);
    printf("*socket type:       %c\n",stClientCfg.sock_type.data.asChar);
    printf("*ip address:        %s\n",stClientCfg.ip_addr.data.asString);
    printf("*payload:           %s\n",stClientCfg.payload.data.asString);
    printf("*******************configure end*****************************\n");
}

void printArgs(int argc,char* argv[])
{
    int i = 0;
    for(i = 0;i < argc;i ++)
    {
        LOG("Args %02d %s",i,argv[i]);
    }
}

int parseCmdline(int argc,char* argv[])
{
    int i = 0,j = 0;
    printArgs(argc,argv);

    //memset(&stClientCfg,0,sizeof(ST_CLIENT_CONF));

    showHelp();
    int num = sizeof(stClientOpt) / sizeof(ST_CLIENT_OPT);
    for(i = 1;i < argc;i ++)
    {
        LOG("%d %s",i,argv[i]);
        if(strlen(argv[i]) > MAX_OPT_LEN)
        {
            LOG("err command");
            showHelp();
            return -1;
        }

        for(j = 0;j < num;j ++)
        {
            if(0 == strncmp(stClientOpt[j].opts,argv[i],strlen(stClientOpt[j].opts)))
            {
                i++;
                if(OPT_TYPE_INT == stClientOpt[j].data_type)
                {
                    stClientOpt[j].data_meta->data_type = OPT_TYPE_INT;
                    stClientOpt[j].data_meta->data.asInt = atoi(argv[i]);
                    break;
                }
                else if(OPT_TYPE_STRING == stClientOpt[j].data_type)
                {
                    stClientOpt[j].data_meta->data_type = OPT_TYPE_STRING;
                    strncpy(stClientOpt[j].data_meta->data.asString,argv[i],MAX_OPT_DATA_META_LEN);
                    break;
                }else if(OPT_TYPE_CHAR == stClientOpt[j].data_type)
                {
                    stClientOpt[j].data_meta->data_type = OPT_TYPE_CHAR;
                    stClientOpt[j].data_meta->data.asChar = argv[i][0];
                    break;
                }else if(OPT_TYPE_NONE == stClientOpt[j].data_type)
                {
                    showHelp();
                    return -1;
                }
            }
        }
        if(j >= num)
        {
            LOG("Unkown Command:%s",argv[i]);
            showHelp();
            return -1;        
        }
    }

    printConf();

    return 0;
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

ST_CONNECTION* create_connection(char* host,char* port,int addrFamily,int socktype)
{
    int ret;
    int s;
    struct addrinfo infos;
    struct addrinfo *serverinfo;
    struct addrinfo *p;
    //struct addrinfo *conn = NULL;
    ST_CONNECTION *conn = NULL;

    //LOG("host:%s port:%s,ip family:%d socktype:%d",host,port,addrFamily,socktype);

    memset(&infos,0,sizeof(struct addrinfo));
    infos.ai_family = addrFamily;
    infos.ai_socktype = socktype;

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
        //LOG("create socket %d",s);
        if (s >= 0)
        {
            print_ipaddr(p->ai_addr);
            //printf("addrlen:%d sizeof addr:%d\n",p->ai_addrlen,sizeof(struct sockaddr));
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                LOG("can not connect,str:%s",strerror(errno));
                close(s);
            }
            else
            {
                LOG("connect succ");
                conn = (ST_CONNECTION*)malloc(sizeof(ST_CONNECTION));
                if(NULL == conn)
                {
                    LOG("malloc failed,err:%s",strerror(errno));
                }
                memset(conn,0,sizeof(ST_CONNECTION));
                memcpy(&(conn->addr),p->ai_addr,p->ai_addrlen);
                conn->addrLen = p->ai_addrlen;
                conn->sock = s;
                break;
            }
            
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

void* signal_handler(int signal_num)
{
    printf("num:%d\n",signal_num);
    return 0;
}

void initClientCfg(void)
{
    stClientCfg.ip_family.data.asInt = 4;
    stClientCfg.sock_type.data.asChar = 't';
    stClientCfg.port.data.asInt = 80;
    
    memset(stClientCfg.ip_addr.data.asString,0,sizeof(stClientCfg.ip_addr.data.asString));
    strcat(stClientCfg.ip_addr.data.asString,"www.baidu.com");
    memset(stClientCfg.payload.data.asString,0,sizeof(stClientCfg.payload.data.asString));
    strcat(stClientCfg.payload.data.asString,"hi,who are you?");
}

int main(int argc,char* argv[])
{
    int ip_family = AF_INET;
    int socktype = SOCK_DGRAM;
    char port[16] = {0};

    int send_num;
    int recv_num;
    char send_buf[20] = "hi,who are you?";
    char recv_buf[100];
    struct sockaddr_storage addr_server;
    int server_len;
    int ret;

    signal(SIGSEGV,signal_handler);

    initClientCfg();
    ret = parseCmdline(argc,argv);
    if(ret < 0)
        return -1;

    if(4 == stClientCfg.ip_family.data.asInt)
        ip_family = AF_INET;
    else if(6 == stClientCfg.ip_family.data.asInt)
        ip_family = AF_INET6;

    if('u' == stClientCfg.sock_type.data.asChar)
    {
        socktype = SOCK_DGRAM;
    }
    else if('t' == stClientCfg.sock_type.data.asChar)
    {
        socktype = SOCK_STREAM;
    }

    sprintf(port,"%d",stClientCfg.port.data.asInt);
    memset(send_buf,0,sizeof(send_buf));
    strcat(send_buf,stClientCfg.payload.data.asString);

    ST_CONNECTION* conn = create_connection(
        stClientCfg.ip_addr.data.asString,
        port,
        ip_family,
        socktype
        );

    if(NULL == conn)
    {
        LOG("connection failed");
        return -1;
    }
    LOG("create connection succ");

    LOG("client send :%s",send_buf);
    if(socktype == SOCK_DGRAM)
    {
        send_num = sendto(
                        conn->sock,
                        send_buf,
                        strlen(send_buf),
                        0,
                        (struct sockaddr*)&(conn->addr),
                        conn->addrLen
                        );
    }
    else
    {
        send_num = write(conn->sock,send_buf,strlen(send_buf));
    }

    if(send_num < 0)
    {
        LOG("send err:%s",strerror(errno));
        ret = -1;
        goto END;
    }
    //LOG("send %d bytes",send_num);

    if(socktype == SOCK_DGRAM)
    {
        server_len = sizeof(addr_server);
        recv_num = recvfrom(
                            conn->sock,
                            recv_buf,
                            sizeof(recv_buf),
                            0,
                            (struct sockaddr*)&addr_server,
                            (socklen_t *)&server_len
                            );
    }
    else
    {
        memset(recv_buf,0,sizeof(recv_buf));
        recv_num = read(conn->sock,recv_buf,sizeof(recv_buf));
    }
    if(recv_num < 0)
    {
        LOG("recv err:%s",strerror(errno));
        ret = -1;
        goto END;
    }

    LOG("recv succ");
    LOG("%s",recv_buf);

END:
    close(conn->sock);
    free(conn);

    return ret;
}


