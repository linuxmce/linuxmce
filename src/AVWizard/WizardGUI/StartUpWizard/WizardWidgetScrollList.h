/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//--------------------------------------------------------------
#ifndef WizardWidgetScrollList_H_
#define WizardWidgetScrollList_H_
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "WizardWidgetPage.h"
#include <SDL.h>
#include "ColorDesc.h"
//---------------------------------------------------------------------------
#include<string>
#include<vector>
//---------------------------------------------------------------------------
class WizardWidgetScrollList : public WizardWidgetBase
{
protected:	
	TColorDesc* BackColor;
	TColorDesc* BackFocusedColor;

	TColorDesc* HighBackColor;
	TColorDesc* HighBackFocusedColor;
	bool Focused;

	std::vector<std::string> ItemNames;
	std::vector<std::string> ItemValues;
	std::vector<std::string> ItemRatios;

	void WriteItem(std::string Name, bool Selected,
		int Index, int TopItemIndex, int BottomItemIndex, 
		int ItemHeight);
	int ItemIndex;
protected:
	WizardWidgetPage* GetContainerPage();
public:
	WizardWidgetScrollList(GenericBackEnd* FrontEnd, std::string Name);
	~WizardWidgetScrollList();

	int GetItemIndex();
	void SetItemIndex(int ItemIndex);
	void SetItemIndex(std::string TextToSelect);
	
	void SetFocus(bool IsFocused);
	bool GetFocus();

	std::string GetSelectedValue();
	std::string GetSelectedName();
	std::string GetSelectedRatio();

	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();
	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

	/**
	 */
	virtual void AddItem(std::string ItemName, std::string ItemValue, std::string ItemRatio);
	
	/**
	 */
	virtual void RemoveItem(std::string ItemValue);
	
	/**
	 */
	virtual void Clear();
};

#endif
