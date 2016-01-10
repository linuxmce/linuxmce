/*
 BD_PC_ReportMyVersion
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

/*
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
*/

#ifdef BLUETOOTH_DONGLE
//#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
//#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

#include "BD_PC_ReportMyVersion.h"

BD_PC_ReportMyVersion::BD_PC_ReportMyVersion(string sVersion) 
	
{
	m_sVersion = sVersion;
}

BD_PC_ReportMyVersion::~BD_PC_ReportMyVersion()
{
}

void BD_PC_ReportMyVersion::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_string(m_sVersion);
}

void BD_PC_ReportMyVersion::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	Read_string(m_sVersion);
}

bool BD_PC_ReportMyVersion::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef BLUETOOTH_DONGLE
	BDCommandProcessor_BluetoothDongle *pBDCommandProcessor_BluetoothDongle = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	pBDCommandProcessor_BluetoothDongle->m_pBluetooth_Dongle->GetEvents()->Mobile_orbiter_linked( pProcessor->m_sMacAddressPhone, m_sVersion ); 
#endif
	return true;
}
