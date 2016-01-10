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
#include "WizardPageUISwitcherTest.h"
#include "Wizard.h"
#include "WizardWidgetLabel.h"
#include "ThreadSleeper.h"

#include <iostream>
#include <sys/wait.h>

WizardPageUISwitcherTest::WizardPageUISwitcherTest(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name), PID_xfwm(0), PID_UIdiag(0)
{
#ifdef DEBUG
	std::cout<<"WizardPageUISwitcherTest::WizardPageUISwitcherTest"<<std::endl;
#endif
}

/*virtual*/ WizardPageUISwitcherTest::~WizardPageUISwitcherTest()
{
#ifdef DEBUG
	std::cout<<"WizardPageUISwitcherTest::~WizardPageUISwitcherTest"<<std::endl;
#endif
	ThreadSleeper::Instance()->Quit();
}

/*virtual*/ void WizardPageUISwitcherTest::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
		std::cout<<"WizardPageUISwitcherTest::DefaultSetup"<<std::endl;
#endif
	WizardWidgetButton* Btn = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("MainBtn"));
	if(Btn)
		Btn->SetFocus(true);

	xfwm_Start();
	UIdiag_Start();

	ThreadSleeper::Instance()->Init(15, false);
	WizardWidgetLabel * Label = dynamic_cast<WizardWidgetLabel*>
		(Page->GetChildRecursive("CounterLabel"));
	ThreadSleeper::Instance()->SetLabel(Label);
}

/*virtual*/ int WizardPageUISwitcherTest::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
		std::cout<<"WizardPageUISwitcherTest::DoApplySetting"<<std::endl;
#endif
	UIdiag_Stop();
	xfwm_Stop();
	return 0;
}

/*virtual*/ void WizardPageUISwitcherTest::DoCancelSetting()
{
#ifdef DEBUG
		std::cout<<"WizardPageUISwitcherTest::DoCancelSetting"<<std::endl;
#endif
		UIdiag_Stop();
		xfwm_Stop();
}

/*virtual*/ void WizardPageUISwitcherTest::DoClickWidget(WizardWidgetBase * pWidget)
{
#ifdef DEBUG
	std::cout << "WizardPageUISwitcherTest::DoClickWidget" << endl;
#endif
	if (pWidget == Page->GetChildRecursive("MainBtn"))
	{
		SDL_Event Event;
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_RETURN;
		SDL_PushEvent(&Event);
	}
}

/*virtual*/ bool WizardPageUISwitcherTest::IsDisplayable()
{
	SettingsDictionaryTree * AVWizardOptions = Wizard::GetInstance()->AVWizardOptions;
	bool TestUI = false;
	if (AVWizardOptions->GetDictionary()->Exists("TestUI"))
		TestUI = AVWizardOptions->GetDictionary()->GetValue("TestUI") == "1";
	return TestUI;
}

void WizardPageUISwitcherTest::xfwm_Start()
{
	if (PID_xfwm > 0)
		return;
	PID_xfwm = fork();
	if (PID_xfwm == 0)
	{
		setpgrp();
		char * args[] = { "xfwm4", NULL };
		execvp(args[0], args);
		_exit(1);
	}
	
	sleep(1);
	//system("wmctrl -i -r $(wmctrl -l|grep AVWizard|awk '{print $1}') -t 1");
	system("/usr/bin/wmctrl -s 1");
}

void WizardPageUISwitcherTest::xfwm_Stop()
{
	if (PID_xfwm <= 0)
		return;
	
	//system("wmctrl -i -r $(wmctrl -l|grep AVWizard|awk '{print $1}') -t 0");
	system("/usr/bin/wmctrl -s 0");
	
	kill(-PID_xfwm, SIGTERM);
	waitpid(PID_xfwm, NULL, 0);
	PID_xfwm = 0;
}

void WizardPageUISwitcherTest::UIdiag_Start()
{
	if (PID_UIdiag > 0)
		return;
	PID_UIdiag = fork();
	if (PID_UIdiag == 0)
	{
		setpgrp();
		SettingsDictionary * Dictionary = Wizard::GetInstance()->AVWizardOptions->GetDictionary();
		std::string UIVersion = Dictionary->GetValue("UIVersion");
		std::map<std::string, const char *> ArgMap;
		ArgMap["UI1"] = NULL;
		ArgMap["UI2_med"] = "mask";
		ArgMap["UI2_hi"] = "both";
		chdir("/usr/pluto/bin");
		printf("Running UIdiag %s\n", ArgMap[UIVersion]);
		char * args[] = { "./UIdiag", (char *) ArgMap[UIVersion], NULL };
		execv(args[0], args);
		_exit(1);
	}
}

void WizardPageUISwitcherTest::UIdiag_Stop()
{
	if (PID_UIdiag <= 0)
		return;
	kill(-PID_UIdiag, SIGKILL);
	waitpid(PID_UIdiag, NULL, 0);
	PID_UIdiag = 0;
}
