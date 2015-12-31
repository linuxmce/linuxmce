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
#ifndef Orbiter_Plugin_h
#define Orbiter_Plugin_h

//	DCE Implemenation for #12 Orbiter Plug-in

#include "Gen_Devices/Orbiter_PluginBase.h"
//<-dceag-d-e->

#include "Orbiter/Floorplan.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "OrbiterGen/RegenMonitor.h"
#include "Gen_Devices/AllScreens.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"

class Database_pluto_main;
class Database_pluto_security;
class Database_pluto_media;
#include "OH_Orbiter.h"
#include "AlarmManager.h"

namespace DCE
{

class UnknownDeviceInfos
{
public:
    int m_iDeviceIDFrom;
    string m_sID;
    bool m_bProcessed;
    int m_iPK_DeviceTemplate;
	bool m_bAppSent;

    UnknownDeviceInfos(int iDeviceIDFrom, string sID)
    {
        m_iDeviceIDFrom = iDeviceIDFrom;
        m_sID = sID;
        m_bProcessed = false;
        m_iPK_DeviceTemplate = 0;
		m_bAppSent = false;
    }
};

class AllowedConnections
{
public:
    time_t m_tExpirationTime;
    int m_iDeviceIDAllowed;

    AllowedConnections(time_t tExpirationTime, int iDeviceIDAllowed)
    {
        m_tExpirationTime = tExpirationTime;
        m_iDeviceIDAllowed = iDeviceIDAllowed;
    }
};

//<-dceag-decl-b->!
    class Orbiter_Plugin : public Orbiter_Plugin_Command, public AlarmEvent, public DataGridGeneratorPlugIn
    {
//<-dceag-decl-e->
        bool m_bFloorPlansArePrepared;
		int m_iThreshHold;
		bool m_bIgnoreAllBluetoothDevices;
		RegenMonitor *m_pRegenMonitor;
		string m_sRegenAllDevicesRooms;
		class AlarmManager *m_pAlarmManager;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Orbiter_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Orbiter_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	virtual void PrepareToDelete();

	// Private member variables
    Database_pluto_main *m_pDatabase_pluto_main;
    Database_pluto_security *m_pDatabase_pluto_security;
    Database_pluto_media *m_pDatabase_pluto_media;
	class FloorplanInfoProvider *m_pLighting_Floorplan,*m_pClimate_Floorplan,*m_pMedia_Floorplan,*m_pSecurity_Floorplan,*m_pTelecom_Floorplan;
	class Media_Plugin *m_pMedia_Plugin;
	class General_Info_Plugin *m_pGeneral_Info_Plugin;
	class Plug_And_Play_Plugin *m_pPlug_And_Play_Plugin;
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	map<string,int> m_mapUsersID;  // ID's fired in presence deteced/lost events mapped to user id's

    // Private methods
    map<string,UnknownDeviceInfos *> m_mapUnknownDevices; // A temporary map to match Bluetooth Dongle's with devices they detect
    void PrepareFloorplanInfo();
    void PrepareFloorplanInfo(OH_Orbiter *pOH_Orbiter);
    UnknownDeviceInfos *m_mapUnknownDevices_Find(string sMacAddress)
    {
        map<string,UnknownDeviceInfos *>::iterator it = m_mapUnknownDevices.find(sMacAddress);
        return it == m_mapUnknownDevices.end() ? NULL : (*it).second;
    }

	void SendAppToPhone(OH_Orbiter *pOH_Orbiter,int nDeviceID, string sMacAddress);
	long GetAppServerAssociatedWithDevice(int nDeviceID);
	void StartRetrievingScreenSaverFiles();
	void AlarmCallback(int id, void* param);

	bool PromptForMissingMediaProviders();
	bool PromptForMissingCapture_Card_Port();

	//from command_impl class
	virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList=NULL);
	list<int> m_listRegenCommands;

	pluto_pthread_mutex_t m_AllowedConnectionsMutex;
    map<string, class AllowedConnections *> m_mapAllowedConnections;

	pluto_pthread_mutex_t m_UnknownDevicesMutex;
	pthread_mutexattr_t m_MutexAttr;
	string m_sPK_Device_AllOrbiters,m_sPK_Device_AllOrbiters_AllowingPopups;
	map<int,OH_Orbiter *> m_mapRemote_2_Orbiter;
	map<int,OH_Orbiter *> m_mapMD_2_Orbiter;

public:
    // Public member variables
    map<int,OH_Orbiter *> m_mapOH_Orbiter;
    OH_Orbiter *m_mapOH_Orbiter_Find(int PK_Device) {
        map<int,class OH_Orbiter *>::iterator it = m_mapOH_Orbiter.find(PK_Device);
        return it==m_mapOH_Orbiter.end() ? NULL : (*it).second;
    }

    OH_Orbiter *m_mapRemote_2_Orbiter_Find(int PK_Device) {
        map<int,class OH_Orbiter *>::iterator it = m_mapRemote_2_Orbiter.find(PK_Device);
        return it==m_mapRemote_2_Orbiter.end() ? NULL : (*it).second;
    }

    OH_Orbiter *m_mapMD_2_Orbiter_Find(int PK_Device) {
        map<int,class OH_Orbiter *>::iterator it = m_mapMD_2_Orbiter.find(PK_Device);
        return it==m_mapMD_2_Orbiter.end() ? NULL : (*it).second;
    }

	map<string, OH_Orbiter *> m_mapOH_Orbiter_Mac;
    OH_Orbiter *m_mapOH_Orbiter_Mac_Find(string MacAddress) {
        map<string, class OH_Orbiter *>::iterator it = m_mapOH_Orbiter_Mac.find(MacAddress);
        return it==m_mapOH_Orbiter_Mac.end() ? NULL : (*it).second;
    }

    map<int,OH_User *> m_mapOH_User;
    OH_User *m_mapOH_User_Find(int PK_User) {
        map<int,class OH_User *>::iterator it = m_mapOH_User.find(PK_User);
        return it==m_mapOH_User.end() ? NULL : (*it).second;
    }

	map<int,int> m_mapRemote_Orbiter; // A map of remote PK_Device_Remote to the current PK_Device_Orbiter they are controlling
    int m_mapRemote_Orbiter_Find(int PK_Device) {
        map<int,int>::iterator it = m_mapRemote_Orbiter.find(PK_Device);
        return it==m_mapRemote_Orbiter.end() ? NULL : (*it).second;
    }

    class AllowedConnections *m_mapAllowedConnections_Find(string sMacAddress) {
        map<string, class AllowedConnections *>::iterator it = m_mapAllowedConnections.find(sMacAddress);
        return it==m_mapAllowedConnections.end() ? NULL : (*it).second;
    }

	string PK_Device_Orbiters_In_Room_get(int PK_Room, bool bOnlyAllowingPopups=true)
	{
		string sPK_Device="";
		for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
			if( it->second->m_tRegistered && it->second->m_dwPK_Room==PK_Room && (!bOnlyAllowingPopups || it->second->m_bSendPopups) )
				sPK_Device += StringUtils::itos(it->first) + ",";
		return sPK_Device;
	}

    bool ConnectionAllowed(int iDevice, string sMacAddress);

	// Our interceptors
    bool RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
    bool MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool ReloadAborted(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool OSD_OnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool DeviceConfigured(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool PresenceDetected( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool PresenceLost( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

    void ProcessUnknownDevice();
    bool IdentifyDevice(const string& sMacAddress, string &sDeviceCategoryDesc, int &iPK_DeviceTemplate, string &sManufacturerDesc);

	class DataGridTable *FloorplanDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	virtual bool SafeToReload(string &sReason);

	/*  If the user wants to use the onscreen display, he may want the tv to always stay on and only
	turn off when the orbiter is specifcally turned off or goes to sleep.  In those cases the user
	will set the DEVICEDATA_Leave_Monitor_on_for_OSD_CONST flag to 1 */
	void OverrideAVPipe(DeviceData_Router *pDevice_OSD,bool bOverride);
	void SetPipesEnable(DeviceData_Router *pDevice,bool bOverride);  // Used to recurse

	bool CheckForNewWizardDevices(DeviceData_Router *pDevice_MD);

	void DisplayMessageOnOrbiter(int dwPK_Device,string sMessage,bool bPromptToResetRouter=false,int iTimeout=0,bool bCantGoBack=false,
		string sOption1="",string sMessage1="",string sOption2="",string sMessage2="",string sOption3="",string sMessage3="",string sOption4="", string sMessage4="" )
	{
		//TODO: remove me!
		DisplayMessageOnOrbiter(StringUtils::itos(dwPK_Device),sMessage,bPromptToResetRouter,iTimeout,bCantGoBack,
			sOption1,sMessage1,sOption2,sMessage2,sOption3,sMessage3,sOption4,sMessage4);
	}

	void DisplayMessageOnOrbiter(string sPK_Device,string sMessage,bool bPromptToResetRouter=false,int iTimeout=0,bool bCantGoBack=false,
		string sOption1="",string sMessage1="",string sOption2="",string sMessage2="",string sOption3="",string sMessage3="",string sOption4="", string sMessage4="" )
	{
		if ( sMessage == "" )
			return;

		if( sPK_Device.size()==0 )
			sPK_Device = m_sPK_Device_AllOrbiters_AllowingPopups;

		SCREEN_PopupMessage_DL SCREEN_PopupMessage_DL(m_dwPK_Device, sPK_Device,
			sMessage + "|" + sMessage1 + "|" + sMessage2 + "|" + sMessage3 + "|" + sMessage4, // Main message
			sOption1 + "|" + sOption2 + "|" + sOption3 + "|" + sOption4, // Command Line
			"generic message", // Description
			bPromptToResetRouter ? "1" : "0", // sPromptToResetRouter
			StringUtils::itos(iTimeout), // sTimeout
			bCantGoBack ? "1" : "0"); // sCannotGoBack
		SendCommand(SCREEN_PopupMessage_DL);
	}

	void FireFollowMe(string sMask,int iPK_Orbiter,int iPK_Users,int iPK_RoomOrEntArea,int iPK_RoomOrEntArea_Left);
	void SetBoundIcons(int iPK_Users,bool bOnOff,string sType);
	bool IsRegenerating(int iPK_Device);  // Is this orbiter's skin being generated?

    void GenerateVMCFiles();
    void GeneratePlutoMOConfig();

	string m_sPK_Device_AllOrbiters_get() { return m_sPK_Device_AllOrbiters; }
	string m_sPK_Device_AllOrbiters_AllowingPopups_get() { return m_sPK_Device_AllOrbiters_AllowingPopups; }

	//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_File_Name_and_Path();
	string DATA_Get_Mapping();
	int DATA_Get_ThreshHold();
	bool DATA_Get_Ignore();
	int DATA_Get_Width();
	int DATA_Get_Height();
	int DATA_Get_Quantity();

			*****EVENT***** accessors inherited from base class
	void EVENT_Follow_Me_Lighting(int iPK_Orbiter,int iPK_Device,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Follow_Me_Climate(int iPK_Orbiter,int iPK_Device,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Follow_Me_Media(int iPK_Orbiter,int iPK_Device,int iPK_Users,int iPK_EntArea,int iPK_EntArea_Left);
	void EVENT_Follow_Me_Telecom(int iPK_Orbiter,int iPK_Device,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Follow_Me_Security(int iPK_Orbiter,int iPK_Device,int iPK_Room,int iPK_Users,int iPK_Room_Left);
	void EVENT_Device_Detected(string sMac_Address,string sText,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory,string sSignature);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #58 - Set Current User */
	/** Set what user is currently using the orbiter.  The 'From' device is assumed to be the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

	virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** Set what entertainment area the orbiter (the 'from' in the message) is in. */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #77 - Set Current Room */
	/** Set what room the orbiter is in. */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #78 - New Orbiter */
	/** Tells orbiter plugin to add a new orbiter, or update the parameters on an existing one. */
		/** @param #2 PK_Device */
			/** If 0 is passed in, the new orbiter device is returned.  Otherwise, update this orbiter. */
		/** @param #14 Type */
			/** CE, Windows, Linux, Symbian60, MsSmartphone */
		/** @param #17 PK_Users */
			/** The primary user of the phone. */
		/** @param #44 PK_DeviceTemplate */
			/** What type of orbiter it is. */
		/** @param #47 Mac address */
			/** The MAC Address of the phone. */
		/** @param #57 PK_Room */
			/** The default room */
		/** @param #60 Width */
			/** Screen Width */
		/** @param #61 Height */
			/** Screen Height */
		/** @param #141 PK_Skin */
			/** The skin, 0=use default */
		/** @param #142 PK_Language */
			/** The language, 0=use default */
		/** @param #143 PK_Size */
			/** The size, 0=use default */

	virtual void CMD_New_Orbiter(string sType,int iPK_Users,int iPK_DeviceTemplate,string sMac_address,int iPK_Room,int iWidth,int iHeight,int iPK_Skin,int iPK_Language,int iPK_Size,int *iPK_Device) { string sCMD_Result; CMD_New_Orbiter(sType.c_str(),iPK_Users,iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,iWidth,iHeight,iPK_Skin,iPK_Language,iPK_Size,iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_New_Orbiter(string sType,int iPK_Users,int iPK_DeviceTemplate,string sMac_address,int iPK_Room,int iWidth,int iHeight,int iPK_Skin,int iPK_Language,int iPK_Size,int *iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #79 - Add Unknown Device */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that the device is a phone and it should be added */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address) { string sCMD_Result; CMD_Add_Unknown_Device(sText.c_str(),sID.c_str(),sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #183 - Get Floorplan Layout */
	/** Gets the layout of all floorplans for the orbiter. */
		/** @param #5 Value To Assign */
			/** A | delimited list in the format, where {} indicate a repeating value: #pages,{#Types,{#Objects,{DeviceDescription, ObjectDescription, FillX Point, FillY Point, PK_DesignObj, Page, PK_Device, Type}}} */

	virtual void CMD_Get_Floorplan_Layout(string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Floorplan_Layout(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Floorplan_Layout(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #186 - Get Current Floorplan */
	/** Gets the current Floorplan status (ie what items are on/off, etc.) for the specified Floorplan type. */
		/** @param #5 Value To Assign */
			/** The status of all the devices within the floorplan. */
		/** @param #10 ID */
			/** The page number (ie the floorplan ID based). */
		/** @param #46 PK_FloorplanType */
			/** The type of floorplan (lights, climate, etc.) */

	virtual void CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Current_Floorplan(sID.c_str(),iPK_FloorplanType,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #255 - Orbiter Registered */
	/** Indicates the orbiter has registered, or unregistered */
		/** @param #8 On/Off */
			/** 1 means it is registering, 0 means it is closing */
		/** @param #17 PK_Users */
			/** If registering, the current user */
		/** @param #19 Data */
			/** a serialized map containing the media types,file list screens, so the orbiter can display the file lists without asking media plugin each time */
		/** @param #45 PK_EntertainArea */
			/** If registering, the current EA */
		/** @param #57 PK_Room */
			/** If registering, the current room */

	virtual void CMD_Orbiter_Registered(string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size) { string sCMD_Result; CMD_Orbiter_Registered(sOnOff.c_str(),iPK_Users,sPK_EntertainArea.c_str(),iPK_Room,pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Orbiter_Registered(string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #261 - Set Follow-Me */
	/** Specifies whether the orbiter has follow-me on or off for the given user. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #9 Text */
			/** Can be 'L', 'M', 'C', 'S', 'T' for Lighting, Media, Climate, Security, Telecom followed by 0 or 1 for off/on. */
		/** @param #17 PK_Users */
			/** The User */

	virtual void CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users) { string sCMD_Result; CMD_Set_FollowMe(iPK_Device,sText.c_str(),iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #266 - Regen Orbiter */
	/** Regenerates an Orbiter.  When regeneration is complete, the "Regen Orbiter Finished" command will be sent */
		/** @param #2 PK_Device */
			/** The Orbiter to regenerate */
		/** @param #21 Force */
			/** Can be -r to force a full regen, or -a for a quick one */
		/** @param #24 Reset */
			/** 'Y' or '1' means reset the router before doing the regen */

	virtual void CMD_Regen_Orbiter(int iPK_Device,string sForce,string sReset) { string sCMD_Result; CMD_Regen_Orbiter(iPK_Device,sForce.c_str(),sReset.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Regen_Orbiter(int iPK_Device,string sForce,string sReset,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #267 - Regen Orbiter Finished */
	/** Regeneration of the indicated Orbiter has been finished */
		/** @param #2 PK_Device */
			/** The Orbiter */

	virtual void CMD_Regen_Orbiter_Finished(int iPK_Device) { string sCMD_Result; CMD_Regen_Orbiter_Finished(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Regen_Orbiter_Finished(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #404 - Set Auto Switch to Remote */
	/** Specifies whether the given orbiter will automatically switch to the remote control when media starts. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #119 True/False */
			/** If true it will switch automatically */

	virtual void CMD_Set_Auto_Switch_to_Remote(int iPK_Device,bool bTrueFalse) { string sCMD_Result; CMD_Set_Auto_Switch_to_Remote(iPK_Device,bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Set_Auto_Switch_to_Remote(int iPK_Device,bool bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #406 - Display Message */
	/** Displays a message on orbiters, or all orbiters. */
		/** @param #9 Text */
			/** The message to display */
		/** @param #14 Type */
			/** For devices implementing VFD_LCD_Base, this is the message type defined in the header */
		/** @param #50 Name */
			/** you can give the message a name, such as "status", "error", etc */
		/** @param #102 Time */
			/** Number of seconds to display the message for */
		/** @param #103 List PK Device */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device) { string sCMD_Result; CMD_Display_Message(sText.c_str(),sType.c_str(),sName.c_str(),sTime.c_str(),sList_PK_Device.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #686 - Display Dialog Box On Orbiter */
	/** Display a dialog box on orbiters, or all orbiter, with 0-4 custom buttons. */
		/** @param #9 Text */
			/** The message to display */
		/** @param #39 Options */
			/** A pipe delimited list with options and messages like this: option1|message1|options2|message2 */
		/** @param #103 List PK Device */
			/** A comma delimited list of orbiters, or all orbiters if empty */

	virtual void CMD_Display_Dialog_Box_On_Orbiter(string sText,string sOptions,string sList_PK_Device) { string sCMD_Result; CMD_Display_Dialog_Box_On_Orbiter(sText.c_str(),sOptions.c_str(),sList_PK_Device.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_Dialog_Box_On_Orbiter(string sText,string sOptions,string sList_PK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #693 - Send File To Phone */
	/** Send a file to the phone. */
		/** @param #47 Mac address */
			/** Phone's mac address */
		/** @param #137 Command Line */
			/** Command line to be sent */
		/** @param #140 App_Server_Device_ID */
			/** App_Server which will spawn the application */

	virtual void CMD_Send_File_To_Phone(string sMac_address,string sCommand_Line,int iApp_Server_Device_ID) { string sCMD_Result; CMD_Send_File_To_Phone(sMac_address.c_str(),sCommand_Line.c_str(),iApp_Server_Device_ID,sCMD_Result,NULL);};
	virtual void CMD_Send_File_To_Phone(string sMac_address,string sCommand_Line,int iApp_Server_Device_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #694 - Get Orbiter Status */
	/** Reports the status of the given orbiter */
		/** @param #2 PK_Device */
			/** The orbiter */
		/** @param #5 Value To Assign */
			/** The status: O=OK to load, N=New, skin generated, need router reset, n=new, no skins at all, R=Regenerating skin now, r=Regenerating skin for new orbiter, U=Unknown, D=Device is not an orbiter */
		/** @param #9 Text */
			/** If a regen is in progress, this is a status. */
		/** @param #48 Value */
			/** If a regen is in progress, this is a percentage */

	virtual void CMD_Get_Orbiter_Status(int iPK_Device,string *sValue_To_Assign,string *sText,int *iValue) { string sCMD_Result; CMD_Get_Orbiter_Status(iPK_Device,sValue_To_Assign,sText,iValue,sCMD_Result,NULL);};
	virtual void CMD_Get_Orbiter_Status(int iPK_Device,string *sValue_To_Assign,string *sText,int *iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #695 - Get Orbiter Options */
	/** Get various options a user may want to setup his orbiter, such as the skins, users, etc. */
		/** @param #5 Value To Assign */
			/** The response in:
ID\tDescription\n
format */
		/** @param #9 Text */
			/** One of the following: Users, Room, Skin, Language, Size, MenuMenu */

	virtual void CMD_Get_Orbiter_Options(string sText,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Orbiter_Options(sText.c_str(),sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Orbiter_Options(string sText,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #758 - Send Orbiter Popups */
	/** Indicates if Orbiter should receive popup messages */
		/** @param #119 True/False */
			/** True = yes, I want popups */
		/** @param #198 PK_Orbiter */
			/** The orbiter */

	virtual void CMD_Send_Orbiter_Popups(bool bTrueFalse,int iPK_Orbiter) { string sCMD_Result; CMD_Send_Orbiter_Popups(bTrueFalse,iPK_Orbiter,sCMD_Result,NULL);};
	virtual void CMD_Send_Orbiter_Popups(bool bTrueFalse,int iPK_Orbiter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #818 - Get Screen Saver Files */
	/** Given an Orbiter, returns the files that Orbiter should use for it's screen saver */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #13 Filename */
			/** A \n delimited list of the files to use */

	virtual void CMD_Get_Screen_Saver_Files(int iPK_Device,string *sFilename) { string sCMD_Result; CMD_Get_Screen_Saver_Files(iPK_Device,sFilename,sCMD_Result,NULL);};
	virtual void CMD_Get_Screen_Saver_Files(int iPK_Device,string *sFilename,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #828 - Get List of Remotes */
	/** Get the list of follow-me enabled remote controls */
		/** @param #9 Text */
			/** The list of remotes in the format:
]PK_Device],[PK_User],[RemoteID] */

	virtual void CMD_Get_List_of_Remotes(string *sText) { string sCMD_Result; CMD_Get_List_of_Remotes(sText,sCMD_Result,NULL);};
	virtual void CMD_Get_List_of_Remotes(string *sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #829 - Get Remote ID */
	/** For the special follow-me controls, this takes in a serial number and returns a unique 8-bit id for the remote, and the PK_Device */
		/** @param #2 PK_Device */
			/** The device id */
		/** @param #48 Value */
			/** The 8-bit unique id for this remote */
		/** @param #206 UID */
			/** The serial number */

	virtual void CMD_Get_Remote_ID(string sUID,int *iPK_Device,int *iValue) { string sCMD_Result; CMD_Get_Remote_ID(sUID.c_str(),iPK_Device,iValue,sCMD_Result,NULL);};
	virtual void CMD_Get_Remote_ID(string sUID,int *iPK_Device,int *iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #830 - Set Active Remote */
	/** Specified which remote control is controling a particular device. */
		/** @param #2 PK_Device */
			/** The device that is controlling it */
		/** @param #197 Fire Event */
			/** If true, a follow me will get fired for this remote */
		/** @param #198 PK_Orbiter */
			/** The orbiter being controlled */

	virtual void CMD_Set_Active_Remote(int iPK_Device,bool bFire_Event,int iPK_Orbiter) { string sCMD_Result; CMD_Set_Active_Remote(iPK_Device,bFire_Event,iPK_Orbiter,sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Remote(int iPK_Device,bool bFire_Event,int iPK_Orbiter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

	virtual void CMD_Abort_Task(int iParameter_ID) { string sCMD_Result; CMD_Abort_Task(iParameter_ID,sCMD_Result,NULL);};
	virtual void CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #918 - Start AV Wizard */
	/** Start AV Wizard on the given device. */
		/** @param #2 PK_Device */
			/** The device to do this for. */

	virtual void CMD_Start_AV_Wizard(int iPK_Device) { string sCMD_Result; CMD_Start_AV_Wizard(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Start_AV_Wizard(int iPK_Device,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
