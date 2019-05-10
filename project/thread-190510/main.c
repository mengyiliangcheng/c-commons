/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/10  automatic
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

/* 用于log打印的宏 */
#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

/* 退出线程 */
#define THREAD_EXIT(cmd) do { \
                                pthread_exit(cmd); \
                                return NULL; \
                            }while(0)

#define FILE_PATH  "./student.txt"
#define MAX_BUFFER_SIZE   1024

int getscore(char* input)
{
    char id[32] = {0};
    char name[32] = {0};
    int  cplusplus_score = 0;
    int  system_score = 0;
    int  database_score = 0;
    int ret = 0;
    if(NULL == input)
    {
        return -1;
    }

    /* 按照格式解析数据 */
    ret = sscanf(input,"%s %s %d %d %d", \
            id,name,&cplusplus_score,&system_score,&database_score);
    if(ret < 0)
    {
        LOG("sscanf failed,err:%d",errno);
    }
    return cplusplus_score;
}

void* callback(void* args)
{
    FILE* fp = NULL;
    char line_buf[MAX_BUFFER_SIZE] = {0};
    int score = 0;
    int ret = 0;
    int num = 0;

    /* 打开文件 */
    fp = fopen(FILE_PATH,"r");
    if(NULL == fp)
    {
        LOG("open file:%s failed,err:%d",FILE_PATH,errno);
        THREAD_EXIT(-1);
    }

    /* 读取每行文件，并解析 */
    while(1)
    {
        /* 一行一行读取 */
        memset(line_buf,0,sizeof(line_buf));
        if(NULL ==  fgets(line_buf,sizeof(line_buf),fp))
        {
            break;
        }
        //LOG("read:%s",line_buf);
        /* 解析每行内容 */
        ret = getscore(line_buf);
        if(ret < 0)
        {
            break;
        }
        //LOG("get score:%d",ret);
        /* 计算总得分 */
        score += ret;
        num ++;
    }

    /* 计算平均分 */
    float average = (float)score / (float)num;
    printf("\ntotal num:%d\ntotal score:%d\naverage score:%f\n",num,score,average);
    THREAD_EXIT(0);
}

int main()
{
    int ret = 0;
    pthread_t thread_id = 0;
    int retval;
    int* p = &retval;

    /* 创建线程 */
    ret = pthread_create(&thread_id,NULL,callback,NULL);
    if(ret < 0)
    {
        LOG("create thread failed,err:%d",errno);
        return -1;
    }

    //LOG("thread id:%ld",thread_id);
    /* 等待线程退出 */
    ret = pthread_join(thread_id,NULL);
    if(ret < 0)
    {
        LOG("thread join failed,err:%d",errno);
        return -1;
    }

    //LOG("finish...");

    return 0;
}


