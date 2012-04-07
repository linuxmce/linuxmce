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
#include "DCE_Browser.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
DCE_Browser::DCE_Browser(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: DCE_Browser_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
DCE_Browser::DCE_Browser(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: DCE_Browser_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
DCE_Browser::~DCE_Browser()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool DCE_Browser::GetConfig()
{
	if( !DCE_Browser_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool DCE_Browser::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
DCE_Browser_Command *Create_DCE_Browser(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new DCE_Browser(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void DCE_Browser::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void DCE_Browser::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void DCE_Browser::SomeFunction()
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

//<-dceag-c606-b->

	/** @brief COMMAND: #606 - Reload */
	/** Reload Web Page */

void DCE_Browser::CMD_Reload(string &sCMD_Result,Message *pMessage)
//<-dceag-c606-e->
{
	cout << "Need to implement command #606 - Reload" << endl;
        this->qWebBrowser->bReload();
}

//<-dceag-c1090-b->

	/** @brief COMMAND: #1090 - Goto Url */
	/** goto provided url */
		/** @param #193 URL */
			/** the url to go to */

void DCE_Browser::CMD_Goto_Url(string sURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c1090-e->
{
	cout << "Need to implement command #1090 - Goto Url" << endl;
	cout << "Parm #193 - URL=" << sURL << endl;

          this->qWebBrowser->gotoUrl(QUrl(QString::fromStdString(sURL)));
}


//<-dceag-c1091-b->

	/** @brief COMMAND: #1091 - Navigate Forward */
	/** Navigate Forward */

void DCE_Browser::CMD_Navigate_Forward(string &sCMD_Result,Message *pMessage)
//<-dceag-c1091-e->
{
	cout << "Need to implement command #1091 - Navigate Forward" << endl;
        this->qWebBrowser->goForward();
}

//<-dceag-c1092-b->

	/** @brief COMMAND: #1092 - Navigate Back */
	/** Navigate Back */

void DCE_Browser::CMD_Navigate_Back(string &sCMD_Result,Message *pMessage)
//<-dceag-c1092-e->
{
	cout << "Need to implement command #1092 - Navigate Back" << endl;
        this->qWebBrowser->goBack();
}

//<-dceag-c1093-b->

	/** @brief COMMAND: #1093 - Bookmark Website */
	/** Creates bookmark for user */

void DCE_Browser::CMD_Bookmark_Website(string &sCMD_Result,Message *pMessage)
//<-dceag-c1093-e->
{
	cout << "Need to implement command #1093 - Bookmark Website" << endl;
}

//<-dceag-c1094-b->

	/** @brief COMMAND: #1094 - Increase DPI */
	/** Increase font size */

void DCE_Browser::CMD_Increase_DPI(string &sCMD_Result,Message *pMessage)
//<-dceag-c1094-e->
{
	cout << "Need to implement command #1094 - Increase DPI" << endl;
}

//<-dceag-c1095-b->

	/** @brief COMMAND: #1095 - Reduce DPI */
	/** Decrease Font Size */

void DCE_Browser::CMD_Reduce_DPI(string &sCMD_Result,Message *pMessage)
//<-dceag-c1095-e->
{
	cout << "Need to implement command #1095 - Reduce DPI" << endl;
}


//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Navigate to the next page if there is one. */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void DCE_Browser::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Navigate to the previous web page if there is one */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void DCE_Browser::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Enter was hit */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void DCE_Browser::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void DCE_Browser::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void DCE_Browser::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void DCE_Browser::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void DCE_Browser::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void DCE_Browser::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void DCE_Browser::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void DCE_Browser::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void DCE_Browser::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void DCE_Browser::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void DCE_Browser::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void DCE_Browser::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void DCE_Browser::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void DCE_Browser::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void DCE_Browser::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void DCE_Browser::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
//<-dceag-c409-b->

	/** @brief COMMAND: #409 - Save Bookmark */
	/** Bookmark the current page. */
		/** @param #17 PK_Users */
			/** The user to save this under, or 0 for public */
		/** @param #19 Data */
			/** The picture to save as the thumbnail, in jpg format.  If not specified the plugin will try to grab a frame from the media player */
		/** @param #29 PK_MediaType */
			/** The media type, if not specified it will get it from the stream */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */
		/** @param #163 Description */
			/** The description for this bookmark. */
		/** @param #188 Position */
			/** The position to save as.  For TV, CHAN: or PROG: or SERIES:.  Leave blank and the plugin will grab the current value */
		/** @param #225 Always */
			/** If true, then this is the start position */

void DCE_Browser::CMD_Save_Bookmark(int iPK_Users,char *pData,int iData_Size,int iPK_MediaType,string sPK_EntertainArea,string sDescription,string sPosition,bool bAlways,string &sCMD_Result,Message *pMessage)
//<-dceag-c409-e->
//<-dceag-c1099-b->

	/** @brief COMMAND: #1099 - Load Url */
	/** the url you want loaded.  */

void DCE_Browser::CMD_Load_Url(string &sCMD_Result,Message *pMessage)
//<-dceag-c1099-e->
