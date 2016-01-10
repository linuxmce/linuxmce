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
#ifndef WizardPageVideoAdjustSize_H_
#define WizardPageVideoAdjustSize_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

class WizardPageVideoAdjustSize :
	public WizardPage
{
	SettingsDictionary* GlobalSettings;

	std::string PreviousResolution;
public:

	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	WizardPageVideoAdjustSize(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoAdjustSize();

	virtual void UpdateSelected();
};

#endif
