/*
     Copyright (C) 2012 Harald Klein <hari@vt100.at

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "ZWave.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "PlutoUtils/LinuxSerialUSB.h"
#include "ZWInterface.h"
#include <math.h>

#define ALARM_NODE_DEAD	1

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
ZWave::ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: ZWave_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pZWInterface = NULL;
	m_bReady = false;
	m_setRecentlyAddedDevices.clear();
	m_pAlarmManager=NULL;
	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);

	m_dwPK_ClimateInterface = 0;
	m_dwPK_SecurityInterface = 0;
	m_iHouseMode = -1;

	// Value labels we are interested in and their polling intensities
	m_mapLabels["Switch"] = 1;
	m_mapLabels["Level"] = 1;
	m_mapLabels["Sensor"] = -1; // Don't poll, these will send events by themselves (or so they should)
	m_mapLabels["Voltage"] = 5;
	m_mapLabels["Temperature"] = 10;
	m_mapLabels["Luminance"] = 10;
	m_mapLabels["Mode"] = 10;
	m_mapLabels["Energy"] = 10;
	m_mapLabels["Power"] = 10;
	m_mapLabels["Relative Humidity"] = 10;
	m_mapLabels["Setpoint"] = 20;
	m_mapLabels["Heating 1"] = 20;
	m_mapLabels["Cooling 1"] = 20;
	m_mapLabels["Auto Changeover"] = 20;
	m_mapLabels["Battery Level"] = 255;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
ZWave::ZWave(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: ZWave_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
ZWave::~ZWave()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool ZWave::GetConfig()
{
	if( !ZWave_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	DeviceData_Impl* pDevice = m_pData->FindSelfOrChildWithinCategory( DEVICECATEGORY_Climate_Interface_CONST );
	if ( pDevice != NULL )
		m_dwPK_ClimateInterface = pDevice->m_dwPK_Device;
	pDevice = m_pData->FindSelfOrChildWithinCategory( DEVICECATEGORY_Security_Interface_CONST );
	if ( pDevice != NULL )
		m_dwPK_SecurityInterface = pDevice->m_dwPK_Device;
	m_bPollingEnabled = DATA_Get_Polling_Enabled();
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool ZWave::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
ZWave_Command *Create_ZWave(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new ZWave(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void ZWave::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command for child %d", pDeviceData_Impl->m_dwPK_Device);
	if (pDeviceData_Impl->m_dwPK_Device == m_dwPK_SecurityInterface) {
		if (pMessage->m_dwID == COMMAND_Set_House_Mode_CONST) {
  			m_iHouseMode = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
		}
		return;
	}
	string nodeInstance = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	uint8 node_id;
	uint8 instance_id;
	PortChannelToNodeInstance(nodeInstance, node_id, instance_id);
	if (node_id > 0 && node_id <= 233) {
		sCMD_Result = "OK";
		int level,duration,temp,fan;
		string heat;
		uint32 homeId = m_pZWInterface->GetHomeId();
		if (OpenZWave::Manager::Get()->IsNodeFailed(homeId, node_id)) {

		}
		switch (pMessage->m_dwID) {
			case COMMAND_Generic_On_CONST:
			case COMMAND_Set_Level_CONST:
			case COMMAND_Generic_Off_CONST:
			{
				// We can get On/Off/Set Level regardless of node type, so we handle all here and use the correct
				// ValueID for the node type
				bool onOffCmd = false;
				bool state = false;
				level = 0;
				if (pMessage->m_dwID == COMMAND_Set_Level_CONST) {
					level = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
					state = level>99;
				} else if (pMessage->m_dwID == COMMAND_Generic_On_CONST) {
					onOffCmd = true;
					state = true;
					level = 100;
				} else if (pMessage->m_dwID == COMMAND_Generic_Off_CONST) {
					onOffCmd = true;
				}
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET ON/OFF/LEVEL RECEIVED FOR CHILD %d/%d, level: %d",node_id,instance_id,level);

				m_pZWInterface->Lock();
				// Look for a Switch value for both binary and multilevel switches
				OpenZWave::ValueID* valueIDSwitch = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, "Switch");
				uint8 genericType = OpenZWave::Manager::Get()->GetNodeGeneric(homeId, node_id);
				// if this is a binary switch, or we got a OnOffCmd to a dimmer, use the "Switch" ValueID
				if (genericType == 0x10 || (onOffCmd && valueIDSwitch != NULL)) {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Using Switch ValueID");
					if (valueIDSwitch != NULL)
					{
						if (OpenZWave::Manager::Get()->SetValue(*valueIDSwitch, state)) {
							LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Set Switch successful");
						} else {
							LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Set Switch FAILED!");
						}
					}
				} else if (genericType == GENERIC_TYPE_THERMOSTAT) {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Using Thermostat Mode");
					if (state) {
						SetThermostatMode(node_id, instance_id, "Resume");
					} else {
						SetThermostatMode(node_id, instance_id, "Off");
					}
				} else {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Assuming node is a multilevel switch");
					OpenZWave::ValueID* valueID = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, "Level");
					if (valueID != NULL)
					{
						if (OpenZWave::Manager::Get()->SetValue(*valueID, (uint8)(level>99?99:level))) {
							LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Set Level successful");
						} else {
							LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Set Level FAILED!");
						}
					} else {
						OpenZWave::Manager::Get()->SetNodeLevel(homeId,node_id,level>99?99:level);
					}
				}
				m_pZWInterface->UnLock();
				break;
				;;
			}
			case COMMAND_Set_Temperature_CONST:
			{
				temp = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				string unit = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Units_CONST);
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET TEMPERATURE RECEIVED FOR CHILD %d, temperature: %d, unit = %s",node_id,temp, unit.c_str());
				if (unit == "F")
				{
					temp = (int)( (9.0/5.0) * (float)temp + 32.0 );
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Temperature converted to F : %d",temp);
				}

	// TODO: need to set temperature for a specific setpoint/mode. Setting it for the current operating mode might work,
	// except when we receive a Set_HeatCool command just after the Set_Temperature command, which might be the usual LMCE way
	// Set temperature for current operating mode, copy temperature to new mode if changed in Set_HeatCool
				OpenZWave::ValueID* valueIDMode = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, "Mode");
				string mode = "Heat";
				if ( valueIDMode != NULL ) {
					OpenZWave::Manager::Get()->GetValueListSelection(*valueIDMode, &mode);
				}
				if ( mode == "Heat" )
					SetThermostatSetpoint(node_id, instance_id, "Heating 1", (float)temp);
				else if ( mode == "Cool" )
					SetThermostatSetpoint(node_id, instance_id, "Cooling 1", (float)temp);
				else if ( mode == "Auto Changeover" ) 
					SetThermostatSetpoint(node_id, instance_id, "Auto Changeover", (float)temp);
				break;
				;;
			}
			case COMMAND_Set_Fan_CONST:
			{
				fan = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str());
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET FAN RECEIVED FOR CHILD %d, level: %d",node_id,fan);
				m_pZWInterface->Lock();
				OpenZWave::ValueID* valueIDFan = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, "Fan Mode");
				// Auto Low, On Low, Auto High, On High, Unknown 4, Unknown 5, Circulate
/*				if (fan == 1) {
					myZWApi->zwThermostatFanModeSet(node_id,3); // on high
					myZWApi->zwThermostatModeSet(node_id,6); // fan only
				} else {
					myZWApi->zwThermostatFanModeSet(node_id,0); // auto 
					myZWApi->zwThermostatModeSet(node_id,10); // auto changeover
				}
*/
				m_pZWInterface->UnLock();
				break;
				;;
			}
			case COMMAND_Set_HeatCool_CONST:
			{
				string mode = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
				char mode_tmp = '_';
				if (mode.size() == 1) {
					mode_tmp = mode.c_str()[0];
				}
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET HEAT/COOL RECEIVED FOR CHILD %d, string: %s",node_id,mode.c_str());
				// A - auto changeover; H - heat; C - cool; F - fan; 
				// ZWave supported modes: Off, Heat, Cool, Auto, Aux Heat, Resume, Fan Only, Furnace, Dry Air, Moist Air, Auto changeover, Heat Econ, Cool Econ, Away
				// Map LMCE modes to ZWave modes		
				switch(mode_tmp) {
					case 'A':
						mode = "Auto Changeover";
						break;
					case 'H':
						mode = "Heat";
						break;
					case 'C':
						mode = "Cool";
						break;
					case 'F':
						mode = "Fan Only";
						break;
				}
				SetThermostatMode(node_id, instance_id, mode);
				break;
				;;
			}
			case COMMAND_Play_Media_CONST:
//				myZWApi->zwAVControlSet(node_id,sequence,3); // vol up
				break;
				;;
		        case COMMAND_Assign_Access_Token_CONST:
		        {
				string id = pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
				// Assumes 0xAB 0xBC 0xDC etc.etc. format on token
				string token = pMessage->m_mapParameters[COMMANDPARAMETER_Tokens_CONST];
				string uid = pMessage->m_mapParameters[COMMANDPARAMETER_UID_CONST];
				vector<string> vect;
				StringUtils::Tokenize(token, " ", vect);
				uint8* data = new uint8[vect.size()];
				char* ch = new char[3];
				for (int i = 0; i < vect.size(); i++) {
					strcpy(ch, vect[i].substr(2, 2).c_str());
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Code part: %s", ch);
					data[i] = (uint8)StringUtils::HexByte2Num(ch);
				}
				delete ch;
				OpenZWave::ValueID* valueID = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, "Code " + id + ":");
				if (valueID != NULL && OpenZWave::Manager::Get()->SetValue(*valueID, data, vect.size())) {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Set Code successful");
				} else {
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Set Code FAILED!");
				}
				delete data;
				break;
				;;
			}
			
		}
	} else {
		sCMD_Result = "UNHANDLED CHILD";
	}
}

bool ZWave::SetThermostatMode(uint8 node_id, uint8 instance_id, string mode) 
{
	bool res = false;

	m_pZWInterface->Lock();
	OpenZWave::ValueID* valueIDMode = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, "Mode");
	if ( valueIDMode != NULL )
	{
		bool res = OpenZWave::Manager::Get()->SetValueListSelection(*valueIDMode,mode);
		if ( res ) {
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Mode set to '%s' succesfully", mode.c_str());
		} else {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MODE NOT SET (unsupported mode?) mode = '%s'", mode.c_str());
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"No such value 'Mode' for this node, unable to set mode!");
	}
	m_pZWInterface->UnLock();
	return res;
}

bool ZWave::SetThermostatSetpoint(uint8 node_id, uint8 instance_id, string setpoint, float val) 
{
	bool res = false;

	m_pZWInterface->Lock();
	OpenZWave::ValueID* valueIDSetpoint = m_pZWInterface->GetValueIdByNodeInstanceLabel(node_id, instance_id, setpoint);
	if ( valueIDSetpoint != NULL )
	{
		res = OpenZWave::Manager::Get()->SetValue(*valueIDSetpoint,(float)val);
		if ( res ) {
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Setpoint '%s' set to %f succesfully", setpoint.c_str(), val);
		} else {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Setpoint '%s' to %f failed!", setpoint.c_str(), val);
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"No such setpoint '%s' found!", setpoint.c_str());
	}
	m_pZWInterface->UnLock();
	return res;
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void ZWave::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void ZWave::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void ZWave::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
	cout << "Need to implement command #756 - Report Child Devices" << endl;
}

//<-dceag-c757-b->

	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

void ZWave::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{
	m_pZWInterface->Lock();
//	OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_ReceiveConfiguration, controller_update, (void*)NULL, true, 0, 0);
	m_pZWInterface->UnLock();
}

//<-dceag-c760-b->

	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void ZWave::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
	int node_id = atoi(sID.c_str());
	int homeId = m_pZWInterface->GetHomeId();
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #760 - Send Command To Child, node id: %i",node_id);
	if (node_id > 0 && node_id <= 233) {
		sCMD_Result = "OK";
		switch (iPK_Command) {
			case COMMAND_Generic_On_CONST:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ON RECEIVED FOR CHILD %d",node_id);
				m_pZWInterface->Lock();
				OpenZWave::Manager::Get()->SetNodeOn(homeId,node_id);
				m_pZWInterface->UnLock();
				break;
				;;
			case COMMAND_Generic_Off_CONST:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"OFF RECEIVED FOR CHILD %d",node_id);
				m_pZWInterface->Lock();
				OpenZWave::Manager::Get()->SetNodeOff(homeId,node_id);
				m_pZWInterface->UnLock();
				break;
				;;
			default:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"NOT IMPLEMENTED COMMAND RECEIVED FOR CHILD %d",node_id);
				sCMD_Result = "ZWave NOT IMPLEMENTED";
		}
	} else {
		sCMD_Result = "UNHANDLED CHILD";
	}
}

//<-dceag-c776-b->

	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

void ZWave::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
{
	if (sArguments == "ResetController") {
		m_pZWInterface->Lock();
		OpenZWave::Manager::Get()->ResetController(m_pZWInterface->GetHomeId());
		m_pZWInterface->UnLock();
	} else {
		m_pZWInterface->Lock();
		OpenZWave::Manager::Get()->SoftReset(m_pZWInterface->GetHomeId());
		m_pZWInterface->UnLock();
	}
}

//<-dceag-c788-b->

	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

void ZWave::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() sArgument %s", sArguments.c_str());
	if (StringUtils::StartsWith(sArguments,"NNU")) {
		uint8 nodeId = atoi(sArguments.substr(3).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() RequestNodeNeighborUpdate node %d", nodeId);
		OpenZWave::Manager::Get()->RequestNodeNeighborUpdate(m_pZWInterface->GetHomeId(), nodeId);
	} else if (StringUtils::StartsWith(sArguments,"NU")) {
		uint8 nodeId = atoi(sArguments.substr(2).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() RequestNetworkUpdate node %d", nodeId);
		OpenZWave::Manager::Get()->RequestNetworkUpdate(m_pZWInterface->GetHomeId(), nodeId);
	} else if (StringUtils::StartsWith(sArguments,"HNN")) {
		uint8 nodeId = atoi(sArguments.substr(3).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() HealNodeNetwork node %d", nodeId);
		OpenZWave::Manager::Get()->HealNetworkNode(m_pZWInterface->GetHomeId(), nodeId, true);
	} else if (StringUtils::StartsWith(sArguments,"HN")) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() HealNetwork");
		OpenZWave::Manager::Get()->HealNetwork(m_pZWInterface->GetHomeId(), true);
	} else if (StringUtils::StartsWith(sArguments,"RFN")) {
		uint8 nodeId = atoi(sArguments.substr(3).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() ReplaceFailedNode node %d", nodeId);
		OpenZWave::Manager::Get()->ReplaceFailedNode(m_pZWInterface->GetHomeId(), nodeId);
	} else if (StringUtils::StartsWith(sArguments,"TNN")) {
		uint8 nodeId = atoi(sArguments.substr(3).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() TestNetworkNode node %d", nodeId);
		OpenZWave::Manager::Get()->TestNetworkNode(m_pZWInterface->GetHomeId(), nodeId, 10);
	} else if (StringUtils::StartsWith(sArguments,"TN")) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() TestNetwork");
		OpenZWave::Manager::Get()->TestNetwork(m_pZWInterface->GetHomeId(), 10);
	}
}

//<-dceag-c820-b->

	/** @brief COMMAND: #820 - Assign Return Route */
	/** Assign Return Route between 2 nodes from ZWave network */
		/** @param #239 NodeID */
			/** Source Node ID */
		/** @param #240 DestNodeID */
			/** Destination Node ID */

void ZWave::CMD_Assign_Return_Route(int iNodeID,int iDestNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c820-e->
{

	OpenZWave::Manager::Get()->AssignReturnRoute(m_pZWInterface->GetHomeId(), iNodeID);
}

//<-dceag-c840-b->

	/** @brief COMMAND: #840 - SetWakeUp */
	/** Set the WakeUp value for Z-Wave PIR devices. */
		/** @param #48 Value */
			/** WakeUp timeout value (1-255 min) */
		/** @param #239 NodeID */
			/** Z-Wave node ID of the device. */

void ZWave::CMD_SetWakeUp(int iValue,int iNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c840-e->
{
	if (m_pZWInterface->SetWakeUp(iNodeID, iValue))
		sCMD_Result = "OK";
}

//<-dceag-c841-b->

	/** @brief COMMAND: #841 - Set Config Param */
	/** Set the configuration parameters for Z-Wave devices (PIR) */
		/** @param #48 Value */
			/** The value of parameter. */
		/** @param #222 Size */
			/** Size of the parameter (1, 2 or 4; 0/empty==autodetect) */
		/** @param #239 NodeID */
			/** Z-Wave node id */
		/** @param #248 Parameter ID */
			/** The configuration parameter ID. */

void ZWave::CMD_Set_Config_Param(int iValue,int iSize,int iNodeID,int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c841-e->
{
	int size = iSize;
	if ( size == 0 ) {
		if ( iValue <= 0xff )
		{
			size = 1;
		} else if ( iValue <= 0xffff )
		{
			size = 2;
		} else {
			size = 4;
		}
	}
	m_pZWInterface->Lock();
	OpenZWave::Manager::Get()->SetConfigParam(m_pZWInterface->GetHomeId(), iNodeID, iParameter_ID, iValue, size);
	m_pZWInterface->UnLock();
}

//<-dceag-c842-b->

	/** @brief COMMAND: #842 - Set Association */
	/** Set the association of Z-Wave nodes into a group. */
		/** @param #239 NodeID */
			/** Z-Wave node ID which associates some nodes in its group. */
		/** @param #249 Group ID */
			/** The group number which includes the associated nodes. */
		/** @param #250 Nodes List */
			/** The list of associated Z-Wave nodes (comma separated). */

void ZWave::CMD_Set_Association(int iNodeID,int iGroup_ID,string sNodes_List,string &sCMD_Result,Message *pMessage)
//<-dceag-c842-e->
{
	int maxGroup = OpenZWave::Manager::Get()->GetNumGroups(m_pZWInterface->GetHomeId(), iNodeID);
	if (iGroup_ID <= maxGroup)
	{
		vector<string> vectNodes;
		StringUtils::Tokenize(sNodes_List, ",", vectNodes);
		for (size_t i = 0; i < vectNodes.size(); i++)
		{
			int targetId = atoi(vectNodes[i].c_str());
			m_pZWInterface->Lock();
			if (targetId > 0)
			{
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::CMD_Set_Association() Adding association: (node %d, group %d) -> (node %d)", iNodeID, iGroup_ID, targetId);
				OpenZWave::Manager::Get()->AddAssociation(m_pZWInterface->GetHomeId(), iNodeID, iGroup_ID, targetId);
			} else if (targetId < 0)
			{
				uint8 id = -targetId;
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::CMD_Set_Association() Removing association: (node %d, group %d) -> (node %d)", iNodeID, iGroup_ID, id);
				OpenZWave::Manager::Get()->RemoveAssociation(m_pZWInterface->GetHomeId(), iNodeID, iGroup_ID, id);
			}
			m_pZWInterface->UnLock();
		}
	}
}

//<-dceag-c966-b->

	/** @brief COMMAND: #966 - Set Polling State */
	/** Set the polling State for the Z-Wave interface */
		/** @param #2 PK_Device */
			/** The device to set polling info for.  If both this and node id are blank, it sets global polling */
		/** @param #5 Value To Assign */
		/** @param #220 Report */
			/** If true, the changes will go back to the router to update device data so they persist after a reload */
		/** @param #225 Always */
			/** If true, this setting will persist.  Otherwise it will only happen one time and revert to default */
		/** @param #239 NodeID */
			/** The node to set polling for.  If both this and PK_device are blank, it sets global polling.  This supercedes PK_Device */

void ZWave::CMD_Set_Polling_State(int iPK_Device,string sValue_To_Assign,bool bReport,bool bAlways,int iNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c966-e->
{
	if (iPK_Device <= 0 || sValue_To_Assign == "")
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ZWave::CMD_Set_Polling_State(): Need to specify both PK_Device and sValue_To_Assign");
	} else {
		uint8 nodeId = 0;
		if (iPK_Device > 0)
		{
			uint8 iInstance = 1;
			DeviceData_Impl* pChildDevice = m_pData->FindChild(iPK_Device);
		        string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			PortChannelToNodeInstance(tmp_node_id, nodeId, iInstance);
			OpenZWave::ValueID* valueId = m_pZWInterface->GetValueIdByNodeInstanceLabel(nodeId, iInstance, sValue_To_Assign);
			if (valueId != NULL)
			{
				int intensity = 1;
				m_pZWInterface->Lock();
				if (bReport)
					OpenZWave::Manager::Get()->EnablePoll(*valueId, intensity);
				else
					OpenZWave::Manager::Get()->DisablePoll(*valueId);
				m_pZWInterface->UnLock();

				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::CMD_Set_Polling_State(): Set polling for node %d/%d/%d, value label %s, intensity %d", nodeId, valueId->GetCommandClassId(), valueId->GetInstance(), sValue_To_Assign.c_str(), intensity);
				if (bAlways)
				{
					// Store config for next time - will store all config, not just this one
					OpenZWave::Manager::Get()->WriteConfig( m_pZWInterface->GetHomeId() );
				}
			} else {
				DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ZWave::CMD_Set_Polling_State(): No such ValueId for node %d, instance %d, label %s", nodeId, iInstance, sValue_To_Assign.c_str());
			}
		}
	}
}

//<-dceag-c967-b->

	/** @brief COMMAND: #967 - Add Node */
	/** Adds a new Z-Wave node, must be SIS or Master */
		/** @param #39 Options */
			/** A string of letters for each of the following options:
H = high power */
		/** @param #48 Value */
			/** empty or 1 = Node Any, 2=node controller, 3=node slave, 4=node existing, 5=node stop, 6=node stop failed */
		/** @param #182 Timeout */
			/** Number of seconds to timeout.  If not specified default is 60 seconds.  0=no timeout; a subsequent node stop must be called. */
		/** @param #259 Multiple */
			/** If true, allow addition of multiple nodes until timeout occurs or add node is called with node stop. */

void ZWave::CMD_Add_Node(string sOptions,int iValue,string sTimeout,bool bMultiple,string &sCMD_Result,Message *pMessage)
//<-dceag-c967-e->
{
	m_pZWInterface->Lock();
	if ( iValue == 5 )
	{
		OpenZWave::Manager::Get()->CancelControllerCommand(m_pZWInterface->GetHomeId());
	} else {
		OpenZWave::Manager::Get()->AddNode(m_pZWInterface->GetHomeId(), sOptions == "S");
	}
	m_pZWInterface->UnLock();
}

//<-dceag-c968-b->

	/** @brief COMMAND: #968 - Remove Node */
	/** Removes a Z-Wave node, must be SIS or Master */
		/** @param #39 Options */
			/** If true, allow deletion of multiple nodes until timeout occurs or add node is called with node stop. */
		/** @param #48 Value */
			/** empty or 1 = Node Any, 2=node controller, 3=node slave, 5=node stop */
		/** @param #182 Timeout */
			/** Number of seconds to timeout.  If not specified default is 60 seconds.  0=no timeout; a subsequent node stop must be called. */
		/** @param #259 Multiple */
			/** If true, allow deletion of multiple nodes until timeout occurs or add node is called with node stop. */

void ZWave::CMD_Remove_Node(string sOptions,int iValue,string sTimeout,bool bMultiple,string &sCMD_Result,Message *pMessage)
//<-dceag-c968-e->
{
	m_pZWInterface->Lock();
	if ( iValue == 5 )
	{
		OpenZWave::Manager::Get()->CancelControllerCommand(m_pZWInterface->GetHomeId());
	} else if ( iValue < 0)
	{
		uint8 nodeId = -iValue;
		OpenZWave::Manager::Get()->RemoveFailedNode(m_pZWInterface->GetHomeId(), nodeId);
	} else {
		OpenZWave::Manager::Get()->RemoveNode(m_pZWInterface->GetHomeId());
	}
	m_pZWInterface->UnLock();
}

//<-dceag-c1085-b->

	/** @brief COMMAND: #1085 - Resync node */
	/** Resynchronize device data */
		/** @param #239 NodeID */
			/** Node id to resync */

void ZWave::CMD_Resync_node(int iNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1085-e->
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::CMD_Resync_node() node %d", iNodeID);
	OpenZWave::Manager::Get()->RefreshNodeInfo(m_pZWInterface->GetHomeId(), iNodeID);
}

void ZWave::OnNotification(OpenZWave::Notification const* _notification, NodeInfo* nodeInfo) {

	switch( _notification->GetType() )
	{

	case OpenZWave::Notification::Type_ValueAdded:
	{
		if( nodeInfo != NULL )
		{
			// Do we want this value polled
			OpenZWave::ValueID id = _notification->GetValueID();
			string label = OpenZWave::Manager::Get()->GetValueLabel(id);
			if (m_mapLabels.find(label) != m_mapLabels.end()) {
				if (m_bPollingEnabled)
				{
					// We automatically poll interesting values
					// intensity = poll this value every X poll cycle/interval
					int intensity = m_mapLabels.find(label)->second;
					if (intensity > 0)
					{
						DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::OnNotification() ValueAdded: Set polling for node %d/%d/%d, value label %s, intensity %d", _notification->GetNodeId(), id.GetCommandClassId(), id.GetInstance(), label.c_str(), intensity);
						OpenZWave::Manager::Get()->EnablePoll(id, intensity);
					}
				} else if (!OpenZWave::Manager::Get()->isPolled(id)) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"ZWave::OnNotification() ValueAdded: Polling not enabled, ignoring node %d/%d/%d, value label %s, You might consider manually setting polling for this value.", _notification->GetNodeId(), id.GetCommandClassId(), id.GetInstance(), label.c_str());
				}
			}
			// Set verified changes - this will prevent a ValueChanged notification for unchanged values
			OpenZWave::Manager::Get()->SetChangeVerified(id, true);
		}
/*		if (m_pZWInterface->IsReady() && nodeInfo != NULL && nodeInfo->m_generic != 0)
		{
			// After initialization is complete, ValueAdded is emitted when a new value is added - so we should check if we need to add new devices
			MapNodeToDevices(nodeInfo);
			}*/
		break;
	}

	case OpenZWave::Notification::Type_ValueChanged:
	{
		if( nodeInfo != NULL )
		{
			// One of the node values has changed
			OpenZWave::ValueID id = _notification->GetValueID();
			string label = OpenZWave::Manager::Get()->GetValueLabel(id);

			int PKDevice = nodeInfo->GetPKDeviceForValue(id);
			if (PKDevice > 0)
			{
				// TODO: find value type of id

				if ( label == "Battery Level" )
				{
					uint8 level = 0;
					OpenZWave::Manager::Get()->GetValueAsByte(id, &level);
					ReportBatteryStatus(PKDevice, level);
				} else if ( label == "Temperature" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					string units = OpenZWave::Manager::Get()->GetValueUnits(id);
					if ( units == "F" ) 
					{
						level = (level-32) *5 / 9;
					}
					SendTemperatureChangedEvent(PKDevice, level);
				} else if ( label == "Sensor" ) {
					bool state;
					OpenZWave::Manager::Get()->GetValueAsBool(id, &state);
					SendSensorTrippedEvent(PKDevice, state);
				} else if ( label == "Basic" ) {
					uint8 level=0;
					OpenZWave::Manager::Get()->GetValueAsByte(id, &level);
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event");
					HandleDeviceChangedEvents (PKDevice, level);
				} else if ( label == "Switch" ) {
					bool state=false;
					OpenZWave::Manager::Get()->GetValueAsBool(id, &state);
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event");
					HandleDeviceChangedEvents (PKDevice, state ? 100 : 0);
				} else if ( label == "Level" ) {
					uint8 level = 0;
					OpenZWave::Manager::Get()->GetValueAsByte(id, &level);
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State(level) changed, send light changed event");
					HandleDeviceChangedEvents (PKDevice, level);
				} else if ( label == "Luminance" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendBrightnessChangedEvent(PKDevice, level);
				} else if ( label == "Relative Humidity" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendRelativeHumidityChangedEvent(PKDevice, level);
				} else if ( label == "Power" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendPowerUsageChangedEvent(PKDevice, level);
				} else if ( label == "Energy" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendPowerUsageCumulativeChangedEvent(PKDevice, level);
				} else if ( label == "Voltage" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendVoltageChangedEvent(PKDevice, level);
				} else if ( label == "Setpoint" || label == "Heating 1" || label == "Cooling 1" || label == "Auto Changeover") {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendSetpointChangedEvent(PKDevice, level);
				} else if ( label == "Access Control" ) {
					string val;
					OpenZWave::Manager::Get()->GetValueAsString(id, &val);
					HandleHouseModeChange(PKDevice, val);
				}
			}
		}
		break;
	}
	case OpenZWave::Notification::Type_ValueRefreshed:
	{
		if( nodeInfo != NULL )
		{
			// One of the node values has changed
			OpenZWave::ValueID id = _notification->GetValueID();
			string label = OpenZWave::Manager::Get()->GetValueLabel(id);

			int PKDevice = nodeInfo->GetPKDeviceForValue(id);
			if (PKDevice > 0)
			{
				// Special case for access control - OZW might think we have a different mode
				// than we actually have, so we also check house mode when value is refreshed
				if ( label == "Access Control" ) {
					string val;
					OpenZWave::Manager::Get()->GetValueAsString(id, &val);
					HandleHouseModeChange(PKDevice, val);
				}
			}
		}
		break;
	}
	case OpenZWave::Notification::Type_NodeEvent:
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Notification::Type_NodeEvent");
		if( nodeInfo != NULL )
		{
			// We have received an event from the node, caused by a
			// basic_set or hail message.
			OpenZWave::ValueID id = _notification->GetValueID();
			string label = OpenZWave::Manager::Get()->GetValueLabel(id);
			uint8 value = _notification->GetEvent();

			int PKDevice = nodeInfo->GetPKDeviceForValue(id);
			// Fall back to main device for this kind of events, as the valueid is not (always?) set
			if (PKDevice == 0)
				PKDevice = nodeInfo->m_vectDevices[0]->m_dwPK_Device;
			if (PKDevice > 0)
			{
				uint8 typeBasic = OpenZWave::Manager::Get()->GetNodeBasic(_notification->GetHomeId(), _notification->GetNodeId());
				if  ( nodeInfo->m_generic == GENERIC_TYPE_SWITCH_REMOTE || typeBasic == BASIC_TYPE_CONTROLLER ) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed: level = %d", value);
					SendOnOffEvent (PKDevice, value);
					
				} else if (nodeInfo->m_generic == GENERIC_TYPE_SENSOR_BINARY || nodeInfo->m_generic == GENERIC_TYPE_SENSOR_MULTILEVEL) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"This is a binary sensor, so we send sensor tripped event: value = %d", value);
					SendSensorTrippedEvent(PKDevice, value);
				} else if (nodeInfo->m_generic == GENERIC_TYPE_SWITCH_BINARY || nodeInfo->m_generic == GENERIC_TYPE_SWITCH_MULTILEVEL) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event: value = %d", value);
					HandleDeviceChangedEvents (PKDevice, value);
				} else {
					DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"No suitable event to send for node basic = %d, generic = %d", typeBasic, nodeInfo->m_generic);
				}
			} else {
				DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING,"No device found for this Node and value, label = %s", label.c_str());
			}
		}
		break;
	}
	case OpenZWave::Notification::Type_EssentialNodeQueriesComplete:
	{
		if( m_pZWInterface->IsReady() && nodeInfo != NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::OnNotification() : Type_EssentialNodeQueriesComplete nodeId = %d", nodeInfo->m_nodeId);
//			MapNodeToDevices(nodeInfo);
		}
		break;
	}
	case OpenZWave::Notification::Type_NodeQueriesComplete:
	{
		if( m_pZWInterface->IsReady() && nodeInfo != NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::OnNotification() : Type_NodeQueriesComplete nodeId = %d", nodeInfo->m_nodeId);
			MapNodeToDevices(nodeInfo);
		}
		break;
	}
	case OpenZWave::Notification::Type_NodeRemoved:
	{
		// Note: cannot use nodes list from zwinterface at this point, as the node has already been deleted
		// TODO: removed for now as it seem to trigger this when doing some controller commands
                //DeleteDevicesForNode(_notification->GetNodeId());
		break;
	}
	case OpenZWave::Notification::Type_Notification:
	{
		if (_notification->GetNotification() == OpenZWave::Notification::Code_Dead)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ZWave::OnNotification() : Node presumed dead, node id = %d", nodeInfo->m_nodeId);
			// add a alarm in 3 minutes that will react to the dead node - we should wait for a while for the network to have recovered from a failed transmission
			m_pAlarmManager->AddRelativeAlarm(180,this,ALARM_NODE_DEAD,nodeInfo);
		}
		else if (_notification->GetNotification() == OpenZWave::Notification::Code_Alive)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::OnNotification() : Node brought back to life, node id = %d", nodeInfo->m_nodeId);
		} else if (_notification->GetNotification() == OpenZWave::Notification::Code_Timeout)
		{
			m_listStatus.push_back("Message timed out, node " + StringUtils::itos(_notification->GetNodeId()));
		}
		break;
	}
	case OpenZWave::Notification::Type_ControllerCommand:
	{
		switch (_notification->GetEvent()) {
		case OpenZWave::Driver::ControllerState_Normal:
			m_listStatus.push_back("Normal");
			break;
		case OpenZWave::Driver::ControllerState_Starting:
			m_listStatus.push_back("Starting");
			break;
		case OpenZWave::Driver::ControllerState_Cancel:
			m_listStatus.push_back("Cancelled");
			break;
		case OpenZWave::Driver::ControllerState_Error:
			m_listStatus.push_back("Error");
			break;
		case OpenZWave::Driver::ControllerState_Waiting:
			m_listStatus.push_back("Waiting for user action");
			break;
		case OpenZWave::Driver::ControllerState_Sleeping:
			m_listStatus.push_back("Command is on sleep queue waiting for device");
			break;
		case OpenZWave::Driver::ControllerState_InProgress:
			m_listStatus.push_back("Communicating with other device");
			break;
		case OpenZWave::Driver::ControllerState_Completed:
			m_listStatus.push_back("Command has completed successfully");
			break;
		case OpenZWave::Driver::ControllerState_Failed:
			m_listStatus.push_back("Command has failed");
			break;
		case OpenZWave::Driver::ControllerState_NodeOK:
			m_listStatus.push_back("NodeOK");
			break;
		case OpenZWave::Driver::ControllerState_NodeFailed:
			m_listStatus.push_back("NodeFailed");
			break;
		default:
			m_listStatus.push_back("Unknown");
			break;
		}
		if (m_listStatus.size() > 100)
			m_listStatus.pop_front();
		break;
	}
	default:
	{
	}

	}

}

void ZWave::SetInterface(ZWInterface* pZWInterface)
{
	m_pZWInterface = pZWInterface;
	if (m_pZWInterface != NULL)
		m_pZWInterface->SetZWave(this);
}

ZWConfigData* ZWave::GetConfigData()
{
	string port = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	int timeout = atoi(DATA_Get_Timeout().c_str());
	return new ZWConfigData(port, timeout);
}

void ZWave::PortChannelToNodeInstance(string pc, uint8 &iNodeId, uint8 &iInstance)
{
	iInstance = 0;
	// handle nodeid/cc/instance
	if (pc.find("/") != string::npos) {
	        vector<string> vectNI;
		StringUtils::Tokenize(pc, "/", vectNI);
	        iNodeId = atoi(vectNI[0].c_str());
		iInstance = atoi(vectNI[1].c_str());
	} else {
	        iNodeId = atoi(pc.c_str());
	}

}

DeviceData_Impl *ZWave::GetDeviceForPortChannel(string sPortChannel) {
        DeviceData_Impl *pChildDevice = NULL;

        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
		        string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			// check if child exists
			if ( tmp_node_id.compare(sPortChannel) == 0) {
			        return pChildDevice;
			}

			// iterate over embedded interfaces
			DeviceData_Impl *pChildDevice1 = NULL;
			for( VectDeviceData_Impl::const_iterator it1 = pChildDevice->m_vectDeviceData_Impl_Children.begin();
				it1 != pChildDevice->m_vectDeviceData_Impl_Children.end(); ++it1 )
			{
				pChildDevice1 = (*it1);
				if( pChildDevice1 != NULL )
				{
					string tmp_node_id = pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if ( tmp_node_id.compare(sPortChannel) == 0) {
					        return pChildDevice1;
					}
				}
			}

		}
	}
	
	return NULL;
}

void ZWave::DoNodeToDeviceMapping()
{
        // make sure zw interface is ready (this will also check if it is inited successfully), if not, we got no nodes in our list and would delete all LMCE devices...
        if (!m_pZWInterface->IsReady()) {
	        return;
	}
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWave::DoNodeToDeviceMapping(): Listing nodes found in ZWave network");
	list<NodeInfo*>::iterator it;
	m_pZWInterface->Lock();
	list<NodeInfo*> nodes = m_pZWInterface->GetNodes();
	for (it = nodes.begin(); it != nodes.end(); it++)
	{
		NodeInfo *node = *it;
		MapNodeToDevices(node);
	}

	// list unaccounted for children devices - maybe some nodes were removed from the ZWave network
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWave::DoNodeToDeviceMapping(): Checking for orphaned devices (where the node has been removed)");
        DeviceData_Impl *pChildDevice = NULL;
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
			if ( pChildDevice->m_dwPK_Device != m_dwPK_ClimateInterface && pChildDevice->m_dwPK_Device != m_dwPK_SecurityInterface )
			{
				string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
				int nodeId = atoi(tmp_node_id.c_str());
				if ( nodeId > 0 && m_pZWInterface->GetNodeInfo(m_pZWInterface->GetHomeId(), nodeId) == NULL ) {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::DoNodeToDeviceMapping(): Node %d, device %d removed.", nodeId, pChildDevice->m_dwPK_Device);
					DeleteDevice(pChildDevice->m_dwPK_Device);
				}
			}

			// iterate over embedded interfaces
			DeviceData_Impl *pChildDevice1 = NULL;
			for( VectDeviceData_Impl::const_iterator it1 = pChildDevice->m_vectDeviceData_Impl_Children.begin();
				it1 != pChildDevice->m_vectDeviceData_Impl_Children.end(); ++it1 )
			{
				pChildDevice1 = (*it1);
				if( pChildDevice1 != NULL )
				{
					string tmp_node_id = pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					int nodeId = atoi(tmp_node_id.c_str());
					if ( nodeId > 0 && m_pZWInterface->GetNodeInfo(m_pZWInterface->GetHomeId(), nodeId) == NULL ) {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::DoNodeToDeviceMapping(): Node %d, device %d removed.", nodeId, pChildDevice1->m_dwPK_Device);
						DeleteDevice(pChildDevice1->m_dwPK_Device);
					}
				}
			}

		}
	}
	m_pZWInterface->UnLock();
}

/**
 * This method will look at all values this node presents, and map them to their correct LMCE devices.
 * By default all values are mapped to the main device, but there are exceptions.
 * If we determine that some values are not "compatible" with the main device, they are
 * mapped to a new device.
 */
void ZWave::MapNodeToDevices(NodeInfo* node)
{
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, " * Node id=%d", node->m_nodeId);
	if (node->m_nodeId == m_pZWInterface->GetNodeId())
		return; // Don't do any mapping for the controller node

	if (node->m_generic == 0) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::MapNodeToDevices() : Called without generic and specific type on node. Setting these now!!");
		node->m_generic = OpenZWave::Manager::Get()->GetNodeGeneric(node->m_homeId, node->m_nodeId);
		node->m_specific = OpenZWave::Manager::Get()->GetNodeSpecific(node->m_homeId, node->m_nodeId);
	}
	// clear values and devices for node before proceeding
	node->ClearDeviceMapping();
	LMCEDevice* pMainDevice = node->m_vectDevices[0];
	if (node->m_values.size() > 0)
	{
		// Looking through all values, by default all will be mapped to the main device
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "  -> Values:");
		list<OpenZWave::ValueID>::iterator valIt;
		for ( valIt = node->m_values.begin(); valIt != node->m_values.end(); valIt++)
		{
            // TODO: It *might* make sense to already at this point decide what the main device DT should be, and
            // use that when deciding wheter other values need a separate device

			OpenZWave::ValueID value = *valIt;
			string label = OpenZWave::Manager::Get()->GetValueLabel(value);
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "   - Instance=%d, class=%d, index=%d, label=%s, type=%s", value.GetInstance(), value.GetCommandClassId(), value.GetIndex(), label.c_str(), OpenZWave::Value::GetTypeNameFromEnum(value.GetType()));

			unsigned int mapToDevice = 0;
			bool needSeparateDevice = false;
			if (value.GetInstance() > 1)
			{
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "    -> Mapping to separate device as this value has instance > 1");
				needSeparateDevice = true;
				mapToDevice = value.GetInstance()-1;
			} else if (!pMainDevice->IsCompatible(value, label)) {
				// By default, all other  values are connected to main device, but we must check to see if this value
				// should be a new device. I.e. a Luminance value should not be the same device as a Temperature value
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "    -> Mapping to separate device as this value is not compatible with main lmce device");
				needSeparateDevice = true;
				mapToDevice = 1;
			}
			if (needSeparateDevice)
			{
				// find first device that this value can map to
				bool done = false;
				while (mapToDevice < node->m_vectDevices.size() && !done)
				{
					if (!node->m_vectDevices[mapToDevice]->IsCompatible(value, label)) {
						// these values can not co-exist as the same lmce device, not compatible
						mapToDevice++;
					} else {
						done = true;
					}
				}
			}
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "    -> Mapping value to device no %d", mapToDevice);
			if (mapToDevice >= node->m_vectDevices.size())
			{
				node->AddDevice(mapToDevice);
			}
			node->MapValueToDevice(mapToDevice, value);
		}
	}
	// When we are done mapping values to devices, check that all devices exists in LMCE
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device mapping:");
	int deviceNo = 0;
	for (vector<LMCEDevice*>::iterator it = node->m_vectDevices.begin(); it != node->m_vectDevices.end(); ++it)
	{
		LMCEDevice* pLmceDevice = *it;
		string sId = StringUtils::itos(node->m_nodeId);
		if ( deviceNo > 0 ) {
			// TODO: use some other number, preferably from the values themselves, so we don't risk mapping to another device the next time
			// because some values were seen in a different order. Not sure if that is going to happen though...
			sId += "/" + StringUtils::itos(deviceNo);
		}

		// Set the device template (and possibly also the main value)
		SetDeviceTemplate(pLmceDevice);

		string label = "<none>";
		if (pLmceDevice->HasMainValue())
		{
			label = OpenZWave::Manager::Get()->GetValueLabel(pLmceDevice->GetMainValue());
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING, " * Device id = %s, dt = %d (%s), main label = %s, pk_parent = %d", sId.c_str(), pLmceDevice->m_dwFK_DeviceTemplate, DTToName(pLmceDevice->m_dwFK_DeviceTemplate).c_str(), label.c_str(), pLmceDevice->m_dwPK_Parent_Device );
		if (pLmceDevice->m_dwPK_Device == 0)
		{
			// PK_Device == 0 means it has not been added, or it has been added since last reload
			if ( m_setRecentlyAddedDevices.find(sId) == m_setRecentlyAddedDevices.end() )
			{
				// ok, not added since last reload
				// Get device data from LMCE or create new device if not found
				DeviceData_Impl* pDevice = GetDeviceForPortChannel(sId);
				if (pDevice == NULL)
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "  -> Device with id = %s not found, creating new device, dt = %d", sId.c_str(), pLmceDevice->m_dwFK_DeviceTemplate);
					pLmceDevice->m_dwPK_Device = AddDevice(pLmceDevice->m_dwPK_Parent_Device, sId, pLmceDevice->m_dwFK_DeviceTemplate);
				} else {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "  -> Device exists, PK_Device=%d", pDevice->m_dwPK_Device);
					pLmceDevice->m_dwPK_Device = pDevice->m_dwPK_Device;
				}
			} else {
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "  -> Device added since last reload, reload required!");
			}
		} else {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "  -> Already mapped to device, PK_Device=%d", pLmceDevice->m_dwPK_Device);
		}
		deviceNo++;
	}

}

/**
 * @brief ZWave::SetDeviceTemplate set device template for one LMCEDevice
  * @param pLmceDevice
 */
void ZWave::SetDeviceTemplate(LMCEDevice* pLmceDevice)
{
    // At this point we already have divided the values into devices!

    // Find the value that fits most nicely with the node type and use that as main value
	int i = 0;
    for (vector<OpenZWave::ValueID>::iterator it = pLmceDevice->m_vectValues.begin();
         it != pLmceDevice->m_vectValues.end() && !pLmceDevice->HasMainValue(); ++it)
	{
		OpenZWave::ValueID ourValue = *it;
		string ourLabel = OpenZWave::Manager::Get()->GetValueLabel(ourValue);
        if (pLmceDevice->m_pNodeInfo->m_generic == GENERIC_TYPE_ALARM_SENSOR &&
                (ourLabel == "Smoke" || ourLabel == "Temperature")) {
            // Alarm sensor - smoke sensor ? Some sensors also have a built-in temperature sensor
            pLmceDevice->SetMainValue(i);
        } else if ((pLmceDevice->m_pNodeInfo->m_generic == GENERIC_TYPE_SENSOR_MULTILEVEL ||
                 pLmceDevice->m_pNodeInfo->m_generic == GENERIC_TYPE_SENSOR_BINARY) &&
                (ourLabel == "Temperature" || ourLabel == "Luminance"))
        {
            pLmceDevice->SetMainValue(i);
        } else if (ourLabel == "Sensor" && pLmceDevice->m_pNodeInfo->m_generic != GENERIC_TYPE_ALARM_SENSOR)
        {
            // There are some devices that is a binary switch, but also have a Sensor input,
            // which we need to map. When selecting the main label here, GetDeviceTemplate will give us
            // a generic sensor device - if not we would get the default, which is binary switch.
            pLmceDevice->SetMainValue(i);
        }
		i++;
	}
	// then find the device template for the device
	int PK_Parent_Device = 0;
	unsigned long deviceTemplate = GetDeviceTemplate(pLmceDevice, PK_Parent_Device);
	pLmceDevice->m_dwFK_DeviceTemplate = deviceTemplate;
	pLmceDevice->m_dwPK_Parent_Device = PK_Parent_Device;
}

/**
 * @brief ZWave::GetDeviceTemplate
 * Get the device template for the specified LMCEDevice based on device type and values
 *
 * @param pLmceDevice
 * @param PK_Device_Parent
 * @return
 */
unsigned long ZWave::GetDeviceTemplate(LMCEDevice *pLmceDevice, int& PK_Device_Parent) {
	unsigned long devicetemplate = 0;
	string label = "<none>";
    NodeInfo* node = pLmceDevice->m_pNodeInfo;
    uint32 homeId = pLmceDevice->m_pNodeInfo->m_homeId;
    uint8 nodeId = pLmceDevice->m_pNodeInfo->m_nodeId;
    string manuId = OpenZWave::Manager::Get()->GetNodeManufacturerId(homeId, nodeId);
    string prodType = OpenZWave::Manager::Get()->GetNodeProductType(homeId, nodeId);
    string prodId = OpenZWave::Manager::Get()->GetNodeProductId(homeId, nodeId);

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "GetDeviceTemplate() manufacturer = %s, prodType = %s, prodId = %s", manuId.c_str(), prodType.c_str(), prodId.c_str());

    // If we have a main value we can do some more inteligent detection and special handling
    if (pLmceDevice->HasMainValue())
	{
		label = OpenZWave::Manager::Get()->GetValueLabel(pLmceDevice->GetMainValue());

        if ((manuId == "001e" && prodType == "0002" && pLmceDevice->GetMainValue().GetInstance() == 1)
                // HSM100 motion detector, value with instance = 1 is motion detector
                || (manuId == "0086" && prodType == "0002" && prodId == "0005" && label == "Sensor")
                // Aeotec multi-sensor motion detector
	        || (manuId == "010f" && prodType == "0800" && prodId == "1001" && label == "Sensor")
	        // Fibaro motion sensor, 010f, prodType = 0800, prodId = 1001
	        || (manuId == "011a" && prodType == "0601" && prodId == "0901" && label == "Sensor")
	        // ZWN-BPC z-wave 360 PIR motion sensor
                ) {
            devicetemplate = DEVICETEMPLATE_Motion_Detector_CONST;
        } else if ( label == "Temperature" )
		{
			devicetemplate = DEVICETEMPLATE_Temperature_sensor_CONST;
			PK_Device_Parent = m_dwPK_ClimateInterface;
		} else if ( label == "Luminance" )
		{
            devicetemplate = DEVICETEMPLATE_Brightness_sensor_CONST;
            PK_Device_Parent = m_dwPK_ClimateInterface;
		} else if ( label == "Relative Humidity" )
		{
			devicetemplate = DEVICETEMPLATE_Humidity_sensor_CONST;
			PK_Device_Parent = m_dwPK_ClimateInterface;
        } else if ( label == "Smoke" )
        {
            devicetemplate = DEVICETEMPLATE_Smoke_Detector_CONST;
            PK_Device_Parent = m_dwPK_SecurityInterface;
        } else if ( label == "Sensor" )
		{
            devicetemplate = DEVICETEMPLATE_Generic_Sensor_CONST;
			PK_Device_Parent = m_dwPK_SecurityInterface;
        }
        if (devicetemplate == DEVICETEMPLATE_Motion_Detector_CONST)
            PK_Device_Parent = m_dwPK_SecurityInterface;
		if (devicetemplate > 0)
			return devicetemplate;
	}

    // The ZWave defined generic and specific type are used as fallback.
    // They are useful for/ catching the basic device types but fails for composite devices
    // with multiple sensors/functions in the same node.
	switch(node->m_generic) {
		case GENERIC_TYPE_GENERIC_CONTROLLER:
		case GENERIC_TYPE_STATIC_CONTROLLER:
			devicetemplate = DEVICETEMPLATE_ZWave_Controller_CONST;
			break;
		case GENERIC_TYPE_THERMOSTAT:
			devicetemplate = DEVICETEMPLATE_Standard_Thermostat_CONST;
			break;
		case GENERIC_TYPE_SWITCH_MULTILEVEL:
			switch (node->m_specific) {
				case SPECIFIC_TYPE_MOTOR_MULTIPOSITION:
				case SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL:
				case SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL:
				case SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL:
					devicetemplate = DEVICETEMPLATE_Drapes_Switch_CONST;
					break;
				case SPECIFIC_TYPE_NOT_USED:
				case SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL:
				case SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL:
					devicetemplate = DEVICETEMPLATE_Light_Switch_dimmable_CONST;
					break;
			}
				
			break;
		case GENERIC_TYPE_SWITCH_REMOTE:
			devicetemplate = DEVICETEMPLATE_Remote_Switch_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Remote_Switch");
			break;
		case GENERIC_TYPE_SWITCH_BINARY:
			devicetemplate = DEVICETEMPLATE_Light_Switch_onoff_CONST;
			break;
		case GENERIC_TYPE_SENSOR_BINARY:
			devicetemplate = DEVICETEMPLATE_Generic_Sensor_CONST;
			break;
		case GENERIC_TYPE_WINDOW_COVERING:
			devicetemplate = DEVICETEMPLATE_Drapes_Switch_CONST;
			break;
		case GENERIC_TYPE_SENSOR_MULTILEVEL:
			// TODO try to detect motion detectors

			devicetemplate = DEVICETEMPLATE_Multilevel_Sensor_CONST;
			break;
        case GENERIC_TYPE_SENSOR_ALARM:
			// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "alarm sensor found, specific: %i",specific);
			switch(node->m_specific) {
				case SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR:
				case SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR:
					devicetemplate = DEVICETEMPLATE_Smoke_Detector_CONST;
				break;
			}
		case GENERIC_TYPE_AV_CONTROL_POINT:
			devicetemplate = DEVICETEMPLATE_Generic_Tuner_CONST;
			break;
	        case GENERIC_TYPE_METER:
			// TODO support other types of meters
			devicetemplate = DEVICETEMPLATE_Standard_Energy_Meter_CONST;
			break;
	        case GENERIC_TYPE_ENTRY_CONTROL:
			devicetemplate = DEVICETEMPLATE_Generic_Access_Control_Panel_CONST;
			break;
	        default:
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "    -> No device template for node with generic %d, specific %d, label %s", node->m_generic, node->m_specific, label.c_str());
			break;

	}
	return devicetemplate;
}

string ZWave::DTToName(unsigned long PK_DeviceTemplate)
{
	string name = "<unknown>";
	switch (PK_DeviceTemplate) {
	case DEVICETEMPLATE_ZWave_Controller_CONST:
		name = "ZWave Controller";
		break;
	case  DEVICETEMPLATE_Temperature_sensor_CONST:
		name = "Temperature Sensor";
		break;
	case  DEVICETEMPLATE_Brightness_sensor_CONST:
		name = "Brightness Sensor";
		break;
	case DEVICETEMPLATE_Standard_Thermostat_CONST:
		name = "Standard Thermostat";
		break;
	case DEVICETEMPLATE_Drapes_Switch_CONST:
		name = "Drapes Switch";
		break;
	case DEVICETEMPLATE_Light_Switch_dimmable_CONST:
		name = "Dimmable Light Switch";
		break;
	case DEVICETEMPLATE_Remote_Switch_CONST:
		name = "Remote Switch";
		break;
	case DEVICETEMPLATE_Light_Switch_onoff_CONST:
		name = "Light Switch";
		break;
	case DEVICETEMPLATE_Generic_Sensor_CONST:
		name = "Generic Sensor";
		break;
    case DEVICETEMPLATE_Motion_Detector_CONST:
        name = "Motion Detector";
        break;
    case DEVICETEMPLATE_Multilevel_Sensor_CONST:
		name = "Multilevel Sensor";
		break;
	case DEVICETEMPLATE_Smoke_Detector_CONST:
		name = "Smoke Detector";
		break;
	case DEVICETEMPLATE_Generic_Tuner_CONST:
		name = "Generic Tuner";
		break;
	case DEVICETEMPLATE_Standard_Energy_Meter_CONST:
		name = "Standard Energy Meter";
		break;
	case DEVICETEMPLATE_Siren_CONST:
		name = "Siren";
		break;
    case DEVICETEMPLATE_Generic_Access_Control_Panel_CONST:
        name = "Access Control Panel";
        break;
	}
	return name;
}
int ZWave::AddDevice(int parent, string sInternalId, int PK_DeviceTemplate) {
	int iPK_Device = 0;
	int tmp_parent = 0;
	if (parent > 0) { tmp_parent = parent; } else { tmp_parent = m_dwPK_Device; }

	if ( m_setRecentlyAddedDevices.find(sInternalId) == m_setRecentlyAddedDevices.end() )
	{
		DeviceData_Impl* pDevice = GetDeviceForPortChannel(sInternalId);
		if (pDevice == NULL) {
			// does not exist, create child
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Adding device for node: %s to parent device %d",sInternalId.c_str(), tmp_parent);
			CMD_Create_Device add_command(m_dwPK_Device,4, PK_DeviceTemplate,"",0,"",
			StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalId, 0,tmp_parent,"",0,0,&iPK_Device) ;
			SendCommand(add_command);
			m_setRecentlyAddedDevices.insert(sInternalId);
		} else {
			iPK_Device = pDevice->m_dwPK_Device;
		}
	}
	return iPK_Device;

} 

void ZWave::DeleteDevice(unsigned long PK_Device)
{
	CMD_Delete_Device del_command(m_dwPK_Device,4,PK_Device);
	DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Deleting Device %i",PK_Device);
	SendCommand(del_command);
}

bool ZWave::DeleteDevicesForNode(int iNodeId) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::DeleteDevicesForNode() nodeId = %d", iNodeId);
	bool bDeleted = false;
        DeviceData_Impl *pChildDevice = NULL;
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
			if ( pChildDevice->m_dwPK_Device != m_dwPK_ClimateInterface && pChildDevice->m_dwPK_Device != m_dwPK_SecurityInterface )
			{
				string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
				int nodeId = atoi(tmp_node_id.c_str());
				if ( nodeId == iNodeId ) {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::DeleteDevicesForNode(): Node %d, device %d removed.", nodeId, pChildDevice->m_dwPK_Device);
					DeleteDevice(pChildDevice->m_dwPK_Device);
					bDeleted = true;
				}
			}

			// iterate over embedded interfaces
			DeviceData_Impl *pChildDevice1 = NULL;
			for( VectDeviceData_Impl::const_iterator it1 = pChildDevice->m_vectDeviceData_Impl_Children.begin();
				it1 != pChildDevice->m_vectDeviceData_Impl_Children.end(); ++it1 )
			{
				pChildDevice1 = (*it1);
				if( pChildDevice1 != NULL )
				{
					string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					int nodeId = atoi(tmp_node_id.c_str());
					if ( nodeId == iNodeId ) {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::DeleteDevicesForNode(): Node %d, device %d removed.", nodeId, pChildDevice->m_dwPK_Device);
						DeleteDevice(pChildDevice->m_dwPK_Device);
						bDeleted = true;
					}
				}
			}
		}
	}
	return bDeleted;
}

void ZWave::SendEvent(long dwPK_Device_From, long dwEvent, long dwEventParameterID, string sParameter) 
{
	m_pEvent->SendMessage( new Message(dwPK_Device_From, DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL, MESSAGETYPE_EVENT, dwEvent,
					   1, dwEventParameterID, sParameter.c_str()) );
}

void ZWave::ReportBatteryStatus(int PK_Device, uint8 status)
{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::ReportBatteryStatus(): PK_Device %d, level = %d", PK_Device, status);
	CMD_Set_Device_Data cmd_Set_Device_Data(m_dwPK_Device, 4, PK_Device, StringUtils::itos(status), DEVICEDATA_Battery_state_CONST);
	SendCommand(cmd_Set_Device_Data);
	SendEvent(PK_Device, EVENT_Battery_Level_Changed_CONST, EVENTPARAMETER_Value_CONST, StringUtils::itos(status));
}

void ZWave::SendTemperatureChangedEvent(unsigned int PK_Device, float value)
{
	string sVal = StringUtils::Format("%.2f", value);
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sending EVENT_Temperature_Changed_CONST event from PK_Device %d, value %s", PK_Device, sVal.c_str());
	SendEvent(PK_Device, EVENT_Temperature_Changed_CONST, EVENTPARAMETER_Value_CONST, sVal);
}

void ZWave::SendSensorTrippedEvent(unsigned int PK_Device, bool value)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Sensor_Tripped_CONST event from PK_Device %d, value %s", PK_Device, (value ? "true" : "false"));
	SendEvent(PK_Device, EVENT_Sensor_Tripped_CONST, EVENTPARAMETER_Tripped_CONST, value ? "1" : "0");
}

void ZWave::HandleDeviceChangedEvents(unsigned int PK_Device, int value) {
	if (value == 0 || value == 100)
	{
		SendOnOffEvent(PK_Device, value);
	} else {
		SendLightChangedEvents(PK_Device, value);
	}
}

void ZWave::SendLightChangedEvents(unsigned int PK_Device, int value)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_State_Changed_CONST event from PK_Device %d, level %d",PK_Device, value);
	SendEvent(PK_Device, EVENT_State_Changed_CONST, EVENTPARAMETER_State_CONST, StringUtils::itos(value));
}

void ZWave::SendPowerUsageChangedEvent(unsigned int PK_Device, int value)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Power_Usage_Changed_CONST event from PK_Device %d, value %d W",PK_Device, value);
	SendEvent(PK_Device, EVENT_Power_Usage_Changed_CONST, EVENTPARAMETER_Watts_CONST, StringUtils::itos(value));
}

void ZWave::SendPowerUsageCumulativeChangedEvent(unsigned int PK_Device,float value)
{
	string sVal = StringUtils::Format("%.3f", value);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Power_Usage_Changed_CONST event from PK_Device %d, cumulated kWh %s kWh",PK_Device, sVal.c_str());
	SendEvent(PK_Device, EVENT_Power_Usage_Changed_CONST, EVENTPARAMETER_WattsMTD_CONST, sVal);
}

void ZWave::SendVoltageChangedEvent(unsigned int PK_Device, int value)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Voltage_Changed_CONST event from PK_Device %d, value %d V",PK_Device, value);
	SendEvent(PK_Device, EVENT_Voltage_Changed_CONST, EVENTPARAMETER_Voltage_CONST, StringUtils::itos(value));
}

void ZWave::SendOnOffEvent(unsigned int PK_Device, int value) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_OnOff_CONST event from PK_Device %d, value %d",PK_Device, value);
	SendEvent(PK_Device, EVENT_Device_OnOff_CONST, EVENTPARAMETER_OnOff_CONST, value == 0 ? "0" : "1");
}

void ZWave::SendBrightnessChangedEvent(unsigned int PK_Device, float value)
{
	string sVal = StringUtils::Format("%.1f", value);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Brightness_Changed_CONST event from PK_Device %d, value %d",PK_Device, sVal.c_str());
	SendEvent(PK_Device, EVENT_Brightness_Changed_CONST, EVENTPARAMETER_Value_CONST, sVal.c_str());
}

void ZWave::SendSetpointChangedEvent(unsigned int PK_Device, float value)
{
	string sVal = StringUtils::Format("%.0f", value);
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending EVENT_Thermostat_Set_Point_Chan_CONST event from PK_Device %d, value %s",PK_Device, sVal.c_str());
	SendEvent(PK_Device, EVENT_Thermostat_Set_Point_Chan_CONST, EVENTPARAMETER_Value_CONST, sVal.c_str());
}

void ZWave::SendRelativeHumidityChangedEvent(unsigned int PK_Device, float value)
{
	string sVal = StringUtils::Format("%.0f", value);
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending EVENT_Humidity_Changed_CONST event from PK_Device %d, value %s",PK_Device, sVal.c_str());
	SendEvent(PK_Device, EVENT_Humidity_Changed_CONST, EVENTPARAMETER_Value_CONST, sVal.c_str());
}

void ZWave::HandleHouseModeChange(unsigned int PK_Device, string id)
{
	DeviceData_Impl* pDevice = GetData()->FindChild(PK_Device);
	string mappingString = pDevice->m_mapParameters_Find(DEVICEDATA_Mapping_CONST);
	
        // Split mappings into lines and compare with id
	vector<string> mappings;
	StringUtils::Tokenize(mappingString, "\n", mappings);
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Alarm Report: id = %s, mapping = %s", id.c_str(), mappingString.c_str());
	for (int i = 0; i < mappings.size(); i++) {
		vector<string> mapping;
		StringUtils::Tokenize(mappings[i], ",", mapping);
		if (mapping[0] == id) {
			// See what to do for that event id
			string houseMode = mapping[1];
			// Only change house mode when it is actually a change
			if (atoi(houseMode.c_str()) != m_iHouseMode)
			{
				string bypassMode = "";
				if (mapping[2].size())
					bypassMode = mapping[2];
				string passwd = pDevice->m_mapParameters_Find(DEVICEDATA_Password_CONST);
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending Set_House_Mode, mode = %s, bypass = %s", houseMode.c_str(), bypassMode.c_str());
				DCE::CMD_Set_House_Mode_DT CMD_Set_House_Mode(PK_Device,DEVICETEMPLATE_Security_Plugin_CONST,BL_SameHouse,houseMode,0,passwd,0,bypassMode);
				SendCommand(CMD_Set_House_Mode);
			}
		}
	}
}
//<-dceag-c870-b->

	/** @brief COMMAND: #870 - Get Data */
	/**  */
		/** @param #9 Text */
			/** What data to return.  This is free form */
		/** @param #19 Data */
			/** The data being returned */

void ZWave::CMD_Get_Data(string sText,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c870-e->
{
	string s = "";
	s += "{ ";
	if (sText == "status")
	{
		s += " \"statusList\": [";
		bool added = false;
		while (m_listStatus.size() > 0) 
		{
			s+= "{ \"description\": \"" + m_listStatus.front() + "\" },";
			m_listStatus.pop_front();
			added = true;
		}
		if (added)
			s = s.substr(0, s.length()-1);
		s += "]";
	} else {
		s += "\"nodes\" : [";
		list<NodeInfo*>::iterator it;
		list<NodeInfo*> nodes = m_pZWInterface->GetNodes();
		for (it = nodes.begin(); it != nodes.end(); it++)
		{
			NodeInfo *node = *it;
			uint8 node_id = node->m_nodeId;
			uint32 homeId = m_pZWInterface->GetHomeId();
			s += "{ \"id\" : " + StringUtils::itos(node_id) + ",";
			
			s += " \"manufacturerName\": \""+OpenZWave::Manager::Get()->GetNodeManufacturerName(homeId, node_id)+"\",";
			s += " \"productName\": \""+OpenZWave::Manager::Get()->GetNodeProductName(homeId, node_id)+"\",";
			s += " \"queryStage\": \""+OpenZWave::Manager::Get()->GetNodeQueryStage(homeId, node_id)+"\",";
			s += " \"isFailed\": ";
			if (OpenZWave::Manager::Get()->IsNodeFailed(homeId, node_id))
				s += "true";
			else
				s+= "false";
			s += ",";
			s += " \"isAwake\": ";
			if (OpenZWave::Manager::Get()->IsNodeAwake(homeId, node_id))
				s += "true";
			else
				s += "false";
			s += ",";
			
			s += " \"devices\": [";
			for (vector<LMCEDevice*>::iterator it = node->m_vectDevices.begin(); it != node->m_vectDevices.end(); ++it)
			{
				LMCEDevice* pLmceDevice = *it;
				s += "{ \"pkDevice\": " + StringUtils::itos(pLmceDevice->m_dwPK_Device) + ",";
				s += " \"fkDeviceTemplate\": " + StringUtils::itos(pLmceDevice->m_dwFK_DeviceTemplate);
				s += "},";
			}
			if (node->m_vectDevices.size() > 0)
				s = s.substr(0, s.length()-1);
			s += "],";
			
			uint8 *neighbors;
			int neighborsCount = OpenZWave::Manager::Get()->GetNodeNeighbors(homeId, node_id, &neighbors);
			s += "\"neighbors\": [";
			for (int i = 0; i < neighborsCount; i++) {
				s += StringUtils::itos(neighbors[i])+",";
			}
			if (neighborsCount > 0)
				s = s.substr(0, s.length()-1);
			delete[] neighbors;
			s += "]";
			
			OpenZWave::Node::NodeData nodeData;
			OpenZWave::Manager::Get()->GetNodeStatistics(homeId, node_id, &nodeData);
			s += ", \"statistics\": {";
			s+= "\"sentCnt\":" + StringUtils::itos(nodeData.m_sentCnt) + ",";
			s+= "\"sentFailed\":" + StringUtils::itos(nodeData.m_sentFailed) + ",";
			s+= "\"retries\":" + StringUtils::itos(nodeData.m_retries) + ",";
			s+= "\"receivedCnt\":" + StringUtils::itos(nodeData.m_receivedCnt) + ",";
			s+= "\"receivedDups\":" + StringUtils::itos(nodeData.m_receivedDups) + ",";
			s+= "\"receivedUnsolicited\":" + StringUtils::itos(nodeData.m_receivedUnsolicited) + ",";
			s+= "\"sentTS\":\"" + nodeData.m_sentTS + "\",";
			s+= "\"receivedTS\":\"" + nodeData.m_receivedTS + "\",";
			s+= "\"lastRequestRTT\":" + StringUtils::itos(nodeData.m_lastRequestRTT) + ",";
			s+= "\"averageRequestRTT\":" + StringUtils::itos(nodeData.m_averageRequestRTT) + ",";
			s+= "\"lastResponseRTT\":" + StringUtils::itos(nodeData.m_lastResponseRTT) + ",";
			s+= "\"averageResponseRTT\":" + StringUtils::itos(nodeData.m_averageResponseRTT) + ",";
			s+= "\"quality\":" + StringUtils::itos(nodeData.m_quality);
			/*      uint8 m_lastReceivedMessage[254];
				list<CommandClassData> m_ccData;   */
			s += "},";
			
			s += " \"values\": [";
			int valCount = 0;
			for (vector<LMCEDevice*>::iterator nodeit = node->m_vectDevices.begin(); nodeit != node->m_vectDevices.end(); ++nodeit)
			{
				LMCEDevice* pLmceDevice = *nodeit;
				for (vector<OpenZWave::ValueID>::iterator it = pLmceDevice->m_vectValues.begin(); it != pLmceDevice->m_vectValues.end(); ++it)
				{
					OpenZWave::ValueID value = *it;
					s += "{ \"label\": \"" + OpenZWave::Manager::Get()->GetValueLabel(value) + "\",";
					s += " \"help\": \"" + StringUtils::Replace(OpenZWave::Manager::Get()->GetValueHelp(value), "\"", "&quot;") + "\",";
					s += " \"index\": " + StringUtils::itos(value.GetIndex()) + ",";
					string val;
					OpenZWave::Manager::Get()->GetValueAsString(value, &val);
					s += " \"value\": \"" + val + "\",";
					s += " \"units\": \"" + OpenZWave::Manager::Get()->GetValueUnits(value) + "\",";
					s += " \"min\": " + StringUtils::itos(OpenZWave::Manager::Get()->GetValueMin(value)) + ",";
					s += " \"max\": " + StringUtils::itos(OpenZWave::Manager::Get()->GetValueMax(value)) + ",";
					string genreText = string(OpenZWave::Value::GetGenreNameFromEnum(value.GetGenre()));
					s += " \"genre\": \"" + genreText + "\",";
					s += " \"pk_device\": " + StringUtils::itos(pLmceDevice->m_dwPK_Device) + ",";
					string polled = OpenZWave::Manager::Get()->isPolled(value) ? "true" : "false";
					s += " \"polling\": " + polled;
					s += "},";
					valCount++;
				}
			}
			if (valCount > 0)
				s = s.substr(0, s.length()-1);
			s += "],";
			
			s += " \"associationGroups\": [";
			uint8 numGroups = OpenZWave::Manager::Get()->GetNumGroups(homeId, node_id);
			for (int i = 0; i < numGroups; i++)
			{
				s += "{ \"name\": \"" + OpenZWave::Manager::Get()->GetGroupLabel(homeId, node_id, i+1) + "\",";
				uint8 maxAssoc = OpenZWave::Manager::Get()->GetMaxAssociations(homeId, node_id, i+1);
				uint8* assoc = new uint8[maxAssoc];
				uint8 numAssoc = OpenZWave::Manager::Get()->GetAssociations(homeId, node_id, i+1, &assoc);
				s += " \"nodes\": [";
				for (int j = 0; j < numAssoc; j++) {
					s += StringUtils::itos((int)assoc[j]) + ",";
				}
				delete[] assoc;
				if (numAssoc > 0)
					s = s.substr(0, s.length() - 1);
				s += "]";
				s += "},";
			}
			if (numGroups > 0)
				s = s.substr(0, s.length()-1);
			s += "]";
			
			s += "},";
		}
		s = s.substr(0, s.length()-1);
		s += "]";
	}
	s += "}";

	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Get Data() returning %s", s.c_str());	
	(*pData) = new char[s.length()+1];
	strcpy((*pData), s.c_str());
	(*iData_Size) = s.length();
}

void ZWave::AlarmCallback(int id, void* param)
{
	if( id==ALARM_NODE_DEAD )
	{
		NodeInfo* nodeInfo = (NodeInfo*)param;
		nodeInfo->m_iDeadCount++;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AlarmCallback(): trying to revive node %d for the %d time", nodeInfo->m_nodeId, nodeInfo->m_iDeadCount);
		// Trying to revive the node by issuing a test node command
		OpenZWave::Manager::Get()->TestNetworkNode(m_pZWInterface->GetHomeId(), nodeInfo->m_nodeId, 1);
		// TODO: implement more logic around this. here are some ideas:
		// - revive node X times - then alert user (email/popup)
		// - if node is brought back to life, try heal node to improve network?
	}
}
