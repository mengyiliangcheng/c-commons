/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/06/18 automatic
*/

#include <stdio.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
    printf("hello,world,I am %d\n",getpid());
    pause();
    _exit(129);
    return -1;
}
