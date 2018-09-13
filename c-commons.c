#include <stdio.h>
#include "utils_string.h"
#include "string.h"
#include "osadapter.h"

typedef struct
{
    char dispName[64];
    int (*fun)(void);    
}ST_TEST_LIST;

int testStringToHex()
{
    int i = 0;
    int ret;
    char input[128] = {0};
    char buffer[128] = {0};
    
    commons_println("please input:");
    scanf("%s",input);
    ret = strings_to_hex(input,strlen(input),buffer);
    if(ret < 0)
    {
        commons_println("function error,error code:%d",ret);
        return 0;
    }
    commons_print_hex(buffer,strlen(input) / 2 + strlen(input) % 2);
    
    return 0;
}

int testMempool()
{
    int ret;
    char* ptr = NULL;
    ret = mempool_init(); 
    commons_println("mempool init ret=%d",ret); 
    
    ptr = mempool_malloc(100);
    if(NULL == ptr)
    {
        commons_println("malloc failed");
    }
} 


ST_TEST_LIST TabTestList[] =
{
    {"test strings_to_hex",        testStringToHex},
    {"test mempool",               testMempool }
    
};

#define TEST_TABLE_SIZE (sizeof(TabTestList) / sizeof(ST_TEST_LIST))

void testProgramUI(void)
{
    int i = 0;
    commons_println("*******************************************");
    for(i = 0;i < TEST_TABLE_SIZE;i ++){
        commons_println("*%d:%s",i+1,TabTestList[i].dispName);
    }
    commons_println("*******************************************");
    return;
}

int testProgramProcess(void)
{
    char buffer[128];
    int num;
    testProgramUI();
    commons_println("please select item:");
    while(1)
    {
        scanf("%s",buffer);
        num = atoi(buffer);
        if(num < 0 || num > TEST_TABLE_SIZE)
        {
            commons_println("input error!!!");
            continue;
        }
        TabTestList[num - 1].fun();
        commons_println("please select item:");
    }
}

int main()
{
    //printf("hello world");
    testProgramProcess();
    
    return 0;
}
