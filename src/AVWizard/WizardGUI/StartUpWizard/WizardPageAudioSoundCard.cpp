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
//
// C++ Implementation: WizardPageAudioSoundCard
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@ciplogic-desktop>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "WizardPageAudioSoundCard.h"

#include "WizardPageAudioSoundCard.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "WizardWidgetLabel.h"
#include "WizardWidgetScrollList.h"
#include "WizardWidgetImage.h"
#include "Wizard.h"

WizardPageAudioSoundCard::WizardPageAudioSoundCard(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioSoundCard::WizardPageAudioSoundCard()"<<std::endl;
#endif
	Buttons["UI Version 2"] = 1;
	Buttons["Simple Interface"] = 2;
	OutputValue = "Yes, I can hear it";
	Selected = NULL;
}

WizardPageAudioSoundCard::~WizardPageAudioSoundCard(void)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioSoundCard::~WizardPageAudioSoundCard()"<<std::endl;
#endif
}

/*virtual*/ int WizardPageAudioSoundCard::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioSoundCard::DoApplySetting()"<<std::endl;
#endif
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("SoundCardScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageAudioSoundCard::DoApplySetting()"<<std::endl;
	#endif
		return -1;
	}

	OutputValue = List->GetSelectedValue();
	Dictionary->Set("SoundCard", OutputValue);

	return 0;
}

/*virtual*/ void WizardPageAudioSoundCard::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioSoundCard::DefaultSetup()"<<std::endl;
#endif
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("SoundCardScroll"));
	system("/usr/pluto/bin/ListSoundCards.sh >/tmp/AVWizard.sound_cards");
	FILE *f = fopen("/tmp/AVWizard.sound_cards", "r");
	if (f != NULL)
	{
		char line[1024];
		while (fgets(line, 1024, f) != NULL)
		{
			// strip EOL
			for (int i = strlen(line) - 1; i >= 0 && (line[i] == '\r' || line[i] == '\n'); i--)
				line[i] = 0;
			List->AddItem(line, line, "");
		}
		fclose(f);
	}
	List->SetFocus(true);

	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("BtnOK"));
	if (Selected)
		Selected->SetFocus(true);
}

/*virtual*/ void WizardPageAudioSoundCard::DoIncreaseSetting()
{
	WizardWidgetButton *Button;
	Button = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("BtnTest"));
	if (Button)
		Button->SetFocus(false);
	Button = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("BtnOK"));
	if (Button)
		Button->SetFocus(true);
	Selected = Button;
}

/*virtual*/ void WizardPageAudioSoundCard::DoDecreaseSetting()
{
	WizardWidgetButton *Button;
	Button = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("BtnOK"));
	if (Button)
		Button->SetFocus(false);
	Button = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("BtnTest"));
	if (Button)
		Button->SetFocus(true);
	Selected = Button;
}

/*virtual*/ void WizardPageAudioSoundCard::DoNextFocusItem()
{
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("SoundCardScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageAudioSoundCard::DoNextFocusItem()"<<std::endl;
	#endif
		return;
	}
	List->SetItemIndex(List->GetItemIndex()+1);
}

/*virtual*/ void WizardPageAudioSoundCard::DoPreviousFocusItem()
{
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("SoundCardScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageAudioSoundCard::DoPreviousFocusItem()"<<std::endl;
	#endif
		return;
	}
	List->SetItemIndex(List->GetItemIndex()-1);
}

/*virtual*/ void WizardPageAudioSoundCard::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetButton* NewSelectedButton = NULL;
	WizardWidgetImage* NewSelectedImage = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			NewSelectedButton = dynamic_cast<WizardWidgetButton*>(pWidget);
			NewSelectedImage = dynamic_cast<WizardWidgetImage*>(pWidget);
			if (NewSelectedButton == NULL && NewSelectedImage == NULL)
				break;

			if (NewSelectedButton != NULL)
			{
				if (NewSelectedButton->GetName() == "BtnOK")
				{
					DoIncreaseSetting();
					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_RETURN;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedButton->GetName() == "BtnTest")
				{
					DoDecreaseSetting();
					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_RETURN;
					SDL_PushEvent(&Event);
				}
			}
			else if (NewSelectedImage != NULL)
			{
				if (NewSelectedImage->GetName() == "UI_Up")
				{
					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "UI_Down")
				{
					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				}
			}
		}
	}
}
