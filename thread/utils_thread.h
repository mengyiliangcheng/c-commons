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

extern u32 utils_thread_create_process(s8* file);
extern pthread_t utils_thread_create_thread(void* (*func)(void *),void* args);

#endif 
 

