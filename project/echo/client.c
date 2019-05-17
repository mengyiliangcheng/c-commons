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
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

#define SERVER_PORT 23643
#define SERVER_IP  "127.0.0.1"

int main(int argc,char* argv[])
{
    int fd = 0;
    int port = 0;
    int ret = 0;
    char ip[32] = {0};
    char readbuf[1024] = {0};
    char writebuf[1024] = {0};
    struct sockaddr_in server_addr;

    /* 创建套接字 */
    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd < 0)
    {
       LOG("Create Socket Failed,err:%d",errno);
       return -1;
    }

    if(NULL != argv[2] && strlen(argv[2]) > 0)
    {
        port = atoi(argv[2]);
    }
    else
    {
        port = SERVER_PORT;
    }

    if(NULL != argv[1] && strlen(argv[1]) > 0)
    {
        strncpy(ip,argv[1],sizeof(ip));
    }
    else
    {
        strcat(ip,SERVER_IP);
    }
    memset(&server_addr,0,sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_aton(ip,&server_addr.sin_addr);

    printf("Connect To %s:%d\n",ip,port);
    /* 连接服务器 */
    ret = connect(fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if(ret < 0)
    {
        LOG("Connect Failed,err:%d",errno);
        close(fd);
        return -1;
    }

    /* 和服务器交互 */
    while(1)
    {
        memset(readbuf,0,sizeof(readbuf));
        memset(writebuf,0,sizeof(writebuf));
        printf("INPUT >");
        scanf("%s",writebuf);

        /* 如果用户输入quit或者exit，客户端则退出 */
        if(0 == strncmp("quit",writebuf,4)
        || 0 == strncmp("exit",writebuf,4))
        {
            printf("Bye bye~\n");
            break;
        }

        /* 将用户输入发送到服务器 */
        write(fd,writebuf,strlen(writebuf));
        /* 等待服务器端回复 */
        ret = read(fd,readbuf,sizeof(readbuf));
        if(ret < 0)
        {
            LOG("Read From Server Failed,err:%d",errno);
            break;
        }
        printf("SERVER>%s\n",readbuf);
    }
    printf("exit...\n");
    close(fd);

    return 0;
}


