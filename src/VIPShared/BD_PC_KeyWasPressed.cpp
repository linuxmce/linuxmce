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
#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include <iostream>
#include <fstream>
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_KeyWasPressed.h"

#ifdef BLUETOOTH_DONGLE
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#include "../DCE/Logger.h"	
#endif

#ifdef APPSERVER
#include "../level2/AppServer/MODCommandImpl.h"
#include "../level1/Controller/AppServer/ControllerAppServer.h"
#include "../pluto_dce/Table_Command.h"
#endif

BD_PC_KeyWasPressed::BD_PC_KeyWasPressed(int Key, int EventType) 
	
{
	m_Key=Key;
    m_EventType = EventType;

#ifdef SYMBIAN
	LOG("#	Sending 'KeyWasPressed' command  #\n");
#endif 
}

void BD_PC_KeyWasPressed::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_Key);
    Write_long(m_EventType);
}

void BD_PC_KeyWasPressed::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_Key = Read_long();
    m_EventType = Read_long();
}


#ifdef BLUETOOTH_DONGLE
bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter || pOrbiter->m_pOrbiter->m_bQuit_get())
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "No keypress command will be dispatch: Orbiter was killed or is exiting.");
		return false;
	}

    if(m_EventType == 2) //execute
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received BD_PC_KeyWasPressed from PlutoMO. Phone code: %d, 'execute'", m_Key);
        pOrbiter->m_pOrbiter->HandleButtonEvent(m_Key);
    }
    else
    {
	    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received BD_PC_KeyWasPressed from PlutoMO. Phone code: %d, %s", m_Key,
            m_EventType ? "key up" : "key down");

	    Orbiter::Event orbiterEvent;
        orbiterEvent.type = m_EventType ? Orbiter::Event::BUTTON_UP : Orbiter::Event::BUTTON_DOWN;
	    orbiterEvent.data.button.m_iPK_Button = m_Key;
	    pOrbiter->m_pOrbiter->ProcessEvent(orbiterEvent);
    }

	return true;
}

#else

bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
#endif

