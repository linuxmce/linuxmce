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
#ifndef BDCommandProcessor_Windows_Bluetooth_h
#define BDCommandProcessor_Windows_Bluetooth_h

#include "BDCommandProcessor.h"

#include "WinBluetooth/inc/BtIfDefinitions.h"
#include "WinBluetooth/inc/BtIfClasses.h"
#include "WinBluetooth/StdDll/StdDLLif.h"
#include "WinBluetooth/StdDll/BtServiceSelection.h"

class BDCommandProcessor_Windows_Bluetooth : public BDCommandProcessor, public CRfCommTranspCallbackIf
{
protected:
	class Socket *m_pSocket;
	CRfCommTransport	*m_ptr_rfcm;
	bool	m_IsConnected;
	char *m_pReceiveBuffer;
	unsigned long m_iSizeReceiveBuffer;
	pluto_pthread_mutex_t m_ReceiveBufferMutex;

public:
	pthread_t m_BDSockThreadID;

	BDCommandProcessor_Windows_Bluetooth(string sMacAddressPhone,string sMacAddressDongle);
	~BDCommandProcessor_Windows_Bluetooth();

	bool SendData(int size,const char *data);
	char *ReceiveData(int size);

	void OnTranspEventReceived(  UINT32 event_code )
	{
	}
	void OnTranspDataReceived( BYTE* data, ULONG len );


};

#endif
