/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Roku_LTHDXDXS_Streaming_Pla.h"
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
Roku_LTHDXDXS_Streaming_Pla::Roku_LTHDXDXS_Streaming_Pla(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Roku_LTHDXDXS_Streaming_Pla_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_RokuMutex("roku")
{
  m_RokuMutex.Init(NULL);
  m_bIsPaused=false;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Roku_LTHDXDXS_Streaming_Pla::~Roku_LTHDXDXS_Streaming_Pla()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Roku_LTHDXDXS_Streaming_Pla::GetConfig()
{
	if( !Roku_LTHDXDXS_Streaming_Pla_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

  // Put your code here to initialize the data in this class
  // The configuration parameters DATA_ are now populated

  m_pRokuDevice = new RokuDevice(m_pData->GetIPAddress());
  if (!m_pRokuDevice->GetConfig())
    return false;

  return true;
}

void Roku_LTHDXDXS_Streaming_Pla::CreateChildren()
{
  // Do nothing atm
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Roku_LTHDXDXS_Streaming_Pla::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Roku_LTHDXDXS_Streaming_Pla::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Roku_LTHDXDXS_Streaming_Pla::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Roku_LTHDXDXS_Streaming_Pla::SomeFunction()
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

//<-dceag-c48-b->

	/** @brief COMMAND: #48 - Eject Disk */
	/**  */
		/** @param #151 Slot Number */
			/** For jukeboxes, which slot to eject */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Eject_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c48-e->
{
	cout << "Need to implement command #48 - Eject Disk" << endl;
	cout << "Parm #151 - Slot_Number=" << iSlot_Number << endl;
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Change channels greater (larger numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Change channels lower (smaller numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c86-b->

	/** @brief COMMAND: #86 - Menu (Show Menu) */
	/** Show the on-screen menu */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Menu_Show_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c86-e->
{
	cout << "Need to implement command #86 - Menu (Show Menu)" << endl;
}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
	cout << "Need to implement command #92 - Pause" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Play HTTP/1.1\r\n\r\n");
	m_bIsPaused=true;
}

//<-dceag-c93-b->

	/** @brief COMMAND: #93 - Scan Fwd/Fast Fwd */
	/**  */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Scan_FwdFast_Fwd(string &sCMD_Result,Message *pMessage)
//<-dceag-c93-e->
{
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Fwd HTTP/1.1\r\n\r\n");
	cout << "Need to implement command #93 - Scan Fwd/Fast Fwd" << endl;
}

//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
	cout << "Need to implement command #95 - Stop" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #203 - Eject=" << bEject << endl;
}

//<-dceag-c98-b->

	/** @brief COMMAND: #98 - Slow Scan Back */
	/**  */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Slow_Scan_Back(string &sCMD_Result,Message *pMessage)
//<-dceag-c98-e->
{
	cout << "Need to implement command #98 - Slow Scan Back" << endl;
}

//<-dceag-c99-b->

	/** @brief COMMAND: #99 - Slow Scan Forward */
	/**  */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Slow_Scan_Forward(string &sCMD_Result,Message *pMessage)
//<-dceag-c99-e->
{
	cout << "Need to implement command #99 - Slow Scan Forward" << endl;
}

//<-dceag-c102-b->

	/** @brief COMMAND: #102 - Record */
	/**  */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Record(string &sCMD_Result,Message *pMessage)
//<-dceag-c102-e->
{
	cout << "Need to implement command #102 - Record" << endl;
}

//<-dceag-c125-b->

	/** @brief COMMAND: #125 - Scan Back/Rewind */
	/**  */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Scan_BackRewind(string &sCMD_Result,Message *pMessage)
//<-dceag-c125-e->
{
	cout << "Need to implement command #125 - Scan Back/Rewind" << endl;
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Rev HTTP/1.1\r\n\r\n");

}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
	cout << "Need to implement command #139 - Play" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Play HTTP/1.1\r\n\r\n");
	m_bIsPaused=false;
}

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Selects an item on the on-screen menu */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	cout << "Need to implement command #190 - Enter/Go" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
	m_pRokuDevice->SendToRoku("POST /keypress/Select HTTP/1.1\r\n\r\n");

}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	cout << "Need to implement command #200 - Move Up" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
	m_pRokuDevice->SendToRoku("POST /keypress/Up HTTP/1.1\r\n\r\n");
	

}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	cout << "Need to implement command #201 - Move Down" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
	m_pRokuDevice->SendToRoku("POST /keypress/Down HTTP/1.1\r\n\r\n");


}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	cout << "Need to implement command #202 - Move Left" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
	m_pRokuDevice->SendToRoku("POST /keypress/Left HTTP/1.1\r\n\r\n");


}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	cout << "Need to implement command #203 - Move Right" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
	m_pRokuDevice->SendToRoku("POST /keypress/Right HTTP/1.1\r\n\r\n");


}

//<-dceag-c225-b->

	/** @brief COMMAND: #225 - Display */
	/** Toggles the on-screen menu/display on/off */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Display(string &sCMD_Result,Message *pMessage)
//<-dceag-c225-e->
{
	cout << "Need to implement command #225 - Display" << endl;
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Info HTTP/1.1\r\n\r\n");

}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Exits/goes back from the menu */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	cout << "Need to implement command #240 - Back / Prior Menu" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
	m_pRokuDevice->SendToRoku("POST /keypress/Back HTTP/1.1\r\n\r\n");


}

//<-dceag-c350-b->

	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage)
//<-dceag-c350-e->
{
	cout << "Need to implement command #350 - Process Incoming Data" << endl;
}

//<-dceag-c351-b->

	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Process_IDLE(string &sCMD_Result,Message *pMessage)
//<-dceag-c351-e->
{
	cout << "Need to implement command #351 - Process IDLE" << endl;
}

//<-dceag-c355-b->

	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Process_Initialize(string &sCMD_Result,Message *pMessage)
//<-dceag-c355-e->
{
	cout << "Need to implement command #355 - Process Initialize" << endl;
}

//<-dceag-c356-b->

	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Process_Release(string &sCMD_Result,Message *pMessage)
//<-dceag-c356-e->
{
	cout << "Need to implement command #356 - Process Release" << endl;
}

//<-dceag-c368-b->

	/** @brief COMMAND: #368 - Help */
	/** Bring up a help menu */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Help(string &sCMD_Result,Message *pMessage)
//<-dceag-c368-e->
{
	cout << "Need to implement command #368 - Help" << endl;
}

//<-dceag-c373-b->

	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage)
//<-dceag-c373-e->
{
	cout << "Need to implement command #373 - Private Method Listing" << endl;
}

//<-dceag-c384-b->

	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage)
//<-dceag-c384-e->
{
	cout << "Need to implement command #384 - Process Receive Command For Child" << endl;
}

//<-dceag-c123-b->

	/** @brief COMMAND: #123 - Info */
	/** Info */
		/** @param #9 Text */
			/** nimic */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Info(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c123-e->
{
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Info HTTP/1.1\r\n\r\n");
}
//<-dceag-c521-b->

	/** @brief COMMAND: #521 - Home */
	/** Go Home */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Home(string &sCMD_Result,Message *pMessage)
//<-dceag-c521-e->
{
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Home HTTP/1.1\r\n\r\n");
}
//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
}
//<-dceag-c32-b->

	/** @brief COMMAND: #32 - Update Object Image */
	/** Display an image on the media player */
		/** @param #3 PK_DesignObj */
			/** The object in which to put the bitmap */
		/** @param #14 Type */
			/** 1=bmp, 2=jpg, 3=png */
		/** @param #19 Data */
			/** The contents of the bitmap, like reading from the file into a memory buffer */
		/** @param #23 Disable Aspect Lock */
			/** If 1, the image will be stretched to fit the object */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
}

//<-dceag-c37-b->

	/** @brief COMMAND: #37 - Play Media */
	/** This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** The media that we need to play. */
		/** @param #42 MediaPosition */
			/** The position at which we need to start playing. */
		/** @param #59 MediaURL */
			/** The file to play, or other media id.  The format is specific on the media type and the media player. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Play HTTP/1.1\r\n\r\n");
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
        PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
        m_pRokuDevice->SendToRoku("POST /keypress/Play HTTP/1.1\r\n\r\n");
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
}
//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
		/** @param #220 Report */
			/** If true, report this speed to the user on the OSD */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
  PLUTO_SAFETY_LOCK(rm, m_RokuMutex);
  if (iMediaPlaybackSpeed<0)
    {
      m_bIsPaused=false;
      m_pRokuDevice->SendToRoku("POST /keypress/Rev HTTP/1.1\r\n\r\n");
    }
  else if (iMediaPlaybackSpeed>1000)
    {
      m_bIsPaused=false;
      m_pRokuDevice->SendToRoku("POST /keypress/Fwd HTTP/1.1\r\n\r\n");
    }
  else if (iMediaPlaybackSpeed==0)
    {
      m_bIsPaused=true;
      m_pRokuDevice->SendToRoku("POST /keypress/Play HTTP/1.1\r\n\r\n");
    }
  else if (iMediaPlaybackSpeed==1000)
    {
      if (m_bIsPaused)
	{
	  m_pRokuDevice->SendToRoku("POST /keypress/Play HTTP/1.1\r\n\r\n");
	  m_bIsPaused=false;
	}
    }
}
//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
}

//<-dceag-c249-b->

	/** @brief COMMAND: #249 - Start Streaming */
	/** Like play media, but it means the destination device is not the same as the source */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #42 MediaPosition */
			/** Where to start playing from */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
}

//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
}

//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
}

//<-dceag-c917-b->

	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
}

//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void Roku_LTHDXDXS_Streaming_Pla::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
}

