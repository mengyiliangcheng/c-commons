/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/06  automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF 516 
#define FOTA_MSG_KEY 0Xabcd1234
#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"fota",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

#define FG_MSG_TYPE_I     101 
#define FG_MSG_TYPE_II    102
#define FG_MSG_TYPE_III   103


typedef struct fl_msg
{
    long msgType;
    long infoType;
    int param1;
    int param2;
    int param3;
    int param4;
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
} FS_DL_Msg;

int main()
{
    int ret;
    int num;
    int total = 0;
    while(1)
    {
        num = 0;
        printf("\npls input malloc size,now alloc:%d\n>",total);
        ret = scanf("%d",&num);
        if(ret < 0)
        {
            LOG("input error");
            break;
        }

        printf("\ninput:%d\n",num);
        char* p = calloc(1,num);
        if(NULL == p)
        {
            LOG("malloc failed,err:%s",strerror(errno));
            continue;
        }
        memset(p,0xaa,num);
        total += num;

    }
    return 0;
}



