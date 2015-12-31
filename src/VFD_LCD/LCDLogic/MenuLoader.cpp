#include "MenuLoader.h"
#include "MenuHolder.h"
#include "MenuItem.h"
#include "InputMenuItem.h"
#include "MenuItemAction.h"

#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
MenuLoader::MenuLoader()
{
	LIBXML_TEST_VERSION;
}
//--------------------------------------------------------------------------------------------------------
MenuLoader::~MenuLoader()
{
    xmlCleanupParser();
    xmlMemoryDump();
}
//--------------------------------------------------------------------------------------------------------
MenuHolder *MenuLoader::Load(string sFilename)
{
	xmlDoc *doc = xmlReadFile(sFilename.c_str(), NULL, 0);

	if(NULL == doc) 
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to parse xml %s", sFilename.c_str());
		return NULL;
	}

	return ParseMenu(doc);
}
//--------------------------------------------------------------------------------------------------------
MenuHolder *MenuLoader::Load(char *pData, size_t nSize)
{
	xmlDoc *doc = xmlReadMemory(pData, static_cast<int>(nSize), NULL, NULL, 0);
	if(NULL == doc) 
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to parse xml %s", pData);
		return NULL;
	}

	return ParseMenu(doc);
}
//--------------------------------------------------------------------------------------------------------
MenuHolder *MenuLoader::ParseMenu(xmlDoc *doc)
{
	xmlNode *menu_node = xmlDocGetRootElement(doc); 
	MenuHolder *pMenuHolder = new MenuHolder();

	if(NULL != menu_node && menu_node->type == XML_ELEMENT_NODE && string(reinterpret_cast<const char *>(menu_node->name)) == "menu") 
	{
		xmlNode *cur_node = NULL;
		for(cur_node = menu_node->children; cur_node; cur_node = cur_node->next)  
		{
			if(NULL!= cur_node && cur_node->type == XML_ELEMENT_NODE) 
			{
				MenuItem *pMenuItem = ParseMenuItem(cur_node, pMenuHolder->RootMenu());

				if(NULL != pMenuItem)
					pMenuHolder->RootMenu()->AddChild(pMenuItem);	
			}
		}
	}

	return pMenuHolder;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuLoader::ParseMenuItem(xmlNode *node, MenuItem *pParent)
{
	if(string(reinterpret_cast<const char *>(node->name)) != "menuitem")
		return NULL;

	map<string, string> mapAttributes;
	ParseAttributes(node, mapAttributes);

	string sDescription = mapAttributes["description"];
	string sAction = mapAttributes["action"];
	string sActionType = mapAttributes["actiontype"];

	ActionType action_type = atNone;
	if(sActionType == "execute_script")
		action_type = atExecuteScript;
	else if(sActionType == "execute_command")
		action_type = atExecuteCommand;
	else if(sActionType == "call_method")
		action_type = atCallMethod;

	//TODO optimize
	string sParameter1 = mapAttributes["parameter1"];
	string sParameter2 = mapAttributes["parameter2"];
	string sParameter3 = mapAttributes["parameter3"];

	map<int, string> mapParameters;

	//TODO optimize
	if(!sParameter1.empty())
		mapParameters[1] = sParameter1;
	if(!sParameter2.empty())
		mapParameters[2] = sParameter2;
	if(!sParameter3.empty())
		mapParameters[3] = sParameter3;

	ItemType type = itListItem;

	if(mapAttributes["type"] == "input")
		type = itInputBox;
	if(mapAttributes["type"] == "expand")
		type = itExpandItem;

	MenuItemAction *pAction = new MenuItemAction(sAction, action_type, mapParameters);
	MenuItem *pMenuItem = NULL;

	if(type == itInputBox)
		pMenuItem = new InputMenuItem(sDescription, pParent, type, pAction, mapAttributes["mask"]);
	else
		pMenuItem = new MenuItem(sDescription, pParent, type, pAction);

	pMenuItem->Status(mapAttributes["status"]);
	pMenuItem->Value(mapAttributes["value"]);

	//parse children
	xmlNode *cur_node = NULL;
	for(cur_node = node->children; cur_node; cur_node = cur_node->next)  
	{
		if(NULL!= cur_node && cur_node->type == XML_ELEMENT_NODE) 
		{
			MenuItem *pChildMenuItem = ParseMenuItem(cur_node, pMenuItem);

			if(NULL != pChildMenuItem)
				pMenuItem->AddChild(pChildMenuItem);
		}
	}

	return pMenuItem;
}
//--------------------------------------------------------------------------------------------------------
void MenuLoader::ParseAttributes(xmlNode *pNode, map<string, string>& mapAttributes)
{
	if(NULL == pNode)
		return;

	xmlAttr *cur_attr = NULL;
	for(cur_attr = pNode->properties; NULL != cur_attr; cur_attr = cur_attr->next)
		mapAttributes[reinterpret_cast<const char *>(cur_attr->name)] =
			NULL != cur_attr->children && cur_attr->children->content ? 
				reinterpret_cast<const char *>(cur_attr->children->content) : "";
}
//--------------------------------------------------------------------------------------------------

