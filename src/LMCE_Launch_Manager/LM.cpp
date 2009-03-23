//TODO
//[ ]initloglevels method, as well as the member map for converting them to the string equivalent
//[ ]logLevel screen to add/remove log levels and write to pluto.conf
//[ ]Rotate Logs button and methods
//[ ]btnSwitchCore_clicked() and related methods to stop/start LMCE
//[ ]btnRegenThisOrbiter_clicked()
//[ ]btnReloadRouter_clicked()
//[ ]btnRegenAllOrbiters_clicked()
//[ ]startCoreServices() and media services

#include "LMCE_Launch_Manager.h"
#include "LM.h"
#include "List.h"
#include "DB.h"
#include "UI.h"
#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"
#include "DCE/ClientSocket.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/ProcessUtils.h"
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

//
// The primary constructor when the class is created as a stand-alone device
LM::LM()
{
	m_pLMCE_Launch_Manager=NULL;
	//m_pPlutoDatabase=NULL;
	m_pResult=NULL;
	m_pAlarmManager = NULL;
}

LM::~LM()

{
	delete m_pAlarmManager;
	m_pAlarmManager=NULL;
}
void LM::Initialize()
{
	 m_pAlarmManager = new DCE::AlarmManager();
	m_pAlarmManager->Start(2);      //4 = number of worker threads
	DCE::LoggerWrapper::SetType(LT_LOGGER_FILE, PLUTO_LOG_DIR "/LaunchManager.log");
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
	//writeLog("=>initialize_ViewLog()"); //<=====Not going to implement in new version, no need to.
	//initialize_ViewLog();
	
	//writeLog("=>initialize_ConfigurationFiles()");  //<======Not going to implement here, re-implement it in the web admin.
	//initialize_ConfigurationFiles();

	// after all is read, initializing server
	writeLog("=>initialize_Start()");
	initialize_Start();
	
	// if the system was already running when we started, let's catch new children and report we are up
	if ( m_bCoreRunning || m_bMediaRunning )
	{
        	updateScripts();
		respawnNewChildren();
		reportDeviceUp();
	}
	
	writeLog("=>loadSettings()");
	// reding autostart settings
	loadSettings();
	
	writeLog("Started!");

	m_pAlarmManager->AddRelativeAlarm(1,this,DO_AUTOSTART,NULL);

	

}

//void LM::writeLog(string s, bool toScreen, int logLevel)
//{
//	if (toScreen)
//	{
//		string timestamp = "Today";//QDateTime::currentDateTime().toString();
//		//cout << timestamp + " " + s << endl;
//		//lbMessages->update();
//		//doRedraw();
//	}
//
//	cout << s << endl;
//	DCE::LoggerWrapper::GetInstance()->Write(logLevel, s.c_str());
//}


void LM::initialize_Connections()
{
	// initing connections
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

	if (m_sDeviceID!="")
	{
		string localOrbiterID = m_dbPlutoDatabase.quickQuery("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + mdID + " AND FK_DeviceCategory IN (2, 3, 5)");
		m_sOrbiterID = localOrbiterID;
	}

	m_sMediaID = mdID;
	
	delete pConfig;
}





bool LM::checkRAStatus()
{
	return getRAid() != "";
}

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

bool LM::openDB()
{
	//information from:
	//low-level mysql tutorial at http://c-programming.suite101.com/article.cfm/using_a_mysql_databases_with_c
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
/*
string LM::queryDB(string query,bool getFirstResult)
{
	string sResult;
	if(m_pResult!=NULL)
		mysql_free_result(m_pResult);//free last result
	

	
		
	if ( mysql_query(m_pPlutoDatabase,query.c_str())==0 )
	{
		m_pResult = mysql_store_result(m_pPlutoDatabase);
		if (getFirstResult==true) {
			if(nextResultDB())
				sResult = valueDB(0);
			else
				writeLog("Query: \"" + query +"\" returned no records", false, LV_DEBUG);
		}
	}
	else
	{
		writeLog("Query: \"" + query +"\" failed", false, LV_DEBUG);
	}
	
	return sResult;
}
bool LM::nextResultDB()
{
	if (m_mysqlRow = mysql_fetch_row(m_pResult))
		return true;
	else
		return false;
}
string LM::valueDB(int i)
{
	return m_mysqlRow[i];
}
*/
/*
//WILL NOT RE-IMPLEMENT THIS, IT IS NOT NEEDED!
bool LM::performQueryDB(string query, MYSQL_RES &queryResult)
{
	writeLog("Running query: \"" + query + "\"", false, LV_DEBUG);

	if ( pPlutoDatabase ) {
		//if (!pPlutoDatabase->isOpen())
		//{
		//	writeLog("Attempted to exec query via closed DB connection, trying to reopen it", false, LV_WARNING);
		//	closeDB();
		//	if (!openDB())
		//	{
		//		writeLog("Reopening failed, giving up", false, LV_CRITICAL);
		//		return false;
		//	}
		//}
		
		queryResult = pPlutoDatabase->exec(query);
		if ( queryResult.isActive() )
			return true;
		
		writeLog("Error executing query", false, LV_CRITICAL);
		QSqlError err = pPlutoDatabase->lastError();
		printSQLerror(err);
		
		writeLog("Trying to reopen DB connection", false, LV_WARNING);
		closeDB();
		if (!openDB())
		{
			writeLog("Reopening failed, giving up", false, LV_CRITICAL);
			return false;
		}
		
		queryResult = pPlutoDatabase->exec(query);
		if ( queryResult.isActive() )
			return true;
		
		writeLog("Error executing query after reopen", false, LV_CRITICAL);
		//printSQLerror(err);
		return false;
	}
	else
	{
		writeLog("Attempted to query not initialized DB", false, LV_CRITICAL);
		return false;
	}
}*/
void LM::initialize_VideoSettings()
{
	if (m_sOrbiterID == "")
		return;

	m_sVideoResolution = m_dbPlutoDatabase.quickQuery("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_sMediaID + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Video_settings_CONST));
	
	//The old Launch Manager used a python script which used the now obsolete xorgconfig module to get the video driver name
	//Let just do it manuall in good ol' c++
	//vector thisText;
	string line;
	string xorgFile;
	ifstream textStream ("/etc/X11/xorg.conf");
	while(getline(textStream,line)){
		line=StringUtils::TrimSpaces(line) + "\n";
		line=StringUtils::Replace(line,"\t","");
		xorgFile += line;
	}
	textStream.close();
	
	//now lets get the correct section
	size_t startPos;

	startPos=xorgFile.find("\nSection \"Device\"\n");
  	xorgFile = xorgFile.substr(startPos,-1);

	
	startPos=xorgFile.find("\nEndSection\n");
	xorgFile = xorgFile.substr(0,startPos+11);
	
	
	//Now we have the Device section, find the driver!
	startPos=xorgFile.find("\nDriver");
	startPos+=7;
	xorgFile=xorgFile.substr(startPos,-1);

	startPos=xorgFile.find("\n");
	xorgFile=xorgFile.substr(0,startPos);
	xorgFile=StringUtils::Replace(xorgFile,"\"","");	
	m_sVideoDriver = xorgFile;
	cout << m_sVideoDriver << endl;
	
	
}

void LM::initialize_AudioSettings()
{
	if (m_sMediaID == "")
		return;

	string sSettings = m_dbPlutoDatabase.quickQuery("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_sMediaID + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Audio_settings_CONST));

	if (sSettings=="")
		return;
	
	//cbAC3Pass->setChecked(aSettings.contains("3"));
	/*	
	int iSelected = 0;
	switch(aSettings.at(0).cell())
	{
		case 'C':
			iSelected = 1;
			break;
		case 'O':
			iSelected = 2;
			break;
		case 'S':
			iSelected = 3;
			break;
		case 'L':
			iSelected = 4;
			break;
		case 'M':
		default:
			break;
	}
	
	cmbAudioConnector->setCurrentItem(iSelected);
	*/
}

void LM::initialize_LogOptions()
{
	// initing list of log levels
	//initLogLevels();
	
	// filling list
	//lvLogLevels->setSorting(-1);
	
	//{
	//	QMap<int,QString>::iterator i=m_qmapLogLevels.end();
	//	while (i!=m_qmapLogLevels.begin())
	//	{
	//		--i;
	//		new QCheckListItem(lvLogLevels, *i, QCheckListItem::CheckBox);
	//	}
	//}
	
	// hiding progressbar
	//pbProgress->hide();
	
	// initing connections
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	string s = pConfig->m_mapParameters_Find("LogLevels");
	delete pConfig;
	//cout << "Log Levels: " + s + " " << endl;
	//QStringList levels = QStringList::split(',', s, false);
	/*
	for (QStringList::iterator i=levels.begin(); i!=levels.end(); ++i)
	{
		bool bOk;
		int logLevel = (*i).toInt(&bOk);
		if (bOk)
		{
			QMap<int,QString>::iterator j = m_qmapLogLevels.find(logLevel);
			if (j!=m_qmapLogLevels.end())
			{
				QString sItem = *j;
				QCheckListItem *pItem = (QCheckListItem*) lvLogLevels->findItem(sItem, 0);
				if (pItem)
					pItem->setState(QCheckListItem::On);
			}
		}
	}*/
}

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

	

	
	//tuneConnectionsWidgets();
	
	//if (!m_pDevicesUpdateTimer)
	//{
	//	m_pDevicesUpdateTimer = new QTimer();
	//	connect(m_pDevicesUpdateTimer, SIGNAL(timeout()), this, SLOT(updateDevicesProgressBar()));
	//}
	
	
		
	//if (!m_bMediaHere)
	//{
	//	twAllTabs->removePage(twAllTabs->page(2));
	//	twAllTabs->removePage(twAllTabs->page(2));
	//}
	
	
	// starting RA monitoring timer
	m_pAlarmManager->AddRelativeAlarm(5,this,UPDATE_RA_STATUS,NULL);
	
	
}

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
		writeLog("Checking for core services...",true,LV_STATUS);	
	}
	
	return bResult;
}

bool LM::checkCore(string coreIP)
{
	writeLog("Checking if core is up...", true);
	
	struct stat fInfo;

	if ( lstat(DCEROUTER_LOCK, &fInfo)==0 )
	{
		writeLog("Found router lock file at " + string(DCEROUTER_LOCK) );
		//string s;

		//s=system("/bin/nc -z 192.168.80.1");
		//cout << s << endl;		
		//return true;
		 
		//time_t tStart = time(NULL);
		//time_t tEnd = tStart;
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
			
			//int iResult = QMessageBox::warning(this, "Important", "The DCERouter is not responding. A reboot may be needed. Reboot the system now?", 
			//		     QMessageBox::Yes, QMessageBox::No);
			//
			//if (iResult == QMessageBox::Yes)
			//{
			//	writeLog("User selected to reboot box");
			//	rebootPC();				
			//	return false;
			//}
			//else
			//{
			//	writeLog("User selected not to reboot box");
			//	return false;
			//}
			
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

bool LM::initialize_LMdevice(bool bRetryForever/*=false*/)
{
	// if we already have a connection
	if (m_pLMCE_Launch_Manager!=NULL)
		return true;
	
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	string sCoreIP = m_sCoreIP;
	
	if (sCoreIP == "localhost")
		sCoreIP = "127.0.0.1";
	
	writeLog("Connecting to router at " + sCoreIP, true, LV_STATUS);

	while (true)
	{

		m_pLMCE_Launch_Manager = new DCE::LMCE_Launch_Manager(atoi(m_sDeviceID.c_str()), sCoreIP,  true, false);//int(m_sDeviceID.c_str())

		bool bConnected = m_pLMCE_Launch_Manager->GetConfig();
		if ( bConnected && m_pLMCE_Launch_Manager->m_pEvent->m_pClientSocket->m_eLastError==DCE::ClientSocket::cs_err_NeedReload )
		{
			//lbMessages->clear();
			writeLog("Please go to an existing Orbiter and choose 'quick reload router'. ", true, LV_WARNING);
			writeLog("This media director will start after you do...", true, LV_WARNING);

			
			sleep(10);

			writeLog("initialize_LMdevice: router should be reloaded, retrying connect", false, LV_WARNING);
			delete m_pLMCE_Launch_Manager;
			m_pLMCE_Launch_Manager = NULL;
		}
		if( bConnected && m_pLMCE_Launch_Manager->Connect(m_pLMCE_Launch_Manager->PK_DeviceTemplate_get()) ) 
		{
			writeLog("initialize_LMdevice: Connect OK", true, LV_STATUS);
			
			map<int,string> devicesMap;
			m_pLMCE_Launch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &devicesMap);
			if (devicesMap.empty())
			{
				writeLog("initialize_LMdevice: Failed to find Orbiter plugin", false, LV_WARNING);
			}
		
			map<int,string>::iterator it = devicesMap.begin();
			m_sOrbiterPluginID = StringUtils::itos((*it).first) ;
			m_pAlarmManager->AddRelativeAlarm(5,this,LM_KEEP_ALIVE,NULL);
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
void LM::AlarmCallback(int id, void* param)
{
	
	if(id==LM_KEEP_ALIVE) {
		LMdeviceKeepAlive();
	} else if (id==UPDATE_RA_STATUS) {
		//auto-schedule next call to create an interval timer
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
		//make an interval timer for now..  It will be killed in
		//updateOrbiterRegenProgress
		m_pAlarmManager->CancelAlarmByType(UPDATE_ORBITER_REGEN_PROGRESS);
		m_pAlarmManager->AddRelativeAlarm(2,this,UPDATE_ORBITER_REGEN_PROGRESS,NULL);
		updateOrbiterRegenProgress();
	}
}
// TODO refactor
void LM::updateScripts()
{
	writeLog("Running UpdateAvailableSerialPorts.sh", true, LV_STATUS);
	string sCmd = "/usr/pluto/bin/UpdateAvailableSerialPorts.sh";
	exec_system(sCmd,true);
	writeLog("Process completed.");

	writeLog("Running checkforRaids.sh",true,LV_STATUS);
	sCmd = "/usr/pluto/bin/checkforRaids.sh";
	writeLog("Process completed.");
				
}

void LM::respawnNewChildren()
{
	writeLog("Requested respawning of new children devices", true, LV_WARNING);
	
	//allowRedraw = false;
	
	syncWithLockList(true);
	
	if (m_bCoreHere && m_bCoreRunning)
	{
		writeLog("Spawning new children devices - core", true, LV_WARNING);
		startCoreDevices(true);
		writeLog("Finished spawning new children devices - core", true, LV_WARNING);
	}
	
	if (m_bMediaHere && m_bMediaRunning)
	{
		writeLog("Spawning new children devices - media", true, LV_WARNING);
		startMediaDevices(true);
		writeLog("Finished spawning new children devices - media", true, LV_WARNING);
	}
	
	writeLog("Finished respawning of new children devices", true, LV_WARNING);
	
	//allowRedraw = true;
}

void LM::syncWithLockList(bool eraseDeadLocalDevices)
{
	writeLog("Reading lockfile");
			
	// reading list
	List::List qsl;
	
	//File f(string(PLUTO_LOCKS));
	//TODO: do some simple error checking to make sure the file opened...
	ifstream textStream (string(PLUTO_LOCKS).c_str());
	if (!textStream.fail())
	{
		string sID;
		int iRet;
		while ( getline(textStream,sID) )
		{
			//if  ( (iRet = f.readLine(sID, 100))!=-1)
			//{
				sID=StringUtils::Replace(sID," ","");
				sID=StringUtils::Replace(sID,"\n","");
				sID=StringUtils::Replace(sID,"\t","");
				//sID = sID.replace(" ", "");
				//sID = sID.replace("\n", "");
				//sID = sID.replace("\t", "");
				
				if (sID != "") 
				{
					qsl.append(sID);
				}
			//}
		}
		
		textStream.close();

		writeLog("Detected running devices: " + qsl.join(" "));
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
		
		// cleaning up dead core devices
		//for (vector<int>::iterator it=m_vCoreDevices.begin(); it!=m_vCoreDevices.end(); ++it)
		for(unsigned int i=0;i<m_vCoreDevices.size();i++)
		{
			string sID = m_vCoreDevices[i];
			if ( !qsl.contains(sID) )
			{
				writeLog("Device " + sID + " seem to be dead" );
			}
			else
				newV.push_back(sID);
		}
		
		m_vCoreDevices = newV;
		newV.clear();
		
		// cleaning up dead core devices
		//for (QValueVector<int>::iterator  it=m_qvvMediaDevices.begin(); it!=m_qvvMediaDevices.end(); ++it)
		for(unsigned int i=0;i<m_vMediaDevices.size();i++)
		{
			string sID = m_vMediaDevices[i];
			if ( !qsl.contains(sID) )
			{
				writeLog("Device " + sID + " seem to be dead");
			}
			else
				newV.push_back(sID);
		}
		
		m_vMediaDevices  = newV;
		newV.clear();
	}
	//writeLog("Detected running devices: " + qsl.join(" ")); // for debug purposes
}

void LM::startCoreDevices(bool checkForAlreadyRunning)
{
	string s;
	DBResult dbrCoreDevices;

	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_iDevicesLevel = 0;
//	m_pDevicesUpdateTimer->start(1000);
	
	writeLog("startCoreDevices()");

	if ( m_dbPlutoDatabase.connected() ) {
		// fetching list of non-MD devices under Core
		List devices;
		
		devices.append(m_sCoreDeviceID);
		
		// filtering out the MD/Hybrid PC
		string extraCondition = "";
		if (m_sMediaID!="")
			extraCondition += " AND PK_Device<>" + m_sMediaID;
		  
		//for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)
		for( unsigned int i=0;i<devices.size();i++ )
		{
			//int i=0; //debug
			string query = "SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE, Device.FK_DeviceTemplate,DeviceTemplate.IsPlugin FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE (PK_Device=" + devices.index(i) + extraCondition + ")";
						
			//cout << query.c_str() << endl << endl;//DEBUG	
				
			dbrCoreDevices=m_dbPlutoDatabase.query(query);
		
			// if device exist
			//while ( bQueryExecResult && q.next() )
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
							//for (QValueVector<int>::iterator jt=m_qvvCoreDevices.begin(); jt!=m_qvvCoreDevices.end(); ++jt)
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
							writeLog("Starting device " + dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1), true);
							
							//screen -d -m -S "$Description-$DeviceID" /usr/pluto/bin/Spawn_Device.sh $DeviceID $DCERouter $Command	
												
							string deviceCommand = dbrCoreDevices.value(2);
							if (deviceCommand=="")
								deviceCommand=StringUtils::Replace(dbrCoreDevices.value(1)," ","_");
								//deviceCommand = q.value(1).toString().simplifyWhiteSpace().replace(" ","_");
							
							if (!FileUtils::FileExists(deviceCommand))
							{
								writeLog("Not starting device " +  dbrCoreDevices.value(0) + " "  + dbrCoreDevices.value(1) + " -  binary is not found.", true, LV_WARNING);
								continue;
							}
							
							string screenName = dbrCoreDevices.value(1);
							screenName = FileUtils::ValidCPPName(screenName + "-" + dbrCoreDevices.value(0));

							// starting device
							//QStringList args;
	
							//args.push_back("/usr/bin/screen");
							//args.push_back("-d");
							//args.push_back("-m");
							//args.push_back("-S");
							//args.push_back(screenName);
							//args.push_back("/usr/pluto/bin/Spawn_Device.sh");
							//args.push_back(q.value(0).toString());
							//args.push_back(leCoreIP->text());
							//args.push_back(deviceCommand);
							string sCmd = "/usr/bin/screen -d -m -S " + screenName;
							exec_system(sCmd);
							sCmd = "/usr/pluto/bin/Spawn_Device.sh " + dbrCoreDevices.value(0) + " " + m_sCoreIP;
							exec_system(sCmd);
							//QProcess process(args);
							writeLog("Starting device " +  dbrCoreDevices.value(0) + " " + dbrCoreDevices.value(1));// + ": " + args.join(" "));
							//process.start();
							
							// wait for register
							waitForDevice(atoi(dbrCoreDevices.value(0).c_str()));
							
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
	//delete &dbrCoreDevices;
	//QApplication::restoreOverrideCursor();
}


void LM::startMediaDevices(bool checkForAlreadyRunning)
{
	string s;
	DBResult dbrMediaDevices;
	if (m_sMediaID=="")
		return;
	
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	m_iDevicesLevel = 0;
//	m_pDevicesUpdateTimer->start(1000);

	writeLog("startMediaDevices()");
	
	if ( m_dbPlutoDatabase.connected()  ) {
		// fetching full list of devices under current MD
		List devices;
		devices.append(m_sMediaID);
		//for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)
		for( unsigned int i=0;i<devices.size();i++ )
		{
			string query = "SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE  PK_Device=" + devices.index(i);
			//QSqlQuery q;
			//bool bQueryExecResult = performQueryDB(query, q);
			
			dbrMediaDevices=m_dbPlutoDatabase.query(query);

			// if device exist
			//if ( bQueryExecResult && q.next() )
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
							//for (QValueVector<int>::iterator jt=m_qvvMediaDevices.begin(); jt!=m_qvvMediaDevices.end(); ++jt)
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
							writeLog("Starting device " +  dbrMediaDevices.value(0) + " "  + dbrMediaDevices.value(1), true);
							
							//screen -d -m -S "$Description-$DeviceID" /usr/pluto/bin/Spawn_Device.sh $DeviceID $DCERouter $Command	
												
							string deviceCommand = dbrMediaDevices.value(2);
							if (deviceCommand=="")
								deviceCommand=StringUtils::Replace(dbrMediaDevices.value(1)," ","_");
								//deviceCommand = dbrMediaDevices.value(1)).simplifyWhiteSpace().replace(" ","_");
							
							if (!FileUtils::FileExists(deviceCommand))
							{
								writeLog("Not starting device " +  dbrMediaDevices.value(0) + " "  + dbrMediaDevices.value(1) + " - binary is not found.", true, LV_WARNING);
								continue;
							}
							
							

							string screenName = dbrMediaDevices.value(1);
							screenName = FileUtils::ValidCPPName(screenName + "-" + dbrMediaDevices.value(0));

							// starting device
							//QStringList args;
	
							//args.push_back("/usr/bin/screen");
							//args.push_back("-d");
							//args.push_back("-m");
							//args.push_back("-S");
							//args.push_back(screenName);
							//args.push_back("/usr/pluto/bin/Spawn_Device.sh");
							//args.push_back(q.value(0).toString());
							//args.push_back(leCoreIP->text());
							//args.push_back(deviceCommand);
							string sCmd = "/usr/bin/screen -d -m -S " + screenName;
							exec_system(sCmd);
							sCmd = "/usr/pluto/bin/Spawn_Device.sh " + dbrMediaDevices.value(0) + " " + m_sCoreIP;
							
							exec_system(sCmd,true);
														
							//system(sCmd.c_str());
							//QProcess process(args);
							writeLog("Starting device " +  dbrMediaDevices.value(0) + " " + dbrMediaDevices.value(1));// + ": " + args.join(" "));
							//process.start();
							
							// wait for register
							waitForDevice(atoi(dbrMediaDevices.value(0).c_str()));
							
							// recording device in list
							m_vMediaDevices.push_back(dbrMediaDevices.value(0));


							
						}
					}
					else
					{
						writeLog("Not starting device " +  dbrMediaDevices.value(0) + " "  + dbrMediaDevices.value(1) + " - it doesn't implement DCE. Adding its children into start queue instead", false);
						// starting devices recursively
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
	
	//QApplication::restoreOverrideCursor();
}
void LM::waitForDevice(int deviceID)
{
	char cStatus = 'N';
	int iCount = MEDIA_DEVICE_TIMEOUT;
	
	if (!m_pLMCE_Launch_Manager)
		cStatus = 'E';
	
	while (cStatus=='N' && iCount--)
	{
		sleep(1);
		//if (allowRedraw)
		//	qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
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
void LM::reportDeviceUp()
{
	// saying we're up
	//long i = atoi(m_sDeviceID.c_str());
	//DCE::Command_Impl::m_bRouterReloading = false;
	DCE::Message *pMessage = new DCE::Message(atoi(m_sDeviceID.c_str()), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_UP, 0);
	//cout << m_sDeviceID;
	
	//cout << StringUtils::itos(i);
	
	if (m_pLMCE_Launch_Manager)
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_UP from LM device " + m_sDeviceID, false);
		m_pLMCE_Launch_Manager->QueueMessageToRouter(pMessage);
		
	}
	else
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_UP from LM device " + m_sDeviceID + " failed", false, LV_WARNING);
	}
}
void LM::loadSettings()
{
}
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
				//tuneConnectionsWidgets();
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
			m_pAlarmManager->CancelAlarmByType(LM_KEEP_ALIVE); //TODO: consolidate all of these CancelAlarmByType calls at the top
                }
	}
}
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
		//for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)
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

void LM::rebootPC()
{
	writeLog("Rebooting");	
	m_uiMainUI.flushLog();
	
	exec_system("/sbin/reboot");
	// giving system time to start actual reboot
	sleep(60);
}
void LM::flushLog()
{
	DCE::LoggerWrapper::GetInstance()->Flush();
}
void LM::writeLog(string s, bool toScreen, int logLevel)
{
	DCE::LoggerWrapper::GetInstance()->Write(logLevel, s.c_str());
	if (toScreen)
	{
		m_uiMainUI.writeLog(s);	
	}


	
}
void LM::appendLog(string s)
{
	//appends the most recent log entry in the UI for dynamic display purposes
	m_uiMainUI.appendLog(s);
}
void LM::updateRAstatus()
{
	bool bRAstatus = checkRAStatus();
	
	if (bRAstatus != m_bRemoteAssistanceRunning)
	{
		m_bRemoteAssistanceRunning = bRAstatus;
		//tuneConnectionsWidgets();
	}
}
string LM::getOrbiterStatus()
{
	int iNull;
	return getOrbiterStatus(iNull);
}

string LM::getOrbiterStatus(int &iProgressValue)
{
	return getOrbiterStatus(m_sOrbiterID, iProgressValue);
}
string LM::getOrbiterStatus(const string &orbiterID, int &iProgressValue)
{
	string orbiterStatus="!";
	iProgressValue=0;

	// ! - is our custom error "while fetching info"
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


void LM::doAutostart()
{
	writeLog(">>Performing autostart if configured..", true);

	m_bDelayUpReport = false;
	
	bool bReport = false;
	
	// checking for core and starting it
	//if (m_bCoreHere && (cbAutostartCore->isChecked() || false))
	if(m_bCoreHere && m_sAutostartCore == "1" )
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
	//TODO: do the start MD stuff after i get the core stuff working
	//if ( ( (m_bCoreHere &&m_bCoreRunning) || !m_bCoreHere) && m_bMediaHere && (cbAutostartMedia->isChecked() || false) )
	//{
	//	if (!m_bMediaRunning)
	//	{
	//		checkScreenDimensions(false);
	//		
	//		writeLog(">>Autostarting media station....", true);
	//		m_bMediaRunning = startMediaStation();
	//		bReport |= m_bMediaRunning;
	//	}
	//	else
	//		writeLog(">>Not autostarting media station - it is already running", true);
	//}
	
	if (bReport) {
		if (m_bDelayUpReport) {
			m_pAlarmManager->AddRelativeAlarm(30,this,REPORT_DEVICE_UP,NULL);
		} else {
			reportDeviceUp();
		}
	}
	//tuneConnectionsWidgets();
}

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

bool LM::startCoreServices()
{
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	//QProcess *pService = new QProcess(QString(START_CORE_SCRIPT));
	
	writeLog("Starting process " + string(START_CORE_SCRIPT), true, LV_WARNING);
	writeLog("Please be patient. This may take a while...", true);
	
	
	string log = LM_PROGRESS_LOG;
	string cmd = "echo -n > " + log; //clear log
	system(cmd.c_str());
	
	//QFile flog(log);
	//textstream ts;
	ifstream textStream (LM_PROGRESS_LOG);
	
	if (textStream.fail())
	{
		writeLog("Failed to open file :" + string( LM_PROGRESS_LOG), false, LV_WARNING);
	}

	int startCoreScript = exec_system(START_CORE_SCRIPT);	
	//TODO: see if it started. For now, lets assume that it did...

	if ( started(startCoreScript) )
	{
		bool bOkDB = m_dbPlutoDatabase.connected();
		
		//pbProgress->setProgress(0);
		//pbProgress->show();
		
		//tlStatusMessages->setText("");
		//tlStatusMessages->show();
		
		string currOrbiterID="";
		//qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);

		int iCounter = CORE_SERVICE_TIMEOUT;
		int iQueryCounter=5; // trigger on start
		while (isRunning(startCoreScript) && iCounter--)
		{
			// every second checking config log messages
			string s, s1;
			//while ( (s=ts.readLine()) != QString::null)
			while(getline(textStream,s))
			{
				s1 = s;
			}
			
			if (s1!="")
			{
				//tlStatusMessages->setText(s1);
				writeLog("Progress message: " + s1, false);
			}
			
			// every 5 seconds checking orbiters
			if (iQueryCounter>=5)
			{
				iQueryCounter = 0;
				
				// retrieve orbiter progress
				if (bOkDB)
				{
					string sQuery = "select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1";
					//QSqlQuery q;
					//bool bQueryExecResult = performQueryDB(sQuery, q);
					DBResult res = m_dbPlutoDatabase.query(sQuery);
					string O_ID;
					string O_Message;
					int O_Progress = 100; //was uint TODO
					
					// some orbiter is being regen-ed
					if (res.next())//( bQueryExecResult && res.next() )
					{
						O_ID = res.value(0);
						O_Progress = atoi(res.value(1).c_str());
						O_Message = "Orbiter #"+O_ID+"\n"+res.value(2);
						
						if (O_ID != currOrbiterID)
						{
							currOrbiterID = O_ID;
							writeLog("Generating skin for Orbiter #" + currOrbiterID + "...", true);
						}
						
						//pbProgress->setProgress(O_Progress);
						//tlStatusMessages->setText(O_Message);
					}
				}
			}
			
			sleep(1);
			iQueryCounter++;
			
		}
		

		textStream.close();
		
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
bool LM::startMediaStation()
{
	updateScripts();

	// retrying forever, as KeepAlive timer is not running yet
	bool bLMinit = initialize_LMdevice(true);
	if (!bLMinit)
	{
		writeLog("Failed to connect to DCERouter!", true, LV_WARNING);
		return false;
	}	
	
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
                    // firing reload event //TODO: shoudl this be implemented through socket?
                    //if (!bRouterReloaded)
                    //   btnReloadRouter_clicked();
                    
                    writeLog("Trying to reload router to use new skin for this Orbiter, please wait (giving router 10 seconds to reload)...", true);
                    //qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
                    
                    // waiting 10 seconds
                    int iCount=10;
                    while (iCount--)
                    {
                        sleep(1);
                        //qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
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
	
	// starting media services
	startMediaDevices();
	
	// jumping to orbiter
	jumpToOrbiter();
	
	return true;
}
bool LM::checkScreenDimensions(bool askUser/*=true*/)
{
	//TODO: port this to standard library stuff
/*
	QDesktopWidget qdw;
	QRect dims = qdw.screenGeometry();
	QString s = QString::number(dims.width()) + " " + QString::number(dims.height());
	QString s1 = QString::number(dims.width()) + "x" + QString::number(dims.height());
		
	QString currentDimensions = leVideoResolution->text();
	int index = currentDimensions.find('/');
	QString resolution, rate;
	if (index!=-1)
	{
		rate = currentDimensions.mid(index);
	}
	else
		rate = "/60";
	
	int newX = dims.width();
	int newY = dims.height();
	
	int currentX=-1;
	int currentY=-1;
	
	index = currentDimensions.find(' ');
	if (index != -1)
	{
		currentX = currentDimensions.mid(0, index).toInt();
		// if it is like "1920 1080 i/60"
		int index1 = currentDimensions.find(" ", index+1);
		// if it is like "1024 768/60"
		if (index1 == -1)
			index1 = currentDimensions.find("/", index+1);
			
		if (index1 != -1)
			currentY = currentDimensions.mid(index+1, index1-index-1).toInt();
	}
	
	QString newDimensions = s+rate;
	
	writeLog("Checking screen resolution: current is " + s1);
	
	if ( (currentX != newX) || (currentY != newY) )
	{
		writeLog("Detected new screen resolution \""+newDimensions+"\" " + " [" + QString::number(currentX)+"x"+QString::number(currentY)+"] => [" + QString::number(newX)+"x"+QString::number(newY)+"]", true);
		int iResult = QMessageBox::No;
		
		if (askUser)
		{
				iResult = 	QMessageBox::question(this, "Resolution changed", "Launch Manager detected that your screen resolution \nrecently changed to " + 
						s1+ ". Do you want to update Orbiter \nconfiguration for this box and regenerate Orbiter skin?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

				if (iResult == QMessageBox::Cancel)
				{
					writeLog("User decided to abort startup", true, LV_WARNING);
					return false;
				}
				
				if (iResult == QMessageBox::Yes)
					writeLog("User decided to update Orbiter configuration", true, LV_WARNING);
				else
					writeLog("User decided not to update Orbiter configuration", true, LV_WARNING);
		}
		else
		{
			iResult = QMessageBox::Yes;
			writeLog("Autorun mode: updating Orbiter configuration", true, LV_WARNING);
		}
		
		if (iResult == QMessageBox::Yes)
		{
			// saving M/D screen info
			queryDB( QString("UPDATE Device_DeviceData SET IK_DeviceData=\""+ newDimensions + "\"WHERE FK_Device=" + m_qsMediaID + " AND FK_DeviceData=" + 
					QString::number(DEVICEDATA_Video_settings_CONST) ));
			
			updateOrbiterSize(dims.width(), dims.height());
			
			// just for case OrbiterGen won't succeed first time
			queryDB( QString("UPDATE Orbiter SET Regen=1 WHERE PK_Orbiter=" + m_qsOrbiterID));
			
			leVideoResolution->setText(newDimensions);
			
			// if the core is already running, ask it to regen the given orbiter
			if (m_bCoreRunning)
			{
				writeLog("Core is running, attempting to send 'regen orbiter' command...");
				if (m_pLaunch_Manager)
				{
					int orbiterPluginID=-1;
					map<int,string> mapDevices;
					m_pLaunch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &mapDevices);
					if (!mapDevices.empty())
					{
						orbiterPluginID = mapDevices.begin()->first;
						DCE::CMD_Regen_Orbiter cmd( m_qsDeviceID.toInt(),  orbiterPluginID, m_qsOrbiterID.toInt(),"-a", "" );
						m_pLaunch_Manager->SendCommandNoResponse(cmd);
						writeLog("Command sent", false);
						
						startOrbiterRegenProgressTracking();
						
						while (m_bRegenInProgress)
						{
							sleep(1);
							qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
						}
					}
					else
						writeLog("Failed to find Orbiter Plugin", false, LV_WARNING);
				}
				else
				{
					writeLog("Command not sent - no connection to core", false, LV_WARNING);
				}
			}
			else
				writeLog("Core is not running, not sending any commands - orbiter will be regenerated on core startup");
				
		}
	}
	*/
	return true;
}
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
				writeLog("First time generating skin for current Orbiter #"+m_sOrbiterID, true);
			else
				writeLog("Regenerating skin for current Orbiter #"+m_sOrbiterID, true);
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
		writeLog("Generated skin successfully", true);
	else
	{
		writeLog("Skin generation completed, server says: " + getOrbiterStatusMessage(sOrbiterStatus), true, LV_WARNING);
	}
	
	//QApplication::restoreOverrideCursor();
	
	return (sOrbiterStatus=="O")||(sOrbiterStatus=="N");
}
bool LM::triggerOrbiterRegen(bool bAllOrbiters/*=false*/)
{
	// just for case OrbiterGen won't succeed first time
	//queryDB( QString("UPDATE Orbiter SET Regen=1 WHERE" + (bAllOrbiters?" 1":(" PK_Orbiter=" + m_qsOrbiterID)) ) );
	m_dbPlutoDatabase.query("UPDATE Orbiter SET Regen=1 WHERE" + (bAllOrbiters?" 1":(" PK_Orbiter=" + m_sOrbiterID)));		
	if (m_pLMCE_Launch_Manager)
	{
		int orbiterPluginID=-1;
		map<int,string> mapDevices;
		m_pLMCE_Launch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &mapDevices);
		if (!mapDevices.empty())
		{
			orbiterPluginID = mapDevices.begin()->first;
			DCE::CMD_Regen_Orbiter cmd( atoi(m_sDeviceID.c_str()),  orbiterPluginID, (bAllOrbiters?0:atoi(m_sOrbiterID.c_str())),"", "" ); //TODO in the morning - convert ints to longs
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
void LM::startOrbiterRegenProgressTracking(bool currentOrbiterOnly/*=true*/)
{
	m_bRegenInProgress = true;
	m_bRegenTrackingCurrentOrbiterOnly = currentOrbiterOnly;

	if (m_pAlarmManager->FindAlarmByType(UPDATE_ORBITER_REGEN_PROGRESS) == -1)
	{
		m_pAlarmManager->AddRelativeAlarm(2,this,UPDATE_ORBITER_REGEN_PROGRESS,NULL);
	}
	
}
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
			//pbProgress->show();
			//pbProgress->setProgress(iRegenProgress);
			
			//QString sQuery = "select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1 AND PK_Orbiter="+m_qsOrbiterID;
			//QSqlQuery q;
			//bool bQueryExecResult = performQueryDB(sQuery, q);
			DBResult res = m_dbPlutoDatabase.query("select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1 AND PK_Orbiter="+m_sOrbiterID);
			//if ( bQueryExecResult && q.next() )
			if(res.next())
			{
				string statusMessage = "Orbiter #" + res.value(0)+ "\n"+res.value(2);
				//TODO: appendLog??
				//tlStatusMessages->setText(statusMessage);
				//tlStatusMessages->show();
			}
			
			writeLog("Waiting for OrbiterGen to complete", false);
		}
		else
		{
			//pbProgress->hide();

			//m_pProgressBarUpdateTimer->stop();
			m_pAlarmManager->CancelAlarmByType(UPDATE_ORBITER_REGEN_PROGRESS);

			//tlStatusMessages->hide();
			//tlStatusMessages->setText("");QString lmce_launch_managerWidget::getOrbiterStatus(const QString &orbiterID, int &iProgressValue)  //TODO: where to display getOrbiterStatus????


			writeLog("OrbiterGen run completed", true);
			
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
				string statusMessage = "Orbiter #" + res.value(0) + "\n"+res.value(2);
				//TODO: where to display this??
				//tlStatusMessages->setText(statusMessage);
				//tlStatusMessages->show();
			}
			
			writeLog("Waiting for OrbiterGen to complete", false);
		}
		else
		{
			//pbProgress->hide();

			//m_pProgressBarUpdateTimer->stop();
			m_pAlarmManager->CancelAlarmByType(UPDATE_ORBITER_REGEN_PROGRESS);

			//tlStatusMessages->setText("");
			//tlStatusMessages->hide();
			
			writeLog("OrbiterGen run completed", true);
			
			m_bRegenInProgress = false;
		}
	}
}


//DONE
void LM::jumpToOrbiter()
{
	if (m_pLMCE_Launch_Manager)
	{
		writeLog("Activating Orbiter...", true);
		DCE::CMD_Activate_PC_Desktop cmd(atoi(m_sDeviceID.c_str()), atoi(m_sOrbiterID.c_str()), 0);
		m_pLMCE_Launch_Manager->SendCommandNoResponse(cmd);
	}
	else
	{
		writeLog("Failed to activate Orbiter - no connection to DCE router", true, LV_WARNING);
	}
}


void LM::Run()
{
	while(true);
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

        writeLog("exec_system: args parsed like this:",false,LV_STATUS);
	for (int i=0; i<arg_count; i++) {
             writeLog("exec_system: arg " + StringUtils::itos(i) +  " is " + args[i]);
	}

        fork_res=fork();

        // Child thread
        if (fork_res==0) {
           int retval;
           writeLog("exec_system: within child fork",false,LV_STATUS);
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
            writeLog("exec_system: fork ok",false,LV_STATUS);

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
