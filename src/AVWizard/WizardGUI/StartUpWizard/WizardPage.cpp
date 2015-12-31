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
#include "WizardPage.h"

#include "WizardWidgetButton.h"
#include "WizardWidgetScrollList.h"
#include "WizardWidgetImage.h"

#include "Wizard.h"
#include "GUIWizardUtils.h"

#include "GenerateWizardConfigDefaults.h"

#include "SkinGenerator.h"

#include "WizardWidgetLabel.h"

#include "math.h"

WizardPage::WizardPage(GenericBackEnd* FrontEnd, std::string Name)
{
	this->FrontEnd = FrontEnd;
	this->Name = Name;
	this->Page = NULL;
}

/*virtual*/ WizardPage::~WizardPage()
{

}

WizardWidgetPage* WizardPage::GetPageLayout()
{
	return this->Page;
}

void WizardPage::SetPageLayout(WizardWidgetPage* PageDescription)
{
	this->Page = PageDescription;
}

void WizardPage::FindFocusableControlsRecursive(WizardWidgetBase* RootWidget)
{
}

/*virtual*/ void WizardPage::DoClickWidget(WizardWidgetBase * pWidget)
{
}

/*virtual*/ void WizardPage::DoNextFocusItem()
{
}

/*virtual*/ void WizardPage::DoPreviousFocusItem()
{
}

/*virtual*/ void WizardPage::DoIncreaseSetting()
{
}

/*virtual*/ void WizardPage::DoDecreaseSetting()
{
}

/*virtual*/ int WizardPage::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;
	return 0;
}

/*virtual*/ void WizardPage::DoCancelSetting()
{

}

void WizardPage::SetWizard(Wizard* MainWizard)
{
	this->MainWizard = MainWizard;
	if(this->MainWizard != NULL)
	{
		this->DefaultSetup(MainWizard->AVWizardOptions->GetDictionary());
	}
}

/*virtual*/ void WizardPage::DefaultSetup(SettingsDictionary* AVWizardSettings)
{

}

/*static*/ void WizardPage::TabStatus(WizardPage* Page, int EnableStatus)
{
}

/*virtual*/ void WizardPage::DoNumberKey(int KeyCode)
{
}

/*virtual*/ bool WizardPage::IsDisplayable()
{
	return true; // all the pages are displayable by default
}

#include <iostream>

/*virtual*/ void WizardPage::DoMouseClick(int x, int y)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetBase * pSelectedWidget = NULL;

	Wizard *pWizard = Wizard::GetInstance();
	int iExtraBorder = 0;
	if (pWizard->m_PageSequence.CurrentPage() == "VideoAdjustSize")
	{
		// XXX: VideoAdjustSize, hardcoded extra border
		iExtraBorder = ARROWS_BORDER;
	}

	int TotalWizardBorder = pWizard->WizardBorder + iExtraBorder;

	int EffectiveScreenWidth = FrontEnd->GetScreenWidth() - 2 * TotalWizardBorder;
	int EffectiveScreenHeight = FrontEnd->GetScreenHeight() - 2 * TotalWizardBorder;
	int EffectiveLeftBorder = pWizard->LeftBorder - pWizard->WizardBorder;
	int EffectiveTopBorder = pWizard->TopBorder - pWizard->WizardBorder;

	int NormalX = (int)((x - TotalWizardBorder - EffectiveLeftBorder) / (EffectiveScreenWidth / 640.0f));
	int NormalY = (int)((y - TotalWizardBorder - EffectiveTopBorder) / (EffectiveScreenHeight / 480.0f));

#ifdef DEBUG
	std::cout << "Screen parameters: " << FrontEnd->GetScreenWidth() << "x" << FrontEnd->GetScreenHeight() << "+" << pWizard->LeftBorder << "+" << pWizard->TopBorder << "+" << TotalWizardBorder << std::endl;
	std::cout << "Effective resolution: " << EffectiveScreenWidth << "x" << EffectiveScreenHeight << "+" << EffectiveLeftBorder << "+" << EffectiveTopBorder << "+" << TotalWizardBorder << std::endl;
	std::cout << "Normalized click coordinates: " << NormalX << "x" << NormalY << std::endl;
#endif

	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		WizardWidgetButton * pWizardWidgetButton = dynamic_cast<WizardWidgetButton*> (*Item);
		WizardWidgetScrollList * pWizardWidgetScrollList = dynamic_cast<WizardWidgetScrollList*> (*Item);
		WizardWidgetImage * pWizardWidgetImage = dynamic_cast<WizardWidgetImage*> (*Item);
		if (pWizardWidgetButton != NULL) // it's a button
		{
#ifdef DEBUG
			std::cout << "Button '" << pWizardWidgetButton->GetName() << "@" << pWizardWidgetButton->Left << "x" << pWizardWidgetButton->Top << "+" << pWizardWidgetButton->Width << "+" << Page->GetFontHeight() << "' exists" << std::endl;
#endif
			if (pWizardWidgetButton->Left - ceil(pWizardWidgetButton->Width / 2.0) < NormalX
					&& pWizardWidgetButton->Left + ceil(pWizardWidgetButton->Width / 2.0) > NormalX
					&& pWizardWidgetButton->Top - Page->GetFontHeight() < NormalY
					&& pWizardWidgetButton->Top + Page->GetFontHeight() > NormalY
				)
			{
				std::cout << "Button '" << pWizardWidgetButton->GetName() << "@" << pWizardWidgetButton->Left << "x" << pWizardWidgetButton->Top << "+" << pWizardWidgetButton->Width << "+" << Page->GetFontHeight() << "' pressed" << std::endl;
				pSelectedWidget = pWizardWidgetButton;
				break;
			}
		}
		else if (pWizardWidgetScrollList != NULL) // it's a list
		{
#ifdef DEBUG
			std::cout << "ScrollList '" << pWizardWidgetScrollList->GetName() << "@" << pWizardWidgetScrollList->Left << "x" << pWizardWidgetScrollList->Top << "+" << pWizardWidgetScrollList->Width << "+" << pWizardWidgetScrollList->Height << "' exists" << std::endl;
#endif
			if (pWizardWidgetScrollList->Left < NormalX
					&& pWizardWidgetScrollList->Left + pWizardWidgetScrollList->Width > NormalX
					&& pWizardWidgetScrollList->Top < NormalY
					&& pWizardWidgetScrollList->Top + pWizardWidgetScrollList->Height > NormalY
				)
			{
				std::cout << "ScrollList '" << pWizardWidgetScrollList->GetName() << "@" << pWizardWidgetScrollList->Left << "x" << pWizardWidgetScrollList->Top << "+" << pWizardWidgetScrollList->Width << "+" << pWizardWidgetScrollList->Height << "' pressed" << std::endl;
				pSelectedWidget = pWizardWidgetScrollList;
				break;
			}
		}
		else if (pWizardWidgetImage != NULL) // it's an image
		{
#ifdef DEBUG
			std::cout << "Image '" << pWizardWidgetImage->GetName() << "@" << pWizardWidgetImage->Left << "x" << pWizardWidgetImage->Top << "+" << pWizardWidgetImage->Width << "+" << pWizardWidgetImage->Height << "' exists" << std::endl;
#endif
			if (pWizardWidgetImage->Left < NormalX
					&& pWizardWidgetImage->Left + pWizardWidgetImage->Width > NormalX
					&& pWizardWidgetImage->Top < NormalY
					&& pWizardWidgetImage->Top + pWizardWidgetImage->Height > NormalY
				)
			{
				std::cout << "Image '" << pWizardWidgetImage->GetName() << "@" << pWizardWidgetImage->Left << "x" << pWizardWidgetImage->Top << "+" << pWizardWidgetImage->Width << "+" << pWizardWidgetImage->Height << "' pressed" << std::endl;
				pSelectedWidget = pWizardWidgetImage;
				break;
			}
		}
	}

	if (pSelectedWidget != NULL)
	{
		DoClickWidget(pSelectedWidget);
	}
}
