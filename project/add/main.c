/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 automatic
*/
#include <stdio.h>

/* 两个数相加，返回j相加后的结果 */
int add(int a,int b)
{
    return a+b;
}

/* 两个数比较大小  */
void compare(int a,int b)
{
    if(a > b)
    {
        printf("First integer is larger\n");
    }else if(a < b)
    {
        printf("Second integer is larger\n");
    }else
    {
        printf("The two integer is equal\n");
    }
}

/* 求阶乘  */
int factorial(int num)
{
    int i;
    int sum = 1;

    /* 如果是负数，则直接返回-1 */
    if(num < 0)
    {
        return -1;
    }

    /* 计算阶乘 */
    for(i = 1;i <= num;i ++)
    {
        sum *= i;
        if(sum < 0) //如果数据溢出，则返回-1
        {
            return -1;
        }
    }

    return sum;
}

/* 求所有的正整数小于或等于 num 之和 */
int sumup(int num)
{
    int i;
    int sum = 0;

    /* 如果传入负数，则直接返回-1 */
    if(sum < 0)
    {
        return -1;
    }

    /* 求和 */
    for(i = 1;i <= num;i ++)
    {
        sum += i;
        if(sum < 0) //如果溢出则返回-1
        {
            return -1;
        }
    }
    return sum;
}

int main()
{
    int result = 0;
    int a,b;

    printf("test add,please input first and second integer:\n");
    scanf("%d,%d",&a,&b);
    result = add(a,b);
    printf("result is :%d\n",result);

    printf("test compare,please input first and second integer:\n");
    scanf("%d,%d",&a,&b);
    compare(a,b);

    printf("test factorial,please input num:\n");
    scanf("%d",&a);
    result = factorial(a);
    if(result == -1)
        printf("the first num is too big\n");
    else
        printf("The factorial result of %d is %d\n",a,result);

    printf("test sumup,please input num:\n");
    scanf("%d",&a);
    result = sumup(a);
    if(result == -1)
        printf("The num is negative or overflow\n");
    else
        printf("The sumup() result of ：%d is: %d\n",a,result);


    return 0;
}


