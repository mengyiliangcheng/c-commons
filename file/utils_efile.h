#ifndef __MG_ENCRYPT_FILE_H__
#define __MG_ENCRYPT_FILE_H__

#if 0
#define MG_LOG_DEBUG(...)   COMMONS_LOG("EFILE", __VA_ARGS__)

#define MG_LOG_INFO(...)    COMMONS_LOG("EFILE", __VA_ARGS__)

#define MG_LOG_WARNING(...) COMMONS_LOG("EFILE", __VA_ARGS__)

#define MG_LOG_ERROR(...)   COMMONS_LOG("EFILE", __VA_ARGS__)

#define MG_EFILE_HDR_LEN        (16)   //header length of file
#define MG_CPU_SN_PATH                 "/sys/devices/soc0/serial_number"
#define MG_EFILE_PRINT_HEX_CHUNK    32
/*
    current header type
*/
#define MG_EFILE_CURR_HDR_TYPE    (MG_EFILE_HDR_TYPE2)

typedef enum
{
    MG_EFILE_READ_MODE,
    MG_EFILE_OVERWRITE_MODE,
    MG_EFILE_APPEND_MODE,
}e_efile_rwmode;

/*
    encrypt file type
*/
typedef enum
{
    MG_EFILE_HDR_TYPE1 = 1,
    MG_EFILE_HDR_TYPE2,
    MG_EFILE_HDR_TYPE3,
    MG_EFILE_HDR_TYPE4,
    MG_EFILE_HDR_TYPE5,
}e_efile_hdr_type;


/*
    file header:16 bytes,contains header type,checksum.it can be extend if necessary
*/
typedef union
{
    char hdr[MG_EFILE_HDR_LEN];
    struct
    {
        /*file header,enum type:e_efile_hdr_type*/
        int hdr_type;
        /*check sum of plain file*/
        int checksum;
    }st_hdr;
}__attribute__((packed, aligned(16)))u_efile_hdr;

typedef struct st_efile
{
    /*read/write/tell/flush/checksum function callback*/
    int             (*read)(struct st_efile *, void *buf, int len);
    int             (*write)(struct st_efile *, const void *buf, int len);
    unsigned        (*tell)(struct st_efile *);
    int             (*flush)(struct st_efile *);
    unsigned int    (*update_cksum)(struct st_efile *, const void *buf, int len);
    unsigned int    check_sum;

    /*file handle*/
    FILE*    fp;
    
    /*encrypt type*/
    e_efile_hdr_type hdr_type;
    
    /*length of header*/
    int hdr_length;

    /*max size of read/write*/
    int max_process_chunk;
    
    /*length of processed bytes*/
    int processed_bytes;
    
    /*permission*/
    mode_t perm;

    /*r/w mode:0---none 1---read 2---write*/
    int rwmode;
    
    /*locker*/
    pthread_rwlock_t lock;
}st_efile;

extern int mg_efile_open(st_efile* efile,const char* file,e_efile_rwmode rwmode);

#endif
#endif
