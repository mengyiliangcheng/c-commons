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
#include <fcntl.h>
#include "osadapter.h"

s32 commons_sprintf(s8* buffer,const s8* format,...)
{
    if(NULL == buffer){
        return -1;
    }
    va_list ap;
    va_start(ap,format);
    vsprintf(buffer,format,ap);
    va_end(ap);
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

s32 commons_print_hex(const void* src,s32 src_len)
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


void commons_flush(FILE *fp)
{
    int ch;
    if(NULL == fp){
        return ;
    }
	while( (ch = fgetc(fp)) != EOF && ch != '\n');
}


s32 commons_scanf(s8* dest,s32 dest_len,s32 type)
{
    s8* tmp = NULL;
    s8* dest_ptr = NULL;
    s8 cmd[128] = {0};
    s32 i;
    s32 len;
    s8 ch;
    if(NULL == dest || dest_len <= 0){
        return -1;
    }
    
    tmp = (s8* )commons_malloc(sizeof(s8) * dest_len);
    if(NULL == tmp){
        return -1;
    }
    BZERO(tmp,sizeof(s8) * dest_len);

    sprintf(cmd,"\%%%ds",dest_len);
    scanf(cmd,tmp);    
    commons_flush(stdin);
    if(strlen(tmp) > sizeof(tmp) * dest_len){
        COMMONS_OS_LOG("overstack !");
        return -1;
    }
    
    i = 0;
    dest_ptr = dest;
    len = 0;
    while(i < strlen(tmp)){
        ch = *(tmp + i);
        if('\n' == ch){
            break;
        }
        if(8 == ch){
            i ++;
            if(dest_ptr == dest){
                continue;
            }
            dest_ptr --;
            *dest_ptr = 0;
            continue;
        }
        if((SCANF_NUMBER == type) && (ch < '0' || ch > '9')){
            COMMONS_OS_LOG("%d not number !",ch);
            i++;
            continue;
        }
        *dest_ptr = ch;
        dest_ptr ++;
        i ++;
        len ++;
    }
    if(SCANF_NUMBER == type && 0 == strlen(dest)){
        COMMONS_OS_LOG("no number !");
    }
    return len;
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

s32 commons_rand()
{
    s32 randnum = 0;
    s32 fd = open("/dev/urandom", O_RDONLY);
    if(-1 == fd)
    {
        COMMONS_OS_LOG("open /dev/urandon failed");
        return rand();
    }

    read(fd, (char *)&randnum, sizeof(int));
    close(fd);

    return randnum;
 
}

s32 commons_rand_str(s8* pheOut,s32 iLen)
{
    int i;
    unsigned char ch;
    if(iLen <= 0 || NULL == pheOut){
        COMMONS_OS_LOG("param error");
        return -4;
    }

    for(i = 0;i < iLen;i ++){
        ch = commons_rand() % 16;
        sprintf(&pheOut[i*2],"%02X",ch);
    }
    
    return 0;
}





