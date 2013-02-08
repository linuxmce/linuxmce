/*
    MythTV_Player

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/


/**
 *
 * @file MythTV_Player.cpp
 * @brief source file for the MythTV_Player class
 *
 */

 //<-dceag-d-b->
#include "MythTV_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Button.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "DCE/DCEConfig.h"
#include "Gen_Devices/AllScreens.h"

#include "WindowUtils/WindowUtils.h"
#include <math.h>

#include <sstream>
// #include <qsqldatabase.h>
//
// #include <libmythtv/programinfo.h>
// #include <libmythtv/remoteencoder.h>
// #include <libmythtv/remoteutil.h>

#include <pthread.h>
// #include "MythMainWindowResizable.h"
MythTV_Player *g_pMythPlayer = NULL;
#define MYTH_SOCKET_TIMEOUT	10  // SECONDS
#define PLUTO_CONFIG "/etc/pluto.conf"

#ifndef WIN32

#include <sys/wait.h>


#define MYTH_WINDOW_NAME "mythfrontend"

#endif

void* monitorMythThread(void* param)
{
	while(g_pMythPlayer->m_bExiting==false)
	{
		if( g_pMythPlayer->m_mythStatus_get() == MYTHSTATUS_DISCONNECTED )
			Sleep(500);
		else
		{
			g_pMythPlayer->pollMythStatus();
			Sleep(5000);
		}
	}
	return NULL;
}

// MythContext *gContext;

/* VideoFrameGeometry Stuff */
VideoFrameGeometry::VideoFrameGeometry (int iWidth, int iHeight)
{
  m_iWidth = iWidth;
  m_iHeight = iHeight;
}

VideoFrameGeometry::~VideoFrameGeometry ()
{
  // Does nothing.
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_Player::MythTV_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_MythMutex("myth"), m_pMythSocket(NULL)
{
	m_MythMutex.Init(NULL);
	g_pMythPlayer = this;
	m_pDevice_MythTV_Plugin=NULL;
    m_iMythFrontendWindowId = 0;
    m_bExiting = false;
    m_threadMonitorMyth = 0;
    m_pDisplay = NULL;
    m_mythStatus = MYTHSTATUS_DISCONNECTED;
    m_CurTime=0;
    m_EndTime=0;
	m_iStreamID=0;
    m_sMediaURL="";
}

//<-dceag-getconfig-b->
bool MythTV_Player::GetConfig()
{
	if( !MythTV_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	string sHostname = "dcerouter";
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	DeviceData_Base *pDevice_Core = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_Generic_PC_as_Core_CONST);
	string sPK_Device = pConfig->m_mapParameters_Find("PK_Device");
	if ( sPK_Device != StringUtils::itos(pDevice_Core->m_dwPK_Device) )
		sHostname = "moon" + sPK_Device;

	DBHelper *pDBHelper_Myth = new DBHelper(
		pConfig->m_mapParameters_Find("MySqlHost"),
		pConfig->m_mapParameters_Find("MySqlUser"),
		pConfig->m_mapParameters_Find("MySqlPassword"),
		"mythconverg");

	UpdateMythSetting(pDBHelper_Myth, "NetworkControlEnabled", "1",     sHostname);
	UpdateMythSetting(pDBHelper_Myth, "NetworkControlPort",    "10001", sHostname);

	delete pDBHelper_Myth;
	delete pConfig;

	m_pDevice_MythTV_Plugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_MythTV_PlugIn_CONST);
	if( !m_pDevice_MythTV_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I need a myth plugin to function");
		return false;
	}
	// Kill any existing myth front ends.   The application spawner / window focus isn't happy
	// if we didn't create it ourselves.

	EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not
	
 	system("killall mythfrontend");
	Sleep(500);
 	pthread_create(&m_threadMonitorMyth, NULL, monitorMythThread, NULL);
	
	m_pDisplay = XOpenDisplay(getenv("DISPLAY")); 

	bool bStarted=false;
	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/bin/screen","restart_myth","-d\t-m\t-S\tRestart_Myth_Backend\t/usr/pluto/bin/Restart_MythBackend.sh","","",false,false,false,false);
		if( SendCommand(CMD_Spawn_Application) )
			bStarted=true;
	}
	if( !bStarted )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Player::GetConfig cannot find app server");
 
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MythTV_Player::~MythTV_Player()
//<-dceag-dest-e->
{
	StopMythFrontend();
	m_bExiting = true;
	if (m_threadMonitorMyth != 0)
		pthread_join(m_threadMonitorMyth, NULL);

	delete m_pMythSocket;
	m_pMythSocket = NULL;

 	system("killall mythfrontend");  // Be sure there's no front end running

	if (m_pDisplay)
		XCloseDisplay(m_pDisplay);

	// Delete the video frame geometry map.
	for (map < int, VideoFrameGeometry * >::iterator it =
	       m_mapVideoFrameGeometry.begin ();
	     it != m_mapVideoFrameGeometry.end (); it++)
	  {
	    delete (*it).second;
	  }

}

bool MythTV_Player::LaunchMythFrontend(bool bSelectWindow)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_Player::LaunchMythFrontend - ready to launch!");

	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		string sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " 1 " 
			TOSTRING(COMMAND_Application_Exited_CONST) " " 
			TOSTRING(COMMANDPARAMETER_Exit_Code_CONST) " ";

		string sWidth = m_pEvent->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_ControlledVia, DEVICEDATA_ScreenWidth_CONST);
		string sHeight = m_pEvent->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_ControlledVia, DEVICEDATA_ScreenHeight_CONST); 

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/bin/mythfrontend", "mythfrontend", "-w\t-geometry\t" + sWidth + "x" + sHeight,
			sMessage + "1",sMessage + "0",false,false,true,false);
		if( SendCommand(CMD_Spawn_Application) )
		 	return true;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Player::LaunchMythFrontend - failed to launch");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Player::LaunchMythFrontend - no app server");
	return false;
 }

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_Player::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

void MythTV_Player::CreateChildren()
{
}
/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void MythTV_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->Window
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void MythTV_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

bool MythTV_Player::UpdateMythSetting(DBHelper *pDBHelper_Myth, string value, string data, string hostname, bool bOnlyIfNotExisting)
{
        string sSQL = "SELECT data FROM settings WHERE value='" + StringUtils::SQLEscape(value) + "' AND " 
                " hostname='" + StringUtils::SQLEscape(hostname) + "'";
        PlutoSqlResult result;
        if( (result.r = pDBHelper_Myth->db_wrapper_query_result(sSQL))==NULL || result.r->row_count==0 )
        {
                sSQL = "INSERT INTO settings(value,hostname,data) VALUES('" + StringUtils::SQLEscape(value) + "',"
                        + "'" + StringUtils::SQLEscape(hostname) + "',"
                        + "'" + StringUtils::SQLEscape(data) + "')";
                pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
                return true;
        }
        else if( bOnlyIfNotExisting )
                return false;

        DB_ROW row;
        row = db_wrapper_fetch_row( result.r ) ;
        if( row[0] && (data==row[0]) )
                return false;

        sSQL = "UPDATE settings set data='" + StringUtils::SQLEscape(data) + "' WHERE value='" + StringUtils::SQLEscape(value) + "' "
                " AND hostname='" + StringUtils::SQLEscape(hostname) + "'";
        pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
        return true;

}

void MythTV_Player::updateMode(string toMode)
{
	if (m_sMediaURL.empty()) 
	{
	  if (toMode!=m_CurrentMode)
	  {				
		  m_CurrentMode = toMode;
		  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Changing mode: %s",toMode.c_str());
		  DCE::CMD_Set_Active_Menu CMD_Set_Active_Menu_(m_dwPK_Device,m_pDevice_MythTV_Plugin->m_dwPK_Device, toMode);
		  SendCommand(CMD_Set_Active_Menu_);
	  }
	}
}

/** 
 * Given a MediaURL which is typically of the form:
 * /home/public/data/videos/tv_shows_1/cccc_yyyymmddhhmmss.mpg
 * do some simple text formatting on it to provide something that
 * can be passed back to a "play program" control port command
 * so that stuff can be played.
*/
string MythTV_Player::parseMediaURL(string sMediaURL) 
{

  string sMediaFilename = FileUtils::FilenameWithoutPath(sMediaURL);

  string sChannelID, sYear, sMonth, sDay, sHour, sMinute, sSeconds;

  // Double defensive measures
  if (sMediaFilename.length() == 20)
  {
    sChannelID 	= sMediaFilename.substr( 0,4 );
    sYear 	= sMediaFilename.substr( 5,4 );
    sMonth	= sMediaFilename.substr( 9,2 );
    sDay	= sMediaFilename.substr(11,2 );
    sHour	= sMediaFilename.substr(13,2 );
    sMinute	= sMediaFilename.substr(15,2 );
    sSeconds	= sMediaFilename.substr(17,2 );

  return sChannelID + " " + sYear + "-" + sMonth + "-" +sDay + "T" + sHour + ":" + sMinute + ":" + sSeconds;
  }
  else
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Malformed Media URL: %s",sMediaURL.c_str());
    return "";
  }

}

string MythTV_Player::getChannelCommandForMediaURL(string sMediaURL)
{
	string sCommand, sChan, sRet; // The final fragments for the command.

	// all channel URLs must begin with CHAN:
	if (sMediaURL.find("CHAN") != string::npos)
	{
		// Okay, let's deal with it.
		// anything beginning with CHAN:i is a chanID.
		if (sMediaURL.find("CHAN:i") != string::npos)
		{
			sCommand = "play chanid"; // CHAN:i1806 a channel ID
			sChan = sMediaURL.substr(6);
		}

		else if (sMediaURL.find("CHAN:") != string::npos) // CHAN:4_1 a channel number
		{
			sCommand = "play channel";
			sChan = sMediaURL.substr(5);
		}

		else 
		{
			// Not correctly parseable, just return empty.
			sCommand = "";
			sChan = "";
		}

		if (!sCommand.empty() && !sChan.empty())
		{
			sRet = sCommand + " " + sChan;
		}
		else
		{
			sRet = "";
		}

	}
	else
	{
		// Double insurance.
		sRet = "";
	}

	return sRet;

}

void MythTV_Player::pollMythStatus()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_Player::pollMythStatus %d",m_mythStatus_get());
#endif
	if (m_mythStatus_get() == MYTHSTATUS_STARTUP)
	{
		PLUTO_SAFETY_LOCK(mm,m_MythMutex);

		LaunchMythFrontend();
		//locateMythTvFrontendWindow(DefaultRootWindow(getDisplay()));
		m_CurrentMode.clear();
		m_CurrentProgram.clear();
		
		// TODO: The controllers don't get updated if I set this there. Why?
		updateMode("live");
	
		string sResult;
		time_t timeout=60+time(NULL);
		bool bPlaybackStarted=false,bCommunication=false;
		do
		{
			if( g_pMythPlayer->m_bExiting==true )
				return;

			if( m_mythStatus_get() != MYTHSTATUS_STARTUP )
			{
			    LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_Player::pollMythStatus state is now %d", (int) m_mythStatus_get());
				return;
			}

		    mm.Release();
		    Sleep(100);
		    mm.Relock();
		    if (!m_sMediaURL.empty() && (m_sMediaURL.find("CHAN:") == string::npos) && FileUtils::FileExists(m_sMediaURL) ) // A MediaURL was passed into MediaHandler.
		    {
		      string sCommand = "play program " + parseMediaURL(m_sMediaURL);
		      sResult = sendMythCommand(sCommand.c_str());
		      if( sResult!="OK" ) 
		      {
			// FIXME: Come back here and implement some error handling.
		      }
		      else
			bCommunication=true; // Connected.
			mm.Release();
			Sleep(3000);
			mm.Relock();
			string sResult = sendMythCommand("query location");
			if ( sResult.find("Playback")!=string::npos && sResult.find("ERROR")==string::npos )
			{
			  bPlaybackStarted=true;
			  LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_Player::pollMythStatus started playing from URL: %s",
			  sResult.c_str());
			  break;
			} 
			else 
			{
			  LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_Player::pollMythStatus no playback yet, reported : %s", sResult.c_str());
			  mm.Release();
			  Sleep(3000); // Give it a longer time
			  mm.Relock();
			  continue;			
			}
		    }
		    else // TV Was launched without MediaURL, just start LiveTV.
		    {
		    sResult = sendMythCommand("jump livetv");
		    if( sResult!="OK" )
			{
			    LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_Player::pollMythStatus no jump livetv %s", sResult.c_str());
				continue;
			}
			else
				bCommunication=true;  // We at least have communication

			mm.Release();
            Sleep(3000);
			mm.Relock();

			// This is a special case to check for sMediaURL's of CHAN:xxxx, if so,
			// Tune the channel. 
			if (!m_sMediaURL.empty() && (m_sMediaURL.find("CHAN:") != string::npos))
			{
				string sChannelCommand = getChannelCommandForMediaURL(m_sMediaURL);
				if (!sChannelCommand.empty())
				{
					sendMythCommand(sChannelCommand.c_str());
				}
			}

			string sResult = sendMythCommand("query location");
			if( sResult.find("Playback")!=string::npos && sResult.find("ERROR")==string::npos )
			{
				bPlaybackStarted=true;
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_Player::pollMythStatus started playing %s",sResult.c_str());
				break;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_Player::pollMythStatus no playback yet, reported : %s", sResult.c_str());
				mm.Release();
				Sleep(3000); // Give it a longer time
				mm.Relock();
				continue;
			}
			}
		} while(time(NULL) < timeout);

		if( m_mythStatus_get() != MYTHSTATUS_STARTUP )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_Player::pollMythStatus state is now %d", (int) m_mythStatus_get());
			return;
		}

		if (bCommunication==false)
		{
			DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,0,0,"",false);
			SendCommand(CMD_MH_Stop_Media_Cat);
			m_mythStatus_set(MYTHSTATUS_DISCONNECTED);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed initial communications with Mythfrontend.");
			StopMythFrontend();
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Killed Mythfrontend.");
		}
		else
			m_mythStatus_set(MYTHSTATUS_LIVETV);

		if( bPlaybackStarted==false && m_pDevice_MythTV_Plugin )
		{
			string sMessage;
			DeviceData_Base *pDevice_App_Server_Local = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
			if( pDevice_App_Server_Local )
				sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pDevice_App_Server_Local->m_dwPK_Device) + " "
					+ StringUtils::itos(MESSAGETYPE_COMMAND) + " " TOSTRING(COMMAND_Spawn_Application_CONST) " "
					TOSTRING(COMMANDPARAMETER_Name_CONST) " killmyth " 
					TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"/usr/pluto/bin/ForciblyKillProcess.sh\" "
					TOSTRING(COMMANDPARAMETER_Arguments_CONST) " mythbackend";

			DeviceData_Base *pDevice_App_Server_Core = m_pDevice_MythTV_Plugin->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
			if( pDevice_App_Server_Core && pDevice_App_Server_Core!=pDevice_App_Server_Local )
			{
				if( sMessage.empty()==false )
					sMessage += " & ";

				sMessage += StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pDevice_App_Server_Core->m_dwPK_Device) + " "
					+ StringUtils::itos(MESSAGETYPE_COMMAND) + " " TOSTRING(COMMAND_Spawn_Application_CONST) " "
					TOSTRING(COMMANDPARAMETER_Name_CONST) " killmyth " 
					TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"/usr/pluto/bin/ForciblyKillProcess.sh\" "
					TOSTRING(COMMANDPARAMETER_Arguments_CONST) " mythbackend";
			}

			DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device, m_pData->m_dwPK_Device_ControlledVia,
				"MythTV is having problems.  Reset the backend?|Yes|No", // Main message
				sMessage, // Command Line
				"myth is dead again", // Description
				"0", // sPromptToResetRouter
				"300", // sTimeout
				"1"); // sCannotGoBack
			SendCommand(SCREEN_PopupMessage);

		}
	} 
	else if (m_mythStatus_get() != MYTHSTATUS_DISCONNECTED)
	{
		PLUTO_SAFETY_LOCK(mm2,m_MythMutex);
		
		string sResult = sendMythCommand("query location");
		
		WindowUtils::FindWindowMatching(m_pDisplay, "MythTV Frontend",m_iMythFrontendWindowId);
		
		if (m_iMythFrontendWindowId != 0)
		  {
		    WindowUtils::GetWindowGeometry(m_pDisplay,
						   m_iMythFrontendWindowId,
						   m_iNowPlayingX,
						   m_iNowPlayingY,
						   m_iNowPlayingW,
						   m_iNowPlayingH);
		    
		  }	
		
		if(sResult.length())
		{
			// We're up and running, if there's an initial channel to tune to do it now
			if( m_sInitialChannel.empty()==false )
			{
				mm2.Release();
				CMD_Tune_to_channel("",m_sInitialChannel);
				m_sInitialChannel="";
				mm2.Relock();
			}

			vector<string> vectResults;
			StringUtils::Tokenize(sResult, " ", vectResults);

			string SetMode = vectResults[0];
			if (SetMode == "Playback")
			{
				m_mythStatus_set(MYTHSTATUS_PLAYBACK);
				if (vectResults[1]=="LiveTV")
				{
					m_CurTime = StringUtils::TimeAsSeconds(vectResults[2]);
					m_EndTime = StringUtils::TimeAsSeconds(vectResults[4]);
					if( m_sChannel != vectResults[6] )
					  {
					    m_sChannel = vectResults[6];
					    EVENT_Playback_Started(m_sChannel,m_iStreamID,"","","ch_" + m_sChannel);
					  }
					
					// Have a 2 second "buffer" for switching between live and nonlive modes so we don't get 
					// flapping.    Unfortunately it takes live TV so long to start up at times that we need
					// to show up to 8 seconds or so as live. 
					
					if (m_CurrentMode == "live")
					{
						if (m_CurTime < m_EndTime - 10)
							SetMode = "nonlive";
						else
							SetMode = "live";
					} 
					else
					{
						if (m_CurTime < m_EndTime - 8)
							SetMode = "nonlive";
						else
							SetMode = "live";
					}
				}
				else
					SetMode="nonlive";
			}
			else
			{
				if (SetMode == "GuideGrid")
					m_mythStatus_set(MYTHSTATUS_GUIDEGRID);
				else
					m_mythStatus_set(MYTHSTATUS_MENU);
			}
			updateMode(SetMode);	
		}
	}
}

void MythTV_Player::selectWindow()
{
}

void MythTV_Player::processKeyBoardInputRequest(int iXKeySym)
{
#ifndef WIN32
    Window oldWindow;
    int oldRevertBehaviour;

	selectWindow();

	XGetInputFocus( getDisplay(), &oldWindow, &oldRevertBehaviour);
    XSetInputFocus( getDisplay(), (Window)m_iMythFrontendWindowId, RevertToParent, CurrentTime );
    XTestFakeKeyEvent( getDisplay(), XKeysymToKeycode(getDisplay(), iXKeySym), True, 0 );
    XTestFakeKeyEvent( getDisplay(), XKeysymToKeycode(getDisplay(), iXKeySym), False, 0 );
    if ( oldWindow )
        XSetInputFocus( getDisplay(), oldWindow, oldRevertBehaviour, CurrentTime );

    XFlush(getDisplay());
#endif
}

string MythTV_Player::sendMythCommand(const char *Cmd)
{
	string sResponse;
	char ch=0;

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Going to send command %s",Cmd);
	
	if (m_pMythSocket == NULL)
	{
		m_pMythSocket = new PlainClientSocket("localhost:10001");
		if( !m_pMythSocket->Connect() )
		{
			delete m_pMythSocket;
			m_pMythSocket = NULL;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to connect to MythTV client");
			return "";
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connected");
	}
	

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== Before mythtv prompt");
	// Wait for Myth's console prompt.
	do
	{
		if ( !m_pMythSocket->ReceiveData( 1, &ch, MYTH_SOCKET_TIMEOUT ) ) 
		{
/*			delete m_pMythSocket;
			m_pMythSocket = NULL;*/
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Timeout waiting for Myth prompt: socket %d, buffer %c", m_pMythSocket->m_Socket, ch);
			return "";
		}
	} while(ch!='#');
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== After mythtv promt");
	if( !m_pMythSocket->SendString(Cmd) )
	{
		delete m_pMythSocket;
		m_pMythSocket = NULL;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send string");
		return "";
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== Command is sent");
	// Receive the response
	do
	{
		if( !m_pMythSocket->ReceiveString(sResponse,MYTH_SOCKET_TIMEOUT) )
		{
/*			delete m_pMythSocket;
			m_pMythSocket = NULL;*/
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Didn't get reply.");
			return "";
		}
	// FIXIT: In Myth's networkcontrol.cpp, it's possible that Myth replies to a previous command with a "lost" query location.  If we get "OK" when asking for status,
	// we're off kilter.  Receive one more line.
	} while (!strcmp(Cmd, "query location") && sResponse=="OK");
	
	
	if( sResponse.find("ERROR")!=string::npos )
	{
		m_pMythSocket->SendString("key enter");
		if( !m_pMythSocket->ReceiveString(sResponse,MYTH_SOCKET_TIMEOUT) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Didn't get reply.");
			return "";
		}
	}
		
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== Got the answer: %s", sResponse.c_str());
		
	sResponse = StringUtils::TrimSpaces(sResponse);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Myth Responded: %s",sResponse.c_str());
	
/*	if( !m_pMythSocket->SendString("QUIT") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send QUIT string");
		return sResponse;
	}*/
	
//	m_pMythSocket->Close();
	
	return sResponse;
}

bool MythTV_Player::checkWindowName(long unsigned int window, string windowName)
{
#ifndef WIN32
/*	XTextProperty text;

	if ( ! checkXServerConnection())
		return false;

	if ( XGetWMName (getDisplay(), window, &text) && windowName == string((const char*)text.value) ) */
        return true;
#endif
    return false;
}

bool MythTV_Player::locateMythTvFrontendWindow(long unsigned int window)
{
/*
#ifndef WIN32
    Window parent_win, root_win, *child_windows;
    unsigned int num_child_windows;


    if ( checkWindowName(window, MYTH_WINDOW_NAME ) )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found window id: 0x%x", window );
        m_iMythFrontendWindowId = window;
        return true;
    }

    XQueryTree(getDisplay(), (Window)window, &root_win, &parent_win, &child_windows, &num_child_windows);

    for ( unsigned int i = 0; i < num_child_windows; i++ )
        if ( locateMythTvFrontendWindow(child_windows[i]) )
            return true;

     we need to free the list of child IDs, as it was dynamically allocated 
     by the XQueryTree function.                                            
    XFree(child_windows);
#endif*/
    return true;
}

/*

 COMMANDS TO IMPLEMENT

*/

//<-dceag-c187-b->

	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #39 Options */
			/** Extra data to allow the receiver to properly identify the channel ( this is the xmltvid value inside the mythtv database). */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

void MythTV_Player::CMD_Tune_to_channel(string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c187-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	vector<string> numbers;
	StringUtils::Tokenize( sProgramID, "|", numbers );

	if ( numbers.size() == 0 )	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Invalid channel specification: \"%d\"", sProgramID.c_str());
		return;
	}

	string sTuneCMD;
	if( numbers[0].size()>1 && numbers[0][0]=='i' )
		sTuneCMD = string("play chanid ")+numbers[0].substr(1);
	else
		sTuneCMD = string("play chanid ")+numbers[0];

	sendMythCommand(sTuneCMD.c_str());
}

//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Capture a Video frame */
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

void MythTV_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{

  PLUTO_SAFETY_LOCK(mm,m_MythMutex);

  if (m_iMythFrontendWindowId != 0)
    {
      string sPath, screenName, sGeometry;
      stringstream ssWidth, ssHeight;
      
      ssWidth << iWidth;
      ssHeight << iHeight;
      sGeometry = ssWidth.str () + "x" + ssHeight.str ();
      
      if (m_mapVideoFrameGeometry_Exists(pMessage->m_dwPK_Device_From))
	{
	  VideoFrameGeometry *vfg = m_mapVideoFrameGeometry_Find(pMessage->m_dwPK_Device_From);
	  delete vfg;
	  m_mapVideoFrameGeometry.erase(pMessage->m_dwPK_Device_From);
	}
      
      m_mapVideoFrameGeometry[pMessage->m_dwPK_Device_From] =
	new VideoFrameGeometry (iWidth, iHeight);
      
      // Use alternate code to display OSD
      sPath = "/tmp/";
      screenName = "OSD";
      
      // use ImageMagick. Awfully slow!
      string cmd =
	"scrot -u -t " + sGeometry + " " + sPath + screenName + ".jpg";
      system (cmd.c_str ());
      cmd =
	"mv " + sPath + screenName + "-thumb.jpg " + sPath + screenName +
	".jpg";
      system (cmd.c_str ());
      *sFormat = "2";		// JPEG.

      size_t size;

      *pData =
	FileUtils::ReadFileIntoBuffer (sPath + screenName + ".jpg", size);
      
      *iData_Size = size;
      
      FileUtils::DelFile (sPath + screenName + ".png");
      FileUtils::DelFile (sPath + screenName + ".jpg");
      
      return;

    }
}

//<-dceag-c129-b->

	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

void MythTV_Player::CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c129-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	CMD_Move_Up(0, sCMD_Result, pMessage);
}

//<-dceag-c130-b->

	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

void MythTV_Player::CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c130-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	CMD_Move_Down(0, sCMD_Result, pMessage);
}
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key enter");
//	processKeyBoardInputRequest(XK_Return);
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key up");
//	processKeyBoardInputRequest(XK_Up);	
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key down");
//	processKeyBoardInputRequest(XK_Down);
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key left");
//	processKeyBoardInputRequest(XK_Left);

}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key right");
//	processKeyBoardInputRequest(XK_Right);
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void MythTV_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 0");
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void MythTV_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 1");
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void MythTV_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 2");
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void MythTV_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 3");
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void MythTV_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 4");
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void MythTV_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 5");
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void MythTV_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 6");
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void MythTV_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 7");
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void MythTV_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 8");
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void MythTV_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("key 9");
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
#ifndef WIN32
	sendMythCommand("key escape");
#endif
}
//<-dceag-createinst-b->!
//<-dceag-sample-b->!

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

void MythTV_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	
	// If a mythTV stream is already present, then we need to just tell the system to play the next bit of media.
	if (!sMediaURL.empty() && ( m_mythStatus_get() != MYTHSTATUS_DISCONNECTED ))
	{
	  if (m_sMediaURL != sMediaURL) 
	  { 
	    m_sMediaURL = sMediaURL;
	    if (sMediaURL.find("CHAN:") != string::npos )
		{
			// CHAN: URL detected, do a channel flip instead.
			sendMythCommand("jump livetv");
			string sCommand = getChannelCommandForMediaURL(sMediaURL);
			if (!sCommand.empty())
			{
				sendMythCommand(sCommand.c_str());
			}
		}
		else
		{
	    		string sCommand = "play program " + parseMediaURL(m_sMediaURL);
	    		sendMythCommand(sCommand.c_str());
		}
	  }
	}
	else // A MythTV Stream was not already present. 
	{

	  if ( m_mythStatus_get() == MYTHSTATUS_DISCONNECTED )
	  {
		  m_sMediaURL = sMediaURL;
		  m_mythStatus_set(MYTHSTATUS_STARTUP);
	  }
	  else
	  {
		  if (m_mythStatus_get() == MYTHSTATUS_LIVETV || m_mythStatus_get() == MYTHSTATUS_PLAYBACK)
			  sendMythCommand("play speed normal");
		  else
			  sendMythCommand("key P");
	  }
  
	  selectWindow();
  
	  // Set the initial channel to tune to after startup
	  m_sInitialChannel = sMediaPosition;
	  m_iStreamID=iStreamID;
	}
#endif
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void MythTV_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	*sMediaPosition = m_sChannel;

	m_mythStatus_set(MYTHSTATUS_DISCONNECTED);

	sendMythCommand("quit");

	Sleep(750);
	StopMythFrontend();
	Sleep(1000);

	delete m_pMythSocket;
	m_pMythSocket = NULL;
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void MythTV_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	sendMythCommand("play speed pause");
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void MythTV_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	sendMythCommand("play speed normal");
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

void MythTV_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	// We send left/right for this
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	string Direction, Speed;

	switch(iMediaPlaybackSpeed)
	{
	case 0:
		sendMythCommand("play speed pause");
		break;
	case 1:
	case 1000:
		sendMythCommand("play speed normal");
		break;
	case 2:
	case 2000:
		sendMythCommand("play speed 2x");
		break;
	case -1:
	case -1000:
		// Myth cannot do -1x with hdtv.
		sendMythCommand("play speed -2x");
		break;
	case -2:
	case -2000:
		sendMythCommand("play speed -2x");
		break;
	default:
		if (iMediaPlaybackSpeed < 0)
		{
			Direction="-";
			iMediaPlaybackSpeed = -iMediaPlaybackSpeed;
		}
		if(iMediaPlaybackSpeed < 1000)
		{
			Speed = "1/"+StringUtils::itos(1000/iMediaPlaybackSpeed);
		}
		else
		{
			Speed = StringUtils::itos(iMediaPlaybackSpeed / 1000);
		}
		sendMythCommand(StringUtils::Format("play speed %s%sx", Direction.c_str(), Speed.c_str()).c_str());
		break;
	}

//	if( iMediaPlaybackSpeed==2 )
//		processKeyBoardInputRequest(XK_Right);
//	else
//		processKeyBoardInputRequest(XK_Left);
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void MythTV_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
	
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if (atoi(sValue_To_Assign.c_str()) < 0) 
		sendMythCommand("play seek rewind");
	else
		sendMythCommand("play seek forward");
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
#ifndef WIN32
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if( sValue_To_Assign.size()==0 || sValue_To_Assign[0]=='+')
	{
		if (m_mythStatus_get() == MYTHSTATUS_PLAYBACK)
			sendMythCommand("play channel up");
		else
			sendMythCommand("key pageup");
	}
	else
	{
		if (m_mythStatus_get() == MYTHSTATUS_PLAYBACK)
			sendMythCommand("play channel down");
		else
			sendMythCommand("key pagedown");
	}
#endif
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

void MythTV_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if (m_mythStatus_get() == MYTHSTATUS_PLAYBACK)
	{
		*sText="325 HBO\tSopranos";
		*sMediaPosition=StringUtils::Format(" POS:%d TOTAL:%d PROG:SH8196770000 SERIES:sh019875 CHAN:1325", m_CurTime, m_EndTime);
	}
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void MythTV_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
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

void MythTV_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	switch(atoi(sPK_Button.c_str()))
	{
	case BUTTON_Up_Arrow_CONST:
		sendMythCommand("key up");
		break;		
	case BUTTON_Down_Arrow_CONST:
		sendMythCommand("key down");
		break;	
	case BUTTON_Left_Arrow_CONST:
		sendMythCommand("key left");
		break;	
	case BUTTON_Right_Arrow_CONST:
		sendMythCommand("key right");
		break;	
	case BUTTON_Scroll_Up_CONST:
		sendMythCommand("key pageup");
		break;
	case BUTTON_Scroll_Down_CONST:
		sendMythCommand("key pagedown");
		break;
	case BUTTON_Enter_CONST:
		sendMythCommand("key enter");
		break;	
	case BUTTON_0_CONST:
		sendMythCommand("key 0");
		break;	
	case BUTTON_1_CONST:
		sendMythCommand("key 1");
		break;	
	case BUTTON_2_CONST:
		sendMythCommand("key 2");
		break;	
	case BUTTON_3_CONST:
		sendMythCommand("key 3");
		break;	
	case BUTTON_4_CONST:
		sendMythCommand("key 4");
		break;	
	case BUTTON_5_CONST:
		sendMythCommand("key 5");
		break;	
	case BUTTON_6_CONST:
		sendMythCommand("key 6");
		break;	
	case BUTTON_7_CONST:
		sendMythCommand("key 7");
		break;	
	case BUTTON_8_CONST:
		sendMythCommand("key 8");
		break;	
	case BUTTON_9_CONST:
		sendMythCommand("key 9");
		break;
	case BUTTON_d_CONST:
	  sendMythCommand("key d");
	  break;
	case BUTTON_i_CONST:
	  sendMythCommand("key i");
	  break;
	case BUTTON_Back_CONST:
		sendMythCommand("key back");
		sendMythCommand("key backspace");
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

void MythTV_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{

  PLUTO_SAFETY_LOCK(mm,m_MythMutex);
  
  if (m_iMythFrontendWindowId != 0)
    {
      VideoFrameGeometry *vfg =
	m_mapVideoFrameGeometry[pMessage->m_dwPK_Device_From];
      double iScaleW = (double) m_iNowPlayingW / (double) vfg->m_iWidth;
      double iScaleH = (double) m_iNowPlayingH / (double) vfg->m_iHeight;
      int iScaledX = floor ((double) iPosition_X * iScaleW);
      int iScaledY = floor ((double) iPosition_Y * iScaleH);
      
      cout << "iScaleW = " << iScaleW << " : iScaleH = " << iScaleH << endl;
      cout << "iScaledX = " << iScaledX << " : iScaledY = " << iScaledY << endl;
      
      WindowUtils::SendClickToWindow (m_pDisplay, m_iMythFrontendWindowId, 1, iScaledX,
				      iScaledY);
    }
}
//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MythTV_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
}
//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MythTV_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
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

void MythTV_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
}
//<-dceag-c123-b->

	/** @brief COMMAND: #123 - Info */
	/** Info about the currently playing program */
		/** @param #9 Text */
			/** nimic */

void MythTV_Player::CMD_Info(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c123-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
#ifndef WIN32
	sendMythCommand("key I");
#endif
}
//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void MythTV_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
	updateMode("GuideGrid");	

	if (m_mythStatus_get() == MYTHSTATUS_PLAYBACK)
		sendMythCommand("key S");
	else
		sendMythCommand("jump guidegrid");

	LockOrbiterPointer();
}
//<-dceag-c367-b->

	/** @brief COMMAND: #367 - Text */
	/** Text */

void MythTV_Player::CMD_Text(string &sCMD_Result,Message *pMessage)
//<-dceag-c367-e->
{
}
//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	if( sText=="old_recordings" )
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump playbackrecordings");
	}
	else if( sText=="future_recordings" )
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump managerecordings");
	}
	else if( sText=="livetv" )
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump livetv");
	}
	else
	{
		updateMode("PlaybackBox");
		sendMythCommand("jump mainmenu");
	}
}

//<-dceag-c762-b->

	/** @brief COMMAND: #762 - Live TV */
	/** Go to Live TV */

void MythTV_Player::CMD_Live_TV(string &sCMD_Result,Message *pMessage)
//<-dceag-c762-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);

	updateMode("live");
	sendMythCommand("jump livetv");
}
//<-dceag-c763-b->

	/** @brief COMMAND: #763 - Exit */
	/** Exit guide */

void MythTV_Player::CMD_Exit(string &sCMD_Result,Message *pMessage)
//<-dceag-c763-e->
{
	PLUTO_SAFETY_LOCK(mm,m_MythMutex);
#ifndef WIN32
	sendMythCommand("key escape");
#endif
}

//<-dceag-c102-b->

	/** @brief COMMAND: #102 - Record */
	/** Record the current show */

void MythTV_Player::CMD_Record(string &sCMD_Result,Message *pMessage)
//<-dceag-c102-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
	sendMythCommand("key R");
}

//<-dceag-c777-b->

	/** @brief COMMAND: #777 - Recorded TV Menu */
	/** go to the recorded tv menu */

void MythTV_Player::CMD_Recorded_TV_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c777-e->
{
	//TODO: implement me!
}

void MythTV_Player::LockOrbiterPointer()
{
	DCE::CMD_Set_Mouse_Behavior CMD_Set_Mouse_Behavior(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","K",false,"X");
	DCE::CMD_Set_Mouse_Behavior CMD_Set_Mouse_Behavior2(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","K",false,"Y");
	DCE::CMD_Remove_Popup CMD_Remove_Popup(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","horiz");
	DCE::CMD_Remove_Popup CMD_Remove_Popup2(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","left");

	CMD_Set_Mouse_Behavior.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Mouse_Behavior2.m_pMessage);
	CMD_Set_Mouse_Behavior.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Popup.m_pMessage);
	CMD_Set_Mouse_Behavior.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Popup2.m_pMessage);
	SendCommand(CMD_Set_Mouse_Behavior);
}

void MythTV_Player::ReleaseOrbiterPointer()
{
	DCE::CMD_Remove_Popup CMD_Remove_Popup(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","horiz");
	DCE::CMD_Remove_Popup CMD_Remove_Popup2(m_dwPK_Device,m_pData->m_dwPK_Device_ControlledVia,"","left");

	CMD_Remove_Popup.m_pMessage->m_vectExtraMessages.push_back(CMD_Remove_Popup2.m_pMessage);
	SendCommand(CMD_Remove_Popup);
}

//<-dceag-c91-b->

	/** @brief COMMAND: #91 - Input Select */
	/** toggle inputs */
		/** @param #71 PK_Command_Input */
			/** The Input to select, or 0=toggle */

void MythTV_Player::CMD_Input_Select(int iPK_Command_Input,string &sCMD_Result,Message *pMessage)
//<-dceag-c91-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
#ifndef WIN32
	sendMythCommand("key c");
#endif
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

void MythTV_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
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

void MythTV_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
}

//<-dceag-c809-b->

	/** @brief COMMAND: #809 - Display Alert */
	/** Displays alert on the box */
		/** @param #9 Text */
			/** The text in the alert */
		/** @param #70 Tokens */
			/** File this alert with this token, and if another alert comes in before timeout with the same token, replace it. */
		/** @param #182 Timeout */
			/** Make the alert go away after this many seconds */
		/** @param #251 Interruption */
			/** How to interrupt the user if something is happening */

void MythTV_Player::CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage)
//<-dceag-c809-e->
{
}

//<-dceag-c870-b->

	/** @brief COMMAND: #870 - Get Data */
	/** Get data from a device */
		/** @param #9 Text */
			/** What data to return.  This is free form */
		/** @param #19 Data */
			/** The data being returned */

void MythTV_Player::CMD_Get_Data(string sText,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c870-e->
{
}

//<-dceag-c812-b->

	/** @brief COMMAND: #812 - Application Exited */
	/** Notify us that Myth Player exited */
		/** @param #227 PID */
			/** Process ID to be passed to the ApplicationExited function */
		/** @param #228 Exit Code */
			/** Exit Code to be passed to the ApplicationExited function */

void MythTV_Player::CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage)
//<-dceag-c812-e->
{
#ifndef WIN32
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Process exited %d %d", iPID, iExit_Code);

	void *data;

//	if ( applicationName.compare(MYTH_WINDOW_NAME) == 0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Send go back to the caller!");
		DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,0,0,"",false);
		SendCommand(CMD_MH_Stop_Media_Cat);
	}
	m_mythStatus_set(MYTHSTATUS_DISCONNECTED);
#endif
}

bool MythTV_Player::StopMythFrontend()
{
	if( !m_pData )
		return false; // Never started up anyway

	DeviceData_Base *pDevice_App_Server = NULL;
	string sResponse;
	if(!m_bRouterReloading)
	{
		pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
		if( pDevice_App_Server )
		{
			DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
				"mythfrontend", false);
			return SendCommand(CMD_Kill_Application,&sResponse);  // Get return confirmation so we know it's gone before we continue
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_Player::StopMythFrontend %p %s", pDevice_App_Server,sResponse.c_str());
	return false;
}

//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void MythTV_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
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

void MythTV_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
}
//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** VDR chan + */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
}
//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** VDR chan - */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
}
//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** VDR pause */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
}
//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** VDR stop */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void MythTV_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** VDR mute */

void MythTV_Player::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
}
//<-dceag-c124-b->

	/** @brief COMMAND: #124 - Favorites */
	/** Show favorites */

void MythTV_Player::CMD_Favorites(string &sCMD_Result,Message *pMessage)
//<-dceag-c124-e->
{
}
//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** vdr play */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
}
//<-dceag-c368-b->

	/** @brief COMMAND: #368 - Help */
	/** Help */

void MythTV_Player::CMD_Help(string &sCMD_Result,Message *pMessage)
//<-dceag-c368-e->
{
}
//<-dceag-c455-b->

	/** @brief COMMAND: #455 - Blue */
	/** VDR blue */

void MythTV_Player::CMD_Blue(string &sCMD_Result,Message *pMessage)
//<-dceag-c455-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
	sendMythCommand("key f5");
}
//<-dceag-c476-b->

	/** @brief COMMAND: #476 - Day Down */
	/** Day + */

void MythTV_Player::CMD_Day_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c476-e->
{
}
//<-dceag-c477-b->

	/** @brief COMMAND: #477 - Day Up */
	/** Day - */

void MythTV_Player::CMD_Day_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c477-e->
{
}
//<-dceag-c518-b->

	/** @brief COMMAND: #518 - Green */
	/** VDR Green */

void MythTV_Player::CMD_Green(string &sCMD_Result,Message *pMessage)
//<-dceag-c518-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
	sendMythCommand("key f3");
}
//<-dceag-c528-b->

	/** @brief COMMAND: #528 - Instant Replay */
	/** Instant replay */

void MythTV_Player::CMD_Instant_Replay(string &sCMD_Result,Message *pMessage)
//<-dceag-c528-e->
{
}
//<-dceag-c535-b->

	/** @brief COMMAND: #535 - Last Play */
	/** Last */

void MythTV_Player::CMD_Last_Play(string &sCMD_Result,Message *pMessage)
//<-dceag-c535-e->
{
}
//<-dceag-c571-b->

	/** @brief COMMAND: #571 - Page Down */
	/** Page + */

void MythTV_Player::CMD_Page_Down(string &sCMD_Result,Message *pMessage)
//<-dceag-c571-e->
{
}
//<-dceag-c572-b->

	/** @brief COMMAND: #572 - Page Up */
	/** Page - */

void MythTV_Player::CMD_Page_Up(string &sCMD_Result,Message *pMessage)
//<-dceag-c572-e->
{
}
//<-dceag-c592-b->

	/** @brief COMMAND: #592 - PVR Menu */
	/** Show the pvr menu */

void MythTV_Player::CMD_PVR_Menu(string &sCMD_Result,Message *pMessage)
//<-dceag-c592-e->
{
}
//<-dceag-c605-b->

	/** @brief COMMAND: #605 - Red */
	/** VDR red */

void MythTV_Player::CMD_Red(string &sCMD_Result,Message *pMessage)
//<-dceag-c605-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
	sendMythCommand("key f2");
}
//<-dceag-c682-b->

	/** @brief COMMAND: #682 - Yellow */
	/** VDR yellow */

void MythTV_Player::CMD_Yellow(string &sCMD_Result,Message *pMessage)
//<-dceag-c682-e->
{
	PLUTO_SAFETY_LOCK(mm, m_MythMutex);
	sendMythCommand("key f4");
}
//<-dceag-c919-b->

	/** @brief COMMAND: #919 - On Demand */
	/** Go to Video on demand */

void MythTV_Player::CMD_On_Demand(string &sCMD_Result,Message *pMessage)
//<-dceag-c919-e->
{
}
//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void MythTV_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
}
