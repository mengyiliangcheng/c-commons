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
#ifndef __ANET_H__
#define __ANET_H__

#define MAX_ACCEPTS_PER_CALL 1000
#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */
#define CLIENT_UNIX_SOCKET (1<<11) /* Client connected via Unix domain socket */

extern void acceptTcpHandler(ST_EVENT_LOOP *el, int fd, void *privdata, int mask);
extern void acceptUnixHandler(ST_EVENT_LOOP *el, int fd, void *privdata, int mask);

#endif
