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
#ifndef Asterisk_h
#define Asterisk_h

//	DCE Implemenation for #45 Asterisk

#include "Gen_Devices/AsteriskBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Asterisk : public Asterisk_Command
	{
//<-dceag-decl-e->
		// Private member variables
		pthread_t voicemailThread;
		std::map <int,string> dev2ext;
		// Private methods
		bool IsAsteriskServerRunning();
public:
		// Public member variables
		int GetMBQuit() {return m_bQuit_get();};
		virtual void PostConnect();
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Asterisk(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Asterisk();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual bool Connect(int iPK_DeviceTemplate);
		virtual void CreateChildren();

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Manual_configuration();
	string DATA_Get_Server_IP();

			*****EVENT***** accessors inherited from base class
	void EVENT_PBX_CommandResult(int iCommandID,int iResult,string sMessage);
	void EVENT_PBX_Ring(string sSource_Channel,string sDestination_Channel,string sSource_Caller_ID,string sDestination_Caller_ID);
	void EVENT_Incoming_Call(string sPhoneCallerID);
	void EVENT_Voice_Mail_Changed(int iValue,int iPK_Users);
	void EVENT_PBX_Hangup(string sChannel_ID,string sReason);
	void EVENT_Extensions_Status(string sText);
	void EVENT_Calls_Status(string sText);
	void EVENT_PBX_Link(string sSource_Channel,string sDestination_Channel,string sSource_Caller_ID,string sDestination_Caller_ID);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #233 - PBX_Originate */
	/** Originate a call */
		/** @param #75 PhoneNumber */
			/** Phone id from which to place the call */
		/** @param #82 PhoneType */
			/** Phone type from which to place the call */
		/** @param #83 PhoneExtension */
			/** Extension to dial */
		/** @param #84 PhoneCallerID */
			/** Caller id */

	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID) { string sCMD_Result; CMD_PBX_Originate(sPhoneNumber.c_str(),sPhoneType.c_str(),sPhoneExtension.c_str(),sPhoneCallerID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #235 - PBX_Transfer */
	/** Transfer a call to other phone */
		/** @param #83 PhoneExtension */
			/** Phone extension to redirect to */
		/** @param #265 Channel 1 */
			/** Channel 1 to transfer */
		/** @param #266 Channel 2 */
			/** Channel 2 to transfer (can be empty) */

	virtual void CMD_PBX_Transfer(string sPhoneExtension,string sChannel_1,string sChannel_2) { string sCMD_Result; CMD_PBX_Transfer(sPhoneExtension.c_str(),sChannel_1.c_str(),sChannel_2.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PBX_Transfer(string sPhoneExtension,string sChannel_1,string sChannel_2,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #237 - PBX_Hangup */
	/** Hangs up a call */
		/** @param #264 Channel */
			/** The channel id to hang up */

	virtual void CMD_PBX_Hangup(string sChannel) { string sCMD_Result; CMD_PBX_Hangup(sChannel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PBX_Hangup(string sChannel,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #922 - Send Asterisk Status */
	/** Ask for current extensions and calls status. */

	virtual void CMD_Send_Asterisk_Status() { string sCMD_Result; CMD_Send_Asterisk_Status(sCMD_Result,NULL);};
	virtual void CMD_Send_Asterisk_Status(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1084 - PBX_Application */
	/** Link an extension to a dialplan application.  (Ex. call an extension and playback a sound file using the playback application) */
		/** @param #83 PhoneExtension */
			/** Extension to dial */
		/** @param #109 Data String */
			/** Application data */
		/** @param #285 Application */
			/** Application to connect with */

	virtual void CMD_PBX_Application(string sPhoneExtension,string sData_String,string sApplication) { string sCMD_Result; CMD_PBX_Application(sPhoneExtension.c_str(),sData_String.c_str(),sApplication.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PBX_Application(string sPhoneExtension,string sData_String,string sApplication,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
