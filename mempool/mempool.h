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

#ifndef __COMMONS_MEMPOOL_H__
#define __COMMONS_MEMPOOL_H__

#define COMMONS_MEMPOOL_MAX_POOL_NUM    (100)
#define COMMONS_MEMPOOL_MAX_POOL_SIZE   (4096)

typedef struct
{
    s32* ptrBodyHead[COMMONS_MEMPOOL_MAX_POOL_NUM];
    s32* ptrBodyTail[COMMONS_MEMPOOL_MAX_POOL_NUM];
    s32  iBodySize[COMMONS_MEMPOOL_MAX_POOL_NUM];
}ST_MEMPOOL_HEAD;

typedef struct
{
    u8 heCheckSum[4];
    u8* ptrBody;
}ST_MEMPOOL_BODY;


#endif

