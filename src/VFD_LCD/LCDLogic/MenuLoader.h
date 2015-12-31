#ifndef __MENU_LOADER_H__
#define __MENU_LOADER_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//--------------------------------------------------------------------------------------------------------
#include <libxml/parser.h>
#include <libxml/tree.h> 
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
class MenuItem;
//--------------------------------------------------------------------------------------------------------
class MenuLoader 
{
public:

	MenuLoader();
	~MenuLoader();

	MenuHolder *Load(string sFilename);
	MenuHolder *Load(char *pData, size_t nSize);

private:

	MenuItem *ParseMenuItem(xmlNode *node, MenuItem *pParent);
	MenuHolder *ParseMenu(xmlDoc *doc);
	void ParseAttributes(xmlNode *pNode, map<string, string>& mapAttributes);
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_LOADER_H__
