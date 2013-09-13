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
#ifndef Telecom_Plugin_h
#define Telecom_Plugin_h

//	DCE Implemenation for #34 Telecom Plug-in

#include "Gen_Devices/Telecom_PluginBase.h"
//<-dceag-d-e->

#include "DCE/DeviceData_Router.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
#include "Orbiter/Floorplan.h"

#include "ExtensionStatus.h"
#include "CallStatus.h"
#include "TelecomTask.h"

#include <pthread.h>

class Database_pluto_main;
class Database_pluto_telecom;
class Row_PhoneNumber;

namespace DCE
{
	class PendingCall
	{
		public:
			int iDeviceOrbiter;
			int iDeviceOwner;
	};
}

//<-dceag-decl-b->!
namespace DCE
{
	class Telecom_Plugin : public Telecom_Plugin_Command, public DataGridGeneratorPlugIn, public FloorplanInfoProvider, public FollowMe_Device
	{
//<-dceag-decl-e->
	// Private member variables
	pthread_t m_displayThread;
	pthread_mutex_t mtx_err_messages;
	pluto_pthread_mutex_t m_TelecomMutex;
	pthread_mutexattr_t m_MutexAttr;
	std::map<int, std::pair<string, string> > m_mapVoiceMailStatus;
	DeviceData_Router* m_pDevice_pbx;
	int m_nPBXDevice;

	string m_sDestChannel;
	bool m_bReplacedChannel;

	bool m_bBabyMonitorActive;

	std::map<int, bool> m_mapOrbiter_IsUsingLiveAV; // Is the Orbiter this SimplePhone is attached to, using LiveAV?
	std::map<int, bool> m_mapOrbiter_WasPaused;     // Is the Orbiter this SimplePHone is attached to, currently paused by the user, and not the Telecom Plugin?

	// Los93soL
	std::map<string,string> m_mapChannel2CallerID;

	// Private methods
public:
	// Public member variables
	// Public methods
	void doDisplayMessages();
	
	/**
	  * Parser for channel information.
	  */
	static bool ParseChannel(   const std::string channel,
								std::string* psphone,
								std::string* psphonetype = 0,
								std::string* psid = 0 );
	
	static string GetMainChannel(const string channel);
	
	/** Just the extension, no context or other extra information.*/
	static string GetSimpleExtension(const string&);
	
	// Telecom API for tasks
	/***/
	bool FindCallStatusForChannel(string sChannelID, map<string, CallStatus*> & calls4channel);
	/***/
	CallStatus* FindCallStatusForChannel(string sChannelID);
	/***/
	CallStatus* FindCallStatusForID(string sCallID);
	/***/
	string FindChannelForExt(CallStatus *, string ext);
	/***/
	string FindChannelForExt(string ext);
	/***/
	string DirectCall2Conference(CallStatus*);

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Telecom_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Telecom_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	virtual void PrepareToDelete();

	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;
	Database_pluto_telecom *m_pDatabase_pluto_telecom;

	// Parse the metadata for the voicemail grid
	string ParseVoiceMailMetadata(string s_VoiceMailFile, map<string, string>& mapVMData);

	// Datagrids
	class DataGridTable *TelecomScenariosGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *PhoneBookAutoCompl(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *PhoneBookListOfNos(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *ActiveCallsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *RecentCallsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *SpeedDialGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *UserVoiceMailGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *ActiveUsersOnCallGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *ExternalChannels(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	
	// Follow-me
	bool TelecomFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);

	// Floorplan
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar);

	// Other
	string GetDialNumber(Row_PhoneNumber *pRow_PhoneNumber);
	void HangupAllCalls();

	// Interceptors
	bool OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);

	// Telecom status
	bool CallsStatusChanged(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
	bool ExtensionsStatusChanged(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Telecom_Local_Prefix();
	int DATA_Get_Telecom_Prepend_Digit();
	int DATA_Get_Telecom_Local_Number_Length();
	int DATA_Get_No_of_sec_to_ring_before_IVR();
	string DATA_Get_Emergency_numbers();
	int DATA_Get_Emergency_phoneline();
	int DATA_Get_PK_Device_Phone_Baby_Monitor();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a DTMF code */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

	virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #232 - PL_Originate */
	/** Originate a call -- * deprecated, use Make call instead * */
		/** @param #2 PK_Device */
			/** Device (phone) from which to place the call */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */
		/** @param #84 PhoneCallerID */
			/** Caller ID */

	virtual void CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string sPhoneCallerID) { string sCMD_Result; CMD_PL_Originate(iPK_Device,sPhoneExtension.c_str(),sPhoneCallerID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #234 - PL_Transfer */
	/** Transfers a call to other phone */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */
		/** @param #17 PK_Users */
			/** User ID to transfer call to */
		/** @param #83 PhoneExtension */
			/** Local Extension to transfer call to */
		/** @param #265 Channel 1 */
			/** Channel to transfer */
		/** @param #266 Channel 2 */
			/** Second channel to transfer (can be empty) */

	virtual void CMD_PL_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,string sChannel_1,string sChannel_2) { string sCMD_Result; CMD_PL_Transfer(iPK_Device,iPK_Users,sPhoneExtension.c_str(),sChannel_1.c_str(),sChannel_2.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,string sChannel_1,string sChannel_2,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #236 - PL_Cancel */
	/** Hangs up a call */
		/** @param #2 PK_Device */
			/** The device to hangup the call for (ie the phone or orbiter).  If 0, the from device is assumed.  If -1, all calls are terminated

* deprecated, use channel id instead * */
		/** @param #264 Channel */
			/** The channel to cancel */

	virtual void CMD_PL_Cancel(int iPK_Device,string sChannel) { string sCMD_Result; CMD_PL_Cancel(iPK_Device,sChannel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Cancel(int iPK_Device,string sChannel,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #313 - Set Volume */
	/** Set volume of Target Audio card device. 0 to 100, with 0 meaning mute and * meaning whatever the previous value was. */
		/** @param #76 Level */
			/** A value between 0 and 100 where 0 is mute and 100 is full volume.  Numbers preceded with a - or + are relative.  +15 means up 15, -10 means down 10 */

	virtual void CMD_Set_Volume(string sLevel) { string sCMD_Result; CMD_Set_Volume(sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Volume(string sLevel,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #334 - Phone_Initiate */
	/** Initiates a call */
		/** @param #2 PK_Device */
			/** The device to iniate the call from.  Used by Telecom plugin to forward to a sip phone.  Not needed when sending to a phone directly.  If this is not specified, telecom plugin assumes the correct option is the from device */
		/** @param #83 PhoneExtension */
			/** Extention to dial */

	virtual void CMD_Phone_Initiate(int iPK_Device,string sPhoneExtension) { string sCMD_Result; CMD_Phone_Initiate(iPK_Device,sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Phone_Initiate(int iPK_Device,string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #335 - Phone_Answer */
	/** Answer a call */

	virtual void CMD_Phone_Answer() { string sCMD_Result; CMD_Phone_Answer(sCMD_Result,NULL);};
	virtual void CMD_Phone_Answer(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #336 - Phone_Drop */
	/** Drop a call */

	virtual void CMD_Phone_Drop() { string sCMD_Result; CMD_Phone_Drop(sCMD_Result,NULL);};
	virtual void CMD_Phone_Drop(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #414 - PL External Originate */
	/** Originate an external call -- * deprecated, use Make call instead * */
		/** @param #75 PhoneNumber */
			/** Phone to call */
		/** @param #81 CallerID */
			/** Caller ID */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */

	virtual void CMD_PL_External_Originate(string sPhoneNumber,string sCallerID,string sPhoneExtension) { string sCMD_Result; CMD_PL_External_Originate(sPhoneNumber.c_str(),sCallerID.c_str(),sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_External_Originate(string sPhoneNumber,string sCallerID,string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #744 - Set User Mode */
	/** Set a user to a given mode (sleeping, etc.) */
		/** @param #17 PK_Users */
			/** The user */
		/** @param #194 PK_UserMode */
			/** The user mode */

	virtual void CMD_Set_User_Mode(int iPK_Users,int iPK_UserMode) { string sCMD_Result; CMD_Set_User_Mode(iPK_Users,iPK_UserMode,sCMD_Result,NULL);};
	virtual void CMD_Set_User_Mode(int iPK_Users,int iPK_UserMode,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #751 - PL_Add_VOIP_Account */
	/** Add a VOIP account */
		/** @param #50 Name */
			/** Provider name */
		/** @param #75 PhoneNumber */
			/** Phone number */
		/** @param #99 Password */
			/** Password */
		/** @param #189 Users */
			/** User name */

	virtual void CMD_PL_Add_VOIP_Account(string sName,string sPhoneNumber,string sPassword,string sUsers) { string sCMD_Result; CMD_PL_Add_VOIP_Account(sName.c_str(),sPhoneNumber.c_str(),sPassword.c_str(),sUsers.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PL_Add_VOIP_Account(string sName,string sPhoneNumber,string sPassword,string sUsers,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #797 - PL_Join_Call */
	/** Will join you to an existing call */
		/** @param #17 PK_Users */
			/** The user to add to call */
		/** @param #83 PhoneExtension */
			/** The extension to add to call */
		/** @param #87 PhoneCallID */
			/** Phone call ID to join to */
		/** @param #263 PK_Device_To */
			/** The device the add to call */

	virtual void CMD_PL_Join_Call(int iPK_Users,string sPhoneExtension,string sPhoneCallID,int iPK_Device_To) { string sCMD_Result; CMD_PL_Join_Call(iPK_Users,sPhoneExtension.c_str(),sPhoneCallID.c_str(),iPK_Device_To,sCMD_Result,NULL);};
	virtual void CMD_PL_Join_Call(int iPK_Users,string sPhoneExtension,string sPhoneCallID,int iPK_Device_To,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #826 - Speak in house */
	/** Lets the user speak in the house or to someone at the intercom, whether the user is in the house or outside */
		/** @param #2 PK_Device */
			/** If not 0, this is presumed to be a device associated with a phone number, such a mobile orbiter or a phone extension, and which will be patched in. */
		/** @param #75 PhoneNumber */
			/** If specified this is the number that will be patched in and PK_Device will be ignored. */
		/** @param #103 List PK Device */
			/** The list of speakerphone type devices to broadcast the voice through.  If this and  PK_Device_Related are not specified all devices are assumed.  If specified this overrides PK_Device_Related */
		/** @param #201 PK_Device_Related */
			/** Broadcast through a speakerphone related to this device.  If List PK Device  is blank then this device can be a camera, doorbell, etc., and the List PK Device will be filled in automatically with the most related device, either explicitly related, or in t */

	virtual void CMD_Speak_in_house(int iPK_Device,string sPhoneNumber,string sList_PK_Device,int iPK_Device_Related) { string sCMD_Result; CMD_Speak_in_house(iPK_Device,sPhoneNumber.c_str(),sList_PK_Device.c_str(),iPK_Device_Related,sCMD_Result,NULL);};
	virtual void CMD_Speak_in_house(int iPK_Device,string sPhoneNumber,string sList_PK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #845 - Delete File */
	/** Used to delete a voicemail from the Voicemail spool */
		/** @param #13 Filename */
			/** The file to delete */

	virtual void CMD_Delete_File(string sFilename) { string sCMD_Result; CMD_Delete_File(sFilename.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Delete_File(string sFilename,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #921 - Make Call */
	/** Create a call. */
		/** @param #17 PK_Users */
			/** The called user. Only one is supported now. */
		/** @param #83 PhoneExtension */
			/** The phone number to be called. */
		/** @param #184 PK_Device_From */
			/** The device which starts the call. */
		/** @param #263 PK_Device_To */
			/** The called device. */

	virtual void CMD_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To) { string sCMD_Result; CMD_Make_Call(iPK_Users,sPhoneExtension.c_str(),sPK_Device_From.c_str(),iPK_Device_To,sCMD_Result,NULL);};
	virtual void CMD_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #924 - Merge Calls */
	/** Merge two calls */
		/** @param #267 Phone Call ID 1 */
			/** First call id */
		/** @param #268 Phone Call ID 2 */
			/** Second call id */

	virtual void CMD_Merge_Calls(string sPhone_Call_ID_1,string sPhone_Call_ID_2) { string sCMD_Result; CMD_Merge_Calls(sPhone_Call_ID_1.c_str(),sPhone_Call_ID_2.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Merge_Calls(string sPhone_Call_ID_1,string sPhone_Call_ID_2,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #925 - Assisted Transfer */
	/** Interactive transfer, not just a blind transfer.
The response is a task id */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */
		/** @param #17 PK_Users */
			/** User ID to transfer call to */
		/** @param #83 PhoneExtension */
			/** Extension to transfer call to */
		/** @param #87 PhoneCallID */
			/** Phone call id to transfer */
		/** @param #257 Task */
			/** Task ID */
		/** @param #264 Channel */
			/** The channel of the owner of the assisted transfer */

	virtual void CMD_Assisted_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,string sPhoneCallID,string sChannel,string *sTask) { string sCMD_Result; CMD_Assisted_Transfer(iPK_Device,iPK_Users,sPhoneExtension.c_str(),sPhoneCallID.c_str(),sChannel.c_str(),sTask,sCMD_Result,NULL);};
	virtual void CMD_Assisted_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,string sPhoneCallID,string sChannel,string *sTask,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #926 - Process Task */
	/** Process a task with actions like "complete transfer now", "merge calls", etc. */
		/** @param #257 Task */
			/** The task id */
		/** @param #258 Job */
			/** The job type */

	virtual void CMD_Process_Task(string sTask,string sJob) { string sCMD_Result; CMD_Process_Task(sTask.c_str(),sJob.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Process_Task(string sTask,string sJob,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #927 - Add Extensions To Call */
	/** Add comma delimited list with extensions to call */
		/** @param #87 PhoneCallID */
			/** The phone call id */
		/** @param #274 Extensions */
			/** Comma delimited list with extensions */

	virtual void CMD_Add_Extensions_To_Call(string sPhoneCallID,string sExtensions) { string sCMD_Result; CMD_Add_Extensions_To_Call(sPhoneCallID.c_str(),sExtensions.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_Extensions_To_Call(string sPhoneCallID,string sExtensions,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #928 - Get Associated Picture For Channel */
	/** Get the associated picture for channel */
		/** @param #19 Data */
			/** The picture associated */
		/** @param #264 Channel */
			/** The channel id */

	virtual void CMD_Get_Associated_Picture_For_Channel(string sChannel,char **pData,int *iData_Size) { string sCMD_Result; CMD_Get_Associated_Picture_For_Channel(sChannel.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Get_Associated_Picture_For_Channel(string sChannel,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #929 - Add To Speed Dial */
	/** Add to favorites */
		/** @param #2 PK_Device */
			/** The device id */
		/** @param #81 CallerID */
			/** The called id */
		/** @param #83 PhoneExtension */
			/** Phone extension */

	virtual void CMD_Add_To_Speed_Dial(int iPK_Device,string sCallerID,string sPhoneExtension) { string sCMD_Result; CMD_Add_To_Speed_Dial(iPK_Device,sCallerID.c_str(),sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_To_Speed_Dial(int iPK_Device,string sCallerID,string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1118 - Phone to Baby Monitor */
	/** Speak in House, except PK_Device is unmated, while all the other SimplePhones are muted. */
		/** @param #2 PK_Device */
			/** If not 0, this is presumed to be a device associated with a phone number, such a mobile orbiter or a phone extension, and which will be patched in. */
		/** @param #75 PhoneNumber */
			/** If specified this is the number that will be patched in and PK_Device will be ignored. */
		/** @param #103 List PK Device */
			/** The list of speakerphone type devices to broadcast the voice through.  If this and  PK_Device_Related are not specified all devices are assumed.  If specified this overrides PK_Device_Related */
		/** @param #201 PK_Device_Related */
			/** Broadcast through a speakerphone related to this device.  If List PK Device  is blank then this device can be a camera, doorbell, etc., and the List PK Device will be filled in automatically with the most related device, either explicitly related, or in t */

	virtual void CMD_Phone_to_Baby_Monitor(int iPK_Device,string sPhoneNumber,string sList_PK_Device,int iPK_Device_Related) { string sCMD_Result; CMD_Phone_to_Baby_Monitor(iPK_Device,sPhoneNumber.c_str(),sList_PK_Device.c_str(),iPK_Device_Related,sCMD_Result,NULL);};
	virtual void CMD_Phone_to_Baby_Monitor(int iPK_Device,string sPhoneNumber,string sList_PK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

private:
	 bool CommandResult( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	 void ProcessResult(int iCommandID, int iResult, std::string sMessage);
	 
	 bool Ring( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	 
	 bool Link( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	 void ProcessRing(std::string sPhoneExtension, std::string sPhoneCallerID, std::string sPhoneCallID);
	 
	bool IncomingCall( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool Hangup( class Socket *pSocket, class Message *pMessage,
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );	
	bool VoIP_Problem( class Socket *pSocket, class Message *pMessage,
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );	
	bool VoiceMailChanged( class Socket *pSocket, class Message *pMessage,
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );	
	bool PlaybackStarted ( class Socket *pSocekt, class Message *pMessage, class DeviceData_Base *pDeviceFrom, 
			       class DeviceData_Base *pDeviceTo);
private:
	DeviceData_Router* find_AsteriskDevice();
	DeviceData_Router* find_Device(int iPK_Device);
	
	/***/
	ExtensionStatus* FindExtensionStatus(string sExt);
	/***/
	string GetNewConferenceID();
	/***/
	CallStatus * LinkChannels(const string&, const string&);
	/***/
	bool RemoveChannel(const string&);
	/***/
	void ReplaceChannels(const string&, const string&, const string&);
	/***/
	void RemoveCallStatus(CallStatus*);
	/***/
	void RemoveExtensionStatus(string);
	/***/
	void CleanStatusMaps();
	/***/
	void CleanTasks();
	/***/
	string DebugPendingCalls() const;
	
	/***/
	ExtensionStatus * FindExtensionStatusByDevice(int iPK_Device, int * pEmbeddedPhone = NULL);
	/***/
	void GetChannelsFromExtension(const string & ext, map<string, string> & channelsExt) const;
	/***/
	string GetPhoneNumber(int iPK_Users,string sPhoneExtension,int iPK_Device_To);
	
	/** Call external number OR caller id
	  * Call first extension OR caller id
	  */
	string GetCallName(CallStatus *) const;

	/***/
	void DumpActiveCalls();

	/**  
	  *   Get orbiter device id for and extension OR channel
	  */
	int GetOrbiterDeviceID(string sExten, string sChannel = "");

	/***/
	CallStatus *FindConferenceByConferenceID(unsigned int unConferenceID);

	/*
	 *	Make a call
	 */
	bool InternalMakeCall(int iFK_Device_From, string sFromExten, string sPhoneNumberToCall, Message *pMessage, bool bStopPause=true);

	/*
	 *	Get the embedded phone from the same room with device
	 */
	bool GetEmbeddedPhoneAssociated(int nDeviceID, int& nEmbeddedPhoneID);

	/*
	 *	Associates a caller name for a channel
	 */

	string GetCallerName(string sChannel, string sCallerID);

	/*
	 *	sound card settings : concurrent access allowed?
	 */
	bool ConcurrentAccessToSoundCardAllowed(int nOrbiterID);

	/**
	 * Turn On Media Director associated with Simplephone Orbiter
	 */
	void TurnOnMediaDirectorAttachedToOrbiter(int nOrbiterID);

private:
	int iCmdCounter;
	int generate_NewCommandID();
	map<int,int> map_orbiter2embedphone;
	map<int,int> map_embedphone2orbiter;
	map<string,int> map_ext2device;
	map<int,string> map_device2ext;
	map<string,string> map_username2ext;
	map<string,string> map_ext2username;
	
	map<string, ExtensionStatus*> map_ext2status;
	map<string, CallStatus*> map_call2status;
	map<unsigned, CallStatus*> map_conference2status;
	map<unsigned, string> map_newconference;
	map<string, TelecomTask*> map_id2task;
	map<string, PendingCall*> map_ext2pending;
	
	map<string,long> map_err_messages;
	unsigned long next_conf_room;
		
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
