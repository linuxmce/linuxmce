/*
 PostCreateOptions

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
#include "PlutoUtils/ProcessUtils.h"
#include "PostCreateOptions.h"
#include "DeviceData_Base.h"
#include "Gen_Devices/AllScreens.h"
#include "DCERouter.h"
#include "Command_Impl.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Table_Device_Device_Related.h"
#include "pluto_security/Define_AlertType.h"

PostCreateOptions::PostCreateOptions(Database_pluto_main *pDatabase_pluto_main,Router *pRouter,Command_Impl *pCommand_Impl)
{
	m_pCommand_Impl=pCommand_Impl;
	m_pDatabase_pluto_main=pDatabase_pluto_main;
	m_pRouter=pRouter;
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
}

void PostCreateOptions::PostCreateDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	DeviceCategory *pDeviceCategory = m_pRouter->m_mapDeviceCategory_Find(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
	if( !pDeviceCategory )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PostCreateOptions::PostCreateDevice category for Device %d is invalid",pRow_Device->PK_Device_get());
		return;
	}

	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Storage_Devices_CONST) )
		PostCreateDevice_NetworkStorage(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_FileMedia_Server_CONST) )
		PostCreateDevice_FileServer(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) )
		PostCreateDevice_Cameras(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Security_Device_CONST) )
		PostCreateSecurityDevice(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Media_Director_CONST) )
		PostCreateDevice_DisklessMD(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Mobile_Internet_Devices_CONST) )
		PostCreateDevice_DisklessMD(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Capture_Cards_CONST) )
		PostCreateDevice_CaptureCard(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Lighting_Interface_CONST) )
		PostCreateDevice_LightingInterface(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST) )
		PostCreateDevice_TV(pRow_Device,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST) )
		PostCreateDevice_Receiver(pRow_Device,pOH_Orbiter);
}

void PostCreateOptions::PostCreateDevice_AlarmPanel(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
}

void PostCreateOptions::PostCreateSecurityDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	int PK_FloorplanObjectType=0,PK_AlertType=0;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
	if( pRow_Device_DeviceData )
		PK_FloorplanObjectType = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());
	pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_EK_AlertType_CONST);
	if( pRow_Device_DeviceData )
		PK_AlertType = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());

	int iPK_DeviceTemplate=pRow_Device->FK_DeviceTemplate_get();
	string sDefaultSecuritySetting;
	if( PK_AlertType==ALERTTYPE_Security_CONST )
	{
		if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_DOOR_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Door_Sensor_CONST )
			sDefaultSecuritySetting = "1,0,N1,1,1,2,N1"; // Monitor mode, security alert when armed, announcement when entertaining
		else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_WINDOW_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Window_Sensor_CONST )
			sDefaultSecuritySetting = "0,0,N1,2,2,0,N1"; // Make an announcement when the user is at home, security breach when away
		else if( iPK_DeviceTemplate==DEVICETEMPLATE_Glass_Break_Sensor_CONST )
			sDefaultSecuritySetting = "0,2,N1,N1,N1,2,N1"; // Always announcement or trigger an alarm
		else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_MOTION_DETECTOR_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Motion_Detector_CONST )
			sDefaultSecuritySetting = "0,0,N1,0,0,0,N1"; // Only active when away
	}
	else if( PK_AlertType==ALERTTYPE_Fire_CONST && (PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_SMOKE_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Smoke_Detector_CONST) )
		sDefaultSecuritySetting = "1,N1,N1,N1,N1,N1,N1"; // Always an alarm
	else if( PK_AlertType==ALERTTYPE_Air_Quality_CONST && (PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_AIRQUALITY_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Air_Quality_Sensor_CONST) )
		sDefaultSecuritySetting = "1,N1,N1,N1,N1,N1,N1"; // Always an alarm

	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Alert_CONST,sDefaultSecuritySetting);
}

void PostCreateOptions::PostCreateDevice_FileServer(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	string sName = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Description_CONST);
	if( sName.empty()==false )
	{
		sName = pRow_Device->Description_get() + " - " + sName;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_NetworkStorage device  %d template %d new name: %s",
			pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get(),sName.c_str());

		pRow_Device->Description_set(sName);
		m_pDatabase_pluto_main->Device_get()->Commit();
	}
}

void PostCreateOptions::PostCreateDevice_NetworkStorage(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_NetworkStorage device  %d template %d",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get());
#endif

	Command_Impl *pCommand_Impl_GIP = m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP ) // Should always be there
	{
		DCE::CMD_Check_Mounts CMD_Check_Mounts(m_pCommand_Impl->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device);
		m_pCommand_Impl->SendCommand(CMD_Check_Mounts);
	}

	string sName = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Share_Name_CONST);
	if( sName.empty()==false )
	{
		pRow_Device->Description_set(pRow_Device->Description_get() + "-" + sName);
		m_pDatabase_pluto_main->Device_get()->Commit();
	}
}

void PostCreateOptions::PostCreateDevice_Cameras(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_Cameras device  %d template %d",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get());
#endif
}

void PostCreateOptions::PostCreateDevice_DisklessMD(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_DisklessMD device  %d template %d",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get());
#endif
	string sPK_Device = StringUtils::itos(pRow_Device->PK_Device_get());
	char * args[] = { (char *)("/usr/pluto/bin/New_PnP_MD.sh"), (char *)(pRow_Device->IPaddress_get().c_str()), (char *)(pRow_Device->MACaddress_get().c_str()),
		(char *)(sPK_Device.c_str()), NULL };
	ProcessUtils::SpawnDaemon(args[0], args);
}

void PostCreateOptions::PostCreateDevice_CaptureCard(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	int PK_Device_TopMost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
	DeviceData_Router *pDevice_PC = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_TopMost);
	DeviceData_Router *pDevice_Orbiter = pDevice_PC ? (DeviceData_Router *) pDevice_PC->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_OnScreen_Orbiter_CONST) : NULL;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_CaptureCard device  %d template %d top %d %p",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get(),PK_Device_TopMost,pDevice_Orbiter);
#endif

	string sUseAutomatically = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Use_Automatically_CONST);
	if( atoi(sUseAutomatically.c_str()) )
	{
		DCE::CMD_Sync_Providers_and_Cards_Cat CMD_Sync_Providers_and_Cards_Cat(m_pCommand_Impl->m_dwPK_Device,DEVICECATEGORY_Media_Player_Plugins_CONST,false,BL_SameHouse,0,pOH_Orbiter ? pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device : 0);
		m_pCommand_Impl->SendCommand(CMD_Sync_Providers_and_Cards_Cat);
		if( pDevice_Orbiter )
		{
			OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pDevice_Orbiter->m_dwPK_Device);
			if( pOH_Orbiter && pOH_Orbiter->m_bFirstRegen==false )  // Don't go to the provider screen if we're still setting it up
			{
				Sleep(2000);  // temporary hack to allow sync providers to send it's "this will take 10 minutes" before this goto screen so they come in the right order
				DCE::SCREEN_Choose_Provider_for_Device SCREEN_Choose_Provider_for_Device(m_pCommand_Impl->m_dwPK_Device,pDevice_Orbiter->m_dwPK_Device);
				m_pCommand_Impl->SendCommand(SCREEN_Choose_Provider_for_Device);
			}
		}
	}
}

void PostCreateOptions::PostCreateDevice_LightingInterface(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	int PK_Device_TopMost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
	DeviceData_Router *pDevice_PC = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_TopMost);
	DeviceData_Router *pDevice_Orbiter = pDevice_PC ? (DeviceData_Router *) pDevice_PC->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_OnScreen_Orbiter_CONST) : NULL;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_LightingInterface device  %d template %d top %d %p",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get(),PK_Device_TopMost,pDevice_Orbiter);
#endif

	if( pDevice_Orbiter )
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pDevice_Orbiter->m_dwPK_Device);
		if( pOH_Orbiter && pOH_Orbiter->m_bFirstRegen==false )  // Don't go to the screen if we're still setting it up
		{
			DCE::SCREEN_LightsSetup SCREEN_LightsSetup(m_pCommand_Impl->m_dwPK_Device,pDevice_Orbiter->m_dwPK_Device);
			m_pCommand_Impl->SendCommand(SCREEN_LightsSetup);
		}
	}
}

void PostCreateOptions::PostCreateDevice_TV(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	int PK_Device_TopMost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
	DeviceData_Router *pDevice_PC = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_TopMost);
	DeviceData_Router *pDevice_Orbiter = pDevice_PC ? (DeviceData_Router *) pDevice_PC->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_OnScreen_Orbiter_CONST) : NULL;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_TV device  %d template %d top %d %p",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get(),PK_Device_TopMost,pDevice_Orbiter);
#endif

	if( pDevice_Orbiter )
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pDevice_Orbiter->m_dwPK_Device);
		if( pOH_Orbiter && pOH_Orbiter->m_bFirstRegen==false )  // Don't go to the screen if we're still setting it up
		{
			DCE::SCREEN_TV_Manufacturer SCREEN_TV_Manufacturer(m_pCommand_Impl->m_dwPK_Device,pDevice_Orbiter->m_dwPK_Device);
			m_pCommand_Impl->SendCommand(SCREEN_TV_Manufacturer);
		}
	}
}

void PostCreateOptions::PostCreateDevice_Receiver(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter)
{
	int PK_Device_TopMost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
	DeviceData_Router *pDevice_PC = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_TopMost);
	DeviceData_Router *pDevice_Orbiter = pDevice_PC ? (DeviceData_Router *) pDevice_PC->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_OnScreen_Orbiter_CONST) : NULL;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PostCreateOptions::PostCreateDevice_Receiver device  %d template %d top %d %p",
		pRow_Device->PK_Device_get(),pRow_Device->FK_DeviceTemplate_get(),PK_Device_TopMost,pDevice_Orbiter);
#endif

	if( pDevice_Orbiter )
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pDevice_Orbiter->m_dwPK_Device);
		if( pOH_Orbiter && pOH_Orbiter->m_bFirstRegen==false )  // Don't go to the screen if we're still setting it up
		{
			DCE::SCREEN_Receiver SCREEN_Receiver(m_pCommand_Impl->m_dwPK_Device,pDevice_Orbiter->m_dwPK_Device);
			m_pCommand_Impl->SendCommand(SCREEN_Receiver);
		}
	}
}
