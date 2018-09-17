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
    if(NULL == file_path || NULL == in){
        return -4;
    }
    FILE* fp = NULL;
    s32 ret;

    fp = fopen(file_path,"wb");
    if(NULL == fp){
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



 