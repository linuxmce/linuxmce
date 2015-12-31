#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <list>
using namespace std;
//--------------------------------------------------------------------------------------------------------
class MenuItemAction;
//--------------------------------------------------------------------------------------------------------
enum ItemType
{
	itStatusItem,
	itListItem,
	itInputBox,
	itExpandItem
};
//--------------------------------------------------------------------------------------------------------
class MenuItem
{
protected:

	ItemType m_type;
	string m_sDescription;
	string m_sStatus;
	string m_sValue;
	MenuItem *m_pParent;
	vector<MenuItem *> m_vectChildren;
	MenuItemAction *m_pMenuItemAction;

public:

	MenuItem(string sDescription, MenuItem *pParent = NULL, ItemType type = itListItem, MenuItemAction *pMenuItemAction = NULL);
	virtual ~MenuItem();

	virtual string Value();
	virtual void Value(string sValue);

	virtual string Description();
	MenuItemAction *Action();
	ItemType Type();

	void Status(string sStatus);
	string Status();

	bool IsLeaf();

	MenuItem *Parent();
	void Parent(MenuItem *pParent);

	const vector<MenuItem *>& Children() const;
	void AddChild(MenuItem *pMenuItem);

	MenuItem *FirstChild();
	MenuItem *NextChild(MenuItem *pChildMenuItem);
	MenuItem *PrevChild(MenuItem *pChildMenuItem);

	virtual bool CanGoRight();
	virtual bool CanGoLeft();
	virtual bool CanGoUp();
	virtual bool CanGoDown();

	virtual MenuItem *MoveUp();
	virtual MenuItem *MoveDown();
	virtual MenuItem *MoveRight();
	virtual MenuItem *MoveLeft();

	virtual MenuItem *Clone();

	void Expand(MenuItem *pChildMenuItem, const list<MenuItem *>& listExpandedItems);
	void ReplaceVariable(string sVariableName, string sValue);
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_ITEM_H__
