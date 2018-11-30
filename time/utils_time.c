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

#include "commons_type.h"
#include "utils_time.h"
#include "osadapter.h"
#include <stdlib.h>

u32 utils_print_time(struct tm* tm_time)
{
    commons_println("tm_year:%d",tm_time->tm_year);
    commons_println("tm_mon :%d",tm_time->tm_mon);
    commons_println("tm_wday:%d",tm_time->tm_wday);
    commons_println("tm_hour:%d",tm_time->tm_hour);
    commons_println("tm_min :%d",tm_time->tm_min);
    commons_println("tm_sec :%d",tm_time->tm_sec);
    return 0;
}

u32 utils_strptime(s8* buf,s8* fmt,struct tm* tm_time)
{
    if(NULL == buf||NULL == fmt||NULL == tm_time)
        return -1;
    strptime(buf,fmt,tm_time);
    if(tm_time->tm_year < 100 || tm_time > 200)
    {
        tm_time->tm_year = 112;
        tm_time->tm_mon = 8;
    }
    return 0;
}

/* src1 - src2*/
time_t utils_strptime_compare(s8* src1,s8* src2,s8* fmt)
{
    struct tm tm_time1;
    struct tm tm_time2;
    time_t time_tmp1;
    time_t time_tmp2;
    utils_strptime(src1,fmt,&tm_time1);
    utils_strptime(src2,fmt,&tm_time2);

    time_tmp1 = mktime(&tm_time1);
    time_tmp2 = mktime(&tm_time2);

    return time_tmp1 - time_tmp2;
}

int utils_time_convert()
{
    time_t time_tmp;
    struct tm* timep;
    time_tmp = 1542433551006;
    char tmp[128] = {0};
    char time_stream[128] = {0};

    time(&time_tmp);
    commons_println("current time:%lld",time_tmp);

    timep = gmtime(&time_tmp);
    strftime(tmp,100,"time is now:%h %e %T ",timep);
    commons_println("%s",tmp);
    commons_println("%lld",time_tmp-315964800);

    commons_println("pls input your time:");
    //commons_flush(stdin);
    commons_scanf(time_stream,sizeof(time_stream),SCANF_NUMBER);
    time_tmp = strtoul(time_stream,NULL,0);
    commons_println("%lld",time_tmp);

    
    timep = gmtime(&time_tmp);
    strftime(tmp,100,"your time is:%Y-%m-%d %H:%M:%S",timep);
    commons_println("%s",tmp);
}



u32 utils_get_utc_time(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    return ts.tv_sec;
}

u32 utils_time_get_up_time(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec;
}

BOOL utils_time_is_expired(u32 src_time)
{
    u32 cur_time;

    cur_time = utils_get_utc_time();
    /*
     *  WARNING: Do NOT change the following code until you know exactly what it do!
     *
     *  check whether it reach destination time or not.
     */
    if ((cur_time - src_time) < (UINT_MAX / 2)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL utils_time_is_end(u32 start_time,u32 delays_time)
{
    u32 cur_time;

    cur_time = utils_get_utc_time();
    
    if((cur_time - start_time - delays_time) < (UINT_MAX / 2))
    {
        return TRUE;
    }else{
        return FALSE;
    }
}

u32 utils_time_add(u32 seconds)
{
    return utils_get_utc_time() + seconds;
}

u32 utils_time_left(u32 end_time)
{
    unsigned int now, res;

    if (utils_time_is_expired(end_time)) {
        return 0;
    }

    now = utils_get_utc_time();
    res = end_time - now;
    return res;
}



