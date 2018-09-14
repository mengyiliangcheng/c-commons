#include <stdio.h>
#include <stdlib.h>
#include "commons_log.h"
#include "utils_string.h"
#include "osadapter.h"

#define COMMONS_MAIN_LOG(...) COMMONS_LOG("MAIN",__VA_ARGS__);


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

int testCommonsScanf()
{
    s8 tmp[256] = {0};
    s32 ret;
    commons_println("please input:");
    ret = commons_scanf(tmp,sizeof(tmp),SCANF_NUMBER);
    if(ret <= 0){
        commons_println("input error");
    }
    
}

int testMempool()
{
    int ret;
    char* ptr = NULL;
    s8 tmp[256] = {0};
    int num;

    commons_println("pls input size:");
    ret = commons_scanf(tmp,sizeof(tmp),SCANF_NUMBER);
    if(ret <= 0){
        commons_println("input error");
        return -1;
    }

    num = atoi(tmp);
    
    ptr = mempool_malloc(num);
    if(NULL == ptr)
    {
        COMMONS_MAIN_LOG("malloc failed");
    }
} 


ST_TEST_LIST TabTestList[] =
{
    {"test strings_to_hex",        testStringToHex},
    {"test mempool",               testMempool },
    {"test commons_scanf",         testCommonsScanf}
    
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
        memset(buffer,0,sizeof(buffer));
        commons_scanf(buffer,sizeof(buffer),SCANF_NUMBER);
        if(0 == strlen(buffer))
        {
            commons_println("input error!!!");
            continue;
        }
        num = atoi(buffer);
        if(num <= 0 || num > TEST_TABLE_SIZE)
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
    testProgramProcess();
    
    return 0;
}
