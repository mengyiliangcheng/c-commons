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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "udp_connection.h"
#include "udp_trans.h"
#include "main.h"

int transaction_free(ST_MG_TRANS_LIST* node)
{
    if(NULL != node)
    {
        LOG_ERROR("invalid param");
        return -1;
    }

    FREE(node);
}


time_t cloud_gettime(void)
{
    struct timeval tv;

    if (0 != gettimeofday(&tv, NULL))
    {
        return -1;
    }

    return tv.tv_sec;
}

ST_MG_TRANS_LIST* transaction_new(int messageId,ST_MG_UDP_PACKAGE* package)
{
    ST_MG_TRANS_LIST* node;

    node = (ST_MG_TRANS_LIST*)MALLOC(sizeof(ST_MG_TRANS_LIST));
    if(NULL == node)
    {
        return NULL;
    }

    memset(node,0,sizeof(ST_MG_TRANS_LIST));

    node->message_id = messageId;
    node->dirty = false;
    node->package = package;

    return node;
}

ST_MG_TRANS_LIST* transaction_remove(ST_MG_TRANS_LIST* head,int sequenceNum)
{
    ST_MG_TRANS_LIST* node = NULL;
    if(NULL == head)
    {
        LOG_ERROR("invalid param");
        return NULL;
    }

    head = LIST_DEL(head,sequenceNum,&node);

    if(NULL != node)
    {
        transaction_free(node);
    }

    return head;
}

int transaction_send(void* context,ST_MG_TRANS_LIST* trans)
{
    ST_CONNECTION* conn = NULL;
    ST_MG_UDP_HANDLE* udp_handle = NULL;
    ST_CLOUD_CONTEXT* ctx = NULL;
    int ret;

    ctx = (ST_CLOUD_CONTEXT*)context;

    if(NULL == ctx || NULL == trans)
    {
        LOG_ERROR("param is invalid");
        return -1;
    }
    udp_handle = (ST_MG_UDP_HANDLE*)ctx->m2mHandle;
#if 0
    conn = create_connection(udp_handle->host,udp_handle->port,udp_handle->addr_family);
    if(NULL == conn)
    {
        LOG_ERROR("create connection failed");
        return -1;
    }
#endif
    if(0 == trans->resend_count)
    {
        trans->resend_count ++;
        trans->resend_time = cloud_gettime() +  ctx->responseTimeout;
    }

    ret = connection_send(udp_handle->conn,trans->package->payload,trans->package->payload_len);
    if(ret < 0)
    {
        LOG_ERROR("send failed");
        return -1;
    }

    return 0;
}

int transaction_response_handler(void* context,ST_MG_UDP_PACKAGE* response)
{
    ST_CLOUD_CONTEXT* ctx = NULL;
    ST_MG_TRANS_LIST* node;
    if(NULL == context || NULL == response)
    {
        LOG_ERROR("param is invalid");
        return -1;
    }

    ctx = (ST_CLOUD_CONTEXT*)context;

    node = LIST_FIND(ctx->transList,response->mid);
    if(NULL == node)
    {
        LOG_ERROR("can not find reponse mid:%d",response->mid);
    }else
    {
        LOG_INFO("received response package,mid:%d",response->mid);
        node->dirty = true;
    }

    return 0;
}


void transaction_process(void* context)
{
    time_t current_time = 0;
    ST_CLOUD_CONTEXT* ctx = NULL;
    ST_MG_TRANS_LIST* trans;
    if(NULL == context)
    {
        LOG_ERROR("param is invalid");
        return -1;
    }

    current_time = cloud_gettime();

    ctx = (ST_CLOUD_CONTEXT*)context;
    trans = ctx->transList;

    while(NULL != trans)
    {
        if(true == trans->dirty)
            goto NEXT;

        if(trans->resend_count > ctx->maxResend)
        {
            LOG_ERROR("trans resend exceed max %d",ctx->maxResend);
            trans->dirty = true;
            goto NEXT;
        }

        if(trans->resend_time < current_time)
        {
            transaction_send(ctx,trans);
            trans->resend_count ++;
            trans->resend_time = current_time + ctx->responseTimeout;
        }
NEXT:
        trans = trans->next;
    }


}

