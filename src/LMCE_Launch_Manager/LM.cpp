
#include "LMCE_Launch_Manager.h"
#include "LM.h"
#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"
#include "DCE/ClientSocket.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/ProcessUtils.h"
#include <iostream>
#include <fstream>
#include "List.h"

using namespace std; //DCE

#include "Gen_Devices/AllCommandsRequests.h"

// timeouts in seconds
#define MEDIA_DEVICE_TIMEOUT 10
#define CORE_DEVICE_TIMEOUT 10
#define CORE_SERVICE_TIMEOUT 600

// time in milliseconds
#define LOG_REFRESH_INTERVAL 30000


#define	PLUTO_CONFIG "/etc/pluto.conf"
#define PLUTO_LOG_DIR "/var/log/pluto"
#define SCREENLOGS_BASE "/home/screenlogs"
#define APP_UI_DIAG "/usr/pluto/bin/UIdiag"
#define APP_UI_DIAG_WORKDIR "/usr/pluto/bin"
#define APP_UI_DIAG_LIBS "/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/lib"

#define SCRIPT_X_CHECK_SETTINGS "/usr/pluto/bin/X-CheckSettings.sh"

#define SCRIPT_SETUP_AV_SETTINGS "/usr/pluto/bin/SetupAudioVideo.sh"

#define VIDEO_DRIVER_NAME_FILE "/tmp/lmce_lm.video.dat"
#define VIDEO_DRIVER_TOOL "/usr/pluto/bin/GetVideoDriver.py"

#define KILL_XORG_SCRIPT "/usr/pluto/bin/Stop_X.sh"

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
}

LM::~LM()

{
	
}
void LM::Initialize()
{
	DCE::LoggerWrapper::SetType(LT_LOGGER_FILE, PLUTO_LOG_DIR "/LaunchManager.log");
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting...");
	
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
	
	// creating required dir
	//QDir qdir(SCREENLOGS_BASE);
	//if (!qdir.exists(SCREENLOGS_BASE, true))
	//	qdir.mkdir(SCREENLOGS_BASE, true);

	// if the system was already running when we started, let's catch new children and report we are up
	if ( m_bCoreRunning || m_bMediaRunning )
	{
        	updateSerialPorts();
		//respawnNewChildren();
		//reportDeviceUp();
	}
	
	writeLog("=>loadSettings()");
	// reding autostart settings
	//loadSettings();
	
	//writeLog("Started!");http://www.google.com/
	

	

}

void LM::writeLog(string s, bool toScreen, int logLevel)
{
	if (toScreen)
	{
		string timestamp = "Today";//QDateTime::currentDateTime().toString();
		//cout << timestamp + " " + s << endl;
		//lbMessages->update();
		//doRedraw();
	}

	cout << s << endl;
	DCE::LoggerWrapper::GetInstance()->Write(logLevel, s.c_str());
}


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
	
	//SHOULD CHANGE THESE TO BOOL
	m_sAutostartCore = pConfig->m_mapParameters_Find("AutostartCore");
	m_sAutostartMedia = pConfig->m_mapParameters_Find("AutostartMedia");
	
	m_bRemoteAssistanceRunning = checkRAStatus();
	
	// trying to connect to DB
	openDB();
	
	m_sDeviceID = pConfig->m_mapParameters_Find("PK_Device");
	
	// detecting core
	m_sCoreDeviceID = queryDB("SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=7");
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
		mdID = queryDB("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + m_sDeviceID + " AND FK_DeviceCategory IN (8)");
		
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
	string sCoreHere = m_bCoreHere?"X":" ";
	string sMediaHere = m_bMediaHere?"X":" ";
	cout << "Core is  here: [" + sCoreHere + "]" << endl;
	cout << "Media is Here: [" +  sMediaHere + "]" << endl;

	
	if (m_sDeviceID!="")
	{
		string localOrbiterID = queryDB("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + mdID + " AND FK_DeviceCategory IN (2, 3, 5)");
		m_sOrbiterID = localOrbiterID;
	}

	m_sMediaID = mdID;
	
	delete pConfig;
}

void LM::Draw() {
	system("clear");  //yes, I know this is not the best way to do it...
	cout << "+------------------------------------------------------------------------------+" << endl;
	cout << "|LinuxMCE Launch Manager, V1.0                                                 |" << endl;
	cout << "+------------------------------------------------------------------------------+" << endl;
	cout << "Core Ip: " + m_sCoreIP + "                     MySQL Host: " + m_sMySQLHost + " " << endl;
	cout << "MD Status:                              MySQL User: " + m_sMySQLUser + " " << endl;
	cout << "Video Driver:                           MySQL Pass: " + m_sMySQLPass + " " << endl;
        cout << "Resolution:                                                                     " << endl;
	//cout << "["+m_bAutostartCore?"X":" "; +"]Autostart Core                                                                                " << endl;
	cout << "                                                                                " << endl;
	cout << "                                                                                " << endl;
	cout << "+-LOG--------------------------------------------------------------------------+" << endl;
	cout << "                                                                                " << endl;
	cout << "                                                                                " << endl;
	cout << "                                                                                " << endl;


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
	//see usage at http://c-programming.suite101.com/article.cfm/using_a_mysql_databases_with_c
	if (pm_mysqlPlutoDatabase)
	{
		writeLog("Attempted to reopen already opened DB, closing current connection first", false, LV_WARNING);
		closeDB();
	}
	
	writeLog("Opening DB connection..", true);
	
	mysql_init(&m_mysqlMysql);
	if(pm_mysqlPlutoDatabase=mysql_real_connect(&m_mysqlMysql,m_sMySQLHost.c_str(),m_sMySQLUser.c_str(),m_sMySQLPass.c_str(),"pluto_main",0,0,0))
	{
		writeLog("Successful", true);
		return true;
	}
	else
	{
		//writeLog("Error: " + mysql_error(&m_mysqlMysql, true, LV_WARNING);
		writeLog("Error: Could not connect to database",true,LV_WARNING);
		delete pm_mysqlPlutoDatabase;
		pm_mysqlPlutoDatabase = NULL;
		return false;
	}
}

bool LM::closeDB()
{
	writeLog("Closing DB connection..");
	
	if (pm_mysqlPlutoDatabase)
	{
		mysql_close(pm_mysqlPlutoDatabase);
		writeLog("Successful", true);
		pm_mysqlPlutoDatabase = NULL;
		return true;
	}
	else
	{
		writeLog("No need: it wasn't open");
		return false;
	}
}

string LM::queryDB(string query)
{
	string sResult;
	MYSQL_RES *queryResult;
	MYSQL_ROW resultRow;

	mysql_query(pm_mysqlPlutoDatabase,query.c_str());

	queryResult = mysql_store_result(pm_mysqlPlutoDatabase);
	resultRow = mysql_fetch_row(queryResult);
	sResult = resultRow[0];
	mysql_free_result(queryResult);
	//if ( performQueryDB(query, queryResult) )
	//{
	//	if (queryResult.next())
	//		sResult = queryResult.value(0).toString();
	///	else
	//		writeLog("Query: \"" + query +"\" returned no records", false, LV_DEBUG);
	//}
	//else
	//{
	//	writeLog("Query: \"" + query +"\" failed", false, LV_DEBUG);
	//}
	
	return sResult;
}
/*
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

	m_sVideoResolution = queryDB("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_sMediaID + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Video_settings_CONST));
	cout << m_sVideoResolution << endl;
	
	//The old Launch Manager used a python script which use the now obsolete xorgconfig module to get the video driver name
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
	
	//system("python /usr/pluto/bin/GetVideoDriver.py /tmp/lmce_lm.video.dat");//" + VIDEO_DRIVER_TOOL.c_Str() + " " + VIDEO_DRIVER_NAME_FILE.c_str());
	// fetching video driver info
	//if ( QFile::exists(VIDEO_DRIVER_TOOL) )
	//{
	//	QProcess qp(QString(VIDEO_DRIVER_TOOL));
	//	qp.addArgument(VIDEO_DRIVER_NAME_FILE);
	//	qp.start();
	//	
	//	int iTimeout = 10; // 10 secs
	//			
	//	while(qp.isRunning() && iTimeout--)
	//	{
	//		sleep(1);
	//	}
	//	
	//	if (  qp.normalExit() && qp.exitStatus()==0 && QFile::exists(VIDEO_DRIVER_NAME_FILE) )
	//	{
	//		QFile f(VIDEO_DRIVER_NAME_FILE);
	//		if (f.open(IO_ReadOnly))
	//		{
	//			QString vDriver;
	//			int iRet = f.readLine(vDriver, 100);
	//			f.close();
	//			if (iRet != -1)
	//				leVideoDriver->setText(vDriver);
	//		}
	//		else
	//			writeLog("Failed to open file" + QString(":") + VIDEO_DRIVER_NAME_FILE, false, LV_WARNING);
	//	}
	//}
}

void LM::initialize_AudioSettings()
{
	if (m_sMediaID == "")
		return;

	string sSettings = queryDB("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_sMediaID + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Audio_settings_CONST));
	cout << sSettings;
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
	cout << "Log Levels: " + s + " " << endl;
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
			//if (m_bMediaHere)
				//m_bMediaRunning = checkMedia();
		}
		else
		{
			writeLog("Failed to connect to DCERouter!", true, LV_WARNING);
		}
	}
	/*
	tuneConnectionsWidgets();
	
	if (!m_pDevicesUpdateTimer)
	{
		m_pDevicesUpdateTimer = new QTimer();
		connect(m_pDevicesUpdateTimer, SIGNAL(timeout()), this, SLOT(updateDevicesProgressBar()));
	}
	
	
	// enabling all tabs
	for (uint i=0; i<=6; i++)
		twAllTabs->setTabEnabled(twAllTabs->page(i), true);
	
	if (!m_bMediaHere)
	{
		twAllTabs->removePage(twAllTabs->page(2));
		twAllTabs->removePage(twAllTabs->page(2));
	}
	
	// starting RA monitoring timer
	m_pRemoteAssistanceStatusRefreshTimer = new QTimer();
	connect(m_pRemoteAssistanceStatusRefreshTimer, SIGNAL(timeout()), this, SLOT(updateRAstatus()));
	m_pRemoteAssistanceStatusRefreshTimer->start(5000);
	*/
}

bool LM::checkMySQL(bool showMessage)
{
	string msg = "Connection to specified MySQL server failed";
	bool bResult = false;
	
	if ( pm_mysqlPlutoDatabase ) {
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
		 
		time_t tStart = time(NULL);
		time_t tEnd = tStart;
		
		string cmd = "/bin/nc -z " + m_sCoreIP + " " + m_sDCERouterPort;

		string sRet = "";
		const double checkCoreTimeout = 60; // 60 seconds
		
		while ( (difftime(tEnd,tStart)<checkCoreTimeout) && (sRet.size()!=1) )
		{
			writeLog("Attempting to connect to " + m_sCoreIP + ":" + m_sDCERouterPort);
			sRet = system(cmd.c_str());
			tEnd = time(NULL);
			sleep(1);
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
	if (m_pLMCE_Launch_Manager)
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
			cout <<m_sOrbiterPluginID << endl;
			//m_pLaunch_Manager->lmWidget = this;
			
			//QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
			
			//QApplication::restoreOverrideCursor();
			return true;
		}
		
		delete m_pLMCE_Launch_Manager;
		m_pLMCE_Launch_Manager = NULL;
		
		writeLog("initialize_LMdevice: Connect failed", true, LV_WARNING);
		
		if (!bRetryForever)
		{
			writeLog("initialize_LMdevice: Not retrying", true, LV_WARNING);
			//QApplication::restoreOverrideCursor();
			return false;
		}
		else
		{
			writeLog("initialize_LMdevice: Retrying (forever)", true, LV_WARNING);
		}
	}
}

// TODO refactor
void LM::updateSerialPorts()
{
	writeLog("Running UpdateAvailableSerialPorts.sh", true, LV_WARNING);
					
	string sCmd = "/usr/pluto/bin/UpdateAvailableSerialPorts.sh";
	//QString qsBackground = "0";
	//QString qsParameter = "";
	//string qsDescription = "Updating available serial ports";
	
	//QFileInfo qfi(qsCommand);
	//QString baseName = qfi.baseName(true);
	//QString qsWorkDir = QString(SCREENLOGS_BASE) + "/" + baseName;
	//QDir qd(qsWorkDir);
	//if (!qd.exists(qsWorkDir, true))
	//{
	//	qd.mkdir(qsWorkDir, true);
	//}
	
	// copied from start foreground service
	//QStringList args;
					
	//args.push_back(qsCommand);
	//if (qsParameter!="")
	//	args.push_back(qsParameter);
					
	//QProcess *pService = new QProcess(args);
	//pService->setWorkingDirectory(qsWorkDir);
	//writeLog("Starting core service " + qsCommand + ": " + args.join(" "));
	//pService->start();
	//int iCounter = 30;
	//while (pService->isRunning() && iCounter)
	//{
	//	sleep(1);
	//	iCounter--;
	//	qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
	//}
					
	//if (pService->isRunning())
	//{
	//	writeLog("Process didn't exit after 30 secs of running, let it run in background", true, LV_WARNING);
	//	m_qpvCoreServices.push_back(pService);
	//}
	//else
	//{
	
	//TODO: Fork this and monitor progress and implement a timeout
	string s;
	s=system(sCmd.c_str());
	writeLog("Process completed with status: " + s);
	//	delete pService;
	//}
					
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
	QStringList qsl;
	QFile f(QString(PLUTO_LOCKS));
	if (f.open(IO_ReadOnly))
	{
		QString sID;
		int iRet;
	
		while ( !f.atEnd() )
		{
			if  ( (iRet = f.readLine(sID, 100))!=-1)
			{
				sID = sID.replace(" ", "");
				sID = sID.replace("\n", "");
				sID = sID.replace("\t", "");
				
				if (sID != "") 
				{
					qsl.append(sID);
				}
			}
		}
		
		f.close();

		writeLog("Detected running devices: " + qsl.join(" "));
	}
	else
	{
		writeLog("Failed to open file" + QString(":") + PLUTO_LOCKS, false, LV_WARNING);
		writeLog("Continuing, assuming no devices are running", false, LV_WARNING);
	}
	
	// purging dead devices
	if (eraseDeadLocalDevices)
	{
		QValueVector<int> newV;
		
		// cleaning up dead core devices
		for (QValueVector<int>::iterator it=m_qvvCoreDevices.begin(); it!=m_qvvCoreDevices.end(); ++it)
		{
			QString sID = QString::number(*it);
			if (qsl.find(sID) == qsl.end())
			{
				writeLog("Device " + QString::number(*it) + " seem to be dead" );
			}
			else
				newV.append(*it);
		}
		
		m_qvvCoreDevices  = newV;
		newV.clear();
		
		// cleaning up dead core devices
		for (QValueVector<int>::iterator  it=m_qvvMediaDevices.begin(); it!=m_qvvMediaDevices.end(); ++it)
		{
			QString sID = QString::number(*it);
			if (qsl.find(sID) == qsl.end())
			{
				writeLog("Device " + QString::number(*it) + " seem to be dead");
			}
			else
				newV.append(*it);
		}
		
		m_qvvMediaDevices  = newV;
		newV.clear();
	}
}

void LM::startCoreDevices(bool checkForAlreadyRunning)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//	m_iDevicesLevel = 0;
//	m_pDevicesUpdateTimer->start(1000);
	
	writeLog("startCoreDevices()");

	if ( pPlutoDatabase ) {
		// fetching list of non-MD devices under Core
		QStringList devices;
		devices.append(m_qsCoreDeviceID);
		
		// filtering out the MD/Hybrid PC
		QString extraCondition = "";
		if (m_qsMediaID!="")
			extraCondition += " AND PK_Device<>" + m_qsMediaID;
		    
		for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)
		{
			QString query = QString("SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE, Device.FK_DeviceTemplate,DeviceTemplate.IsPlugin FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE (PK_Device=") + *it + extraCondition + ")";
			QSqlQuery q;
			bool bQueryExecResult = performQueryDB(query, q);
		
			// if device exist
			while ( bQueryExecResult && q.next() )
			{
				if (q.value(3).toString()=="1")
					writeLog("Not starting device " + q.value(0).toString() + " "  + q.value(1).toString() + " - it is disabled", true);
				else if (q.value(6).toInt()==1)
					writeLog("Not starting device " + q.value(0).toString() + " "  + q.value(1).toString() + " - it is plugin", true);
				else
				{			
					// if device implements DCE, start it
					if (q.value(4).toString()=="1")
					{
						bool alreadyRunning=false;
						
						if (checkForAlreadyRunning)
							for (QValueVector<int>::iterator jt=m_qvvCoreDevices.begin(); jt!=m_qvvCoreDevices.end(); ++jt)
							{
								if ( *jt == q.value(0).toInt() )
								{
									alreadyRunning = true;
									break;
								}
							}
						
						if (alreadyRunning)
						{
							writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - it is already running", true);
						}
						else
						{
							writeLog("Starting device " +  q.value(0).toString() + " "  + q.value(1).toString(), true);
							
							//screen -d -m -S "$Description-$DeviceID" /usr/pluto/bin/Spawn_Device.sh $DeviceID $DCERouter $Command	
												
							QString deviceCommand = q.value(2).toString();
							if (deviceCommand=="")
								deviceCommand = q.value(1).toString().simplifyWhiteSpace().replace(" ","_");
							
							if (!QFile::exists("/usr/pluto/bin/"+deviceCommand))
							{
								writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " -  binary is not found, probably it is in the middle of installation", true, LV_WARNING);
								continue;
							}
							
							QString screenName = q.value(1).toString();
							screenName = FileUtils::ValidCPPName(screenName) + "-" + q.value(0).toString();

							// starting device
							QStringList args;
	
							args.push_back("/usr/bin/screen");
							args.push_back("-d");
							args.push_back("-m");
							args.push_back("-S");
							args.push_back(screenName);
							args.push_back("/usr/pluto/bin/Spawn_Device.sh");
							args.push_back(q.value(0).toString());
							args.push_back(leCoreIP->text());
							args.push_back(deviceCommand);
							
							QProcess process(args);
							writeLog("Starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + ": " + args.join(" "));
							process.start();
							
							// wait for register
							waitForDevice(q.value(0).toInt());
							
							// recording device in list
							m_qvvCoreDevices.append(q.value(0).toInt());
						}
					}
					else
					{
						writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - it doesn't implement DCE. Adding its children into start queue instead", false);
						
						// adding child devices of this device - maybe some of them implement DCE
						QString child_query = QString("SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=") + q.value(0).toString();
						QString children = "[ ";
						QSqlQuery cq;
						bool bChildQueryExecResult = performQueryDB(child_query, cq);

						while( bChildQueryExecResult && cq.next() )
						{
							devices.append(cq.value(0).toString());
							children += cq.value(0).toString() + " ";
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
	
	QApplication::restoreOverrideCursor();
}


void LM::startMediaDevices(bool checkForAlreadyRunning)
{
	if (m_qsMediaID=="")
		return;
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
//	m_iDevicesLevel = 0;
//	m_pDevicesUpdateTimer->start(1000);

	writeLog("startMediaDevices()");
	
	if ( pPlutoDatabase  ) {
		// fetching full list of devices under current MD
		QStringList devices;
		devices.append(m_qsMediaID);
		for (QStringList::iterator it=devices.begin(); it!=devices.end(); ++it)
		{
			QString query = QString("SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled, DeviceTemplate.ImplementsDCE FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE  PK_Device=") + *it;
			QSqlQuery q;
			bool bQueryExecResult = performQueryDB(query, q);
			
			// if device exist
			if ( bQueryExecResult && q.next() )
			{
				if (q.value(3).toString()=="1")
					writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - it is disabled", true);
				else
				{
					// starting device if it implements DCE
					if (q.value(4).toString()=="1")
					{
						bool alreadyRunning=false;
						
						if (checkForAlreadyRunning)
							for (QValueVector<int>::iterator jt=m_qvvMediaDevices.begin(); jt!=m_qvvMediaDevices.end(); ++jt)
							{
								if ( *jt == q.value(0).toInt() )
								{
									alreadyRunning = true;
									break;
								}
							}
						
						if (alreadyRunning)
						{
							writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - it is already running", true);
						}
						else
						{
							writeLog("Starting device " +  q.value(0).toString() + " "  + q.value(1).toString(), true);
							
							//screen -d -m -S "$Description-$DeviceID" /usr/pluto/bin/Spawn_Device.sh $DeviceID $DCERouter $Command	
												
							QString deviceCommand = q.value(2).toString();
							if (deviceCommand=="")
								deviceCommand = q.value(1).toString().simplifyWhiteSpace().replace(" ","_");
							
							if (!QFile::exists("/usr/pluto/bin/"+deviceCommand))
							{
								writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - binary is not found, probably it is in the middle of installation", true, LV_WARNING);
								continue;
							}
							
							QString screenName = q.value(1).toString();
							screenName = FileUtils::ValidCPPName(screenName) + "-" + q.value(0).toString();
							
							// starting device
							QStringList args;
	
							args.push_back("/usr/bin/screen");
							args.push_back("-d");
							args.push_back("-m");
							args.push_back("-S");
							args.push_back(screenName);
							args.push_back("/usr/pluto/bin/Spawn_Device.sh");
							args.push_back(q.value(0).toString());
							args.push_back(leCoreIP->text());
							args.push_back(deviceCommand);
							
							QProcess process(args);
							writeLog("Starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + ": " + args.join(" "));						
							process.start();
							
							// wait for register
							waitForDevice(q.value(0).toInt());
							
							// recording device ID in list
							m_qvvMediaDevices.push_back(q.value(0).toInt());
						}
					}
					else
					{
						writeLog("Not starting device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - it doesn't implement DCE. Adding its children into start queue instead", false);
						// starting devices recursively
						QString child_query = QString("SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=") + *it;
						QString children = "[ ";
						QSqlQuery cq;
						bool bChildQueryExecResult = performQueryDB(child_query, cq);
						while(bChildQueryExecResult && cq.next())
						{
							devices.append(cq.value(0).toString());
							children += cq.value(0).toString() + " ";
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
	
	QApplication::restoreOverrideCursor();
}

