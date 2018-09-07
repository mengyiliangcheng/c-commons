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
#include <pthread.h>
#include "osadapter.h"
#include "mempool.h"

static ST_MEMPOOL_HEAD _gstMempoolHead;
static pthread_mutex_t _gst_mutex_lock = PTHREAD_MUTEX_INITIALIZER;

s32 mempool_malloc(s32 size)
{
    return 0;
}

s32 mempool_free(void* ptr)
{
    return 0;
}

ST_MEMPOOL_HEAD* getMempoolHead()
{
    return &_gstMempoolHead;
}

s32 mempool_checksum(const u8* src,s32 src_len,u8* checksum,s32 type)
{
    //u8* ptr = NULL;

    if(NULL == src || NULL == checksum)
    {
        return -1;
    }

    return 0;
}

BOOL mempool_body_check(ST_MEMPOOL_BODY* body)
{
    return TRUE;
}

BOOL mempool_check()
{
    ST_MEMPOOL_BODY* body = NULL;
    ST_MEMPOOL_HEAD* head = getMempoolHead();
    s32 num;

    for(num = 0;num < COMMONS_MEMPOOL_MAX_POOL_NUM;num ++)
    {
        body = head->ptrBodyHead[num];
        if(mempool_body_check(body)){
            mempool_delete(body);
        }
        head->ptrBodyHead[num] = NULL;
    }
	return TRUE;
}

ST_MEMPOOL_BODY* mempool_init_body()
{
    ST_MEMPOOL_BODY* body = NULL;

    body = (ST_MEMPOOL_BODY*)commons_malloc(sizeof(ST_MEMPOOL_BODY));
    if(NULL == body)
    {
        return NULL;
    }
    BZERO(body,sizeof(ST_MEMPOOL_BODY));

    body->ptrBody = (u8*)commons_malloc(sizeof(u8)*COMMONS_MEMPOOL_MAX_POOL_SIZE);
    if(NULL == body->ptrBody)
    {
        commons_free(body);
        return NULL;
    }
    BZERO(body->ptrBody,sizeof(u8)*COMMONS_MEMPOOL_MAX_POOL_SIZE);

    return body;
}

s32 mempool_init()
{
    ST_MEMPOOL_BODY* body = NULL;
    ST_MEMPOOL_HEAD* head = getMempoolHead();
    s32 num;

    pthread_mutex_lock(&_gst_mutex_lock);

    /*  check the validity of body,if valid,free the memory of body  */
    for(num = 0;num < COMMONS_MEMPOOL_MAX_POOL_NUM;num ++)
    {
        body = head->ptrBodyHead[num];
        if(mempool_body_check(body)){
            mempool_delete(body);
        }
        head->ptrBodyHead[num] = NULL;
    }

    BZERO(head,sizeof(ST_MEMPOOL_HEAD));

    /*  create first body for head  */
    body= mempool_init_body();
    if(NULL == body)
    {
        return -1;
    }
    head->iBodySize[0] = 0;
    head->ptrBodyHead[0] = body;
    head->ptrBodyTail[0] = body->ptrBody;
    head->iTotalBodyNum = 1;

    pthread_mutex_unlock(&_gst_mutex_lock);

    return 0;
}

s32 mempool_delete()
{
    return 0;
}

s32 mempool_add()
{
    return 0;
}

