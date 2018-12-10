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

#ifndef __UTILS_SEM_H__
#define __UTILS_SEM_H__

extern s32 utils_thread_get_sem(void);
extern s32 utils_thread_p(int semid,int index);
extern s32 utils_thread_v(int semid,int index);


#endif

