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
//---------------------------------------------------------------------------
#ifndef WizardWidgetLabelH
#define WizardWidgetLabelH
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
enum FontAlign {
	AlignLeft = 0,
	AlignCenter = 1,
	AlignRight = 2
};
//---------------------------------------------------------------------------

class WizardWidgetLabel : public WizardWidgetBase
{
	std::string Caption;
	int Font_Height;
	int Align, Style;

	std::string Font_Name;
	TColorDesc* Font_Color;
public:
	WizardWidgetLabel(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardWidgetLabel();

	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

	virtual void SetCaption(std::string Caption);

	virtual void Paint();
};
//---------------------------------------------------------------------------
#endif
