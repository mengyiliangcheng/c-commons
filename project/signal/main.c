/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 automatic
*/
#include <stdio.h>
#include <signal.h>
//#include <>

int process_exit(int num)
{
    printf("child is disconnect\n");
    return 0;
}

int main()
{
    
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        printf("error\n");
        exit(-1);
    }
    else if(pid == 0)
    {
        printf("child process\n");
        sleep(3);
        exit(0);
    }
    else
    {
        signal(SIGCHLD,process_exit);
        printf("parent process\n");
        int status;
        int ret = wait(&status);
        printf("wait ret:%d,status:%d",ret,status);
    }

    printf("hello world");
    return 0;
}


