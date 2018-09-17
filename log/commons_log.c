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
#include "commons_log.h"

#define COMMONS_LOG_FILE  "./log/commons.log" 


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


s32 commons_log(const u8* module,const u8* format, ...)
{
    va_list ap;
    s32 ret;    
    //static FILE *fp = NULL;

    printf("[%s] ",module);
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

    return ret;
}




