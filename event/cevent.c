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
#include "osadapter.h"
#include "commons_log.h"
#include "cevent.h"

#define LOG(...) COMMONS_LOG("EVENT",__VA_ARGS__);

static void aeGetTime(long *seconds, long *milliseconds)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}


static void aeAddMillisecondsToNow(long long milliseconds, long *sec, long *ms) {
    long cur_sec, cur_ms, when_sec, when_ms;

    aeGetTime(&cur_sec, &cur_ms);
    when_sec = cur_sec + milliseconds/1000;
    when_ms = cur_ms + milliseconds%1000;
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }
    *sec = when_sec;
    *ms = when_ms;
}

static ST_EVENT_TIME *aeSearchNearestTimer(ST_EVENT_LOOP *eventLoop)
{
    ST_EVENT_TIME *te = eventLoop->timeEventHead;
    ST_EVENT_TIME *nearest = NULL;

    while(te) {
        if (!nearest || te->when_sec < nearest->when_sec ||
                (te->when_sec == nearest->when_sec &&
                 te->when_ms < nearest->when_ms))
            nearest = te;
        te = te->next;
    }
    return nearest;
}

void aeDeleteFileEvent(ST_EVENT_LOOP *eventLoop, int fd, int mask)
{
    if (fd >= eventLoop->setsize) return;
    ST_EVENT_FILE *fe = &eventLoop->fileEvents[fd];
    if (fe->mask == AE_NONE) return;

    /* We want to always remove AE_BARRIER if set when AE_WRITABLE
     * is removed. */
    if (mask & AE_WRITABLE) mask |= AE_BARRIER;

    aeApiDelEvent(eventLoop, fd, mask);
    fe->mask = fe->mask & (~mask);
    if (fd == eventLoop->maxfd && fe->mask == AE_NONE) {
        /* Update the max fd */
        int j;

        for (j = eventLoop->maxfd-1; j >= 0; j--)
            if (eventLoop->fileEvents[j].mask != AE_NONE) break;
        eventLoop->maxfd = j;
    }
}


int aeCreateFileEvent(ST_EVENT_LOOP *eventLoop, int fd, int mask,
        eventFileProc *proc, void *clientData)
{
    if (fd >= eventLoop->setsize) {
        return AE_ERR;
    }
    ST_EVENT_FILE *fe = &eventLoop->fileEvents[fd];

    LOG("add fd:%d to event",fd);
    if (aeApiAddEvent(eventLoop, fd, mask) == -1)
        return AE_ERR;
    fe->mask |= mask;
    if (mask & AE_READABLE) fe->rFileProc = proc;
    if (mask & AE_WRITABLE) fe->wFileProc = proc;
    fe->clientData = clientData;
    if (fd > eventLoop->maxfd)
        eventLoop->maxfd = fd;
    return AE_OK;
}


long long aeCreateTimeEvent(ST_EVENT_LOOP *eventLoop, long long milliseconds,
        eventTimeProc *proc, void *clientData,
        eventFinalizerProc *finalizerProc)
{
    long long id = eventLoop->timeEventNextId++;
    ST_EVENT_TIME *te;

    te = commons_malloc(sizeof(*te));
    if (te == NULL) return -1;
    te->id = id;
    aeAddMillisecondsToNow(milliseconds,&te->when_sec,&te->when_ms);
    te->timeProc = proc;
    te->finalizerProc = finalizerProc;
    te->clientData = clientData;
    te->prev = NULL;
    te->next = eventLoop->timeEventHead;
    if (te->next)
        te->next->prev = te;
    eventLoop->timeEventHead = te;
    return id;
}

ST_EVENT_LOOP* aeCreateEventLoop(int setsize)
{
    ST_EVENT_LOOP* eventLoop;
    int i;

    eventLoop = commons_malloc(sizeof(ST_EVENT_LOOP));
    if(NULL == eventLoop) goto err;
    eventLoop->fileEvents = commons_malloc(sizeof(ST_EVENT_FILE)*setsize);
    eventLoop->firedEvents = commons_malloc(sizeof(ST_EVENT_FIRED)*setsize);
    if(NULL == eventLoop->fileEvents || NULL == eventLoop->firedEvents) goto err;

    eventLoop->setsize = setsize;
    eventLoop->lastTime = time(NULL);
    eventLoop->timeEventHead = NULL;
    eventLoop->timeEventNextId = 0;
    eventLoop->stop = 0;
    eventLoop->maxfd = -1;
    eventLoop->beforeSleepProc = NULL;
    eventLoop->afterSleepProc = NULL;

    if(aeApiCreate(eventLoop) < 0) goto err;

    for (i = 0;i < setsize;i ++)
        eventLoop->fileEvents[i].mask = AE_NONE;
    return eventLoop;

err:
    if(eventLoop)
    {
        commons_free(eventLoop->fileEvents);
        commons_free(eventLoop->firedEvents);
        commons_free(eventLoop);
    }
}


/* Process time events */
static int processTimeEvents(ST_EVENT_LOOP *eventLoop) {
    int processed = 0;
    ST_EVENT_TIME *te;
    long long maxId;
    time_t now = time(NULL);

    /* If the system clock is moved to the future, and then set back to the
     * right value, time events may be delayed in a random way. Often this
     * means that scheduled operations will not be performed soon enough.
     *
     * Here we try to detect system clock skews, and force all the time
     * events to be processed ASAP when this happens: the idea is that
     * processing events earlier is less dangerous than delaying them
     * indefinitely, and practice suggests it is. */
    if (now < eventLoop->lastTime) {
        te = eventLoop->timeEventHead;
        while(te) {
            te->when_sec = 0;
            te = te->next;
        }
    }
    eventLoop->lastTime = now;

    te = eventLoop->timeEventHead;
    maxId = eventLoop->timeEventNextId-1;
    while(te) {
        long now_sec, now_ms;
        long long id;

        /* Remove events scheduled for deletion. */
        if (te->id == AE_DELETED_EVENT_ID) {
            ST_EVENT_TIME *next = te->next;
            if (te->prev)
                te->prev->next = te->next;
            else
                eventLoop->timeEventHead = te->next;
            if (te->next)
                te->next->prev = te->prev;
            if (te->finalizerProc)
                te->finalizerProc(eventLoop, te->clientData);
            commons_free(te);
            te = next;
            continue;
        }

        /* Make sure we don't process time events created by time events in
         * this iteration. Note that this check is currently useless: we always
         * add new timers on the head, however if we change the implementation
         * detail, this check may be useful again: we keep it here for future
         * defense. */
        if (te->id > maxId) {
            te = te->next;
            continue;
        }
        aeGetTime(&now_sec, &now_ms);
        if (now_sec > te->when_sec ||
            (now_sec == te->when_sec && now_ms >= te->when_ms))
        {
            int retval;

            id = te->id;
            retval = te->timeProc(eventLoop, id, te->clientData);
            processed++;
            if (retval != AE_NOMORE) {
                aeAddMillisecondsToNow(retval,&te->when_sec,&te->when_ms);
            } else {
                te->id = AE_DELETED_EVENT_ID;
            }
        }
        te = te->next;
    }
    return processed;
}


int aeProcessEvent(ST_EVENT_LOOP* eventLoop,int flags)
{
    int processed = 0, numevents;
    struct timeval tv, *tvp;
    tvp = &tv;

    ST_EVENT_TIME *shortest = NULL;
    shortest = aeSearchNearestTimer(eventLoop);
    if(shortest)
    {
        LOG("find shortest event");
        long now_sec, now_ms;

        aeGetTime(&now_sec, &now_ms);
        tvp = &tv;

        /* How many milliseconds we need to wait for the next
         * time event to fire? */
        long long ms =
            (shortest->when_sec - now_sec)*1000 +
            shortest->when_ms - now_ms;
        LOG("ms:%ld",ms);

        if (ms > 0) {
            tvp->tv_sec = ms/1000;
            tvp->tv_usec = (ms % 1000)*1000;
        } else {
            tvp->tv_sec = 0;
            tvp->tv_usec = 0;
        }
    }else
    {
        LOG("can not find shortest event");
        tvp = NULL;
    }
    numevents = aeApiPoll(eventLoop,tvp);
    LOG("events number:%d",numevents);
    if(eventLoop->afterSleepProc)
        eventLoop->afterSleepProc(eventLoop);
    //usleep(50000);
    if(numevents > 0)
    {
        int j;
        ST_EVENT_FILE* fe = NULL;
        for(j = 0;j < numevents;j ++)
        {
            int fd = eventLoop->firedEvents[j].fd;
            LOG("fired fd:%d",fd);
            fe = &eventLoop->fileEvents[eventLoop->firedEvents[j].fd];

            if(fe->mask & AE_READABLE)
            {
                if(fe->rFileProc)
                {
                    fe->rFileProc(eventLoop,fd,fe->clientData,fe->mask);
                }else
                {
                    LOG("rFileProc is null");
                }
            }

            if(fe->mask & AE_WRITABLE)
            {
                if(fe->wFileProc)
                {
                    fe->wFileProc(eventLoop,fd,fe->clientData,fe->mask);
                }else
                {
                    LOG("wFileProc is null");
                }
            }
        }
    }
    if(flags & AE_TIME_EVENTS)
    {
        processTimeEvents(eventLoop);
    }
}


int aeMain(ST_EVENT_LOOP* eventLoop)
{
    eventLoop->stop = 0;
    
    while(!eventLoop->stop)
    {
        if(eventLoop->beforeSleepProc)
            eventLoop->beforeSleepProc(eventLoop);
        aeProcessEvent(eventLoop,AE_ALL_EVENTS);
    }
}




