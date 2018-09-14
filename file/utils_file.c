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
 
 