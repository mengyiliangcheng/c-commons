/*
* Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#ifndef __DYNAMIC_MEM_MANAGE_H__
#define __DYNAMIC_MEM_MANAGE_H__

typedef enum
{
    TRUNCK_FREE,
    TRUNCK_OCCUPY,
}e_trunck_tag;

typedef struct WORD
{
    union
    {
        struct WORD *llink;  //head domain,previous node
        struct WORD *uplink; //tail domain,pointing to the head of node
    };
    int tag;                 //trunck flag,0:free,1:occupy
    int size;                //head domain,size of trunck
    struct WORD *rlink;      //head domain,next node
    
}WORD,head,foot,*Space;

#define FootLoc(p)                  (p + p->size -1)
#define MIN_MEMORY_MARGIN           (5)
#define MAX_MEMORY_SIZE             (10000)

#endif
