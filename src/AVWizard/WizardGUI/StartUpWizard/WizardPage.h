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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPage_H_
#define WizardPage_H_

#include "WizardWidgetPage.h"

#include <vector>

class Wizard;

class WizardPage 
{
protected:
	GenericBackEnd* FrontEnd;

	std::string Name;
	/**
	 *	Builds the list with focusable items
	 */
	void FindFocusableControlsRecursive(WizardWidgetBase* RootWidget);
	/**
	 *	Widget that describes the wizard layout
	 */
	WizardWidgetPage* Page;
	Wizard* MainWizard;

	

	/**
	*	Must override function, used to get setup defaults of the dialog from the wizard
	*/
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
public:
	/**
	 * Default constructor
	 */
	WizardPage(GenericBackEnd* FrontEnd, std::string Name);

	/**
	 * Default destructor
	 */
	virtual ~WizardPage();
	
	WizardWidgetPage* GetPageLayout();

	/**
	 *    Gives a layout description to the current page
	 * @param PageDescription 
	 */
	void SetPageLayout(WizardWidgetPage* PageDescription);

	/**
	 *   Execute widget and page-specific click action
	 */
	virtual void DoClickWidget(WizardWidgetBase * pWidget);

	/**
	 *	Similar with pressing on current item the down arrow
	 */
	virtual void DoNextFocusItem();

	/**
	 *	Similar with pressing on current item the up arrow
	 */
	virtual void DoPreviousFocusItem();

	/**
	 *	Simiar with pressing on current item the right arrow
	 */
	virtual void DoIncreaseSetting();

	/**
	 *	Simiar with pressing on current item the left arrow
	 */
	virtual void DoDecreaseSetting();

	/**
	 *	Similar with pressing on current item the Enter key
	 * @param Dictionary The place where stores curretn values
	 * @return Return a negative value if it wants to ignore that command
	 */
	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	/**
	 *	Similar with pressing on current item the Escape key
	 */
	virtual void DoCancelSetting();

	/**
	 *  Focus clicked button, if a button was clicked
	 */
	virtual void DoMouseClick(int x, int y);

	/**
	 *  Run action associated with numeric key
	 */
	virtual void DoNumberKey(int KeyCode);

	/**
	 *  Return true if page is displayable, false otherwise
	 */
	virtual bool IsDisplayable();

	/**
	 *	Builds the list with focusable items
	 */
	void FindFocusableControls();

	/**
	 *	Get the MainWizard reference to notify the Wizard on change screen
	 */
	void SetWizard(Wizard* MainWizard);

	/**
	 *	Enable/disable some tabs if they are in different by zero status
	 * 	They are set-up in bits
	 */
	static void TabStatus(WizardPage* Page, int EnableStatus);
};

#endif
