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
#ifndef __COMMONS_TEST_H__
#define __COMMONS_TEST_H__

typedef struct
{
    char dispName[64];
    int num;
    int (*fun)(void);
}ST_TEST_LIST;

typedef struct
{
    char* cmd;
    int (*func)(char* cmd,void* args);
}ST_BUILDIN_CMD;


extern void testProgramUI(void);
extern void testStringToHex();
extern void testCommonsScanf();
extern void testMempool();
extern void testCommonsRandStr();
extern void testFileSaveOverlap();
extern void testCrc8();
extern void testFileCopy();
extern void testReadXMl();
extern void testFilePackage();
extern void testCbcEncrypt();
extern void testAnalyzeLog();
extern void testOthers();
extern void testConvertTime();
extern void testThreadCond();
extern void testCreateServer();
extern void testCreateClient();
extern void testHexToString(void);
extern void testCurl();
extern void testEvent(void);
extern void testEventClient(void);
extern void testThreadPool();
extern void testUtilsSignal();
extern void testBinaryTree();
extern void testSemWait();
extern void testHuffmanTree();
extern void testDynamicMem();
extern void testPrintStaceFrame();
extern void testStringIsDigit();
extern void testStringSplit();
extern void testFileCopyFast();
extern void testCreateProcess();
extern void sigint_handler(int signum);
extern void testSignal();
extern void testCreateThread();
extern void testStringIssapce();
extern int buildin_unixcmd(char* cmd,void* args);
extern int buildin_helpDisplay(char* cmd,void* args);
extern int buildin_exit(char* cmd,void* args);
extern int buildin_showProgram(char* cmd,void* args);

#endif

