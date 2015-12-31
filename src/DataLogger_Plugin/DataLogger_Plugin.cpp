/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "DataLogger_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
DataLogger_Plugin::DataLogger_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: DataLogger_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
DataLogger_Plugin::DataLogger_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: DataLogger_Plugin_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
DataLogger_Plugin::~DataLogger_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_lmce_datalog;
	
}

//<-dceag-getconfig-b->
bool DataLogger_Plugin::GetConfig()
{
	if( !DataLogger_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	m_pDatabase_lmce_datalog = new Database_lmce_datalog(LoggerWrapper::GetInstance());
	if( !m_pDatabase_lmce_datalog->Connect( "localhost","root","", "lmce_datalog", 3306 ) )
	// if( !m_pDatabase_lmce_datalog->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "lmce_datalog", m_pRouter->iDBPort_get( ) ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database 'lmce_datalog'!" );
		return false;
	}
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool DataLogger_Plugin::Register()
//<-dceag-reg-e->
{

	RegisterMsgInterceptor((MessageInterceptorFn)(&DataLogger_Plugin::ProcessEvent) ,0,0,0,0,MESSAGETYPE_EVENT,0);

	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
DataLogger_Plugin_Command *Create_DataLogger_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new DataLogger_Plugin(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void DataLogger_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void DataLogger_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

bool DataLogger_Plugin::ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo) {

	
	bool handled = true;
	int unit = 0;
	float fValue = 0;
	int iValue = 0;
	string sValue = "";

	// LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DataLogger Received Event: %d",pMessage->m_dwID);

	switch( pMessage->m_dwID ) {

		case EVENT_CO2_Level_Changed_CONST:
			unit = 1;
			sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			fValue = atoi(sValue.c_str());
			;;
			break;
		case EVENT_Temperature_Changed_CONST:
			unit = 2;
			sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			sscanf(sValue.c_str(),"%f",&fValue);
			;;
			break;
		case EVENT_Humidity_Changed_CONST:
			unit = 3;
			sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			sscanf(sValue.c_str(),"%i",&iValue);
			fValue = iValue;
			;;
			break;
		case EVENT_Brightness_Changed_CONST:
			unit = 4;
			sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			sscanf(sValue.c_str(),"%i",&iValue);
			fValue = iValue;
			;;
			break;
                case EVENT_State_Changed_CONST:
                        unit = 5;
                        sValue = pMessage->m_mapParameters[EVENTPARAMETER_State_CONST];
                        sscanf(sValue.c_str(),"%i",&iValue);
                        fValue = iValue;
                        ;;
                        break;
		case EVENT_Voltage_Changed_CONST:
			unit = 6;
			sValue = pMessage->m_mapParameters[EVENTPARAMETER_Voltage_CONST];
                        sscanf(sValue.c_str(),"%f",&fValue);
			;;
			break;
		case EVENT_Power_Usage_Changed_CONST:
			// This is an event that contain several bits of data, we need to check which one to handle
                        sValue = pMessage->m_mapParameters[EVENTPARAMETER_Watts_CONST];
			if ( sValue != "" )
			{
				unit = 7;
				sscanf(sValue.c_str(),"%f",&fValue);
			} else {
				sValue = pMessage->m_mapParameters[EVENTPARAMETER_WattsMTD_CONST];
				if ( sValue != "" )
				{
					unit = 10;
					sscanf(sValue.c_str(),"%f",&fValue);
				} else {
					handled = false;
				}
			}
                        ;;
			break;
		case EVENT_Energy_Cost_Changed_CONST:
                        unit = 8;
                        sValue = pMessage->m_mapParameters[EVENTPARAMETER_Cost_CONST];
                        sscanf(sValue.c_str(),"%f",&fValue);
                        ;;
			break;
		case EVENT_Sensor_Tripped_CONST:
			unit = 9;
			sValue = pMessage->m_mapParameters[EVENTPARAMETER_Tripped_CONST];
			sscanf(sValue.c_str(),"%i",&iValue);
			fValue = iValue;
			;;
			break;
		default:
			handled=false;
			;;
	}

	if (handled) {
		// LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DataLogger Handling Event: %d",pMessage->m_dwID);
		time_t timestamp;
		struct tm *timestruct;
		timestamp = time(NULL);
		timestruct = localtime(&timestamp); 

		char tmp_timestamp[512];
		strftime(tmp_timestamp,512,"%Y-%m-%d %H:%M:%S",timestruct);

		Row_Datapoints *pRow_Datapoints = m_pDatabase_lmce_datalog->Datapoints_get()->AddRow();
		pRow_Datapoints->Datapoint_set(fValue);
		pRow_Datapoints->FK_Unit_set(unit);
		pRow_Datapoints->EK_Device_set(pMessage->m_dwPK_Device_From);
		pRow_Datapoints->timestamp_set(tmp_timestamp);
		// LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "DATALOGGER FROM %i",pMessage->m_dwPK_Device_From );
		m_pDatabase_lmce_datalog->Datapoints_get()->Commit();
		// pRow_Datapoints->Delete();
		//
		// TODO: FIXME: I'm sure i need to free something here
		//delete pRow_Datapoints;
	}

	return false;

}


/*

	COMMANDS TO IMPLEMENT

*/


