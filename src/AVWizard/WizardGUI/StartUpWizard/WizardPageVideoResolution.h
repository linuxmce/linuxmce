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
#ifndef WizardPageVideoResolution_H_
#define WizardPageVideoResolution_H_

#include "WizardPage.h"

#include "WizardWidgetScrollList.h"

#include <map>
#include <string>

class WizardPageVideoResolution :
	public WizardPage
{
	std::map<std::string, int> Buttons;

	std::string OutputValue;

	WizardWidgetScrollList* Selected;
	int RatioMode;

	void SearchSelectedItem();
	void FillResolutionStandard(WizardWidgetScrollList*, const int);
	void FillRefresh(WizardWidgetScrollList*, const int);

public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	WizardPageVideoResolution(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoResolution(void);

};

#endif
