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
#ifndef __BDCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__
#define __BDCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__

namespace DCE
{
	class Bluetooth_Dongle;
}

using namespace DCE;

//-----------------------------------------------------------------------------------------------------
#ifdef BT_SOCKET
#else
	#ifdef WIN32
		#include "../BD/BDCommandProcessor_Windows_Bluetooth.h"
	#else
		#include "../BD/BDCommandProcessor_Linux_Bluetooth.h"
	#endif //WIN32
#endif //BT_SOCKET
//-----------------------------------------------------------------------------------------------------
#ifdef BT_SOCKET
	#ifdef WIN32
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Windows_Socket
	#else
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Linux_Socket
	#endif
#else
	#ifdef WIN32
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Windows_Bluetooth
	#else
class BDCommandProcessor_BluetoothDongle : public BDCommandProcessor_Linux_Bluetooth
	#endif //WIN32
#endif //BT_SOCKET
{
public:

	class Bluetooth_Dongle *m_pBluetooth_Dongle;

	BDCommandProcessor_BluetoothDongle(
		Bluetooth_Dongle *pBluetooth_Dongle,
		string sMacAddressPhone,
		class PhoneDevice *pPhoneDevice
#ifdef BT_SOCKET
		,class Socket *pSocket
#endif
	);
};
//-----------------------------------------------------------------------------------------------------
#endif // __BDCOMMANDPROCESSOR_BLUETOOTH_DONGLE_H__
