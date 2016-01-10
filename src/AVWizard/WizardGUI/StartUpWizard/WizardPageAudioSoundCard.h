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
//
// C++ Interface: WizardPageAudioSoundCard
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@ciplogic-desktop>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WizardPageAudioSoundCard_H_
#define WizardPageAudioSoundCard_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

class WizardPageAudioSoundCard :
	public WizardPage
{
	std::string OutputValue;

	std::map<std::string, int> Buttons;

	WizardWidgetButton* Selected;

public:
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	WizardPageAudioSoundCard(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageAudioSoundCard();
};

#endif 
