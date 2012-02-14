#ifndef ORBITERWINDOW_H
#define ORBITERWINDOW_H

#include <QObject>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QVariant>

class orbiterWindow : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString message READ getMessage WRITE setMessage NOTIFY MessageChanged)
    Q_PROPERTY (bool newOrbiter READ getOrbiterState WRITE setOrbiterState NOTIFY StatusChanged)
    Q_PROPERTY (bool b_connectionPresent READ getConnectionState WRITE setConnectionState NOTIFY connectionChanged)
    Q_PROPERTY (bool b_devicePresent READ getdeviceState WRITE setDeviceState NOTIFY deviceChanged)
    Q_PROPERTY (bool b_localConfigReady READ getLocalConfigState WRITE setLocalConfigState NOTIFY configStatus )
    Q_PROPERTY (bool b_orbiterConfigReady READ getOrbiterConfigState WRITE setOrbiterConfigState NOTIFY orbiterConfigStatus )
    Q_PROPERTY (bool b_skinIndexReady READ getSkinIndexState WRITE setSkinIndexState NOTIFY skinIndexStatus() )
    Q_PROPERTY (bool b_skinDataReady READ getSkinDataState WRITE setSkinDataState NOTIFY skinDataLoaded  )


public:
    explicit orbiterWindow(long deviceid, std::string routerip, QObject *parent = 0);
    //public members

    QString message;
    QDeclarativeView  mainView;

    QString buildType;
    QString qrcPath;
    Q_INVOKABLE void forceResponse (QString forced);

    std::string router;
    long deviceno;

    bool newOrbiter;

    //---inline for my sanity
    bool b_connectionPresent;
    bool b_devicePresent;
    bool b_localConfigReady;
    bool b_orbiterConfigReady;
    bool b_skinIndexReady;
    bool b_skinDataReady;

    void setOrbiterState(bool state);
    bool getOrbiterState();
    void showSplash();

    void setSkinDataState (bool b) {b_skinDataReady = b; emit connectionChanged(); }
    bool getSkinDataState () {return b_skinDataReady;}

    void setSkinIndexState (bool b) {b_skinIndexReady = b; emit skinIndexStatus(); }
    bool getSkinIndexState () {return b_skinIndexReady;}

    void setOrbiterConfigState (bool b) {b_orbiterConfigReady = b; emit orbiterConfigStatus(); }
    bool getOrbiterConfigState () {return b_orbiterConfigReady;}

    void setLocalConfigState (bool b) {b_localConfigReady = b; emit configStatus(); }
    bool getLocalConfigState () {return b_localConfigReady;}

    void setConnectionState (bool b) {b_connectionPresent = b; emit connectionChanged(); }
    bool getConnectionState () {return b_connectionPresent;}

    void setDeviceState (bool b) {b_devicePresent = b; emit deviceChanged(); }
    bool getdeviceState () {return b_devicePresent;}

signals:
    void MessageChanged();
    void setupLmce(QString device, QString routerIp);
    void StatusChanged();
    void connectionChanged();
    void configStatus();
    void orbiterConfigStatus();
    void skinIndexStatus();
    void skinDataLoaded();
    void deviceChanged();
    void orientationChanged(QSize);
    void checkRes();


public slots:
    void qmlSetupLmce(QString device, QString ip);
    void setMessage(QString imsg);
    QString getMessage();
    void showSetup();


};
#endif



