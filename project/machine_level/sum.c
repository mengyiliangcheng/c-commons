/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/12  automatic
*/
#include <stdio.h>

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

long plus(long x ,long y)
{
    return x + y;
}

void sumstore(long x,long y,long* dest)
{
    long t = plus(x,y);
    *dest = t;
}

int main(int argc,char* argv[])
{
    long x = atoi(argv[1]);
    long y = atoi(argv[2]);
    long z;

    sumstore(x,y,&z);
    printf("%ld + %ld = %ld\n",x,y,z);

    return 0;
}


