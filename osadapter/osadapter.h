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
#ifndef __COMMONS_OSADAPTER_H__
#define __COMMONS_OSADAPTER_H__

/****************************headers*******************************************/
#include "commons_log.h"
#include <assert.h>

/****************************macro*******************************************/

#undef MIN
#define MIN(a,b) (a < b ? a : b)

#undef MAX
#define MAX(a, b) (a > b ? a : b)

#define BZERO(dest,count)   (commons_memset(dest,0,count))
#define Assert(x) assert(x)

#define UNUSED(V) ((void) V)


typedef enum
{
    SCANF_STRING,
    SCANF_NUMBER,
}E_SCANF_TYPE;


/****************************function*******************************************/

extern void* commons_malloc(s32 size);

extern void commons_free(void* ptr);

extern void commons_memset(void* dest,s32 ch,s32 count);

extern s32 commons_println(const char* format, ...);

extern s32 commons_print_hex(const void* src,int src_len);

extern int commons_rand();
extern s32 commons_scanf(s8* dest,s32 dest_len,s32 type);

extern s32 commons_logger(const s8* format, ...);


#endif

