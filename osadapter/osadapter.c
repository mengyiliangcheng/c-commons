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
#include <string.h>
#include <stdarg.h>
#include "osadapter.h"

s32 commons_sprintf(s8* buffer,const s8* format,...)
{

	return 0;
}

s32 commons_println(const s8* format, ...)
{
    va_list ap;
    va_start(ap,format);
    vprintf(format,ap);
    va_end(ap);
    printf("\n");
}

s32 commons_print(const s8* format, ...)
{
    va_list ap;
    va_start(ap,format);
    vprintf(format,ap);
    va_end(ap);
}

s32 commons_print_hex(const void* src,int src_len)
{
    s32 i;
    u8* ptr = NULL;
    if(NULL == src || src_len <= 0){
        return -1;
    }
    
    ptr = (u8*)src;
    for(i = 0;i < src_len;i ++)
    {
        commons_print("%02X ",ptr[i]);
    }
    return 0;
}

void* commons_malloc(s32 size)
{
	return malloc(size);
}

void commons_free(void* ptr)
{
    if(NULL != ptr)
    {
        free(ptr);
    }
}

void commons_memset(void* dest,s32 ch,s32 count)
{
    if(NULL == dest)
    {
        return ;
    }
    memset(dest,ch,count);
}





