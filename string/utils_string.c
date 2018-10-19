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
#include <limits.h>
#include <assert.h>
#include "osadapter.h"
#include "utils_string.h"

//static u8 hex_table[16] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

/*
*description:convert string data to hex data: "1a2b3c" ==> \x1a \x2b \x3c
*
**/
s32 strings_to_hex(const u8* src,s32 src_len,u8* dest)
{
    u8 ch;
    u8* ptr = NULL;
    u8* dest_ptr = NULL;
    if(NULL == src || NULL == dest){
        return -4;
    }
    //commons_println("src:%s",src);
    ptr = src;
    dest_ptr = dest;
    while(src_len > 1){
        ch = 0;
        if(*ptr >= '0' && *ptr <= '9'){
            ch = ((*ptr) - '0') << 4;
        }else if(*ptr >= 'a' && *ptr <= 'f')
        {
            ch = ((*ptr) - 'a' + 10) << 4;
        }else if(*ptr >= 'A' && *ptr <= 'F')
        {
            ch = ((*ptr) - 'A' + 10) << 4;
        }else{
            return -2;
        }
        //commons_println("ptr:%c",*ptr);
        //commons_println("ch:%02x",ch);
        ptr ++;
        

        if(*ptr >= '0' && *ptr <= '9'){
            ch += *ptr - '0';
        }else if (*ptr >= 'a' && *ptr <= 'f')
        {
            ch += *ptr - 'a' + 10;
        }else if (*ptr >= 'A' && *ptr <= 'F')
        {
            ch += *ptr - 'A' + 10;
        }else{
            return -2;
        }
        ptr ++;
        
        //commons_println("ptr:%c",*ptr);
        //commons_println("ch:%02x",ch);
        *dest_ptr = ch;
        dest_ptr ++;
        
        src_len -= 2;
    }

    if(1 == src_len)
    {
        if(*ptr >= '0' && *ptr <= '9'){
            ch = ((*ptr) - '0') << 4;
        }else if(*ptr >= 'a' && *ptr <= 'f')
        {
            ch = ((*ptr) - 'a' + 10) << 4;
        }else if(*ptr >= 'A' && *ptr <= 'F')
        {
            ch = ((*ptr) - 'A' + 10) << 4;
        }else{
            return -2;
        }
        *dest_ptr = ch;
    }
    
    return 0;
}

s32 strings_hex_to_str(const u8* src,s32 src_len,u8* dest)
{
    
}

s32 strings_len(const s8* str)
{
    s32 i;
    if(NULL == str)
    {
        Assert(str);
        return -1;
    }

    i = 0;
    while(*str)
    {
        i++;
        str++;
        if(i >= (INT_MAX-1)) break;
    }
    return i;
}

void strings_copy(s8* dest,const s8* src,s32 dest_size)
{
    if(NULL == dest || NULL == src || dest_size <= 1)
    {
        Assert(dest);
        Assert(src);
        Assert(dest_size > 1);
        return -1;
    }

    while(*src && dest_size > 1){
        *dest++ = *src++;
        dest_size --;
    }
    *dest = 0;
}

void strings_concat(s8* dest,const s8* src)
{
    if(NULL == dest || NULL == src)
    {
        Assert(dest);
        Assert(src);
        return;
    }

    while(*dest) dest++;

    while(*src)
    {
        *dest++ = *src++;
    }
    *dest = 0;
}

void strings_concat_s(s8* dest,const s8* src,s32 dest_size)
{
    if(NULL == dest || NULL == src || dest_size <= 1)
    {
        Assert(dest);
        Assert(src);
        Assert(dest_size > 1);
        return;
    }

    while(*dest) 
    {
        dest++;
        dest_size --;
        if(dest_size <= 0) return ;
    }

    while(*src)
    {
        *dest++ = *src++;
        
        dest_size --;
        if(dest_size <= 0) return ;
    }
    *dest = 0;
}

s32 strings_compare(const s8* str1,const s8* str2)
{
    if(NULL == str1 || NULL == str2)
    {
        Assert(str1);
        Assert(str2);
    }

    while(*str1 == *str2 && *str1 && *str2)
    {
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

s32 strings_compare_s(const s8* str1,const s8* str2,s32 len)
{
    if(NULL == str1 || NULL == str2 || len <= 0)
    {
        Assert(str1);
        Assert(str2);
        Assert(len > 0);
    }

    while(*str1 == *str2 && *str1 && *str2)
    {
        len --;
        if(len <= 0) break;
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

void strings_move(s8* dest,s8* src,s32 len)
{
    if(NULL == dest || NULL == src || len < 0)
    {
        Assert(dest);
        Assert(src);
        Assert(len >= 0);
        return;
    }

    while(len > 0)
    {
        *dest++ = *src++;
        len --;
    }
}

s32 strings_insert(s8* str1,s32 pos,s8* str2)
{
    if(NULL == str1 || NULL == str2 || pos < 0)
    {
        Assert(str1);
        Assert(str2);
        Assert(pos >= 0);
        return -4;
    }

    while(pos > 0)
    {
        str1++;
        pos--;
    }
}


void strings_replace_first(s8* str1,s8* str2,s8* str3)  
{
    if(NULL == str1 || NULL == str2 || NULL == str3){
        return ;
    }
    s8 str4[strlen(str1)+1];  
    s8* p;
    
    strcpy(str4,str1);  
    if((p=strstr(str1,str2))!=NULL)
    {
        while(str1!=p&&str1!=NULL)
        {
            str1++;  
        }
        str1[0]='\0';
        strcat(str1,str3);
        strcat(str1,strstr(str4,str2)+strlen(str2));
    }
}  


void strings_replace(char *str1,char *str2,char *str3)  
{
    while(strstr(str1,str2)!=NULL)
    {
        strings_replace_first(str1,str2,str3);
    }
}

/*截取src字符串中,从下标为start开始到end-1(end前面)的字符串保存在dest中(下标从0开始)*/  
void strings_substringxx(char *dest,char *src,int start,int end)  
{
    
    
    s32 i = start;
    if(start > strings_len(src))
        return;
    if(end > strings_len(src))
        end=strings_len(src);
    while(i < end)
    {
        dest[i-start]=src[i];
        i++;
    }
    dest[i-start]='\0';
    return;
}

void strings_substring(s8* dest,const s8* src,s32 pos,s32 len)
{
    if(NULL == dest || NULL == src || pos < 0 || len <= 0)
    {
        Assert(dest);
        Assert(src);
        Assert(len > 0);
        Assert(pos > 0);
        return ;
    }

    while(*src && pos--) src++;
    if(!*src) return;

    while(*src && len--)
    {
        *dest++ = *src++;
    }
    *dest = 0;
}

/*返回src中下标为index的字符*/  
char strings_char_at(char *src,int index)
{
    char *p=src;
    int i=0;
    if(index<0||index>strlen(src))
        return 0;
    while(i<index)i++;
    return p[i];
}

/*返回str2第一次出现在str1中的位置(下表索引),不存在返回-1*/  
int strings_indexOf(char *str1,char *str2)
{
    char *p=str1;
    int i=0;
    p=strstr(str1,str2);
    if(p==NULL)
        return -1;
    else{
        while(str1!=p)
        {
            str1++;
            i++;
        }
    }
    return i;
}  
/*返回str1中最后一次出现str2的位置(下标),不存在返回-1*/  
int strings_lastIndexOf(char *str1,char *str2)  
{
    char *p=str1;
    int i=0,len=strlen(str2);
    p=strstr(str1,str2);
    if(p==NULL)return -1;
    while(p!=NULL)
    {
        for(;str1!=p;str1++)i++;
        p=p+len;
        p=strstr(p,str2);
    }
    return i;
}

void strings_ltrim(char *str)
{
    int i=0,j,len=strlen(str);
    while(str[i]!='\0')
    {
        if(str[i]!=32&&str[i]!=9)
            break;/*32:空格,9:横向制表符*/  
        i++;
    }
    if(i!=0)
    for(j=0;j<=len-i;j++)
    {
        str[j]=str[j+i];/*将后面的字符顺势前移,补充删掉的空白位置*/  
    }
}

/*删除str最后一个非空白字符后面的所有空白字符(空格符和横向制表符)*/  
void strings_rtrim(char *str)
{
    char *p=str;
    int i=strlen(str)-1;
    while(i>=0)
    {
        if(p[i]!=32&&p[i]!=9)
            break;
        i--;
    }
    str[++i]='\0';
}

/*删除str两端的空白字符*/  
void strings_trim(char *str)
{
    strings_ltrim(str);
    strings_rtrim(str);
}


ST_UTILS_STRINGS* strings_new()
{
    ST_UTILS_STRINGS* str = commons_malloc(sizeof(ST_UTILS_STRINGS));

    str->str = (s8*)commons_malloc(UTILS_STRINGS_MAX_LEN);
    if(!str->str)
    {
        commons_free(str);
        return NULL;
    }

    return str;
}

void strings_delete(ST_UTILS_STRINGS* str)
{
    if(!str)
    {
        return ;
    }

    if(NULL != str->str)
    {
        commons_free(str->str);
        str->str = NULL;
    }
    commons_free(str);
}






