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

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
ZWave::ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: ZWave_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pZWInterface = NULL;
	m_bReady = false;
	m_setRecentlyAddedDevices.clear();
	
	m_dwPK_ClimateInterface = 0;
	m_dwPK_SecurityInterface = 0;
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command for child");
	string nodeInstance = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	int node_id;
	int instance_id;
	// TODO handle nodeid/cc/instance
	if (nodeInstance.find("/") != string::npos) {
	        vector<string> vectNI;
		StringUtils::Tokenize(nodeInstance, "/", vectNI);
		node_id = atoi(vectNI[0].c_str());
		instance_id = atoi(vectNI[1].c_str());
	} else {
	        node_id = atoi(nodeInstance.c_str());
		instance_id = 1;
	}
	// TODO: use instance id in commands below
	if (node_id > 0 && node_id <= 233) {
		sCMD_Result = "OK";
		int level,duration,temp,fan;
		string heat;
		uint32 homeId = m_pZWInterface->GetHomeId();
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
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET ON/OFF/LEVEL RECEIVED FOR CHILD %d, level: %d",node_id,level);

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
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET TEMPERATURE RECEIVED FOR CHILD %d, temperature: %d",node_id,temp);
/*				// tempf = (int)( (9.0/5.0) * (float)temp + 32.0 );
				myZWApi->zwThermostatSetpointSet(node_id,1,temp); // heating
				myZWApi->zwThermostatSetpointSet(node_id,2,temp); // cooling
				myZWApi->zwThermostatSetpointSet(node_id,10,temp); // auto changeover
*/
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
	OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_ReceiveConfiguration, controller_update, (void*)NULL, true, 0, 0);
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
		OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_RequestNodeNeighborUpdate, controller_update, NULL, false, nodeId, 0);
	} else if (StringUtils::StartsWith(sArguments,"NU")) {
		uint8 nodeId = atoi(sArguments.substr(2).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() RequestNetworkUpdate node %d", nodeId);
		OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_RequestNetworkUpdate, controller_update, NULL, false, nodeId, 0);
	} else if (StringUtils::StartsWith(sArguments,"HNN")) {
		uint8 nodeId = atoi(sArguments.substr(3).c_str());
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() HealNodeNetwork node %d", nodeId);
		OpenZWave::Manager::Get()->HealNetworkNode(m_pZWInterface->GetHomeId(), nodeId, true);
	} else if (StringUtils::StartsWith(sArguments,"HN")) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::StatusReport() HealNetwork");
		OpenZWave::Manager::Get()->HealNetwork(m_pZWInterface->GetHomeId(), true);
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

	OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_AssignReturnRoute, controller_update, NULL, false, iNodeID, iDestNodeID);
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
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::OnNotification() Adding association: (node %d, group %d) -> (node %d)", iNodeID, iGroup_ID, targetId);
				OpenZWave::Manager::Get()->AddAssociation(m_pZWInterface->GetHomeId(), iNodeID, iGroup_ID, targetId);
			} else if (targetId < 0)
			{
				uint8 id = -targetId;
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::OnNotification() Removing association: (node %d, group %d) -> (node %d)", iNodeID, iGroup_ID, id);
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
	cout << "Need to implement command #966 - Set Polling State" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #220 - Report=" << bReport << endl;
	cout << "Parm #225 - Always=" << bAlways << endl;
	cout << "Parm #239 - NodeID=" << iNodeID << endl;
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
		OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_AddDevice, controller_update, NULL, sOptions == "H");
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
		OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_RemoveFailedNode, controller_update, NULL, true, nodeId);
	} else {
		OpenZWave::Manager::Get()->BeginControllerCommand(m_pZWInterface->GetHomeId(), OpenZWave::Driver::ControllerCommand_RemoveDevice, controller_update, NULL, true);
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


void ZWave::controller_update(OpenZWave::Driver::ControllerState state, OpenZWave::Driver::ControllerError error, void *context) {
	DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"controller state update:");
	switch(state) {
		case OpenZWave::Driver::ControllerState_Normal:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"no command in progress");
			// nothing to do
			break;
		case OpenZWave::Driver::ControllerState_Waiting:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"waiting for user action");
			// waiting for user action
			break;
		case OpenZWave::Driver::ControllerState_InProgress:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"communicating with other device");
			// communicating with device
			break;
		case OpenZWave::Driver::ControllerState_Completed:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"command has completed successfully");
			break;
		case OpenZWave::Driver::ControllerState_Failed:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"command has failed");
			// houston..
			break;
		case OpenZWave::Driver::ControllerState_NodeOK:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"node ok");
			break;
		case OpenZWave::Driver::ControllerState_NodeFailed:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"node failed");
			break;
		default:
			DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"unknown response");
			break;
	}
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
			if ( label == "Switch" || label == "Battery Level" || label == "Temperature" || label == "Luminance" ||
			     label == "Power" || label == "Voltage" || label == "Energy" ||
			     label == "Setpoint" || label == "Heating 1" || label == "Cooling 1" || label == "Auto Changeover" ||
			     label == "Mode" )
			{
				// intensity = poll this value every X poll cycle/interval
				// When polling interval is 60000 (60 seconds), this value effectively becomes every X second
				uint8 intensity = 1;
				if ( label == "Battery Level" ) {
					intensity = 255; // battery level does not change very often
				} else if ( label == "Temperature" || label == "Mode" || label == "Energy" || label == "Power") {
					intensity = 10; // neither does temperature
				} else if ( label == "Setpoint" || label == "Heating 1" || label == "Cooling 1" || label == "Auto Changeover" ) {
					intensity = 15;
				}
				DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ZWave::OnNotification() ValueAdded: Set polling for node %d/%d/%d, value label %s, intensity %d", _notification->GetNodeId(), id.GetCommandClassId(), id.GetInstance(), label.c_str(), intensity);
				OpenZWave::Manager::Get()->EnablePoll(id, intensity);
			}
		}
		break;
	}

	case OpenZWave::Notification::Type_ValueChanged:
	{
		if( nodeInfo != NULL )
		{
			// One of the node values has changed
			OpenZWave::ValueID id = _notification->GetValueID();
			string label = OpenZWave::Manager::Get()->GetValueLabel(id);

			int PKDevice = GetPKDevice(nodeInfo->m_nodeId, id.GetCommandClassId(), id.GetInstance());
			if (PKDevice > 0)
			{
				if ( label == "Battery Level" )
				{
					int level = 0;
					OpenZWave::Manager::Get()->GetValueAsInt(id, &level);
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
					SendLightChangedEvents (PKDevice, level);
				} else if ( label == "Switch" ) {
					bool state=false;
					OpenZWave::Manager::Get()->GetValueAsBool(id, &state);
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event");
					SendLightChangedEvents (PKDevice, state ? 100 : 0);
				} else if ( label == "Luminance" ) {
					float level = 0;
					OpenZWave::Manager::Get()->GetValueAsFloat(id, &level);
					SendBrightnessChangedEvent(PKDevice, level);
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

			int PKDevice = GetPKDevice(nodeInfo->m_nodeId, id.GetCommandClassId(), id.GetInstance());
			if (PKDevice > 0)
			{
				uint8 typeGeneric = OpenZWave::Manager::Get()->GetNodeGeneric(_notification->GetHomeId(), _notification->GetNodeId());
				uint8 typeBasic = OpenZWave::Manager::Get()->GetNodeBasic(_notification->GetHomeId(), _notification->GetNodeId());
				if  ( typeGeneric == GENERIC_TYPE_SWITCH_REMOTE || typeBasic == BASIC_TYPE_CONTROLLER ) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed: level = %d", value);
					SendOnOffEvent (PKDevice, value);
					
				} else if (typeGeneric == GENERIC_TYPE_SENSOR_BINARY || typeGeneric == GENERIC_TYPE_SENSOR_MULTILEVEL) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"This is a binary sensor, so we send sensor tripped event: value = %d", value);
					SendSensorTrippedEvent(PKDevice, value);
				} else if (typeGeneric == GENERIC_TYPE_SWITCH_BINARY || typeGeneric == GENERIC_TYPE_SWITCH_MULTILEVEL) {
					DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"State changed, send light changed event: value = %d", value);
					SendLightChangedEvents (PKDevice, value);
				}
			}
		}
		break;
	}
	case OpenZWave::Notification::Type_NodeAdded:
	{

		break;
	}	
	case OpenZWave::Notification::Type_NodeRemoved:
	{
		// Note: cannot use nodes list from zwinterface at this point, as the node has already been deleted
		// TODO: removed for now as it seem to trigger this when doing some controller commands
                //DeleteDevicesForNode(_notification->GetNodeId());
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

	return new ZWConfigData(port);
}

DeviceData_Impl *ZWave::GetDevice(int iNodeId, uint8 iCommandClass, int iInstanceID) {
        DeviceData_Impl *pChildDevice = NULL;

	if ( iNodeId <= 0 ) return NULL;
	string sInternalIDInst = StringUtils::itos(iNodeId);
	if (iInstanceID > 0) {
	        sInternalIDInst += "/" + StringUtils::itos(iCommandClass) + "/" + StringUtils::itos(iInstanceID);
	}
	pChildDevice = GetDeviceForPortChannel(sInternalIDInst);
	// if not found with instance id, look up without command class first, then try without instance id
	if (pChildDevice == NULL) {
		sInternalIDInst = StringUtils::itos(iNodeId) + "/" + StringUtils::itos(iInstanceID);
		pChildDevice = GetDeviceForPortChannel(sInternalIDInst);
	}
	pChildDevice = GetDeviceForPortChannel(sInternalIDInst);
	if (pChildDevice == NULL) {
		sInternalIDInst = StringUtils::itos(iNodeId);
		pChildDevice = GetDeviceForPortChannel(sInternalIDInst);
	}
	if (pChildDevice == NULL) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::GetDevice() No device found for id %s", sInternalIDInst.c_str());
	}
	return pChildDevice;
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

int ZWave::GetPKDevice(int iNodeId, uint8 iCommandClass, int iInstanceID) {
	DeviceData_Impl *pDevice = GetDevice(iNodeId, iCommandClass, iInstanceID);
	if (pDevice != NULL)
		return pDevice->m_dwPK_Device;
	else
		return -1;
}

void ZWave::DoNodeToDeviceMapping()
{
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

void ZWave::MapNodeToDevices(NodeInfo* node)
{
	DeviceData_Impl* pDevice = GetDeviceForPortChannel(StringUtils::itos(node->m_nodeId));
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, " * Node id=%d (PK_Device=%d)", node->m_nodeId, pDevice != NULL ? pDevice->m_dwPK_Device : 0);
	if (pDevice == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "  -> DEVICE NOT FOUND! Adding new device!");
	}
	if (node->m_values.size() > 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "  -> Values:");
		list<OpenZWave::ValueID>::iterator valIt;
		for ( valIt = node->m_values.begin(); valIt != node->m_values.end(); valIt++)
		{	
			OpenZWave::ValueID value = *valIt;
			string label = OpenZWave::Manager::Get()->GetValueLabel(value);
			DeviceData_Impl* pDevice_Inst = GetDevice(node->m_nodeId, value.GetCommandClassId(), value.GetInstance());
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "   - Instance=%d, class=%d, label=%s, type=%s (PK_Device=%d)", value.GetInstance(), value.GetCommandClassId(), label.c_str(), OpenZWave::Value::GetTypeNameFromEnum(value.GetType()), pDevice_Inst != NULL ? pDevice_Inst->m_dwPK_Device : 0);

			// TODO : create correct ID, also see TODO below
			string sId = StringUtils::itos(node->m_nodeId);
			if ( value.GetInstance() > 1 ) {
				sId += "/" + StringUtils::itos(value.GetInstance());
			}
			// Check that this device has not been added since the last reload
			// (in which case it wont be returned from GetDeviceForPortChannel)
			if ( m_setRecentlyAddedDevices.find(sId) != m_setRecentlyAddedDevices.end() )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Has been added since last reload - reload required!");
			} else {
				if (pDevice_Inst == NULL)
				{
					int PK_Parent_Device = 0;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> NOT FOUND! Adding new device!");
					int deviceTemplate = GetDeviceTemplate(node, value, PK_Parent_Device);
			/** TODO: Should ideally add devices for every value we are interested in, but there are some problems:
			 * Some devices have duplicate instance numbers for a second command class 
			 * (Fibaro universal sensor, for instance, has instance 1 and 2 for both binary sensor and for temperature sensor)
			 * This is solved by adding the command class to the port/channel id string.
			 * But we dont want to add all values in a device, only the ones that are interesting for us
			 * There are several command classes for the first instance(main instance) - this is still to be considered only one device, so how to
			 * tell the difference between those ccs and the ccs we need to add several devices for?
			 * - probably using the CC itself, binary and multilevel sensor is likely separate devices
			 * In some cases there are two CCs on instance 1 that still belongs to the same device
			 * (HSM100 has a binary sensor CC and a multilevel sensor on instance 1, and they belong to the same device, motion detector)
			 * So in that case the above does not fully apply.
			 */
					AddDevice(PK_Parent_Device, sId, deviceTemplate);
				}
			}
		}
	}
}

int ZWave::GetDeviceTemplate(NodeInfo* node, OpenZWave::ValueID value, int& PK_Device_Parent) {
	int devicetemplate = 0;
	uint8 generic = OpenZWave::Manager::Get()->GetNodeGeneric(node->m_homeId, node->m_nodeId);
	uint8 specific = OpenZWave::Manager::Get()->GetNodeSpecific(node->m_homeId, node->m_nodeId);
	string label = OpenZWave::Manager::Get()->GetValueLabel(value);
	
	if ( label == "Temperature" )
	{
		devicetemplate = DEVICETEMPLATE_Temperature_sensor_CONST;
		PK_Device_Parent = m_dwPK_ClimateInterface;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Temperature_Sensor");
		return devicetemplate;
	}

	switch(generic) {
		case GENERIC_TYPE_GENERIC_CONTROLLER:
		case GENERIC_TYPE_STATIC_CONTROLLER:
			devicetemplate = DEVICETEMPLATE_ZWave_Controller_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> ZWave_Controller");
			break;
		case GENERIC_TYPE_THERMOSTAT:
			devicetemplate = DEVICETEMPLATE_Standard_Thermostat_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Standard_Thermostat");
			break;
		case GENERIC_TYPE_SWITCH_MULTILEVEL:
			switch (specific) {
				case SPECIFIC_TYPE_MOTOR_MULTIPOSITION:
				case SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL:
				case SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL:
				case SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL:
					devicetemplate = DEVICETEMPLATE_Drapes_Switch_CONST;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Drapes_Switch");
					break;
				case SPECIFIC_TYPE_NOT_USED:
				case SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL:
				case SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL:
					devicetemplate = DEVICETEMPLATE_Light_Switch_dimmable_CONST;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Light_Switch");
					break;
			}
				
			break;
		case GENERIC_TYPE_SWITCH_REMOTE:
			devicetemplate = DEVICETEMPLATE_Remote_Switch_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Remote_Switch");
			break;
		case GENERIC_TYPE_SWITCH_BINARY:
			devicetemplate = DEVICETEMPLATE_Light_Switch_onoff_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Light_Switch_OnOff");
			break;
		case GENERIC_TYPE_SENSOR_BINARY:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Generic_Sensor");
			devicetemplate = DEVICETEMPLATE_Generic_Sensor_CONST;
			break;
		case GENERIC_TYPE_WINDOW_COVERING:
			devicetemplate = DEVICETEMPLATE_Drapes_Switch_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Drapes_Switch");
			break;
		case GENERIC_TYPE_SENSOR_MULTILEVEL:
			devicetemplate = DEVICETEMPLATE_Multilevel_Sensor_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Multilevel_Sensor");
			break;
		case GENERIC_TYPE_SENSOR_ALARM:
			// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "alarm sensor found, specific: %i",specific);
			switch(specific) {
				case SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR:
				case SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR:
				case SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR:
					devicetemplate = DEVICETEMPLATE_Smoke_Detector_CONST;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Smoke_Detector");
				break;
			}
		case GENERIC_TYPE_AV_CONTROL_POINT:
			devicetemplate = DEVICETEMPLATE_Generic_Tuner_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Generic_Tuner");
			break;
	        case GENERIC_TYPE_METER:
			// TODO support other types of meters
			devicetemplate = DEVICETEMPLATE_Standard_Energy_Meter_CONST;
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "    -> Standard Energy Meter");
			break;
	}
	return devicetemplate;
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

void ZWave::ReportBatteryStatus(int PK_Device, int status)
{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::ReportBatteryStatus(): PK_Device %d", PK_Device);
	CMD_Set_Device_Data cmd_Set_Device_Data(m_dwPK_Device, 4, PK_Device, StringUtils::itos(status), DEVICEDATA_Battery_state_CONST);
	SendCommand(cmd_Set_Device_Data);
}

void ZWave::SendTemperatureChangedEvent(unsigned int PK_Device, float value)
{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::SendTemperatureChangedEvent(): PK_Device %d", PK_Device);
	char tempstr[512];
	sprintf(tempstr, "%.2f", value);
	m_pEvent->SendMessage( new Message(PK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Temperature_Changed_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
		);

}

void ZWave::SendSensorTrippedEvent(unsigned int PK_Device, bool value)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending sensor tripped event from PK_Device %d, value %s", PK_Device, (value ? "true" : "false"));
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Sensor_Tripped_CONST,
					   1,
					   EVENTPARAMETER_Tripped_CONST,
					   value ? "1" : "0")
		);
}

void ZWave::SendLightChangedEvents(unsigned int PK_Device, int value)
{
	string svalue = StringUtils::itos(value);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_State_Changed_CONST event from PK_Device %d, level %s",PK_Device,svalue.c_str());
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_State_Changed_CONST,
					   1,
					   EVENTPARAMETER_State_CONST,
					   svalue.c_str())
		);
}

void ZWave::SendPowerUsageChangedEvent(unsigned int PK_Device, int value)
{
	string svalue = StringUtils::itos(value);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Power_Usage_Changed_CONST event from PK_Device %d, value %s W",PK_Device,svalue.c_str());
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Power_Usage_Changed_CONST,
					   1,
					   EVENTPARAMETER_Watts_CONST,
					   svalue.c_str())
		);
}
void ZWave::SendPowerUsageCumulativeChangedEvent(unsigned int PK_Device,float value)
{
	char tempstr[512];
	sprintf(tempstr, "%.3f", value);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Power_Usage_Changed_CONST event from PK_Device %d, cumulated kWh %s kWh",PK_Device, tempstr);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Power_Usage_Changed_CONST,
					   1,
					   EVENTPARAMETER_WattsMTD_CONST,
					   tempstr)
		);
}

void ZWave::SendVoltageChangedEvent(unsigned int PK_Device, int value)
{
	string svalue = StringUtils::itos(value);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_Voltage_Changed_CONST event from PK_Device %d, value %s V",PK_Device,svalue.c_str());
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Voltage_Changed_CONST,
					   1,
					   EVENTPARAMETER_Voltage_CONST,
					   svalue.c_str())
		);
}

void ZWave::SendOnOffEvent(unsigned int PK_Device, int value) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending EVENT_OnOff_CONST event from PK_Device %d, value %d",PK_Device, value);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,
					   EVENT_Device_OnOff_CONST,
					   1,
					   EVENTPARAMETER_OnOff_CONST,
					   (value == 0) ? "0" : "1")
		);
}

void ZWave::SendBrightnessChangedEvent(unsigned int PK_Device, int value)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending brightness level changed event from PK_Device %d, value %d",PK_Device, value);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Brightness_Changed_CONST, 1, 
					   EVENTPARAMETER_Value_CONST, StringUtils::itos(value).c_str())
		);
}

void ZWave::SendSetpointChangedEvent(unsigned int PK_Device, float value)
{
	char tempstr[512];
	int iValue = round(value);
	sprintf(tempstr, "%d", iValue);
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending setpoint changed event from PK_Device %d, value %d",PK_Device, iValue);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Thermostat_Set_Point_Chan_CONST, 1, 
					   EVENTPARAMETER_Value_CONST, tempstr)
		);
}
