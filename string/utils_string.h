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

#define UTILS_STRINGS_API_2

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

#define s_malloc commons_malloc
#define s_memcpy memcpy
#define s_free free
#define s_realloc realloc
#define s_memmove memmove

typedef char* STRING;

typedef struct 
{
    s32 len;  /* length of used*/
    s32 alloc; /* excluding the header and null terminator*/
    s8 buf[];
}__attribute__((__packed__)) ST_STR_HDR;

#define STR_HDR_VAR(s) (ST_STR_HDR*) sh = (ST_STR_HDR*)((s) - sizeof(ST_STR_HDR))
#define STR_HDR(s) ((ST_STR_HDR*)((s) - sizeof(ST_STR_HDR)))
#define STR_HDR_SIZE (sizeof(ST_STR_HDR))
#define STR_LEN(s) ((STR_HDR(s))->len)
#define STR_AVAIL(s) (STR_HDR(s)->alloc - STR_HDR(s)->len)
#define STR_MAX_PREALLOC (1024)
#define STR_ALLOC_SIZE(s) (STR_HDR_SIZE + STR_HDR(s)->alloc + 1)


/****************************function**********************************************/

s32 strings_to_hex(const u8* src,s32 src_len,u8* dest);




#endif

