/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/12  automatic
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

void print_ipaddr(struct sockaddr* addr)
{
    char buf[INET6_ADDRSTRLEN] = {0};

    //printf("ip family:%d\n",addr->sa_family);
    //printf("ipv4 is :%d ipv6 is :%d\n",AF_INET,AF_INET6);
    if(AF_INET == addr->sa_family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)addr;
        printf("IPV4 [%s]:[%d]\n",inet_ntop(AF_INET,&saddr->sin_addr,buf,INET6_ADDRSTRLEN),saddr->sin_port);
        
    }
    else if(AF_INET6 == addr->sa_family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)addr;
        printf("IPV6 [%s]:[%d]\n",inet_ntop(AF_INET6,&saddr->sin6_addr,buf,INET6_ADDRSTRLEN),saddr->sin6_port);
    }
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
        char buf[16];
        struct sockaddr_in *addr;
        addr = (struct sockaddr_in *)p->ai_addr;
        //LOG("%s\n",inet_ntop(AF_INET,&addr->sin_addr, buf, 16));
        print_ipaddr(p->ai_addr);
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



int main()
{
    char* port = "11111";
    int addr_family = AF_INET6;
    int sock_fd = create_socket(port,addr_family);
    printf("create socket:%d\n",sock_fd);

    int len = 0;
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
        LOG("server send %s to socket %d",send_buf,sock_fd);
        send_num = sendto(sock_fd,send_buf,strlen(send_buf),0,
                    (struct sockaddr*)&addr_client,len);
        if(send_num < 0)
        {
            LOG("send err:%s",strerror(errno));
            return -1;
        }
    }
    close(sock_fd);

    printf("hello world\n");
    return 0;
}


