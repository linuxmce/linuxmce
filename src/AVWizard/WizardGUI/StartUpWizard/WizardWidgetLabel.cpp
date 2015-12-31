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
#include "WizardWidgetLabel.h"
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"
#include <SDL_ttf.h>
//---------------------------------------------------------------------------

WizardWidgetLabel::WizardWidgetLabel(GenericBackEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	Font_Color = Utils::StringToColorDesc("000000");
	Align = AlignLeft;
	Style = 0;
}

WizardWidgetLabel::~WizardWidgetLabel()
{
	delete Font_Color;
}

/*virtual*/ void WizardWidgetLabel::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "FontHeight")
	{
		Font_Height = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Caption")
	{
		Caption = DataValue;
	}
	else
	if(Attribute == "FontName")
	{
		Font_Name = DataValue;
	}
	else
	if(Attribute == "Align")
	{
		if (DataValue == "Left")
		{
			Align = AlignLeft;
		}
		else
		if (DataValue == "Center")
		{
			Align = AlignCenter;
		}
		else
		if (DataValue == "Right")
		{
			Align = AlignRight;
		}
		else
			Align = AlignLeft;
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
	else
        WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}

/*virtual*/ void WizardWidgetLabel::Paint()
{
	if(!Visible)
		return;
	FrontEnd->SetCurrentFont(Font_Name, Font_Height, Style);
	FrontEnd->TextOutput((char*)Caption.c_str(), Left, Top, *Font_Color, Align);
	WizardWidgetBase::Paint();
}

/*virtual*/ void WizardWidgetLabel::SetCaption(std::string Caption)
{
	this->Caption = Caption;
}
