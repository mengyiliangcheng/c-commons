#include <iostream>

/*
*给定一个树的层次序列和各结点的度，写树构造函数
*/

using namespace std;
typedef char DataType;

typedef struct ST_NODE
{
    DataType data;
    struct ST_NODE *child;
    struct ST_NODE *sibling;
}ST_NODE,*ST_TREE;

/*e[]:层次序列 degree[]:各结点的度 n:树节点个数*/
void create_tree(ST_TREE* tree,DataType e[],int degree[],int n)
{
    ST_NODE** pointer = new ST_TREE[100];
    
    int i = 0,j = 0,d = 0,k = 0;
    /* 初始化 */
    for(i = 0;i < n;i ++)
    {
        pointer[i] = new ST_NODE;
        pointer[i]->data = e[i];
        pointer[i]->child = pointer[i]->sibling = NULL;
    }
    for(i = 0;i < n;i ++)
    {
        d = degree[i];
        if(d)
        {
            k++;
            pointer[i]->child = pointer[k];
            for(j = 2;j <= d;j ++)
            {
                pointer[k]->sibling = pointer[k+1];
                k++;
            }
        }
    }
    *tree = *pointer;
}

void print_tree(ST_TREE tree)
{
    if(tree)
    {
        cout << tree->data << " " ;
        //cout <<"print child:" ;
        print_tree(tree->child);
        //cout << "print sibling:";
        print_tree(tree->sibling);
    }
}


int main()
{
    ST_TREE tree;
    DataType e[] = { 'a','b','c','d','e','f','g','h','i' };
    int degree[] = { 3,3,2,0,0,0,0,0,0 };
    create_tree(&tree,e,degree,sizeof(e)/sizeof(DataType));
    print_tree(tree);
    return 0;
}


