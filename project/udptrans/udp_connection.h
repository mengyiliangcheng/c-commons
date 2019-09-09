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

#ifndef __UDP_CONNECTION_H__
#define __UDP_CONNECTION_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct
{
    int sock;
    struct sockaddr_in6 addr;
    size_t addrLen;
}ST_CONNECTION;

typedef struct _ST_MG_UDP_PACKAGE
{
    int mid;
    char* payload;
    int payload_len;
}ST_MG_UDP_PACKAGE;

typedef struct _ST_MG_UDP_HANDLE
{
    char* host;
    char* port;
    int addr_family;
    ST_CONNECTION* conn;
}ST_MG_UDP_HANDLE;

#define Error() errno

extern int connection_send(ST_CONNECTION* conn,unsigned char* buf,int len);
extern ST_CONNECTION* create_connection(char* host,char* port,int addrFamily);
extern void connection_free(ST_CONNECTION* conn);

//typedef int (*udp_recv_callback)(unsigned char* buf,int bufsize)

extern int connection_recv(ST_CONNECTION* conn,unsigned char* buf,int bufsize);

#endif

