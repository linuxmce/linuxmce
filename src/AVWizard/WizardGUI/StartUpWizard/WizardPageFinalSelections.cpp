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
#include "WizardPageFinalSelections.h"

#include "WizardWidgetLabel.h"
#include "GUIWizardUtils.h"

#include "Wizard.h"
#include <iostream>

WizardPageFinalSelections::WizardPageFinalSelections(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::WizardPageFinalSelections()"<<std::endl;
#endif
	Buttons["I do not agree"] = 1;
	Buttons["I agree"] = 2;
	OutputValue = "I agree";
	Selected = NULL;
}

WizardPageFinalSelections::~WizardPageFinalSelections(void)
{
}

/*virtual*/ void WizardPageFinalSelections::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::DefaultSetup()"<<std::endl;
#endif

	if(Wizard::GetInstance()->GetAnalogSoundMode())
		TabStatus(this, 0xC0);
	WizardWidgetLabel*Selected;
	std::string LabelName;
	std::string CaptionValue;
	for(int i = 1; i<=8; i++)
	{
		LabelName = "OptionText"+Utils::Int32ToString(i);
		Selected = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive(LabelName));

		CaptionValue = Utils::Int32ToString(i) + ". ";
		switch(i) {
		case 1:
			CaptionValue = CaptionValue +"Screen ratio: "+ GetVideoRatio(AVWizardSettings);
			break;
		case 2:
			CaptionValue = CaptionValue +"Video output: "+ GetVideoOutput(AVWizardSettings);
			break;
		case 3:
			CaptionValue = CaptionValue +"Video resolution: "+ GetVideoResolution(AVWizardSettings);			
			break;
		case 4:
			CaptionValue = CaptionValue +"Video refresh: "+ GetVideoRefresh(AVWizardSettings);			
			break;
		case 5: 
			CaptionValue = CaptionValue +"Audio connector: "+ GetAudioConnector(AVWizardSettings);
			break;
		case 6:
			CaptionValue = CaptionValue +"Audio volume: "+ GetVolume(AVWizardSettings);
			break;
		case 7: CaptionValue = CaptionValue +"Dolby sound test passed: "
					+ GetDolbyTestStatus(AVWizardSettings);
			break;
		case 8: CaptionValue = CaptionValue +"DTS sound test passed: "
				   + GetDTSTestStatus(AVWizardSettings);
			break;
		default:;
		}
		Selected->SetCaption(CaptionValue);
	}
	SearchSelectedItem();
}

std::string WizardPageFinalSelections::GetVideoRatio(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetVideoRatio()"<<std::endl;
#endif
	
	if (!AVWizardSettings->Exists("Resolution"))
	  {
	    return "Unknown";
	  }
	else if (!AVWizardSettings->Exists("Ratio"))
	  {
	    return "Unknown";
	  }
	else
	  {
	    return AVWizardSettings->GetValue("Ratio");
	  }

}

std::string WizardPageFinalSelections::GetVideoOutput(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetVideoRatio()"<<std::endl;
#endif
	if(!AVWizardSettings->Exists("VideoOutput"))
	{
		return "VGA";
	}
	std::string OutputValue = AVWizardSettings->GetValue("VideoOutput");

	return OutputValue;
}

std::string WizardPageFinalSelections::GetVideoResolution(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetVideoRatio()"<<std::endl;
#endif
	std::string ResolutionStr = "640x480";
	if(AVWizardSettings->Exists("VideoResolution"))
		ResolutionStr =  AVWizardSettings->GetValue("VideoResolution");

	return ResolutionStr;
}

std::string WizardPageFinalSelections::GetVideoRefresh(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetVideoRatio()"<<std::endl;
#endif
	std::string RefreshStr = "60Hz";
	if(AVWizardSettings->Exists("VideoRefresh"))
		RefreshStr =  AVWizardSettings->GetValue("VideoRefresh");

	return RefreshStr;
}

std::string WizardPageFinalSelections::GetAudioConnector(SettingsDictionary* AVWizardSettings)
{
	if(!AVWizardSettings->Exists("AudioConnector"))
	{
#ifdef DEBUG
	std::cout<<"AudioConnector: Toslink"<<std::endl;
#endif
		return "Toslink";
	}
	std::string OutputValue = AVWizardSettings->GetValue("AudioConnector");
#ifdef DEBUG
	std::cout<<"AudioConnector: "<<OutputValue<<std::endl;
#endif
	return OutputValue;
}

std::string WizardPageFinalSelections::GetVolume(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetVideoRatio()"<<std::endl;
#endif
	int AudioVolumeMin, AudioVolumeMax, AudioVolumeCurrent;
	AudioVolumeMin = 0;
	AudioVolumeMax = 31;
	AudioVolumeCurrent = 15;

	std::string OptionName = "AudioVolumeMin";
	if(!AVWizardSettings->Exists(OptionName))
		AudioVolumeMin = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMin"));
#ifdef DEBUG
	std::cout<<"AudioVolumeMin:"<< AudioVolumeMin <<std::endl;
#endif

	if(AVWizardSettings->Exists("AudioVolumeMax"))
		AudioVolumeMax = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMax"));

#ifdef DEBUG
	std::cout<<"AudioVolumeMax:"<< AudioVolumeMax <<std::endl;
#endif

	if(AVWizardSettings->Exists("AudioVolumeCurrent"))
		AudioVolumeCurrent = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeCurrent"));

#ifdef DEBUG
	std::cout<<"AudioVolumeCurrent:"<< AudioVolumeCurrent <<std::endl;
#endif
	return Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";;

}

std::string WizardPageFinalSelections::GetDolbyTestStatus(SettingsDictionary* AVWizardSettings)
{
	std::string Value = AVWizardSettings->GetValue("DolbyTest");

	if (Wizard::GetInstance()->GetAnalogSoundMode())
	{
	#ifdef DEBUG
		std::cout<<"GetDolbyTestStatus: not available - Analog sound selected"<<std::endl;
	#endif
		return "not available";
	}


	int PassDolbyTest = Utils::StringToInt32(Value) != 0;

	if (PassDolbyTest)
	{
#ifdef DEBUG
	std::cout<<"GetDolbyTestStatus: true"<<std::endl;
#endif
		return "true";
	}
	
#ifdef DEBUG
	std::cout<<"GetDolbyTestStatus: false"<<std::endl;
#endif
	return "false";
}

std::string WizardPageFinalSelections::GetDTSTestStatus(SettingsDictionary* AVWizardSettings)
{
	if (Wizard::GetInstance()->GetAnalogSoundMode())
	{
	#ifdef DEBUG
		std::cout<<"GetDolbyTestStatus: not available - Analog sound selected"<<std::endl;
	#endif
		return "not available";
	}


#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetDTSTestStatus()"<<std::endl;
#endif
	std::string Value = AVWizardSettings->GetValue("DTSTest");
	int PassDolbyTest = Utils::StringToInt32(Value) != 0;
	if (PassDolbyTest)
		return "true";
	

	return "false";
}

/*virtual*/ void WizardPageFinalSelections::DoIncreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageFinalSelections::GetVideoRatio()"<<std::endl;
#endif
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText);
	if(ButtonIndex == 2)
		return;
	Selected->SetFocus(false);
	ButtonIndex++;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageFinalSelections::DoDecreaseSetting()
{
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

/*virtual*/ void WizardPageFinalSelections::SearchSelectedItem()
{
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ int WizardPageFinalSelections::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Selected == NULL)
		return 0;

	if(Buttons[Selected->GetCaption()] == 1)
	{
		Wizard::GetInstance()->SetExitWithCode(1);
	}
	return 0;
}

/*virtual*/ void WizardPageFinalSelections::DoClickWidget(WizardWidgetBase *pWidget)
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
