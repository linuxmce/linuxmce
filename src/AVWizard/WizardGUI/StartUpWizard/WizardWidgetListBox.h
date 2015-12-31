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
#ifndef WizardWidgetListBox_H_
#define WizardWidgetListBox_H_
//---------------------------------------------------------------------------
#include "WizardWidgetButton.h"
#include "WizardWidgetPage.h"
#include <SDL.h>
//---------------------------------------------------------------------------
class WizardWidgetListBox : public WizardWidgetButton
{
	/**
	 *	The containing text of the button	
	 */
	std::string Caption;

	bool DisplayLeftArrow, DisplayRightArrow;

	void SetSurface(std::string FileName, int IndexSurface);


public:
	WizardWidgetListBox(GenericBackEnd* FrontEnd, std::string Name);
	~WizardWidgetListBox();

	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();
	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
	/**
	 *	Say if listbox has focus or doesn't
	 */
	virtual void SetFocus(bool IsFocused);

	virtual void DisplayArrows(bool Left, bool Right);

};

#endif
