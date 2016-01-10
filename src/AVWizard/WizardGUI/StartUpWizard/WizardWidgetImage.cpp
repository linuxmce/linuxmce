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
#include "WizardWidgetImage.h"
//---------------------------------------------------------------------------
#include "SDL_image.h"
//---------------------------------------------------------------------------

WizardWidgetImage::WizardWidgetImage(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	Surface = NULL;
	Top = 0;
	Left = 0;
	Width = -1;
	Height = -1;
}

WizardWidgetImage::~WizardWidgetImage()
{
	if(Surface != NULL)
	{
		SDL_FreeSurface(Surface);
		Surface = NULL;
	}
}

/*virtual*/ void WizardWidgetImage::Paint()
{
	if(!Surface)
	{
		WizardWidgetBase::Paint();
		return;
	}
	SDL_Rect Src, Dest;
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

	FrontEnd->Blit(Surface, Src, Dest);
	WizardWidgetBase::Paint();
}

void WizardWidgetImage::SetSurface(std::string FileName)
{
	if(Surface!=NULL)
		SDL_FreeSurface(Surface);

	Surface = IMG_Load(FileName.c_str());
	if(Surface== NULL)
		return;

	Width = Surface->w;
	Height = Surface->h;
}

/*virtual*/ void WizardWidgetImage::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Picture")
		SetSurface(DataValue);
	else
        WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}
