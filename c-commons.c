#include <stdio.h>
#include <stdlib.h>
#include "commons_log.h"
#include "utils_string.h"
#include "osadapter.h"
#include "test.h"

#define COMMONS_MAIN_LOG(...) COMMONS_LOG("MAIN",__VA_ARGS__);

typedef struct
{
    char dispName[64];
    int (*fun)(void);
}ST_TEST_LIST;


int testOthers()
{
    //mg_aes_cbc_encrypt();
    return 0;
}


ST_TEST_LIST TabTestList[] =
{
    {"test strings_to_hex",        testStringToHex},
    {"test mempool",               testMempool },
    {"test commons_scanf",         testCommonsScanf},
    {"test commons_rand_str",      testCommonsRandStr},
    {"test file_save_overlap",     testFileSaveOverlap},
    {"test utils_cal_crc_8",       testCrc8},
    {"test file_copy",             testFileCopy},
    {"test package",               testFilePackage},
    {"test read xml",              testReadXMl},
    {"test others",                testOthers}

};

#define TEST_TABLE_SIZE (sizeof(TabTestList) / sizeof(ST_TEST_LIST))



void testProgramUI(void)
{
    int i = 0;
    commons_println("************c-commons test function*****************");
    for(i = 0;i < TEST_TABLE_SIZE;i ++){
        commons_println("*%d:%s",i+1,TabTestList[i].dispName);
    }
    commons_println("****************************************************");
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
        if(NULL == TabTestList[num - 1].fun)
        {
            commons_println("no function");
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
