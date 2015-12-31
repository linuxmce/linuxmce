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
#include "PlutoUtils/PlutoDefs.h"
#include <iostream>
#include <fstream>
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "Logger.h"
#endif
//---------------------------------------------------------------------------------------------------------
#include "PlutoUtils/MyStl.h"
#include "BD_PC_GetMenuImages.h"
//---------------------------------------------------------------------------------------------------------
#ifdef BLUETOOTH_DONGLE
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#include "../DCE/Logger.h"	
#include "../Bluetooth_Dongle/OrbiterBluetooth.h"
#endif
//---------------------------------------------------------------------------------------------------------

BD_PC_GetMenuImages::BD_PC_GetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo	>& vItems ) 
	
{    
	m_nItemsType = nItemsType;

	m_vItems = vItems;


#ifdef SYMBIAN
	LOG("#	Sending 'GetMenuImages' command  #\n");
#endif 
}

BD_PC_GetMenuImages::~BD_PC_GetMenuImages()
{
}


void BD_PC_GetMenuImages::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char( m_nItemsType );
	Write_long( (long)m_vItems.size() );
	for (vector<MenuItemInfo>::iterator iter=m_vItems.begin(); iter!=m_vItems.end(); ++iter ) {
		(*iter).write( m_nItemsType, *this );
	}

}

void BD_PC_GetMenuImages::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	#ifdef BLUETOOTH_DONGLE
		m_vItems.clear();

		m_nItemsType = (MenuItemInfo::ItemType)Read_unsigned_char( );
		long nSize = Read_long( );
		MenuItemInfo ItemInfo;	
		for ( long i=0; i<nSize; ++i ){
			ItemInfo.read( m_nItemsType, *this );
			m_vItems.push_back( ItemInfo );
		}
	#endif
}


#ifdef BLUETOOTH_DONGLE
bool BD_PC_GetMenuImages::ProcessCommand(BDCommandProcessor *pProcessor)
{
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter || pOrbiter->m_pOrbiter->m_bQuit_get())
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "No GetMenuImages command will be dispatch: Orbiter was killed or is exiting.");
		return false;
	}


	// Execute GetMenuImages in Blootooth_Dongle
	OrbiterBluetooth *pOrbiterBluetooth = dynamic_cast<OrbiterBluetooth *>(pOrbiter->m_pOrbiter);
	if ( pOrbiterBluetooth ) pOrbiterBluetooth->GetMenuImages( m_nItemsType, m_vItems );

	return true;
}

#else

bool BD_PC_GetMenuImages::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
#endif

