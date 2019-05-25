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
#include <stdarg.h>

static void print_float(int fd,float value,int sign);
int myprintf(int num,...)
{
    va_list p_args ;
    va_start(p_args,num);
    int args_val;
    int args_index = 0;

    args_val = num;
    while(args_val != -1)
    {
        printf("%d args is %d\n",++args_index,args_val);
        args_val = va_arg(p_args,int);
        
    }
    va_end(p_args);
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

#undef putc
#define putc(...) 
#define printint(...)
#define uint unsigned int
void my_printf(int fd,char* fmt,...)
{
  char *s;
  float * fp;
  int c, i, state;
  uint *ap;

  state = 0;
  printhex(&fmt-1,20);
  ap = (uint*)(void*)&fmt + 1;
  printf("fmt:%p\n",fmt);
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, *ap, 10, 1);
        ap++;
      }else if(c == 'f'){
        printf("ap:%p\n",ap);
        printf("*ap:%X\n",*ap);
        //fp = 
        uint* p = ap;
        p ++;
        printf("*p:%d %f\n",*p,*(float*)p);
       // printf("");
        printf("fp:%f\n",*fp);
        print_float(fd,*fp,1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        printint(fd, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, *ap);
        ap++;
      } else if(c == '%'){
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    }
  } 
}

static void print_float(int fd,float value,int sign)
{
    static double pow10[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
    char buf[32] = {0};
    int i;
    int len = 0U;
    float base = 10.0;

    int negative = 0;
    if(value < 0)
    {
        negative = 1;
        value = 0 - value;
    }
    int prec = 8;
    int whole = (int)value;
    //printf("value:%f whole:%d\n",value,whole);
    //printf("pow10:%f\n",pow10[8]);
    //printf("value-whole:%f\n",value-whole);
    //printf("*100000:%f\n",(value-whole)*(pow10[prec]));
    double tmp = (value - whole)*(pow10[prec]);
    //printf("tmp:%f\n",tmp);
    unsigned long frac = (unsigned long)tmp;
    //printf("frac:%ld\n",frac);

    unsigned int count = prec;
    while(len < sizeof(buf)){
        --count ;
        buf[len++] = (char)(48U + (frac % 10U));
        if(!(frac /= 10U))
             break;
    }
   // printf("buf:%s\n",buf);

    while(((len < sizeof(buf)) && (count-- > 0U)))
    {
        buf[len++] = '0';
    }
    if(len < sizeof(buf))
        buf[len++] = '.';
    //printf("buf:%s\n",buf);

    while(len < sizeof(buf))
    {
        buf[len++] = (char)(48 + (whole% 10));
        if(!(whole /= 10))
            break;
    }

    //printf("buf:%s\n",buf);
}

int main()
{
    float aa = 123.456;
    //print_float(1,aa,1);        
    //printf("\n");
    //printf("int size:%d float size:%d\n",sizeof(int),sizeof(float));
    my_printf(1,"%f",123);
    return 0;
}


