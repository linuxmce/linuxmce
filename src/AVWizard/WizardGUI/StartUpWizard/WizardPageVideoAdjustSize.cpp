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
#include "WizardPageVideoAdjustSize.h"

#include "ConfigureCommons.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "SettingsDictionaryTree.h"

#include "Wizard.h"

#include "SkinGenerator.h"

WizardPageVideoAdjustSize::WizardPageVideoAdjustSize(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Wizard::GetInstance()->WizardBorder = 0;
	Wizard::GetInstance()->LeftBorder = 0;	
	Wizard::GetInstance()->TopBorder = 0;

}

WizardPageVideoAdjustSize::~WizardPageVideoAdjustSize(void)
{
}


/*virtual*/ int WizardPageVideoAdjustSize::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;

	return 0;
}

/*virtual*/ void WizardPageVideoAdjustSize::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	this->GlobalSettings = AVWizardSettings;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoIncreaseSetting()
{
	if(Wizard::GetInstance()->WizardBorder*2 > 
		Wizard::GetInstance()->LeftBorder )
	Wizard::GetInstance()->LeftBorder +=BORDER_JUMP;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoDecreaseSetting()
{
	if(Wizard::GetInstance()->LeftBorder>BORDER_JUMP)
		Wizard::GetInstance()->LeftBorder -=BORDER_JUMP;
	else
		Wizard::GetInstance()->LeftBorder = 0;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoNextFocusItem()
{
	if(Wizard::GetInstance()->WizardBorder*2 > 
		Wizard::GetInstance()->TopBorder )
	Wizard::GetInstance()->TopBorder +=BORDER_JUMP;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoPreviousFocusItem()
{
	if(Wizard::GetInstance()->TopBorder>BORDER_JUMP)
		Wizard::GetInstance()->TopBorder -= BORDER_JUMP;
	else
		Wizard::GetInstance()->TopBorder = 0;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::UpdateSelected()
{
	if (Wizard::GetInstance()->WizardBorder < 0)
		Wizard::GetInstance()->WizardBorder = 0;

	int LeftBorder = Wizard::GetInstance()->LeftBorder;
	int TopBorder = Wizard::GetInstance()->TopBorder;
	int Border = Wizard::GetInstance()->WizardBorder;

	WizardWidgetButton* Button = NULL;
	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonLeft"));
	Button->SetFocus(LeftBorder > 0);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonUp"));
	Button->SetFocus(TopBorder > 0);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonRight"));
	Button->SetFocus(LeftBorder < Border*2);
	
	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonDown"));
	Button->SetFocus(TopBorder < Border*2);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonPlus"));
	Button->SetFocus(0 < Border);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonMinus"));
	Button->SetFocus(Border*5 < FrontEnd->GetScreenWidth());

	Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Set("WizardBorder", Border);
	Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Set("LeftBorder", LeftBorder);
	Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Set("TopBorder", TopBorder);

#ifdef DEBUG
	cout << "WizardPageVideoAdjustSize::UpdateSelected()" << endl;
	cout << "LeftBorder: " << LeftBorder << endl;
	cout << "TopBorder: " << TopBorder << endl;
	cout << "WizardBorder: " << Border << endl;
#endif
}

/*virtual*/ void WizardPageVideoAdjustSize::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetButton* ClickedButton = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			ClickedButton = dynamic_cast<WizardWidgetButton*>(pWidget);
			if (!ClickedButton)
				break;

			SDL_Event Event;

			if (ClickedButton->GetName() == "ButtonPlus")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_PLUS;
				SDL_PushEvent(&Event);
			}
			else if (ClickedButton->GetName() == "ButtonMinus")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_MINUS;
				SDL_PushEvent(&Event);
			}
			else if (ClickedButton->GetName() == "ButtonUp")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_UP;
				SDL_PushEvent(&Event);
			}
			else if (ClickedButton->GetName() == "ButtonDown")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_DOWN;
				SDL_PushEvent(&Event);
			}
			else if (ClickedButton->GetName() == "ButtonLeft")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_LEFT;
				SDL_PushEvent(&Event);
			}
			else if (ClickedButton->GetName() == "ButtonRight")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_RIGHT;
				SDL_PushEvent(&Event);
			}
			else if (ClickedButton->GetName() == "ButtonOK")
			{
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_RETURN;
				SDL_PushEvent(&Event);
			}
		}
	}
}
