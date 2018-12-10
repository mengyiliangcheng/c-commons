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

/*��ȡsrc�ַ�����,���±�Ϊstart��ʼ��end-1(endǰ��)���ַ���������dest��(�±��0��ʼ)*/  
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

/*����src���±�Ϊindex���ַ�*/  
char strings_char_at(char *src,int index)
{
    char *p=src;
    int i=0;
    if(index<0||index>strlen(src))
        return 0;
    while(i<index)i++;
    return p[i];
}

/*����str2��һ�γ�����str1�е�λ��(�±�����),�����ڷ���-1*/  
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
/*����str1�����һ�γ���str2��λ��(�±�),�����ڷ���-1*/  
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
            break;/*32:�ո�,9:�����Ʊ��*/  
        i++;
    }
    if(i!=0)
    for(j=0;j<=len-i;j++)
    {
        str[j]=str[j+i];/*��������ַ�˳��ǰ��,����ɾ���Ŀհ�λ��*/  
    }
}

/*ɾ��str���һ���ǿհ��ַ���������пհ��ַ�(�ո���ͺ����Ʊ��)*/  
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

/*ɾ��str���˵Ŀհ��ַ�*/  
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

#ifdef UTILS_STRINGS_API_2



static inline size_t str_hdr_size(void)
{
    return sizeof(ST_STR_HDR);
}


STRING str_new_len(const void* init,size_t initlen)
{
    ST_STR_HDR* sh;
    STRING s;

    sh = s_malloc(STR_HDR_SIZE + initlen + 1);
    if(!sh) return NULL;

    if(!init)
        BZERO(sh,STR_HDR_SIZE + initlen + 1);

    s = (char*)sh + STR_HDR_SIZE;

    sh->len = initlen;
    sh->alloc = initlen;

    if(init && initlen)
        s_memcpy(s,init,initlen);

    s[initlen] = '\0';

    return s;
}

STRING str_empty(void)
{
    return str_new_len("",0);
}

STRING str_new(const char* init)
{
    size_t initlen = (NULL == init) ? 0 : strlen(init);
    return str_new_len(init,initlen);
}

STRING str_dup(STRING s)
{
    return str_new_len(s,STR_LEN(s));
}

void str_free(STRING s)
{
    if(!s)
        return;
    s_free(STR_HDR(s));
}

void str_setlen(STRING s,size_t newlen)
{
    STR_HDR(s)->len = newlen;
    return;
}

void str_updatelen(STRING s)
{
    int reallen = strlen(s);
    str_setlen(s,reallen);
}

void str_clear(STRING s)
{
    str_setlen(s,0);
    s[0] = '\0';
}

STRING str_make_room_for(STRING s,size_t addlen)
{
    ST_STR_HDR* newsh;
    size_t newlen;
    if(STR_AVAIL(s) >= addlen)
        return s;

    /* alloc a big enough room for string*/
    newlen = STR_LEN(s) + addlen;
    if(newlen < STR_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += STR_MAX_PREALLOC;
    
    newsh = s_realloc(STR_HDR(s),STR_HDR_SIZE + newlen + 1);
    if(!newsh) return NULL;

    /*refresh data pointer*/
    s = newsh->buf;

    /*refresh alloc*/
    newsh->alloc = newlen;

    return s;
}

void str_inclen(STRING s,size_t incr)
{
    assert(incr > 0 && (STR_HDR(s)->alloc -STR_HDR(s)->len > incr));
    s[STR_HDR(s)->len + incr] = '\0';
}

STRING str_catlen(STRING s,const void* t,size_t len)
{
    size_t currentlen = STR_LEN(s);

    s= str_make_room_for(s,len);
    if(!s) return NULL;

    s_memcpy(s+currentlen,t,len);

    str_setlen(s,currentlen + len);

    s[currentlen + len] = '\0';

    return s;
}

STRING str_cat(STRING s,const void * t)
{
    str_catlen(s,t,strlen(t));
}



#endif




