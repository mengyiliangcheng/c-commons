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

#ifndef __CEVENT_H__
#define __CEVENT_H__

#include <time.h>

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4
#define AE_CALL_AFTER_SLEEP 8

#define AE_NONE 0       /* No events registered. */
#define AE_READABLE 1   /* Fire when descriptor is readable. */
#define AE_WRITABLE 2   /* Fire when descriptor is writable. */
#define AE_BARRIER 4    /* With WRITABLE, never fire the event if the
                           READABLE event already fired in the same event
                           loop iteration. Useful when you want to persist
                           things to disk before sending replies, and want
                           to do that in a group fashion. */

#define AE_DELETED_EVENT_ID -1
#define AE_NOMORE -1

#define AE_OK 0
#define AE_ERR -1



typedef long long size_id;

typedef void eventFileProc(struct ST_EVENT_LOOP* eventLoop,int fd,void* clientData,int mask);
typedef int eventTimeProc(struct ST_EVENT_LOOP* eventLoop,size_id id,void* clientData);
typedef void eventFinalizerProc(struct ST_EVENT_LOOP* eventLoop,void* clientData);
typedef void eventBeforeSleepProc(struct ST_EVENT_LOOP* eventLoop);

typedef struct ST_EVENT_FILE
{
    int mask;     //
    eventFileProc* rFileProc;
    eventFileProc* wFileProc;
    char* clientData;
}ST_EVENT_FILE;


typedef struct ST_EVENT_TIME 
{
    size_id id;
    long when_sec;
    long when_ms;
    eventTimeProc* timeProc;
    eventFinalizerProc* finalizerProc;
    void* clientData;
    struct ST_EVENT_TIME *prev;
    struct ST_EVENT_TIME *next;
}ST_EVENT_TIME;

typedef struct ST_EVENT_FIRED
{
    int fd;
    int mask;
}ST_EVENT_FIRED;

typedef struct ST_EVENT_LOOP
{
    int maxfd;   //max file descriptor registered
    int setsize;
    int stop;
    long long timeEventNextId;
    time_t lastTime;
    ST_EVENT_FILE *fileEvents;
    ST_EVENT_FIRED *firedEvents;
    ST_EVENT_TIME *timeEventHead;
    void* poolApiData;
    eventBeforeSleepProc *beforeSleepProc;
    eventBeforeSleepProc *afterSleepProc;
}ST_EVENT_LOOP;


int aeMain();
long long aeCreateTimeEvent(ST_EVENT_LOOP *eventLoop, long long milliseconds,
        eventTimeProc *proc, void *clientData,
        eventFinalizerProc *finalizerProc);


#endif
