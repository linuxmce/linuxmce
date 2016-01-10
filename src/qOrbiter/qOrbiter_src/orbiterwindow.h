#ifndef ORBITERWINDOW_H
#define ORBITERWINDOW_H

#include <QObject>
#include <QDebug>
#include <contextobjects/existingorbiter.h>
#ifdef QT5
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <qtquick2applicationviewer/qtquick2applicationviewer.h>
#include "qqmlapplicationengine.h"
#else
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <qmlapplicationviewer.h>

#endif
#include <QVariant>
#ifdef GLENABLED
#include <QtOpenGL/QGLWidget>
#include <shaders/filereader.h>
#endif
#include <QtNetwork/QNetworkReply>

#ifdef ANDROID
#include <QKeyEvent>
#endif
class PromptData;
/*!
 * \brief orbiterWindow - Responsible for the initial setup of the application window.

 *\ingroup qt_controllers
 *orbiterWindow is responsible for setting up the initial application window and states of the application. It also initializes opengl
 *surfaces and other special windowing needs for each platform including setting up the QRC locations and relative skin location for the application.
 *It provides a means in which the application can kickstart before its connected to the DCErouter to download its configuration. There are some
 *hard defaults based on platform in this class to establish a usable baseline before the application takes on its 'running' form.
 */
class orbiterWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool orbiterInitialized READ getOrbiterInitialized() NOTIFY beginLoading)
    Q_PROPERTY (QString router READ getRouterAddress WRITE setRouterAddress NOTIFY routerChanged)
    Q_PROPERTY (int deviceno READ getDeviceNumber WRITE setDeviceNumber NOTIFY deviceChanged)
    Q_PROPERTY (QString message READ getMessage WRITE setMessage NOTIFY MessageChanged)
    Q_PROPERTY (bool newOrbiter READ getOrbiterState WRITE setOrbiterState NOTIFY StatusChanged)
    Q_PROPERTY (bool b_connectionPresent READ getConnectionState WRITE setConnectionState NOTIFY connectionChanged) /*! window.b_connectionPresent:True if router is located \ingroup startup_properties  */
    Q_PROPERTY (bool b_devicePresent READ getdeviceState WRITE setDeviceState NOTIFY deviceChanged)/*! window.b_devicePresent : This changes when the device number does \warning use only on splash screen. \ingroup startup_properties */
    Q_PROPERTY (bool b_localConfigReady READ getLocalConfigState WRITE setLocalConfigState NOTIFY configStatus ) /*! window.b_localConfigReady : True when the local xml file is read and is valid. \ingroup startup_properties */
    Q_PROPERTY (bool b_orbiterConfigReady READ getOrbiterConfigState WRITE setOrbiterConfigState NOTIFY orbiterConfigStatus ) /*! window.b_orbiterConfigReady : The status of the remote configuration \ingroup startup_properties */
    Q_PROPERTY (bool b_skinIndexReady READ getSkinIndexState WRITE setSkinIndexState NOTIFY skinIndexStatus )/*! window.b_skinIndexReady: The status of the skin loading \ingroup startup_properties  */
    Q_PROPERTY (bool b_skinDataReady READ getSkinDataState WRITE setSkinDataState NOTIFY skinDataLoaded  ) /*! window.b_skinDataReady: True when skins are done being loaded \ingroup startup_properties */
    Q_PROPERTY(bool b_reloadStatus READ getReloadStatus WRITE setReloadStatus NOTIFY reloadStatusChanged) /*! window.b_reloadStatus \ingroup startup_properties */
    Q_PROPERTY (QString qmlPage READ getQmlPage WRITE setQmlPage NOTIFY pageChanged)/*! window.qmlPage \ingroup startup_properties */
    Q_PROPERTY (QString internalIp READ getInternalIp WRITE setInternalIp NOTIFY internalIpChanged)
    Q_PROPERTY (QString externalIp READ getExternalIp WRITE setExternalIp NOTIFY externalIpChanged)
    Q_PROPERTY (bool usingExternal READ getUsingExternal WRITE setUsingExternal NOTIFY usingExternalChanged)
public:

    /*!
 * \brief orbiterWindow - Responsible for the initial setup of the application window.
 * \param deviceid - Initial device id passed from command line or set by the default (-1)
 * \param routerip - Initial router ip passed from command line or set by the default (192.168.80.1)
 * \param fullScreen - Set true if you want the device to be set to full screen \warning desktop only
 * \param frameless - set to true of no frame is desired.
 * \param parent
     */
    explicit orbiterWindow(int deviceid, std::string routerip, bool fullScreen, bool frameless, int simScreenSize, QQmlApplicationEngine *engine,  QObject *parent = 0);
    //public members

    QString message;
#ifdef QT5
    QQuickView mainView;
#else
   QDeclarativeView  mainView;
#endif

    QString buildType;
    QString qrcPath;
    QString router;
    QList<ExistingOrbiter*> orbiterList;
    QList<QObject*> userList;
    QList<QObject*> roomList;
#ifdef GLENABLED
    QGLWidget *glWidget;
    FileReader *fileReader;
#endif

    long deviceno;
    bool newOrbiter;
    bool b_connectionPresent;
    bool b_devicePresent;
    bool b_localConfigReady;
    bool b_orbiterConfigReady;
    bool b_skinIndexReady;
    bool b_skinDataReady;
    bool b_reloadStatus;
    bool b_commandStatus;
    QString qmlPage;
    QString localPath;
    bool phoneSize;
    bool fullScreenOrbiter;
    bool orbiterInitialized;
    QString internalIp;
    QString externalIp;
    bool usingExternal;

    void initView();

public slots:
    void setUsingExternal(bool x){ if(x!= usingExternal) {usingExternal = x; emit usingExternalChanged();}  }
    bool getUsingExternal() {return usingExternal;}

    void setExternalIp(QString ip){qDebug() << "updating external ip for window."; if(externalIp != ip){externalIp = ip; emit externalIpChanged();} }
    QString getExternalIp(){return externalIp;}

    void setInternalIp(QString ip) { if(internalIp != ip) {internalIp = ip; emit internalIpChanged();} }
    QString getInternalIp(){return internalIp;}

    void setDeviceNumber(int d){deviceno = d; qDebug() << "new Device number "<< d ;emit deviceChanged();}
    int getDeviceNumber(){return deviceno;}

    QString getRouterAddress(){return router;}
    void setRouterAddress(QString r) {router = r; emit routerChanged();}

    void setQmlPage(QString p) {qmlPage = p; emit pageChanged(); }
    QString getQmlPage() {return qmlPage;}

    Q_INVOKABLE void forceResponse (QString forced);
    Q_INVOKABLE bool hasOrbiters() {return orbiterList.count() != 0;}
    void loadSetupPage();

    bool getOrbiterInitialized(){return orbiterInitialized;}
    void setOrbiterInitialized(){orbiterInitialized = true; qDebug() <<Q_FUNC_INFO << "::Orbiter Initialized!";emit beginLoading(); ;}




    /*!
     * \brief qmlSetupLmce - Initiates a connection and startup procedure
     * \param device
     * \param ip
     *This function starts a connection to the router given the params. Depending on if the ip and or device are valid, the application
     *will start or it will present other options.
     */
    void qmlSetupLmce(int device, QString ip);

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
    /*!
     * \brief prepareExistingOrbiters - This function exists to emit a signal to the router to request a list of ExistingOrbiters
     */
    void prepareExistingOrbiters();

    /*!
     * \brief displayPromptResponse - Populates prompt model with data
     * \param type - the type of data
     * \param pList
     *Returns the last requested prompt data.
     */
    void displayPromptResponse(int type, QList<PromptData*> *pList);

    /*!
     * \brief setupNewOrbiter - Sets up a new orbiter given the params
     * \param user
     * \param room
     * \param skin
     * \param lang
     * \param height
     * \param w
     *This function takes the set options from the NewOrbiter screen and passes them to the router connection for a new orbiter to be created
     *After this is done, a regen occurs to generate to configurations
     * \warning This function needs more testing and implementing it is not recommended at this time
     */
    void setupNewOrbiter(int user, int room, int skin, int lang, int height, int w, QString deviceName);

    bool getReloadStatus() {return b_reloadStatus;}
    void setReloadStatus(bool b) {b_reloadStatus = b ; emit reloadStatusChanged();}

signals:
    void MessageChanged();
    void setupLmce(int device, QString router);
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
    void beginSetupNewOrbiter();
    void reloadStatusChanged();
    void newOrbiterData(int u, int r, int s, int l, int h, int w, QString dev);
    void pageChanged();
    void routerChanged();
    void backButtonPressed();
    void beginLoading();
    void internalIpChanged();
    void externalIpChanged();
    void usingExternalChanged();
    void creationComplete(bool complete);

private :
    QQmlApplicationEngine *m_appEngine;
};
#endif



