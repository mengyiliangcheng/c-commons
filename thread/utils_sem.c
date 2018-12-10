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

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "osadapter.h"
#include "commons_log.h"

#define MY_SEM_KEY 8888
#define LOG commons_logger

union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short  *array;
};


s32 utils_thread_init_sem(void)
{
    int semid;
    int ret;
    union semun sem_args;
    unsigned short array[1] = {1};
    semid = semget(MY_SEM_KEY,1,IPC_CREAT|0666);
    if(semid < 0)
    {
        LOG("create sem failed,err:%s",strerror(errno));
        return -1;
    }
    /*ipcs -s to check it
      ipcs -s semid to delete it
    */

    ret = semctl(semid,0,SETALL,sem_args);
    if(ret < 0)
    {
        LOG("semctl failed,err:%d",strerror(errno));
    }
    return semid;
}

s32 utils_thread_get_sem(void)
{
    int semid;
    semid = semget(MY_SEM_KEY,1,IPC_CREAT|0666);
    if(semid < 0)
    {
        LOG("create sem failed,err:%s",strerror(errno));
        return -1;
    }
    return semid;
}

s32 utils_thread_p(int semid,int index)
{
    struct sembuf sem_op_p[1];
    int ret;
    sem_op_p[0].sem_num = index;
    sem_op_p[0].sem_op = -1;
    sem_op_p[0].sem_flg = SEM_UNDO;
    ret = semop(semid,&sem_op_p,1);
    if(ret < 0)
    {
        LOG("semop failed,err:%s",strerror(errno));
        return -1;
    }
    return 0;
}

s32 utils_thread_v(int semid,int index)
{
    struct sembuf sem_op_v[1];
    int ret;
    sem_op_v[0].sem_num = index;
    sem_op_v[0].sem_op = 1;
    sem_op_v[0].sem_flg = SEM_UNDO;
    ret = semop(semid,&sem_op_v,1);
    if(ret < 0)
    {
        LOG("semop failed,err:%s",strerror(errno));
        return -1;
    }
    return 0;
}




