/*
 BD_CP_SendMeKeystrokes
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "VIPShared/PlutoConfig.h"
#endif
#endif

#ifdef SMARTPHONE
#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_SendMeKeystrokes.h"


BD_CP_SendMeKeystrokes::BD_CP_SendMeKeystrokes(char YesNo) 
	
{
	m_YesNo=YesNo;
}

void BD_CP_SendMeKeystrokes::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_YesNo);
}

void BD_CP_SendMeKeystrokes::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_YesNo = Read_unsigned_char();
}

bool BD_CP_SendMeKeystrokes::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

//g_pPlutoConfig->m_bSendKeyStrokes = m_YesNo;

#ifdef SMARTPHONE
	OrbiterApp::GetInstance()->SendKeyStrokes(m_YesNo);
#endif

#ifdef SYMBIAN 
	LOG("#	Received 'SendKeyStrokes' command  #\n");
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bSendKeyStrokes = m_YesNo;
#endif

#endif
	return true;
}

