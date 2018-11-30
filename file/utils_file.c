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
 
	/* ���㻺������С����Ϊ������ڴ� */
	blen = compressBound(tlen);	/* ѹ����ĳ����ǲ��ᳬ��blen�� */
    printf("blen:%d\n",blen);
	if((buf = (char*)malloc(sizeof(char) * blen)) == NULL)
	{
		commons_println("no enough memory!\n");
		return -1;
	}
 
	/* ѹ�� */
    ret = compress(buf, &blen, text, tlen);
	if(ret != Z_OK)
	{
		commons_println("compress failed!ret:%d\n",ret);
		return -1;
	}
    commons_print_hex(buf,sizeof(buf));
 
	/* ��ѹ�� */
	if(uncompress(text, &tlen, buf, blen) != Z_OK)
	{
		commons_println("uncompress failed!\n");
		return -1;
	}
 
	/* ��ӡ��������ͷ��ڴ� */
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
    /* ѹ������ */
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
    /* 1-ʹ���ϴ�״̬ 2-�������һ���ֽ�*/
    int record_type;
    char last_descript[256];
}ST_ANALYZE_LOG;

ST_ANALYZE_LOG TAB_ANALYZE_LOG[] = 
{
    {"MCU powerkey wakeup event !!!"    ,"MCU powerkey wakeup event",1,"��������"},
    {"[gps] JSON"                       ,"��λ�ɹ�"},
    {"===========tracker service has start=============","Ӧ������",1,"����psm"},
    {"flp position wait timeout"        ,"��λʧ��"},
    {"dev logout",                      "�ϱ�����ʧ��"},
    {"start task,dev status:"           ,"��ʼ�ϱ�"},
    {"report succ,current total count"  ,"�ϱ��ɹ�"},
    {"HandleAppItem: sid:86, cid:2",    "mcu֪ͨ��λ"},
    {"timeout int 35 s!!!",             "hilink��¼��ģʽ��ʱ"},
    {"start report cache hander"        ,"�����ϱ�"},
    {"AT+CPSMS=1, Psm Open Status"      ,"sleepģ��ʹ��psm"},
    {"AT+CPSMS=0, Psm Close Status."    ,"sleepģ��ر�psm"},
    {"MCU  wakeup event "               ,"���߻���"},
    {"Received SOS msg"                 ,"mcu����sos"},
    {"[Alarm] JSON:{\"code\":1"         ,"�յ����籨��"},
    {"[Alarm] JSON:{\"code\":6"         ,"�յ���Χ���澯"},
    {"[Alarm] JSON:{\"code\":7"         ,"�յ�sos�澯"},
    {"The system will shutdown"         ,"mcu֪ͨ9206�ػ�"},
    {"psmEnable sig : 1"                ,"mcu֪ͨ���Խ���psm"},
    {"psmEnable sig : 0"                ,"mcu֪ͨ�����Խ���psm"},
    {"Current Dialup Status Changed to 1","���ųɹ�"},
    {"report failed:no reg network",     "�ϱ�ʧ�ܣ�ûע��"},
    {"Sntp Sync successed!!!!",          "nitz/sntpУʱ�ɹ�"},
    {"Test  power_on_reason****"         ,"����ԭ��:",              2},
};

const char *const boot_reason_list[8] =
{
    [0] = ("reboot"),
    [2] = ("TAU����"),
    [6] = ("MCU����"),
};


void analyze_log(s8* start_time,s8* end_time,BOOL b_check_time)
{
    FILE* fp = NULL;
    FILE* analyze_res_fp = NULL;
    fp = fopen("./tracker.log","r+");
    char tmp[1024];
    char s_date[128];
    char buffer[1024];
    char s_last_record[1024];
    char s_last_date[128];
    BOOL start_flag = FALSE;
    time_t time_tmp;
    int i = 0;
    unlink("./result.csv");
    analyze_res_fp = fopen("./result.csv","a+");

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
                }else
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
    commons_println("analyze log finish,output file:./result.csv");
}



 
