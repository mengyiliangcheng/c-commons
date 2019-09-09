/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/29  automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "udp_connection.h"
#include "udp_trans.h"
#include "main.h"

#define UDP_PRINT_HEX_CHUNK       (128)
#define UDP_PRINT_BTYES_PER_LINE  (8)
#define UDP_MAX_PROCESS_CHUNK     (128)

#ifndef MAX
#define MAX(a, b) (a) > (b) ? (a):(b)
#endif

#ifndef MIN
#define MIN(a, b) (a) < (b) ? (a):(b)
#endif

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))


bool Isprint(char c)
{
    return ((c > 0x20) && (c < 0x7e) ? true : false);
}

static void udp_print_hex(const void* buf,int len)
{
    int i = 0;
    int offset = 0;
    int write_num;
    char logbuffer[UDP_PRINT_HEX_CHUNK + 1];
    int logoffset = 0;
    unsigned char* src = (unsigned char*)buf;

    if(NULL == buf)
        return;

    BUILD_BUG_ON(UDP_MAX_PROCESS_CHUNK < (UDP_PRINT_BTYES_PER_LINE*5 + 8));

    while(len > 0)
    {
        write_num = MIN(len,UDP_PRINT_BTYES_PER_LINE);
        memset(logbuffer,0,sizeof(logbuffer));
        sprintf(logbuffer,"%07X ",offset * UDP_PRINT_BTYES_PER_LINE);
        logoffset = 8;

        for(i = 0;i < write_num;i ++)
        {
            //printf("\n%02X \n",(unsigned int )src[i]);
            sprintf(&logbuffer[logoffset],"%02X ",(unsigned int )src[i]);
            logoffset += 3;
        }

        sprintf(&logbuffer[logoffset],"%c ",'|');
        logoffset += 2;

        for(i = 0;i < write_num;i ++)
        {
            if(Isprint(src[i]))
            {
                sprintf(&logbuffer[logoffset],"%c ",(unsigned char )src[i]);
            }else
            {
                sprintf(&logbuffer[logoffset],"%c ",'.');
            }
            
            logoffset += 2;
        }

        LOG("%s",logbuffer);
        offset ++;
        src += write_num;
        len -= write_num;
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
            //printf("addrlen:%d sizeof addr:%d\n",p->ai_addrlen,sizeof(struct sockaddr));
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                LOG("can not connect,str:%d",Error());           
                close(s);
            }
            else
            {
                LOG("connect succ");
                conn = (ST_CONNECTION*)MALLOC(sizeof(ST_CONNECTION));
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
            LOG("can not create socket,err:%d",Error());
        }
    }

    if (NULL != serverinfo)
    {
        freeaddrinfo(serverinfo);
    }

    return conn;
}

void connection_free(ST_CONNECTION* conn)
{
    if(NULL == conn)
    {
        LOG_ERROR("Invalid Param");
        return ;
    }

    FREE(conn);
}

int connection_send(ST_CONNECTION* conn,unsigned char* buf,int len)
{
    int offset;
    int send_bytes_num;
    if(NULL == conn || NULL == buf || len < 0)
    {
        LOG_ERROR("Invalid Param");
        return -1;
    }

    print_ipaddr((struct sockaddr*)&(conn->addr));
    offset = 0;
    while(offset < len)
    {
        send_bytes_num = sendto(conn->sock,buf + offset, len - offset,0,(struct sockaddr*)&(conn->addr),conn->addrLen);
        if(send_bytes_num < 0)
        {
            LOG_ERROR("send failed,ret:%d",Error());
            return -1;
        }
        LOG_INFO("send %d bytes",send_bytes_num);
        udp_print_hex(buf + offset,send_bytes_num);
        offset += send_bytes_num;
    }

    return offset;
}

int connection_recv(ST_CONNECTION* conn,unsigned char* buf,int bufsize)
{
    struct sockaddr_storage addr_server;
    int server_len;
    int recv_num;

    server_len = sizeof(addr_server);
    recv_num = recvfrom(conn->sock,buf,bufsize,0,&addr_server,&server_len);
    if(recv_num < 0)
    {
        LOG("recv err:%s",strerror(errno));
        return -1;
    }
    LOG("recv %d bytes",recv_num);
    udp_print_hex(buf,recv_num);
    return 0;
}

