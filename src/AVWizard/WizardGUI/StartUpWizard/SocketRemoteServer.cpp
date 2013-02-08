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
#include "SocketRemoteServer.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#include <iostream>
#include <stdio.h>

#include "GUIWizardUtils.h"

void signal_handler(int signal);

SocketRemoteServer::SocketRemoteServer()
{
	this->SocketDescriptor = 0;
	ServerThreadID = 0;
	pthread_mutex_init(&SafeMutex, NULL);
}


SocketRemoteServer::~SocketRemoteServer()
{
	pthread_mutex_destroy(&SafeMutex);	
}

int SocketRemoteServer::StartServer(int Port)
{
	SocketDescriptor = socket( AF_INET, SOCK_STREAM, 0 );
	if (!SocketDescriptor)
	{
		std::cout<<"Error on creating socket"<<std::endl; // couldn't set the sock option
		return -10;
	}

	int iOpt = 1;
	if( setsockopt( SocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &iOpt, sizeof(iOpt) ) ) // error
	{
		std::cout<<"setsockopt failed"<<std::endl; // couldn't set the sock option
		close(SocketDescriptor);
		SocketDescriptor = 0;
		return -1;
	}

	struct sockaddr_in  addrT;
	memset( &addrT, 0, sizeof(addrT) ); // clearing it
	addrT.sin_family = AF_INET;
	addrT.sin_port = htons( Port );
	addrT.sin_addr.s_addr = INADDR_ANY;

	if(bind(SocketDescriptor, (struct sockaddr*)&addrT, sizeof(addrT)) != 0)
	{
		std::cout<<"Error on bind"<<std::endl; // couldn't set the sock option
		close(SocketDescriptor);
		SocketDescriptor = 0;
		return -2;
	}

	if(listen(SocketDescriptor, SOMAXCONN) != 0)
	{
		std::cout<<"Error on listen"<<std::endl; // couldn't set the sock option
		close(SocketDescriptor);
		SocketDescriptor = 0;
		return -3;
	}

	long dwFlags = fcntl( SocketDescriptor, F_GETFL );
	dwFlags |= O_NONBLOCK;
	fcntl( SocketDescriptor, F_SETFL, dwFlags );

	// set close-on-exec flag
	dwFlags = fcntl(SocketDescriptor, F_GETFD);
	dwFlags |= FD_CLOEXEC;
	fcntl(SocketDescriptor, F_SETFD, dwFlags);

	if(ServerThreadID == 0)
		pthread_create(&ServerThreadID, NULL, ServerThreadCallBack, this);

	return 0;
}

void SocketRemoteServer::MainLoop()
{
	if(!SocketDescriptor)
		return;

	signal(SIGPIPE, signal_handler);
	Finish = false;
	Closed = false;
	while(!Finish)
	{
		fd_set rfds;
		struct timeval tv;

		int iRet = 0;

		while( !Finish && iRet != 1 )
		{
			FD_ZERO( &rfds );
			FD_SET( SocketDescriptor, &rfds );
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			fflush( stdout );
			iRet = select( (int) (SocketDescriptor+1), &rfds, NULL, NULL, &tv );
		}
		if( Finish )
			break;

		struct sockaddr_in addr;
		socklen_t len = sizeof( struct sockaddr );
		int newsock = accept( SocketDescriptor, (sockaddr *)&addr, &len ); // creating a new socket
		if ( newsock != 0 )
		{
			if ( Closed )
			{
				std::cout<<"Got an incoming connection, but the server state is closed."<<std::endl;
				send( newsock, "CLOSED\n", 6, 0 );
				close( newsock );
			}
			else
			{
				std::cout<<"TCPIP: Accepting incoming connection on socket "<< newsock <<", from IP "<<
					inet_ntoa(addr.sin_addr) <<"."<<std::endl;

				long dwFlags = fcntl( newsock, F_GETFL );
				fcntl( newsock, F_SETFL, dwFlags );

				CreateClient(newsock);

			}
		}
		else
		{
			if ( !Finish )
			{
				std::cout<<"Invalid connection socket (accept failed).\r"<<std::endl;
				wizSleep( 100 );
			}
		}
		wizSleep( 0 );
	}
	std::cout<<"Server closed!"<<std::endl;
}

void SocketRemoteServer::CreateClient(int SocketClient)
{
	SocketRemoteClient* Client = new SocketRemoteClient(this, SocketClient);
	Clients.push_back(Client);
}

void* ServerThreadCallBack(void* SocketServerCallBackPtr)
{
	SocketRemoteServer*Server = (SocketRemoteServer*)SocketServerCallBackPtr;
	Server->MainLoop();
	return NULL;
}
