
#ifndef LM_h
#define LM_h

#include "LMCE_Launch_Manager.h"
#include "List.h"
#include "Gen_Devices/LMCE_Launch_ManagerBase.h"
#include <mysql.h>




class LM
{
private:	
	// Private member variables
	DCE::LMCE_Launch_Manager * m_pLMCE_Launch_Manager;
	int m_iDevicesLevel;
	bool m_bRemoteAssistanceRunning;

	bool m_bCoreRunning;
	bool m_bMediaRunning;

	bool m_bCoreHere;
	bool m_bMediaHere;

	bool m_bRegenInProgress;
	bool m_bRegenTrackingCurrentOrbiterOnly;
	
	string m_sAutostartCore;
	string m_sAutostartMedia;

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

	//MySQL Related 	
	MYSQL *pm_mysqlPlutoDatabase;
	MYSQL m_mysqlMysql;
	MYSQL_RES *m_msqlResult;
	MYSQL_ROW m_mysqlRow;
	string m_sCoreIP;
	string m_sMySQLHost;
	string m_sMySQLUser;
	string m_sMySQLPass;
	string m_qsMySQLPort;

	// Private methods
	void writeLog(string s, bool toScreen=false,int logLevel=LV_STATUS);
	string getRAid();
	bool checkRAStatus();  
	bool openDB();
	bool closeDB();
	string queryDB(string query);
	bool checkMySQL(bool showMessage=false);
	bool checkCore(string coreIP);
	void initialize_Start();
	void initialize_Connections();
	void initialize_VideoSettings();
	void initialize_AudioSettings();
	void initialize_LogOptions();
	bool initialize_LMdevice(bool bRetryForever=false);
	void updateSerialPorts();
	void respawnNewChildren()
	void syncWithLockList(bool eraseDeadLocalDevices=false)
	void startMediaDevices(bool checkForAlreadyRunning=false);
	void startCoreDevices(bool checkForAlreadyRunning=false);
public:
	// Public member variables

	
public:
	// Constructors/Destructor
	LM();
	~LM();
	void Initialize();
	void Draw();	
	



};


#endif

