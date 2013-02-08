#ifndef _LMCE_LAUNCH_MANAGERWIDGET_H_
#define _LMCE_LAUNCH_MANAGERWIDGET_H_

#include <qvaluevector.h>
#include <qprocess.h>
#include <qsettings.h>
#include <qsqldatabase.h>

#include "lmce_launch_managerwidgetbase.h"
#include "Launch_Manager.h"

// to run start/stop every 30 seconds
//#define START_STOP_DEBUG

namespace DCE
{
	class Launch_Manager;
}

class lmce_launch_managerWidget : public lmce_launch_managerWidgetBase
{
    Q_OBJECT

public:
    lmce_launch_managerWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~lmce_launch_managerWidget();
    /*$PUBLIC_FUNCTIONS$*/

    void initialize();
    bool canClose (  );
	
private:
	// initing tabs
	void initialize_Start();
	void initialize_Connections();
	void initialize_VideoSettings();
	void initialize_AudioSettings();
	void initialize_LogOptions();
	void initialize_ViewLog();
	void initialize_ConfigurationFiles();
	
	bool initialize_LMdevice(bool bRetryForever=false);
	void deinitialize_LMdevice();

	void doRedraw();
	void enableButtons(bool bEnable);
	
	void initLogLevels();
	QMap<int, QString> m_qmapLogLevels;

	void writeLog(QString s, bool toScreen=false,int logLevel=LV_STATUS);

	QTimer *m_pLogsUpdateTimer;
	QTimer *m_pProgressBarUpdateTimer;
	QTimer *m_pDevicesUpdateTimer;
	QTimer *m_pRemoteAssistanceStatusRefreshTimer;
	
#ifdef START_STOP_DEBUG
	QTimer *m_pStartStopDebugTimer;
#endif
	
	int m_iDevicesLevel;
	bool m_bRemoteAssistanceRunning;

	bool checkMySQL(bool showMessageBox=false);
	bool checkCore(QString coreIP);
	bool checkMedia();
	
	QSettings appSettings;

	bool m_bCoreRunning;
	bool m_bMediaRunning;

	bool m_bCoreHere;
	bool m_bMediaHere;
	
	void loadSettings();
	void saveSettings();
	
	bool checkIfOrbiterIsOK();

	QString m_qsDeviceID;
	QString m_qsOrbiterID;
	QString m_qsOrbiterPluginID;
	QString m_qsMediaID;
	
	QString m_qsCoreDeviceID;

	QString m_qsMySQLPort;
	QString m_qsDCERouterPort;
	
	QString m_qsRAInfo;
	
	bool m_bRegenInProgress;
	bool m_bRegenTrackingCurrentOrbiterOnly;
	
	void tuneConnectionsWidgets();
	bool startCore();
	void stopCore();
	bool startMediaStation();
	bool stopMediaStation();

	bool startCoreServices();
	bool stopCoreServices();

	void startMediaDevices(bool checkForAlreadyRunning=false);
	void stopMediaDevices();

	void startCoreDevices(bool checkForAlreadyRunning=false);
	void stopCoreDevices();
	
	void waitForDevice(int deviceID);
	void jumpToOrbiter();

	QString queryDB(QString query);
	QStringList queryDB_array(QString query);

	QValueVector<int> m_qvvCoreDevices;
	QValueVector<int> m_qvvMediaDevices;
	
	void waitForDeviceShutdown(int deviceID);
	void updateSerialPorts();
	
	QValueVector<QProcess*> m_qpvCoreServices;
	
	DCE::Launch_Manager *m_pLaunch_Manager;
	
	map<QString, QListViewItem *> m_mapLogListItems;
	
	void startOrbiterRegenProgressTracking(bool currentOrbiterOnly=true);
	
	bool checkScreenDimensions(bool askUser=true);
	bool checkXsettings();
	
	bool openDB();
	bool closeDB();

	bool allowRedraw;
	void findRunningMediaDevices();
	
	bool confirmOrbiterSkinIsReady();
		
	void updateOrbiterSize(int width, int height);
	bool triggerOrbiterRegen(bool bAllOrbiters=false);
	
	QSqlDatabase *pPlutoDatabase;
	bool performQueryDB(QString query, QSqlQuery &queryResult);
	void printSQLerror(QSqlError &err);
	bool m_bDelayUpReport;
	void syncWithLockList(bool eraseDeadLocalDevices=false);
	
	bool checkRAStatus();
	QString getRAid();
	void flushLog();
	void rebootPC();
		
protected:
	
public slots:
    /*$PUBLIC_SLOTS$*/
	void updateLogsList();
	void lvLogs_currentChanged( QListViewItem * );
	void doAutostart();
	void updateOrbiterRegenProgress();
	void updateDevicesProgressBar();
	void respawnNewChildren();
	void LMdeviceKeepAlive();
	QString getOrbiterStatus();
	QString getOrbiterStatus(int &iProgressValue);
	QString getOrbiterStatus(const QString &orbiterID, int &iProgressValue);
	QString getOrbiterStatusMessage(const QString &orbiterStatus);
	
	// updates RA config
	void updateRAstatus();

	void reportDeviceUp();
	void reportDeviceDown();
	
	// logs operations
	virtual void btnViewLog_clicked();
	virtual void btnTailLog_clicked();
    	virtual void btnEditConfigurationFile_clicked();
	virtual void btnApplyLogOptions_clicked();
	virtual void btnSaveConnectionsSettings_clicked();
	virtual void btnRotateAllLogs_clicked();
	virtual void btnOpenAdminSite_clicked();
	virtual void btnTestMySQLConnection_clicked();
	virtual void btnSwitchCore_clicked();
	virtual void btnSwitchCoreAndMedia_clicked();
	virtual void btnSwitchMedia_clicked();
	virtual void btnRegenThisOrbiter_clicked();
	virtual void btnReloadRouter_clicked();
	virtual void btnRegenAllOrbiters_clicked();
	virtual void btnApplyAudioSettings_clicked();
	virtual void btnActivateOrbiter_clicked();
	virtual void saveBootupSettings_clicked();
	virtual void btn_StartUIDiag_clicked();
	virtual void twAllTabs_currentChanged( QWidget * );
	virtual void btn_SwitchRA_clicked();
	virtual void btn_ViewIP_clicked();
	virtual void btnChange_Resolution_clicked();
		
	//custom events
	virtual bool event(QEvent *e);

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

};

#endif

