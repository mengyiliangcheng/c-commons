#ifndef __COMMONS_FILE_H__
#define __COMMONS_FILE_H__
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
 #include "osadapter.h"
 
 /****************************macro*******************************************/
 
 #define UTILS_FILE_MAX_NAME_LEN      (256)
 
 
 /****************************function*******************************************/
 
 extern BOOL file_exist(s8* file_path);

 extern s32 utils_file_size(s8* file_path);
 
 #endif