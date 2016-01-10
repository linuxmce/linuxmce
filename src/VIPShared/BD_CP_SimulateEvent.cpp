/*
 BD_CP_SimulateEvent
 
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
	#include "BD/BDCommandProcessor.h"
#else
	#include "VIPShared/VIPIncludes.h"
	#include "PlutoMOAppUi.h"
	#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_SimulateEvent.h"
#include "PlutoUtils/PlutoDefs.h"

BD_CP_SimulateEvent::BD_CP_SimulateEvent(		
	long EventType,
	long key
) 
{	
	m_EventType = EventType;
	m_key = key;
}

BD_CP_SimulateEvent::~BD_CP_SimulateEvent()
{
}

void BD_CP_SimulateEvent::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_EventType);
	Write_long(m_key);
}

void BD_CP_SimulateEvent::ParseCommand(unsigned long size, const char *data)
{
	BDCommand::ParseCommand(size, data);

	m_EventType = Read_long();
	m_key = Read_long();
}

bool BD_CP_SimulateEvent::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE

#ifdef SYMBIAN
	m_EventType = Read_long();
	m_key = Read_long();

	LOG("#	Received 'SimulateEvent' command  #\n");

	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->SimulateEvent(m_EventType, m_key);
#endif //SYMBIAN

#ifdef VIPDESIGN
	#pragma error "Not implemented!"
#endif //VIPDESIGN

#endif //VIPPHONE

	return true;
}
