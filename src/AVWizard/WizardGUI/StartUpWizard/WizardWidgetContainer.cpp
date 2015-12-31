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
#include "WizardWidgetContainer.h"
#include "GUIWizardUtils.h"

WizardWidgetContainer::WizardWidgetContainer(GenericBackEnd* FrontEnd, std::string Name)
: WizardWidgetBase(FrontEnd, Name)
{
	this->Visible = true;
}

WizardWidgetContainer::~WizardWidgetContainer()
{

}

/*virtual*/ void WizardWidgetContainer::Paint()
{
	if(!Visible)
		return;
	WizardWidgetBase::Paint();
}

/*virtual*/ void WizardWidgetContainer::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Visible")
		DataValue = (Utils::StringToInt32(DataValue)!= 0);
	else
		WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}
