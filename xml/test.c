#include <stdio.h>
#include <libxml/parser.h>
#include "libxml/tree.h"
#include "iconv.h"

void main()
{
	xmlDocPtr doc;           //定义解析文档指针
    xmlNodePtr node;          //定义结点指针(你需要它为了在各个结点间移动)
	xmlKeepBlanksDefault(0);  //避免将空格当作一个节点
	doc = xmlParseFile("222");	
}
