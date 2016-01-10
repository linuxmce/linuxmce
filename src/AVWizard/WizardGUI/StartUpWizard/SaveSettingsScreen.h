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

#ifndef SaveSettingsScreen_H_
#define SaveSettingsScreen_H_

#include "WizardPage.h"

class SaveSettingsScreen :
	public WizardPage
{
public:
	SaveSettingsScreen(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~SaveSettingsScreen(void);
};

#endif
