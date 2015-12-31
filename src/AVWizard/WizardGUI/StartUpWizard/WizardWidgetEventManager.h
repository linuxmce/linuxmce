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
#ifndef WizardWidgetEventManager_H_
#define WizardWidgetEventManager_H_
//---------------------------------------------------------------------------
#include "DelegatesEvent.h"
#include <map>
//---------------------------------------------------------------------------
#include "SettingsDictionary.h"
//---------------------------------------------------------------------------
class WizardWidgetBase;
//---------------------------------------------------------------------------
typedef DelegatesEvent<void, void*> WizardWidgetEvent;
//---------------------------------------------------------------------------
/**
 *	WizardWidgetEventManager - class that consists in the XML assigned events 
 *	to a control
 */
class WizardWidgetEventManager
{
	WizardWidgetBase* Control;
	std::map<std::string, WizardWidgetEvent*> CustomEvents;

public:
	WizardWidgetEventManager(WizardWidgetBase* Control);
	virtual ~WizardWidgetEventManager();

	void SetEvents(SettingsDictionary* Dictionary);

	WizardWidgetEvent* GetEvent(std::string Name);
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
