#ifndef __COMMONS_ENCRYPT_H__
#define __COMMONS_ENCRYPT_H__
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
extern int utils_cbc_encrypt(void *in, size_t inLen, char *key, void *out);
extern int utils_cbc_decrypt(void *in, size_t inLen, char *key, void *out);

#endif

