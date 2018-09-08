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

#include <stdlib.h>
#include <stdio.h>
#include "osadapter.h"

s32 commons_sprintf(char* buffer,const char* format,...)
{

	return 0;
}

void* commons_malloc(s32 size)
{
	return malloc(size);
}

void* commons_free(void* ptr)
{
    free(ptr);
}

void commons_memset(void* dest,s32 ch,s32 count)
{
    memset(dest,ch,count);
}





