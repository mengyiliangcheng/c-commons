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
#include <sys/types.h>
#include <dirent.h>
#include <utils_string.h>
#include "utils_file.h"
#include "digest.h"
#include "zlib.h"
#include "zip.h"

#define LOG(...) COMMONS_LOG("UTILS_FILE",__VA_ARGS__);

 
/**
*description:check file exist or not
*
*/
BOOL file_exist(s8* file_path)
{
    if(NULL == file_path)
    {
        return FALSE;
    }
    FILE* fp = fopen(file_path,"r");
    if(NULL == fp)
    {
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

BOOL file_dir_exist(s8* dir_path)
{
    if(NULL == dir_path)
    {
        return FALSE;
    }
    DIR* dir = opendir(dir_path);

    if(NULL == dir)
    {
        return FALSE;
    }
    closedir(dir);
    return TRUE;
}

s32 file_write(s8* file_path,s8* in,s32 len)
{
    if(NULL == file_path || NULL == in)
    {
        return -4;
    }
    FILE* fp = NULL;
    s32 ret;

    fp = fopen(file_path,"wb");
    if(NULL == fp)
    {
        return -1;
    }

    ret = fwrite(in,sizeof(s8),len,fp);

    fclose(fp);

    return ret;
}

typedef int (*file_find_callback) (s8* dir,s8* path);

int file_find(s8* base_path,s8* file_name,file_find_callback func)
{    
    char buffer[256];
    struct dirent *dir_entry = NULL;
    struct dirent *dir_entry_next = NULL;
    DIR* dir = NULL;

    if(!file_dir_exist(base_path))
        return -1;

    dir = opendir(base_path);
    if(NULL == dir)
    {
        return -1;
    }
    while((dir_entry = readdir(dir)) != NULL)
    {
        BZERO(buffer,sizeof(buffer));
        sprintf(buffer,"%s/%s",base_path,dir_entry->d_name);    
        //commons_println("file name:%s",buffer);
        if(0 == strncmp(dir_entry->d_name,"..",2) || 0 == strncmp(dir_entry->d_name,".",1))
            continue;
        if(0 == strncmp(dir_entry->d_name,file_name,strlen(file_name)))
        {
            //commons_println("find!!!file name:%s",buffer);
            func(base_path,buffer);
            continue;
        }
        if(4 == dir_entry->d_type)
        {
            if(0 != file_find(buffer,file_name,func))
                break;
        }
    }
    closedir(dir);
    return 0;
}

s32 file_read(s8* file_path,s8* out,s32 len)
{
    if(NULL == file_path || NULL == out)
    {
        return -4;    
    }

    FILE* fp = NULL;
    s32 ret;

    fp = fopen(file_path,"rb");
    if(NULL == fp)
    {
        return -1;
    }

    ret = fread(out,sizeof(s8),len,fp);

    fclose(fp);

    return ret;
}

s32 file_copy(s8* src_path,s8* dest_path)
{
    s32 ret = 0;
    s32 len;
    s8 buffer[1024];
    FILE* fp_src = NULL;
    FILE* fp_dest = NULL;

    if(!src_path || !dest_path)
    {
        return -1;
    }

    fp_src = fopen(src_path,"rb");
    if(NULL == fp_src)
    {
        commons_println("open src error");
        return -1;
    }

    fp_dest = fopen(dest_path,"wb");
    if(NULL == fp_dest)
    {
        commons_println("open dest error");
        fclose(fp_src);
        return -1;
    }

    BZERO(buffer,sizeof(buffer));
    while((len = fread(buffer,1,sizeof(buffer),fp_src)) > 0)
    {
        ret = fwrite(buffer,1,len,fp_dest);
        if(ret != len)
        {
            fclose(fp_src);
            fclose(fp_dest);
            return -1;
        }
    }

    fclose(fp_src);
    fclose(fp_dest);
    
    return 0;
}

void file_remove(s8* file_path)
{
    if(file_exist(file_path))
    {
        unlink(file_path);
        sync();
    }
}

s32 utils_file_size(s8* file_path)
{
    if(NULL == file_path)
    {
        return -1;
    }

    FILE* fp = NULL;
    s32 ret;

    fp = fopen(file_path,"rb");
    if(NULL == fp){
        return -1;
    }
    fseek(fp,0L,SEEK_END);
    ret = ftell(fp);
    fclose(fp);
    return ret;
}

s32 file_save_overlap(s8* file_path,s8* in,s32 len)
{
    s32 ret;
    s8 buffer[256] = {0};
    
    ret = file_write(file_path,in,len);
    if(ret <= 0)
    {
       return ret;
    }
    if(strlen(file_path) > sizeof(buffer))
    {
        return -2;
    }
    
    sprintf(buffer,"%s%s",file_path,".bak");
    file_remove(buffer);
    ret = file_write(buffer,in,len);

    return ret;
}

s32 file_read_overlap(s8* file_path,s8* out,s32 len)
{
    file_read(file_path,out,len);    
}



s32 file_package1()
{
    char text[] = "zlib compress and uncompress test\nturingo@163.com\n2012-11-05\n";
	u64 tlen = strlen(text) + 1;
	char* buf = NULL;
	u64 blen;
    s32 ret;
 
	/* 计算缓冲区大小，并为其分配内存 */
	blen = compressBound(tlen);	/* 压缩后的长度是不会超过blen的 */
    printf("blen:%d\n",blen);
	if((buf = (char*)malloc(sizeof(char) * blen)) == NULL)
	{
		commons_println("no enough memory!\n");
		return -1;
	}
 
	/* 压缩 */
    ret = compress(buf, &blen, text, tlen);
	if(ret != Z_OK)
	{
		commons_println("compress failed!ret:%d\n",ret);
		return -1;
	}
    commons_print_hex(buf,sizeof(buf));
 
	/* 解压缩 */
	if(uncompress(text, &tlen, buf, blen) != Z_OK)
	{
		commons_println("uncompress failed!\n");
		return -1;
	}
 
	/* 打印结果，并释放内存 */
	printf("%s", text);
	if(buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
 
    return 0;
}

s32 file_compress(s8* src_file,s8* dest_file)
{
    FILE* file;
    u64 flen;
    u8* fbuf = NULL;
    u64 clen;
    u8* cbuf = NULL;
    
    if((file = fopen(src_file, "rb")) == NULL)
    {
        commons_println("Can\'t open %s!\n", src_file);
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    flen = ftell(file);
    fseek(file, 0L, SEEK_SET);
    if((fbuf = (unsigned char*)malloc(sizeof(unsigned char) * flen)) == NULL)
    {
        commons_println("No enough memory!\n");
        fclose(file);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, file); 
    /* 压缩数据 */
    clen = compressBound(flen);
    if((cbuf = (unsigned char*)malloc(sizeof(unsigned char) * clen)) == NULL)
    {
        printf("No enough memory!\n");
        fclose(file);
        return -1;
    }
    if(compress(cbuf, &clen, fbuf, flen) != Z_OK)
    {
        printf("Compress %s failed!\n",src_file);
        return -1;
    }

    fclose(file);

    if((file = fopen(dest_file, "wb")) == NULL)
    {
        printf("Can\'t create %s!\n",dest_file);
        return -1;
    }
    fwrite(&flen, sizeof(u64), 1, file);
    fwrite(&clen, sizeof(u64), 1, file);
    fwrite(cbuf, sizeof(u8), clen, file);
    fclose(file);
 
    free(fbuf);
    free(cbuf);
 
    return 0;

}

s32 file_check_duplicate_name(s8* old_filename,s8* new_filename,u32 dept)
{
    s8 package_name[UTILS_FILE_MAX_NAME_LEN + 16] = {0};

    if(NULL == old_filename || NULL == new_filename)
    {
        return -1;
    }
    
    if(file_exist(old_filename))
    {
        BZERO(package_name,sizeof(package_name));
        sprintf(package_name,"%s-%d",old_filename,dept);
        file_check_duplicate_name(package_name,new_filename,dept+1);
    }else
    {
        strncpy(new_filename,old_filename,strlen(old_filename));
        return 0;
    }
}

s32 file_del_suffix_name(s8* file_name)
{
    s32 i = 0;
    BOOL dot_found = FALSE;
    if(NULL == file_name) return -1;
    for(;i < strlen(file_name);i ++)
    {
        if('.' == file_name[i])
        {
            file_name[i] = 0;
            break;
        }
    }
    return 0;
}

s32 file_add_suffix_name(s8* file_name)
{
    s32 i = 0;
    BOOL dot_found = FALSE;
    if(NULL == file_name) return -1;
    for(;i < strlen(file_name);i ++)
    {
        if('.' == file_name[i])
            dot_found = TRUE;

        if(!dot_found)
            strcat(file_name,".zip");
    }
    return 0;
}

s32 file_package(s8* fileNameInZip,s8* packageName)
{
#if 0
    zipFile zf;
    zip_fileinfo file_info;
    s32 ret;
    s8 package_name[UTILS_FILE_MAX_NAME_LEN + 16] = {0};
    s8 dest_file[UTILS_FILE_MAX_NAME_LEN + 16] = {0};

    if(NULL == fileNameInZip || NULL == dest_file)
    {
        return -1;
    }

    BZERO(dest_file,sizeof(dest_file));
    strings_copy(dest_file,packageName,MIN(sizeof(dest_file),strlen(packageName)));
    
    file_del_suffix_name(dest_file);
    file_check_duplicate_name(dest_file,package_name,0);
    file_add_suffix_name(package_name);
    LOG("package name:%s",package_name);  
    
    zf = zipOpen(dest_file, APPEND_STATUS_CREATE);
    if(NULL == zf)
    {
        LOG("zip open failed");
    }

    BZERO(&file_info,sizeof(file_info));
    filetime(fileNameInZip,&file_info.tmz_date,&file_info.dosDate);
   #endif 
}


typedef struct
{
    char reason[128];
    char descript[256];
    /* 1-使用上次状态 2-输出后面一个字节 3-开始上报 4-上报结束*/
    int record_type;
    char last_descript[256];
}ST_ANALYZE_LOG;

ST_ANALYZE_LOG TAB_ANALYZE_LOG[] = 
{
    {"MCU powerkey wakeup event !!!"    ,"MCU powerkey wakeup event",1,"进入休眠"},
    {"[gps] JSON"                       ,"定位成功"},
    {"===========tracker service has start=============","应用启动",1,"进入psm"},
    {"flp position wait timeout"        ,"定位失败"},
    {"dev logout",                      "上报数据失败",5},
    {"start task,dev status:"           ,"开始上报",3},
    {"report succ,current total count"  ,"上报成功",4},
    {"HandleAppItem: sid:86, cid:2",    "mcu通知定位"},
    {"timeout int 35 s!!!",             "hilink登录云模式超时",5},
    {"report failed:no network",        "上报失败，未拨号"},  
    {"report failed:no reg network",     "上报失败，没注网"},
    {"report result:bad snr",            "上报失败，snr差",5},
    {"start report cache hander"        ,"触发上报"},
    {"AT+CPSMS=1, Psm Open Status"      ,"sleep模块使能psm"},
    {"AT+CPSMS=0, Psm Close Status."    ,"sleep模块关闭psm"},
    {"MCU  wakeup event "               ,"休眠唤醒"},
    {"Received SOS msg"                 ,"mcu触发sos"},
    //{"[Alarm] JSON:{\"code\":1,"         ,"收到满电报警"},
    {"[Alarm] JSON:{\"code\":4,"         ,"收到进无服务区报警"},
    {"[Alarm] JSON:{\"code\":11,"         ,"收到出无服务区报警"},
    {"[Alarm] JSON:{\"code\":6"         ,"收到出围栏告警"},
    {"[Alarm] JSON:{\"code\":7"         ,"收到sos告警"},
    {"The system will shutdown"         ,"mcu通知9206关机"},
    {"psmEnable sig : 1"                ,"mcu通知可以进入psm"},
    {"psmEnable sig : 0"                ,"mcu通知不可以进入psm"},
    {"Current Dialup Status Changed to 1","拨号成功"},
    {"Sntp Sync successed!!!!",          "nitz/sntp校时成功"},
    {"Test  power_on_reason****"         ,"唤醒原因:",              2},
    {"HandleAppItem: sid:86, cid:3"      ,"mcu通知上报电池电量"},
    {"Current Network Changed to 0",      "注网状态变为0"},
    {"Current Network Changed to 1",      "注网状态变为1"},
};

const char *const boot_reason_list[8] =
{
    [0] = ("reboot"),
    [2] = ("TAU唤醒"),
    [6] = ("MCU唤醒"),
};


void analyze_log(s8* path,s8* start_time,s8* end_time,BOOL b_check_time)
{
    FILE* fp = NULL;
    FILE* analyze_res_fp = NULL;
    char tmp[1024];
    char output_path[1014];
    char s_date[128];
    char buffer[1024];
    char s_last_record[1024];
    char s_last_date[128];
    char s_start_report_ts[128];
    BOOL start_flag = FALSE;
    time_t time_tmp;
    time_t report_time;
    int i = 0;
    int report_count = 0;

    if(!path)
    {
        commons_println("invalid param");
        return;
    }

    BZERO(tmp,sizeof(tmp));
    sprintf(tmp,"%s/tracker.log",path);
    fp = fopen(tmp,"r+");

    BZERO(output_path,sizeof(output_path));
    sprintf(output_path,"%s/result.csv",path);
    unlink(output_path);
    analyze_res_fp = fopen(output_path,"a+");

    if(fp == NULL || analyze_res_fp == NULL)
    {
        commons_println("log file [./tracker.log] is not exist");
        return;
    }

    while(!feof(fp))
    {
        bzero(tmp,sizeof(tmp));
        fgets(tmp,sizeof(tmp),fp);

        if(!start_flag && b_check_time)
        {
            bzero(s_date,sizeof(s_date));
            memcpy(s_date,tmp,15);
            if(utils_strptime_compare(start_time,s_date,"%h %e %T") > 0)
            {
                continue;
            }
            start_flag = TRUE;
        }
        
        for(i = 0;i < sizeof(TAB_ANALYZE_LOG)/sizeof(ST_ANALYZE_LOG);i++)
        {
            if(strstr(tmp,TAB_ANALYZE_LOG[i].reason))
            {
                if(3 == TAB_ANALYZE_LOG[i].record_type)
                {
                    bzero(s_start_report_ts,sizeof(s_start_report_ts));
                    memcpy(s_start_report_ts,tmp,15);
                }
                
                if(1 == TAB_ANALYZE_LOG[i].record_type)
                {
                    bzero(buffer,sizeof(buffer));
                    bzero(s_date,sizeof(s_date));
                    memcpy(s_date,s_last_record,15);

                    time_tmp = utils_strptime_compare(s_date,s_last_date,"%h %e %T");
                    //commons_println("%d",time_tmp);
                    sprintf(buffer,"%s,%s,%d\n",s_date,TAB_ANALYZE_LOG[i].last_descript,time_tmp);
                    fwrite(buffer,sizeof(char),strlen(buffer),analyze_res_fp);
                }
                bzero(buffer,sizeof(buffer));
                bzero(s_date,sizeof(s_date));
                memcpy(s_date,tmp,15);
                if(2 == TAB_ANALYZE_LOG[i].record_type)
                {
                    char* pos = strstr(tmp,TAB_ANALYZE_LOG[i].reason);
                    int boot_reason = atoi((pos+strlen(TAB_ANALYZE_LOG[i].reason)));
                    //commons_println("boot reason:%d",boot_reason);
                    sprintf(buffer,"%s,%s%s,\n",s_date,TAB_ANALYZE_LOG[i].descript,boot_reason_list[boot_reason]);
                }
                else if(4 == TAB_ANALYZE_LOG[i].record_type)
                {
                    if(strlen(s_start_report_ts) > 3)
                    {
                        report_time = utils_strptime_compare(s_date,s_start_report_ts,"%h %e %T");
                        sprintf(buffer,"%s,%s,,1,,%d\n",s_date,TAB_ANALYZE_LOG[i].descript,report_time);
                    }else
                    {
                        sprintf(buffer,"%s,%s,\n",s_date,TAB_ANALYZE_LOG[i].descript);
                    }
                }
                else if(5 == TAB_ANALYZE_LOG[i].record_type)
                {
                    if(strlen(s_start_report_ts) > 3)
                    {
                        report_time = utils_strptime_compare(s_date,s_start_report_ts,"%h %e %T");
                        sprintf(buffer,"%s,%s,,,1,%d\n",s_date,TAB_ANALYZE_LOG[i].descript,report_time);
                    }else
                    {
                        sprintf(buffer,"%s,%s,\n",s_date,TAB_ANALYZE_LOG[i].descript);
                    }
                }
                else
                {
                    sprintf(buffer,"%s,%s,\n",s_date,TAB_ANALYZE_LOG[i].descript);
                }
                fwrite(buffer,sizeof(char),strlen(buffer),analyze_res_fp);

                if(i == 0 || i == 2)
                {
                    strcpy(s_last_date,s_date);
                }
                break;
            }
            
        }
        bzero(s_last_record,sizeof(s_last_record));
        strcpy(s_last_record,tmp);

        if(b_check_time)
        {
            bzero(s_date,sizeof(s_date));
            memcpy(s_date,tmp,15);
            if(utils_strptime_compare(end_time,s_date,"%h %e %T") < 0)
            {
                break;
            }
        }
    }
    fclose(fp);
    fclose(analyze_res_fp);

    commons_println("output:%s",output_path);
}

#define LOG_BASE_DIR "./tracker-log"

int analyze_log_callback(s8* dir,s8* path)
{  
    //commons_println("dir:%s",dir);
    analyze_log(dir,"Nov 20 05:50:30","Nov 21 12:21:58",FALSE);
}


void analyze_log_client(void)
{
    file_find(LOG_BASE_DIR,"tracker.log",analyze_log_callback);
}


 
