/*
 PnpQueue

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "PlutoUtils/LinuxSerialUSB.h"	 
#include "PnpQueue.h"
#include "PnpQueueEntry.h"
#include "Plug_And_Play_Plugin.h"
#include "BD/PhoneDevice.h"
#include "General_Info_Plugin/Web_DHCP_Query.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "Media_Plugin/Media_Plugin.h"
#include "Gen_Devices/AllScreens.h"
#include "DCERouter.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Manufacturer.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Define_PnpProtocol.h"

using namespace DCE;
using namespace nsWeb_DHCP_Query;

#ifndef WEB_QUERY_DEBUG
static const string sURL_Base = "http://plutohome.com/getRegisteredDevices.php";
#else
static const string sURL_Base = "http://10.0.0.175/plutohome-com/getRegisteredDevices.php";
#endif

#define WAIT_SECONDS_AFTER_ORBITER_REGISTERS	35

void *PnpThread(void *p)
{
	PnpQueue *pPnpQueue = (PnpQueue *) p;
	pPnpQueue->Run();
	return NULL;
}

PnpQueue::PnpQueue(class Plug_And_Play_Plugin *pPlug_And_Play_Plugin)
	: m_Pnp_PreCreateOptions(this,pPlug_And_Play_Plugin), m_PnpQueueThread_Id(pthread_t(NULL))
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pDatabase_pluto_main=m_pPlug_And_Play_Plugin->m_pDatabase_pluto_main;

	m_bThreadRunning=true;
	if(pthread_create( &m_PnpQueueThread_Id, NULL, PnpThread, (void*)this) )
	{
		m_bThreadRunning=false;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create PNP thread" );
	}

	m_mapCategoryLocateDevice["fileserver"] = &PnpQueue::LocateFileServer;
	m_mapCategoryLocateDevice["fileshare"] = &PnpQueue::LocateFileShare;
}

PnpQueue::~PnpQueue()
{
}

void PnpQueue::Run()
{
	PLUTO_SAFETY_LOCK(pnp,m_pPlug_And_Play_Plugin->m_PnpMutex);

	pnp.Release();
	// Read in any outstanding requests.  By now Orbiters should have started registering so we know which ones are in which rooms
	ReadOutstandingQueueEntries();
	while( m_pPlug_And_Play_Plugin->m_pRouter->m_bIsLoading_get() )
		Sleep(1000); // Wait for the router to be ready before we start to process

	// We used to put a wait in here for GSD devices and OSD to start up.  However now 
	// that we have a built-in mechanism for RS-232 devices to wait until 20 seconds after the 'md up' event
	// and the architecture will wait for Orbiter, this isn't needed anymore.
	/*
	time_t tTimeout = time(NULL) + 30; 
	while( time(NULL) < tTimeout && !m_pPlug_And_Play_Plugin->m_bQuit_get())
		Sleep(1000);
	*/

	pnp.Relock();

	bool bOnlyBlockedEntries=false;  // Set this to true if every entry in the list is blocked and we don't need to process anymore until something happens
	while( !m_pPlug_And_Play_Plugin->m_bQuit_get())
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Run size %d only blocked %d", m_mapPnpQueueEntry.size(), (int) bOnlyBlockedEntries);
#endif
		if( m_mapPnpQueueEntry.size()==0 || bOnlyBlockedEntries )  // bOnlyBlockedEntries will be true if the last loop had only blocked entries
			pnp.TimedCondWait(60,0);  // Release the mutex so other items can be added to the queue and so we can handle incoming events.  Only sleep for 1 minutes in case an Orbiter is blocked and not responding

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Run size %d woke up", m_mapPnpQueueEntry.size());
#endif
		bOnlyBlockedEntries = m_mapPnpQueueEntry.size()>0;

		map<int,class PnpQueueEntry *> mapPnpQueueEntry_temp; // A temporary copy of those queues we're going to process so can release the mutex protecting the real map
		for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)  // The pnp mutex is held so we can safely do what we like
		{
			PnpQueueEntry *pPnpQueueEntry = it->second;

			// If we're blocked waiting for a device like HAL to startup, check if it's since finished registering
			if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_waiting_for_initial_detection )
			{
				if( m_mapSignaturesReady.find(pPnpQueueEntry->m_pRow_PnpQueue->Signature_get())!=m_mapSignaturesReady.end() )
					pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
			}

			// If we've been sitting too long at either a user prompt or a detection script we need to continue anyway
			if( pPnpQueueEntry->m_EBlockedState != PnpQueueEntry::pnpqe_blocked_none &&
				( (pPnpQueueEntry->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_prompting_options && pPnpQueueEntry->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_prompting_device_template) || time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER) &&
				( pPnpQueueEntry->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_running_detection_scripts || time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_DETECTION_SCRIPT ) &&
				( pPnpQueueEntry->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_waiting_for_orbiters || time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_WAITING_ORBITERS ) &&
				( pPnpQueueEntry->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_waiting_for_other_device || time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_WAITING_FOR_DEVICE ) 
				)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Run queue %d blocked %d time %d now %d by %d", pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),
					(int) pPnpQueueEntry->m_EBlockedState,(int) pPnpQueueEntry->m_tTimeBlocked, (int) time(NULL),pPnpQueueEntry->m_dwPK_PnpQueue_BlockingFor);
				continue;
			}
			else
				bOnlyBlockedEntries=false;  // There are some entries that are not blocked, so loop again

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Run processing %d at stage %d", pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pPnpQueueEntry->m_pRow_PnpQueue->Stage_get());

			mapPnpQueueEntry_temp[it->first] = it->second;
		}

		for(map<int,class PnpQueueEntry *>::iterator it=mapPnpQueueEntry_temp.begin();it!=mapPnpQueueEntry_temp.end();++it)  
		{
			PnpQueueEntry *pPnpQueueEntry = it->second;
			if( Process(pPnpQueueEntry)==true )  // Meaning it needs to be removed from the list
			{
				ReleaseQueuesBlockedFromPromptingState(pPnpQueueEntry);
				delete pPnpQueueEntry;
				map<int,class PnpQueueEntry *>::iterator it_del = m_mapPnpQueueEntry.find( it->first );
				if( it_del != m_mapPnpQueueEntry.end() )
					m_mapPnpQueueEntry.erase( it_del );  // Remove it
			}

			pnp.Release();
			Sleep(1);   // It won't hurt if another thread modifies m_mapPnpQueueEntry since we're iterating a copy anyway
			pnp.Relock();
		}

		if( !bOnlyBlockedEntries )  // If we have only blocked entries, we don't need to sleep here since we'll immediately hit the timedcondwait, and if we did, entries may become unblocked while we're sleeping
		{
			pnp.Release();
			Sleep(1000); // In case we're in a polling loop, don't hog all the CPU.  Nothing here is that time critical 
			pnp.Relock();
		}
	}
	m_bThreadRunning=false;
}

void PnpQueue::NewEntry(PnpQueueEntry *pPnpQueueEntry)
{
	if( !pPnpQueueEntry->m_pRow_Device_Reported )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::NewEntry -- don't know who reported this");
		return;
	}
	PLUTO_SAFETY_LOCK(pnp,m_pPlug_And_Play_Plugin->m_PnpMutex);

	for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)
	{
		PnpQueueEntry *pPnpQueueEntry2 = it->second;
		if( pPnpQueueEntry2->IsDuplicate(pPnpQueueEntry) )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::NewEntry ignoring queue %d, it's a duplicate of %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			pPnpQueueEntry->m_pRow_PnpQueue->Processed_set(1);
			m_pDatabase_pluto_main->PnpQueue_get()->Commit();
			delete pPnpQueueEntry;
			return;  // It was just a duplicate anyway
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::NewEntry adding %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
	m_mapPnpQueueEntry[pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()]=pPnpQueueEntry;
	pnp.Release();
	pthread_cond_broadcast( &m_pPlug_And_Play_Plugin->m_PnpCond );
}

bool PnpQueue::Process(PnpQueueEntry *pPnpQueueEntry)
{
	switch( pPnpQueueEntry->m_pRow_PnpQueue->Stage_get() )
	{
	case	PNP_DETECT_STAGE_DETECTED:
		return Process_Detect_Stage_Detected(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT:
		return Process_Detect_Stage_Confirm_Possible_DT(pPnpQueueEntry);
	case PNP_DETECT_STAGE_RUNNING_DETECTION_SCRIPTS:
		return Process_Detect_Stage_Running_Detction_Scripts(pPnpQueueEntry);
	case	PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT:
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT);
		return Process_Detect_Stage_Prompting_User_For_DT(pPnpQueueEntry);  // See notes in PnpQueueEntry.h for PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT
	case	PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT:
		return Process_Detect_Stage_Prompting_User_For_DT(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT:
		return Process_Detect_Stage_Running_Pre_Pnp_Script(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT:
		return Process_Detect_Stage_Prompting_User_For_Options(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_ADD_DEVICE:
		return Process_Detect_Stage_Add_Device(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_ADD_SOFTWARE:
		ReleaseQueuesBlockedFromPromptingState(pPnpQueueEntry);
		return Process_Detect_Stage_Add_Software(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_START_DEVICE:
		return Process_Detect_Stage_Start_Device(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_DONE:
		return true;

	case PNP_REMOVE_STAGE_REMOVED:
		return Process_Remove_Stage_Removed(pPnpQueueEntry);
	case PNP_REMOVE_STAGE_DONE:
		return true;
	};
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process ID %d is an unprocessable state %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->Stage_get());
	return true;  // Remove this because we can't do anything with it
}

bool PnpQueue::Process_Detect_Stage_Detected(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Detected: %s", pPnpQueueEntry->ToString().c_str());
#endif

	if( pPnpQueueEntry->m_pRow_PnpQueue->Signature_get().empty()==false && m_mapSignaturesReady.find(pPnpQueueEntry->m_pRow_PnpQueue->Signature_get())==m_mapSignaturesReady.end() )
	{
		pPnpQueueEntry->Block( PnpQueueEntry::pnpqe_blocked_waiting_for_initial_detection );
		return false;
	}

	// If this is using RS232 we need to wait until the machine it's running on has finished starting all devices
	// at least 20 seconds ago.  This way if the user has moved serial devices around on the ports this will give 
	// the GSD script enough time to finish initializing the serial device, see that it's not connected and disable 
	// it, so we can re-enable it on any other port
	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_RS232_CONST )
	{
		DeviceData_Router *pDevice = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceData_Router_Find( pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Reported_get() );
		if( pDevice )
		{
			pDevice = pDevice->GetTopMostDevice();  // Get the computer it's on

			// Detecting serial devices can be slow since it's a brute force trial and error.  The user may want to disable this.
			bool bIgnoreSerial = atoi(pDevice->m_mapParameters_Find(DEVICEDATA_Dont_Detect_Serial_Devices_CONST).c_str());
			if( !bIgnoreSerial )
			{
				// This could be a core/hybrid with an embedded media director.  So scan all the children too and if any of them 
				// have this flag  set, then ignore this device
				for(VectDeviceData_Impl::iterator it=pDevice->m_vectDeviceData_Impl_Children.begin();it!=pDevice->m_vectDeviceData_Impl_Children.end();++it)
				{
					DeviceData_Impl *pDevice_Child = *it;
					if( atoi(pDevice_Child->m_mapParameters_Find(DEVICEDATA_Dont_Detect_Serial_Devices_CONST).c_str()) )
					{
						bIgnoreSerial=true;
						break;
					}
				}
			}

			if( bIgnoreSerial )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Detected: queue %d ignoring serial devices",
					pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
				return true;
			}

			pair<time_t,time_t> pTimeUp = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceUpStatus_Find( pDevice->m_dwPK_Device );
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Detected: queue %d %s uptime for rs232 on %d is %d", 
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),
				pPnpQueueEntry->ToString().c_str(), pDevice->m_dwPK_Device, pTimeUp.first);
#endif
			if( pTimeUp.first==0 || time(NULL)-pTimeUp.first<20 )
			{
				pPnpQueueEntry->Block( PnpQueueEntry::pnpqe_blocked_waiting_for_other_device );
				return false;
			}
		}
	}


	if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="serial" )
	{
		// The serial ports on this box probably changed
		DeviceData_Router *pDevice_AppServer=NULL,*pDevice_Detector = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceData_Router_Find(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get());
		if( pDevice_Detector )
			pDevice_AppServer = (DeviceData_Router *) pDevice_Detector->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );  // Don't wait for it
		if( pDevice_AppServer )
		{
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pPlug_And_Play_Plugin->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
				"/usr/pluto/bin/UpdateAvailableSerialPorts.sh", "serialports",
				"","","",false,false,false,true);
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
			m_pPlug_And_Play_Plugin->SendCommand(CMD_Spawn_Application);
		}
	}
	else if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="storage" || pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="volume" || pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="scsi_host")
	{
		// The serial ports on this box probably changed
		DeviceData_Router *pDevice_AppServer=NULL,*pDevice_Detector = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceData_Router_Find(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get());
		if( pDevice_Detector )
			pDevice_AppServer = (DeviceData_Router *) pDevice_Detector->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );   // Don't wait for it
		if( pDevice_AppServer )
		{
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pPlug_And_Play_Plugin->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
				"/usr/pluto/bin/StorageDevices_Radar.sh", "storagedevices",
				"","","",false,false,false,true);
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
			m_pPlug_And_Play_Plugin->SendCommand(CMD_Spawn_Application);
		}
	}

	// Check before LocateDevice since this may loop back around after Process_Detect_Stage_Prompting_User_For_DT
	Row_Device *pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow() : NULL;  // This will be NULL if it's a new device
	if( !pRow_Device_Created )
		LocateDevice(pPnpQueueEntry);
	pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow() : NULL;  // This will be NULL if it's a new device
	if( pRow_Device_Created )
	{
		DetermineOrbitersForPrompting(pPnpQueueEntry,false); // For the Display Alert

		pPnpQueueEntry->RemoveBlockedDeviceData();
		pPnpQueueEntry->AssignDeviceData(pRow_Device_Created);
		int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device_Created->PK_Device_get());
		// See if this is a local device that has since moved from one system to another, like rs232 or usb
		if( PK_Device_Topmost != pPnpQueueEntry->m_dwPK_Device_TopLevel && 
			(pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_RS232_CONST || pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_USB_CONST || pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_Firewire_CONST || pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_PCI_CONST) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected for queue %d device %d has moved",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get());
			// This has moved from one machine to another.  It should have been disabled back when it was removed from the prior machine
			if( pRow_Device_Created->Disabled_get()==0 )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Detected for queue %d device %d moved but wasn't disbled",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get());

			int PK_Device_ControlledVia=DatabaseUtils::FindControlledViaCandidate(m_pDatabase_pluto_main,pRow_Device_Created->PK_Device_get(),
				pRow_Device_Created->FK_DeviceTemplate_get(),pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_pRow_Device_Reported->FK_Installation_get());
			if( !PK_Device_ControlledVia )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Detected for queue %d device %d no valid FindControlledViaCandidate",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get());
				PK_Device_ControlledVia = pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get();
			}

			pRow_Device_Created->FK_Device_ControlledVia_set(PK_Device_ControlledVia);
			SetDisableFlagForDeviceAndChildren(pRow_Device_Created,false);
			m_pDatabase_pluto_main->Device_get()->Commit();

			string sMessage = StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + " Moved existing device: " + pRow_Device_Created->Description_get();
			if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty()==false )
			{
				DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts,
					sMessage,"pnp_enabled_" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()),"5",interuptNoVideo);
				m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
			}
			
			Message *pMessage_Kill = new Message(m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get(),PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_QUIT,0);
			m_pPlug_And_Play_Plugin->QueueMessageToRouter(pMessage_Kill); // Kill the device at the old location
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_ADD_SOFTWARE);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d was existing device, changing controlled via",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			return Process_Detect_Stage_Add_Software(pPnpQueueEntry);
		}
		else if( pRow_Device_Created->Disabled_get()!=0 )
		{
			return ReenableDevice(pPnpQueueEntry,pRow_Device_Created);
		}
		else
		{
			if( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().size() )
				DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device_Created->PK_Device_get(),DEVICEDATA_Serial_Number_CONST,pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get(),true);
			RunSetupScript(pPnpQueueEntry);
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d was existing device, nothing to do",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			return true;
		}
	}
	else
	{
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d is new device, processing",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		return Process_Detect_Stage_Confirm_Possible_DT(pPnpQueueEntry);
	}
}

bool PnpQueue::Process_Detect_Stage_Confirm_Possible_DT(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Confirm_Possible_DT: %s", pPnpQueueEntry->ToString().c_str());
#endif

	vector<Row_UnknownDevices *> vectRow_UnknownDevices;
	string sSqlWhere;
	if( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("MacAddress like '%") + pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() + "%'";

	if( pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("'") + pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() + string("' like concat(VendorModelID,'%') and VendorModelID != ''");
	else if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("VendorModelId='DT:") + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get()) + "'";

	if( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("SerialNumber='") + StringUtils::SQLEscape(pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get()) + "'";
	else if( pPnpQueueEntry->m_pRow_PnpQueue->Parms_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("SerialNumber='") + StringUtils::SQLEscape(pPnpQueueEntry->m_pRow_PnpQueue->Parms_get()) + "'";

	// Check if device is in the UnknownDevices table, skip if it is.
	if( sSqlWhere.size() )
	{
		// Unknown only applies if it's on the same machine, unless it's an ethernet device which is universal
		string sAdditional = pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().size() ? "" : " AND FK_Device_PC=" + StringUtils::itos(pPnpQueueEntry->m_dwPK_Device_TopLevel);
		m_pDatabase_pluto_main->UnknownDevices_get()->GetRows(sSqlWhere + sAdditional,&vectRow_UnknownDevices);
		if( vectRow_UnknownDevices.size() )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d already unknown",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
			return true;
		}
	}

	// It's a new device.  Did we get a valid device template already?
	Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow() : NULL;  // This will be NULL if there's no device template in the PnpQueueEntry.
	if( pRow_DeviceTemplate )
	{
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d is new device, already know template",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		return Process_Detect_Stage_Prompting_User_For_DT(pPnpQueueEntry);
	}

	// Build a query to check the DHCPDevice table for an existing matching entry
	if( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().size()>=11 && pPnpQueueEntry->m_pRow_PnpQueue->FK_PnpProtocol_get() != PNPPROTOCOL_Interactor_CONST)  // It's IP based
	{
		PhoneDevice pd("", pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(), 0);
		sSqlWhere = StringUtils::i64tos(pd.m_iMacAddress) + ">=Mac_Range_Low AND " + StringUtils::i64tos(pd.m_iMacAddress) + "<=Mac_Range_High";
	}
	else if( pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get().size() )  // It's usb or similar that has a vendor/model ID
	{
		sSqlWhere = string("'") + pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() + "' like concat(VendorModelID,'%') and VendorModelID != '' ";
		string sCategory = pPnpQueueEntry->m_pRow_PnpQueue->Category_get();
		if( sCategory.empty()==false )
            sSqlWhere += " AND (Category IS NULL or Category='" + sCategory + "')";
	}
	else // Brute force, like RS232 or similar, where we have to check every device that matches the com method
		sSqlWhere = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PnpDetectionScript IS NOT NULL AND FK_CommMethod=" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get());

	vector<Row_DHCPDevice *> vectRow_DHCPDevice;
	m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sSqlWhere,&vectRow_DHCPDevice);

	// Since we didn't find any specific match in DHCPDevice (known devices), try to match anything in the same Category with the same VendorModelID.
	bool bMatchingCategory=false;
	if( vectRow_DHCPDevice.size()==0 && pPnpQueueEntry->m_pRow_PnpQueue->Category_get().size() )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d checking for category",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		sSqlWhere = "(VendorModelID IS NULL OR VendorModelID='') AND Parms='CAT:" + StringUtils::SQLEscape(pPnpQueueEntry->m_pRow_PnpQueue->Category_get()) + "'";
		m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sSqlWhere,&vectRow_DHCPDevice);
		bMatchingCategory=true;
	}

	if( vectRow_DHCPDevice.size()==0 )  // Try one last time just to find anything in that category
	{
		sSqlWhere = "(VendorModelID IS NULL OR VendorModelID='') AND Category='" + StringUtils::SQLEscape(pPnpQueueEntry->m_pRow_PnpQueue->Category_get()) + "'";
		m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sSqlWhere,&vectRow_DHCPDevice);
		bMatchingCategory=true;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d has %d candidates",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) vectRow_DHCPDevice.size());
#endif
	if( vectRow_DHCPDevice.size()>0 )  // At least one device has matched known devices, continue to narrow the results.
	{
		for(vector<Row_DHCPDevice *>::iterator it=vectRow_DHCPDevice.begin();it!=vectRow_DHCPDevice.end();++it)
		{
			Row_DHCPDevice *pRow_DHCPDevice = *it;
			if( pRow_DHCPDevice->SerialNumber_get().size() && pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().find(pRow_DHCPDevice->SerialNumber_get())==string::npos )
				continue; // Don't do this (add to possible devices) if the SerialNumber doesn't match
			if( !bMatchingCategory && pRow_DHCPDevice->Parms_get().size() && pPnpQueueEntry->m_pRow_PnpQueue->Parms_get().find(pRow_DHCPDevice->Parms_get())==string::npos )
				continue; // Don't do this (add to possible devices) if the Parms don't match
			if( !bMatchingCategory && pRow_DHCPDevice->Category_get().size() && pPnpQueueEntry->m_pRow_PnpQueue->Category_get()!=pRow_DHCPDevice->Category_get() )
			{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d skipping PK_DHCPDevice %d because category %s doesn't match",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),vectRow_DHCPDevice[0]->PK_DHCPDevice_get(),pPnpQueueEntry->m_pRow_PnpQueue->Category_get().c_str());
#endif
				continue; // Don't do this (add to possible devices) if a category is specified and it doesn't match
			}

			pPnpQueueEntry->m_mapPK_DHCPDevice_possible[pRow_DHCPDevice->PK_DHCPDevice_get()]=pRow_DHCPDevice;  // This is a possibility
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d could be %d devices 1st is PK_DHCPDevice %d, actually matched %d",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) vectRow_DHCPDevice.size(),vectRow_DHCPDevice[0]->PK_DHCPDevice_get(),pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size());
#endif
	}
	
	// Special case.  If we're sure this is a Serial Port, such as a USB->RS232 dongle, change the comm method to rs232, strip out vendor/model/etc., and start over
	if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==1 )
	{
		Row_DHCPDevice *pRow_DHCPDevice = pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()->second;
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_DHCPDevice->FK_DeviceTemplate_getrow();
		if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Serial_Ports_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d is usb->rs232",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			pPnpQueueEntry->m_mapPK_DHCPDevice_possible.clear();
			pPnpQueueEntry->m_iPK_DHCPDevice=0;
			pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_set(COMMMETHOD_RS232_CONST);
			pPnpQueueEntry->m_pRow_PnpQueue->FK_PnpProtocol_set(PNPPROTOCOL_Proprietary_CONST);
			pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_set(0);  // there's a glitch in sql2cpp that setnull will set the field to null, but subsequent _get still report the same value
			pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_set("");
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_set("");
			pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_set("");
			pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_setNull(true);
			pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_setNull(true);
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_setNull(true);
			pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_setNull(true);
			pPnpQueueEntry->m_pRow_PnpQueue->Path_set("");
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DETECTED);
			return Process_Detect_Stage_Detected(pPnpQueueEntry);
		}
	}
	if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==0 && CheckForDeviceTemplateOnWeb(pPnpQueueEntry) )  // We didn't match a known device locally.  Check the web for a DT.
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d identified on web",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT);
		return Process_Detect_Stage_Running_Pre_Pnp_Script(pPnpQueueEntry);
	}

	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_RUNNING_DETECTION_SCRIPTS);
	return false;  // We may have blocked the mutex a while already.  Exit, let the mutex get released, instead of directly doing: return Process_Detect_Stage_Running_Detction_Scripts(pPnpQueueEntry);
}

/*
struct Web_DeviceData
{
	string m_sMacAddress;
	int m_iPK_DeviceTemplate;
	map<int, string> m_mapDeviceData;
};
*/
bool PnpQueue::CheckForDeviceTemplateOnWeb(PnpQueueEntry *pPnpQueueEntry)
{
	/*
	string sMacAddress=pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get();
	Web_DHCP_Query Web_Query(sURL_Base);
	Web_DHCP_Query_Params Web_Params;
	Web_DHCP_Query_Result Web_Result;

	Web_Params["MAC"] = sMacAddress;
	Web_Query.Query(Web_Params, Web_Result);

	if (Web_Result.size() == 0 || Web_Result[0].size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Empty result from web query");
		return false;
	}
	else if (Web_Result[0][0] == "FAIL")
	{
		if (Web_Result[0].size() >= 2)
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Query failed. Server said: %s", Web_Result[0][1].c_str());
		else
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Query failed. Server didn't return reason message.");
		return false;
	}
	else if (Web_Result[0].size() < 2)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Query failed. Server returned incomplete answer");
		return false;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Query succeeded. Server returned: '%s', '%s'", Web_Result[0][0].c_str(), Web_Result[0][1].c_str());
		
		int iPK_WebQuery = atoi(Web_Result[0][0].c_str());
		string sWeb_Description = Web_Result[0][1];
		if (iPK_WebQuery <= 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Invalid PK in result: %d", iPK_WebQuery);
			return false;
		}
		else
		{
			m_mapMacPKDescription[sMacAddress] = pairPKDescription(iPK_WebQuery, sWeb_Description);
			
			DCE::SCREEN_NewMacAddress_DL SCREEN_NewMacAddress_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get(), sMacAddress, sIPAddress);
			SendCommand(SCREEN_NewMacAddress_DL);

			return false;
		}
	}
*/
	return false;
}

bool PnpQueue::Process_Detect_Stage_Prompting_User_For_DT(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Prompting_User_For_DT: %s", pPnpQueueEntry->ToString().c_str());
#endif

	// Be sure another duplicate wasn't recently added before we ask the user.  This happened when a device was added as a NAS, and also detected as a file server
	LocateDevice(pPnpQueueEntry);
	Row_Device *pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow() : NULL;  
	if( pRow_Device_Created!=NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Prompting_User_For_DT: %s it was a new device, but now it's %d", pPnpQueueEntry->ToString().c_str(), pRow_Device_Created->PK_Device_get());
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DETECTED);
		return Process_Detect_Stage_Detected(pPnpQueueEntry);  // Loop back around.  We won't come here again
	}

	if( pPnpQueueEntry->m_iPK_DHCPDevice ) // See if the user already picked this from the menu
	{
		if( pPnpQueueEntry->m_iPK_DHCPDevice!=-1 )
		{
			Row_DHCPDevice *pRow_DHCPDevice = m_pDatabase_pluto_main->DHCPDevice_get()->GetRow(pPnpQueueEntry->m_iPK_DHCPDevice);
			if( pRow_DHCPDevice ) // Should always be true
				pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_set( pRow_DHCPDevice->FK_DeviceTemplate_get() );
			else
				pPnpQueueEntry->m_iPK_DHCPDevice=0;  // Something went really wrong
		}
		Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow() : NULL;  // This will be NULL if there's no device template
		if( pRow_DeviceTemplate )  // We know what it is, ask the user for options.  Since m_iPK_DHCPDevice!=0, the user either chose a valid entry from the grid, or his sole selection (-1)
		{
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT);
			return Process_Detect_Stage_Running_Pre_Pnp_Script(pPnpQueueEntry);
		}
	}

	Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow() : NULL;  // This will be NULL if there's no device template
	if( pRow_DeviceTemplate )  // We know what it is, ask the user for options
	{
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_PNP_Create_Without_Prompting_CONST);
		if( pRow_DeviceTemplate_DeviceData && atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str()) )
		{
			pPnpQueueEntry->m_bCreateWithoutPrompting=true;
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT);
			return Process_Detect_Stage_Running_Pre_Pnp_Script(pPnpQueueEntry);
		}
	}
	// If there's only 1 possible device template, and it's DEVICEDATA_PNP_Create_Without_Prompting_CONST is true, then just it without prompting
	else if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==1 )
	{
		Row_DHCPDevice *pRow_DHCPDevice = pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()->second;
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DHCPDevice->FK_DeviceTemplate_get(),DEVICEDATA_PNP_Create_Without_Prompting_CONST);
		if( pRow_DeviceTemplate_DeviceData && atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str()) )
		{
			pPnpQueueEntry->m_bCreateWithoutPrompting=true;
			pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_set(pRow_DHCPDevice->FK_DeviceTemplate_get());
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT);
			return Process_Detect_Stage_Running_Pre_Pnp_Script(pPnpQueueEntry);
		}
	}

	if( !pRow_DeviceTemplate && pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==0 )  // We have no possibilities.  We'll have to skip it
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Prompting_User_For_DT no possibilities: %s", pPnpQueueEntry->ToString().c_str());
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
		return true;
	}

	if( m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
		return false; // We're waiting for user input.  Give the user more time.
	}

	if( pPnpQueueEntry->m_EBlockedState == PnpQueueEntry::pnpqe_blocked_prompting_device_template )
	{
		if( time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
			return false; // We're waiting for user input.  Give the user more time.
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT user didn't respond to queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		pPnpQueueEntry->UseAllOrbitersForPrompt();
	}

	if( DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up

	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_device_template);
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT);

	if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==1 || pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
	{
		Row_DHCPDevice *pRow_DHCPDevice = pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==1 ? pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()->second : NULL;
		Row_DeviceTemplate *pRow_DeviceTemplate = NULL;
		if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
			pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow();
		if( !pRow_DeviceTemplate && pRow_DHCPDevice )
			pRow_DeviceTemplate	= pRow_DHCPDevice->FK_DeviceTemplate_getrow();
		else if( !pRow_DHCPDevice )
		{
			for(map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();++it)
			{
				Row_DHCPDevice *pRow_DHCPDevice2 = it->second;
				if( pRow_DHCPDevice2->FK_DeviceTemplate_get()==pRow_DeviceTemplate->PK_DeviceTemplate_get() )
				{
					pRow_DHCPDevice = pRow_DHCPDevice2;
					break;
				}
			}
			if( pRow_DeviceTemplate && !pRow_DHCPDevice )
			{
				vector<Row_DHCPDevice *> vectRow_DHCPDevice;
				pRow_DeviceTemplate->DHCPDevice_FK_DeviceTemplate_getrows(&vectRow_DHCPDevice);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT queue %d pRow_DeviceTemplate && !pRow_DHCPDevice %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) vectRow_DHCPDevice.size());
#endif
				if( vectRow_DHCPDevice.size()>0 )
					pRow_DHCPDevice = vectRow_DHCPDevice[0];
			}
		}
		if( pRow_DeviceTemplate && pRow_DHCPDevice )
		{
			Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_Autoassign_to_parents_room_CONST);
			int PK_Room=0;
			string sRoom;
			if( pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()!=COMMMETHOD_Ethernet_CONST && (!pRow_DeviceTemplate_DeviceData || atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str())==0) )
			{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT pk: %d %s room %d",
					  pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_pRow_Device_Reported->Description_get().c_str(),pPnpQueueEntry->m_pRow_Device_Reported->FK_Room_get());
#endif

				pPnpQueueEntry->m_pRow_Device_Reported->Reload();
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT 2 pk: %d %s room %d",
					  pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_pRow_Device_Reported->Description_get().c_str(),pPnpQueueEntry->m_pRow_Device_Reported->FK_Room_get());
#endif
				PK_Room = pPnpQueueEntry->m_pRow_Device_Reported->FK_Room_get();
				Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(PK_Room);
				if( pRow_Room )
					sRoom = pRow_Room->Description_get();
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT Auto assign queue %d to parents room %d %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),PK_Room,sRoom.c_str());
#endif
			}

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT 1 possibility queue %d pRow_DeviceTemplate %d && pRow_DHCPDevice %d",
					  pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_DeviceTemplate->PK_DeviceTemplate_get(),pRow_DHCPDevice->PK_DHCPDevice_get());
#endif
			DCE::SCREEN_New_Pnp_Device_One_Possibility_DL SCREEN_New_Pnp_Device_One_Possibility_DL(m_pPlug_And_Play_Plugin->m_dwPK_Device, pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
				PK_Room,
				pRow_DHCPDevice->PK_DHCPDevice_get(),
				GetDescription(pPnpQueueEntry),
				sRoom,
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),interuptOnlyAudio,false,true);
			m_pPlug_And_Play_Plugin->SendCommand(SCREEN_New_Pnp_Device_One_Possibility_DL);
			return false;
		}
	}

	// For now we'll skip over rs232 devices if we don't detect it via pnp.  Sometimes the serial port just appears to be active
	// even when it's not connected because the h/w flow control is high
	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_RS232_CONST )
	{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT queue %d todo -- create a new screen when it's serial and we have no possibilities -- there are too many options for the main one",
pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
		return true;
	}
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_DT queue %d multiple choices",
					  pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	DCE::SCREEN_NewPnpDevice_DL SCREEN_NewPnpDevice_DL(m_pPlug_And_Play_Plugin->m_dwPK_Device, pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, GetDescription(pPnpQueueEntry), pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),interuptOnlyAudio,false,true);
	m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NewPnpDevice_DL);
	return false;  // Now we wait
}

bool PnpQueue::Process_Detect_Stage_Running_Pre_Pnp_Script(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Running_Pre_Pnp_Script: %s", pPnpQueueEntry->ToString().c_str());
#endif
	if( pPnpQueueEntry->m_EBlockedState == PnpQueueEntry::pnpqe_blocked_pre_pnp_script )
	{
		if( time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PRE_PNP_SCRIPT )
			return false; // We're waiting for user input.  Give the user more time.
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Running_Pre_Pnp_Script queue %d pre pnp script didn't finish",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	}

	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get());
	if( pRow_DeviceTemplate  ) // Should always be true
	{
		Row_Device *pRow_Device = FindDisabledDeviceTemplateOnPC(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pRow_DeviceTemplate->PK_DeviceTemplate_get());
		if( pRow_Device )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::Process_Detect_Stage_Running_Pre_Pnp_Script queue %d re-enabling %d", pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pRow_Device->PK_Device_get());
			return ReenableDevice(pPnpQueueEntry,pRow_Device);
		}
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow( pRow_DeviceTemplate->PK_DeviceTemplate_get(), DEVICEDATA_Pre_Pnp_Script_CONST);
		if( pRow_DeviceTemplate_DeviceData && pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().empty()==false )
		{
			DeviceData_Base *pDevice_AppServer = m_pPlug_And_Play_Plugin->m_pData->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST, m_pPlug_And_Play_Plugin, 0 ); // Don't wait for it
			if( pDevice_AppServer )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pPlug_And_Play_Plugin->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
					pRow_DeviceTemplate_DeviceData->IK_DeviceData_get(), "prepnp_" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()),
					StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + "\t" + StringUtils::itos(m_pPlug_And_Play_Plugin->m_dwPK_Device),
					"","",false,false,false,true);
				if( m_pPlug_And_Play_Plugin->SendCommand(CMD_Spawn_Application)==false )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Pre_Pnp_Script queue %d failed -- aborting",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());  // App Server may not be running yet
					pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_waiting_for_other_device);
					return false;
				}
				else
				{
					pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_pre_pnp_script);
					return false;
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Running_Pre_Pnp_Script queue %d no app server",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_waiting_for_other_device);
				return false;
			}
		}
	}
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT);
	return Process_Detect_Stage_Prompting_User_For_Options(pPnpQueueEntry);
}

bool PnpQueue::Process_Detect_Stage_Prompting_User_For_Options(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Prompting_User_For_Options: %s", pPnpQueueEntry->ToString().c_str());
#endif
	if( pPnpQueueEntry->m_bCreateWithoutPrompting==false && BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up, if it's not one to just auto create regardless

	if( pPnpQueueEntry->m_EBlockedState == PnpQueueEntry::pnpqe_blocked_prompting_options )
	{
		if( time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
			return false; // We're waiting for user input.  Give the user more time.
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Prompting_User_For_Options user didn't respond to queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
	}

	if( m_Pnp_PreCreateOptions.OkayToCreateDevice(pPnpQueueEntry)==false )  // See if the user needs to specify some options
		return false;  // The user needs to specify some options

	ReleaseQueuesBlockedFromPromptingState(pPnpQueueEntry);

	// We're good to go, let's create the device
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_ADD_DEVICE);
	return false;  // Adding the device takes a long time.  We may have blocked the mutex a while already.  Exit, let the mutex get released, instead of directly doing: return Process_Detect_Stage_Add_Device(pPnpQueueEntry);
}

bool PnpQueue::Process_Detect_Stage_Add_Device(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Add_Device: %s", pPnpQueueEntry->ToString().c_str());
#endif
	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow();
	if( !pRow_DeviceTemplate || !pCommand_Impl_GIP ) // They must all be here
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Add_Device something went wrong for queue %d (%p)!",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pCommand_Impl_GIP);
		return true; // Delete this, something went terribly wrong
	}

	if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="mobile_phone" )
	{
		// For mobile phones we use the 'new orbiter' command instead
		int PK_Device=0;
		DCE::CMD_New_Orbiter CMD_New_Orbiter(m_pPlug_And_Play_Plugin->m_dwPK_Device,m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_dwPK_Device,
			"",
			atoi(pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_PK_Users_CONST].c_str()),
			pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get(),
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(),
			0,0,0,0,0,0,&PK_Device);
		m_pPlug_And_Play_Plugin->SendCommand(CMD_New_Orbiter);
		if( !PK_Device )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Add_Device couldn't create mobile phone queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		else
		{
			Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
			if( pRow_Device )
				pPnpQueueEntry->AssignDeviceData(pRow_Device);
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CreateDevice already adds the software and starts it, so we're done
		return true; // Delete this, something went terribly wrong
	}

	pPnpQueueEntry->RemoveBlockedDeviceData();

	int PK_Device_Related = pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get();
	if( pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Computers_CONST || 
		pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Core_CONST || 
		pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST )
			PK_Device_Related = 0;  // These are top level devices
			
	int iPK_Device=0;
	DCE::CMD_Create_Device CMD_Create_Device( m_pPlug_And_Play_Plugin->m_dwPK_Device, pCommand_Impl_GIP->m_dwPK_Device, 
		pRow_DeviceTemplate->PK_DeviceTemplate_get(), pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(), pPnpQueueEntry->m_iPK_Room, pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get(),
		pPnpQueueEntry->DeviceDataAsString(),pPnpQueueEntry->m_iPK_DHCPDevice,0 /* let it find the parent based on the relationship */,
		GetDeviceName(pPnpQueueEntry),
		pPnpQueueEntry->m_pOH_Orbiter_Active_get() ? pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device : 0,
		PK_Device_Related,&iPK_Device);

	m_pPlug_And_Play_Plugin->SendCommand(CMD_Create_Device);
	if( !iPK_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Add_Device couldn't create device queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		return true; // Delete this, something went terribly wrong
	}
	pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(iPK_Device);

	if( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().size() )
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_Serial_Number_CONST,pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get(),true);

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	if( pRow_Device )  // Should always be the case,
	{
		pRow_Device->Reload();
		if( pRow_Device->Disabled_get()==1 )  // It's possible this is a 'only one per pc' that was already there but disabled, like a Bluetooth Dongle
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Add_Device: enable device %d: %s", pRow_Device->PK_Device_get(), pPnpQueueEntry->ToString().c_str());
			ReenableDevice(pPnpQueueEntry,pRow_Device);
		}
		else if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty()==false )
		{
			string sMessage = StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + " Added device: " + pRow_Device->Description_get();
			DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts,
				sMessage,"pnp_added_" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()),"5",interuptAlways);
			m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
		}
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Add_Device: done %s", pPnpQueueEntry->ToString().c_str());
#endif
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CreateDevice already adds the software and starts it, so we're done
	return true; 
}

bool PnpQueue::Process_Detect_Stage_Add_Software(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Add_Software: %s", pPnpQueueEntry->ToString().c_str());
#endif
	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP )
	{
		string sResponse;
		DCE::CMD_Check_for_updates CMD_Check_for_updates(m_pPlug_And_Play_Plugin->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device);
		if( !m_pPlug_And_Play_Plugin->SendCommand(CMD_Check_for_updates,&sResponse) )
		{
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_installing_software);
			return false;  // This will cause us to block for 3 minutes
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CMD_Check_for_updates starts any devices, so no need to do anything here
	}

	RunSetupScript(pPnpQueueEntry);
	return true;
}

void PnpQueue::RunSetupScript(PnpQueueEntry *pPnpQueueEntry)
{
	Row_Device *pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow();
	if( pRow_Device_Created )
	{
		Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device_Created->PK_Device_get(),DEVICEDATA_Setup_Script_CONST);
		if( pRow_Device_DeviceData )
		{
			string sBinary = pRow_Device_DeviceData->IK_DeviceData_get();
			if( sBinary.empty()==false )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::RunSetupScript queue %d binary %s",
					pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sBinary.c_str());
#endif
				DeviceData_Router *pDevice_AppServer=NULL,*pDevice_Detector = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceData_Router_Find(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get());
				if( pDevice_Detector )
					pDevice_AppServer = (DeviceData_Router *) pDevice_Detector->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );  // Don't wait for it
				if( pDevice_AppServer )
				{
					string sArguments = StringUtils::itos(pRow_Device_Created->PK_Device_get());
					pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device_Created->PK_Device_get(),DEVICEDATA_Location_on_PCI_bus_CONST);
					if( pRow_Device_DeviceData )
						sArguments += "\t" + pRow_Device_DeviceData->IK_DeviceData_get();

					DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pPlug_And_Play_Plugin->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
						"/usr/pluto/bin/" + sBinary,"setup_script",sArguments,"","",false,false,false,true);
					CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Persist;
					m_pPlug_And_Play_Plugin->SendCommand(CMD_Spawn_Application);
				}
			}
		}
	}
}

bool PnpQueue::Process_Detect_Stage_Start_Device(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Start_Device: %s", pPnpQueueEntry->ToString().c_str());
#endif
	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP )
	{
		string sResponse;
		// Check for updates will also start any devices
		DCE::CMD_Check_for_updates CMD_Check_for_updates(m_pPlug_And_Play_Plugin->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device);
		if( !m_pPlug_And_Play_Plugin->SendCommand(CMD_Check_for_updates,&sResponse) )
		{
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_installing_software);
			return false;  // This will cause us to block for 3 minutes
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CMD_Check_for_updates starts any devices, so no need to do anything here
	}

	return true;
}

bool PnpQueue::Process_Remove_Stage_Removed(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Remove_Stage_Removed: %s", pPnpQueueEntry->ToString().c_str());
#endif
	LocateDevice(pPnpQueueEntry);
	Row_Device *pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow() : NULL;  // This will be NULL if it's a new device
	if( pRow_Device_Created )
	{
		SetDisableFlagForDeviceAndChildren(pRow_Device_Created,true);
		m_pDatabase_pluto_main->Device_get()->Commit();

		DetermineOrbitersForPrompting(pPnpQueueEntry,false); // For the Display Alert
		string sMessage = StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + " Disabled: " + pRow_Device_Created->Description_get();
		if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty()==false )
		{
			DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts,
				sMessage,"pnp_enabled_" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()),"5",interuptNoVideo);
			m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Remove_Stage_Removed killing queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		Message *pMessage_Kill = new Message(m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get(),PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_QUIT,0);
		m_pPlug_And_Play_Plugin->QueueMessageToRouter(pMessage_Kill); // Kill the device at the old location
	}
	
	for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)
	{
		// If there's some add's that are older than this remove, get rid of them
		PnpQueueEntry *pPnpQueueEntry2 = it->second;
		if( pPnpQueueEntry2->m_pRow_PnpQueue->Removed_get()==0 && pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get()<pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get() &&
			( (pPnpQueueEntry2->m_pRow_PnpQueue->SerialNumber_get().size() && pPnpQueueEntry2->m_pRow_PnpQueue->SerialNumber_get()==pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get()) ||
			(pPnpQueueEntry2->m_pRow_PnpQueue->MACaddress_get().size() && pPnpQueueEntry2->m_pRow_PnpQueue->MACaddress_get()==pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get()) ||
			(pPnpQueueEntry2->m_pRow_PnpQueue->IPaddress_get().size() && pPnpQueueEntry2->m_pRow_PnpQueue->IPaddress_get()==pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get()) ) )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Remove_Stage_Removed: removing extra queue %d", pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			pPnpQueueEntry2->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
			pPnpQueueEntry2->Stage_set(PNP_DETECT_STAGE_DONE);
			DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
				m_pPlug_And_Play_Plugin->m_dwPK_Device, m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NewPnpDevice_CONST);
			DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL2(
				m_pPlug_And_Play_Plugin->m_dwPK_Device, m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_New_Pnp_Device_One_Possibility_CONST);
			CMD_Remove_Screen_From_History_DL.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Screen_From_History_DL2.m_pMessage);
			m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
			ReleaseQueuesBlockedFromPromptingState(pPnpQueueEntry2);
		}
	}

	pPnpQueueEntry->Stage_set(PNP_REMOVE_STAGE_DONE);
	return true;
}

bool PnpQueue::LocateDevice(PnpQueueEntry *pPnpQueueEntry)
{
	Row_Device *pRow_Device = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? m_pDatabase_pluto_main->Device_get()->GetRow( pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ) : NULL;
	if( pRow_Device )
		return true;  // This device already exists

	//  See if there's a special locator for this category of device
	map<string,fnLocateDevice>::iterator itLD; 
	if( (itLD = m_mapCategoryLocateDevice.find( pPnpQueueEntry->m_pRow_PnpQueue->Category_get() ))!=m_mapCategoryLocateDevice.end() )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d running special locator",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		return CALL_MEMBER_FN(*this,itLD->second) (pPnpQueueEntry);
	}

	string sPK_Device_TopLevel = StringUtils::itos(pPnpQueueEntry->m_dwPK_Device_TopLevel);
	vector<Row_Device *> vectRow_Device;
	string sSerialOrMac;
	if( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().size() )
		sSerialOrMac = "MACaddress='" + pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() + "'";
	else if( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().size() )
	{
		sSerialOrMac = "LEFT JOIN Device AS P1 ON Device.FK_Device_ControlledVia = P1.PK_Device "
			"LEFT JOIN Device AS P2 ON P1.FK_Device_ControlledVia = P2.PK_Device "
			"LEFT JOIN Device_DeviceData As SerialNumber ON SerialNumber.FK_Device=Device.PK_Device AND SerialNumber.FK_DeviceData=" TOSTRING(DEVICEDATA_Serial_Number_CONST) " "
			"WHERE (Device.FK_Device_ControlledVia=" + sPK_Device_TopLevel + " OR P1.FK_Device_ControlledVia=" + sPK_Device_TopLevel +
			" OR P2.FK_Device_ControlledVia=" + sPK_Device_TopLevel + ")";

		sSerialOrMac += " AND SerialNumber.IK_DeviceData='" + StringUtils::SQLEscape(pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get()) + "'";
	}

	if( sSerialOrMac.size() )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d if( sSerialOrMac.size() ) %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sSerialOrMac.c_str());
#endif
		m_pDatabase_pluto_main->Device_get()->GetRows(sSerialOrMac,&vectRow_Device);
		if( vectRow_Device.size() )
		{
			pRow_Device = vectRow_Device[0];
			if( vectRow_Device.size()>1 )
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"PnpQueue::LocateDevice queue %d more than 1 device matched %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sSerialOrMac.c_str());
			pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice( queue %d already a device %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get());
#endif
			return true;
		}
	}

	bool bFirstWhere=true;
	string sSql_Primary = "JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
		"LEFT JOIN Device AS P1 ON Device.FK_Device_ControlledVia = P1.PK_Device "
		"LEFT JOIN Device AS P2 ON P1.FK_Device_ControlledVia = P2.PK_Device "
		"LEFT JOIN DHCPDevice ON DHCPDevice.FK_DeviceTemplate=PK_DeviceTemplate "
		"LEFT JOIN Device_DeviceData As ComPort ON ComPort.FK_Device=Device.PK_Device AND ComPort.FK_DeviceData=" TOSTRING(DEVICEDATA_COM_Port_on_PC_CONST) " "
		"LEFT JOIN DeviceTemplate_DeviceData As OnePerPC ON OnePerPC.FK_DeviceTemplate=PK_DeviceTemplate AND OnePerPC.FK_DeviceData=" TOSTRING(DEVICEDATA_Only_One_Per_PC_CONST) " "
		"WHERE (Device.FK_Device_ControlledVia=" + sPK_Device_TopLevel + " OR P1.FK_Device_ControlledVia=" + sPK_Device_TopLevel +
		" OR P2.FK_Device_ControlledVia=" + sPK_Device_TopLevel + ")";


	string sSql_Model,sSql_Port;

	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		sSql_Model += " AND Device.FK_DeviceTemplate=" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get());

	string sMacAddress=pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get();
	if( sMacAddress.size() )
		sSql_Model += " AND Device.MacAddress like '%" + sMacAddress + "%'";

	bool bUsbSerial=false,bSpecifiedComPort=false;
	string sVendorModelId = pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get();
	if( sVendorModelId.size() )
	{
		// See if this is a USB->Serial
		string sSqlUSB = "SELECT DeviceTemplate.FK_DeviceCategory FROM DHCPDevice "
			"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
			"WHERE VendorModelId like '" + sVendorModelId + "%' OR Parms = 'CAT:serial'";

		PlutoSqlResult result_set;
		DB_ROW row=NULL;
		if( ( result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result( sSqlUSB ) )!=0 && ( row = db_wrapper_fetch_row( result_set.r ) )!=NULL && row[0] && atoi(row[0])==DEVICECATEGORY_Serial_Ports_CONST )
		{
			bUsbSerial=true;
			if( pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d is a usb to serial with no com port",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				return false;  // If there's no port specified don't bother
			}
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d is a usb to serial with a com port",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			sSql_Model += " AND FK_CommMethod=" TOSTRING(COMMMETHOD_RS232_CONST) " ";  // Don't match vendor model.  We'll match the com port only
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d is not a usb to serial %s %p %s", pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), sSqlUSB.c_str(), row, row ? row[0] : "*NULL*");

			// To determine the actual match, we always first find devices that match vendor/model ID, and only if nothing matches, then we check category
			// If we don't check category, devices like Bluetooth_Dongle where the serial number changes won't get matched
			// If we check category, but don't exclude devices where a vendor/model matches, then devices like embedded disk drives in the jukebox will match Disk_Drive which only checks categories
			bool bOkayToMatchCategory=false;

			if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get().size() )
			{
				string sWhere = "'" + sVendorModelId + "' like concat(VendorModelID,'%') and VendorModelID != ''";
				vector<Row_DHCPDevice *> vectRow_DHCPDevice;
				m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sWhere,&vectRow_DHCPDevice);
				bOkayToMatchCategory = vectRow_DHCPDevice.size()==0;
			}

			if( bOkayToMatchCategory )
				sSql_Model += " AND DHCPDevice.Parms='CAT:" + pPnpQueueEntry->m_pRow_PnpQueue->Category_get() + "'";
			else
				sSql_Model += " AND '" + sVendorModelId + "' like concat(VendorModelID,'%') and VendorModelID != ''";
		}
	}

	if( pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end() )
	{
		bSpecifiedComPort=true;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d has com port %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST].c_str());
		if( pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_RS232_CONST )
			// This is just RS232.  There's no way to tell if the user moved a device or not.  If there's still an active device on this port
			// We can assume it's a match, since it would have been disabled if the user didn't remove it.  We don't want to always re-identify the same devices
			sSql_Port += " AND ComPort.IK_DeviceData='" + pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST] + "' AND Device.Disabled=0";
		else if( sSql_Model.size() )
			// This may be a usb-serial device.  We have some identifying attributes, so if there's a similar device on another port that's disabled, it's a match--the user must have moved it
			sSql_Port += " AND (ComPort.IK_DeviceData='" + pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST] + "' OR Device.Disabled=1)";
	}

	// If we have no defining characteristics, skip it, unless it's an rs232 and we're  going to scan for an existing device on this port
	if( sSql_Model.size()==0 && (pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()!=COMMMETHOD_RS232_CONST || sSql_Port.size()==0)  )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d has no identifying attributes",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		return false;
	}
	
	// See if this exact item with the same serial number exists already
	m_pDatabase_pluto_main->Device_get()->GetRows(sSql_Primary + sSql_Model + sSql_Port,&vectRow_Device);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d checking same serial number %s got %d",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(sSql_Primary + sSql_Model + sSql_Port).c_str(),(int) vectRow_Device.size());
#endif
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		pRow_Device = *it;
		if( bUsbSerial && (pRow_Device->Disabled_get()==1 || bSpecifiedComPort==false) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d device %d skipping because this is usb->serial %d/%d",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get(),pRow_Device->Disabled_get(),bSpecifiedComPort);
			continue;
		}
		if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Hard_Drives_CONST )
		{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d device %d is a hdd",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get());
#endif
			// For Hard drives the block device parameter must be the same
			Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
			if( !pRow_Device_DeviceData || pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Block_Device_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() ||
				pRow_Device_DeviceData->IK_DeviceData_get()!=pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_Block_Device_CONST] )
			{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d device %d is not the same hdd",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get());
#endif
					continue;  // It's not the same hard drive
			}
		}

		// If either has a location on the PCI bus, be sure they're both the same
		string sPCI2 = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Location_on_PCI_bus_CONST);
		string sPCI1 = pPnpQueueEntry->m_mapPK_DeviceData_Find(DEVICEDATA_Location_on_PCI_bus_CONST);
		if( (sPCI2.empty()==false || sPCI1.empty()==false) && sPCI1!=sPCI2 )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d device %d has different location on pci bus %s/%s",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get(),sPCI1.c_str(),sPCI2.c_str());
			continue;  // It's not the same hard drive
		}

		bool bDhcpDevice=true;
		// Don't bother checking criteria if this isn't a DHCP device.  First see if this is a DHCP Device, but just didn't match
		if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==0 )
		{
			bDhcpDevice=false;
			Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
			if( pRow_DeviceTemplate )
			{
				vector<Row_DHCPDevice *> vectRow_DHCPDevice;
				pRow_DeviceTemplate->DHCPDevice_FK_DeviceTemplate_getrows(&vectRow_DHCPDevice);
				if( vectRow_DHCPDevice.size() )
					bDhcpDevice=true;  // It is after all
			}
		}
		// Don't bother checking criteria if this isn't a DHCP device.
		if( bDhcpDevice==false || DeviceMatchesCriteria(pRow_Device,pPnpQueueEntry) )
		{
			pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice( queue %d mac:%s already a device",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sMacAddress.c_str());
#endif
			return true;
		}
	}

	vectRow_Device.clear();
	// Nope.  See if there is another similar item, perhaps a different unit (serial #), but the 'one per pc' is set.  If so, it's a match since we can only have 1 per pc anyway
	string sOnePerPc = " AND OnePerPC.IK_DeviceData IS NOT NULL AND OnePerPC.IK_DeviceData=1";

	m_pDatabase_pluto_main->Device_get()->GetRows(sSql_Primary + sSql_Model + sOnePerPc,&vectRow_Device);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d one per pc %s got %d",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(sSql_Primary + sSql_Model + sOnePerPc).c_str(),vectRow_Device.size());
#endif
	if( vectRow_Device.size() )
	{
		pRow_Device = vectRow_Device[0];
		if( DeviceMatchesCriteria(pRow_Device,pPnpQueueEntry) )
		{
			pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice( - queue %d mac:%s already a device",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sMacAddress.c_str());
#endif
			return true;
		}
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d no match",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	return false;
}

bool PnpQueue::DeviceMatchesCriteria(Row_Device *pRow_Device,PnpQueueEntry *pPnpQueueEntry)
{
	vector<Row_DHCPDevice *> vectRow_DHCPDevice;
	string sSQL = "JOIN DeviceTemplate ON DHCPDevice.FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN Device ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
		"WHERE PK_Device=" + StringUtils::itos(pRow_Device->PK_Device_get()) + " " 
		"AND (DHCPDevice.SerialNumber IS NOT NULL "
		"OR DHCPDevice.Parms IS NOT NULL)";
	m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sSQL,&vectRow_DHCPDevice);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DeviceMatchesCriteria queue %d device %d criteria %s got %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get(),sSQL.c_str(),(int) vectRow_DHCPDevice.size());
#endif
	for(vector<Row_DHCPDevice *>::iterator it=vectRow_DHCPDevice.begin();it!=vectRow_DHCPDevice.end();++it)
	{
		Row_DHCPDevice *pRow_DHCPDevice = *it;
		if( pRow_DHCPDevice->SerialNumber_get().size() && pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().find(pRow_DHCPDevice->SerialNumber_get())==string::npos )
			continue; // Don't do this if the serial number doesn't match
		if( pRow_DHCPDevice->Parms_get().size() )
		{
			if( StringUtils::StartsWith(pRow_DHCPDevice->Parms_get(),"CAT:") )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DeviceMatchesCriteria queue %d device %d category %s/%s",
					pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get(),
					pPnpQueueEntry->m_pRow_PnpQueue->Category_get().c_str(),pRow_DHCPDevice->Parms_get().c_str());
#endif
				if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get().size()==0 || pRow_DHCPDevice->Parms_get().find( pPnpQueueEntry->m_pRow_PnpQueue->Category_get() )==string::npos )
					continue; // The category doesn't match either
			}
			else if( pPnpQueueEntry->m_pRow_PnpQueue->Parms_get().find(pRow_DHCPDevice->Parms_get())==string::npos )
				continue; // Don't do this if the serial number doesn't match
		}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DeviceMatchesCriteria queue %d device %d matches %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get(),pRow_DHCPDevice->PK_DHCPDevice_get());
#endif
		return true;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DeviceMatchesCriteria queue %d device %d no match",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get());
#endif
	return false;
}

bool PnpQueue::Process_Detect_Stage_Running_Detction_Scripts(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Running_Detction_Scripts: %s", pPnpQueueEntry->ToString().c_str());
#endif

	// There's a special case for mobile phones since they have their own screen to prompt the user
	if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==0 )
	{
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d has no candidates",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		return true;
	}

	if( pPnpQueueEntry->m_EBlockedState == PnpQueueEntry::pnpqe_blocked_running_detection_scripts )
	{
		if( time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_DETECTION_SCRIPT )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d is blocked by %d.  waiting...",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_dwPK_PnpQueue_BlockingFor);
#endif
			return false; // We're waiting for user input.  Give the user more time.
		}
		else
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d is blocked and timed out",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			for(map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();)
			{
				Row_DHCPDevice *pRow_DHCPDevice = it->second;
				if( pPnpQueueEntry->m_sDetectionScript_Running==pRow_DHCPDevice->PnpDetectionScript_get() )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d script %s PK_DHCPDevice %d timed out",
						pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pPnpQueueEntry->m_sDetectionScript_Running.c_str(), pRow_DHCPDevice->PK_DHCPDevice_get());

					pPnpQueueEntry->m_mapPK_DHCPDevice_possible.erase(it++);
					continue;
				}
				++it;
			}
		}
	}

	DetermineOrbitersForPrompting(pPnpQueueEntry,false); // For the Display Alert

#ifdef DEBUG
	string sPossibilities;
	for(map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();++it)
		sPossibilities += StringUtils::itos(it->first) + ",";

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d has %d possibilities %s",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size(),sPossibilities.c_str());
#endif
	Row_DHCPDevice *pRow_DHCPDevice_To_Detect = NULL;
	for(map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();++it)
	{
		Row_DHCPDevice *pRow_DHCPDevice = it->second;
		if( pRow_DHCPDevice->PnpDetectionScript_get().size() && (pRow_DHCPDevice_To_Detect==NULL || pRow_DHCPDevice->PnpDetectionScript_get()<pRow_DHCPDevice_To_Detect->PnpDetectionScript_get()) )  // Do them in alpha order
			pRow_DHCPDevice_To_Detect = pRow_DHCPDevice;
	}

	if( pRow_DHCPDevice_To_Detect )
	{  
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d checking detection %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_DHCPDevice_To_Detect->PnpDetectionScript_get().c_str());
#endif
		DeviceData_Router *pDevice_AppServer=NULL,*pDevice_Detector=NULL;
		pDevice_Detector = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceData_Router_Find(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get());

		if( pDevice_Detector )
		{
			pDevice_AppServer = (DeviceData_Router *) pDevice_Detector->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST, m_pPlug_And_Play_Plugin, 0 );  // Don't wait for it
			if( !pDevice_AppServer )
			{
				DeviceData_Base *pDevice = pDevice_Detector->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d checking detection %s but cannot find app server - w/out reg found %p",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_DHCPDevice_To_Detect->PnpDetectionScript_get().c_str(),pDevice);
				pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_waiting_for_other_device);
				return false;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d checking detection %s but cannot find detector",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_DHCPDevice_To_Detect->PnpDetectionScript_get().c_str());
			return true;
		}

		if( pDevice_AppServer )
		{
			if( time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_DETECTION_SCRIPT && pPnpQueueEntry->m_sDetectionScript_Running==pRow_DHCPDevice_To_Detect->PnpDetectionScript_get() )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d started again, but stilll running %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_sDetectionScript_Running.c_str());
#endif
				pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_running_detection_scripts);
				return false;
			}

			string sPath;  // Where to find the device
			Row_DeviceTemplate *pRow_DeviceTemplate = pRow_DHCPDevice_To_Detect->FK_DeviceTemplate_getrow();
			if( pRow_DeviceTemplate )
			{
				if( pRow_DeviceTemplate->FK_CommMethod_get()==COMMMETHOD_RS232_CONST && pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end() )
				{
					int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pPlug_And_Play_Plugin->m_pDatabase_pluto_main,pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get());
					string sIPAddress = DatabaseUtils::GetIpOfDevice(m_pPlug_And_Play_Plugin->m_pDatabase_pluto_main,PK_Device_Topmost);
					if (PK_Device_Topmost == 1)
					{
						// Device is on core.
						sPath = TranslateSerialUSB(pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST]);
					}
					else
					{
						sPath = TranslateSerialUSB(pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST],sIPAddress);
					}
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d topmost %d ip %s port %s",
						pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),PK_Device_Topmost,
						sIPAddress.c_str(),sPath.c_str());
				}
				else if( pRow_DeviceTemplate->FK_CommMethod_get()==COMMMETHOD_Ethernet_CONST )
					sPath = pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get();
			}

			string sAlert = "Running detection script: " + pRow_DHCPDevice_To_Detect->PnpDetectionScript_get();
			if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty()==false )
			{
				DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts,
					sAlert,"pnp_" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()),"60",interuptNoVideo);
				m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
			}
			// The arguments are this device, the queue ie, the path where to find the device, the pnp script name
			string sArguments = StringUtils::itos(m_pPlug_And_Play_Plugin->m_dwPK_Device) + "\t" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + 
				"\t" + sPath + "\t" + pRow_DHCPDevice_To_Detect->PnpDetectionScript_get() + "\t" + pPnpQueueEntry->m_mapPK_DeviceData_Find(DEVICEDATA_Block_Device_CONST) + "\t" +
				pPnpQueueEntry->m_mapPK_DeviceData_Find(DEVICEDATA_Location_on_PCI_bus_CONST);
			string sName = "PNP Detection " + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + " " + pRow_DHCPDevice_To_Detect->PnpDetectionScript_get();
			string sMessage = "0 " + StringUtils::itos(m_pPlug_And_Play_Plugin->m_dwPK_Device) + " 1 " + TOSTRING(COMMAND_PNP_Detection_Script_Finished_CONST) + " "
				+ TOSTRING(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " 
				+ StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + " " + TOSTRING(COMMANDPARAMETER_Filename_CONST) + " \""
				+ pRow_DHCPDevice_To_Detect->PnpDetectionScript_get() + "\" " + TOSTRING(COMMANDPARAMETER_Errors_CONST) + " ";

			string sResponse;
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pPlug_And_Play_Plugin->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
				"/usr/pluto/pnp/" + pRow_DHCPDevice_To_Detect->PnpDetectionScript_get(), sName,
				sArguments,sMessage + "FAIL",sMessage + "OK",false,false,false,false);
			if( !m_pPlug_And_Play_Plugin->SendCommand(CMD_Spawn_Application,&sResponse) )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts app server %d not responding",pDevice_AppServer->m_dwPK_Device);
			
			// We have a detection script we need to run
			pPnpQueueEntry->m_sDetectionScript_Running=pRow_DHCPDevice_To_Detect->PnpDetectionScript_get();
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d now running %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_sDetectionScript_Running.c_str());
#endif
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_running_detection_scripts);
			return false;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Running_Detction_Scripts queue %d cannot find app server!",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_waiting_for_other_device);
			return false;
		}
	}

	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT);
	return Process_Detect_Stage_Prompting_User_For_DT(pPnpQueueEntry);

}

void PnpQueue::ReleaseQueuesBlockedFromPromptingState(PnpQueueEntry *pPnpQueueEntry)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::ReleaseQueuesBlockedFromPromptingState prossing %d", pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)
	{
		PnpQueueEntry *pPnpQueueEntry2 = it->second;
		if( pPnpQueueEntry2!=pPnpQueueEntry && pPnpQueueEntry2->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_waiting_for_other_prompting && pPnpQueueEntry2->m_dwPK_PnpQueue_BlockingFor==pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get() )
		{
			pPnpQueueEntry2->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
			if( pPnpQueueEntry2->m_pRow_PnpQueue->Stage_get()==PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT )
				pPnpQueueEntry2->Stage_set(PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT); // See notes in PnpQueueEntry.h for PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT
		}
	}
}

bool PnpQueue::BlockIfOtherQueuesAtPromptingState(PnpQueueEntry *pPnpQueueEntry)
{
	for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)
	{
		PnpQueueEntry *pPnpQueueEntry2 = it->second;
		if( pPnpQueueEntry2!=pPnpQueueEntry && pPnpQueueEntry2->m_EBlockedState!=PnpQueueEntry::pnpqe_block_processing_suspended && 
			pPnpQueueEntry2->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_waiting_for_other_prompting && 
			pPnpQueueEntry2->m_EBlockedState!=PnpQueueEntry::pnpqe_blocked_waiting_for_orbiters && 
			(pPnpQueueEntry2->m_pRow_PnpQueue->Stage_get()==PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT || pPnpQueueEntry2->m_pRow_PnpQueue->Stage_get()==PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT) )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::BlockIfOtherQueuesAtPromptingState queue %d auto-blocking for others %d blocked",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_waiting_for_other_prompting);
			pPnpQueueEntry->m_dwPK_PnpQueue_BlockingFor=pPnpQueueEntry2->m_pRow_PnpQueue->PK_PnpQueue_get();
			return true;
		}
	}
	return false;
}

void PnpQueue::ReadOutstandingQueueEntries()
{
	PLUTO_SAFETY_LOCK(pnp,m_pPlug_And_Play_Plugin->m_PnpMutex);

	vector<Row_PnpQueue *> vectRow_PnpQueue;
	m_pDatabase_pluto_main->PnpQueue_get()->GetRows("Processed=0",&vectRow_PnpQueue);
	for(vector<Row_PnpQueue *>::iterator it=vectRow_PnpQueue.begin();it!=vectRow_PnpQueue.end();++it)
	{
		Row_PnpQueue *pRow_PnpQueue = *it;
		if( pRow_PnpQueue->Signature_get().size() )
		{
			// It's possible a HAL device fired events that were added a previous time, and we've since rebooted and they're 
			// not here anymore.  So if there's a signature, don't read in old entries because they'll be redetected anyway
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::ReadOutstandingQueueEntries not adding signatured queue %d",
				pRow_PnpQueue->PK_PnpQueue_get());
#endif
		pRow_PnpQueue->Processed_set(1);
			continue;
		}

		PnpQueueEntry *pPnpQueueEntry = new PnpQueueEntry(m_pPlug_And_Play_Plugin,pRow_PnpQueue);
		bool bWasDuplicate=false;
		for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)
		{
			PnpQueueEntry *pPnpQueueEntry2 = it->second;
			if( pPnpQueueEntry2->IsDuplicate(pPnpQueueEntry) )
			{
				pPnpQueueEntry->m_pRow_PnpQueue->Processed_set(1);
				delete pPnpQueueEntry;
				bWasDuplicate=true;
				break;
			}
		}
		if( bWasDuplicate )
			continue;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::ReadOutstandingQueueEntries adding queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
		m_mapPnpQueueEntry[pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()]=pPnpQueueEntry;
	}
	pnp.Release();
	m_pDatabase_pluto_main->PnpQueue_get()->Commit();
}

bool PnpQueue::DetermineOrbitersForPrompting(PnpQueueEntry *pPnpQueueEntry,bool bBlockIfNone)
{
	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get()==COMMMETHOD_Ethernet_CONST || pPnpQueueEntry->m_bUseAllOrbitersForPrompt ) // This is universal, could be anywhere, ask on all orbiters
	{
		pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts=m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get();
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DetermineOrbitersForPrompting queue %d use all orbiters (%d): %s",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) pPnpQueueEntry->m_bUseAllOrbitersForPrompt, pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.c_str());
#endif
	}
	else
	{
		pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts = "";
		string s = m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get();
		string::size_type pos=0;
		while(pos<s.size())
		{
			string sPK_Orbiter = StringUtils::Tokenize(s,",",pos);
			OH_Orbiter *pOH_Orbiter = m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( atoi(sPK_Orbiter.c_str()) );
			if( !pOH_Orbiter || !pPnpQueueEntry->m_pRow_Device_Reported )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::DetermineOrbitersForPrompting queue %d orbiter %s/%p reported %p",sPK_Orbiter.c_str(),pOH_Orbiter,pPnpQueueEntry->m_pRow_Device_Reported);
				continue;   // Shouldn't really happen
			}
			if( pOH_Orbiter->m_dwPK_Room!=pPnpQueueEntry->m_pRow_Device_Reported->FK_Room_get() && 
				pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Room!=pPnpQueueEntry->m_pRow_Device_Reported->FK_Room_get() )
			{
				// This orbiter is not in the same room.  So unless it's an OSD, we will skip it
				if(	!pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device_MD ||
					(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device_MD!=pPnpQueueEntry->m_dwPK_Device_TopLevel &&
					pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device_Core!=pPnpQueueEntry->m_dwPK_Device_TopLevel) )
				{  // It's either not an OSD, or not the osd for this MD
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DetermineOrbitersForPrompting queue %d orbiter %d reported %d in room %d/%d being skipped",
						pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
						pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Room,pPnpQueueEntry->m_pRow_Device_Reported->FK_Room_get());
#endif
					continue;   // It doesn't normally belong in this room, and it's not currently in this room, and it's totally unrelated, so don't use it
				}
			}
			pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts += sPK_Orbiter + ",";
		}
	}

	// Now go through the list and remove any that aren't registered
	string sOutput;
	string::size_type pos=0;
	while( pos<pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.size() )
	{
		OH_Orbiter *pOH_Orbiter = m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( atoi(StringUtils::Tokenize(pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts,",",pos).c_str()) );
		if( pOH_Orbiter && pOH_Orbiter->m_tRegistered && time(NULL)-pOH_Orbiter->m_tRegistered>WAIT_SECONDS_AFTER_ORBITER_REGISTERS )  // Give it some time to get rid of the scrolling progress bar and any other messages
		{
			if( sOutput.empty()==false )
				sOutput += ",";
			sOutput += StringUtils::itos( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device );
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DetermineOrbitersForPrompting queue %d orbiter %d not registered",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
	}

	if( sOutput.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DetermineOrbitersForPrompting queue %d no orbiters",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		if( bBlockIfNone )
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_waiting_for_orbiters);
		return false;
	}

	pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts = sOutput;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DetermineOrbitersForPrompting queue %d returning %s",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.c_str());
#endif
	return true;
}

string PnpQueue::GetDescription(PnpQueueEntry *pPnpQueueEntry)
{
	string sDescription;

	if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="mobile_phone" )
	{
		sDescription = pPnpQueueEntry->m_pRow_PnpQueue->Description_get();
		if( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().empty()==false )
			sDescription += "(" + pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() + ")";
		if( sDescription.empty()==false )
			return sDescription;
	}

	Row_DeviceTemplate *pRow_DeviceTemplate = NULL;
	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow();
	else if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end() )
		pRow_DeviceTemplate=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()->second->FK_DeviceTemplate_getrow();

	if( pRow_DeviceTemplate )
	{
		if( pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Hard_Drives_CONST )
			sDescription = "Drive: " + pPnpQueueEntry->m_pRow_PnpQueue->Description_get();
		else if( pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Network_Storage_CONST && pPnpQueueEntry->m_pRow_Device_Reported )
			sDescription = pPnpQueueEntry->m_pRow_Device_Reported->Description_get() + " / " + pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_Share_Name_CONST];
		else if( pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_FileMedia_Server_CONST )
			sDescription = pRow_DeviceTemplate->Description_get() + " (" + pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get() + ") " +
			pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_Description_CONST];

		bool bDefaultDescription=false; // Will add more comments if this is just the device template description as the default
		if( sDescription.empty() )
		{
			sDescription = pRow_DeviceTemplate->Description_get();
			bDefaultDescription=true;
		}

		map<int,string>::iterator it;
		if( (it=pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Description_CONST))!=pPnpQueueEntry->m_mapPK_DeviceData.end() )
			sDescription += "(" + it->second + ")";
		else if( bDefaultDescription )
		{
			Row_Manufacturer *pRow_Manufacturer = pRow_DeviceTemplate->FK_Manufacturer_getrow();
			Row_DeviceCategory *pRow_DeviceCategory = pRow_DeviceTemplate->FK_DeviceCategory_getrow();
			sDescription += "(" + (pRow_Manufacturer ? pRow_Manufacturer->Description_get() + " / " : "") +
				(pRow_DeviceCategory ? pRow_DeviceCategory->Description_get() : "") + ")";
		}
	}

	if( sDescription.size()==0 )
		sDescription = pPnpQueueEntry->m_pRow_Device_Reported->Description_get();
	return sDescription;
}

string PnpQueue::GetDeviceName(PnpQueueEntry *pPnpQueueEntry)
{
	Row_DeviceTemplate *pRow_DeviceTemplate = NULL;
	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow();
	else if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end() )
		pRow_DeviceTemplate=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin()->second->FK_DeviceTemplate_getrow();

	if( !pRow_DeviceTemplate )
		return NULL;

	if( pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST && pPnpQueueEntry->m_iPK_Room )
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(pPnpQueueEntry->m_iPK_Room);
		if( pRow_Room )
			return pRow_Room->Description_get();
	}
	else if( pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Hard_Drives_CONST )
	{
		string sDescription;
		if( pPnpQueueEntry->m_pRow_PnpQueue->Description_get().size() )
			sDescription = pPnpQueueEntry->m_pRow_PnpQueue->Description_get() + "-" + pPnpQueueEntry->m_pRow_Device_Reported->Description_get();
		else
		{
			sDescription = pRow_DeviceTemplate->Description_get() + "-" + pPnpQueueEntry->m_pRow_Device_Reported->Description_get();
			string sDevice = pPnpQueueEntry->m_mapPK_DeviceData_Find(DEVICEDATA_Block_Device_CONST);
			if( StringUtils::StartsWith(sDevice,"/dev/") )
				sDescription += " (" + sDevice.substr(5) + ")";
		}
		// The mount scripts don't allow these characters
		StringUtils::Replace(&sDescription,"/","-");
		StringUtils::Replace(&sDescription,"[","(");
		StringUtils::Replace(&sDescription,"]",")");
		return sDescription;
	}
	return pPnpQueueEntry->m_pRow_PnpQueue->Description_get();
}

void PnpQueue::SetDisableFlagForDeviceAndChildren(Row_Device *pRow_Device,bool bDisabled)
{
	pRow_Device->Disabled_set(bDisabled ? 1 : 0);

	// Be sure we update the main record for the router so it knows this is disabled
	DeviceData_Router *pDevice = m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceData_Router_Find( pRow_Device->PK_Device_get() );
	if( pDevice && pDevice->m_pRow_Device )
		pDevice->m_pRow_Device->Disabled_set(bDisabled ? 1 : 0);  // So the router also knows it's disabled


#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::SetDisableFlagForDeviceAndChildren Device %d is %d",
		pRow_Device->PK_Device_get(),(int) bDisabled);
#endif

	// If this is a usb->serial device, something else may get plugged in to the old port, so be sure
	// to clear out the serial number.  By default we'll do this for everything, if a device should
	// keep the serial number, set the DEVICEDATA_Keep_Serial_Number_On_Disable_CONST for the template to 1, 
	// but be sure nothing else will use the same serial number as happens a lot
	Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
	if( bDisabled && pRow_DeviceTemplate )
	{
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_Keep_Serial_Number_On_Disable_CONST);
		if( pRow_DeviceTemplate_DeviceData==NULL || atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str())==0 )
			DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Serial_Number_CONST,"",true);
		string sSQL = "UPDATE Device_DeviceData SET IK_DeviceData=NULL where FK_Device=" + StringUtils::itos(pRow_Device->PK_Device_get()) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_COM_Port_on_PC_CONST);
		m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL,true);
	}

	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
		SetDisableFlagForDeviceAndChildren(*it,bDisabled);
}

bool PnpQueue::ReenableDevice(PnpQueueEntry *pPnpQueueEntry,Row_Device *pRow_Device)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::ReenableDevice queue %d Device %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get());
#endif
	pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set( pRow_Device->PK_Device_get() );
	pPnpQueueEntry->RemoveBlockedDeviceData();
	pPnpQueueEntry->AssignDeviceData(pRow_Device);
	SetDisableFlagForDeviceAndChildren(pRow_Device,false);
	m_pDatabase_pluto_main->Device_get()->Commit();

	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP )
	{
		string sResponse;
		// Check for updates will also start any devices
		DCE::CMD_Enable_Device CMD_Enable_Device(m_pPlug_And_Play_Plugin->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device,pRow_Device->PK_Device_get(),0);
		if( !m_pPlug_And_Play_Plugin->SendCommand(CMD_Enable_Device,&sResponse) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueue::ReenableDevice queue %d Device %d can't send re-enable command",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get());
		}
	}

	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_ADD_SOFTWARE);

	DetermineOrbitersForPrompting(pPnpQueueEntry,false); // For the Display Alert
	string sMessage = StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()) + " Enabled existing device: " + pRow_Device->Description_get();
	if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty()==false )
	{
		DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts,
			sMessage,"pnp_enabled_" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()),"5",interuptAlways);
		m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::ReenableDevice queue %d was existing device, but disabled",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif

	int PK_Device = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
	m_pPlug_And_Play_Plugin->QueueMessageToRouter( 
		new Message(m_pPlug_And_Play_Plugin->m_dwPK_Device, PK_Device, PRIORITY_NORMAL, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_SPAWN_NEW_CHILDREN, 0) 
	);
	return Process_Detect_Stage_Add_Software(pPnpQueueEntry);
}

Row_Device *PnpQueue::FindDisabledDeviceTemplateOnPC(int PK_Device_PC,int PK_DeviceTemplate)
{
	int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,PK_Device_PC);
	if( !PK_Device_Topmost )
		return NULL;

	string sPK_Device = StringUtils::itos(PK_Device_Topmost);
	string sSQL = "SELECT Device.PK_Device FROM Device "
		"LEFT JOIN Device AS P1 on Device.FK_Device_ControlledVia=P1.PK_Device "
		"LEFT JOIN Device AS P2 on P1.FK_Device_ControlledVia=P2.PK_Device "
		"LEFT JOIN Device AS P3 on P2.FK_Device_ControlledVia=P3.PK_Device "
		"LEFT JOIN Device AS P4 on P3.FK_Device_ControlledVia=P4.PK_Device "
		"WHERE Device.FK_DeviceTemplate=" + StringUtils::itos(PK_DeviceTemplate) + 
		" AND Device.Disabled=1 "
		" AND (P1.PK_Device=" + sPK_Device + " OR P2.PK_Device=" + sPK_Device + 
		" OR P3.PK_Device=" + sPK_Device + " OR P4.PK_Device=" + sPK_Device + " OR P4.FK_Device_ControlledVia=" + sPK_Device + ")";

	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( ( result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result( sSQL ) )==0 || ( row = db_wrapper_fetch_row( result_set.r ) )==NULL || !row[0] )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "PnpQueue::FindDisabledDeviceTemplateOnPC %s returned nothing %p %p",sSQL.c_str(),row,row ? row[0] : NULL );
		return NULL;
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "PnpQueue::FindDisabledDeviceTemplateOnPC %s returned %s",sSQL.c_str(),row[0] );
	return m_pDatabase_pluto_main->Device_get()->GetRow(atoi(row[0]));
}

bool PnpQueue::LocateFileShare(PnpQueueEntry *pPnpQueueEntry)
{
	// File shares come from the parent device (the file server) and must have a unique serial number.  Also confirm it's the same type
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_Serial_Number_CONST)
		" WHERE FK_DeviceTemplate=" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get())
		+ " AND FK_Device_ControlledVia=" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Reported_get())
		+ " AND IK_DeviceData='" + StringUtils::SQLEscape(pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get()) + "'",
		&vectRow_Device);

	if( vectRow_Device.size() )
	{
		pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(vectRow_Device[0]->PK_Device_get());
		return true;
	}

	return false;
}

bool PnpQueue::LocateFileServer(PnpQueueEntry *pPnpQueueEntry)
{
	// The file server must be of the same type, with the same mac and ip.  The ip should never change because once it's added dhcp will allocate that ip
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("IPaddress='" + pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get() + "'"
		+ " AND MACaddress='" + pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() + "'",
		&vectRow_Device);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "PnpQueue::LocateFileServer queue %d records %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), (int) vectRow_Device.size() );

	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		Row_Device *pRow_Device = *it;
		if( pRow_Device->FK_DeviceTemplate_get()==pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "PnpQueue::LocateFileServer queue %d device template matches %d",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pRow_Device->PK_Device_get() );

			pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
			return true;
		}
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
		if( pRow_DeviceTemplate && (pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_FileMedia_Server_CONST || 
			pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Network_Storage_CONST ||
			pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST) )  // media directors are't file servers
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "PnpQueue::LocateFileServer queue %d device category matches %d",
				pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pRow_Device->PK_Device_get() );

			pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
			return true;
		}
	}

	return false;
}

void PnpQueue::DoneDetectingDevices(string sSignature)
{
	PLUTO_SAFETY_LOCK(pnp,m_pPlug_And_Play_Plugin->m_PnpMutex);

	// Find all devices using this signature that are not processed
	map<int,bool> mapDevices_Signature;
	string sSQL = "SELECT PK_Device FROM Device JOIN Device_DeviceData ON FK_Device=PK_Device WHERE Disabled=0 AND FK_DeviceData=" TOSTRING(DEVICEDATA_PNP_Signature_CONST) " AND IK_DeviceData='" + sSignature + "'";
	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( ( result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result( sSQL ) ) )
	{
		while( row = db_wrapper_fetch_row( result_set.r ) )
			mapDevices_Signature[ atoi(row[0]) ] = true;
	}

	int InitialCount = mapDevices_Signature.size();

	// Now mapDevices_Signature has all devices with sSignature.  See which have been detected so far
	for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();++it)  // The pnp mutex is held so we can safely do what we like
	{
		PnpQueueEntry *pPnpQueueEntry = it->second;

		// If we're blocked waiting for a device like HAL to startup, check if it's since finished registering
		if( (pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_waiting_for_initial_detection || pPnpQueueEntry->m_pRow_PnpQueue->Stage_get()==PNP_DETECT_STAGE_DETECTED)
			&& pPnpQueueEntry->m_pRow_PnpQueue->Signature_get()==sSignature )
		{
			LocateDevice(pPnpQueueEntry);
			if( pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() )
				mapDevices_Signature.erase(pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get());

			if( pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()<=m_pPlug_And_Play_Plugin->m_iPK_PnpQueue_Starting )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DoneDetectingDevices removing old queue %d for %s",
					pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sSignature.c_str());

#endif
				// This is an old entry using the same signature.  Get rid of it
				pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
			}
		}
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DoneDetectingDevices %d devices with signature %s, %d not found", 
		InitialCount, sSignature.c_str(), mapDevices_Signature.size());
#endif

	// So all that's left in mapDevices_Signature are ones that were reported previously and are no longer detected
	for(map<int,bool>::iterator it=mapDevices_Signature.begin();it!=mapDevices_Signature.end();++it)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DoneDetectingDevices disabling %d", it->first);
#endif
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(it->first);
		if( pRow_Device )
			SetDisableFlagForDeviceAndChildren(pRow_Device,true);
	}
	m_pDatabase_pluto_main->Device_get()->Commit();

	m_mapSignaturesReady[sSignature]=true;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueue::DoneDetectingDevices %s", sSignature.c_str());
	pnp.Release();
	pthread_cond_broadcast( &m_pPlug_And_Play_Plugin->m_PnpCond );
}

