/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/04/15 automatic
*/
#include <iostream>

#include<stdio.h>
#include<stdlib.h>

struct List {
	int flag;
	int x;//X的次幂 
	int y;//Y的次幂 
	int num;//系数 
	struct List* next;
};//链表定义 
 List* creat(void) {
 	struct List *p;
 	p=(struct List *)malloc(sizeof(struct List));
 	p->flag=1;
 	p->x=0;
	 p->y=0;
	 p->num=0;
	 p->next=NULL;
	 return p; 
 }//链表初始化函数
 List* add(struct List* p,struct List* q) ;//加法函数 
 List* minus(struct List* p,struct List* q) ;//减法函数
 List* multiply(struct List* p,struct List* q) ;//乘法函数
 List* rank(struct List* p) ;//排序函数
 List* conbine(struct List* p) ;//合并同类项函数
 List* SCAN(char s[]) ;//字符输入转化为多项式
 void PRINT(List* p);//输出函数 
 
 List* rank(struct List* p) {
   List *ptr1,*ptr2,*T;//中介链表 
   ptr1=ptr2=NULL;
   T=creat(); 
   for(ptr1=p;ptr1->next!=NULL;ptr1=ptr1->next){
   	for(ptr2=p->next;ptr2!=NULL;ptr2=ptr2->next){
   		if( (ptr1->x+ptr1->y) <= (ptr2->x+ptr2->y) && (ptr1->x)<(ptr2->x)){
   			T->x=ptr1->x;
   			T->y=ptr1->y;
   			T->num=ptr1->num;
   			ptr1->x=ptr2->x;
   			ptr1->y=ptr2->y;
   			ptr1->num=ptr2->num;
   			ptr2->x=T->x;
   			ptr2->y=T->y;
   			ptr2->num=T->num;
		   }//选择排序 
	   }
   }
   return p;
 } //排序函数 
 void PRINT(List* p){
 	List *ptr;
 	int t=0;
 	for(ptr=p;ptr!=NULL;ptr=ptr->next){
 		if(p->flag==-1){
 			printf("-%dX^%dY^%d",ptr->num,ptr->x,ptr->y);
 			t++;
		 }
 		else {
 			if(t==0){
 				printf("%dX^%dY^%d",ptr->num,ptr->x,ptr->y);
 				t++;
			 }//判断是否为首项 
		    else 
			printf("+%dX^%dY^%d",ptr->num,ptr->x,ptr->y);
		 }
	 }
 }//输出函数 
 
 //123 + 5
 List* SCAN1(char s[])
 {
	 List* head = NULL,*p = NULL;
	 head = creat();
	 
 }
 
 List* SCAN(char s[]) {
 	List *head,*tail,*p;//定义头尾结点链表 
 	int i=0;
 	head=tail=NULL;
 	p=creat();//链表初始化 
 	while(s[i]!='\0'){
 		if(s[i]=='-'){
 			p->flag=-1;
 			i++;
		 }
		 while(s[i]>='0'&&s[i]<='9') {
		 p->num=p->num*10+s[i];
		 i++;
		 }//读取系数 
		 if(s[i]=='X'){
 			p->x=s[i+2];
 			i=i+3;
		 }//读取X指数幂 
		 if(s[i]=='Y'){
		 	p->y=s[i+2];
 			i=i+3;
		 }//读取Y指数幂
		 if(s[i]=='+'){
		 	if(head==NULL)
		 	head=p;
		 	else
		 	tail->next=p;
		 	tail=p;
		 	i++;
		 }//数据读入链表 
		 if(s[i]=='-'){
		 	if(head==NULL)
		 	head=p;
		 	else
		 	tail->next=p;
		 	tail=p;
		 }//数据读入链表，读到-号，i不移动 
		 if(s[i]=='\0'){
		 	if(head==NULL)
		 	head=p;
		 	else
		 	tail->next=p;
		 	tail=p;
		 } //数据读入链表，读到结尾，i不移动 
	 }
 }//输入函数 
 
struct List *add(struct List *a,struct List *b)  //将两链表相加，并返回一个新链表的头指针
{
 	struct List *p1;
 	p1=creat();
 	p1=a;
 	while(p1->next) p1=p1->next;  //令 p1 指向a链表的最后一个节点 
 	p1->next=b->next;  //将两链表连接 
 	return a;
 }
 
struct List *minus(struct List *a,struct List *b)  //第一个链表减去第二个链表，并返回一个新链表的头指针 
{
	int x;
	struct List *p1;
	p1=creat();
	p1=b->next;
	while(p1){          //将b链表中系数取相反数 
		x=p1->num;
		p1->num=-x;
		p1=p1->next;
	}
	add(a,b);
	return a;
}

struct List *multiply(struct List *a,struct List *b)  //将两链表相乘，并返回一个新链表的头指针 
{
	struct List *p1,*p2,*head,*p;
	head=creat();
	p1=a->next;
	while(p1){            //将a中每一个节点与b中每一个节点相乘 
		p2=b->next;
		while(p2){
			p=creat();
			p->flag=p1->flag*p2->flag;   //符号改变 
			p->num=p1->num*p2->num;   //系数相乘 
			p->x=p1->x+p2->x;   // x次数相加 
			p->y=p1->y+p2->y;    //y次数相加 
			p->next=head->next;
			head->next=p;       //将新节点逆序加到head为头指针的新链表中 
			p2=p2->next;
		}
		p1=p1->next;
	}
	return head;
}
List* conbine(struct List* root)
{
	if(NULL == root)
		return NULL;
	struct List* next = NULL;
	struct List* p = root->next;
	struct List* tmp = NULL;
	while(p)
	{
		tmp = p;
		next = p->next;  /* 指向下一个结点 */
		while(next)
		{
			if(p->x == next->x && p->y == next->y && p->num)  /* 如果p与next的幂和系数相同，并且p的系数不为0 */
			{
				/* 把系数相加 */
				p->num =(p->flag)*(p->num) + (next->flag)*(next->num);
                if(p->num < 0)
                {
                    p->num = -(p->num);
                    p->flag = -1;
                }else
                {
                    p->flag = 1;
                }
				p->num += next->num;          
				tmp->next = next->next;  /*输出next结点 */
				free(next);
				next = tmp->next;
			}
			else if(p->num == 0 && next->num == 0)  /* 如果p的系数为0并且next的结点也为0 */
			{
				//p->x = 0;
				//p->y = 0;
				tmp->next  = next->next;
				free(next);
				next = tmp->next;
			}
			else /* 不能合并，指针指向下一个结点 */
			{
				tmp = tmp->next;
				next = next->next;
			}
		}
		p = p->next; /* p指向下一个结点 */
	}
	return root;
}

/*
using namespace std;
int main()
{
	cout << "hello world" <<endl;
	return 0;
}
*/
int main(void){
 	printf("1-多项式输入 ；\n2-多项式加（+）；\n3-多项式减（-）；\n4-多项式乘（*）；\n5-多项式输出；\n0-退出");
 	int c;//操作输入符
	struct List *N ,*R; //N为新多项式链表输入，r为多项式结果 
	char s[100];//多项式链表字符输入 
    N=creat();
	R=NULL;
 	scanf("%d",&c);
	while(1){
		if(c==1){
			printf("Enter multinomial\n");
			scanf("%s",s);//输入多项式字符 
			if(R==NULL)
			R=SCAN(s);//字符转化为链表 
			else 
			N=SCAN(s);//字符转化为链表
		} 
		if(c==2){
		R=add(R,N);//多项式相加 
		R=conbine(R);//合并同类项	
		R=rank(R);//指数幂降序排序
		}
		if(c==3){
		R=minus(R,N);//多项式相减 
		R=conbine(R);//合并同类项
		R=rank(R);//指数幂降序排序	
		} 
		if(c==4){
		R=multiply(R,N);//多项式相乘 
		R=conbine(R);//合并同类项 
		R=rank(R);//指数幂降序排序	
		}
		if(c==5)
		PRINT(R);//输出结果	
		if(c==0)
		break;//结束运算 
	}  
	 return 0;
 } 

