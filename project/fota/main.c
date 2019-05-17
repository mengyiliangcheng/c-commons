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
    int ret = 0;
    int id = 0;
    int type = 0;
    int param = 0;
    FS_DL_Msg st_fota;

    id = msgget(FOTA_MSG_KEY,IPC_EXCL |IPC_CREAT | 0666);
    if(id < 0)
    {
        LOG("msgget failed,err:%d",errno);        
        id = msgget(FOTA_MSG_KEY,IPC_EXCL|0666);
        if(id < 0)
        {
            LOG("msgget failed,err:%d",errno);
            return -1;
        }
    }

    while(1)
    {
        printf("pls input type:");
        ret = scanf("%d",&type);
        if(ret < 0)
        {
            LOG("Scanf Failed,err:%d\n",errno);
            continue;
        }
        else if (ret == 0)
        {
            LOG("No Matched Characters\n");
            getchar();
            continue;
        }
        printf("pls input param:");
        ret = scanf("%d",&param);
        if(ret < 0)
        {
            LOG("Scanf Failed,err:%d\n",errno);
            continue;
        }
        else if (ret == 0)
        {
            LOG("No Matched Characters\n");
            getchar();
            continue;
        }
        memset(&st_fota,0,sizeof(st_fota));
        st_fota.msgType = FG_MSG_TYPE_I;
        st_fota.infoType = type;
        st_fota.param1 = param;
        LOG("id:%d data size:%d",id,sizeof(st_fota)-sizeof(long));
        ret = msgsnd(id,(void*)&st_fota,sizeof(st_fota)-sizeof(long),00004000);
        if(ret < 0)
        {
            LOG("msgsnd failed,err:%d",errno);
        }

        memset(&st_fota,0,sizeof(st_fota));
        ret = msgrcv(id,(void*)&st_fota,sizeof(st_fota),FG_MSG_TYPE_II,0);
        if(ret < 0)
        {
            LOG("msgrcv failed,err:%d",errno);
        }
        LOG("result:\nparam1:%d\nparam2:%d",st_fota.param1,st_fota.param2);
    }

    printf("hello world");
    return 0;
}



