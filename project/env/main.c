/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/06/12  automatic
*/
#include <stdio.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)
int main(int argc,char** argv,char** envp)
{
    int i = 0;
    for(i = 0;i < argc;i++)
    {
        printf("argv[%2d]:%s\n",i,*argv);
        argv++;
    }
    i = 0;
    while(NULL != *envp)
    {
        LOG("env[%2d]:%s\n",i,*envp);
        envp++;
        i ++;
    }
    return 0;
}


