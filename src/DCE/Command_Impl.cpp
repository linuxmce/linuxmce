/*
	Command_Impl

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU Lesser General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"
#include "Gen_Devices/AllCommandsRequests.h"

#ifdef LINK_TO_ROUTER
#include "DCERouter.h"
#endif

#if defined(WIN32) && !defined(WINCE)
	#include <conio.h>
#endif

#include "DeviceData_Impl.h"
#include "Command_Impl.h"
#include "Event_Impl.h"
#include "PreformedCommand.h"

using namespace DCE;

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);

Command_Impl *g_pCommand_Impl=NULL;

/**
 * @brief entry point for the MessageQueue thread
 */
void* MessageQueueThread_DCECI( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	Command_Impl* p = (Command_Impl*)param;
	if( p->m_bMessageQueueThreadRunning )  // The constructor should have set this to true
		p->ProcessMessageQueue();
	p->m_bMessageQueueThreadRunning=false;

	LoggerWrapper::GetInstance()->Write(LV_SOCKET,"Exiting MessageQueueThread_DCECI thread...");

	return NULL;
}

void *WatchDogThread( void *pData )
{
#ifdef WIN32

	Command_Impl *pCommand_Impl = (Command_Impl *)pData;

	Sleep(3000);
	bool bConnectionLost = false;

	while( !pCommand_Impl->m_bStopWatchdog)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to send PING to the router.");
#endif
		string sResponse = pCommand_Impl->SendReceiveString( "PING " + StringUtils::itos( pCommand_Impl->m_dwPK_Device ), PING_TIMEOUT );
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sent PING to the router.");
#endif

		if ( sResponse != "PONG" )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Before Disconnect.");
			pCommand_Impl->Disconnect();
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "After Disconnect.");

			bConnectionLost = true;
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Connection for client socket reported %s, device %d",
				sResponse.c_str(), pCommand_Impl->m_dwPK_Device );
			break;
		}
		else
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received PONG from the router.");
#endif
			Sleep(5000);
		}
	}

	if(bConnectionLost)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Before MB_ICONEXCLAMATION.");
		::MessageBeep( MB_ICONEXCLAMATION );
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WatchDog stopped");
	pCommand_Impl->m_bWatchdogRunning = false;

#endif

	return NULL;
}

Command_Impl::Command_Impl( int DeviceID, string ServerAddress, bool bLocalMode, class Router *pRouter )
	: HandleRequestSocket( DeviceID, ServerAddress, "Command_Impl1 Dev #" + StringUtils::itos(DeviceID) ), m_listMessageQueueMutex( "MessageQueue" )
{
	m_bRouterReloading = false;
	m_iInstanceID = (int) time(NULL);
	m_pEvent = NULL;
	m_pData = NULL;
	m_bKillSpawnedDevicesOnExit = true;
	m_pRouter = pRouter;
	m_pcRequestSocket = NULL;
	m_bLocalMode = bLocalMode;
	m_bReload = false;
	m_pPrimaryDeviceCommand = this;
	m_bHandleChildren = false;
	m_bMessageQueueThreadRunning = false;
	m_pParent = NULL;
	m_bAskBeforeReload=false;
	m_bImplementsPendingTasks=false;
	pthread_cond_init( &m_listMessageQueueCond, NULL );
	m_listMessageQueueMutex.Init( NULL, &m_listMessageQueueCond );
	m_bGeneric = false;
	m_dwMessageInterceptorCounter=1;
	m_pMessageBuffer=NULL;
#ifdef PTHREAD2
        m_pthread_queue_id.p=0;
#else
        m_pthread_queue_id=0;
#endif
}

Command_Impl::Command_Impl( Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, class Router *pRouter )
	: HandleRequestSocket( pData->m_dwPK_Device, pPrimaryDeviceCommand->m_sIPAddress, "Command_Impl2 Dev #" ), m_listMessageQueueMutex( "MessageQueue" )
{
	m_bRouterReloading = false;
	m_bKillSpawnedDevicesOnExit=true;
	m_pRouter = pRouter;
	m_pcRequestSocket = NULL;
	m_bReload = false;
	m_pPrimaryDeviceCommand = pPrimaryDeviceCommand;
	m_bLocalMode = m_pPrimaryDeviceCommand->m_bLocalMode;
	m_pData = pData;
	m_pEvent = pEvent;
	m_bHandleChildren = false;
	m_bMessageQueueThreadRunning = false;
	m_bAskBeforeReload=false;
	m_bImplementsPendingTasks=false;
	pthread_cond_init( &m_listMessageQueueCond, NULL );
	m_listMessageQueueMutex.Init( NULL, &m_listMessageQueueCond );
	m_bGeneric = false;
	m_dwMessageInterceptorCounter=0;
        m_pMessageBuffer=NULL;
#ifdef PTHREAD2
        m_pthread_queue_id.p=0;
#else
        m_pthread_queue_id=0;
#endif
	// Connect the primary device's event processor
	// to the Data class so Data class changes can be sent.
	m_pData->m_pEvent_Impl = m_pPrimaryDeviceCommand->m_pEvent;
}

Command_Impl::~Command_Impl()
{
	g_pCommand_Impl = NULL;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Waiting for message queue thread to quit" );

	m_bQuit_set(true);

	PrepareToDelete();

	if (m_pData)
		delete m_pData;
	if (this==m_pPrimaryDeviceCommand && m_pEvent)
		delete m_pEvent;
	if( m_pcRequestSocket )
		delete m_pcRequestSocket;

	m_pEvent=m_pcRequestSocket=NULL;

	for(list<Message *>::iterator it = m_listMessageQueue.begin(); it != m_listMessageQueue.end(); it++)
		delete *it;
	m_listMessageQueue.clear();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "~Command_Impl finished" );

	pthread_mutex_destroy(&m_listMessageQueueMutex.mutex);
}

void Command_Impl::PrepareToDelete()
{
	m_bQuit_set(true);
	if( m_pEvent && m_pEvent->m_pClientSocket && m_pEvent->m_pClientSocket->m_Socket != INVALID_SOCKET )
		m_pEvent->m_pClientSocket->Close();
	if( m_pcRequestSocket && m_pcRequestSocket->m_pClientSocket && m_pcRequestSocket->m_pClientSocket->m_Socket != INVALID_SOCKET )
		m_pcRequestSocket->m_pClientSocket->Close();

	time_t tTime = time(NULL);

	// Stop any threads
	while( m_bMessageQueueThreadRunning )
	{
		pthread_cond_broadcast( &m_listMessageQueueCond );
		Sleep(10); // Should happen right away
		if( tTime + 5 < time(NULL) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"m_bMessageQueueThreadRunning had blocked!!!!");
			exit(1);
		}
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Message queue thread quit" );
#ifdef PTHREAD2
        if( m_pthread_queue_id.p != 0 )
        {
                pthread_join(m_pthread_queue_id, NULL);
                m_pthread_queue_id.p = 0;
        }
#else
        if( m_pthread_queue_id != 0 )
        {
                pthread_join(m_pthread_queue_id, NULL);
                m_pthread_queue_id = 0;
        }
#endif
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Message queue thread joined" );

	if( m_bKillSpawnedDevicesOnExit )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "About to call kill spawned devices" );
		KillSpawnedDevices();
	}
}

Command_Impl *Command_Impl::CreateCommand( int iPK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent )
{
	if ( m_pPrimaryDeviceCommand != this )
	{
		return m_pPrimaryDeviceCommand->CreateCommand( iPK_DeviceTemplate, pPrimaryDeviceCommand, pData, pEvent );
	}
	return NULL;
}

void Command_Impl::CreateChildren()
{
	for( int i=0; i < (int)m_pData->m_vectDeviceData_Impl_Children.size(); i++ )
	{
		DeviceData_Impl *pDeviceData_Impl_Child = m_pData->m_vectDeviceData_Impl_Children[i];

		// This device was marked as disabled
		if (pDeviceData_Impl_Child->m_bDisabled)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Child device %d is disabled", pDeviceData_Impl_Child->m_dwPK_Device);
			continue;
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating child %d", pDeviceData_Impl_Child->m_dwPK_Device);

		// This device has it's own executible. Try to spawn it. If that fails, we will try to create it ourselves
		if( pDeviceData_Impl_Child->m_bImplementsDCE && !pDeviceData_Impl_Child->m_bIsEmbedded )
		{
			if( SpawnChildDevice(pDeviceData_Impl_Child->m_dwPK_Device,pDeviceData_Impl_Child->m_sCommandLine) )
				continue;
		}
		Event_Impl *pEvent = m_pEvent->CreateEvent( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
		if ( !pEvent )
		{
			pEvent = new Event_Impl( m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom event handler for.  It will not fire events.",
					pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
		}
		Command_Impl *pCommand = m_pPrimaryDeviceCommand->CreateCommand( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent );
		if ( !pCommand )
		{
			pCommand = new Command_Impl(m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent, m_pRouter);
			pCommand->m_bGeneric=true;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom handler for.  This is normal for IR.", pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
		}
		pCommand->m_pParent = this;
		pCommand->CreateChildren();
		m_mapCommandImpl_Children[pDeviceData_Impl_Child->m_dwPK_Device] = pCommand;
	}
}

void Command_Impl::CreateNewChildren()
{
	string sChildren = m_pEvent->SendReceiveString("CHILD_DEVICES " + StringUtils::itos(m_dwPK_Device),5);
	if( StringUtils::StartsWith(sChildren,"CHILD_DEVICES") )
	{
		string::size_type pos = 14;
		while(pos<sChildren.size())
		{
			int PK_Device=atoi(StringUtils::Tokenize(sChildren,"\t",pos).c_str());
			string sCommand = StringUtils::Tokenize(sChildren,"\t",pos);

			if( !PK_Device || sCommand.empty() )
				continue;

			PLUTO_SAFETY_LOCK( sSM, m_listMessageQueueMutex );  // Use this to protect m_mapSpawnedDevices
			if( m_mapSpawnedDevices.find(PK_Device)==m_mapSpawnedDevices.end() )
				SpawnChildDevice(PK_Device,sCommand);
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Command_Impl::CreateNewChildren can't get list from router");

	PLUTO_SAFETY_LOCK( sSM, m_listMessageQueueMutex );  // Use this to protect m_mapSpawnedDevices
	for(map<int,string>::iterator it=m_mapSpawnedDevices.begin();it!=m_mapSpawnedDevices.end();++it)
	{
		Message *pMessage = new Message(m_dwPK_Device,it->first,PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_SPAWN_NEW_CHILDREN,0);
		QueueMessageToRouter(pMessage);
	}
}

void Command_Impl::PostConfigCleanup()
{
	m_pData->m_pDeviceCategory = m_pData->m_AllDevices.m_mapDeviceCategory_Find( m_pData->m_dwPK_DeviceCategory );
	for(VectDeviceData_Impl::iterator it=m_pData->m_vectDeviceData_Impl_Children.begin();it!=m_pData->m_vectDeviceData_Impl_Children.end();++it)
		PostConfigCleanup(*it);
}

void Command_Impl::PostConfigCleanup(DeviceData_Impl *pDevice)
{
	pDevice->m_pDeviceCategory = m_pData->m_AllDevices.m_mapDeviceCategory_Find( pDevice->m_dwPK_DeviceCategory );
	for(VectDeviceData_Impl::iterator it=pDevice->m_vectDeviceData_Impl_Children.begin();it!=pDevice->m_vectDeviceData_Impl_Children.end();++it)
		PostConfigCleanup(*it);
}

bool Command_Impl::SpawnChildDevice( int PK_Device, string sCommand, string sDisplay )
{
#ifndef WIN32
	string sPrefix="";

	// searching for the Spawn_Device script in some locations
	if( !FileUtils::FileExists( sPrefix + "Spawn_Device.sh" ) )
	{
		sPrefix = "./";
		if( !FileUtils::FileExists( sPrefix + "Spawn_Device.sh" ) )
		{
            sPrefix = "/usr/pluto/bin";
			if( !FileUtils::FileExists( sPrefix + "Spawn_Device.sh" ) )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Spawn_Device.sh");
				return false;
			}
		}
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Spawning device: %d %s on display: %s ip: %s", PK_Device, sCommand.c_str(), sDisplay.c_str(), m_sIPAddress.c_str() );
	system( ("screen -d -m -h 3000 -S " + sCommand + "_" + StringUtils::itos( PK_Device ) +
			" /bin/bash -x " + sPrefix + "Spawn_Device.sh " + StringUtils::itos(PK_Device) + " " + m_sIPAddress + " " + sDisplay + " " + sCommand).c_str() );
#else
	//TO BE IMPLEMENTED
	/*
	#ifndef WINCE
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory ( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory ( &pi, sizeof(pi) );
		CreateProcess( "C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	#endif
	*/
#endif

	PLUTO_SAFETY_LOCK( sSM, m_listMessageQueueMutex );  // Use this to protect m_mapSpawnedDevices
	m_mapSpawnedDevices[PK_Device]=sCommand; // save the spawned device

	return true;
}

void Command_Impl::KillSpawnedDevices()
{
#ifndef WIN32
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to kill %d child devices", (int)m_mapSpawnedDevices.size());
	/*
	string sSpawnedDevices("");
	for( size_t s=0; s < m_vectSpawnedDevices.size(); ++s )
	{
		sSpawnedDevices += m_vectSpawnedDevices[s] + " ";
//		string sCmd = string("") + "screen -list | grep '" + m_vectSpawnedDevices[s] + "' | cut -f 1 -d '.' | cut -f 2 -d '\t' | xargs kill -9";
//		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Killing spawned device %s with %s",m_vectSpawnedDevices[s].c_str(),sCmd.c_str());
//		system( sCmd.c_str() );
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Initiating kill for: %s", sSpawnedDevices.c_str());
	*/
//	string sCmd = string("/usr/pluto/bin/KillScreens.sh ") + sSpawnedDevices;
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Cmd: %s", sCmd.c_str());
// Temporary hack - for some unexplainable reason, the KillScreens script, although it appears correct
// is actually causing *everything* to die, including the screen session and bash that runs Orbiter, preventing them from
// ever restarting.  Temporarily remove this, and let the spawned devices re-spawn themselves, not being tied to Orbiter
	//	system(sCmd.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Killing completed.");
#endif
}

void Command_Impl::ReplaceParams( ::std::string sReplacement ) {

	if ( sReplacement != "" )
	{
		string sParam1, sParam2;
		bool bDone;
		int iParam_value;

		string::size_type pos;
		pos = 0;
		bDone = false;

		do
		{
			sParam1 = StringUtils::Tokenize( sReplacement, ::std::string(","), pos ); // get the first token
			sParam2 = StringUtils::Tokenize( sReplacement, ::std::string(","), pos ); // get the second token

			iParam_value = atoi( sParam1.c_str() ); // convert it to int

			if (( sParam1 != "" ) && ( sParam2 != "" ) && ( iParam_value != 0 ))
			{
				m_pData->m_mapParameters.erase( iParam_value ); // erase the old value
				m_pData->m_mapParameters.insert( pair<int, string>( iParam_value, sParam2 ) ); // insert a new one
			}
			else bDone = true;

		} while ( !bDone );
	}
}

bool Command_Impl::Connect(int iPK_DeviceTemplate, std::string)
{
	if( m_bLocalMode )
	{
		PostConnect();
		return true;
	}

	bool bResult = true;

	// Must have an event socket to proceed.
	if (!m_pEvent)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No event handler for device ID: %d", this->m_dwPK_Device);
		bResult = false;
	}

	if (bResult && m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
	{
		m_pEvent->m_pClientSocket->Connect(iPK_DeviceTemplate);
		if (m_pEvent->m_pClientSocket->m_Socket == INVALID_SOCKET)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No client socket for device ID: %d", this->m_dwPK_Device);
			bResult = false;
		}
	}

	if( bResult )
	{
		m_bMessageQueueThreadRunning = true;
#ifdef PTHREAD2
                if(m_pthread_queue_id.p==0 && pthread_create( &m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this) )
                {
                        m_pthread_queue_id.p=0;
                        m_bMessageQueueThreadRunning=false;
                        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create message processing queue" );
                }
#else
                if(m_pthread_queue_id==0 && pthread_create( &m_pthread_queue_id, NULL, MessageQueueThread_DCECI, (void*)this) )
                {
                        m_pthread_queue_id=0;
                        m_bMessageQueueThreadRunning=false;
                        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create message processing queue" );
                }
#endif
#ifdef LL_DEBUG
		SendString("COMMENT COMMAND " + StringUtils::itos(DeviceID));
		m_pEvent->SendString("COMMENT EVENT " + StringUtils::itos(DeviceID));
		m_pRequestHandler->SendString("COMMENT REQHAND " + StringUtils::itos(DeviceID));
#endif
	}

	string sFlags;
	if( m_bAskBeforeReload )
		sFlags += " ASK_BEFORE_RELOAD ";
	if( m_bImplementsPendingTasks )
		sFlags += " IMPLEMENTS_PENDING_TASKS ";

	if (bResult && !ClientSocket::Connect(iPK_DeviceTemplate,(m_iInstanceID ? " INSTANCE " + StringUtils::itos(m_iInstanceID) : "") + sFlags ) )
	{
#if (!defined(UNDER_CE) || !defined(DEBUG))
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DeviceCommand connect failed %p, device ID: %d",this,this->m_dwPK_Device);
#endif

		if (m_pEvent)
			m_pEvent->m_pClientSocket->Disconnect();

		bResult = false;
	}

	if( m_Socket == INVALID_SOCKET )
		bResult = false;

#ifdef LINK_TO_ROUTER
	if(NULL != m_pRouter && m_pRouter->IsPlugin(m_dwPK_Device))
		m_pcRequestSocket->m_pClientSocket->SetSendReceiveTimeout(SOCKET_TIMEOUT_PLUGIN);
#endif

	if( m_pData && m_pData->m_bUsePingToKeepAlive )
	{
		// No need to start the ping loop for this socket because the CommandImpl
		// is an incoming socket, and the server will have already started this
		m_pEvent->m_pClientSocket->StartPingLoop();
		m_pcRequestSocket->m_pClientSocket->StartPingLoop();
		m_pEvent->m_pClientSocket->m_pSocket_PingFailure=this;
		m_pcRequestSocket->m_pClientSocket->m_pSocket_PingFailure=this;
	}

	m_bStopWatchdog = false;
	m_bWatchdogRunning = false;

#ifdef WIN32
	//StartWatchDog(10000);
#endif

	MiscCleanup();
	if( bResult )
		PostConnect();

	return bResult;
}

void Command_Impl::MiscCleanup()
{
	SetDeviceCategories(m_pData);
}

void Command_Impl::SetDeviceCategories(DeviceData_Impl *pData)
{
	m_pData->m_pDeviceCategory = pData->m_AllDevices.m_mapDeviceCategory_Find(m_pData->m_dwPK_DeviceCategory);
	for( int i=0; i < (int)m_pData->m_vectDeviceData_Impl_Children.size(); i++ )
		m_pData->m_vectDeviceData_Impl_Children[i]->m_pDeviceCategory = pData->m_AllDevices.m_mapDeviceCategory_Find(m_pData->m_vectDeviceData_Impl_Children[i]->m_dwPK_DeviceCategory);
}


bool Command_Impl::RouterNeedsReload()
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"The router must be reloaded before this device is fully functional");
	return false;
}

int Command_Impl::DeviceIdInvalid()
{
	if( m_dwPK_Device==DEVICEID_MESSAGESEND )
		return 0;
#ifndef WINCE
	cout << "Device ID is invalid.  Exiting..." << endl;
	exit(3);

	map<int,string> mapDevices;
	GetDevicesByTemplate(PK_DeviceTemplate_get(),&mapDevices);
	cout << "A valid device ID is required to connect to the router." << endl
		<< "Note that 2 devices cannot share the same ID." << endl;

	if( mapDevices.size()==0 )
		return 0;

	cout << "Enter 0 and run this with -d [DeviceID] or enter" << endl
		<< "a device ID.  Here is a list of devices with this" << endl
		<< "device template.  Devices with an * already" << endl
		<< "have registered and should not be used." << endl;

	for(map<int,string>::iterator it=mapDevices.begin();it!=mapDevices.end();++it)
		cout << it->first << " " << it->second << endl;
#ifdef WIN32
	int Response;
	cin >> Response;
	return Response;
#else
	return atoi(GetPrompt(30).c_str());
#endif

#else
	return 0;
#endif
}

void Command_Impl::GetDevicesByTemplate(int PK_DeviceTemplate,map<int,string> *p_mapDevices)
{
	Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName, true, -1, -1, m_bIsSSL);
	pEvent->m_pClientSocket->SendString("DEVICES BY TEMPLATE " + StringUtils::itos(PK_DeviceTemplate));
	string sResponse;
	if ( pEvent->m_pClientSocket->ReceiveString( sResponse ) && sResponse.substr(0,11)=="DEVICE_INFO" )
	{
		string::size_type pos=11; // Skip the "DEVICE_INFO"
		int NumDevices = atoi(StringUtils::Tokenize(sResponse,"\t",pos).c_str());
		for(int iCount=0;iCount<NumDevices;++iCount)
		{
			int DeviceID = atoi(StringUtils::Tokenize(sResponse,"\t",pos).c_str());
			string sName = StringUtils::Tokenize(sResponse,"\t",pos);
			string sRoom = StringUtils::Tokenize(sResponse,"\t",pos);
			string sDefaultIP = StringUtils::Tokenize(sResponse,"\t",pos);
			string sRegisteredIP = StringUtils::Tokenize(sResponse,"\t",pos);

			(*p_mapDevices)[DeviceID] =
				(sRegisteredIP.size() ? "* " + sRegisteredIP : "")
				+ " " + sName + " (" + sRoom + ")";
		}
	}
#ifndef for_windows
    delete pEvent;
#endif
}

void Command_Impl::GetDevicesByCategory(int PK_DeviceCategory,map<int,string> *p_mapDevices)
{
	Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName, true, -1, -1, m_bIsSSL);
	pEvent->m_pClientSocket->SendString("DEVICES BY CATEGORY " + StringUtils::itos(PK_DeviceCategory));
	string sResponse;
	if ( pEvent->m_pClientSocket->ReceiveString( sResponse ) && sResponse.substr(0,11)=="DEVICE_INFO" )
	{
		string::size_type pos=11; // Skip the "DEVICE_INFO"
		int NumDevices = atoi(StringUtils::Tokenize(sResponse,"\t",pos).c_str());
		for(int iCount=0;iCount<NumDevices;++iCount)
		{
			int DeviceID = atoi(StringUtils::Tokenize(sResponse,"\t",pos).c_str());
			string sName = StringUtils::Tokenize(sResponse,"\t",pos);
			string sRoom = StringUtils::Tokenize(sResponse,"\t",pos);
			string sDefaultIP = StringUtils::Tokenize(sResponse,"\t",pos);
			string sRegisteredIP = StringUtils::Tokenize(sResponse,"\t",pos);

			(*p_mapDevices)[DeviceID] =
				(sRegisteredIP.size() ? "* " + sRegisteredIP : "")
				+ " " + sName + " (" + sRoom + ")";
		}
	}
	delete pEvent;
}

void Command_Impl::CannotReloadRouter()
{
#ifndef WINCE
	cout << "The router cannot reload now.  Please try later." << endl
		<< "Press any key to continue...";
	#ifdef __APPLE_CC__
		std::cin.get();
	#else
		#ifndef WIN32
			getch_timeout(30);
		#else
			getch();
		#endif
	#endif
  	cout << endl;
#endif
}

void Command_Impl::ReceivedString( string sLine, int nTimeout/*= -1*/)
{
	if( sLine=="SAFE_TO_RELOAD" )
	{
		string sReason;
		if( SafeToReload(sReason) )
			SendString("RELOAD_YES");
		else
			SendString("RELOAD_NO " + sReason);
		return;
	}
	MapCommand_Impl::iterator i;
	for( i=m_mapCommandImpl_Children.begin(); i != m_mapCommandImpl_Children.end(); ++i )
	{
		(*i).second->ReceivedString( sLine, nTimeout );
	}
}

ReceivedMessageResult Command_Impl::ReceivedMessage( Message *pMessage )
{
	if( pMessage->m_dwMessage_Type == MESSAGETYPE_MESSAGE_INTERCEPTED )
	{
		InterceptedMessage(pMessage);
		return rmr_Processed;
	}

	// The class that overrides this will try to handle it
	// If it can't it will pass it to this function.
	map<long, string>::iterator p;

	// Ignore if it's not directly for us.  It could be for a virtual device
	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND && pMessage->m_dwPK_Device_To==m_dwPK_Device )
	{
		if( pMessage->m_dwID == SYSCOMMAND_QUIT )
		{
			SendString("BYE");
			Sleep(250);
			OnQuit();
			/* see notes in header file 1/2/2004
			Disconnect();
			if (m_pEvent && m_pEvent->m_pClientSocket)
				m_pEvent->m_pClientSocket->Disconnect();
			*/
			return rmr_Processed;
		}
		else if( pMessage->m_dwID == SYSCOMMAND_SEGFAULT )
		{
			char *pNULLPointer=NULL;
			strcpy(pNULLPointer,"this always crashes any os");
		}
		else if( pMessage->m_dwID == SYSCOMMAND_DEADLOCK )
		{
			PLUTO_SAFETY_LOCK( mq, m_listMessageQueueMutex );
			mq.m_bReleased=true;
			PLUTO_SAFETY_LOCK( sSM, m_SocketMutex );
			sSM.m_bReleased=true;
		}
	}

	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_SYSCOMMAND )
	{
		if( pMessage->m_dwID == SYSCOMMAND_RELOAD && pMessage->m_dwPK_Device_To==m_dwPK_Device )
		{
			if( pMessage->m_mapParameters.find(1)!=pMessage->m_mapParameters.end() )
				m_bRouterReloading=true;
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got a reload command from %d %s",pMessage->m_dwPK_Device_From,pMessage->m_mapParameters[COMMANDPARAMETER_Description_CONST].c_str());
			SendString("BYE");
			Sleep(250);
			OnReload();
			return rmr_Processed;
		}
		if( pMessage->m_dwID == SYSCOMMAND_ROTATE && pMessage->m_dwPK_Device_To==m_dwPK_Device)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Going to rotate logs...");
			LoggerWrapper::GetInstance()->Rotate();
			return rmr_Processed;
		}
		if( pMessage->m_dwID == SYSCOMMAND_RELOAD_LOGLEVEL && pMessage->m_dwPK_Device_To==m_dwPK_Device)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Going to reload log levels...");
			LoggerWrapper::GetInstance()->ReloadLogLevels();
			return rmr_Processed;
		}
		if( pMessage->m_dwID == SYSCOMMAND_SPAWN_NEW_CHILDREN && pMessage->m_dwPK_Device_To==m_dwPK_Device)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Going to spawn new children...");
			CreateNewChildren();
			return rmr_Processed;
		}
	}
	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_START_PING)
	{
		if( !m_pEvent->m_pClientSocket->m_bUsePingToKeepAlive )
			m_pEvent->m_pClientSocket->StartPingLoop();
		if( !m_pcRequestSocket->m_pClientSocket->m_bUsePingToKeepAlive )
			m_pcRequestSocket->m_pClientSocket->StartPingLoop();
		m_pEvent->m_pClientSocket->m_pSocket_PingFailure=this;
		m_pcRequestSocket->m_pClientSocket->m_pSocket_PingFailure=this;
	}
	else if ( pMessage->m_dwMessage_Type == MESSAGETYPE_STOP_PING)
	{
		m_pEvent->m_pClientSocket->m_bUsePingToKeepAlive = m_pcRequestSocket->m_pClientSocket->m_bUsePingToKeepAlive = false;
	}

	if ( m_bHandleChildren && pMessage->m_dwPK_Device_To != m_dwPK_Device )
	{
		// This is slightly inelegant because we don't want to change
		// the base class implementation in order to do this.  If the target device ID is not this device
		// and we're handling children, set the target device ID to the child, change the target,
		// and run it back through ReceivedMessage so the higher level class processes it as
		// it's own.
		m_iTargetDeviceID  = pMessage->m_dwPK_Device_To;
		pMessage->m_dwPK_Device_To = m_dwPK_Device;
		return ReceivedMessage( pMessage );
	}

	if ( pMessage->m_dwMessage_Type == MESSAGETYPE_DATAPARM_REQUEST && pMessage->m_dwPK_Device_To == m_dwPK_Device )
	{
		map<int, string>::iterator p = m_pData->m_mapParameters.find(pMessage->m_dwID);
		pMessage->m_bRespondedToMessage = true;
		if (p==m_pData->m_mapParameters.end())
		{
			SendMessage(new Message(m_dwPK_Device, pMessage->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, pMessage->m_dwID, "ERR Parameter not found"));
		}
		else
		{
			RequestingParameter(pMessage->m_dwID);
			SendMessage(new Message(m_dwPK_Device, pMessage->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, pMessage->m_dwID, (*p).second.c_str()));
		}

		return rmr_Processed;
	}
	else if ( pMessage->m_dwMessage_Type == MESSAGETYPE_PENDING_TASKS && pMessage->m_dwPK_Device_To == m_dwPK_Device )
	{
		PendingTaskList pendingTaskList;
		ReportPendingTasks(&pendingTaskList);
		string sResponse;

		Message *pMessage_Out = new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_PENDING_TASKS,0,0);
		pMessage_Out->m_mapParameters[0] = pendingTaskList.ToString();
		pMessage->m_bRespondedToMessage=true;
		SendMessage(pMessage_Out);
		return rmr_Processed;
	}
	else
		if ( pMessage->m_dwMessage_Type == MESSAGETYPE_DATAPARM_CHANGE && pMessage->m_dwPK_Device_To == m_dwPK_Device )
		{
			p = pMessage->m_mapParameters.begin();
			if ( p != pMessage->m_mapParameters.end() )
			{
				string ValueOld = m_pData->m_mapParameters[(*p).first];
				m_pData->m_mapParameters[(*p).first] = (*p).second;
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Updating data parm %d with %s (Device %d).", (*p).first, (*p).second.c_str(), m_dwPK_Device );
#endif
				SendString( "OK" );
				OnDataChange( (*p).first, ValueOld, (*p).second );
			}
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Got a data parm change without the value to change." );
				SendString( "ERR Input" );
			}
			return rmr_Processed;
		}

	return rmr_NotProcessed;
}

void Command_Impl::QueueMessageToRouter( Message *pMessage )
{
	if( m_bRouterReloading )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Command_Impl::QueueMessageToRouter m_bRouterReloading ignoring message type %d id %d to %d",
			pMessage->m_dwMessage_Type, pMessage->m_dwID, pMessage->m_dwPK_Device_To);
		return;
	}
	if( m_bLocalMode )
	{
#ifndef WINCE
		cout << pMessage->ToString() << endl;
#endif
		return;
	}

	PLUTO_SAFETY_LOCK( mq, m_listMessageQueueMutex );
	m_listMessageQueue.push_back( pMessage );
	pthread_cond_broadcast( &m_listMessageQueueCond );
}

void Command_Impl::ProcessMessageQueue()
{
	PLUTO_SAFETY_LOCK( mq, m_listMessageQueueMutex );
	while( !m_bQuit_get() )
	{
		while( m_listMessageQueue.size() == 0 )
		{
			mq.CondWait();
			if( m_bQuit_get() )
				return;
		}

		list<Message*> copyMessageQueue;

		// Make a copy so that we're not holding the lock while we send
		list<Message *>::iterator itMessageQueue;
		for( itMessageQueue=m_listMessageQueue.begin(); itMessageQueue != m_listMessageQueue.end(); ++itMessageQueue )
		{
			//Message *pMessage = *itMessageQueue;
			copyMessageQueue.push_back( *itMessageQueue );
		}


		m_listMessageQueue.clear();
		mq.Release();

		for( itMessageQueue = copyMessageQueue.begin(); itMessageQueue != copyMessageQueue.end(); ++itMessageQueue )
		{
			//Message *pMessage = *itMessageQueue;
			if( !m_pEvent->SendMessage( *itMessageQueue ) )
			{
				// If the connection between this device and dcerouter is still ok, we will have always gotten a true,
				// whether the destination device responded or not.  If we didn't, our socket must be bad.  Exit and
				// let the framework restart us
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"InternalSendCommand ProcessMessageQueue cannot send.  Going to quit");
				OnReload();
			}
		}
		mq.Relock();
	}
}

bool Command_Impl::InternalSendCommand( PreformedCommand &pPreformedCommand, int iConfirmation, string *p_sResponse )
{
	if( m_bRouterReloading )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Command_Impl::InternalSendCommand m_bRouterReloading ignoring message type %d id %d to %d",
			pPreformedCommand.m_pMessage->m_dwMessage_Type, pPreformedCommand.m_pMessage->m_dwID, pPreformedCommand.m_pMessage->m_dwPK_Device_To);
		return true;  // Router is reloading anyway
	}

	// Just put it in the queue.  The queue will delete pPreformedCommand.m_pMessage after sending
	if( iConfirmation == 0 || ( iConfirmation == -1 && !pPreformedCommand.m_pcResponse ) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"InternalSendCommand queue  id %d conf %d resp %p",pPreformedCommand.m_pMessage->m_dwID,iConfirmation,pPreformedCommand.m_pcResponse);
#endif
		if( m_bLocalMode )
		{
#ifndef WINCE
			cout << pPreformedCommand.m_pMessage->ToString() << endl;
#endif
			return true; // If it's just a test, local mode without a router
		}

		pPreformedCommand.m_pMessage->m_eExpectedResponse = ER_None;
		QueueMessageToRouter( pPreformedCommand.m_pMessage );
		return true;
	}

	// We need a response.  It will be a string if there are no out parameters
	if( !pPreformedCommand.m_pcResponse )
	{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"InternalSendCommand confirmation id %d conf %d resp %p",pPreformedCommand.m_pMessage->m_dwID,iConfirmation,pPreformedCommand.m_pcResponse);
#endif
		pPreformedCommand.m_pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;  // i.e. just an "OK"
		string sResponse; // We'll use this only if a response wasn't passed in
		if( !p_sResponse )
			p_sResponse = &sResponse;

		bool bResult;

		int Type = pPreformedCommand.m_pMessage->m_dwMessage_Type;
		int ID = pPreformedCommand.m_pMessage->m_dwID;
		int PK_Device_To = pPreformedCommand.m_pMessage->m_dwPK_Device_To;

		if( m_bLocalMode )
		{
#ifndef WINCE
			cout << pPreformedCommand.m_pMessage->ToString() << endl;
			cout << "ENTER REQUIRED STRING RESPONSE: ";
			cin >> sResponse;
			cout << endl;
#endif
			bResult=true;
		}
		else
			bResult = m_pcRequestSocket->SendMessage( pPreformedCommand.m_pMessage, *p_sResponse );

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"InternalSendCommand confirmation done id %d conf %d resp %p (%d) %s type %d id %d to %d",
					  ID, iConfirmation,pPreformedCommand.m_pcResponse,(int) bResult,p_sResponse->c_str(),Type,ID,PK_Device_To);
#endif
		if( !bResult )
		{
			// If the connection between this device and dcerouter is still ok, we will have always gotten a response,
			// whether the destination device responded or not.  If we didn't, our socket must be bad.  Exit and
			// let the framework restart us
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"InternalSendCommand cannot send message type %d id %d to %d with confirmation.  Going to quit", Type,ID,PK_Device_To);
			OnReload();
		}
		return bResult && *p_sResponse == "OK";
	}
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"InternalSendCommand id %d out parm conf %d resp %p",
					  pPreformedCommand.m_pMessage->m_dwID,iConfirmation,pPreformedCommand.m_pcResponse);
#endif
	// There are out parameters, we need to get a message back in return
	pPreformedCommand.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse;

	int Type = pPreformedCommand.m_pMessage->m_dwMessage_Type;
	int ID = pPreformedCommand.m_pMessage->m_dwID;
	int PK_Device_To = pPreformedCommand.m_pMessage->m_dwPK_Device_To;

	if( m_bLocalMode )
	{
#ifndef WINCE
		cout << pPreformedCommand.m_pMessage->ToString() << endl;
		cout << "ENTER REQUIRED RESPONSE MESSAGE: ";
#endif
		pResponse = GetLocalModeResponse();
	}
	else
		pResponse = m_pcRequestSocket->SendReceiveMessage( pPreformedCommand.m_pMessage );

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"InternalSendCommand out done id %d conf %d resp %p %p %d type %d id %d to %d",
	ID,iConfirmation,pPreformedCommand.m_pcResponse,pResponse,(pResponse ? pResponse->m_dwID : 0),Type,ID,PK_Device_To);
#endif
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;
		else
		{
			// If the connection between this device and dcerouter is still ok, we will have always gotten a response,
			// whether the destination device responded or not.  If we didn't, our socket must be bad.  Exit and
			// let the framework restart us
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"InternalSendCommand cannot send with return message.  type %d id %d to %d Going to quit", Type,ID,PK_Device_To);
			OnReload();
		}

		return false;
	}

	pPreformedCommand.ParseResponse( pResponse );
	string sResponse = pResponse->m_mapParameters[0];
	if( p_sResponse )
		*p_sResponse = sResponse;

	bool bResult = sResponse=="OK";
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"InternalSendCommand out id %d parm exiting conf %d resp %p",
									ID,iConfirmation,pPreformedCommand.m_pcResponse);
#endif
	delete pResponse;
	return bResult;
}

void Command_Impl::StartWatchDog( clock_t Timeout )
{
	m_bStopWatchdog = false;
	m_clockTimeout = clock() + Timeout;
	if ( m_bWatchdogRunning ) return;
	m_bWatchdogRunning = true;
    pthread_create( &m_pThread, NULL, WatchDogThread, (void *)this );
    Sleep(1);
}

void Command_Impl::StopWatchDog()
{
#ifdef PTHREAD2
    if ( m_pThread.p != 0 )
    {
        m_bStopWatchdog = true;
        pthread_join( m_pThread, NULL );
        m_pThread.p = 0;
    }
#else
    if ( m_pThread != 0 )
    {
        m_bStopWatchdog = true;
        pthread_join( m_pThread, NULL );
        m_pThread = 0;
    }
#endif

}

int Command_Impl::RegisterMsgInterceptor(MessageInterceptorFn pMessageInterceptorFn,int PK_Device_From,int PK_Device_To,int PK_DeviceTemplate,int PK_DeviceCategory,int MessageType,int MessageID)
{
#ifdef LINK_TO_ROUTER
	if( m_pRouter )
	{
		m_pRouter->RegisterMsgInterceptor(
			new MessageInterceptorCallBack(this, pMessageInterceptorFn), PK_Device_From, PK_Device_To, PK_DeviceTemplate, PK_DeviceCategory, MessageType, MessageID );
		return 0;
	}
	else
#endif
	{
		int dwMessageInterceptorCounter=m_dwMessageInterceptorCounter;
		m_mapMessageInterceptorFn[m_dwMessageInterceptorCounter] = pMessageInterceptorFn;
		Message *pMessage = new Message(m_dwPK_Device,0,PRIORITY_NORMAL,MESSAGETYPE_REGISTER_INTERCEPTOR,m_dwMessageInterceptorCounter,6,
			PARM_FROM, StringUtils::itos(PK_Device_From).c_str(), PARM_TO, StringUtils::itos(PK_Device_To).c_str(),PARM_TEMPLATE, StringUtils::itos(PK_DeviceTemplate).c_str(),
			PARM_CATEGORY, StringUtils::itos(PK_DeviceCategory).c_str(), PARM_MESSAGE_TYPE, StringUtils::itos(MessageType).c_str(), PARM_MESSAGE_ID, StringUtils::itos(MessageID).c_str());
		SendMessageToRouter(pMessage);
		m_dwMessageInterceptorCounter++;
		return dwMessageInterceptorCounter;
	}
}

void Command_Impl::PurgeInterceptors()
{
#ifndef WINCE
	Message *pMessage = new Message(m_dwPK_Device,0,PRIORITY_NORMAL,MESSAGETYPE_PURGE_INTERCEPTORS,0,0);
	SendMessageToRouter(pMessage);
#endif
}

void Command_Impl::InterceptedMessage(Message *pMessage)
{
#ifndef WINCE
	MessageInterceptorFn pMessageInterceptorFn = m_mapMessageInterceptorFn_Find(pMessage->m_dwID);
	if( !pMessageInterceptorFn || pMessage->m_vectExtraMessages.size()!=1)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Got intercepted message, but there is no corresponding callback");
		return;
	}


	Message *pMessageOriginal = pMessage->m_vectExtraMessages[0];
	if( CALL_MEMBER_FN(*this,pMessageInterceptorFn) (NULL, pMessageOriginal,
		m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessageOriginal->m_dwPK_Device_From),
		m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessageOriginal->m_dwPK_Device_To)) )
	{
		if( pMessage->m_eExpectedResponse==ER_ReplyMessage ) // Should always be true
		{
			pMessage->m_bRespondedToMessage=true;
			Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
			pMessageOut->m_mapParameters[0]="OK";
			pMessageOut->m_mapParameters[-1]="ABORT";
			SendMessage(pMessageOut);
		}
	}
#endif
}

void Command_Impl::ExecCommandGroup(int PK_CommandGroup)
{
	if( !PK_CommandGroup )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Ignoring ExecCommandGroup 0");
		return;
	}
	Message *pMessage = new Message(m_dwPK_Device,0,PRIORITY_NORMAL,MESSAGETYPE_EXEC_COMMAND_GROUP,PK_CommandGroup,0);
	QueueMessageToRouter(pMessage);
}

string Command_Impl::GetState( int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return "";

	string sResult;
	m_pcRequestSocket->m_pClientSocket->SendString("GET_STATE " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) );
	m_pcRequestSocket->m_pClientSocket->ReceiveString(sResult);
	return sResult;
}

string Command_Impl::GetStatus( int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return "";

	string sResult;
	m_pcRequestSocket->m_pClientSocket->SendString("GET_STATUS " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) );
	m_pcRequestSocket->m_pClientSocket->ReceiveString(sResult);
	return sResult;
}

bool Command_Impl::SetState( string sState, int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return false;

	return m_pcRequestSocket->m_pClientSocket->SendString("SET_STATE " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) + " " + sState );
}

bool Command_Impl::SetStatus( string sStatus, int dwPK_Device )
{
	if( !m_pcRequestSocket )
		return false;

	return m_pcRequestSocket->m_pClientSocket->SendString("SET_STATUS " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) + " " + sStatus );
}

string Command_Impl::GetIpAddress( int dwPK_Device )
{
	// If this is for us and the IP address is already known, just pass it
	if( (dwPK_Device==0 || dwPK_Device==m_pData->m_dwPK_Device) && m_pData->m_sIPAddress.empty()==false )
		return m_pData->m_sIPAddress;

	// Get it from the server
	if( !m_pcRequestSocket )
		return "";

	string sResult;
	m_pcRequestSocket->m_pClientSocket->SendString("GET_IP " + StringUtils::itos( dwPK_Device ? dwPK_Device : m_dwPK_Device ) );
	m_pcRequestSocket->m_pClientSocket->ReceiveString(sResult);
	return sResult;
}

bool Command_Impl::ReportPendingTasksFromDevice(Socket *pSocket,int PK_Device_Requestor,int PK_Device,PendingTaskList *pPendingTaskList)
{
	Message *pMessage = new Message(PK_Device_Requestor,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_PENDING_TASKS,0,0);
	Message *pResponse = pSocket->SendReceiveMessage(pMessage);

	// We get back a return message with a paramter 0 that will be the pending tasks

	bool bFoundTasks=false;
	if( pResponse )
	{
		string sResponse = pResponse->m_mapParameters[0];
		string::size_type pos = 0;
		while( pos<sResponse.size() )
		{
			string s = StringUtils::Tokenize(sResponse,"\n",pos);
			if( s.empty() )
				continue;
			bFoundTasks=true;
			if( pPendingTaskList )
			{
				PendingTask *pPendingTask = new PendingTask(s);
				pPendingTaskList->m_listPendingTask.push_back(pPendingTask);
			}
		}
	}

	return bFoundTasks;
}

void Command_Impl::EnableDevice( int PK_Device, bool bEnabled )
{
	DCE::CMD_Set_Enable_Status_DT CMD_Set_Enable_Status_DT(m_dwPK_Device,DEVICETEMPLATE_General_Info_Plugin_CONST,BL_SameHouse,PK_Device,bEnabled);
	SendCommand(CMD_Set_Enable_Status_DT);
}

void Command_Impl::SetDeviceDataInDB(int PK_Device,int PK_DeviceData,string Value)
{
	Message *pMessage = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_DATAPARM_CHANGE,PK_Device,1,PK_DeviceData,Value.c_str());
	m_pcRequestSocket->SendMessage(pMessage);
}

void Command_Impl::DeleteGlobalAllocs()
{
	MutexTracking::Delete();
}

void Command_Impl::WaitForMessageQueue()
{
	time_t tTimeout = time(NULL) + 15;  // Timeout in 15 seconds
	while(true)
	{
		PLUTO_SAFETY_LOCK_ERRORSONLY( mq, m_listMessageQueueMutex );
		if( m_listMessageQueue.size()==0 )
			return;
		mq.Release();
		Sleep(50);
		if( time(NULL) > tTimeout )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Command_Impl::WaitForMessageQueue timed out");
			return;
		}
	}
}

int Command_Impl::FindUnregisteredRelatives(map<int,bool> *p_mapUnregisteredRelatives)
{
	DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device);
	if( !pDevice )
		return 0;  // Should never happen

	FindUnregisteredRelativesLoop(pDevice,p_mapUnregisteredRelatives);

	int UnRegistered=0;
	for(map<int,bool>::iterator it=p_mapUnregisteredRelatives->begin();it!=p_mapUnregisteredRelatives->end();++it)
		if( it->second==false )
			UnRegistered++;
	return UnRegistered;
}

void Command_Impl::FindUnregisteredRelativesLoop(DeviceData_Base *pDevice,map<int,bool> *p_mapUnregisteredRelatives,bool bScanParent,int PK_Device_ExcludeChild)
{
	if( pDevice->WithinCategory(DEVICECATEGORY_DCE_Router_CONST) )
		return;  // Don't worry about the router

	if( pDevice->m_dwPK_Device!=m_dwPK_Device && pDevice->m_bImplementsDCE && !pDevice->m_bIsEmbedded && !pDevice->m_bDisabled ) // Exclude ourself, only non-embedded DCE Devices
	{
		char cDeviceRegistered = DeviceIsRegistered(pDevice->m_dwPK_Device);
		if (cDeviceRegistered != 'D')
			(*p_mapUnregisteredRelatives)[pDevice->m_dwPK_Device] = (cDeviceRegistered=='Y');
	}
	for(size_t s=0;s<pDevice->m_vectDeviceData_Base_Children.size();++s)
	{
		DeviceData_Base *pDeviceData_Base = pDevice->m_vectDeviceData_Base_Children[s];
		if( pDeviceData_Base->m_dwPK_Device==PK_Device_ExcludeChild )
			continue;
		FindUnregisteredRelativesLoop(pDeviceData_Base,p_mapUnregisteredRelatives,false);
	}

	if( bScanParent && pDevice->m_pDevice_ControlledVia )
		FindUnregisteredRelativesLoop(pDevice->m_pDevice_ControlledVia,p_mapUnregisteredRelatives,true,pDevice->m_dwPK_Device);
}

char Command_Impl::DeviceIsRegistered(int PK_Device) // Returns Y, N, D (for disabled)
{
	string sResponse = m_pcRequestSocket->m_pClientSocket->SendReceiveString("DEVICE_REGISTERED " + StringUtils::itos(PK_Device));
	if( sResponse.substr(0,17)!="DEVICE_REGISTERED" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine if device %d registered",PK_Device);
		return 'E';
	}
	return sResponse[18];
}

bool Command_Impl::GetChildDeviceData( int PK_Device, int PK_DeviceData, string &sValue, DeviceData_Impl *pDeviceData_Impl)
{
	if( !pDeviceData_Impl )
		pDeviceData_Impl = m_pData; // Only happens on the top level

	if( !pDeviceData_Impl )
		return false;

	if( pDeviceData_Impl->m_dwPK_Device == PK_Device )
	{
		sValue=pDeviceData_Impl->m_mapParameters_Find(PK_DeviceData);
		return true;
	}

	for(size_t s=0;s<pDeviceData_Impl->m_vectDeviceData_Impl_Children.size();++s)
	{
		DeviceData_Impl *pDeviceData_Impl_Child = pDeviceData_Impl->m_vectDeviceData_Impl_Children[s];
		if( GetChildDeviceData(PK_Device,PK_DeviceData,sValue,pDeviceData_Impl_Child) )
			return true;
	}
	return false;
}

void Command_Impl::RunLocalMode()
{
#if !defined(WINCE) && !defined(__APPLE_CC__)
	while(true)
	{
		cout << "Enter incoming message or QUIT: ";
		string sMessage;
		while(true)
		{
			char ch = getch();
			if (ch == EOF)
			{
				cout << "EOF received" << endl;
				while (true) Sleep(1000);
			}
			if( ch=='\n' || ch=='\r' )
				break;
			cout << ch;
			sMessage += ch;
		}
		if( StringUtils::ToUpper(sMessage)=="QUIT" )
			return;
		cout << endl;
		Message *pMessage = new Message(sMessage);
		ReceivedMessage(pMessage);
	}
#endif
}

Message *Command_Impl::GetLocalModeResponse()
{
	return NULL;
}

void Command_Impl::OnQuit()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Command_Impl::OnQuit forwarding quit's to children");
	PLUTO_SAFETY_LOCK( sSM, m_listMessageQueueMutex );  // Use this to protect m_mapSpawnedDevices
	for(map<int,string>::iterator it=m_mapSpawnedDevices.begin();it!=m_mapSpawnedDevices.end();++it)
	{
		Message *pMessage = new Message(m_dwPK_Device,it->first,PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_QUIT,0);
		m_pcRequestSocket->SendMessage(pMessage);
	}
	sSM.Release();

	m_bQuit_set(true);
	m_bTerminate=true;
	pthread_cond_broadcast( &m_listMessageQueueCond );

#ifdef LINK_TO_ROUTER
	if(NULL != m_pRouter && m_pRouter->IsPlugin(m_pcRequestSocket->m_dwPK_Device))
		m_pRouter->Quit();
#endif
}

void Command_Impl::OnReload()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Command_Impl::OnReload %d", m_dwPK_Device);
	m_bReload = true;
	m_bQuit_set(true);
	m_bTerminate=true;
	pthread_cond_broadcast( &m_listMessageQueueCond );

#ifdef LINK_TO_ROUTER
	if(NULL != m_pRouter && m_pRouter->IsPlugin(m_pcRequestSocket->m_dwPK_Device))
		m_pRouter->Reload();
#endif
}
