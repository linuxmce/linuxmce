//TODO import pbProgress and tlStatusMessages stuff!!!;

#include "LMCE_Launch_Manager.h"
#include "LM.h"
#include "List.h"
#include "DB.h"
#include "UI.h"
//#include "ServerSocket.h"
//#include "SocketException.h"
#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"
#include "DCE/ClientSocket.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/getch.h"
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

using namespace std; //DCE

#include "Gen_Devices/AllCommandsRequests.h"

// timeouts in seconds
#define MEDIA_DEVICE_TIMEOUT 10
#define CORE_DEVICE_TIMEOUT 10
#define CORE_SERVICE_TIMEOUT 600

// time in milliseconds
#define LOG_REFRESH_INTERVAL 30000


#define	PLUTO_CONFIG "/etc/pluto.conf"

#define SCREENLOGS_BASE "/home/screenlogs"
#define APP_UI_DIAG "/usr/pluto/bin/UIdiag"
#define APP_UI_DIAG_WORKDIR "/usr/pluto/bin"
#define APP_UI_DIAG_LIBS "/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/lib"

#define SCRIPT_X_CHECK_SETTINGS "/usr/pluto/bin/X-CheckSettings.sh"

#define SCRIPT_SETUP_AV_SETTINGS "/usr/pluto/bin/SetupAudioVideo.sh"

#define VIDEO_DRIVER_NAME_FILE "/tmp/lmce_lm.video.dat"
#define VIDEO_DRIVER_TOOL "/usr/pluto/bin/GetVideoDriver.py"

#define KILL_XORG_SCRIPT "/usr/pluto/bin/Stop_X.sh"
#define PLUTO_LOG_DIR "/var/log/pluto"
#define PLUTO_LOCKS "/usr/pluto/locks/pluto_spawned_local_devices.txt"
#define LM_PROGRESS_LOG "/var/log/pluto/LaunchManager.progress.log"
#define NETWORK_SETTINGS_SCRIPT "/usr/pluto/bin/Network_DisplaySettings.sh --all"
#define RA_SCRIPT "/usr/pluto/bin/RA-handler.sh"

// Remote Assistance script options
#define RA_ON "--enable"
#define RA_OFF "--disable"

#define START_CORE_SCRIPT "/usr/pluto/bin/StartCoreServices.sh"
#define STOP_CORE_SCRIPT "/usr/pluto/bin/StopCoreServices.sh"

#define DCEROUTER_LOCK "/usr/pluto/locks/DCERouter"


// TODO: comb over and make sure everything is bein initialized that needs to be
LM::LM()
{
	m_pLMCE_Launch_Manager=NULL;
	m_pDisplay=NULL;
	m_iDisplayWidth=m_iDisplayHeight=0;
	m_iRootWindow=0;
	m_iScreen=0;
	m_pResult=NULL;
	m_pAlarmManager = NULL;
	//Init progress tracking vars	
	m_iProgress = 0;
	m_bShowProgress = false;
	m_bMediaRunning = false;	// FIXME: Verify this behaviour is correct. Thanks Phenigma.
	//m_SocketConnection = NULL;
}

// TODO: comb over and make sure everything is bein initialized that needs to be
LM::~LM()
{
	delete m_pAlarmManager;
	m_pAlarmManager=NULL;
	delete m_pDisplay;
	m_pDisplay=NULL;
}
//COMPLETE
void LM::Initialize()
{
	if (getenv("DISPLAY") != NULL) 
	{
		m_pDisplay = XOpenDisplay(getenv("DISPLAY"));
		if (m_pDisplay)
		{
			m_iScreen = DefaultScreen(m_pDisplay);
			m_iRootWindow = RootWindow(m_pDisplay,m_iScreen);
			m_pSc = XRRGetScreenInfo(m_pDisplay,m_iRootWindow);
		} else {
			// running in console all by itself.
		}

	}

	m_pAlarmManager = new DCE::AlarmManager();
	m_pAlarmManager->Start(2);      
	DCE::LoggerWrapper::SetType(LT_LOGGER_FILE, PLUTO_LOG_DIR "/LaunchManager.log");
	flushLog();
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting...");
	m_uiMainUI.initialize("LinuxMCE Launch Manager");

	writeLog("=>initialize_Connections()",true);
	initialize_Connections();
	writeLog("=>initialize_VideoSettings()");
	initialize_VideoSettings();
	writeLog("=>initialize_AudioSettings()");
	initialize_AudioSettings();
	writeLog("=>initialize_LogOptions()");
	initialize_LogOptions();
	// after all is read, initialize server
	writeLog("=>initialize_Start()");
	initialize_Start();

	writeOSD("Welcome to LinuxMCE");

	// if the system was already running when we started, let's catch new children and report we are up
	if ( m_bCoreRunning || m_bMediaRunning )
	{
        	updateScripts();
		respawnNewChildren();
		reportDeviceUp();
	}
	
	writeLog("=>loadSettings()");
	loadSettings();
	
	writeLog("Started!");

	//Schedule the Autostart routine to start in 1 second...
	m_pAlarmManager->AddRelativeAlarm(1,this,DO_AUTOSTART,NULL);
	syncUI();
}
//COMPLETE
void LM::initialize_Connections()
{
	// initing connections.. Load in the current pluto.conf config
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	m_sCoreIP = pConfig->m_mapParameters_Find("DCERouter");
	m_sMySQLHost = pConfig->m_mapParameters_Find("MySqlHost");
	m_sMySQLUser = pConfig->m_mapParameters_Find("MySqlUser");
	m_sMySQLPass = pConfig->m_mapParameters_Find("MySqlPassword");
	m_sMySQLPort = pConfig->m_mapParameters_Find("MySqlPort");
	if (m_sMySQLPort=="")
		m_sMySQLPort = "3306";
	m_sDCERouterPort = pConfig->m_mapParameters_Find("DCERouterPort");
	if (m_sDCERouterPort== "")
		m_sDCERouterPort = "3450";
	
	//TODO:SHOULD CHANGE THESE TO BOOL?
	m_sAutostartCore = pConfig->m_mapParameters_Find("AutostartCore");  //a string "1" or "0"
	m_sAutostartMedia = pConfig->m_mapParameters_Find("AutostartMedia");
	
	
	m_bRemoteAssistanceRunning = checkRAStatus();
	// trying to connect to DB
	openDB();
	m_sDeviceID = pConfig->m_mapParameters_Find("PK_Device");
	// detecting core
	m_sCoreDeviceID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=7");
	if (m_sCoreDeviceID=="")
	{
		m_sCoreDeviceID = "1";
		writeLog("Hmm... core device ID is empty, setting it to 1", false, LV_WARNING);
	}
	else
		writeLog("Detected core as device #" + m_sCoreDeviceID, false, LV_WARNING);
	
	string mdID;
	
	if (m_sDeviceID == m_sCoreDeviceID)
	{
		m_bCoreHere = true;
		mdID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + m_sDeviceID + " AND FK_DeviceCategory IN (8)");
		
		if (mdID != "")
		{
			m_bMediaHere = true;
		}
	}
	else
	{
		m_bMediaHere = true;
		mdID = m_sDeviceID;
	}

	if (mdID!="")
	{
		string localOrbiterID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + mdID + " AND FK_DeviceCategory IN (2, 3, 5)");
		m_sOrbiterID = localOrbiterID;
	}

	m_sMediaID = mdID;
	
	delete pConfig;
}

//COMPLETE
bool LM::checkRAStatus()
{
	return getRAid() != "";
}

//COMPLETE
string LM::getRAid()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	string sPass = pConfig->m_mapParameters_Find("remote");
	string sInst =  pConfig->m_mapParameters_Find("PK_Installation");
	delete pConfig;
	
	if (sPass == "")
		return m_sRAInfo = "";
	else
		return m_sRAInfo = sInst + "-" + sPass;
}
//COMPLETE
bool LM::openDB()
{
	if (m_dbPlutoDatabase.connected())
	{
		writeLog("Attempted to reopen already opened DB, closing current connection first", false, LV_WARNING);
		closeDB();
	}
	
	writeLog("Opening DB connection..", true);
	
	if(m_dbPlutoDatabase.connect(m_sMySQLHost,m_sMySQLUser,m_sMySQLPass,"pluto_main"))
	{
		writeLog("Successful", true);
		return true;
	}
	else
	{
		writeLog("Error: Could not connect to database",true,LV_WARNING);
		m_dbPlutoDatabase.close();

		return false;
	}
}
//COMPLETE
bool LM::closeDB()
{
	writeLog("Closing DB connection..");
	
	if (m_dbPlutoDatabase.connected())
	{
		m_dbPlutoDatabase.close();
		writeLog("Successful", true);
		return true;
	}
	else
	{
		writeLog("No need: it wasn't open");
		return false;
	}
}
//COMPLETE
void LM::initialize_LogOptions() //TODO: is this needed? I have no plans for LM to have log interaction like the old one did.. This functionality can be moved to the web admin
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	string s = pConfig->m_mapParameters_Find("LogLevels");
	delete pConfig;
}
//COMPLETE
void LM::initialize_Start()
{
	writeLog("Checking for mysql...");
	bool bMySQL = checkMySQL();
	string s;
	s = bMySQL?"connection OK":"connection failed";
	writeLog("Done checking for mysql..." + s);
	
	//TODO decide are we dedicated core, hybrid or disk-based MD
	
	writeLog("Checking for core services...");
	bool bCore=checkCore(m_sCoreIP);
	s=bCore?"core is running":"core is not running";
	writeLog("Done checking for core services..." + s);
	m_bCoreRunning = bCore;
	
	// connecting LM device to core
	if (m_bCoreRunning)
	{
		if ( initialize_LMdevice() )
		{
			if (m_bMediaHere)
				m_bMediaRunning = checkMedia();
		}
		else
		{
			writeLog("Failed to connect to DCERouter!", true, LV_WARNING);
		}
	}

	// starting RA monitoring timer
	m_pAlarmManager->AddRelativeAlarm(5,this,UPDATE_RA_STATUS,NULL);
}
//COMPLETE
bool LM::checkMySQL(bool showMessage)
{
	string msg = "Connection to specified MySQL server failed";
	bool bResult = false;
	
	if ( m_dbPlutoDatabase.connected() ) {
		msg = "Connection to specified MySQL server succeeded";
		bResult = true;
	}

	if (showMessage)
	{
		writeLog(msg.c_str(),true,LV_STATUS);
	}

	return bResult;
}
//COMPLETE
void LM::AlarmCallback(int id, void* param)
{
	
	if (id==INITIALIZATION) {
		m_pAlarmManager->CancelAlarmByType(INITIALIZATION);
		Initialize();
	} else if(id==LM_KEEP_ALIVE) {
		LMdeviceKeepAlive();
	} else if (id==UPDATE_RA_STATUS) {
		//auto-schedule next call to immitate an interval timer
		m_pAlarmManager->CancelAlarmByType(UPDATE_RA_STATUS);
		m_pAlarmManager->AddRelativeAlarm(5,this,UPDATE_RA_STATUS,NULL);
		updateRAstatus();
	} else if (id == REPORT_DEVICE_UP) {
		m_pAlarmManager->CancelAlarmByType(UPDATE_RA_STATUS);
		reportDeviceUp();
	} else if (id == DO_AUTOSTART) {
		m_pAlarmManager->CancelAlarmByType(DO_AUTOSTART);
		doAutostart();
	} else if (id==UPDATE_ORBITER_REGEN_PROGRESS) {
		//The alarm will re-schedule its self in the callback function to keep
		//the alarm going until the regen is finished.
		m_pAlarmManager->CancelAlarmByType(UPDATE_ORBITER_REGEN_PROGRESS);
		updateOrbiterRegenProgress();
	}
}
//COMPLETE
void LM::respawnNewChildren()
{
	writeLog("Requested respawning of new children devices", true, LV_WARNING);
	writeOSD("Respawning New Devices");

	syncWithLockList(true);
	
	if (m_bCoreHere && m_bCoreRunning)
	{
		writeLog("Spawning new children devices - core", true, LV_WARNING);
		writeOSD("Spawning new Core Devices.");
		startCoreDevices(true);
		writeLog("Finished spawning new children devices - core", true, LV_WARNING);
	}
	
	if (m_bMediaHere && m_bMediaRunning)
	{
		writeLog("Spawning new children devices - media", true, LV_WARNING);
		writeOSD("Spawning new Media Devices.");
		startMediaDevices(true);
		writeLog("Finished spawning new children devices - media", true, LV_WARNING);
	}
	
	writeLog("Finished respawning of new children devices", true, LV_WARNING);
	
}
//COMPLETE
void LM::waitForDevice(int deviceID)
{
	char cStatus = 'N';
	int iCount = MEDIA_DEVICE_TIMEOUT;
	
	if (!m_pLMCE_Launch_Manager)
		cStatus = 'E';
	
	while (cStatus=='N' && iCount--)
	{
		sleep(1);

		cStatus = m_pLMCE_Launch_Manager->DeviceIsRegistered(deviceID);
	}
						
	switch (cStatus)
	{
		case 'Y':
			writeLog("Device registered", true);
			break;
		case 'N':
			writeLog("Timeout waiting for device registration", true);
			m_bDelayUpReport = true;
			break;
		case 'D':
			writeLog("Device is disabled", true);
			break;
		case 'E':
			writeLog("Error when querying device status", true);
			break;
		default:
			writeLog("Unknown status '" + std::string(1,cStatus) + "'when calling Command_Impl::DeviceIsRegistered", true);
			break;
	}
}
//COMPLETE
void LM::reportDeviceUp()
{
	// saying we're up
	DCE::Message *pMessage = new DCE::Message(atoi(m_sDeviceID.c_str()), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_UP, 0);

	if (m_pLMCE_Launch_Manager)
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_UP from LM device " + m_sDeviceID, false);
		m_pLMCE_Launch_Manager->QueueMessageToRouter(pMessage);
		writeOSD("");		
	}
	else
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_UP from LM device " + m_sDeviceID + " failed", false, LV_WARNING);
	}
}
void LM::reportDeviceDown()
{
	// saying we're down
	DCE::Message *pMessage = new DCE::Message(atoi(m_sDeviceID.c_str()), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_DOWN, 0);
	if (m_pLMCE_Launch_Manager)
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_DOWN from LM device " + m_sDeviceID, false);
		m_pLMCE_Launch_Manager->QueueMessageToRouter(pMessage);
	}
	else
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_DOWN from LM device " + m_sDeviceID + " failed", false, LV_WARNING);
	}
}
//COMPLETE - but not used(yet?)
void LM::loadSettings()
{
}
//COMPLETE - but not used(yet?)
void LM::saveSettings()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	pConfig->AddInteger("AutostartCore", atoi(m_sAutostartCore.c_str()));
	pConfig->AddInteger("AutostartMedia", atoi(m_sAutostartMedia.c_str()));
	pConfig->WriteSettings();
	delete pConfig;
}
//COMPLETE - but not used(yet?)
void LM::actionSaveBootupSettings()
{
	saveSettings();
}

//COMPLETE
void LM::deinitialize_LMdevice()
{
	if (m_pLMCE_Launch_Manager)
	{
		m_pLMCE_Launch_Manager->OnQuit();
		pthread_join(m_pLMCE_Launch_Manager->m_RequestHandlerThread, NULL);
		delete m_pLMCE_Launch_Manager;
		m_pLMCE_Launch_Manager = NULL;
	}
}
//COMPLETE
bool LM::checkMedia()
{
	writeLog("Checking if Media Station is already running..", true);
	
	findRunningMediaDevices();
	
	char cStatus = m_pLMCE_Launch_Manager->DeviceIsRegistered(atoi(m_sOrbiterID.c_str()));
	if (cStatus == 'Y')
	{
		writeLog("Orbiter #" + m_sOrbiterID + " is registered");
		return true;
	}
	else
	{
		writeLog("Orbiter #" + m_sOrbiterID + " is not registered");
		return false;
	}
}
//COMPLETE (though not yet used)
void LM::rebootPC()
{
	writeLog("Rebooting");	
	m_uiMainUI.flushLog();
	
	exec_system("/sbin/reboot",false);
	// giving system time to start actual reboot
	sleep(120);
}
//COMPLETE
void LM::updateRAstatus()
{
	bool bRAstatus = checkRAStatus();
	
	if (bRAstatus != m_bRemoteAssistanceRunning)
	{
		m_bRemoteAssistanceRunning = bRAstatus;
	}
}
//COMPLETE
string LM::getOrbiterStatus()
{
	int iNull;
	return getOrbiterStatus(iNull);
}
//COMPLETE
string LM::getOrbiterStatus(int &iProgressValue)
{
	return getOrbiterStatus(m_sOrbiterID, iProgressValue);
}
//COMPLETE
string LM::getOrbiterStatus(const string &orbiterID, int &iProgressValue)
{
	string orbiterStatus="!";// ! - is our custom error "while fetching info"
	iProgressValue=0;

	if (orbiterID!="")
	{
		writeLog("Getting status for Orbiter #" + orbiterID, false, LV_STATUS);
		
		if (m_pLMCE_Launch_Manager)
		{		
			string sValue_To_Assign, sText;
			int iValue;
			DCE::CMD_Get_Orbiter_Status cmd(atoi(m_sDeviceID.c_str()), atoi(m_sOrbiterPluginID.c_str()), atoi(orbiterID.c_str()), &sValue_To_Assign, &sText, &iValue);
			if ( m_pLMCE_Launch_Manager->SendCommand(cmd) )
			{
				orbiterStatus = sValue_To_Assign;
				iProgressValue = iValue;
				writeLog("Got orbiter status: " + orbiterStatus, false, LV_STATUS);
			}
			else
				writeLog("Getting status for Orbiter failed - failed to send command", false, LV_WARNING);
		}
		else
			writeLog("Getting status for Orbiter failed - no connection to core", false, LV_WARNING);
	}
	else
	{
		writeLog("Getting status for Orbiter failed - no # passed", false, LV_WARNING);
	}
	
	return orbiterStatus;
}
//COMPLETE
string LM::getOrbiterStatusMessage(const string &orbiterStatus)
{
	if (orbiterStatus=="!")
		return "Failed to get Orbiter status";
	
	if (orbiterStatus=="O")
		return "Orbiter status is OK";
	
	if (orbiterStatus=="N")
		return "Generated skin for new Orbiter, you need to reload router";
	
	if (orbiterStatus=="n")
		return "There are no skins generated yet for this new Orbiter";
	
	if (orbiterStatus=="R")
		return "Skin generation for Orbiter is in progress now";
	
	if (orbiterStatus=="r")
		return "Skin generation for new Orbiter is in progress now";
	
	if (orbiterStatus=="U")
		return "The device with this ID is unknown";
	
	if (orbiterStatus=="D")
		return "The device with this ID is not an Orbiter";
	
	return "Unknown status: " + orbiterStatus;
}
//COMPLETE
bool LM::startCore()
{
        updateScripts();
  
	if ( startCoreServices() )
	{	
		const int iMaxRetries = 10;
		int iRetries = 0;
		bool bLMinit = false;
		
		while( (iRetries++<iMaxRetries) && !bLMinit)
		{
			writeLog("Trying to connect to DCERouter, attempt " + StringUtils::itos(iRetries) + " of " + StringUtils::itos(iMaxRetries));
			bLMinit = initialize_LMdevice();
		}
		
		if (!bLMinit)
		{
			writeLog("Failed to connect to DCERouter!", true, LV_WARNING);
			return false;
		}
		else
		{
			// start core devices
			startCoreDevices();
			m_bCoreRunning = true;
			return true;
		}
	}
	else
		return false;
}
//COMPLETE
bool LM::confirmOrbiterSkinIsReady()
{
	if (m_sOrbiterID=="")
		return false;
	
	// if we already have a record in the list for this Orbiter
	string orbiterRecordID = m_dbPlutoDatabase.quickQuery("SELECT PK_Orbiter FROM Orbiter WHERE PK_Orbiter=" + m_sOrbiterID);
	
	// if Orbiter needs a regen (flag set by AVWizard)
	string orbiterRegenFlag = m_dbPlutoDatabase.quickQuery("SELECT Regen FROM Orbiter WHERE PK_Orbiter=" + m_sOrbiterID);
	
	if (orbiterRecordID=="" || orbiterRegenFlag=="1")
	{
		if ( !triggerOrbiterRegen() )
		{
			writeLog("Failed to start generation of skin for current Orbiter #"+m_sOrbiterID, true, LV_WARNING);
			return false;
		}
		else
		{
			if (orbiterRecordID=="")
			{
				writeLog("First time generating skin for current Orbiter "+m_sOrbiterID, true);
				writeOSD("First time generating skin for Orbiter "+m_sOrbiterID);
			}
			else
			{
				writeOSD("Regenerating skin for current Orbiter "+m_sOrbiterID);
				writeLog("Regenerating skin for current Orbiter "+m_sOrbiterID, true);
			}
			sleep(3);
		}
	}
	
	startOrbiterRegenProgressTracking();
	
	while (m_bRegenInProgress)
	{
		sleep(1);
	}
	
	string sOrbiterStatus = getOrbiterStatus();
	
	if (sOrbiterStatus=="O")
	{
		writeLog("Generated skin successfully", true);
		writeOSD("Generated skin successfully");
	}
	else
	{
		writeLog("Skin generation completed, server says: " + getOrbiterStatusMessage(sOrbiterStatus), true, LV_WARNING);
	}
	
	return (sOrbiterStatus=="O")||(sOrbiterStatus=="N");
}
//COMPLETE
bool LM::triggerOrbiterRegen(bool bAllOrbiters/*=false*/)
{
	// just for case OrbiterGen won't succeed first time
	m_dbPlutoDatabase.query("UPDATE Orbiter SET Regen=1 WHERE" + (bAllOrbiters?" 1":(" PK_Orbiter=" + m_sOrbiterID)));		
	if (m_pLMCE_Launch_Manager)
	{
		int orbiterPluginID=-1;
		map<int,string> mapDevices;
		m_pLMCE_Launch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &mapDevices);
		if (!mapDevices.empty())
		{
			orbiterPluginID = mapDevices.begin()->first;
			DCE::CMD_Regen_Orbiter cmd( atoi(m_sDeviceID.c_str()),  orbiterPluginID, (bAllOrbiters?0:atoi(m_sOrbiterID.c_str())),"", "" );
			m_pLMCE_Launch_Manager->SendCommandNoResponse(cmd);
			writeLog("Command sent", false);
			return true;
		}
		else
		{
			writeLog("Failed to find Orbiter Plugin", false, LV_WARNING);
			return false;
		}
	}
	else
	{
		writeLog("Command not sent - no connection to core", false, LV_WARNING);
		return false;
	}
}
//COMPLETE
void LM::startOrbiterRegenProgressTracking(bool currentOrbiterOnly/*=true*/)
{
	m_bRegenInProgress = true;
	m_bRegenTrackingCurrentOrbiterOnly = currentOrbiterOnly;

	if (m_pAlarmManager->FindAlarmByType(UPDATE_ORBITER_REGEN_PROGRESS) == -1)
	{
		m_pAlarmManager->AddRelativeAlarm(5,this,UPDATE_ORBITER_REGEN_PROGRESS,NULL);
	}
	
}
//COMPLETE
void LM::jumpToOrbiter()
{
	if (m_pLMCE_Launch_Manager)
	{
		writeLog("Activating Orbiter...", true);
		writeOSD("");
		DCE::CMD_Activate_PC_Desktop cmd(atoi(m_sDeviceID.c_str()), atoi(m_sOrbiterID.c_str()), 0);
		m_pLMCE_Launch_Manager->SendCommandNoResponse(cmd);
	}
	else
	{
		writeLog("Failed to activate Orbiter - no connection to DCE router", true, LV_WARNING);
	}
}
//COMPLETE
bool LM::checkIfOrbiterIsOK()
{
	if (!m_bMediaHere || m_sOrbiterID=="")
	{
		writeLog("Not getting Orbiter status - there is no Orbiter at this PC", true);
		return false;
	}
	
	return getOrbiterStatus() == "O";
}
//COMPLETE
void LM::actionReloadRouter()
{
	if (m_pLMCE_Launch_Manager)
	{
		writeLog("Reloading router", true);
		DCE::Message *pMessage = new DCE::Message(0, DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_RELOAD, 0);
		m_pLMCE_Launch_Manager->QueueMessageToRouter(pMessage);
	}
	else
	{
		writeLog("Failed to reload router -  no connection", true);
		return;
	}
	
	// updating serial ports
	updateScripts();
}
//COMPLETE
void LM::LMdeviceKeepAlive()
{
	if (m_pLMCE_Launch_Manager)
	{
		// if device is finishing
		if (m_pLMCE_Launch_Manager->m_bTerminate)
		{
			writeLog("Connection to DCERouter terminated", true);
			bool bReload = m_pLMCE_Launch_Manager->m_bReload;
			deinitialize_LMdevice();
			if (bReload)
			{
				writeLog("Reconnecting to DCERouter as requested", true);
				writeOSD("Reconnecting to DCE Router as requested");
				if (!initialize_LMdevice())
				{
					writeLog("Will retry later..", true);
					m_pAlarmManager->CancelAlarmByType(LM_KEEP_ALIVE);
					m_pAlarmManager->AddRelativeAlarm(5,this,LM_KEEP_ALIVE,NULL);
				}
				else
				{
                                        updateScripts();
					respawnNewChildren();
					reportDeviceUp();
				}
			}
			else
			{
				writeLog("Reconnect to DCERouter was not requested", true);
				m_bCoreRunning = false;
				m_bMediaRunning = false;
			}
		}
		else
		{
			m_pAlarmManager->CancelAlarmByType(LM_KEEP_ALIVE);
			m_pAlarmManager->AddRelativeAlarm(5,this,LM_KEEP_ALIVE,NULL);
		}
	}
	else
	{
		if (m_bCoreRunning || m_bMediaRunning)
		{
			writeLog("Reconnecting to DCERouter as requested", true);
			writeLog("Reconnecting to DCERouter as requested");
			if (!initialize_LMdevice())
			{
				writeLog("Will retry later..", true);
				m_pAlarmManager->CancelAlarmByType(LM_KEEP_ALIVE);
				m_pAlarmManager->AddRelativeAlarm(5,this,LM_KEEP_ALIVE,NULL);
			}
			else
			{
                                updateScripts();
				respawnNewChildren();
				reportDeviceUp();
			}
		}
                else
                {
                    writeLog("LMdeviceKeepAlive() not reconnecting, as this is not required");
			m_pAlarmManager->CancelAlarmByType(LM_KEEP_ALIVE);
                }
	}
}
//COMPLETE
void LM::stopCoreDevices()
{
}
//TODO: test
void LM::waitForDeviceShutdown(int deviceID)
{
	// TODO generalize to 'wait for device status'?
	char cStatus = 'Y';
	int iCount = MEDIA_DEVICE_TIMEOUT;
	
	if (!m_pLMCE_Launch_Manager)
		cStatus = 'E';
	
	while (cStatus=='Y' && iCount--)
	{
		sleep(1);
		cStatus = m_pLMCE_Launch_Manager->DeviceIsRegistered(deviceID);
	}
	
	if (cStatus=='Y')
		writeLog("Timeout waiting for device shutdown", false, LV_WARNING);
}
//TODO: Test!
void LM::actionActivateOrbiter()
{
	jumpToOrbiter();
}
//TODO: test!
void LM::updateOrbiterSize(int width, int height)
{
	writeLog("Updating Orbiter size to " + StringUtils::itos(width) + "x" + StringUtils::itos(height));
	writeOSD("Updating Orbiter size to " + StringUtils::itos(width) + "x" + StringUtils::itos(height));
	if (m_sOrbiterID!="")
	{
		string query = "SELECT PK_Size FROM Size WHERE Width=" + StringUtils::itos(width) + " AND Height=" + StringUtils::itos(height);
		writeLog("Query: " + query, false, LV_DEBUG);
		string sizeID = m_dbPlutoDatabase.quickQuery(query);
		if (sizeID=="")
		{
			writeLog("No direct match, trying to find Size that will fit into screen and have maximal square");
			query = "SELECT PK_Size, Width*Height AS Square FROM Size WHERE Width<=" + StringUtils::itos(width) + " AND Height<=" + StringUtils::itos(height) + " ORDER BY Square DESC";
			writeLog("Query: " + query, false, LV_DEBUG);
			sizeID = m_dbPlutoDatabase.quickQuery(query);
		}
		
		if (sizeID=="")
		{
			writeLog("No luck, aborting :(");
		}
		else
		{
			writeLog("Setting PK_Size for Orbiter #"+m_sOrbiterID + " to " + sizeID);
			string query = "UPDATE Device_DeviceData SET IK_DeviceData='"+sizeID+"' WHERE FK_Device="+m_sOrbiterID +
					 " AND FK_DeviceData="+StringUtils::itos(DEVICEDATA_PK_Size_CONST);
			writeLog("Query: " + query, false, LV_DEBUG);
			m_dbPlutoDatabase.quickQuery(query);
		}
	} else {
		writeLog("Aborting, orbiterID is empty");
	}
}
//TODO:Test!
void LM::action_Resolution()
{
	// scheduling AVWizard
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	pConfig->AddString("AVWizardOverride", "1");
	pConfig->WriteSettings();	
	delete pConfig;
	
	writeLog("AVWizard will start on the next reboot.",true);
	//TODO: prompt client to reboot!	
	
	
	//TODO: Need to execute these on client response??
	//if (iResult == QMessageBox::Yes)
	//{
	//	writeLog("Stopping LMCE");	
	//	
	//	if ( m_bMediaHere && m_bMediaRunning )
	//		actionSwitchMedia();
	//		
	//	if ( m_bCoreHere && m_bCoreRunning )
	//		actionSwitchCore();
	//	
	//	rebootPC();
	//}
}
//TODO: test
void LM::actionSaveConnectionsSettings()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	
	pConfig->AddString("DCERouter", m_sCoreIP);
	pConfig->AddString("MySqlHost", m_sMySQLHost);
	pConfig->AddString("MySqlUser", m_sMySQLUser);
	pConfig->AddString("MySqlPassword", m_sMySQLPass);
	pConfig->WriteSettings();
	
	delete pConfig;
	
	writeLog("Saved new settings to "+ string(PLUTO_CONFIG));

	//TODO should we restart anything here?
}
//TODO: move this to its own source file and call the function from here. This will make it easier in the future to change how we get this information. Also, clean up this mess of code!!!!!
void LM::initialize_VideoSettings()
{
	//Lets assume that we can't find the driver information
	m_sVideoDriver = "Unknown";
	if (m_sOrbiterID == "")
		return;

	m_sVideoResolution = m_dbPlutoDatabase.quickQuery("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_sMediaID + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Video_settings_CONST));
	
	//The old Launch Manager used a python script which used the now obsolete xorgconfig module to get the video driver name
	//Let just do it manuall in good ol' c++
	string line;
	string xorgFile;
	ifstream textStream ("/etc/X11/xorg.conf");
	if (textStream.fail())
		return;

	while(getline(textStream,line)){
		line=StringUtils::TrimSpaces(line) + "\n";
		line=StringUtils::Replace(line,"\t","");
		xorgFile += line;
	}
	textStream.close();
	
	//now lets get the correct section
	size_t startPos;

	startPos=xorgFile.find("\nSection \"Device\"\n");
	if (startPos==string::npos)
		return;
	xorgFile = xorgFile.substr(startPos,-1);

	
	startPos=xorgFile.find("\nEndSection\n");
	if (startPos==string::npos)
		return;	
	xorgFile = xorgFile.substr(0,startPos+11);
	
	
	//Now we have the Device section, find the driver!
	startPos=xorgFile.find("\nDriver");
	if (startPos==string::npos)
		return;
	startPos+=7;
	xorgFile=xorgFile.substr(startPos,-1);

	startPos=xorgFile.find("\n");
	if (startPos==string::npos)
		return;
	xorgFile=xorgFile.substr(0,startPos);
	xorgFile=StringUtils::Replace(xorgFile,"\"","");	
	m_sVideoDriver = xorgFile;
}
//COMPLETE
void LM::initialize_AudioSettings()
{
	if (m_sMediaID == "")
		return;

	string sSettings = m_dbPlutoDatabase.quickQuery("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_sMediaID + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Audio_settings_CONST));

	if (sSettings=="")
		return;
	
	if(sSettings.find("3")!=string::npos) {	
		m_bAC3Pass = true;
	} else {
		m_bAC3Pass = false;
	}
		

	if(sSettings=="C"){
		m_sSoundSetting = "SPDIF Coax";
	}else if(sSettings=="O"){
		m_sSoundSetting = "SPDIF TosLink";
	}else if(sSettings=="S"){
		m_sSoundSetting = "Stereo";
	}else if(sSettings=="L"){
		m_sSoundSetting = "Multichannel Analog";
	}else{
		m_sSoundSetting = "Manual/Unknown";
	}
	
}

//TODO: when socket layer is implemented, send a signal to client that tells the user they should reboot...
bool LM::checkCore(string coreIP)
{
	writeLog("Checking if core is up...", true);
	writeOSD("Checking Core. Please Wait.");	
	struct stat fInfo;

	if ( lstat(DCEROUTER_LOCK, &fInfo)==0 )
	{
		writeLog("Found router lock file at " + string(DCEROUTER_LOCK) );

		int iElapsedSeconds=0;

		string cmd = "/bin/nc -z " + m_sCoreIP + " " + m_sDCERouterPort;

		string sRet = "";
		const int checkCoreTimeout = 60; // 60 seconds
		
		while ( (iElapsedSeconds<checkCoreTimeout) && (sRet.size()!=1) )
		{
			writeLog("Attempting to connect to " + m_sCoreIP + ":" + m_sDCERouterPort);
			sRet = system(cmd.c_str());
			
			sleep(1);
			iElapsedSeconds++;
		}
		
		if (sRet.size() != 1)
		{
			writeLog("Timeout waiting for router restart, suggesting user to reboot the system");
			//TODO:send socket message here			
			return false;
		}
		else
		{
			writeLog("Core is running, connected successfully");
			return true;
		}
	}
	else
	{
		writeLog("No router lock file found, core is down");
		return false;
	}
}
//TODO: after socket layer is implemented, send message to clients to perform reload router
bool LM::initialize_LMdevice(bool bRetryForever/*=false*/)
{
	// if we already have a connection
	if (m_pLMCE_Launch_Manager!=NULL)
		return true;
	
	string sCoreIP = m_sCoreIP;
	
	if (sCoreIP == "localhost")
		sCoreIP = "127.0.0.1";
	
	writeLog("Connecting to router at " + sCoreIP, true, LV_STATUS);
	writeLog("Connecting to Core. Please Wait.");

	while (true)
	{

		m_pLMCE_Launch_Manager = new DCE::LMCE_Launch_Manager(atoi(m_sDeviceID.c_str()), sCoreIP,  true, false);//int(m_sDeviceID.c_str())

		bool bConnected = m_pLMCE_Launch_Manager->GetConfig();
		if ( bConnected && m_pLMCE_Launch_Manager->m_pEvent->m_pClientSocket->m_eLastError==DCE::ClientSocket::cs_err_NeedReload )
		{
			//TODO:refactor this part using the socket layer
			writeLog("Please go to an existing Orbiter and choose 'quick reload router'. ", true, LV_WARNING);
			writeLog("This media director will start after you do...", true, LV_WARNING);
			writeOSD("Please go to an existing Orbiter and choose 'quick reload router'");

			//Display a message on the screen that a reload is necessary.
			string sReloadMessage = "This Media Director is now ready to go, but first you must reload the router.";
//			DCE::CMD_XPromptReload cmd( atoi(m_sDeviceID.c_str()),  atoi(m_sOrbiterID.c_str()), sReloadMessage );
//			m_pLMCE_Launch_Manager->SendCommandNoResponse(cmd);
			sleep(30);

			writeLog("initialize_LMdevice: router should be reloaded, retrying connect", false, LV_WARNING);
//			delete m_pLMCE_Launch_Manager;
//			m_pLMCE_Launch_Manager = NULL;
		}
		else if( bConnected && m_pLMCE_Launch_Manager->Connect(m_pLMCE_Launch_Manager->PK_DeviceTemplate_get()) ) 
		{
			writeLog("Connected OK", true, LV_STATUS);
			
			map<int,string> devicesMap;
			m_pLMCE_Launch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &devicesMap);
			if (devicesMap.empty())
			{
				writeLog("Connected, but failed to find Orbiter plugin", false, LV_WARNING);
			}
		
			map<int,string>::iterator it = devicesMap.begin();
			m_sOrbiterPluginID = StringUtils::itos((*it).first) ;
			m_pAlarmManager->AddRelativeAlarm(5,this,LM_KEEP_ALIVE,NULL);

			// Link the DCE device to our UI
			m_pLMCE_Launch_Manager->lmWidget = this;

			return true;
		}
		
		delete m_pLMCE_Launch_Manager;
		m_pLMCE_Launch_Manager = NULL;
		
		writeLog("initialize_LMdevice: Connect failed", true, LV_WARNING);
		
		if (!bRetryForever)
		{
			writeLog("initialize_LMdevice: Not retrying", true, LV_WARNING);

			return false;
		}
		else
		{
			writeLog("initialize_LMdevice: Retrying (forever)", true, LV_WARNING);
		}
	}
}

//COMPLETED
void LM::updateScripts()
{
	//Put all scripts we want to run on boot and each reload here
	writeOSD("Detecting Serial Ports");
	writeLog("Running UpdateAvailableSerialPorts.sh", true, LV_STATUS);
	string sCmd = "/usr/pluto/bin/UpdateAvailableSerialPorts.sh";
	exec_system(sCmd,true);

	writeOSD("Detecting Sound Cards");
	writeLog("Running UpdateAvailableSoundCards.sh", true, LV_STATUS);
	string sCmd2="/usr/pluto/bin/UpdateAvailableSoundCards.sh";
	exec_system(sCmd2,true);

	writeOSD("Detecting Soft Squeeze Devices");
	writeLog("Running UpdateSqueezeDevices.sh", true, LV_STATUS);
	string sCmd3="/usr/pluto/bin/UpdateSqueezeDevices.sh";
	exec_system(sCmd3,true);

	writeLog("Process completed.");
}

//TODO: Do I really need a custom list type? It was used to make porting easier, but maybe using a vector would be best.. Also general cleanup of code...
void LM::syncWithLockList(bool eraseDeadLocalDevices)
{
	writeLog("Reading lockfile");
	List LockList;

	ifstream textStream (string(PLUTO_LOCKS).c_str());
	if (!textStream.fail())
	{
		string sID;
		int iRet;
		while ( getline(textStream,sID) )
		{
			sID=StringUtils::Replace(sID," ","");
			sID=StringUtils::Replace(sID,"\n","");
			sID=StringUtils::Replace(sID,"\t","");				
			if (sID != "") 
			{
				LockList.append(sID);
			}
		}
		
		textStream.close();

		writeLog("Detected running devices: " + LockList.join(" "));
	}
	else
	{
		writeLog("Failed to open file "+ string(PLUTO_LOCKS), false, LV_WARNING);
		writeLog("Continuing, assuming no devices are running", false, LV_WARNING);
	}
	
	// purging dead devices
	if (eraseDeadLocalDevices)
	{
		vector<string> newV;	
	// purging dead devices
		
		// cleaning up dead core devices
		//for (vector<int>::iterator it=m_vCoreDevices.begin(); it!=m_vCoreDevices.end(); ++it) //TODO: learn how to use iterators properly and refactor the for clause...
		for(unsigned int i=0;i<m_vCoreDevices.size();i++)
		{
			string sID = m_vCoreDevices[i];
			if ( !LockList.contains(sID) )
			{
				writeLog("Device " + sID + " seem to be dead" );
			}
			else
				newV.push_back(sID);
		}
		
		m_vCoreDevices = newV;
		newV.clear();
		
		// cleaning up dead core devices
		//for (QValueVector<int>::iterator  it=m_qvvMediaDevices.begin(); it!=m_qvvMediaDevices.end(); ++it)//TODO: see above
		for(unsigned int i=0;i<m_vMediaDevices.size();i++)
		{
			string sID = m_vMediaDevices[i];
			if ( !LockList.contains(sID) )
			{
				writeLog("Device " + sID + " seem to be dead");
			}
			else
				newV.push_back(sID);
		}
		
		m_vMediaDevices  = newV;	
	// purging dead devices
		newV.clear();
	}

}
//TODO: see todo's below.
void LM::startCoreDevices(bool checkForAlreadyRunning)
{
	string s;
	DBResult dbrCoreDevices;

	m_iDevicesLevel = 0;
//	m_pDevicesUpdateTimer->start(1000); TODO: Need to add an Alarm to do the same thing....
	writeLog("startCoreDevices()");
	writeOSD("Starting Core Devices");

	writeLog("startCoreDevices() - calling Config_Device_Changes.sh");
	string sCmd = "/usr/pluto/bin/Config_Device_Changes.sh";
	exec_system(sCmd,true);


	if ( m_dbPlutoDatabase.connected() ) {
		// fetching list of non-MD devices under Core
		List devices;
		
		devices.append(m_sCoreDeviceID);
		
		// filtering out the MD/Hybrid PC
		string extraCondition = "";
		if (m_sMediaID!="")
			extraCondition += " AND PK_Device<>" + m_sMediaID;
		  
		//for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)//TODO: learn iterators and refactor...
		for( unsigned int i=0;i<devices.size();i++ )
		{
			//int i=0; //debug
			string query = "SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE, Device.FK_DeviceTemplate,DeviceTemplate.IsPlugin FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE (PK_Device=" + devices.index(i) + extraCondition + ")";
						
			dbrCoreDevices=m_dbPlutoDatabase.query(query);
		
			// if device exist
			while (dbrCoreDevices.next())
			{
				if (dbrCoreDevices.value(3)=="1")
					writeLog("Not starting device " + dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1) + " - it is disabled", true);
				else if (dbrCoreDevices.value(6)=="1")
					writeLog("Not starting device " + dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1) + " - it is a plugin", true);
				else
				{			
									
					//if device implements DCE, start it
					if (dbrCoreDevices.value(4)=="1")
					{
						bool alreadyRunning=false;
						
						if (checkForAlreadyRunning)
							//for (QValueVector<int>::iterator jt=m_qvvCoreDevices.begin(); jt!=m_qvvCoreDevices.end(); ++jt)//TODO: Learn iterators and refactor
							for (int j=0;j<m_vCoreDevices.size();j++)
							{
								if ( m_vCoreDevices[j] == dbrCoreDevices.value(0) )
								{
									alreadyRunning = true;
									break;
								}
							}
						
						if (alreadyRunning)
						{
							writeLog("Not starting device " +  dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1) + " - it is already running", true);
						}
						else
						{
							string deviceCommand = dbrCoreDevices.value(2);
							
							if (deviceCommand=="")
								deviceCommand=StringUtils::Replace(dbrCoreDevices.value(1)," ","_");//TODO: remove leading and trialing whitespace, as well as simplifying multiple spaces to a single space before replacing with "_"

							if (!FileUtils::FileExists("/usr/pluto/bin/"+deviceCommand))
							{
								writeLog("Not starting device " +  dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1) + " -  binary \"" + deviceCommand + "\" is not found.", true, LV_WARNING);
								continue;
							}
							//TODO: Add a case for the executable not having the executable bit set before running....
							string screenName = dbrCoreDevices.value(1);
							screenName = FileUtils::ValidCPPName(screenName + "-" + dbrCoreDevices.value(0));

							string sCmd = "/usr/bin/screen -d -m -S " + screenName;
							sCmd += " /usr/pluto/bin/Spawn_Device.sh " + dbrCoreDevices.value(0) + " " + m_sCoreIP + " " + deviceCommand;
							//writeLog("Command string: " + sCmd,true);//TODO REMOVE DEBUG
							exec_system(sCmd,true);
							writeLog("Starting device " +  dbrCoreDevices.value(0) + " " + dbrCoreDevices.value(1),true);// + ": " + args.join(" "));//TODO make a join method for custom list type
							writeOSD("Starting device " +  dbrCoreDevices.value(0) + " " + dbrCoreDevices.value(1));
							// wait for the device to register
//							waitForDevice(atoi(dbrCoreDevices.value(0).c_str()));
							
							// recording device in list
							m_vCoreDevices.push_back(dbrCoreDevices.value(0));
						}
					}
					else
					{
						writeLog("Not starting device " +  dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1) + " - it doesn't implement DCE. Adding its children into start queue instead", false);
						
						// adding child devices of this device - maybe some of them implement DCE
						string child_query = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=" + dbrCoreDevices.value(0);
						string children = "[ ";
						DBResult dbrChildren;
						dbrChildren = m_dbPlutoDatabase.query(child_query);

						while( dbrChildren.next() )

						{
							devices.append(dbrChildren.value(0));
							children += dbrChildren.value(0) + " ";
						}
						children += "]";
						writeLog("Children list: " + children, false);
						
						
					}
				}
			}
		}
	}
	else
	{
		writeLog("Failed to query MySQL DB");
	}
}

//TODO: see todo's below
void LM::startMediaDevices(bool checkForAlreadyRunning)
{
	string s;
	DBResult dbrMediaDevices;
	if (m_sMediaID=="")
		return;
	
	m_iDevicesLevel = 0;
//	m_pDevicesUpdateTimer->start(1000);//TODO: Should I implement an alarm here, to update progress more??

	writeLog("startMediaDevices()");
	writeOSD("Starting Media Director Devices");
	if ( m_dbPlutoDatabase.connected()  ) {
		// fetching full list of devices under current MD
		List devices;
		devices.append(m_sMediaID);
		//for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)//TODO:learn iterators and refactor?
		for( unsigned int i=0;i<devices.size();i++ )
		{
			string query = "SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE  PK_Device=" + devices.index(i);
			
			dbrMediaDevices=m_dbPlutoDatabase.query(query);

			// if device exist
			if(dbrMediaDevices.next())
			{
				if (dbrMediaDevices.value(3)=="1")
					writeLog("Not starting device " +  dbrMediaDevices.value(0) + " "  +dbrMediaDevices.value(1) + " - it is disabled", true);
				else
				{
					// starting device if it implements DCE
					if (dbrMediaDevices.value(4)=="1")
					{
						bool alreadyRunning=false;
						
						if (checkForAlreadyRunning)
							//for (QValueVector<int>::iterator jt=m_qvvMediaDevices.begin(); jt!=m_qvvMediaDevices.end(); ++jt)//TODO:learn iterators and refactor??
							for (int j=0;j<m_vMediaDevices.size();j++)							
							{
								if ( m_vMediaDevices[j] == dbrMediaDevices.value(0) )
								{
									alreadyRunning = true;
									break;
								}
							}
						
						if (alreadyRunning)
						{
							writeLog("Not starting device " +  dbrMediaDevices.value(0) + " "  + dbrMediaDevices.value(1) + " - it is already running", true);
						}
						else
						{
							string deviceCommand = dbrMediaDevices.value(2);
												
							
							if (deviceCommand=="")
								deviceCommand=StringUtils::Replace(dbrMediaDevices.value(1)," ","_");
								//deviceCommand = dbrMediaDevices.value(1)).simplifyWhiteSpace().replace(" ","_");//TODO: remove leading and trialing whitespace, as well as simplifying multiple spaces to a single space before replacing with "_"

							if (!FileUtils::FileExists("/usr/pluto/bin/"+deviceCommand))
							{
								writeLog("Not starting device " +  dbrMediaDevices.value(0) + " "  + dbrMediaDevices.value(1) + " - binary \"" +deviceCommand+"\" is not found.", true, LV_WARNING);
								continue;
							}
							
							

							string screenName = dbrMediaDevices.value(1);
							screenName = FileUtils::ValidCPPName(screenName + "-" + dbrMediaDevices.value(0));

							string sCmd = "/usr/bin/screen -d -m -S " + screenName;
							sCmd += " /usr/pluto/bin/Spawn_Device.sh " + dbrMediaDevices.value(0) + " " + m_sCoreIP + " " + deviceCommand;
							//writeLog("Command string: " + sCmd,true);//TODO REMOVE DEBUG
							exec_system(sCmd,true);

							writeLog("Starting device " +  dbrMediaDevices.value(0) + " " + dbrMediaDevices.value(1),true);// + ": " + args.join(" "));
							writeOSD("Starting device " +  dbrMediaDevices.value(0) + " " + dbrMediaDevices.value(1));
							// wait for device to register
//							waitForDevice(atoi(dbrMediaDevices.value(0).c_str()));
							
							// recording device in list
							m_vMediaDevices.push_back(dbrMediaDevices.value(0));

						}
					}
					else
					{
						writeLog("Not starting device " +  dbrMediaDevices.value(0) + " "  + dbrMediaDevices.value(1) + " - it doesn't implement DCE. Adding its children into start queue instead", false);
						// starting child devices recursively
						string child_query = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=" + devices.index(i);
						string children = "[ ";
						DBResult dbrChildren;
						dbrChildren = m_dbPlutoDatabase.query(child_query);
						while(dbrChildren.next())
						{
							devices.append(dbrChildren.value(0));
							children += dbrChildren.value(0) + " ";
						}
						children += "]";
						writeLog("Children list: " + children, false);
					}
				}
			}
		}
		
	}
	else
	{
		writeLog("Failed to query MySQL DB");
	}
}


//TODO:see below
void LM::findRunningMediaDevices()
{
	if (m_sMediaID=="")
		return;

	writeLog("Searching for running media devices...");
	
	if ( m_dbPlutoDatabase.connected() ) {
		string runningDevices;
		// fetching full list of devices under current MD
		List devices;
		devices.append(m_sMediaID);
		//for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)//TODO: learn iterators and refactor?
		for (int i=0; i<devices.size();i++)
		{
			string query = "SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE  PK_Device=" + devices.index(i);
			DBResult res;
			res= m_dbPlutoDatabase.query(query);
			
			// if device exist
			if ( res.next() )
			{
				if (res.value(3)!="1")
				{
					// checking device if it implements DCE
					if (res.value(4)=="1")
					{
						writeLog("Checking device " +  res.value(0) + " "  + res.value(1), false);
						char cStatus = m_pLMCE_Launch_Manager->DeviceIsRegistered(atoi(res.value(0).c_str()));
						if (cStatus == 'Y')
						{
							m_vMediaDevices.push_back(res.value(0));
							runningDevices += res.value(0) + " ";
						}
					}
					else
					{
						writeLog("Not checking device " +  res.value(0) + " "  + res.value(1) + " - it doesn't implement DCE. Adding its children into check instead", false);
						// adding devices recursively
						string child_query = "SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=" + devices.index(i);
						string children = "[ ";
						DBResult childRes;
						childRes = m_dbPlutoDatabase.query(child_query);
						
						while( childRes.next() )
						{
							devices.append(childRes.value(0));
							children += childRes.value(0) + " ";
						}
						children += "]";
						writeLog("Children list: " + children, false);
					}
				}
			}
		}
		
		writeLog("Running media devices found: " + runningDevices, true);
	}
	else
	{
		writeLog("Failed to query MySQL DB");
	}
}

//TODO: This doesn't work because the function needs over-ridden....
void LM::flushLog()
{
	DCE::LoggerWrapper::GetInstance()->Flush();
}
//TODO:REfactor after socket layer is in place
void LM::writeLog(string s, bool toScreen, int logLevel)
{
	DCE::LoggerWrapper::GetInstance()->Write(logLevel, s.c_str());
	if (toScreen)
	{
		m_uiMainUI.writeLog(s);
	}	
}

void LM::writeOSD(string s)
{
	string command = "/usr/pluto/bin/BootMessage.sh \"" + s + "\"";
	system(command.c_str());
}

//TODO: refactor after socket layer is in place
void LM::appendLog(string s)
{
	//appends the most recent log entry in the UI for dynamic display purposes
	m_uiMainUI.appendLog(s);
}


//TODO:see toso below
void LM::doAutostart()
{
	writeLog(">>Performing autostart if configured..", true);
	writeOSD("Loading LinuxMCE. Please Wait.");
	m_bDelayUpReport = false;
	
	bool bReport = false;
	
	// checking for core and starting it
	if(m_bCoreHere && (m_sAutostartCore == "1" || false) )
	{
		if (!m_bCoreRunning)
		{
			writeLog(">>Autostarting core....", true);
			if (startCore())
			{
				m_bCoreRunning = true;
				bReport = true;
			}
		}
		else
			writeLog(">>Not autostarting core - it is already running", true);
	}
	if ( ( (m_bCoreHere &&m_bCoreRunning) || !m_bCoreHere) && m_bMediaHere && (m_sAutostartMedia=="1" || false) )
	{
		if (!m_bMediaRunning)
		{
			checkScreenDimensions(false);//TODO: This needs re-implemented..  Its purpose is to regen orbiters if screen resolution is different than waht is listed in xorg.conf... Is this even needed?
			writeLog(">>Autostarting media station....", true);
			m_bMediaRunning = startMediaStation();
			bReport |= m_bMediaRunning;
		}
		else
			writeLog(">>Not autostarting media station - it is already running", true);
	}
	
	if (bReport) {
		if (m_bDelayUpReport) {
			m_pAlarmManager->AddRelativeAlarm(30,this,REPORT_DEVICE_UP,NULL);
		} else {
			reportDeviceUp();
		}
	}
}

//TODO: Clean this up.. Also, not too sure yet if LM_PROCESS_LOG is working like it should.....
bool LM::startCoreServices()
{
	
	writeLog("Starting process " + string(START_CORE_SCRIPT), true, LV_WARNING);
	writeOSD("Starting Core Services. Please be patient.");
	writeLog("Please be patient. This may take a while...", true);
	
	
	string log = LM_PROGRESS_LOG;
	string cmd = "echo -n > " + log; //clear log
	system(cmd.c_str());
	

	ifstream textStream (LM_PROGRESS_LOG);
	
	if (textStream.fail())
	{
		writeLog("Failed to open file :" + string( LM_PROGRESS_LOG), false, LV_WARNING);
	}

	int startCoreScript = exec_system(START_CORE_SCRIPT,false);

	if ( started(startCoreScript) )
	{
		bool bOkDB = m_dbPlutoDatabase.connected();
		m_iProgress = 0;
		m_bShowProgress = true;
		
		string currOrbiterID="";

		int iCounter = CORE_SERVICE_TIMEOUT;
		int iQueryCounter=5; // trigger on start
		while (isRunning(startCoreScript) && iCounter--)  //TODO: Should this be done with the orbiter regen tracking function???
		{
			// every second checking config log messages //TODO: is this extra information from LM_PROGRESS_LOG even useful or needed?? As of now, it is overwritten by the appendLog calls...
			string s, s1;
			while(getline(textStream,s))
			{
				if(s!="") {				
					writeLog("+Progress message: " + s, true);
				}
			}
						
			// every 5 seconds checking orbiters
			if (iQueryCounter>=5)
			{
				iQueryCounter = 0;
				
				// retrieve orbiter progress
				if (bOkDB)
				{
					string sQuery = "select PK_Orbiter, RegenPercent,RegenStatus,Regen from Orbiter where RegenInProgress=1 ORDER BY PK_Orbiter";
					DBResult res = m_dbPlutoDatabase.query(sQuery);
					string O_ID;
					string O_Message;
					int O_Progress = 100; 
					
					// some orbiter is being regen-ed
					if (res.next())
					{
						O_ID = res.value(0);
						O_Progress = atoi(res.value(1).c_str());
						
						//correct for incorrect calculations above 100%
						if (O_Progress>100) 
							O_Progress = 100;

						O_Message = "Orbiter #"+O_ID+"\n"+res.value(2);
						
						if (O_ID != currOrbiterID) 
						{
							//Don't append the log on the first time through
							if(currOrbiterID!="") {
								appendLog("Regenerating Orbiter " + currOrbiterID +" - Finished.");
								writeOSD("Regenerating Orbiter " + currOrbiterID + " - Finished");
							}
							currOrbiterID = O_ID;
							writeLog("Regenerating Orbiter " + O_ID +" - starting...",true);
							writeOSD("Regenerating Orbiter " + O_ID +" - starting");	
						} else {
							appendLog("Regenerating Orbiter " + O_ID + " - "+StringUtils::itos(O_Progress) + "% ("+res.value(2)+")");
							writeOSD ("Regenerating Orbiter " + O_ID + " - "+StringUtils::itos(O_Progress) + "% ("+res.value(2)+")");
						}
						m_iProgress = O_Progress;
					}
				} else {
					writeLog("Error. No database connection to track orbiter progress.",true,LV_CRITICAL);
				}
			}
			
			sleep(1);
			iQueryCounter++;
			
		}
		

		textStream.close();
		m_iProgress = 0;
		m_bShowProgress = false;

		if (isRunning(startCoreScript))
		{
			writeLog("Process didn't exit after " + StringUtils::itos(CORE_SERVICE_TIMEOUT) + " secs of running, let it run in background");
			return true;
		}
		else
		{
			writeLog("Process completed.");
			return true;
		}
	}
	else
	{
		writeLog("Failed to start process " + string(START_CORE_SCRIPT), true, LV_WARNING);
		return false;
	}
}

//TODO: a lot of cleanup and TODO's below
bool LM::startMediaStation()
{
	updateScripts();

	writeOSD("Loading LinuxMCE. Please Wait.");

	// retrying forever, as KeepAlive timer is not running yet
	bool bLMinit = initialize_LMdevice(true);
	if (!bLMinit)
	{
		writeLog("Failed to connect to DCERouter!", true, LV_WARNING);
		return false;
	}

	if ( m_sOrbiterID != "") {
		writeLog("Checking if Orbiter skin is OK");
		if ( !confirmOrbiterSkinIsReady() )
		{
			writeLog("Failed to regenerate skin for Orbiter!!", true, LV_WARNING);
			return false;
		}
		else
	        {
	                string status = getOrbiterStatus();
	                bool bRouterReloaded = false;
	                // repeat while status is "N"
	                while (status != "O")
	                {
	                    // firing reload event //TODO: should this be implemented through socket once socket layer is implemented??
	                    if (!bRouterReloaded)
	                       actionReloadRouter();

	                    writeLog("Trying to reload router to use new skin for this Orbiter, please wait (giving router 10 seconds to reload)...", true);

	                    // waiting 10 seconds
	                    int iCount=10;
	                    while (iCount--)
	                    {
	                        sleep(1);
	                    }

	                    // handling disconnection from router, the media station is not running,
	                    // so it may not auto-reconnect
	                    if (!m_pLMCE_Launch_Manager)
	                    {
	                        // assuming we were disconnected because of router reload
	                        bRouterReloaded = true;
	                        initialize_LMdevice();
	                    }

	                    status = getOrbiterStatus();
	                    if (status != "O" && status != "N" && status !="!")
	                    {
	                        writeLog("Received unknown status for this Orbiter, aborting Media Station start!", true, LV_WARNING);
	                        writeLog("Status was: "+status, true, LV_WARNING);
	                        return false;
	                    }
	                }

	                writeLog("Check completed");
	        }
	}
	// starting media services
	startMediaDevices();

	// jumping to orbiter
	if ( m_sOrbiterID != "")
		jumpToOrbiter();

	return true;
}
//TODO:decide if this is needed and port if it is
bool LM::checkScreenDimensions(bool askUser/*=true*/)
{

	if (m_pDisplay != NULL)
	{
		int nSizes; 	// Number of returned screen sizes. (resolutions)
		int have_pixel_size = 0;
		Rotation rotation, current_rotation, rotations;
	
		SizeID current_size = XRRConfigCurrentConfiguration (m_pSc, &current_rotation);
		int current_rate = XRRConfigCurrentRate(m_pSc);
		m_pSizes = XRRConfigSizes(m_pSc,&nSizes);
	
		int current_width = DisplayWidth(m_pDisplay,m_iScreen);
		int current_height = DisplayHeight(m_pDisplay,m_iScreen);
	
	
		if (!m_pDisplay) 
		{
			return true;	// If X is not running, then just ignore this whole funnction.
		}
	
		return true;	// for now until i finish this sucker.
	}
	return true;  // added to remove compiler warning fn is not currently used
}

//TODO: A lot of cleanup and hook into socket once socket layer is complete
void LM::updateOrbiterRegenProgress()
{
	// if we are tracking current Orbiter only, we are asking Orbiter plugin for status, otherwise we are doing DB calls
	if (m_bRegenTrackingCurrentOrbiterOnly)
	{
		if (!m_bMediaHere || m_sOrbiterID=="")
		{
			writeLog("Not waiting for OrbiterGen - there is no Orbiter at this PC", true);
			return;
		}
		
		if (!m_pLMCE_Launch_Manager)
		{
			writeLog("Cannot get OrbiterGen status - connection problems", true, LV_WARNING);
			return;
		}
	
		int iRegenProgress;
		string sOrbiterStatus = getOrbiterStatus(iRegenProgress);
		
		if (sOrbiterStatus=="R" || sOrbiterStatus=="r")
		{
			m_bShowProgress = true;
			m_iProgress = iRegenProgress;
			
			
			DBResult res = m_dbPlutoDatabase.query("select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1 AND PK_Orbiter="+m_sOrbiterID);
			if(res.next())
			{
				string statusMessage = "Orbiter " + res.value(0)+ "\n"+res.value(2);
				//TODO: appendLog??
				appendLog("Regenerating orbiter " +res.value(0)+"..."+res.value(1)+"%");
				writeOSD("Regenerating Orbiter "+res.value(0)+" "+res.value(1)+"%");
				//tlStatusMessages->setText(statusMessage);
				//tlStatusMessages->show();
			}
			m_pAlarmManager->AddRelativeAlarm(5,this,UPDATE_ORBITER_REGEN_PROGRESS,NULL);
			//writeLog("Waiting for OrbiterGen to complete", false);
		}
		else
		{
			m_bShowProgress = false;

			//m_pProgressBarUpdateTimer->stop();
			
			//tlStatusMessages->hide();
			//tlStatusMessages->setText("");QString lmce_launch_managerWidget::getOrbiterStatus(const QString &orbiterID, int &iProgressValue)  //TODO: where to display getOrbiterStatus????

			//writeLog(getOrbiterStatus(string &orbiterID,&iProgressValue)); //TODO - why doesn't this work?
			writeLog("OrbiterGen run completed", true);
			writeOSD("Orbiters were regenerated.");
			
			m_bRegenInProgress = false;
		}
	}
	else		// directly quering DB for 'average progress' for all orbiters
	{
		string regenProgress = m_dbPlutoDatabase.quickQuery("SELECT RegenPercent FROM Orbiter WHERE RegenInProgress=1");
		string activeRegens = m_dbPlutoDatabase.quickQuery("SELECT COUNT(*) FROM Orbiter WHERE Regen=1 OR Regen=2");
		
		if (atoi(activeRegens.c_str())>0)
		{
			//pbProgress->show(); //TODO uncomment and decide where to show
			//if (regenProgress != NULL)//QString::null)
				//pbProgress->setProgress(regenProgress.toInt());
			
			//QString sQuery = "select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1";
			//QSqlQuery q;
			//bool bQueryExecResult = performQueryDB(sQuery, q);
			DBResult res = m_dbPlutoDatabase.query("select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1");
			//if ( bQueryExecResult && q.next() )
			if(res.next())
			{
				string statusMessage = "Orbiter " + res.value(0) + "\n"+res.value(2);
				//TODO: where to display this??
				appendLog("Regenerating orbiter " +res.value(0)+"..."+res.value(1)+"%");
				writeOSD("Regenerating Orbiter "+res.value(0)+" "+res.value(1)+"%");
				//tlStatusMessages->setText(statusMessage);
				//tlStatusMessages->show();
			}
			m_pAlarmManager->AddRelativeAlarm(5,this,UPDATE_ORBITER_REGEN_PROGRESS,NULL);
			//writeLog("Waiting for OrbiterGen to complete", false);
		}
		else
		{
			m_bShowProgress = false; //pbProgress->hide();

			//m_pProgressBarUpdateTimer->stop();
			//tlStatusMessages->setText("");
			//tlStatusMessages->hide();
			
			writeLog("OrbiterGen run completed ", true);
			writeOSD("Orbiters were Regenerated.");
			
			m_bRegenInProgress = false;
		}
	}
}




//TODO: Anything we want to process while we're in here??
void LM::Run()
{
	//schedule initialization to run
	//m_pAlarmManager->AddRelativeAlarm(1,this,INITIALIZATION,NULL);

	while(true) {
		char c = getch();  //TODO: use timeout version if we ever have to do other things in main loop

		if(c=='1') {
			//Reload Router
			writeLog("Received command to reload the router...",true);
			actionReloadRouter();
		} else if(c=='2') {
			//Regen Orbiter
			writeLog("Received command to regen this orbiter...",true);
			triggerOrbiterRegen(false);
			startOrbiterRegenProgressTracking();
		} else if(c=='3') {
			//Regen All Orbiters
			writeLog("Received command to regen ALL orbiters...",true);
			triggerOrbiterRegen(true);
			startOrbiterRegenProgressTracking();
		} else if(c=='4') {
			//Change Resolutions (schedule A/V setup wizard for next reboot)
			writeLog("Received command to change resolutions...",true);
			action_Resolution();
		} else if(c=='5') {
			//Toggle Remote Assistance
			writeLog("Received command to toggle RA. COMMAND NOT YET IMPLEMENTED.",true);
			//Hell, just for fun, lets fake this one for now
			if(m_bRemoteAssistanceRunning) {
				m_bRemoteAssistanceRunning = false;
			} else {
				m_bRemoteAssistanceRunning = true;
			}
			syncUI();
			m_uiMainUI.draw();
		} else if(c=='6') {
			//Toggle Autostart Core
			//TODO: make separate function to crate a new config, set the plag and write it out to pluto.conf
			writeLog("Received command to toggle Autostart Core. COMMAND NOT YET IMPLEMENTED.",true);
		} else if(c=='7') {
			//Toggle Autostart Media
			//TODO: make separate function to crate a new config, set the plag and write it out to pluto.conf
			writeLog("Received command to toggle Autostart Media. COMMAND NOT YET IMPLEMENTED.",true);
		} else if(c=='8') {
			//Reboot
			writeLog("Received command to reboot...",true);
			rebootPC(); //TODO - confirmation!! (handled here or in rebootPC()?)
		} else {
			writeLog("Unrecognized Command.",true);
		}
		sleep(1); //TODO: get rid of this cheap hack and record as last_char for comparison at the top of loop for a once-per-click effect
	}

	return;
}
















void LM::syncUI() 
{
	m_uiMainUI.setCoreIP(m_sCoreIP);
	m_uiMainUI.setVideoDriver(m_sVideoDriver);
	m_uiMainUI.setVideoResolution(m_sVideoResolution);	
	m_uiMainUI.setMySQLInfo(m_sMySQLHost, m_sMySQLUser, m_sMySQLPass);
	m_uiMainUI.setAudioInfo(m_sSoundSetting, m_bAC3Pass);
	m_uiMainUI.setStatus(m_sAutostartCore, m_sAutostartMedia, m_bRemoteAssistanceRunning);
	m_uiMainUI.draw();


}


//////////////////////////////////////////////
//TODO: move process methods to a new class!
int LM::exec_system(std::string cmd, bool wait) {
	
        char command[1024];
	char *args[512];
        int fork_res;
	int arg_count;
	std::string::size_type pos;
	std::string token;
	bool done;

        sprintf(command,cmd.c_str());

	// Parse the arguments
	pos=0;
	done=false;
	arg_count=0;

	while (!done) {
	    token=StringUtils::Tokenize(cmd," ",pos);

	    if (token!="") {
		args[arg_count]=strdup(token.c_str());
		arg_count++;
	    }

	    if (pos>=cmd.length()) {
		done=true;
	    }
	}

	args[arg_count]=0; // NULL terminte the arg list

        //writeLog("exec_system: args parsed like this:",false,LV_STATUS);
//	for (int i=0; i<arg_count; i++) {
             //writeLog("exec_system: arg " + StringUtils::itos(i) +  " is " + args[i]);
//	}

        fork_res=fork();

        // Child thread
        if (fork_res==0) {
           int retval;
           //writeLog("exec_system: within child fork",false,LV_STATUS);
	   //close terminal output!!!(stdin,stdout,stderr - 0,1 and 2)
	   close(0); close(1); close(2);
	   retval=execvp(args[0],args);

           if (retval<0) {
               writeLog("exec_system: child exec returned res = " + StringUtils::itos(retval),false,LV_WARNING);
           }
           exit(0);
        }

        if (fork_res<0) {
            writeLog("exec_system: fork failed!",false,LV_CRITICAL);
        } else {
            //writeLog("exec_system: fork ok",false,LV_STATUS);

            if (wait) {
                int wait_res;
                writeLog("exec_system: waiting for child process " + StringUtils::itos(fork_res) + " to finish",false,LV_STATUS);

                wait_res=waitpid(fork_res,NULL,0);

                if (wait_res<0) {
                   writeLog("exec_system: wait_res returned an error! res="+StringUtils::itos(wait_res),false,LV_WARNING);
                } else if (wait_res==fork_res) {
                    writeLog("exec_system: detected end of child process OK",false,LV_STATUS);
                }
            }
        }
	return fork_res; //return the pid
    }

bool LM::started(int iPID)
{
	if (iPID > 0)
		return true;
	else
		return false;
}

bool LM::isRunning(int iPID)
{
	int pid;
	int status;

	if ((pid=waitpid(iPID,&status,WNOHANG)) == -1) {
		//error
	} else if (pid == 0) {
		//still running!
		return true;
	} else {
		//did not exit successfully (but exited??)
		return true;
	}
	return false;

}
