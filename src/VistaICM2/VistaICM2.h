/*

   Written for LinuxMCE
   Licensed under the GNU GPL v3. See COPYING for details.

Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
	Adam Shortland <los93sol@los93sol.com>

*/
//<-dceag-d-b->
#ifndef VistaICM2_h
#define VistaICM2_h

//	DCE Implemenation for #2001 VistaICM2

#include "Gen_Devices/VistaICM2Base.h"
//<-dceag-d-e->

#include <pthread.h>
#include <vector>
#include <map>

//<-dceag-decl-b->
namespace DCE
{
	class VistaICM2 : public VistaICM2_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VistaICM2(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VistaICM2();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

public:
		string m_sURL;
		string m_sCurrentHouseMode;
		string m_sZones;
		string m_sHouseCode;

		// The CCP UDP thread.
		pthread_t m_ccpThread;
		bool m_bRunning;		// Is the thread running?
		bool m_bAlarmActive;		// Is an alarm already active?
		vector<int> m_viHouseModes;	// The Current House Modes per Devicegroups
		map<int,int> m_miZones;		// The zones mapped to this device.			

		// Maintenance of house modes
		vector<int> parseHouseModes();	

		// Maintenance of Zones
		map<int,int> parseZones();
		void sendTripped(int iPK_Device, string sSensorStatus);
		int getDeviceForZone(int iZS);

		// Called from the CCP thread.
		void doArmEvent(int iValue);
		void doFireEvent(string s_Value);
		void doAlarmEvent(string s_Value);

private:	// Internal methods
		bool sendCommand(string s_Command);
		void sendDigits(string s_Digits);
		void setHome(string s_Password);
		void setAway(string s_Password);
		void setStay(string s_Password);
		void setInstant(string s_Password);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		VistaICM2(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Password();
	string DATA_Get_Zones();

			*****EVENT***** accessors inherited from base class
	void EVENT_Sensor_Tripped(bool bTripped);
	void EVENT_Fire_Alarm(int iPK_Device);
	void EVENT_Reporting_Child_Devices(string sError_Message,string sText);
	void EVENT_Alarm_panel_mode_change(string sValue,int iZoneNo);

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

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
