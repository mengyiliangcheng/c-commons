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

#include "digest.h"
#include "commons_log.h"

#define COMMONS_DIGEST_LOG(...) COMMONS_LOG("DIGEST",__VA_ARGS__);


u8 utils_cal_crc_8(u8* src,s32 len)
{
    u8 crc = 0;
    if(!src || len <= 0)
    {
        return 0;
    }
    
    crc = *src;
    while(--len)
    {
        src ++;
        crc ^= *src;        
    };
    return crc;
}


