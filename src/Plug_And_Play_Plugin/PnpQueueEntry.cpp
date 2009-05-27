/*
 PnpQueueEntry

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
#include "PnpQueueEntry.h"
#include "PnpQueue.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "Plug_And_Play_Plugin.h"
#include "Gen_Devices/AllScreens.h"
#include "Orbiter_Plugin/OH_Orbiter.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"

using namespace DCE;

// Constructor for device detected
PnpQueueEntry::PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,
	Database_pluto_main *pDatabase_pluto_main,
	string sDeviceData,
	string sIPAddress,
	string sMacAddress,
	string sCategory,
	int PK_CommMethod,
	int PK_DeviceTemplate,
	int PK_Device_Reported,
	int PK_PnpProtocol,
	string sPnpSerialNumber,
	string sText,
	string sVendorModelId,
	string sSignature)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pDatabase_pluto_main=pDatabase_pluto_main;
	m_pRow_PnpQueue = pDatabase_pluto_main->PnpQueue_get()->AddRow();
	m_pRow_PnpQueue->DetectedDate_set(StringUtils::SQLDateTime(time(NULL)));
	m_pRow_PnpQueue->IPaddress_set(sIPAddress);
	m_pRow_PnpQueue->MACaddress_set(sMacAddress);
	m_pRow_PnpQueue->Category_set(sCategory);
	m_pRow_PnpQueue->VendorModelId_set(sVendorModelId);
	m_pRow_PnpQueue->Path_set("");
	m_pRow_PnpQueue->Description_set(sText);
	m_pRow_PnpQueue->SerialNumber_set(sPnpSerialNumber);
	m_pRow_PnpQueue->FK_CommMethod_set(PK_CommMethod);
	m_pRow_PnpQueue->FK_PnpProtocol_set(PK_PnpProtocol);
	m_pRow_PnpQueue->FK_DeviceTemplate_set(PK_DeviceTemplate);
	m_pRow_PnpQueue->FK_Device_Created_set(0);
	m_pRow_PnpQueue->FK_Device_Reported_set(PK_Device_Reported);
	m_pRow_PnpQueue->Removed_set(0);
	m_pRow_PnpQueue->Stage_set(PNP_DETECT_STAGE_DETECTED);
	m_pRow_PnpQueue->Parms_set(sDeviceData);
	m_pRow_PnpQueue->Signature_set(sSignature);
	m_pRow_PnpQueue->Processed_set(0);
	pDatabase_pluto_main->PnpQueue_get()->Commit();
	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_dwPK_PnpQueue_BlockingFor=m_iPK_DHCPDevice=0;
	m_iPK_Room=-1;  // This means auto for create device
	m_pOH_Orbiter_Active=NULL;
	m_bUseAllOrbitersForPrompt=m_bCreateWithoutPrompting=false;
	ParseDeviceData(sDeviceData);
	FindTopLevelDevice();
}

// Constructor for device removed
PnpQueueEntry::PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,
	Database_pluto_main *pDatabase_pluto_main,
	string sDeviceData,
	string sIPAddress,
	string sMacAddress,
	int PK_CommMethod,
	int PK_Device_Created,
	int PK_Device_Reported,
	int PK_DeviceTemplate,
	int PK_PnpProtocol,
	string sPnpSerialNumber,
	string sText,
	string sVendorModelId)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pDatabase_pluto_main=pDatabase_pluto_main;
	m_pRow_PnpQueue = pDatabase_pluto_main->PnpQueue_get()->AddRow();
	m_pRow_PnpQueue->DetectedDate_set(StringUtils::SQLDateTime(time(NULL)));
	m_pRow_PnpQueue->IPaddress_set(sIPAddress);
	m_pRow_PnpQueue->MACaddress_set(sMacAddress);
	m_pRow_PnpQueue->VendorModelId_set(sVendorModelId);
	m_pRow_PnpQueue->Path_set("");
	m_pRow_PnpQueue->Description_set(sText);
	m_pRow_PnpQueue->SerialNumber_set(sPnpSerialNumber);
	m_pRow_PnpQueue->FK_CommMethod_set(PK_CommMethod);
	m_pRow_PnpQueue->FK_PnpProtocol_set(PK_PnpProtocol);
	m_pRow_PnpQueue->FK_DeviceTemplate_set(PK_DeviceTemplate);
	m_pRow_PnpQueue->FK_Device_Created_set(PK_Device_Created);
	m_pRow_PnpQueue->FK_Device_Reported_set(PK_Device_Reported);
	m_pRow_PnpQueue->Removed_set(1);
	m_pRow_PnpQueue->Stage_set(PNP_REMOVE_STAGE_REMOVED);
	m_pRow_PnpQueue->Parms_set(sDeviceData);
	m_pRow_PnpQueue->Processed_set(0);
	pDatabase_pluto_main->PnpQueue_get()->Commit();
	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_dwPK_PnpQueue_BlockingFor=m_iPK_DHCPDevice=0;
	m_iPK_Room=-1;  // This means auto for create device
	m_pOH_Orbiter_Active=NULL;
	m_bUseAllOrbitersForPrompt=m_bCreateWithoutPrompting=false;
	ParseDeviceData(sDeviceData);
	FindTopLevelDevice();
}

PnpQueueEntry::PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,Row_PnpQueue *pRow_PnpQueue)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pRow_PnpQueue=pRow_PnpQueue;
	m_pDatabase_pluto_main = m_pRow_PnpQueue->Table_PnpQueue_get()->Database_pluto_main_get();

	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_dwPK_PnpQueue_BlockingFor=m_iPK_DHCPDevice=0;
	m_iPK_Room=-1;  // This means auto for create device
	m_pOH_Orbiter_Active=NULL;
	m_bUseAllOrbitersForPrompt=m_bCreateWithoutPrompting=false;
	ParseDeviceData(m_pRow_PnpQueue->Parms_get());
	FindTopLevelDevice();

	m_pRow_PnpQueue->Stage_set( m_pRow_PnpQueue->Removed_get()==1 ? PNP_REMOVE_STAGE_REMOVED : PNP_DETECT_STAGE_DETECTED );
}

void PnpQueueEntry::Stage_set(int Stage)
{ 
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueueEntry::Stage_set queue %d now set to %d", m_pRow_PnpQueue->PK_PnpQueue_get(),Stage);
#endif
	m_pRow_PnpQueue->Stage_set(Stage); 
	if( Stage==PNP_DETECT_STAGE_DONE || Stage==PNP_REMOVE_STAGE_DONE )
		m_pRow_PnpQueue->Processed_set(1);
	m_pDatabase_pluto_main->PnpQueue_get()->Commit();

	if( m_pRow_Device_Reported )
	{
		string sMessage = StringUtils::itos(m_pRow_PnpQueue->PK_PnpQueue_get()) + " " + StageAsText();
		if( m_pRow_PnpQueue->Category_get().size() )
			sMessage += " " + m_pRow_PnpQueue->Category_get();

		Row_DeviceTemplate *pRow_DeviceTemplate;
		Row_Device *pRow_Device;
		if( m_pRow_PnpQueue->FK_Device_Created_get() && (pRow_Device=m_pRow_PnpQueue->FK_Device_Created_getrow()) )
			sMessage += " Added: " + pRow_Device->Description_get();
		else if( m_pRow_PnpQueue->FK_DeviceTemplate_get() && (pRow_DeviceTemplate=m_pRow_PnpQueue->FK_DeviceTemplate_getrow()) )
			sMessage += pRow_DeviceTemplate->Description_get();
		if( m_sPK_Orbiter_List_For_Prompts.empty()==false )
		{
			DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(m_pRow_Device_Reported->PK_Device_get(),m_sPK_Orbiter_List_For_Prompts,sMessage,
				"pnp_" + StringUtils::itos(m_pRow_PnpQueue->PK_PnpQueue_get()),(Stage==PNP_DETECT_STAGE_DONE || Stage==PNP_REMOVE_STAGE_DONE) ? "2" : "3",interuptNoVideo);
			m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
		}
	}
}

void PnpQueueEntry::Block(EBlockedState eBlockedState)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueueEntry::Block queue %d blocked state was %d, now %d", m_pRow_PnpQueue->PK_PnpQueue_get(),(int) m_EBlockedState,(int) eBlockedState);
#endif
	m_EBlockedState=eBlockedState;
	m_tTimeBlocked=time(NULL);
}

string PnpQueueEntry::DeviceDataAsString()
{
	string sResult;
	for(map<int,string>::iterator it=m_mapPK_DeviceData.begin();it!=m_mapPK_DeviceData.end();++it)
		sResult += StringUtils::itos(it->first) + "|" + it->second + "|";

	if( m_pRow_PnpQueue->Signature_get().empty()==false )
		sResult += StringUtils::itos(DEVICEDATA_PNP_Signature_CONST) + "|" + m_pRow_PnpQueue->Signature_get() + "|";
	return sResult;
}

void PnpQueueEntry::ParseDeviceData(string sDeviceData)
{
	string::size_type pos=0;
	while(pos<sDeviceData.size())
	{
		int PK_DeviceData = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str());
		string sValue = StringUtils::Tokenize(sDeviceData,"|",pos);
		m_mapPK_DeviceData[PK_DeviceData]=sValue;
	}
}

void PnpQueueEntry::FindTopLevelDevice()
{
	m_pRow_Device_Reported = m_pRow_PnpQueue->FK_Device_Reported_getrow();
	if(NULL == m_pRow_Device_Reported) // Just use the plugin's device as the reporter if none was specified
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PnpQueueEntry::FindTopLevelDevice queue %d has no reporter %d",
			m_pRow_PnpQueue->PK_PnpQueue_get(),m_pRow_PnpQueue->FK_Device_Reported_get());

		m_pRow_Device_Reported = m_pDatabase_pluto_main->Device_get()->GetRow(m_pPlug_And_Play_Plugin->m_dwPK_Device);
	}

#ifdef DEBUG
	if(NULL != m_pRow_Device_Reported)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueueEntry::FindTopLevelDevice queue %d has reporter %d/ room%d",
			m_pRow_PnpQueue->PK_PnpQueue_get(), m_pRow_Device_Reported->PK_Device_get(),m_pRow_Device_Reported->FK_Room_get());
	}
#endif

	m_dwPK_Device_TopLevel=0;
	Row_Device *pRow_Device=m_pRow_Device_Reported;
	while(pRow_Device)
	{
		m_dwPK_Device_TopLevel = pRow_Device->PK_Device_get();
		pRow_Device = pRow_Device->FK_Device_ControlledVia_get() ? pRow_Device->FK_Device_ControlledVia_getrow() : NULL;
	}
}

void PnpQueueEntry::AssignDeviceData(Row_Device *pRow_Device)
{
	for(map<int,string>::iterator it=m_mapPK_DeviceData.begin();it!=m_mapPK_DeviceData.end();++it)
	{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueueEntry::AssignDeviceData queue %d Device %d Data %d=%s",
		m_pRow_PnpQueue->PK_PnpQueue_get(),pRow_Device->PK_Device_get(),it->first,it->second.c_str());
#endif
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),it->first,it->second);
	}
	if( m_pRow_PnpQueue->SerialNumber_get().size() )
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Serial_Number_CONST,m_pRow_PnpQueue->SerialNumber_get(),true);
	if( m_pRow_PnpQueue->Signature_get().size() )
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_PNP_Signature_CONST,m_pRow_PnpQueue->Signature_get());
}

void PnpQueueEntry::RemoveBlockedDeviceData()
{
	Row_DeviceTemplate *pRow_DeviceTemplate = NULL;
	
	if( m_pRow_PnpQueue->FK_Device_Created_get() )
	{
		Row_Device *pRow_Device = m_pRow_PnpQueue->FK_Device_Created_getrow();
		if( pRow_Device )
			pRow_DeviceTemplate=pRow_Device->FK_DeviceTemplate_getrow();
	}
	if( pRow_DeviceTemplate==NULL && m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		pRow_DeviceTemplate=m_pRow_PnpQueue->FK_DeviceTemplate_getrow();

	if( pRow_DeviceTemplate )
	{
		Row_DeviceTemplate_DeviceData *Row_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_DeviceTemplate->PK_DeviceTemplate_get(),DEVICEDATA_Pnp_Ignore_Device_Data_CONST);
		if( Row_DeviceTemplate_DeviceData )
		{
			string sData = Row_DeviceTemplate_DeviceData->IK_DeviceData_get();
			string::size_type pos=0;
			while(pos<sData.size())
			{
				int PK_DeviceData = atoi(StringUtils::Tokenize(sData,",",pos).c_str());
				m_mapPK_DeviceData.erase(PK_DeviceData);
			}
		}
	}
}
bool PnpQueueEntry::IsDuplicate(PnpQueueEntry *pPnpQueueEntry)
{
	bool bForcedMatch=false;  // So we can do some additional determinations of matching

	// Com ports switch from usb to serial and the vendor/model id gets stripped
	if( m_pRow_PnpQueue->Category_get()=="serial" && pPnpQueueEntry->m_pRow_PnpQueue->Category_get()=="serial" )
	{
		string sPort = m_mapPK_DeviceData_Find(DEVICEDATA_COM_Port_on_PC_CONST);
		if( sPort.empty()==false && sPort==pPnpQueueEntry->m_mapPK_DeviceData_Find(DEVICEDATA_COM_Port_on_PC_CONST) )
			bForcedMatch=true;
	}

	if( bForcedMatch ||
		(m_pRow_PnpQueue->FK_CommMethod_get()==pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get() &&
		m_pRow_PnpQueue->Removed_get()==pPnpQueueEntry->m_pRow_PnpQueue->Removed_get() && 
		m_pRow_PnpQueue->Path_get()==pPnpQueueEntry->m_pRow_PnpQueue->Path_get() &&
		m_pRow_PnpQueue->VendorModelId_get()==pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() &&
		m_pRow_PnpQueue->IPaddress_get()==pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get() &&
		m_pRow_PnpQueue->MACaddress_get()==pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() &&
		m_pRow_PnpQueue->FK_DeviceTemplate_get()==pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() &&
		m_pRow_PnpQueue->FK_PnpProtocol_get()==pPnpQueueEntry->m_pRow_PnpQueue->FK_PnpProtocol_get() &&
		(m_pRow_PnpQueue->SerialNumber_get()==pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get() || CompareShortSerialNumberAndPCILocation(pPnpQueueEntry)) &&
		m_pRow_PnpQueue->Category_get()==pPnpQueueEntry->m_pRow_PnpQueue->Category_get() &&
		CompareParms(pPnpQueueEntry)) )
	{
		if( m_pRow_PnpQueue->FK_CommMethod_get()!=COMMMETHOD_Ethernet_CONST && 
			m_pRow_PnpQueue->FK_Device_Reported_get()!=pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Reported_get() )
		{
			// It's a local, internal device and it's on a different machine so it's not a match
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueueEntry::IsDuplicate queue %d and %d are on different systems",
			m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	return false;
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueueEntry::IsDuplicate queue %d and %d match so far.  comparing %d to %d",
			m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),
			(int) m_mapPK_DeviceData.size(),(int) pPnpQueueEntry->m_mapPK_DeviceData.size());
#endif

		// So far it's a match.  Check if there's a com port, since there can be the same identical device on 2 serial ports
		if( m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)==m_mapPK_DeviceData.end() ||
			pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() ||
			m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST]==pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST] )
		{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueueEntry::IsDuplicate queue %d and %d are a match",
			m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
			return true;
		}
	}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PnpQueueEntry::IsDuplicate queue %d and %d are no match",
			m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
	return false;
}

bool PnpQueueEntry::CompareShortSerialNumberAndPCILocation(PnpQueueEntry *pPnpQueueEntry)
{
	string sBaseSerialNumber1 = BaseSerialNumber(m_pRow_PnpQueue->SerialNumber_get());
	string sBaseSerialNumber2 = BaseSerialNumber(pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get());

	string sPCILocation1 = m_mapPK_DeviceData_Find(DEVICEDATA_Location_on_PCI_bus_CONST);
	string sPCILocation2 = pPnpQueueEntry->m_mapPK_DeviceData_Find(DEVICEDATA_Location_on_PCI_bus_CONST);

	if( (sBaseSerialNumber1.empty()==false || m_pRow_PnpQueue->SerialNumber_get().empty()==false) && 
		(sBaseSerialNumber1==sBaseSerialNumber2 || sBaseSerialNumber1==pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get() || sBaseSerialNumber2==m_pRow_PnpQueue->SerialNumber_get())
		&& sPCILocation1.empty()==false && sPCILocation1==sPCILocation2 )
		return true;
	return false;
}

bool PnpQueueEntry::CompareParms(PnpQueueEntry *pPnpQueueEntry)
{
	for( map<int,string>::iterator it=m_mapPK_DeviceData.begin();it!=m_mapPK_DeviceData.end();++it )
	{
		int PK_DeviceData = it->first;
		// Some hardcoded things to ignore
		if( m_pRow_PnpQueue->Category_get()=="video4linux" && PK_DeviceData==DEVICEDATA_Block_Device_CONST )  // The same device may have many block devices.  Ignore them because if the pci location is the same it's a duplicate
			continue;
		if( pPnpQueueEntry->m_mapPK_DeviceData_Find(it->first)!=it->second )
			return false;
	}

	for( map<int,string>::iterator it=m_mapPK_DeviceData.begin();it!=m_mapPK_DeviceData.end();++it )
	{
		int PK_DeviceData = it->first;
		// Some hardcoded things to ignore
		if( m_pRow_PnpQueue->Category_get()=="video4linux" && PK_DeviceData==DEVICEDATA_Block_Device_CONST )  // The same device may have many block devices.  Ignore them because if the pci location is the same it's a duplicate
			continue;
		if( pPnpQueueEntry->m_mapPK_DeviceData_Find(it->first)!=it->second )
			return false;
	}

	return true;
}

string PnpQueueEntry::BaseSerialNumber(string sString)
{
	// If the serial number is in the format: /a/b/c/d_e  strip the e.  But only with that syntax
    string::size_type pos_slash = sString.rfind("/");
    string::size_type pos_underscore = sString.rfind("_");
	if( pos_slash==string::npos || pos_underscore==string::npos || pos_underscore<pos_slash )
		return sString;

	return sString.substr(0,pos_underscore);
}

string PnpQueueEntry::StageAsText()
{
	switch(m_pRow_PnpQueue->Stage_get())
	{
	case PNP_DETECT_STAGE_DETECTED:
		return "Detected";
	case PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT:
		return "Analyzing";
	case PNP_DETECT_STAGE_RUNNING_DETECTION_SCRIPTS:
		return "Querying device.  Please wait...";
	case PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT:
	case PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT:
		return "Confirming";
	case PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT:
		return "Options";
	case PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT:
		return "Pre PNP Script";
	case PNP_DETECT_STAGE_ADD_DEVICE:
		return "Adding";
	case PNP_DETECT_STAGE_ADD_SOFTWARE:
		return "Installing";
	case PNP_DETECT_STAGE_START_DEVICE:
		return "Starting";
	case PNP_DETECT_STAGE_DONE:
		return "Done with new device";

	case PNP_REMOVE_STAGE_REMOVED:
		return "Removed";
	case PNP_REMOVE_STAGE_DONE:
		return "Done removing device";
	};
	return "ERROR--Stage";
}

string PnpQueueEntry::ToString()
{
    return 
		string("PnpQueueEntry( queue ") + StringUtils::itos(m_pRow_PnpQueue->PK_PnpQueue_get()) + " " +
		"m_bCreateWithoutPrompting = " + StringUtils::ltos((long)m_bCreateWithoutPrompting) + ", "
		"m_EBlockedState = " + StringUtils::ltos((long)m_EBlockedState) + ", "
		"Stage = " + StageAsText() + ", "
		" )";
}


void PnpQueueEntry::m_pOH_Orbiter_Active_set(int PK_Device)
{
	m_pOH_Orbiter_Active_set( m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin_get()->m_mapOH_Orbiter_Find(PK_Device) );
}

void PnpQueueEntry::m_pOH_Orbiter_Active_set(OH_Orbiter *pOH_Orbiter)
{
	m_bUseAllOrbitersForPrompt=false;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueueEntry::m_pOH_Orbiter_Active_set queue %d set to orbiter %d", 
		m_pRow_PnpQueue->PK_PnpQueue_get(),pOH_Orbiter ? pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device : -1);
#endif
	m_pOH_Orbiter_Active = pOH_Orbiter;
}

void PnpQueueEntry::UseAllOrbitersForPrompt()
{
	m_bUseAllOrbitersForPrompt=true;
	m_sPK_Orbiter_List_For_Prompts=m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin_get()->m_sPK_Device_AllOrbiters_get();
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PnpQueueEntry::UseAllOrbitersForPrompt queue %d", 
		m_pRow_PnpQueue->PK_PnpQueue_get());
#endif
}
