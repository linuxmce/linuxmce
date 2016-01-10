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
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef SOCKETREMOTESERVER_H
#define SOCKETREMOTESERVER_H
#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

/**
	That class is used to get commands remote (using a socket interface)
	@author Ciprian Mustiata <ciprian.m@plutohome.com>
*/

#include "SocketRemoteClient.h"
#include <vector>
#include <pthread.h>

class SocketRemoteServer{
	int SocketDescriptor;
	bool Closed;

	std::vector <SocketRemoteClient*> Clients;
	pthread_t ServerThreadID;
public:
	pthread_mutex_t SafeMutex;

	bool Finish;

    SocketRemoteServer();
    ~SocketRemoteServer();

	int StartServer(int Port);
	void Terminate();
	
	void MainLoop();

	void CreateClient(int SocketClient);

};

void Sleep(int Miliseconds);
void* ServerThreadCallBack(void* SocketClientCallBackPtr);

#endif
