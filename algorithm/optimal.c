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
#include "optimal.h"

s32  index_kmp(s8* src_str,s8* match_str,s32 pos)
{
    s32 i;
    s32 j;
    
    i = pos;
    j = 1;
    while(i < src_str[0] && j <= match_str[0])
    {
        if(j == 0 || src_str[i] == match_str[j])
        {
            ++i;
            ++j;
        }
        else
        {
            j = next[j];    
        }

        if(j > match_str[0]) 
            return i-match_str[0];
        else
            return 0;
    }
    
}


