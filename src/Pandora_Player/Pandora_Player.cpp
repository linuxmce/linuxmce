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
#include "Pandora_Player.h"
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
Pandora_Player::Pandora_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Pandora_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Pandora_Player::Pandora_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Pandora_Player_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Pandora_Player::~Pandora_Player()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Pandora_Player::GetConfig()
{
	if( !Pandora_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Pandora_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Pandora_Player_Command *Create_Pandora_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Pandora_Player(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Pandora_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Pandora_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Pandora_Player::SomeFunction()
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

//<-dceag-c37-b->

	/** @brief COMMAND: #37 - Play Media */
	/** Start playing a stream */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** The media that we need to play. */
		/** @param #42 MediaPosition */
			/** The position at which we need to start playing. */
		/** @param #59 MediaURL */
			/** The file to play, or other media id.  The format is specific on the media type and the media player. */

void Pandora_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** Restart media after a pause. */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void Pandora_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	cout << "Need to implement command #40 - Restart Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Skip Forward to next song. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Pandora_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** In this case, Jump forward to next song. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Pandora_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause a stream */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Pandora_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
	cout << "Need to implement command #92 - Pause" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the stream */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Pandora_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
	cout << "Need to implement command #139 - Play" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c651-b->

	/** @brief COMMAND: #651 - Thumbs Down */
	/** Signify currently playing song has thumbs down. */
		/** @param #41 StreamID */
			/** For Smart Media Players, the currently Playing Stream ID */

void Pandora_Player::CMD_Thumbs_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c651-e->
{
	cout << "Need to implement command #651 - Thumbs Down" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c652-b->

	/** @brief COMMAND: #652 - Thumbs Up */
	/** Signify currently playing song in stream has thumbs up. */
		/** @param #41 StreamID */
			/** For smart media players, the currently playing media stream. */

void Pandora_Player::CMD_Thumbs_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c652-e->
{
	cout << "Need to implement command #652 - Thumbs Up" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1076-b->

	/** @brief COMMAND: #1076 - Add Station */
	/** Add Radio Station to this User's list of stations. */
		/** @param #10 ID */
			/** The Returned Station ID */
		/** @param #14 Type */
			/** Station Type. For Pandora this is default or genre */
		/** @param #41 StreamID */
			/** The Stream ID to modify. */
		/** @param #50 Name */
			/** The name of the Station to add. For genre this is a Genre ID. For Default, This is used as a Search term. */

void Pandora_Player::CMD_Add_Station(string sType,int iStreamID,string sName,string *sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1076-e->
{
	cout << "Need to implement command #1076 - Add Station" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #14 - Type=" << sType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c1077-b->

	/** @brief COMMAND: #1077 - Remove Station */
	/** Remove Radio Station from this User's List of stations. */
		/** @param #10 ID */
			/** Station ID to remove. */
		/** @param #41 StreamID */
			/** The Stream ID from which to remove */

void Pandora_Player::CMD_Remove_Station(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1077-e->
{
	cout << "Need to implement command #1077 - Remove Station" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1078-b->

	/** @brief COMMAND: #1078 - Rename Station */
	/** Rename Radio Station in a user's list of radio stations. */
		/** @param #10 ID */
			/** The Radio Station ID to rename */
		/** @param #41 StreamID */
			/** The Stream ID */
		/** @param #50 Name */
			/** The New Name of the Radio Station */

void Pandora_Player::CMD_Rename_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c1078-e->
{
	cout << "Need to implement command #1078 - Rename Station" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c1079-b->

	/** @brief COMMAND: #1079 - Explain Song */
	/** Ask the radio station for an explanation of why this song is playing. */
		/** @param #41 StreamID */
			/** The Stream ID of the currently playing song. */

void Pandora_Player::CMD_Explain_Song(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1079-e->
{
	cout << "Need to implement command #1079 - Explain Song" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1080-b->

	/** @brief COMMAND: #1080 - Add Music to Station */
	/** Add Music to an already existing Radio Station */
		/** @param #10 ID */
			/** The Radio Station ID to add terms to */
		/** @param #41 StreamID */
			/** The Stream ID */
		/** @param #50 Name */
			/** Search terms such as an artist or song title. */

void Pandora_Player::CMD_Add_Music_to_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c1080-e->
{
	cout << "Need to implement command #1080 - Add Music to Station" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c1081-b->

	/** @brief COMMAND: #1081 - Add Station to QuickMix */
	/** Add A Radio Station to the Currently Playing QuickMix */
		/** @param #10 ID */
			/** The Station ID to add */
		/** @param #41 StreamID */
			/** The StreamID with the quickmix to modify */

void Pandora_Player::CMD_Add_Station_to_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1081-e->
{
	cout << "Need to implement command #1081 - Add Station to QuickMix" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1082-b->

	/** @brief COMMAND: #1082 - Remove Station from QuickMix */
	/** Remove Radio Station from the currently playing QuickMix */
		/** @param #10 ID */
			/** The Station ID */
		/** @param #41 StreamID */
			/** The Stream ID */

void Pandora_Player::CMD_Remove_Station_from_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1082-e->
{
	cout << "Need to implement command #1082 - Remove Station from QuickMix" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}


