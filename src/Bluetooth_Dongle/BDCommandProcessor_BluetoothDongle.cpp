/*
 BDCommandProcessor_BluetoothDongle
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

 
/**
 *
 * @file BDCommandProcessor_BluetoothDongle.cpp
 * @brief source file for the BDCommandProcessor_BluetoothDongle classes
 * @author
 * @todo notcommented
 *
 */


#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"

//-----------------------------------------------------------------------------------------------------

BDCommandProcessor_BluetoothDongle::BDCommandProcessor_BluetoothDongle( class Bluetooth_Dongle *pBluetooth_Dongle, string sMacAddressPhone, class PhoneDevice *pPhoneDevice
#ifdef BT_SOCKET
	, class Socket *pSocket
#endif
 ) :
#ifdef BT_SOCKET
		BDCommandProcessor_Windows_Socket( sMacAddressPhone, pSocket )
#else
	#ifdef WIN32
		BDCommandProcessor_Windows_Bluetooth( sMacAddressPhone, pBluetooth_Dongle->MacAddress() )
	#else
		BDCommandProcessor_Linux_Bluetooth( sMacAddressPhone, pBluetooth_Dongle->MacAddress(), pPhoneDevice )
	#endif //WIN32
#endif //BT_SOCKET
{
	m_pBluetooth_Dongle = pBluetooth_Dongle;
}

//-----------------------------------------------------------------------------------------------------