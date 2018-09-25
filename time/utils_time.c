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



