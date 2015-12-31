/*
 BD_CP_CurrentSignalStrength
 
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

#include "PlutoUtils/MyStl.h"
#include "BD_CP_CurrentSignalStrength.h"


#ifdef SMARTPHONE
#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
#endif

BD_CP_CurrentSignalStrength::BD_CP_CurrentSignalStrength(int iSignalStrength) 
{
    m_iSignalStrength = iSignalStrength;
}

void BD_CP_CurrentSignalStrength::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_iSignalStrength);
}

void BD_CP_CurrentSignalStrength::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_iSignalStrength = Read_long();
}


bool BD_CP_CurrentSignalStrength::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifdef SYMBIAN 
	LOG("#	Received 'BD_CP_CurrentSignalStrength' command  #\n");
    ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->SetCurrentSignalStrength(m_iSignalStrength);
#endif

#ifdef SMARTPHONE
	OrbiterApp::GetInstance()->SetCurrentSignalStrength(m_iSignalStrength);
#endif

#endif

	return true;
}

