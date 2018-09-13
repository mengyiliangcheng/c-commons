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







