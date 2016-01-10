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
#include "BD/BDCommandProcessor.h"
#else
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_SetImageQuality.h"

#ifdef BLUETOOTH_DONGLE
#include "../Orbiter/Orbiter.h"
#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

BD_PC_SetImageQuality::BD_PC_SetImageQuality(unsigned long nImageQuality) 
{
    m_nImageQuality = nImageQuality;

#ifdef SYMBIAN
	LOG("#	Sending 'Set Image Quality' command  #\n");
#endif 
}

void BD_PC_SetImageQuality::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
    Write_long(m_nImageQuality);
}

void BD_PC_SetImageQuality::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
    m_nImageQuality = Read_long();
}

bool BD_PC_SetImageQuality::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef BLUETOOTH_DONGLE
    BDCommandProcessor_BluetoothDongle *m_pProcessor = 
        (BDCommandProcessor_BluetoothDongle *)pProcessor;

    BD_Orbiter *pOrbiter = 
        m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

    if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter || pOrbiter->m_pOrbiter->m_bQuit_get())
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "No keypress command will be dispatch: Orbiter was killed or is exiting.");
        return false;
    }

    pOrbiter->m_pOrbiter->SetImageQuality(m_nImageQuality);
#endif

    return true;
}

