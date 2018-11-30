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
#ifndef __COMMONS_LOG_H__
#define __COMMONS_LOG_H__

#include "commons_type.h"

extern u8* commons_get_shortname(u8* path);

#define COMMONS_LOG1(module,format, ... ) do{ \
        commons_log(module,"%s|%d::"format,commons_get_shortname(__FILE__),__LINE__,##__VA_ARGS__); \
       }while(0)

#define COMMONS_LOG(module,format, ... ) do{ \
        commons_log("%s %s:%s|%d::"format,utils_time_get_timestamp(),module,commons_get_shortname(__FILE__),__LINE__,##__VA_ARGS__); \
       }while(0) 


extern void analyze_log(s8* start_time,s8* end_time,BOOL b_check_time);
#endif

