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
#ifndef BDCommandProcessor_Linux_Bluetooth_h
#define BDCommandProcessor_Linux_Bluetooth_h

#include "BD/BDCommandProcessor.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

class BDCommandProcessor_Linux_Bluetooth : public BDCommandProcessor
{
protected:
	int m_CommHandle;
	bool m_bQuit;
	bool m_bRunning;
	class PhoneDevice *m_pDevice;

public:
	BDCommandProcessor_Linux_Bluetooth(string sMacAddressPhone,string sMacAddressDongle,class PhoneDevice *pDevice);
	virtual ~BDCommandProcessor_Linux_Bluetooth() ;

	void Connect();

	bool SendData(int size,const char *data);
	char *ReceiveData(int size);

};

#endif
/*


class MobileOrbiterSocket
{
	int m_CommHandle;
	string m_ID;
	bool m_bExit;
public:	
	pthread_t m_BDSockThreadID;
	bool m_bDead;
	
	MobileOrbiterSocket(int CommHandle, string ID, MOBILE_ORBITER_DETECTORCommandImpl *pCmd);
	~MobileOrbiterSocket();
	
	bool WriteFile(int handle, char* buffer,int maxbytes, long unsigned int* byteswritten);
	bool ReadFile(int handle,char* buffer,int maxbytes,long unsigned int* bytesread);
	bool WriteImageToPhone(char *Image, int ImageLength);
	void PressedKey(string Key);
	void ProcessSocket();
};

*/
