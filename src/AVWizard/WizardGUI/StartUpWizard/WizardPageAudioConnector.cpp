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
#include "WizardPageAudioConnector.h"

#include "GUIWizardUtils.h"
#include "ConfigureCommons.h"

#include "SkinGenerator.h"
#include "Wizard.h"

#include <iostream>

WizardPageAudioConnector::WizardPageAudioConnector(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::WizardPageAudioConnector"<<std::endl;
#endif
	Buttons["Analog Stereo"] = 1;
	Buttons["SPDIF Coaxial"] = 2;
	Buttons["SPDIF Optical"] = 3;
	Buttons["HDMI"] = 4;
	Buttons["No sound"] = 5;
	OutputValue = "SPDIF Optical";
	Selected = NULL;
}

WizardPageAudioConnector::~WizardPageAudioConnector(void)
{
}


/*virtual*/ int WizardPageAudioConnector::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DoApplySetting"<<std::endl;
#endif
	if(Dictionary == NULL)
		return -1;
	OutputValue = Selected->GetCaption();
	Dictionary->Set("AudioConnector", OutputValue);
	Wizard::GetInstance()->SetAnalogSoundMode(Buttons[OutputValue] == 1);
	if(Buttons[OutputValue] == 5)
		Dictionary->Set("NoAudioDevice", 1);
	else
	{
		std::string Command = SkinGenerator::Instance()->CommandSetAudioConnector;
		
		Command = Command + " '" + Dictionary->GetValue("AudioConnector") + "' '" + Dictionary->GetValue("SoundCard") + "'";
		system(Command.c_str());
		Dictionary->Set("NoAudioDevice", 0);
	}

	return 0;
}

/*virtual*/ void WizardPageAudioConnector::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DefaultSetup"<<std::endl;
#endif
	if(!AVWizardSettings->Exists("AudioConnector"))
	{
		SearchSelectedItem();
		return;
	}
	OutputValue = AVWizardSettings->GetValue("AudioConnector");
	SearchSelectedItem();
}

/*virtual*/ void WizardPageAudioConnector::DoNextFocusItem()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DoIncreaseSetting"<<std::endl;
#endif
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText);
	if(ButtonIndex == 5)
		return;
	Selected->SetFocus(false);
	ButtonIndex++;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageAudioConnector::DoPreviousFocusItem()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DoDecreaseSetting"<<std::endl;
#endif
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText);
	if(ButtonIndex == 1)
		return;
	Selected->SetFocus(false);
	ButtonIndex--;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageAudioConnector::SearchSelectedItem()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::SearchSelectedItem"<<std::endl;
#endif
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageAudioConnector::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetButton* NewSelectedButton = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			NewSelectedButton = dynamic_cast<WizardWidgetButton*>(pWidget);
			if (NewSelectedButton)
			{
				Selected->SetFocus(false);
				Selected = NewSelectedButton;
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
