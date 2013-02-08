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
#include "WizardPageDolbyTest.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "WizardWidgetLabel.h"

WizardPageDolbyTest::WizardPageDolbyTest(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::WizardPageDolbyTest"<<std::endl;
#endif
	Buttons["No, I cannot hear it"] = 1;
	Buttons["Yes, I can hear it"] = 2;
	OutputValue = "Yes, I can hear it";
	Selected = NULL;
#ifndef WIN32
	Player = NULL;
#endif
}

WizardPageDolbyTest::~WizardPageDolbyTest(void)
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::WizardPageDolbyTest"<<std::endl;
#endif
#ifndef WIN32
	Player->StopPlayerEngine();
#endif
}

/*virtual*/ int WizardPageDolbyTest::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::WizardPageDolbyTest"<<std::endl;
#endif
	if((Dictionary == NULL) || (Selected == NULL))
		return -1;
	OutputValue = Selected->GetCaption();
	Dictionary->Set("DolbyTest", OutputValue!= "No, I cannot hear it");
	return 0;
}

/*virtual*/ void WizardPageDolbyTest::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::DefaultSetup"<<std::endl;
#endif
	std::string FileName, ConfigName;
#ifndef WIN32
	Player = XinePlayer::GetInstance();
#endif
	if(AVWizardSettings->Exists("XineConfigFile"))
	{
		ConfigName = AVWizardSettings->GetValue("XineConfigFile");

		if(AVWizardSettings->Exists("DolbyTestFile"))
			FileName = AVWizardSettings->GetValue("DolbyTestFile");
#ifndef WIN32
		if(FileName != "")
			Player->InitPlayerEngine(ConfigName, FileName);
#endif
	}

	if(FileName != "")
	{
		FILE* f = fopen(FileName.c_str(), "r");
		if(NULL != f)
		{
			fclose(f);
			WizardWidgetLabel* Label = NULL;
			Label = dynamic_cast<WizardWidgetLabel*>(Page->GetChildRecursive("DescribeText3"));
			Label->SetUpAttribute( "Visible", "0");
			Label = (WizardWidgetLabel*)Page->GetChildRecursive("DescribeText4");
			Label->SetUpAttribute( "Visible", "0");
		}
	}

	if(!AVWizardSettings->Exists("DolbyTest"))
	{
		SearchSelectedItem();
		return;
	}
	std::string Value = AVWizardSettings->GetValue("DolbyTest");
	if (Utils::StringToInt32(Value.c_str()) != 0)
		OutputValue = "Yes, I can hear it";
	else
		OutputValue = "No, I cannot hear it";

	SearchSelectedItem();
}

/*virtual*/ void WizardPageDolbyTest::DoIncreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::DoIncreaseSetting"<<std::endl;
#endif
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText.c_str());
	if(ButtonIndex == 2)
		return;
	Selected->SetFocus(false);
	ButtonIndex++;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageDolbyTest::DoDecreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::DoDecreaseSetting"<<std::endl;
#endif
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText.c_str());
	if(ButtonIndex == 1)
		return;
	Selected->SetFocus(false);
	ButtonIndex--;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageDolbyTest::SearchSelectedItem()
{
#ifdef DEBUG
	std::cout<<"WizardPageDolbyTest::SearchSelectedItem()"<<std::endl;
#endif
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageDolbyTest::DoClickWidget(WizardWidgetBase *pWidget)
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

/*virtual*/ bool WizardPageDolbyTest::IsDisplayable()
{
	Wizard * pWizard = Wizard::GetInstance();
	SettingsDictionaryTree * AVWizardOptions = pWizard->AVWizardOptions;
	std::string NoAudioDevice = "0";
	if (AVWizardOptions->GetDictionary()->Exists("NoAudioDevice"))
		NoAudioDevice = AVWizardOptions->GetDictionary()->GetValue("NoAudioDevice");
	return Utils::StringToInt32(NoAudioDevice) == 0 && !pWizard->GetAnalogSoundMode();
}
