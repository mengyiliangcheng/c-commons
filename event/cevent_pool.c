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
#include <time.h>
#include <sys/epoll.h>
#include <errno.h>
#include "osadapter.h"
#include "commons_log.h"
#include "cevent.h"
#include "cevent_pool.h"
#include "commons_log.h"

#define LOG(...) COMMONS_LOG("POOL",__VA_ARGS__)


#define zmalloc commons_malloc
#define zfree commons_free

int aeApiCreate(ST_EVENT_LOOP *eventLoop) 
{
    ST_POOL_API_STATE *state = zmalloc(sizeof(ST_POOL_API_STATE));

    if (!state) return -1;
    state->events = zmalloc(sizeof(struct epoll_event)*eventLoop->setsize);
    if (!state->events) 
    {
        zfree(state);
        return -1;
    }
    state->epfd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (state->epfd == -1) 
    {
        zfree(state->events);
        zfree(state);
        return -1;
    }
    eventLoop->poolApiData = state;
    return 0;
}

int aeApiAddEvent(ST_EVENT_LOOP *eventLoop, int fd, int mask) {
    ST_POOL_API_STATE *state = eventLoop->poolApiData;
    struct epoll_event ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = eventLoop->fileEvents[fd].mask == AE_NONE ?
            EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= eventLoop->fileEvents[fd].mask; /* Merge old events */
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->epfd,op,fd,&ee) == -1) 
    {
        LOG("epoll ctl failed,err:%s",strerror(errno));
        return -1;
    }
    return 0;
}


int aeApiPoll(ST_EVENT_LOOP *eventLoop, struct timeval *tvp)
{
    ST_POOL_API_STATE *state = eventLoop->poolApiData;
    int retval, numevents = 0;

    retval = epoll_wait(state->epfd,state->events,eventLoop->setsize,
            tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
    if (retval > 0) 
    {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) 
        {
            int mask = 0;
            struct epoll_event *e = state->events+j;

            if (e->events & EPOLLIN) mask |= AE_READABLE;
            if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
            if (e->events & EPOLLERR) mask |= AE_WRITABLE;
            if (e->events & EPOLLHUP) mask |= AE_WRITABLE;
            eventLoop->firedEvents[j].fd = e->data.fd;
            eventLoop->firedEvents[j].mask = mask;
        }
    }
    return numevents;
}

void aeApiDelEvent(ST_EVENT_LOOP *eventLoop, int fd, int delmask) {
    ST_POOL_API_STATE *state = eventLoop->poolApiData;
    struct epoll_event ee = {0}; /* avoid valgrind warning */
    int mask = eventLoop->fileEvents[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (mask != AE_NONE) {
        epoll_ctl(state->epfd,EPOLL_CTL_MOD,fd,&ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        epoll_ctl(state->epfd,EPOLL_CTL_DEL,fd,&ee);
    }
}



