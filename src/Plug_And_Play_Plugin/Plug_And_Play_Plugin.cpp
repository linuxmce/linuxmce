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
//<-dceag-d-b->
#include "Plug_And_Play_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "PnpQueueEntry.h"
#include "PnpQueue.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DataGrid.h"
#include "PlutoUtils/DatabaseUtils.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Plug_And_Play_Plugin::Plug_And_Play_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Plug_And_Play_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_PnpMutex("pnp")
{
	m_pDatabase_pluto_main=NULL;
	m_pPnpQueue=NULL;
	m_bSuspendProcessing=false;

	pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_cond_init(&m_PnpCond, NULL);
	m_PnpMutex.Init(&m_MutexAttr,&m_PnpCond);
}

//<-dceag-dest-b->
Plug_And_Play_Plugin::~Plug_And_Play_Plugin()
//<-dceag-dest-e->
{
	delete m_pPnpQueue;
	delete m_pDatabase_pluto_main;
}


void Plug_And_Play_Plugin::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::PrepareToDelete waiting for thread");
	while( m_pPnpQueue && m_pPnpQueue->m_bThreadRunning )
	{
		pthread_cond_broadcast( &m_PnpCond );
		Sleep(50);
	}

	if(m_pPnpQueue->m_PnpQueueThread_Id)
		pthread_join(m_pPnpQueue->m_PnpQueueThread_Id, NULL);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::PrepareToDelete done");
}

//<-dceag-getconfig-b->
bool Plug_And_Play_Plugin::GetConfig()
{
	if( !Plug_And_Play_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit_set(true);
        return false;
    }

	string sSQL = "SELECT max(PK_PnpQueue) FROM PnpQueue";
	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( ( result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result( sSQL ) ) && ( row = db_wrapper_fetch_row( result_set.r ) ) && row[0] )
		m_iPK_PnpQueue_Starting = atoi( row[0] );
	else
		m_iPK_PnpQueue_Starting = 0;

	if( !DatabaseUtils::AlreadyHasRooms(m_pDatabase_pluto_main,m_pRouter->iPK_Installation_get()) || !DatabaseUtils::AlreadyHasUsers(m_pDatabase_pluto_main,m_pRouter->iPK_Installation_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::GetConfig System has no rooms or users yet.  Suspending processing.");
		m_bSuspendProcessing=true;
	}

	// Serial->usb adapters don't keep valid serial numbers in between reboots
	// Reset any serial numbers so the match will occur only on the usb bus
	sSQL="UPDATE Device_DeviceData "
		" JOIN Device ON FK_Device=PK_Device "
		" JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		" SET IK_DeviceData='' "
		" WHERE FK_DeviceData=" TOSTRING(DEVICEDATA_Serial_Number_CONST) " AND FK_CommMethod=" TOSTRING(COMMMETHOD_RS232_CONST);
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Plug_And_Play_Plugin::Register()
//<-dceag-reg-e->
{
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	if( !m_pOrbiter_Plugin || !m_pDatagrid_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to pnp plugin");
		return false;
	}

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Plug_And_Play_Plugin::DeviceDetected ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_Detected_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Plug_And_Play_Plugin::DeviceRemoved ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_Removed_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Plug_And_Play_Plugin::DoneDetectingDevices ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Done_Detecting_Devices_CONST );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&Plug_And_Play_Plugin::PNPDevices)), 
		DATAGRID_New_PNP_Devices_CONST,PK_DeviceTemplate_get());

	m_pPnpQueue = new PnpQueue(this);

	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Plug_And_Play_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Plug_And_Play_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

/*

	COMMANDS TO IMPLEMENT

*/

bool Plug_And_Play_Plugin::DeviceDetected( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PnpQueueEntry *pPnpQueueEntry = new PnpQueueEntry(this,
		m_pDatabase_pluto_main,
		pMessage->m_mapParameters[EVENTPARAMETER_DeviceData_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Category_CONST],
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_CommMethod_CONST].c_str()),
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceTemplate_CONST].c_str()),
		pMessage->m_dwPK_Device_From,
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_PnpProtocol_CONST].c_str()),
		pMessage->m_mapParameters[EVENTPARAMETER_PNP_Serial_Number_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_VendorModelID_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Signature_CONST]);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::DeviceDetected queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
// no, then auto-create are blocked?	m_pPnpQueue->DetermineOrbitersForPrompting(pPnpQueueEntry);
	m_pPnpQueue->NewEntry(pPnpQueueEntry);
	return false;
}

bool Plug_And_Play_Plugin::DoneDetectingDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sSignature = pMessage->m_mapParameters[EVENTPARAMETER_Signature_CONST];
	m_pPnpQueue->DoneDetectingDevices(sSignature);
	return false;
}

bool Plug_And_Play_Plugin::DeviceRemoved( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PnpQueueEntry *pPnpQueueEntry = new PnpQueueEntry(this,
		m_pDatabase_pluto_main,
		pMessage->m_mapParameters[EVENTPARAMETER_DeviceData_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST],
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_CommMethod_CONST].c_str()),
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str()),
		pMessage->m_dwPK_Device_From,
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceTemplate_CONST].c_str()),
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_PnpProtocol_CONST].c_str()),
		pMessage->m_mapParameters[EVENTPARAMETER_PNP_Serial_Number_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_VendorModelID_CONST]);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::DeviceRemoved %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
	m_pPnpQueue->NewEntry(pPnpQueueEntry);

	return false;
}

//<-dceag-const2-b->!
//<-dceag-createinst-b->!
//<-dceag-sample-b->!
//<-dceag-c700-b->

	/** @brief COMMAND: #700 - Choose Pnp Device Template */
	/** We have chosen a new pnp device template */
		/** @param #57 PK_Room */
			/** The room this is in.  0 if not known */
		/** @param #150 PK_DHCPDevice */
			/** The template for the device */
		/** @param #224 PK_PnpQueue */
			/** The queue entry we're selecting for */

void Plug_And_Play_Plugin::CMD_Choose_Pnp_Device_Template(int iPK_Room,int iPK_DHCPDevice,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c700-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PnpQueue::PickDeviceTemplate queue %d is invalid", iPK_PnpQueue);
		return;
	}

	pPnpQueueEntry->m_pOH_Orbiter_Active_set(pMessage->m_dwPK_Device_From);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::CMD_Choose_Pnp_Device_Template queue %d set to orbiter %p/%d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pPnpQueueEntry->m_pOH_Orbiter_Active_get(), pMessage->m_dwPK_Device_From);
	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	if( iPK_DHCPDevice )
	{
		pPnpQueueEntry->m_iPK_DHCPDevice = iPK_DHCPDevice;
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
			m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NewPnpDevice_CONST);
		DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL2(
			m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_New_Pnp_Device_One_Possibility_CONST);
		CMD_Remove_Screen_From_History_DL.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Screen_From_History_DL2.m_pMessage);
		SendCommand(CMD_Remove_Screen_From_History_DL);
	}
	if( iPK_Room )
		pPnpQueueEntry->m_iPK_Room = iPK_Room;
	pthread_cond_broadcast( &m_PnpCond );
}

//<-dceag-c804-b->

	/** @brief COMMAND: #804 - Set Pnp Options */
	/** Set options for a pnp device that will soon be created */
		/** @param #5 Value To Assign */
			/** The value to assign to it */
		/** @param #52 PK_DeviceData */
			/** The device data to assign */
		/** @param #224 PK_PnpQueue */
			/** The pnp entry */

void Plug_And_Play_Plugin::CMD_Set_Pnp_Options(string sValue_To_Assign,int iPK_DeviceData,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c804-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PnpQueue::CMD_Set_Pnp_Options queue %d is invalid", iPK_PnpQueue);
		return;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueue::CMD_Set_Pnp_Options queue %d option %d set to %s", iPK_PnpQueue,iPK_DeviceData,sValue_To_Assign.c_str());
	pPnpQueueEntry->m_pOH_Orbiter_Active_set(pMessage->m_dwPK_Device_From);
	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	pPnpQueueEntry->m_mapPK_DeviceData[iPK_DeviceData] = sValue_To_Assign;
	string s = pPnpQueueEntry->m_pRow_PnpQueue->Parms_get();
	if( s.empty()==false && s[ s.size()-1 ] != '|' )
		s += "|";

	pPnpQueueEntry->m_pRow_PnpQueue->Parms_set( s + StringUtils::itos(iPK_DeviceData) + "|" + sValue_To_Assign );
	pPnpQueueEntry->m_pRow_PnpQueue->Table_PnpQueue_get()->Commit();
	pthread_cond_broadcast( &m_PnpCond );
}

class DataGridTable *Plug_And_Play_Plugin::PNPDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{	
	DataGridTable *pDataGrid = new DataGridTable( );
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(atoi(Parms.c_str()));
	if( !pPnpQueueEntry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Plug_And_Play_Plugin::PNPDevices Parms=%s is invalid", Parms.c_str());
		return pDataGrid;
	}

	int RowCount=0;
	DataGridCell *pCell;
	Row_DeviceTemplate *pRow_DeviceTemplate;
	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() && (pRow_DeviceTemplate=pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow()) )
	{
		pCell = new DataGridCell( pRow_DeviceTemplate->Description_get(), "-1" );
        pDataGrid->SetData( 0, 0, pCell );
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Plug_And_Play_Plugin::PNPDevices Parms=%s returning sole device template %d %s", Parms.c_str(),pRow_DeviceTemplate->PK_DeviceTemplate_get(),pRow_DeviceTemplate->Description_get().c_str());
		return pDataGrid;
	}

	string sPK_DeviceTemplate;
	for( map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();++it )
	{
		Row_DHCPDevice *pRow_DHCPDevice = it->second;
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_DHCPDevice->FK_DeviceTemplate_getrow();
		if( !pRow_DeviceTemplate )
			continue;  // Shouldn't happen

		pCell = new DataGridCell( pRow_DeviceTemplate->Description_get(), StringUtils::itos(pRow_DHCPDevice->PK_DHCPDevice_get()) );

		sPK_DeviceTemplate += StringUtils::itos(pRow_DHCPDevice->PK_DHCPDevice_get()) + "," + pRow_DeviceTemplate->Description_get() + " / ";
        pDataGrid->SetData( 0, RowCount++, pCell );
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::PNPDevices Parms=%s returning templates %s", Parms.c_str(), sPK_DeviceTemplate.c_str());
	return pDataGrid;
}

//<-dceag-c805-b->

	/** @brief COMMAND: #805 - Ignore PNP Device */
	/** Ignore a device detected */
		/** @param #224 PK_PnpQueue */
			/** The entry in the pnp queue */
		/** @param #225 Always */
			/** If true, blacklist this device in the 'unknown devices' list so we never ask again */

void Plug_And_Play_Plugin::CMD_Ignore_PNP_Device(int iPK_PnpQueue,bool bAlways,string &sCMD_Result,Message *pMessage)
//<-dceag-c805-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Plug_And_Play_Plugin::CMD_Ignore_PNP_Device queue %d is invalid", iPK_PnpQueue);
		return;
	}
	pPnpQueueEntry->m_pOH_Orbiter_Active_set(pMessage->m_dwPK_Device_From);
	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	pPnpQueueEntry->Stage_set( PNP_DETECT_STAGE_DONE );

	if( bAlways )
	{
		Row_UnknownDevices *pRow_UnknownDevices = m_pDatabase_pluto_main->UnknownDevices_get()->AddRow();
		pRow_UnknownDevices->MacAddress_set( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() );
		pRow_UnknownDevices->IPAddress_set( pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get() );
		pRow_UnknownDevices->FK_Device_PC_set( pPnpQueueEntry->m_dwPK_Device_TopLevel );
		if( pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get().size() )
			pRow_UnknownDevices->VendorModelId_set( pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() );
		else if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
			pRow_UnknownDevices->VendorModelId_set( "DT:" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get()) );

		if( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().size() )
			pRow_UnknownDevices->SerialNumber_set( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get() );
		else
			pRow_UnknownDevices->SerialNumber_set( pPnpQueueEntry->m_pRow_PnpQueue->Parms_get() );

		pRow_UnknownDevices->FK_Device_PC_set( pPnpQueueEntry->m_dwPK_Device_TopLevel );
		m_pDatabase_pluto_main->UnknownDevices_get()->Commit();
	}

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NewPnpDevice_CONST);
	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL2(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_New_Pnp_Device_One_Possibility_CONST);
	CMD_Remove_Screen_From_History_DL.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Screen_From_History_DL2.m_pMessage);
    SendCommand(CMD_Remove_Screen_From_History_DL);

	pthread_cond_broadcast( &m_PnpCond );
}
//<-dceag-c806-b->

	/** @brief COMMAND: #806 - PNP Detection Script Finished */
	/** A PNP Detection script has finished running */
		/** @param #13 Filename */
			/** The name of the PNP Detection Script */
		/** @param #18 Errors */
			/** If not empty, there were errors.  These will be logged. */
		/** @param #44 PK_DeviceTemplate */
			/** The device template of the device.  0 means this script couldn't identify the device */
		/** @param #109 Data String */
			/** The device data to be assigned when creating the device
PK_DeviceData\tIK_DeviceData\n
.... */
		/** @param #224 PK_PnpQueue */
			/** The queue entry */

void Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished(string sFilename,string sErrors,int iPK_DeviceTemplate,string sData_String,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c806-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d is invalid", iPK_PnpQueue);
		return;
	}

string sFilesInQueue;
for(map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();++it)
{
	Row_DHCPDevice *pRow_DHCPDevice = it->second;
	sFilesInQueue+=pRow_DHCPDevice->PnpDetectionScript_get() + "/";
}
LoggerWrapper::GetInstance()->Write(LV_WARNING, "Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d got %s %d script: %s (NOW RUNNING %s).  files %d %s", 
iPK_PnpQueue,sErrors.c_str(),iPK_DeviceTemplate,sFilename.c_str(),pPnpQueueEntry->m_sDetectionScript_Running.c_str(),pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size(),sFilesInQueue.c_str());

	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )  // This could just be confirming after it was assigned
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d already assigned", iPK_PnpQueue);  
		return;
	}

	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	pthread_cond_broadcast( &m_PnpCond );  // We got the mutex, it won't run until we're done anyway
	if( iPK_DeviceTemplate )  // Great, we know what it is
	{
		pPnpQueueEntry->m_sDetectionScript_Running="";  // This isn't running anymore
		pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_set(iPK_DeviceTemplate);
		pPnpQueueEntry->ParseDeviceData(sData_String);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d prompting user", iPK_PnpQueue);
		Row_Device *pRow_Device = m_pPnpQueue->FindDisabledDeviceTemplateOnPC(pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),iPK_DeviceTemplate);
		if( pRow_Device )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d re-enabling %d", iPK_PnpQueue,pRow_Device->PK_Device_get());
			m_pPnpQueue->ReenableDevice(pPnpQueueEntry,pRow_Device);
			return; 
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT);
		return; 
	}

	// It's either an error, a no match, or just letting us know that the script should have exited by now
	for(map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();)
	{
		Row_DHCPDevice *pRow_DHCPDevice = it->second;
		if( sFilename==pRow_DHCPDevice->PnpDetectionScript_get() )
		{
			string sMessage;
			if( sErrors=="OK" ) // This means the script finished running.  If so, it should have reported either an error, a device template, or a no match.  It shouldn't still be in the queue
				sMessage = "**still in queue**";
			else if( sErrors=="FAIL" )
				sMessage = "**couldn't run script**";
			else if( sErrors.size() )
				sMessage = "**script reported: " + sErrors + "**";

			if( sMessage.size() )  // If it's empty, this isn't an error.  Just a normal 'this isn't the right device' message.  We'll wake up the process later and check the next one
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d script %s PK_DHCPDevice %d %s",
					pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(), pPnpQueueEntry->m_sDetectionScript_Running.c_str(), pRow_DHCPDevice->PK_DHCPDevice_get(),sMessage.c_str());

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::CMD_PNP_Detection_Script_Finished queue %d erasing possible template %d (%d)", iPK_PnpQueue, pRow_DHCPDevice->FK_DeviceTemplate_get(),pRow_DHCPDevice->PK_DHCPDevice_get());
			pPnpQueueEntry->m_mapPK_DHCPDevice_possible.erase(it++);
			continue;
		}
		++it;
	}
}
//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Start processing any incoming alerts again */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void Plug_And_Play_Plugin::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::CMD_On with %d entries",(int) m_pPnpQueue->m_mapPnpQueueEntry.size());
	m_bSuspendProcessing=false;
	for(map<int,class PnpQueueEntry *>::iterator it=m_pPnpQueue->m_mapPnpQueueEntry.begin();it!=m_pPnpQueue->m_mapPnpQueueEntry.end();++it)
	{
		PnpQueueEntry *pPnpQueueEntry = it->second;
		if( pPnpQueueEntry->m_EBlockedState==PnpQueueEntry::pnpqe_block_processing_suspended )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug_And_Play_Plugin::CMD_On unblocking %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
		}
	}
	pthread_cond_broadcast( &m_PnpCond );
}
//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Stop processing incoming pnp alerts unless they have 'auto create without prompting'.  Used during the video wizard when there are no rooms/users */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Plug_And_Play_Plugin::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	m_bSuspendProcessing=true;
}
//<-dceag-c822-b->

	/** @brief COMMAND: #822 - Pre Pnp Script Done */
	/** The script that runs before prompting the user if he wants to create the device has finished.  This is in the 'Pre Pnp Script' parameter. */
		/** @param #109 Data String */
			/** The device data to be assigned when creating the device
PK_DeviceData\tIK_DeviceData\n
.... */
		/** @param #119 True/False */
			/** If false, don't bother trying to create the device */
		/** @param #224 PK_PnpQueue */
			/** the queue id */

void Plug_And_Play_Plugin::CMD_Pre_Pnp_Script_Done(string sData_String,bool bTrueFalse,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c822-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Plug_And_Play_Plugin::CMD_Pre_Pnp_Script_Done queue %d is invalid", iPK_PnpQueue);
		return;
	}

	pthread_cond_broadcast( &m_PnpCond );  // We got the mutex, it won't run until we're done anyway
	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	if( !bTrueFalse )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Plug_And_Play_Plugin::CMD_Pre_Pnp_Script_Done queue %d done", pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
		return;
	}

	string s = pPnpQueueEntry->m_pRow_PnpQueue->Parms_get();
	if( s.empty()==false && s[ s.size()-1 ] != '|' )
		s += "|";

	pPnpQueueEntry->m_pRow_PnpQueue->Parms_set( s + sData_String );
	pPnpQueueEntry->m_pRow_PnpQueue->Table_PnpQueue_get()->Commit();

	pPnpQueueEntry->ParseDeviceData(sData_String);
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT);
}
