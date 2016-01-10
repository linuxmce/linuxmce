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
#include "WizardPageVideoResolutionConfirm.h"

#include "ConfigureCommons.h"
#include "GUIWizardUtils.h"
#include "SettingsDictionaryTree.h"
#include "Wizard.h"
#include "SkinGenerator.h"
#include "WizardWidgetLabel.h"
#include <iostream>
#include "ThreadSleeper.h"

WizardPageVideoResolutionConfirm::WizardPageVideoResolutionConfirm(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
}

WizardPageVideoResolutionConfirm::~WizardPageVideoResolutionConfirm(void)
{
	ThreadSleeper::Instance()->Quit();
}


/*virtual*/ int WizardPageVideoResolutionConfirm::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;

	if (Selected->GetName() == "BackBtn")
	{
		return 1;
	}

	return 0;
}

/*virtual*/ void WizardPageVideoResolutionConfirm::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	Selected = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("BackBtn"));
	if(Selected == NULL)
	{
		std::cout<<"Warning, no back button!";
		return;
	}
	Selected->SetFocus(true);
	int Seconds = Utils::StringToInt32(SkinGenerator::Instance()->WaitForAcceptResolution);
	if(!Seconds)
		Seconds = 3;

	std::string ResolutionValue;
	std::string RefreshValue;
	if(!AVWizardSettings->Exists("Resolution"))
	{
		std::cerr<<"Warning: Resolution not set at this step!"<<std::endl;
	}
	else
		ResolutionValue = AVWizardSettings->GetValue("Resolution");

	if(!AVWizardSettings->Exists("Refresh"))
	{
		std::cerr<<"Warning: Refresh not set at this step!"<<std::endl;
	}
	else
		RefreshValue = AVWizardSettings->GetValue("Refresh");

	WizardWidgetLabel* Label = dynamic_cast<WizardWidgetLabel*>
		(Page->GetChildRecursive("DescribeText0"));

	std::string LabelValue = "You chose the following resolution: ";

	Label->SetCaption(LabelValue + ResolutionValue+ "@" + RefreshValue + " Hz");


	ThreadSleeper::Instance()->Init(Seconds);
	Label = dynamic_cast<WizardWidgetLabel*>
		(Page->GetChildRecursive("CounterLabel"));
	ThreadSleeper::Instance()->SetLabel(Label);
	
}

/*virtual*/ void WizardPageVideoResolutionConfirm::DoIncreaseSetting()
{
	if(Selected)
		Selected->SetFocus(false);
	Selected = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("ContinueBtn"));
	if(Selected == NULL)
	{
		std::cout<<"Warning, no continue button!";
		return;
	}
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoResolutionConfirm::DoDecreaseSetting()
{
	if(Selected)
		Selected->SetFocus(false);
	Selected = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("BackBtn"));
	if(Selected == NULL)
	{
		std::cout<<"Warning, no back button!";
		return;
	}
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoResolutionConfirm::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetButton* NewSelected = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			NewSelected = dynamic_cast<WizardWidgetButton*>(pWidget);
			if (NewSelected)
			{
				Selected->SetFocus(false);
				Selected = NewSelected;
				Selected->SetFocus(true);

				SDL_Event Event;
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_RETURN;
				SDL_PushEvent(&Event);
			}
		}
	}
}
