/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/04/13 automatic
*/
#include<stdio.h>
#include<string.h>
int count=0;
/*
void swap(char &a,char &b)
{	
	char temp;	temp=a;	a=b;	b=temp;
}*/
int finish(char list[],int k,int i)
{//第i个元素是否在前面元素[k...i-1]中出现过	
	if(i>k)	
	{		
		for(int j=k;j<i;j++)			if(list[j]==list[i])				return 0;		
	}	
	return 1;
}
void perm(char list[],int k,int m)
{	if(k==m)    
	//当只剩下一个元素时则输出 	
	{		
		count++;		
		for(int i=0;i<=m;i++)			
			printf("%c",list[i]);		
		putchar('\n');	
	}	
	for(int i=k;i<=m;i++)  
		//还有多个元素待排列，递归产生排列	
	{		
		if(finish(list,k,i))		
		{			
			//swap(list[k],list[i]);
			char tmp;
			tmp = list[k];
			list[k] = list[i];
			list[i] = tmp;
			perm(list,k+1,m);			
			//swap(list[k],list[i]);	
			tmp = list[k];
			list[k] = list[i];
			list[i] = tmp;			
		}	
	}		
}

int getlist1(char tmp[],int a,int b)
{
	memset(tmp,0,sizeof(tmp));
	printf("a:%d,b:%d\n",a,b);
	for(int x = 0;x < a;x ++)
	{
		strcat(tmp,"3");	
	}
	for(int x = 0;x < b;x++)
	{
		strcat(tmp,"1");
	}
}

int getlist(char tmp[],int a,int b)
{
	int i = 0;
	for(int x = 0;x < a;x ++)
	{
		//strcat(tmp,"3");	
		tmp[i++] = '3';
	}
	for(int x = 0;x < b;x++)
	{
		//strcat(tmp,"1");
		tmp[i++] = '1';
	}
	tmp[i] = 0;
}

int main()
{	/*int i,n;	
	printf("请输入元素个数:\n"); 	scanf("%d",&n);	printf("请输入待排列的元素:\n");	getchar();	char *a=new char[n];	
	for(i=0;i<n;i++)		
		scanf("%c",&a[i]);	*/
	int num = 9;
	int i ,j;
	i = num / 3;
	j = num - i*3;
	char tmp[100] = {0};
	for(int x = 0;x <= i;x ++)
	{
		getlist(tmp,x,num-x*3);
		printf("tmp:%s,strlen:%d",tmp,strlen(tmp));
		printf("所有不同排列为:\n");	
	perm(tmp,0,strlen(tmp)-1);	
	printf("排列总数为：%d\n",count);	
	}
	
	
	return 0;
} 


