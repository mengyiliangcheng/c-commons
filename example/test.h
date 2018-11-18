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
#ifndef __COMMONS_TEST_H__
#define __COMMONS_TEST_H__

typedef struct
{
    char dispName[64];
    int (*fun)(void);
}ST_TEST_LIST;


extern void testProgramUI(void);
extern int testStringToHex();
extern int testCommonsScanf();
extern int testMempool();
extern int testCommonsRandStr();
extern int testFileSaveOverlap();
extern int testCrc8();
extern int testFileCopy();
extern int testReadXMl();
extern int testFilePackage();
extern int testCbcEncrypt();
extern int testOthers();

#endif

