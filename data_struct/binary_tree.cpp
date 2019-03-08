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
#include "osadapter.h"
#include "commons_log.h"
#include "binary_tree.h"

int create_binary_tree(ST_BINARY_TREE *tree,TREE_DATA_TYPE node_data)
{
    if(node_data > 100)
    {
        return 0;
    }
    tree = (ST_BINARY_TREE_NODE)alloc(sizeof(ST_BINARY_TREE_NODE));
    if(!tree)
    {
        return -2;
    }
    printf("%d ",node_data);

    *tree->data = node_data;
    create_binary_tree(*tree->lchild,node_data+1);
    create_binary_tree(*tree->rchild,node_data+2);
    return 0;
}

int binary_tree_main()
{
    ST_BINARY_TREE tree;
    create_binary_tree(&tree,1);
}
