/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/29  automatic
*/

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>

/* structure */
typedef struct _stCloudContext
{
    void *        m2mHandle;
    int maxResend;
    int responseTimeout;
    ST_MG_TRANS_LIST* transList;
}ST_CLOUD_CONTEXT;

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,time(NULL),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

#define LOG_INFO LOG
#define LOG_ERROR LOG

#define MALLOC malloc
#define FREE   free

#endif
