/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/05  automatic
*/
#include <stdio.h>
#include <sys/timerfd.h>
#include <time.h>
#include <stdint.h>

int main()
{
    int fd;
    int ret;
    struct itimerspec ts;
    uint64_t num = 0;

    fd = timerfd_create(CLOCK_REALTIME_ALARM,0);
    if(fd < 0)
    {
        printf("error");
    }

    ts.it_interval.tv_sec = 3;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = 3;
    ts.it_value.tv_nsec = 0;
    ret = timerfd_settime(fd,0,&ts,NULL);
    if(ret < 0)
    {
        printf("failed to set time\n");
    }

    int flag = 0;
    while(1)
    {
        ret = read(fd,&num,sizeof(uint64_t));
        if(ret < 0)
        {
            printf("read failed\n");
        }

        printf("num:%d\n",num);



    if(flag == 0){
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;
        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 0;
        ret = timerfd_settime(fd,0,&ts,NULL);
        flag = 1;
        printf("clear\n");
    }
    }
    close(fd);
    printf("hello world");
    return 0;
}


