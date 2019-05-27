#include "types.h"
#include "stat.h"
#include "user.h"

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

static void print_float(int fd,float value,int sgn)
{
	static double pow10[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
    char buf[32] = {0};
    int len = 0U;

    int negative = 0;
    if(value < 0)
    {
        negative = 1;
        value = 0 - value;
    }
    int prec = 8;
    int whole = (int)value;
    printf(fd,"whole:%d\n",whole);
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
    printf(fd,"buf:%s\n",buf);

    while(((len < sizeof(buf)) && (count-- > 0U)))
    {
        buf[len++] = '0';
    }
    if(len < sizeof(buf))
        buf[len++] = '.';
    printf(fd,"buf:%s\n",buf);

    while(len < sizeof(buf))
    {
        buf[len++] = (char)(48 + (whole% 10));
        if(!(whole /= 10))
            break;
    }

    printf(fd,"buf:%s\n",buf);
    if(1 == negative)
    {
        buf[len++] = '-';
    }

    while(--len > 0)
    {
        putc(fd,buf[len]);
    }
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, char *fmt, ...)
{
  char *s;
  float * fp;
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
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
        fp = (float*)ap;
        print_float(fd,*fp,1);
        //ap++;
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
