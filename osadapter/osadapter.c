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
#include <unistd.h>
#include "osadapter.h"

#define LOG(...) COMMONS_LOG("osadapter",__VA_ARGS__);
#define COMMONS_OS_LOG(...) COMMONS_LOG("osadapter",__VA_ARGS__);


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

#include <pthread.h>
#include <errno.h>

pthread_mutex_t commons_log_lock = PTHREAD_MUTEX_INITIALIZER;

static char* get_timestamp(void)
{
    time_t time_tmp;
    struct tm* timep;
    static char time_stream[64] = {0};

    time(&time_tmp);
    timep = gmtime(&time_tmp);
    strftime(time_stream,100,"%h %e %T ",timep);
    return time_stream;
}


s32 commons_logger(const s8* format, ...)
{
    va_list ap;
    if(NULL == format)
    {
        return -1;
    }

    pthread_mutex_lock(&commons_log_lock);

    static FILE *fp = NULL;
    va_start(ap, format);
    if(NULL == fp)
    {
        fp = fopen("./log/commons.log", "a+");
        if(NULL == fp)
        {
            printf("error:%s",strerror(errno));
            pthread_mutex_unlock(&commons_log_lock);
            return -1;
        }
    }
    char* timestamp = get_timestamp();
    if(NULL != timestamp)
    {
        fwrite(timestamp,strlen(timestamp),1,fp);
    }
    vfprintf(fp, format, ap);
    va_end(ap);
    fwrite("\r\n",strlen("\r\n"),1,fp);

    fflush(fp);
    //fclose(fp);
    //fp = NULL;

    pthread_mutex_unlock(&commons_log_lock);
}


s32 commons_println(const s8* format, ...)
{
    if(NULL == format)
    {
        return -1;
    }
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
    commons_println("");
    return 0;
}

s32 commons_print_hex_no_space(const void* src,s32 src_len)
{
    s32 i;
    u8* ptr = NULL;
    if(NULL == src || src_len <= 0){
        return -1;
    }
    
    ptr = (u8*)src;
    for(i = 0;i < src_len;i ++)
    {
        commons_print("%02X",ptr[i]);
    }
    commons_println("");
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


s32 commons_scanf(s8* dest,const s32 dest_len,const s32 type)
{
    s8* tmp = NULL;
    s8* dest_ptr = NULL;
    s8 cmd[128] = {0};
    s32 i;
    s32 len;
    s8 ch;
    s32 input_type = type;
    if(NULL == dest || dest_len <= 0)
    {
        return -1;
    }
    
    tmp = (s8* )commons_malloc(sizeof(s8) * dest_len);
    if(NULL == tmp)
    {
        return -1;
    }
    BZERO(tmp,sizeof(s8) * dest_len);

    //commons_println("type before:%d",input_type);
    //commons_print_hex(&type,10);
    //commons_print_hex(&input_type,10);

    //commons_print_hex(&tmp,dest_len);
    //commons_println("pls input:");
    sprintf(cmd,"\%%%ds",dest_len-1);
    scanf(cmd,tmp);
    //commons_print_hex(&tmp,dest_len);
    //commons_flush(stdin);
    if(strlen(tmp) > sizeof(tmp) * dest_len)
    {
        commons_println("overstack !");
        free(tmp);
        return -1;
    }
    //commons_println("type111:%d",input_type);
    //commons_print_hex(&type,10);
    //commons_print_hex(&input_type,10);

    
    i = 0;
    dest_ptr = dest;
    len = 0;
    while(i < strlen(tmp))
    {
        ch = *(tmp + i);
        if('\n' == ch)
        {
            break;
        }
        if(8 == ch)
        {
            i ++;
            if(dest_ptr == dest)
            {
                continue;
            }
            dest_ptr --;
            *dest_ptr = 0;
            continue;
        }
        if(SCANF_NUMBER == type)
        {
            if((ch < '0' || ch > '9') && '-' != ch)
            {
                commons_println("%c not number !",ch);
                i++;
                continue;
            }
        }
        *dest_ptr = ch;
        dest_ptr ++;
        i ++;
        len ++;
    }
    if(SCANF_NUMBER == type && 0 == strlen(dest)){
        commons_println("no number !");
    }
    free(tmp);
    return len;
}


void* commons_malloc(s32 size)
{
    void* ptr = NULL;
    if(size <= 0)
    {
        Assert(size > 0);
        return NULL;
    }
    
	ptr = malloc(size);
    if(!ptr)
    {
        return NULL;
    }

    commons_memset(ptr,0,size);
    
    return ptr;
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

void commons_bzero(void* dest,s32 count)
{
    commons_memset(dest,0,count);
}

s32 commons_rand()
{
    s32 randnum = 0;
    s32 fd = open("/dev/urandom", O_RDONLY);
    if(-1 == fd)
    {
        commons_println("open /dev/urandon failed");
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
        commons_println("param error");
        return -4;
    }

    for(i = 0;i < iLen;i ++){
        ch = commons_rand() % 16;
        sprintf(&pheOut[i*2],"%02X",ch);
    }
    
    return 0;
}





