/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/06/06 automatic
*/
#ifdef RUNTIME
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static int malloc_num = 0;
static int free_num = 0;

void *malloc(size_t size)
{
    void *(*mallocp)(size_t size);
    char* error;

    mallocp = dlsym(RTLD_NEXT,"malloc");
    if(NULL != (error = dlerror()))
    {
        fputs(error,stderr);
        exit(1);
    }

    malloc_num ++;
    char *ptr = mallocp(size);
    printf("malloc(%d) = %p total:%d\n", (int)size,ptr,malloc_num);
    return ptr;
}

void free(void* ptr)
{
    void (*freep)(void *) = NULL;
    char *error;

    freep = dlsym(RTLD_NEXT,"free");
    if(NULL != (error = dlerror()))
    {
        fputs(error,stderr);
        exit(1);
    }

    free_num ++;
    freep(ptr);
    printf("free(%p) total:%d\n",ptr,free_num);
}

#endif

