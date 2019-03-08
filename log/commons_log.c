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

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include "commons_log.h"

#define COMMONS_LOG_FILE  "./log/commons.log" 


char* utils_time_get_timestamp(void)
{
    time_t time_tmp;
    struct tm* timep;
    static char time_stream[64] = {0};

    time(&time_tmp);
    timep = gmtime(&time_tmp);
    strftime(time_stream,100,"%h %e %T ",timep);
    return time_stream;
}

u8* commons_get_shortname(u8* path)
{
    u8* prev;
    u8* split;

    if (NULL == path)
    {
        return "";
    }
    prev = path;
    for (split = path; *split; split++)
    {
        if ('/' == *split || '\\' == *split)
        {
            prev = split;
            ++prev;
        }
    }
    return prev;
}

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER ;

s32 commons_log(const u8* format, ...)
{
    va_list ap;
    s32 ret;

    pthread_mutex_lock(&lock);

    //static FILE *fp = NULL;

    //printf("[%s] ",module);
    va_start(ap, format);
    
    ret = vprintf(format, ap);
    //if(NULL == fp){
    //    fp = fopen(COMMONS_LOG_FILE, "a+");
    //}
    //ret = vfprintf(fp, format, ap);
    va_end(ap);
    printf("\n");
    //fclose(fp);
    //fp = NULL;
    pthread_mutex_unlock(&lock);

    return ret;
}



