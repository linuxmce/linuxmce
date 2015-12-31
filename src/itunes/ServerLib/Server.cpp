
#include "stdafx.h"
#include "Server.h"
#include "ServerConnection.h"
#include "Log.h"

using namespace PlutoITunesMediaConnector::Broadcaster;

///////////////////////////////////////////////////////////////////////////
// Threads:
///////////////////////////////////////////////////////////////////////////

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {
		unsigned __stdcall ServerThread(void * __args);
	}
}

unsigned __stdcall PlutoITunesMediaConnector::Broadcaster::ServerThread(void * __args) {

	REGISTER_RUNTIME_POINT("ServerThread", __FILE__, __LINE__);

	CoInitialize(0);

	Server::ServerThreadParams *const params = (Server::ServerThreadParams*)__args;

	HANDLE events[3] = {
		params->shutdownEvent,
		params->dbUpdateEvent,
		WSACreateEvent()
	};

	WSAEventSelect(params->socket, events[2], FD_ACCEPT);

    DWORD waitTime = INFINITE;
	for (;;) {
		DWORD object = WaitForMultipleObjects(3, events, FALSE, waitTime);
		if (object == WAIT_TIMEOUT) {
		    waitTime = INFINITE;
		} else {
			object -= WAIT_OBJECT_0;
			if (object == 2) {
				sockaddr sa;
				int sockAddrSize = sizeof(sa);
				SOCKET accepted;
				if ((accepted = accept(params->socket, &sa, &sockAddrSize)) != INVALID_SOCKET) {
					ServerConnection connection(accepted);
					closesocket(accepted);
				}
			} else if (object == 1) {
				// update db
			} else {
				break;
			}
			ResetEvent(events[object]);
		}
	}

	CoUninitialize();

	_endthreadex(0);
	return 0;
}


///////////////////////////////////////////////////////////////////////////
// Class:
///////////////////////////////////////////////////////////////////////////

Server::Server() {
	REGISTER_RUNTIME_POINT("Server::Server", __FILE__, __LINE__);
	CoInitialize(0);
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);
}


Server::~Server(void) {
	REGISTER_RUNTIME_POINT("Server::~Server", __FILE__, __LINE__);
	stop();
	WSACleanup();
	CoUninitialize();
}


Server& Server::getInstance(void) {
	REGISTER_RUNTIME_POINT("Server::getInstance", __FILE__, __LINE__);
	static Server server;
	return server;
}


void Server::start(	const unsigned short int port,
					const unsigned short int connectionsPoolSize,
					HANDLE dbUpdateEvent) {

	REGISTER_RUNTIME_POINT("Server::start", __FILE__, __LINE__);

	if (_threadParams.thread) {
		stop();
	}
	
	_threadParams.internetAddr.sin_port = htons(port);
	_threadParams.socket = socket(PF_INET, SOCK_STREAM, 0);
	if (	SOCKET_ERROR == bind(_threadParams.socket, (LPSOCKADDR)&_threadParams.internetAddr, sizeof _threadParams.internetAddr)
		||	listen(_threadParams.socket, port)) {

		// silent error
		stop();

	} else {

		_threadParams.connectionsPoolSize = connectionsPoolSize;
		_threadParams.dbUpdateEvent = dbUpdateEvent;
		
		_threadParams.shutdownEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		ResetEvent(_threadParams.shutdownEvent);
		
		_threadParams.thread = (HANDLE)_beginthreadex(
			NULL, 0, &ServerThread, &_threadParams, 0, &_threadParams.threadId);
		if (!_threadParams.thread) {
			// silent error
			stop();
		}

	}

}


void Server::stop() {
	
	REGISTER_RUNTIME_POINT("Server::stop", __FILE__, __LINE__);

	if (_threadParams.thread) {
		SetEvent(_threadParams.shutdownEvent);
		WaitForSingleObject(_threadParams.thread, INFINITE);
		_threadParams.thread = NULL;
	}

	if (_threadParams.socket) {
		closesocket(_threadParams.socket);
		_threadParams.socket = NULL;
	}

	if (_threadParams.shutdownEvent) {
		CloseHandle(_threadParams.shutdownEvent);
		_threadParams.shutdownEvent = NULL;
	}

}
