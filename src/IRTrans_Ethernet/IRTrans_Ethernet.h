/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef IRTrans_Ethernet_h
#define IRTrans_Ethernet_h

//	DCE Implemenation for #1986 IRTrans Ethernet

#include "Gen_Devices/IRTrans_EthernetBase.h"
//<-dceag-d-e->


#include "../LIRC_DCE/IRReceiverBase.h"
#include "../VFD_LCD/VFD_LCD_Base.h"
#include "IRBase/IRBase.h"

//<-dceag-decl-b->!
namespace DCE
{
	class IRTrans_Ethernet : public IRTrans_Ethernet_Command, public IRReceiverBase, public VFD_LCD_Base, public IRBase
	{
//<-dceag-decl-e->
		// Private member variables
		int irtrans_socket;
		struct sockaddr_in server_addr;
		struct hostent *host;

		map<string,int> m_mapNameToDevice;

		// Private methods
public:
		// Public member variables
		virtual void SendIR(string Port, string IRCode,int iRepeat); // Required from IRBase
		virtual void CreateChildren(); // Must override so we can call IRBase::Start() after creating children

//<-dceag-const-b->
public:
		// Constructors/Destructor
		IRTrans_Ethernet(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~IRTrans_Ethernet();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		
		virtual void OnQuit() { LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans_Ethernet will quit"); Command_Impl::OnQuit(); };
		virtual void OnReload() { LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans_Ethernet will reload");Command_Impl::OnQuit(); }

//<-dceag-const2-b->!
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_TCP_Port();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

	virtual void CMD_Toggle_Power(string sOnOff) { string sCMD_Result; CMD_Toggle_Power(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage);


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

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
