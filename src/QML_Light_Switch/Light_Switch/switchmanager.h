#ifndef SWITCHMANAGER_H
#define SWITCHMANAGER_H

#include <QMainWindow>
#include <QtDeclarative>
#include <QDebug>
#include <QStringList>

//#include <QMYSQLDriver>
#include <QtSql/QSqlDatabase>
#include <QtSql/qsql_mysql.h>
#include "/usr/include/mysql/mysql.h"

#include <Gen_Devices/QML_Light_SwitchBase.h>
#include <QML_Light_Switch/QML_Light_Switch.h>

class switchManager : public QMainWindow
{
    Q_OBJECT
public:
     switchManager(QWidget *parent = 0);
     DCE::QML_Light_Switch dceLightSwitch;
     //ui area
     QDeclarativeView *ui;

     //dce configuration and startup
     //runs through intilizations of variables
     void initialize_Start();



signals:

public slots:

private:
     int initializeDCE();
     void loadUI();

     //connections for configuration data
     void initialize_Connections();
     QString m_qsDeviceID;
     QString m_qsOrbiterID;
     QString m_qsOrbiterPluginID;
     QString m_qsMediaID;

     //starts linuxMCE DCE device
     DCE::QML_Light_Switch *m_pLightSwitch;
     bool initialize_LMdevice(bool bRetryForever=false);
     bool m_bRegenInProgress;

     //stops linuxMCE DCE device
     void deinitialize_LMdevice();

     //core data
     bool checkCore(QString coreIP);
     QString m_qsDCERouterPort;
     QString m_qsCoreDeviceID;

     //mysql and core connections
     bool checkMySQL(bool showMessageBox=false);
     QString m_qsMySQLPort;
     QString queryDB(QString query);
     QStringList queryDB_array(QString query);
     bool openDB();
     bool closeDB();
     QSqlDatabase pPlutoDatabase;
     bool performQueryDB(QString query, QSqlQuery &queryResult);
     void printSQLerror(QSqlError &err);

};

#endif // SWITCHMANAGER_H
