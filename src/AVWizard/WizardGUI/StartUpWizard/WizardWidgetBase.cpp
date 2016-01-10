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
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"
//---------------------------------------------------------------------------

WizardWidgetBase::WizardWidgetBase(GenericBackEnd* FrontEnd, std::string Name)
	: Visible(true), Parent(NULL)
{
	EventManager.reset(new WizardWidgetEventManager(this));
    this->Name = Name;
	this->FrontEnd = FrontEnd;
}

WizardWidgetBase::~WizardWidgetBase()
{
    std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); )
	{
        delete* Item;
        Item = Children.erase(Item);
    }
}

void WizardWidgetBase::SetParent(WizardWidgetBase* Parent)
{
	this->Parent = Parent;
	if(Parent == NULL)
		return;
	Parent->AddChild(this);
}

void WizardWidgetBase::DeleteChild(WizardWidgetBase* Child)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
	{
		if (*Item == Child) {
			Children.erase(Item);
			return;
		}
    }
}   

void WizardWidgetBase::AddChild(WizardWidgetBase* Child)
{
	if(Child == NULL)
		return;
	
	Child->Parent = this;
	Children.push_back(Child);
}

void WizardWidgetBase::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Width")
	{
		this->Width = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Height")
	{
		this->Height = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Left")
	{
		this->Left = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Top")
	{
		this->Top = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Visible")
	{
		this->Visible = Utils::StringToInt32(DataValue);
	}

    Attributes[Attribute] = DataValue;
}

/*virtual*/ void WizardWidgetBase::ApplyAttributes()
{
    std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
        (*Item)->ApplyAttributes();

    Attributes.clear();
}

/*virtual*/ void WizardWidgetBase::Paint()
{
    std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
        (*Item)->Paint();
}

/*virtual*/ bool WizardWidgetBase::ExistAttribute(std::string Name)
{
    return Attributes.find(Name) != Attributes.end();
}

string WizardWidgetBase::GetAttribute(std::string Name)
{
	if (!ExistAttribute(Name))
			return "";
	return Attributes[Name];
}

/*virtual*/ bool WizardWidgetBase::ApplyDictionary(SettingsDictionary* DefinitionDictionary)
{
	if(DefinitionDictionary == NULL)
		return false;

	std::map<std::string, std::string>::iterator AttributeItem;
	for(AttributeItem = DefinitionDictionary->Options.begin(); 
			AttributeItem != DefinitionDictionary->Options.end();
			AttributeItem++)
	{
		std::string AttributeName = AttributeItem->first;
		std::string AttributeValue = AttributeItem->second; 
		this->SetUpAttribute(AttributeName, AttributeValue);
	}

	ApplyAttributes();
	return true;
}

/*virtual*/ WizardWidgetBase* WizardWidgetBase::GetChild(std::string ChildName)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
	{
		if ((*Item)->GetName() == ChildName) 
			return *Item;
	}

	return NULL;
}

/*virtual*/ WizardWidgetBase* WizardWidgetBase::GetChildRecursive(std::string ChildName)
{
	WizardWidgetBase* Result = GetChild(ChildName);
	if (Result != NULL)
		return Result;

	std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
	{
		Result = (*Item)->GetChildRecursive(ChildName);
		if(Result != NULL)
			return Result;
	}
	return NULL;
}

std::string WizardWidgetBase::GetName()
{
	return Name;
}
