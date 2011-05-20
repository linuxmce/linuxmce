#ifndef SWITCHMANAGER_H
#define SWITCHMANAGER_H

#include <QMainWindow>
#include <QtDeclarative>
#include <QDebug>
#include <QStringList>
#include <QFile>

#include <Gen_Devices/QML_Light_SwitchBase.h>
#include <QML_Light_Switch/QML_Light_Switch.h>

namespace DCE
{
class QML_Light_Switch ;
}


class switchManager : public QMainWindow
{
    Q_OBJECT
public:
     switchManager(QWidget *parent = 0);

     //ui area
     QDeclarativeView *ui;

     //dce configuration and startup
     //runs through intilizations of variables

     void initialize_Start();

signals:

public slots:
    void close_app();
Q_INVOKABLE void writeLog(QString msg);

private:
     void loadUI();



     //connections for configuration data
     void initialize_Connections();
     bool check_log();
     QFile *logFile;
     QString m_qsDeviceID;
     QString m_qsOrbiterID;
     QString m_qsOrbiterPluginID;
     QString m_qsMediaID;

     //starts linuxMCE DCE device
     DCE::QML_Light_Switch *m_pLightSwitch;

     bool initialize_LMdevice(bool bRetryForever=false);
     bool m_bRegenInProgress;
     bool bConnected;

     //stops linuxMCE DCE device
     void deinitialize_LMdevice();

     //core data
     bool checkCore(QString coreIP);
     QString m_qsDCERouterPort;
     QString m_qsCoreDeviceID;

};

#endif // SWITCHMANAGER_H
