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
#ifndef SimplePhone_h
#define SimplePhone_h

//	DCE Implemenation for #1759 Orbiter Embedded Phone

#include "Gen_Devices/SimplePhoneBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class SimplePhone : public SimplePhone_Command
	{
//<-dceag-decl-e->
private:
        // Private methods
        // Private member variables
        pthread_t m_SIP_Thread;
		string Get_MD_AudioSettings();
		string m_sExtension;
		string m_sPassword;
public:
        // Public member variables
        // Public methods
        virtual void CreateChildren();
	string GetExtension();
	string GetPassword();

	// Used to find any USB microphones
	DeviceData_Base *m_pUSBMicrophoneDevice;
	string m_sSoundCardNumber;
	
//<-dceag-const-b->
public:
		// Constructors/Destructor
		SimplePhone(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~SimplePhone();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		SimplePhone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_FloorplanObjectType();
	string DATA_Get_PhoneType();
	string DATA_Get_PhoneNumber();
	bool DATA_Get_Speak_in_the_House();
	string DATA_Get_Password();
	int DATA_Get_Volume_Level();
	string DATA_Get_Server_IP();
	string DATA_Get_Sound_Card();
	bool DATA_Get_Ignore_Video_Pipes();

			*****EVENT***** accessors inherited from base class
	void EVENT_Incoming_Call(string sPhoneCallerID);

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

//<-dceag-h-e->
    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
