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

#define MS 

int cloud_select(int nfds,fd_set* readfds,int timeout)
{
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000)*1000;
    return select(nfds,readfds,NULL,NULL,&tv);
}

ST_MG_UDP_PACKAGE* package_decode(const unsigned char* buf,int bufsize)
{
    if(NULL == buf)
    {
        LOG_ERROR("param is invalid");
        return NULL;
    }
    ST_MG_UDP_PACKAGE* package;

    package = (ST_MG_UDP_PACKAGE*) MALLOC (sizeof(ST_MG_UDP_PACKAGE));
    if(NULL == package)
    {
        LOG_ERROR("malloc failed");
        return NULL;
    }
    memset(package,0,sizeof(ST_MG_UDP_PACKAGE));

    if(strstr(buf,"110"))
    {
        package->mid = 110;

    }else if(strstr(buf,"111"))
    {
        package->mid = 111;
    }else
    {
        package->mid = 0;
    }

    package->payload = strdup(buf);
    package->payload_len = strlen(buf);

    return package;
}

int main()
{
    ST_CLOUD_CONTEXT* context = NULL;
    ST_MG_UDP_HANDLE* udp_handle = NULL;
    ST_MG_UDP_PACKAGE* package = NULL;
    ST_MG_UDP_PACKAGE* package1 = NULL;
    ST_MG_TRANS_LIST* trans;
    unsigned char *msg = "110 hello from client";
    unsigned char *msg1 = "111 hello from client";

    context = (ST_CLOUD_CONTEXT*) MALLOC (sizeof(ST_CLOUD_CONTEXT));
    if(NULL == context)
    {
        LOG_ERROR("malloc failed");
        return -1;
    }
    memset(context,0,sizeof(ST_CLOUD_CONTEXT));

    context->maxResend = 5;
    context->responseTimeout = 3;
    
    udp_handle = (ST_MG_UDP_HANDLE*) MALLOC (sizeof(ST_MG_UDP_HANDLE));
    if(NULL == udp_handle)
    {
        LOG_ERROR("malloc failed");
        return -1;
    }

    udp_handle->port = "8000";
    udp_handle->host = "localhost";
    udp_handle->addr_family = AF_INET;
    udp_handle->conn = create_connection(udp_handle->host,udp_handle->port,udp_handle->addr_family);
    if(NULL == udp_handle->conn)
    {
        LOG_ERROR("create connection failed");
        return -1;
    }

    context->m2mHandle = udp_handle;

    package = (ST_MG_UDP_PACKAGE*) MALLOC (sizeof(ST_MG_UDP_PACKAGE));
    if(NULL == package)
    {
        LOG_ERROR("malloc failed");
        return -1;
    }

    package->mid = 110;
    package->payload = msg;
    package->payload_len = strlen(msg);

    trans = transaction_new(110,package);

    context->transList = LIST_ADD(context->transList,trans);
    transaction_send(context,trans);

    sleep(2);

    package1 = (ST_MG_UDP_PACKAGE*) MALLOC (sizeof(ST_MG_UDP_PACKAGE));
    if(NULL == package1)
    {
        LOG_ERROR("malloc failed");
        return -1;
    }

    package->mid = 111;
    package1->payload = msg1;
    package1->payload_len = strlen(msg1);

    trans = transaction_new(111,package1);

    context->transList = LIST_ADD(context->transList,trans);
    transaction_send(context,trans);

    fd_set readfds;
    int ret;

    char recvbuf[128];
    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(udp_handle->conn->sock,&readfds);
        ret = cloud_select(udp_handle->conn->sock + 1,&readfds,500 MS);
        if(ret < 0)
        {
            LOG_ERROR("errno:%d",Error());
        }else if(ret > 0)
        {
            memset(recvbuf,0,sizeof(recvbuf));
            ret = connection_recv(udp_handle->conn,recvbuf,sizeof(recvbuf));
            if(ret > 0)
            {
                ST_MG_UDP_PACKAGE* response = package_decode(recvbuf,sizeof(recvbuf));
                transaction_response_handler(context,response); 
            }
        }else
        {
            transaction_process(context);
            LOG_ERROR("no data received");
        }
        
    }

    return 0;
}


