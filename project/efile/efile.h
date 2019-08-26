/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/21 automatic
*/
#ifndef __EFILE_H__
#define __EFILE_H__


#define EFILE_HDR_LEN               (32)   //header length of file
#define EFILE_PRINT_HEX_CHUNK       (128)
#define EFILE_PRINT_BTYES_PER_LINE  (8)
#define EFILE_MAX_PROCESS_CHUNK     (128)
#define EFILE_TDES_LEN              (16)

#ifndef MAX
#define MAX(a, b) (a) > (b) ? (a):(b)
#endif

#ifndef MIN
#define MIN(a, b) (a) < (b) ? (a):(b)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
     当前文件加密类型
*/
#define EFILE_CURR_HDR_TYPE    (EFILE_HDR_TYPE1)

typedef enum
{
    EFILE_NONE_MODE,
    EFILE_READ_MODE,
    EFILE_OVERWRITE_MODE,
    EFILE_APPEND_MODE,
}e_efile_rwmode;

/*
    encrypt file type
*/
typedef enum
{
    EFILE_HDR_TYPE_MIN = 0,
    EFILE_HDR_TYPE1 = 1,
    EFILE_HDR_TYPE2,
    EFILE_HDR_TYPE3,
    EFILE_HDR_TYPE4,
    EFILE_HDR_TYPE5,
    EFILE_HDR_TYPE_MAX,
}e_efile_hdr_type;

/*
    error code
*/
typedef enum
{
    EFILE_RET_OK,
    EFILE_RET_ERR,
    EFILE_RET_SYS_ERR,
    EFILE_RET_PARAM_ERR,
    EFILE_FILE_OPEN_ERR,
    EFILE_FILE_READ_ERR,
    EFILE_FILE_WRITE_ERR,
    EFILE_FILE_FORMAT_ERR,
}e_efile_error_code;

/*
    structure of file header
*/
typedef union
{
    char hdr[EFILE_HDR_LEN];
    struct
    {
        /*file header,enum type:e_efile_hdr_type*/
        e_efile_hdr_type hdr_type;
        /*check sum of plain file*/
        unsigned int checksum;
    }st_hdr;
}u_efile_hdr;

/*
    structure of efile
*/
typedef struct st_efile
{
    /*read/write/tell/flush/checksum function callback*/
    int             (*read)(struct st_efile *, void *buf, int len);
    int             (*write)(struct st_efile *, const void *buf, int len);
    long            (*tell)(struct st_efile *);
    int             (*flush)(struct st_efile *);
    void            (*close)(struct st_efile *);
    unsigned int    (*update_cksum)(struct st_efile *, const void *buf, int len);
    unsigned int    check_sum;

    /*file handle*/
    void*    fp;

    /*file size,now only valid in MG_EFILE_READ_MODE*/
    int file_size;

    /*encrypt type*/
    e_efile_hdr_type hdr_type;

    /*file checksum,only valid in MG_EFILE_READ_MODE*/
    unsigned int hdr_checksum;

    /*length of header*/
    int hdr_length;

    /*max size of read/write*/
    int max_process_chunk;

    /*length of processed bytes*/
    int processed_bytes;

    /*initial key*/
    unsigned char init_key[64];
    int init_key_len;

    /*permission*/
    int perm;

    /*r/w mode:0---none 1---read 2---write*/
    int rwmode;

    /*locker,not used now*/
    //pthread_rwlock_t lock;
}st_efile;

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
extern st_efile* mg_efile_new(void);

/****************************************************************
* Function: mg_efile_open
* Description: 打开一个文件
* Param:
* Input:  st_efile* efile efile对象
          const char* file 文件名
          e_efile_rwmode rwmode 读写模式
* Output:
* Return:   MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_FILE_READ_FAILED 读取文件失败
            MG_ERR_FILE_OPEN_FAILED 打开文件失败
            MG_NO_ERR 成功
* Others:
* Author: pengyicheng
****************************************************************/
extern int mg_efile_open(st_efile* efile,const char* file,e_efile_rwmode rwmode);

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
extern void mg_efile_free(st_efile* efile);

/****************************************************************
* Function: mg_efile_close
* Description: 关闭一个文件
* Param:
* Input:  st_efile* efile efile对象
* Output:
* Return:   MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_FILE_READ_FAILED 读取文件失败
            MG_ERR_FILE_OPEN_FAILED 打开文件失败
            MG_NO_ERR 成功
* Others:
* Author: pengyicheng
****************************************************************/
extern int mg_efile_close(st_efile* efile);

/****************************************************************
* Function: mg_efile_write
* Description: 将数据写入文件
* Param:
* Input:    st_efile* efile  efile对象
            int len 写入的长度
* Output:   const void* buf  需要写入的缓存
* Return:   MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_SYS_ERROR 写入错误
            MG_NO_ERR 成功
* Others:
* Author: pengyicheng
****************************************************************/
extern int mg_efile_write(st_efile* efile,const void* buf,int len);

/****************************************************************
* Function: mg_efile_read
* Description: 从文件中读取
* Param:
* Input:    st_efile* efile  efile对象
            int len 读取的最大长度
* Output:   const void* buf  需要读取的缓存
* Return:   MG_ERR_PARAMETER_ERROR 参数错误
            MG_ERR_SYS_ERROR 读取错误
            MG_ERR_FILE_FORMAT_ERROR 格式错误
            MG_NO_ERR 成功
* Others:
* Author: pengyicheng
****************************************************************/
extern int mg_efile_read(st_efile* efile,const void* buf,int len);

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
extern bool mg_efile_check(st_efile* efile);

/****************************************************************
* Function: mg_efile_setkey
* Description: 设置初始化密钥
* Param:
* Input:  st_efile* efile efile对象
          unsigned char* initKey 初始化密钥
          int length  密钥长度
* Output:
* Return: MG_ERR_PARAMETER_ERROR 参数错误
          MG_NO_ERR 成功
* Others:
* Author: pengyicheng
****************************************************************/
extern int mg_efile_setkey(st_efile* efile,const unsigned char* initKey1,const unsigned char* initKey2,int length);


#endif


