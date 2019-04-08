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

#include <stdio.h>
#include <execinfo.h>


#define MAX_DEPTH       (20)

/*需要在编译的时候加上编译器选项-rdynamic -funwind-tables -ffunction-sections*/
void print_stack_frame(void)
{
    void* buffer[MAX_DEPTH];
    int depth = backtrace(buffer,MAX_DEPTH);
    char** func_names = backtrace_symbols(buffer,depth);
    int i;
    for(i = 0;i < depth;i ++)
    {
        printf("Depth: %d, func name: %s\n", i, func_names[i]);
    }
}


