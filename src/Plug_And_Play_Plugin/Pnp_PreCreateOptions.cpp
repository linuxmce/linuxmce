/*
 Pnp_PreCreateOptions

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "Pnp_PreCreateOptions.h"
#include "PnpQueue.h"
#include "PnpQueueEntry.h"
#include "Plug_And_Play_Plugin.h"
#include "DeviceData_Base.h"
#include "Gen_Devices/AllScreens.h"
#include "DCERouter.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Screen.h"

using namespace DCE;

Pnp_PreCreateOptions::Pnp_PreCreateOptions(class PnpQueue *pPnpQueue,class Plug_And_Play_Plugin *pPlug_And_Play_Plugin)
{
	m_pPnpQueue=pPnpQueue;
	m_pDatabase_pluto_main=pPlug_And_Play_Plugin->m_pDatabase_pluto_main;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice(PnpQueueEntry *pPnpQueueEntry)
{
	// See if there is anything special we need to do for this type of device
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get());
	if( !pRow_DeviceTemplate ) // Shouldn't happen
		return true;

	DeviceCategory *pDeviceCategory = m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pRouter->m_mapDeviceCategory_Find(pRow_DeviceTemplate->FK_DeviceCategory_get());
	if( !pDeviceCategory )
		return true;

	if( pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts.empty() )
		m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,false);  // Don't block since we may not need to ask anyway
	
	if( !OkayToCreateDevice_Username(pPnpQueueEntry,pRow_DeviceTemplate) )  // It needs a username
		return false;
	if( !OkayToCreateDevice_Room(pPnpQueueEntry,pRow_DeviceTemplate) )  // It needs a room
		return false;
	if( !OkayToCreateDevice_Presets(pPnpQueueEntry,pRow_DeviceTemplate) )  // There are some pre-defined values we need to specify in device data first
		return false;
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Storage_Devices_CONST) )
		return OkayToCreateDevice_NetworkStorage(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) )
		return OkayToCreate_Cameras(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Mobile_Orbiter_CONST) )
		return OkayToCreate_MobilePhone(pPnpQueueEntry,pRow_DeviceTemplate);
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Capture_Cards_CONST) )
		return OkayToCreate_CaptureCard(pPnpQueueEntry,pRow_DeviceTemplate);

	return true;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_Presets(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_PNP_Prompt_For_Options_CONST);
	if( pRow_DeviceTemplate_DeviceData==NULL || pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().empty() )
		return true; // Nothing special to prompt for

	// The format is: 
	// PK_DeviceData\tPK_Text for question\tPK_Text for Answer 1\tValue for answer 1\tPK_Text for Answer 2 ....   \n
	// PK_DeviceData ....

	vector<string> vectValues;
	string s=pRow_DeviceTemplate_DeviceData->IK_DeviceData_get();
	StringUtils::Tokenize(s,"\n",vectValues);
	for(vector<string>::iterator it=vectValues.begin();it!=vectValues.end();++it)
	{
		int PK_DeviceData = atoi( it->c_str() );
		if( !PK_DeviceData )
			continue; // Shouldn't happen

		if( pPnpQueueEntry->m_mapPK_DeviceData.find(PK_DeviceData)!=pPnpQueueEntry->m_mapPK_DeviceData.end() )
			continue; // the user already specified this

		if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
			return false; // No orbiters.  Skip this one for now

		if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
			return false; // Let this one get backed up

		if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
			pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters

		if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_Presets blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
			return false; // We're waiting for the user to complete the setup wizard
		}

		// We need to ask the user for this info
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
		if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
		{
			DCE::SCREEN_PNP_Generic_Options SCREEN_PNP_Generic_Options(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device, 
				s,pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_PNP_Generic_Options);
		}
		else
		{
			DCE::SCREEN_PNP_Generic_Options_DL SCREEN_PNP_Generic_Options_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
				s,pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_PNP_Generic_Options_DL);
		}

		return false;  
	}

	return true;
}
bool Pnp_PreCreateOptions::OkayToCreateDevice_Username(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	// See if a password is required
	map<int,string>::iterator it;
	if( (it=pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Password_Required_CONST))!=pPnpQueueEntry->m_mapPK_DeviceData.end() &&
		atoi(it->second.c_str())==1 )
	{
		bool bHasUserName = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Username_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
		// It is, and it's not specified
		if( bHasUserName==false ||
			pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Password_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() )
		{
			if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
				return false; // No orbiters.  Skip this one for now

			if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
				return false; // Let this one get backed up

			if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_Username blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
				return false; // We're waiting for the user to complete the setup wizard
			}	

			if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
				pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters

			// We need to get it and block until we do
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

			if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
			{
				DCE::SCREEN_Get_Username_Password_For_Devices SCREEN_Get_Username_Password_For_Devices(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device, 
					bHasUserName,pRow_DeviceTemplate->Description_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Get_Username_Password_For_Devices);
			}
			else
			{
				DCE::SCREEN_Get_Username_Password_For_Devices_DL SCREEN_Get_Username_Password_For_Devices_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
					bHasUserName,pRow_DeviceTemplate->Description_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
				m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Get_Username_Password_For_Devices_DL);
			}
			return false;
		}
	}
	return true;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_Room(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	if( pPnpQueueEntry->m_iPK_Room!=-1 )
		return true; // The user already specified this

	if( pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="mobile_phone" )
		return true; // Mobile phones float around the house anyway

	Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_Autoassign_to_parents_room_CONST);
	if( pRow_DeviceTemplate_DeviceData )
	{
		int Value = atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str());
		if( Value==-1 )
			pPnpQueueEntry->m_iPK_Room = 0;  // Means don't put it in any room
		return true; // We will automatically pick the room in create device
	}
	
	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up

	if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_Room blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
		return false; // We're waiting for the user to complete the setup wizard
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters

	// We need to ask the user for the room
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
	if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
	{
		DCE::SCREEN_Pick_Room_For_Device SCREEN_Pick_Room_For_Device(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get() * -1, pRow_DeviceTemplate->Description_get(),pRow_DeviceTemplate->Comments_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Pick_Room_For_Device);
	}
	else
	{
		DCE::SCREEN_Pick_Room_For_Device_DL SCREEN_Pick_Room_For_Device_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get() * -1, pRow_DeviceTemplate->Description_get(),pRow_DeviceTemplate->Comments_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Pick_Room_For_Device_DL);
	}

	return false;  
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_AlarmPanel(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	return true;
}

bool Pnp_PreCreateOptions::OkayToCreate_MobilePhone(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	// The user must answer 2 questions before we create a network storage device: 1) use it automatically, 2) use our directory structure.  See which questions were answered
	bool bUserSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_PK_Users_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
	bool bPhoneSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Mobile_Orbiter_Phone_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_MobilePhone %d/%d",(int) bUserSpecified,(int) bPhoneSpecified);
#endif

	if( bUserSpecified && bPhoneSpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,
			 StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NewPhoneDetected_CONST);

		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified both options
	}

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up

	if( bUserSpecified && !bPhoneSpecified && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
	{
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
		return false;  // The user specified only 1 option.  Give him TIMEOUT_PROMPTING_USER seconds to specify the other
	}

	if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_MobilePhone blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
		return false; // We're waiting for the user to complete the setup wizard
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters

	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
	{
		DCE::SCREEN_NewPhoneDetected SCREEN_NewPhoneDetected(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(),m_pPnpQueue->GetDescription(pPnpQueueEntry),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NewPhoneDetected);
	}
	else
	{
		DCE::SCREEN_NewPhoneDetected_DL SCREEN_NewPhoneDetected_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(),m_pPnpQueue->GetDescription(pPnpQueueEntry),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NewPhoneDetected_DL);
	}
	return false;
}

bool Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	string sUUID = pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_UUID_CONST];

	//check the UUID for non-network storage devices 
	if(pRow_DeviceTemplate->FK_DeviceCategory_get() != DEVICECATEGORY_Network_Storage_CONST && sUUID.empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage queue %d has no uuid",
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());

		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
		return false;
	}

	// The user must answer 2 questions before we create a network storage device: 1) use it automatically, 2) use our directory structure.  See which questions were answered
	bool bUseAutoSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Use_Automatically_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();
	bool bDirectoryStructSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_PK_Users_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage %d/%d",(int) bUseAutoSpecified,(int) bDirectoryStructSpecified);
#endif
	if( bUseAutoSpecified && bDirectoryStructSpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NAS_Options_CONST);
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified both options
	}

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up

	if( bUseAutoSpecified && !bDirectoryStructSpecified && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
	{
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
		return false;  // The user specified only 1 option.  Give him TIMEOUT_PROMPTING_USER seconds to specify the other
	}

	if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreateDevice_NetworkStorage blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
		return false; // We're waiting for the user to complete the setup wizard
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters
	
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
	{
		DCE::SCREEN_NAS_Options SCREEN_NAS_Options(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NAS_Options);
	}
	else
	{
		DCE::SCREEN_NAS_Options_DL SCREEN_NAS_Options_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NAS_Options_DL);
	}
	return false;
}

bool Pnp_PreCreateOptions::OkayToCreate_Cameras(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_Cameras queue %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	string sSqlSensors = "SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="
		+ StringUtils::itos(DEVICECATEGORY_Security_Device_CONST) + " LIMIT 1";

	string sSqlLights = "SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="
		+ StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST) + " LIMIT 1";

	PlutoSqlResult result_set_sensors,result_set_lights;
	bool bHasSensors = ( (result_set_sensors.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSqlSensors)) && result_set_sensors.r->row_count );
	bool bHasLights = ( (result_set_lights.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSqlLights)) && result_set_lights.r->row_count );
	if( !bHasSensors && !bHasLights )
		return true;

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up

	if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_Cameras blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
		return false; // We're waiting for the user to complete the setup wizard
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters

	// Be sure the user specified the related cameras and lights
	if( pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_sPK_Device_Relations_For_Creat_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() )
	{
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);
		string sOptions = (bHasSensors && bHasLights ? "3" : (bHasLights ? "1" : "2"));
		if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
		{
			DCE::SCREEN_Sensors_Viewed_By_Camera SCREEN_Sensors_Viewed_By_Camera(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device, pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device,
				sOptions,pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Sensors_Viewed_By_Camera);
		}
		else
		{
			DCE::SCREEN_Sensors_Viewed_By_Camera_DL SCREEN_Sensors_Viewed_By_Camera_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device, m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,
				sOptions,pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_Sensors_Viewed_By_Camera_DL);
		}
		return false;
	}

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_Sensors_Viewed_By_Camera_CONST);
	m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);

	return true;
}

bool Pnp_PreCreateOptions::OkayToCreate_CaptureCard(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate)
{
	DeviceData_Base *pDevice = m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get());
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_CaptureCard queue %d 1 %p %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pDevice,pDevice ? pDevice->m_dwPK_Device : -1);
	if( pDevice )
	{
		DeviceData_Base *pDevice_OSD = pDevice->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_OnScreen_Orbiter_CONST);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_CaptureCard queue %d 2 %p %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pDevice_OSD,pDevice_OSD ? pDevice_OSD->m_dwPK_Device : -1);
		if( pDevice_OSD )
		{
			OH_Orbiter *pOH_Orbiter = m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pDevice_OSD->m_dwPK_Device);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_CaptureCard 3 queue %d %p %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pOH_Orbiter,pOH_Orbiter ? (int) pOH_Orbiter->m_bFirstRegen : -1);
			if( !pOH_Orbiter || pOH_Orbiter->m_bFirstRegen==true )
				return true;
		}
	}
	bool bUseAutoSpecified = pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_Use_Automatically_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_CaptureCard 4 queue %d use %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) bUseAutoSpecified);
	if( bUseAutoSpecified )
	{
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			 m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,  m_pPnpQueue->m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, 
			 StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_AutoConfigure_TV_CONST);
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(CMD_Remove_Screen_From_History_DL);
		return true;  // The user specified this option
	}

	if( m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry,true)==false )
		return false; // No orbiters.  Skip this one for now

	if( m_pPnpQueue->BlockIfOtherQueuesAtPromptingState(pPnpQueueEntry) )
		return false; // Let this one get backed up

	if( m_pPnpQueue->m_pPlug_And_Play_Plugin->m_bSuspendProcessing )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pnp_PreCreateOptions::OkayToCreate_CaptureCard blocking queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_block_processing_suspended);
		return false; // We're waiting for the user to complete the setup wizard
	}

	if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_blocked_prompting_options )
		pPnpQueueEntry->UseAllOrbitersForPrompt();  // The user isn't responding.  Ask on all orbiters
	
	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_options);

	if( pPnpQueueEntry->m_pOH_Orbiter_Active_get() )
	{
		DCE::SCREEN_AutoConfigure_TV SCREEN_AutoConfigure_TV(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pOH_Orbiter_Active_get()->m_pDeviceData_Router->m_dwPK_Device, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_AutoConfigure_TV);
	}
	else
	{
		DCE::SCREEN_AutoConfigure_TV_DL SCREEN_AutoConfigure_TV_DL(m_pPnpQueue->m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_sPK_Orbiter_List_For_Prompts, 
			pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		m_pPnpQueue->m_pPlug_And_Play_Plugin->SendCommand(SCREEN_AutoConfigure_TV_DL);
	}
	return false;
}
