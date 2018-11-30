#ifndef __COMMONS_TIME_H__
#define __COMMONS_TIME_H__
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
 
#include <limits.h>
#include <time.h>
/****************************macro**********************************************/
 
/****************************function*******************************************/
extern u32 utils_print_time(struct tm* tm_time);
extern u32 utils_strptime(s8* buf,s8* fmt,struct tm* tm_time);
extern time_t utils_strptime_compare(s8* src1,s8* src2,s8* fmt);
extern int utils_time_convert();
extern char* utils_time_get_timestamp(void);
#endif

