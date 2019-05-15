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
#include <stdbool.h>
#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

bool little_endian(void)
{
    union
    {
        char a;
        int b;
    }u_endian;

    u_endian.b = 1;

    return (u_endian.a == 1 ? true:false);
}

int main()
{
    printf("%d\n",little_endian());
    printf("hello world");
    return 0;
}


