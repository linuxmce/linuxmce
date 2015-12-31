#ifndef __MENU_ITEM_ACTION_H__
#define __MENU_ITEM_ACTION_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <map>
using namespace std;
//--------------------------------------------------------------------------------------------------------
enum ActionType
{
	atNone,
	atExecuteScript,
	atExecuteCommand,
	atCallMethod
};
//--------------------------------------------------------------------------------------------------------
enum ParameterType
{
	ptValue,
};
//--------------------------------------------------------------------------------------------------------
class MenuItemAction
{
	string m_sAction;
	ActionType m_type;
	map<int, string> m_mapParameters;

public:

	MenuItemAction(string sAction, ActionType type);
	MenuItemAction(string sAction, ActionType type, const map<int, string>& mapParameters);
	~MenuItemAction();

	string Description();
	ActionType Type();
	const map<int, string>& Parameters() const;

	void UpdateValueParam(string sValue);
	MenuItemAction *Clone();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_ITEM_ACTION_H__
