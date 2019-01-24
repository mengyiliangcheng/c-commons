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
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "osadapter.h"
#include "cevent.h"
#include "mc_server.h"
#include "anet.h"

#define LOG(...) COMMONS_LOG("ANET",__VA_ARGS__)

static int anetGenericAccept(char *err, int s, struct sockaddr *sa, socklen_t *len) {
    int fd;
    while(1) {
        fd = accept(s,sa,len);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
                LOG("accept: %s", strerror(errno));
                return -1;
            }
        }
        break;
    }
    return fd;
}


int anetTcpAccept(char *err, int s, char *ip, size_t ip_len, int *port) {
    int fd;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    if ((fd = anetGenericAccept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return -1;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return fd;
}

void acceptTcpHandler(ST_EVENT_LOOP *el, int fd, void *privdata, int mask) {
    int cport, cfd, max = MAX_ACCEPTS_PER_CALL;
    char cip[NET_IP_STR_LEN];
    UNUSED(el);
    UNUSED(mask);
    UNUSED(privdata);

    while(max--) {
        cfd = anetTcpAccept(NULL, fd, cip, sizeof(cip), &cport);
        if (cfd == -1) {
            if (errno != EWOULDBLOCK)
                LOG("Accepting client connection");
            return;
        }
        LOG("Accepted %s:%d", cip, cport);
        //acceptCommonHandler(cfd,0,cip);
    }
}

void readQueryFromClient(ST_EVENT_LOOP *el, int fd, void *privdata, int mask) {
    client *c = (client*) privdata;
    int nread, readlen;
    size_t qblen;
    UNUSED(el);
    UNUSED(mask);

    c->querybuf = commons_malloc(1024);

    nread = read(fd, c->querybuf, 1024);
    if (nread == -1) {
        if (errno == EAGAIN) {
            return;
        } else {
            LOG("Reading from client: %s",strerror(errno));
            freeClient(c);
            return;
        }
    } else if (nread == 0) {
        LOG("Client closed connection");
        freeClient(c);
        return;
    }
    LOG("Reading from client: %s",c->querybuf);

    nread = write(fd,c->querybuf,strlen(c->querybuf));
    if(nread < 0)
    {
        LOG("write to client err: %s",strerror(errno));
    }
}



static int anetSetReuseAddr(char *err, int fd) {
    int yes = 1;
    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        LOG("setsockopt SO_REUSEADDR: %s", strerror(errno));
        return -1;
    }
    return 0;
}


static int anetCreateSocket(char *err, int domain) {
    int s;
    if ((s = socket(domain, SOCK_STREAM, 0)) == -1) {
        LOG("creating socket: %s", strerror(errno));
        return -1;
    }

    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (anetSetReuseAddr(err,s) == -1) {
        close(s);
        return -1;
    }
    return s;
}

static int anetListen(char *err, int s, struct sockaddr *sa, socklen_t len, int backlog) {
    if (bind(s,sa,len) == -1) {
        LOG("bind: %s", strerror(errno));
        close(s);
        return -1;
    }

    if (listen(s, backlog) == -1) {
        LOG("listen: %s", strerror(errno));
        close(s);
        return -1;
    }
    return 0;
}


int anetSetBlock(char *err, int fd, int non_block) {
    int flags;

    /* Set the socket blocking (if non_block is zero) or non-blocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        LOG("fcntl(F_GETFL): %s", strerror(errno));
        return -1;
    }

    if (non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1) {
        LOG("fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return -1;
    }
    return 0;
}

int anetNonBlock(char *err, int fd) {
    return anetSetBlock(err,fd,1);
}

static int anetSetTcpNoDelay(char *err, int fd, int val)
{
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1)
    {
        LOG("setsockopt TCP_NODELAY: %s", strerror(errno));
        return -1;
    }
    return 0;
}


int anetEnableTcpNoDelay(char *err, int fd)
{
    return anetSetTcpNoDelay(err, fd, 1);
}

client *createClient(int fd) {
    client *c = commons_malloc(sizeof(client));

    /* passing -1 as fd it is possible to create a non connected client.
     * This is useful since all the commands needs to be executed
     * in the context of a client. When commands are executed in other
     * contexts (for instance a Lua script) we need a non connected client. */
    if (fd != -1) {
        //anetNonBlock(NULL,fd);
        //anetEnableTcpNoDelay(NULL,fd);
        if (aeCreateFileEvent(server.el,fd,AE_READABLE,
            readQueryFromClient, c) == AE_ERR)
        {
            close(fd);
            commons_free(c);
            return NULL;
        }
    }

    c->id = server.next_client_id++;
    c->fd = fd;
    LOG("create client succ");
    return c;
}


int anetUnixServer(char *path, mode_t perm, int backlog)
{
    int s;
    struct sockaddr_un sa;

    if ((s = anetCreateSocket(NULL,AF_LOCAL)) == -1)
        return -1;

    memset(&sa,0,sizeof(sa));
    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
    if (anetListen(NULL,s,(struct sockaddr*)&sa,sizeof(sa),backlog) == -1)
        return -1;
    if (perm)
        chmod(sa.sun_path, perm);
    return s;
}

int anetUnixAccept(char *err, int s) {
    int fd;
    struct sockaddr_un sa;
    socklen_t salen = sizeof(sa);
    if ((fd = anetGenericAccept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return -1;

    return fd;
}

void acceptUnixHandler(ST_EVENT_LOOP *el, int fd, void *privdata, int mask) {
    int cfd, max = MAX_ACCEPTS_PER_CALL;
    UNUSED(el);
    UNUSED(mask);
    UNUSED(privdata);
    LOG("acceptUnixHandler");

    while(max--) {
        cfd = anetUnixAccept(NULL, fd);
        if (cfd == -1) {
            if (errno != EWOULDBLOCK)
                LOG("Accepting client connection:");
            return;
        }
        LOG("Accepted connection to %s,cfd:%d", server.unixsocket,cfd);
        acceptCommonHandler(cfd,CLIENT_UNIX_SOCKET,NULL);
    }
}


void acceptCommonHandler(int fd, int flags, char *ip) {
    client *c;
    if ((c = createClient(fd)) == NULL) {
        LOG("Error registering fd event for the new client: %s (fd=%d)",
            strerror(errno),fd);
        close(fd); /* May be already closed, just ignore errors */
        return;
    }

    server.stat_numconnections++;
    
    LOG("acceptCommonHandler finish");
}

void freeClient(client *c)
{
    aeDeleteFileEvent(server.el,c->fd,AE_WRITABLE);
    aeDeleteFileEvent(server.el,c->fd,AE_READABLE);
    close(c->fd);
    if(!c) return;
    if(c->querybuf)
    commons_free(c->querybuf);
    if(c)
    commons_free(c);
}







