    #ifndef SWITCHMANAGER_H
#define SWITCHMANAGER_H

#include <QMainWindow>
#include <qtlightswitch.h>
#include <QtDeclarative>
#include <QDebug>
#include <QStringList>
#include <QFile>



#include <Gen_Devices/AllCommandsRequests.h>
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
     switchManager( int pk_deviceID, QWidget *parent = 0);

     //ui area
     QDeclarativeEngine engine;
     QDeclarativeComponent interFace;
     QDeclarativeView *ui;
     QTlightSwitch  switchInstance;
     QObject qml_view;

     //dce configuration and startup
     //runs through intilizations of variables

     void initialize_Start();
     void loadUI();
     void initialize_QML();
     bool initialize_LMdevice(bool bRetryForever=false);
     QString m_pFloorplanObjType;
     QString r_lightLevel;
     QVariant gVar;
     //qt device command slots
signals:
     QString light_status(QString);
     void DCEon();
     void DCEoff();

public slots:
    void close_app();
    void LMdeviceKeepAlive();
    void r_On();
    void r_Off();
    void r_setLevel(int rLevel);

    Q_INVOKABLE void writeLog(QString msg);
    Q_INVOKABLE void  light_on();
    Q_INVOKABLE void  light_off();
    Q_INVOKABLE void  set_level();



private:
     //connections for configuration data
     void initialize_Connections();
     bool check_log();
     QFile *logFile;
     int m_qsDeviceID;
     QString m_qsOrbiterID;
     QString m_qsOrbiterPluginID;
     QString m_qsMediaID;

     //starts linuxMCE DCE device
     DCE::QML_Light_Switch *m_pLightSwitch;

     bool m_bRegenInProgress;
     bool bConnected;
     bool bReconnect;

     //stops linuxMCE DCE device
     void deinitialize_LMdevice();

     //core data
     bool checkCore(QString coreIP);
     QString m_qsDCERouterPort;
     QString m_qsCoreDeviceID;



};

#endif // SWITCHMANAGER_H
