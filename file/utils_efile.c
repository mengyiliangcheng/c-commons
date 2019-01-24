/**
* @file     mg_encrypt_file.c
* @brief    文件加密API
* @author   pengyicheng
* @version  1.0
* @date     2019-01-08
**/
#if 0
/**
* @file     mg_encrypt_file.c
* @brief    文件加密API
* @author   pengyicheng
* @version  1.0
* @date     2019-01-08
**/
/*===========================================================================

                          EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who           what, where, why
--------  ------------   -----------------------------------------------------------------
19/01/08   pengyicheng    create file Xi'an 文件加密API
===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <openssl/aes.h>
#include "mg_aes.h"
#include "mg_error.h"
#include "mg_type.h"
#include "mg_efile.h"
#include "mg_syslog.h"

#define MG_LOG_DEBUG(...)   MG_SYSLOG_DEBUG("EFILE", __VA_ARGS__)

#define MG_LOG_INFO(...)    MG_SYSLOG_INFO("EFILE", __VA_ARGS__)

#define MG_LOG_WARNING(...) MG_SYSLOG_WARNING("EFILE", __VA_ARGS__)

#define MG_LOG_ERROR(...)   MG_SYSLOG_ERR("EFILE", __VA_ARGS__)


/**
    函数名:    get_cpu_sn <获得设备CPU的SN号>
    参数:     char* outData       输出SN号
            unsigned int uiLen  输出的最大长度
    返回值: -1 失败
            >0 获取到的SN长度
*/
static int get_cpu_sn(char* outData,unsigned int uiLen)
{
    static char tmp[32] = {0};
    static boolean flag = FALSE;
    int fd = 0;
    int ret;

    if(NULL == outData)
    {
        MG_LOG_ERROR("param error");
        return -1;
    }

    if(flag)
    {
        strncpy(outData,tmp,MIN(strlen(tmp),uiLen));
        return strlen(outData);
    }

    fd = open(MG_CPU_SN_PATH,O_RDONLY,0400);
    if(fd < 0)
    {
        MG_LOG_ERROR("open %s failed",MG_CPU_SN_PATH);
        return -1;
    }
    ret = read(fd,tmp,sizeof(tmp));
    if(ret < 0)
    {
        close(fd);
        MG_LOG_ERROR("read %s failed",MG_CPU_SN_PATH);
        return -1;
    }

    //LOG_ERROR("cpu sn:%s",tmp);
    close(fd);
    flag = TRUE;

    strncpy(outData,tmp,MIN(strlen(tmp),uiLen));
    return strlen(outData);
}

/**
    函数名:    efile_print_hex <已十六进制方式打印>
    参数:     const void* buf 打印的内容
            int len 打印的长度
    返回值:
*/
static void efile_print_hex(const void* buf,int len)
{
    int bytes_to_print = 0;
    char* ptr_buf = NULL;
    char tmp[MG_EFILE_PRINT_HEX_CHUNK * 3 + 1];
    int i = 0;

    if(NULL == buf)
        return;

    ptr_buf = (char*)buf;
    while(len > 0)
    {
        bytes_to_print = MIN(MG_EFILE_PRINT_HEX_CHUNK,len);
        bzero(tmp,sizeof(tmp));
        for(i = 0;i < bytes_to_print;i ++)
        {
            sprintf(&tmp[i*3],"%02X ",ptr_buf[i]);
        }
        MG_LOG_ERROR("%s",tmp);
        len -= bytes_to_print;
        ptr_buf = ptr_buf + bytes_to_print;
    }
}

/**
    函数名:    mg_efile_get_key <根据加密类型，获取秘钥>
    参数:     int type 加密类型
            unsigned char* outKey  输出密钥
            int inLen  密钥最大长度
    返回值: -1 失败
            >=0成功
*/
static int mg_efile_get_key(int type,unsigned char* initKey,unsigned char* outKey,int inLen)
{
    int ret = 0;
    int length = 0;
    if(NULL == initKey || NULL == outKey)
    {
        return MG_ERR_PARAMETER_ERROR;
    }
    if(inLen < MG_EFILE_TDES_LEN)
    {
        return MG_ERR_PARAMETER_ERROR;
    }

    length = MG_EFILE_TDES_LEN;

    memset(outKey,0,inLen);
    memcpy(outKey,initKey,length);

    if(type == MG_EFILE_HDR_TYPE2)
    {
        ret = get_cpu_sn(outKey,length);
        if(ret < 0)
        {
            MG_LOG_ERROR("get sn failed");
            return -1;
        }
    }
    return 0;
}

/**
    函数名:    efile_encrypt_data <加密数据>
    参数:     void *in 输入的数据
            int inLen 输入的长度
            void *out 输出的数据
            boolean bisEncrypt TRUE：加密 FALSE：解密
            int type 加密类型
    返回值: MG_ERR_PARAMETER_ERROR 失败，参数错误
            -1 失败，获取密钥失败
            0 成功
*/
static int efile_encrypt_data(void *in, int inLen, void *out,unsigned char* key,boolean bisEncrypt)
{
    //char key[128] = {0};
    int i = 0,ret;
    AES_KEY aes;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    if(NULL == in || NULL == out || NULL == key)
    {
        MG_LOG_ERROR("param is null");
        return MG_ERR_PARAMETER_ERROR;
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

/**
    函数名:    efile_write <加密数据，写入文件>
    参数:     st_efile* efile：efile对象
            const void *buf：写入的内容
            int len：写入的长度
    返回值: MG_ERR_PARAMETER_ERROR 失败，参数错误
            -1 失败，获取密钥失败
            >=0 写入的字节数
*/
static int efile_write(st_efile* efile, const void *buf, int len)
{
    char buffer_to_write[MG_EFILE_MAX_PROCESS_CHUNK + 1] = {0};
    char key[128] = {0};
    int ret;

    if(NULL == efile || NULL == buf || len > MG_EFILE_MAX_PROCESS_CHUNK)
    {
        MG_LOG_ERROR("param is not valid");
        return MG_ERR_PARAMETER_ERROR;
    }
    //MG_LOG_ERROR("efile_write");
    //efile_print_hex(buf,len);

    /*get unique key*/
    ret = mg_efile_get_key(efile->hdr_type,efile->init_key,key,sizeof(key));
    if(ret < 0)
    {
        MG_LOG_ERROR("get key failed,ret:%d",ret);
        return -1;
    }

    //MG_LOG_ERROR("print key");
    //efile_print_hex(key,ret);

    ret = efile_encrypt_data(buf,len,buffer_to_write,key,TRUE);
    if(ret < 0)
    {
        MG_LOG_ERROR("encrypt data error");
        return -1;
    }
    //MG_LOG_ERROR("efile_write after:");
    //efile_print_hex(buffer_to_write,len);

    if(NULL != efile->fp)
    {
        ret = fwrite(buffer_to_write,sizeof(char),len,efile->fp);
        MG_LOG_ERROR("efile_write bytes:%d",ret);
        return ret;
    }
    return 0;
}

/**
    函数名:    efile_read <读取文件，解密数据>
    参数:     st_efile* efile：efile对象
            const void *buf：写入的内容
            int len：写入的长度
    返回值: MG_ERR_PARAMETER_ERROR 失败，参数错误
            -1 失败，获取密钥失败
            >=0 读取的字节数
*/
static int efile_read(st_efile* efile, const void *buf, int len)
{
    char buffer_to_read[MG_EFILE_MAX_PROCESS_CHUNK + 1] = {0};
    char key[128] = {0};
    int ret;
    int bytes_to_read = 0;

    if(NULL == efile || NULL == buf || len > MG_EFILE_MAX_PROCESS_CHUNK)
    {
        MG_LOG_ERROR("param is not valid");
        return MG_ERR_PARAMETER_ERROR;
    }
    MG_LOG_ERROR("efile_read");

    /*read from file*/
    if(NULL != efile->fp)
    {
        ret = fread(buffer_to_read,sizeof(char),len,efile->fp);
        MG_LOG_ERROR("efile_read bytes:%d",ret);
        if(ret < 0)
        {
            MG_LOG_ERROR("read file failed,err:%s",strerror(errno));
            return MG_ERR_FILE_READ_FAILED;
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
    //efile_print_hex(key,ret);

    /*decrypt data*/
    bzero(buf,len);
    ret = efile_encrypt_data(buffer_to_read,bytes_to_read,buf,key,FALSE);
    if(ret < 0)
    {
        MG_LOG_ERROR("encrypt data error");
        return -1;
    }
    //MG_LOG_ERROR("efile_read after,read bytes:%d",bytes_to_read);
    //efile_print_hex(buf,bytes_to_read);

    return bytes_to_read;
}

/**
    函数名:    efile_tell <得到当前文件操作位置>
    参数:     st_efile* efile：efile对象
    返回值: MG_ERR_PARAMETER_ERROR 失败，参数错误
            >=0 当前文件操作位置
*/
static int efile_tell(st_efile* efile)
{
    if((NULL != efile) && (NULL != efile->fp))
    {
        return ftell(efile->fp);
    }
    return MG_ERR_PARAMETER_ERROR;
}

/**
    函数名:    efile_flush <清空缓存区>
    参数:     st_efile* efile：efile对象
    返回值: MG_ERR_PARAMETER_ERROR 失败，参数错误
            >=0 成功
*/
static int efile_flush(st_efile* efile)
{
    if((NULL != efile) && (NULL != efile->fp))
    {
        return fflush(efile->fp);
    }
    return MG_ERR_PARAMETER_ERROR;
}

/**
    函数名:    efile_size <计算文件大小>
    参数:     FILE* fp
    返回值: 0 失败，参数错误
            >=0 成功
*/
static long efile_size(FILE* fp)
{
    long cur_pos = 0;
    long file_size = 0;
    if(NULL != fp)
    {
        cur_pos = ftell(fp);
        fseek(fp,0,SEEK_END);
        file_size = ftell(fp);
        fseek(fp,cur_pos,SEEK_SET);

        return file_size;
    }
    MG_LOG_ERROR("param is null");
    return 0;
}

/**
    函数名:    crc8 <计算crc8>
    参数:     unsigned char crc 初始值
            const unsigned char* s 需要计算的数据
            int length 需要计算的长度
    返回值:
*/
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

/**
    函数名:    efile_checksum <计算checksum>
    参数:     st_efile* efile efile对象
            const void *buf 需要计算的数据
            int len 需要计算的长度
    返回值:
*/
static unsigned int efile_checksum(st_efile* efile, const void *buf, int len)
{
    if(NULL == efile || NULL == buf)
    {
        return MG_ERR_PARAMETER_ERROR;
    }
    //efile_print_hex(buf,len);

    unsigned char crc = (unsigned char)efile->check_sum;
    efile->check_sum = crc8(crc,buf,len);
    return efile->check_sum;
}

/**
    函数名:    efile_update_hdr <更新文件头>
    参数:     st_efile* efile efile对象
    返回值:
*/
static int efile_update_hdr(st_efile* efile)
{
    long current_pos = 0;
    u_efile_hdr efile_hdr;
    if(NULL == efile || NULL == efile->fp)
    {
        MG_LOG_ERROR("param is not valid");
        return MG_ERR_PARAMETER_ERROR;
    }

    MG_LOG_ERROR("checksum:%d",efile->check_sum);
    MG_LOG_ERROR("hdr_type:%d",efile->hdr_type);

    /*record current postion*/
    current_pos = ftell(efile->fp);
    /*set to head of file*/
    fseek(efile->fp,0,SEEK_SET);

    /*write header to file*/
    bzero(&efile_hdr,sizeof(efile_hdr));
    efile_hdr.st_hdr.hdr_type = efile->hdr_type;
    efile_hdr.st_hdr.checksum = efile->check_sum;
    fwrite(&efile_hdr,sizeof(efile_hdr),1,efile->fp);

    /*return to current postion*/
    fseek(efile->fp,current_pos,SEEK_SET);

    return MG_NO_ERR;
}

/**
    函数名:    mg_efile_new <创建一个efile对象>
    参数:
    返回值:    st_efile* efile efile对象
*/
st_efile* mg_efile_new(void)
{
    st_efile* efile = NULL;

    efile = malloc(sizeof(st_efile));
    if(NULL == efile)
    {
        MG_LOG_ERROR("malloc failed,err:%s",strerror(errno));
        return NULL;
    }
    memset(efile,0,sizeof(st_efile));

    /*init params*/
    efile->fp = NULL;
    efile->hdr_type = MG_EFILE_CURR_HDR_TYPE;
    efile->hdr_length = MG_EFILE_HDR_LEN;
    efile->max_process_chunk = MG_EFILE_MAX_PROCESS_CHUNK;
    efile->write = efile_write;
    efile->read = efile_read;
    efile->tell = efile_tell;
    efile->flush = efile_flush;
    efile->processed_bytes = 0;
    efile->update_cksum = efile_checksum;
    efile->check_sum = 0;
    efile->rwmode = MG_EFILE_NONE_MODE;
    efile->perm = S_IRUSR|S_IWUSR;

    return efile;
}
/**
    函数名:    mg_efile_setkey <设置初始化密钥>
    参数:   st_efile* efile efile对象
          unsigned char* initKey 初始化密钥
          int length  密钥长度
    返回值:MG_ERR_PARAMETER_ERROR 参数错误
           MG_NO_ERR 成功
*/
int mg_efile_setkey(st_efile* efile,unsigned char* initKey1,unsigned char* initKey2,int length)
{
    int i = 0;
    char key[128] = {0};
    if(NULL == initKey1 || NULL == efile || NULL == initKey2)
    {
        return MG_ERR_PARAMETER_ERROR;
    }

    if(length < MG_EFILE_TDES_LEN || length > sizeof(efile->init_key))
    {
        return MG_ERR_PARAMETER_ERROR;
    }

    for(i = 0;i < length;i ++)
    {
        key[i] = initKey1[i] ^ initKey2[i];
    }

    memcpy(efile->init_key,key,length);
    efile->init_key_len = length;
    return MG_NO_ERR;
}

/**
    函数名:    mg_efile_free <释放一个efile对象>
    参数:   st_efile* efile
    返回值:
*/
void mg_efile_free(st_efile* efile)
{
    if(NULL == efile)
        return;

    if(NULL != efile->fp)
    {
        fclose(efile->fp);
        efile->fp = NULL;
    }
    free(efile);
}

/**
    函数名:    mg_efile_open <打开一个文件>
    参数:   st_efile* efile efile对象
          const char* file 文件名
          e_efile_rwmode rwmode 读写模式
    返回值: MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_FILE_READ_FAILED 读取文件失败
            MG_ERR_FILE_OPEN_FAILED 打开文件失败
            MG_NO_ERR 成功
*/
int mg_efile_open(st_efile* efile,const char* file,e_efile_rwmode rwmode)
{
    FILE* fp = NULL;
    u_efile_hdr file_hdr;

    if((NULL == efile) || (NULL == file))
    {
        return MG_ERR_PARAMETER_ERROR;
    }
    efile->rwmode = rwmode;

    if(MG_EFILE_READ_MODE == efile->rwmode)
    {
        fp = fopen(file,"rb");
        if(NULL == fp)
        {
            MG_LOG_ERROR("fopen failed,err:%s",strerror(errno));
            return MG_ERR_FILE_OPEN_FAILED;
        }
        if(fread(&file_hdr,1,sizeof(file_hdr),fp) < sizeof(file_hdr))
        {
            MG_LOG_ERROR("read file header failed,err:%s",strerror(errno));
            return MG_ERR_FILE_READ_FAILED;
        }
        if(file_hdr.st_hdr.hdr_type < MG_EFILE_HDR_TYPE_MIN
            || file_hdr.st_hdr.hdr_type > MG_EFILE_HDR_TYPE_MAX)
        {
            MG_LOG_ERROR("invalid file header");
            return MG_ERR_FILE_READ_FAILED;
        }
        efile->hdr_type = file_hdr.st_hdr.hdr_type;
        efile->hdr_checksum = file_hdr.st_hdr.checksum;
        efile->file_size = efile_size(fp);
        MG_LOG_ERROR("efile->file_size:%d",efile->file_size);
        MG_LOG_ERROR("efile->hdr_checksum:%d",efile->hdr_checksum);
    }else if(MG_EFILE_OVERWRITE_MODE == efile->rwmode)
    {
        unlink(file);
        fp = fopen(file,"wb+");
        if(NULL == fp)
        {
            MG_LOG_ERROR("fopen failed,err:%s",strerror(errno));
            return MG_ERR_FILE_OPEN_FAILED;
        }
        if(0 != efile->perm)
        {
            int fd = fileno(fp);
            fchmod(fd,efile->perm);
        }
    }

    efile->fp = fp;
    return MG_NO_ERR;
}

/**
    函数名:    mg_efile_close <关闭一个文件>
    参数:   st_efile* efile efile对象
    返回值: MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_FILE_READ_FAILED 读取文件失败
            MG_ERR_FILE_OPEN_FAILED 打开文件失败
            MG_NO_ERR 成功
*/
int mg_efile_close(st_efile* efile)
{
    if(NULL == efile || NULL == efile->fp)
    {
        MG_LOG_ERROR("param is not valid");
        return MG_ERR_PARAMETER_ERROR;
    }

    if(MG_EFILE_OVERWRITE_MODE == efile->rwmode)
    {
        /*update file header*/
        efile_update_hdr(efile);

        /*flush to disk*/
        if(efile->flush)
        {
            efile->flush(efile);
        }
        sync();
    }

    fclose(efile->fp);
    efile->fp = NULL;
    efile->check_sum = 0;
    efile->processed_bytes = 0;
    efile->rwmode = MG_EFILE_NONE_MODE;
    efile->perm = S_IRUSR|S_IWUSR;
    return MG_NO_ERR;
}

/**
    函数名:    mg_efile_write <将数据写入文件>
    参数:   st_efile* efile  efile对象
          const void* buf  需要写入的缓存
          int len 写入的长度
    返回值: MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_SYS_ERROR 写入错误
            MG_NO_ERR 成功
*/
int mg_efile_write(st_efile* efile,const void* buf,int len)
{
    int ret;
    char* ptr_buf = NULL;
    int bytes_to_write = 0;

    if(NULL == buf || len <= 0 || NULL == efile)
    {
        MG_LOG_ERROR("param is not valid");
        return MG_ERR_PARAMETER_ERROR;
    }
    if(MG_EFILE_OVERWRITE_MODE != efile->rwmode)
    {
        MG_LOG_ERROR("rwmode error:%d",efile->rwmode);
        return MG_ERR_PARAMETER_ERROR;
    }
    if(0 != (len % 16))
    {
        MG_LOG_ERROR("data length is not correct,len:%d",len);
        return MG_ERR_PARAMETER_ERROR;
    }

    ptr_buf = (char*)buf;
    if(NULL == efile->write)
    {
        MG_LOG_ERROR("no write function");
        return MG_ERR_PARAMETER_ERROR;
    }
    if(NULL == efile->fp)
    {
        MG_LOG_ERROR("no file pointer,pls open first");
        return MG_ERR_PARAMETER_ERROR;
    }

    if(ftell(efile->fp) < efile->hdr_length)
    {
        fseek(efile->fp,efile->hdr_length,SEEK_SET);
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
            return MG_ERR_SYS_ERROR;
        }
        len -= bytes_to_write;
        buf = (char*)buf + bytes_to_write;
        efile->processed_bytes += bytes_to_write;
    }
    return MG_NO_ERR;
}

/**
    函数名:    mg_efile_read <从文件中读取>
    参数:   st_efile* efile  efile对象
          const void* buf  需要读取的缓存
          int len 读取的最大长度
    返回值: MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_SYS_ERROR 读取错误
            MG_ERR_FILE_FORMAT_ERROR 格式错误
            MG_NO_ERR 成功
*/
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
        return MG_ERR_PARAMETER_ERROR;
    }

    if(MG_EFILE_READ_MODE != efile->rwmode)
    {
        MG_LOG_ERROR("rwmode error:%d",efile->rwmode);
        return MG_ERR_PARAMETER_ERROR;
    }

    ptr_buf = (char*)buf;
    if(NULL == efile->read)
    {
        MG_LOG_ERROR("no read function");
        return MG_ERR_PARAMETER_ERROR;
    }
    if(NULL == efile->fp)
    {
        MG_LOG_ERROR("no file pointer,pls open first");
        return MG_ERR_PARAMETER_ERROR;
    }

    //偏移指针到数据段的开头
    if(ftell(efile->fp) < efile->hdr_length)
    {
        fseek(efile->fp,efile->hdr_length,SEEK_SET);
    }

    file_size = efile->file_size;
    if((file_size < efile->hdr_length)
        || (file_size < ftell(efile->fp)))
    {
        MG_LOG_ERROR("file length is error,size:%d",file_size);
        return MG_ERR_FILE_FORMAT_ERROR;
    }

    //获取可读的长度
    file_size = efile->file_size - ftell(efile->fp);
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
                return MG_ERR_SYS_ERROR;
            }
        }
        if(efile->update_cksum)
        {
            efile->update_cksum(efile,ptr_buf,bytes_to_read);
        }

        len -= bytes_to_read;
        buf = (char*)buf + bytes_to_read;
        efile->processed_bytes += bytes_to_read;
        processed_bytes += bytes_to_read;

        if(!feof(efile->fp))
            break;
    }

    MG_LOG_ERROR("checksum:%d",efile->check_sum);
    MG_LOG_ERROR("hdr_type:%d",efile->hdr_type);
    return processed_bytes;
}

/**
    函数名:    mg_efile_check <校验文件完整性，需要在读取完文件后调用>
    参数:   st_efile* efile  efile对象
    返回值: TRUE：文件完整
            FALSE:文件不完整
*/
boolean mg_efile_check(st_efile* efile)
{
    if(NULL == efile)
    {
        return FALSE;
    }

    return (efile->check_sum == efile->hdr_checksum) ? TRUE : FALSE;
}

#endif


