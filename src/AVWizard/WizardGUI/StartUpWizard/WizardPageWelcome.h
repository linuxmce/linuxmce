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
#ifndef WizardPageWelcome_H_
#define WizardPageWelcome_H_

#include "WizardPage.h"

#include "WizardWidgetListBox.h"

class WizardPageWelcome :
	public WizardPage
{
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DoCancelSetting();
	virtual void DoClickWidget(WizardWidgetBase * pWidget);
	virtual void DoNumberKey(int KeyCode);

	WizardPageWelcome(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageWelcome();
};

#endif //WizardPageWelcome_H_
