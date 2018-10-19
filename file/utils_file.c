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

s32 file_size(s8* file_path)
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


 