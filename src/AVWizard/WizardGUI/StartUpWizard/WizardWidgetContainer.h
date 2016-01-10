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
#ifndef WizardWidgetContainer_H_
#define WizardWidgetContainer_H_
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
class WizardWidgetContainer: public WizardWidgetBase
{
	bool Visible;
public:
	WizardWidgetContainer(GenericBackEnd* FrontEnd, std::string Name);
	~WizardWidgetContainer();

	virtual void Paint();
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
};
#endif
