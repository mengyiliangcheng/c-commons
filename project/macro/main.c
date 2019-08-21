/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/07/26  automatic
*/
#include <stdio.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

typedef enum qapi_Net_LWM2M_DL_Msg_e
{
QAPI_NET_LWM2M_READ_REQ_E                  = 1,   /**< Read request. */
QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E         = 2,   /**< Write replace request. */
QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E  = 3,   /**< Write partial update request. */
QAPI_NET_LWM2M_WRITE_ATTR_REQ_E            = 4,   /**< Write attribute request. */
QAPI_NET_LWM2M_DISCOVER_REQ_E              = 5,   /**< Discover request. */
QAPI_NET_LWM2M_EXECUTE_REQ_E               = 6,   /**< Execute request. */
QAPI_NET_LWM2M_DELETE_REQ_E                = 7,   /**< Delete request. */
QAPI_NET_LWM2M_OBSERVE_REQ_E               = 8,   /**< Observe request. */
QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E        = 9,   /**< Cancel observe request. */
QAPI_NET_LWM2M_ACK_MSG_E                   = 10,  /**< Acknowledge message. */
QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E       = 11,  /**< Internal client indication. */
QAPI_NET_LWM2M_CREATE_REQ_E                = 12,  /**< Create request. */
QAPI_NET_LWM2M_DELETE_ALL_REQ_E            = 13,  /**< Delete all request. */
} qapi_Net_LWM2M_DL_Msg_t;


#define STR_DATA(S,C) (S) == (C) ? (#C) :

#define STR_MSG(S) \
( \
(STR_DATA(S,QAPI_NET_LWM2M_READ_REQ_E) \
(STR_DATA(S,QAPI_NET_LWM2M_OBSERVE_REQ_E) \
"Unknown" \
)))
#if 0
#define STR_MSGTYPE(S)                                \
((S) == QAPI_NET_LWM2M_READ_REQ_E ? "QAPI_NET_LWM2M_READ_REQ_E" :      \
((S) == QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E ? "QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E" :      \
((S) == QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E ? "QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E" :  \
((S) == QAPI_NET_LWM2M_WRITE_ATTR_REQ_E ? "QAPI_NET_LWM2M_WRITE_ATTR_REQ_E" :        \
((S) == QAPI_NET_LWM2M_DISCOVER_REQ_E ? "QAPI_NET_LWM2M_DISCOVER_REQ_E" :      \
((S) == QAPI_NET_LWM2M_EXECUTE_REQ_E ? "QAPI_NET_LWM2M_EXECUTE_REQ_E" :      \
((S) == QAPI_NET_LWM2M_DELETE_REQ_E ? "QAPI_NET_LWM2M_DELETE_REQ_E" :      \
((S) == QAPI_NET_LWM2M_OBSERVE_REQ_E ? "QAPI_NET_LWM2M_OBSERVE_REQ_E" :      \
((S) == QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E ? "QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E" :  \
((S) == QAPI_NET_LWM2M_ACK_MSG_E ? "QAPI_NET_LWM2M_ACK_MSG_E" :        \
((S) == QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E ? "QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E" :      \
((S) == QAPI_NET_LWM2M_CREATE_REQ_E ? "QAPI_NET_LWM2M_CREATE_REQ_E" :      \
((S) == QAPI_NET_LWM2M_CREATE_REQ_E ? "QAPI_NET_LWM2M_CREATE_REQ_E" :      \
((S) == QAPI_NET_LWM2M_DELETE_ALL_REQ_E ? "QAPI_NET_LWM2M_DELETE_ALL_REQ_E" :      \
"Unknown"))))))))))))))
#endif

#define STR_MSGTYPE(S)                                \
(STR_DATA(S,QAPI_NET_LWM2M_READ_REQ_E) \
(STR_DATA(S,QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_WRITE_ATTR_REQ_E) \
(STR_DATA(S,QAPI_NET_LWM2M_DISCOVER_REQ_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_EXECUTE_REQ_E) \
(STR_DATA(S,QAPI_NET_LWM2M_DELETE_REQ_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_OBSERVE_REQ_E) \
(STR_DATA(S,QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_ACK_MSG_E) \
(STR_DATA(S,QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_CREATE_REQ_E)    \
(STR_DATA(S,QAPI_NET_LWM2M_DELETE_ALL_REQ_E)    \
"Unknown")))))))))))))

#define FUNC_ENTRY()    printf("enter func [%s]\n",__func__);
#define FUNC_EXIT()     printf("exit func [%s]\n",__func__)
#define UNUSED(param)   (void)(param)

int main()
{
    int aa;
    FUNC_ENTRY();
    FUNC_EXIT();
    UNUSED(aa);
    printf("%s \n",STR_MSGTYPE(QAPI_NET_LWM2M_CREATE_REQ_E));
    printf("%s\n",STR_MSG(QAPI_NET_LWM2M_OBSERVE_REQ_E));
    printf("hello world");
    return 0;
}


