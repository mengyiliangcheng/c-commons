#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void signal_hander()
{
    printf("signal hander\n");
}

int main()
{
    signal(SIGRTMIN+10,signal_hander);
    printf("test thread\n");
    while(1)
    {
         printf("hello,pid:%d\n",getpid());
         sleep(1);
    }
}
