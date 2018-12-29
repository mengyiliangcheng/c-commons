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
#include "commons_log.h"
#include "commons_type.h"
#include "osadapter.h"
#include "cevent.h"
#include "mc_server.h"
#include "anet.h"

#define LOG(...) COMMONS_LOG("SERVER",__VA_ARGS__)
ST_MC_SERVER server;

int serverCron(struct ST_EVENT_LOOP* eventLoop,size_id id,void* clientData)
{
    LOG("serverCron process");
    return 10000;
}

void initServer(void)
{
    int ret;
    int j;

    server.neterr = commons_malloc(100);
    server.ipfd_count = 10;
    
    /* Open the listening Unix domain socket. */
    if (server.unixsocket != NULL) {
        unlink(server.unixsocket); /* don't care if this fails */
        server.sofd = anetUnixServer(server.unixsocket,
            server.unixsocketperm, server.tcp_backlog);
        if (server.sofd == -1) {
            LOG("Opening Unix socket failed");
        }
        anetNonBlock(NULL,server.sofd);
        
        LOG("open  listening Unix domain socket succ,fd:%d",server.sofd);
    }

    server.el = aeCreateEventLoop(10);
    if(!server.el)
    {
        LOG("create event loop failed");
        return -1;
    }

    //create time event
    ret = aeCreateTimeEvent(server.el,10000,serverCron,NULL,NULL);
    if(ret < 0)
    {
        LOG("create time event failed");
        return -1;
    }

#if 0
     /* Create an event handler for accepting new connections in TCP and Unix
     * domain sockets. */
    for (j = 0; j < server.ipfd_count; j++) {
        if (aeCreateFileEvent(server.el, server.ipfd[j], AE_READABLE,
            acceptTcpHandler,NULL) == AE_ERR)
            {
                LOG("Unrecoverable error creating server.ipfd file event.");
            }
    }
#endif
    if (server.sofd > 0 && aeCreateFileEvent(server.el,server.sofd,AE_READABLE,
        acceptUnixHandler,NULL) == AE_ERR) LOG("Unrecoverable error creating server.sofd file event.");
}


int serverMain(void)
{
    int ret;
    
    server.unixsocket = commons_malloc(strlen(UNIX_SOCKET) + 1);
    if(server.unixsocket)
    {
        strcat(server.unixsocket,UNIX_SOCKET);
    }

    initServer();

    aeMain(server.el);
}



