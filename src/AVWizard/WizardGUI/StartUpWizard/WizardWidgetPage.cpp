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
#include "WizardWidgetPage.h"
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"
#include <SDL_image.h>
//---------------------------------------------------------------------------

WizardWidgetPage::WizardWidgetPage(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	for(int i = 0; i<10; i++)
		ButtonSurfaces[i] = NULL;

	Font_Color = Utils::StringToColorDesc("000000");;
}

WizardWidgetPage::~WizardWidgetPage()
{
	for(int i = 0; i<6; i++)
		if(ButtonSurfaces[i] == NULL)
		{
			SDL_FreeSurface(ButtonSurfaces[i]);
			ButtonSurfaces[i] = NULL;
		}
	delete Font_Color;
}

void WizardWidgetPage::SetSurface(std::string FileName, int IndexSurface)
{
	SDL_Surface* Surface = IMG_Load(FileName.c_str());
	if(Surface== NULL)
		return;
	if(ButtonSurfaces[IndexSurface] != NULL)
	{
		SDL_FreeSurface(ButtonSurfaces[IndexSurface]);
		ButtonSurfaces[IndexSurface] = NULL;
	}

	ButtonSurfaces[IndexSurface] = Surface;
}


/*virtual*/ void WizardWidgetPage::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Picture")
		SetSurface(DataValue, SURFACE_MAIN);
	else
	if(Attribute == "PictureLeft")
		SetSurface(DataValue, SURFACE_LEFT);
	else
	if(Attribute == "PictureRight")
		SetSurface(DataValue, SURFACE_RIGHT);
	else
	if(Attribute == "PictureHigh")
		SetSurface(DataValue, SURFACE_HIGH_MAIN);
	else
	if(Attribute == "PictureHighLeft")
		SetSurface(DataValue, SURFACE_HIGH_LEFT);
	else
	if(Attribute == "PictureHighRight")
		SetSurface(DataValue, SURFACE_HIGH_RIGHT);
	else
	if(Attribute == "PictureArrowLeft")
		SetSurface(DataValue, SURFACE_ARROW_LEFT);
	else
	if(Attribute == "PictureArrowRight")
		SetSurface(DataValue, SURFACE_ARROW_RIGHT);
	else
	if(Attribute == "PictureHighArrowLeft")
		SetSurface(DataValue, SURFACE_HIGH_ARROW_LEFT);
	else
	if(Attribute == "PictureHighArrowRight")
		SetSurface(DataValue, SURFACE_HIGH_ARROW_RIGHT);
	else
	if(Attribute == "FontName")
	{
		Font_Name = DataValue;
	}
	else
	if(Attribute == "FontHeight")
	{
		FontHeight = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Style")
	{
		if (DataValue == "Regular")
		{
			Style = 0;
		}
		else
		if (DataValue == "Bold")
		{
			Style = TTF_STYLE_BOLD;
		}
		else
		if (DataValue == "Italic")
		{
			Style = TTF_STYLE_ITALIC;
		}
		else
		if (DataValue == "Bold-Italic")
		{
			Style = TTF_STYLE_ITALIC | TTF_STYLE_BOLD;
		}
		else
			Style = 0;
	}
	else
	if(Attribute == "FontColor")
	{
		if(Font_Color)
			delete Font_Color;
		Font_Color = Utils::StringToColorDesc(DataValue);
	}
	if(Attribute == "Caption")
		Caption = DataValue;
}

void WizardWidgetPage::Paint()
{
	WizardWidgetBase::Paint();
}

SDL_Surface* WizardWidgetPage::GetSurface(int IndexSurface)
{
	if (IndexSurface <0)
		return NULL;
	if(IndexSurface>=10)
		return NULL;
	return ButtonSurfaces[IndexSurface];
}

int WizardWidgetPage::GetFontHeight()
{
	return this->FontHeight;
}

std::string WizardWidgetPage::GetFontName()
{
	return this->Font_Name;
}

TColorDesc *WizardWidgetPage::GetFontColor()
{
	return this->Font_Color;
}

/*virtual*/ void WizardWidgetPage::SetDefaultFont()
{
	FrontEnd->SetCurrentFont(Font_Name, FontHeight, 1);
}
