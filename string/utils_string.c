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
void strings_substring(char *dest,char *src,int start,int end)  
{
    int i=start;
    if(start>strlen(src))
        return;
    if(end>strlen(src))
        end=strlen(src);
    while(i<end)
    {
        dest[i-start]=src[i];
        i++;
    }
    dest[i-start]='\0';
    return;
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






