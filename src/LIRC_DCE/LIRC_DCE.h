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
#ifndef LIRC_DCE_h
#define LIRC_DCE_h

//	DCE Implemenation for #77 LIRC DCE

#include "Gen_Devices/LIRC_DCEBase.h"
//<-dceag-d-e->

#include "IRReceiverBase.h"
#include "IRBase/IRBase.h"
#include "DCE/DeviceData_Router.h"
#ifndef WIN32
#include <lirc/lirc_client.h>
#endif
class Database_pluto_main;


//<-dceag-decl-b->!
namespace DCE
{
        class LIRC_DCE : public LIRC_DCE_Command, IRReceiverBase, IRBase
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
		int lirc_leech(int DeviceID);
		map<string,int> m_mapNameToDevice;

//<-dceag-const-b->!
public:
		// Constructors/Destructor
		LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~LIRC_DCE();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
		void CreateChildren();
		virtual void SendIR(string Port, string IRCode, int iRepeat); // Required from IRBase
		//<-dceag-const-e->

		Router *pRoute;
		int m_DeviceID;
		pthread_t m_LeechingThread;

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Device();
	string DATA_Get_Serial_Port();
	string DATA_Get_System_Device();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command) { string sCMD_Result; CMD_Learn_IR(iPK_Device,sOnOff.c_str(),iPK_Text,iPK_Command,sCMD_Result,NULL);};
	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

	virtual void CMD_Set_Screen_Type(int iValue) { string sCMD_Result; CMD_Set_Screen_Type(iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
