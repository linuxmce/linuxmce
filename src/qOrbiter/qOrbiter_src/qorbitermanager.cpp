/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGlobal>
#include <QtXml/QtXml>
#if (QT5)
#include <QtCore/QFile>
#include <QtQml/QtQml>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#else
#include <QtDeclarative/QDeclarativeProperty>
#include <QFile>
#include <QProcess>
#include <QtDeclarative/QDeclarativeEngine>
#include <QApplication>
#include <QTimer>
#include <QtDeclarative/QtDeclarative>
#endif
#include <imageProviders/filedetailsimageprovider.h>
#include <contextobjects/epgchannellist.h>
#include <datamodels/skindatamodel.h>

#include "qorbitermanager.h"




//#include "OrbiterData.h"


/*!
  this file is responsible for the main connection between the dce thread and the qt thread.
   pqOrbiter->qmlUI = this; inside of setupLMCE initializes the dce thread, while other functions are
   responsible for signals and slots between dce and qml.
  */

using namespace DCE;

/*!
 *
  This is the latest incarnation of the orbiter constructor. its purpose is to quickly spawn a splash window, which can
  then (hopefully) notify us of background progress. A splash bar or loading indicator needs to be added, but a textual
  messaging system will be the initial method of communication
*/
#if (QT5)
qorbiterManager::qorbiterManager(QQuickView *view, QObject *parent) :
    #else
qorbiterManager::qorbiterManager(QDeclarativeView *view, QObject *parent) :
    #endif
    QObject(parent),qorbiterUIwin(view)
{
    //view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_bStartingUp= true;
    // b_skinsReady = false;
    // b_orbiterReady = false;
    bAppError = false;
    isPhone = 0;

#ifndef __ANDROID__
    b_localLoading = true; /*! this governs local vs remote loading. condensed to one line, and will be configurable from the ui soon. */
#else
    b_localLoading = false;
#endif
    //pqOrbiter->qmlUI = this;
    setDceResponse("Starting...");


    if (readLocalConfig())
    {
        emit localConfigReady(true);
        // QApplication::processEvents(QEventLoop::AllEvents);
    }
    else
    {
        emit localConfigReady(false);
    }

    QApplication::processEvents(QEventLoop::AllEvents);

    myOrbiters = new ExistingOrbiterModel(new ExistingOrbiter(), this);
    devices = new DeviceModel(new AvDevice(), this );
    deviceCommands = new AvCodeGrid(new AvCommand(), this);
  //  qorbiterUIwin->rootContext()->setContextProperty("srouterip", QString(qs_routerip) );
  //  qorbiterUIwin->rootContext()->setContextProperty("deviceid", QString::number((iPK_Device)) );
    qorbiterUIwin->rootContext()->setContextProperty("extip", qs_ext_routerip );
    qorbiterUIwin->rootContext()->setContextProperty("manager", this); //providing a direct object for qml to call c++ functions of this class
    qorbiterUIwin->rootContext()->setContextProperty("dcemessage", dceResponse);
    qorbiterUIwin->rootContext()->setContextProperty("orbiterList", myOrbiters);
    qorbiterUIwin->rootContext()->setContextProperty("deviceList", devices);
    qorbiterUIwin->rootContext()->setContextProperty("deviceCommands", deviceCommands);
    appHeight = qorbiterUIwin->height() ;
    appWidth = qorbiterUIwin->width() ;

    //Resize to view as opposed to the root item
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qorbiterUIwin->setResizeMode(QQuickView::SizeRootObjectToView);
#else
    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif

    qorbiterUIwin->rootContext()->setContextProperty("appH", appHeight);
    qorbiterUIwin->rootContext()->setContextProperty("appW", appWidth);

    //Don't think we need this with Qt5 QQuickView as it resizes Object to View above
#ifndef QT5
    QObject::connect(qorbiterUIwin, SIGNAL(sceneResized(QSize)),  SLOT(checkOrientation(QSize)) );
#else
    QObject::connect(qorbiterUIwin, SIGNAL(heightChanged(int)), this, SLOT(setAppH(int)));
    QObject::connect(qorbiterUIwin, SIGNAL(widthChanged(int)), this, SLOT(setAppW(int)));
#endif

    QObject::connect(this, SIGNAL(orbiterReady(bool)), this, SLOT(showUI(bool)));
    QObject::connect(this, SIGNAL(skinDataLoaded(bool)), SLOT(showUI(bool)));
    QObject::connect(view->engine(), SIGNAL(quit()), this, SLOT(closeOrbiter()));


    /*!
     * \todo move buildtype / qrc path code to its own function and return it here
     */

#ifdef for_desktop
#ifndef QT5
    buildType = "/qml/desktop";
#else
    buildType = "/qml/rpi";
#endif
    qrcPath = buildType+"/Splash.qml";
    //mainView.setSource(QApplication::applicationDirPath()+qrcPath);
#elif defined (WIN32)
    buildType="/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
#elif defined (for_freemantle)
    buildType = "/qml/freemantle";
    qrcPath = "qrc:freemantle/Splash.qml";
#elif defined (for_harmattan)
    buildType="/qml/harmattan";
    qrcPath = "qrc:harmattan/Splash.qml";
#elif defined (Q_OS_MACX)
    buildType="/qml/desktop";
    qrcPath = "qrc:osx/Splash.qml";
#elif defined (RPI)
    buildType="/qml/rpi";

#elif defined (__ANDROID__)
    if (qorbiterUIwin->width() > 480 && qorbiterUIwin-> height() > 854 || qorbiterUIwin->height() > 480 && qorbiterUIwin-> width() > 854 )
    {
        setFormFactor(2);
        buildType = "/qml/android/tablet";
    }
    else
    {
        setFormFactor(1);
        if(isPhone !=2){
            buildType = "/qml/android/phone";
        }
    }

    qrcPath = ":android/Splash.qml";
    droidPath = "/";

    if(qorbiterUIwin->width() > 480 && qorbiterUIwin-> height() > 854 || qorbiterUIwin->height() > 480 && qorbiterUIwin-> width() > 854)
    {
        emit mediaSeperatorChanged(50);
    }
    else
    {
        emit mediaSeperatorChanged(20);
    }
#elif defined (for_android)
    if (qorbiterUIwin->width() > 480 && qorbiterUIwin-> height() > 854 || qorbiterUIwin->height() > 480 && qorbiterUIwin-> width() > 854 )
    {
        buildType = "/qml/android/tablet";
    }
    else
    {
        buildType = "/qml/android/phone";

    }

    qrcPath = ":android/Splash.qml";
    droidPath = "/";
#else
    buildType = "/qml/desktop";
    qrcPath = ":desktop/Splash.qml";
#endif

    qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin"));
    setApplicationPath(QApplication::applicationDirPath());
    localDir = qmlPath.append(buildType);

    initializeGridModel();  //begins setup of media grid listmodel and its properties
    initializeSortString(); //associated logic for navigating media grids

    //managing where were are variables
    i_current_command_grp = 0;
    i_current_mediaType =0;
    videoDefaultSort = "13";
    audioDefaultSort = "2";
    photoDefaultSort = "13";
    gamesDefaultSort = "49";
    i_currentFloorplanType = 0;
    backwards = false;

    //file details object and imageprovider setup
    filedetailsclass = new FileDetailsClass();
    qorbiterUIwin->rootContext()->setContextProperty("filedetailsclass" ,filedetailsclass);
    filedetailsclass->clear();

    nowPlayingButton = new NowPlayingClass();
    qorbiterUIwin->rootContext()->setContextProperty("dcenowplaying" , nowPlayingButton);

    //screen parameters class that could be extended as needed to fetch other data
    ScreenParameters = new ScreenParamsClass;
    qorbiterUIwin->rootContext()->setContextProperty("screenparams", ScreenParameters);

    //floorplan model initialization for slots in main.cpp
    floorplans = new FloorPlanModel( new FloorplanDevice , this);


    //----------------Security Video setup
    SecurityVideo = new SecurityVideoClass();
    qorbiterUIwin->rootContext()->setContextProperty("securityvideo", SecurityVideo);

    //-alarms
    sleeping_alarms = new SleepingAlarmModel(this);
    qorbiterUIwin->rootContext()->setContextProperty("alarms",sleeping_alarms );

    QApplication::processEvents(QEventLoop::AllEvents);
    attribFilter = new AttributeSortModel(new AttributeSortItem,6, this);
    uiFileFilter = new AttributeSortModel(new AttributeSortItem,2, this);
    mediaTypeFilter = new AttributeSortModel(new AttributeSortItem,1, this);
    genreFilter = new AttributeSortModel(new AttributeSortItem,3, this);

    gotoScreenList = new QStringList();
    ScreenSaver = new ScreenSaverClass(this);
    qorbiterUIwin->engine()->rootContext()->setContextProperty("screensaver", ScreenSaver);
    /*!
     * \todo move filters to their own initialization function, possibly multiple to account for dynamic setting of each one later.
     */
}

qorbiterManager::~qorbiterManager()
{
    this->deleteLater();
}


void qorbiterManager::gotoQScreen(QString s)
{

    if(s == "Screen_1.qml")
    {
        bool t = false;
        emit keepLoading(t);
        emit clearModel();
        emit cancelRequests();
        emit resetFilter();
    }

    //send the qmlview a request to go to a screen, needs error handling
    //This allows it to execute some transition or other if it wants to

    setDceResponse("Starting screen switch");
    QString lameFix = s.replace("http://", "https://");
    qDebug() << lameFix;
    QVariant screenname= lameFix;

    QObject *item = qorbiterUIwin->rootObject();
    setDceResponse("About to call screenchange()");
    if (QMetaObject::invokeMethod(item, "screenchange", Qt::QueuedConnection, Q_ARG(QVariant, screenname))) {
        setDceResponse("Done call to screenchange()");
        setCurrentScreen(lameFix);
        if(!currentScreen.contains("187"))
            gotoScreenList->append(currentScreen);

    } else {
        setDceResponse("screenchange() FAILED");
    }
}

//! Send the user back to the previous screen in the list
void qorbiterManager::goBacktoQScreen()
{
    if(gotoScreenList->count() > 10)
        gotoScreenList->removeFirst();

    if(!gotoScreenList->isEmpty()){
        gotoScreenList->removeLast();
        setDceResponse("Starting screen switch");
        QVariant screenname= QVariant::fromValue(gotoScreenList->last());
        currentScreen = gotoScreenList->last();
        QObject *item = qorbiterUIwin->rootObject();
        setDceResponse("About to call screenchange()");
        if (QMetaObject::invokeMethod(item, "screenchange", Qt::QueuedConnection, Q_ARG(QVariant, screenname))) {
            setDceResponse("Done call to screenchange()");

        } else {
            setDceResponse("screenchange() FAILED");
        }
    }
}

/*!
 * \brief qorbiterManager::initializeManager: Initializes the QorbiterManager startup procedure.
 * \param sRouterIP
 * \param device_id
 * \return
 *This function is responsible for setting the various paths associated with the QML skins, intiating the loading of those skins
 *and reporting sucess or failure in loading those skins.
 */
bool qorbiterManager::initializeManager(string sRouterIP, int device_id)
{
    setDeviceNumber(device_id);
    setDceResponse("Starting Manager");
    QObject::connect(this,SIGNAL(screenChange(QString)), this, SLOT(gotoQScreen(QString)));

#ifdef __ANDROID__
    if (qorbiterUIwin->width() > 480 && qorbiterUIwin-> height() > 854 || qorbiterUIwin->height() > 480 && qorbiterUIwin-> width() > 854 )
    {
        remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/android/tablet";
        setDceResponse("Guessing Android Tablet, Loading Tablet Skins");
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    else
    {
        remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/android/phone";
        setDceResponse("Guessing Android Phone, Loading Phone Skins");
        QApplication::processEvents(QEventLoop::AllEvents);
    }

#elif MACOSX
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/macosx";
#elif for_desktop
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/desktop";
#elif WIN32
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/desktop";
#elif RPI
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/rpi";
#elif for_harmattan
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/harmattan";
#elif for_android
    if (qorbiterUIwin->width() > 480 && qorbiterUIwin-> height() > 854 || qorbiterUIwin->height() > 480 && qorbiterUIwin-> width() > 854 )
    {
        buildType = "/qml/android/tablet";
        remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/android/tablet";
        setDceResponse("Guessing Android Tablet, Loading Tablet Skins");
    }
    else
    {
        buildType = "/qml/android/phone";
        remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins/android/phone";
        setDceResponse("Guessing Android Phone, Loading Phone Skins");
    }
#else
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins"+qmlPath;
#endif
    QString qmlPath = adjustPath(QApplication::applicationDirPath().remove("bin"));
    QString localDir = qmlPath.append(buildType);


#ifdef __ANDROID__
    setDceResponse("Loading Skins");
    if( !loadSkins(QUrl(remoteDirectoryPath)))
    {   emit skinIndexReady(false);
        b_skinReady = false;
        return false;
    }
#elif for_android
    setDceResponse("Loading skins");
    if( !loadSkins(QUrl(localDir)))
    {   emit skinIndexReady(false);
        return false;
    }
#elif WIN32
    setDceResponse("Loading skins");
    if( !loadSkins(QUrl::fromLocalFile(localDir)))
    {   emit skinIndexReady(false);
        return false;
    }
#elif for_desktop
    if(b_localLoading == true)
    {
        if( !loadSkins(QUrl(localDir)))
        {
            emit skinIndexReady(false);
            return false;
        }
    }
#elif for_harmattan
    if(b_localLoading == true)
    {
        if( !loadSkins(QUrl(localDir)))
        {
            emit skinIndexReady(false);
            return false;
        }
    }
    else
    {
        if( !loadSkins(QUrl(remoteDirectoryPath)))
        {
            emit skinIndexReady(false);
            return false;
        }
    }

#elif RPI

    if( !loadSkins(QUrl(localDir)))
    {
        emit skinIndexReady(false);
        return false;
    }
#else
    if( !loadSkins(QUrl(localDir)))
    {
        emit skinIndexReady(false);
        return false;
    }
#endif
}

//this functions purpose is to change the UI to the new skin pointed to. It will evolve to encompass orbiter regen to some extent
/*!
 * \brief qorbiterManager::refreshUI: This function re-initializes the qml engines skin.
 * \param url - The relative or absolute path of the skin to the binary.
 */
void qorbiterManager::refreshUI(QUrl url)
{

#if (QT5)
    qorbiterUIwin->setResizeMode(QQuickView::SizeRootObjectToView);
#else
    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif


    QUrl fixed  = skin->entryUrl();
    if(fixed.scheme()=="http"){
       // fixed.setScheme("https");
    }


    qorbiterUIwin->rootContext()->setBaseUrl(fixed);

    qorbiterUIwin->setSource(fixed);

}


// get conf method that reads config file
/*!
 * \brief qorbiterManager::processConfig: Process XML based config file.
 * \param config
 *This function is called when the configuration xml file needs to be processed. It goes through the file and reads the XML into an object
 *where its data is extracted at various parts to populate the user interface.
 *
 * \note Many of the sections here have been made dynamic in an attempt to remove the need for the xml based configuration file.
 */
void qorbiterManager::processConfig(QByteArray config)
{
    iPK_Device_DatagridPlugIn =  long(6);
    iPK_Device_OrbiterPlugin = long(9);
    iPK_Device_GeneralInfoPlugin = long(4);
    iPK_Device_SecurityPlugin = long(13);
    iPK_Device_LightingPlugin = long(8);
    m_dwIDataGridRequestCounter = 0;
    iOrbiterPluginID = 9;
    iMediaPluginID = 10;
    iPK_Device_eventPlugin = 12;
    iSize = 0;
    m_pOrbiterCat = 5;
    s_onOFF = "1";

    QDomDocument configData;
    QByteArray tConf =  config;
    configData.setContent(config,false);
    if(configData.isDocument() == false)
    {
        setDceResponse("Invalid config for device: " + QString::number(iPK_Device));
        setDceResponse("Please run http://dcerouter/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device)) ;
        setDceResponse("Invalid Config");
        emit orbiterConfigReady(false);
    }

    setDceResponse("Attempting to write config");
    if (!writeConfig())
    {
        setDceResponse("Couldnt save config!");
    }

    QDomElement root = configData.documentElement();        //represent configuration in memeory

    //------------DEFAULTS-FOR-ORBITER------------------------------------------------------------
    QDomElement defaults = root.firstChildElement("Default");
    QString sPK_User = defaults.attribute("sPK_User");
    iFK_Room = defaults.attribute("DefaultRoom").toInt();
    iea_area = defaults.attribute("DefaultEA").toInt();
    iPK_User = defaults.attribute("PK_User").toInt();
    if(iPK_User == 0)
        iPK_User =1;
    setDceResponse("Defaults Set");

    //-floorplans-----------------------------------------------------------------------------------------------------
    QDomElement floorplanXml = root.firstChildElement("Floorplans");

    QDomNodeList floorplanList = floorplanXml.childNodes();
    for(int index = 0; index < floorplanList.count(); index++)
    {
        floorplans->totalPages = index;
        QString m_installation= floorplanList.at(index).attributes().namedItem("Installation").nodeValue();
        floorplans->m_installation = m_installation;
        QString m_description= floorplanList.at(index).attributes().namedItem("Description").nodeValue();
        int m_page= floorplanList.at(index).attributes().namedItem("Page").nodeValue().toInt();
        floorplans->imageBasePath = "/usr/pluto/orbiter/floorplans/inst"+m_installation+"/";
        QString m_imgPath = "/usr/pluto/orbiter/floorplans/inst"+m_installation+"/"+QString::number(m_page)+".png";
        pages.append(new FloorPlanItem(m_installation,m_description, m_page, m_imgPath, floorplans));
    }
    emit loadingMessage("floorplans complete");
    QApplication::processEvents(QEventLoop::AllEvents);

    //floorplan devices
    QDomElement floorplan_devices = root.firstChildElement("FloorplanDevices");
    QDomNodeList floorplan_device_list = floorplan_devices.childNodes();
    for(int index = 0; index < floorplan_device_list.count(); index++)
    {
        QString name = floorplan_device_list.at(index).attributes().namedItem("Name").nodeValue();
        int fp_deviceno = floorplan_device_list.at(index).attributes().namedItem("Device").nodeValue().toInt();
        QString position = floorplan_device_list.at(index).attributes().namedItem("Position").nodeValue();
        if(position.isNull())
        {
            position = "-1,-1" ;
        }

        int fp_deviceType = floorplan_device_list.at(index).attributes().namedItem("Type").nodeValue().toInt();
        int fpType = floorplan_device_list.at(index).attributes().namedItem("fpType").nodeValue().toInt();
        if (fpType == 7)
        {fpType = 6;}
        else if (fpType == 1)
        {
            fpType = 5;
        }
        QImage icon;
        floorplans->appendRow(new FloorplanDevice( name, fp_deviceno, fp_deviceType, fpType, position, icon, floorplans));
    }
    emit loadingMessage("Floorplan Devices complete");
    QApplication::processEvents(QEventLoop::AllEvents);


    //-USERS-----------------------------------------------------------------------------------------------------
    QDomElement userXml = root.firstChildElement("Users");
    userList = new UserModel( new UserItem, this);
    QDomNodeList userXmlList = userXml.childNodes();
    for(int index = 0; index < userXmlList.count(); index++)
    {
        QString m_userName = userXmlList.at(index).nodeName();      //username
        QString m_firstName= userXmlList.at(index).attributes().namedItem("FirstName").nodeValue();
        QString m_lastName= userXmlList.at(index).attributes().namedItem("LastName").nodeValue();
        QString m_nickName= userXmlList.at(index).attributes().namedItem("NickName").nodeValue();
        int m_pk_user= userXmlList.at(index).attributes().namedItem("PK_User").nodeValue().toInt();
        int m_userMode= userXmlList.at(index).attributes().namedItem("UserMode").nodeValue().toInt();
        int m_requirePin= userXmlList.at(index).attributes().namedItem("RequirePin").nodeValue().toInt();
        int m_phoneExtension= userXmlList.at(index).attributes().namedItem("PhoneExtension").nodeValue().toInt();
        int m_defaultUser= userXmlList.at(index).attributes().namedItem("isDefault").nodeValue().toInt();
        QImage m_image;
        userList->appendRow(new UserItem(m_userName,m_firstName, m_lastName, m_nickName, m_pk_user, m_userMode, m_requirePin, m_phoneExtension, m_image, m_defaultUser , userList));
    }

    //-----ROOMS-AND-ENTERTAIN-AREAS-------------------------------------------------------------------
    QDomElement roomXml = root.firstChildElement("Rooms");  //rooms
    m_lRooms = new LocationModel(new LocationItem, this);   //roomlistmodel
    QMap <QString, int> RroomMapping;                       //map for later reference
    QDomNodeList roomListXml = roomXml.childNodes();
    for(int index = 0; index < roomListXml.count(); index++)
    {
        QString m_name = roomListXml.at(index).attributes().namedItem("Description").nodeValue();
        QString ea= roomListXml.at(index).attributes().namedItem("EA").nodeValue();
        int m_val = roomListXml.at(index).attributes().namedItem("PK_Room").nodeValue().toInt();
        int m_iEA = roomListXml.at(index).attributes().namedItem("PK_EntertainArea").nodeValue().toInt();
        int m_iType = roomListXml.at(index).attributes().namedItem("FK_RoomType").nodeValue().toInt();
        if (ea.isEmpty()){
            ea = roomListXml.at(index).attributes().namedItem("Description").nodeValue().append(QString::number(m_iEA));
        }

        QUrl imagePath;

        switch (m_iType){
        case 1:
            imagePath = QUrl("../img/icons/backgrounds/livingroom.png");
            break;
        case 3:
            imagePath = QUrl("../img/icons/backgrounds/bedroom.png");
            break;
        case 5:
            imagePath = QUrl("../img/icons/backgrounds/kitchen.png");
            break;
        case 11:
            imagePath = QUrl("../img/icons/backgrounds/mstrbedroom.png");
            break;
        default:
            imagePath = QUrl("../img/lmcesplash.png");
            break;
        }
        RroomMapping.insert(m_name, m_val);
        m_lRooms->appendRow(new LocationItem(m_name, m_val, m_iEA,ea, m_iType, imagePath, m_lRooms));
    }
    m_lRooms->sdefault_Ea = defaults.attribute("DefaultLocation");
    m_lRooms->idefault_Ea = RroomMapping.value(m_lRooms->sdefault_Ea);

    setDceResponse("Room Done");

    //--LIGHTING SCENARIOS----------------------------------------------------------------------------------

    QDomElement lScenarios = root.firstChildElement("LightingScenarios");
    QDomNodeList lScenarioList = lScenarios.childNodes();

    for (int index = 0; index < lScenarioList.count(); index++)
    {
        QDomNodeList lScenarioRoom = lScenarioList.at(index).childNodes();
        LightingScenarioModel *lightModelHolder = new LightingScenarioModel(new LightingScenarioItem, this);

        int LroomMapNo = lScenarioList.at(index).attributes().namedItem("int").nodeValue().toInt();
        for (int innerIndex = 0; innerIndex < lScenarioRoom.count(); innerIndex++)
        {
            QString m_name = lScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_label = lScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_param =lScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue() ;
            QString m_command = lScenarioRoom.at(innerIndex).attributes().namedItem("eaDescription").nodeValue();
            QString m_goto = lScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue();
            QString imgName = lScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QImage m_image = QImage("Qrc:/icons/"+imgName);

            lightModelHolder->appendRow(new LightingScenarioItem(m_name,m_label, m_param, m_command, m_goto, m_image, lightModelHolder));
        }
        roomLightingScenarios.insert(LroomMapNo, lightModelHolder);
        roomLights = roomLightingScenarios.value(m_lRooms->idefault_Ea);
    }
    setDceResponse("Lighting Done");

    //---MEDIA--------------SCENARIOS----------------------------------------------------------------------------
    QDomElement mScenarios = root.firstChildElement("MediaScenarios");
    QDomNodeList mScenarioList = mScenarios.childNodes();

    for (int index = 0; index < mScenarioList.count(); index++)
    {
        QDomNodeList mScenarioRoom = mScenarioList.at(index).childNodes();
        MediaScenarioModel *mediaModelHolder = new MediaScenarioModel(new MediaScenarioItem, this);
        QString eaTitle = mScenarioList.at(index).attributes().namedItem("EAstring").nodeValue();

        int MroomMapNo = mScenarioList.at(index).attributes().namedItem("EntertainArea").nodeValue().toInt();
        for (int innerIndex = 0; innerIndex < mScenarioRoom.count(); innerIndex++)
        {
            QString m_name = mScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_label = mScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_param =mScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue() ;
            QString m_command = mScenarioRoom.at(innerIndex).attributes().namedItem("eaDescription").nodeValue();
            QString m_goto = mScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue();
            QString imgName = mScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QImage m_image = QImage("Qrc:/icons/"+imgName);
            mediaModelHolder->appendRow(new MediaScenarioItem(eaTitle,m_label, m_param, m_command, m_goto, m_image, mediaModelHolder));
        }

        roomMediaScenarios.insert(MroomMapNo, mediaModelHolder);
        roomMedia = roomMediaScenarios.value(m_lRooms->idefault_Ea);
    }

    setDceResponse("Media Done");

    //CLIMATE-----------SCENARIOS---------------------------------------------------------------------------------

    QDomElement cScenarios = root.firstChildElement("ClimateScenarios");
    QDomNodeList cScenarioList = cScenarios.childNodes();

    for (int index = 0; index < cScenarioList.count(); index++)
    {
        QDomNodeList cScenarioRoom = cScenarioList.at(index).childNodes();
        ClimateScenarioModel *climateModelHolder = new ClimateScenarioModel(new ClimateScenarioItem, this);

        int roomMapNo = cScenarioList.at(index).attributes().namedItem("int").nodeValue().toInt();
        for (int innerIndex = 0; innerIndex < cScenarioRoom.count(); innerIndex++)
        {
            QString m_name = cScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_label = cScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_param =cScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue() ;
            QString m_command = cScenarioRoom.at(innerIndex).attributes().namedItem("eaDescription").nodeValue();
            QString m_goto = cScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue();
            QString imgName = cScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QImage m_image = QImage("Qrc:/icons/"+imgName);

            climateModelHolder->appendRow(new ClimateScenarioItem(m_name,m_label, m_param, m_command, m_goto, m_image, climateModelHolder));
        }
        roomClimateScenarios.insert(roomMapNo, climateModelHolder);
        roomClimate = roomClimateScenarios.value(m_lRooms->idefault_Ea);
    }
    setDceResponse("Climate Done");

    //TELECOM------SCENARIOS-------------------------------------------------------------------------------------
    QDomElement tScenarios = root.firstChildElement("TelecomScenarios");
    QDomNodeList tScenarioList = tScenarios.childNodes();

    for (int index = 0; index < tScenarioList.count(); index++)
    {
        QDomNodeList tScenarioRoom = tScenarioList.at(index).childNodes();
        TelecomScenarioModel *telecomModelHolder = new TelecomScenarioModel(new TelecomScenarioItem, this);

        int troomMapNo = tScenarioList.at(index).attributes().namedItem("int").nodeValue().toInt();
        for (int innerIndex = 0; innerIndex < tScenarioRoom.count(); innerIndex++)
        {
            QString m_name = tScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_label = tScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_param =tScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue() ;
            QString m_command = tScenarioRoom.at(innerIndex).attributes().namedItem("eaDescription").nodeValue();
            QString m_goto = tScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue();
            QString imgName = tScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QImage m_image = QImage("Qrc:/icons/"+imgName);
            telecomModelHolder->appendRow(new TelecomScenarioItem(m_name,m_label, m_param, m_command, m_goto, m_image, telecomModelHolder));
        }
        roomTelecomScenarios.insert(troomMapNo, telecomModelHolder);
        roomTelecom = roomTelecomScenarios.value(m_lRooms->idefault_Ea);
    }
    setDceResponse("Telecom Done");
    //SECURIY---SCENARIOS-----------------------------------------------------------------------------------------
    QDomElement secScenarios = root.firstChildElement("SecurityScenarios");
    QDomNodeList secScenarioList = secScenarios.childNodes();

    for (int index = 0; index < secScenarioList.count(); index++)
    {
        QDomNodeList secScenarioRoom = secScenarioList.at(index).childNodes();
        SecurityScenarioModel *securityModelHolder = new SecurityScenarioModel(new SecurityScenarioItem, this);

        int secroomMapNo = secScenarioList.at(index).attributes().namedItem("int").nodeValue().toInt();
        for (int innerIndex = 0; innerIndex < secScenarioRoom.count(); innerIndex++)
        {
            QString m_name = secScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_label =  secScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_param =secScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue() ;
            QString m_command = secScenarioRoom.at(innerIndex).attributes().namedItem("eaDescription").nodeValue();
            QString m_goto = secScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue();
            QString imgName = secScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QImage m_image = QImage("Qrc:/icons/"+imgName);

            securityModelHolder->appendRow(new SecurityScenarioItem(m_name,m_label, m_param, m_command, m_goto, m_image, securityModelHolder));
        }
        roomSecurityScenarios.insert(secroomMapNo, securityModelHolder);
        roomSecurity = roomSecurityScenarios.value(m_lRooms->idefault_Ea);
    }
    setDceResponse("Security Done");
    QApplication::processEvents(QEventLoop::AllEvents);

    /*!
     *Various context objects are set in this function after processing is done
     */
    //------------------------------------------context objects----------------------------------------------------
    qorbiterUIwin->rootContext()->setContextObject(this);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomLights", roomLights);                 //current room scenarios model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomMedia", roomMedia);                   //current room media model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomClimate", roomClimate);               //curent room climate model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomTelecom", roomTelecom);               //curret room telecom model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomSecurity", roomSecurity);             //current room security model
    qorbiterUIwin->rootContext()->setContextProperty("current_floorplan_devices", QVariant::fromValue(current_floorplan_devices));
    qorbiterUIwin->rootContext()->setContextProperty("floorplan_devices", floorplans);                  //floorplan devices
    qorbiterUIwin->rootContext()->setContextProperty("floorplan_pages", QVariant::fromValue(pages));    //pages for floorplans
    qorbiterUIwin->rootContext()->setContextProperty("currentFloorplanType", QVariant::fromValue(i_currentFloorplanType));
    qorbiterUIwin->rootContext()->setContextProperty("currentuser", sPK_User);
    qorbiterUIwin->rootContext()->setContextProperty("iPK_Device", QVariant::fromValue(iPK_Device));  //orbiter device number
    qorbiterUIwin->rootContext()->setContextProperty("currentroom", m_lRooms->sdefault_Ea);           //custom room list item provided
    qorbiterUIwin->rootContext()->setContextProperty("userList", userList);                           //custom user list provided
    qorbiterUIwin->rootContext()->setContextProperty("roomList", m_lRooms);                           //custom room list  provided
    qorbiterUIwin->rootContext()->setContextProperty("gmediaType", q_mediaType);
    qorbiterUIwin->rootContext()->setContextProperty("screenshotAttributes", QVariant::fromValue(screenshotVars));
    qorbiterUIwin->rootContext()->setContextProperty("avcodes", QVariant::fromValue(buttonList));
    qorbiterUIwin->rootContext()->setContextProperty("device_commands", QVariant::fromValue(commandList));
    qorbiterUIwin->rootContext()->setContextProperty("currentBookmarks", QVariant::fromValue(current_bookmarks));

    setDceResponse("Properties Done");

    setDceResponse("Setting location");
    QApplication::processEvents(QEventLoop::AllEvents);
    //------------not sure if neccesary since it knows where we are.
      qDebug() << iPK_User;
    setActiveRoom(iFK_Room, iea_area);

    setCurrentUser(QString::number(iPK_User));

    //-----setting up the FILEFORMAT model------------------------------------------------------------------------
    this->qorbiterUIwin->rootContext()->setContextProperty("fileformatmodel", uiFileFilter); //custom fileformatmodel for selection filter item
    connect(uiFileFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
    connect(this, SIGNAL(resetFilter()), uiFileFilter, SLOT(clear()));

    //-----setting up the MEDIASUBTYPE model------------------------------------------------------------------------
    this->qorbiterUIwin->rootContext()->setContextProperty("mediatypefilter", mediaTypeFilter); //custom mediatype selection model
    connect(mediaTypeFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
    connect(this, SIGNAL(resetFilter()), mediaTypeFilter, SLOT(clear()));

    //-----setting up the GENRE model------------------------------------------------------------------------
    QDomElement genreElement = root.firstChildElement("GenreList");
    QDomNodeList genrelist = genreElement.childNodes();

    for(int index = 0; index < genrelist.count(); index++)
    {
        QString name = genrelist.at(index).attributes().namedItem("Name").nodeValue();
        QString pk= genrelist.at(index).attributes().namedItem("PK_Attribute").nodeValue();
        genreFilter->appendRow(new AttributeSortItem(name,pk, "",false,genreFilter));
    }
    this->qorbiterUIwin->rootContext()->setContextProperty("genrefilter", genreFilter); //custom mediatype selection model
    connect(genreFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
    QObject::connect(this, SIGNAL(resetFilter()), genreFilter, SLOT(resetStates()));

    //-----setting up the ATTRIBUTE model------------------------------------------------------------------------
    this->qorbiterUIwin->rootContext()->setContextProperty("attribfilter", attribFilter); //custom mediatype selection model
    connect(attribFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
    QObject::connect(this, SIGNAL(resetFilter()), attribFilter, SLOT(clear()) );
    binaryConfig.clear();

    //---update object image
    setDceResponse(" Remote Config Complete");

    activateScreenSaver();

    emit registerOrbiter((userList->find(sPK_User)->data(4).toInt()), QString::number(iea_area), iFK_Room );
    setOrbiterStatus(true);
}

bool qorbiterManager::OrbiterGen()
{
    return true;
}

/*!
 * \brief qorbiterManager::swapSkins: Switches UI skins
 * \param incSkin
 * This function switches the engines base path for the style to the incoming path, effectively changing the skin.
 */
void qorbiterManager::swapSkins(QString incSkin)
{
    emit skinMessage("swapping sking to::" + incSkin);
#ifdef WIN32
    incSkin = "default";
#endif
#ifdef debug
    qDebug() << tskinModel->rowCount();
#endif
    checkOrientation(qorbiterUIwin->size());
    if (tskinModel->rowCount() > 0)
    {
        emit skinMessage("Setting Skin to:" + incSkin);
        skin = tskinModel->find(incSkin);
        emit skinMessage("Got it from the model : " + skin->baseUrl().toString());
        setImagePath(skin->baseUrl().toString()+"/img/");
        //load the actual skin entry point
        currentSkin = incSkin;
        qorbiterUIwin->engine()->rootContext()->setContextProperty("style", skin->styleView());
#if (QT5)
        QObject::connect(qorbiterUIwin, SIGNAL(statusChanged(QQuickView::Status)),
                         this, SLOT(skinLoaded(QQuickView::Status)));
#else
        QObject::connect(qorbiterUIwin, SIGNAL(statusChanged(QDeclarativeView::Status)),
                         this, SLOT(skinLoaded(QDeclarativeView::Status)));
#endif
        QMetaObject::invokeMethod(this, "refreshUI", Qt::QueuedConnection, Q_ARG(QUrl, skin->entryUrl()));
    }
    else
    {
        emit skinMessage("Major skin Error!!");

        exit(0);
    }
}

#if (QT5)
void qorbiterManager::skinLoaded(QQuickView::Status status)
#else
void qorbiterManager::skinLoaded(QDeclarativeView::Status status)
#endif
{
    emit skinMessage("Skin appears to have finished loading ..");
#if (QT5)
    if (status == QQuickView::Error) {
#else
    if (status == QDeclarativeView::Error) {
#endif
        qWarning() << "Skin loading has FAILED";
        qWarning() << qorbiterUIwin->errors();
        emit skinDataLoaded(false);
        emit skinMessage("Loading has failed, falling back to failsafe!");
        swapSkins("default");

    } else {

        m_bStartingUp = false;
        QApplication::processEvents(QEventLoop::AllEvents);
        startOrbiter();
    }
}

void qorbiterManager::updateSelectedAttributes(QString attributes)
{
    qDebug() << "Updated Attributes::" << attributes.split("|");
    QStringList ta = attributes.split("|");
    setTypeSort(ta.at(6));
    setSubType(ta.at(1));
    setGridMediaType(ta.at(0));
}


//takes care of un-registering the orbiter from the DCERouter and then shutting down
void qorbiterManager::closeOrbiter()
{
    if(writeConfig()){
        setDceResponse("Shutting Down");
    }
#ifndef __ANDROID__
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Exiting, Unregistering 1st");
#endif
    //  emit unregisterOrbiter((userList->find(sPK_User)->data(4).toInt()), QString(iFK_Room), iea_area );
    emit orbiterClosing();
    this->deleteLater();
}

/*
  Requesting a datagrid of media files:
 's' is the filter string which will more than likely be refined
  'sType' refers to the grid type, a temporay hack im employing to show contextual images.
  model refers to the datamodel for the grid that is created as opposed to manually creating each cell item
  its added as a context property and the variables are availible to the ui with callback automatically registerd.
  */
bool qorbiterManager::requestDataGrid()
{
    m_dwIDataGridRequestCounter++;
    return true;
}

void qorbiterManager::setActiveRoom(int room,int ea)
{
    emit setLocation(room, ea);
    emit keepLoading(false);
    emit clearModel();
    emit resetFilter();
    nowPlayingButton->resetData();

#ifdef debug
    emit qtMessage("LocationModel::" +m_lRooms->find(QString::number(room))) ;
#endif
    //setCurrentRoom(m_lRooms->find(QString::number(room))->data(1).toString());
    m_lRooms->setLocation(ea, room);

    roomLights = roomLightingScenarios.value(room);
    roomMedia = roomMediaScenarios.value(ea);
    roomClimate = roomClimateScenarios.value(room);
    roomTelecom = roomTelecomScenarios.value(room);
    roomSecurity = roomSecurityScenarios.value(room);

    qorbiterUIwin->rootContext()->setContextProperty("currentRoomLights", roomLights);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomMedia", roomMedia);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomClimate", roomClimate);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomTelecom", roomTelecom);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomSecurity", roomSecurity);
}


void qorbiterManager::execGrp(int grp)
{
    i_current_command_grp = grp;
    qorbiterUIwin->rootContext()->setContextProperty("currentcommandgrp", i_current_command_grp);
    QApplication::processEvents(QEventLoop::AllEvents);
    emit executeCMD(grp);
}


void qorbiterManager::addMediaItem(gridItem* g)
{

}

void qorbiterManager::updateModel()
{

}

void qorbiterManager::clearMediaModel()
{


}

void qorbiterManager:: setLocation(const int &room, const int &ea)
{
    iFK_Room = room;
    iea_area = ea;
    emit locationChanged(room, ea);
    // pqOrbiter->setLocation(room, ea);
    QApplication::processEvents(QEventLoop::AllEvents);
}

int qorbiterManager::getlocation() const
{
    return iFK_Room;
}

void qorbiterManager::regenOrbiter(int deviceNo)
{
    emit screenChange("WebRegen.qml");
}

void qorbiterManager::regenComplete(int i)
{
    if (i == 0)
    {
        //processConfig(iPK_Device);
    }
    else
    {
        emit screenChange("LoadError.qml");
    }
}

QString qorbiterManager::adjustPath(const QString &path)
{

#ifdef __ANDROID__
    return path+"android";
#endif

#ifdef Q_OS_UNIX

#endif

#ifdef Q_OS_MAC
    return QCoreApplication::applicationDirPath().remove("MacOS").append("Resources");

#endif

#ifdef for_harmattan
    QString p = path;
    return p.remove("bin");
#endif

    //#else  !RPI
    //    const QString pathInShareDir = QCoreApplication::applicationDirPath()
    //            + QLatin1String("/../share/")
    //            + QFileInfo(QCoreApplication::applicationFilePath()).fileName()
    //            + QLatin1Char('/') + path;
    //    if (QFileInfo(pathInShareDir).exists())
    //        return pathInShareDir;
    //#endif
    return path;

}

void qorbiterManager::setSorting(int i)
{
    setGridMediaType(QString::number(i));
    emit gridTypeChanged(i);
}

void qorbiterManager::setNowPlayingIcon(bool b)
{
}

void qorbiterManager::nowPlayingChanged(bool b)
{
    nowPlayingButton->setStatus(b);
}

void qorbiterManager::mountMediaDevice(int d)
{

    QString mountProg = "mount";
    QStringList args;
    args << qs_routerip+":/mnt/device/"+QString::number(d) << "/mnt/remote/"+QString::number(d);
    QProcess *mountProcess = new QProcess(this);
    mountProcess->start(mountProg, args);

}

bool qorbiterManager::loadSkins(QUrl base)
{
    emit skinMessage("Local Skins path" +base.toString());
    tskinModel = new SkinDataModel(base, new SkinDataItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("skinsList", tskinModel);
    QObject::connect(tskinModel, SIGNAL(skinsFinished(bool)), this, SLOT(setSkinStatus(bool)));

    /*
      TODO ASAP:
      Write feeder function that preceeds this call to creat a simple string list
      of all availible skins for the build. a simple parsing of the skins directory
      should suffice and improper skins are dealt with later.
      */

    QStringList localSkins;
    QStringList excludes;
    excludes << "!lib" << "!startup";

#ifdef for_harmattan
    tskinModel->addSkin("default");

#elif __ANDROID__
    if(isPhone < 2){
        tskinModel->addSkin("default");
    }
    else{
        tskinModel->addSkin("default,data,wip,lustylizard");
    }
#elif for_android
    tskinModel->addSkin("default");
#elif RPI
    QDir desktopQmlPath(QString(base.toString()),"",QDir::Name, QDir::NoDotAndDotDot);
    desktopQmlPath.setNameFilters(excludes);
    setDceResponse("Skin Search Path:"+ desktopQmlPath.dirName());
    localSkins = desktopQmlPath.entryList(QDir::Dirs |QDir::NoDotAndDotDot);

    qDebug()<<"inside of skins we find" << localSkins.join(",");
    tskinModel->addSkin(localSkins.join(","));
#else
    QDir desktopQmlPath(QString(base.toString()),"",QDir::Name, QDir::NoDotAndDotDot);
    setDceResponse("Skin Search Path:"+ desktopQmlPath.dirName());
    localSkins = desktopQmlPath.entryList(QDir::Dirs |QDir::NoDotAndDotDot);

#ifdef QT_DEBUG
    qDebug()<<"inside of skins we find" << localSkins.join(",");
#endif
    tskinModel->addSkin(localSkins.join(","));
#endif
    return true;
}

void qorbiterManager::quickReload()
{
    emit reloadRouter();
    // pqOrbiter->QuickReload();
    //bool connected = pqOrbiter->m_bRouterReloading;
}

void qorbiterManager::showUI(bool b)
{

    if( b_orbiterReady && b_skinReady )
    {
        swapSkins(currentSkin);
    }
    else
    {
        setDceResponse("Orbiter Cant Show UI");
#ifdef debug
        qDebug() << "Orbiter Status:" << b_orbiterReady;
        qDebug() << "Skin Status:" << b_skinReady;
#endif
    }

}

void qorbiterManager::displayModelPages(QList<QObject *> pages)
{
    qorbiterUIwin->rootContext()->setContextProperty("pageList", QVariant::fromValue(pages));
}


void qorbiterManager::getFloorplanDevices(int floorplantype)
{
    for (int i=0; i < floorplans->rowCount(); i++)
    {
#ifdef debug
        qDebug() << floorplans->index(i, 0, QModelIndex()).data(1);
#endif

        if(floorplans->index(i).data(6).toInt() == floorplantype)
        {

            QString markerID = floorplans->index(i).data(1).toString();
            current_floorplan_devices.append(floorplans->find(markerID));
        }
    }
    //  qorbiterUIwin->rootContext()->setContextProperty("current_floorplan_devices", QVariant::fromValue(current_floorplan_devices));
}

void qorbiterManager::setFloorplanType(int t)
{
    i_currentFloorplanType = t;
    emit floorplanTypeChanged(t);
}


void qorbiterManager::qmlSetupLmce(QString incdeviceid, QString incrouterip)
{

    setDceResponse("Triggering connection to LMCE Device ID [" + incdeviceid + "] port Router Address [" + incrouterip + "]") ;

    setInternalIp(incrouterip);
    setDeviceNumber(incdeviceid.toInt());

    setDceResponse("Initializing Local Manager");
    initializeManager(incrouterip.toStdString(), incdeviceid.toLong());
}

/*!
 * \brief qorbiterManager::readLocalConfig Read the locally stored xml configuration file
 * This function reads the configuration file, locally stored and in the xml file format. Inside the configuration file
 * are various options and settings. These items are read and written to the file over the course of the application
 * depending on if the user changes user configurable items.
 *
 *\note Android requires some additional handling in that the mobile storage location must be determined first, and then file options can
 * be set. This has to occur everytime the application starts, and we hope to improve this in the future.
 *
 * \note The property 'firstrun' is checked initially. If it is true, then all most of the settings are ignored, as they
 * are defaults set other wise. In addition, this prevents over writing of command line variables.
 *
 * \return
 */
bool qorbiterManager::readLocalConfig()
{

    QDomDocument localConfig;
#ifdef Q_OS_MAC
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("MacOS").append("Resources").append("/config.xml").toStdString());
#elif __ANDROID__
    QString xmlPath;
    if(setupMobileStorage()){
        xmlPath = mobileStorageLocation+"/config.xml" ;
    }

#elif WIN32
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#elif RPI
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("/bin").toStdString())+"/config/config.xml";
#else
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#endif
    QFile localConfigFile;
    //**todo!! - add function for 1st run on android that copies the file to the xml path, then performs checks. we cannot install directly there.

    localConfigFile.setFileName(xmlPath);

#ifdef __ANDROID__
    if (createAndroidConfig())
    {

        if (!localConfigFile.exists())
        {
            setDceResponse("android config absent!");
            return false;
        }

    }
    else
    {
        return false;
    }
#endif

    if (!localConfigFile.open(QFile::ReadWrite))
    {
        setDceResponse("config not found!::"+localConfigFile.fileName());
        return false;
    }
    else
    {
        QByteArray tDoc = localConfigFile.readAll();
        localConfigFile.close();
        if (!localConfig.setContent( tDoc))
        {
            setDceResponse("Could not parse config!");
            return false;
        }
        else
        {
            QDomElement configVariables = localConfig.documentElement().toElement();

            if(configVariables.namedItem("firstrun").attributes().namedItem("id").nodeValue()=="true")
            {
#ifdef QT5
                currentSkin = "noir";
#else
                currentSkin = "default";
#endif
                return true;
            }
            else
            {
                if(!configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue().isEmpty())
                {qs_routerip = configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue();}
                else
                {qs_routerip = "192.168.80.1";}

                currentSkin = configVariables.namedItem("skin").attributes().namedItem("id").nodeValue();
                if (currentSkin.isEmpty()){
    #ifdef QT5
                    currentSkin = "noir";
    #else
                    currentSkin = "default";
    #endif
                }

                if(configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong() !=0)
                {iPK_Device = configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong();}
                else
                {iPK_Device = -1;}

                if(!configVariables.namedItem("externalip").attributes().namedItem("id").nodeValue().isEmpty() )
                {setExternalIp(configVariables.namedItem("externalip").attributes().namedItem("id").nodeValue());}
                else
                {setExternalIp("fill.me.in.com");}

                if(!configVariables.namedItem("debug").attributes().namedItem("id").nodeValue().toInt() == 0 )
                {setDebugMode(false);}
                else
                {setDebugMode(true);}

                if(!configVariables.namedItem("phone").attributes().namedItem("id").nodeValue().toInt() == 1 )
                {setFormFactor(1);}
                else
                {setFormFactor(2);}
            }
        }
        return true;
    }
}



bool qorbiterManager::writeConfig()
{

    QDomDocument localConfig;
#ifdef Q_OS_MAC
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("MacOS").append("Resources").append("/config.xml").toStdString());
#elif __ANDROID__
    QString xmlPath = mobileStorageLocation+"/config.xml";
#elif WIN32
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#else
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#endif
    QFile localConfigFile;

    localConfigFile.setFileName(xmlPath);
    if (!localConfigFile.open(QFile::ReadWrite))
    {
        setDceResponse(localConfigFile.errorString());
        //setDceResponse("Local Config is missing!");
        return false;
    }
    else
    {
        if (!localConfig.setContent( &localConfigFile))
        {
            setDceResponse("Cannot set document type!");
            return false;
        }
        else
        {
            localConfigFile.close();
            localConfigFile.remove();
            QDomElement configVariables = localConfig.documentElement().toElement();
            configVariables.namedItem("routerip").attributes().namedItem("id").setNodeValue(qs_routerip);           //internal ip
            configVariables.namedItem("routeraddress").attributes().namedItem("id").setNodeValue(internalHost);     //internal hostname
            configVariables.namedItem("skin").attributes().namedItem("id").setNodeValue(currentSkin);              //curent skin


            configVariables.namedItem("externalip").attributes().namedItem("id").setNodeValue(qs_ext_routerip);     //externalip
            configVariables.namedItem("externalHost").attributes().namedItem("id").setNodeValue(externalHost);      //external host

            configVariables.namedItem("device").attributes().namedItem("id").setNodeValue(QString::number(iPK_Device));
            configVariables.namedItem("firstrun").attributes().namedItem("id").setNodeValue(QString("false"));
            configVariables.namedItem("debug").attributes().namedItem("id").setNodeValue(debugMode ==true? "true" : "false");
            configVariables.namedItem("phone").attributes().namedItem("id").setNodeValue(QString::number(isPhone));
            configVariables.namedItem("mobile_storage").attributes().namedItem("id").setNodeValue(QString::number(isPhone));
            QByteArray output = localConfig.toByteArray();
            localConfigFile.open(QFile::ReadWrite);
            if (!localConfigFile.write(output))
            {
                localConfigFile.close();
                setDceResponse("save failed");
                return false;
            }
            localConfigFile.close();
            setDceResponse("save succeded");
            return true;
        }
    }
}

void qorbiterManager::setStringParam(int paramType, QString param)
{
    emit setDceGridParam(paramType, param);
}

void qorbiterManager::initializeSortString()
{
    goBack.clear();
    QString datagridVariableString ;
    //datagrid option variables
    //  QString q_mediaType;           //1
    q_subType="";             //2
    q_fileFormat="";          //3
    q_attribute_genres="";    //4
    q_mediaSources ="1,2";         //5 need comma delineation
    q_usersPrivate = "0";        //6
    q_attributetype_sort="";  //7
    q_pk_users="0";             //8
    q_last_viewed="2";        //9
    q_pk_attribute="";        //10
    qs_seek ="";

    datagridVariableString.append(q_mediaType).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);

    // goBack.append(datagridVariableString);
    setDceResponse("Dg Variables Reset");
}


void qorbiterManager::initializeGridModel()
{
    basicProvider = new basicImageProvider();
    qorbiterUIwin->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());
    setDceResponse("Grid Initialized");
}

void qorbiterManager::goBackGrid()
{
    //  setRequestMore(true);
    emit gridGoBack();
    //    backwards= true;

    //    if(goBack.isEmpty())
    //    {
    //        initializeSortString();
    //        emit clearAndContinue();
    //    }
    //    else
    //    {
    //        goBack.removeLast();
    //        int back = goBack.count() - 1;
    //#ifdef debug
    //        qDebug() << back;
    //        qDebug() << "Going back to::" << goBack.at(back);
    //#endif

    //        QStringList reverseParams = goBack.at(back).split("|", QString::KeepEmptyParts);
    //        q_mediaType = reverseParams.first();
    //        q_subType = reverseParams.at(1);
    //        q_fileFormat = reverseParams.at(2);
    //        q_attribute_genres = reverseParams.at(3);
    //        q_mediaSources = reverseParams.at(4);
    //        q_usersPrivate = reverseParams.at(5);
    //        q_attributetype_sort = reverseParams.at(6);
    //        q_pk_users = reverseParams.at(7);
    //        q_last_viewed = reverseParams.at(8);
    //        q_pk_attribute = reverseParams.at(9);
    //        emit clearAndContinue(i_current_mediaType);


    //datagridVariableString.append(q_mediaType).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);



}



void qorbiterManager::showFileInfo(QString fk_file)
{

    // pqOrbiter->GetMediaAttributeGrid(fk_file);
}

void qorbiterManager::requestSecurityPic(int i_pk_camera_device, int h, int w)
{
    emit getSingleCam(i_pk_camera_device,  h,  w);
}


void qorbiterManager::showfloorplan(int fptype)
{
    setFloorplanType(fptype);
    QString Screen = QString("Screen_").append(QString::number(fptype).append(".qml"));
    gotoQScreen(Screen);

    //pqOrbiter->ShowFloorPlan(fptype);
}


void qorbiterManager::updateImageChanged(QImage img)
{

}


void qorbiterManager::setScreenShotVariables(screenshotAttributes *t)
{

    qDebug("Setting thumbnail attributes to screen");

    screenshotVars.append(t);

}

void qorbiterManager::setMediaScreenShot(QImage screen_shot)
{
    mediaScreenShot = screen_shot;
    emit mediaScreenShotReady();
}

void qorbiterManager::saveScreenShot(QString attribute)
{

    emit saveMediaScreenShot(attribute);
}



void qorbiterManager::setCommandList(QList<QObject *> &l)
{
    commandList = l;
    //  qorbiterUIwin->rootContext()->setContextProperty("device_commands", QVariant::fromValue(commandList));
}

void qorbiterManager::showBookmarks(QList<QObject *> t)
{

    current_bookmarks = t;
#ifdef debug
    qDebug() << current_bookmarks.size();
#endif
    //  qorbiterUIwin->rootContext()->setContextProperty("currentBookmarks", QVariant::fromValue(current_bookmarks));
}

void qorbiterManager::setHouseMode(int mode, int pass)
{
    // pqOrbiter->SetSecurityMode(pass, mode);
}

void qorbiterManager::setCurrentUser(QString inc_user)
{
#ifdef debug
    qDebug() << "Incoming user::" << inc_user;
#endif
    sPK_User = userList->find(inc_user)->id();
    int user = inc_user.toInt();
    emit userChanged(user);
}

void qorbiterManager::setRequestMore(bool state)
{
    requestMore = state;
    emit stillLoading( requestMore);
}

bool qorbiterManager::getRequestMore()
{
    return requestMore;
}

void qorbiterManager::updateAlarm(bool toggle, int grp)
{
    sleeping_alarms->clear();
    emit setAlarm(toggle, grp);

}

void qorbiterManager::showSleepingAlarms(SleepingAlarm *s)
{
    sleeping_alarms->appendRow(s);
}


void qorbiterManager::checkConnection()
{
    setDceResponse("Disconnect detected, checking");
    emit pingTheRouter();

}



void qorbiterManager::regenError(QProcess::ProcessError)
{
    setDceResponse("Error! Reloading existing config for device " + iPK_Device);
    //processConfig(iPK_Device);
}

bool qorbiterManager::cleanupData()
{
    roomLights= NULL;                 //current room scenarios model
    roomMedia=NULL;                   //current room media model
    return true;
}

void qorbiterManager::showMessage(QString message, int duration, bool critical)
{

}

void qorbiterManager::startOrbiter()
{
    if (b_skinReady && b_orbiterReady)
    {
        setDceResponse("Showing main orbiter window");
        QApplication::processEvents(QEventLoop::AllEvents);

        m_bStartingUp = false;
        qorbiterUIwin->setWindowTitle("LinuxMCE Orbiter " + QString::number(iPK_Device));
#if (QT5)
        qorbiterUIwin->setResizeMode(QQuickView::SizeRootObjectToView);
#else
        qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif
        QApplication::processEvents(QEventLoop::AllEvents);
        emit screenChange("Screen_1.qml");
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    else
    {
        setDceResponse("This orbiter is not ready yet, please be patient");
    }


}

void qorbiterManager::processError(QString msg)
{
    emit raiseSplash();
    setDceResponse("Process Error Slot:" + msg) ;
}

void qorbiterManager::setActiveSkin(QString name)
{
#ifdef QT_DEBUG
    qDebug("Setting Skin");
#endif
    swapSkins(name);
}

void qorbiterManager::cleanupScreenie()
{
    //  mediaScreenShot = QImage();
    screenshotVars.clear();
    setDceResponse("Cleaned up Screenshot model");

}

void qorbiterManager::initializeConnections()
{
    //QObject::connect(pqOrbiter, SIGNAL(disconnected(QString)), this, SLOT(checkConnection(QString)));
    QObject::connect(this, SIGNAL(continueSetup()), this, SLOT(startSetup()));
    setDceResponse("Connections Complete");
    emit continueSetup();
}

void qorbiterManager::reloadHandler()
{
    gotoQScreen("ReloadHandler.qml");

}

void qorbiterManager::setDceResponse(QString response)
{
    dceResponse = response;
    emit loadingMessage(dceResponse);
    emit dceResponseChanged();


    qDebug() << dceResponse;


}

QString qorbiterManager::getDceResponse()
{
    return dceResponse;
}


int qorbiterManager::loadSplash()
{
    return 0;
}

void qorbiterManager::activateScreenSaver()
{

    ScreenSaver->setActive(true);

}

void qorbiterManager::killScreenSaver()
{

}
/*!
 * \brief qorbiterManager::createAndroidConfig
 *This function tries to determine the external storage location for a given android device so that it can read / write the user settings.
 * \return
 */
bool qorbiterManager::createAndroidConfig()
{

    QFile droidConfig(mobileStorageLocation+"/config.xml");
    setDceResponse(mobileStorageLocation);
    if (droidConfig.exists() && droidConfig.size() != 0)
    {
        setDceResponse("Data exists, exiting 1st run");
        return true;
    }
    else
    {
        QDir filePath;
        if(filePath.mkpath(mobileStorageLocation))
        {
            setDceResponse("Made path");
        }

        QFile defaultConfig(":main/config.xml");

        if(defaultConfig.copy(mobileStorageLocation+"/config.xml"))
        {
            setDceResponse("file copied, verifying");
            /*
            droidConfig.setPermissions(QFile::WriteOther);
            setDceResponse(droidConfig.errorString());
            droidConfig.setPermissions(QFile::ReadOther);
            setDceResponse(droidConfig.errorString());
            droidConfig.setPermissions(QFile::ExeOwner);
            setDceResponse(droidConfig.errorString());
            */
            if (droidConfig.exists() && droidConfig.size() !=0)
            {
                setDceResponse("config size: "+ QString::number(droidConfig.size()));
                return true;
            }
        }
        else
        {
            setDceResponse("Cannot install configuration!!");
            setDceResponse(droidConfig.errorString());
            return false;
        }
    }
    return false;
}
/*!
 * \brief This function is called when the application window size changes. When it does,
 * the \ref setOrientation() function is called.
 *
 */
void qorbiterManager::checkOrientation(QSize)
{
    //NOTE: Is this not handled by the window manager and Orientation change signals?
#ifndef QT5
    if(qorbiterUIwin->height() < qorbiterUIwin->width())
    {
        //setDceResponse("wide");
        appHeight = qorbiterUIwin->window()->rect().height();
        appWidth = qorbiterUIwin->window()->rect().width() ;
        setOrientation(false);
    }
    else
    {
        appHeight = qorbiterUIwin->window()->rect().height();
        appWidth = qorbiterUIwin->window()->rect().width() ;

        setOrientation( true);
    }

    qDebug() << qorbiterUIwin->window()->rect().size();
    setDceResponse("orientation change");
#else

    if(appHeight < appWidth)
    {
        setOrientation(false);
    }
    else
    {
        setOrientation( true);
    }

#endif
    qDebug() << qorbiterUIwin->size();


}

void qorbiterManager::getGrid(int i)
{
    setRequestMore(true);
    emit gridTypeChanged(i);
    emit mediaRequest(i);
}


QString qorbiterManager::getCurrentScreen()
{
    return currentScreen;
}

void qorbiterManager::setCurrentScreen(QString s)
{
    if(s!=currentScreen){
        currentScreen = s;
        emit screenChange(s);
    }

}

void qorbiterManager::connectionWatchdog()
{
    QTimer::singleShot(15000, this, SIGNAL(reInitialize()));
#ifdef debug
    emit qtMessage("Starting Watchdog");
#endif

}


void qorbiterManager::showExistingOrbiter(const QList<QObject*> l )
{
    QList<QObject*> t ;
    t = l ;
    qorbiterUIwin->rootContext()->setContextProperty("orbiterList", QVariant::fromValue(t));
}

/*!
 * \brief qorbiterManager::setupMobileStorage
 * \return
 * this function saves the mobile storage devices external storage path.
 */
bool qorbiterManager::setupMobileStorage()
{

#ifdef ANDROID
    QStringList androidStorageLocation;
    androidStorageLocation <<"/mnt/sdcard/"<< "/data/"<< "/mnt/external/"<< "/mnt/sdcard/ext_sd/" << "/mnt/sdcard-ext/"<< "/mnt/sdcard/external_sd"<< "/mnt/extSdCard/"<< "/mnt/external1/"<< "/Removable/MicroSD/";

    QDir extLocation;

    for(int i = 0; i < androidStorageLocation.count(); i++)
    {
        extLocation.setPath(androidStorageLocation.at(i));

        if(extLocation.isReadable())
        {
            setMobileStorage(androidStorageLocation.at(i)+"LinuxMCE/");
            return true;
        }
    }
    return false;
#endif

}
