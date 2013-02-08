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
#include "WizardPagesFactory.h"
//---------------------------------------------------------------------------
#include <stdlib.h>
//---------------------------------------------------------------------------
#include "WizardWidgetsFactory.h"
#include "GenerateWizardConfigDefaults.h"
//---------------------------------------------------------------------------
//classes for factory
#include "WizardPageWelcome.h"
#include "WizardPageVideoResolution.h"

#include "WizardPageUISwitcher.h"
#include "WizardPageUISwitcherTest.h"

#include "WizardPageVideoResolutionConfirm.h"
#include "WizardPageVideoAdjustSize.h"
#include "WizardPageAudioSoundCard.h"
#include "WizardPageAudioConnector.h"
#include "WizardPageAudioVolume.h"
#include "WizardPageDolbyTest.h"
#include "WizardPageDTSTest.h"
#include "WizardPageFinalSelections.h"

WizardPagesFactory* WizardPagesFactory::Instance = NULL;



WizardPagesFactory::WizardPagesFactory()
	: WizardWidgetsFactory()
{
	this->RuningMode = 0;
}

WizardPagesFactory::~WizardPagesFactory(void)
{
}


/*static*/ WizardPagesFactory* WizardPagesFactory::GetInstance()
{
	if(Instance == NULL)
		Instance = new WizardPagesFactory();
	return Instance;
}

void WizardPagesFactory::SetRunMode(int RunMode)
{
	this->RuningMode = RunMode;
}

void WizardPagesFactory::SetSDLFrontEnd(GenericBackEnd* FrontEnd)
{
	this->FrontEnd = FrontEnd;
}

WizardPage* WizardPagesFactory::CreateWizardPage(std::string XMLFile)
{
	SettingsDictionaryTree* DialogSettings = new SettingsDictionaryTree();
	if(!DialogSettings->LoadFromXMLFile(XMLFile))
		return NULL;

	WizardPage* Result = CreateWidget(DialogSettings);
	delete DialogSettings;
	DialogSettings = NULL;

    return Result;
}

//---------------------------------------------------------------------------
WizardPage* WizardPagesFactory::CreateWidget(SettingsDictionaryTree* 
													 DialogSource)
{
	if(DialogSource == NULL)
		return NULL;

	SettingsDictionary* Dictionary = DialogSource->GetDictionary();
	std::string Name = Dictionary->GetName();

	WizardWidgetsFactory*Factory =  WizardWidgetsFactory::GetInstance();
	Factory->SetSDLFrontEnd(FrontEnd);

	WizardWidgetBase* Page = dynamic_cast<WizardWidgetBase*> 
		(Factory->CreateWidget(DialogSource));

	Factory->CleanUp();
	if(Page == NULL)
		return NULL;

	Page->ApplyDictionary(Dictionary);

	WizardPage* Result = new WizardPage(FrontEnd, Name);
	Result->SetPageLayout(dynamic_cast<WizardWidgetPage*>(Page));


	return Result;
}


WizardPage* WizardPagesFactory::CreatePredefinedWizardPage(const std::string Name)
{
	SettingsDictionaryTree* DialogSettings = new SettingsDictionaryTree();
	if(!DialogSettings->LoadFromXMLFile("/tmp/" + Name + ".xml"))
	{
		delete DialogSettings;
		return NULL;
	}

	WizardPage* Result = NULL;

	SettingsDictionary *Dictionary = DialogSettings->GetDictionary();
	std::string PageName = Dictionary->GetName();

	WizardWidgetsFactory *Factory =  WizardWidgetsFactory::GetInstance();
	Factory->SetSDLFrontEnd(FrontEnd);

	WizardWidgetBase* Page = dynamic_cast<WizardWidgetBase*> 
		(Factory->CreateWidget(DialogSettings));

	Factory->CleanUp();
	if(Page == NULL)
		return NULL;

	Page->ApplyDictionary(Dictionary);

	if (Name == "Welcome")
		Result = new WizardPageWelcome(FrontEnd, Name);
	else if (Name == "VideoResolution")
		Result = new WizardPageVideoResolution(FrontEnd, Name);
	else if (Name == "VideoResolutionConfirm")
		Result = new WizardPageVideoResolutionConfirm(FrontEnd, Name);
	else if (Name == "UISwitcher")
		Result = new WizardPageUISwitcher(FrontEnd, Name);
	else if (Name == "UISwitcherTest")
		Result = new WizardPageUISwitcherTest(FrontEnd, Name);
	else if (Name == "VideoAdjustSize")
		Result = new WizardPageVideoAdjustSize(FrontEnd, Name);
	else if (Name == "AudioSoundCard")
		Result = new WizardPageAudioSoundCard(FrontEnd, Name);
	else if (Name == "AudioConnector")
		Result = new WizardPageAudioConnector(FrontEnd, Name);
	else if (Name == "AudioVolume")
		Result = new WizardPageAudioVolume(FrontEnd, Name);
	else if (Name == "DolbyTest")
		Result = new WizardPageDolbyTest(FrontEnd, Name);
	else if (Name == "DTSTest")
		Result = new WizardPageDTSTest(FrontEnd, Name);
	else if (Name == "FinalSelections")
		Result = new WizardPageFinalSelections(FrontEnd, Name);

	Result->SetPageLayout(dynamic_cast<WizardWidgetPage*>(Page));

	return Result;
}
