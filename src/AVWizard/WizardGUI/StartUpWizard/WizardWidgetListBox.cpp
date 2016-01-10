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
//---------------------------------------------------------------------------
#include "WizardWidgetListBox.h"
//---------------------------------------------------------------------------

WizardWidgetListBox::WizardWidgetListBox(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetButton(FrontEnd, Name)
{
	
}

WizardWidgetListBox::~WizardWidgetListBox()
{

}

/*virtual*/ void WizardWidgetListBox::Paint()
{
	WizardWidgetButton::Paint();
	WizardWidgetPage* Page = WizardWidgetButton::GetContainerPage();
	SDL_Surface* LeftArrow, *RightArrow;

	if(!Focused)
	{
		LeftArrow = NULL;
		RightArrow = NULL;
		//LeftArrow = Page->GetSurface(SURFACE_ARROW_LEFT);
		//RightArrow = Page->GetSurface(SURFACE_ARROW_RIGHT);
	}
	else
	{
		LeftArrow = Page->GetSurface(SURFACE_ARROW_LEFT);
		RightArrow = Page->GetSurface(SURFACE_ARROW_RIGHT);
	}

	if(LeftArrow == NULL)
		return;
	if(RightArrow == NULL)
		return;

	SDL_Rect Src, Dest;
	//Left button area
	Src.x = 0; Src.y = 0; Src.w = RightArrow->w; Src.h = RightArrow->h;
	Dest.x = Left - Width/2 - Src.w - 22; Dest.y = Top - Page->GetFontHeight(); Dest.w = Src.w; Dest.h = Src.h; 
	FrontEnd->Blit(LeftArrow, Src, Dest);

	//Right button area
	Src.x = 0; Src.y = 0; Src.w = RightArrow->w; Src.h = RightArrow->h;
	Dest.x = Left + Width/2 + 22; Dest.y = Top - Page->GetFontHeight(); Dest.w = Src.w; Dest.h = Src.h; 
	FrontEnd->Blit(RightArrow, Src, Dest);
}


/*virtual*/ void WizardWidgetListBox::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	WizardWidgetButton::SetUpAttribute(Attribute, DataValue);
}

/*virtual*/ void WizardWidgetListBox::SetFocus(bool IsFocused)
{
	WizardWidgetButton::SetFocus(IsFocused);
}


/*virtual*/ void WizardWidgetListBox::DisplayArrows(bool Left, bool Right)
{
	this->DisplayLeftArrow = Left;
	this->DisplayRightArrow = Right;
}
