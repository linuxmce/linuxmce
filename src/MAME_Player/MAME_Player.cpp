/*
     Copyright (C) 2008 LOCALE|concept

     www.localeconcept.com

     Phone: +1 (617) 319-8219
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "MAME_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "WindowUtils/WindowUtils.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Button.h"
#include "Gen_Devices/AllScreens.h"

#include <sstream>
#include <pthread.h>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MAME_Player::MAME_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MAME_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_MAMEMutex("mame")
{

	m_MAMEMutex.Init(NULL);
	m_pAlarmManager = NULL;
	m_pDevice_App_Server = NULL;

}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MAME_Player::~MAME_Player()
//<-dceag-dest-e->
{
	
}

void MAME_Player::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
	m_pDevice_App_Server = NULL;
}

//<-dceag-getconfig-b->
bool MAME_Player::GetConfig()
{
	if( !MAME_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDevice_MAME_Plugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_MAME_PlugIn_CONST);
	if( !m_pDevice_MAME_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I need a MAME plugin to function.");
		return false;
	}

	m_pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( !m_pDevice_App_Server )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I need an App Server to function.");
		return false;
	}

	m_iMAMEWindowId = 0;
	m_iEventSerialNum = 0;
 	m_pDisplay = XOpenDisplay(getenv("DISPLAY"));


	// EVENT_Playback_Completed("",0,false);

	// Attach an alarm for detecting mame run process.

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MAME_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/**
 * UpdateMAMEConfig(sMediaURL) - Update MAME Configuration before MAME
 * Launches. This is mainly used to point rompath inside the config 
 * file to the appropriate place where the actual rom being selected.
 * it is also used for setting things like ctrlr files. It uses a
 * standard MAME configuration file, which has the appropriate 
 * values substituted
 */
bool MAME_Player::UpdateMAMEConfig(string sMediaURL) 
{

	// Vars to replace inside the config file:
	string s_MediaPath = FileUtils::BasePath(sMediaURL);			// for rompath
	string s_HardwareAccelleration = DATA_Get_Hardware_acceleration();	// for mapping Video Driver values.
	string s_VideoDriver;
	const string s_OutputFile = "/root/.mame/mame.ini";
	const string s_OutputDir = "/root/.mame";

	// map hardware accelleration values
	if (s_HardwareAccelleration == "opengl") 
	{
		s_VideoDriver = "opengl";
	} else {
		s_VideoDriver = "soft";	
	}

	string s_ConfigFile = 
		"<UNADORNED0>\r\n"
		"\r\n"
		"#\r\n"
		"# CORE CONFIGURATION OPTIONS\r\n"
		"#\r\n"
		"readconfig                1\r\n"
		"\r\n"
		"#\r\n"
		"# CORE SEARCH PATH OPTIONS\r\n"
		"#\r\n"
		"rompath                   ##ROMPATH##\r\n"
		"samplepath                /home/mamedata/samples\r\n"
		"artpath                   /home/mamedata/artwork\r\n"
		"ctrlrpath                 /home/mamedata/ctrlr\r\n"
		"inipath                   $HOME/.mame;.;ini\r\n"
		"fontpath                  .\r\n"
		"\r\n"
		"#\r\n"
		"# CORE OUTPUT DIRECTORY OPTIONS\r\n"
		"#\r\n"
		"cfg_directory             /home/mamedata/cfg\r\n"
		"nvram_directory           /home/mamedata/nvram\r\n"
		"memcard_directory         /home/mamedata/memcard\r\n"
		"input_directory           /home/mamedata/inp\r\n"
		"state_directory           /home/mamedata/sta\r\n"
		"snapshot_directory        /home/mamedata/shots\r\n" 
		"diff_directory            /home/mamedata/diff\r\n"
		"comment_directory         /home/mamedata/comments\r\n"
		"\r\n"
		"#\r\n"
		"# CORE FILENAME OPTIONS\r\n"
		"#\r\n"
		"cheat_file                cheat.dat\r\n"
		"\r\n"
		"#\r\n"
		"# CORE STATE/PLAYBACK OPTIONS\r\n"
		"#\r\n"
		"state\r\n"
		"autosave                  0\r\n"
		"playback\r\n"
		"record\r\n"                    
		"mngwrite\r\n"                 
		"wavwrite\r\n"                  
		"\r\n"
		"#\r\n"
		"# CORE PERFORMANCE OPTIONS\r\n"
		"#\r\n"
		"autoframeskip             0\r\n"
		"frameskip                 0\r\n"
		"seconds_to_run            0\r\n"
		"throttle                  1\r\n"
		"sleep                     1\r\n"
		"speed                     1.0\r\n"
		"refreshspeed              0\r\n"
		"\r\n"
		"#\r\n"
		"# CORE ROTATION OPTIONS\r\n"
		"#\r\n"
		"rotate                    1\r\n"
		"ror                       0\r\n"
		"rol                       0\r\n"
		"autoror                   0\r\n"
		"autorol                   0\r\n"
		"flipx                     0\r\n"
		"flipy                     0\r\n"
		"\r\n"
		"#\r\n"
		"# CORE ARTWORK OPTIONS\r\n"
		"#\r\n"
		"artwork_crop              1\r\n"
		"use_backdrops             1\r\n"
		"use_overlays              1\r\n"
		"use_bezels                1\r\n"
		"\r\n"
		"#\r\n"
		"# CORE SCREEN OPTIONS\r\n"
		"#\r\n"
		"brightness                1.0\r\n"
		"contrast                  1.0\r\n"
		"gamma                     1.0\r\n"
		"pause_brightness          0.65\r\n"
		"\r\n"
		"\r\n#"
		"# CORE VECTOR OPTIONS\r\n"
		"#\r\n"
		"antialias                 1\r\n"
		"beam                      1.0\r\n"
		"flicker                   0\r\n"
		"\r\n"
		"#\r\n"
		"# CORE SOUND OPTIONS\r\n"
		"#\r\n"
		"sound                     1\r\n"
		"samplerate                48000\r\n"
		"samples                   1\r\n"
		"volume                    0\r\n"
		"\r\n"
		"#\r\n"
		"# CORE INPUT OPTIONS\r\n"
		"#\r\n"
		"ctrlr\r\n"                     
		"mouse                     1\r\n"
		"joystick                  1\r\n" 
		"lightgun                  0\r\n"
		"multikeyboard             0\r\n"
		"multimouse                0\r\n"
		"steadykey                 0\r\n"
		"offscreen_reload          0\r\n"
		"joystick_map              auto\r\n"
		"joystick_deadzone         0.3\r\n"
		"joystick_saturation       0.85\r\n"
		"\r\n"
		"#\r\n"
		"# CORE INPUT AUTOMATIC ENABLE OPTIONS\r\n"
		"#\r\n"
		"paddle_device             keyboard\r\n"
		"adstick_device            keyboard\r\n"
		"pedal_device              keyboard\r\n"
		"dial_device               keyboard\r\n"
		"trackball_device          mouse\r\n"
		"lightgun_device           keyboard\r\n"
		"positional_device         keyboard\r\n"
		"mouse_device              mouse\r\n"
		"\r\n"
		"#\r\n"
		"# CORE DEBUGGING OPTIONS\r\n"
		"#\r\n"
		"log                       0\r\n"
		"verbose                   0\r\n"
		"update_in_pause           0\r\n"
		"\r\n"
		"#\r\n"
		"# CORE MISC OPTIONS\r\n"
		"#\r\n"
		"bios                      default\r\n"
		"cheat                     0\r\n"
		"skip_gameinfo             1\r\n"
		"\r\n"
		"#\r\n"
		"# DEBUGGING OPTIONS\r\n"
		"#\r\n"
		"oslog                     0\r\n"
		"\r\n"
		"#\r\n"
		"# PERFORMANCE OPTIONS\r\n"
		"#\r\n"
		"multithreading            0\r\n"
		"sdlvideofps               0\r\n"
		"\r\n"
		"#\r\n"
		"# VIDEO OPTIONS\r\n"
		"#\r\n"
		"video                     ##VIDEO##\r\n" 
		"numscreens                1\r\n"
		"window                    1\r\n" 
		"keepaspect                1\r\n"
		"unevenstretch             1\r\n"
		"effect                    none\r\n"
		"centerh                   1\r\n"
		"centerv                   1\r\n"
		"waitvsync                 0\r\n"
		"yuvmode                   none\r\n"
		"\r\n"
		"#\r\n"
		"# OpenGL-SPECIFIC OPTIONS\r\n"
		"#\r\n"
		"filter                    1\r\n"
		"prescale                  1\r\n"
		"gl_forcepow2texture       0\r\n"
		"gl_notexturerect          0\r\n"
		"gl_vbo                    1\r\n"
		"gl_pbo                    1\r\n"
		"gl_glsl                   0\r\n"
		"gl_glsl_filter            1\r\n"
		"glsl_shader_mame0         none\r\n"
		"glsl_shader_mame1         none\r\n"
		"glsl_shader_mame2         none\r\n"
		"glsl_shader_mame3         none\r\n"
		"glsl_shader_mame4         none\r\n"
		"glsl_shader_mame5         none\r\n"
		"glsl_shader_mame6         none\r\n"
		"glsl_shader_mame7         none\r\n"
		"glsl_shader_mame8         none\r\n"
		"glsl_shader_mame9         none\r\n"
		"glsl_shader_screen0       none\r\n"
		"glsl_shader_screen1       none\r\n"
		"glsl_shader_screen2       none\r\n"
		"glsl_shader_screen3       none\r\n"
		"glsl_shader_screen4       none\r\n"
		"glsl_shader_screen5       none\r\n"
		"glsl_shader_screen6       none\r\n"
		"glsl_shader_screen7       none\r\n"
		"glsl_shader_screen8       none\r\n"
		"glsl_shader_screen9       none\r\n"
		"gl_glsl_vid_attr          1\r\n"
		"\r\n"
		"#\r\n"
		"# PER-WINDOW VIDEO OPTIONS\r\n"
		"#\r\n"
		"screen                    auto\r\n"
		"aspect                    auto\r\n"
		"resolution                auto\r\n"
		"view                      auto\r\n"
		"screen0                   auto\r\n"
		"aspect0                   auto\r\n"
		"resolution0               auto\r\n"
		"view0                     auto\r\n"
		"screen1                   auto\r\n"
		"aspect1                   auto\r\n"
		"resolution1               auto\r\n"
		"view1                     auto\r\n"
		"screen2                   auto\r\n"
		"aspect2                   auto\r\n"
		"resolution2               auto\r\n"
		"view2                     auto\r\n"
		"screen3                   auto\r\n"
		"aspect3                   auto\r\n"
		"resolution3               auto\r\n"
		"view3                     auto\r\n"
		"\r\n"
		"#\r\n"
		"# FULL SCREEN OPTIONS\r\n"
		"#\r\n"
		"switchres                 0\r\n"
		"useallheads               0\r\n"
		"\r\n"
		"#\r\n"
		"# SOUND OPTIONS\r\n"
		"#\r\n"
		"audio_latency             3\r\n"
		"\r\n"
		"#\r\n"
		"# SDL KEYBOARD MAPPING\r\n"
		"#\r\n"
		"keymap                    0\r\n"
		"keymap_file               keymap.dat\r\n"
		"\r\n"
		"#\r\n"
		"# SDL JOYSTICK MAPPING\r\n"
		"#\r\n"
		"remapjoys                 0\r\n"
		"remapjoyfile              joymap.dat\r\n"
		"sixaxis                   0\r\n";
	
	// replace, and spit out the config.
	
	s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##ROMPATH##",s_MediaPath);
	s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##VIDEO##",s_VideoDriver);
	
	if (!FileUtils::DirExists(s_OutputDir)) 
	{
		// hopefully try to create it.
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%s didn't exist, attempting to create.",s_OutputDir.c_str());
		FileUtils::MakeDir(s_OutputDir);
		if (!FileUtils::DirExists(s_OutputDir)) 
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't create %s, bailing...",s_OutputDir.c_str());
			return false;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created %s Successfully.",s_OutputDir.c_str());		
		}
	}

	if (!FileUtils::WriteTextFile(s_OutputFile,s_ConfigFile)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't write %s",s_OutputFile.c_str());		
		return false;	
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Wrote %s",s_OutputFile.c_str());
		return true;	
	}
}

bool MAME_Player::LaunchMAME(string sMediaURL) 
{

	size_t iROMNameSize = FileUtils::FilenameWithoutPath(sMediaURL).size();
	m_sROMName = FileUtils::FilenameWithoutPath(sMediaURL).substr(0,iROMNameSize-4);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ROM is: %s",m_sROMName.c_str());
	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		string sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " 1 " 
			TOSTRING(COMMAND_Application_Exited_CONST) " " 
			TOSTRING(COMMANDPARAMETER_Exit_Code_CONST) " ";

		if (!UpdateMAMEConfig(sMediaURL)) 
		{
			return false;
		} // Make sure MAME's configuration is correct. 

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"mame", "mame", sMediaURL,
			sMessage + "1",sMessage + "0",false,false,true,false);
		if( SendCommand(CMD_Spawn_Application) ) {
			m_bMAMEIsRunning = true;

	Sleep(5000); // FIXME !!!!

	if (!WindowUtils::FindWindowMatching(m_pDisplay, "MAME", m_iMAMEWindowId))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't find MAME Window");	
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MAME window found: Window ID %d",m_iMAMEWindowId);
		
	}

		 	return true; }
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAME_Player::LaunchMAME - failed to launch");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAME_Player::LaunchMAME - no app server");
	return false;
	
}

bool MAME_Player::StopMAME() {

        DeviceData_Base *pDevice_App_Server = NULL;
        string sResponse;
        if(!m_bRouterReloading)
        {
                pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
                if( pDevice_App_Server )
                {
                        DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
                                "mame", false);
			m_iMAMEWindowId = 0;
                        return SendCommand(CMD_Kill_Application,&sResponse);  // Get return confirmation so we know it's gone before we continue
                }
        }

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MAME_Player::StopMAME %p %s", pDevice_App_Server,sResponse.c_str());
        return false;

}

void MAME_Player::CheckMAME() 
{

	// reset the alarm thread
	//m_pAlarmManager->CancelAlarmByType(CHECK_MAME);
	//m_pAlarmManager->AddRelativeAlarm(5,this,CHECK_MAME,NULL);

}

void MAME_Player::AlarmCallback(int id, void* param) 
{

}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void MAME_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void MAME_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void MAME_Player::SomeFunction()
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

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void MAME_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	cout << "Need to implement command #28 - Simulate Keypress" << endl;
	cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #50 - Name=" << sName << endl;

	switch(atoi(sPK_Button.c_str()))
	{
		case BUTTON_Up_Arrow_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Up,m_iEventSerialNum++);
		break;		
	case BUTTON_Down_Arrow_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Down,m_iEventSerialNum++);
		break;	
	case BUTTON_Left_Arrow_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Left,m_iEventSerialNum++);
		break;	
	case BUTTON_Right_Arrow_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Right,m_iEventSerialNum++);
		break;	
	case BUTTON_Scroll_Up_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Up,m_iEventSerialNum++);
		break;
	case BUTTON_Scroll_Down_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Down,m_iEventSerialNum++);
		break;
	case BUTTON_Enter_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Return,m_iEventSerialNum++);
		break;	
	case BUTTON_0_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_0,m_iEventSerialNum++);
		break;	
	case BUTTON_1_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);
		break;	
	case BUTTON_2_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++);
		break;	
	case BUTTON_3_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_3,m_iEventSerialNum++);
		break;	
	case BUTTON_4_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_4,m_iEventSerialNum++);
		break;	
	case BUTTON_5_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_5,m_iEventSerialNum++);
		break;	
	case BUTTON_6_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_6,m_iEventSerialNum++);
		break;	
	case BUTTON_7_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_7,m_iEventSerialNum++);
		break;	
	case BUTTON_8_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_8,m_iEventSerialNum++);
		break;	
	case BUTTON_9_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_9,m_iEventSerialNum++);
		break;	
	case BUTTON_Back_CONST:
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Escape,m_iEventSerialNum++);
		break;	
	}

}

//<-dceag-c29-b->

	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simlate a mouse click at a certain position on the screen */
		/** @param #11 Position X */
			/** position X */
		/** @param #12 Position Y */
			/** position Y */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
	cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
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

void MAME_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
	cout << "Need to implement command #32 - Update Object Image" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #14 - Type=" << sType << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
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

void MAME_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

	LaunchMAME(sMediaURL);


}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void MAME_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

	StopMAME();

}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void MAME_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	cout << "Need to implement command #39 - Pause Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay, m_iMAMEWindowId,XK_p,m_iEventSerialNum++);	

}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void MAME_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	cout << "Need to implement command #40 - Restart Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay, m_iMAMEWindowId,XK_p,m_iEventSerialNum++);	

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

void MAME_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
	cout << "Parm #220 - Report=" << bReport << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay, m_iMAMEWindowId,XK_p,m_iEventSerialNum++);	// funny.


}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void MAME_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
	cout << "Need to implement command #42 - Jump to Position in Stream" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;



}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MAME_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
	cout << "Need to implement command #81 - Navigate Next" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MAME_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
	cout << "Need to implement command #82 - Navigate Prev" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get's the current video frame from the media player. */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

void MAME_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
	cout << "Need to implement command #84 - Get Video Frame" << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #20 - Format=" << sFormat << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #60 - Width=" << iWidth << endl;
	cout << "Parm #61 - Height=" << iHeight << endl;

	PLUTO_SAFETY_LOCK(mm,m_MAMEMutex);
	string snapPath = "/home/mamedata/shots/" + m_sROMName + "/";
	FileUtils::DelFile("/home/mamedata/shots/"+m_sROMName + "/0000.png");
	FileUtils::DelFile("/home/mamedata/shots/"+m_sROMName + "/0000.jpg");

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F12,m_iEventSerialNum++); // hehe, hack++
	
	Sleep(50);
	string s_OutputString = "convert -sample 800x800 /home/mamedata/shots/" + m_sROMName + "/0000.png /home/mamedata/shots/" + m_sROMName + "/0000.jpg";	
	system(s_OutputString.c_str());
	size_t size;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MAME_Player::CMD_Get_Video_Frame got %d",size);
	*pData = FileUtils::ReadFileIntoBuffer("/home/mamedata/shots/"+m_sROMName+"/0000.png",size);
	*iData_Size = size;

	FileUtils::DelFile("/home/mamedata/shots/"+m_sROMName + "/0000.png");
	FileUtils::DelFile("/home/mamedata/shots/"+m_sROMName + "/0000.jpg");

	return;	


}

//<-dceag-c87-b->

	/** @brief COMMAND: #87 - Goto Media Menu */
	/** Goto to the current media Root Menu. */
		/** @param #41 StreamID */
			/** The stream ID */
		/** @param #64 MenuType */
			/** The type of menu that the user want to jump to.
(For DVD handlers usually this applies)
0 - Root menu 
1 - Title menu
2 - Media menu */

void MAME_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
	cout << "Need to implement command #87 - Goto Media Menu" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #64 - MenuType=" << iMenuType << endl;

	switch(iMenuType) {
		case SHOW_GAME_VIEW:
			EVENT_Menu_Onscreen(iStreamID,true);
			break;
		case SHOW_REMOTE:
			EVENT_Menu_Onscreen(iStreamID,false);
			break;
		default:
			EVENT_Menu_Onscreen(iStreamID,false);
			break;
		}

}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
	cout << "Need to implement command #92 - Pause" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_p,m_iEventSerialNum++);

}

//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** Stop the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void MAME_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
	cout << "Need to implement command #95 - Stop" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #203 - Eject=" << bEject << endl;
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void MAME_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
	cout << "Need to implement command #139 - Play" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	cout << "Need to implement command #190 - Enter/Go" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Return,m_iEventSerialNum++);

}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	cout << "Need to implement command #200 - Move Up" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Up,m_iEventSerialNum++);

}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	cout << "Need to implement command #201 - Move Down" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Down,m_iEventSerialNum++);

}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	cout << "Need to implement command #202 - Move Left" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Left,m_iEventSerialNum++);

}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	cout << "Need to implement command #203 - Move Right" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Right,m_iEventSerialNum++);

}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void MAME_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	cout << "Need to implement command #204 - 0" << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_0,m_iEventSerialNum++);

}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void MAME_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	cout << "Need to implement command #205 - 1" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);


}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void MAME_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	cout << "Need to implement command #206 - 2" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++);
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void MAME_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	cout << "Need to implement command #207 - 3" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_3,m_iEventSerialNum++);
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void MAME_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	cout << "Need to implement command #208 - 4" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_4,m_iEventSerialNum++);

}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void MAME_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	cout << "Need to implement command #209 - 5" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_5,m_iEventSerialNum++);
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void MAME_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	cout << "Need to implement command #210 - 6" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_6,m_iEventSerialNum++);
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void MAME_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	cout << "Need to implement command #211 - 7" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_7,m_iEventSerialNum++);
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void MAME_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	cout << "Need to implement command #212 - 8" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_8,m_iEventSerialNum++);
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void MAME_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	cout << "Need to implement command #213 - 9" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_9,m_iEventSerialNum++);
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	cout << "Need to implement command #240 - Back / Prior Menu" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_0,m_iEventSerialNum++);
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

void MAME_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	cout << "Need to implement command #249 - Start Streaming" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;
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

void MAME_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void MAME_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	cout << "Need to implement command #412 - Set Media Position" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MAME_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
	cout << "Need to implement command #548 - Menu" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c812-b->

	/** @brief COMMAND: #812 - Application Exited */
	/** Notify us that Myth Player exited */
		/** @param #227 PID */
			/** Process ID to be passed to the ApplicationExited function */
		/** @param #228 Exit Code */
			/** Exit Code to be passed to the ApplicationExited function */

void MAME_Player::CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage)
//<-dceag-c812-e->
{
	cout << "Need to implement command #812 - Application Exited" << endl;
	cout << "Parm #227 - PID=" << iPID << endl;
	cout << "Parm #228 - Exit_Code=" << iExit_Code << endl;

#ifndef WIN32
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Process exited %d %d", iPID, iExit_Code);

	void *data;

//	if ( applicationName.compare(MYTH_WINDOW_NAME) == 0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Send go back to the caller!");
		DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,0,0,"",false);
		SendCommand(CMD_MH_Stop_Media_Cat);
	}

#endif

}

//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void MAME_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
	cout << "Need to implement command #916 - Set Aspect Ratio" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #260 - Aspect_Ratio=" << sAspect_Ratio << endl;
}

//<-dceag-c917-b->

	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

void MAME_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
	cout << "Need to implement command #917 - Set Zoom" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #261 - Zoom_Level=" << sZoom_Level << endl;
}

//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void MAME_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	cout << "Need to implement command #920 - Set Media ID" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c942-b->

	/** @brief COMMAND: #942 - Get Ripping Status */
	/** Tell Game to Start 1 Player */
		/** @param #199 Status */
			/** Ripping status */

void MAME_Player::CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage)
//<-dceag-c942-e->
{
	cout << "Need to implement command #942 - Get Ripping Status" << endl;
	cout << "Parm #199 - Status=" << sStatus << endl;
}

//<-dceag-c943-b->

	/** @brief COMMAND: #943 - Game 1P Start */
	/** Tell Game to Start 2 Players */

void MAME_Player::CMD_Game_1P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c943-e->
{
	cout << "Need to implement command #943 - Game 1P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);
}

//<-dceag-c944-b->

	/** @brief COMMAND: #944 - Game 2P Start */
	/** Tell Game to Start 3 Players */

void MAME_Player::CMD_Game_2P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c944-e->
{
	cout << "Need to implement command #944 - Game 2P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++);
}

//<-dceag-c945-b->

	/** @brief COMMAND: #945 - Game 3P Start */
	/** Tell Game to Start 4 Players */

void MAME_Player::CMD_Game_3P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c945-e->
{
	cout << "Need to implement command #945 - Game 3P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_3,m_iEventSerialNum++);
}

//<-dceag-c946-b->

	/** @brief COMMAND: #946 - Game 4P Start */
	/** Tell Game to insert a coin. */

void MAME_Player::CMD_Game_4P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c946-e->
{
	cout << "Need to implement command #946 - Game 4P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_4,m_iEventSerialNum++);
}

//<-dceag-c947-b->

	/** @brief COMMAND: #947 - Game Insert Coin */
	/** Tell game to go into Service Mode */

void MAME_Player::CMD_Game_Insert_Coin(string &sCMD_Result,Message *pMessage)
//<-dceag-c947-e->
{
	cout << "Need to implement command #947 - Game Insert Coin" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_5,m_iEventSerialNum++);
}

//<-dceag-c948-b->

	/** @brief COMMAND: #948 - Game Service */
	/** Tell Game to Press Start key */

void MAME_Player::CMD_Game_Service(string &sCMD_Result,Message *pMessage)
//<-dceag-c948-e->
{
	cout << "Need to implement command #948 - Game Service" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F2,m_iEventSerialNum++);
}

//<-dceag-c949-b->

	/** @brief COMMAND: #949 - Game Start */
	/** Tell Game to Press Select key */

void MAME_Player::CMD_Game_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c949-e->
{
	cout << "Need to implement command #949 - Game Start" << endl;
}

//<-dceag-c950-b->

	/** @brief COMMAND: #950 - Game Select */
	/** Tell Game to Press Option key */

void MAME_Player::CMD_Game_Select(string &sCMD_Result,Message *pMessage)
//<-dceag-c950-e->
{
	cout << "Need to implement command #950 - Game Select" << endl;
}

//<-dceag-c951-b->

	/** @brief COMMAND: #951 - Game Option */
	/** Tell Game to Reset */

void MAME_Player::CMD_Game_Option(string &sCMD_Result,Message *pMessage)
//<-dceag-c951-e->
{
	cout << "Need to implement command #951 - Game Option" << endl;
}


