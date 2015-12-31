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
#ifndef WizardPageAudioConnector_H_
#define WizardPageAudioConnector_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

#include <map>
#include <string>

class WizardPageAudioConnector :
	public WizardPage
{
	std::string OutputValue;

	std::map<std::string, int> Buttons;

	WizardWidgetButton* Selected;

	void SearchSelectedItem();
public:
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	WizardPageAudioConnector(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageAudioConnector();
};

#endif
