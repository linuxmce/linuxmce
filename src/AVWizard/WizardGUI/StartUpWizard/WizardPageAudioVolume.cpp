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
#include "WizardPageAudioVolume.h"
#include "WizardWidgetImage.h"
#include "WizardWidgetButton.h"

#include "GUIWizardUtils.h"
#include "Wizard.h"

#include <stdlib.h>
#include <iostream>

WizardPageAudioVolume::WizardPageAudioVolume(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::WizardPageAudioVolume"<<std::endl;
#endif
	AudioVolumeMin = 0;
	AudioVolumeMax = 31;
	AudioVolumeCurrent = 15;
	AudioVolumeIncrement = 1;
#ifndef WIN32
	Player = NULL;
#endif
}

WizardPageAudioVolume::~WizardPageAudioVolume()
{
#ifdef DEBUG
	std::cout<<"dtor WizardPageAudioVolume"<<std::endl;
#endif
#ifndef WIN32
	Player->StopPlayerEngine();
#endif
}


int WizardPageAudioVolume::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::WizardPageAudioVolume"<<std::endl;
#endif
	if(Dictionary == NULL)
		return -1;
	Dictionary->Set("AudioVolumeMin", AudioVolumeMin);
	Dictionary->Set("AudioVolumeMax", AudioVolumeMax);
	Dictionary->Set("AudioVolumeCurrent", AudioVolumeCurrent);
	Dictionary->Set("AudioVolumeIncrement", AudioVolumeIncrement);
	return 0;
}

/*virtual*/ void WizardPageAudioVolume::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::DefaultSetup"<<std::endl;
#endif
	std::string FileName, ConfigName;
#ifndef WIN32
	Player = XinePlayer::GetInstance();
#endif
	if(AVWizardSettings->Exists("XineConfigFile"))
	{
		ConfigName = AVWizardSettings->GetValue("XineConfigFile");

	}
	
	if(AVWizardSettings->Exists("SoundTestFile"))
		FileName = AVWizardSettings->GetValue("SoundTestFile");
#ifndef WIN32
	if(FileName != "")
		Player->InitPlayerEngine(ConfigName, FileName);
#endif

	if(FileName != "")
	{
		FILE* f = fopen(FileName.c_str(), "r");
		if(NULL != f)
		{
			fclose(f);
			WizardWidgetLabel* Label = NULL;
			Label = dynamic_cast<WizardWidgetLabel*>(Page->GetChildRecursive("DescribeText2"));
			Label->SetUpAttribute( "Visible", "0");
			Label = (WizardWidgetLabel*)Page->GetChildRecursive("DescribeText3");
			Label->SetUpAttribute( "Visible", "0");
		}
	}
	if(!AVWizardSettings->Exists("AudioVolumeMin"))
		AudioVolumeMin = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMin"));

	if(AVWizardSettings->Exists("AudioVolumeMax"))
		AudioVolumeMax = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMax"));

	if(AVWizardSettings->Exists("AudioVolumeCurrent"))
		AudioVolumeCurrent = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeCurrent"));

	if(AVWizardSettings->Exists("AudioVolumeIncrement"))
		AudioVolumeIncrement = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeIncrement"));

	int VolumePercent = (AudioVolumeCurrent - AudioVolumeMin) * 100 / (AudioVolumeMax - AudioVolumeMin);
	std::string VolumeLabelCaption = Utils::Int32ToString(VolumePercent) + "%";

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeTextShadow"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	VolumeLabel->SetCaption(VolumeLabelCaption);

	std::string Command = "/usr/pluto/bin/AVWizard_UpdateVolume.sh " + Utils::Int32ToString(VolumePercent);
	system(Command.c_str());
}

void WizardPageAudioVolume::DoIncreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::DoIncreaseSetting"<<std::endl;
#endif
	int NewVolume = AudioVolumeCurrent + AudioVolumeIncrement;
	if (NewVolume > AudioVolumeMax)
		NewVolume = AudioVolumeMax;

	AudioVolumeCurrent = NewVolume;

	int VolumePercent = (AudioVolumeCurrent - AudioVolumeMin) * 100 / (AudioVolumeMax - AudioVolumeMin);
	std::string VolumeLabelCaption = Utils::Int32ToString(VolumePercent) + "%";

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeTextShadow"));
	VolumeLabel->SetCaption(VolumeLabelCaption);

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	VolumeLabel->SetCaption(VolumeLabelCaption);

	std::string Command = "/usr/pluto/bin/AVWizard_UpdateVolume.sh " + Utils::Int32ToString(VolumePercent);
	system(Command.c_str());
}

void WizardPageAudioVolume::DoDecreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::DoDecreaseSetting"<<std::endl;
#endif
	int NewVolume = AudioVolumeCurrent - AudioVolumeIncrement;
	if (NewVolume < AudioVolumeMin)
		NewVolume = AudioVolumeMin;
	AudioVolumeCurrent = NewVolume;

	int VolumePercent = (AudioVolumeCurrent - AudioVolumeMin) * 100 / (AudioVolumeMax - AudioVolumeMin);
	std::string VolumeLabelCaption = Utils::Int32ToString(VolumePercent) + "%";

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeTextShadow"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
	
	std::string Command = "/usr/pluto/bin/AVWizard_UpdateVolume.sh " + Utils::Int32ToString(VolumePercent);
	system(Command.c_str());
}

/*virtual*/ void WizardPageAudioVolume::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetImage* ClickedImage = NULL;
	WizardWidgetButton* ClickedButton = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			ClickedImage = dynamic_cast<WizardWidgetImage*>(pWidget);
			ClickedButton = dynamic_cast<WizardWidgetButton*>(pWidget);

			SDL_Event Event;
			if (ClickedImage)
			{	
				if (ClickedImage->GetName() == "VolumeLeft")
				{
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_LEFT;
					SDL_PushEvent(&Event);
				}
				else if (ClickedImage->GetName() == "VolumeRight")
				{
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_RIGHT;
					SDL_PushEvent(&Event);
				}
			}
			else if (ClickedButton)
			{
				if (ClickedButton->GetName() == "BtnOK")
				{
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_RETURN;
					SDL_PushEvent(&Event);
				}
			}
		}
	}
}

/*virtual*/ bool WizardPageAudioVolume::IsDisplayable()
{
	SettingsDictionaryTree * AVWizardOptions = Wizard::GetInstance()->AVWizardOptions;
	if (AVWizardOptions->GetDictionary()->Exists("NoAudioDevice"))
	{
		std::string NoAudioDevice = AVWizardOptions->GetDictionary()->GetValue("NoAudioDevice");
		return Utils::StringToInt32(NoAudioDevice) == 0;
	}
	return true;
}
