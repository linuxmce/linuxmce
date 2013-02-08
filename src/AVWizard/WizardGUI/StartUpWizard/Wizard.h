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
#ifndef Wizard_H_
#define Wizard_H_

#include <signal.h>

#include "SDLFrontEnd.h"
#include "BackEndFactory.h"

#include "ScreenManager.h"

#define Wizard_Default_Width 640
#define Wizard_Default_Height 480

#define WIZARD_CHANGE_RESOLUTION 1
#define WIZARD_CHANGE_REFRESH 2

#define WIZARD_SELECTED_FONT_SIZE		50
#define WIZARD_SELECTED_LABEL_SIZE		30
#define WIZARD_DESELECTED_FONT_SIZE		30
#define WIZARD_DESELECTED_LABEL_SIZE	20
#define BORDER_JUMP (9)
#define ARROWS_BORDER (40)


#ifdef WIN32
#define snprintf _snprintf
#endif


/**
 *	For now may change only the resolution and the refresh
 */
#include "WizardPage.h"

#include "SettingsDictionaryTree.h"

#ifndef WIN32
#include "SocketRemoteServer.h"
#endif

#include <pthread.h>
#include <list>

#include "WizardRemote.h"
#include "PageSequence.h"

/**
 *	Singleton class that consist all the wizard, is singleton because 
 *	is the core class of the AVWizard
 */
class Wizard
{
	pthread_mutex_t SafeMutex;

	int ExitCode;
	/**
	 *	Current video mode that runs the wizard
	 */
	int Width;
	int Height;
	bool FullScreen;

	/**
	 *	SettingsDictionary that keeps the options of command line
	 */
	SettingsDictionary CommandLineOptions;

	/**
	 *	The current page of the wizard
	 */
	WizardPage* MainPage;
	
	/**
	 * Quit attribute is true if the application is pending to quit
	 */
	bool Quit;
	/**
	 * The actual application should display everything using a SDL application
	 */
	GenericBackEnd* FrontEnd;
	
	/**
	 * Last pending event definition
	 */
	list<WM_Event> Events;
	/**
	 * Treat the current keys and window events
	 */
	void TreatEvents();
	

	bool StatusChange;

	int WizardChangeType;
	/**
	 * Make the decrease action, depends of the 
	 * WizardChangeType mode
	 */
	void DoDecreaseAction();
	/**
	 * Make the increase action, depends of the 
	 * WizardChangeType mode
	 */
	void DoIncreaseAction();
	/**
	 * Change WizardChangeType mode to the previous one
	 */
	void DoChangeActionBefore();
	/**
	 * Change WizardChangeType mode to the next one
	 */
	void DoChangeActionAfter();
	/**
	 *	Apply the current settings from the current screen
	 */
	void DoApplyScreen(SettingsDictionary* Settings);
	/**
	 *	Apply the current settings from the current screen
	 */
	void DoCancelScreen();
	/**
	 *  Focus clicked button, if a button was clicked
	 */
	void DoMouseClick(int x, int y);
	/**
	 *  Do numeric key action
	 */
	void DoNumberKey(int KeyCode);
	
	void EvaluateEvent(WM_Event& Event);

	void ZoomIn();
	void ZoomOut();

	void PaintStatus();

	void PushEvent(WM_Event& Event);
	void EvaluateEvents();


	/**
	 *	Private constructor, because is a signleton class	
	 */
	Wizard();

	static Wizard* Instance;
#ifndef WIN32
	SocketRemoteServer Server;
#endif

	/**
	 *	If is analog sound the pages with Dolby and DTS should be skipped
	 */
	bool IsAnalogSound;
public:
	/**
	 *	Page sequence in the wizard
	 */
	PageSequence m_PageSequence;

	int LeftBorder, TopBorder;
	int WizardBorder;

	/**
	 * Process that handles remote control events
	 */
	std::string RemoteCmd;
	
	/** 
	 * Process that handles gamepad events
         */
	std::string GamepadCmd;

#ifndef WIN32
	RemoteProcess m_WizardRemote;
	RemoteProcess m_GamepadRemote; // added for USB Gamepad support.
#endif
	
	/**
	 *	SettingsDictionary that keep global AVWizard Settings
	 */
	SettingsDictionaryTree* AVWizardOptions;

	virtual ~Wizard();
	static Wizard* GetInstance();
	void CleanUp();

	int ParseCommandLineParameters(int argc, char** argv);

	void StartSDLVideoMode();

	void Configure();

	void MainLoop();

	/**
	 *	Generate an custom event
	 */
	void GenerateCustomEvent(WM_Event Event);

	void CreateDialogs();

	int GetExitCode();

	void SetExitWithCode(int Code);

	void Resize(bool FullScreen);

	void SetAnalogSoundMode(bool IsAnalogSound);
	bool GetAnalogSoundMode();
};

#endif /*Wizard_H_*/
