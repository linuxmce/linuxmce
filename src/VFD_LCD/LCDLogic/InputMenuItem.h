#ifndef __INPUT_MENU_ITEM_H__
#define __INPUT_MENU_ITEM_H__
//--------------------------------------------------------------------------------------------------------
#include "MenuItem.h"
//--------------------------------------------------------------------------------------------------------
class InputMenuItem : public MenuItem
{
	string m_sMask; 
	size_t m_nCaretIndex; 
	size_t m_nOptionIndex;
	const string m_csCaretOptions;

	bool m_bBlinked;

public:

	InputMenuItem(string sDescription, MenuItem *pParent = NULL, ItemType type = itListItem, 
		MenuItemAction *pMenuItemAction = NULL, string sMask = "");
	~InputMenuItem();	

	string Mask();

	string Value();
	string Description();

	void Value(string sValue);

	bool CanGoRight();
	bool CanGoLeft();
	bool CanGoUp();
	bool CanGoDown();

	MenuItem *MoveUp();
	MenuItem *MoveDown();
	MenuItem *MoveRight();
	MenuItem *MoveLeft();
};
//--------------------------------------------------------------------------------------------------------
#endif //__INPUT_MENU_ITEM_H__
