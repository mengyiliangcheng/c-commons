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

#define CONFIG_BINDADDR_MAX 16


typedef struct ST_MC_SERVER 
{
    char* unixsocket;
    int sofd;             //unix socket file descriptor
    mode_t unixsocketperm;
    int tcp_backlog;     /* TCP listen() backlog */
    ST_EVENT_LOOP *el;
    char* neterr;
    int ipfd_count;
    int ipfd[CONFIG_BINDADDR_MAX]; /* TCP socket file descriptors */
    int stat_numconnections;
    int next_client_id;
}ST_MC_SERVER;

/* With multiplexing we need to take per-client state.
 * Clients are taken in a linked list. */
typedef struct client
{
    int id;            /* Client incremental unique ID. */
    int fd;                 /* Client socket. */
    char* querybuf;
}client;


#define UNIX_SOCKET "/tmp/mc_socket"

extern ST_MC_SERVER server;

extern int serverMain(void);


