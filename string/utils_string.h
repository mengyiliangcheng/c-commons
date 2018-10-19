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

#ifndef __UTILS_STRING_H__
#define __UTILS_STRING_H__

#include "string.h"
#include "osadapter.h"

/****************************macro**********************************************/
#define UTILS_STRINGS_MAX_LEN 128

/****************************structure**********************************************/
typedef struct
{
    s8* str;
    int length;
}ST_UTILS_STRINGS;

/****************************function**********************************************/

s32 strings_to_hex(const u8* src,s32 src_len,u8* dest);




#endif

