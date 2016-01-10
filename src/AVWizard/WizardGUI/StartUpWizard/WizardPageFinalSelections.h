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
#ifndef WizardPageFinalSelections_H_
#define WizardPageFinalSelections_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"
#include <map>
#include <string>

class WizardPageFinalSelections :
	public WizardPage
{
	std::map <std::string, int> Buttons;
	WizardWidgetButton* Selected;
	std::string OutputValue;

	std::string GetVideoRatio(SettingsDictionary* AVWizardSettings);
	std::string GetVideoOutput(SettingsDictionary* AVWizardSettings);
	std::string GetVideoResolution(SettingsDictionary* AVWizardSettings);
	std::string GetVideoRefresh(SettingsDictionary* AVWizardSettings);
	std::string GetAudioConnector(SettingsDictionary* AVWizardSettings);
	std::string GetVolume(SettingsDictionary* AVWizardSettings);
	std::string GetDolbyTestStatus(SettingsDictionary* AVWizardSettings);
	std::string GetDTSTestStatus(SettingsDictionary* AVWizardSettings);
public:
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	virtual void SearchSelectedItem();

	WizardPageFinalSelections(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageFinalSelections(void);

	void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
};

#endif
