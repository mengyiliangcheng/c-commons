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
#ifndef __OSADAPTER_H__
#define __OSADAPTER_H__

/****************************macro*******************************************/
#undef s32
#define s32 int

#undef u32 
#define u32 unsigned int

#undef s8
#define s8 char

#undef u8
#define u8 unsigned char

#undef BOOL
#define BOOL int

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

void* commons_malloc(s32 size);

void* commons_free(void* ptr);

void commons_memset(void* dest,s32 ch,s32 count);


#endif

