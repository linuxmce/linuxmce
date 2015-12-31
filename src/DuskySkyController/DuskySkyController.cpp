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
#include "DuskySkyController.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "sky-control-lib.h"
#include "skycodes.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
DuskySkyController::DuskySkyController(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: DuskySkyController_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
DuskySkyController::DuskySkyController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: DuskySkyController_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
DuskySkyController::~DuskySkyController()
//<-dceag-dest-e->
{
	cout << "Dusky sky destruct called" << endl;
	sky_usb_destroy(m_sky);
	sky_usb_finalise();
	
}

//<-dceag-getconfig-b->
bool DuskySkyController::GetConfig()
{
	if( !DuskySkyController_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	cout << "Init and create sky control" << endl;
	sky_usb_initialise();
	m_sky = sky_usb_create(1, USB_TIMEOUT);

	if(m_sky == NULL)
		cout << "device creation failed" << endl;

	sky_usb_finddevice(m_sky, 0);
	sky_usb_printinfo(m_sky);

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool DuskySkyController::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
DuskySkyController_Command *Create_DuskySkyController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new DuskySkyController(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void DuskySkyController::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	string sPort = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Port_CONST);
	string sType = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Type_CONST);
#ifdef DEBUG
	cout << "destination port [" << sPort << "]" << endl;
	cout << "child command [" << pMessage->m_dwID << "] received" << endl;
	cout << "box type [" << sType << "]" << endl;
#endif

	if (sType == "Sky+HD")
		m_boxtype = skyplushd;
	else if (sType == "Sky+")
		m_boxtype = skyplus;
	else if (sType == "Sky")
		m_boxtype = sky;
	else
		cout << "unknown box type" << sType << endl;

	switch (pMessage->m_dwID)
	{
		// Handle 0-9 together as they are consecutive
		case COMMAND_0_CONST...COMMAND_9_CONST:
			sky_usb_sendcommand(m_sky,
								atoi(sPort.c_str()),
								(pMessage->m_dwID - COMMAND_0_CONST) + m_boxtype);
			sCMD_Result = "OK";
			break;

		//colour buttons - even though they aren't on the template.
		case COMMAND_Red_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), RED + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Green_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), GREEN + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Yellow_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), YELLOW + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Blue_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), BLUE + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Play_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), PLAY + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Record_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), RECORD + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Pause_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), PAUSE + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Stop_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), STOP + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Scan_FwdFast_Fwd_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), FFWD + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Scan_BackRewind_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), RWD + m_boxtype);
			sCMD_Result = "OK";
			break;

		//Up, Down, Left, Right
		case COMMAND_Move_Up_CONST...COMMAND_Move_Right_CONST:
			sky_usb_sendcommand(m_sky,
								atoi(sPort.c_str()),
								(pMessage->m_dwID - (COMMAND_Move_Up_CONST-UP)) + m_boxtype);
			sCMD_Result = "OK";
			break;

		case COMMAND_Send_Generic_EnterGo_CONST:
			sky_usb_sendcommand(m_sky, atoi(sPort.c_str()), SELECT + m_boxtype);
			sCMD_Result = "OK";
			break;

		default:
			sCMD_Result = "UNHANDLED CHILD";
			break;
	}


}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void DuskySkyController::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void DuskySkyController::SomeFunction()
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


