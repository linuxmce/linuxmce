/*
 BD_CP_ShowMenu
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#include "VIPShared/PlutoConfig.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/PlutoDefs.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#ifdef SMARTPHONE
#include "OrbiterApp.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_ShowMenu.h"

BD_CP_ShowMenu::BD_CP_ShowMenu( long nCrtRoom )  
	
{
	m_nCrtRoom = nCrtRoom;
}

BD_CP_ShowMenu::~BD_CP_ShowMenu()
{
}

void BD_CP_ShowMenu::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_nCrtRoom);
}

void BD_CP_ShowMenu::ParseCommand(unsigned long size,const char *data)
{
#ifdef SYMBIAN
    LOG("#	BD_CP_ShowMenu::ParseCommand  #\n");
#endif

	BDCommand::ParseCommand(size, data);

    m_nCrtRoom = Read_long();
}

bool BD_CP_ShowMenu::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE
#ifdef SYMBIAN
	 LOG("#	Received 'ShowMenu' command  #\n");

#endif //SYMBIAN

#ifdef SMARTPHONE
	 OrbiterApp::GetInstance()->ShowMenu(m_nCrtRoom);
#endif

#ifdef VIPDESIGN
#endif //VIPDESIGN

#endif //VIPPHONE

	return true;
}
