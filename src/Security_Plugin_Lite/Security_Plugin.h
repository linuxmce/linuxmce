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
/**
 *
 * @file Security_Plugin.h
 * @brief header file for the Security_Plugin class
 *
 */
 
//<-dceag-d-b->
#ifndef Security_Plugin_h
#define Security_Plugin_h

//	DCE Implemenation for #33 Security Plug-in

#include "Gen_Devices/Security_PluginBase.h"
//<-dceag-d-e->

#include "DeviceData_Router.h"
#include "AlarmManager.h"

/*
	All sensors' states are in this form: Mode,Bypass,Delay
	where Mode=one of the house modes used in GetModeString: ARMED, AT HOME SECURE, SLEEPING, ENTERTAINING, EXTENDED AWAY,UNARMED
	Bypass=PERMBYPASS (permanently bypass the sensor, ignoring anything that happens to it),
		WAIT (wait for the sensor to return to a normal state, and then consider it active),
		BYPASS (bypass the sensor for this session.  When the sensor's mode changes, revert to normal)
		[empty] (normal, the sensor is active)
	ModeChange=[PK_ModeChange] The record in the database when we changed this mode.  We will do a lookup if the sensor
		is triggered to see if the change occurred during the exit delay

	So each sensor can be independent.  This allows for unlimited 'zones', since each sensor can do its own thing.
	The SetHouseMode command goes through and sets all the sensors to the appropriate mode.  If SetHouseMode is passed
	in a PK_DeviceGroup, it sets only the sensors in that group, thus the device group acts like a zone.

	When a sensor is tripped, SensorTrippedEvent is called.  If the sensor is in WAIT bypass and returns to normal, the bypass flag is cleared.
	Otherwise the house mode for that sensor is determined, and GetAlertType is called to determine which type of alert
	is appropriate, see pluto_security db, table: AlertType (e.g., Security, Fire, Air Quality, Movement, Information).
	Or perhaps it's not an alert, just an announcement or a snap photo.

*/

//<-dceag-decl-b->!
namespace DCE
{
	class SecurityAlert
	{
	public:
		bool m_bSensorDisarmed;
		time_t m_tTime;
		DeviceData_Router *m_pDevice;
		int m_iAlertID;

		SecurityAlert(int iAlertID,DeviceData_Router *pDevice)
		{
			m_bSensorDisarmed=false;
			m_iAlertID=iAlertID;
			m_pDevice=pDevice;
			m_tTime=time(NULL);
		}
	};

	class Security_Plugin : public Security_Plugin_Command, public AlarmEvent
	{
//<-dceag-decl-e->

	// Private member variables 
    pluto_pthread_mutex_t m_SecurityMutex;
	pthread_mutexattr_t m_MutexAttr; /** < make it recursive */
	class AlarmManager *m_pAlarmManager;
	int m_iAlertID;
	map<int,SecurityAlert *> m_mapSecurityAlert;
	string m_sRA_User,m_sRA_Password;
	map<int, list<int> > m_mapSensorsToCameras;

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Security_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Security_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	void PrepareToDelete();

	bool SensorIsTripped(int PK_HouseMode,DeviceData_Router *pDevice);
	string GetModeString(int PK_HouseMode);
	int GetModeID(string Mode);
	// Returns the PK_AlertType for this mode/house.  Or ALERTTYPE_DONOTHING, ALERTTYPE_ANNOUNCMENT, ALERTTYPE_PHOTO
	int GetAlertType(int PK_HouseMode,DeviceData_Router *pDevice,bool *bNotify=NULL);
	void SecurityBreach(DeviceData_Router *pDevice);

	/** Interceptors */
    bool SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool SensorTrippedEventHandler(class DeviceData_Router *pDevice,bool bIsTripped);
	bool PanelChangeState(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

	// Alarm callback
	virtual void AlarmCallback(int id, void* param);
	void ProcessSensorTripped(SecurityAlert *pSecurityAlert);
	void UpdateAlertLog(SecurityAlert *pSecurityAlert);
	void ProcessCameras(SecurityAlert *pSecurityAlert);
	void ProcessCamera(SecurityAlert *pSecurityAlert,DeviceData_Router *pDevice_Camera);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Path();
	string DATA_Get_Mobile_Orbiter_Notification();
	string DATA_Get_Other_Phone_Notifications();
	string DATA_Get_Neighbors_to_Call();
	string DATA_Get_Configuration();
	void DATA_Set_Configuration(string Value,bool bUpdateDatabase=false);
	string DATA_Get_PK_Device();
	string DATA_Get_Emergency_Calls();

			*****EVENT***** accessors inherited from base class
	void EVENT_Security_Breach(int iPK_Device);
	void EVENT_Fire_Alarm(int iPK_Device);
	void EVENT_Reset_Alarm();
	void EVENT_Air_Quality(int iPK_Device);
	void EVENT_Doorbell(int iPK_Device);
	void EVENT_Monitor_Mode(int iPK_Device);
	void EVENT_House_Mode_Changed(int iPK_DeviceGroup,int iPK_HouseMode);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #19 - Set House Mode */
	/** Sets the current security setting (at home, away, etc.) for the house */
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


	/** @brief COMMAND: #387 - Verify PIN */
	/** Verifies a user's PIN Code */
		/** @param #17 PK_Users */
			/** The user */
		/** @param #40 IsSuccessful */
			/** true if successful, false otherwise */
		/** @param #99 Password */
			/** The pin code, either raw or preferrably in md5 format for security */

	virtual void CMD_Verify_PIN(int iPK_Users,string sPassword,bool *bIsSuccessful) { string sCMD_Result; CMD_Verify_PIN(iPK_Users,sPassword.c_str(),bIsSuccessful,sCMD_Result,NULL);};
	virtual void CMD_Verify_PIN(int iPK_Users,string sPassword,bool *bIsSuccessful,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #969 - Restore To NonTemp State */
	/** Restore a security device to the state in State_NonTemporary */
		/** @param #2 PK_Device */
			/** The device to restore */

	virtual void CMD_Restore_To_NonTemp_State(int iPK_Device) { string sCMD_Result; CMD_Restore_To_NonTemp_State(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Restore_To_NonTemp_State(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #972 - Set Sensor State */
	/** Sets the state of a sensor */
		/** @param #2 PK_Device */
			/** The sensor */
		/** @param #5 Value To Assign */
			/** Can be: ARMED or UNARMED */

	virtual void CMD_Set_Sensor_State(int iPK_Device,string sValue_To_Assign) { string sCMD_Result; CMD_Set_Sensor_State(iPK_Device,sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Sensor_State(int iPK_Device,string sValue_To_Assign,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	// make these inline so Orbiter Plugin can call them without adding the security plugin.cpp file
	// TODO: All the plugins should call the other plugins using the shared objects/dll's rather than static linking
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
