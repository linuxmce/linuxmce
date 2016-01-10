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
#include "WizardPageVideoResolution.h"
#include "Wizard.h"
#include "GUIWizardUtils.h"
#include "ConfigureCommons.h"
#include "SkinGenerator.h"
#include "WizardWidgetImage.h"

#include <iostream>
#include "../../../X-Resolution/libresolution/libresolution.h"

WizardPageVideoResolution::WizardPageVideoResolution(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
	std::cout << "WizardPageVideoResolution constructor" << std::endl;
}

WizardPageVideoResolution::~WizardPageVideoResolution(void)
{
	std::cout << "WizardPageVideoResolution destructor" << std::endl;
}

/*virtual*/ int WizardPageVideoResolution::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL || Selected == NULL)
		return -1;
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	std::string ResolutionValue = List->GetSelectedValue();
	std::string ResolutionName  = List->GetSelectedName();
	std::string ResolutionRatio = List->GetSelectedRatio();
	std::cout<<"Selected resolution is: "<< ResolutionValue << "(" << ResolutionName << ")" ;

	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	std::string RefreshValue = List->GetSelectedValue();
	if (RefreshValue == "") {
		RefreshValue = "60";
	}
	std::cout<<"Selected Refresh is: "<<RefreshValue;

	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
	std::string VideoConnectorValue = List->GetSelectedValue();
	int VideoConnectorIndex = List->GetItemIndex();
	std::cout<<"Selected Video Connector is: "<< VideoConnectorValue;

	Dictionary->Set("Resolution", ResolutionValue);
	Dictionary->Set("Refresh", RefreshValue);
	Dictionary->Set("VideoOutput", VideoConnectorValue);
	Dictionary->Set("Ratio",ResolutionRatio);

	std::string Cmd = SkinGenerator::Instance()->CommandChangeResolution + " '" + ResolutionValue
		+ "' '" + RefreshValue + "' '" + VideoConnectorValue +"' '" + ResolutionName +"'";
	std::cout << "\n-------- " << Cmd << std::endl;

	system(Cmd.c_str());

	return 0;
}

/*virtual*/ void WizardPageVideoResolution::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	system(SkinGenerator::Instance()->CommandResetResolution.c_str());
	

	// Fill connector select list
	WizardWidgetScrollList *List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
	List->AddItem("VGA", "VGA","");
	List->AddItem("VGA 2", "VGA-2","");
	List->AddItem("DVI/HDMI", "DVI","");
	List->AddItem("DVI/HDMI 2", "DVI-2","");
	List->AddItem("HDMI", "HDMI-0","");
	List->AddItem("LVDS", "LVDS","");
	List->AddItem("Component", "Component","");
	List->AddItem("Composite", "Composite","");
	List->AddItem("S-Video","S-Video","");
	if(AVWizardSettings->Exists("VideoOutput")) {
		std::string VideoConnectorValue = AVWizardSettings->GetValue("VideoOutput");
		List->SetItemIndex(VideoConnectorValue);
	} else {
		List->SetItemIndex(0);
	}
	List->SetFocus(true);
	Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
	int SelectedConnector = List->GetItemIndex();


	// Fill resolution / video standard list
	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	FillResolutionStandard(List, SelectedConnector);

	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	if(AVWizardSettings->Exists("Resolution"))
	{
		std::string ResolutionValue = AVWizardSettings->GetValue("Resolution");
		List->SetItemIndex(ResolutionValue);
	}
	else 
	{
		List->SetItemIndex(0);
	}
	List->SetFocus(false);


	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	FillRefresh(List, SelectedConnector);
	List->SetFocus(false);

	if(AVWizardSettings->Exists("Refresh"))
	{
		std::string RefreshValue = AVWizardSettings->GetValue("Refresh");
		List->SetItemIndex(RefreshValue);
	}
	else
		List->SetItemIndex(1);
}

void WizardPageVideoResolution::FillResolutionStandard(WizardWidgetScrollList* List, const int FillType) {
	List->Clear();	
	switch (FillType) {
		default:
		case 0: // VGA
		case 1: // VGA 2
		case 2: // DVI
		case 3: // DVI 2
		case 4: // HDMI-0
		case 5: // LVDS
		{
			ResolutionConf ResCnf("/usr/pluto/share/Resolutions.conf");
			ResResolutionVector ResolutionVector;
			ResCnf.GetResolutionList(ResolutionVector);

			size_t ResolutionVectorSize = ResolutionVector.size();
			for (size_t i = 0; i < ResolutionVectorSize; i++)
			{
				ResResolution & Resolution = ResolutionVector[i];
				if (!Resolution.Visible)
					continue;
				string Label = Resolution.Name;
				if (Resolution.AspectRatio != "")
					Label += " (" + Resolution.AspectRatio + ")";
				List->AddItem(Label, Resolution.Name, Resolution.AspectRatio);
			}
		}
			break;
		case 6: // Component
			List->AddItem("HD480p", "480p","4:3");
			List->AddItem("HD720p", "720p","16:9");
#if 0
			List->AddItem("HD1080i", "1080i","16:9");
			List->AddItem("HD1080p", "1080p","16:9");
#endif
			break;
		case 7: // Composite
		case 8:	// S-Video
			List->AddItem("PAL-B", "640x480","4:3");
			List->AddItem("PAL-D", "640x480","4:3");
			List->AddItem("PAL-G", "640x480","4:3");
			List->AddItem("PAL-H", "640x480","4:3");
			List->AddItem("PAL-I", "640x480","4:3");
			List->AddItem("PAL-K1", "640x480","4:3");
			List->AddItem("PAL-M", "640x480","4:3");
			List->AddItem("PAL-N", "640x480","4:3");
			List->AddItem("PAL-NC", "640x480","4:3");
			List->AddItem("NTSC-J", "640x480","4:3");
			List->AddItem("NTSC-M", "640x480","4:3");
			break;
	}

}

void WizardPageVideoResolution::FillRefresh(WizardWidgetScrollList* List, const int FillType) {
	//SettingsDictionary* AVWizardSettings = Wizard::GetInstance()->AVWizardOptions->GetDictionary();

	List->Clear();
	switch(FillType) {
		default:
		case 0: // VGA
		case 1: // VGA 2
		case 2: // DVI
		case 3: // DVI 2
		case 4: // HDMI-0
			List->AddItem("24 Hz", "24","");
			List->AddItem("50 Hz", "50","");
			List->AddItem("60 Hz", "60","");
			List->AddItem("65 Hz", "65","");
			List->AddItem("70 Hz", "70","");
			List->AddItem("75 Hz", "75","");
			List->AddItem("80 Hz", "80","");
			List->AddItem("85 Hz", "85","");
			List->SetItemIndex(1);
			break;
		case 5: // LVDS
		case 6: // Component
		case 7: // Composite
		case 8:	// S-Video
			break;
	}
}

void WizardPageVideoResolution::DoIncreaseSetting()
{
	Selected->SetFocus(false);
	if ( Selected ==  dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll")) ) {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll")); 
	} else {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	}
	Selected->SetFocus(true);
}

void WizardPageVideoResolution::DoDecreaseSetting()
{
	Selected->SetFocus(false);
	if ( Selected == dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"))) {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	} else {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
	}
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoResolution::DoNextFocusItem()
{
	Selected->SetItemIndex(Selected->GetItemIndex()+1);
	
	WizardWidgetScrollList* ResolutionStandardList = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive ("ResolutionScroll"));
	WizardWidgetScrollList* RefreshList = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive ("RefreshScroll"));

	if ( Selected == dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"))) {
		FillResolutionStandard(ResolutionStandardList, Selected->GetItemIndex());
		FillRefresh(RefreshList, Selected->GetItemIndex());
	}	
}

/*virtual*/ void WizardPageVideoResolution::DoPreviousFocusItem()
{
	Selected->SetItemIndex(Selected->GetItemIndex()-1);
	
	WizardWidgetScrollList* ResolutionStandardList = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive ("ResolutionScroll"));
	WizardWidgetScrollList* RefreshList = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive ("RefreshScroll"));

	if ( Selected == dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"))) {
		FillResolutionStandard(ResolutionStandardList, Selected->GetItemIndex());
		FillRefresh(RefreshList, Selected->GetItemIndex());
	}
}


/*virtual*/ void WizardPageVideoResolution::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetScrollList* NewSelectedScrollList = NULL;
	WizardWidgetImage* NewSelectedImage = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			NewSelectedScrollList = dynamic_cast<WizardWidgetScrollList*>(pWidget);
			NewSelectedImage = dynamic_cast<WizardWidgetImage*>(pWidget);

			if (NewSelectedScrollList)
			{
				Selected->SetFocus(false);
				Selected = NewSelectedScrollList;
				Selected->SetFocus(true);
			}
			else if (NewSelectedImage)
			{
				if (NewSelectedImage->GetName() == "ResolutionUp")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "ResolutionDown")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "RefreshUp")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "RefreshDown")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				} 
				else if (NewSelectedImage->GetName() == "VideoConnectorUp")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "VideoConnectorDown") 
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				}
			}
			else if ((*Item)->GetName() == "BtnOK")
			{
				SDL_Event Event;
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_RETURN;
				SDL_PushEvent(&Event);
			}
		}
	}
}
