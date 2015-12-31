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
#include "BD_PC_GetSignalStrength.h"
#include "BD_CP_CurrentSignalStrength.h"

#ifdef BLUETOOTH_DONGLE
#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

BD_PC_GetSignalStrength::BD_PC_GetSignalStrength() 
{
#ifdef SYMBIAN
	LOG("#	Sending 'Get Signal Strength' command  #\n");
#endif 
}

void BD_PC_GetSignalStrength::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
}

void BD_PC_GetSignalStrength::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
}

bool BD_PC_GetSignalStrength::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef BLUETOOTH_DONGLE
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

    string sMacAddress = m_pProcessor->m_sMacAddressPhone;
    int iQualityLink = m_pProcessor->m_pBluetooth_Dongle->GetLinkQuality(sMacAddress.c_str());
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Phone %s requested link quality, which is %d. Sending reply...",
        sMacAddress.c_str(), iQualityLink);

    BD_CP_CurrentSignalStrength *pBD_CP_CurrentSignalStrength = new BD_CP_CurrentSignalStrength(iQualityLink);
    m_pProcessor->AddCommand(pBD_CP_CurrentSignalStrength);
#endif

    return true;
}

