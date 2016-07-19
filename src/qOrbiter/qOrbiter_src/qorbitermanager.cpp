/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    Author: Langston Ball
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
#ifdef QT5
#include <QtCore/QFile>
#include <QtQml/QtQml>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <QtXml/QDomDocument>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScriptTools/QtScriptTools>
#include <QQmlFileSelector>
#include <QTranslator>
#include <QStandardPaths>
#include <contextobjects/settinginterface.h>
#else
#include <QtDeclarative/QDeclarativeProperty>
#include <QFile>
#include <QProcess>
#include <QtDeclarative/QDeclarativeEngine>
#include <QApplication>
#include <QTimer>
#include <QtDeclarative/QtDeclarative>
#include <QtScript/QScriptValueList>
#endif

#include <contextobjects/epgchannellist.h>
#include <datamodels/skindatamodel.h>
#include <datamodels/DataGridHandler.h>

#include "qorbitermanager.h"
#include <RemoteCommands.h>



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
#if (QT5) && !defined (ANDROID)
qorbiterManager::qorbiterManager(QObject * qOrbiter_ptr, QQuickView *view, QQmlApplicationEngine *engine, int testSize, SettingInterface *appSettings, QString overridePath, bool isOsd, QObject *parent) :
    #elif ANDROID && QT5
qorbiterManager::qorbiterManager(QObject *qOrbiter_ptr, QQuickView *view, QQmlApplicationEngine *engine, AndroidSystem *jniHelper,SettingInterface *appSettings, QString overridePath, bool isOsd, QObject *parent) :
    #elif ANDROID
qorbiterManager::qorbiterManager(QObject *qOrbiter_ptr, QDeclarativeView *view, QQmlApplicationEngine *engine, AndroidSystem *jniHelper, SettingInterface *appSettings, QString overridePath, bool isOsd, QObject *parent) :
    #else
qorbiterManager::qorbiterManager(QObject *qOrbiter_ptr, QDeclarativeView *view, QQmlApplicationEngine *engine, int testSize,SettingInterface *appSettings,QString overridePath, bool isOsd, QObject *parent) :

    #endif
    QObject(parent),qorbiterUIwin(view), tskinModel(NULL),
    m_appEngine(engine),
    appHeight(view->height()),
    appWidth(view->width()),
    settingsInterface(appSettings),
    m_style(0),
    m_fontDir(""),
    iPK_Device(-1),
    gotoScreenList( new QStringList()),
    m_ipAddress("192.168.80.1"),
    mp_fileDetails(new FileDetailsClass(qOrbiter_ptr, this)),
    nowPlayingButton( new NowPlayingClass(qOrbiter_ptr, this)),
    mp_screenParameters( new ScreenParamsClass(this)),
    floorplans( new FloorPlanModel ( new FloorplanDevice, this) ),
    m_lRooms(new LocationModel(new LocationItem, this)),
    mp_securityVideo( new SecurityVideoClass (qOrbiter_ptr, this)),
    userList(new UserModel( new UserItem, this)),
    myOrbiters(new ExistingOrbiterModel(new ExistingOrbiter(), this)),
    attribFilter( new AttributeSortModel(new AttributeSortItem,6, false, this)),
    currentScreen("Screen_1.qml"),
    m_skinOverridePath(overridePath),
    m_window(NULL),
    m_dceRequestNo(1),
    m_useQueueInsteadOfInstantPlay(false), m_bIsOSD(isOsd),
    usingExternal(false),
    m_routerHelper(new RouterHelper(qOrbiter_ptr))

{

    mediaPlayerID=-1; orbiterInit=true; m_ipAddress="";  m_bStartingUp= true;  homeNetwork=false;  alreadyConfigured = false;  iFK_Room = -1;
    iea_area= -1; bAppError = false; isPhone = 0; hostDevice=HostSystemData::OTHER_EMBEDDED; appConfigPath=""; status="starting";
    setUsingExternal(false); disconnectCount=0;  reloadCount=0;
    i_current_command_grp = 0;  i_current_mediaType =0;  videoDefaultSort = "13";  audioDefaultSort = "2"; photoDefaultSort = "13";
    gamesDefaultSort = "49"; i_currentFloorplanType = 0; backwards = false;  screenSaverTimeout = 60; screenPowerOffTimeout = 60; currentIndex=0;

    iFK_Room = settingsInterface->getLastRoom();
    iea_area = settingsInterface->getLastEa();

   // qDebug() << QString("Loaded location settings Room %1 and EA %2").arg(QString::number(iFK_Room)).arg(QString::number(iea_area));

    m_mediaHelper = new DceMediaHelper(qOrbiter_ptr, m_routerHelper,this);
    connect(m_mediaHelper, &DceMediaHelper::forwardDceCommand, this, &qorbiterManager::sendDceCommand);

    uiFileFilter = new AttributeSortModel(new AttributeSortItem,2, true, this);
    mediaTypeFilter = new AttributeSortModel(new AttributeSortItem,1, false, this);
    genreFilter = new AttributeSortModel(new AttributeSortItem,3, true, this);

    m_appEngine->rootContext()->setContextProperty("manager", this);
    m_appEngine->rootContext()->setContextProperty("settings", settingsInterface);
    //appHeight=480; appWidth=640;
    currentSkin="default";
    QString path;

    if(m_skinOverridePath==""){
#ifdef NOQRC
        qDebug() << "Using NOQRC";
        path=qApp->applicationDirPath()+"/../qOrbiter_src/qml/Index.qml";
        setSkinEntryFile(qApp->applicationDirPath()+"/../qOrbiter_src/qml/skins/"+currentSkin+"/Main.qml");
        m_appEngine->setBaseUrl(QUrl::fromLocalFile(qApp->applicationDirPath()+"/../qOrbiter_src/qml/"));
#else
        qDebug() << "Using QRC";
        path="Index.qml";
        setSkinEntryFile("qrc:/qml/skins/"+currentSkin+"/Main.qml");
        m_appEngine->setBaseUrl(QUrl("qrc:/qml/"));
#endif

    } else {
        qDebug() << "Using Override";
        qDebug() << "Override Path detected :: " << m_skinOverridePath;
        path = m_skinOverridePath+"Index.qml";
        setSkinEntryFile(m_skinOverridePath+"/skins/"+currentSkin+"/Main.qml");
        m_appEngine->setBaseUrl(QUrl::fromLocalFile(m_skinOverridePath));
    }
    qDebug() << "Base URL for loading qml " << m_appEngine->baseUrl();
    qDebug() << "Initial Loading Path" << path;

    if( registerConnections(qOrbiter_ptr) ){

    }

#ifdef __ANDROID__
    int testSize=-1;
#endif
    if(settingsInterface->ready){
        if(restoreSettings()){  }
    }

#ifdef NOQRC
    m_appEngine->load(path);
#else
    m_appEngine->load(QUrl(path));
#endif

    m_testScreenSize =testSize;
    m_fontsHelper = new FontsHelper();
    QString mlocale = QLocale::system().name().append(".qm");
    if(  translator.load(":/lang/translations/"+mlocale) ) {  }
    qApp->installTranslator(&translator);
    m_screenInfo = new ScreenInfo();
    m_selector=new QFileSelector(m_appEngine);
    selector=new QQmlFileSelector(m_appEngine);


#ifdef ANDROID
    m_appEngine->addImportPath("assets:/imports/androidComponents");
    m_appEngine->addPluginPath(QDir::homePath()+"/../lib");
    m_appEngine->addPluginPath("assets:/lib");
#endif

    m_appEngine->addPluginPath("lib");
    m_appEngine->addPluginPath("imports");
    m_appEngine->addImportPath(QApplication::applicationDirPath()+"/imports");
    m_appEngine->addImportPath("lib");
    m_appEngine->addImportPath("qml");

    connect(m_screenInfo, SIGNAL(screenSizeChanged()), this, SLOT(resetScreenSize()));
    // connect(qorbiterUIwin->engine(), SIGNAL(warnings(QList<QQmlError>)), this, SLOT(handleViewError(QList<QQmlError>)));
    m_appEngine->rootContext()->setContextProperty("screenInfo", m_screenInfo);




#ifndef __ANDROID__
    b_localLoading = true; /*! this governs local vs remote loading. condensed to one line, and will be configurable from the ui soon. */
#elif defined QT5 && ANDROID || defined(ANDROID)
    androidHelper = jniHelper;
    //   qorbiterUIwin->rootContext()->setContextProperty("android",androidHelper);
    b_localLoading = true;
    if(androidHelper->updateExternalStorageLocation()){
        androidHelper->updateBuildInformation();
    }
#endif

    setDceResponse("Starting...");
    if (readLocalConfig()){
        emit localConfigReady(true);
    }else{
        emit localConfigReady(false);
    }

    setupEarlyContexts();
    setupUiSelectors();
    initializeGridModel();
    setupContextObjects();
    // Prepares models in this qt thread so owner thread is not QML as they would have been if they were created later
    prepareModelPool(5);

    /*Needs Doin at construction */

    orbiterInit=true;
    QObject::connect(&mediaFilter, SIGNAL(currentMediaTypeChanged(QString)), this, SLOT(setGridMediaType(QString)));
    QObject::connect(&mediaFilter, SIGNAL(newMediaFilter()), SLOT(updateMediaString()));
    QObject::connect(userList, SIGNAL(privateUserChanged(int)), &mediaFilter, SLOT(setPrivateUser(int)));

    //    QObject::connect(&mediaFilter, SIGNAL(attributeGenresChanged(QString)), this, SLOT(setGridAttributeGenres(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(attributeTypeSortChanged(QString)), this, SLOT(setAttributeTypeSort(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(fileFormatChanged(QString)), this, SLOT(setGridFileFormat(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(pkAttributeChanged(QString)), this, SLOT(setGridPkAttribute(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(pkUsersChanged(QString)), this, SLOT(setGridPkUsers(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(usersPrivateChanged(QString)), this, SLOT(setGridUsersPrivate(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(lastViewedChanged(QString)), this, SLOT(setGridLastViewed(QString)));

    m_appEngine->rootContext()->setContextProperty("roomList", m_lRooms);

    emit orbiterInitialized();

    if(m_appEngine->rootObjects().length()!=0){
        m_window =qobject_cast<QQuickWindow*>(engine->rootObjects().at(0));
        connect(m_window, SIGNAL(screenChanged(QScreen*)), this , SLOT(handleScreenChanged(QScreen*)));
        //  connect(m_window->screen(), SIGNAL(orientationChanged(Qt::ScreenOrientation)), this, SLOT(checkOrientation(Qt::ScreenOrientation)));
        connect(m_window, &QQuickWindow::widthChanged, [=](int w) { setAppW(w);} );
        connect(m_window, &QQuickWindow::heightChanged, [=](int h) {setAppH(h);} );

        connect(m_window->screen(), SIGNAL(primaryOrientationChanged(Qt::ScreenOrientation)), this, SLOT(checkOrientation(Qt::ScreenOrientation)));
        // connect(m_window, SIGNAL(contentOrientationChanged(Qt::ScreenOrientation)), this, SLOT(checkOrientation(Qt::ScreenOrientation)));

#if !defined(QANDROID) && !defined(Q_OS_IOS)


        if(m_testScreenSize==0){
            m_window->setVisibility(QWindow::FullScreen);
        } else{
            m_window->setVisibility(QWindow::Windowed);
            m_window->resize(appWidth, appHeight);
            m_window->show();
        }

        checkOrientation(m_window->size());
#else
        m_window->setVisibility(QWindow::FullScreen);
        appHeight = m_window->height();
        appWidth= m_window->width();
        checkOrientation(m_window->screen()->size());
#endif

    }

    emit splashReady();
}

qorbiterManager::~qorbiterManager(){
    // clear any generic datagrids
    // clearAllDataGrid();
    //  cleanupData();
}

void qorbiterManager::gotoQScreen(QString s){

    // clearAllDataGrid();

    if(s.contains("Screen_1.qml"))
    {
        logQtMessage("QOrbiter clearing models because screen is 1");
        clearDataGrid("sleepingAlarms");
        mediaFilter.setGenericOptions("");
        emit keepLoading(false);
        emit cancelRequests();
        emit resetFilter();
    }

    //send the qmlview a request to go to a screen, needs error handling
    //This allows it to execute some transition or other if it wants to


    QVariant screenname= s;

    QObject *item = qorbiterUIwin->rootObject();
    setDceResponse("About to call screenchange()");
    if (QMetaObject::invokeMethod(item, "screenchange", Qt::QueuedConnection, Q_ARG(QVariant, screenname))) {
        setDceResponse("Done call to screenchange()");
    } else {
        setDceResponse("screenchange() FAILED, sending request screen");
    }
}

//! Send the user back to the previous screen in the list
void qorbiterManager::goBacktoQScreen(){
    if(!gotoScreenList->isEmpty()){
        gotoScreenList->removeLast();
        setDceResponse("Starting backwards screen switch");
        if(gotoScreenList->isEmpty()){
            gotoScreenList->append("Screen_1.qml");
        }
        QVariant screenname= QVariant::fromValue(gotoScreenList->last());
        QObject *item = qorbiterUIwin->rootObject();
        setDceResponse("About to call screenchange() for " + gotoScreenList->last());
        setCurrentScreen(gotoScreenList->last());
        if (QMetaObject::invokeMethod(item, "screenchange", Qt::QueuedConnection, Q_ARG(QVariant, screenname))) {
            setDceResponse("Done call to backwards screenchange()");
            screenChange(currentScreen);
        } else {
            setDceResponse(" backwards screenchange() FAILED");
        }
    }
}

void qorbiterManager::addScreenToHistory(QString s)
{
    if(gotoScreenList->isEmpty()) gotoScreenList->append(s); return;

    if( gotoScreenList->last()==s) return;
    gotoScreenList->append(s);

}

/*!
 * \brief qorbiterManager::initializeManager: Initializes the QorbiterManager startup procedure.
 * \param sRouterIP
 * \param device_id
 * \return
 *This function is responsible for setting the various paths associated with the QML skins, intiating the loading of those skins
 *and reporting sucess or failure in loading those skins.
 */
bool qorbiterManager::initializeManager(string sRouterIP, int device_id){
    setDeviceNumber(device_id);
    setDceResponse("Starting Manager with connection to :"+QString::fromStdString(sRouterIP));

    if(sRouterIP.length() < 7){
        setDceResponse("Ip is empty, using alternate :"+this->m_currentRouter);
        sRouterIP = m_currentRouter.toStdString();
    }
    QObject::connect(this,SIGNAL(screenChange(QString)), this, SLOT(gotoQScreen(QString)));
    if(mb_useNetworkSkins){
        //   setupNetworkSkins(); no longer a thing.
    } else {
        // swapSkins("default");
        //  emit setSkinStatus(true);
    }
return true;
}

void qorbiterManager::initiateRestart(){
    emit restartOrbiter();
    //  screenChange("Screen_1.qml");
}

//this functions purpose is to change the UI to the new skin pointed to. It will evolve to encompass orbiter regen to some extent
/*!
 * \brief qorbiterManager::refreshUI: This function re-initializes the qml engines skin.
 * \param url - The relative or absolute path of the skin to the binary.
 */
void qorbiterManager::refreshUI(QUrl url){
    Q_UNUSED(url);
    emit currentSkinChanged();
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
void qorbiterManager::processConfig(QNetworkReply *config)
{

    if(!alreadyConfigured){
        m_dwIDataGridRequestCounter = 0;
        iSize = 0;
        m_pOrbiterCat = 5;
        s_onOFF = "1";
    }


    QDomDocument configData;//(config->readAll());
    QByteArray tConf= config->readAll().replace("\n", "");
    configData.setContent(tConf,false);
    if(configData.isDocument() == false)
    {
        setDceResponse("Invalid config for device: " + QString::number(iPK_Device));
        setDceResponse("Please run http://dcerouter/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device)) ;
        setDceResponse("Invalid Config");
        emit orbiterConfigReady(false);

        return;
    }

    setDceResponse("Attempting to write config");
    if (!writeConfig()){
        setDceResponse("Couldnt save config!");
    }

    QDomElement root = configData.documentElement();        //represent configuration in memeory


    //------------DEFAULTS-FOR-ORBITER------------------------------------------------------------
    QDomElement defaults = root.firstChildElement("Default");
    QString sPK_User = defaults.attribute("sPK_User");
    if(!alreadyConfigured){

//        if(iFK_Room == -1)
//        iFK_Room = defaults.attribute("DefaultRoom").toInt();

//        if(iea_area == -1)
//        iea_area = defaults.attribute("DefaultEA").toInt();

        iPK_User = defaults.attribute("PK_User").toInt();
        if(iPK_User == 0)
            iPK_User =1;
        setDceResponse("Defaults Set");
    }
    //-floorplans-----------------------------------------------------------------------------------------------------
    QDomElement floorplanXml = root.firstChildElement("Floorplans");

    QDomNodeList floorplanList = floorplanXml.childNodes();
    pages.clear();
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
    floorplans->clear();
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
        int room = floorplan_device_list.at(index).attributes().namedItem("Room").nodeValue().toInt();
        /*        if (fpType == 7)
            fpType = 6;
        else */

        if (fpType == 4)  fpType = 1;// Move cameras to the security floorplan

        floorplans->appendRow(new FloorplanDevice( name,
                                                   fp_deviceno,
                                                   fp_deviceType,
                                                   fpType,
                                                   position,
                                                   room,
                                                   floorplans)
                              );
    }
    emit loadingMessage("Floorplan Devices complete");
    QApplication::processEvents(QEventLoop::AllEvents);


    //-USERS-----------------------------------------------------------------------------------------------------
    QDomElement userXml = root.firstChildElement("Users");

    QDomNodeList userXmlList = userXml.childNodes();
    userList->clear();
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

    QMap <QString, int> RroomMapping;                       //map for later reference
    QDomNodeList roomListXml = roomXml.childNodes();
    m_lRooms->clear();
    for(int index = 0; index < roomListXml.count(); index++)
    {
        QString m_name = roomListXml.at(index).attributes().namedItem("Description").nodeValue();
        QString ea= roomListXml.at(index).attributes().namedItem("EA").nodeValue();
        int m_val = roomListXml.at(index).attributes().namedItem("PK_Room").nodeValue().toInt();
        int m_iEA = roomListXml.at(index).attributes().namedItem("PK_EntertainArea").nodeValue().toInt();
        int m_iType = roomListXml.at(index).attributes().namedItem("FK_RoomType").nodeValue().toInt();
        bool m_isHidden = roomListXml.at(index).attributes().namedItem("HideFromOrbiter").nodeValue().toInt();

        if (ea.isEmpty()){
            ea = roomListXml.at(index).attributes().namedItem("Description").nodeValue().append(QString::number(m_iEA));
        } else {
            m_lRooms->addTimeCodeTrack(ea, m_iEA, m_val);
        }

        RroomMapping.insert(m_name, m_val);
        QUrl imgFile;
        if(m_lRooms->check(m_val)){
            LocationItem *t= m_lRooms->find(m_name);
            t->addEa(ea, m_iEA);

        }
        else{
            m_lRooms->appendRow(new LocationItem(m_name, m_val, m_iType, imgFile, m_isHidden, m_lRooms));
            LocationItem *t= m_lRooms->find(m_name);
            t->addEa(ea, m_iEA);


        }
    }
    m_lRooms->sdefault_Ea = iea_area;
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
            QImage m_image = QImage("qrc:/icons/"+imgName);

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
            QImage m_image = QImage("qrc:/icons/"+imgName);
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
    // qorbiterUIwin->rootContext()->setContextProperty("lmceData", &linuxmceData);                 //current room scenarios model
    if(!alreadyConfigured){
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


        m_appEngine->rootContext()->setContextProperty("currentRoomLights", roomLights);                 //current room scenarios model
        m_appEngine->rootContext()->setContextProperty("currentRoomMedia", roomMedia);                   //current room media model
        m_appEngine->rootContext()->setContextProperty("currentRoomClimate", roomClimate);               //curent room climate model
        m_appEngine->rootContext()->setContextProperty("currentRoomTelecom", roomTelecom);               //curret room telecom model
        m_appEngine->rootContext()->setContextProperty("currentRoomSecurity", roomSecurity);             //current room security model
        m_appEngine->rootContext()->setContextProperty("current_floorplan_devices", QVariant::fromValue(current_floorplan_devices));
        m_appEngine->rootContext()->setContextProperty("floorplan_devices", floorplans);                  //floorplan devices
        m_appEngine->rootContext()->setContextProperty("floorplan_pages", QVariant::fromValue(pages));    //pages for floorplans
        m_appEngine->rootContext()->setContextProperty("currentFloorplanType", QVariant::fromValue(i_currentFloorplanType));
        m_appEngine->rootContext()->setContextProperty("currentuser", sPK_User);
        m_appEngine->rootContext()->setContextProperty("iPK_Device", QVariant::fromValue(iPK_Device));  //orbiter device number
        m_appEngine->rootContext()->setContextProperty("currentroom", m_lRooms->sdefault_Ea);           //custom room list item provided
        m_appEngine->rootContext()->setContextProperty("userList", userList);                           //custom user list provided
        m_appEngine->rootContext()->setContextProperty("roomList", m_lRooms);                           //custom room list  provided
        m_appEngine->rootContext()->setContextProperty("gmediaType", q_mediaType);
        m_appEngine->rootContext()->setContextProperty("screenshotAttributes", QVariant::fromValue(screenshotVars));
        m_appEngine->rootContext()->setContextProperty("avcodes", QVariant::fromValue(buttonList));
        m_appEngine->rootContext()->setContextProperty("device_commands", QVariant::fromValue(commandList));
        m_appEngine->rootContext()->setContextProperty("currentBookmarks", QVariant::fromValue(current_bookmarks));
    }

    //-----setting up the FILEFORMAT model------------------------------------------------------------------------

    if(!alreadyConfigured){
        this->qorbiterUIwin->rootContext()->setContextProperty("fileformatmodel", uiFileFilter); //custom fileformatmodel for selection filter item
        m_appEngine->rootContext()->setContextProperty("fileformatmodel", uiFileFilter );

        connect(uiFileFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
        connect(this, SIGNAL(resetFilter()), uiFileFilter, SLOT(clear()));

        //-----setting up the MEDIASUBTYPE model------------------------------------------------------------------------
        this->qorbiterUIwin->rootContext()->setContextProperty("mediatypefilter", mediaTypeFilter); //custom mediatype selection model
        m_appEngine->rootContext()->setContextProperty("mediatypefilter", mediaTypeFilter);
        connect(mediaTypeFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
        connect(this, SIGNAL(resetFilter()), mediaTypeFilter, SLOT(clear()));
    }

    //-----setting up the GENRE model------------------------------------------------------------------------
    QDomElement genreElement = root.firstChildElement("GenreList");
    QDomNodeList genrelist = genreElement.childNodes();

    for(int index = 0; index < genrelist.count(); index++)
    {
        QString name = genrelist.at(index).attributes().namedItem("Name").nodeValue();
        QString pk= genrelist.at(index).attributes().namedItem("PK_Attribute").nodeValue();
        genreFilter->appendRow(new AttributeSortItem(name,pk, "",false,genreFilter));
    }

    if(!alreadyConfigured){

        this->qorbiterUIwin->rootContext()->setContextProperty("genrefilter", genreFilter); //custom mediatype selection model
        m_appEngine->rootContext()->setContextProperty("genrefilter", genreFilter);

        connect(genreFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
        QObject::connect(this, SIGNAL(resetFilter()), genreFilter, SLOT(resetStates()));

        //-----setting up the ATTRIBUTE model------------------------------------------------------------------------
        this->qorbiterUIwin->rootContext()->setContextProperty("attribfilter", attribFilter); //custom mediatype selection model
        m_appEngine->rootContext()->setContextProperty("attribfilter", attribFilter);

        connect(attribFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
        QObject::connect(this, SIGNAL(resetFilter()), attribFilter, SLOT(clear()) );
    }

    binaryConfig.clear();
    tConf.clear();
    configData.clear();
    //---update object image
    getMediaDevices();
    setDceResponse(" Remote Config Complete");
    emit registerOrbiter((userList->find(sPK_User)->data(4).toInt()), QString::number(iea_area), iFK_Room );

    if(!alreadyConfigured && mb_useNetworkSkins){
        setOrbiterStatus(true);
    }

    alreadyConfigured=true;
    setDceResponse("Properties Done");
    setDceResponse("Setting location");
     setActiveRoom(iFK_Room , iea_area);
    setCurrentUser(QString::number(iPK_User));
    beginSetup();

}

void qorbiterManager::getConfiguration()
{
    QNetworkRequest updateDevice;
    QNetworkAccessManager *ud= new QNetworkAccessManager();
    updateDevice.setUrl("http://"+m_currentRouter+"/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device ));
    QObject::connect(ud, SIGNAL(finished(QNetworkReply*)), this, SLOT(processConfig(QNetworkReply*)));
    ud->get(updateDevice);
    // DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Connected, requesting configuration for device %d", m_dwPK_Device);
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

    emit skinMessage("swapping skin to::" + incSkin);

    if(currentSkin==incSkin) return;
    currentSkin= incSkin;
    emit currentSkinChanged();
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
        writeConfig();
        startOrbiter();
    }
}

void qorbiterManager::updateSelectedAttributes(QString attributes)
{

    QStringList ta = attributes.split("|");
    setSubType(ta.at(1));
    setGridMediaType(ta.at(0));
    setGridFileFormat(ta.at(2));
    setGridPkUsers(ta.at(7));
    setGridMediaSources(ta.at(4));
    setGridPkAttribute(ta.at(9));
    setGridLastViewed(ta.at(8));
    setGridAttributeGenres(ta.at(3));
    setGridUsersPrivate(QString::number(userList->currentPrivateUser));
    setGridAttributeTypeSort(ta.at(6));
}


//takes care of un-registering the orbiter from the DCERouter and then shutting down
void qorbiterManager::closeOrbiter()
{  
    if(writeConfig()){
        setDceResponse("Shutting Down");
    }
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Exiting, Unregistering 1st");
    // emit unregisterOrbiter((userList->find(sPK_User)->data(4).toInt()), QString(iFK_Room), iea_area );

    m_window->close();
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

void qorbiterManager::prepareDataGrid(QString dataGridId, QString dgName, int height, int width)
{
    modelPoolLock.lockForRead();
    if (m_mapDataGridModels.contains(dataGridId))
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "prepareDataGrid, clearing model");
        m_mapDataGridModels[dataGridId]->clear();
        m_mapDataGridModels[dataGridId]->setDgName(dgName);
        m_mapDataGridModels[dataGridId]->setWindowSize(media_pageSeperator);
        m_mapDataGridModels[dataGridId]->setTotalCols(width);
        // set total row count last, as this will make the model look populated to the view

        m_mapDataGridModels[dataGridId]->setTotalRows(height);
    }
    modelPoolLock.unlock();

}

//not really adding an item so much as parsing an entire grid, slowing down GUI
void qorbiterManager::addDataGridItem(QString dataGridId, int PK_DataGrid, int indexStart, int numRows, DataGridTable* pTable)
{

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "addDataGridItem() start");
    // Make sure datagrid is still in the active map
    modelPoolLock.lockForRead();
    if (m_mapDataGridModels.contains(dataGridId))
    {
        GenericFlatListModel* pModel = m_mapDataGridModels[dataGridId];
        if (pModel->isSeeking())
        {
            // when seeking, just indicate the row to search to, and the model will request a window containing that row
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "addDataGridItem() seeking, request start = %d, end = %d", indexStart, indexStart+numRows-1);
            pModel->seekResult(indexStart);
        } else {
            // If the request is backwards, we need to insert in reverse order
            int direction = pModel->isForwardRequest() ? 1 : -1;
            int start = pModel->isForwardRequest() ? indexStart : indexStart+numRows-1;
            int count = 0;
            for (int row = start; count < numRows; row = row + direction)
            {
                QVector<GenericModelItem*> items(m_mapDataGridModels[dataGridId]->getTotalColumns());
                for (int col = 0; col < m_mapDataGridModels[dataGridId]->getTotalColumns(); col++)
                {
                    items[col] = DataGridHandler::GetModelItemForCell(PK_DataGrid, pTable, row, col);
                    // TODO: stop if datagrid model is reset or request stopped
                }
                m_mapDataGridModels[dataGridId]->insertRow(row, items);
                count++;
            }
        }

    }
    modelPoolLock.unlock();
    pTable->ClearData();
    delete pTable;
    emit modelChanged();
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "addDataGridItem() end");
}

void qorbiterManager::updateItemData(QString dataGridId, int row, int role, QVariant value)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "updateItemData() start");

    modelPoolLock.lockForRead();
    if (m_mapDataGridModels.contains(dataGridId))
    {
        GenericFlatListModel* pModel = m_mapDataGridModels.value(dataGridId);

        pModel->updateItemData(row, role, value);
    } else {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "updateItemData() no such datagridid %s", dataGridId.toStdString().c_str());
    }
    modelPoolLock.unlock();
    emit modelChanged();
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "updateItemData() end");
}

void qorbiterManager::clearDataGrid(QString dataGridId)
{
    if(!modelPoolLock.tryLockForWrite()){
        return;
    }

    if (m_mapDataGridModels.contains(dataGridId))
    {
        GenericFlatListModel* pModel = m_mapDataGridModels.take(dataGridId);
        // TODO: stop any requests on this datagrid model
        if(!pModel->clearing){

            pModel->clear();
            pModel->setModelName("");
            pModel->setOption("");
            m_modelPool.push(pModel);
        }


    }
    modelPoolLock.unlock();
    emit modelChanged();
}

void qorbiterManager::clearAllDataGrid()
{

    modelPoolLock.lockForWrite();
    foreach (QString id, m_mapDataGridModels.keys())
    {
        GenericFlatListModel* pModel = m_mapDataGridModels.take(id);
        pModel->clear();
        pModel->setModelName("");
        pModel->setOption("");
        m_modelPool.push(pModel);
    }
    modelPoolLock.unlock();

}

void qorbiterManager::prepareModelPool(int poolSize)
{
    // We must create the model here, so that QML will get a valid model returned from this method
    // because this method will probably be called first (at property binding) from QML (before loadDataGrid)

    for (int i = 0; i < poolSize; i++)
    {
        GenericFlatListModel* pModel = new GenericFlatListModel(this);
        QObject::connect(pModel,SIGNAL(rowsRemoved(QModelIndex,int,int)), pModel, SLOT(removeComplete()),Qt::QueuedConnection);
        m_modelPool.push(pModel);
    }
}


GenericFlatListModel* qorbiterManager::getDataGridModel(QString dataGridId, int PK_DataGrid, QString initOption)

{
    // qDebug() << "Fetching dg " << dataGridId << ": type : " << PK_DataGrid;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "getDataGridModel() id = %s", dataGridId.toStdString().c_str());
    GenericFlatListModel* pModel = NULL;
    // qDebug() <<" Searching for model, id ::" << dataGridId;
    if (!m_mapDataGridModels.contains(dataGridId))
    {
        // GenericFlatListModels are kept in a pool in qorbitermanager, because we want the
        // Objects to belong to this thread, and not the QML thread, which is the one that calls
        // this method. Creating them in this method would create them in the QML thread.
        // lock this section so we don't prepare the same model more than once

        // qDebug() <<dataGridId << " doesnt exist creating new one.";

        if(!modelPoolLock.tryLockForWrite()){
            //  qDebug() << "Mutex locked, comeback later.";
            return pModel;
        }
        // Check that the model still is not in map
        if (!m_mapDataGridModels.contains(dataGridId))
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "getDataGridModel() preparing GenericFlatListModel");
            pModel = m_modelPool.pop();
            pModel->clear();
            pModel->setPrototype(DataGridHandler::GetModelItemType(PK_DataGrid));
            pModel->setModelName(dataGridId);
            pModel->setOption(initOption);
            pModel->setPK_DataGrid(PK_DataGrid);
            m_mapDataGridModels.insert(dataGridId, pModel);

            if(!currentIndexMap.isEmpty()){
                currentIndexMap.clear();
            }

            QString option = initOption == NULL ? "" : initOption;
            switch(PK_DataGrid){


            case DATAGRID_Phone_Book_Auto_Compl_CONST:
                option = QString::number(iPK_User).append("|%").arg(initOption);
                break;

            case DATAGRID_Media_Browser_CONST:
                mediaFilter.setDataGridId(dataGridId);
                option = mediaFilter.getFilterString();
                break;

            case DATAGRID_Playlists_CONST:
            case DATAGRID_Current_Media_Sections_CONST:
                option="38";
                break;

            case DATAGRID_EPG_All_Shows_CONST:
            case DATAGRID_EPG_Current_Shows_CONST:
                option = QString::number(iPK_User)+","+QString::number(this->iea_area);
                break;


            default:
                setMediaResponse("qOrbiterManager::getDataGridModel()::No Grid option set");
                option =initOption;
                break;
            }

            pModel->setOption(option);

            LoggerWrapper::GetInstance()->Write(LV_DEBUG, "getDataGridModel() emit loadDataGrid  ");
            // qDebug() << "Checks complete, fetching grid " << dataGridId ;
            emit loadDataGrid(dataGridId, PK_DataGrid, option); // loads data
        } else {
            pModel = m_mapDataGridModels[dataGridId];
        }

        modelPoolLock.unlock();

    } else {
        pModel = m_mapDataGridModels[dataGridId];
        // qDebug() << "Model found with " << pModel->rowCount() << "row, returning;";
    }

    return pModel;
}

GenericFlatListModel* qorbiterManager::getDataGridModel(QString dataGridId, int PK_DataGrid)
{
    return getDataGridModel(dataGridId, PK_DataGrid, "");
}

void qorbiterManager::seekGrid(QString dataGridId, QString s)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager.seekGrid() id = %s, seek = %s", dataGridId.toStdString().c_str(), s.toStdString().c_str());
    if (m_mapDataGridModels.contains(dataGridId))
    {
        GenericFlatListModel* pModel = m_mapDataGridModels[dataGridId];
        pModel->seek(s);
    }
}

void qorbiterManager::mediaFilterChanged(QString dataGridId)
{
    if(currentScreen=="Screen_1.qml"){
        clearDataGrid(dataGridId);
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Screen_1, ignoring mediaFilterChanged(), clearing model ");
        return;
    }
    qDebug() << "mediaFilterChanged() for " << dataGridId;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::mediaFilterChanged() start");
    GenericFlatListModel* pModel = NULL;
    if (m_mapDataGridModels.contains(dataGridId))
    {
        qDebug() << "mediaFilterChanged()::" << "existing model type, checking filter of " << mediaFilter.getFilterString();
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::mediaFilterChanged() model exists");
        pModel = m_mapDataGridModels[dataGridId];
        pModel->setOption(mediaFilter.getFilterString());
        pModel->refreshData();
    }
    updateSelectedAttributes(mediaFilter.getFilterString());
}

void qorbiterManager::genericFilterChanged(QString dataGridId)
{


    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::genericFilterChanged() start");
    GenericFlatListModel* pModel = NULL;
    if (m_mapDataGridModels.contains(dataGridId))
    {
        // qDebug() << "genericFilterChanged()::" << "existing model type, checking filter of " << mediaFilter.getGenericOptions();
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::genericFilterChanged() model exists");
        pModel = m_mapDataGridModels[dataGridId];
        //      pModel->setOption(mediaFilter.getGenericOptions());
        pModel->refreshData();
    }

}

QString qorbiterManager::translateMediaType(int mediaType)
{
    QString rVal = "";
    switch(mediaType){
    case 1:
        rVal="LinuxMCE LiveTV";
        break;
    case 2:
        rVal="LinuxMCE CD";
        break;
    case 3:
        rVal="LinuxMCE DVD";
        break;
    case 4:
        rVal="LinuxMCE Stored Audio";
        break;
    case 5:
        rVal="LinuxMCE Stored Video";
        break;
    case 6:
        rVal="LinuxMCE Live Radio";
        break;
    case 7:
        rVal="LinuxMCE Pictures";
        break;
    case 11:
        rVal="LiveTv";
        break;
    case 12:
        rVal="DVD";
        break;
    case 13:
        rVal="OTA-Radio";
        break;
    case 14:
        rVal="Satellite Radio";
        break;
    case 15:
        rVal="Cable Raio";
        break;
    case 16:
        rVal="VideoTape";
        break;
    case 17:
        rVal="LaserDisc";
        break;
    case 18:
        rVal="Game";
        break;
    case 19:
        rVal="CD";
        break;
    case 20:
        rVal="Burning";
        break;
    case 21:
        rVal="Playlist";
        break;
    case 22:
        rVal="Edit Playlist";
        break;
    case 23:
        rVal="Blank Media";
        break;
    case 24:
        rVal="Doc Viewer";
        break;
    case 25:
        rVal="Cassette";
        break;
    case 26:
        rVal="LinuxMCE CD";
        break;
    case 27:
        rVal="LinuxMCE HD-DVD";
        break;
    case 28:
        rVal="LinuxMCE Blu-ray Disc";
        break;
    case 29:
        rVal="LinuxMCE Game";
        break;
    case 30:
        rVal="Atari 2600";
        break;
    case 31:
        rVal="Blu-Ray";
        break;
    case 32:
        rVal="LinuxMCE Atari 5200";
        break;
    case 33:
        rVal="LinuxMCE Atari 7800";
        break;
    case 63:
        rVal="YouTube";
        break;
    case 64:
        rVal="Internet Media";
        break;
    default:
        rVal = "Unknown.";
        break;
    }
    return rVal;
}

void qorbiterManager::setActiveRoom(int room,int ea)
{
    emit setLocation(room, ea);
    emit keepLoading(false);
    clearDataGrid("Playlist");
    emit resetFilter();
    nowPlayingButton->resetData();

#ifdef debug
    emit qtMessage("LocationModel::" +m_lRooms->find(QString::number(room))) ;
#endif
    //setCurrentRoom(m_lRooms->find(QString::number(room))->data(1).toString());


    roomLights = roomLightingScenarios.value(room);
    roomMedia = roomMediaScenarios.value(ea);
    roomClimate = roomClimateScenarios.value(room);
    roomTelecom = roomTelecomScenarios.value(room);
    roomSecurity = roomSecurityScenarios.value(room);

    m_appEngine->rootContext()->setContextProperty("currentRoomLights", roomLights);
    m_appEngine->rootContext()->setContextProperty("currentRoomMedia", roomMedia);
    m_appEngine->rootContext()->setContextProperty("currentRoomClimate", roomClimate);
    m_appEngine->rootContext()->setContextProperty("currentRoomTelecom", roomTelecom);
    m_appEngine->rootContext()->setContextProperty("currentRoomSecurity", roomSecurity);

  m_lRooms->setLocation(ea, room);
}


void qorbiterManager::execGrp(int grp)
{
    i_current_command_grp = grp;
    QApplication::processEvents(QEventLoop::AllEvents);
    emit executeCMD(grp);
    qorbiterUIwin->rootContext()->setContextProperty("currentcommandgrp", i_current_command_grp);
    m_appEngine->rootContext()->setContextProperty("currentcommandgrp", i_current_command_grp);
}



void qorbiterManager:: setLocation(const int &room, const int &ea)
{

    if(room == -1 && ea == -1)
        return;

    iFK_Room = room;
    iea_area = ea;

    if(room != -1 && ea!=-1){
          settingsInterface->setLastLocation(room, ea);
    }

    sEntertainArea = QString::number(iea_area).toStdString();
    emit locationChanged(room, ea);
    // pqOrbiter->setLocation(room, ea);
    QApplication::processEvents(QEventLoop::AllEvents);
}

int qorbiterManager::getlocation() const
{
    return iFK_Room;
}

void qorbiterManager::regenOrbiter()
{
    setCurrentScreen("Screen_Reload.qml");

    QNetworkRequest updateDevice;
    QNetworkAccessManager *ud= new QNetworkAccessManager();
    updateDevice.setUrl("http://"+m_currentRouter+"/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device ));
    QObject::connect(ud, SIGNAL(finished(QNetworkReply*)), this, SLOT(regenComplete(QNetworkReply*)));
    ud->get(updateDevice);

}

void qorbiterManager::regenComplete(QNetworkReply*r)
{

    if(!alreadyConfigured){
        m_dwIDataGridRequestCounter = 0;
        iSize = 0;
        m_pOrbiterCat = 5;
        s_onOFF = "1";
    }


    QDomDocument configData;//(config->readAll());
    QByteArray tConf= r->readAll().replace("\n", "");
    configData.setContent(tConf,false);
    if(configData.isDocument() == false)
    {
        return;
    }

    setDceResponse("Attempting to write config");


    QDomElement root = configData.documentElement();        //represent configuration in memeory


    //------------DEFAULTS-FOR-ORBITER------------------------------------------------------------
    QDomElement defaults = root.firstChildElement("Default");
    QString sPK_User = defaults.attribute("sPK_User");
    if(!alreadyConfigured){
        iFK_Room = defaults.attribute("DefaultRoom").toInt();
        iea_area = defaults.attribute("DefaultEA").toInt();
        iPK_User = defaults.attribute("PK_User").toInt();
        if(iPK_User == 0)
            iPK_User =1;
        setDceResponse("Defaults Set");
    }
    //-floorplans-----------------------------------------------------------------------------------------------------
    QDomElement floorplanXml = root.firstChildElement("Floorplans");
    QDomNodeList floorplanList = floorplanXml.childNodes();
    pages.clear();
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
    floorplans->clear();
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
        int room = floorplan_device_list.at(index).attributes().namedItem("Room").nodeValue().toInt();
        /*        if (fpType == 7)
            fpType = 6;
        else */

        if (fpType == 4)  fpType = 1;// Move cameras to the security floorplan

        floorplans->appendRow(new FloorplanDevice( name,
                                                   fp_deviceno,
                                                   fp_deviceType,
                                                   fpType,
                                                   position,
                                                   room,
                                                   floorplans)
                              );
    }
    emit loadingMessage("Floorplan Devices complete");
    QApplication::processEvents(QEventLoop::AllEvents);
    //-USERS-----------------------------------------------------------------------------------------------------
    QDomElement userXml = root.firstChildElement("Users");
    QDomNodeList userXmlList = userXml.childNodes();
    userList->clear();
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
    QMap <QString, int> RroomMapping;                       //map for later reference
    QDomNodeList roomListXml = roomXml.childNodes();
    m_lRooms->clear();
    for(int index = 0; index < roomListXml.count(); index++)
    {
        QString m_name = roomListXml.at(index).attributes().namedItem("Description").nodeValue();
        QString ea= roomListXml.at(index).attributes().namedItem("EA").nodeValue();
        int m_val = roomListXml.at(index).attributes().namedItem("PK_Room").nodeValue().toInt();
        int m_iEA = roomListXml.at(index).attributes().namedItem("PK_EntertainArea").nodeValue().toInt();
        int m_iType = roomListXml.at(index).attributes().namedItem("FK_RoomType").nodeValue().toInt();
        bool m_isHidden = roomListXml.at(index).attributes().namedItem("HideFromOrbiter").nodeValue().toInt();

        if (ea.isEmpty()){
            ea = roomListXml.at(index).attributes().namedItem("Description").nodeValue().append(QString::number(m_iEA));
        } else {
            m_lRooms->addTimeCodeTrack(ea, m_iEA, m_val);
        }

        RroomMapping.insert(m_name, m_val);
        QUrl imgFile;
        if(m_lRooms->check(m_val)){
            LocationItem *t= m_lRooms->find(m_name);
            t->addEa(ea, m_iEA);

        }
        else{
            m_lRooms->appendRow(new LocationItem(m_name, m_val, m_iType, imgFile, m_isHidden, m_lRooms));
            LocationItem *t= m_lRooms->find(m_name);
            t->addEa(ea, m_iEA);
        }
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
            QImage m_image = QImage("qrc:/icons/"+imgName);

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
            QImage m_image = QImage("qrc:/icons/"+imgName);
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
    binaryConfig.clear();
    tConf.clear();
    configData.clear();
    //---update object image
    getMediaDevices();
    setDceResponse(" Remote Config Complete");

    if(!alreadyConfigured && mb_useNetworkSkins){
        setOrbiterStatus(true);
    }

    alreadyConfigured=true;

    emit configurationChanged();
    setActiveRoom(iFK_Room , iea_area);
    emit registerOrbiter((userList->find(sPK_User)->data(4).toInt()), QString::number(iea_area), iFK_Room );
    delayedReloadQml();
}

QString qorbiterManager::adjustPath(const QString &path)
{
#ifdef __ANDROID__
    return path+"android";
#elif defined(Q_OS_UNIX) && !defined(Q_OS_IOS)

#elif defined(Q_OS_MAC) && !defined(Q_OS_IOS)
    return QCoreApplication::applicationDirPath().remove("MacOS").append("Resources");

#elif for_harmattan
    QString p = path;
    return p.remove("bin");

#elif defined(Q_OS_IOS)
    return "";

#endif

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




void qorbiterManager::getMediaDevices()
{

    QNetworkAccessManager * m = new QNetworkAccessManager();
    QNetworkRequest req;
    req.setUrl(QUrl("http://"+m_currentRouter+"/lmce-admin/qOrbiterGenerator.php?m"));
    m->get(req);
    QObject::connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(setMediaDevices(QNetworkReply*)));
}

void qorbiterManager::setMediaDevices(QNetworkReply *d)
{
    QString str= d->readAll().replace("\n", "");
    QScriptEngine script;
    QVariantList p = script.evaluate(str).toVariant().toList();
    storageDevices = p;
}

void qorbiterManager::playMedia(QString FK_Media, bool queue =false) {
    if(FK_Media == "!G"){
        FK_Media.append(QString::number(iPK_Device));
    }

    bool q;

    if(queue){
        q = queue;
    } else {
        if(!queue && m_useQueueInsteadOfInstantPlay ){
            q = queue;
        } else {
            q = m_useQueueInsteadOfInstantPlay;
        }
    }

    //changed to remove media type as that is decided on by the media plugin and passed back
    CMD_MH_Play_Media cmd(
                iPK_Device,                 //device from
                m_routerHelper->mediaPluginId(),             //device to
                0 ,
                FK_Media.toStdString(),     //media file name !FXX or !PXXX
                0,                          //media type
                0,                          //device template (why 0)
                sEntertainArea,             //entertain area
                false,                      //resume
                false,                       //repeat
                q,                          //queue
                false,                      //bypass event
                false                       //dont setupav
                );
    emit sendDceCommand(cmd);
}

void qorbiterManager::playMediaFromDrive(int device, int disc, int ea)
{
    QString fkFile = QString("!r" + QString::number(disc) + ":" + QString::number(device));

    CMD_MH_Play_Media playDisc(
                iPK_Device,
                m_routerHelper->mediaPluginId() ,
                0,
                fkFile.toStdString(), // see MediaAttributes_LowLevel::TransformFilenameToDeque
                0,
                0,
                sEntertainArea,
                false,
                0,
                0 /* bQueue */,
                0 /* bBypass_Event */,
                0 /* bDont_Setup_AV */
                );

    emit sendDceCommand(playDisc);
}

void qorbiterManager::mythTvPlay(){
    CMD_Change_Playback_Speed cmd(iPK_Device, m_routerHelper->mediaPluginId(), this->nowPlayingButton->getStreamID() , 1000, true);
    emit sendDceCommand(cmd);
}

void qorbiterManager::playResume(){
    if(nowPlayingButton->getStreamID() != -1){
        CMD_Play cmd(iPK_Device, nowPlayingButton->nowPlayingDevice(), nowPlayingButton->getStreamID());
        emit sendDceCommand(cmd);
    }
}

void qorbiterManager::stopMedia() {/*emit stopPlayback();*/
    CMD_MH_Stop_Media endMedia(iPK_Device, m_routerHelper->mediaPluginId(),0,i_current_mediaType ,0,sEntertainArea,false);
    string response;
    emit sendDceCommand(endMedia);
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Response: %s", response.c_str());
    setDirectAv(false);
}

void qorbiterManager::stop_AV() {
    // sends a Stop command, which is normally routed to an actual AV device through IR, CEC or other means
    // this does not stop the "lmce activity"
    CMD_Stop avStop(iPK_Device, m_routerHelper->mediaPluginId(), 0,false);
    emit sendDceCommand(avStop);
}

bool qorbiterManager::loadSkins(QUrl base)
{

    emit skinMessage("LoadSkins()::Skins path" +base.toString());
    /*
      TODO ASAP:
      Write feeder function that preceeds this call to creat a simple string list
      of all availible skins for the build. a simple parsing of the skins directory
      should suffice and improper skins are dealt with later.
      */

    QStringList localSkins;
    QStringList excludes;
    excludes << "!lib" << "!startup";
    tskinModel->clear();


#ifdef __ANDROID__
    if(hostDevice == HostSystemData::ANDROID_PHONE){
#ifdef QT5
        tskinModel->addSkin("default");
#elif NECESSITAS
        tskinModel->addSkin("default");
#endif
    }else{
#ifdef QT5
        tskinModel->addSkin("default");
#else
        tskinModel->addSkin("default");
#endif
    }
#elif defined(for_harmattan)
    tskinModel->addSkin("default");
#elif defined(for_android)
    tskinModel->addSkin("default");
#elif defined(RPI)
    //    QDir desktopQmlPath(QString(base.toString()),"",QDir::Name, QDir::NoDotAndDotDot);
    //    desktopQmlPath.setNameFilters(excludes);
    //    setDceResponse("Skin Search Path:"+ desktopQmlPath.dirName());
    //    localSkins = desktopQmlPath.entryList(QDir::Dirs |QDir::NoDotAndDotDot);

    //    qDebug()<<"inside of skins we find" << localSkins.join(",");
    tskinModel->addSkin("default");
#elif defined(Q_OS_IOS)
    tskinModel->addSkin("default");
#else
    if(b_localLoading ){
        QDir desktopQmlPath(QString(base.toString()),"",QDir::Name, QDir::NoDotAndDotDot);
        setDceResponse("Desktop Skin Search Path:"+ desktopQmlPath.dirName());
        localSkins = desktopQmlPath.entryList(QDir::Dirs |QDir::NoDotAndDotDot);


        qDebug()<<"inside of skins we find" << localSkins.join(",");
        if(localSkins.count()==0){
            tskinModel->addSkin("default");
        } else {
            tskinModel->addSkin(localSkins.join(","));
        }
    } else {
        if(deviceTemplate==DEVICETEMPLATE_OnScreen_qOrbiter_CONST){
#if defined (QT5)
            tskinModel->addSkin("default");
#else
            tskinModel->addSkin("default");
#endif

        } else {
            tskinModel->addSkin("default");
        }

    }
#endif
    return true;
}

void qorbiterManager::quickReload()
{
    emit reloadRouter();
}

void qorbiterManager::forceReload()
{
    emit forceReloadRouter();
}

void qorbiterManager::showUI(bool b){

    if( b_orbiterReady && b_skinReady ) {
        setActiveSkin(currentSkin);
    } else  {
        setDceResponse("Orbiter Cant Show UI");
#ifdef debug
        qDebug() << "Orbiter Status:" << b_orbiterReady;
        qDebug() << "Skin Status:" << b_skinReady;
#endif
    }

}

void qorbiterManager::displayModelPages(QList<QObject *> pages){
    qorbiterUIwin->rootContext()->setContextProperty("pageList", QVariant::fromValue(pages));
    m_appEngine->rootContext()->setContextProperty("pageList", QVariant::fromValue(pages));
}


void qorbiterManager::getFloorplanDevices(int floorplantype){


    for (int i=0; i < floorplans->rowCount(); i++) {
#ifdef debug
        qDebug() << floorplans->index(i, 0, QModelIndex()).data(1);
#endif
        //qDebug() << Q_FUNC_INFO << floorplans->get(i)->floorplanType() << "::" << floorplantype;
        if(floorplans->index(i).data(6).toInt() == floorplantype)  {
            QString markerID = floorplans->index(i).data(1).toString();
            current_floorplan_devices.append(floorplans->find(markerID));
        }
    }
    qorbiterUIwin->rootContext()->setContextProperty("current_floorplan_devices", QVariant::fromValue(current_floorplan_devices));
    m_appEngine->rootContext()->setContextProperty("current_floorplan_device", QVariant::fromValue(current_floorplan_devices));
}

void qorbiterManager::setFloorplanType(int t)
{
    i_currentFloorplanType = t;
    emit floorplanTypeChanged(t);
}


void qorbiterManager::qmlSetupLmce(QString incdeviceid, QString incrouterip){
    getConfiguration();

    if(status=="starting"){
        setDceResponse("Triggering connection to LMCE Device ID [" + incdeviceid + "] port Router Address [" + incrouterip + "]") ;
        setInternalIp(incrouterip);
        setDeviceNumber(incdeviceid.toInt());
        setDceResponse("Initializing Local Manager");
        initializeManager(incrouterip.toStdString(), incdeviceid.toLong());

    } else if (status=="reconnect") {
        status="running";
        setDceResponse("Re -Initializing Local Manager");
        setCurrentScreen("Screen_1.qml");
        setReloadStatus(false);
    }

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
bool qorbiterManager::readLocalConfig(){
    if(settingsInterface->ready){
        if(restoreSettings()){

        }
        return true;
    } else {

        return false;
    }
}



bool qorbiterManager::writeConfig()
{
    settingsInterface->setOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Router, m_ipAddress);
    qDebug() <<  Q_FUNC_INFO << "Router IP: " << settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Router);
    settingsInterface->setOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID, iPK_Device);
    qDebug() <<  Q_FUNC_INFO << "Device ID: " << settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID).toInt();
    /* old below this line and will be replaced */
    // qDebug() << Q_FUNC_INFO;
    //setDceResponse( QString::fromLocal8Bit(Q_FUNC_INFO) << "Writing Local Config");
    settingsInterface->setOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Last_Used, m_currentRouter);
    QDomDocument localConfig;
    QString xmlPath;

#ifdef Q_OS_IOS
    xmlPath = mobileStorageLocation+"/config.xml";
    appConfigPath = xmlPath;
#elif MACBUILD
    xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("MacOS").append("Resources").append("/config.xml").toStdString());
#elif __ANDROID__
    xmlPath = mobileStorageLocation+"/config.xml";
#elif WIN32
    xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#else
    xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#endif
    QFile localConfigFile;

    if(appConfigPath.isEmpty()){
        appConfigPath = xmlPath;
        qDebug() << "Empty application write path. adjusted to: " <<xmlPath;
    }
    qDebug() << appConfigPath;
    localConfigFile.setFileName(appConfigPath);

    if (!localConfigFile.open(QFile::ReadWrite)){
        setDceResponse(localConfigFile.errorString());
        setDceResponse("Local Config is missing!");
        return false;
    } else {
        if (!localConfig.setContent(&localConfigFile)){
            setDceResponse("Cannot set document type!");
            return false;
        } else {
            localConfigFile.close();
            localConfigFile.remove(); // have to remove and then re-create!
            QDomElement configVariables = localConfig.documentElement().toElement();
            configVariables.namedItem("routerip").attributes().namedItem("id").setNodeValue(m_ipAddress);           //internal ip
            configVariables.namedItem("routeraddress").attributes().namedItem("id").setNodeValue(internalHost);     //internal hostname
#ifdef QT5
            configVariables.namedItem("qt5skin").attributes().namedItem("id").setNodeValue(currentSkin);
#elif NECESSITAS
            configVariables.namedItem("skin").attributes().namedItem("id").setNodeValue(currentSkin);
#endif
            configVariables.namedItem("routerport").attributes().namedItem("id").setNodeValue(routerPort);
            configVariables.namedItem("externalip").attributes().namedItem("id").setNodeValue(externalHost);     //externalip
            configVariables.namedItem("externalHost").attributes().namedItem("id").setNodeValue(externalHost);      //external host

            configVariables.namedItem("device").attributes().namedItem("id").setNodeValue(QString::number(iPK_Device));

            configVariables.namedItem("firstrun").attributes().namedItem("id").setNodeValue(QString("false"));
            configVariables.namedItem("debug").attributes().namedItem("id").setNodeValue(debugMode ==true? "true" : "false");
            setDceResponse("Writing device type as "+ QString::number(isPhone));
            configVariables.namedItem("phone").attributes().namedItem("id").setNodeValue(QString::number(isPhone));
            configVariables.namedItem("lastconnect").attributes().namedItem("id").setNodeValue(usingExternal ? "external" : "internal");
            if(!mobileStorageLocation.isEmpty()){
                configVariables.namedItem("mobile_storage").attributes().namedItem("id").setNodeValue(mobileStorageLocation);
            }

            QByteArray output = localConfig.toByteArray();
            localConfigFile.open(QFile::ReadWrite);
            if (!localConfigFile.write(output)){
                localConfigFile.close();
                setDceResponse("save failed");
                qWarning("Save Failed");
                return false;
            }
            localConfigFile.close();
            setDceResponse("Save succeded to " + mobileStorageLocation);
            return true;
        }
    }
}

void qorbiterManager::setStringParam(int paramType, QString param)
{
    mediaFilter.setStringParam(paramType, param);
    qDebug() << "Param = " << param;
    switch (paramType)
    {
    case 0: setGridMediaType(param);            break;
    case 1: setSubType(param);                  break;
    case 2: setGridFileFormat(param);           break;
    case 3: setGridAttributeGenres(param);        break;
    case 4:
        if (!param.contains("!D")){

            if(param.contains("!F") || param.contains("!P")){
                break;
            } else{
                setGridPkAttribute(param.remove("!A"));
            }
        }else{                                  // 5||||1,2	!D'/home/public/data/videos/1.82 TB (sdd1) ST2000DL003-9VT [51]'|0|0|0 | 2 |
            int t = param.indexOf("\t");
            QString apnd = param.mid(t);
            setGridMediaSources(apnd);
        }
        break;

    case 5:setGridUsersPrivate(param+","+QString::number(userList->currentPrivateUser));  break;
    case 6:
        if (param.contains("!P"))    {
            break;
        } else {
            setGridAttributeTypeSort(param);
            break;
        }
    case 7: setGridPkUsers(param);         break;
    case 8: setGridLastViewed(param);      break;
    case 9:
        if(param.contains("!F") || param.contains("!P")){
            break;
        } else {
            setGridPkAttribute(param.remove("!A"));
            break;
        }
    }
}

void qorbiterManager::initializeGridModel()
{
    basicProvider = new basicImageProvider();
    qorbiterUIwin->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());
    m_appEngine->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());
    setDceResponse("Grid Initialized");
}

bool qorbiterManager::goBackGrid()
{
    removeRestoreIndex();
    return mediaFilter.goBack();
}

void qorbiterManager::mediaItemSelected(QString itemID)
{
    mp_fileDetails->setVisible(true);
    mp_fileDetails->setFile(itemID);
}


void qorbiterManager::requestSecurityPic(int i_pk_camera_device, int h, int w)
{
    emit getSingleCam(i_pk_camera_device,  h,  w);
}


void qorbiterManager::showfloorplan(int fptype)
{
    QString Screen ;
    switch(fptype){
    case 1: // - security floorplan. screen 5
        setFloorplanType(fptype);
        Screen = QString("Screen_5.qml");
        break;
    case 5: // - entertainment floorplan. screen 3
        setFloorplanType(fptype);
        Screen = QString("Screen_3.qml");
        break;
    case 6: // -general av equipment - piped to misc screen 7
        setFloorplanType(fptype);
        Screen = QString("Screen_7.qml");
        break;
    case 3: //climate screen 4
        setFloorplanType(fptype);
        Screen = QString("Screen_4.qml");
        break;
    case 7: //telecom
        setFloorplanType(fptype);
        Screen = QString("Screen_6.qml");
        break;
    default:
        setFloorplanType(fptype);
        Screen = QString("Screen_").append(QString::number(fptype).append(".qml"));

        break;
    }


    setCurrentScreen(Screen);
    //pqOrbiter->ShowFloorPlan(fptype);
}

void qorbiterManager::setFloorPlanCommand(QVariantMap t)
{
    // qDebug() << t;

    FloorplanDevice *p = floorplans->find(t["device"].toInt());
    if(p){

        QVariantMap b;
        b.insert("commands", t["commands"]);

        if(t.contains("media")){
            b.insert("media", t["media"]);
        }

        p->setDeviceCommand(b);
        //       foreach(QVariant cmd , t["commands"].toMap()){
        //           QVariantMap l = cmd.toMap();
        //           qDebug() << l["command_name"] << " => " << l["command_number"];      }
    }
}

void qorbiterManager::getDeviceState(int PK_Device, string* data)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::getDeviceState");
    CMD_Get_Device_State getDeviceState(iPK_Device, m_routerHelper->generalInfoPluginId() , PK_Device, data);
    sendDceCommand(getDeviceState);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::getDeviceState done");
}

int qorbiterManager::scheduleRecording(QString sType, QString sProgramID)
{
    //TODO: currently the media plugin does not allow us to send a this command through it if there is no
    // active stream. We need to be able to do this to allow scheduling recordings from the EPG screen without
    // actively watching TV. So we now do a slight hack and send to both VDT and MythTV plugin DTs
    // only one of them is installed at once anyway.

    DCECommand *pCmd = getDCECommand();
    CMD_Schedule_Recording_DT *cmd = new CMD_Schedule_Recording_DT(iPK_Device, DEVICETEMPLATE_VDRPlugin_CONST,
                                                                   BL_SameHouse, sType.toStdString(), "", sProgramID.toStdString(),
                                                                   pCmd->getString(COMMANDPARAMETER_ID_CONST));
    pCmd->setCommand(cmd);
    emit sendDceCommandResponse(pCmd);
    return pCmd->getCallback();
    /*
       CMD_Schedule_Recording_DT *cmd2 = new CMD_Schedule_Recording_DT(iPK_Device, DEVICETEMPLATE_MythTV_PlugIn_CONST,
                                                                       BL_SameHouse, sType.toStdString(), "", sProgramID.toStdString(), ps2);
                                                                       */
}

int qorbiterManager::cancelRecording(QString sID, QString sProgramID) {
    DCECommand *pCmd = getDCECommand();
    CMD_Remove_Scheduled_Recording_DT *cmd = new CMD_Remove_Scheduled_Recording_DT(iPK_Device, DEVICETEMPLATE_VDRPlugin_CONST, BL_SameHouse, sID.toStdString(), sProgramID.toStdString());
    pCmd->setCommand(cmd);
    emit sendDceCommandResponse(pCmd);
    /*        CMD_Remove_Scheduled_Recording_DT cmd2(iPK_Device, DEVICETEMPLATE_MythTV_PlugIn_CONST, BL_SameHouse, sID.toStdString(), sProgramID.toStdString());
        emit sendDceCommand(cmd2, cbno);
        return cbno;*/
    return pCmd->getCallback();
}

void qorbiterManager::ejectDisc(int discDrive, int slot)
{
    emit ejectDiscDrive((long)discDrive, slot);
}

void qorbiterManager::updatePlaylist()
{
    GenericFlatListModel *pModel = getDataGridModel("Playlist", 18, "38");
    if(pModel){pModel->refreshData();}
}

DCECommand* qorbiterManager::getDCECommand()
{
    return new DCECommand(m_dceRequestNo.fetchAndAddAcquire(1));
}

void qorbiterManager::handleUseQueueChanged(bool useQueue)
{

    if(m_useQueueInsteadOfInstantPlay == useQueue) return;
    m_useQueueInsteadOfInstantPlay = useQueue;

}

void qorbiterManager::jumpToAttributeGrid(int attributeType, int attribute)
{
    mediaFilter.setFilterFromMediaGrid(attributeType,attribute);
}

void qorbiterManager::handleDceAlert(QString text, QString tokens, int timeout, int interruption)
{
    emit newDceAlert(text, QVariant(tokens), timeout, interruption);
}

void qorbiterManager::updateImageChanged(QImage img)
{

}


void qorbiterManager::setScreenShotVariables(screenshotAttributes *t)
{

    qDebug("Setting thumbnail attributes to screen");

    screenshotVars.append(new screenshotAttributes(t->attributeName, t->attributeType, t->attributeNo, this));

}

void qorbiterManager::setMediaScreenShot(const QImage screen_shot)
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

void qorbiterManager::toggleSkinType()
{
    return;
    {
        qDebug() << tskinModel->m_baseUrl;
        QString newPath= tskinModel->m_baseUrl.toString();
        if(hostDevice==HostSystemData::ANDROID_PHONE){
            setHostDevice(HostSystemData::ANDROID_TABLET);
            setFormFactor(2);
            newPath.replace("phone", "tablet");
        } else {
            setHostDevice(HostSystemData::ANDROID_PHONE);
            setFormFactor(1);
            newPath.replace("tablet", "phone");
        }
        tskinModel->m_baseUrl=QUrl(newPath);
        qDebug() << newPath;
        qDebug() << tskinModel->m_baseUrl;
        loadSkins(tskinModel->m_baseUrl);

        currentSkin= "default";
        if(writeConfig()){
            swapSkins( "default");
        }
    }
}

void qorbiterManager::showBookmarks(QList<QObject *> t)
{

    current_bookmarks = t;
#ifdef debug
    qDebug() << current_bookmarks.size();
#endif
    //  qorbiterUIwin->rootContext()->setContextProperty("currentBookmarks", QVariant::fromValue(current_bookmarks));
}

void qorbiterManager::setHouseMode(QString pass, int mode, QString handling)
{
    emit newHouseMode(pass, mode, handling);
    // pqOrbiter->SetSecurityMode(pass, mode);
}

void qorbiterManager::setCurrentUser(QString inc_user)
{
   // qDebug() << "Incoming user::" << inc_user;
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

void qorbiterManager::updateAlarm(QString dataGridId,int row,int role,bool s, int g)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::updateAlarm");
    emit setAlarm(dataGridId, row, role, s, g);
}

void qorbiterManager::checkConnection()
{
    setDceResponse("Disconnect detected, checking");
    emit pingTheRouter();
}



bool qorbiterManager::cleanupData()
{
    roomLights->clear();                 //current room scenarios model


    if(roomMedia){
        roomMedia->clear();
    }
    if (tskinModel != NULL) {
        tskinModel->clear();
        delete tskinModel;
    }

    roomClimate->clear();
    roomClimateScenarios.clear();
    roomLightingScenarios.clear();
    roomMediaScenarios.clear();
    roomSecurity->clear();
    roomSecurityScenarios.clear();
    roomTelecom->clear();
    roomTelecomScenarios.clear();
    nowPlayingButton->resetData();
    mp_fileDetails->clear();
    devices->clear();
    myOrbiters->clear();
    deviceCommands->clear();
    floorplans->clear();
    nowPlayingButton->resetData();
    mp_fileDetails->clear();
    screensaverImages.clear();

    attribFilter->clear();
    genreFilter->clear();
    mediaTypeFilter->clear();
    uiFileFilter->clear();
    userList->clear();

    m_bStartingUp=true;
    return true;
}

void qorbiterManager::showMessage(QString message, int duration, bool critical)
{

}

void qorbiterManager::startOrbiter()
{
    if (b_skinReady && b_orbiterReady)    {
        setDceResponse("Showing main orbiter window");
        QApplication::processEvents(QEventLoop::AllEvents);

        m_bStartingUp = false;

#if (QT5)
        qorbiterUIwin->setTitle("LinuxMCE Orbiter "+ QString::number(iPK_Device));
        // qorbiterUIwin->setResizeMode(QQuickView::SizeRootObjectToView);
#else
        qorbiterUIwin->setWindowTitle("LinuxMCE Orbiter " + QString::number(iPK_Device));
        qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif
        //   QApplication::processEvents(QEventLoop::AllEvents);
        //  setCurrentScreen("Screen_1.qml");
        // QApplication::processEvents(QEventLoop::AllEvents);
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

void qorbiterManager::setActiveSkin(QString name){


    if(!mb_useNetworkSkins){
        swapSkins(name);
        return;
    }


    if(name=="splash_fallback"){

    }
    else {
        //    tskinModel->setActiveSkin(deviceTemplate == DEVICETEMPLATE_OnScreen_qOrbiter_CONST ? "default" : name);
    }

    qDebug("Setting Skin");
}

void qorbiterManager::cleanupScreenie(){
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

void qorbiterManager::setupContextObjects()
{
    //file details object and imageprovider setup

    qorbiterUIwin->rootContext()->setContextProperty("filedetailsclass" ,mp_fileDetails);
    m_appEngine->rootContext()->setContextProperty("filedetailsclass", mp_fileDetails);

    qorbiterUIwin->rootContext()->setContextProperty("dcenowplaying" , nowPlayingButton);
    m_appEngine->rootContext()->setContextProperty("dcenowplaying", nowPlayingButton);

    //screen parameters class that could be extended as needed to fetch other data
    qorbiterUIwin->rootContext()->setContextProperty("screenparams", mp_screenParameters);
    m_appEngine->rootContext()->setContextProperty("screenparams", mp_screenParameters);

    //----------------Security Video setup
    qorbiterUIwin->rootContext()->setContextProperty("securityvideo", mp_securityVideo);
    m_appEngine->rootContext()->setContextProperty("securityvideo", mp_securityVideo);

    m_appEngine->rootContext()->setContextProperty("mediaHelper", m_mediaHelper);


}

void qorbiterManager::setupEarlyContexts()
{

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

    m_appEngine->rootContext()->setContextProperty("dcemessage", dceResponse);
    m_appEngine->rootContext()->setContextProperty("orbiterList", myOrbiters);
    m_appEngine->rootContext()->setContextProperty("deviceList", devices);
    m_appEngine->rootContext()->setContextProperty("deviceCommands", deviceCommands);
    m_appEngine->rootContext()->setContextProperty("fontsHelper", m_fontsHelper);

    //Resize to view as opposed to the root item
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
    // qorbiterUIwin->setResizeMode(QQuickView::SizeViewToRootObject);
    m_screenInfo = new ScreenInfo();
    qorbiterUIwin->rootContext()->setContextProperty("screenInfo", m_screenInfo);
#else
    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif

#ifdef QT4
    qorbiterUIwin->rootContext()->setContextProperty("appH", QApplication::desktop()->height());
    qorbiterUIwin->rootContext()->setContextProperty("appW", QApplication::desktop()->width());
#endif

    //Don't think we need this with Qt5 QQuickView as it resizes Object to View above
#ifndef QT5
    QObject::connect(qorbiterUIwin, SIGNAL(sceneResized(QSize)),  SLOT(checkOrientation(QSize)) );

#else
    QObject::connect(m_window, SIGNAL(contentOrientationChanged(Qt::ScreenOrientation)), this, SLOT(checkOrientation(Qt::ScreenOrientation)));

#endif

    QObject::connect(this, SIGNAL(orbiterReady(bool)), this, SLOT(showUI(bool)));
    // QObject::connect(this, SIGNAL(skinDataLoaded(bool)), SLOT(showUI(bool)));
    QObject::connect(m_appEngine, SIGNAL(quit()), this, SLOT(closeOrbiter()));
}

void qorbiterManager::setupUiSelectors(){


    qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin"));
    setApplicationPath(QApplication::applicationDirPath());
    localDir = qmlPath.append(buildType.remove("/qml"));
    remoteDirectoryPath = "http://"+m_currentRouter+"/lmce-admin/skins"+buildType.remove("/qml");
    m_remoteQmlPath = remoteDirectoryPath;


#ifdef __ANDROID__
    m_localQmlPath="qrc:/qml/";
#elif defined Q_OS_IOS
    m_localQmlPath="qrc:/qml/";
#elif defined WIN32
    m_localQmlPath=qApp->applicationDirPath()+"/";
#elif defined MACBUILD
    m_localQmlPath="qrc:/qml";
    // m_localQmlPath=qApp->applicationDirPath()+"/../Resources/";
#ifdef simulate
    m_localQmlPath=qApp->applicationDirPath();
    m_localQmlPath.remove("/build-output/");
    m_localQmlPath.remove("qorbiter-core-gl.app/Contents/MacOS");
    m_localQmlPath.append("/qOrbiter_src/qml/");
#endif

#elif defined Q_OS_LINUX
    //  m_localQmlPath=qApp->applicationDirPath()+"/";

#ifdef NOQRC
    m_localQmlPath="";
#else
    m_localQmlPath="qrc:/qml/";
#endif

#ifdef simulate

#ifdef NOQRC
    m_localQmlPath="../qOrbiter_src/qml/";
#else
    m_localQmlPath="qrc:/qml/";
#endif

#endif

#endif

    if(m_skinOverridePath!=""){
        m_localQmlPath = m_skinOverridePath;
    }

    skinMessage("build type set to:: "+buildType);
    qDebug() << "Local path set to " << m_localQmlPath;
}

void qorbiterManager::beginSetup()
{
    if(getDeviceNumber()==-1){
        emit splashReady();
        return;
    }

    if(setSizeSelector() ){

    }

    if(createThemeStyle()){
        setUiReady(true);
    }


}

bool qorbiterManager::setSizeSelector()
{

    if(!m_window){
        qWarning() << " Window Not Set ";
        return false;
    } else {
        qWarning() << " Window Set, resetting screen size";
    }

    QString psize = m_screenInfo->primaryScreen()->pixelDensityString();
    QStringList t;


    if(m_testScreenSize==-1){
        qDebug () << Q_FUNC_INFO << "Using device information ";

        if(m_bIsOSD){
            t  << "md"<<  m_screenInfo->primaryScreen()->deviceSizeString() <<  psize  << m_screenInfo->primaryScreen()->resolutionString(); ;

            qDebug() << "Device set as embedded (rpi) device";
        } else {
            t <<  m_screenInfo->primaryScreen()->deviceSizeString() << psize  << m_screenInfo->primaryScreen()->resolutionString();
        }
        m_currentSizeSelector = m_screenInfo->primaryScreen()->deviceSizeString() ;
        m_deviceSize = m_screenInfo->primaryScreen()->deviceSize();
#if !defined(QANDROID) && !defined(Q_OS_IOS)
        m_window->setPosition(250, 250);
        // setAppH(640);
        // setAppW(800);
#else
        m_window->showFullScreen();
#endif
    } else {

        qDebug() << Q_FUNC_INFO << "Using test screen size";
        QString testDeviceString;
        switch(m_testScreenSize){
        case ScreenData::Device_Small: testDeviceString="small";m_deviceSize = ScreenData::Device_Small;        break;
        case ScreenData::Device_Medium: testDeviceString="medium";m_deviceSize = ScreenData::Device_Medium;     break;
        case ScreenData::Device_Large: testDeviceString="large";m_deviceSize = ScreenData::Device_Large;        break;
        case ScreenData::Device_XLarge:testDeviceString="xlarge";m_deviceSize = ScreenData::Device_XLarge;      break;
        default: testDeviceString="large"; break;
        }

        t <<testDeviceString << psize << QString::number(qorbiterUIwin->height() );
       m_currentSizeSelector = testDeviceString;
    }

    m_selector->setExtraSelectors(t);
    selector->setSelector(m_selector);
    setCurrentTheme(t.join("|"));

    qDebug() << Q_FUNC_INFO << "QFile Selector Set to "<< m_selector->allSelectors().join("\n");
    return true;
}

bool qorbiterManager::createThemeStyle()
{
    setUiReady(false);
    qDebug() << Q_FUNC_INFO  << " enter ";
    if(!m_window){
        qWarning() << " Window Not Set cannot create theme ";
        return false ;
    } else {
        qWarning() << " Window Set, loading theme style";
    }


    selector->setSelector(m_selector);
    if(m_style ){
        qDebug() << Q_FUNC_INFO << "Deleting style";
        m_style->deleteLater();
    }

    if(m_bIsOSD){
        setSkinEntryFile(selectPath( "skins/"+currentSkin+"/+md/Main.qml"));
    } else {
        setSkinEntryFile(selectPath( "skins/"+currentSkin+"/Main.qml"));
    }

    qDebug() << "Skin entry file set to " << m_skinEntryFile;
    QString fp;

#ifdef NOQRC
    qDebug() << "Using local path for NOQRC flag";
    fp =m_localQmlPath+"skins/"+currentSkin+"/+"+m_currentSizeSelector+"/Style.qml";
#else
    if(m_bIsOSD)
        fp ="skins/"+currentSkin+"/+md/Style.qml";
    else
        fp ="skins/"+currentSkin+"/+"+m_currentSizeSelector+"/Style.qml";
#endif
    qWarning() << QString("Selecting Style.qml for theme %1 for skin %2 from path %3").arg(getCurrentTheme()).arg(currentSkin).arg(fp);
    qDebug() << Q_FUNC_INFO << "Current Selectors \n\t" << m_selector->allSelectors().join("\n\t");
    QString filePath =  selectPath(fp);

    qDebug() << "Style file path " << filePath;
    QQmlComponent nustyle(m_appEngine , QUrl(filePath), QQmlComponent::PreferSynchronous);
    m_style = nustyle.create();

    if(m_style){
        qDebug() << Q_FUNC_INFO << " New style applied. " << filePath;
    } else {
        qDebug() << Q_FUNC_INFO << nustyle.errors();
        qDebug() << "New style failed application! " << filePath;
    }

    //  m_appEngine->clearComponentCache();
    m_appEngine->rootContext()->setContextProperty("Style", m_style);

    qDebug() << Q_FUNC_INFO << " exit ";
    return true;
}

QString qorbiterManager::getCurrentTheme() const
{
    return m_currentTheme;
}

void qorbiterManager::setCurrentTheme(const QString &currentTheme)
{
    if(m_currentTheme==currentTheme)return;
    m_currentTheme = currentTheme;
    emit currentThemeChanged();
}


bool qorbiterManager::getUiReady() const
{
    return uiReady;
}

void qorbiterManager::setUiReady(bool value)
{
    if(uiReady==value) return;
    uiReady = value; emit uiReadyChanged();
}

QString qorbiterManager::skinEntryFile() const
{
    return m_skinEntryFile;
}

void qorbiterManager::setSkinEntryFile(const QString &skinEntryFile)
{
    if(m_skinEntryFile==skinEntryFile) return;
    m_skinEntryFile = skinEntryFile; emit skinEntryFileChanged();
}


bool qorbiterManager::restoreSettings()
{

    int tId= settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID).toInt();
    qDebug() << Q_FUNC_INFO << "Settings device id" << tId;
    QString trouter = settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Router).toString();
    QString extRouter = settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_ExternalHostname).toString();
    mb_useNetworkSkins = settingsInterface->getOption(SettingsInterfaceType::Settings_UI, SettingsKeyType::Setting_Ui_NetworkLoading).toBool();
    QString last =  settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Last_Used).toString();
    //setLocation( settingsInterface->getLastRoom(), settingsInterface->getLastEa() );
    qDebug() << "Current router " << m_currentRouter;
    /*Fix Me! */ currentSkin="default";
    qDebug() << "Using network skins?" << mb_useNetworkSkins;
    if(tId && !trouter.isEmpty()){
        qDebug() << Q_FUNC_INFO << "Read Device Number";
        setDeviceNumber(tId);
        setInternalIp(trouter);
        setExternalHost(extRouter);
        setCurrentRouter(!usingExternal ? trouter : extRouter);
    }
    return true;
}


bool qorbiterManager::setupLocalSkins()
{
    QFile themeFile(":///qml/qml/Themes.json");
    QString themeJson;

    if(themeFile.open(QFile::ReadOnly)){
        themeJson = themeFile.readAll();
        themeFile.close();
    } else {
        qDebug() << themeFile.errorString();
        qDebug() << Q_FUNC_INFO << "failed to find Themes.json";
        return false;
    }

    QJsonDocument themeDoc = QJsonDocument::fromJson(themeJson.toUtf8());

    QJsonObject themeObject = themeDoc.object();

    if(themeObject.isEmpty())
        return false;

    QVariantMap t = themeObject.toVariantMap();
}

void qorbiterManager::handleViewError(QList<QQmlError>)
{


}

void qorbiterManager::reloadHandler()
{
    setConnectedState(false);
    setReloadStatus(true);
    setCurrentScreen("Screen_Reload.qml", true);
}

void qorbiterManager::disconnectHandler()
{
    setConnectedState(false);
}

void qorbiterManager::replaceHandler()
{
    closeOrbiter();
}

void qorbiterManager::setDceResponse(QString response)
{
    dceResponse = response;
    emit loadingMessage(dceResponse);
    emit dceResponseChanged();
    logQtMessage(response);
}

QString qorbiterManager::getDceResponse()
{
    return dceResponse;
}


int qorbiterManager::loadSplash()
{
    return 0;
}

/*!
 * \brief This function is called when the application window size changes. When it does,
 * the \ref setOrientation() function is called.
 *
 */
void qorbiterManager::checkOrientation(QSize s)
{
    if(s.height()==0 || s.width() == 0) return;

    setDceResponse("checkOrientation(QSize)::start");
    //NOTE: Is this not handled by the window manager and Orientation change signals?

    appHeight=s.height();  appWidth = s.width();
    setOrientation(appHeight > appWidth);

}

#ifdef QT5
void qorbiterManager::checkOrientation(Qt::ScreenOrientation o)
{


    //return;

    switch (o) {
    case Qt::InvertedLandscapeOrientation: qDebug() << "Inverted Landscape";
    case Qt::LandscapeOrientation: qDebug() << "Landscape";
        setOrientation(false);
        break;
    case Qt::PrimaryOrientation: qDebug() << "Primary "; ;
    case Qt::InvertedPortraitOrientation:qDebug() << "Inverted portait"; ;
    case Qt::PortraitOrientation:qDebug() << "Portrait";
        setOrientation(true);
        break;
    default: qDebug() << "unknown orientation";
        break;
    }

}
#endif

void qorbiterManager::getGrid(int i)
{
    setRequestMore(true);
    emit gridTypeChanged(i);
    emit mediaRequest(i);
}


QString qorbiterManager::getCurrentScreen(){
    return currentScreen;
}

void qorbiterManager::setCurrentScreen(QString s, bool force)
{

    if(!s.contains(".qml")){
        setCurrentScreen(s.toInt(), force);
        return;
    }

    if(s=="Screen_1.qml"){
        clearAllDataGrid();
    }

    if(s!=currentScreen || force){
        currentScreen = s;
        emit screenChange(s);
    }

}

void qorbiterManager::setCurrentScreen(int s, bool force)
{

    QString i = QString("Screen_%1.qml").arg(QString::number(s));

    if(i!=currentScreen || force){
        currentScreen = i;
        emit screenChange(i);

        if(!currentScreen.contains("187")){
            gotoScreenList->append(currentScreen);
        }
    }
}

void qorbiterManager::setCurrentOsdScreen(int s, bool force)
{

    QString i = QString("Screen_%1.qml").arg(QString::number(s));

    if(i!=m_currentOsdScreen || force){
        m_currentOsdScreen = i;
        emit currentOsdScreenChanged(i);
    }

}

void qorbiterManager::setCurrentOsdScreen(QString s, bool force )
{

     if(!s.contains(".qml")){
        setCurrentOsdScreen(s.toInt(), force);
        return;
    }

    if(s=="Screen_1.qml"){
        clearAllDataGrid();
    }

    if(s!=m_currentOsdScreen || force){
        m_currentOsdScreen = s;
        emit currentOsdScreenChanged(s);
    }
}

void qorbiterManager::setCurrentOsd(QString b)
{
    {setCurrentOsdScreen(b, false); }
}

void qorbiterManager::setCurrentRemotePopup(int p)
{
    QString i = QString("Remote_%1.qml").arg(QString::number(p));

    if(i!= m_currentRemotePopup){
        m_currentRemotePopup = i;
        emit currentRemotePopupChanged(i);
    }
}


void qorbiterManager::requestSingleView(int camera)
{
    emit getSingleCam(camera, 600,800, true);
}

void qorbiterManager::connectionWatchdog()
{
    status="reconnect";
    QTimer::singleShot(5000, this, SIGNAL(reInitialize()));
    emit qtMessage("Starting Watchdog");
}


void qorbiterManager::showExistingOrbiter(const QList<QObject*> l )
{
    Q_UNUSED(l);
}

void qorbiterManager::setText(QString sDesignObj, QString sValue, int iPK_Text)
{
    emit textChanged(sDesignObj, sValue, iPK_Text);
}

void qorbiterManager::makeCall(int iPK_Users, QString sPhoneExtension, QString sPK_Device_From,int iPK_Device_To) {
    CMD_Make_Call makeCall(iPK_Device, m_routerHelper->telecomPluginId(), iPK_Users, sPhoneExtension.toStdString(), sPK_Device_From.toStdString(), iPK_Device_To);
    sendDceCommand(makeCall);
}

QVariant qorbiterManager::systemFontList()
{   
    return QVariant(m_fontDir.entryList());
}

void qorbiterManager::reloadQml()
{
    setUiReady(false);

    if(!m_window){
        qWarning() << " Window Not Set ";
        return;
    } else {
        qWarning() << " Window Set, reloading QML";
    }

    if(m_style && !mb_useNetworkSkins ){
        qDebug() << Q_FUNC_INFO << "Deleting style";
        m_style->deleteLater();
    }

    selector->setSelector(m_selector);

    qDebug() << m_selector->select("skins/"+currentSkin+"/Main.qml");
    qDebug() << m_appEngine->baseUrl();

    qDebug() << Q_FUNC_INFO << "Current Selectors \n" << m_selector->allSelectors().join("\n\t");
    QString fp ="skins/"+currentSkin+"/+"+m_currentSizeSelector+"/Style.qml";
    qDebug () << Q_FUNC_INFO << selectPath(fp);
    QString filePath = m_selector->select(fp);

#ifdef NOQRC
    filePath.prepend(m_localQmlPath);
#else
    if(m_skinOverridePath.isEmpty()){

    } else{
        filePath.prepend("qrc:///");
    }

#endif

    qDebug() << "Style file path " << filePath;
    QQmlComponent nustyle(m_appEngine , QUrl(filePath), QQmlComponent::PreferSynchronous);
    m_style = nustyle.create();

    if(m_style){
        qDebug() << Q_FUNC_INFO << " New style created. " << filePath;
    } else {
        qDebug() << Q_FUNC_INFO << nustyle.errors();
        qDebug() << "New style failed application! " << filePath;
    }

    m_appEngine->clearComponentCache();
    m_appEngine->rootContext()->setContextProperty("Style", m_style);

    setUiReady(true);
    emit screenChange(currentScreen);
}

void qorbiterManager::handleScreenChanged(QScreen *screen)
{
    Q_UNUSED(screen);
    resetScreenSize();
}

void qorbiterManager::resetScreenSize(){

    if(m_testScreenSize==-1){
        return;
    }

    if(!m_window){
        qWarning() << " Window Not Set ";
        return;
    } else {
        qWarning() << " Window Set, resetting screen size";
    }

    QString psize = m_screenInfo->primaryScreen()->pixelDensityString();
    QStringList t;
    // m_testScreenSize = m_screenInfo->primaryScreen()->deviceSize();

    if(m_testScreenSize==-1){
        qDebug () << Q_FUNC_INFO << "Using device information ";
        t <<  m_screenInfo->primaryScreen()->deviceSizeString() << psize  << m_screenInfo->primaryScreen()->resolutionString();
        m_deviceSize = m_screenInfo->primaryScreen()->deviceSize();
#if !defined(QANDROID) && !defined(Q_OS_IOS)
        m_window->resize(appWidth, appHeight);
        m_window->showNormal();
#else
        m_window->showFullScreen();
#endif

    } else {

        qDebug() << Q_FUNC_INFO << "Using test screen size";
        QString testDeviceString;
        switch(m_testScreenSize){
        case ScreenData::Device_Small: testDeviceString="small";m_deviceSize = ScreenData::Device_Small;        break;
        case ScreenData::Device_Medium: testDeviceString="medium";m_deviceSize = ScreenData::Device_Medium;     break;
        case ScreenData::Device_Large: testDeviceString="large";m_deviceSize = ScreenData::Device_Large;        break;
        case ScreenData::Device_XLarge:testDeviceString="xlarge";m_deviceSize = ScreenData::Device_XLarge;      break;
        default: testDeviceString="large"; break;
        }

        t <<testDeviceString << psize << QString::number(m_window->height() );
    }

    m_selector->setExtraSelectors(t);
    selector->setSelector(m_selector);

    qDebug() << Q_FUNC_INFO << "QFile Selector Set to "<< m_selector->allSelectors().join("\n");
    delayedReloadQml();
    // qorbiterUIwin->setSource(QUrl("qrc:/qml/Index.qml"));
    // qorbiterUIwin->setSource(qorbiterUIwin->source());

}

bool qorbiterManager::registerConnections(QObject *qOrbiter_ptr)
{
    qOrbiter * ptr = qobject_cast<qOrbiter*>(qOrbiter_ptr);
    QObject::connect(this, SIGNAL(forceReloadRouter()), ptr, SLOT(forceReloadRouter()), Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::showAlert, this, &qorbiterManager::handleDceAlert, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::dgRequestFinished, this, &qorbiterManager::handleDgRequestFinished, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::discreteAudioChanged, this, &qorbiterManager::setDiscreteAudio, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::commandResponseChanged, this, &qorbiterManager::setCommandResponse, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::mediaResponseChanged, this, &qorbiterManager::setMediaResponse, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::deviceResponseChanged, this, &qorbiterManager::setDeviceResponse, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::eventResponseChanged, this, &qorbiterManager::setEventResponse, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::liveAvPath, this, &qorbiterManager::setLiveAvPath, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::containsVideo, this, &qorbiterManager::setContainsVideo, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::isOsd, this, &qorbiterManager::setOsd, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::monitorStatusChanged, this, &qorbiterManager::setMonitorStatus, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::newMessageSend, ptr, &qOrbiter::executeMessageSend, Qt::QueuedConnection);

    QObject::connect(this, &qorbiterManager::newGridChannel,ptr, &qOrbiter::TuneToChannel, Qt::QueuedConnection );
    //QObject::connect(this, SIGNAL(clearModel()), simpleEPGmodel, SLOT(clear()), Qt::QueuedConnection);
    //security video frames
    QObject::connect(this, &qorbiterManager::newHouseMode,ptr, &qOrbiter::SetSecurityMode, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::getSingleCam,ptr, &qOrbiter::GetSingleSecurityCam, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::mediaSeperatorChanged,ptr, &qOrbiter::setGridSeperator, Qt::QueuedConnection);

    QObject::connect(this, &qorbiterManager::changeTrack,ptr, &qOrbiter::changedTrack, Qt::QueuedConnection);
    // QObject::connect(ptr, SIGNAL(clearPlaylist()), ptr, SLOT(updatePlaylist()), Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::newPlaylistPosition,ptr, &qOrbiter::jumpToPlaylistPosition, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::movePlistEntry,ptr, &qOrbiter::movePlaylistEntry, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::savePlist,ptr, &qOrbiter::saveCurrentPlaylist, Qt::QueuedConnection);


    QObject::connect(this, &qorbiterManager::switchIpAddress, ptr,&qOrbiter::changeAndRestart, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::addScreenParam, mp_screenParameters, &ScreenParamsClass::addParam, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::locationChanged, ptr, &qOrbiter::setLocation,Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::userChanged, ptr, &qOrbiter::setUser,Qt::QueuedConnection);

    QObject::connect(this, &qorbiterManager::executeCMD, ptr, &qOrbiter::executeCommandGroup, Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::registerOrbiter, ptr,&qOrbiter::registerDevice,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::startManager, this, &qorbiterManager::qmlSetupLmce,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::addExistingOrbiter, myOrbiters, &ExistingOrbiterModel::appendRow);
    QObject::connect(ptr, &qOrbiter::clearExistingOrbiters, myOrbiters, &ExistingOrbiterModel::clear,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::screenSaverImages, this, &qorbiterManager::setScreenSaverImages,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::screenSaverTimerOutChanged, this, &qorbiterManager::setScreensaverTimerout);
    QObject::connect(this, &qorbiterManager::updateDceScreenSaverTimeout, ptr, &qOrbiter::updateScreenSaverTimeout);
    QObject::connect(ptr, &qOrbiter::setMyIp, this, &qorbiterManager::setInternalIp,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::statusMessage, this , &qorbiterManager::setDceResponse,Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(commandComplete(DCECommand*)), this, SLOT(commandCompleted(DCECommand*)), Qt::QueuedConnection);
    /*CHild Devices*/
    QObject::connect(ptr, &qOrbiter::qMediaPlayerIDChanged, this, &qorbiterManager::setMediaPlayerID, Qt::QueuedConnection);
    //messaging
    //  QObject::connect(mediaModel, SIGNAL(statusMessage(QString)), this, SLOT(setDceResponse(QString)),Qt::QueuedConnection);
    //sleeping alarms
    QObject::connect(this, &qorbiterManager::setAlarm, ptr, &qOrbiter::setAlarm, Qt::QueuedConnection);

    //navigation

    QObject::connect(ptr, SIGNAL(gotoQml(QString)), this, SLOT(setCurrentScreen(QString)),Qt::QueuedConnection); //old style because its overloaded
    QObject::connect(ptr, SIGNAL(gotoOsdQml(QString)), this, SLOT(setCurrentOsd(QString)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(setRemotePopup(int)), this, SLOT(setCurrentRemotePopup(int)), Qt::QueuedConnection);
    //floorplans
    QObject::connect(this, &qorbiterManager::floorplanTypeChanged, ptr, &qOrbiter::ShowFloorPlan, Qt::QueuedConnection); /*!< Should move into floorplan object */
    QObject::connect(ptr, &qOrbiter::floorPlanImageData, floorplans, &FloorPlanModel::setImageData, Qt::QueuedConnection);/*!< Should move into floorplan object */
    QObject::connect(floorplans, &FloorPlanModel::pageChanged, ptr, &qOrbiter::getFloorPlanImage, Qt::QueuedConnection);/*!< Should move into floorplan object */
    QObject::connect(floorplans, &FloorPlanModel::requestNewFloorPlanData, ptr, &qOrbiter::updateFloorPlan, Qt::QueuedConnection);/*!< Should move into floorplan object */
    QObject::connect(ptr,&qOrbiter::floorplanTypeChanged, floorplans, &FloorPlanModel::setCurrentFloorPlanType,Qt::QueuedConnection);/*!< Should move into floorplan object */

    QObject::connect(this, &qorbiterManager::populate_floorplan_device_commands, ptr, &qOrbiter::getFloorplanDeviceCommand, Qt::QueuedConnection);/*!< Should move into floorplan object */
    QObject::connect(ptr,&qOrbiter::addFloorplanDeviceCommand, this, &qorbiterManager::setFloorPlanCommand, Qt::QueuedConnection);/*!< Should move into floorplan object */
    QObject::connect(this, &qorbiterManager::getDeviceStatus, ptr, &qOrbiter::getFloorplanDeviceStatus, Qt::QueuedConnection);/*!< Should move into floorplan object */
    QObject::connect(ptr, &qOrbiter::floorplanDeviceStatus, floorplans, &FloorPlanModel::updateStatus, Qt::QueuedConnection );/*!< Should move into floorplan object */
    //mediagrid
    QObject::connect(this, &qorbiterManager::gridStatus, ptr, &qOrbiter::setGridStatus,Qt::QueuedConnection);/*!< Should move into media object */
    QObject::connect(this, SIGNAL(resetSearchParams()), ptr, SLOT(initializeGrid()), Qt::QueuedConnection);/*!< Should move into media object */
    QObject::connect(this, SIGNAL(cancelRequests()), ptr, SLOT(cancelAllRequests()), Qt::QueuedConnection);/*!< Should move into media object */
    // QObject::connect(mediaModel, SIGNAL(pagingCleared()), ptr,SLOT(populateAdditionalMedia()), Qt::QueuedConnection);
    // QObject::connect(ptr, SIGNAL(clearPageGrid()), mediaModel, SLOT(clearForPaging()), Qt::QueuedConnection);
    // QObject::connect(mediaModel, SIGNAL(itemAdded(int)), ptr, SLOT(setCurrentRow(int)),Qt::QueuedConnection);
    //  QObject::connect(this, SIGNAL(clearModel()), mediaModel,SLOT(clear()), Qt::QueuedConnection);
    //  QObject::connect(ptr,SIGNAL(addItem(gridItem*)), mediaModel, SLOT(appendRow(gridItem*)),Qt::QueuedConnection);
    //  QObject::connect(ptr,SIGNAL(gridModelSizeChange(int)), mediaModel, SLOT(setTotalCells(int)), Qt::QueuedConnection);
    //  QObject::connect(ptr, SIGNAL(clearModel()), mediaModel, SLOT(reset()), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(clearFileDetails()), attribFilter, SLOT(clear()), Qt::QueuedConnection);/*!< Should move into attribute filter object */

    // QObject::connect(this, SIGNAL(gridTypeChanged(int)), mediaModel, SLOT(setGridType(int)), Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::authUserMedia, ptr, &qOrbiter::authorizePrivateMedia, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::mediaAuthChanged, userList, &UserModel::setCurrentPrivateUser, Qt::QueuedConnection); /*!< Should move into media object */
    //  QObject::connect(w.userList, SIGNAL(privateUserChanged(int, QString)), ptr, SLOT(setStringParam(int,QString)),Qt::QueuedConnection);
    // QObject::connect(mediaModel,SIGNAL(gridTypeChanged(int)), w.userList, SLOT(unsetPrivate()));

    // QObject::connect(mediaModel, SIGNAL(pauseChanged(bool)), ptr, SLOT(setGridPause(bool)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(keepLoading(bool)), ptr,SLOT(setGridStatus(bool)),Qt::QueuedConnection);/*!< Should move into media object */
    //  QObject::connect(ptr, SIGNAL(modelPagesChanged(int)), mediaModel, SLOT(setTotalPages(int)),Qt::QueuedConnection);
    // QObject::connect(ptr, SIGNAL(pageSeperatorChanged(int)) , mediaModel, SLOT(setSeperator(int)), Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::requestDcePages, ptr, &qOrbiter::requestPage, Qt::QueuedConnection);/*!< Should move into media object */
    QObject::connect(this, SIGNAL(seekGrid(QString)), ptr, SLOT(seekToGridPosition(QString)), Qt::QueuedConnection);/*!< Should move into media object */
    QObject::connect(ptr, &qOrbiter::pageSeperatorChanged, this, &qorbiterManager::setGridSeperator,Qt::QueuedConnection);/*!< Should move into media object */
    QObject::connect(ptr, &qOrbiter::mediaPageChanged, this, &qorbiterManager::setCurrentPage, Qt::QueuedConnection);/*!< Should move into media object */
    QObject::connect(this, &qorbiterManager::dceGridSepChanged, ptr, &qOrbiter::setGridSeperator, Qt::QueuedConnection);/*!< Should move into media object */

    //controls
    QObject::connect(ptr,SIGNAL(newDeviceCommand(AvCommand*)), this, SLOT(addCommandToList(AvCommand*)),Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(populateDeviceCommands(int)), ptr, SLOT(GetAdvancedMediaOptions(int)), Qt::QueuedConnection);
    QObject::connect(ptr,SIGNAL(addDevice(AvDevice*)), this, SLOT(addDeviceToList(AvDevice*)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(bookmarkList(QList<QObject*>)), this, SLOT(showBookmarks(QList<QObject*>)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(extraButton(QString)), ptr, SLOT(extraButtons(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(resendAvCodes()), ptr, SLOT(showAdvancedButtons()), Qt::QueuedConnection );
    QObject::connect(ptr, SIGNAL(deviceAudioLevelChanged(int)), this, SLOT(setDeviceVolume(int)));

    QObject::connect(this, SIGNAL(moveArrowDirection(int)), ptr, SLOT(moveDirection(int)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(signalGoBack()), ptr, SLOT(osdBack()), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(show_dvdMenu()), ptr, SLOT(showMenu()));


    /*Device control related*/
    QObject::connect(this, SIGNAL(resendDeviceCode(int,int)), ptr, SLOT(sendAvCommand(int,int)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(osdChanged(bool)), ptr, SLOT(displayToggle(bool)),Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(setDceVar(int,QString)), ptr, SLOT(setVariable(int,QString)), Qt::QueuedConnection);
    // QObject::connect(ptr,SIGNAL(routerReloading(QString)), this, SLOT(reloadHandler()) );
    QObject::connect(this, SIGNAL(newLightLevel(QString)), ptr, SLOT(adjustRoomLights(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(zoomLevelChanged(QString)), ptr, SLOT(setZoom(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(aspectRatioChanged(QString)), ptr, SLOT(setAspect(QString)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(routerConnectionChanged(bool)), this, SLOT(setConnectedState(bool)), Qt::QueuedConnection);

    //FIXME: below emits error: QObject::connect: Attempt to bind non-signal orbiterWindow::close()
    //QObject::connect (this,SIGNAL, this, SLOT(closeOrbiter()), Qt::DirectConnection);
    QObject::connect(this, SIGNAL(reloadRouter()), ptr, SLOT(quickReload()), Qt::QueuedConnection);
    QObject::connect(ptr,SIGNAL(routerReload()), this, SLOT(reloadHandler()), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(replaceDevice()), this, SLOT(replaceHandler()), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(routerDisconnect()), this, SLOT(disconnectHandler()),Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(reInitialize()), ptr, SLOT(setupEa()), Qt::QueuedConnection);
    //   QObject::connect(this, SIGNAL(deviceNumberChanged(int)), ptr, SLOT(setDeviceId(int)));


    // Media filter
    QObject::connect(ptr, SIGNAL(showFileListMediaType(int)), &mediaFilter, SLOT(setMediaType(int)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(showFileListMediaType(int)), this, SLOT(setGridMediaType(QString)), Qt::QueuedConnection);
    // QObject::connect(&mediaFilter, &MediaFilter::backIndexChanged, this, &qorbiterManager::setCurrentIndex);

    //        QObject::connect(ptr, SIGNAL(showFileListMediaType(int)), this, SLOT(prepareFileList()), Qt::QueuedConnection);
    //QObject::connect(this.mediaFilter, SIGNAL(filterChanged(int)), mediaModel, SLOT(clearAndRequest(int)), Qt::QueuedConnection);
    QObject::connect(&mediaFilter, SIGNAL(genericOptionsChanged(QString)), this, SLOT(genericFilterChanged(QString)), Qt::QueuedConnection);
    QObject::connect(&mediaFilter, SIGNAL(filterStringChanged(QString)), this, SLOT(mediaFilterChanged(QString)), Qt::QueuedConnection);
    QObject::connect(&mediaFilter, SIGNAL(itemSelected(QString)), ptr, SLOT(GetFileInfoForQml(QString)), Qt::QueuedConnection);
    QObject::connect(&mediaFilter, SIGNAL(itemSelected(QString)), this, SLOT(mediaItemSelected(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(gridGoBack()), &mediaFilter, SLOT(goBack()), Qt::QueuedConnection);
    //QObject::connect(ptr, SIGNAL(noMediaFound()), this.mediaFilter, SLOT(noMedia()),Qt::QueuedConnection);

    QObject::connect(ptr, SIGNAL(newAttributeSort(AttributeSortItem*)), attribFilter, SLOT(appendRow(AttributeSortItem*)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(newMediaSubtype(AttributeSortItem*)), mediaTypeFilter, SLOT(appendRow(AttributeSortItem*)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(newFileFormatSort(AttributeSortItem*)), uiFileFilter, SLOT(appendRow(AttributeSortItem*)), Qt::QueuedConnection);

    // generic datagrid signals
    QObject::connect(ptr,SIGNAL(prepareDataGrid(QString,QString,int,int)), this, SLOT(prepareDataGrid(QString,QString,int,int)),Qt::QueuedConnection);
    QObject::connect(ptr,SIGNAL(addDataGridItem(QString,int,int,int,DataGridTable*)), this, SLOT(addDataGridItem(QString,int,int,int,DataGridTable*)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(loadDataGrid(QString,int,QString)), ptr, SLOT(loadDataGrid(QString,int,QString)),Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(updateItemData(QString,int,int,QVariant)), this, SLOT(updateItemData(QString,int,int,QVariant)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(loadDataForDataGrid(QString,QString,int,QString,int,int,int,QString)), ptr, SLOT(loadDataForDataGrid(QString,QString,int,QString,int,int,int,QString)), Qt::QueuedConnection);

    //now playing signals
    QObject::connect(this, SIGNAL(screenChange(QString)), ptr, SLOT(setCurrentScreen(QString)));
    QObject::connect(this, SIGNAL(requestStreamImage()), ptr, SLOT(getStreamingVideo()), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(requestStoredMediaImage(QString)), ptr, SLOT(grabScreenshot(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(requestVideoFrame()), ptr, SLOT(getStreamingVideo()), Qt::QueuedConnection );
    QObject::connect(ptr, SIGNAL(videoGrabReady(QImage)), this, SLOT(setMediaScreenShot(QImage)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(screenShotReady(QImage)), this, SLOT(setMediaScreenShot(QImage)), Qt::QueuedConnection);

    QObject::connect(ptr, SIGNAL(mediaTypeChanged(int)), this, SLOT(setMediaType(int)), Qt::QueuedConnection);

    QObject::connect(this, SIGNAL(seekPositionChanged(QString)), ptr, SLOT(setPosition(QString)), Qt::QueuedConnection ); /*!< Put in timecode object? */
    QObject::connect(this, SIGNAL(jogToPosition(QString)),ptr, SLOT(JogStream(QString)), Qt::QueuedConnection);          /*!< Put in timecode object? */

    //attributes
    QObject::connect(ptr, &qOrbiter::screenshotVariablesReady, this, &qorbiterManager::showScreenShotVariables,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::addScreenShotVar, this, &qorbiterManager::setScreenShotVariables, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::screenShotReady, this, &qorbiterManager::setMediaScreenShot,Qt::QueuedConnection);
    QObject::connect (this, &qorbiterManager::saveMediaScreenShot, ptr, &qOrbiter::saveScreenAttribute,Qt::QueuedConnection);
    QObject::connect(this, &qorbiterManager::isOsd, ptr, &qOrbiter::handleOSDstatusChanged, Qt::QueuedConnection);

    /*Remote command signal */
    QObject::connect(ptr, SIGNAL(dceGuiCommand(int)), this, SLOT(handleDceGuiCommand(int)), Qt::QueuedConnection);


    QObject::connect(ptr, SIGNAL(timecodeEvent(QString,QMap<long,std::string>)), m_lRooms, SLOT(setEaTimeCode(QString,QMap<long,std::string>)), Qt::QueuedConnection);
    QObject::connect(ptr, SIGNAL(timecodeEvent(int,QMap<long,std::string>)), m_lRooms, SLOT(setEaTimeCode(int,QMap<long,std::string>)), Qt::QueuedConnection);
    QObject::connect(m_lRooms, &LocationModel::seekToTime, ptr, &qOrbiter::JogStream, Qt::QueuedConnection );

    QObject::connect(this, SIGNAL(ejectDiscDrive(long,int)), ptr, SLOT(ejectDisc(long,int)), Qt::QueuedConnection);

    //config options

    QObject::connect(ptr, &qOrbiter::useQueueInsteadOfInstantPlayChanged, this, &qorbiterManager::handleUseQueueChanged , Qt::QueuedConnection);
    return true;

}
