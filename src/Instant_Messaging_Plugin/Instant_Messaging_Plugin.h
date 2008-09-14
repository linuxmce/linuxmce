/*
     Copyright (C) 2008 LinuxMCE

     www.linuxmce.org

     Author: Thomas Cherryhomes <thom.cherryhomes@linuxmce.org>
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Instant_Messaging_Plugin_h
#define Instant_Messaging_Plugin_h

//	DCE Implemenation for #1958 Instant Messaging Plugin

#include "Gen_Devices/Instant_Messaging_PluginBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Instant_Messaging_Plugin : public Instant_Messaging_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Instant_Messaging_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Instant_Messaging_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Instant_Messaging_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Online();

			*****EVENT***** accessors inherited from base class
	void EVENT_Instant_Message_Recieved(string sText,int iPK_Users,string sIM_User_ID);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #981 - Send IM */
	/** Send Instant Message */
		/** @param #9 Text */
			/** The Message to send */
		/** @param #17 PK_Users */
			/** The LinuxMCE user the message is from. */
		/** @param #282 IM User */
			/** The IM User this message is being sent to. */

	virtual void CMD_Send_IM(string sText,int iPK_Users,string sIM_User) { string sCMD_Result; CMD_Send_IM(sText.c_str(),iPK_Users,sIM_User.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_IM(string sText,int iPK_Users,string sIM_User,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
