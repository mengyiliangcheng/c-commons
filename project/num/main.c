/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/04/13  automatic
*/
#include <stdio.h>

int insert(int a,int b)
{
	int tmp[100] = {0};
	int i;
	for(i = 0;i < a;i ++)
	{
		memset(tmp,'1',99);
		tmp[i] = '3';
	}
	////
}
int swap(int *a,int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

int finish()

int swap1(int a,int b)
{
	int i,j;
	//printf("3 num:%d,1 num:%d\n",a,b);
	for(j = 0;j < a;j ++)
	{
		printf("3");
	}
	for(j = 0;j < b;j ++)
	{
		printf("1");
	}
	
	printf("\n");
}

int main()
{
    int n = 9;
    int a = n / 3;
    int b[100] = {0};
    int i,j;
    for(i = 0;i <= a;i ++)
    {
		swap(i,n-i*3);
    }
    
    printf("hello world\n");
    return 0;
}


