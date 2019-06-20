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
#include <errno.h>
#include "commons_log.h"
#include "utils_string.h"
#include "osadapter.h"
#ifdef SUPPORT_XML
#include "utils_xml.h"
#endif
#include "utils_network.h"
#include "test.h"
#include "cevent.h"
#include "utils_trace.h"
#include "utils_thread.h"

#ifdef USE_PRINTLN
#define LOG commons_println
#else
#define LOG(...) COMMONS_LOG("MAIN",__VA_ARGS__);
#endif

#define CMD_PROMPT  " > "

ST_TEST_LIST TabTestList[] =
{
#if 1
    {"test strings_to_hex",        testStringToHex},
    {"test strings_isdigit",       testStringIsDigit},
    {"test string split",          testStringSplit},
    {"test mempool",               testMempool },
    {"test commons_scanf",         testCommonsScanf},
    {"test commons_rand_str",      testCommonsRandStr},
    {"test file_save_overlap",     testFileSaveOverlap},
    {"test utils_cal_crc_8",       testCrc8},
    {"test file_copy",             testFileCopy},
    {"test File Copy Fast",        testFileCopyFast},
    {"test package",               testFilePackage},
#ifdef SUPPORT_XML
    {"test read xml",              testReadXMl},
#endif
    {"test others",                testOthers},
    {"testCbcEncrypt",             testCbcEncrypt},
    {"testAnalyzeLog",             testAnalyzeLog},
    {"testConvertTime",            testConvertTime},
    {"testSignal",                 testSignal},
    {"testCreateProcess",          testCreateProcess},
    {"testThreadCond",             testThreadCond},
    {"testCreateServer",           testCreateServer},
    {"testCreateClient",           testCreateClient},
    {"testHexToString",            testHexToString},
    {"testCurl",                   testCurl},
    {"testEvent",                  testEvent},
    {"testEventClient",            testEventClient},
    {"testThreadPool",             testThreadPool},
    {"testUtilsSignal",            testUtilsSignal},
    {"testBinaryTree",             testBinaryTree},
    {"testHuffmanTree",            testHuffmanTree},
    {"testSemWait",                testSemWait},
    {"testDynamicMem",             testDynamicMem},
    {"testPrintStaceFrame",        testPrintStaceFrame},
    
#endif
    //{"testAnalyzeLog",             testAnalyzeLog},
};

#define TEST_TABLE_SIZE (sizeof(TabTestList) / sizeof(ST_TEST_LIST))

void testProgramUI(void)
{
    int i = 0;
    commons_println("************c-commons test function*****************");
    for(i = 0;i < TEST_TABLE_SIZE;i ++){
        commons_println("*%02d:%s",i+1,TabTestList[i].dispName);
    }
    commons_println("****************************************************");
    return;
}


int testProgramProcess(void)
{
    char buffer[128];
    int num;
    testProgramUI();
    commons_print(CMD_PROMPT);
    while(1)
    {
        memset(buffer,0,sizeof(buffer));
        commons_scanf(buffer,sizeof(buffer),SCANF_NUMBER);
        if(0 == strlen(buffer))
        {
            commons_println("input error!!!");
            goto CONTINUE;
            continue;
        }
        num = atoi(buffer);
        if(99 == num)
        {
            return 0;
        }
        if(num <= 0 || num > TEST_TABLE_SIZE)
        {
            commons_println("input error!!!");
            goto CONTINUE;
            continue;
        }
        if(NULL == TabTestList[num - 1].fun)
        {
            commons_println("no function");
            goto CONTINUE;
            continue;
        }
        commons_println("%02d:%s",num,TabTestList[num - 1].dispName);
        TabTestList[num - 1].fun();
        goto CONTINUE;

CONTINUE:
        commons_print(CMD_PROMPT);

    }
    return 0;
}

void testStringIsDigit()
{
    char c;
    char buffer[128];
    int ret = 0;
    char* p = NULL;

    commons_println("please input:");
    commons_flush(stdin);
    scanf("%c",&c);

    commons_println("char is:%c,result:%d",c,strings_isdigit(c));

    c = '0';
    commons_println("char is:0,result:%d",strings_isdigit(c));

    c = '1';
    commons_println("char is:1,result:%d",strings_isdigit(c));

    c = '9';
    commons_println("char is:9,result:%d",strings_isdigit(c));

    c = 'a';
    commons_println("char is:a,result:%d",strings_isdigit(c));

    c = ' ';
    commons_println("char is:`blank`,result:%d",strings_isdigit(c));

    c = '\0';
    commons_println("char is:`null`,result:%d",strings_isdigit(c));
}

void testStringSplit()
{
    int i = 0;
    char buf[1024];
    char delim;
    char** res = NULL;
    commons_println("pls input string:");
    scanf("%s",buf);
    commons_println("pls input delim:0-blank 1-,");
    scanf("%d",&i);
    if(i == 0)
    {
        delim = ' ';
    }else if(i == 1)
    {
        delim = ',';
    }else
    {
        delim = ' ';
    }
    commons_println("delim:%c,string:%s",delim,buf);
    res = strings_split(buf,delim);
    if(res == NULL)
    {
        commons_println("failed!!!");
    }

    while(res[i] != 0)
    {
        commons_println("%s",res[i]);
        i++;
    }

    free(res);
    free(res[0]);

}

void testStringToHex()
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

}

void testHexToString(void)
{
    char key1[]={ 0x08,0x09,0x0b,0x00,0x00,0x03,0x08,0x00,0x0a,0x03, \
                                        0x0f,0x01,0x01,0x0f,0x01,0x0a,0x0f,0x06,0x02,0x00, \
                                        0x01,0x02,0x06,0x09,0x07,0x0b,0x03,0x07,0x0b,0x09, \
                                        0x0b,0x09};
    char buffer[1024] = {0};
    
    strings_hex_to_str(key1,sizeof(key1),buffer,1);
    
    commons_println("buffer:%s",buffer);
}

void testCommonsScanf()
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

void testMempool()
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


void testCommonsRandStr()
{
    int len = 512;
    char tmp[1025] = {0};
    commons_rand_str(tmp,len);
    commons_println(tmp);
}

void testFileSaveOverlap()
{
    char path[] = "./tmp.txt";
    char buf[] = "helloworld";
    s32 ret;
    ret = file_save_overlap(path,buf,strlen(buf));
    LOG("save file:%d",ret);
}

void testCrc8()
{
    char buf[] = {1,2,3};
    u8 crc;
    crc = utils_cal_crc_8(buf,sizeof(buf));
    LOG("crc:%d",crc);
}

void testFileCopy()
{
    file_copy("./c-commons.elf","./hello.txt");
}

void testFileCopyFast()
{
#include <time.h>

    time_t start = time(NULL);
    file_copy_f("./visio.iso","./hello.bin");
    file_copy_f("./visio.iso","./hello.bin");
    file_copy_f("./visio.iso","./hello.bin");
    file_copy_f("./visio.iso","./hello.bin");
    time_t end = time(NULL);
    commons_println("start:%d,end:%d",start,end);

    start = time(NULL);
    file_copy("./visio.iso","./hello.bin");
    file_copy("./visio.iso","./hello.bin");
    file_copy("./visio.iso","./hello.bin");
    file_copy("./visio.iso","./hello.bin");
    end = time(NULL);

    commons_println("start:%d,end:%d",start,end);

    start = time(NULL);
    system("cp -rf ./visio.iso ./hello.bin");
    system("cp -rf ./visio.iso ./hello.bin");
    system("cp -rf ./visio.iso ./hello.bin");
    system("cp -rf ./visio.iso ./hello.bin");
    end = time(NULL);

    commons_println("start:%d,end:%d",start,end);
}
#ifdef SUPPORT_XML
void testReadXMl()
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
#endif

void testFilePackage()
{
    file_package("./commons_type.h","commons_type-compress");
}

void testCbcEncrypt()
{
    int i;
#if 0
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
#endif
    char key[] = {0x31,0x34,0x30,0x30,0x30,0x32,0x39,0x38,0x33,0x38,0x0A,0x06,0x53,0x5F,0x53,0x58};
    char org_data[] = {0x61,0x63,0x37,0x39,0x30,0x61,0x63,0x63,0x34,0x35,0x30,0x34,0x32,0x36,0x32,0x64,0x35,0x34,0x32,0x61,0x39,0x63,0x36,0x61,0x66,0x65,0x63,0x39,0x30,0x33,0x32,0x33};

    commons_println("key:");
    commons_print_hex_no_space(key,16);
    commons_println("org:");
    commons_print_hex_no_space(org_data,sizeof(org_data));
    utils_cbc_encrypt(org_data,sizeof(org_data),key,org_data);
    commons_println("after:");
    commons_print_hex_no_space(org_data,sizeof(org_data));

    utils_cbc_decrypt(org_data,sizeof(org_data),key,org_data);
    commons_println("de after:");
    commons_print_hex_no_space(org_data,sizeof(org_data));
}

void testAnalyzeLog()
{
    analyze_log_client();
}

void testConvertTime()
{
    utils_time_convert();
}

void testThreadCond()
{
    utils_thread_cond();
}

#include<signal.h>
void testSignal()
{
    //pid_t pid;
    //pid = getPidByName("testThread.elf");
    //LOG("pid:%d",pid);
    //kill(pid,SIGRTMIN+10);
    signal(SIGINT,sigint_handler);
    int ret = sleep(100);
    commons_println("ret:%d err:%d",ret,errno);
    signal(SIGINT,SIG_DFL);
}

void sigint_handler(int signum)
{
    commons_println("sig interp");
}

void testCreateProcess()
{
    char path[128];
    char cmd[128];
    int block;
    commons_println("pls input process path:");
    scanf("%s",path);
    commons_println("pls input process cmd:");
    scanf("%s",cmd);
    commons_println("pls input select block(0/not 1/yes):");
    scanf("%d",&block);

    commons_println("path:[%s] cmd:[%s] block[%d]",path,cmd,block);

    signal(SIGINT,sigint_handler);
    //utils_thread_create_process_block(path,cmd);

    if(block == 0)
    {
        utils_thread_create_process(path,cmd);
    }else
    {
        utils_thread_create_process_block(path,cmd);
    }

    signal(SIGINT,SIG_DFL);

}

void testCreateThread()
{
    utils_thread_create_process("./thread/test/testThread.elf",NULL);
    sleep(3);
    testSignal();
}

void testCreateServer()
{
    utils_thread_create_process("./network/test/testServer.elf",NULL);
}

void testEvent(void)
{
    serverMain();
}

void testEventClient(void)
{
    Client("/tmp/mc_socket");
}

void testCreateClient()
{
    int semid;
    semid = utils_thread_init_sem();
    if(semid < 0)
    {
        return -1;
    }
    utils_thread_create_thread((void*)utils_network_create_client,NULL);

    utils_thread_p(semid,0);
}

void testCurl()
{
    open_webservice();
}

void testOthers()
{

    time_t ts,time_offset;
    int rc;
    
    commons_println("genoff update time hander");

    time(&ts);
    commons_println("Daemon:%s: time:%lld\n",
                __func__,ts);
}

void testThreadPool()
{
    thread_pool_main();
}

void testUtilsSignal()
{
    signal_main();
}

void testBinaryTree()
{
    binary_tree_main();
}

void testHuffmanTree()
{
    huffman_tree_main();
}

void testSemWait()
{
    sem_wait_main();
}

void testDynamicMem()
{
    dynamic_mem_manage_main();
}

void testPrintStaceFrame()
{
    print_stack_frame();
}



