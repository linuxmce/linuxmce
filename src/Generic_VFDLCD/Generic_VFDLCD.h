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
#ifndef Generic_VFDLCD_h
#define Generic_VFDLCD_h

#include <memory>
using namespace std;

//	DCE Implemenation for #1896 Generic VFD-LCD

#include "Gen_Devices/Generic_VFDLCDBase.h"
//<-dceag-d-e->
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
class LCDManager;
class LCDRenderer;
class SocketStatusInputProvider;
class Database_pluto_main;
//--------------------------------------------------------------------------------------------------------
struct LCDMessage
{
	string text;
	int type;
	string name;
	int time;

	LCDMessage(string sText, int nType, string sName, int nTime) :
		text(sText), type(nType), name(sName), time(nTime)
	{
	}
};
//--------------------------------------------------------------------------------------------------------
class ILCDMessageProvider
{
private:

	virtual void ProvideMessage(LCDMessage *) = 0;

public:

	ILCDMessageProvider() {}
	virtual ~ILCDMessageProvider() {}

	virtual LCDMessage *GetNextMessage() = 0;
	virtual void WaitForAnyMessage() = 0;
	virtual bool IsQuitting() = 0;
};
//--------------------------------------------------------------------------------------------------------
class ILCDMessageProcessor
{
public:
	
	ILCDMessageProcessor() {}
	virtual ~ILCDMessageProcessor() {}

	virtual void ProcessMessage(LCDMessage *pLCDMessage) = 0;
	virtual void Interrupt() = 0;
};
//--------------------------------------------------------------------------------------------------------
//<-dceag-decl-b->
namespace DCE
{
	class Generic_VFDLCD : public Generic_VFDLCD_Command, public ILCDMessageProvider, public ILCDMessageProcessor
	{
//<-dceag-decl-e->
		// Private member variables
		auto_ptr<MenuHolder> m_spMenu_Holder;
		auto_ptr<LCDManager> m_spLCDManager;
		auto_ptr<LCDRenderer> m_spLCDRenderer;
		auto_ptr<SocketStatusInputProvider> m_spSocketStatusInputProvider;

		pluto_pthread_mutex_t m_LCDMessageMutex;  
		pthread_cond_t m_LCDMessageCond;
		pthread_t m_WorkerThreadID;

		pluto_pthread_mutex_t m_MessageProcessingMutex;  
		pthread_cond_t m_MessageProcessingCond;

		list<LCDMessage *> m_listLCDMessages;

		Database_pluto_main *m_pDatabase_pluto_main;

		// Private methods
		void ProvideMessage(LCDMessage *);

public:

	    //Public methods
		LCDMessage *GetNextMessage();
		void WaitForAnyMessage();
		bool IsQuitting();

		void ProcessMessage(LCDMessage *pLCDMessage);
		void Interrupt();

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_VFDLCD(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_VFDLCD();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		bool Setup(string sXMLMenuFilename, string sLCDSerialPort, int nSocketServerPort);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Generic_VFDLCD(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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


	/** @brief COMMAND: #406 - Display Message */
	/** Display a message on the lcd/vfd display */
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


	/** @brief COMMAND: #837 - Show Media Playback State */
	/** Show the current state of the media playback */
		/** @param #5 Value To Assign */
			/** Empty = no media playing, otherwise a speed, 0=pause, 1000=normal forward, -4000 = 4x reverse, etc. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #76 Level */
			/** The level of the volume, from 0-100.  empty means it's not known, or "MUTE" */

	virtual void CMD_Show_Media_Playback_State(string sValue_To_Assign,int iPK_MediaType,string sLevel) { string sCMD_Result; CMD_Show_Media_Playback_State(sValue_To_Assign.c_str(),iPK_MediaType,sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Media_Playback_State(string sValue_To_Assign,int iPK_MediaType,string sLevel,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
