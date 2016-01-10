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
#include "WizardWidgetBackground.h"
//---------------------------------------------------------------------------
#include "SDL_image.h"
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"

WizardWidgetBackground::WizardWidgetBackground(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	Surface = NULL;
	Top = 0;
	Left = 0;
	Width = -1;
	Height = -1;
	Color = Utils::StringToColorDesc("009D00");
}

WizardWidgetBackground::~WizardWidgetBackground()
{
	if(Surface != NULL)
	{
		SDL_FreeSurface(Surface);
		Surface = NULL;
	}
	delete Color;
}

/*virtual*/ void WizardWidgetBackground::Paint()
{
	SDL_Rect Src, Dest;
	if(!Surface)
	{
		FrontEnd->BackFillColor(Color);
		WizardWidgetBase::Paint();
		return;
	}
	Src.x = 0; Src.y = 0;
	Src.w = Surface->w;
	Src.h = Surface->h;

	Dest.x = Left;
	Dest.y = Top;
	if (Width == -1)
		Dest.w = Surface->w;
	else
        Dest.w = Width;

	if (Height == -1)
		Dest.h = Surface->h;
	else
		Dest.h = Height;

	FrontEnd->BackBlit(Surface, Src, Dest);
	WizardWidgetBase::Paint();
}

void WizardWidgetBackground::SetSurface(std::string FileName)
{
	if(Surface!=NULL)
		SDL_FreeSurface(Surface);

	Surface = IMG_Load(FileName.c_str());
	if(Surface== NULL)
		return;

	Width = Surface->w;
	Height = Surface->h;
}

/*virtual*/ void WizardWidgetBackground::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Picture")
		SetSurface(DataValue);
	else
	if(Attribute == "Color")
	{
		if(Color)
			delete Color;
		Color = Utils::StringToColorDesc(DataValue);
	}
	else
        WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}
