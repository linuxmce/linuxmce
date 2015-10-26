/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "ZWInterface.h"
#include <openzwave/platform/Log.h>

using namespace DCE;

ZWInterface::ZWInterface()
{
	m_pZWave = NULL;
	m_pConfigData = NULL;
	g_homeId = 0;
	m_iPolledValues = 0;
	g_initFailed = false;
	m_bInitDone = false;
	m_sLogFile = "";

	initCond  = PTHREAD_COND_INITIALIZER;
	initMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutexattr_t mutexattr;
	
	pthread_mutexattr_init ( &mutexattr );
	pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_RECURSIVE );
	
	pthread_mutex_init( &g_criticalSection, &mutexattr );
	pthread_mutexattr_destroy( &mutexattr );
	
}

ZWInterface::~ZWInterface()
{
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::~() destructor, calling OpenZWave::Manager::Destroy()");
	if (m_pConfigData != NULL)
	        OpenZWave::Manager::Get()->RemoveDriver(m_pConfigData->m_sPort );
	OpenZWave::Manager::Get()->RemoveWatcher( OnNotification_static, this );
	OpenZWave::Manager::Destroy();
	OpenZWave::Options::Destroy();
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::~() destructor, OZW destroyed");
	Lock();
	for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
	{
		NodeInfo* nodeInfo = *it;
		delete nodeInfo;
		g_nodes.erase( it );
	}
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::~() destructor, internal node data destroyed");
	UnLock();
	if (m_pConfigData != NULL) {
	        delete m_pConfigData;
		m_pConfigData = NULL;
	}
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::~() destructor, done");
}

NodeInfo* ZWInterface::GetNodeInfo ( OpenZWave::Notification const* _notification) {
	uint32 const homeId = _notification->GetHomeId();
	uint8 const nodeId = _notification->GetNodeId();
	return GetNodeInfo(homeId, nodeId);
}

NodeInfo* ZWInterface::GetNodeInfo( uint32 homeId, uint8 nodeId )
{
	for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
	{
		NodeInfo* nodeInfo = *it;
		if( ( nodeInfo->m_homeId == homeId ) && ( nodeInfo->m_nodeId == nodeId ) )
		{
			return nodeInfo;
		}
	}
	
	return NULL;
}

uint32 ZWInterface::GetHomeId() {
	return g_homeId;
}

void ZWInterface::Lock() {
	pthread_mutex_lock( &g_criticalSection );
}

void ZWInterface::UnLock() {
	pthread_mutex_unlock( &g_criticalSection );
}

bool ZWInterface::Init(ZWConfigData* data) {
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() start");
	
	pthread_mutex_lock( &initMutex );

	if (m_pConfigData != NULL)
	        delete m_pConfigData;
	m_pConfigData = data;

	if ( m_sLogFile != "")
	{
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() setting OpenZWave log file to %s", m_sLogFile.c_str());
		OpenZWave::Log* pLog = OpenZWave::Log::Create(m_sLogFile, true, false, OpenZWave::LogLevel_Info, OpenZWave::LogLevel_Debug, OpenZWave::LogLevel_Error);
		pLog->SetLogFileName(m_sLogFile); // Make sure, in case Log::Create already was called before we got here
		pLog->SetLoggingState(OpenZWave::LogLevel_Info, OpenZWave::LogLevel_Debug, OpenZWave::LogLevel_Error);
	}

	// Create the OpenZWave Manager.
	// The first argument is the path to the config files (where the manufacturer_specific.xml file is located
	// The second argument is the path for saved Z-Wave network state and the log file.  If you leave it NULL 
	// the log file will appear in the program's working directory.
	OpenZWave::Options::Create( "/etc/openzwave/", "/etc/openzwave/", "" );
	if (data->m_itimeout >= 10) {
	        LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() RetryTimeout = %d", data->m_itimeout);
	        OpenZWave::Options::Get()->AddOptionInt("RetryTimeout", data->m_itimeout);
	}
	OpenZWave::Options::Get()->Lock();

	OpenZWave::Manager::Create();
	
	OpenZWave::Manager::Get()->AddWatcher( OnNotification_static, this );
	
	OpenZWave::Manager::Get()->AddDriver(data->m_sPort );
	
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() waiting for OZW init");
	// Now we just wait for the driver to become ready
	// TODO: if ZWave get quit request while we wait, we will not shut down.
	pthread_cond_wait( &initCond, &initMutex );
	
	m_bInitDone = true;
	if( !g_initFailed )
	{
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() init successfull");
		OpenZWave::Manager::Get()->WriteConfig( g_homeId );
		
		OpenZWave::Driver::DriverData data;
		OpenZWave::Manager::Get()->GetDriverStatistics( g_homeId, &data );
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "SOF: %d ACK Waiting: %d Read Aborts: %d Bad Checksums: %d\n", data.m_SOFCnt, data.m_ACKWaiting, data.m_readAborts, data.m_badChecksum);
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Reads: %d Writes: %d CAN: %d NAK: %d ACK: %d Out of Frame: %d\n", data.m_readCnt, data.m_writeCnt, data.m_CANCnt, data.m_NAKCnt, data.m_ACKCnt, data.m_OOFCnt);
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Dropped: %d Retries: %d\n", data.m_dropped, data.m_retries);

		m_ourNodeId = OpenZWave::Manager::Get()->GetControllerNodeId(g_homeId);

		return true;
	}
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() init failed!");
	return false;
}

void ZWInterface::SetZWave(ZWave* pZWave) {
	m_pZWave = pZWave;
}

bool ZWInterface::RequireRestart(ZWConfigData* data) {
        if (m_pConfigData != NULL && data != NULL) {
	  LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::RequireRestart() %d != %d", m_pConfigData->m_itimeout, data->m_itimeout);
	        if (m_pConfigData->m_itimeout != data->m_itimeout)
		        return true;
	}
	return false;
}

bool ZWInterface::IsReady() {
	// perhaps we should also check the status of the init, has ozw
	// sent driver ready? not a problem as long as ozw init not in its own thread...
	bool ready = m_bInitDone && !g_initFailed;
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::IsReady() returns %s", ready ? "true" : "false");
	return ready;
}

void ZWInterface::UpdatePolling() {
	// Set the poll interval
	int32 interval = m_iPolledValues * 5000; // make sure we don't poll a value more often than every 5 second.
	if (interval < 60000) {
		interval = 60000; // don't do a polling cycle more often than once a minute
	}
	OpenZWave::Manager::Get()->SetPollInterval(interval, false);
}

void ZWInterface::OnNotification_static(OpenZWave::Notification const* _notification, void* _context) {
	ZWInterface *zwI = (ZWInterface*)_context;
	zwI->OnNotification(_notification);
}

void ZWInterface::OnNotification(OpenZWave::Notification const* _notification) {
	// Must do this inside a critical section to avoid conflicts with the main thread
	Lock();
	switch( _notification->GetType() )
	{
		
	case OpenZWave::Notification::Type_ValueRefreshed:
	case OpenZWave::Notification::Type_Group:
		break;
	case OpenZWave::Notification::Type_NodeQueriesComplete:
		// If we get here after init, write the configuration
		if ( m_bInitDone && !g_initFailed )
			OpenZWave::Manager::Get()->WriteConfig( _notification->GetHomeId() );
		break;
	case OpenZWave::Notification::Type_DriverReady:
	{
		g_homeId = _notification->GetHomeId();
		break;
	}
	
	
	case OpenZWave::Notification::Type_DriverFailed:
	{
		g_initFailed = true;
		pthread_cond_broadcast(&initCond);
		break;
	}
	
	case OpenZWave::Notification::Type_AwakeNodesQueried:
	case OpenZWave::Notification::Type_AllNodesQueried:
	{
		pthread_cond_broadcast(&initCond);
		break;
	}
	
	case OpenZWave::Notification::Type_NodeAdded:
	{
		// Add the new node to our list
		NodeInfo* nodeInfo = new NodeInfo();
		nodeInfo->m_homeId = _notification->GetHomeId();
		nodeInfo->m_nodeId = _notification->GetNodeId();
		nodeInfo->m_polled = false;		
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Node Added nodeId = %d", nodeInfo->m_nodeId);
		g_nodes.push_back( nodeInfo );
		break;
	}
	case OpenZWave::Notification::Type_EssentialNodeQueriesComplete:
	{
		if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_EssentialNodeQueriesComplete nodeId = %d", nodeInfo->m_nodeId);
			nodeInfo->m_generic = OpenZWave::Manager::Get()->GetNodeGeneric(nodeInfo->m_homeId, nodeInfo->m_nodeId);
			nodeInfo->m_specific = OpenZWave::Manager::Get()->GetNodeSpecific(nodeInfo->m_homeId, nodeInfo->m_nodeId);
			// If we get here after init, write the configuration
			if ( m_bInitDone && !g_initFailed )
				OpenZWave::Manager::Get()->WriteConfig( nodeInfo->m_homeId );
		}
		break;
	}
	case OpenZWave::Notification::Type_NodeRemoved:
	{
		// Remove the node from our list
		uint32 const homeId = _notification->GetHomeId();
		uint8 const nodeId = _notification->GetNodeId();
		for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
		{
			NodeInfo* nodeInfo = *it;
			if( ( nodeInfo->m_homeId == homeId ) && ( nodeInfo->m_nodeId == nodeId ) )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Node Removed nodeId = %d", nodeId);
				g_nodes.erase( it );
				delete nodeInfo;
				break;
			}
		}
		break;
	}
	
	case OpenZWave::Notification::Type_ValueAdded:
	{
		if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
		{
			// Add the new value to our list
			nodeInfo->m_values.push_back( _notification->GetValueID() );
			OpenZWave::ValueID id = _notification->GetValueID();
			string label = OpenZWave::Manager::Get()->GetValueLabel(id);
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Value Added Home 0x%08x Node %d Genre %d Class %d Instance %d Index %d Type %d, label = %s\n", _notification->GetHomeId(), _notification->GetNodeId(), id.GetGenre(), id.GetCommandClassId(), id.GetInstance(), id.GetIndex(), id.GetType(), label.c_str());

			// Write config if we get new values after first init is done
			if ( m_bInitDone && !g_initFailed )
				OpenZWave::Manager::Get()->WriteConfig( g_homeId );

		}
		break;
	}
	
	case OpenZWave::Notification::Type_ValueRemoved:
	{
		if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
		{
			// Remove the value from out list
			for( list<OpenZWave::ValueID>::iterator it = nodeInfo->m_values.begin(); it != nodeInfo->m_values.end(); ++it )
			{
				OpenZWave::ValueID id = _notification->GetValueID();
				if( (*it) == id )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Value Removed Home 0x%08x Node %d Genre %d Class %d Instance %d Index %d Type %d\n", _notification->GetHomeId(), _notification->GetNodeId(), id.GetGenre(), id.GetCommandClassId(), id.GetInstance(), id.GetIndex(), id.GetType());
					nodeInfo->m_values.erase( it );
					break;
				}
			}
		}
		break;
	}
	case OpenZWave::Notification::Type_ValueChanged:
	{
		if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
		{
			// One of the node values has changed
			// TBD...
			// nodeInfo = nodeInfo;
			OpenZWave::ValueID id = _notification->GetValueID();
			string str;
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : Value Changed Home 0x%08x Node %d Genre %d Class %d Instance %d Index %d Type %d", _notification->GetHomeId(), _notification->GetNodeId(), id.GetGenre(), id.GetCommandClassId(), id.GetInstance(), id.GetIndex(), id.GetType());
			if (OpenZWave::Manager::Get()->GetValueAsString(id, &str)) {
				string label = OpenZWave::Manager::Get()->GetValueLabel(id);
				string units = OpenZWave::Manager::Get()->GetValueUnits(id);
				string level = str;
				if (str == "True") level="255";
				if (str == "False") level="0";
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : Value: %s Label: %s Unit: %s",str.c_str(),label.c_str(),units.c_str());
				
			}
		}
		break;
	}
	
	case OpenZWave::Notification::Type_PollingDisabled:
	{
		if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
		{
			if (m_iPolledValues > 0) {
				m_iPolledValues--;
			}
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Now polling %d values", m_iPolledValues);
			UpdatePolling();
			nodeInfo->m_polled = false;
		}
		break;
	}
	
	case OpenZWave::Notification::Type_PollingEnabled:
	{
		if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
		{
			m_iPolledValues++;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Now polling %d values", m_iPolledValues);
			UpdatePolling();
			nodeInfo->m_polled = true;
		}
		break;
	}
	case OpenZWave::Notification::Type_Notification:
	{
		switch (_notification->GetNotification()) {
		case OpenZWave::Notification::Code_MsgComplete:
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : Message complete, node %d", _notification->GetNodeId());
			break;
		case OpenZWave::Notification::Code_Timeout:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Message timed out, node %d", _notification->GetNodeId());
			break;
		case OpenZWave::Notification::Code_NoOperation:
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : NoOperation message completed, node %d", _notification->GetNodeId());
			break;
		case OpenZWave::Notification::Code_Awake:
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : Node %d has woken up", _notification->GetNodeId());
			break;
		case OpenZWave::Notification::Code_Sleep:
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : Node %d went to sleep", _notification->GetNodeId());
			break;
		case OpenZWave::Notification::Code_Dead:
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ZWInterface::OnNotification() : Node %d presumed dead!", _notification->GetNodeId());
			break;
		case OpenZWave::Notification::Code_Alive:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Node %d revived", _notification->GetNodeId());
			break;
		default:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_Notification, unhandled/new code = %d", _notification->GetNotification());
			break;
		}
		break;
	}	
	case OpenZWave::Notification::Type_ControllerCommand:
	{
		switch (_notification->GetEvent()) {
		case OpenZWave::Driver::ControllerState_Normal:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Normal");
			break;
		case OpenZWave::Driver::ControllerState_Starting:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Starting");
			break;
		case OpenZWave::Driver::ControllerState_Cancel:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Cancel");
			break;
		case OpenZWave::Driver::ControllerState_Error :
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Error ");
			break;
		case OpenZWave::Driver::ControllerState_Waiting:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Waiting for user action");
			break;
		case OpenZWave::Driver::ControllerState_Sleeping:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Command is on sleep queue waiting for device");
			break;
		case OpenZWave::Driver::ControllerState_InProgress:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Controller is communicating with device");
			break;
		case OpenZWave::Driver::ControllerState_Completed:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Command has completed successfully");
			break;
		case OpenZWave::Driver::ControllerState_Failed:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Command has failed");
			break;
		case OpenZWave::Driver::ControllerState_NodeOK:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Controller thinks node is OK");
			break;
		case OpenZWave::Driver::ControllerState_NodeFailed:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State Controller thinks node has failed");
			break;
		default:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : Type_ControllerCommand: State (new state unhandled by code):  %d", _notification->GetNotification());
			break;
		}
		break;
	}
	default:
	{
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::OnNotification() : unhandled/ignored notification = %d", _notification->GetType());
		break;
	}
	} // switch
	if (m_pZWave != NULL && m_pZWave->IsReady())
		m_pZWave->OnNotification( _notification, GetNodeInfo( _notification ));
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWInterface::OnNotification() : m_pZWave == NULL or m_pZWave->IsReady == false, possible lost notification");
	
	UnLock();
}

OpenZWave::ValueID* ZWInterface::GetValueIdByNodeInstanceLabel(int iNodeId, unsigned int iInstance, string label)
{
	NodeInfo* pNodeInfo = GetNodeInfo(g_homeId, iNodeId);
	if ( pNodeInfo != NULL )
	{
		return GetValueIdByLabel(pNodeInfo, iInstance, label);
	}
	return NULL;
}

OpenZWave::ValueID* ZWInterface::GetValueIdByLabel(NodeInfo* pNodeInfo, unsigned int iInstance, string label)
{
	for( list<OpenZWave::ValueID>::iterator it = pNodeInfo->m_values.begin(); it != pNodeInfo->m_values.end(); ++it )
	{
		LMCEDevice* pDevice = NULL;
		if (iInstance < pNodeInfo->m_vectDevices.size())
			pDevice = pNodeInfo->m_vectDevices[iInstance];
		if (pDevice != NULL)
		{
			for( vector<OpenZWave::ValueID>::iterator it = pDevice->m_vectValues.begin(); it != pDevice->m_vectValues.end(); ++it )
			{
				string lab = OpenZWave::Manager::Get()->GetValueLabel((*it));
				if ( lab == label )
					return &(*it);
			}
		}
	}
	return NULL;
}

bool ZWInterface::SetIntValue(int iNodeId, int iInstance, string label, int iValue)
{
	NodeInfo* pNodeInfo = GetNodeInfo(g_homeId, iNodeId);
	if ( pNodeInfo != NULL )
	{
		OpenZWave::ValueID* valueId = GetValueIdByLabel(pNodeInfo, iInstance, label);
		if ( valueId != NULL ) {
			OpenZWave::Manager::Get()->SetValue(*valueId, iValue);
			return true;
		}
	}
	return false;
}

bool ZWInterface::SetWakeUp(int iNodeId, int iValue) {
	return SetIntValue(iNodeId, 1, "Wake-up Interval", iValue);
// TODO: When to use the "normal" set wakeup CC ?
}
