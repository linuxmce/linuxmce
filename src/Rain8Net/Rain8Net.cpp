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
#include "Rain8Net.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "PlutoUtils/LinuxSerialUSB.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Rain8Net::Rain8Net(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Rain8Net_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Rain8Net::Rain8Net(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Rain8Net_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Rain8Net::~Rain8Net()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Rain8Net::GetConfig()
{
	if( !Rain8Net_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	string port = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	if (rain8.init(port.c_str()) != 0) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot initialize serial port %s",port.c_str());
		return false;
	}

	if (rain8.comCheck() != 0) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not detect Rain8Net device, aborting");
		return false;

	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Connected to Rain8Net device on port %s",port.c_str());

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Rain8Net::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Rain8Net_Command *Create_Rain8Net(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Rain8Net(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Rain8Net::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	// LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Received command for child");
	string portChannel = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	int valve = atoi(portChannel.c_str());
	if ((valve > 0) && (valve < 9) ) {
		sCMD_Result = "OK";
		unsigned char status;
		switch (pMessage->m_dwID) {
			case COMMAND_Generic_On_CONST:
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ON received for zone %d",valve);
				if (rain8.zoneOn(1,valve) != 0) {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ERROR: Zone %d did not respond",valve);
					sCMD_Result = "ERROR";

				}
				if (rain8.getStatus(1,status) == 0) {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Zone status: 8:%d 7:%d 6:%d 5:%d 4:%d 3:%d 2:%d 1:%d", (status & 128)?1:0, (status & 64)?1:0, (status &32)?1:0,(status&16)?1:0,(status&8)?1:0,(status&4)?1:0,(status&2)?1:0,(status&1)?1:0);
				} else {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ERROR: could not fetch zone status");
				}
				break;	
				
				;;
			case COMMAND_Generic_Off_CONST:
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OFF received for zone %d",valve);
				if (rain8.zoneOff(1,valve) != 0) {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ERROR: Zone %d did not respond",valve);
					sCMD_Result = "ERROR";

				}
				if (rain8.getStatus(1,status) == 0) {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Zone status: 8:%d 7:%d 6:%d 5:%d 4:%d 3:%d 2:%d 1:%d", (status & 128)?1:0, (status & 64)?1:0, (status &32)?1:0,(status&16)?1:0,(status&8)?1:0,(status&4)?1:0,(status&2)?1:0,(status&1)?1:0);
				} else {
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ERROR: could not fetch zone status");
				}
				break;
				;;
			default:
				sCMD_Result = "INVALID COMMAND";
				break;
				;;
		}

	} else {
		sCMD_Result = "BAD CHANNEL";
	}
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Rain8Net::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Rain8Net::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


