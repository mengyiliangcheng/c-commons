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
#include <unistd.h>
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
#define LISTEN_FILENO  STDIN_FILENO
#define MAX_BUF_SIZE  1024


ST_BUILDIN_CMD TabBuildInCmd[] =
{
    {"ls",              buildin_unixcmd},
    {"cd",              buildin_unixcmd},
    {"list",            buildin_showProgram},
    {"exit",            buildin_exit},
    {"quit",            buildin_exit},
    {"help",            buildin_helpDisplay},
};

ST_TEST_LIST TabTestList[] =
{
#if 1
    {"test strings_to_hex",        0,testStringToHex},
    {"test strings_isdigit",       0,testStringIsDigit},
    {"test String Issapce",        0,testStringIssapce},
    {"test String Isprint",        0,testStringIsprint},
    {"test string split",          0,testStringSplit},
    {"test string inputfix",       0,testStringInputFix},
    {"test string to lower",       0,testStringTolower},
    {"test string atof",           0,testStringAtof},
    {"test string path fix",       0,testStringPathFix},
    {"test string strdup",         0,testStringStrdup},
    {"test string strlcpy",        0,testStringStrlcpy},
    {"test mempool",               0,testMempool },
    {"test commons_scanf",         0,testCommonsScanf},
    {"test commons_rand_str",      0,testCommonsRandStr},
    {"test file_save_overlap",     0,testFileSaveOverlap},
    {"test utils_cal_crc_8",       0,testCrc8},
    {"test file_copy",             0,testFileCopy},
    {"test File Copy Fast",        0,testFileCopyFast},
    {"test package",               0,testFilePackage},
#ifdef SUPPORT_XML
    {"test read xml",              0,testReadXMl},
#endif
    {"test others",                0,testOthers},
    {"testCbcEncrypt",             0,testCbcEncrypt},
    {"testAnalyzeLog",             0,testAnalyzeLog},
    {"testConvertTime",            0,testConvertTime},
    {"testSignal",                 100,testSignal},
    {"testCreateThread",           0,testCreateThread},
    {"testCreateProcess",          0,testCreateProcess},
    {"testThreadCond",             0,testThreadCond},
    {"testCreateServer",           0,testCreateServer},
    {"testCreateClient",           0,testCreateClient},
    {"testHexToString",            0,testHexToString},
    {"testCurl",                   0,testCurl},
    {"testEvent",                  0,testEvent},
    {"testEventClient",            0,testEventClient},
    {"testThreadPool",             0,testThreadPool},
    {"testUtilsSignal",            0,testUtilsSignal},
    {"testBinaryTree",             0,testBinaryTree},
    {"testHuffmanTree",            0,testHuffmanTree},
    {"testSemWait",                0,testSemWait},
    {"testDynamicMem",             0,testDynamicMem},
    {"testPrintStaceFrame",        0,testPrintStaceFrame},
    
#endif
    //{"testAnalyzeLog",             testAnalyzeLog},
};

#define TEST_TABLE_SIZE (sizeof(TabTestList) / sizeof(ST_TEST_LIST))

void testProgramUI(void)
{
    int i = 0;
    commons_println("************c-commons test function*****************");
    for(i = 0;i < TEST_TABLE_SIZE;i ++){
        commons_println(" *%02d:%s",i+1,TabTestList[i].dispName);
    }
    commons_println("****************************************************");
    return;
}


int testProgramProcess(void)
{
    fd_set fds;
    int fdsize = 0;
    int ret;
    int num;
    int i = 0;
    int read_bytes;
    char buffer[MAX_BUF_SIZE];

    testProgramUI();
    commons_print(CMD_PROMPT);
    fflush(stdout);
    while(1)
    {
        FD_ZERO(&fds);
        FD_SET(LISTEN_FILENO,&fds);
        fdsize = LISTEN_FILENO + 1;

        ret = select(fdsize,&fds,NULL,NULL,NULL);
        if(ret < 0)
        {
            commons_print("Select Failed,err:%d",errno);
            continue;
        }
        else if(ret > 0)
        {
            if(FD_ISSET(LISTEN_FILENO, &fds))
            {
                memset(buffer,0,MAX_BUF_SIZE);
                read_bytes = read(LISTEN_FILENO, buffer, MAX_BUF_SIZE - 1);
                if(read_bytes <= 0)
                {
                    commons_println("read error!!! %s",strerror(errno));
                    goto CONTINUE;
                }

                LOG("i:%d buffer:%s",i,buffer);
                ret = commandProcess(buffer);
                if(ret != 0)
                {
                    if(ret == -1)
                        commons_println("input error!!!");
                    goto CONTINUE;
                }

                for(i = 0;i < read_bytes;i ++)
                {
                    if(!strings_isspace(buffer[i]) && !strings_isdigit(buffer[i]))
                    {
                        commons_println("input error!!!");
                        goto CONTINUE;
                    }
                }

                num = atoi(buffer);
                if(num <= 0 || num > TEST_TABLE_SIZE)
                {
                    commons_println("input error!!!");
                    goto CONTINUE;
                }
                if(NULL == TabTestList[num - 1].fun)
                {
                    commons_println("no function");
                    goto CONTINUE;
                }
                commons_println("%02d:%s",num,TabTestList[num - 1].dispName);
                TabTestList[num - 1].fun();
                goto CONTINUE;
            }
        }

CONTINUE:
        commons_print(CMD_PROMPT);
        fflush(stdout);
    }
}

int commandProcess(char* cmdline)
{
    int i = 0;
    int num;
    int cmdline_len = strlen(cmdline);

    while(i < cmdline_len)
    {
        if(strings_isdigit(cmdline[i])) return 0;  //Contain digit

        if(strings_isspace(cmdline[i]))
        {
            i++;
            continue;
        }

        for(num = 0;num < sizeof(TabBuildInCmd) / sizeof(ST_BUILDIN_CMD);num ++)
        {
            if(0 == strncmp(&cmdline[i],TabBuildInCmd[num].cmd,strlen(TabBuildInCmd[num].cmd)))
            {
                TabBuildInCmd[num].func(TabBuildInCmd[num].cmd,&cmdline[i+strlen(TabBuildInCmd[num].cmd)]);
                return 1;  //OK
            }
        }
        return -1; //No Valid Command
    }
    return -2;  //No Valid Char
}

int buildin_unixcmd(char* cmd,void* args)
{
    char buf[128] = {0};
    if(NULL == cmd)
    {
        return -1;
    }

    strcat(buf,cmd);
    if(NULL != args)
    {
        strcat(buf," ");
        strcat(buf,(char*)args);
    }
    system(buf);

    return 0;
}

int buildin_showProgram(char* cmd,void* args)
{
    testProgramUI();

    return 0;
}

int buildin_exit(char* cmd,void* args)
{
    exit(0);
    return 0;
}

int buildin_helpDisplay(char* cmd,void* args)
{
    int num;
    if(NULL == cmd)
    {
        return -1;
    }

    commons_println("support command:");
    for(num = 0;num < sizeof(TabBuildInCmd) / sizeof(ST_BUILDIN_CMD);num ++)
    {
        commons_print("%s ",TabBuildInCmd[num].cmd);
    }
    commons_print("\n");
}

void testStringInputFix()
{
    int read_bytes;
    char buffer[128] = {0x31,0x32,0x33,0x33,0x08,0x08,0x34,0x35,0x00};

    read_bytes = strlen(buffer);

    LOG("fix before");
    commons_print_hex(buffer,strlen(buffer));

    strings_inputfix(buffer);

    LOG("fix after");
    commons_print_hex(buffer,strlen(buffer));
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

void testStringIssapce()
{
    char c;

    c = 0x32;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));

    c = 0x20;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));

    c = 0x09;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));

    c = 0x0a;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));

    c = 0x0b;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));

    c = 0x0c;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));

    c = 0x0d;
    commons_println("char is:%02X,result:%d",c,strings_isspace(c));
}

void testStringTolower()
{
    char c;

    c = 'C';
    commons_println("char is %c,result:%c",c,strings_tolower(c));
    return ;
}

void testStringAtof()
{
    char buffer[32] = {0};
    commons_println("pls input number");
    scanf("%s",buffer);
    commons_println("string:%s number:%f",buffer,strings_atof(buffer));

    char latitude[128] = {0};
    char longitude[128] = {0};
    char altitude[128] = {0};
    char speed[128] = {0};
    char accuracy[128] = {0};
    commons_println("pls input latitude;longitude;altitude;speed;accuracy");

    scanf("%s %s %s %s %s",
        latitude,
        longitude,
        altitude,
        speed,
        accuracy);

    commons_println("latitude: %s,longitude: %s,altitude: %s",
        latitude,longitude,altitude);

    return ;
}

void testStringIsprint()
{
    unsigned char c;

    c = 0x32;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));

    c = 0x20;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));

    c = 0x09;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));

    c = 0x8a;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));

    c = 0x4c;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));

    c = 0x2c;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));

    c = 0xdd;
    commons_println("char is:%02X %c,result:%d",c,c,strings_isprint(c));
}

void testStringSplit()
{
#if 0
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
#endif
    int ret;
    unsigned char buffer[128] = {0};
    int i = 0;
    char** res = NULL;
    commons_println("pls input latitude;longitude;altitude;speed;accuracy");

    ret = scanf("%s",buffer);
    if(ret <= 0)
    {
        commons_println("read failed");
        return -1;
    }
    commons_println("read:%s",buffer);

    res = strings_split((char*)buffer,';');
    if(NULL == res)
    {
        commons_println("parse failed");
        return -1;
    }

    commons_println("parse succ");
    while(NULL != res[i])
    {
        commons_println("res[%d] %s ",i,res[i]);
        i++;
    }
    commons_println("print over");
    free(res);

}

void testStringPathFix()
{
    char path[128] = {0};
    commons_println("pls input:");
    scanf("%s",path);
    commons_println("before:%s",path);
    strings_pathfix(path,sizeof(path));
    commons_println("after:%s",path);
}

void testStringStrlcpy()
{
    char buf[128];
    char dest[16];
    commons_println("pls input:");
    scanf("%s",buf);
    commons_println("%d",strings_strlcpy(dest,buf,sizeof(dest)));
    commons_println("%s",dest);
}

void testStringStrdup()
{
    char* str = "hello,world";
    char* ptr;
    ptr = strings_strdup(str);
    commons_println("%s",ptr);
    free(ptr);
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
    //analyze_log_client();
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

void thread_handler(void* args)
{
    LOG("thread handler");

    sleep(100);
    LOG("thread handler exit");
}

void testCreateThread()
{
    pthread_t pth_id;
    utils_thread_create_thread(&pth_id,thread_handler,NULL,10*1024);
    LOG("pid:%p",pth_id);
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
    //utils_thread_create_thread((void*)utils_network_create_client,NULL,10*1024);

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



