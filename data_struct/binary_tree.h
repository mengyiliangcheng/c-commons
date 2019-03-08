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
#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

typedef int TREE_DATA_TYPE;

typedef struct ST_BINARY_TREE_NODE
{
    TREE_DATA_TYPE data;
    struct ST_BINARY_TREE_NODE *lchild,*rchild;
}ST_BINARY_TREE_NODE,*ST_BINARY_TREE;

typedef enum
{
    POINT_LINK,
    POINT_THREAD,
}E_POINT_TAG;

typedef struct ST_BINARY_THREAD_NODE
{
    TREE_DATA_TYPE data;
    struct ST_BINARY_THREAD_NODE *lchild,*rchild;
    E_POINT_TAG ltag,rtag;
}ST_BINARY_THREAD_NODE,*ST_BINARY_THREAD_TREE;


extern int create_binary_tree(ST_BINARY_TREE *tree,TREE_DATA_TYPE node_data);
extern int binary_tree_main();

#endif
