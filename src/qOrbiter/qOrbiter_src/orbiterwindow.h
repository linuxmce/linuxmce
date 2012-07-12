#ifndef ORBITERWINDOW_H
#define ORBITERWINDOW_H

#include <QObject>
#if (QT_VERSION >= 0x050000)
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#else
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#endif
#include <QVariant>

#if GLENABLED
#include <QtOpenGL/QGLWidget>
#endif
#include <QtNetwork/QNetworkReply>


class orbiterWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString message READ getMessage WRITE setMessage NOTIFY MessageChanged)
    Q_PROPERTY (bool newOrbiter READ getOrbiterState WRITE setOrbiterState NOTIFY StatusChanged)
    Q_PROPERTY (bool b_connectionPresent READ getConnectionState WRITE setConnectionState NOTIFY connectionChanged)
    Q_PROPERTY (bool b_devicePresent READ getdeviceState WRITE setDeviceState NOTIFY deviceChanged)
    Q_PROPERTY (bool b_localConfigReady READ getLocalConfigState WRITE setLocalConfigState NOTIFY configStatus )
    Q_PROPERTY (bool b_orbiterConfigReady READ getOrbiterConfigState WRITE setOrbiterConfigState NOTIFY orbiterConfigStatus )
    Q_PROPERTY (bool b_skinIndexReady READ getSkinIndexState WRITE setSkinIndexState NOTIFY skinIndexStatus )
    Q_PROPERTY (bool b_skinDataReady READ getSkinDataState WRITE setSkinDataState NOTIFY skinDataLoaded  )

public:
    explicit orbiterWindow(long deviceid, std::string routerip, QObject *parent = 0);
    //public members

    QString message;
#if (QT_VERSION >= 0x050000)
    QQuickView  mainView;
#else
    QDeclarativeView  mainView;
#endif
    QString buildType;
    QString qrcPath;
    std::string router;
    QList<QObject*> orbiterList;
#if GLENABLED
    QGLWidget *glWidget;
#endif

    long deviceno;
    bool newOrbiter;
    bool b_connectionPresent;
    bool b_devicePresent;
    bool b_localConfigReady;
    bool b_orbiterConfigReady;
    bool b_skinIndexReady;
    bool b_skinDataReady;

public slots:
    Q_INVOKABLE void forceResponse (QString forced);

    void qmlSetupLmce(QString device, QString ip);

    void setMessage(QString imsg);    
    QString getMessage();

    void showSetup();
    void setOrbiterState(bool state);
    bool getOrbiterState();

    void showSplash();

    void setSkinDataState (bool b);
    bool getSkinDataState () {return b_skinDataReady;}

    void setSkinIndexState (bool b);
    bool getSkinIndexState () {return b_skinIndexReady;}

    void setOrbiterConfigState (bool b);
    bool getOrbiterConfigState () {return b_orbiterConfigReady;}

    void setLocalConfigState (bool b);
    bool getLocalConfigState () {return b_localConfigReady;}

    void setConnectionState (bool b) ;
    bool getConnectionState () {return b_connectionPresent;}

    void setDeviceState (bool b) ;
    bool getdeviceState () {return b_devicePresent;}

    void prepareExistingOrbiters(QList<QObject*> ex_list);

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
    void showList();
    void showExternal();

};
#endif



