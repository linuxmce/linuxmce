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
#ifndef BDCommandProcessor_Smartphone_Bluetooth_h
#define BDCommandProcessor_Smartphone_Bluetooth_h

#include "BDCommandProcessor.h"

class BDCommandProcessor_Smartphone_Bluetooth : public BDCommandProcessor
{
public:
	SOCKET m_ServerSocket;
	SOCKET m_ClientSocket;

	pluto_pthread_mutex_t m_ClientSocketMutex;
	pluto_pthread_mutex_t m_ServerSocketMutex;
	pthread_cond_t m_ClientSocketCond;
	pthread_cond_t m_ServerSocketCond;

	bool m_bClientConnected;
	bool m_bRunning;
	
	pthread_t m_ServerThreadID;
	pthread_t m_ProcessCommandsThreadID;

	BDCommandProcessor_Smartphone_Bluetooth();
	~BDCommandProcessor_Smartphone_Bluetooth();

	//from BDCommandProcessor
	bool SendData(int size,const char *data);
	char *ReceiveData(int size);

	void AddCommand(class BDCommand *pCommand);

private:
	bool BT_SetService();
};

#endif
