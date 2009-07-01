/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "IRTrans_Ethernet.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

extern "C" {

#include <sys/types.h>      
#include <sys/socket.h>

}
//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
IRTrans_Ethernet::IRTrans_Ethernet(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: IRTrans_Ethernet_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
IRTrans_Ethernet::IRTrans_Ethernet(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: IRTrans_Ethernet_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
IRTrans_Ethernet::~IRTrans_Ethernet()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool IRTrans_Ethernet::GetConfig()
{
	if( !IRTrans_Ethernet_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	// read hostname string from device data
	host= (struct hostent *) gethostbyname((char *)GetIpAddress().c_str());

	if ((irtrans_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return false;
	}

	server_addr.sin_family = AF_INET;

	// read the port from device data, TCP is a bit misleading, we do UDP
	server_addr.sin_port = htons(DATA_Get_TCP_Port());

	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool IRTrans_Ethernet::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
IRTrans_Ethernet_Command *Create_IRTrans_Ethernet(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new IRTrans_Ethernet(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void IRTrans_Ethernet::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{

	map<long, string>::iterator i;
	for (i = pMessage->m_mapParameters.begin(); i != pMessage->m_mapParameters.end(); i++)
        {
		 cout << "Parameter: " << i->first << " Value: " << i->second << endl;
        }
	if (pMessage->m_dwID == COMMAND_Send_Code_CONST)
        {
		string udpcommand;
		udpcommand.assign("sndccf ");
		udpcommand.append(pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST]);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "sending udp payload: %s",udpcommand.c_str());

	        sendto(irtrans_socket, udpcommand.c_str(), udpcommand.length(), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

		sCMD_Result = "OK";
		return;
	}

	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void IRTrans_Ethernet::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void IRTrans_Ethernet::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	string udpcommand;
	udpcommand.assign("sndccf ");
	udpcommand.append(sText);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "sending udp payload: %s",udpcommand.c_str());
	cout << "Parm #9 - Text=" << udpcommand << endl;

       sendto(irtrans_socket, udpcommand.c_str(), udpcommand.length(), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void IRTrans_Ethernet::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
{
	cout << "Need to implement command #194 - Toggle Power" << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c245-b->

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

void IRTrans_Ethernet::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
	cout << "Need to implement command #245 - Learn IR" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
	cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
	cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
}


