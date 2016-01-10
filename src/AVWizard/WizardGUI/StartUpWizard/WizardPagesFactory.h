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
#ifndef WizardPagesFactory_H_
#define WizardPagesFactory_H_

#define WIZARDPAGES_ALL_PAGES 1
#define WIZARDPAGES_ONE_PAGE 2

#include "WizardWidgetsFactory.h"
#include "WizardPage.h"

#include "SDLFrontEnd.h"

class WizardPagesFactory : public WizardWidgetsFactory
{
	static WizardPagesFactory* Instance;
	WizardPagesFactory(void);

	int RuningMode;

	WizardPage* CreateWidget(SettingsDictionaryTree* 
		DialogSource);

public:
	static WizardPagesFactory* GetInstance();
	virtual ~WizardPagesFactory(void);

	void SetRunMode(int RunMode);
	void SetSDLFrontEnd(GenericBackEnd* FrontEnd);

	WizardPage* CreateWizardPage(std::string XMLFile);
	WizardPage* CreatePredefinedWizardPage(std::string Name);
};

#endif
