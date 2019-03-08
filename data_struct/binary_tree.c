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
#include <stdlib.h>
#include "osadapter.h"
#include "commons_log.h"
#include "binary_tree.h"

#define LOG(...) COMMONS_LOG("BTREE",__VA_ARGS__);


typedef int (*visit_callback)(TREE_DATA_TYPE tree_data);

int create_binary_tree(ST_BINARY_TREE *tree,TREE_DATA_TYPE node_data)
{
    if(node_data > 10)
    {
        //printf("#");
        return 0;
    }
    *tree = (ST_BINARY_TREE_NODE*)calloc(sizeof(ST_BINARY_TREE_NODE),1);
    if(!(*tree))
    {
        printf(" err ");
        return -2;
    }
    printf("%d ",node_data);

    (*tree)->data = node_data;
    //printf(" l");
    create_binary_tree(&(*tree)->lchild,node_data+1);
    //printf(" r");
    create_binary_tree(&(*tree)->rchild,node_data+1);
    return 0;
}

int preorder_traverse(ST_BINARY_TREE tree,visit_callback callback)
{
    if(tree)
    {
        printf("%d ",tree->data);
        printf(" l");
        preorder_traverse(tree->lchild,callback);
        printf(" r");
        preorder_traverse(tree->rchild,callback);
    }else{
        printf("#");
    }
}

int midorder_traverse(ST_BINARY_TREE tree,visit_callback callback)
{
    if(tree)
    {
        printf(" l");
        midorder_traverse(tree->lchild,callback);
        printf(" D%d ",tree->data);
        printf(" r");
        midorder_traverse(tree->rchild,callback);
    }else{
        printf("#");
    }
}

int postorder_traverse(ST_BINARY_TREE tree,visit_callback callback)
{
    if(tree)
    {
        printf(" l");
        postorder_traverse(tree->lchild,callback);
        printf(" r");
        postorder_traverse(tree->rchild,callback);
        printf(" D%d",tree->data);
    }else
    {
        printf("#");
    }
}

int midorder_traverse_thread(ST_BINARY_THREAD_TREE tree,visit_callback callback)
{
    ST_BINARY_THREAD_NODE *node = NULL;
    node = tree->lchild;
    //if(node)
}

ST_BINARY_THREAD_TREE pre_thread_tree;

void in_threading(ST_BINARY_THREAD_TREE tree)
{
    if(tree)
    {
        in_threading(tree->lchild);
        if(!tree->lchild)  //前驱线索
        {
            tree->ltag = POINT_THREAD;
            tree->lchild = pre_thread_tree;
        }
        if(!pre_thread_tree->rchild)  //后继线索
        {
            pre_thread_tree->rtag = POINT_THREAD;
            pre_thread_tree->rchild = tree;
        }
        pre_thread_tree = tree;
        in_threading(tree->rchild);
    }
}

int midorder_threading(ST_BINARY_THREAD_TREE *tree,ST_BINARY_THREAD_TREE T)
{
    *tree = (ST_BINARY_THREAD_TREE)calloc(sizeof(ST_BINARY_THREAD_NODE),1);
    if(!(*tree))
    {
        printf("err");
        return -2;
    }

    (*tree)->ltag = POINT_LINK;
    (*tree)->rtag = POINT_THREAD;
    (*tree)->rchild = *tree;

    if(!T)
    {
        (*tree)->lchild = tree;
    }
    else
    {
        (*tree)->lchild = T;
        pre_thread_tree = *tree;
        in_threading(T);
        pre_thread_tree->rtag = POINT_THREAD;
        pre_thread_tree->rchild = *tree;
        (*tree)->rchild = pre_thread_tree;
    }
    
}

typedef struct
{
    unsigned int weight;
    unsigned int parent,lchild,rchild;
}HTNode,*HuffmanTree;

typedef char ** HuffmanCode;

void HuffmanSelect(HuffmanTree *huffmanTree,int n,int* s1,int* s2)
{
    int i = 0;
    int min,min_sec;
    int flag;

    min = 1;min_sec = 1;

    //先取一个最小点
    for(i = 1;i <= n;i ++)
    {
        if((*huffmanTree)[i].parent == 0)
        {
            min = i;
            min_sec = i;
            break;
        }
    }

    //遍历查找最小和次小值
    for(i = 1;i <= n;i ++)
    {
        LOG("[%d] parent:[%d] weight:[%d] lchild:[%d] rchild:[%d]",
            i,(*huffmanTree)[i].parent,(*huffmanTree)[i].weight,
            (*huffmanTree)[i].lchild,(*huffmanTree)[i].rchild);
        //只去查找没有父节点的节点
        if((*huffmanTree)[i].parent == 0)
        {
            //LOG("=====[%d] parent:[%d] weight:[%d] lchild:[%d] rchild:[%d] min[%d] weight:[%d] sec[%d] weight:[%d]",
            //i,(*huffmanTree)[i].parent,(*huffmanTree)[i].weight,
            //(*huffmanTree)[i].lchild,(*huffmanTree)[i].rchild,min,(*huffmanTree)[min].weight,
            //min_sec,(*huffmanTree)[min_sec].weight);

            flag = FALSE;
            //查找比最小值小的节点
            if((*huffmanTree)[i].weight < (*huffmanTree)[min].weight)
            {
                /* 已经找到了比最小值还小的值，此时需要刷新最小值和次小值 */
                //如果最小值比次小值小，则刷新次小值
                if((*huffmanTree)[min].weight < (*huffmanTree)[min_sec].weight)
                {
                    min_sec = min;
                }
                //刷新最小值
                min = i;
                //刷新找到最小值的flag
                flag = TRUE;
            }
            //如果最小值等于次小值，则取当前值为次小值
            if(min == min_sec)
            {
                min_sec = i;
            }
            //如果该节点不是最小值节点，并且该节点数据小于次小值，则刷新次小值
            if((!flag)&&((*huffmanTree)[i].weight <= (*huffmanTree)[min_sec].weight))
            {
                min_sec = i;
            }
        }
    }

    *s1 = min;
    *s2 = min_sec;
}


void HuffmanCoding(HuffmanTree* huffmanTree,HuffmanCode* huffmanCode,int num)
{
    int m = 0,i;
    int p;
    if(num <= 1)
        return;
    int start;
    int c;

    *huffmanCode = (HuffmanCode)calloc(sizeof(char *),num + 1);
    if(NULL == (*huffmanCode))
    {
        return ;
    }
    
    char* cd = (char*)calloc(sizeof(char),num);
    if(NULL == cd)
    {
        free(*huffmanCode);
        return ;
    }

    for(i = 1;i <= num;i ++)
    {
        start = num -1;
        for(c = i,p = (*huffmanTree)[i].parent;p != 0;c = p,p = (*huffmanTree)[p].parent)
        {
            if((*huffmanTree)[p].lchild == c)
            {
                cd[--start] = '0';
            }
            else
            {
                cd[--start] = '1';
            }
        }
        (*huffmanCode)[i] = (char*)calloc(sizeof(char),num - start);

        LOG("i:[%d] code:[%s]",i,&cd[start]);
        strcpy((*huffmanCode)[i],&cd[start]);
    }
}

void HuffmanTreeCreate(HuffmanTree *huffmanTree,int* weight,int num)
{
    int m = 0;
    int s1,s2;
    int i;

    m = 2 * num - 1;

    *huffmanTree = (HuffmanTree)calloc(sizeof(HTNode),(m + 1));
    if(NULL == (*huffmanTree))
    {
        return ;
    }

    for(i = 1;i <= num;i ++)
    {
        (*huffmanTree)[i].weight = weight[i];
        LOG("[%d] weight [%d]",i,weight[i]);
        (*huffmanTree)[i].lchild = 0;
        (*huffmanTree)[i].rchild = 0;
        (*huffmanTree)[i].parent = 0;
    }

    for(i = num + 1;i <= m;i ++)
    {
        (*huffmanTree)[i].weight = 0;
        (*huffmanTree)[i].lchild = 0;
        (*huffmanTree)[i].rchild = 0;
        (*huffmanTree)[i].parent = 0;
    }

    for(i = num + 1;i <= m;i ++)
    {
        HuffmanSelect(huffmanTree,i - 1,&s1,&s2);
        LOG("i:%d,s1:%d,s2:%d",i,s1,s2);
        (*huffmanTree)[s1].parent = i;
        (*huffmanTree)[s2].parent = i;
        (*huffmanTree)[i].lchild = s2;
        (*huffmanTree)[i].rchild = s1;
        (*huffmanTree)[i].weight = (*huffmanTree)[s1].weight + (*huffmanTree)[s2].weight;
        LOG("i:[%d] weight:%d,s1 weight:%d,s2 weight:%d",i,(*huffmanTree)[i].weight,(*huffmanTree)[s1].weight,(*huffmanTree)[s2].weight);
    }
    HuffmanSelect(huffmanTree,i - 1,&s1,&s2);
}

void HuffmanPreorderTraverse(HuffmanTree tree,visit_callback callback)
{
    if(tree)
    {
        printf("%d ",tree->weight);
        printf(" l");
        preorder_traverse(tree->lchild,callback);
        printf(" r");
        preorder_traverse(tree->rchild,callback);
    }else{
        printf("#");
    }
}

int huffman_tree_main()
{
    HuffmanTree tree = NULL;
    HuffmanCode code = NULL;

    int weight[] = {0,5,29,7,8,14,23,3,11};
    
    HuffmanTreeCreate(&tree,weight,(sizeof(weight) / sizeof(int)) - 1);

    HuffmanCoding(&tree,&code,(sizeof(weight) / sizeof(int)) - 1);
}

int binary_tree_main()
{
    ST_BINARY_TREE tree = NULL;
    create_binary_tree(&tree,1);
    commons_println("\r\npreorder");
    printf("h");
    preorder_traverse(tree,NULL);
    commons_println("\r\nmid order");
    midorder_traverse(tree,NULL);
    commons_println("\r\npost order");
    postorder_traverse(tree,NULL);
}




