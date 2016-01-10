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
#ifndef WizardPageAudioVolume_H_ 
#define  WizardPageAudioVolume_H_

#include "WizardPage.h"

#include "WizardWidgetLabel.h"
#ifndef WIN32
#include "XinePlayer.h"
#endif

class WizardPageAudioVolume :
	public WizardPage
{
	int AudioVolumeMin, AudioVolumeMax, AudioVolumeCurrent, AudioVolumeIncrement;
	WizardWidgetLabel* VolumeLabel;

#ifndef WIN32
	XinePlayer* Player;
#endif
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);
	virtual bool IsDisplayable();

	WizardPageAudioVolume(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageAudioVolume(void);
};

#endif
