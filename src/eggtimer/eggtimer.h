/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef eggtimer_h
#define eggtimer_h

//	DCE Implemenation for #2336 Egg Timer

#include "Gen_Devices/eggtimerBase.h"
#include "AlarmManager.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class eggtimer : public eggtimer_Command, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables
		class AlarmManager *m_pAlarmManager;
		
		// Private methods
public:
		// Public member variables
		void AlarmCallback(int id, void* param);

//<-dceag-const-b->
public:
		// Constructors/Destructor
		eggtimer(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~eggtimer();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
		
		virtual void CommandOff(int PK_Device);
		virtual void CommandOn(int PK_Device);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		eggtimer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #1156 - Start Egg Timer */
	/** Start egg timer and send ON to device in question.  */
		/** @param #124 DeviceToLink */
			/** Device to send the ON command and to setup the egg timer. */
		/** @param #182 Timeout */
			/** Time before the OFF command is set to the device in question. */
		/** @param #290 VerifyStateDeviceID */
			/** Device ID to verify for state tripped upon end of timer. If tripped, the timer be extended. */

	virtual void CMD_Start_Egg_Timer(int iDeviceToLink,string sTimeout,int iVerifyStateDeviceID) { string sCMD_Result; CMD_Start_Egg_Timer(iDeviceToLink,sTimeout.c_str(),iVerifyStateDeviceID,sCMD_Result,NULL);};
	virtual void CMD_Start_Egg_Timer(int iDeviceToLink,string sTimeout,int iVerifyStateDeviceID,string &sCMD_Result,Message *pMessage);



	/** @brief COMMAND: #1147 - Cancel Egg Timer */
	/** Manually stop egg timer  */
		/** @param #124 DeviceToLink */
			/** Device ID with associated egg timer */
		/** @param #289 SendOFF */
			/** Do send the off command to the designated device. */

	virtual void CMD_Cancel_Egg_Timer(int iDeviceToLink,bool bSendOFF) { string sCMD_Result; CMD_Cancel_Egg_Timer(iDeviceToLink,bSendOFF,sCMD_Result,NULL);};
	virtual void CMD_Cancel_Egg_Timer(int iDeviceToLink,bool bSendOFF,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
