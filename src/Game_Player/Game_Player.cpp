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
#include "Game_Player.h"
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
#include <math.h>

/* VideoFrameGeometry Stuff */
VideoFrameGeometry::VideoFrameGeometry(int iWidth, int iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

VideoFrameGeometry::~VideoFrameGeometry()
{
	// Does nothing.
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Game_Player::Game_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Game_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_GameMutex("game_player")
{

	m_GameMutex.Init(NULL);
	m_pAlarmManager = NULL;
	m_pDevice_App_Server = NULL;
	m_iPK_MediaType = 0;
	m_iStreamID = 0;
	m_bLoadSavedGame = 0;
	m_sMachineType = "";
	m_sPeripheralType = "";
	m_iModifier = 0;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Game_Player::~Game_Player()
//<-dceag-dest-e->
{
	{
		PLUTO_SAFETY_LOCK(gm, m_GameMutex);
		// Delete the VideoFrameGeometry map
		for (map<int, VideoFrameGeometry *>::iterator it=m_mapVideoFrameGeometry.begin(); 
		                                              it!=m_mapVideoFrameGeometry.end();
							      it++)
		{
			delete (*it).second;
		}
	}
}

void Game_Player::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
	m_pDevice_App_Server = NULL;
	m_iPK_MediaType = 0;
}

//<-dceag-getconfig-b->
bool Game_Player::GetConfig()
{
	if( !Game_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	m_pAlarmManager = new AlarmManager();
        m_pAlarmManager->Start(2);      //4 = number of worker threads

	m_pDevice_Game_Plugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_Game_PlugIn_CONST);
	if( !m_pDevice_Game_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I need a Game plugin to function.");
		return false;
	}

	m_pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( !m_pDevice_App_Server )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I need an App Server to function.");
		return false;
	}

	m_pDevice_Joystick = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Joysticks_CONST);
	if ( !m_pDevice_Joystick )
	  {
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"No Special Joysticks Found");
	  }
	else
	  {
	      if (!m_pDevice_Joystick->m_bDisabled)
	      {
		m_sJoystick_Configuration = m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(m_pDevice_Joystick->m_dwPK_Device,DEVICEDATA_Configuration_CONST);
	      }
	      else
	      {
		m_sJoystick_Configuration = "";
	      }
	  }

	m_iMAMEWindowId = 0;
	m_iEventSerialNum = 0;
 	m_pDisplay = XOpenDisplay(getenv("DISPLAY"));


	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Game_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/**
 * UpdateMESSConfig() - similar to the function just below, except.
 * tailored for Stella's configuration file.
 */
bool Game_Player::UpdateMESSConfig(string sMediaURL)
{
	// Vars to replace inside the config file:
	string s_MediaPath = FileUtils::BasePath(sMediaURL);			// for rompath
	string s_HardwareAccelleration = DATA_Get_Hardware_acceleration();	// for mapping Video Driver values.
	string s_VideoDriver;
	const string s_OutputFile = "/root/.mess/mess.ini";
	const string s_OutputDir = "/root/.mess";

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
		"hashpath		   /home/mamedata/hash\r\n"
		"samplepath                /home/mamedata/samples\r\n"
		"artpath                   /home/mamedata/artwork\r\n"
		"ctrlrpath                 /home/mamedata/ctrlr\r\n"
		"inipath                   $HOME/.mess;.;ini\r\n"
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
		"aviwrite                  \r\n"
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
		"beam                      2.0\r\n"
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
		"ctrlr			   ##CTRLR##\r\n"                     
		"mouse                     0\r\n"
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
		"paddle_device             mouse\r\n"
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
		"multithreading            1\r\n"
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
	
	if (m_sJoystick_Configuration == "")
	{
	  // Do nothing.
	}
	else
	{
	  // Custom Controller Information found, specify and update controller file.
	  s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##CTRLR##","GamePlayer");
	  UpdateControllerFile();
	  ParseControllerFile();
	}

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

xmlXPathObjectPtr Game_Player::getnodeset (xmlDocPtr doc, xmlChar *xpath)
{

        xmlXPathContextPtr context;
        xmlXPathObjectPtr result;

        context = xmlXPathNewContext(doc);
        if (context == NULL) {
                return NULL;
        }
        result = xmlXPathEvalExpression(xpath, context);
        xmlXPathFreeContext(context);
        if (result == NULL) {
                return NULL;
        }
        if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
                xmlXPathFreeObject(result);
                return NULL;
        }
        return result;
}


/**
 * ParseControllerEntry(sEntryName, sKeyCode) - parses a key code, and shoves it into the
 * Controller map for use elsewhere in this code.
 */
void Game_Player::ParseControllerEntry(string sEntryName, string sKeyCode)
{
	// for now, this is super ($#@%@# simple. It 
}

/**
 * ParseControllerFile() - If a controller file is updated, parse it and grab the relevant 
 * controller information into a Controller class so we can map keys appropriately for Orbiter.
 */
void Game_Player::ParseControllerFile()
{

        xmlDocPtr doc;
        doc = xmlParseFile("/home/mamedata/ctrlr/GamePlayer.cfg");

        if (doc == NULL ) {
                return;
        }

	

	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *resultText;

	if (doc == NULL)
		return;
	
	// now evaluate specific xpath expressions to grab needed mappings
	// for orbiter commands. This is hardcoded, yes.. we can do it better
	// but that comes later. 
	
	// Grab/Parse P1 Start Key if there.
	result = getnodeset(doc,(xmlChar *)"//mameconfig/system[@name='default']/input/port[@type='START1']/*");
	if (result)
	{
		nodeset = result->nodesetval;
		resultText = xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
		string resultString = (char *)resultText;
		ParseControllerEntry("P1",resultString);
		xmlFree(resultText);
		xmlXPathFreeObject(result);
	}
	
        // Grab/Parse P2 Start Key if there.
        result = getnodeset(doc,(xmlChar *)"//mameconfig/system[@name='default']/input/port[@type='START2']/*");
        if (result)
        {
                nodeset = result->nodesetval;
                resultText = xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
                string resultString = (char *)resultText;
                ParseControllerEntry("P2",resultString);
                xmlFree(resultText);
                xmlXPathFreeObject(result);
        }

        // Grab/Parse Coin 1 key if there.
        result = getnodeset(doc,(xmlChar *)"//mameconfig/system[@name='default']/input/port[@type='COIN1']/*");
        if (result)
        {
                nodeset = result->nodesetval;
                resultText = xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
                string resultString = (char *)resultText;
                ParseControllerEntry("C1",resultString);
                xmlFree(resultText);
                xmlXPathFreeObject(result);
        }

        // Grab/Parse Coin 2 Key if there.
        result = getnodeset(doc,(xmlChar *)"//mameconfig/system[@name='default']/input/port[@type='COIN2']/*");
        if (result)
        {
                nodeset = result->nodesetval;
                resultText = xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
                string resultString = (char *)resultText;
                ParseControllerEntry("C2",resultString);
                xmlFree(resultText);
                xmlXPathFreeObject(result);
        }

	// Done.

	xmlFreeDoc(doc);
	return;

}

/**
 * UpdateControllerFile() - If a joystick device is found, with configuration devicedata
 * then this is run to update the controller file. It does no sanity checking.
 */
bool Game_Player::UpdateControllerFile()
{

	string s_OutputDir = "/home/mamedata/ctrlr";
	string s_OutputFile = "/home/mamedata/ctrlr/GamePlayer.cfg";

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

	if (!FileUtils::WriteTextFile(s_OutputFile,m_sJoystick_Configuration)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't write %s",s_OutputFile.c_str());		
		return false;	
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Wrote %s",s_OutputFile.c_str());
		return true;	
	}

}


/**
 * UpdateMAMEConfig(sMediaURL) - Update MAME Configuration before MAME
 * Launches. This is mainly used to point rompath inside the config 
 * file to the appropriate place where the actual rom being selected.
 * it is also used for setting things like ctrlr files. It uses a
 * standard MAME configuration file, which has the appropriate 
 * values substituted
 */
bool Game_Player::UpdateMAMEConfig(string sMediaURL) 
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
		"aviwrite		   \r\n"
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
		"beam                      1.8\r\n"
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
		"ctrlr		           ##CTRLR##\r\n"                     
		"mouse                     0\r\n"
		"joystick                  0\r\n" 
		"lightgun                  0\r\n"
		"multikeyboard             1\r\n"
		"multimouse                1\r\n"
		"steadykey                 0\r\n"
		"offscreen_reload          0\r\n"
		"joystick_map              auto\r\n"
		"joystick_deadzone         0.3\r\n"
		"joystick_saturation       0.85\r\n"
		"\r\n"
		"#\r\n"
		"# CORE INPUT AUTOMATIC ENABLE OPTIONS\r\n"
		"#\r\n"
		"paddle_device             mouse\r\n"
		"adstick_device            joystick\r\n"
		"pedal_device              keyboard\r\n"
		"dial_device               keyboard\r\n"
		"trackball_device          mouse\r\n"
		"lightgun_device           keyboard\r\n"
		"positional_device         mouse\r\n"
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
		"multithreading            1\r\n"
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

	if (m_sJoystick_Configuration == "")
	{
	  // Do nothing.
	}
	else
	{
	  // Custom Controller Information found, specify and update controller file.
	  s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##CTRLR##","GamePlayer");
	  UpdateControllerFile();
	}
	
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

// Attempt to figure out what to pass to MESS.
string Game_Player::GetMessParametersFor(string sMediaURL)
{
  string sParameters, sPeripheralType, sExtra;     // The final parameters to send.
  string sTmpPath = FileUtils::BasePath(sMediaURL);
  string sFileName = FileUtils::FilenameWithoutPath(sMediaURL);
  string sToken = "/";
  vector<string> vect_Path;

  StringUtils::Tokenize(sTmpPath,sToken,vect_Path);  // split it up
  
  string sMachineType = vect_Path[vect_Path.size()-1]; // Get the last element.
  m_sMachineType = sMachineType;

	string sLFileName = StringUtils::ToLower(sFileName);

	// Apple 2 disk types
	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_apple2_CONST)
	{
		sPeripheralType = "-flop1";	// TODO: support 2 floppies?
		m_bOSDIsVisible=true;		// apple 2 hack for now
	}

	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_jaguar_CONST)
	{
		sPeripheralType = "-cart";
	}

	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_vic20_CONST)
	{
		m_bOSDIsVisible=true;
		sPeripheralType = "-cart";  // usually cart, unless...
		if (sFileName.find(StringUtils::ToLower(".prg")) != string::npos ||
	            sFileName.find(StringUtils::ToLower(".p00")) != string::npos)
		{
			sPeripheralType = "-quik";  // quicklaunch. TODO: Add case in Game Plugin
		}
	}

	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_c64_CONST)
	{
		sPeripheralType = "-flop"; // Usually floppy, unless...
		m_bOSDIsVisible=true;
		if (sFileName.find(StringUtils::ToLower(".crt")) != string::npos ||
		    sFileName.find(StringUtils::ToLower(".80")) != string::npos)
		{
			sPeripheralType = "-cart1"; // cartridge
		}
		if (sFileName.find(StringUtils::ToLower(".prg")) != string::npos ||
		    sFileName.find(StringUtils::ToLower(".p00")) != string::npos ||
		    sFileName.find(StringUtils::ToLower(".t64")) != string::npos)
		{
			sPeripheralType = "-quik"; // quicklaunch.
		}
	}

	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_Atari800_CONST)
	  {
	    m_bOSDIsVisible=true;
	    sPeripheralType = "-flop1"; // usually floppy, unless...
	    if (sFileName.find(StringUtils::ToLower(".rom")) != string::npos ||
		sFileName.find(StringUtils::ToLower(".bin")) != string::npos)
	      {
		sPeripheralType = "-cart1";
	      }
	  }

  if (
		(sFileName.find(StringUtils::ToLower(".fds")) != string::npos))
	{
		sPeripheralType = "-flop";
	}
	
  if (            (sLFileName.find(".bin") != string::npos) || 
                  (sLFileName.find(".a26") != string::npos) || 
		  (sLFileName.find(".a52") != string::npos) ||
		  (sLFileName.find(".a78") != string::npos) ||
		  (sLFileName.find(".col") != string::npos) ||
		  (sLFileName.find(".int") != string::npos) ||
		  (sLFileName.find(".itv") != string::npos) ||
		  (sLFileName.find(".sg") != string::npos) ||
		  (sLFileName.find(".sms") != string::npos) ||
		  (sLFileName.find(".nes") != string::npos) ||
		  (sLFileName.find(".zip") != string::npos) ||
		  (sLFileName.find(".smc") != string::npos) ||
		  (sLFileName.find(".sfc") != string::npos) || 
		  (sLFileName.find(".fig") != string::npos) ||
		  (sLFileName.find(".swc") != string::npos) ||
		  (sLFileName.find(".gen") != string::npos) ||
		  (sLFileName.find(".smd") != string::npos) ||
		  (sLFileName.find(".md") != string::npos) ||
		  (sLFileName.find(".pce") != string::npos) ||
		  (sLFileName.find(".vec") != string::npos))
    {
      sPeripheralType = "-cart";
    }

  // If Peripheral type is still unset here, it may be a zip file. 
  // come back here and write this when we need it.

	// Hack to add overlays for the Vectrex.
	if (m_sMachineType == "vectrex")
	{
		string sTmp = sLFileName.substr(0,sLFileName.length()-4);
		if (sTmp.find("(") != string::npos)
		{
			sTmp = sTmp.substr(0,sTmp.find("(")); // get only the name
			sExtra = "-view\t"+sTmp+"";
		} 
	}

  m_sPeripheralType = sPeripheralType; // used by PostLaunchMESS()

  sParameters = sMachineType + "\t" + sPeripheralType + "\t" + sMediaURL + "\t" +sExtra+"";

  return sParameters;
}

string Game_Player::CreateWindowIDString(long unsigned int window)
{
	// FIXME: Provide some sanity checking later.
	stringstream ss;
	ss << window;
	return ss.str();
}

bool Game_Player::LaunchMESS(string sMediaURL)
{
  string sState = "";
  size_t iROMNameSize = FileUtils::FilenameWithoutPath(sMediaURL).size();
  m_sROMName = FileUtils::FilenameWithoutPath(sMediaURL).substr(0,iROMNameSize-4);
  m_sFilename = FileUtils::FilenameWithoutPath(sMediaURL);
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ROM is: %s",m_sROMName.c_str());
  DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
  if( pDevice_App_Server )
    {
      string sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " 1 " 
	TOSTRING(COMMAND_Application_Exited_CONST) " " 
	TOSTRING(COMMANDPARAMETER_Exit_Code_CONST) " ";
      
      if (!UpdateMESSConfig(sMediaURL)) 
	{
	  return false;
	} // Make sure stella's configuration is correct. 
      
      string sParameters = GetMessParametersFor(sMediaURL);

      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sParameters is: %s%s",sParameters.c_str(),sState.c_str());

	if (m_bLoadSavedGame) // m_bLoadSavedGame is set in CMD_Play_Media
	{
		//sState = "\t-state\t9";
	}

      DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"mess", "mess", sParameters+sState,
						       sMessage + "1",sMessage + "0",false,false,true,false);
      if( SendCommand(CMD_Spawn_Application) ) {
	m_bMAMEIsRunning = true;
	
	Sleep(5000); // FIXME !!!!
	
	if (!WindowUtils::FindWindowMatching(m_pDisplay, "MESS", m_iMAMEWindowId))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't find MESS Window");	
	  }
	else
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"MESS window found: Window ID %d",m_iMAMEWindowId);
	    m_sMAMEWindowId = CreateWindowIDString(m_iMAMEWindowId); 
	  }
	m_pAlarmManager->AddRelativeAlarm(5,this,CHECK_MAME,NULL);
	PostLaunchMESS();
	
	return true; }
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMESS - failed to launch");
    }
  else
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMESS - no app server");
  return false;
  
}

/**
 * This is used, in some emulators to send keypresses to the emulated window
 * after launch, eg. to facilitate -quik loaded images in vic20/c64
 */
void Game_Player::PostLaunchMESS()
{

	Sleep(4000); // Hackorama.

	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_c64_CONST)
	{
		if (m_sPeripheralType == "-quik")
		{
			// Quik Launch. type RUN<enter>
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_R,m_iEventSerialNum++);
			Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_U,m_iEventSerialNum++);
			Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_N,m_iEventSerialNum++);
			Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Return,m_iEventSerialNum++);
			Sleep(100);
		 	return;	
		}

		if (m_sPeripheralType == "-flop")
		{
			// A disk was loaded, type LOAD"*",8,1
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_L,m_iEventSerialNum++);
			Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_O,m_iEventSerialNum++);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_A,m_iEventSerialNum++);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_D,m_iEventSerialNum++);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++,XK_Shift_L);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_bracketright,m_iEventSerialNum++);
                        Sleep(100);

                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++,XK_Shift_L);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_comma,m_iEventSerialNum++);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_8,m_iEventSerialNum++);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_comma,m_iEventSerialNum++);
                        Sleep(100);
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);
                        Sleep(100);
                        WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_semicolon,m_iEventSerialNum++);
                        Sleep(100);
                        WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_R,m_iEventSerialNum++);
                        Sleep(100);
                        WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_U,m_iEventSerialNum++);
                        Sleep(100);
                        WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_N,m_iEventSerialNum++);
                        Sleep(100);

			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Return,m_iEventSerialNum++);
                        Sleep(100);
			return;
		}
	}
}

bool Game_Player::LaunchMAME(string sMediaURL) 
{

	string sState = "";

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

		// m_bLoadSavedGame is set in CMD_Play_Media
		if (m_bLoadSavedGame)
		{
			//sState = "\t-state\t9"; // load saved state from slot 9
		}

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"mame", "mame", sMediaURL+ sState,
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
	 	m_sMAMEWindowId = CreateWindowIDString(m_iMAMEWindowId);	
	}
			m_pAlarmManager->AddRelativeAlarm(5,this,CHECK_MAME,NULL);	
		 	return true; }
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMAME - failed to launch");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMAME - no app server");
	return false;

}

bool Game_Player::StopMESS() 
{
  DeviceData_Base *pDevice_App_Server = NULL;
  string sResponse;
  if(!m_bRouterReloading)
    {
      m_pAlarmManager->CancelAlarmByType(CHECK_MAME);
      pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
      if( pDevice_App_Server )
	{
	  DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
							 "mess", false);
	  m_iMAMEWindowId = 0;
	  return SendCommand(CMD_Kill_Application,&sResponse);  // Get return confirmation so we know it's gone before we continue
	}
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Game_Player::StopMESS %p %s", pDevice_App_Server,sResponse.c_str());
  return false;
}

bool Game_Player::StopMAME() {

        DeviceData_Base *pDevice_App_Server = NULL;
        string sResponse;
        if(!m_bRouterReloading)
        {
		m_pAlarmManager->CancelAlarmByType(CHECK_MAME);
                pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
                if( pDevice_App_Server )
                {
                        DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
                                "mame", false);
			m_iMAMEWindowId = 0;
                        return SendCommand(CMD_Kill_Application,&sResponse);  // Get return confirmation so we know it's gone before we continue
                }
        }

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Game_Player::StopMAME %p %s", pDevice_App_Server,sResponse.c_str());
        return false;

}

bool Game_Player::LaunchPCSX(string sMediaURL) 
{

	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		string sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " 1 " 
			TOSTRING(COMMAND_Application_Exited_CONST) " " 
			TOSTRING(COMMANDPARAMETER_Exit_Code_CONST) " ";

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"pcsx", "pcsx", "-nogui\t-psxout\t-cdfile\t" + sMediaURL,
			sMessage + "1",sMessage + "0",false,false,true,false);
		if( SendCommand(CMD_Spawn_Application) ) {
			m_bMAMEIsRunning = true;

	Sleep(5000); // FIXME !!!!

	if (!WindowUtils::FindWindowMatching(m_pDisplay, "PCSX", m_iMAMEWindowId))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't find PCSX Window");	
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PCSX window found: Window ID %d",m_iMAMEWindowId);
	 	m_sMAMEWindowId = CreateWindowIDString(m_iMAMEWindowId);	
	}

		 	return true; }
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMAME - failed to launch");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMAME - no app server");
	return false;

}

bool Game_Player::StopPCSX() 
{
  DeviceData_Base *pDevice_App_Server = NULL;
  string sResponse;
  if(!m_bRouterReloading)
    {
      pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
      if( pDevice_App_Server )
	{
	  DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
							 "pcsx", false);
	  m_iMAMEWindowId = 0;
	  return SendCommand(CMD_Kill_Application,&sResponse);  // Get return confirmation so we know it's gone before we continue
	}
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Game_Player::StopMESS %p %s", pDevice_App_Server,sResponse.c_str());
  return false;
}

// come back here
bool Game_Player::LaunchPCSX2(string sMediaURL) 
{

	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		string sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " 1 " 
			TOSTRING(COMMAND_Application_Exited_CONST) " " 
			TOSTRING(COMMANDPARAMETER_Exit_Code_CONST) " ";

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/pcsx2/bin/pcsx2-dev", "pcsx2", "--nogui\t" + sMediaURL,
			sMessage + "1",sMessage + "0",false,false,true,false);
		if( SendCommand(CMD_Spawn_Application) ) {
			m_bMAMEIsRunning = true;

	Sleep(5000); // FIXME !!!!

	if (!WindowUtils::FindWindowMatching(m_pDisplay, "PCSX2", m_iMAMEWindowId))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't find PCSX Window");	
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PCSX window found: Window ID %d",m_iMAMEWindowId);
	 	m_sMAMEWindowId = CreateWindowIDString(m_iMAMEWindowId);	
	}

		 	return true; }
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMAME - failed to launch");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::LaunchMAME - no app server");
	return false;

}

bool Game_Player::StopPCSX2() 
{
  DeviceData_Base *pDevice_App_Server = NULL;
  string sResponse;
  if(!m_bRouterReloading)
    {
      pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
      if( pDevice_App_Server )
	{
	  DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
							 "pcsx2", false);
	  m_iMAMEWindowId = 0;
	  return SendCommand(CMD_Kill_Application,&sResponse);  // Get return confirmation so we know it's gone before we continue
	}
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Game_Player::StopMESS %p %s", pDevice_App_Server,sResponse.c_str());
  return false;
}

/**
 * Periodically check the game player's window geometry.
 */
void Game_Player::CheckMAME() 
{

	int x,y,w,h;
	if (WindowUtils::GetWindowGeometry(m_pDisplay,m_iMAMEWindowId,x,y,w,h))
	{

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Game_Player::CheckMAME() - Current Window Geometry is x:%d y:%d w:%dpx h:%dpx",x,y,w,h);
#endif

		m_iNowPlayingX = x;
		m_iNowPlayingY = y;
		m_iNowPlayingW = w;
		m_iNowPlayingH = h;

	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CheckMAME() - Called and could not get Window Geometry, is Emulator running?");
	}

	// reset the alarm thread
	m_pAlarmManager->CancelAlarmByType(CHECK_MAME);
	m_pAlarmManager->AddRelativeAlarm(5,this,CHECK_MAME,NULL);

}

void Game_Player::AlarmCallback(int id, void* param) 
{
	switch (id)
	{
		case CHECK_MAME:
			CheckMAME();
			break;	
	}
}

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
void Game_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Game_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Game_Player::SomeFunction()
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

/**
 * Process modifier. If Shift/Ctrl/etc are pressed. Set flags. 
 */
void Game_Player::ProcessModifier(int iPK_Button)
{
	m_iModifier = iPK_Button;
}

/**
 * Process alphanumeric keys.
 */
void Game_Player::ProcessAlphanumeric(int iXKeySym)
{
	if (m_iModifier == BUTTON_Shift_Left_CONST)
	{
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iXKeySym,m_iEventSerialNum++,XK_Shift_L);
		m_iModifier = 0;
	}
	else if (m_iModifier == BUTTON_Shift_Right_CONST)
	{
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iXKeySym,m_iEventSerialNum++,XK_Shift_R);
		m_iModifier = 0;
	}
	else if (m_iModifier == BUTTON_Control_CONST)
	{
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iXKeySym,m_iEventSerialNum++,XK_Control_L);
		m_iModifier = 0;
	}
	else 
	{
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iXKeySym,m_iEventSerialNum++);
	}
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

void Game_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	cout << "Need to implement command #28 - Simulate Keypress" << endl;
	cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #50 - Name=" << sName << endl;

	// Basic alphanumeric processing.
	int iPK_Button = atoi(sPK_Button.c_str());
	
	// Shift and control key processing.
	switch (iPK_Button)
	{
		case BUTTON_Shift_Left_CONST:
			ProcessModifier(BUTTON_Shift_Left_CONST);
		break;
		case BUTTON_Shift_Right_CONST:
			ProcessModifier(BUTTON_Shift_Right_CONST);
		break;
		case BUTTON_Control_CONST:
			ProcessModifier(BUTTON_Control_CONST);
		break;
	}
	
	// Function keys
	switch (iPK_Button)
	{
		case BUTTON_F1_CONST:
			ProcessAlphanumeric(XK_F1);
		break;
               case BUTTON_F2_CONST:
                        ProcessAlphanumeric(XK_F2);
                break;
               case BUTTON_F3_CONST:
                        ProcessAlphanumeric(XK_F3);
                break;
               case BUTTON_F4_CONST:
                        ProcessAlphanumeric(XK_F4);
                break;
               case BUTTON_F5_CONST:
                        ProcessAlphanumeric(XK_F5);
                break;
               case BUTTON_F6_CONST:
                        ProcessAlphanumeric(XK_F6);
                break;
               case BUTTON_F7_CONST:
                        ProcessAlphanumeric(XK_F7);
                break;
               case BUTTON_F8_CONST:
                        ProcessAlphanumeric(XK_F8);
                break;
               case BUTTON_F9_CONST:
                        ProcessAlphanumeric(XK_F9);
                break;
               case BUTTON_F10_CONST:
                        ProcessAlphanumeric(XK_F10);
                break;

	}

	// Standard Alphanumeric Processing.
	switch (iPK_Button)
	{
		case BUTTON_1_CONST:
			ProcessAlphanumeric(XK_1);
		break;
                case BUTTON_2_CONST:
                        ProcessAlphanumeric(XK_2);
                break;
                case BUTTON_3_CONST:
                        ProcessAlphanumeric(XK_3);
                break;
                case BUTTON_4_CONST:
                        ProcessAlphanumeric(XK_4);
                break;
                case BUTTON_5_CONST:
                        ProcessAlphanumeric(XK_5);
                break;
                case BUTTON_6_CONST:
                        ProcessAlphanumeric(XK_6);
                break;
                case BUTTON_7_CONST:
                        ProcessAlphanumeric(XK_7);
                break;
                case BUTTON_8_CONST:
                        ProcessAlphanumeric(XK_8);
                break;
                case BUTTON_9_CONST:
                        ProcessAlphanumeric(XK_9);
                break;
                case BUTTON_0_CONST:
                        ProcessAlphanumeric(XK_0);
                break;
                case BUTTON_q_CONST:
                        ProcessAlphanumeric(XK_q);
                break;
                case BUTTON_w_CONST:
                        ProcessAlphanumeric(XK_w);
                break;
                case BUTTON_e_CONST:
                        ProcessAlphanumeric(XK_e);
                break;
                case BUTTON_r_CONST:
                        ProcessAlphanumeric(XK_r);
                break;
                case BUTTON_t_CONST:
                        ProcessAlphanumeric(XK_t);
                break;
                case BUTTON_y_CONST:
                        ProcessAlphanumeric(XK_y);
                break;
                case BUTTON_u_CONST:
                        ProcessAlphanumeric(XK_u);
                break;
                case BUTTON_i_CONST:
                        ProcessAlphanumeric(XK_i);
                break;
                case BUTTON_o_CONST:
                        ProcessAlphanumeric(XK_o);
                break;
                case BUTTON_p_CONST:
                        ProcessAlphanumeric(XK_p);
                break;
                case BUTTON_a_CONST:
                        ProcessAlphanumeric(XK_a);
                break;
                case BUTTON_s_CONST:
                        ProcessAlphanumeric(XK_s);
                break;
                case BUTTON_d_CONST:
                        ProcessAlphanumeric(XK_d);
                break;
                case BUTTON_f_CONST:
                        ProcessAlphanumeric(XK_f);
                break;
                case BUTTON_g_CONST:
                        ProcessAlphanumeric(XK_g);
                break;
                case BUTTON_h_CONST:
                        ProcessAlphanumeric(XK_h);
                break;
                case BUTTON_j_CONST:
                        ProcessAlphanumeric(XK_j);
                break;
                case BUTTON_k_CONST:
                        ProcessAlphanumeric(XK_k);
                break;
                case BUTTON_l_CONST:
                        ProcessAlphanumeric(XK_l);
                break;
                case BUTTON_z_CONST:
                        ProcessAlphanumeric(XK_z);
                break;
                case BUTTON_x_CONST:
                        ProcessAlphanumeric(XK_x);
                break;
                case BUTTON_c_CONST:
                        ProcessAlphanumeric(XK_c);
                break;
                case BUTTON_v_CONST:
                        ProcessAlphanumeric(XK_v);
                break;
                case BUTTON_b_CONST:
                        ProcessAlphanumeric(XK_b);
                break;
                case BUTTON_n_CONST:
                        ProcessAlphanumeric(XK_n);
                break;
                case BUTTON_m_CONST:
                        ProcessAlphanumeric(XK_m);
                break;
		case BUTTON_Enter_CONST:
			ProcessAlphanumeric(XK_Return);
		break;
		case BUTTON_Back_CONST:
			ProcessAlphanumeric(XK_BackSpace);
		break;
	case BUTTON_space_CONST:
	  ProcessAlphanumeric(XK_space);
	  break;
	case BUTTON_tab_CONST:
	  ProcessAlphanumeric(XK_Tab);
	  break;
	case BUTTON_escape_CONST:
	  ProcessAlphanumeric(XK_Escape);
	  break;
	case BUTTON_dash_CONST:
	  if (m_iPK_MediaType == MEDIATYPE_lmce_Game_vic20_CONST ||
	      m_iPK_MediaType == MEDIATYPE_lmce_Game_c64_CONST)
	    {
	      ProcessAlphanumeric(XK_equal);
	    }
	  else
	    ProcessAlphanumeric(XK_minus);
	  break;
	case BUTTON_equals_sign_CONST:
	  if (m_iPK_MediaType==MEDIATYPE_lmce_Game_vic20_CONST ||
	      m_iPK_MediaType==MEDIATYPE_lmce_Game_c64_CONST)
	    {
	      ProcessAlphanumeric(XK_backslash);
	    }
	  else
	    ProcessAlphanumeric(XK_equal);
	  break;
	case BUTTON_Bracket_Left_CONST:
	  ProcessAlphanumeric(XK_bracketleft);
	  break;
	case BUTTON_Bracket_Right_CONST:
	  ProcessAlphanumeric(XK_bracketright);
	  break;
	case BUTTON_Backslash_CONST:
	  ProcessAlphanumeric(XK_backslash);
	  break;
	case BUTTON_colon_CONST:
	  ProcessAlphanumeric(XK_semicolon);
	  break;
	case BUTTON_semicolumn_CONST:
	  if (m_iPK_MediaType == MEDIATYPE_lmce_Game_vic20_CONST ||
	      m_iPK_MediaType == MEDIATYPE_lmce_Game_c64_CONST)
	    {
	      ProcessAlphanumeric(XK_apostrophe);
	    }
	  else
	    ProcessAlphanumeric(XK_semicolon);
	  break;
	case BUTTON_single_quote_CONST:
	  ProcessAlphanumeric(XK_apostrophe);
	  break;
	case BUTTON_comma_CONST:
	  ProcessAlphanumeric(XK_comma);
	  break;
	case BUTTON_period_CONST:
	  ProcessAlphanumeric(XK_period);
	  break;
	case BUTTON_slash_CONST:
	  ProcessAlphanumeric(XK_slash);
	  break;
	case BUTTON_plus_CONST:
	  if (m_iPK_MediaType == MEDIATYPE_lmce_Game_vic20_CONST || 
	      m_iPK_MediaType == MEDIATYPE_lmce_Game_c64_CONST)
	    {
	      ProcessAlphanumeric(XK_minus);
	    }
	  break;
	case BUTTON_sterling_CONST:
          // FIXME: DEADKEY. NO KNOWN KEYCODE.
	  // ProcessAlphanumeric(XK_asciicircum);
	  break;
	case BUTTON_commoClrHome_CONST:
	  ProcessAlphanumeric(XK_Insert);
	  break;
	case BUTTON_commoInsDel_CONST:
	  ProcessAlphanumeric(XK_BackSpace);
	  break;
	case BUTTON_Restore_CONST:
	  ProcessAlphanumeric(XK_Print); // is this right??
	    break;
	case BUTTON_at_CONST:
	  ProcessAlphanumeric(XK_bracketleft);
	  break;
	case BUTTON_asterisk_CONST:
	  ProcessAlphanumeric(XK_bracketright);
	  break;
	case BUTTON_commoUp_CONST:
	  ProcessAlphanumeric(XK_Delete);
	  break;
	case BUTTON_commoLeft_CONST:
	  ProcessAlphanumeric(XK_asciitilde);
	  break;
	case BUTTON_Run_Stop_CONST:
	  ProcessAlphanumeric(XK_Home);
	  break;
	case BUTTON_commoKey_CONST:
	  // FIXME is this a meta? Talk to a c64 user.
	  break;
	case BUTTON_commoCRSRLR_CONST:
	  ProcessAlphanumeric(XK_Control_R);
	  break;
	case BUTTON_commoCRSRUD_CONST:
	  ProcessAlphanumeric(XK_Meta_R);
	  break;
	}

}

void Game_Player::ProcessPoundForMediaType(int iPK_MediaType)
{
	switch (iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_Enter,m_iEventSerialNum++);
			break;
		case MEDIATYPE_lmce_Game_coleco_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_minus,m_iEventSerialNum++);
			break;
		case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_Add,m_iEventSerialNum++);
			break;
		default:
			// Do we need a default?
			break;
	}
}

void Game_Player::ProcessAsteriskForMediaType(int iPK_MediaType)
{
	switch (iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_Add,m_iEventSerialNum++);
			break;
		case MEDIATYPE_lmce_Game_coleco_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_equal,m_iEventSerialNum++);
			break;
		case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_Subtract,m_iEventSerialNum++);
			break;
		default:
			// Do we need a default?
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

void Game_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
	cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	VideoFrameGeometry* vfg = m_mapVideoFrameGeometry[pMessage->m_dwPK_Device_From];
	double iScaleW = (double)m_iNowPlayingW / (double)vfg->m_iWidth;
	double iScaleH = (double)m_iNowPlayingH / (double)vfg->m_iHeight;
	int iScaledX = floor((double)iPosition_X * iScaleW);
	int iScaledY = floor((double)iPosition_Y * iScaleH);
	
	cout << "iScaleW = " << iScaleW << " : iScaleH = " << iScaleH << endl;
	cout << "iScaledX = " << iScaledX << " : iScaledY = " << iScaledY << endl;

	// Send a double click, due to MAME's assumption that it is a mouse.
	WindowUtils::SendClickToWindow(m_pDisplay, m_iMAMEWindowId, 1, iScaledX, iScaledY);
	Sleep(100);
	WindowUtils::SendClickToWindow(m_pDisplay, m_iMAMEWindowId, 1, iScaledX, iScaledY);
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

void Game_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

	m_bOSDIsVisible = false;
	m_iPK_MediaType = iPK_MediaType;
	m_iStreamID = iStreamID;

	if (!sMediaPosition.empty())
	{
		m_bLoadSavedGame = true;
	        string sPath = GetSaveGamePath();
        	string sCopyCmd = "cp "+sMediaPosition+" "+sPath+"/9.sta";
        	system(sCopyCmd.c_str());
	}
	else
	{
		m_bLoadSavedGame = false;
	}

	switch(iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_CONST:
			LaunchMAME(sMediaURL);
			break;
		case MEDIATYPE_lmce_Game_a2600_CONST:
		case MEDIATYPE_lmce_Game_a5200_CONST:
		case MEDIATYPE_lmce_Game_a7800_CONST:
		case MEDIATYPE_lmce_Game_coleco_CONST:
		case MEDIATYPE_lmce_Game_intv_CONST:
		case MEDIATYPE_lmce_Game_sg1000_CONST:
		case MEDIATYPE_lmce_Game_sms_CONST:
		case MEDIATYPE_lmce_Game_nes_CONST:
		case MEDIATYPE_lmce_Game_famicom_CONST:
		case MEDIATYPE_lmce_Game_snes_CONST:
		case MEDIATYPE_lmce_Game_genesis_CONST:
		case MEDIATYPE_lmce_Game_megadriv_CONST:
		case MEDIATYPE_lmce_Game_tg16_CONST:
		case MEDIATYPE_lmce_Game_pce_CONST:
		case MEDIATYPE_lmce_Game_sgx_CONST:
		case MEDIATYPE_lmce_Game_vectrex_CONST:
		case MEDIATYPE_lmce_Game_apple2_CONST:
		case MEDIATYPE_lmce_Game_jaguar_CONST:
		case MEDIATYPE_lmce_Game_vic20_CONST:
		case MEDIATYPE_lmce_Game_c64_CONST:
	case MEDIATYPE_lmce_Game_Atari800_CONST:
			LaunchMESS(sMediaURL);
			break;
		case MEDIATYPE_lmce_Game_ps1_CONST:
			LaunchPCSX(sMediaURL);
			break;
		case MEDIATYPE_lmce_Game_ps2_CONST:
			LaunchPCSX2(sMediaURL);
			break;
		default:
			LaunchMAME(sMediaURL);
			break;
	}
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Game_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

        // First, send a command to save to slot 1.
        WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F7,m_iEventSerialNum++,XK_Shift_L);
        Sleep(100);
        WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);

        string sPath = GetSaveGamePath();
        char *cName;

	// FIXME: come back here and handle case for PCSX.

        if (m_iPK_MediaType == MEDIATYPE_lmce_Game_CONST)
          {
            cName = tempnam(sPath.c_str(),m_sROMName.c_str());
          }
        else
          {
            cName = tempnam(sPath.c_str(),m_sMachineType.c_str());
          }

        string sName = cName;
        string sMoveCmd = "mv "+sPath+"/1.sta "+sName;

        system(sMoveCmd.c_str());

	*sMediaPosition = sName;

	switch( m_iPK_MediaType)
	  {
	  case MEDIATYPE_lmce_Game_CONST:
	    StopMAME();
	    break;
	  case MEDIATYPE_lmce_Game_a2600_CONST:
	  case MEDIATYPE_lmce_Game_a5200_CONST:
	  case MEDIATYPE_lmce_Game_a7800_CONST:
	  case MEDIATYPE_lmce_Game_coleco_CONST:
	  case MEDIATYPE_lmce_Game_intv_CONST:
	  case MEDIATYPE_lmce_Game_sg1000_CONST:
	  case MEDIATYPE_lmce_Game_sms_CONST:
	  case MEDIATYPE_lmce_Game_famicom_CONST:
	  case MEDIATYPE_lmce_Game_nes_CONST:
	  case MEDIATYPE_lmce_Game_snes_CONST:
          case MEDIATYPE_lmce_Game_genesis_CONST:
          case MEDIATYPE_lmce_Game_megadriv_CONST:
	  case MEDIATYPE_lmce_Game_pce_CONST:
	  case MEDIATYPE_lmce_Game_sgx_CONST:
	  case MEDIATYPE_lmce_Game_tg16_CONST:
	  case MEDIATYPE_lmce_Game_vectrex_CONST:
	  case MEDIATYPE_lmce_Game_apple2_CONST:
	  case MEDIATYPE_lmce_Game_jaguar_CONST:
	  case MEDIATYPE_lmce_Game_vic20_CONST:
	  case MEDIATYPE_lmce_Game_c64_CONST:
	  case MEDIATYPE_lmce_Game_Atari800_CONST:
	    StopMESS();
	    break;
	  case MEDIATYPE_lmce_Game_ps1_CONST:
	    StopPCSX();
	    break;
	  case MEDIATYPE_lmce_Game_ps2_CONST:
	    StopPCSX2();
		break;
	  }
	m_bOSDIsVisible=false;
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Game_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
	cout << "Parm #220 - Report=" << bReport << endl;
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void Game_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	// Ironically, this is here to implement the save game stuff.
	if (sValue_To_Assign == "0")
	{
		// This is the original game, just send a reset. 
		CMD_Game_Reset();
		return;
	}
	else
	{
		// This is a save point, call Set Media Position.
		CMD_Set_Media_Position(m_iStreamID,sValue_To_Assign);
	}

}

//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void Game_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
	cout << "Need to implement command #84 - Get Video Frame" << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #20 - Format=" << sFormat << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #60 - Width=" << iWidth << endl;
	cout << "Parm #61 - Height=" << iHeight << endl;

	PLUTO_SAFETY_LOCK(mm,m_GameMutex);

	string sPath, screenName, sGeometry;
	stringstream ssWidth, ssHeight;

	ssWidth << iWidth;
	ssHeight << iHeight;
	sGeometry = ssWidth.str()+"x"+ssHeight.str();

	m_mapVideoFrameGeometry[pMessage->m_dwPK_Device_From] = new VideoFrameGeometry(iWidth,iHeight);

	if (m_bOSDIsVisible)
	{
		// Use alternate code to display OSD
		sPath = "/tmp/";
		screenName = "OSD";

		// use ImageMagick. Awfully slow!
		// string cmd = "import -window " + m_sMAMEWindowId + " "+sPath+screenName+".png";
		string cmd = "scrot -u -t "+sGeometry+" "+sPath+screenName+".jpg";
		system(cmd.c_str());
		cmd = "mv "+sPath+screenName+"-thumb.jpg "+sPath+screenName+".jpg";
		system(cmd.c_str());
		*sFormat="2"; // JPEG.
	}
	else
	{
	
		switch(m_iPK_MediaType)
		  {
		  case MEDIATYPE_lmce_Game_CONST:
		    sPath = "/home/mamedata/shots/";
		    screenName = m_sROMName + "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_a2600_CONST:
		    sPath = "/home/mamedata/shots/a2600";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_a5200_CONST:
		    sPath = "/home/mamedata/shots/a5200";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_a7800_CONST:
		    sPath = "/home/mamedata/shots/a7800";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_coleco_CONST:
		    sPath = "/home/mamedata/shots/coleco";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_intv_CONST:
		    sPath = "/home/mamedata/shots/intv";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_sg1000_CONST:
		    sPath = "/home/mamedata/shots/sg1000";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_sms_CONST:
		    sPath = "/home/mamedata/shots/sms";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_famicom_CONST:
		    sPath = "/home/mamedata/shots/famicom";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_nes_CONST:
		    sPath = "/home/mamedata/shots/nes";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_snes_CONST:
		    sPath = "/home/mamedata/shots/snes";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_genesis_CONST:
		    sPath = "/home/mamedata/shots/genesis";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_megadriv_CONST:
		    sPath = "/home/mamedata/shots/megadriv";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_tg16_CONST:
		    sPath = "/home/mamedata/shots/tg16";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_pce_CONST:
		    sPath = "/home/mamedata/shots/pce";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_sgx_CONST:
		    sPath = "/home/mamedata/shots/sgx";
                    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_vectrex_CONST:
		    sPath = "/home/mamedata/shots/vectrex";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_apple2_CONST:
		    sPath = "/home/mamedata/shots/apple2cp";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_jaguar_CONST:
		    sPath = "/home/mamedata/shots/jaguar";
		    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_vic20_CONST:
		    sPath = "/home/mamedata/shots/vic20";
	            screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_c64_CONST:
                    sPath = "/home/mamedata/shots/c64";
                    screenName = "/0000";
		    break;
		  case MEDIATYPE_lmce_Game_Atari800_CONST:
		    sPath = "/home/mamedata/shots/a800";
		    screenName = "/0000";
		  }
	
		string snapPath = sPath + m_sROMName + "/";
		FileUtils::DelFile( sPath + screenName + ".png");
		FileUtils::DelFile( sPath + screenName + ".jpg");
	
		WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F12,m_iEventSerialNum++); // hehe, hack++
	}

	// Only run Imagemagick to downsize if OSD isn't visible. Let's make this faster too later.
	if (!m_bOSDIsVisible)
	{
		Sleep(50);
		string s_OutputString = "convert -sample 800x800 " + sPath + screenName + ".png "+sPath+screenName+".jpg";	
		system(s_OutputString.c_str());
	}

	size_t size;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MAME_Player::CMD_Get_Video_Frame got %d",size);
	if (!m_bOSDIsVisible)
	{
		*pData = FileUtils::ReadFileIntoBuffer(sPath+screenName+".png",size);
	}
	else
	{
		*pData = FileUtils::ReadFileIntoBuffer(sPath+screenName+".jpg",size);
	}
	*iData_Size = size;

	FileUtils::DelFile(sPath+screenName+".png");
	FileUtils::DelFile(sPath+screenName+".jpg");
	
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

void Game_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
	cout << "Need to implement command #87 - Goto Media Menu" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #64 - MenuType=" << iMenuType << endl;

	switch(iMenuType) {
		case SHOW_GAME_VIEW:
			if (m_bOSDIsVisible)
			{
				CMD_Simulate_Keypress(StringUtils::itos(BUTTON_tab_CONST),iStreamID,""); // toggle off
			}
			m_bOSDIsVisible = false;
			EVENT_Menu_Onscreen(iStreamID,true);
			break;
		case SHOW_REMOTE:
			if (m_bOSDIsVisible)
			{
				CMD_Simulate_Keypress(StringUtils::itos(BUTTON_tab_CONST),iStreamID,""); // toggle off
			}
			m_bOSDIsVisible = false;
			EVENT_Menu_Onscreen(iStreamID,false);
			break;
		case SHOW_OSD:
			m_bOSDIsVisible = !m_bOSDIsVisible; // Toggle.
			CMD_Simulate_Keypress(StringUtils::itos(BUTTON_tab_CONST),iStreamID,""); // also a toggle.
			if (m_bOSDIsVisible)
			{
				EVENT_Menu_Onscreen(iStreamID,true);
			}
			else
			{
				EVENT_Menu_Onscreen(iStreamID,false);
			}
			break;
		default:
			m_bOSDIsVisible = false;
			EVENT_Menu_Onscreen(iStreamID,false);
			break;
		}

}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
	cout << "Need to implement command #95 - Stop" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #203 - Eject=" << bEject << endl;
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void Game_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	cout << "Need to implement command #126 - Guide" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Tab,m_iEventSerialNum++);
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
	cout << "Need to implement command #139 - Play" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c140-b->

	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	cout << "Need to implement command #203 - Move Right" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Right,m_iEventSerialNum++);


}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void Game_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	cout << "Need to implement command #204 - 0" << endl;

	switch(m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_0,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_0,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void Game_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	cout << "Need to implement command #205 - 1" << endl;

	switch(m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
		case MEDIATYPE_lmce_Game_coleco_CONST:
		case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_1,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void Game_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	cout << "Need to implement command #206 - 2" << endl;

	switch(m_iPK_MediaType)
	{	
		case MEDIATYPE_lmce_Game_a5200_CONST:
		case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_2,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void Game_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	cout << "Need to implement command #207 - 3" << endl;

	switch(m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_3,m_iEventSerialNum++);
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_3,m_iEventSerialNum++);
			break;
	}

}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void Game_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	cout << "Need to implement command #208 - 4" << endl;

	switch(m_iPK_MediaType)
	{	
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_4,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_4,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void Game_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	cout << "Need to implement command #209 - 5" << endl;
	
	switch(m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_5,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_5,m_iEventSerialNum++);
			break;
	}

}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void Game_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	cout << "Need to implement command #210 - 6" << endl;

	switch(m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_6,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_6,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void Game_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	cout << "Need to implement command #211 - 7" << endl;

	switch(m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_7,m_iEventSerialNum++);
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_7,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void Game_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	cout << "Need to implement command #212 - 8" << endl;

	switch(m_iPK_MediaType)
	{	
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_8,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_8,m_iEventSerialNum++);
			break;

	}
}
//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void Game_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	cout << "Need to implement command #213 - 9" << endl;

	switch(m_iPK_MediaType)
	{
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_9,m_iEventSerialNum++);
			break;
		case MEDIATYPE_lmce_Game_a5200_CONST:
                case MEDIATYPE_lmce_Game_coleco_CONST:
                case MEDIATYPE_lmce_Game_intv_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_KP_9,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	cout << "Need to implement command #240 - Back / Prior Menu" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_Escape,m_iEventSerialNum++);
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

void Game_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	cout << "Need to implement command #249 - Start Streaming" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;
}

/**
 * Get Save Game Path for a given media type.
 */
string Game_Player::GetSaveGamePath()
{

  string sPath;

  switch(m_iPK_MediaType)
    {
    case MEDIATYPE_lmce_Game_CONST:
      sPath = "/home/mamedata/sta/" + m_sROMName + "";
      break;
    case MEDIATYPE_lmce_Game_a2600_CONST:
      sPath = "/home/mamedata/sta/a2600";
      break;
    case MEDIATYPE_lmce_Game_a5200_CONST:
      sPath = "/home/mamedata/sta/a5200";
      break;
    case MEDIATYPE_lmce_Game_a7800_CONST:
      sPath = "/home/mamedata/sta/a7800";
      break;
    case MEDIATYPE_lmce_Game_coleco_CONST:
      sPath = "/home/mamedata/sta/coleco";
      break;
    case MEDIATYPE_lmce_Game_intv_CONST:
      sPath = "/home/mamedata/sta/intv";
      break;
    case MEDIATYPE_lmce_Game_sg1000_CONST:
      sPath = "/home/mamedata/sta/sg1000";
      break;
    case MEDIATYPE_lmce_Game_sms_CONST:
      sPath = "/home/mamedata/sta/sms";
      break;
    case MEDIATYPE_lmce_Game_famicom_CONST:
      sPath = "/home/mamedata/sta/famicom";
      break;
    case MEDIATYPE_lmce_Game_nes_CONST:
      sPath = "/home/mamedata/sta/nes";
      break;
    case MEDIATYPE_lmce_Game_snes_CONST:
      sPath = "/home/mamedata/sta/snes";
      break;
    case MEDIATYPE_lmce_Game_genesis_CONST:
      sPath = "/home/mamedata/sta/genesis";
      break;
    case MEDIATYPE_lmce_Game_megadriv_CONST:
      sPath = "/home/mamedata/sta/megadriv";
      break;
    case MEDIATYPE_lmce_Game_tg16_CONST:
      sPath = "/home/mamedata/sta/tg16";
      break;
    case MEDIATYPE_lmce_Game_pce_CONST:
      sPath = "/home/mamedata/sta/pce";
      break;
    case MEDIATYPE_lmce_Game_sgx_CONST:
      sPath = "/home/mamedata/sta/sgx";
      break;
    case MEDIATYPE_lmce_Game_vectrex_CONST:
      sPath = "/home/mamedata/sta/vectrex";
      break;
    case MEDIATYPE_lmce_Game_apple2_CONST:
      sPath = "/home/mamedata/sta/apple2cp";
      break;
    case MEDIATYPE_lmce_Game_jaguar_CONST:
      sPath = "/home/mamedata/sta/jaguar";
      break;
    case MEDIATYPE_lmce_Game_vic20_CONST:
      sPath = "/home/mamedata/sta/vic20";
      break;
    case MEDIATYPE_lmce_Game_c64_CONST:
      sPath = "/home/mamedata/sta/c64";
      break;
    case MEDIATYPE_lmce_Game_Atari800_CONST:
      sPath = "/home/mamedata/sta/a800";
    default:
      sPath = "";
      break;
    }

  return sPath;
  
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

void Game_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

	// First, send a command to save to slot 1.
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F7,m_iEventSerialNum++,XK_Shift_L);
	Sleep(100);
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_9,m_iEventSerialNum++);

	string sPath = GetSaveGamePath();
	char *cName;
	
	if (m_iPK_MediaType == MEDIATYPE_lmce_Game_CONST)
	  {
	    cName = tempnam(sPath.c_str(),m_sROMName.c_str());
	  }
	else
	  {
	    cName = tempnam(sPath.c_str(),m_sMachineType.c_str());
	  }

	string sName = cName; 
	string sMoveCmd = "mv "+sPath+"/9.sta "+sName;
	
	system(sMoveCmd.c_str());

	*sMediaPosition = sName;
	*sText = "Save Game";
	
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void Game_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	cout << "Need to implement command #412 - Set Media Position" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	
	string sPath = GetSaveGamePath();
	string sCopyCmd = "cp "+sMediaPosition+" "+sPath+"/9.sta";
	system(sCopyCmd.c_str());	
	// Send a command to load the game back.
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F7,m_iEventSerialNum++);
	Sleep(100);
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_9,m_iEventSerialNum++);
	Sleep(1000);
	CMD_Pause(iStreamID);	// because the game will just start up from that point.
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage)
//<-dceag-c812-e->
{
	cout << "Need to implement command #812 - Application Exited" << endl;
	cout << "Parm #227 - PID=" << iPID << endl;
	cout << "Parm #228 - Exit_Code=" << iExit_Code << endl;

#ifndef WIN32
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Process exited %d %d", iPID, iExit_Code);

	// void *data;

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

void Game_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	cout << "Need to implement command #920 - Set Media ID" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c943-b->

	/** @brief COMMAND: #943 - Game 1P Start */
	/** 1P start */

void Game_Player::CMD_Game_1P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c943-e->
{
  int iKey;
	cout << "Need to implement command #943 - Game 1P Start" << endl;
	switch(m_iPK_MediaType)
	  {
	  case MEDIATYPE_lmce_Game_CONST:
	  case MEDIATYPE_lmce_Game_snes_CONST:
	  case MEDIATYPE_lmce_Game_genesis_CONST:
          case MEDIATYPE_lmce_Game_megadriv_CONST:
	    iKey = XK_1;
	    break;
	  case MEDIATYPE_lmce_Game_a2600_CONST:
	  case MEDIATYPE_lmce_Game_nes_CONST:
	    iKey = XK_2;
	    break;
	  case MEDIATYPE_lmce_Game_a5200_CONST:
	    iKey = XK_F1;
	    break;
	  case MEDIATYPE_lmce_Game_a7800_CONST:
	    iKey = XK_r;
	    break;
	  case MEDIATYPE_lmce_Game_coleco_CONST:
	  case MEDIATYPE_lmce_Game_intv_CONST:
	    iKey = XK_KP_1;
	    break;
	  }
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iKey,m_iEventSerialNum++);

}

//<-dceag-c944-b->

	/** @brief COMMAND: #944 - Game 2P Start */
	/** 2P Start */

void Game_Player::CMD_Game_2P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c944-e->
{
	cout << "Need to implement command #944 - Game 2P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++);

}

//<-dceag-c945-b->

	/** @brief COMMAND: #945 - Game 3P Start */
	/** 3P Start */

void Game_Player::CMD_Game_3P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c945-e->
{
	cout << "Need to implement command #945 - Game 3P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_3,m_iEventSerialNum++);
}

//<-dceag-c946-b->

	/** @brief COMMAND: #946 - Game 4P Start */
	/** 4P Start */

void Game_Player::CMD_Game_4P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c946-e->
{
	cout << "Need to implement command #946 - Game 4P Start" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_4,m_iEventSerialNum++);

}

//<-dceag-c947-b->

	/** @brief COMMAND: #947 - Game Insert Coin */
	/** Insert Coin */

void Game_Player::CMD_Game_Insert_Coin(string &sCMD_Result,Message *pMessage)
//<-dceag-c947-e->
{
	cout << "Need to implement command #947 - Game Insert Coin" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_5,m_iEventSerialNum++);

}

//<-dceag-c948-b->

	/** @brief COMMAND: #948 - Game Service */
	/** Service Mode */

void Game_Player::CMD_Game_Service(string &sCMD_Result,Message *pMessage)
//<-dceag-c948-e->
{
	cout << "Need to implement command #948 - Game Service" << endl;
	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_F2,m_iEventSerialNum++);

}

//<-dceag-c949-b->

	/** @brief COMMAND: #949 - Game Start */
	/** Game Start */

void Game_Player::CMD_Game_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c949-e->
{
	cout << "Need to implement command #949 - Game Start" << endl;
	switch (m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_nes_CONST:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_2,m_iEventSerialNum++);
			break;
		default:
			WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,XK_1,m_iEventSerialNum++);
			break;
	}
}

//<-dceag-c950-b->

	/** @brief COMMAND: #950 - Game Select */
	/** Game Select */

void Game_Player::CMD_Game_Select(string &sCMD_Result,Message *pMessage)
//<-dceag-c950-e->
{
  int iKey;
  switch (m_iPK_MediaType)
    {
    case MEDIATYPE_lmce_Game_CONST: 
      break;
    case MEDIATYPE_lmce_Game_a2600_CONST:
      iKey = XK_1;
      break;
    case MEDIATYPE_lmce_Game_a7800_CONST:
      iKey = XK_s;
      break;
    case MEDIATYPE_lmce_Game_nes_CONST:
	iKey = XK_6;
	break;
    }

  WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iKey,m_iEventSerialNum++);

}

//<-dceag-c951-b->

	/** @brief COMMAND: #951 - Game Option */
	/** Game Option */

void Game_Player::CMD_Game_Option(string &sCMD_Result,Message *pMessage)
//<-dceag-c951-e->
{
	cout << "Need to implement command #951 - Game Option" << endl;
}

//<-dceag-c942-b->

	/** @brief COMMAND: #942 - Get Ripping Status */
	/** Tell Game to Start 1 Player */
		/** @param #199 Status */
			/** Ripping status */

void Game_Player::CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage)
//<-dceag-c942-e->
{
}
//<-dceag-c982-b->

	/** @brief COMMAND: #982 - Set Game Options */
	/** Set Options for the running Game System driver. */
		/** @param #5 Value To Assign */
			/** Dependent on driver, but usually a single line in the format of key,value */

void Game_Player::CMD_Set_Game_Options(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c982-e->
{
}
//<-dceag-c983-b->

	/** @brief COMMAND: #983 - Get Game Options */
	/** Get Options for the currently running driver. */
		/** @param #5 Value To Assign */
			/** The Returned value. */
		/** @param #219 Path */
			/** The Parameter to return, usually left side of comma in Set Game Options. */

void Game_Player::CMD_Get_Game_Options(string sPath,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c983-e->
{
}
//<-dceag-c952-b->

	/** @brief COMMAND: #952 - Game Reset */
	/** Game Reset */

void Game_Player::CMD_Game_Reset(string &sCMD_Result,Message *pMessage)
//<-dceag-c952-e->
{
	int iKey;

	switch (m_iPK_MediaType)
	{
		case MEDIATYPE_lmce_Game_CONST:
			break;
		case MEDIATYPE_lmce_Game_a2600_CONST:
			iKey = XK_1;
			break;
		case MEDIATYPE_lmce_Game_a7800_CONST:
			iKey = XK_R;
			break;
		default:
			iKey = XK_F3;
			break;
	}

	WindowUtils::SendKeyToWindow(m_pDisplay,m_iMAMEWindowId,iKey,m_iEventSerialNum++);
}
