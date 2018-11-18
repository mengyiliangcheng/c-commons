/*
 * Copyright (c) 2018-2018 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include "commons_log.h"
#include "utils_string.h"
#include "osadapter.h"
#include "utils_xml.h"
#include "test.h"

#define LOG(...) COMMONS_LOG("MAIN",__VA_ARGS__);

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
    {"test others",                testOthers},
    {"testCbcEncrypt",						 testCbcEncrypt},

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
    commons_println("input: %s",tmp);
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
        LOG("malloc failed");
    }
}


int testCommonsRandStr()
{
    int len = 512;
    char tmp[1025] = {0};
    commons_rand_str(tmp,len);
    commons_println(tmp);
}

int testFileSaveOverlap()
{
    char path[] = "./tmp.txt";
    char buf[] = "helloworld";
    s32 ret;
    ret = file_save_overlap(path,buf,strlen(buf));
    LOG("save file:%d",ret);
}

int testCrc8()
{
    char buf[] = {1,2,3};
    u8 crc;
    crc = utils_cal_crc_8(buf,sizeof(buf));
    LOG("crc:%d",crc);
}

int testFileCopy()
{
    file_copy("./c-commons.elf","./hello.txt");
}

int testReadXMl()
{
    char* content = NULL;
    content = utils_xml_read_node("./xml/ca.xml","www.baidu.com");
    if(NULL == content)
    {
        return 0;
    }

    commons_println("value:%s",content);
    free(content);
}

int testFilePackage()
{
    file_package("./commons_type.h","commons_type-compress");
}

int testCbcEncrypt()
{
    int i;
    char key1[]={ 0x08,0x09,0x0b,0x00,0x00,0x03,0x08,0x00,0x0a,0x03, \
                                            0x0f,0x01,0x01,0x0f,0x01,0x0a,0x0f,0x06,0x02,0x00, \
                                            0x01,0x02,0x06,0x09,0x07,0x0b,0x03,0x07,0x0b,0x09, \
                                            0x0b,0x09};

    char key2[]={ 0x05,0x0d,0x03,0x0a,0x07,0x08,0x08,0x00,0x0b,0x0e, \
                                            0x0c,0x01,0x05,0x02,0x00,0x0b,0x05,0x0d,0x07,0x04, \
                                            0x06,0x03,0x08,0x09,0x0a,0x00,0x0b,0x03,0x01,0x09, \
                                            0x00,0x04};
    char key[128]={0};
    for(i=0;i<sizeof(key1);i++)
    {
        key[i]=(key1[i]) ^ (key2[i]);
    }
    char org_data[] = { 0x08,0x09,0x0b,0x00,0x00,0x03,0x08,0x00,0x0a,0x03, \
                                            0x0f,0x01,0x01,0x0f,0x01,0x0a,0x0f,0x06,0x02,0x00, \
                                            0x01,0x02,0x06,0x09,0x07,0x0b,0x03,0x07,0x0b,0x09, \
                                            0x0b,0x09};
    commons_println("key:");
    commons_print_hex_no_space(key,32);
    commons_println("org:");
    commons_print_hex_no_space(org_data,sizeof(org_data));
    utils_cbc_encrypt(org_data,sizeof(org_data),key,org_data);
    commons_println("after:");
    commons_print_hex_no_space(org_data,sizeof(org_data));
}


int testOthers()
{
 
}



