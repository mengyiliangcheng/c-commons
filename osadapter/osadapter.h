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

/****************************macro*******************************************/
typedef int s32;

typedef unsigned int u32;

typedef char s8;

typedef unsigned char u8;

typedef int BOOL;

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#undef NULL
#define NULL (void*)0

#undef MIN
#define MIN(a,b) (a < b ? a : b)

#define BZERO(dest,count)   (commons_memset(dest,0,count))

/****************************function*******************************************/

extern void* commons_malloc(s32 size);

extern void commons_free(void* ptr);

extern void commons_memset(void* dest,s32 ch,s32 count);

extern s32 commons_println(const char* format, ...);

extern s32 commons_print_hex(const void* src,int src_len);

#endif

