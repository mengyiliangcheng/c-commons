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
        COMMONS_MAIN_LOG("malloc failed");
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
    COMMONS_MAIN_LOG("save file:%d",ret);
}

int testCrc8()
{
    char buf[] = {1,2,3};
    u8 crc;
    crc = utils_cal_crc_8(buf,sizeof(buf));
    COMMONS_MAIN_LOG("crc:%d",crc);
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


#if 1
#include <openssl/aes.h>

int mg_aes_cbc_encrypt(void *in, size_t inLen, char *key, void *out)
{
    AES_KEY aes;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    if(!in || !inLen || !key || !out)
    {
        return -1;
    }

    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return -1;
    }

    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, inLen, &aes, iv, AES_ENCRYPT);

    return 0;
}

int mg_aes_cbc_decrypt(void *in, size_t inLen, char *key, void *out)
{
    AES_KEY aes;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    if(!in || !key || !out)
    {
        return -1;
    }

    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return -1;
    }

    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, inLen, &aes, iv, AES_DECRYPT);

    return 0;
}
#endif


int testOthers1()
{
    char tmp[4096] = {0};
    int i;
    file_read("./fence.conf",tmp,sizeof(tmp));
    commons_print_hex_no_space(tmp,1392);
    char fence_aes_key1[]={ 0x08,0x09,0x0b,0x00,0x00,0x03,0x08,0x00,0x0a,0x03, \
                                            0x0f,0x01,0x01,0x0f,0x01,0x0a,0x0f,0x06,0x02,0x00, \
                                            0x01,0x02,0x06,0x09,0x07,0x0b,0x03,0x07,0x0b,0x09, \
                                            0x0b,0x09};

    char fence_aes_key2[]={ 0x05,0x0d,0x03,0x0a,0x07,0x08,0x08,0x00,0x0b,0x0e, \
                                            0x0c,0x01,0x05,0x02,0x00,0x0b,0x05,0x0d,0x07,0x04, \
                                            0x06,0x03,0x08,0x09,0x0a,0x00,0x0b,0x03,0x01,0x09, \
                                            0x00,0x04};
    char fence_aes_key[128]={0};
    for(i=0;i<sizeof(fence_aes_key1);i++)
    {
        fence_aes_key[i]=(fence_aes_key1[i]) ^ (fence_aes_key2[i]);
    }
    commons_print_hex_no_space(fence_aes_key,32);

    
}



