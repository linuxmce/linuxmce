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
#include "WizardWidgetButton.h"
//---------------------------------------------------------------------------
#include "SDL_image.h"
#include "GUIWizardUtils.h"
//---------------------------------------------------------------------------
#include <iostream>
//---------------------------------------------------------------------------

WizardWidgetButton::WizardWidgetButton(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	Expands = true;
	Focused = false;
}

WizardWidgetButton::~WizardWidgetButton()
{
}

void WizardWidgetButton::PaintExpandedButton(
	SDL_Surface *BtnTexture, 
	SDL_Surface *BtnLeft, 
	SDL_Surface *BtnRight)
{
	if(!BtnTexture || !BtnLeft || !BtnRight)
		return;

	int x, y;
	WizardWidgetPage* Page = GetContainerPage();
	Page->SetDefaultFont();

	SDL_Rect Src, Dest;

	int FontWidth = FrontEnd->TextOutWidth(Caption);
	int FontHeight = Page->GetFontHeight();
	int FW2 = FontWidth / 2, FH2 = FontHeight / 2;

	//Left button area
	Src.x = 0; Src.y = 0; Src.w = BtnLeft->w; Src.h = BtnLeft->h;
	Dest.x = Left - FW2 - Src.w; Dest.y = Top - FontHeight; Dest.w = Src.w; Dest.h = Src.h; 
	FrontEnd->Blit(BtnLeft, Src, Dest);

	//Right button area
	Src.x = 0; Src.y = 0; Src.w = BtnRight->w; Src.h = BtnRight->h;
	Dest.x = Left + FW2; Dest.y = Top - FontHeight; Dest.w = Src.w; Dest.h = Src.h; 
	FrontEnd->Blit(BtnRight, Src, Dest);

	Width = Src.w*2;

	//Texture area
	int TextureLeft = Left - FW2;
	int TextureTop = Top - FontHeight;
	int XSteps = FontWidth/BtnTexture->w + 1;
	int YSteps = BtnLeft->h/BtnTexture->h + 1;
	for(x = 0; x < XSteps; x++)
		for(y = 0; y < YSteps; y++)
		{
			SDL_Rect Src, Dest;
			Src.x = 0; Src.y = 0;
			if (x == XSteps-1)
				Src.w = FontWidth % BtnTexture->w;
			else
				Src.w = BtnTexture->w;

			if (y == YSteps-1)
				Src.h = BtnLeft->h % BtnTexture->h;
			else
				Src.h = BtnTexture->h;

			Dest.x = TextureLeft+ x*BtnTexture->w;
			Dest.y = TextureTop + y*BtnTexture->h;
			Dest.w = Src.w;	Dest.h = Src.h;
			FrontEnd->Blit(BtnTexture, Src, Dest);
		}
	TColorDesc* Color = new TColorDesc(32, 32, 32);
	FrontEnd->TextOutput((char*)Caption.c_str(), Left - FW2 + 1, Top - FH2 - 3, *Color, 0);
	delete Color;
	if(!Focused)
		Color = new TColorDesc(127, 127, 127);
	else
		Color = new TColorDesc(255, 255, 255);
	FrontEnd->TextOutput((char*)Caption.c_str(), Left - FW2, Top - FH2 -4, *Color, 0);
	delete Color;
	Width += FontWidth;
}

void WizardWidgetButton::PaintNonExpandedButton(
	SDL_Surface *BtnTexture, 
	SDL_Surface *BtnLeft, 
	SDL_Surface *BtnRight)
{
	int x, y;
	WizardWidgetPage* Page = GetContainerPage();
	Page->SetDefaultFont();

	SDL_Rect Src, Dest;

	int FontWidth = Width / 2;
	int FontHeight = Page->GetFontHeight();
	int FW2 = FontWidth / 2, FH2 = FontHeight / 2;

	//Left button area
	if(BtnLeft == NULL)
	{
		std::cout<<"Warning: there is no surface setup for left part of button!" <<std::endl;
	}
	else
	{
		Src.x = 0; Src.y = 0; Src.w = BtnLeft->w; Src.h = BtnLeft->h;
		Dest.x = Left - FW2 - Src.w; Dest.y = Top - FontHeight; Dest.w = Src.w; Dest.h = Src.h; 
		FrontEnd->Blit(BtnLeft, Src, Dest);
	}

	//Right button area
	if(BtnRight == NULL)
	{
		std::cout<<"Warning: there is no surface setup for right part of button!" <<std::endl;
	}
	else
	{
		Src.x = 0; Src.y = 0; Src.w = BtnRight->w; Src.h = BtnRight->h;
		Dest.x = Left + FW2; Dest.y = Top - FontHeight; Dest.w = Src.w; Dest.h = Src.h; 
		FrontEnd->Blit(BtnRight, Src, Dest);
	}

	//Texture area
	int TextureLeft = Left - FW2;
	int TextureTop = Top - FontHeight;
	//Right button area
	if(BtnTexture == NULL)
	{
		std::cout<<"Warning: there is no surface setup for texture part of button!" <<std::endl;
		return;
	}

	int XSteps = FontWidth/BtnTexture->w + 1;
	int YSteps = BtnLeft->h/BtnTexture->h + 1;
	for(x = 0; x < XSteps; x++)
		for(y = 0; y < YSteps; y++)
		{
			SDL_Rect Src, Dest;
			Src.x = 0; Src.y = 0;
			if (x == XSteps-1)
				Src.w = FontWidth % BtnTexture->w;
			else
				Src.w = BtnTexture->w;

			if (y == YSteps-1)
				Src.h = BtnLeft->h % BtnTexture->h;
			else
				Src.h = BtnTexture->h;

			Dest.x = TextureLeft+ x*BtnTexture->w;
			Dest.y = TextureTop + y*BtnTexture->h;
			Dest.w = Src.w;	Dest.h = Src.h;
			FrontEnd->Blit(BtnTexture, Src, Dest);
		}

	TColorDesc* Color = new TColorDesc(32, 32, 32);
	FrontEnd->TextOutput((char*)Caption.c_str(), Left+1, Top - FH2-3, *Color, 1);
	delete Color;
	if(!Focused)
		Color = new TColorDesc(127, 127, 127);
	else
		Color = new TColorDesc(255, 255, 255);

	FrontEnd->TextOutput((char*)Caption.c_str(), Left, Top - FH2-4, *Color, 1);
	delete Color;

}

/*virtual*/ void WizardWidgetButton::Paint()
{
	SDL_Surface* BtnTexture, *BtnLeft, *BtnRight; 
	WizardWidgetPage* Page = GetContainerPage();
	if(!Focused)
	{
		BtnTexture = Page->GetSurface(SURFACE_MAIN);
		BtnLeft = Page->GetSurface(SURFACE_LEFT);
		BtnRight = Page->GetSurface(SURFACE_RIGHT);
	}
	else
	{
		BtnTexture = Page->GetSurface(SURFACE_HIGH_MAIN);
		BtnLeft = Page->GetSurface(SURFACE_HIGH_LEFT);
		BtnRight = Page->GetSurface(SURFACE_HIGH_RIGHT);
	}
	
	if (Expands)
	{
		PaintExpandedButton(BtnTexture, BtnLeft, BtnRight);
	}
	else
	{
		PaintNonExpandedButton(BtnTexture, BtnLeft, BtnRight);
	}
}

/*virtual*/ void WizardWidgetButton::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Expands")
		Expands = Utils::StringToInt32(DataValue) != 0;
	else
	if(Attribute == "Focused")
		Focused = Utils::StringToInt32(DataValue) != 0;
	else
	if(Attribute == "Caption")
		Caption = DataValue;
	else
		WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}

WizardWidgetPage* WizardWidgetButton::GetContainerPage()
{
	WizardWidgetBase* Result = this;
	while(Result->Parent != NULL)
		Result = Result->Parent;

	WizardWidgetPage* Page = (WizardWidgetPage*) Result;
	return Page;
}

/*virtual*/ void WizardWidgetButton::SetFocus(bool IsFocused)
{
	Focused = IsFocused;
}

/*virtual*/ std::string WizardWidgetButton::GetCaption()
{
	return Caption;
}

/*virtual*/ void WizardWidgetButton::SetCaption(std::string Caption)
{
	this->Caption = Caption;
}
