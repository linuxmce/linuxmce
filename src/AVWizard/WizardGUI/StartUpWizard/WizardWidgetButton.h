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
#ifndef WizardWidgetButton_H_
#define WizardWidgetButton_H_
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "WizardWidgetPage.h"
#include <SDL.h>
//---------------------------------------------------------------------------
class WizardWidgetButton : public WizardWidgetBase
{
protected:	
	/**
	 *	Describe drawing model, if button expands at the text size or doesn't
	 */
	bool Expands;
	/**
	 *	Say if the button is focused or it doesn't	
	 */
	bool Focused;

	/**
	 *	The containing text of the button	
	 */
	std::string Caption;

	void SetSurface(std::string FileName, int IndexSurface);

	/**
	 *	Paint expanded button, it gets the text size
	 */
	void PaintExpandedButton(
		SDL_Surface* BtnTexture, 
		SDL_Surface *BtnLeft, 
		SDL_Surface *BtnRight);
	/**
	 *	Paint unexpanded button, it gets the width size as reference
	 */
	void PaintNonExpandedButton(
		SDL_Surface* BtnTexture, 
		SDL_Surface *BtnLeft, 
		SDL_Surface *BtnRight);

protected:
	WizardWidgetPage* GetContainerPage();
public:
	WizardWidgetButton(GenericBackEnd* FrontEnd, std::string Name);
	~WizardWidgetButton();

	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();
	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
	/**
	 *	Say if button has focus or doesn't
	 */
	virtual void SetFocus(bool IsFocused);

	virtual std::string GetCaption();

	virtual void SetCaption(std::string Caption);

};

#endif
