/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * this licence is created by tool:newfile.sh VER:1.0 in  2019/08/23 automatic
*/
#ifndef __MG_AES_H__
#define __MG_AES_H__

#include <stddef.h>
#include "modes.h"

# if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64))
#  define SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
#  define GETU32(p) SWAP(*((u32 *)(p)))
#  define PUTU32(ct, st) { *((u32 *)(ct)) = SWAP((st)); }
# else
#  define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#  define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); (ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
# endif


# ifdef AES_LONG
typedef unsigned long u32;
# else
typedef unsigned int u32;
# endif
typedef unsigned short u16;
typedef unsigned char u8;

# define MAXKC   (256/32)
# define MAXKB   (256/8)
# define MAXNR   14

/* This controls loop-unrolling in aes_core.c */
# undef FULL_UNROLL

# define AES_ENCRYPT     1
# define AES_DECRYPT     0

/*
 * Because array size can't be a const in C, the following two are macros.
 * Both sizes are in bytes.
 */
# define AES_MAXNR 14
# define AES_BLOCK_SIZE 16

/* This should be a hidden type, but EVP requires that the size be known */
struct aes_key_st {
# ifdef AES_LONG
    unsigned long rd_key[4 * (AES_MAXNR + 1)];
# else
    unsigned int rd_key[4 * (AES_MAXNR + 1)];
# endif
    int rounds;
};
typedef struct aes_key_st AES_KEY;

void CRYPTO_cbc128_encrypt(const unsigned char *in, unsigned char *out,
                           size_t len, const void *key,
                           unsigned char ivec[16], block128_f block);

void CRYPTO_cbc128_decrypt(const unsigned char *in, unsigned char *out,
                          size_t len, const void *key,
                          unsigned char ivec[16], block128_f block);

void AES_cbc_encrypt(const unsigned char *in, unsigned char *out,
                   size_t len, const AES_KEY *key,
                   unsigned char *ivec, const int enc);

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);

int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);


#endif


