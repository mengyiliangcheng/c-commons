/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/05/06  automatic
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF 516 
#define FOTA_MSG_KEY 0Xabcd1234
#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"fota",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)


#define STATISTICS_1TB   40 
#define STATISTICS_1GB    (30)      /*1024*1024*1024 Byte*/

void printll(unsigned long long *in,int len)
{
    unsigned char* ch = (unsigned char*)in;
    if(NULL == ch)
        return ;

    while((len--) > 0)
    {
        printf("%02X",ch[len]);
    }
    printf("\n");
}

void printhex(void* in,int len)
{
    if(NULL == in)
        return ;
    unsigned char* ch = (unsigned char*)in;

    while(len-- > 0)
    {
        printf("%02X",*ch);
        ch ++;
    }
    printf("\n");
}

int main()
{
    char buf[256];
    int *x;
    x = (int *) & buf[2];
    //强制类型转换的是指针而非char型变量，指针指向的内存数据可以认为是任意类型，char、int 、float、double甚至结构    
    *x = 68000;
    printf("x addr:%p\n",x);
    //假设buf的开始地址为0x1000，则这句将0x1001～0x1004的内容拷贝给x（x为int型）,如前所述，int型数据地址必须为4的整数倍，此处0x1001不满足该条件，故会引起CPU内部总线错误    
    printf("*x = %d\n",*x);

    while(1)
    {
        sleep(1);
    }
}

int main1()
{
    unsigned long long a = 5024000000;
    int b = 0;
    b = a >> 20;
    LOG("%lld(0x%016X)>>10:%d(0x%016X)",a,a,b,b);
    printf("a:");
    printll(&a,sizeof(a));
    printf("a>>20:");
    printll(&b,sizeof(b));

    unsigned long long tst;
    tst = 0;
    printll(&tst,8);
    a = ((((unsigned long long)1)<<STATISTICS_1GB)-(unsigned long long)1);
    printf("1<<%d-1:",STATISTICS_1GB);
    printll(&a,8);
    printf("(1<<%d-1)*100:",STATISTICS_1GB);
    a = ((((unsigned long long)1)<<STATISTICS_1GB)-(unsigned long long)1)*(unsigned long long)100;
    printll(&a,8);
    unsigned long long value = 5024000000;
    a = (((value&(((unsigned long long)1<<STATISTICS_1TB)-(unsigned long long)1))*(unsigned long long)100)>>STATISTICS_1TB);
    printf("convert %lld:",a);
    printll(&a,8);
     LOG(" statisticscalculateResult  0X%016X  0X%016X 0x%016x", 
         ((((unsigned long long)1)<<STATISTICS_1GB)-(unsigned long long)1),
        ((((unsigned long long)1)<<STATISTICS_1GB)-(unsigned long long)1)*(unsigned long long)100,
        (((unsigned long long)1)<<STATISTICS_1GB)
        );

}

