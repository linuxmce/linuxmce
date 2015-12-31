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
#include "BDCommandProcessor_Smartphone_Bluetooth.h"

#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Logger.h"
#include "BD/BDCommandProcessor.h"
#include "BD/BDCommand.h"

#include "VIPShared/BD_PC_ReportMyVersion.h"
#include "PlutoUtils/CommonIncludes.h"

#include "Orbiter/CENet_Smartphone/OrbiterApp.h"
using namespace DCE;

#if defined(SMARTPHONE2005) || defined(_VC80_UPGRADE)			//--- CHANGED4WM5 ----//
	#include <wce_time.h>
	#define time		wceex_time
	#define localtime	wceex_localtime
	#define mktime		wceex_mktime
#endif

//------------------------------------------------------------------------------------------------------------
#include <Ws2bth.h>
#include <bthapi.h>
//------------------------------------------------------------------------------------------------------------
#define RECV_TIMEDOUT 15 //seconds
#define BLUETOOTH_PORT 19
#define VERSION "2006.11.09"
//---------------------------------------------------------------------------------------------------------
void *ProcessCommandsThread(void *p)
{
	BDCommandProcessor_Smartphone_Bluetooth* pBDCommandProcessor = (BDCommandProcessor_Smartphone_Bluetooth* )p;

	OrbiterApp::GetInstance()->Show();

	#if defined(SMARTPHONE2005)
		HANDLE hWait = CreateEvent( NULL, FALSE, FALSE, _T("ProcessCommand_Wait") );
	#endif
	bool bImmediateCallback = false;
	while(!pBDCommandProcessor->m_bDead && pBDCommandProcessor->SendCommand(bImmediateCallback))
	{
		if(!bImmediateCallback) //nothing to process right now
		{
			timespec abstime;
			#if defined(SMARTPHONE2005)
				if (hWait)	WaitForSingleObject( hWait, 1000 );
			#else				
				abstime.tv_sec = (long) (time(NULL) + 1);
				abstime.tv_nsec = 0;

				PLUTO_SAFETY_LOCK(csm, pBDCommandProcessor->m_ClientSocketMutex);

				#ifdef DEBUG_BLUETOOTH
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "Nothing to process right now, sleeping 1 sec");
				#endif

				csm.TimedCondWait(abstime);
			#endif
			
		}
	}
	#if defined(SMARTPHONE2005)
		if ( hWait ) CloseHandle( hWait );
	#endif

	pBDCommandProcessor->m_bClientConnected = false;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Client disconnected. Waking up server thread");
	pthread_cond_broadcast(&pBDCommandProcessor->m_ServerSocketCond);

	OrbiterApp::GetInstance()->ShowDisconnected();

	return NULL;
}
//------------------------------------------------------------------------------------------------------------
void *ServerThread(void *p)
{
	BDCommandProcessor_Smartphone_Bluetooth *pBDCommandProcessor = (BDCommandProcessor_Smartphone_Bluetooth *)p;
	pBDCommandProcessor->m_bRunning = true;

	while(!pBDCommandProcessor->m_bDead) 
	{
		SOCKADDR_BTH sab;
		int ilen = sizeof(sab);

		// Moved in OrbiterApp to avoid a crash
		//Sleep(10000);
		//OrbiterApp::GetInstance()->Hide();

		pBDCommandProcessor->m_ClientSocket = accept(pBDCommandProcessor->m_ServerSocket, (SOCKADDR *)&sab, &ilen);
		if (pBDCommandProcessor->m_ClientSocket == INVALID_SOCKET) 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to accept connection, error %d", WSAGetLastError());
			continue;
		}

		pBDCommandProcessor->m_bClientConnected = true;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Client successfully connected. Connection came from %04x%08x to channel %d",
			GET_NAP(sab.btAddr), GET_SAP(sab.btAddr), sab.port);

		pBDCommandProcessor->AddCommand(new BD_PC_ReportMyVersion(string(VERSION)));
		pthread_create(&pBDCommandProcessor->m_ProcessCommandsThreadID, NULL, ProcessCommandsThread, 
			(void*)pBDCommandProcessor); 

		//--------------------------------------------
		OrbiterApp::GetInstance()->Connected();
		//------------ Send Configuration ------------
		OrbiterApp::GetInstance()->SendConfiguration();
		//-------
		
		PLUTO_SAFETY_LOCK(ssm, pBDCommandProcessor->m_ServerSocketMutex);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Client connected. Sleeping until no client is connected.");
		ssm.CondWait();
	}

	pBDCommandProcessor->m_bRunning = false;
	return NULL;
}
//------------------------------------------------------------------------------------------------------------
BDCommandProcessor_Smartphone_Bluetooth::BDCommandProcessor_Smartphone_Bluetooth()
	: BDCommandProcessor("dummy"), m_ClientSocketMutex("client socket mutex"), 
	m_ServerSocketMutex("sever socket mutex"), m_ServerThreadID(pthread_t(NULL)),
	m_ProcessCommandsThreadID(pthread_t(NULL))
{
	pthread_cond_init(&m_ClientSocketCond, NULL);
	m_ClientSocketMutex.Init(NULL,&m_ClientSocketCond);

	pthread_cond_init(&m_ServerSocketCond, NULL);
	m_ServerSocketMutex.Init(NULL,&m_ServerSocketCond);

	m_ServerSocket = 0;
	m_ClientSocket = 0;

	m_bDead = false;
	m_bClientConnected = false;
	m_bRunning = false;

	//setting the service
	BT_SetService();

	//creating the socket
	m_ServerSocket = socket (AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);	
	if(m_ServerSocket == INVALID_SOCKET) 
	{
		m_bDead = true;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Socket creation failed, error %d", WSAGetLastError());
		return;
	}

	//preparing bind structures
	SOCKADDR_BTH sab;
	memset (&sab, 0, sizeof(sab));
	sab.addressFamily  = AF_BTH;
	sab.port = BLUETOOTH_PORT;

	//binding to the port
	if(0 != bind(m_ServerSocket, (SOCKADDR *) &sab, sizeof(sab))) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Socket bind, error %d", WSAGetLastError());
		closesocket (m_ServerSocket);
		return;
	}

	//listening
	listen (m_ServerSocket, SOMAXCONN);



	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Ready to accept connections!");
	pthread_create(&m_ServerThreadID, NULL, ServerThread, (void*)this); 
}
//------------------------------------------------------------------------------------------------------------
BDCommandProcessor_Smartphone_Bluetooth::~BDCommandProcessor_Smartphone_Bluetooth()
{
	m_bDead = true;

	//wait for client thread to finish
	pthread_cond_broadcast(&m_ClientSocketCond);
	if(m_ProcessCommandsThreadID)
	{
		pthread_join(m_ProcessCommandsThreadID, NULL);
		m_ProcessCommandsThreadID = (pthread_t)NULL;
	}

	//wait for server thread to finish
	pthread_cond_broadcast(&m_ServerSocketCond);
	if(m_ServerThreadID)
	{
		pthread_join(m_ServerThreadID, NULL);
		m_ServerThreadID = (pthread_t)m_ServerThreadID;
	}

	if(m_ServerSocket)
	{
		closesocket(m_ServerSocket);
		CloseHandle ((LPVOID)m_ServerSocket);
	}

	if(m_ClientSocket)
	{
		closesocket(m_ClientSocket);
		CloseHandle ((LPVOID)m_ClientSocket);
	}
	
	pthread_mutex_destroy(&m_ClientSocketMutex.mutex);
	pthread_mutex_destroy(&m_ServerSocketMutex.mutex);
}
//------------------------------------------------------------------------------------------------------------
bool BDCommandProcessor_Smartphone_Bluetooth::SendData(int size,const char *data)
{
#ifdef DEBUG_BLUETOOTH
//	PROF_START();
#endif

	if(!m_bClientConnected)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"The client is not connected!");
		return false;
	}

#ifdef DEBUG_BLUETOOTH
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending %d bytes of data", size);
#endif
	
	int br;
	int bytes_to_send = size;
	int bytes_sent = 0;

	while(bytes_to_send > 0 && !m_bDead)
	{
		//hehes
		br = send(m_ClientSocket, data + bytes_sent, bytes_to_send, 0);

		if(br == -1) 
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to send %d bytes of data", size);
			m_bClientConnected = false;
			return false;
		}
		else 
		{
			bytes_sent += br;
			bytes_to_send -= br;

#ifdef DEBUG_BLUETOOTH
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sent %d bytes [total %d/%d]", br, bytes_sent, size);
#endif
		}

		if ( br == 0 || WSAGetLastError() == WSAECONNRESET ) 
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Connection closed");
  		    return false;
		}
	}

	if(m_bDead)
	{
		m_bClientConnected = false;
		return false;
	}

#ifdef DEBUG_BLUETOOTH
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sent %d bytes of data", size);
#endif

#ifdef DEBUG_BLUETOOTH
//	PROF_STOP("Send data");
#endif

	return true;
}
//------------------------------------------------------------------------------------------------------------
char *BDCommandProcessor_Smartphone_Bluetooth::ReceiveData(int size)
{
#ifdef DEBUG_BLUETOOTH
//	PROF_START();
#endif

	if(!m_bClientConnected)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"The client is not connected!");
		return NULL;
	}

	char *buffer = (char *)malloc(size);

#ifdef DEBUG_BLUETOOTH
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to receive %d bytes of data", size);
#endif		

	int br = 0;
	int bytes_to_receive = size;
	int bytes_received = 0;

	while(bytes_to_receive > 0 && !m_bDead)
	{
		br = recv(m_ClientSocket, buffer + bytes_received, bytes_to_receive, 0);

		if (br==-1) 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to receive %d bytes of data", size);
			m_bClientConnected = false;
			return NULL;
		}
		else 
		{
			bytes_received += br;
			bytes_to_receive -= br;

#ifdef DEBUG_BLUETOOTH			
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received %d bytes [total %d/%d]", br, bytes_received, size);
#endif
		}

		if ( br == 0 || WSAGetLastError() == WSAECONNRESET ) 
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Connection closed");
  		    return NULL;
		}
	}

	if(m_bDead)
	{
		m_bClientConnected = false;
		return NULL;
	}

#ifdef DEBUG_BLUETOOTH
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received %d bytes of data", size);
#endif		

#ifdef DEBUG_BLUETOOTH
//	PROF_STOP("Receive data");
#endif

	return buffer; 
}
//------------------------------------------------------------------------------------------------------------
bool BDCommandProcessor_Smartphone_Bluetooth::BT_SetService()
{
	/*
   ULONG recordHandle = 0;
   #define SDP_RECORD_SIZE   0x0000004f

   BYTE rgbSdpRecord[] = {
         0x15, 0x4d, 0x09, 0x00, 0x01, 0x35, 0x11, 0x1c,
         0x29, 0xf9, 0xc0, 0xfd, 0xbb, 0x6e, 0x47, 0x97,
         0x9f, 0xa9, 0x3e, 0xc9, 0xa8, 0x54, 0x29, 0x0c,
         0x09, 0x00, 0x04, 0x35, 0x0c, 0x31, 0x03, 0x19,
         0x01, 0x00, 0x35, 0x05, 0x19, 0x00, 0x03, 0x08,
         0x1a, 0x09, 0x00, 0x06, 0x35, 0x09, 0x09, 0x65,
         0x6e, 0x09, 0x00, 0x6f, 0x09, 0x01, 0x00, 0x09,
         0x00, 0x09, 0x35, 0x08, 0x35, 0x06, 0x19, 0x11,
         0x05, 0x09, 0x01, 0x00, 0x09, 0x01, 0x00, 0x25,
         0x06, 0x53, 0x65, 0x72, 0x69, 0x61, 0x6a
   };
   struct 
   {
      BTHNS_SETBLOB   b;
      unsigned char   uca[SDP_RECORD_SIZE];
   } bigBlob;
   ULONG ulSdpVersion = BTH_SDP_VERSION;
   bigBlob.b.pRecordHandle   = &recordHandle;
   bigBlob.b.pSdpVersion     = &ulSdpVersion;
   bigBlob.b.fSecurity       = 0;
   bigBlob.b.fOptions        = 0;
   bigBlob.b.ulRecordLength  = SDP_RECORD_SIZE;
   memcpy (bigBlob.b.pRecord, rgbSdpRecord, SDP_RECORD_SIZE);
   
   BLOB blob;
   blob.cbSize    = sizeof(BTHNS_SETBLOB) + SDP_RECORD_SIZE - 1;
   blob.pBlobData = (PBYTE) &bigBlob;

   WSAQUERYSET Service;
   memset (&Service, 0, sizeof(Service));
   Service.dwSize = sizeof(Service);
   Service.lpBlob = &blob;
   Service.dwNameSpace = NS_BTH;

   return WSASetService(&Service,RNRSERVICE_REGISTER,0) != SOCKET_ERROR;
   */
	return true;
}
//------------------------------------------------------------------------------------------------------------
void BDCommandProcessor_Smartphone_Bluetooth::AddCommand(BDCommand *pCommand)
{
	BDCommandProcessor::AddCommand(pCommand);

#ifdef DEBUG_BLUETOOTH
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "You got mail! Wake up and process the command, dear Mr. Socket.");
#endif

	pthread_cond_broadcast(&m_ClientSocketCond);
}
//------------------------------------------------------------------------------------------------------------