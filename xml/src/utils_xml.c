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

#include "libxml/parser.h"
#include "libxml/tree.h"
#include "iconv.h"

char* utils_xml_read_node(char* filename,char* node_content)
{
    xmlDocPtr doc = NULL;           //定义解析文档指针
    xmlNodePtr node = NULL;          //定义结点指针(你需要它为了在各个结点间移动)
    char* buffer = NULL;
    xmlChar* content = NULL;

    xmlKeepBlanksDefault(0);  //避免将空格当作一个节点
    doc = xmlReadFile(filename,"UTF-8",XML_PARSE_RECOVER);
	if(NULL == doc)
	{
        commons_println("parse error\n");
        return NULL;
	}
	node = xmlDocGetRootElement(doc);
	if(NULL == node)
	{
        commons_println("doc has no content\n");
        xmlFreeDoc(doc);
        return NULL;
	}

	node = node->children;
	while(NULL != node)
	{
	    commons_println("node name:%s",node->name);
		if(xmlStrcmp(node->name,(const xmlChar*)node_content)==0)
		{
		    commons_println("find");
			content = xmlNodeGetContent(node);
            if(NULL == content)
            {
                break;
            }
            buffer = malloc(strlen(content) + 1);
            memset(buffer,0,strlen(content) + 1);
            strcpy(buffer,content);
            break;
        }
		node = node->next;
	}

    /*  释放资源  */
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();
    return buffer;
}



