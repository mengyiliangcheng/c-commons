/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/29  automatic
*/

#ifndef __LIST_H__
#define __LIST_H__


#define LIST_ADD(a,b)       list_add((stLinklist*)a,(stLinklist*)b)
#define LIST_DEL(a,b,c)     list_delete((stLinklist*)a,b,(stLinklist**)c)
#define LIST_FIND(a,b)      list_find((stLinklist*)a,b)
#define LIST_COUNT(a)       list_count((stLinklist*)a)

typedef struct _stLinklist
{
    struct _stLinklist* next;
    int id;
}stLinklist;

typedef struct _stTest
{
    struct _stTest* next;
    int id;
    int dirty;
    int value;
}stTest;

#endif
