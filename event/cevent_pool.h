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

#ifndef __CEVENT_POOL_H__
#define __CEVENT_POOL_H__

typedef struct ST_POOL_API_STATE {
    int epfd;
    struct epoll_event *events;
}ST_POOL_API_STATE;

extern int ceApiPoll(ST_EVENT_LOOP *eventLoop, struct timeval *tvp);
extern int aeApiAddEvent(ST_EVENT_LOOP *eventLoop, int fd, int mask);
extern void aeApiDelEvent(ST_EVENT_LOOP *eventLoop, int fd, int delmask);


#endif
