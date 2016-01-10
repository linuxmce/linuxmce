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
#ifndef WizardWidgetBaseH
#define WizardWidgetBaseH
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <map>
#include <memory>
//---------------------------------------------------------------------------
#include "SDLFrontEnd.h"
#include "SettingsDictionary.h"
#include "WizardWidgetEventManager.h"
//---------------------------------------------------------------------------
/**
 *  Base widget using in the wizard forms
 */
class WizardWidgetBase
{
protected:
	std::string Name;
	std::map<std::string, std::string> Attributes;
	bool Visible;
	GenericBackEnd* FrontEnd;
public:
	std::auto_ptr<WizardWidgetEventManager> EventManager;

	int Top, Left, Width, Height;

	/**
     *	Parent control, may be used to attach on different events to be sure that on
     *	it's events to be handled well by current component (like OnClose)
     */
	WizardWidgetBase* Parent;
    /**
     *	Children components to the current control
     */
	std::vector<WizardWidgetBase*> Children;
	/**
	 *	Constructor
	 */
	WizardWidgetBase(GenericBackEnd* FrontEnd, std::string Name);
	/**
	 *	Virtual destructor
	 */
	virtual ~WizardWidgetBase();
    /**
     *	Set the parent and adds itself from childrens of the parent
     */
	void SetParent(WizardWidgetBase* Parent);
    /**
     *	Delete specified control from childrens
     */
	void DeleteChild(WizardWidgetBase* Child);
    /**
     *	Add a child control to the current children
     */
	void AddChild(WizardWidgetBase* Child);

	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
	/**
	 *	Method that overwrites the attributes of the current control
	 */
	virtual void ApplyAttributes();
	/**
	 *	Paints the control and the children of the curent component
	 */
    virtual void Paint();
    /**
     *	Test if exists one attribute in config file (which defines the dialog)
     *	It should be overwritten to ignore some items or to treat as always existing
     *	some controls with default behaviour
     */
    virtual bool ExistAttribute(std::string Name);

	string GetAttribute(std::string Name);
	/**
	 *	Set up the attributes from a specified dictionary	
	 */
	virtual bool ApplyDictionary(SettingsDictionary* DefinitionDictionary);
	/**
	 *	Get the child control that match the name, else return NULL
	 */
	virtual WizardWidgetBase* GetChild(std::string ChildName);
	/**
	 *	Get the child control in all tree of children that match the name, else return NULL
	 */
	virtual WizardWidgetBase* GetChildRecursive(std::string ChildName);
	/**
	 *	Get the name of the control
	 */
	std::string GetName();
};
//---------------------------------------------------------------------------
#endif
