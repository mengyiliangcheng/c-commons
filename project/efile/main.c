/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/21  automatic
*/
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "efile.h"
#include "mg_aes.h"

#define LOG(format, ...) do { \
                                printf("%s:%s/%s|%d::%d::"format,"LOG",__FILE__,__func__,__LINE__,getpid(),##__VA_ARGS__); \
                                printf("\n"); \
                            }while(0)

#define MG_LOG_ERROR LOG

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

void* Fopen(const char *path, const char *mode)
{
    return fopen(path,mode);
}

int Fwrite(void *ptr, int size, int nmemb,void *stream)
{
    return fwrite(ptr,size,nmemb,stream);
}

int Fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fread(ptr,size,nmemb,stream);
}

int Fclose(void* stream)
{
    return fclose(stream);
}

int Fseek(void* stream,long offset,int whence)
{
    return fseek(stream,offset,whence);
}

long Ftell(void* stream)
{
    return ftell(stream);
}

int Fflush(void* stream)
{
    return fflush(stream);
}

void Sync(void)
{
    return sync();
}


bool Isprint(char c)
{
    return ((c > 0x20) && (c < 0x7e) ? true : false);
}

void Bzero(void* in,int size)
{
    if(NULL != in)
        memset(in,0,size);
}

int Unlink(const char* path)
{
    return unlink(path);
}

int Fileno(void* fp)
{
    return fileno(fp);
}

int Fchmod(int fd, int mode)
{
    return fchmod(fd,mode);
}

/****************************************************************
* Function: efile_print_hex
* Description: 以十六进制方式打印
* Param:
* Input:   const void* buf 打印的内容
           int len 打印的长度
* Output:
* Return:
* Others:
* Author: pengyicheng
****************************************************************/
static void efile_print_hex(const void* buf,int len)
{
    int i = 0;
    int offset = 0;
    int write_num;
    char logbuffer[EFILE_PRINT_HEX_CHUNK + 1];
    int logoffset = 0;
    unsigned char* src = (unsigned char*)buf;

    if(NULL == buf)
        return;
    
    BUILD_BUG_ON(EFILE_PRINT_HEX_CHUNK < (EFILE_PRINT_BTYES_PER_LINE*5 + 8));

    while(len > 0)
    {
        write_num = MIN(len,EFILE_PRINT_BTYES_PER_LINE);
        memset(logbuffer,0,sizeof(logbuffer));
        sprintf(logbuffer,"%07X ",offset * EFILE_PRINT_BTYES_PER_LINE);
        logoffset = 8;

        for(i = 0;i < write_num;i ++)
        {
            //printf("\n%02X \n",(unsigned int )src[i]);
            sprintf(&logbuffer[logoffset],"%02X ",(unsigned int )src[i]);
            logoffset += 3;
        }

        sprintf(&logbuffer[logoffset],"%c ",'|');
        logoffset += 2;

        for(i = 0;i < write_num;i ++)
        {
            if(Isprint(src[i]))
            {
                sprintf(&logbuffer[logoffset],"%c ",(unsigned char )src[i]);
            }else
            {
                sprintf(&logbuffer[logoffset],"%c ",'.');
            }
            
            logoffset += 2;
        }

        LOG("%s",logbuffer);
        offset ++;
        src += write_num;
        len -= write_num;
    }
}

/****************************************************************
* Function: mg_efile_get_key
* Description: 获取密钥
* Param:
* Input:   int type 加密类型
           unsigned char* initKey 初始密钥
           unsigned char* outKey  最终密钥
           int inLen  输入密钥buf大小
* Output:  unsigned char* outKey  输出密钥
* Return:  -1 失败
            >=0成功
* Others:
* Author: pengyicheng
****************************************************************/
static int mg_efile_get_key(int type,unsigned char* initKey,unsigned char* outKey,int inLen)
{
//    int ret = 0;
    int length = 0;
    if(NULL == initKey || NULL == outKey)
    {
        return EFILE_RET_PARAM_ERR;
    }
    if(inLen < EFILE_TDES_LEN)
    {
        return EFILE_RET_PARAM_ERR;
    }

    length = EFILE_TDES_LEN;

    memset(outKey,0,inLen);
    memcpy(outKey,initKey,length);

#if 0
    if(type == EFILE_HDR_TYPE2)
    {
        //ret = get_cpu_sn(outKey,length);
        if(ret < 0)
        {
            MG_LOG_ERROR("get sn failed");
            return -1;
        }
    }
#endif
    return 0;
}

/****************************************************************
* Function: efile_encrypt_data
* Description: 加密数据
* Param:
* Input:   void *in 输入的数据
           int inLen 输入的长度
           bool bisEncrypt TRUE：加密 FALSE：解密
           int type 加密类型
* Output:  void *out 输出的数据
* Return:  EFILE_RET_PARAM_ERR 失败，参数错误
           -1 失败，获取密钥失败
           0 成功
* Others:
* Author: pengyicheng
****************************************************************/
static int efile_encrypt_data(const void *in, int inLen, void *out,unsigned char* key,bool bisEncrypt)
{
    AES_KEY aes;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    if(NULL == in || NULL == out || NULL == key)
    {
        MG_LOG_ERROR("param is null");
        return EFILE_RET_PARAM_ERR;
    }

    if(bisEncrypt)
    {
        if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
        {
            MG_LOG_ERROR("set aes key failed");
            return -1;
        }
        AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, inLen, &aes, iv, AES_ENCRYPT);
    }
    else
    {
        if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
        {
            return -1;
        }
        AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, inLen, &aes, iv, AES_DECRYPT);
    }

    return 0;
}

int efile_exist(const char* file)
{
    void* fp = NULL;
    if(NULL == file)
    {
        LOG("file path is NULL error");
        return FALSE;
    }

    fp = Fopen(file,"r");
    if(NULL == fp)
    {
        //LOG("open file failed");
        return FALSE;
    }

    Fclose(fp);

    return TRUE;
}

/****************************************************************
* Function: efile_write
* Description: 加密数据，写入文件
* Param:
* Input:   st_efile* efile：efile对象
           const void *buf：写入的内容
           int len：写入的长度
* Output:
* Return:  EFILE_RET_PARAM_ERR 失败，参数错误
            -1 失败，获取密钥失败
            >=0 写入的字节数
* Others:
* Author: pengyicheng
****************************************************************/
static int efile_write(st_efile* efile, const void *buf, int len)
{
    char buffer_to_write[EFILE_MAX_PROCESS_CHUNK + 1] = {0};
    unsigned char key[128] = {0};
    int ret;

    if(NULL == efile || NULL == buf || len > EFILE_MAX_PROCESS_CHUNK)
    {
        MG_LOG_ERROR("param is not valid");
        return EFILE_RET_PARAM_ERR;
    }
    MG_LOG_ERROR("=============orgin data============");
    efile_print_hex(buf,len);
    MG_LOG_ERROR("=============orgin data============");

    MG_LOG_ERROR("=============print init key============");
    efile_print_hex(efile->init_key,efile->init_key_len);
    MG_LOG_ERROR("=============print init key============");

    /*get unique key*/
    ret = mg_efile_get_key(efile->hdr_type,efile->init_key,key,sizeof(key));
    if(ret < 0)
    {
        MG_LOG_ERROR("get key failed,ret:%d",ret);
        return -1;
    }
    MG_LOG_ERROR("=============print key============");
    efile_print_hex(key,sizeof(key));
    MG_LOG_ERROR("=============print key============");

    ret = efile_encrypt_data(buf,len,buffer_to_write,key,TRUE);
    if(ret < 0)
    {
        MG_LOG_ERROR("encrypt data error");
        return -1;
    }
    MG_LOG_ERROR("=============after============");
    efile_print_hex(buffer_to_write,len);
    MG_LOG_ERROR("=============after============");

    if(NULL != efile->fp)
    {
        ret = Fwrite(buffer_to_write,sizeof(char),len,efile->fp);
        if(ret != len)
        {
            MG_LOG_ERROR("efile_write bytes:%d,err:%s",ret,strerror(errno));
        }
        return ret;
    }
    return 0;
}

/****************************************************************
* Function: efile_read
* Description: 读取文件，解密数据
* Param:
* Input:   st_efile* efile：efile对象
           const void *buf：写入的内容
           int len：写入的长度
* Output:
* Return:  EFILE_RET_PARAM_ERR 失败，参数错误
           -1 失败，获取密钥失败
           >=0 读取的字节数
* Others:
* Author: pengyicheng
****************************************************************/
static int efile_read(st_efile* efile, void *buf, int len)
{
    char buffer_to_read[EFILE_MAX_PROCESS_CHUNK + 1] = {0};
    unsigned char key[128] = {0};
    int ret;
    int bytes_to_read = 0;

    if(NULL == efile || NULL == buf || len > EFILE_MAX_PROCESS_CHUNK)
    {
        MG_LOG_ERROR("param is not valid");
        return EFILE_RET_PARAM_ERR;
    }
    //MG_LOG_ERROR("raw efile_read");

    /*read from file*/
    if(NULL != efile->fp)
    {
        ret = Fread(buffer_to_read,sizeof(char),len,efile->fp);
        //MG_LOG_ERROR("efile_read bytes:%d Ftell:%ld",ret,Ftell(efile->fp));
        if(ret < 0)
        {
            MG_LOG_ERROR("read file failed,err:%s",strerror(errno));
            return EFILE_FILE_READ_ERR;
        }
        bytes_to_read = ret;
    }
    //efile_print_hex(buffer_to_read,len);

    /*get unique key*/
    ret = mg_efile_get_key(efile->hdr_type,efile->init_key,key,sizeof(key));
    if(ret < 0)
    {
        MG_LOG_ERROR("get key failed,ret:%d",ret);
        return -1;
    }

    //MG_LOG_ERROR("print key");
    //efile_print_hex(key,sizeof(key));

    /*decrypt data*/
    Bzero(buf,len);
    ret = efile_encrypt_data(buffer_to_read,bytes_to_read,buf,key,FALSE);
    if(ret < 0)
    {
        MG_LOG_ERROR("encrypt data error");
        return -1;
    }
    //MG_LOG_ERROR("decrypt efile_read after,read bytes:%d",bytes_to_read);
    //efile_print_hex(buf,bytes_to_read);

    return bytes_to_read;
}

/****************************************************************
* Function: efile_tell
* Description: 得到当前文件操作位置
* Param:
* Input:   st_efile* efile：efile对象
* Output:
* Return:  EFILE_RET_PARAM_ERR 失败，参数错误
            >=0 当前文件操作位置
* Others:
* Author: pengyicheng
****************************************************************/
static long efile_tell(st_efile* efile)
{
    if((NULL != efile) && (NULL != efile->fp))
    {
        return Ftell(efile->fp);
    }
    return EFILE_RET_PARAM_ERR;
}

/****************************************************************
* Function: efile_flush
* Description: 清空缓存区
* Param:
* Input:   st_efile* efile：efile对象
* Output:
* Return:  EFILE_RET_PARAM_ERR 失败，参数错误
           >=0 成功
* Others:
* Author: pengyicheng
****************************************************************/
static int efile_flush(st_efile* efile)
{
    if((NULL != efile) && (NULL != efile->fp))
    {
        return Fflush(efile->fp);
    }
    return EFILE_RET_PARAM_ERR;
}

/****************************************************************
* Function: efile_size
* Description: 计算文件大小
* Param:
* Input:   FILE* fp
* Output:
* Return:  0 失败，参数错误
            >=0 成功
* Others:
* Author: pengyicheng
****************************************************************/
static long efile_size(void* fp)
{
    long cur_pos = 0;
    long file_size = 0;
    if(NULL != fp)
    {
        cur_pos = Ftell(fp);
        if(Fseek(fp,0,SEEK_END) < 0)
        {
            MG_LOG_ERROR("Fseek failed,err:%s",strerror(errno));
            return EFILE_RET_SYS_ERR;
        }
        file_size = Ftell(fp);
        if(cur_pos >= 0)
        {
            if(Fseek(fp,cur_pos,SEEK_SET) < 0)
            {
                MG_LOG_ERROR("Fseek failed,err:%s",strerror(errno));
                return EFILE_RET_SYS_ERR;
            }
        }
        return file_size;
    }
    MG_LOG_ERROR("param is null");
    return 0;
}

/****************************************************************
* Function: crc8
* Description: 计算crc8
* Param:
* Input:  unsigned char crc 初始值
          const unsigned char* s 需要计算的数据
          int length 需要计算的长度
* Output:
* Return:  crc
* Others:
* Author: pengyicheng
****************************************************************/
static unsigned char crc8(unsigned char crc,const unsigned char* s,int length)
{
    int i;
    if(NULL == s)
    {
        MG_LOG_ERROR("param is null");
        return 0;
    }

    for(i = 0;i < length;i ++)
    {
        crc ^= s[i];
    }
    return crc;
}

/****************************************************************
* Function: efile_checksum
* Description: 计算checksum
* Param:
* Input:  st_efile* efile efile对象
            const void *buf 需要计算的数据
            int len 需要计算的长度
* Output:
* Return:  check sum
* Others:
* Author: pengyicheng
****************************************************************/
static unsigned int efile_checksum(st_efile* efile, const void *buf, int len)
{
    unsigned char crc;
    if(NULL == efile || NULL == buf)
    {
        return EFILE_RET_PARAM_ERR;
    }
    //efile_print_hex(buf,len);

    crc = (unsigned char)efile->check_sum;
    efile->check_sum = crc8(crc,buf,len);
    return efile->check_sum;
}

/****************************************************************
* Function: efile_update_hdr
* Description: 更新文件头
* Param:
* Input:  st_efile* efile efile对象
* Output:
* Return:  EFILE_RET_PARAM_ERR 参数错误
           EFILE_RET_OK 成功
* Others:
* Author: pengyicheng
****************************************************************/
static int efile_update_hdr(st_efile* efile)
{
    long current_pos = 0;
    u_efile_hdr efile_hdr;
    if(NULL == efile || NULL == efile->fp)
    {
        MG_LOG_ERROR("param is not valid");
        return EFILE_RET_PARAM_ERR;
    }

    MG_LOG_ERROR("checksum:%d",efile->check_sum);
    MG_LOG_ERROR("hdr_type:%d",efile->hdr_type);

    /*record current postion*/
    current_pos = Ftell(efile->fp);
    /*set to head of file*/
    if(Fseek(efile->fp,0,SEEK_SET) < 0)
    {
        MG_LOG_ERROR("Fseek failed,err:%s",strerror(errno));
        return EFILE_RET_SYS_ERR;
    }

    /*write header to file*/
    Bzero(&efile_hdr,sizeof(efile_hdr));
    efile_hdr.st_hdr.hdr_type = efile->hdr_type;
    efile_hdr.st_hdr.checksum = efile->check_sum;
    Fwrite(&efile_hdr,sizeof(efile_hdr),1,efile->fp);

    /*return to current postion*/
    if(current_pos >= 0)
    {
        if(Fseek(efile->fp,current_pos,SEEK_SET) < 0)
        {
            MG_LOG_ERROR("Fseek failed,err:%s",strerror(errno));
            return EFILE_RET_SYS_ERR;
        }
    }

    LOG("update file head succ");
    return EFILE_RET_OK;
}


/****************************************************************
* Function: mg_efile_new
* Description: 创建一个efile对象
* Param:
* Input:
* Output:
* Return: st_efile* efile efile对象
* Others:
* Author: pengyicheng
****************************************************************/
st_efile* mg_efile_new(void)
{
    int i = 0;
    st_efile* efile = NULL;
    static const char _aes_key1[] = {0x38,0x39,0x64,0x30,0x30,0x33,0x38,0x30,0x61,0x33,0x66,0x31,0x31,0x66,0x31,0x61};
    static const char _aes_key2[] = {0x66,0x36,0x32,0x30,0x31,0x32,0x36,0x39,0x37,0x62,0x33,0x37,0x62,0x39,0x62,0x39};

    efile = malloc(sizeof(st_efile));
    if(NULL == efile)
    {
        MG_LOG_ERROR("malloc failed,err:%s",strerror(errno));
        return NULL;
    }
    memset(efile,0,sizeof(st_efile));

    /*init params*/
    efile->fp = NULL;
    efile->hdr_type = EFILE_CURR_HDR_TYPE;
    efile->hdr_length = EFILE_HDR_LEN;
    efile->max_process_chunk = EFILE_MAX_PROCESS_CHUNK;
    efile->write = efile_write;
    efile->read = efile_read;
    efile->tell = efile_tell;
    efile->flush = efile_flush;
    efile->processed_bytes = 0;
    efile->update_cksum = efile_checksum;
    efile->check_sum = 0;
    efile->rwmode =  EFILE_NONE_MODE;
    efile->perm = 0;

    BUILD_BUG_ON(sizeof(_aes_key1) > sizeof(efile->init_key));

    for(i = 0;i < sizeof(_aes_key1);i ++)
    {
        efile->init_key[i] = _aes_key1[i] ^ _aes_key2[i];
    }
    efile->init_key_len = sizeof(_aes_key1);

    return efile;
}

/****************************************************************
* Function: mg_efile_setkey
* Description: 设置初始化密钥
* Param:
* Input:  st_efile* efile efile对象
          unsigned char* initKey 初始化密钥
          int length  密钥长度
* Output:
* Return: EFILE_RET_PARAM_ERR 参数错误
          EFILE_RET_OK 成功
* Others:
* Author: pengyicheng
****************************************************************/
int mg_efile_setkey(st_efile* efile,const unsigned char* initKey1,const unsigned char* initKey2,int length)
{
    int i = 0;
    char key[128] = {0};
    if(NULL == initKey1 || NULL == efile || NULL == initKey2)
    {
        return EFILE_RET_PARAM_ERR;
    }

    if(length < EFILE_TDES_LEN || length > sizeof(efile->init_key))
    {
        return EFILE_RET_PARAM_ERR;
    }

    for(i = 0;i < length;i ++)
    {
        key[i] = initKey1[i] ^ initKey2[i];
    }

    memcpy(efile->init_key,key,length);
    efile->init_key_len = length;

    MG_LOG_ERROR("=============print init key============");
    efile_print_hex(efile->init_key,efile->init_key_len);
    MG_LOG_ERROR("=============print init key============");

    return EFILE_RET_OK;
}

/****************************************************************
* Function: mg_efile_free
* Description: 释放一个efile对象
* Param:
* Input:  st_efile* efile efile对象
* Output:
* Return:
* Others:
* Author: pengyicheng
****************************************************************/
void mg_efile_free(st_efile* efile)
{
    if(NULL == efile)
        return;

    if(NULL != efile->fp)
    {
        Fclose(efile->fp);
        efile->fp = NULL;
    }
    free(efile);
}

/****************************************************************
* Function: mg_efile_open
* Description: 打开一个文件
* Param:
* Input:  st_efile* efile efile对象
          const char* file 文件名
          e_efile_rwmode rwmode 读写模式
* Output:
* Return:   EFILE_RET_PARAM_ERR 参数错误
            EFILE_FILE_READ_ERR 读取文件失败
            MG_ERR_FILE_OPEN_FAILED 打开文件失败
            EFILE_RET_OK 成功
* Others:
* Author: pengyicheng
****************************************************************/
int mg_efile_open(st_efile* efile,const char* file,e_efile_rwmode rwmode)
{
    #if 1
    void* fp = NULL;
    u_efile_hdr file_hdr;

    if((NULL == efile) || (NULL == file))
    {
        return EFILE_RET_PARAM_ERR;
    }
    efile->rwmode = rwmode;

    if(EFILE_READ_MODE == efile->rwmode)
    {
        fp = Fopen(file,"rb");
        if(NULL == fp)
        {
            MG_LOG_ERROR("Fopen failed,err:%s",strerror(errno));
            return EFILE_FILE_OPEN_ERR;
        }
        if(Fread(&file_hdr,1,sizeof(file_hdr),fp) < sizeof(file_hdr))
        {
            Fclose(fp);
            MG_LOG_ERROR("read file header failed,err:%s",strerror(errno));
            return EFILE_FILE_READ_ERR;
        }
        if(file_hdr.st_hdr.hdr_type <= EFILE_HDR_TYPE_MIN
            || file_hdr.st_hdr.hdr_type > EFILE_HDR_TYPE_MAX)
        {
            MG_LOG_ERROR("invalid file header");
            Fclose(fp);
            return EFILE_FILE_READ_ERR;
        }
        efile->hdr_type = file_hdr.st_hdr.hdr_type;
        efile->hdr_checksum = file_hdr.st_hdr.checksum;
        efile->file_size = efile_size(fp);
        MG_LOG_ERROR("efile->file_size:%d",efile->file_size);
        MG_LOG_ERROR("efile->hdr_checksum:%d",efile->hdr_checksum);
        Fseek(fp,efile->hdr_length,SEEK_SET);
    }
    else if(EFILE_OVERWRITE_MODE == efile->rwmode)
    {
        Unlink(file);
        fp = Fopen(file,"wb+");
        if(NULL == fp)
        {
            MG_LOG_ERROR("Fopen failed,err:%s",strerror(errno));
            return EFILE_FILE_OPEN_ERR;
        }
        if(0 != efile->perm)
        {
            int fd = Fileno(fp);
            Fchmod(fd,efile->perm);
        }
        efile->fp = fp;
        efile_update_hdr(efile);
    }
    else if(EFILE_APPEND_MODE == efile->rwmode)
    {
        if(!efile_exist(file)) /* create file if file not exist */
        {
            fp = Fopen(file,"ab+");
            if(NULL == fp)
            {
                MG_LOG_ERROR("Fopen %s failed,err:%s",file,strerror(errno));
                return EFILE_FILE_OPEN_ERR;
            }
            efile->fp = fp;

            efile_update_hdr(efile);
        }
        else    /* read file head if file exist */
        {
            fp = Fopen(file,"ab+");
            if(NULL == fp)
            {
                MG_LOG_ERROR("Fopen %s failed,err:%s",file,strerror(errno));
                return EFILE_FILE_OPEN_ERR;
            }
        }

        Fseek(fp,0,SEEK_SET);
        if(Fread(&file_hdr,1,sizeof(file_hdr),fp) < sizeof(file_hdr))
        {
            Fclose(fp);
            MG_LOG_ERROR("read file header failed,err:%s",strerror(errno));
            return EFILE_FILE_READ_ERR;
        }
        if(file_hdr.st_hdr.hdr_type <= EFILE_HDR_TYPE_MIN
            || file_hdr.st_hdr.hdr_type > EFILE_HDR_TYPE_MAX)
        {
            MG_LOG_ERROR("invalid file header");
            Fclose(fp);
            return EFILE_FILE_READ_ERR;
        }
        efile->hdr_type = file_hdr.st_hdr.hdr_type;
        efile->hdr_checksum = file_hdr.st_hdr.checksum;
        MG_LOG_ERROR("efile->file_size:%d",efile->file_size);
        MG_LOG_ERROR("efile->hdr_checksum:%d",efile->hdr_checksum);

        Fseek(fp,0,SEEK_END);
    }

    efile->fp = fp;
#endif
    return EFILE_RET_OK;
}

/****************************************************************
* Function: mg_efile_close
* Description: 关闭一个文件
* Param:
* Input:  st_efile* efile efile对象
* Output:
* Return:   EFILE_RET_PARAM_ERR 参数错误
            EFILE_FILE_READ_ERR 读取文件失败
            MG_ERR_FILE_OPEN_FAILED 打开文件失败
            EFILE_RET_OK 成功
* Others:
* Author: pengyicheng
****************************************************************/
int mg_efile_close(st_efile* efile)
{
    if(NULL == efile || NULL == efile->fp)
    {
        MG_LOG_ERROR("param is not valid");
        return EFILE_RET_PARAM_ERR;
    }

    if(EFILE_OVERWRITE_MODE == efile->rwmode
    && EFILE_APPEND_MODE == efile->rwmode)
    {
        /*update file header*/
        efile_update_hdr(efile);

        /*flush to disk*/
        if(efile->flush)
        {
            efile->flush(efile);
        }
        Sync();
    }

    Fclose(efile->fp);
    efile->fp = NULL;
    efile->check_sum = 0;
    efile->processed_bytes = 0;
    efile->rwmode = EFILE_NONE_MODE;
    efile->perm = 0;
    return EFILE_RET_OK;
}

/****************************************************************
* Function: mg_efile_write
* Description: 将数据写入文件
* Param:
* Input:    st_efile* efile  efile对象
            int len 写入的长度
* Output:   const void* buf  需要写入的缓存
* Return:   EFILE_RET_PARAM_ERR 参数错误
            EFILE_RET_SYS_ERR 写入错误
            EFILE_RET_OK 成功
* Others:
* Author: pengyicheng
****************************************************************/
int mg_efile_write(st_efile* efile,const void* buf,int len)
{
    int ret;
    char* ptr_buf = NULL;
    int bytes_to_write = 0;

    if(NULL == buf || len <= 0 || NULL == efile)
    {
        MG_LOG_ERROR("param is not valid");
        return EFILE_RET_PARAM_ERR;
    }
    if(EFILE_OVERWRITE_MODE != efile->rwmode
        && EFILE_APPEND_MODE != efile->rwmode)
    {
        MG_LOG_ERROR("rwmode error:%d",efile->rwmode);
        return EFILE_RET_PARAM_ERR;
    }
    if(0 != (len % 16))
    {
        MG_LOG_ERROR("data length is not correct,len:%d",len);
        return EFILE_RET_PARAM_ERR;
    }

    ptr_buf = (char*)buf;
    if(NULL == efile->write)
    {
        MG_LOG_ERROR("no write function");
        return EFILE_RET_PARAM_ERR;
    }
    if(NULL == efile->fp)
    {
        MG_LOG_ERROR("no file pointer,pls open first");
        return EFILE_RET_PARAM_ERR;
    }

    if(efile->rwmode == EFILE_APPEND_MODE)
    {
        Fseek(efile->fp,0,SEEK_END);
    }
    else
    {
        if(Ftell(efile->fp) < efile->hdr_length)
        {
            if(Fseek(efile->fp,efile->hdr_length,SEEK_SET) < 0)
            {
                MG_LOG_ERROR("Fseek failed,err:%s",strerror(errno));
                return EFILE_FILE_FORMAT_ERR;
            }
        }
    }

    /*write to file*/
    while(len > 0)
    {
        bytes_to_write = (efile->max_process_chunk < len) ? efile->max_process_chunk : len;

        if(efile->update_cksum)
        {
            efile->update_cksum(efile,ptr_buf,bytes_to_write);
        }
        ret = efile->write(efile,ptr_buf,bytes_to_write);
        if(ret < 0 || ret != bytes_to_write)
        {
            MG_LOG_ERROR("write to file failed,ret:%d",ret);
            return EFILE_RET_SYS_ERR;
        }
        len -= bytes_to_write;
        buf = (char*)buf + bytes_to_write;
        efile->processed_bytes += bytes_to_write;
    }
    return EFILE_RET_OK;
}

/****************************************************************
* Function: mg_efile_read
* Description: 从文件中读取
* Param:
* Input:    st_efile* efile  efile对象
            int len 读取的最大长度
* Output:   const void* buf  需要读取的缓存
* Return:   EFILE_RET_PARAM_ERR 参数错误
            EFILE_RET_SYS_ERR 读取错误
            MG_ERR_FILE_FORMAT_ERROR 格式错误
            EFILE_RET_OK 成功
* Others:
* Author: pengyicheng
****************************************************************/
int mg_efile_read(st_efile* efile,const void* buf,int len)
{
    int bytes_to_read = 0;
    char* ptr_buf = NULL;
    int ret;
    int file_size = 0;
    int processed_bytes = 0;

    if(NULL == efile || NULL == buf)
    {
        MG_LOG_ERROR("param is not valid");
        return EFILE_RET_PARAM_ERR;
    }

    if(EFILE_READ_MODE != efile->rwmode)
    {
        MG_LOG_ERROR("rwmode error:%d",efile->rwmode);
        return EFILE_RET_PARAM_ERR;
    }

    ptr_buf = (char*)buf;
    if(NULL == efile->read)
    {
        MG_LOG_ERROR("no read function");
        return EFILE_RET_PARAM_ERR;
    }
    if(NULL == efile->fp)
    {
        MG_LOG_ERROR("no file pointer,pls open first");
        return EFILE_RET_PARAM_ERR;
    }

    //获取可读的长度
    file_size = efile->file_size - Ftell(efile->fp);
    //MG_LOG_ERROR("file_size:%d efile->file_size:%d Ftell(efile->fp):%ld",file_size,efile->file_size,Ftell(efile->fp));
    len = MIN((int)file_size,len);

    /*read from file*/
    while(len > 0)
    {
        bytes_to_read = MIN(efile->max_process_chunk,len);
        ret = efile->read(efile,ptr_buf,bytes_to_read);
        if(ret < 0 || ret != bytes_to_read)
        {
            if(!feof(efile->fp))
            {
                MG_LOG_ERROR("read from file failed,ret:%d",ret);
                return EFILE_RET_SYS_ERR;
            }
        }
        if(efile->update_cksum)
        {
            efile->update_cksum(efile,ptr_buf,bytes_to_read);
        }

        len -= bytes_to_read;
        //MG_LOG_ERROR("read :");
        //efile_print_hex(buf,bytes_to_read);
        ptr_buf = ptr_buf + bytes_to_read;
        efile->processed_bytes += bytes_to_read;
        processed_bytes += bytes_to_read;
        
        //MG_LOG_ERROR("read bytes:%d len:%d",bytes_to_read,len);
        if(feof(efile->fp))
        {
            LOG("end of file");
            //break;
        }
    }

    //MG_LOG_ERROR("checksum:%d",efile->check_sum);
    //MG_LOG_ERROR("hdr_type:%d",efile->hdr_type);
    MG_LOG_ERROR("read bytes:%d",processed_bytes);
    return processed_bytes;
}

/****************************************************************
* Function: mg_efile_check
* Description: 校验文件完整性，需要在读取完文件后调用
* Param:
* Input:    st_efile* efile  efile对象
* Output:
* Return:   TRUE：文件完整
            FALSE:文件不完整
* Others:
* Author: pengyicheng
****************************************************************/
bool mg_efile_check(st_efile* efile)
{
    if(NULL == efile)
    {
        return FALSE;
    }

    return (efile->check_sum == efile->hdr_checksum) ? TRUE : FALSE;
}

int main()
{
    char buf[128] = {0};
    char read_buf[1024];
    int ret;
    st_efile* efile = NULL;

    int i;
    for(i = 0;i < 128;i ++)
    {
        buf[i] = i;
    }

    efile = mg_efile_new();
    if(NULL == efile)
    {
        LOG("create efile handle failed");
        return -1;
    }
#if 1  /* write file */
    ret = mg_efile_open(efile,"./data",EFILE_OVERWRITE_MODE);
    if(ret < 0)
    {
        LOG("open efile failed");
        return -1;
    }

    ret = mg_efile_write(efile,buf,sizeof(buf));
    if(ret < 0)
    {
        LOG("write to file failed");
        return -1;
    }

    mg_efile_close(efile);
#endif

    /* read file */
    ret = mg_efile_open(efile,"./data",EFILE_READ_MODE);
    if(ret < 0)
    {
        LOG("open efile failed");
        mg_efile_free(efile);
        return -1;
    }
    

    for(;;)
    {
        ret = mg_efile_read(efile,read_buf,128);
        if(ret < 0)
        {
            LOG("read from file failed");
            mg_efile_free(efile);
            return -1;
        }
        if(ret == 0)
        {
            LOG("end of file");
            break;
        }
        efile_print_hex(read_buf,ret);
    }

    mg_efile_close(efile);

    mg_efile_free(efile);

    printf("hello world\n");
    return 0;
}


