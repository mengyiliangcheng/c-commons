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
#ifndef __UTILS_THREAD_H__
#define __UTILS_THREAD_H__

#include "pthread.h"

typedef enum
{
    E_THREAD_SUCC = 0,                    /* 成功 */
    E_THREAD_ERR = -1,                    /* 失败 */
    E_THREAD_PARAM_ERR = -2,              /* 参数错误 */
    E_THREAD_AGAIN = -3,                  /* 稍后重试 */
}E_THREADPOOL_ERRCODE;


extern s32 utils_thread_create_process(s8* file,char* cmdline);
extern int utils_thread_create_thread(pthread_t *thread,void* (*start_routine)(void *),void* args,int stacksize);
extern s32 utils_thread_create_process_block(char* file,char* cmdline);
extern void utils_thread_sigchld_handler(int signum);



#endif 
 

