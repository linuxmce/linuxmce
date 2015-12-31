// xml_message.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp ; diff tree1.tmp tree1.res ; rm tree1.tmp
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>
using namespace std;

static void
print_element_names(xmlNode * a_node, string offset = "")
{
    xmlNode *cur_node = NULL;
	xmlAttr *cur_attr = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) 
	{
        if (cur_node->type == XML_ELEMENT_NODE) 
		{
			printf("%snode type: Element, name: %s\n", offset.c_str(), cur_node->name);

			for(cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next)
			{
				printf("%sattr name: %s, value: %s\n", (offset + " - ").c_str(), 
					cur_attr->name, cur_attr->children ? (const char *)cur_attr ->children->content : "");

							
			}
		}

        print_element_names(cur_node->children, offset + "\t");
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
	xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    LIBXML_TEST_VERSION

	//read from memory
	string sXmlData = "<message type=\"4\" id=\"0\" ><from id=\"4\" /><target id=\"20\" type=\"device\" /><expected_response type=\"default\" /><parameters><parameter id=\"0\" value=\"OK\" /><parameter id=\"91\" value=\"d41d8cd98f00b204e9800998ecf8427e\" /><parameter id=\"92\" value=\"0\" /></parameters><extra_messages></extra_messages></message>";
	doc = xmlReadMemory(sXmlData.c_str(), static_cast<int>(sXmlData.length()), NULL, NULL, 0);

	//... or from a file
    //doc = xmlReadFile("test.xml", NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", "/test.xml");
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

	return 0;
}

