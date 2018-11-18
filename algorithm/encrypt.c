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
#include <openssl/aes.h>
#include "encrypt.h"

int utils_cbc_encrypt(void *in, size_t inLen, char *key, void *out)
{
    AES_KEY aes;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    if(!in || !inLen || !key || !out)
    {
        return -1;
    }

    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return -1;
    }

    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, inLen, &aes, iv, AES_ENCRYPT);

    return 0;
}

int utils_cbc_decrypt(void *in, size_t inLen, char *key, void *out)
{
    AES_KEY aes;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    if(!in || !key || !out)
    {
        return -1;
    }

    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return -1;
    }

    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, inLen, &aes, iv, AES_DECRYPT);

    return 0;
}




