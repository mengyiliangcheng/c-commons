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

#ifndef __UDP_TRANS_H__
#define __UDP_TRANS_H__

#include <time.h>

typedef struct _ST_MG_TRANS_LIST
{
    struct _ST_MG_TRANS_LIST* next;
    int message_id;
    int dirty;
    time_t resend_time;
    int resend_count;
    ST_MG_UDP_PACKAGE* package;
}ST_MG_TRANS_LIST;

extern int transaction_send(void* context,ST_MG_TRANS_LIST* transList);
extern ST_MG_TRANS_LIST* transaction_remove(ST_MG_TRANS_LIST* head,int sequenceNum);
extern ST_MG_TRANS_LIST* transaction_new(int sequenceNum,ST_MG_UDP_PACKAGE* package);
extern int transaction_free(ST_MG_TRANS_LIST* node);

#endif
