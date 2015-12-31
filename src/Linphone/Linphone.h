/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Linphone_h
#define Linphone_h

//	DCE Implemenation for #1602 Linphone

#include "Gen_Devices/LinphoneBase.h"
//<-dceag-d-e->

#include "LinphoneManager.h"

//<-dceag-decl-b->!
namespace DCE
{
	class Linphone : public Linphone_Command, public LinphoneMangerInterceptor
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Linphone(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Linphone();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual bool Connect(int iPK_DeviceTemplate);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Linphone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_PhoneType();
	string DATA_Get_PhoneNumber();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #334 - Phone_Initiate */
	/** Initiates a call */
		/** @param #83 PhoneExtension */
			/** Extention to dial */

	virtual void CMD_Phone_Initiate(string sPhoneExtension) { string sCMD_Result; CMD_Phone_Initiate(sPhoneExtension.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Phone_Initiate(string sPhoneExtension,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #335 - Phone_Answer */
	/** Answer a call */

	virtual void CMD_Phone_Answer() { string sCMD_Result; CMD_Phone_Answer(sCMD_Result,NULL);};
	virtual void CMD_Phone_Answer(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #336 - Phone_Drop */
	/** Drop a call */

	virtual void CMD_Phone_Drop() { string sCMD_Result; CMD_Phone_Drop(sCMD_Result,NULL);};
	virtual void CMD_Phone_Drop(string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->

protected:
	virtual void OnConnected();
	virtual void OnFailConnecting();
	virtual void OnInvite();

private:
	LinphoneManager manager_;
	
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
