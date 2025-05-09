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
//---------------------------------------------------------------------------------------------------------
#include "PlutoUtils/MyStl.h"
#include "BD_PC_Configure.h"
//---------------------------------------------------------------------------------------------------------
#ifdef BLUETOOTH_DONGLE
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#include "../DCE/Logger.h"	
#include "../Bluetooth_Dongle/OrbiterBluetooth.h"
#endif
//---------------------------------------------------------------------------------------------------------

BD_PC_Configure::BD_PC_Configure( PhoneConfig& cfg ) 
	
{
    m_Cfg = cfg;

#ifdef SYMBIAN
	LOG("#	Sending 'Configure' command  #\n");
#endif 
}

void BD_PC_Configure::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	m_Cfg.Write( *this );
}

void BD_PC_Configure::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_Cfg.Read( *this );
}


#ifdef BLUETOOTH_DONGLE
bool BD_PC_Configure::ProcessCommand(BDCommandProcessor *pProcessor)
{
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter || pOrbiter->m_pOrbiter->m_bQuit_get())
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "No configure command will be dispatch: Orbiter was killed or is exiting.");
		return false;
	}


	// Execute Configure in Blootooth_Dongle
	OrbiterBluetooth *pOrbiterBluetooth = dynamic_cast<OrbiterBluetooth *>(pOrbiter->m_pOrbiter);
	if ( pOrbiterBluetooth ) pOrbiterBluetooth->Configure( m_Cfg );

	return true;
}

#else

bool BD_PC_Configure::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
#endif

