/*
 *      Copyright (C) 2008 Harald Klein <hari@vt100.at>
 *
 *      This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      See the GNU General Public License for more details.
 */

#include <stddef.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/file.h>

#include "Generic_Serial_Device/IOUtils.h"

#include "ZWApi.h"

//<-dceag-d-b->
#ifndef ZWave_h
#define ZWave_h

//	DCE Implemenation for #1754 ZWave

#include "Gen_Devices/ZWaveBase.h"
//<-dceag-d-e->
#include "../DCE/AlarmManager.h"

//<-dceag-decl-b->
namespace DCE
{
	class ZWave : public ZWave_Command, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables
		ZWApi::ZWApi *myZWApi;
		int myDeviceID;
		AlarmManager* m_pAlarmManager;

		// Private methods
		virtual DeviceData_Impl *InternalIDToDevice(string sInternalID, int instance_id);
		virtual vector<DeviceData_Impl*> FindDevicesForNode(string sInternalID);
		void AlarmCallback(int id, void* param);
		
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~ZWave();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void SendSensorTrippedEvents(unsigned short node_id, int instance_id, bool value);
		virtual void SendFireAlarmEvent(unsigned short node_id, int instance_id);
		virtual void SendAirQualityEvent(unsigned short node_id, int instance_id);
		virtual void SendCO2LevelChangedEvent(unsigned short node_id, int instance_id, int value);
		virtual void SendLightChangedEvents(unsigned short node_id, int instance_id, int value);
		virtual void SendPowerUsageChangedEvent(unsigned short node_id, int instance_id, int value);
		virtual void SendTemperatureChangedEvent(unsigned short node_id, int instance_id, float value);
		virtual void SendSetpointChangedEvent(unsigned short node_id, int instance_id, float value);
		virtual void SendBrightnessChangedEvent(unsigned short node_id, int instance_id, int value);
		virtual void SendHumidityChangedEvent(unsigned short node_id, int instance_id, float value);
		virtual void SendOnOffEvent(unsigned short node_id, int instance_id, int value);
		virtual void SendOrbiterPopup(const char *message); 
        	virtual int AddDevice(int parent, string sInternalID, int iInstanceID, int PK_DeviceTemplate);
		virtual int GetPKDevice(int node_id, int instance_id);
        	virtual bool DeleteDevicesForNode(string sInternalID);
		virtual string GetCapabilities(int iNodeID, int iInstanceID);
		virtual string GetSchedule(int iNodeID);
		virtual bool SetCapabilities(int iNodeID, int iInstanceID, string sCapabilities);
		virtual void AddCapability(int PKDevice, int capability);
		virtual void SetManufacturerSpecificString(int iPKDevice,string sManufacturerSpecific);
		virtual std::map<int, int> FindCCInstanceCountForNode(string sInternalID);
		virtual void ReportBatteryStatus(int iNode, int status);


//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		ZWave(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();

			*****EVENT***** accessors inherited from base class
	void EVENT_Sensor_Tripped(bool bTripped);
	void EVENT_Device_OnOff(bool bOnOff);
	void EVENT_Reporting_Child_Devices(string sError_Message,string sText);
	void EVENT_Download_Config_Done(string sError_Message);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

	virtual void CMD_Report_Child_Devices() { string sCMD_Result; CMD_Report_Child_Devices(sCMD_Result,NULL);};
	virtual void CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

	virtual void CMD_Download_Configuration(string sText) { string sCMD_Result; CMD_Download_Configuration(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters) { string sCMD_Result; CMD_Send_Command_To_Child(sID.c_str(),iPK_Command,sParameters.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

	virtual void CMD_Reset(string sArguments) { string sCMD_Result; CMD_Reset(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

	virtual void CMD_StatusReport(string sArguments) { string sCMD_Result; CMD_StatusReport(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #820 - Assign Return Route */
	/** Assign Return Route between 2 nodes from ZWave network */
		/** @param #239 NodeID */
			/** Source Node ID */
		/** @param #240 DestNodeID */
			/** Destination Node ID */

	virtual void CMD_Assign_Return_Route(int iNodeID,int iDestNodeID) { string sCMD_Result; CMD_Assign_Return_Route(iNodeID,iDestNodeID,sCMD_Result,NULL);};
	virtual void CMD_Assign_Return_Route(int iNodeID,int iDestNodeID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #840 - SetWakeUp */
	/** Set the WakeUp value for Z-Wave PIR devices. */
		/** @param #48 Value */
			/** WakeUp timeout value (1-255 min) */
		/** @param #239 NodeID */
			/** Z-Wave node ID of the device. */

	virtual void CMD_SetWakeUp(int iValue,int iNodeID) { string sCMD_Result; CMD_SetWakeUp(iValue,iNodeID,sCMD_Result,NULL);};
	virtual void CMD_SetWakeUp(int iValue,int iNodeID,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Set_Config_Param(int iValue,int iSize,int iNodeID,int iParameter_ID) { string sCMD_Result; CMD_Set_Config_Param(iValue,iSize,iNodeID,iParameter_ID,sCMD_Result,NULL);};
	virtual void CMD_Set_Config_Param(int iValue,int iSize,int iNodeID,int iParameter_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #842 - Set Association */
	/** Set the association of Z-Wave nodes into a group. */
		/** @param #239 NodeID */
			/** Z-Wave node ID which associates some nodes in its group. */
		/** @param #249 Group ID */
			/** The group number which includes the associated nodes. */
		/** @param #250 Nodes List */
			/** The list of associated Z-Wave nodes (comma separated). */

	virtual void CMD_Set_Association(int iNodeID,int iGroup_ID,string sNodes_List) { string sCMD_Result; CMD_Set_Association(iNodeID,iGroup_ID,sNodes_List.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Association(int iNodeID,int iGroup_ID,string sNodes_List,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Set_Polling_State(int iPK_Device,string sValue_To_Assign,bool bReport,bool bAlways,int iNodeID) { string sCMD_Result; CMD_Set_Polling_State(iPK_Device,sValue_To_Assign.c_str(),bReport,bAlways,iNodeID,sCMD_Result,NULL);};
	virtual void CMD_Set_Polling_State(int iPK_Device,string sValue_To_Assign,bool bReport,bool bAlways,int iNodeID,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Add_Node(string sOptions,int iValue,string sTimeout,bool bMultiple) { string sCMD_Result; CMD_Add_Node(sOptions.c_str(),iValue,sTimeout.c_str(),bMultiple,sCMD_Result,NULL);};
	virtual void CMD_Add_Node(string sOptions,int iValue,string sTimeout,bool bMultiple,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Remove_Node(string sOptions,int iValue,string sTimeout,bool bMultiple) { string sCMD_Result; CMD_Remove_Node(sOptions.c_str(),iValue,sTimeout.c_str(),bMultiple,sCMD_Result,NULL);};
	virtual void CMD_Remove_Node(string sOptions,int iValue,string sTimeout,bool bMultiple,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1085 - Resync node */
	/** Resynchronize device data */
		/** @param #239 NodeID */
			/** Node id to resync */

	virtual void CMD_Resync_node(int iNodeID) { string sCMD_Result; CMD_Resync_node(iNodeID,sCMD_Result,NULL);};
	virtual void CMD_Resync_node(int iNodeID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
