/*
     Written for LinuxMCE. (c) LinuxMCE Foundation.
     Licensed under the GNU GPL v3, see COPYING for more details.

     Author: Adam Shortland <los93sol@gmail.com>
*/
//<-dceag-d-b->
#ifndef HAI_Omni_Series_RS232_h
#define HAI_Omni_Series_RS232_h

//	DCE Implemenation for #2116 HAI Omni Series RS232

#include "Gen_Devices/HAI_Omni_Series_RS232Base.h"
//<-dceag-d-e->

// Relative Alarm CONSTS
#define MODE_POLL_FAST  1
#define MODE_POLL_SLOW  2
#define MODE_TIMEUPDATE 3
#define MODE_QUEUE      4

// Queue Processing CONSTS
#define Q_FUNCT_POLL_SECURITYZONESTATUS         1
#define Q_FUNCT_POLL_SECURITYZONES              2
#define Q_FUNCT_POLL_SECURITYMODE               3
#define Q_FUNCT_POLLTHERMOS                     4
#define Q_FUNCT_CONTROLPANEL                    5
#define Q_FUNCT_ADDCHILDREN                     6

#include "AlarmManager.h"
#include <map>
#include <queue>
#include <deque>

//<-dceag-decl-b->
namespace DCE
{
	class HAI_Omni_Series_RS232 : public HAI_Omni_Series_RS232_Command, public AlarmEvent
	{
//<-dceag-decl-e->
        // Whenever re-running DCEGen, make sure the above declaration is:
        // class HAI_Omni_Series_RS232 : public HAI_Omni_Series_RS232_Command, public AlarmEvent

		// Private member variables
private:
		HAI_Omni_Series_RS232 *m_pHAI_Omni_Series_RS232;

		// Private methods
public:
		// Public member variables
                // Connection Setup and Initialization
                bool UpdateHAIConfig();
                void AlarmCallback(int, void*);
                class AlarmManager *m_pAlarmManager;

                // AutoDetection
                void AddChildren();
                map<string,string> m_mvNewChildren;

                // Queues
                queue<string> m_qCommand;
                queue<int> m_qFunctionCallback;
                void QueueProcess();
                void QueueMessageToPanel(string thisCommand, int thisFunction);

                // Polling
                // Climate
                void PollThermos();
                string ProcessTemps(string sTempLine);

                // Security
                void PollSecurityMode();
                //void PollSecurityZones();
                void PollSecurityZoneStatus();

                // Zone and Status Maintenance
                // Climate
                map<int,string> m_mvThermoMode;
                map<int,string> m_mvThermoFanMode;
                map<int,string> m_mvThermoTemp;
                map<int,string> m_mvThermoCoolTemp;
                map<int,string> m_mvThermoHeatTemp;
                //map<int,string> m_mvThermoAutoTemp;

                // Security
                map<int,string> m_mvSecurityZoneStatus;
                int GetZoneNumber(string ZoneLine);
                int m_iCurrentHouseMode;

                // Unit
                map<int,string> m_mvUnitState;

                // Helpers
                int getDeviceForZone(int iZone, unsigned int DeviceCategory);
                int getExactDeviceForZone(int iZone, unsigned int DeviceCategory, int DeviceTemplate);

//<-dceag-const-b->
public:
		// Constructors/Destructor
		HAI_Omni_Series_RS232(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~HAI_Omni_Series_RS232();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		HAI_Omni_Series_RS232(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	string DATA_Get_Connector();
	string DATA_Get_Password();
	string DATA_Get_Units();

			*****EVENT***** accessors inherited from base class
	void EVENT_Sensor_Tripped(bool bTripped);
	void EVENT_Temperature_Changed(int iValue);
	void EVENT_Thermostat_Set_Point_Chan(int iValue);
	void EVENT_Alarm_panel_mode_change(int iValue,int iZoneNo);
	void EVENT_Thermostat_Mode_Changed(int iValue);
	void EVENT_Fan_Mode_Changed(int iValue);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #19 - Set House Mode */
	/** Set house mode */
		/** @param #5 Value To Assign */
			/** A value from the HouseMode table, or -1=toggle monitor mode */
		/** @param #17 PK_Users */
			/** The user setting the mode.  If this is 0, it will match any user who has permission to set the house mode. */
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

	virtual void CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { string sCMD_Result; CMD_Set_House_Mode(sValue_To_Assign.c_str(),iPK_Users,sPassword.c_str(),iPK_DeviceGroup,sHandling_Instructions.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #192 - On */
	/** Turn the thermostat on */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turn the thermostat off */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #278 - Set Temperature */
	/** Sets the temperature to an absolute or relative value. */
		/** @param #5 Value To Assign */
			/** Can be absolute, like 20, 18, etc., or relative, like +2, -2, etc.  The temperature is always in degrees celsius, and the application is expected to convert to Fahrenheit if necesssary. */

	virtual void CMD_Set_Temperature(string sValue_To_Assign) { string sCMD_Result; CMD_Set_Temperature(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Temperature(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #279 - Set Fan */
	/** Force the fan to be on all the time to circulate air. */
		/** @param #8 On/Off */
			/** If 1, the fan will always be on regardless of temperature, if 0, it will be turned on by the climate system when needed. */

	virtual void CMD_Set_Fan(string sOnOff) { string sCMD_Result; CMD_Set_Fan(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Fan(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #280 - Set Heat/Cool */
	/** Indicates if the thermostat should be heat-only, cool-only, or either one */
		/** @param #8 On/Off */
			/** H=heat only, C=cool only, A=automatic */

	virtual void CMD_Set_HeatCool(string sOnOff) { string sCMD_Result; CMD_Set_HeatCool(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_HeatCool(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report Child Devices */

	virtual void CMD_Report_Child_Devices() { string sCMD_Result; CMD_Report_Child_Devices(sCMD_Result,NULL);};
	virtual void CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #759 - Get Sensors List */
	/** Get the list of sensors */
		/** @param #109 Data String */
			/** [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n */

	virtual void CMD_Get_Sensors_List(string *sData_String) { string sCMD_Result; CMD_Get_Sensors_List(sData_String,sCMD_Result,NULL);};
	virtual void CMD_Get_Sensors_List(string *sData_String,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1073 - Trip Unit */
	/** Trips the onboard "unit" relays for HAI security panels. */

	virtual void CMD_Trip_Unit() { string sCMD_Result; CMD_Trip_Unit(sCMD_Result,NULL);};
	virtual void CMD_Trip_Unit(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
