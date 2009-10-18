
#ifndef LM_h
#define LM_h

//#include "ServerSocket.h"
//#include "SocketException.h"
#include "LMCE_Launch_Manager.h"
#include "List.h"
#include "DB.h"
#include "UI.h"
#include "Gen_Devices/LMCE_Launch_ManagerBase.h"
#include <mysql.h>
#include "AlarmManager.h"

#define LM_KEEP_ALIVE	1
#define UPDATE_RA_STATUS 2
#define REPORT_DEVICE_UP 3
#define DO_AUTOSTART 4
#define UPDATE_ORBITER_REGEN_PROGRESS 5
#define INITIALIZATION 6

// This is for the RandR bits.
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/extensions/Xrandr.h>



class LM : public DCE::AlarmEvent
{
private:	
	// Private member variables
	Display *m_pDisplay;
	int m_iDisplayWidth;
	int m_iDisplayHeight;
	short m_iRate;
	XRRScreenConfiguration *m_pSc;
	Window m_iRootWindow;	// The Root Window
	int m_iScreen;  	// The Screen #
	SizeID m_siCurrentSize;	// The Current Size as reported by RandR
       	XRRScreenSize *m_pSizes;

	DCE::LMCE_Launch_Manager * m_pLMCE_Launch_Manager;
	UI m_uiMainUI;
	vector<string> m_vCoreDevices;
	vector<string> m_vMediaDevices;
	bool m_bRemoteAssistanceRunning;
	bool m_bDelayUpReport;
	bool m_bCoreRunning;
	bool m_bMediaRunning;

	bool m_bCoreHere;
	bool m_bMediaHere;

	bool m_bRegenInProgress;
	bool m_bRegenTrackingCurrentOrbiterOnly;
	int m_iDevicesLevel;
	string m_sAutostartCore;
	string m_sAutostartMedia;
	//Progress Tracking
	int m_iProgress;
	bool m_bShowProgress;

	string m_sDeviceID;
	string m_sOrbiterID;
	string m_sOrbiterPluginID;
	string m_sMediaID;
	string m_sCoreDeviceID;
	string m_sMySQLPort;
	string m_sDCERouterPort;
	string m_sRAInfo;
	string m_sVideoResolution;
	string m_sVideoDriver;
	bool m_bAC3Pass;
	string m_sSoundSetting;

	//MySQL Related 	
	DB m_dbPlutoDatabase;
	MYSQL m_mysqlMysql;
	MYSQL_RES *m_pResult;
	MYSQL_ROW m_mysqlRow;
	string m_sCoreIP;
	string m_sMySQLHost;
	string m_sMySQLUser;
	string m_sMySQLPass;
	string m_qsMySQLPort;  //TODO: m_qs??
	DB testDB;
	//ServerSocket m_Socket;
	//ServerSocket m_SocketConnection;

	// Private methods
	void writeLog(string s, bool toScreen=false,int logLevel=LV_STATUS);
	void writeOSD(string s);
	void appendLog(string s);
	string getRAid();
	bool checkRAStatus();  
	bool openDB();
	bool closeDB();
	string queryDB(string query, bool getFirstResult=true);
	bool checkMySQL(bool showMessage=false);
	bool checkCore(string coreIP);
	void initialize_Start();
	void initialize_Connections();
	void initialize_VideoSettings();
	void initialize_AudioSettings();
	void initialize_LogOptions();
	bool initialize_LMdevice(bool bRetryForever=false);
	void updateScripts();
	void syncWithLockList(bool eraseDeadLocalDevices=false);
	void startMediaDevices(bool checkForAlreadyRunning=false);
	void startCoreDevices(bool checkForAlreadyRunning=false);
	bool nextResultDB();
	string valueDB(int i);
	void waitForDevice(int deviceID);
	void reportDeviceUp();
	void reportDeviceDown();
	bool checkIfOrbiterIsOK();
	void loadSettings();
	
	void deinitialize_LMdevice();
	bool checkMedia();
	void findRunningMediaDevices();
	void rebootPC();
	void flushLog();
	void updateRAstatus();
	string getOrbiterStatusMessage(const string &orbiterStatus);
	
	void LMdeviceKeepAlive();
	void doAutostart();
	bool startCore();
	bool startCoreServices();
	bool checkScreenDimensions(bool askUser/*=true*/);
	bool startMediaStation();
	void updateOrbiterRegenProgress();
	void startOrbiterRegenProgressTracking(bool currentOrbiterOnly=true);
	bool triggerOrbiterRegen(bool bAllOrbiters=false);
	string getOrbiterStatus(const string &orbiterID, int &iProgressValue);
	string getOrbiterStatus();
	string getOrbiterStatus(int &iProgressValue);
	bool confirmOrbiterSkinIsReady();
	void updateOrbiterSize(int width, int height);
	void waitForDeviceShutdown(int deviceID);
	void stopCoreDevices();
	void actionActivateOrbiter();
	void saveSettings();
	void actionSaveBootupSettings();
	void actionReloadRouter();
	void actionSaveConnectionsSettings();
	void action_Resolution();
	//Process Methods
	//TODO: group together and name similarly...  Candidate for a new class??
	int exec_system(std::string cmd, bool wait=true);
	bool isRunning(int iPID);
	bool started(int iPID);

	void syncUI();
public:
	// Public member variables
	class DCE::AlarmManager *m_pAlarmManager;
	
public:
	// Constructors/Destructor
	LM();
	~LM();
	void Initialize();
	void Draw();	
	void AlarmCallback(int id, void* param);
	void Run();
	void respawnNewChildren();
	void jumpToOrbiter();

		

};


#endif

