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
#include <stdarg.h>

int myprintf(int num,...)
{
    va_list p_args ;
    va_start(p_args,num);
    int args_val;
    int args_index = 0;

    args_val = num;
    while(args_val != -1)
    {
        printf("%d args is %d\n",++args_index,args_val);
        args_val = va_arg(p_args,int);
        
    }
    va_end(p_args);
}

int main()
{
    myprintf(12,13,14,15);
    printf("hello world");
    return 0;
}


