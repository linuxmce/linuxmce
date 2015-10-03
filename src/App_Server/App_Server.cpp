/*
    App_Server

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/

/*
	AppServer will also set the State of the m/d to indicate the currently installed operating system on the hard drive (LINUX / WINDOWS)
	and will set the Status to one of the following:
	MD_ON, PC_ON, MD_BLACK, PC_BLACK, MD_OFF, PC_OFF, MD_REBOOTING, PC_REBOOTING

	Where MD means it booted up or will boot up as a media director over the network, and PC means a normal hard drive boot
	ON = it's on and the display is on.  BLACK = The display was shut off
	OFF = It's turned off   REBOOTING = It's being rebooted and should come online soon
*/

//<-dceag-d-b->
#include "App_Server.h"
#include "DCE/DCEConfig.h"
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
#include "pluto_main/Define_DeviceTemplate.h"
#include "VFD_LCD/VFD_LCD_Base.h"

#include "SignalHandler.h"

#ifndef WIN32
//#include "utilities/linux/RatpoisonHandler.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
void EnablePrivileges()
{
	HANDLE hToken; // handle to process token
	TOKEN_PRIVILEGES tkp; // pointer to token structure
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES |
					TOKEN_QUERY, &hToken); // Get the LUID for shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	// Get shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
}
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
App_Server::App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: App_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_AppMutex("AppServer")
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_AppMutex.Init(&m_MutexAttr);
	m_bLastMute=false;
}

//<-dceag-getconfig-b->
bool App_Server::GetConfig()
{
	if( !App_Server_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

#ifndef WIN32
	m_MasterMix.SetOn(1);
	m_iLastVolume = m_MasterMix.GetVolumePercent();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ALSA Master volume: %d%%", m_iLastVolume);

	SignalHandler_Start(this);
#endif

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
App_Server::~App_Server()
//<-dceag-dest-e->
{
	ProcessUtils::KillAllApplications();
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool App_Server::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

bool App_Server::Connect(int iPK_DeviceTemplate )
{
	bool bResult = App_Server_Command::Connect(iPK_DeviceTemplate);
	m_dwPK_Device_MD = m_dwPK_Device; // Shouldn't use this value.  Only relevant if something is corrupted below

	// Find the media director
	DeviceData_Base *pDevice_This = m_pData->m_AllDevices.m_mapDeviceData_Base_Find( m_dwPK_Device );
	if( !pDevice_This )
		return bResult;  // Router must need a reload
	DeviceData_Base *pDevice_MD = pDevice_This->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Media_Director_CONST);
	
	if( !pDevice_MD )
		pDevice_MD = pDevice_This->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Computers_CONST);  // Shouldn't happen.  Be more broad
	if( pDevice_MD )
		m_dwPK_Device_MD = pDevice_MD->m_dwPK_Device;

	if( m_bHardDrive )
	{
		// Store whether this hard drive boot is windows or linux
#ifdef WIN32
		SetState("WINDOWS",m_dwPK_Device_MD);
#else
		SetState("LINUX",m_dwPK_Device_MD);
#endif
	}

	if( m_bHardDrive )
		SetStatus("PC_ON",m_dwPK_Device_MD);
	else
		SetStatus("MD_ON",m_dwPK_Device_MD);

	return bResult;
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
void App_Server::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void App_Server::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a keypress event to an application */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void App_Server::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
    cout << "Need to implement command #28 - Simulate Keypress" << endl;
    cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c67-b->

	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawn an application */
		/** @param #13 Filename */
			/** The name of the executable file to spawn */
		/** @param #50 Name */
			/** A name that we'll remember the application by for future kill commands */
		/** @param #51 Arguments */
			/** Command arguments, tab delimited */
		/** @param #94 SendOnFailure */
			/** Send this messages if the process exited with failure error code. */
		/** @param #95 SendOnSuccess */
			/** Send this messages if the process exited with success error code. */
		/** @param #115 Show logo */
			/** If this is set then we will first select the logo  before spawning the application. */
		/** @param #120 Retransmit */
			/** If false, and if Exclusive is true and another instance is killed, the 'send messages on termination' will not be sent. */
		/** @param #126 Exclusive */
			/** If true, then kill other apps with this same name */
		/** @param #241 Detach */
			/** Detach application after spawning / Don't kill this app on reload. */

void App_Server::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
	PLUTO_SAFETY_LOCK(ap,m_AppMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SpawnApp file: %s; name: %s; args: %s; success: %s; failure: %s; logo: %d;",
		sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnSuccess.c_str(),sSendOnFailure.c_str(),(int) bShow_logo);
	if( bExclusive )
		CMD_Kill_Application(sName,bRetransmit);

    // we don't need this anymore
//  if ( bShow_logo )
//	{
//		// Popup a black window so the user doesn't see some other program that may be running while he's
//		// waiting for this one to appear
//#ifndef WIN32
//		Display *XServerDisplay;
//	    if( (XServerDisplay = XOpenDisplay(getenv("DISPLAY"))) != NULL)
//	    {
//	    	XLockDisplay(XServerDisplay);
//		    int iCurrentScreen   = XDefaultScreen(XServerDisplay);
//			static Window w=0;
//			if( w )
//				XDestroyWindow(XServerDisplay,w);

//			w = XCreateSimpleWindow(XServerDisplay, XDefaultRootWindow(XServerDisplay),
//							0, 0, (DisplayWidth(XServerDisplay, iCurrentScreen)), DisplayHeight(XServerDisplay, iCurrentScreen),
//							1, 0, 0);

//			XClassHint		classHint;
//			classHint.res_name = (char*) ("AppServer " + StringUtils::itos(m_dwPK_Device)).c_str();
//			classHint.res_class = (char*) ("AppServer " + StringUtils::itos(m_dwPK_Device)).c_str();
//			XSetClassHint ( XServerDisplay, w, &classHint );

//			XMapWindow(XServerDisplay, w);

//			XSync(XServerDisplay, True);
//	    	XUnlockDisplay(XServerDisplay);
//			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Window %d",(int) w);
//		}
//#endif
//	}

	int pid = ProcessUtils::SpawnApplication(sFilename, sArguments, sName, new pair<string, string>(sSendOnFailure, sSendOnSuccess), true, bDetach);
	if (pid == 0)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't spawn '%s': %s %s.", sName.c_str(), sFilename.c_str(), sArguments.c_str());
        sCMD_Result = "Failed";
    }
    else
    {
        sCMD_Result = "OK";
    }
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Finished spawning '%s': %s %s. PID: %d", sName.c_str(), sFilename.c_str(), sArguments.c_str(), pid);
}

//<-dceag-c69-b->

	/** @brief COMMAND: #69 - Kill Application */
	/** Kill an application */
		/** @param #50 Name */
			/** Application name given at spawn time */
		/** @param #120 Retransmit */
			/** If false, the 'send messages' won't be processed when the app terminates */

void App_Server::CMD_Kill_Application(string sName,bool bRetransmit,string &sCMD_Result,Message *pMessage)
//<-dceag-c69-e->
{
#ifndef WIN32
	vector<void *> messagesToSend;
	if ( ! ProcessUtils::KillApplication(sName, messagesToSend) )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to kill the application with name: %s", sName.c_str());
		sCMD_Result="Cannot kill";
	}

	if( bRetransmit )
	{
		for(vector<void *>::const_iterator itAttachedData = messagesToSend.begin();itAttachedData != messagesToSend.end();++itAttachedData)
		{
	void *pV = (*itAttachedData);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "pV %p",pV);
			pair<string, string> *pStringsPair = (pair<string, string> *)(*itAttachedData);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "s1 %s s2 %s",pStringsPair->first.c_str(),pStringsPair->second.c_str());

			SendMessageList(pStringsPair->first);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "after send message");
			delete pStringsPair;
		}
	}
#endif
}

//<-dceag-c70-b->

	/** @brief COMMAND: #70 - Hide Application */
	/** Hide an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

void App_Server::CMD_Hide_Application(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c70-e->
{
    cout << "Need to implement command #70 - Hide Application" << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

/* Private */
void App_Server::EnsureLogoIsDisplayed()
{
	return;  // don't do this anymore, it was a real mess
}

void App_Server::ProcessExited(int pid, int status)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "App_Server::ProcessExited() Child with pid %d", pid);
	PLUTO_SAFETY_LOCK(ap,m_AppMutex);
	string applicationName;
	void *data;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "PID %d exited with code %d", pid, status);
	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "App_Server::ProcessExited() Child with pid %d was not found in our internal data structure. Ignoring signal!", pid);
		return;
	}

	if ( data )
	{
		pair<string, string> *pMessagesLists = (pair<string, string>*)data;
		string *p_str = status ? &pMessagesLists->first : &pMessagesLists->second;
		if( p_str->find("<=spawn_log=>")!=string::npos )
		{
			string sFilename = "/var/log/pluto/Spawn_" + applicationName + "_" + StringUtils::itos(pid) + ".log";
			size_t size;
			char *pBuffer = FileUtils::ReadFileIntoBuffer(sFilename,size);

			if( !pBuffer )
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"App_Server::ProcessExited pid %d exited -- can't open log %s",pid,sFilename.c_str());
			else
				StringUtils::Replace(p_str,"<=spawn_log=>",pBuffer);

			delete pBuffer;
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"App_Server::ProcessExited pid %d exited -- no spawn_log",pid);
		SendMessageList( *p_str );
		delete pMessagesLists;
	}
}

void App_Server::SendMessageList(string messageList)
{
	StringUtils::Replace(&messageList, "@AppDev@", StringUtils::itos(m_dwPK_Device));
	if (messageList.length() > 0)
		messageList += " " + StringUtils::itos(COMMANDPARAMETER_Comment_CONST) + " \"App_Server::SendMessageList\"";
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending this command chain: -->%s<--", messageList.c_str() );

    Message *pMessage = new Message(messageList);    // empty message (to only caryy the rest with him).
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message to: %d type: %d id: %d extra %d",
			pMessage->m_dwID,pMessage->m_dwMessage_Type,pMessage->m_dwID,(int) pMessage->m_vectExtraMessages.size());

	QueueMessageToRouter( pMessage  );
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "After queing the message to the router queue!");
}

//<-dceag-sample-b->!
//<-dceag-createinst-b->!

//<-dceag-c323-b->

	/** @brief COMMAND: #323 - Halt Device */
	/** Halt or reboot this device */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */

void App_Server::CMD_Halt_Device(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c323-e->
{
        if ( sForce=="" ) {
	        // Get device data and check power off mode
	        string powerOffMode = m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(m_dwPK_Device_MD,DEVICEDATA_PowerOff_mode_CONST);
		if (powerOffMode == "") {
		        int ret = system("/usr/bin/pm-is-supported --suspend");
			if (ret < 0) {
		                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Error checking if suspend is supported (running /usr/bin/pm-is_supported --suspend)");
				sForce = "H"; // halt, as we don't know what is supported
			} else {
			        if ( ret ) {
				        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Suspend not supported - halting instead");
					sForce = "H"; // halt if we don't have suspend capabilities
				} else {
				        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Suspend supported - suspending");
					sForce = "S";
				}
			}
		} else {
		        sForce = powerOffMode;
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"PowerOff mode overridden from MD device data: %s", sForce.c_str());
		}
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Halt_Device %s",sForce.c_str());

	switch( sForce[0] )
	{
	case 'S':
		if( m_bHardDrive )
			SetStatus("PC_SUSPEND",m_dwPK_Device_MD);
		else
			SetStatus("MD_SUSPEND",m_dwPK_Device_MD);

		DisplayMessageOnOrbVFD("Suspending...");

#ifndef WIN32
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling initiateSuspend.sh");
		system("/usr/pluto/bin/initiateSuspend.sh");
#else
		EnablePrivileges();
		::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
#endif
		break;
	case 'R':
	case 'N':
	case 'V':
	case 'A':
		if( sForce[0]=='R')
			SetStatus("REBOOTING",m_dwPK_Device_MD);
		else if ( sForce[0]=='A' )
		{
			SetStatus("REBOOTING",m_dwPK_Device_MD);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Rebooting with AVWizard");
			DCEConfig dceconfig;
			dceconfig.AddString("AVWizardOverride","1");
			dceconfig.WriteSettings();
		}
		else if( sForce[0]=='N' )
			SetStatus("MD_REBOOTING",m_dwPK_Device_MD);
		else if( sForce[0]=='V' )
			SetStatus("PC_REBOOTING",m_dwPK_Device_MD);

		DisplayMessageOnOrbVFD("Rebooting...");

#ifndef WIN32
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling reboot");
		system("reboot");  // Don't know how to do this
#else
		EnablePrivileges();
		::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
#endif
		break;
	case 'I': // ignore
		break;
	default:
		if( m_bHardDrive )
			SetStatus("PC_OFF",m_dwPK_Device_MD);
		else
			SetStatus("MD_OFF",m_dwPK_Device_MD);

		DisplayMessageOnOrbVFD("Powering off...");

#ifndef WIN32
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling poweroff");
		system("poweroff");
#else
		EnablePrivileges();
		::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
#endif

		// TODO: Power off this system
		break;
	}
}

void App_Server::DisplayMessageOnOrbVFD(string sMessage)
{
	DeviceData_Base *pDevice_OSD = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Orbiter_CONST);
	DeviceData_Base *pDevice_VFD = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_LCDVFD_Displays_CONST);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Displaying on OSD: %d VFD: %d %s",
		(pDevice_OSD ? pDevice_OSD->m_dwPK_Device : 0),
		(pDevice_VFD ? pDevice_VFD->m_dwPK_Device : 0),
		sMessage.c_str());

	string sResponse; // Get Return confirmation so we know the message gets through before continuing
	if( pDevice_OSD )
	{
		DCE::CMD_Display_Message_DT CMD_Display_Message_DT(m_dwPK_Device,DEVICETEMPLATE_Orbiter_Plugin_CONST,BL_SameHouse,
			sMessage,"","","30",StringUtils::itos(pDevice_OSD->m_dwPK_Device));
		SendCommand(CMD_Display_Message_DT,&sResponse);
	}
	if( pDevice_VFD )
	{
		DCE::CMD_Display_Message CMD_Display_Message(m_dwPK_Device,pDevice_VFD->m_dwPK_Device,
			sMessage,
			StringUtils::itos(VL_MSGTYPE_RUNTIME_NOTICES),"app serve","30","");
		SendCommand(CMD_Display_Message,&sResponse);
	}
}

void App_Server::KillSpawnedDevices()
{
#ifndef WIN32
	SignalHandler_Stop();
#endif
	App_Server_Command::KillSpawnedDevices();

	// This will only be called when we are dying, so we won't care about what happens to our spawned children.
	// We had a problem that KillSpawnedDevices called KillPids.sh, which when exited, called the sh signal handler and hung at wait
}
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** Increase the volume 1% */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void App_Server::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	if (iRepeat_Command < 1)
		iRepeat_Command = 1;

	bool bLastMute = m_bLastMute;
	m_bLastMute=false;
	m_iLastVolume+=iRepeat_Command;
	if (m_iLastVolume > 100)
		m_iLastVolume = 100;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Volume is now %d",m_iLastVolume);
	DATA_Set_Volume_Level(m_iLastVolume,true);

#ifndef WIN32
	m_MasterMix.SetVolumePercent(m_iLastVolume);
#endif	
	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}

//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** Decrease the volume 1% */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void App_Server::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	if (iRepeat_Command < 1)
		iRepeat_Command = 1;

	bool bLastMute = m_bLastMute;
	m_bLastMute=false;
	m_iLastVolume-=iRepeat_Command;
	if (m_iLastVolume < 0)
		m_iLastVolume = 0;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Volume is now %d",m_iLastVolume);
	DATA_Set_Volume_Level(m_iLastVolume,true);

#ifndef WIN32
	m_MasterMix.SetVolumePercent(m_iLastVolume);
#endif

	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}

//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** Toggle mute/unmute */

void App_Server::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	bool bLastMute = m_bLastMute;
	m_bLastMute=!m_bLastMute;
#ifndef WIN32
	m_MasterMix.SetOn(bLastMute ? 1 : 0); // toggle mute
#endif

	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}

//<-dceag-c313-b->

	/** @brief COMMAND: #313 - Set Volume */
	/** Set the volume to a specific level between 0-100 */
		/** @param #76 Level */
			/** A value between 0 and 100 where 0 is mute and 100 is full volume.  Numbers preceded with a - or + are relative.  +15 means up 15, -10 means down 10 */

void App_Server::CMD_Set_Volume(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c313-e->
{
	int iVolume = 0;
#ifndef WIN32
	iVolume = m_MasterMix.GetVolumePercent();
#endif

	if (sLevel.size() > 1 && sLevel[0] == '+' || sLevel[0] == '-')
		iVolume += atoi(sLevel.c_str());
	else
		iVolume = atoi(sLevel.c_str());

	bool bLastMute = m_bLastMute;
	m_bLastMute=false;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Volume is now %d%%", iVolume);
	DATA_Set_Volume_Level(iVolume, true);

#ifndef WIN32
	m_MasterMix.SetVolumePercent(iVolume);
	m_MasterMix.SetOn(1);
#endif
	m_iLastVolume = iVolume;

	// TODO: check that the mixer actually worked
	sCMD_Result = "OK";
}

//<-dceag-c696-b->

	/** @brief COMMAND: #696 - Application is Running */
	/** Checks if the given application is running. */
		/** @param #50 Name */
			/** Application name given at spawn time */
		/** @param #119 True/False */
			/** Indicates if it's running */

void App_Server::CMD_Application_is_Running(string sName,bool *bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c696-e->
{
#ifndef WIN32
	*bTrueFalse = ProcessUtils::ApplicationIsLaunchedByMe(sName);
#endif
}
//<-dceag-c812-b->

	/** @brief COMMAND: #812 - Application Exited */
	/** Used by the appserver to tell itself that it received a SIGCHLD, in order to avoid a deadlock */
		/** @param #227 PID */
			/** Process ID to be passed to the ApplicationExited function */
		/** @param #228 Exit Code */
			/** Exit Code to be passed to the ApplicationExited function */

void App_Server::CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage)
//<-dceag-c812-e->
{
		ProcessExited(iPID, iExit_Code);
}
