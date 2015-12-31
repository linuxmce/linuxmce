/*
 *      Copyright (C) 2008 Harald Klein <hari@vt100.at>
 *
 *      This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      See the GNU General Public License for more details.
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

int sequence=1;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "PlutoUtils/LinuxSerialUSB.h"

#include "ZWApi.h"
#include <deque>
#include <math.h>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
ZWave::ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: ZWave_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	// string port = DATA_Get_COM_Port_on_PC();
	myZWApi=new ZWApi::ZWApi(this);
	myDeviceID=DeviceID;

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);

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
	
	m_pAlarmManager->Stop();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}

//<-dceag-getconfig-b->
bool ZWave::GetConfig()
{
	if( !ZWave_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	string port = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	myZWApi->init(port.c_str());
	// give the z-wave stack some time to settle
	// sleep(12);
	// CMD_Report_Child_Devices();

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
	if (nodeInstance.find("/") != string::npos) {
	        vector<string> vectNI;
		StringUtils::Tokenize(nodeInstance, "/", vectNI);
		node_id = atoi(vectNI[0].c_str());
		instance_id = atoi(vectNI[1].c_str());
	} else {
	        node_id = atoi(nodeInstance.c_str());
		instance_id = 0;
	}
	// TODO: use instance id in commands below
	if (node_id > 0 && node_id <= 233) {
		sCMD_Result = "OK";
		int level,duration,temp,fan;
		string heat;
		switch (pMessage->m_dwID) {
			case COMMAND_Generic_On_CONST:
			        LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ON RECEIVED FOR CHILD %d/%d",node_id, instance_id);
				myZWApi->zwBasicSet(node_id,255,instance_id);
				break;
				;;
			case COMMAND_Generic_Off_CONST:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"OFF RECEIVED FOR CHILD %d",node_id);
				myZWApi->zwBasicSet(node_id,0,instance_id);
				break;
				;;
			case COMMAND_Set_Level_CONST:
				level = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
				duration = 0;
				// duration = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Duration_CONST].c_str());
				if (duration) {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET LEVEL RECEIVED FOR CHILD %d, level: %d, duration: %d",node_id,level,duration);
					myZWApi->zwMultilevelSet(node_id,level>99?99:level,duration,instance_id);
	
				} else {
					LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET LEVEL RECEIVED FOR CHILD %d, level: %d",node_id,level);
					myZWApi->zwBasicSet(node_id,level>99?99:level,instance_id);
				}
				break;
				;;
			case COMMAND_Set_Temperature_CONST:
				temp = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET TEMPERATURE RECEIVED FOR CHILD %d, level: %d",node_id,temp);
				// tempf = (int)( (9.0/5.0) * (float)temp + 32.0 );
				myZWApi->zwThermostatSetpointSet(node_id,1,temp); // heating
				myZWApi->zwThermostatSetpointSet(node_id,2,temp); // cooling
				myZWApi->zwThermostatSetpointSet(node_id,10,temp); // auto changeover
				SendSetpointChangedEvent(node_id, instance_id, (float)temp);
				break;
				;;
			case COMMAND_Set_Fan_CONST:
				fan = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str());
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET FAN RECEIVED FOR CHILD %d, level: %d",node_id,fan);
				if (fan == 1) {
					myZWApi->zwThermostatFanModeSet(node_id,3); // on high
					myZWApi->zwThermostatModeSet(node_id,6); // fan only
				} else {
					myZWApi->zwThermostatFanModeSet(node_id,0); // auto 
					myZWApi->zwThermostatModeSet(node_id,10); // auto changeover
				}
				break;
				;;	
			case COMMAND_Play_Media_CONST:
				myZWApi->zwAVControlSet(node_id,sequence,3); // vol up
				break;
				;;
			case COMMAND_Set_HeatCool_CONST:
				heat = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
				char mode_tmp = 'A';
				if (heat.size()>0) {
					mode_tmp = heat.c_str()[0];
				}
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET HEAT/COOL RECEIVED FOR CHILD %d, string: %s",node_id,heat.c_str());
				// A - auto changeover; H - heat; C - cool; F - fan; 
				switch(mode_tmp) {
					default:
					case 'A':
						myZWApi->zwThermostatModeSet(node_id,10); // auto changeover
						break;
					case 'H':
						myZWApi->zwThermostatModeSet(node_id,1); // heat
						break;
					case 'C':
						myZWApi->zwThermostatModeSet(node_id,2); // cool
						break;
					case 'F':
						myZWApi->zwThermostatModeSet(node_id,6); // fan only
						break;
				}
				break;
				;;
	
		}
	} else {
		sCMD_Result = "BAD NODE ID";
	}
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

//<-dceag-sample-b->!
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #756 - Report Child Devices");
	// LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Reporting child devices");
	//EVENT_Reporting_Child_Devices("",myZWApi->getDeviceList() );

}

//<-dceag-c757-b->

	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

void ZWave::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #757 - Download Configuration");
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Download configuration start");
	myZWApi->zwReplicateController(1);
	sCMD_Result = "OK";

	m_pAlarmManager->AddRelativeAlarm(60, this, 1, NULL);
}

void ZWave::AlarmCallback(int id, void* param)
{
	if (id == 1)
	{
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Download configuration end");
		myZWApi->zwReplicateController(0);
		EVENT_Download_Config_Done("");
	}
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #760 - Send Command To Child, node id: %i",node_id);
	if (node_id > 0 && node_id <= 233) {
		sCMD_Result = "OK";
		switch (iPK_Command) {
			case COMMAND_Generic_On_CONST:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"ON RECEIVED FOR CHILD %d",node_id);
				myZWApi->zwBasicSet(node_id,255,0);
				break;
				;;
			case COMMAND_Generic_Off_CONST:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"OFF RECEIVED FOR CHILD %d",node_id);
				myZWApi->zwBasicSet(node_id,0,0);
				break;
				;;
			default:
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"NOT IMPLEMENTED COMMAND RECEIVED FOR CHILD %d",node_id);
				sCMD_Result = "ZWave NOT IMPLEMENTED";
		}
	} else {
		sCMD_Result = "BAD NODE ID";
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #776 - Reset");
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Resetting the eprom and all node information");
	myZWApi->zwSetDefault();
}

//<-dceag-c788-b->

	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

void ZWave::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #788 - StatusReport");
	if (sArguments.find('SR') != std::string::npos) {
		myZWApi->zwStatusReport();
	} else if (sArguments.find('NU') != std::string::npos) {
		myZWApi->zwRequestNodeNeighborUpdate(atoi(sArguments.substr(2).c_str()));
	} else if (sArguments.find('NI') != std::string::npos) {
		myZWApi->zwRequestNodeInfo(atoi(sArguments.substr(2).c_str()));
	} else if (sArguments.find('B') != std::string::npos) {
		myZWApi->zwControllerBackup();
	} else if (sArguments.find('R') != std::string::npos) {
		myZWApi->zwControllerRestore();
	} else if (sArguments.find('S') != std::string::npos) {
		myZWApi->zwRequestMultilevelSensorReport(11);
	} else if (sArguments.find('P') != std::string::npos) {
		myZWApi->zwSetPromiscMode(1);
	} else {
		myZWApi->zwSoftReset();
		sleep(2);
		myZWApi->zwStatusReport();
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
	if (iDestNodeID == 0) {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Assigning SUC return route");
		myZWApi->zwAssignSUCReturnRoute(iNodeID);
	} else {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWave::CMD_Assign_Return_Route %d -> %d",iNodeID,iDestNodeID);
		myZWApi->zwAssignReturnRoute(iNodeID,iDestNodeID);
	}
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWave::CMD_SetWakeUp Node: %d Value: %d",iNodeID,iValue);
	myZWApi->zwWakeupSet(iNodeID,iValue,false);
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWave::CMD_Set_Config_Param: Nodeid: %d Parameter: %d Value: %d",iNodeID,iParameter_ID,iValue);
	if (iSize==-1) {
		myZWApi->zwConfigurationGet(iNodeID,iParameter_ID);
	} else if(iSize > 0) {
		myZWApi->zwConfigurationSet(iNodeID,iParameter_ID,iValue,iSize);
	} else {	
		myZWApi->zwConfigurationSet(iNodeID,iParameter_ID,iValue,0);
	}
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWave::CMD_Set_Association");

	// empty node list: show actual associations for group
	// positive node id in list: add to group
	// negative node id in list: remove from group
	if (sNodes_List.size() > 0) {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "parsing node list");
		std::vector<std::string> target;
		StringUtils::Tokenize(sNodes_List, std::string(","), target);
		for(std::vector<string>::iterator it=target.begin(); it!=target.end(); ++it) {
			std::vector<std::string> target2;
			StringUtils::Tokenize((*it), std::string("/"), target2);
			std::vector<string>::iterator it2=target2.begin();
			int nodeid = atoi((*it2).c_str());
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "node: %i %s", nodeid, (*it2).c_str());
			int instance = -1;
			if (it2!=target2.end()) {
				instance = atoi((*it2).c_str());
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "instance: %i %s", instance, (*it2).c_str());
			}
			// FIXME: hack until the instance parsing above is working correctly
			instance = -1;
			if (instance == -1) {	
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "sending single instance association");
				if (nodeid > 0) {
					myZWApi->zwAssociationSet(iNodeID,iGroup_ID,nodeid);
				}
				if (nodeid < 0) {
					myZWApi->zwAssociationRemove(iNodeID,iGroup_ID,nodeid * -1);
				}
			} else {
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "sending multi instance association");
				if (nodeid > 0) {
					myZWApi->zwAssociationSetMulti(iNodeID,iGroup_ID,nodeid, instance);
				}
				// if (nodeid < 0) {
				//	myZWApi->zwAssociationRemoveMulti(iNodeID,iGroup_ID,nodeid * -1, instance);
				// }

			}
		}

	//	myZWApi->zwAssociationGet(iNodeID,iGroup_ID);
	} else {
		myZWApi->zwAssociationGet(iNodeID,iGroup_ID);
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #966 - Set Polling State");
	if (sValue_To_Assign.find('0') != std::string::npos) {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Deactivating polling");
		myZWApi->zwPollDevices(false);
	} else {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Activating polling");
		myZWApi->zwPollDevices(true);
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #967 - Add Node");
	bool highpower = false;
	if (sOptions.find('H') != std::string::npos) {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Option high power inclusion set");
		highpower = true;
	}

	if (iValue == 5) {
		// we only honor "node stop" for now
		myZWApi->zwAddNodeToNetwork(0,highpower);
	} else {
		// we simply call start (param==1), that implements "Node Any"
		myZWApi->zwAddNodeToNetwork(1,highpower);
	}

/*	we ignore most of the other parameters for now
        cout << "Parm #39 - Options=" << sOptions << endl;
        cout << "Parm #48 - Value=" << iValue << endl;
        cout << "Parm #182 - Timeout=" << sTimeout << endl;
        cout << "Parm #259 - Multiple=" << bMultiple << endl; */
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
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Received command #968 - Remove Node");

	if (iValue == 5) {
		// we only honor "node stop" for now
		myZWApi->zwRemoveNodeFromNetwork(0);
	} else if (iValue < 0) {
		myZWApi->zwIsFailedNode(-iValue);
		myZWApi->zwRemoveFailedNodeId(-iValue);
	} else {
		// we simply call start (param==1), that implements "Node Any"
		myZWApi->zwRemoveNodeFromNetwork(1);
	}
}


void ZWave::SendSensorTrippedEvents(unsigned short node_id, int instance_id, bool value)
{
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending sensor tripped event from node %d",node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Sensor_Tripped_CONST,
			1,
			EVENTPARAMETER_Tripped_CONST,
			value ? "1" : "0")
		);
	}
}

void ZWave::SendFireAlarmEvent(unsigned short node_id, int instance_id)
{
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending fire alarm event from node %d",node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Fire_Alarm_CONST,
			1,
			EVENTPARAMETER_PK_Device_CONST,
			tmp_node_id)
		);
	}
}

void ZWave::SendAirQualityEvent(unsigned short node_id, int instance_id)
{
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending air quality event from node %d",node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Sensor_Tripped_CONST,
			1,
			EVENTPARAMETER_PK_Device_CONST,
			tmp_node_id)
		);
	}
}

void ZWave::SendCO2LevelChangedEvent(unsigned short node_id, int instance_id, int value)
{
	char tmp_node_id[16];
	char tmp_value[16];
	sprintf(tmp_node_id,"%i",node_id);
	sprintf(tmp_value,"%i",value);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending co2 level changed event from node %s",tmp_node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_CO2_Level_Changed_CONST,
			1,
			EVENTPARAMETER_Value_CONST,
			tmp_value)
		);
	}

}

void ZWave::SendSetpointChangedEvent(unsigned short node_id, int instance_id, float value)
{
	char tempstr[512];
	int iValue = round(value);
	sprintf(tempstr, "%d", iValue);
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending setpoint changed event from node %s",tmp_node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Thermostat_Set_Point_Chan_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
		);
	}
}

void ZWave::SendTemperatureChangedEvent(unsigned short node_id, int instance_id, float value)
{
	char tempstr[512];
	sprintf(tempstr, "%.2f", value);
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending temperature changed event from node %s",tmp_node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Temperature_Changed_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
		);
	}

}
void ZWave::SendHumidityChangedEvent(unsigned short node_id, int instance_id, float value)
{
	char tempstr[512];
	sprintf(tempstr, "%.2f", value);
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending humidity level changed event from node %s",tmp_node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Humidity_Changed_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
		);
	}

}
void ZWave::SendBrightnessChangedEvent(unsigned short node_id, int instance_id, int value)
{
	char tempstr[512];
	sprintf(tempstr, "%i", value);
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if (pChildDevice != NULL) {		
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending brightness level changed event from node %s",tmp_node_id);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Brightness_Changed_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
		);
	}

}

void ZWave::SendLightChangedEvents(unsigned short node_id, int instance_id, int value)
{
	char svalue[32];
	sprintf(svalue, "%d", value==99?100:value);
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if( pChildDevice != NULL )
                {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending EVENT_State_Changed_CONST event from node %s/%d, level %s",tmp_node_id,instance_id,svalue);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_State_Changed_CONST,
			1,
			EVENTPARAMETER_State_CONST,
			svalue)
		);
	}
}

void ZWave::SendPowerUsageChangedEvent(unsigned short node_id, int instance_id, int value)
{
	char svalue[32];
	sprintf(svalue, "%d", value);
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
        DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, instance_id);
	if( pChildDevice != NULL )
                {
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Sending EVENT_Power_Usage_Changed_CONST event from node %s/%d, value %s W",tmp_node_id,instance_id,svalue);
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Power_Usage_Changed_CONST,
			1,
			EVENTPARAMETER_Watts_CONST,
			svalue)
		);
	}
}

void ZWave::SendOnOffEvent(unsigned short node_id, int instance_id, int value) {
        DeviceData_Impl *pChildDevice = NULL;	
	if ( (pChildDevice = InternalIDToDevice(StringUtils::itos(node_id), instance_id) ) != NULL) {
		m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
			DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,
			EVENT_Device_OnOff_CONST,
			1,
			EVENTPARAMETER_OnOff_CONST,
			(value == 0) ? "0" : "1")
		);
	}
}

void ZWave::SendOrbiterPopup(const char *message) {
	string timeout = "10";
	string tokens = "";
	LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "sending popup message: %s",message);

	// m_pEvent->SendMessage( new Message(-305,
	//
	// ../Orbiter/Orbiter.cpp:2432:	Message *pMessage = new Message(m_dwPK_Device,m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
	// ../Orbiter/Orbiter.cpp-2433-	COMMAND_Set_Variable_CONST,StringUtils::itos(VARIABLE_EPG_GUIDE_VIEW_CONST).c_str(),
	// ../Orbiter/Orbiter.cpp-2434-	COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(DESIGNOBJ_MNUTVEPG1_CONST).c_str());
	// ../Orbiter/Orbiter.cpp-2435-	GetEvents()->SendMessage(pMessage);
		// COMMANDPARAMETER_Text_CONST, message,
	//
	m_pEvent->SendMessage( new Message(m_dwPK_Device,DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
		PRIORITY_NORMAL,
		MESSAGETYPE_COMMAND,
		COMMAND_Display_Alert_CONST,
		4,
		COMMANDPARAMETER_Text_CONST, message,
		COMMANDPARAMETER_Tokens_CONST, "ZWave",
		COMMANDPARAMETER_Timeout_CONST, "10",
		COMMANDPARAMETER_Interruption_CONST, "0")
	);
}

int ZWave::GetPKDevice(int node_id, int iInstanceID) {
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",node_id);
	DeviceData_Impl *pDevice = InternalIDToDevice(tmp_node_id, iInstanceID);
	if (pDevice != NULL)
		return pDevice->m_dwPK_Device;
	else
		return -1;
}

int ZWave::AddDevice(int parent, string sInternalID, int iInstanceID, int PK_DeviceTemplate) {
	int iPK_Device = 0;

	int tmp_parent = 0;
	if (parent > 0) { tmp_parent = parent; } else { tmp_parent = m_dwPK_Device; }

	string tmp_s = StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID;
	DeviceData_Impl *pDevice = InternalIDToDevice(sInternalID, iInstanceID);
	bool bFound = pDevice != NULL;
	
	if (!bFound) {
	        string sInternalIDInst = sInternalID;
		if (iInstanceID > 0) {
		        sInternalIDInst = sInternalIDInst + "/" + StringUtils::itos(iInstanceID);
		}
		// does not exist, create child
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Adding device for node: %s",sInternalIDInst.c_str());
		/*
		m_pEvent->SendMessage( new Message(m_dwPK_Device,4,
		// m_pEvent->SendMessage( new Message(m_dwPK_Device,DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST,
			PRIORITY_NORMAL,
			MESSAGETYPE_COMMAND,
			COMMAND_Create_Device_CONST,
			10,
			COMMANDPARAMETER_PK_DeviceTemplate_CONST, StringUtils::itos(PK_DeviceTemplate).c_str(),
			COMMANDPARAMETER_Mac_address_CONST, "",
			COMMANDPARAMETER_PK_Room_CONST, "0",
			COMMANDPARAMETER_IP_Address_CONST, "",
			COMMANDPARAMETER_Data_String_CONST, tmp_s.c_str(),
			COMMANDPARAMETER_PK_DHCPDevice_CONST, "",
			COMMANDPARAMETER_PK_Device_ControlledVia_CONST, StringUtils::itos(tmp_parent).c_str(),
			COMMANDPARAMETER_Description_CONST, "",
			COMMANDPARAMETER_PK_Orbiter_CONST, "0",
			COMMANDPARAMETER_PK_Device_Related_CONST, "", 
			COMMANDPARAMETER_PK_Device_CONST, "") // StringUtils::itos(*iPK_Device).c_str() )
		); */
		// CMD_Create_Device add_command(m_dwPK_Device,DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST, PK_DeviceTemplate,"",0,"",
		CMD_Create_Device add_command(m_dwPK_Device,4, PK_DeviceTemplate,"",0,"",
			StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalIDInst, 0,tmp_parent,"",0,0,&iPK_Device) ;
		SendCommand(add_command);
	} else {
	        iPK_Device = pDevice->m_dwPK_Device;
	}

	return iPK_Device;

} 
bool ZWave::DeleteDevicesForNode(string sInternalID) {
	DeviceData_Impl *pTargetChildDevice = NULL;
	// CMD_Delete_Device del_command(m_dwPK_Device,DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST,device_id);

        vector<DeviceData_Impl *> vectDevices = FindDevicesForNode(sInternalID);
	if (vectDevices.size() > 0) {
	        for( vector<DeviceData_Impl*>::const_iterator it = vectDevices.begin(); it != vectDevices.end(); ++it )
		{
		        pTargetChildDevice = (*it);
			CMD_Delete_Device del_command(m_dwPK_Device,4,pTargetChildDevice->m_dwPK_Device);
			DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Deleting DCE child %i, z-wave node id %s",pTargetChildDevice->m_dwPK_Device,sInternalID.c_str());

			SendCommand(del_command);
		}
		return true;
	} else {
		return false;
	}
}

string ZWave::GetSchedule(int iNodeID) {
	string sSchedule;
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",iNodeID);
	DeviceData_Impl* device = InternalIDToDevice(tmp_node_id, -1);
	if (device != NULL) {
	        CMD_Get_Device_Data cmd_Get_Device_Data(m_dwPK_Device, 4,
							device->m_dwPK_Device, DEVICEDATA_Schedule_CONST, true,
							&sSchedule
							);
		SendCommand(cmd_Get_Device_Data);
	}
	return sSchedule;
}

string ZWave::GetCapabilities(int iNodeID, int iInstanceID) {
	string sCapabilities;
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",iNodeID);
	DeviceData_Impl* device = InternalIDToDevice(tmp_node_id, iInstanceID);
	if (device != NULL) {
	        CMD_Get_Device_Data cmd_Get_Device_Data(m_dwPK_Device, 4,
							device->m_dwPK_Device, DEVICEDATA_Capabilities_CONST, true,
							&sCapabilities
							);
		SendCommand(cmd_Get_Device_Data);
	}
	return sCapabilities;
}

bool ZWave::SetCapabilities(int iNodeID, int iInstanceID, string sCapabilities) {
	int PKDevice = -1;
	char tmp_node_id[16];
	sprintf(tmp_node_id,"%i",iNodeID);
	DeviceData_Impl *pChildDevice = InternalIDToDevice(tmp_node_id, iInstanceID);
	if (pChildDevice != NULL) {
		PKDevice = pChildDevice->m_dwPK_Device;
		LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Setting capabilities device data for ZWave node %d/%d (device id %i, capabilities: %s)",iNodeID, iInstanceID, PKDevice, sCapabilities.c_str());
		CMD_Set_Device_Data cmd_Set_Device_Data(m_dwPK_Device, 4, PKDevice, sCapabilities.c_str(), DEVICEDATA_Capabilities_CONST);
		SendCommand(cmd_Set_Device_Data);
		return true;
	} else {
		return false;
	}
}

void ZWave::AddCapability(int PKDevice, int iCC) {
	if (PKDevice != -1) {
	        string sCapabilities;
	        CMD_Get_Device_Data cmd_Get_Device_Data(m_dwPK_Device, 4,
							PKDevice, DEVICEDATA_Capabilities_CONST, true,
							&sCapabilities
							);
		SendCommand(cmd_Get_Device_Data);
		if (sCapabilities.find("BAD DEVICE") == string::npos) {
		        // can happen when device was just added(before reload), it will still add the device data below
		        sCapabilities = "";
		}
		if (sCapabilities.find(StringUtils::itos(iCC)) == string::npos) {
		        if (sCapabilities.size() > 0)
			         sCapabilities += ",";
		        sCapabilities += StringUtils::itos(iCC);
			CMD_Set_Device_Data cmd_Set_Device_Data(m_dwPK_Device, 4, PKDevice, sCapabilities.c_str(), DEVICEDATA_Capabilities_CONST);
			SendCommand(cmd_Set_Device_Data);
		  }
	}
}

void ZWave::SetManufacturerSpecificString(int iPKDevice, string sManufacturerSpecific) {
	string a,b,c;	
	// CMD_Update_Device cmd_Update_Device(m_dwPK_Device, 4, iPKDevice, a,0,b,c, sManufacturerSpecific);
	// SendCommand(cmd_Update_Device);
}

DeviceData_Impl *ZWave::InternalIDToDevice(string sInternalID, int iInstanceID) {
        DeviceData_Impl *pChildDevice = NULL;

	if (sInternalID == "") return NULL;
	string sInternalIDInst = sInternalID;
	if (iInstanceID > 0) {
	        sInternalIDInst = sInternalIDInst + "/" + StringUtils::itos(iInstanceID);
	}
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
		        string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			// check if child exists
			if ( tmp_node_id.compare(sInternalIDInst) == 0) {
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
					if ( tmp_node_id.compare(sInternalIDInst) == 0) {
					        return pChildDevice1;
					}
				}
			}

		}
	}
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "ZWave::InternalIDToDevice() No device found for id %s", sInternalIDInst.c_str());
	return NULL;
}

map<int, int> ZWave::FindCCInstanceCountForNode(string sInternalID) {
        vector<DeviceData_Impl*> vecDevices = FindDevicesForNode(sInternalID);
	map<int, int> mapCCInstanceCount;
	for(uint i=0; i < vecDevices.size(); ++i) {
	        string nodeInstance = vecDevices[i]->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
		string sNodeid;
		int instanceid = 0;
		if (nodeInstance.find("/") != string::npos) {
		        vector<string> parts;
			StringUtils::Tokenize(nodeInstance, "/", parts);
			sNodeid = parts[0].c_str();
			instanceid = atoi(parts[1].c_str()); 
		} else {
		        sNodeid = nodeInstance;
		}
		int nodeid =  atoi(nodeInstance.c_str());
		string capa = GetCapabilities(nodeid, instanceid);
		vector<string> vectCCs;
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Node/instance: %d/%d, capabilities: %s", nodeid, instanceid, capa.c_str());
		StringUtils::Tokenize(capa, ",", vectCCs);
		// Count number of instances that support each CC
		for (int i = 0; i < vectCCs.size(); i++) {
		        int cc = atoi(vectCCs[i].c_str());
			if (mapCCInstanceCount.find(cc) == mapCCInstanceCount.end()) {
			        mapCCInstanceCount[cc] = 0;
			}
			mapCCInstanceCount[cc] = mapCCInstanceCount[cc] + 1;
		}
	}
	return mapCCInstanceCount;
}

vector<DeviceData_Impl*> ZWave::FindDevicesForNode(string sInternalID) {
        vector<DeviceData_Impl*> vecDevices;
	DeviceData_Impl* pChildDevice = NULL;

	if (sInternalID == "") return vecDevices;
        string sInternalIDInst = sInternalID;
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
        {
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
		        string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			if (tmp_node_id.find("/") != string::npos) {
 	                        vector<string> parts;
 	                        StringUtils::Tokenize(tmp_node_id, "/", parts);
 	                        tmp_node_id = parts[0];
 	                } 
			// check if child exists
			if ( tmp_node_id.compare(sInternalIDInst) == 0) {
			// if ( StringUtils::StartsWith(tmp_node_id, sInternalIDInst) ) {
			      vecDevices.push_back(pChildDevice);
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
					if (tmp_node_id.find("/") != string::npos) {
						vector<string> parts;
						StringUtils::Tokenize(tmp_node_id, "/", parts);
						tmp_node_id = parts[0];
					} 
					if ( tmp_node_id.compare(sInternalIDInst) == 0) {
					         vecDevices.push_back(pChildDevice1);
					}
				}
			}

		}
	}
	return vecDevices;
}
//<-dceag-c1085-b->

	/** @brief COMMAND: #1085 - Resync node */
	/** Resynchronize device data */
		/** @param #239 NodeID */
			/** Node id to resync */

void ZWave::CMD_Resync_node(int iNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1085-e->
{
  myZWApi->zwRequestNodeInfo(iNodeID); 
}

void ZWave::ReportBatteryStatus(int iNode, int status)
{
	int PKDevice = GetPKDevice(iNode, -1);
	if (PKDevice > 0)
	{
		CMD_Set_Device_Data cmd_Set_Device_Data(m_dwPK_Device, 4, PKDevice, StringUtils::itos(status), DEVICEDATA_Battery_state_CONST);
		SendCommand(cmd_Set_Device_Data);
	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Unknown node: %d", iNode);
	}
}
