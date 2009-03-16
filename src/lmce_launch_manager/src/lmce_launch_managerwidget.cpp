#include <qlabel.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qprocess.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <qsocketdevice.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qptrvector.h>
#include <qprogressbar.h>
#include <qapplication.h>
#include <qthread.h>
#include <qsettings.h>
#include <qdesktopwidget.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qcursor.h>
#include <qdns.h>

#include "lmce_launch_managerwidget.h"

#include "Launch_Manager.h"

#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"
#include "DCE/ClientSocket.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/ProcessUtils.h"


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

extern char **environ;

const QString qsVideoRestoreInfo("If the resolution changes make your display unviewable, reset and hold down the \n"
	"shift key while booting to reset the resolution and start the audio/video setup wizard.");

lmce_launch_managerWidget::lmce_launch_managerWidget(QWidget* parent, const char* name, WFlags fl)
        : lmce_launch_managerWidgetBase(parent,name,fl)
{
	tlStatusMessages->hide();
	
	m_pLaunch_Manager = NULL;
	m_pLogsUpdateTimer = NULL;
	m_pProgressBarUpdateTimer = NULL;
	m_pDevicesUpdateTimer = NULL;
	m_pRemoteAssistanceStatusRefreshTimer = NULL;
	
#ifdef START_STOP_DEBUG
	m_pStartStopDebugTimer = NULL;
#endif

	pPlutoDatabase = NULL;
	
	for (uint i=0; i<=6; i++)
		twAllTabs->setTabEnabled(twAllTabs->page(i), false);
	twAllTabs->setCurrentPage(0);

	m_bRegenInProgress = false;
	m_bRegenTrackingCurrentOrbiterOnly = false;
	
	m_bCoreRunning = false;
	m_bMediaRunning = false;

	m_bCoreHere = false;
	m_bMediaHere = false;
	
	allowRedraw = true;
	
	tuneConnectionsWidgets();
}

lmce_launch_managerWidget::~lmce_launch_managerWidget()
{
	closeDB();
	
	if (m_pLogsUpdateTimer)
	{
		m_pLogsUpdateTimer->stop();
		delete m_pLogsUpdateTimer;
	}
	
	if (m_pProgressBarUpdateTimer)
	{
		m_pProgressBarUpdateTimer->stop();
		delete m_pProgressBarUpdateTimer;
	}
	
	if (m_pDevicesUpdateTimer)
	{
		m_pDevicesUpdateTimer->stop();
		delete m_pDevicesUpdateTimer;
	}
	
	if (m_pRemoteAssistanceStatusRefreshTimer)
	{
		m_pRemoteAssistanceStatusRefreshTimer->stop();
		delete m_pRemoteAssistanceStatusRefreshTimer;
	}

#ifdef START_STOP_DEBUG
	if (m_pStartStopDebugTimer)
	{
		m_pStartStopDebugTimer->stop();
		delete m_pStartStopDebugTimer;
	}
#endif
	
	deinitialize_LMdevice();
}

/*$SPECIALIZATION$*/

void lmce_launch_managerWidget::deinitialize_LMdevice()
{
	if (m_pLaunch_Manager)
	{
		m_pLaunch_Manager->OnQuit();
		pthread_join(m_pLaunch_Manager->m_RequestHandlerThread, NULL);
		delete m_pLaunch_Manager;
		m_pLaunch_Manager = NULL;
	}
}

void lmce_launch_managerWidget::initialize_ViewLog()
{
	updateLogsList();
	
	m_pLogsUpdateTimer = new QTimer(this);
	connect(m_pLogsUpdateTimer, SIGNAL(timeout()), this, SLOT(updateLogsList()));
}

void lmce_launch_managerWidget::btnViewLog_clicked()
{
	QListViewItem *item = lvLogs->selectedItem();
	if (item)
	{
		QString s = item->text(2);
		if (s!=QString::null)
		{
			QStringList args;
			args.push_back("konsole");
			
			args.push_back("-T");
			
			if (s.endsWith(".gz"))
				args.push_back("zless -R " + s);
			else
			{
				args.push_back("less -R " + s);
			}
			
			args.push_back("--nomenubar");
			args.push_back("--notabbar");
			args.push_back("-e");
			
			if (s.endsWith(".gz"))
				args.push_back("zless");
			else
			{
				args.push_back("less");
			}
			
			args.push_back("-R");
			args.push_back(s);
			
			QProcess process(args);			
			writeLog("Spawning log viewer: " + args.join(" "));			
			process.start();
		}
	}
}

void lmce_launch_managerWidget::btnTailLog_clicked()
{
	QListViewItem *item = lvLogs->selectedItem();
	if (item)
	{
		QString s = item->text(2);
		if (s!=QString::null)
		{
			QStringList args;
			args.push_back("konsole");			
			args.push_back("-T");
			args.push_back("tail -f " + s);
			args.push_back("--nomenubar");
			args.push_back("--notabbar");
			args.push_back("-e");
			args.push_back("tail");
			args.push_back("-f");
			args.push_back(s);
			
			QProcess process(args);
			writeLog("Spawning log tailer: " + args.join(" "));			
			process.start();
		}
	}
}

void lmce_launch_managerWidget::initialize_Start()
{
	writeLog("Checking for mysql...");
	bool bMySQL = checkMySQL();
	writeLog(QString("Done checking for mysql...") + (bMySQL?"connection OK":"connection failed"));
	
	//TODO decide are we dedicated core, hybrid or disk-based MD
	
	writeLog("Checking for core services...");
	bool bCore = checkCore(leCoreIP->text());	
	writeLog(QString("Done checking for core services...") + (bCore?"core is running":"core is not running") );
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
			writeLog(QString("Failed to connect to DCERouter!"), true, LV_WARNING);
		}
	}
	
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
}

void lmce_launch_managerWidget::initialize_Connections()
{
	// initing connections
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);

	QString s = pConfig->m_mapParameters_Find("DCERouter");
	leCoreIP->setText(s);
	
	s = pConfig->m_mapParameters_Find("MySqlHost");
	leDBIP->setText(s);

	s = pConfig->m_mapParameters_Find("MySqlUser");
	leDBUser->setText(s);

	s = pConfig->m_mapParameters_Find("MySqlPassword");
	leDBPass->setText(s);

	m_qsMySQLPort = pConfig->m_mapParameters_Find("MySqlPort");
	if (m_qsMySQLPort=="")
		m_qsMySQLPort = "3306";
	
	m_qsDCERouterPort = pConfig->m_mapParameters_Find("DCERouterPort");
	if (m_qsDCERouterPort== "")
		m_qsDCERouterPort = "3450";
	
	cbAutostartCore->setChecked( pConfig->m_mapParameters_Find("AutostartCore") == "1" );
	cbAutostartMedia->setChecked( pConfig->m_mapParameters_Find("AutostartMedia") == "1" );
	
	m_bRemoteAssistanceRunning = checkRAStatus();
	
	// trying to connect to DB
	openDB();
	
	m_qsDeviceID = pConfig->m_mapParameters_Find("PK_Device");
	
	// detecting core
	m_qsCoreDeviceID = queryDB("SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=7");
	if (m_qsCoreDeviceID=="")
	{
		m_qsCoreDeviceID = "1";
		writeLog("Hmm... core device ID is empty, setting it to 1", false, LV_WARNING);
	}
	else
		writeLog("Detected core as device #" + m_qsCoreDeviceID, false, LV_WARNING);
	
	QString mdID;
	
	if (m_qsDeviceID == m_qsCoreDeviceID)
	{
		m_bCoreHere = true;
		mdID = queryDB("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + m_qsDeviceID + " AND FK_DeviceCategory IN (8)");
		
		if (mdID != "")
		{
			m_bMediaHere = true;
		}
	}
	else
	{
		m_bMediaHere = true;
		mdID = m_qsDeviceID;
	}
	
	if (m_qsDeviceID!="")
	{
		QString localOrbiterID = queryDB("SELECT PK_Device FROM Device LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=" + mdID + " AND FK_DeviceCategory IN (2, 3, 5)");
		m_qsOrbiterID = localOrbiterID;
	}

	m_qsMediaID = mdID;
	
	delete pConfig;
}
	
void lmce_launch_managerWidget::initialize_VideoSettings()
{
	if (m_qsOrbiterID == "")
		return;

	QString videoResolution = queryDB( QString("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_qsMediaID + " AND FK_DeviceData=" + 
		QString::number(DEVICEDATA_Video_settings_CONST) ));
	leVideoResolution->setText(videoResolution);
	
	// fetching video driver info
	if ( QFile::exists(VIDEO_DRIVER_TOOL) )
	{
		QProcess qp(QString(VIDEO_DRIVER_TOOL));
		qp.addArgument(VIDEO_DRIVER_NAME_FILE);
		qp.start();
		
		int iTimeout = 10; // 10 secs
				
		while(qp.isRunning() && iTimeout--)
		{
			sleep(1);
		}
		
		if (  qp.normalExit() && qp.exitStatus()==0 && QFile::exists(VIDEO_DRIVER_NAME_FILE) )
		{
			QFile f(VIDEO_DRIVER_NAME_FILE);
			if (f.open(IO_ReadOnly))
			{
				QString vDriver;
				int iRet = f.readLine(vDriver, 100);
				f.close();
				if (iRet != -1)
					leVideoDriver->setText(vDriver);
			}
			else
				writeLog("Failed to open file" + QString(":") + VIDEO_DRIVER_NAME_FILE, false, LV_WARNING);
		}
	}
}

void lmce_launch_managerWidget::initialize_AudioSettings()
{
	if (m_qsMediaID == "")
		return;

	QString aSettings = queryDB( QString("SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=" + m_qsMediaID + " AND FK_DeviceData=" +
			QString::number(DEVICEDATA_Audio_settings_CONST) ));
	
	if (aSettings=="")
		return;
	
	cbAC3Pass->setChecked(aSettings.contains("3"));
	
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
}

void lmce_launch_managerWidget::initialize_LogOptions()
{
	// initing list of log levels
	initLogLevels();
	
	// filling list
	lvLogLevels->setSorting(-1);
	
	{
		QMap<int,QString>::iterator i=m_qmapLogLevels.end();
		while (i!=m_qmapLogLevels.begin())
		{
			--i;
			new QCheckListItem(lvLogLevels, *i, QCheckListItem::CheckBox);
		}
	}
	
	// hiding progressbar
	pbProgress->hide();
	
	// initing connections
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	QString s = pConfig->m_mapParameters_Find("LogLevels");
	delete pConfig;

	QStringList levels = QStringList::split(',', s, false);

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
	}
}

void lmce_launch_managerWidget::initialize_ConfigurationFiles()
{
	new QListViewItem(lvConfigs, "Linux MCE", PLUTO_CONFIG);
	new QListViewItem(lvConfigs, "MySQL", "/etc/mysql/my.cnf");
	new QListViewItem(lvConfigs, "DHCP", "/etc/dhcp3/dhclient.conf");
	new QListViewItem(lvConfigs, "Apache", "/etc/apache2/apache2.conf");
	new QListViewItem(lvConfigs, "Network", "/etc/network/interfaces");
	new QListViewItem(lvConfigs, "Resolver", "/etc/resolv.conf");
}
	
void lmce_launch_managerWidget::btnEditConfigurationFile_clicked()
{
	QListViewItem *item = lvConfigs->selectedItem();
	if (item)
	{
		QString s = item->text(1);
		if (s!=QString::null)
		{
			QStringList args;
			args.push_back("kate");
			args.push_back(s);
			
			QProcess process(args);
			writeLog("Spawning config editor: " + args.join(" "));			
			process.start();
		}
	}
}
	
void lmce_launch_managerWidget::initLogLevels()
{
	m_qmapLogLevels[LV_CRITICAL] = "LV_CRITICAL";
	m_qmapLogLevels[LV_WARNING] = "LV_WARNING";
	m_qmapLogLevels[LV_EVENTHANDLER] = "LV_EVENTHANDLER";
	m_qmapLogLevels[LV_EVENT] = "LV_EVENT";
	m_qmapLogLevels[LV_ACTION] = "LV_ACTION";
	m_qmapLogLevels[LV_CRITERIA] = "LV_CRITERIA";
	m_qmapLogLevels[LV_STATUS] = "LV_STATUS";
	m_qmapLogLevels[LV_EPG] = "LV_EPG";
	m_qmapLogLevels[LV_ALARM] = "LV_ALARM";
	m_qmapLogLevels[LV_SOCKET] = "LV_SOCKET";
	m_qmapLogLevels[LV_LIGHTING] = "LV_LIGHTING";
	m_qmapLogLevels[LV_CONTROLLER] = "LV_CONTROLLER";
	m_qmapLogLevels[LV_APPDAEMON] = "LV_APPDAEMON";
	m_qmapLogLevels[LV_CAMERA] = "LV_CAMERA";
	m_qmapLogLevels[LV_CORPCLIENT] = "LV_CORPCLIENT";
	m_qmapLogLevels[LV_FILEREQUEST] = "LV_FILEREQUEST";
	m_qmapLogLevels[LV_DATAGRID] = "LV_DATAGRID";
	m_qmapLogLevels[LV_LOCKING] = "LV_LOCKING";
	m_qmapLogLevels[LV_REGISTRATION] = "LV_REGISTRATION";
	m_qmapLogLevels[LV_XINE] = "LV_XINE";
	m_qmapLogLevels[LV_BURNER] = "LV_BURNER";
	m_qmapLogLevels[LV_1394] = "LV_1394";
	m_qmapLogLevels[LV_SPHINX] = "LV_SPHINX";
	m_qmapLogLevels[LV_FESTIVAL] = "LV_FESTIVAL";
	m_qmapLogLevels[LV_IR] = "LV_IR";
	m_qmapLogLevels[LV_START] = "LV_START";
	m_qmapLogLevels[LV_START_DEVICE] = "LV_START_DEVICE";
	m_qmapLogLevels[LV_STOP] = "LV_STOP";
	m_qmapLogLevels[LV_MOUSE_CLICKS] = "LV_MOUSE_CLICKS";
	m_qmapLogLevels[LV_EPG_UPDATE] = "LV_EPG_UPDATE";
	m_qmapLogLevels[LV_MEDIA] = "LV_MEDIA";
	m_qmapLogLevels[LV_DEBUG] = "LV_DEBUG";
	m_qmapLogLevels[LV_ZWAVE] = "LV_ZWAVE";
	m_qmapLogLevels[LV_PROFILER] = "LV_PROFILER";
	m_qmapLogLevels[LV_PROCESSUTILS] = "LV_PROCESSUTILS";
	m_qmapLogLevels[LV_TV] = "LV_TV";
	m_qmapLogLevels[LV_COLOR] = "LV_COLOR";
}
	
	
void lmce_launch_managerWidget::btnApplyLogOptions_clicked()
{
	QStringList ls;
	for (QMap<int,QString>::iterator i=m_qmapLogLevels.begin(); i!=m_qmapLogLevels.end(); i++)
	{
		QCheckListItem *pItem = (QCheckListItem*) lvLogLevels->findItem(*i, 0);
		if (pItem && (pItem->state()==QCheckListItem::On))
			ls.append(QString::number(i.key()));
	}
	
	string s = ls.join(",");
	
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	
	pConfig->AddString("LogLevels", s);
	pConfig->WriteSettings();
	
	delete pConfig;
	
	writeLog("Saved new settings to " PLUTO_CONFIG);
	
	// tell all devices re-read log options
	// /usr/pluto/bin/MessageSend dcerouter 0 -1002 1 11
	// message to router=-1002, command=1, SYSCOMMAND_RELOAD_LOGLEVEL=11
	QProcess process(QString("/usr/pluto/bin/MessageSend"));
	process.addArgument("dcerouter");
	process.addArgument("0");
	process.addArgument("-1002");
	process.addArgument("1");
	process.addArgument("11");
	process.start();
}
	
void lmce_launch_managerWidget::writeLog(QString s, bool toScreen, int logLevel)
{
	if (toScreen)
	{
		QString timestamp = QDateTime::currentDateTime().toString();
		QListViewItem *pItem = new QListViewItem(lbMessages, lbMessages->lastItem(), timestamp,  s);
		lbMessages->ensureItemVisible(pItem);
		lbMessages->update();
		doRedraw();
	}

#ifdef DEBUG
	if ( logLevel!=LV_CRITICAL ) 
		logLevel = LV_WARNING;
#endif
	
	DCE::LoggerWrapper::GetInstance()->Write(logLevel, s);
}
	
void lmce_launch_managerWidget::btnSaveConnectionsSettings_clicked()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	
	pConfig->AddString("DCERouter", (string) leCoreIP->text().latin1());
	pConfig->AddString("MySqlHost", (string) leDBIP->text().latin1());
	pConfig->AddString("MySqlUser", (string) leDBUser->text().latin1());
	pConfig->AddString("MySqlPassword", (string) leDBPass->text().latin1());
	pConfig->WriteSettings();
	
	delete pConfig;
	
	writeLog("Saved new settings to " PLUTO_CONFIG);

	//TODO should we restart anything here?
}

void lmce_launch_managerWidget::btnRotateAllLogs_clicked()
{
	// rotating logs
	// /usr/pluto/bin/MessageSend dcerouter 0 -1002 1 10
	// message to router=-1002, command=1, SYSCOMMAND_ROTATE=10
	QProcess process(QString("/usr/pluto/bin/MessageSend"));
	process.addArgument("dcerouter");
	process.addArgument("0");
	process.addArgument("-1002");
	process.addArgument("1");
	process.addArgument("10");
	process.start();
	
	updateLogsList();
}

void lmce_launch_managerWidget::updateLogsList()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				
	writeLog("=>updateLogsList()");
	
	lvLogs->setSorting(-1);

	QStringList all_files;
	
	all_files.append("/var/log/messages");
	all_files.append("/var/log/syslog");
	
//	writeLog("=>updateLogsList() - stage1");
	// create all list of files
	QDir dir(PLUTO_LOG_DIR, QString::null, QDir::Name, QDir::Files);
	for (uint i=0; i<dir.count(); i++)
	{
		QString fname = dir[i];
		QString full_fname = PLUTO_LOG_DIR "/" + fname;
		all_files.append(full_fname);
	}
	
//	writeLog("=>updateLogsList() - stage2");
	
	map<QString, QListViewItem *> new_list;
	
	for (uint i=0; i<all_files.count(); i++)
	{
		QString full_fname = all_files[i];
		QFileInfo finfo(full_fname);
		
		map<QString, QListViewItem *>::iterator it = m_mapLogListItems.find(full_fname);
		if (it!=m_mapLogListItems.end())
		{
			it->second->setText(1, QString::number(finfo.size()));
			new_list[full_fname] = it->second;
			m_mapLogListItems.erase(it);
		}
		else
		{
			new_list[full_fname] = new QListViewItem(lvLogs, finfo.fileName(), QString::number(finfo.size()), full_fname);
		}
		
	}

//	writeLog("=>updateLogsList() - stage3");

	for (map<QString, QListViewItem *>::iterator it = m_mapLogListItems.begin(); it!=m_mapLogListItems.end(); ++it)
	{
		delete it->second;
	}
	
	m_mapLogListItems.clear();
	
//	writeLog("=>updateLogsList() - stage4");
	
	m_mapLogListItems = new_list;
	
//	writeLog("=>updateLogsList() - stage5");
	
	lvLogs->setSorting(0);

	writeLog("<=updateLogsList()");
	

	// scheduling again
	if (m_pLogsUpdateTimer)
	{
		if ( twAllTabs->tabLabel( twAllTabs->currentPage() ) == "&View Log" )
		{
			writeLog("Scheduling logs list updater", false);
			m_pLogsUpdateTimer->start(LOG_REFRESH_INTERVAL, true);
		}
	}
	
	QApplication::restoreOverrideCursor();
}
		
void lmce_launch_managerWidget::lvLogs_currentChanged( QListViewItem * item)
{
	// we can only tail files that are not archived
	QString fname = item->text(0);
	btnTailLog->setDisabled( fname.endsWith(".gz") || fname.endsWith(".bz2") );
}

void lmce_launch_managerWidget::btnOpenAdminSite_clicked()
{
	QString browserApp = "/usr/bin/x-www-browser";
	
	// trying to find firefox
	
	if ( QFile::exists("/usr/bin/firefox") )
		browserApp = "/usr/bin/firefox";
	else
		if ( QFile::exists("/usr/bin/mozilla-firefox") )
			browserApp = "/usr/bin/mozilla-firefox";
		else
			writeLog("Firefox not found, falling back to default system browser: " + browserApp, false, LV_WARNING);
	
	QProcess process(browserApp);
	process.addArgument("http://"+leCoreIP->text()+"/pluto-admin/");
	process.start();
}

void lmce_launch_managerWidget::btnTestMySQLConnection_clicked()
{
	closeDB();
	openDB();
	checkMySQL(true);
}

bool lmce_launch_managerWidget::checkMySQL(bool showMessageBox)
{
	QString msg = "Connection to specified MySQL server failed";
	bool bResult = false;
	
	if ( pPlutoDatabase ) {
		msg = "Connection to specified MySQL server succeeded";
		bResult = true;
	}

	if (showMessageBox)
	{
		QMessageBox mb(QString("Checking MySQL..."), msg, QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		mb.exec();
	}
	
	return bResult;
}

void lmce_launch_managerWidget::initialize()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	lbMessages->setSorting(-1);
	
	DCE::LoggerWrapper::SetType(LT_LOGGER_FILE, PLUTO_LOG_DIR "/LaunchManager.log");
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting...");
	
	// setting our settings storage path
	appSettings.setPath("linuxmce.com", "lmce_launch_manager",  QSettings::User);
	
	
	writeLog("=>initialize_Connections()");
	initialize_Connections();
	
	writeLog("=>initialize_VideoSettings()");
	initialize_VideoSettings();
	writeLog("=>initialize_AudioSettings()");
	initialize_AudioSettings();
	writeLog("=>initialize_LogOptions()");
	initialize_LogOptions();
	writeLog("=>initialize_ViewLog()");
	initialize_ViewLog();
	writeLog("=>initialize_ConfigurationFiles()");
	initialize_ConfigurationFiles();

	// after all is read, initializing server
	writeLog("=>initialize_Start()");
	initialize_Start();
	
	// creating required dir
	QDir qdir(SCREENLOGS_BASE);
	if (!qdir.exists(SCREENLOGS_BASE, true))
		qdir.mkdir(SCREENLOGS_BASE, true);

	// if the system was already running when we started, let's catch new children and report we are up
	if ( m_bCoreRunning || m_bMediaRunning )
	{
          updateSerialPorts();
		respawnNewChildren();
		reportDeviceUp();
	}
	
	writeLog("=>loadSettings()");
	// reding autostart settings
	loadSettings();
	
	writeLog("Started!");
	
	QApplication::restoreOverrideCursor();
	
#ifdef START_STOP_DEBUG
	m_pStartStopDebugTimer = new QTimer(this);
	connect(m_pStartStopDebugTimer, SIGNAL(timeout()), this, SLOT(btnSwitchCoreAndMedia_clicked()));
	m_pStartStopDebugTimer->start(30000);
#endif

}

bool lmce_launch_managerWidget::initialize_LMdevice(bool bRetryForever/*=false*/)
{
	// if we already have a connection
	if (m_pLaunch_Manager)
		return true;
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	string sCoreIP = leCoreIP->text();
	
	if (sCoreIP == "localhost")
		sCoreIP = "127.0.0.1";
	
	writeLog(QString("Connecting to router at ") + sCoreIP, true, LV_STATUS);
	
	while (true)
	{
		m_pLaunch_Manager = new DCE::Launch_Manager(
				m_qsDeviceID.toInt(), sCoreIP,  true, false);

		bool bConnected = m_pLaunch_Manager->GetConfig();
		if ( bConnected && m_pLaunch_Manager->m_pEvent->m_pClientSocket->m_eLastError==DCE::ClientSocket::cs_err_NeedReload )
		{
			lbMessages->clear();
			writeLog(QString("Please go to an existing Orbiter and choose 'quick reload router'. "), true, LV_WARNING);
			writeLog(QString("This media director will start after you do..."), true, LV_WARNING);

			
			sleep(10);

			writeLog(QString("initialize_LMdevice: router should be reloaded, retrying connect"), false, LV_WARNING);
			delete m_pLaunch_Manager;
			m_pLaunch_Manager = NULL;
		}
		if( bConnected && m_pLaunch_Manager->Connect(m_pLaunch_Manager->PK_DeviceTemplate_get()) ) 
		{
			writeLog(QString("initialize_LMdevice: Connect OK"), true, LV_STATUS);
			
			map<int,string> devicesMap;
			m_pLaunch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &devicesMap);
			if (devicesMap.empty())
			{
				writeLog("initialize_LMdevice: Failed to find Orbiter plugin", false, LV_WARNING);
			}
		
			map<int,string>::iterator it = devicesMap.begin();
		
			m_qsOrbiterPluginID = QString::number( (*it).first );
			
			m_pLaunch_Manager->lmWidget = this;
			
			QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
			
			QApplication::restoreOverrideCursor();
			return true;
		}
		
		delete m_pLaunch_Manager;
		m_pLaunch_Manager = NULL;
		
		writeLog(QString("initialize_LMdevice: Connect failed"), true, LV_WARNING);
		
		if (!bRetryForever)
		{
			writeLog(QString("initialize_LMdevice: Not retrying"), true, LV_WARNING);
			QApplication::restoreOverrideCursor();
			return false;
		}
		else
		{
			writeLog(QString("initialize_LMdevice: Retrying (forever)"), true, LV_WARNING);
		}
	}
}

bool lmce_launch_managerWidget::checkCore(QString coreIP)
{
	writeLog("Checking if core is up...", true);
	
	struct stat fInfo;
	
	if ( lstat(DCEROUTER_LOCK, &fInfo)==0 )
	{
		writeLog("Found router lock file at " + QString(DCEROUTER_LOCK));
		
		uint tStart = QDateTime::currentDateTime().toTime_t();
		uint tEnd = tStart;
		
		QString cmd = "/bin/nc -z " + coreIP + " " + m_qsDCERouterPort;
		int iRet = 1;
		const int checkCoreTimeout = 60; // 60 seconds
		
		while ( (tEnd-tStart<checkCoreTimeout) && (iRet!=0) )
		{
			writeLog("Attempting to connect to " + coreIP + ":" + m_qsDCERouterPort);
			iRet = system(cmd);
			tEnd = QDateTime::currentDateTime().toTime_t();
			sleep(1);
		}
		
		if (iRet != 0)
		{
			writeLog("Timeout waiting for router restart, suggesting user to reboot the system");
			
			int iResult = QMessageBox::warning(this, "Important", "The DCERouter is not responding. A reboot may be needed. Reboot the system now?", 
					     QMessageBox::Yes, QMessageBox::No);
			
			if (iResult == QMessageBox::Yes)
			{
				writeLog("User selected to reboot box");
				rebootPC();				
				return false;
			}
			else
			{
				writeLog("User selected not to reboot box");
				return false;
			}
			
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

void lmce_launch_managerWidget::tuneConnectionsWidgets()
{
	// set visibility & controls description
	if (m_bMediaHere && m_bCoreRunning)
		btnRegenThisOrbiter->show();
	else
		btnRegenThisOrbiter->hide();
	
	if ( m_bCoreRunning)
		btnRegenAllOrbiters->show();
	else
		btnRegenAllOrbiters->hide();
		
	// dedicated core
	if (m_bCoreHere && !m_bMediaHere)
	{
		btnSwitchCore->show();
		btnSwitchCoreAndMedia->hide();
		btnSwitchMedia->hide();
		btnActivateOrbiter->hide();
		
		btnSwitchCore->setText(m_bCoreRunning?"Stop &Core services":"Start &Core services");
		btnSwitchCore->setPaletteForegroundColor( (m_bCoreRunning)?QColor(255,0,0):QColor(0,170,0));
		
		btn_SwitchRA->show();
	}
	else
	// hybrid
	if (m_bCoreHere && m_bMediaHere)
	{
		btnSwitchCore->hide();
		btnSwitchCoreAndMedia->hide();
		btnSwitchMedia->hide();
		btnActivateOrbiter->hide();
		
		btnSwitchCore->setText(m_bCoreRunning?"Stop &Core services":"Start &Core services");
		btnSwitchCore->setPaletteForegroundColor( (m_bCoreRunning)?QColor(255,0,0):QColor(0,170,0));
		
		btnSwitchMedia->setText(m_bMediaRunning?"Stop &Media Station":"Start &Media Station");
		btnSwitchMedia->setPaletteForegroundColor( (m_bMediaRunning)?QColor(255,0,0):QColor(0,170,0));
		
		if (m_bMediaRunning)
			btnActivateOrbiter->show();
		else
			btnActivateOrbiter->hide();
		
		btnSwitchCoreAndMedia->setText( (m_bCoreRunning&&m_bMediaRunning)?"&Stop Linux MCE":"&Start Linux MCE" );
		btnSwitchCoreAndMedia->setPaletteForegroundColor( (m_bCoreRunning&&m_bMediaRunning)?QColor(255,0,0):QColor(0,170,0));
		
		if (!m_bCoreRunning)
		{
			btnSwitchCore->show();
			btnSwitchCoreAndMedia->show();
		}
		else
			if (m_bCoreRunning && !m_bMediaRunning)
			{
				btnSwitchCore->show();
				btnSwitchMedia->show();
		}
		else
			if (m_bCoreRunning && m_bMediaRunning)
			{
				btnSwitchCoreAndMedia->show();
				btnSwitchMedia->show();
			}
			
		btn_SwitchRA->show();
	}
	else
	// only media station
	if (!m_bCoreHere && m_bMediaHere)
	{
		btnSwitchCore->hide();
		btnSwitchCoreAndMedia->hide();
		btnSwitchMedia->show();
		
		btnSwitchMedia->setText(m_bMediaRunning?"Stop &Media Station":"Start &Media Station");
		btnSwitchMedia->setPaletteForegroundColor( (m_bMediaRunning)?QColor(255,0,0):QColor(0,170,0));
		if (m_bMediaRunning)
			btnActivateOrbiter->show();
		else
			btnActivateOrbiter->hide();
		
		btn_SwitchRA->hide();
	}
	else
	// nothing here
	if (!m_bCoreHere && !m_bMediaHere)
	{
		btnSwitchCore->hide();
		btnSwitchCoreAndMedia->hide();
		btnSwitchMedia->hide();
		btn_SwitchRA->hide();
	}
	
	// tuning a couple of other buttons
	if (m_bCoreRunning)
	{
		btnReloadRouter->show();
	}
	else
	{
		btnReloadRouter->hide();
	}
	
	// setting labels
	QString coreStatus = QString("") + (m_bCoreRunning?"running ":"not running ") + ( m_bCoreHere?"locally":("on " + leCoreIP->text()) );
	tlCoreStatus->setText(coreStatus);	

	QString mediaStatus = ((!m_bMediaHere)?"not installed": (m_bMediaRunning?"running":"not running") );
	tlMediaStatus->setText(mediaStatus);	
	
	btn_SwitchRA->setText(m_bRemoteAssistanceRunning?"Disable Remote Assistance":"Enable Remote Assistance");
	
	if ( m_bCoreHere )
	{
		if (m_bRemoteAssistanceRunning)
			tlRAStatus->setText("enabled, service code is " + m_qsRAInfo);
		else
			tlRAStatus->setText("disabled");
	}
	else
		tlRAStatus->setText("not available on media station");
}

void lmce_launch_managerWidget::btnSwitchCore_clicked()
{
	enableButtons(false);
	
	if (m_bCoreRunning)
	{
		writeLog("Stopping Core", true);
		
		// marking device as going down
		reportDeviceDown();
		
		stopCore();
	}
	else
	{
		m_bDelayUpReport = false;
		
		writeLog("Starting Core",true);
		startCore();
		
		if (m_bDelayUpReport)
		{
			QTimer::singleShot( 30000, this, SLOT(reportDeviceUp()) );
		}
		else
			reportDeviceUp();
	}
	
	enableButtons(true);
	
	tuneConnectionsWidgets();
}


void lmce_launch_managerWidget::btnSwitchCoreAndMedia_clicked()
{
#ifdef START_STOP_DEBUG
	if (m_pStartStopDebugTimer)
	{
		m_pStartStopDebugTimer->stop();
	}
#endif

	enableButtons(false);
	
	if (m_bMediaRunning&&m_bCoreRunning)
	{
		writeLog("Stopping Media Station and Core", true);
		
		// marking device as going down
		reportDeviceDown();
	
		stopMediaStation();
		stopCore();
	}
	else
	{
		m_bDelayUpReport = false;
		bool bAbort = false;
		
		if (m_bMediaHere)
		{
			// checking X settings and exit if user asked
			if (!checkXsettings())
			{
				QApplication::exit(1);
				return;
			}
			
			// check if we need to update screen dims and run the orbitergen
			bAbort = !checkScreenDimensions();
		}
	
		if (!bAbort)
		{
			writeLog("Starting Core and  Media Station", true);
			if (startCore())
			{
				m_bMediaRunning = startMediaStation();
			}
			
			if (m_bDelayUpReport)
			{
				QTimer::singleShot( 30000, this, SLOT(reportDeviceUp()) );
			}
			else
				reportDeviceUp();
		}
		else
		{
			writeLog("Startup of Core and  Media Station cancelled", true);
		}
	}
	
	enableButtons(true);
	
	tuneConnectionsWidgets();

#ifdef START_STOP_DEBUG
	if (m_pStartStopDebugTimer)
	{
		m_pStartStopDebugTimer->start(30000);
	}
#endif
}


void lmce_launch_managerWidget::btnSwitchMedia_clicked()
{
	enableButtons(false);
	
	if (m_bMediaRunning)
	{
		writeLog("Stopping Media Station", true);
		
		// reporting device is going down if we are M/D
		reportDeviceDown();
		
		stopMediaStation();
	}
	else
	{
		// checking X settings and exit if user asked
		if (!checkXsettings())
			qApp->exit(1);
		
		// check if we need to update screen dims and run the orbitergen
		if ( !checkScreenDimensions() )
		{
			writeLog("Startup of Media Station cancelled", true);
		}
		else
		{
			m_bDelayUpReport = false;
			
			writeLog("Starting Media Station", true);
			m_bMediaRunning = startMediaStation();
			
			// reporting device is up if we are M/D
			if (m_bMediaRunning)
				if (m_bDelayUpReport)
				{
					QTimer::singleShot( 30000, this, SLOT(reportDeviceUp()) );
				}
				else
					reportDeviceUp();
		}
	}
	
	enableButtons(true);
	
	tuneConnectionsWidgets();
}


void lmce_launch_managerWidget::btnRegenThisOrbiter_clicked()
{
	// if no orbiter here
	if (m_qsOrbiterID=="" || !checkIfOrbiterIsOK())
		return;
	
	if ( !triggerOrbiterRegen() )
	{
		writeLog("Failed to start generation of skin for current Orbiter #"+m_qsOrbiterID, true, LV_WARNING);
		return;
	}
	else
	{
		writeLog("Regenerating skin for current Orbiter #"+m_qsOrbiterID, true);
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		sleep(3);
	}
	
	startOrbiterRegenProgressTracking();
	
	while (m_bRegenInProgress)
	{
		sleep(1);
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
	}
	
	QString qsOrbiterStatus=getOrbiterStatus();
	
	if (qsOrbiterStatus=="O")
		writeLog("Regenerated skin successfully", true);
	else
	{
		writeLog("Skin generation completed, server says: " + getOrbiterStatusMessage(qsOrbiterStatus), true, LV_WARNING);
	}
	
	QApplication::restoreOverrideCursor();
}


void lmce_launch_managerWidget::btnReloadRouter_clicked()
{
	if (m_pLaunch_Manager)
	{
		writeLog("Reloading router", true);
		DCE::Message *pMessage = new DCE::Message(0, DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_RELOAD, 0);
		m_pLaunch_Manager->QueueMessageToRouter(pMessage);
	}
	else
	{
		writeLog("Failed to reload router -  no connection", true);
		return;
	}
	
	// updating serial ports
	updateSerialPorts();
}


void lmce_launch_managerWidget::btnRegenAllOrbiters_clicked()
{
	if ( !triggerOrbiterRegen(true) )
	{
		writeLog("Failed to start generation of skins for all Orbiters", true, LV_WARNING);
		return;
	}
	else
	{
		writeLog("Regenerating skins for all Orbiters", true);
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		sleep(3);
	}
	
	startOrbiterRegenProgressTracking(false);
	
	while (m_bRegenInProgress)
	{
		sleep(1);
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
	}
	
	writeLog("Generation of skins finished", true);
	QApplication::restoreOverrideCursor();
}

bool lmce_launch_managerWidget::startCore()
{
        updateSerialPorts();
  
	if ( startCoreServices() )
	{	
		const int iMaxRetries = 10;
		int iRetries = 0;
		bool bLMinit = false;
		
		while( (iRetries++<iMaxRetries) && !bLMinit)
		{
			writeLog("Trying to connect to DCERouter, attempt " + QString::number(iRetries) + " of " + QString::number(iMaxRetries));
			bLMinit = initialize_LMdevice();
		}
		
		if (!bLMinit)
		{
			writeLog(QString("Failed to connect to DCERouter!"), true, LV_WARNING);
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

void lmce_launch_managerWidget::stopCore()
{
	// tell router quit
	// /usr/pluto/bin/MessageSend dcerouter 0 -1002 1 1
	// message to router=-1000, syscommand=7, SYSCOMMAND_QUIT=0
	
	writeLog(QString("") + "Called /usr/pluto/bin/MessageSend dcerouter 0 -1000 7 0");
	
	QProcess process(QString("/usr/pluto/bin/MessageSend"));
	process.addArgument("dcerouter");
	process.addArgument("0");
	process.addArgument("-1000");
	process.addArgument("7");
	process.addArgument("0");
	process.start();
	
	stopCoreDevices();
	
	stopCoreServices();
	
	m_bCoreRunning = false;
	
	// just a cleanup - it should exit already
	deinitialize_LMdevice();
}


bool lmce_launch_managerWidget::startMediaStation()
{
	updateSerialPorts();

	// retrying forever, as KeepAlive timer is not running yet
	bool bLMinit = initialize_LMdevice(true);
	if (!bLMinit)
	{
		writeLog(QString("Failed to connect to DCERouter!"), true, LV_WARNING);
		return false;
	}	
	
	writeLog("Checking if Orbiter skin is OK");
	if ( !confirmOrbiterSkinIsReady() )
	{
		writeLog(QString("Failed to regenerate skin for Orbiter!!"), true, LV_WARNING);
		return false;
	}
	else
        {
                QString status = getOrbiterStatus();
                bool bRouterReloaded = false;
                // repeat while status is "N"
                while (status != "O")
                {
                    // firing reload event
                    if (!bRouterReloaded)
                        btnReloadRouter_clicked();
                    
                    writeLog("Trying to reload router to use new skin for this Orbiter, please wait (giving router 10 seconds to reload)...", true);
                    qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
                    
                    // waiting 10 seconds
                    int iCount=10;
                    while (iCount--)
                    {
                        sleep(1);
                        qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
                    }
                    
                    // handling disconnection from router, the media station is not running,
                    // so it may not auto-reconnect
                    if (!m_pLaunch_Manager)
                    {
                        // assuming we were disconnected because of router reload
                        bRouterReloaded = true;
                        initialize_LMdevice();
                    }

                    status = getOrbiterStatus();
                    if (status != "O" && status != "N" && status !="!")
                    {
                        writeLog(QString("Received unknown status for this Orbiter, aborting Media Station start!"), true, LV_WARNING);
                        writeLog(QString("Status was: ")+status, true, LV_WARNING);
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

bool lmce_launch_managerWidget::stopMediaStation()
{
	//TODO add code
	stopMediaDevices();
	
	m_bMediaRunning = false;
	
	return true;
}

bool lmce_launch_managerWidget::performQueryDB(QString query, QSqlQuery &queryResult)
{
	writeLog("Running query: \"" + query + "\"", false, LV_DEBUG);

	if ( pPlutoDatabase ) {
		if (!pPlutoDatabase->isOpen())
		{
			writeLog("Attempted to exec query via closed DB connection, trying to reopen it", false, LV_WARNING);
			closeDB();
			if (!openDB())
			{
				writeLog("Reopening failed, giving up", false, LV_CRITICAL);
				return false;
			}
		}
		
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
		printSQLerror(err);
		return false;
	}
	else
	{
		writeLog("Attempted to query not initialized DB", false, LV_CRITICAL);
		return false;
	}
}

void lmce_launch_managerWidget::printSQLerror(QSqlError &err)
{
	switch ( err.type() )
	{
		case QSqlError::None:
			writeLog("No error occured", false, LV_STATUS);
			break;
		case QSqlError::Connection:
			writeLog("Connection error", false, LV_CRITICAL);
			break;
		case QSqlError::Statement:
			writeLog("Statement error", false, LV_CRITICAL);
			break;
		case QSqlError::Transaction:
			writeLog("Transaction error", false, LV_CRITICAL);
			break;
		case QSqlError::Unknown:
			writeLog("Unknown error", false, LV_CRITICAL);
			break;
		default:
			writeLog("Unknown error type", false, LV_CRITICAL);
	}
	
	if (err.type() != QSqlError::None)
		writeLog("Database says: " + err.databaseText() + "\t Driver says: " + err.driverText(), false, LV_CRITICAL);
}

QString lmce_launch_managerWidget::queryDB(QString query)
{
	QString sResult;
	QSqlQuery queryResult;
	if ( performQueryDB(query, queryResult) )
	{
		if (queryResult.next())
			sResult = queryResult.value(0).toString();
		else
			writeLog("Query: \"" + query +"\" returned no records", false, LV_DEBUG);
	}
	else
	{
		writeLog("Query: \"" + query +"\" failed", false, LV_DEBUG);
	}
	
	return sResult;
}


QStringList lmce_launch_managerWidget::queryDB_array(QString query)
{
	QStringList sResult;
	QSqlQuery queryResult;
	if ( performQueryDB(query, queryResult) )
	{
		while(queryResult.next())
		{
			sResult.append( queryResult.value(0).toString() );
		}
		
		if ( !sResult.count() )
			writeLog("Query: \"" + query +"\" returned no records", false, LV_DEBUG);

	}
	else
	{
		writeLog("Query: \"" + query +"\" failed", false, LV_DEBUG);
	}
	
	return sResult;
}

void lmce_launch_managerWidget::btnApplyAudioSettings_clicked()
{
	//TODO process saving errors
	writeLog("Saved audio settings");
	
	if (m_qsMediaID == "")
		return;
	
	QString aSettings;
	char cType[] = {'M', 'C', 'O', 'S', 'L'};
	uint iSelected = cmbAudioConnector->currentItem();
	if (iSelected<sizeof(cType))
		aSettings += cType[iSelected];
	
	if (cbAC3Pass->isChecked())
		aSettings += "3";
	

	queryDB( QString("UPDATE Device_DeviceData SET IK_DeviceData=\"" + aSettings + "\" WHERE FK_Device=" + m_qsMediaID + " AND FK_DeviceData=" +
		QString::number(DEVICEDATA_Audio_settings_CONST) ));
	
}

bool lmce_launch_managerWidget::openDB()
{
	if (pPlutoDatabase)
	{
		writeLog("Attempted to reopen already opened DB, closing current connection first", false, LV_WARNING);
		closeDB();
	}
	
	writeLog("Opening DB connection..", true);
	
	pPlutoDatabase = QSqlDatabase::addDatabase( "QMYSQL3" );
	pPlutoDatabase->setDatabaseName( "pluto_main" );
	pPlutoDatabase->setUserName( leDBUser->text() );
	pPlutoDatabase->setPassword( leDBPass->text() );
	pPlutoDatabase->setHostName( leDBIP->text() );
	pPlutoDatabase->setPort(m_qsMySQLPort.toInt());
	
	if (pPlutoDatabase->open())
	{
		writeLog("Successful", true);
		return true;
	}
	else
	{
		writeLog("Error: " + pPlutoDatabase->lastError().text(), true, LV_WARNING);
		QSqlDatabase::removeDatabase(pPlutoDatabase);
		pPlutoDatabase = NULL;
		return false;
	}
}

bool lmce_launch_managerWidget::closeDB()
{
	writeLog("Closing DB connection..");
	
	if (pPlutoDatabase)
	{
		pPlutoDatabase->close();
		writeLog("Successful", true);
		QSqlDatabase::removeDatabase(pPlutoDatabase);
		pPlutoDatabase = NULL;
		return true;
	}
	else
	{
		writeLog("No need: it wasn't open");
		return false;
	}
}

void lmce_launch_managerWidget::startMediaDevices(bool checkForAlreadyRunning)
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

void lmce_launch_managerWidget::stopMediaDevices()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	while (!m_qvvMediaDevices.empty())
	{
		int PK_Device = m_qvvMediaDevices.back();
		m_qvvMediaDevices.pop_back();
		
		if (m_pLaunch_Manager)
		{
			writeLog("Stopping media device " + QString::number(PK_Device), true);
			DCE::Message *pMessage = new DCE::Message(0, PK_Device, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_QUIT, 0);
			m_pLaunch_Manager->QueueMessageToRouter(pMessage);
		}
		else
		{
			writeLog("Failed to stop media device  -  no connection", true);
			QApplication::restoreOverrideCursor();
			return;
		}
		
		waitForDeviceShutdown(PK_Device);

/*		
		// tell device quit
		// /usr/pluto/bin/MessageSend dcerouter 0 deviceID 7 0
		// message to device, syscommand=7, SYSCOMMAND_QUIT=0
	
		writeLog(QString("Stopping device ") + QString::number(PK_Device) + " : /usr/pluto/bin/MessageSend dcerouter 0 " + QString::number(PK_Device) + " 7 0");
	
		QProcess process(QString("/usr/pluto/bin/MessageSend"));
		process.addArgument("dcerouter");
		process.addArgument("0");
		process.addArgument(QString::number(PK_Device));
		process.addArgument("7");
		process.addArgument("0");
		process.start();
		
*/
	}
	
	// waiting for devices to kill their children
	writeLog("Giving devices time to shutdown their children");
	sleep(5);
	
	QApplication::restoreOverrideCursor();
}

void lmce_launch_managerWidget::startCoreDevices(bool checkForAlreadyRunning)
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

void lmce_launch_managerWidget::stopCoreDevices()
{
}

bool lmce_launch_managerWidget::startCoreServices()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QProcess *pService = new QProcess(QString(START_CORE_SCRIPT));
	
	writeLog("Starting process " + QString(START_CORE_SCRIPT), true, LV_WARNING);
	writeLog("Please be patient. This may take a while...", true);
	
	QString log = LM_PROGRESS_LOG;
	QString cmd = "echo -n > " + log;
	system(cmd);
	
	QFile flog(log);
	QTextStream ts;

	if (flog.open(IO_ReadOnly))
	{
		ts.setDevice(&flog);
	}
	else
		writeLog("Failed to open file" + QString(":") + VIDEO_DRIVER_NAME_FILE, false, LV_WARNING);

	
	if (pService->start() )
	{
		bool bOkDB = pPlutoDatabase;
		
		pbProgress->setProgress(0);
		pbProgress->show();
		
		tlStatusMessages->setText("");
		tlStatusMessages->show();
		
		QString currOrbiterID="";
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);

		int iCounter = CORE_SERVICE_TIMEOUT;
		int iQueryCounter=5; // trigger on start
		while (pService->isRunning() && iCounter--)
		{
			// every second checking config log messages
			QString s, s1;
			while ( (s=ts.readLine()) != QString::null)
			{
				s1 = s;
			}
			
			if (s1!="")
			{
				tlStatusMessages->setText(s1);
				writeLog("Progress message: " + s1, false);
			}
			
			// every 5 seconds checking orbiters
			if (iQueryCounter>=5)
			{
				iQueryCounter = 0;
				
				// retrieve orbiter progress
				if (bOkDB)
				{
					QString sQuery = "select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1";
					QSqlQuery q;
					bool bQueryExecResult = performQueryDB(sQuery, q);

					QString O_ID;
					QString O_Message;
					uint O_Progress = 100;
					
					// some orbiter is being regen-ed
					if ( bQueryExecResult && q.next() )
					{
						O_ID = q.value(0).toString();
						O_Progress = q.value(1).toInt();
						O_Message = "Orbiter #"+O_ID+"\n"+q.value(2).toString();
						
						if (O_ID != currOrbiterID)
						{
							currOrbiterID = O_ID;
							writeLog("Generating skin for Orbiter #" + currOrbiterID + "...", true);
						}
						
						pbProgress->setProgress(O_Progress);
						tlStatusMessages->setText(O_Message);
					}
				}
			}
			
			qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
			sleep(1);
			iQueryCounter++;
			
		}
		
		ts.unsetDevice();
		flog.close();
		
		tlStatusMessages->hide();
		pbProgress->hide();
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
		
		if (pService->isRunning())
		{
			writeLog("Process didn't exit after " + QString::number(CORE_SERVICE_TIMEOUT) + " secs of running, let it run in background");
							// m_qpvCoreServices.push_back(pService);
			QApplication::restoreOverrideCursor();
			return true;
		}
		else
		{
			writeLog("Process completed with status: " +QString::number( pService->exitStatus()));
			delete pService;
			QApplication::restoreOverrideCursor();
			return true;
		}
	}
	else
	{
		writeLog("Failed to start process " + QString(START_CORE_SCRIPT), true, LV_WARNING);
		QApplication::restoreOverrideCursor();
		return false;
	}
}

bool lmce_launch_managerWidget::stopCoreServices()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QProcess *pService = new QProcess(QString(STOP_CORE_SCRIPT));
	
	writeLog("Starting process " + QString(STOP_CORE_SCRIPT), true, LV_WARNING);
	if (pService->start())
	{
		int iCounter = CORE_SERVICE_TIMEOUT;
		while (pService->isRunning() && iCounter--)
		{
			sleep(1);
			qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
		}
						
		if (pService->isRunning())
		{
			writeLog("Process didn't exit after " + QString::number(CORE_SERVICE_TIMEOUT) + " secs of running, let it run in background");
							// m_qpvCoreServices.push_back(pService);
		}
		else
		{
			writeLog("Process completed with status: " + QString::number(pService->exitStatus()));
			delete pService;
		}
	}
	else
		writeLog("Failed to start process " + QString(STOP_CORE_SCRIPT), true, LV_WARNING);
	
	QApplication::restoreOverrideCursor();
	return true;
}

void lmce_launch_managerWidget::waitForDevice(int deviceID)
{
	char cStatus = 'N';
	int iCount = MEDIA_DEVICE_TIMEOUT;
	
	if (!m_pLaunch_Manager)
		cStatus = 'E';
	
	while (cStatus=='N' && iCount--)
	{
		sleep(1);
		if (allowRedraw)
			qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
		cStatus = m_pLaunch_Manager->DeviceIsRegistered(deviceID);
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
			writeLog(QString("Unknown status '") + cStatus + "'when calling Command_Impl::DeviceIsRegistered", true);
			break;
	}
}

// TODO generalize to 'wait for device status'?
void lmce_launch_managerWidget::waitForDeviceShutdown(int deviceID)
{
	char cStatus = 'Y';
	int iCount = MEDIA_DEVICE_TIMEOUT;
	
	if (!m_pLaunch_Manager)
		cStatus = 'E';
	
	while (cStatus=='Y' && iCount--)
	{
		sleep(1);
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
		cStatus = m_pLaunch_Manager->DeviceIsRegistered(deviceID);
	}
	
	if (cStatus=='Y')
		writeLog("Timeout waiting for device shutdown", false, LV_WARNING);
}

void lmce_launch_managerWidget::jumpToOrbiter()
{
	if (m_pLaunch_Manager)
	{
		writeLog("Activating Orbiter...", true);
		DCE::CMD_Activate_PC_Desktop cmd(m_qsDeviceID.toInt(), m_qsOrbiterID.toInt(), 0);
		m_pLaunch_Manager->SendCommandNoResponse(cmd);
	}
	else
	{
		writeLog("Failed to activate Orbiter - no connection to DCE router", true, LV_WARNING);
	}
}

void lmce_launch_managerWidget::btnActivateOrbiter_clicked()
{
	jumpToOrbiter();
}

void lmce_launch_managerWidget::doRedraw()
{
	if (allowRedraw)
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
}

void lmce_launch_managerWidget::enableButtons(bool bEnable)
{
	btnSwitchCore->setEnabled(bEnable);
	btnSwitchCoreAndMedia->setEnabled(bEnable);
	btnSwitchMedia->setEnabled(bEnable);
	btnActivateOrbiter->setEnabled(bEnable);
	
	doRedraw();
}

void lmce_launch_managerWidget::loadSettings()
{
}

void lmce_launch_managerWidget::saveSettings()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	pConfig->AddInteger("AutostartCore", (cbAutostartCore->isChecked()?1:0));
	pConfig->AddInteger("AutostartMedia", (cbAutostartMedia->isChecked()?1:0));
	pConfig->WriteSettings();
	delete pConfig;
}

void lmce_launch_managerWidget::doAutostart()
{
	writeLog(">>Performing autostart if configured..", true);
	qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);

	m_bDelayUpReport = false;
	
	bool bReport = false;
	
	// checking for core and starting it
	if (m_bCoreHere && (cbAutostartCore->isChecked() || false))
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
	
	if ( ( (m_bCoreHere &&m_bCoreRunning) || !m_bCoreHere) && m_bMediaHere && (cbAutostartMedia->isChecked() || false) )
	{
		if (!m_bMediaRunning)
		{
			checkScreenDimensions(false);
			
			writeLog(">>Autostarting media station....", true);
			m_bMediaRunning = startMediaStation();
			bReport |= m_bMediaRunning;
		}
		else
			writeLog(">>Not autostarting media station - it is already running", true);
	}
	
	if (bReport)
		if (m_bDelayUpReport)
		{
			QTimer::singleShot( 30000, this, SLOT(reportDeviceUp()) );
		}
		else
			reportDeviceUp();
	
	tuneConnectionsWidgets();
}

void lmce_launch_managerWidget::saveBootupSettings_clicked()
{
	saveSettings();
}

void lmce_launch_managerWidget::startOrbiterRegenProgressTracking(bool currentOrbiterOnly/*=true*/)
{
	m_bRegenInProgress = true;
	m_bRegenTrackingCurrentOrbiterOnly = currentOrbiterOnly;

	if (!m_pProgressBarUpdateTimer)
	{
		m_pProgressBarUpdateTimer = new QTimer();
		connect(m_pProgressBarUpdateTimer, SIGNAL(timeout()), this, SLOT(updateOrbiterRegenProgress()));
	}
	
	m_pProgressBarUpdateTimer->start(2000);
}

bool lmce_launch_managerWidget::checkIfOrbiterIsOK()
{
	if (!m_bMediaHere || m_qsOrbiterID=="")
	{
		writeLog("Not getting Orbiter status - there is no Orbiter at this PC", true);
		return false;
	}
	
	return getOrbiterStatus() == "O";
}

void lmce_launch_managerWidget::updateOrbiterRegenProgress()
{
	// if we are tracking current Orbiter only, we are asking Orbiter plugin for status, otherwise we are doing DB calls
	if (m_bRegenTrackingCurrentOrbiterOnly)
	{
		if (!m_bMediaHere || m_qsOrbiterID=="")
		{
			writeLog("Not waiting for OrbiterGen - there is no Orbiter at this PC", true);
			pbProgress->hide();
			return;
		}
		
		if (!m_pLaunch_Manager)
		{
			writeLog("Cannot get OrbiterGen status - connection problems", true, LV_WARNING);
			pbProgress->hide();
			return;
		}
	
		int iRegenProgress;
		QString qsOrbiterStatus = getOrbiterStatus(iRegenProgress);
		
		if (qsOrbiterStatus=="R" || qsOrbiterStatus=="r")
		{
			pbProgress->show();
			pbProgress->setProgress(iRegenProgress);
			
			QString sQuery = "select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1 AND PK_Orbiter="+m_qsOrbiterID;
			QSqlQuery q;
			bool bQueryExecResult = performQueryDB(sQuery, q);

			if ( bQueryExecResult && q.next() )
			{
				QString statusMessage = "Orbiter #" + q.value(0).asString() + "\n"+q.value(2).asString();
				tlStatusMessages->setText(statusMessage);
				tlStatusMessages->show();
			}
			
			writeLog("Waiting for OrbiterGen to complete", false);
		}
		else
		{
			pbProgress->hide();
			m_pProgressBarUpdateTimer->stop();
			
			tlStatusMessages->hide();
			tlStatusMessages->setText("");

			writeLog("OrbiterGen run completed", true);
			
			m_bRegenInProgress = false;
		}
	}
	else		// directly quering DB for 'average progress' for all orbiters
	{
		QString regenProgress = queryDB("SELECT RegenPercent FROM Orbiter WHERE RegenInProgress=1");
		QString activeRegens = queryDB("SELECT COUNT(*) FROM Orbiter WHERE Regen=1 OR Regen=2");
		
		if (activeRegens.toInt()>0)
		{
			pbProgress->show();
			if (regenProgress != QString::null)
				pbProgress->setProgress(regenProgress.toInt());
			
			QString sQuery = "select PK_Orbiter, RegenPercent,RegenStatus from Orbiter where RegenInProgress=1";
			QSqlQuery q;
			bool bQueryExecResult = performQueryDB(sQuery, q);

			if ( bQueryExecResult && q.next() )
			{
				QString statusMessage = "Orbiter #" + q.value(0).asString() + "\n"+q.value(2).asString();
				tlStatusMessages->setText(statusMessage);
				tlStatusMessages->show();
			}
			
			writeLog("Waiting for OrbiterGen to complete", false);
		}
		else
		{
			pbProgress->hide();
			m_pProgressBarUpdateTimer->stop();
			tlStatusMessages->setText("");
			tlStatusMessages->hide();
			
			writeLog("OrbiterGen run completed", true);
			
			m_bRegenInProgress = false;
		}
	}
}

void lmce_launch_managerWidget::updateDevicesProgressBar()
{
	if (m_iDevicesLevel==-1)
	{
		m_pDevicesUpdateTimer->stop();
		pbProgress->hide();
	}
	else
	{
		pbProgress->show();
		pbProgress->setProgress(m_iDevicesLevel);
	}
}

// TODO refactor
void lmce_launch_managerWidget::updateSerialPorts()
{
	writeLog(QString("Running UpdateAvailableSerialPorts.sh"), true, LV_WARNING);
					//continue;

	QString qsCommand = "/usr/pluto/bin/UpdateAvailableSerialPorts.sh";
	QString qsBackground = "0";
	QString qsParameter = "";
	QString qsDescription = "Updating available serial ports";
	
	QFileInfo qfi(qsCommand);
	QString baseName = qfi.baseName(true);
	QString qsWorkDir = QString(SCREENLOGS_BASE) + "/" + baseName;
	QDir qd(qsWorkDir);
	if (!qd.exists(qsWorkDir, true))
	{
		qd.mkdir(qsWorkDir, true);
	}
	
	// copied from start foreground service
	QStringList args;
					
	args.push_back(qsCommand);
	if (qsParameter!="")
		args.push_back(qsParameter);
					
	QProcess *pService = new QProcess(args);
	pService->setWorkingDirectory(qsWorkDir);
	writeLog("Starting core service " + qsCommand + ": " + args.join(" "));
	pService->start();
	int iCounter = 30;
	while (pService->isRunning() && iCounter)
	{
		sleep(1);
		iCounter--;
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
	}
					
	if (pService->isRunning())
	{
		writeLog("Process didn't exit after 30 secs of running, let it run in background", true, LV_WARNING);
		m_qpvCoreServices.push_back(pService);
	}
	else
	{
		writeLog("Process completed with status: " + QString::number(pService->exitStatus()));
		delete pService;
	}
					
}

void lmce_launch_managerWidget::respawnNewChildren()
{
	writeLog("Requested respawning of new children devices", true, LV_WARNING);
	
	allowRedraw = false;
	
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
	
	allowRedraw = true;
}

/*
	returns true if user selected update/not update orbiter configuration and continue
	returns false if user select to abort startup
*/
bool lmce_launch_managerWidget::checkScreenDimensions(bool askUser/*=true*/)
{
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
	
	return true;
}

void lmce_launch_managerWidget::LMdeviceKeepAlive()
{
	if (m_pLaunch_Manager)
	{
		// if device is finishing
		if (m_pLaunch_Manager->m_bTerminate)
		{
			writeLog("Connection to DCERouter terminated", true);
			bool bReload = m_pLaunch_Manager->m_bReload;
			deinitialize_LMdevice();
			if (bReload)
			{
				writeLog("Reconnecting to DCERouter as requested", true);
				if (!initialize_LMdevice())
				{
					writeLog("Will retry later..", true);
					QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
				}
				else
				{
                                        updateSerialPorts();
					respawnNewChildren();
					reportDeviceUp();
				}
			}
			else
			{
				writeLog("Reconnect to DCERouter was not requested", true);
				m_bCoreRunning = false;
				m_bMediaRunning = false;
				tuneConnectionsWidgets();
			}
		}
		else
			QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
	}
	else
		if (m_bCoreRunning || m_bMediaRunning)
		{
			writeLog("Reconnecting to DCERouter as requested", true);
			if (!initialize_LMdevice())
			{
				writeLog("Will retry later..", true);
				QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
			}
			else
			{
                                updateSerialPorts();
				respawnNewChildren();
				reportDeviceUp();
			}
		}
                else
                {
                    writeLog("LMdeviceKeepAlive() not reconnecting, as this is not required");
                }
}

bool lmce_launch_managerWidget::checkXsettings()
{
	// check X settings
	writeLog("Starting script: " + QString(SCRIPT_X_CHECK_SETTINGS), false, LV_STATUS);
	
	QString log = "/var/log/pluto/LaunchManager.X-Check.log";
	QString cmd = QString(SCRIPT_X_CHECK_SETTINGS) + " > " + log;
	
	int iRet = system(cmd);
	
	switch(WEXITSTATUS(iRet))
	{
		// all is ok
		case 0:
			writeLog("Xorg settings are OK", true, LV_STATUS);
			return true;
			
		// some error
		case 1:
		// some warning
		case 2:
		{
			QFile fout(log);
			fout.open(IO_ReadOnly);
			QTextStream ts(&fout);
			
			QStringList out;
			while (!ts.atEnd())
			{
				QString s = ts.readLine();
				if (s != QString::null)
					out.append(s);
			}
			ts.unsetDevice();
			fout.close();
			
			writeLog("Script returned " + QString::number(iRet) + " and said: \n" + out.join("\n"), false, LV_STATUS);
			writeLog("Checking X settings - some problems found...");
			
			QMessageBox mb(QString("Some problems found..."), out.join("\n") +"\nDo you want to run helper script to try to fix it ( this can cause system to reboot)?\nPress Cancel if you want to close LMCE Launch Manager without running helper script.", QMessageBox::NoIcon, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
			
			int iAnswer=mb.exec();
			
			if (iAnswer==QMessageBox::Yes)
			{
				int iResult = QMessageBox::information(this, "Important",qsVideoRestoreInfo, QMessageBox::Ok, QMessageBox::Cancel);
		
				if (iResult == QMessageBox::Cancel)
				{
					writeLog("RestoreInfo: User decided not to continue");
					iAnswer = QMessageBox::Cancel;
				}
				else
					writeLog("RestoreInfo: User decided to continue");
			}
			
			switch (iAnswer)
			{
				// apply settings and ask user to restart X
				case QMessageBox::Yes:
				{
					// correct X settings
					writeLog("Starting script: " + QString(SCRIPT_SETUP_AV_SETTINGS), false, LV_STATUS);
					flushLog();
					
					QString cmd = QString(SCRIPT_SETUP_AV_SETTINGS);
	
					int iRet = system(cmd);
					
					return true;
				}
				break;
				
				// don't apply and continue
				case QMessageBox::No:
					writeLog("User decided don't apply and continue", false, LV_STATUS);
					return true;
				
				// don't apply and break
				case QMessageBox::Cancel:
					writeLog("User decided don't apply and break", false, LV_STATUS);
					return false;
				
				default:
					writeLog("Checking Xorg settings failed: user selected " + QString::number(iAnswer), true, LV_WARNING);
					return true;
			}
		}
		break;
		
		default:
			writeLog("Checking Xorg settings failed: script returned " + QString::number(WEXITSTATUS(iRet)), true, LV_WARNING);
			return true;
	}
	
	return true;
}

void lmce_launch_managerWidget::btn_StartUIDiag_clicked()
{
	QProcess p(QString(APP_UI_DIAG));
	p.setWorkingDirectory(QString(APP_UI_DIAG_WORKDIR));
	
	writeLog("Starting UI diagnostics tool", true);
	
	QString lib_path(APP_UI_DIAG_LIBS);
	if (lib_path=="")
	{
		p.start();
	}
	else
	{
		lib_path = "LD_LIBRARY_PATH=" + lib_path;
		
		char *old = getenv("LD_LIBRARY_PATH");
		if (old)
			lib_path += QString(":") + old;
		
		QStringList *qsl = new QStringList();
		
		for (int i=0; environ[i]; i++)
		{
			QString line = environ[i];
			if (!line.startsWith("LD_LIBRARY_PATH="))
				qsl->append(line);
		}
		
		qsl->append(lib_path);
		
		p.start(qsl);
	}
}

void lmce_launch_managerWidget::updateOrbiterSize(int width, int height)
{
	writeLog("Updating Orbiter size to " + QString::number(width) + "x" + QString::number(height));
	
	if (m_qsOrbiterID!="")
	{
		QString query = "SELECT PK_Size FROM Size WHERE Width=" + QString::number(width) + " AND Height=" + QString::number(height);
		writeLog("Query: " + query, false, LV_DEBUG);
		QString sizeID = queryDB(query);
		if (sizeID=="")
		{
			writeLog("No direct match, trying to find Size that will fit into screen and have maximal square");
			query = "SELECT PK_Size, Width*Height AS Square FROM Size WHERE Width<=" + QString::number(width) + " AND Height<=" + QString::number(height) + " ORDER BY Square DESC";
			writeLog("Query: " + query, false, LV_DEBUG);
			sizeID = queryDB(query);
		}
		
		if (sizeID=="")
		{
			writeLog("No luck, aborting :(");
		}
		else
		{
			writeLog("Setting PK_Size for Orbiter #"+m_qsOrbiterID + " to " + sizeID);
			QString query = "UPDATE Device_DeviceData SET IK_DeviceData='"+sizeID+"' WHERE FK_Device="+m_qsOrbiterID +
					 " AND FK_DeviceData="+QString::number(DEVICEDATA_PK_Size_CONST);
			writeLog("Query: " + query, false, LV_DEBUG);
			queryDB(query);
		}
	}
	else
		writeLog("Aborting, orbiterID is empty");
}

bool lmce_launch_managerWidget::checkMedia()
{
	writeLog("Checking if Media Station is already running..", true);
	
	findRunningMediaDevices();
	
	char cStatus = m_pLaunch_Manager->DeviceIsRegistered(m_qsOrbiterID.toInt());
	if (cStatus == 'Y')
	{
		writeLog("Orbiter #" + m_qsOrbiterID + " is registered");
		return true;
	}
	else
	{
		writeLog("Orbiter #" + m_qsOrbiterID + " is not registered");
		return false;
	}
}

bool  lmce_launch_managerWidget::event(QEvent *e)
{
	// TODO remove duplication
	const int eventSpawnNewChildren = QEvent::User + 100;
	
	if (e->type() == eventSpawnNewChildren)
	{
		writeLog("Received event: request to spawn new devices");
		respawnNewChildren();
		writeLog("Respawn new devices completed");
		return true;
	}
	else
		return lmce_launch_managerWidgetBase::event(e);
}

void lmce_launch_managerWidget::findRunningMediaDevices()
{
	if (m_qsMediaID=="")
		return;

	writeLog("Searching for running media devices...");
	
	if ( pPlutoDatabase ) {
		QString runningDevices;
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
				if (q.value(3).toString()!="1")
				{
					// checking device if it implements DCE
					if (q.value(4).toString()=="1")
					{
						writeLog("Checking device " +  q.value(0).toString() + " "  + q.value(1).toString(), false);
						char cStatus = m_pLaunch_Manager->DeviceIsRegistered(q.value(0).toInt());
						if (cStatus == 'Y')
						{
							m_qvvMediaDevices.append(q.value(0).toInt());
							runningDevices += q.value(0).toString() + " ";
						}
					}
					else
					{
						writeLog("Not checking device " +  q.value(0).toString() + " "  + q.value(1).toString() + " - it doesn't implement DCE. Adding its children into check instead", false);
						// adding devices recursively
						QString child_query = QString("SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=") + *it;
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
		
		writeLog("Running media devices found: " + runningDevices, true);
	}
	else
	{
		writeLog("Failed to query MySQL DB");
	}
}

void lmce_launch_managerWidget::twAllTabs_currentChanged( QWidget * w)
{
	if (m_pLogsUpdateTimer)
	{
		writeLog("Current changed: " + twAllTabs->tabLabel(w) );
		if ( twAllTabs->tabLabel(w) == "&View Log" )
		{
			writeLog("Started logs list updater", false);
			m_pLogsUpdateTimer->start(LOG_REFRESH_INTERVAL, true);
		}
		else
		{
			writeLog("Stopped logs list updater", false);
			m_pLogsUpdateTimer->stop();
		}
	}
}

bool lmce_launch_managerWidget::confirmOrbiterSkinIsReady()
{
	if (m_qsOrbiterID=="")
		return false;
	
	// if we already have a record in the list for this Orbiter
	QString orbiterRecordID = queryDB("SELECT PK_Orbiter FROM Orbiter WHERE PK_Orbiter=" + m_qsOrbiterID);
	
	// if Orbiter needs a regen (flag set by AVWizard)
	QString orbiterRegenFlag = queryDB("SELECT Regen FROM Orbiter WHERE PK_Orbiter=" + m_qsOrbiterID);
	
	if (orbiterRecordID=="" || orbiterRegenFlag=="1")
	{
		if ( !triggerOrbiterRegen() )
		{
			writeLog("Failed to start generation of skin for current Orbiter #"+m_qsOrbiterID, true, LV_WARNING);
			return false;
		}
		else
		{
			if (orbiterRecordID=="")
				writeLog("First time generating skin for current Orbiter #"+m_qsOrbiterID, true);
			else
				writeLog("Regenerating skin for current Orbiter #"+m_qsOrbiterID, true);
			
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			sleep(3);
		}
	}
	
	startOrbiterRegenProgressTracking();
	
	while (m_bRegenInProgress)
	{
		sleep(1);
		qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
	}
	
	QString qsOrbiterStatus = getOrbiterStatus();
	
	if (qsOrbiterStatus=="O")
		writeLog("Generated skin successfully", true);
	else
	{
		writeLog("Skin generation completed, server says: " + getOrbiterStatusMessage(qsOrbiterStatus), true, LV_WARNING);
	}
	
	QApplication::restoreOverrideCursor();
	
	return (qsOrbiterStatus=="O")||(qsOrbiterStatus=="N");
}

/*	
 *	Triggers regen of current Orbiter 
 *	returns:
 *	true - if regen was triggered immeditely
 *	false - otherwise 
 */
bool lmce_launch_managerWidget::triggerOrbiterRegen(bool bAllOrbiters/*=false*/)
{
	// just for case OrbiterGen won't succeed first time
	queryDB( QString("UPDATE Orbiter SET Regen=1 WHERE" + (bAllOrbiters?" 1":(" PK_Orbiter=" + m_qsOrbiterID)) ) );
			
	if (m_pLaunch_Manager)
	{
		int orbiterPluginID=-1;
		map<int,string> mapDevices;
		m_pLaunch_Manager->GetDevicesByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST, &mapDevices);
		if (!mapDevices.empty())
		{
			orbiterPluginID = mapDevices.begin()->first;
			DCE::CMD_Regen_Orbiter cmd( m_qsDeviceID.toInt(),  orbiterPluginID, (bAllOrbiters?0:m_qsOrbiterID.toInt()),"", "" );
			m_pLaunch_Manager->SendCommandNoResponse(cmd);
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

void lmce_launch_managerWidget::reportDeviceUp()
{
	// saying we're up
	DCE::Message *pMessage = new DCE::Message(m_qsDeviceID.toInt(), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_UP, 0);
	if (m_pLaunch_Manager)
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_UP from LM device " + m_qsDeviceID, false);
		m_pLaunch_Manager->QueueMessageToRouter(pMessage);
	}
	else
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_UP from LM device " + m_qsDeviceID + " failed", false, LV_WARNING);
	}
}
		
void lmce_launch_managerWidget::reportDeviceDown()
{
	// saying we're down
	DCE::Message *pMessage = new DCE::Message(m_qsDeviceID.toInt(), DEVICEID_DCEROUTER, DCE::PRIORITY_NORMAL, DCE::MESSAGETYPE_SYSCOMMAND, DCE::SYSCOMMAND_DEVICE_DOWN, 0);
	if (m_pLaunch_Manager)
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_DOWN from LM device " + m_qsDeviceID, false);
		m_pLaunch_Manager->QueueMessageToRouter(pMessage);
	}
	else
	{
		writeLog("Sending  SYSCOMMAND_DEVICE_DOWN from LM device " + m_qsDeviceID + " failed", false, LV_WARNING);
	}
}

void lmce_launch_managerWidget::syncWithLockList(bool eraseDeadLocalDevices)
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
	
void lmce_launch_managerWidget::btn_SwitchRA_clicked()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	m_pRemoteAssistanceStatusRefreshTimer->stop();
	
	writeLog(QString("User clicked Switch RA button, current status is: ") + (m_bRemoteAssistanceRunning? "on" :"off"));
	
	QProcess p(QString(RA_SCRIPT));
	p.addArgument(QString(m_bRemoteAssistanceRunning?RA_OFF:RA_ON));
	p.start();
	while ( p.isRunning() )	
	{
		sleep(1);
	}
	
	if ( p.normalExit() )
	{
		if ( !m_bRemoteAssistanceRunning )
		{
			if ( checkRAStatus() )
			{
				writeLog(QString("Remote Assistance enabled"));
				QMessageBox::information(this, "Success", "Remote Assistance enabled,\nplease give this ID to Pluto support: " + getRAid(), QMessageBox::Ok);
			}
			else
			{
				writeLog(QString("Failed to enable network assistance, script didn't succeed"), false, LV_WARNING);
				QMessageBox::critical(this, "Error", "Failed to enable network assistance, script didn't succeed", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
		}
		else
		{
			if ( checkRAStatus() )
			{
				writeLog(QString("Failed to disable network assistance, script didn't succeed"), false, LV_WARNING);
				QMessageBox::critical(this, "Error", "Failed to disable network assistance, script didn't succeed", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
			else
				writeLog(QString("Remote Assistance disabled"));
		}
	}
	else
	{
		writeLog(QString("Failed to start helper script"), false, LV_WARNING);
		QMessageBox::critical(this, "Error", "Failed to start helper script", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}

	m_bRemoteAssistanceRunning = checkRAStatus();
	
	writeLog(QString("Current RA status is: ") + (m_bRemoteAssistanceRunning? "on" :"off") );
	
	tuneConnectionsWidgets();
	
	m_pRemoteAssistanceStatusRefreshTimer->start(5000);
	
	QApplication::restoreOverrideCursor();
}


void lmce_launch_managerWidget::btn_ViewIP_clicked()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	string sInfo;
	if ( ProcessUtils::RunApplicationAndGetOutput(NETWORK_SETTINGS_SCRIPT, sInfo) )
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::information(this, "Network settings", sInfo, QMessageBox::Ok);
	}
	else
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(this, "Error", "Failed to fetch network settings", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
	
	
}

bool lmce_launch_managerWidget::checkRAStatus()
{
	return getRAid() != "";
}

QString lmce_launch_managerWidget::getRAid()
{
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	QString sPass = pConfig->m_mapParameters_Find("remote");
	QString sInst =  pConfig->m_mapParameters_Find("PK_Installation");
	delete pConfig;
	
	if (sPass == "")
		return m_qsRAInfo = "";
	else
		return m_qsRAInfo = sInst + "-" + sPass;
}

void lmce_launch_managerWidget::flushLog()
{
	DCE::LoggerWrapper::GetInstance()->Flush();
}

void lmce_launch_managerWidget::updateRAstatus()
{
	bool bRAstatus = checkRAStatus();
	
	if (bRAstatus != m_bRemoteAssistanceRunning)
	{
		m_bRemoteAssistanceRunning = bRAstatus;
		tuneConnectionsWidgets();
	}
}

QString lmce_launch_managerWidget::getOrbiterStatus()
{
	int iNull;
	return getOrbiterStatus(iNull);
}

QString lmce_launch_managerWidget::getOrbiterStatus(int &iProgressValue)
{
	return getOrbiterStatus(m_qsOrbiterID, iProgressValue);
}

QString lmce_launch_managerWidget::getOrbiterStatus(const QString &orbiterID, int &iProgressValue)
{
	QString orbiterStatus="!";
	iProgressValue=0;

	// ! - is our custom error "while fetching info"
	if (orbiterID!="")
	{
		writeLog(QString("Getting status for Orbiter #") + orbiterID, false, LV_STATUS);
		
		if (m_pLaunch_Manager)
		{		
			string sValue_To_Assign, sText;
			int iValue;
			DCE::CMD_Get_Orbiter_Status cmd(m_qsDeviceID.toInt(), m_qsOrbiterPluginID.toInt(), orbiterID.toInt(), &sValue_To_Assign, &sText, &iValue);
			if ( m_pLaunch_Manager->SendCommand(cmd) )
			{
				orbiterStatus = sValue_To_Assign;
				iProgressValue = iValue;
				writeLog("Got orbiter status: " + orbiterStatus, false, LV_STATUS);
			}
			else
				writeLog(QString("Getting status for Orbiter failed - failed to send command"), false, LV_WARNING);
		}
		else
			writeLog(QString("Getting status for Orbiter failed - no connection to core"), false, LV_WARNING);
	}
	else
	{
		writeLog(QString("Getting status for Orbiter failed - no # passed"), false, LV_WARNING);
	}
	
	return orbiterStatus;
}

QString lmce_launch_managerWidget::getOrbiterStatusMessage(const QString &orbiterStatus)
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

bool lmce_launch_managerWidget::canClose()
{
	writeLog(QString("Processing closeEvent() "), false, LV_WARNING);
	
	// checking if LMCE is running
	if ( (m_bCoreHere && m_bCoreRunning) || (m_bMediaHere && m_bMediaRunning) )
	{
		int iRes = QMessageBox::warning( this, "Stop LMCE", "This will stop LinuxMCE. Proceed anyway?", QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::NoButton);
		if ( iRes == QMessageBox::Ok )
		{
			if ( m_bMediaHere && m_bMediaRunning )
				btnSwitchMedia_clicked();
			
			if ( m_bCoreHere && m_bCoreRunning )
				btnSwitchCore_clicked();
			
			return true;
		}
		else
			return false;
	}
	else
		return true;
}

void lmce_launch_managerWidget::btnChange_Resolution_clicked()
{
	// scheduling AVWizard
	DCEConfig *pConfig = new DCEConfig(PLUTO_CONFIG);
	pConfig->AddString("AVWizardOverride", "1");
	pConfig->WriteSettings();	
	delete pConfig;
	
	writeLog("Scheduled AVWizard run");	
	
	int iResult = QMessageBox::question( this, "Reboot PC?", "AV Wizard will start on the next reboot. Do you want to reboot now?", QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
	
	if (iResult == QMessageBox::Yes)
	{
		writeLog("Stopping LMCE");	
		
		if ( m_bMediaHere && m_bMediaRunning )
			btnSwitchMedia_clicked();
			
		if ( m_bCoreHere && m_bCoreRunning )
			btnSwitchCore_clicked();
		
		rebootPC();
	}
}

void lmce_launch_managerWidget::rebootPC()
{
	writeLog("Rebooting");	
	flushLog();
	
	system("/sbin/reboot");
	// giving system time to start actual reboot
	sleep(60);
}

#include "lmce_launch_managerwidget.moc"

