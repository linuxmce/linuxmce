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
qorbiterManager::qorbiterManager(QQuickView *view, int testSize, SettingInterface *appSettings, QObject *parent) :
    #elif ANDROID && QT5
qorbiterManager::qorbiterManager(QQuickView *view, AndroidSystem *jniHelper,SettingInterface *appSettings,  QObject *parent) :
    #elif ANDROID
qorbiterManager::qorbiterManager(QDeclarativeView *view, AndroidSystem *jniHelper, SettingInterface *appSettings,  QObject *parent) :
    #else
qorbiterManager::qorbiterManager(QDeclarativeView *view, int testSize,SettingInterface *appSettings,  QObject *parent) :

    #endif
    QObject(parent),qorbiterUIwin(view),
    appHeight(view->height()),
    appWidth(view->width()),
    settingsInterface(appSettings),
    m_style(0),
    m_fontDir(""),
    iPK_Device(-1)
{
#ifdef __ANDROID__
    int testSize=-1;
#endif
    if(settingsInterface->ready){

        if(restoreSettings()){
            qDebug() << "Settings Restored.";
        }
    }
    m_testScreenSize =testSize;
    m_fontDir.setPath(QStandardPaths::standardLocations(QStandardPaths::FontsLocation).first());
    qDebug() <<m_fontDir.entryList();

    QString mlocale = QLocale::system().name().append(".qm");
    qDebug() << "Local set to "<< mlocale;
    if(  translator.load(":/lang/translations/"+mlocale) ) {
        qDebug() << "Translator loaded default translation";
    } else {
        qDebug() << translator.isEmpty();
    }

    qApp->installTranslator(&translator);

#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
    selector=new QQmlFileSelector(view->engine());
    m_selector=new QFileSelector(this);
    selector->setSelector(m_selector);
    m_screenInfo = new ScreenInfo();

    connect(m_screenInfo, SIGNAL(screenSizeChanged()), this, SLOT(resetScreenSize()));
    connect(qorbiterUIwin, SIGNAL(screenChanged(QScreen*)), this , SLOT(handleScreenChanged(QScreen*)));
    connect(qorbiterUIwin, SIGNAL(heightChanged(int)), this, SLOT(setAppH(int)));
    connect(qorbiterUIwin, SIGNAL(widthChanged(int)), this, SLOT(setAppW(int)));
    connect(qorbiterUIwin->engine(), SIGNAL(warnings(QList<QQmlError>)), this, SLOT(handleViewError(QList<QQmlError>)));
    resetScreenSize();
    qorbiterUIwin->rootContext()->setContextProperty("screenInfo", m_screenInfo);

#endif

#ifdef __ANDROID__ || defined(Q_OS_IOS)
    //qorbiterUIwin->showMaximized();
    checkOrientation(Qt::LandscapeOrientation);
#endif

    mediaPlayerID=-1; orbiterInit=true; m_ipAddress="";  m_bStartingUp= true;  homeNetwork=false;  alreadyConfigured = false;  iFK_Room = -1;
    iea_area= -1; bAppError = false; isPhone = 0; hostDevice=HostSystemData::OTHER_EMBEDDED; appConfigPath=""; status="starting";
    setUsingExternal(false); disconnectCount=0;  reloadCount=0;
    i_current_command_grp = 0;  i_current_mediaType =0;  videoDefaultSort = "13";  audioDefaultSort = "2"; photoDefaultSort = "13";
    gamesDefaultSort = "49"; i_currentFloorplanType = 0; backwards = false;  screenSaverTimeout = 60; screenPowerOffTimeout = 60;

#ifndef __ANDROID__
    b_localLoading = false; /*! this governs local vs remote loading. condensed to one line, and will be configurable from the ui soon. */
#elif defined QT5 && ANDROID || defined(ANDROID)
    androidHelper = jniHelper;
    //   qorbiterUIwin->rootContext()->setContextProperty("android",androidHelper);
    b_localLoading = false;
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

    appHeight = qorbiterUIwin->height() ;
    appWidth = qorbiterUIwin->width() ;
    gotoScreenList = new QStringList();
    // ScreenSaver = new ScreenSaverClass(this);
    // qorbiterUIwin->engine()->rootContext()->setContextProperty("screensaver", ScreenSaver);
    // Prepares models in this qt thread so owner thread is not QML as they would have been if they were created later
    prepareModelPool(5);

    /*Needs Doin at construction */
    userList = new UserModel( new UserItem, this);
    orbiterInit=true;
    QObject::connect(&mediaFilter, SIGNAL( currentMediaTypeChanged(QString)), this, SLOT(setGridMediaType(QString)));
    QObject::connect(&mediaFilter, SIGNAL(newMediaFilter()), SLOT(updateMediaString()));

    //    QObject::connect(&mediaFilter, SIGNAL(attributeGenresChanged(QString)), this, SLOT(setGridAttributeGenres(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(attributeTypeSortChanged(QString)), this, SLOT(setAttributeTypeSort(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(fileFormatChanged(QString)), this, SLOT(setGridFileFormat(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(pkAttributeChanged(QString)), this, SLOT(setGridPkAttribute(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(pkUsersChanged(QString)), this, SLOT(setGridPkUsers(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(usersPrivateChanged(QString)), this, SLOT(setGridUsersPrivate(QString)));
    //    QObject::connect(&mediaFilter, SIGNAL(lastViewedChanged(QString)), this, SLOT(setGridLastViewed(QString)));

    emit orbiterInitialized();
}

qorbiterManager::~qorbiterManager(){
    // clear any generic datagrids
    clearAllDataGrid();
    cleanupData();
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
        if(!currentScreen.contains("187")){
            gotoScreenList->append(currentScreen);
        }

    } else {
        setDceResponse("screenchange() FAILED, sending request screen");

    }
}

//! Send the user back to the previous screen in the list
void qorbiterManager::goBacktoQScreen()
{


    if(!gotoScreenList->isEmpty()){
        gotoScreenList->removeLast();
        setDceResponse("Starting backwards screen switch");
        if(gotoScreenList->isEmpty()){
            gotoScreenList->append("Screen_1.qml");
        }
        QVariant screenname= QVariant::fromValue(gotoScreenList->last());
        QObject *item = qorbiterUIwin->rootObject();
        setDceResponse("About to call screenchange() for " + gotoScreenList->last());
        if (QMetaObject::invokeMethod(item, "screenchange", Qt::QueuedConnection, Q_ARG(QVariant, screenname))) {
            setDceResponse("Done call to backwards screenchange()");
            currentScreen=gotoScreenList->last();
            screenChange(currentScreen);
        } else {
            setDceResponse(" backwards screenchange() FAILED");
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
    setDceResponse("Starting Manager with connection to :"+QString::fromStdString(sRouterIP));
    if(sRouterIP.length() < 7){
        setDceResponse("Ip is empty, using alternate :"+this->m_ipAddress);
        sRouterIP = m_ipAddress.toStdString();
    }
    QObject::connect(this,SIGNAL(screenChange(QString)), this, SLOT(gotoQScreen(QString)));
    if(!mb_useLocalSkins){
        setupNetworkSkins();
    } else {

        swapSkins("default");
        emit setSkinStatus(true);
    }
}

void qorbiterManager::initiateRestart(){
    emit restartOrbiter();
    screenChange("Screen_1.qml");
}

//this functions purpose is to change the UI to the new skin pointed to. It will evolve to encompass orbiter regen to some extent
/*!
 * \brief qorbiterManager::refreshUI: This function re-initializes the qml engines skin.
 * \param url - The relative or absolute path of the skin to the binary.
 */
void qorbiterManager::refreshUI(QUrl url){

#if (QT5)
    // qorbiterUIwin->setResizeMode(QQuickView::SizeRootObjectToView);
#else
    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif

    qorbiterUIwin->engine()->clearComponentCache();
    qorbiterUIwin->rootContext()->setBaseUrl(url);
    qorbiterUIwin->setSource(url);
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

    qDebug() << "Processing config.";
    if(!alreadyConfigured){
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
        qDebug() << "Error in config";
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
        /*        if (fpType == 7)
            fpType = 6;
        else */if (fpType == 4) // Move cameras to the security floorplan
            fpType = 1;
        QImage icon;
        floorplans->appendRow(new FloorplanDevice( name, fp_deviceno, fp_deviceType, fpType, position, floorplans));
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
    m_lRooms = new LocationModel(new LocationItem, this);   //roomlistmodel
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
        if(m_lRooms->check(m_val)){
            LocationItem *t= m_lRooms->find(m_name);
            t->addEa(ea, m_iEA);
        }
        else{
            m_lRooms->appendRow(new LocationItem(m_name, m_val, m_iType, imagePath, m_isHidden, m_lRooms));
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
    }


    setDceResponse("Properties Done");

    setDceResponse("Setting location");
    setActiveRoom(iFK_Room, iea_area);
    setCurrentUser(QString::number(iPK_User));


    //-----setting up the FILEFORMAT model------------------------------------------------------------------------

    if(!alreadyConfigured){
        this->qorbiterUIwin->rootContext()->setContextProperty("fileformatmodel", uiFileFilter); //custom fileformatmodel for selection filter item
        connect(uiFileFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
        connect(this, SIGNAL(resetFilter()), uiFileFilter, SLOT(clear()));

        //-----setting up the MEDIASUBTYPE model------------------------------------------------------------------------
        this->qorbiterUIwin->rootContext()->setContextProperty("mediatypefilter", mediaTypeFilter); //custom mediatype selection model

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
        connect(genreFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
        QObject::connect(this, SIGNAL(resetFilter()), genreFilter, SLOT(resetStates()));

        //-----setting up the ATTRIBUTE model------------------------------------------------------------------------
        this->qorbiterUIwin->rootContext()->setContextProperty("attribfilter", attribFilter); //custom mediatype selection model
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

    if(!alreadyConfigured && !mb_useLocalSkins){
        setOrbiterStatus(true);
    }

    alreadyConfigured=true;


}

void qorbiterManager::getConfiguration()
{
    QNetworkRequest updateDevice;
    QNetworkAccessManager *ud= new QNetworkAccessManager();
    qDebug() << "Getting config";
    updateDevice.setUrl("http://"+m_ipAddress+"/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device ));
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

    if(mb_useLocalSkins){

        incSkin="default";
        currentSkin=incSkin;

        QQmlComponent styleData(qorbiterUIwin->engine(), QUrl(m_localQmlPath+"skins/"+incSkin+"/Style.qml"), QQmlComponent::PreferSynchronous);
        m_style=styleData.create();

        refreshUI(QUrl(m_localQmlPath+"skins/"+incSkin+"/Main.qml"));
        return;
    }

#ifdef WIN32
    incSkin = "default";
#endif
#ifdef debug
    qDebug() << tskinModel->rowCount();
#endif
    checkOrientation(qorbiterUIwin->size());
    if (tskinModel->rowCount() > 0){
        emit skinMessage("Setting Skin to:" +incSkin);
        emit skinMessage("Got it from the model : " + tskinModel->m_baseUrl.toString());
        setImagePath(tskinModel->m_baseUrl.toString()+"/"+incSkin+"/img/");
        //load the actual skin entry point
        qDebug() << "skin break";
        currentSkin = incSkin;
        m_style=tskinModel->currentItem;
#ifdef __ANDROID__
        qorbiterUIwin->engine()->rootContext()->setContextProperty("style", tskinModel->currentItem);
        qorbiterUIwin->engine()->rootContext()->setContextProperty("Style", m_style);
#else
        qorbiterUIwin->engine()->rootContext()->setContextProperty("Style", m_style);
#endif

#if (QT5)
        QObject::connect(qorbiterUIwin, SIGNAL(statusChanged(QQuickView::Status)),
                         this, SLOT(skinLoaded(QQuickView::Status)));
#else
        QObject::connect(qorbiterUIwin, SIGNAL(statusChanged(QDeclarativeView::Status)),
                         this, SLOT(skinLoaded(QDeclarativeView::Status)));
#endif

#ifdef QT4
        //qorbiterUIwin->engine()->clearComponentCache();
#elif QT5

#endif
        QMetaObject::invokeMethod(this, "refreshUI", Qt::QueuedConnection, Q_ARG(QUrl, tskinModel->m_entryUrl));
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
        writeConfig();
        startOrbiter();
    }
}

void qorbiterManager::updateSelectedAttributes(QString attributes)
{
    qDebug() << "Updated Attributes::" << attributes.split("|");
    QStringList ta = attributes.split("|");
    setSubType(ta.at(1));
    setGridMediaType(ta.at(0));
    setGridFileFormat(ta.at(2));
    setGridPkUsers(ta.at(7));
    setGridMediaSources(ta.at(4));
    setGridPkAttribute(ta.at(9));
    setGridLastViewed(ta.at(8));
    setGridAttributeGenres(ta.at(3));
    setGridUsersPrivate(ta.at(5));
    setGridAttributeTypeSort(ta.at(6));
}


//takes care of un-registering the orbiter from the DCERouter and then shutting down
void qorbiterManager::closeOrbiter()
{
    if(writeConfig()){
        setDceResponse("Shutting Down");
    }
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Exiting, Unregistering 1st");
    //  emit unregisterOrbiter((userList->find(sPK_User)->data(4).toInt()), QString(iFK_Room), iea_area );

    emit orbiterClosing();
    QApplication::quit();
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

void qorbiterManager::addDataGridItem(QString dataGridId, int PK_DataGrid, int indexStart, int numRows, DataGridTable* pTable)
{
    qDebug() << "Adding item";
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
                GenericModelItem* pItem = DataGridHandler::GetModelItemForCell(PK_DataGrid, pTable, row);
                // TODO: stop if datagrid model is reset or request stopped
                m_mapDataGridModels[dataGridId]->insertRow(row, pItem);

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
    qDebug() << "manager.clearDataGrid() " << dataGridId;

    if(!modelPoolLock.tryLockForWrite()){
        qDebug() << "clearDatagrid("<<dataGridId<<"):: failed to lock mutex.";
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
    qDebug() << "manager.clearDataGrid() end";
    emit modelChanged();
}

void qorbiterManager::clearAllDataGrid()
{
    qDebug() << "manager.clearAllDataGrid()";
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
    qDebug() << "manager.clearAllDataGrid() end";
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
    qDebug() << "Fetching dg " << dataGridId << ": type : " << PK_DataGrid;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "getDataGridModel() id = %s", dataGridId.toStdString().c_str());
    GenericFlatListModel* pModel = NULL;
    qDebug() <<" Searching for model, id ::" << dataGridId;
    if (!m_mapDataGridModels.contains(dataGridId))
    {
        // GenericFlatListModels are kept in a pool in qorbitermanager, because we want the
        // Objects to belong to this thread, and not the QML thread, which is the one that calls
        // this method. Creating them in this method would create them in the QML thread.
        // lock this section so we don't prepare the same model more than once

        qDebug() <<dataGridId << " doesnt exist creating new one.";

        if(!modelPoolLock.tryLockForWrite()){
            qDebug() << "Mutex locked, comeback later.";
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
                option = QString::number(iPK_User).append("|%");
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
            qDebug() << "Checks complete, fetching grid " << dataGridId ;
            emit loadDataGrid(dataGridId, PK_DataGrid, option); // loads data
        } else {
            pModel = m_mapDataGridModels[dataGridId];
        }

        modelPoolLock.unlock();

    } else {
        pModel = m_mapDataGridModels[dataGridId];
        qDebug() << "Model found with " << pModel->rowCount() << "row, returning;";
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
    qDebug() << "genericFilterChanged() for " << dataGridId;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::genericFilterChanged() start");
    GenericFlatListModel* pModel = NULL;
    if (m_mapDataGridModels.contains(dataGridId))
    {
        qDebug() << "genericFilterChanged()::" << "existing model type, checking filter of " << mediaFilter.getGenericOptions();
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



void qorbiterManager:: setLocation(const int &room, const int &ea)
{
    iFK_Room = room;
    iea_area = ea;
    sEntertainArea = QString::number(iea_area).toStdString();
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
    setCurrentScreen("WebRegen.qml");
}

void qorbiterManager::regenComplete(int i)
{
    if (i == 0)
    {
        //processConfig(iPK_Device);
    }
    else
    {
        setCurrentScreen("LoadError.qml");
    }
}

QString qorbiterManager::adjustPath(const QString &path)
{

    qDebug() <<Q_FUNC_INFO << "::Incoming Path" << path;

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



void qorbiterManager::mountMediaDevices()
{
    /* Set to be refactored */

    //    qDebug() << "Mounting LinuxMCE Home Dir";

    //    QString dirCmd = "";
    //#ifndef RPI
    //    dirCmd  = "gksudo";
    //#endif
    //    QDir mntDir;
    //    mntDir.setPath("/mnt/remote/LinuxMCE");
    //    qDebug() << "Mount directory exists? " << mntDir.exists();
    //    if(!mntDir.exists()){
    //        QProcess *mkPath = new QProcess(this);
    //        QStringList dArgs;
    //        dArgs.append("mkdir -p /mnt/remote/LinuxMCE");
    //        mkPath->start(dirCmd, dArgs);
    //        mkPath->waitForFinished(10000);
    //        while(mkPath->state()==QProcess::Running){
    //            qDebug() << mkPath->readAllStandardOutput();
    //        }
    //    }

    //    qWarning() << mntDir.entryList().count();
    //    if(mntDir.exists() && mntDir.entryList().length() ==2){
    //       qWarning() << mntDir.path() << " is not mounted, mounting";

    //        QString mountProg = "gksudo";
    //#ifdef RPI
    //        mountProg="";
    //#endif
    //        QStringList args;
    //        args.append(QString("mount -t nfs "+m_ipAddress+":/home "+ mntDir.path()+"" ));
    //        QProcess *mountProcess = new QProcess(this);
    //        mountProcess->start(mountProg, args);
    //        mountProcess->waitForFinished(10000);

    //        qDebug() << "Process Status ::" <<mountProcess->state();
    //        if(mountProcess->state()== QProcess::FailedToStart){
    //            qWarning() << "command failed to start!";
    //            qDebug() << mountProcess->readAllStandardError();
    //            qDebug() << mountProcess->errorString();
    //        }
    //        qWarning() << "QProcess Exiting, state is :"<< mountProcess->state();
    //        qWarning() << "Process exited with::"<< mountProcess->exitCode();
    //    }


    //    if(mntDir.entryList(QDir::NoDotAndDotDot).count() == 0){
    //        qDebug() << "Existing files in path" << mntDir.entryList(QDir::NoDotAndDotDot).count();
    //
    //    }

    //    for(int dc =0; dc < storageDevices.count(); dc++){
    //        qDebug()<< "Starting process for device " << storageDevices.at(dc).toMap()["Description"].toString() << " #"<<storageDevices.at(dc).toMap()["Device"].toString();
    //        int d = storageDevices.at(dc).toMap()["Device"].toInt();
    //        QString dirCmd = "";
    //#ifndef RPI
    //        dirCmd  = "gksudo";
    //#endif
    //        QDir mntDir;
    //        mntDir.setPath("/mnt/remote/"+QString::number(d));
    //        qDebug() << mntDir.exists();
    //        if(!mntDir.exists()){
    //            QProcess *mkPath = new QProcess(this);
    //            QStringList dArgs;
    //            dArgs.append("mkdir -p /mnt/remote/"+QString::number(d));
    //            mkPath->start(dirCmd, dArgs);
    //            mkPath->waitForFinished(10000);

    //        }
    //        else{

    //        }

    //        if(mntDir.entryList(QDir::NoDotAndDotDot).count() == 0){
    //            qDebug() << "Existing files in path" << mntDir.entryList(QDir::NoDotAndDotDot).count();
    //            QString mountProg = "gksudo";
    //#ifdef RPI
    //            mountProg="";
    //#endif
    //            QStringList args;

    //            args.append(QString("mount -t nfs "+m_ipAddress+":/mnt/device/"+QString::number(d) + " /mnt/remote/"+QString::number(d)) +" -o vers=3" );
    //            QProcess *mountProcess = new QProcess(this);
    //            mountProcess->start(mountProg, args);
    //            mountProcess->waitForFinished(10000);
    //            qDebug() << "Process Status ::" <<mountProcess->state();
    //            if(mountProcess->state()== QProcess::FailedToStart){
    //                qWarning() << "command failed to start!";
    //                qDebug() << mountProcess->readAllStandardError();
    //                qDebug() << mountProcess->errorString();
    //            }
    //            qWarning() << "QProcess Exiting, state is :"<< mountProcess->state();
    //            qWarning() << "Process exited with::"<< mountProcess->exitCode();
    //        }
    //    }

    //    QString dirCmd = "gksudo";
    //    QDir mntDir;
    //    mntDir.setPath("/mnt/remote/dvd");
    //    qDebug() << mntDir.exists();
    //#ifdef RPI
    //    dirCmd="";
    //#endif
    //    if(!mntDir.exists()){
    //        QProcess *mkPath = new QProcess(this);
    //        QStringList dArgs;
    //        dArgs.append("mkdir -p /mnt/remote/dvd");
    //        mkPath->start(dirCmd, dArgs);
    //        mkPath->waitForFinished(10000);
    //    }


}

void qorbiterManager::getMediaDevices()
{
    qDebug() << "Getting mount devices";
    QNetworkAccessManager * m = new QNetworkAccessManager();
    QNetworkRequest req;
    req.setUrl(QUrl("http://"+m_ipAddress+"/lmce-admin/qOrbiterGenerator.php?m"));
    m->get(req);
    QObject::connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(setMediaDevices(QNetworkReply*)));
}

void qorbiterManager::setMediaDevices(QNetworkReply *d)
{
    QString str= d->readAll().replace("\n", "");
    qDebug() <<"Recieved mount devices";
    qDebug() <<"Devices" << str;

    QScriptEngine script;
    QVariantList p = script.evaluate(str).toVariant().toList();
    foreach (QVariant x, p){
        QVariantMap l = x.toMap();
        qDebug() << l["Device"].toString();
    }
    storageDevices = p;

#ifndef ANDROID
    //    if(!storageDevices.isEmpty()){
    //        mountMediaDevices();
    //    }
#endif
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

        qDebug() << localSkins.count();
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
    setReloadStatus(true);

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
}


void qorbiterManager::getFloorplanDevices(int floorplantype){

    for (int i=0; i < floorplans->rowCount(); i++) {
#ifdef debug
        qDebug() << floorplans->index(i, 0, QModelIndex()).data(1);
#endif

        if(floorplans->index(i).data(6).toInt() == floorplantype)  {

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
    qDebug() << "Current Status ==> " << status;


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
    } else {

    }

    //  getConfiguration(); /* Connect to skins ready signal */
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
    QDomDocument localConfig;
    QString xmlPath;

    QFile test(":///config.xml");
    if(test.exists()){
        qDebug() << "QRC file found";
    } else {
        qDebug() << "QRC file not found at " << test.fileName();
    }

#ifdef Q_OS_IOS

    QDir dir;
    dir.setPath(QDir::homePath());
    qDebug() << dir.absolutePath()+"/Library/Application_Support";
    xmlPath =dir.absolutePath()+"/Library/Application_Support/LinuxMCE/config.xml";

    setMobileStorage(dir.absolutePath()+"/Library/Application_Support/LinuxMCE");
    if(createMobileConfig()){
        xmlPath=mobileStorageLocation+"/config.xml";
    }

#elif defined(Q_OS_ANDROID)
#ifdef NECESSITAS

    qDebug() << "Mobile Storage Location::" << mobileStorageLocation;
    if(setupMobileStorage(androidHelper->externalStorageLocation)){
        xmlPath = mobileStorageLocation+"/config.xml" ;
    }
    else{

    }
#elif QT5 && ANDROID
    if(setupMobileStorage(androidHelper->externalStorageLocation)){
        xmlPath = mobileStorageLocation+"/config.xml" ;
        qCritical() <<xmlPath;
    }
#endif
#elif defined(WIN32)
    xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#elif RPI
    xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#else
    xmlPath = QDir::homePath()+"/linuxmce/config.xml";
#endif

    QFile localConfigFile;
    //**todo!! - add function for 1st run on android that copies the file to the xml path, then performs checks. we cannot install directly there.
    qDebug() << "Xml Path is " << xmlPath;


#ifdef __ANDROID__
    if (createMobileConfig())
    {
        localConfigFile.setFileName(xmlPath);
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



    // existence check
    QFileInfo chk(xmlPath);

    if(!chk.exists()){

        setDceResponse("Did not find config.xml file in application path, searching /usr/pluto/bin");
        this->logQtMessage("Did not find config.xml file in application path, searching /usr/pluto/bin");
        localConfigFile.setFileName("/usr/pluto/bin/config.xml");
        appConfigPath="/usr/pluto/bin/config.xml";
    } else {
        localConfigFile.setFileName(xmlPath);
        setDceResponse("Found config.xml in app path, reading "+localConfigFile.fileName());
        appConfigPath=xmlPath;
    }

    if (!localConfigFile.open(QFile::ReadWrite)) {
        setDceResponse(localConfigFile.errorString());
        setDceResponse("Not Able to read config::"+localConfigFile.fileName());
        setInternalIp("192.168.80.1");
        currentSkin="default";
        setDceResponse("Failed to read config, exiting func");
        return false;
    } else {
        setDceResponse("Reading Local Config");
        QByteArray tDoc;
        tDoc = localConfigFile.readAll();
        //  localConfigFile.close();

        if (!localConfig.setContent(tDoc))
        {

            setDceResponse("Could not parse config!");
            return false;
        }
        else
        {
            setDceResponse("Reading elements");
            QDomElement configVariables = localConfig.documentElement().toElement();
            if(configVariables.namedItem("firstrun").attributes().namedItem("id").nodeValue()=="true")
            {
                setRunStatus(true);
            }
            else {
                setRunStatus(false);
            }


            if(!configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue().isEmpty())
            {

                setInternalIp(configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue());}
            else
            {
                setInternalIp("192.168.80.1");

            }
#ifdef QT5
            QString configSkin = configVariables.namedItem("qt5skin").attributes().namedItem("id").nodeValue();
#else
            QString configSkin = configVariables.namedItem("skin").attributes().namedItem("id").nodeValue();
#endif
            if (configSkin.isEmpty()){
                currentSkin = "noir";
            }

            QString tRp = configVariables.namedItem("routerport").attributes().namedItem("id").nodeValue();
            if(tRp.isEmpty()){
                setRouterPort("80");
            }
            else{
                setRouterPort(tRp);
            }

            if(configVariables.namedItem("networkload").attributes().namedItem("id").nodeValue()=="true"){
                b_localLoading=false;
            } else {
                b_localLoading=true;
            }

            qDebug() << Q_FUNC_INFO << "Reading device number ";
            if(configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong() !=0)
            {
                setDeviceNumber(iPK_Device = configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong());}
            else
            {
                setDeviceNumber(iPK_Device = -1)
                        ;}

            if(!configVariables.namedItem("externalip").attributes().namedItem("id").nodeValue().isEmpty() )
            {setExternalIp(configVariables.namedItem("externalip").attributes().namedItem("id").nodeValue());}
            else
            {setExternalIp("fill.me.in.com");}

            if(!configVariables.namedItem("debug").attributes().namedItem("id").nodeValue().toInt() == 0 )
            {setDebugMode(false);}
            else
            {setDebugMode(true);}

            if(!first_run){
                int ty = configVariables.namedItem("phone").attributes().namedItem("id").nodeValue().toInt();
                qDebug() << "Type::"<<ty;
                switch(ty){
                case 1:
                    qDebug() << "Phone read from config.";
                    setFormFactor(1);
                    break;
                case 2:
                    qDebug() << "Tablet read from config.";
                    setFormFactor(2);
                    break;
                default:
                    qDebug() << "error, setting default.";
                    setFormFactor(1);
                    break;

                }
            }

            QString ext = configVariables.namedItem("lastconnect").attributes().namedItem("id").nodeValue();
            if(ext=="internal"){
                setUsingExternal(false);
            } else {
                setUsingExternal(true);
            }
        }
#ifdef __ANDROID__
        setDceResponse("Finished READ of config.xml at::"+mobileStorageLocation);
#else
        setDceResponse("Finished READ of config.xml at::"+appConfigPath);
#endif
        return true;
    }
}



bool qorbiterManager::writeConfig()
{
    settingsInterface->setOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID, iPK_Device);
    qDebug() <<  Q_FUNC_INFO << settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID).toInt();
    /* old below this line and will be replaced */
    qDebug() << Q_FUNC_INFO;
    //   setDceResponse( QString::fromLocal8Bit(Q_FUNC_INFO) << "Writing Local Config");
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

    switch (paramType)
    {
    case 0:
        setGridMediaType(param);
        break;
    case 1:
        setSubType(param);
        break;
    case 2:
        setGridFileFormat(param);
        break;
    case 3:
        setGridAttributeGenres(param);
        break;
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

    case 5:
        setGridUsersPrivate(param+",0");
        break;
    case 6:
        if (param.contains("!P"))    {
            break;
        } else {
            setGridAttributeTypeSort(param);
            break;
        }
    case 7:
        setGridPkUsers(param);
        break;
    case 8:
        setGridLastViewed(param);
        break;
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
    setDceResponse("Grid Initialized");
}

bool qorbiterManager::goBackGrid()
{
    removeRestoreIndex();
    return mediaFilter.goBack();
}

void qorbiterManager::mediaItemSelected(QString itemID)
{
    filedetailsclass->setVisible(true);
    filedetailsclass->setFile(itemID);
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
    qDebug() << "Device " << t["device"].toInt();
    FloorplanDevice *p = floorplans->find(t["device"].toInt());
    if(p){

        QVariantMap b;
        b.insert("commands", t["commands"]);
        //  qDebug() << b;
        p->setDeviceCommand(b);
        //       foreach(QVariant cmd , t["commands"].toMap()){
        //           QVariantMap l = cmd.toMap();
        //           qDebug() << l["command_name"] << " => " << l["command_number"];      }
    }
}

void qorbiterManager::getDeviceState(int PK_Device, string* data)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::getDeviceState");
    CMD_Get_Device_State getDeviceState(iPK_Device, iPK_Device_GeneralInfoPlugin, PK_Device, data);
    sendDceCommand(getDeviceState);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "qorbiterManager::getDeviceState done");
}

void qorbiterManager::getDeviceStatus(int PK_Device, string* data)
{
    CMD_Get_Device_Status getDeviceStatus(iPK_Device, iPK_Device_GeneralInfoPlugin, PK_Device, data);
    sendDceCommand(getDeviceStatus);
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

void qorbiterManager::setHouseMode(QString pass, int mode)
{
    emit newHouseMode(pass, mode);
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



void qorbiterManager::regenError(QProcess::ProcessError)
{
    setDceResponse("Error! Reloading existing config for device " + iPK_Device);
    //processConfig(iPK_Device);
}

bool qorbiterManager::cleanupData()
{
    qDebug() << "Cleaning up data";
    roomLights->clear();                 //current room scenarios model


    if(roomMedia){
        roomMedia->clear();
    }
    tskinModel->clear();

    roomClimate->clear();
    roomClimateScenarios.clear();
    roomLightingScenarios.clear();
    roomMediaScenarios.clear();
    roomSecurity->clear();
    roomSecurityScenarios.clear();
    roomTelecom->clear();
    roomTelecomScenarios.clear();
    nowPlayingButton->resetData();
    filedetailsclass->clear();
    devices->clear();
    myOrbiters->clear();
    deviceCommands->clear();
    floorplans->clear();
    nowPlayingButton->resetData();
    filedetailsclass->clear();
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
        setCurrentScreen("Screen_1.qml");
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

    qDebug() <<"qOrbiterManager::setActiveSkin("<<name<<")";
    if(mb_useLocalSkins){
        swapSkins(name);
        return;
    }


    if(name=="splash_fallback"){

    }
    else {
        tskinModel->setActiveSkin(deviceTemplate == DEVICETEMPLATE_OnScreen_qOrbiter_CONST ? "default" : name);
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
    filedetailsclass = new FileDetailsClass(this);
    qorbiterUIwin->rootContext()->setContextProperty("filedetailsclass" ,filedetailsclass);
    filedetailsclass->clear();

    nowPlayingButton = new NowPlayingClass();
    qorbiterUIwin->rootContext()->setContextProperty("dcenowplaying" , nowPlayingButton);

    //screen parameters class that could be extended as needed to fetch other data
    ScreenParameters = new ScreenParamsClass(this);
    qorbiterUIwin->rootContext()->setContextProperty("screenparams", ScreenParameters);

    //floorplan model initialization for slots in main.cpp
    floorplans = new FloorPlanModel( new FloorplanDevice , this);

    //----------------Security Video setup
    SecurityVideo = new SecurityVideoClass(this);
    qorbiterUIwin->rootContext()->setContextProperty("securityvideo", SecurityVideo);

    attribFilter = new AttributeSortModel(new AttributeSortItem,6, this);
    uiFileFilter = new AttributeSortModel(new AttributeSortItem,2, this);
    mediaTypeFilter = new AttributeSortModel(new AttributeSortItem,1, this);
    genreFilter = new AttributeSortModel(new AttributeSortItem,3, this);
}

void qorbiterManager::setupEarlyContexts()
{
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
    QObject::connect(qorbiterUIwin, SIGNAL(contentOrientationChanged(Qt::ScreenOrientation)), this, SLOT(checkOrientation(Qt::ScreenOrientation)));

#endif

    QObject::connect(this, SIGNAL(orbiterReady(bool)), this, SLOT(showUI(bool)));
    QObject::connect(this, SIGNAL(skinDataLoaded(bool)), SLOT(showUI(bool)));
    QObject::connect(qorbiterUIwin->engine(), SIGNAL(quit()), this, SLOT(closeOrbiter()));
}

void qorbiterManager::setupUiSelectors(){
    /*!
     * \todo move buildtype / qrc path code to its own function and return it here
     */

#ifdef for_desktop

#ifdef QT4_8
    buildType = "/qml/desktop";
    setHostDevice(3);
#elif QT5 && defined RPI
    buildType = "/qml/qt5-desktop";
    setHostDevice(1);
#elif QT5 && !defined RPI && !defined ANDROID
    setHostDevice(3);
    buildType="/qml/qt5-desktop";
#endif
    qrcPath = buildType+"/Splash.qml";
#elif  WIN32
    setHostDevice(HostSystemData::WINDOWS_DESKTOP);
    buildType="/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
#elif defined (for_freemantle)
    buildType = "/qml/freemantle";
    qrcPath = "qrc:freemantle/Splash.qml";
#elif defined (for_harmattan)
    buildType="/qml/harmattan";
    qrcPath = "qrc:harmattan/Splash.qml";
#elif defined (Q_OS_MACX)
    setHostDevice(HostSystemData::OSX_DESKTOP);
    buildType="/qml/qt5-desktop";
    qrcPath = "qrc:osx/Splash.qml";
#elif defined (RPI)
    buildType="/qml/qt5-desktop";
    setHostDevice(HostSystemData::RASPBERRY_PI);
#elif defined ANDROID

#ifndef QT5
    qDebug() << "Resolution::"<<QApplication::desktop()->width()<<"w x "<<QApplication::desktop()->height()<<"h";
    int h = QApplication::desktop()->height();
    int w = QApplication::desktop()->width();
    int pixelsX = QApplication::desktop()->physicalDpiX();
    int pixelsY= QApplication::desktop()->physicalDpiY();
    qDebug() << "Guessed DPI\n x::" << pixelsX << " Y::"<<pixelsY;

    if(isPhone==0){ //no default set
        setCommandResponse("No default form factor set, making initial selection");
        if (w > 480 && h > 854 || h > 480 && w > 854 ){
            setFormFactor(2);
            buildType = "/qml/android/tablet";
            setHostDevice(HostSystemData::ANDROID_TABLET);
        }
        else{
            setFormFactor(1);
            setHostDevice(HostSystemData::ANDROID_PHONE);
            buildType = "/qml/android/phone";
        }
    } else{         //its set, overide setting

        switch(isPhone){
        case 1:
            buildType = "/qml/android/phone";
            qDebug() << buildType;
            break;
        case 2:
            buildType = "/qml/android/tablet";
            qDebug() << buildType;
            break;
        default:
            setCommandResponse("Should not have gotten here, selecting phone for default");
            buildType = "/qml/android/phone";
            break;
        }
        setCommandResponse("Form factor set from config, using "+ buildType);
    }
    qrcPath = ":android/Splash.qml";
    droidPath = "/";
#endif
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
#elif defined Q_OS_IOS
    setHostDevice(HostSystemData::IOS_TABLET);
    buildType = "/qml/ios";
    qrcPath = "qrc:main/IosWelcome.qml";
#else

    buildType = "/qml/desktop";
    qrcPath = ":desktop/Splash.qml";
#endif


#ifdef QANDROID
    // qDebug() << "Resolution::"<<QApplication::desktop()->width()<<"w x "<<QApplication::desktop()->height()<<"h";
    int h = qorbiterUIwin->height();
    int w = qorbiterUIwin->width();
    if(isPhone==0){ //no default set
        setCommandResponse("No default form factor set, making initial selection");
        if (w > 480 && h > 854 || h > 480 && w > 854 ){
            setFormFactor(2);
            buildType = "/tablet";
            setHostDevice(HostSystemData::ANDROID_TABLET);
        }
        else{
            setFormFactor(1);
            setHostDevice(HostSystemData::ANDROID_PHONE);
            buildType = "/phone";
        }
    } else{         //its set, overide setting

        switch(isPhone){
        case 1:
            buildType = "/phone";
            qDebug() << buildType;
            setHostDevice(HostSystemData::ANDROID_PHONE);
            break;
        case 2:
            buildType = "/tablet";

            setHostDevice(HostSystemData::ANDROID_TABLET);
            qDebug() << buildType;
            break;
        default:
            setCommandResponse("Should not have gotten here, selecting phone for default");
            buildType = "/phone";
            setHostDevice(HostSystemData::ANDROID_PHONE);
            break;
        }
        setCommandResponse("Form factor for Qt5 set from config, using "+ buildType);
    }
    qrcPath = ":android/Splash.qml";
    droidPath = "/";
#endif

    qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin"));
    setApplicationPath(QApplication::applicationDirPath());
    localDir = qmlPath.append(buildType.remove("/qml"));
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins"+buildType.remove("/qml");
    m_remoteQmlPath = remoteDirectoryPath;
#ifdef __ANDROID__
    m_localQmlPath="qrc:/qml/";
#elif defined Q_OS_IOS
    m_localQmlPath="qrc:/qml/"; // m_localQmlPath=QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first()+"/";
#elif defined WIN32
    m_localQmlPath=qApp->applicationDirPath()+"/";
#elif defined MACBUILD

    m_localQmlPath=qApp->applicationDirPath()+"/../Resources/";
#ifdef simulate
    m_localQmlPath=qApp->applicationDirPath();
    m_localQmlPath.remove("/build-output/");
    m_localQmlPath.remove("qorbiter-core-gl.app/Contents/MacOS");
    m_localQmlPath.append("/qOrbiter_src/qml/");
#endif

#elif defined Q_OS_LINUX
    //  m_localQmlPath=qApp->applicationDirPath()+"/";
    m_localQmlPath="qrc:/qml/";
#ifdef simulate

#ifdef NOQRC
    m_localQmlPath="../qOrbiter_src/qml/";
#else
    m_localQmlPath="qrc:/qml/";
#endif

#endif

#endif

    skinMessage("build type set to:: "+buildType);
    qDebug() << "Local path set to " << m_localQmlPath;
    qorbiterUIwin->setSource(m_localQmlPath+"splash/Splash.qml");
}

bool qorbiterManager::restoreSettings()
{

    int tId= settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID).toInt();
    qDebug() << Q_FUNC_INFO << "Settings device id" << tId;
    QString trouter = settingsInterface->getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Router).toString();
    mb_useLocalSkins = settingsInterface->getOption(SettingsInterfaceType::Settings_UI, SettingsKeyType::Setting_Ui_NetworkLoading).toBool();
    qDebug() << "Using local skins?" << mb_useLocalSkins;
    if(tId && !trouter.isEmpty()){
        qDebug() << Q_FUNC_INFO << "Read Device Number";
        setDeviceNumber(tId);
        setInternalIp(trouter);
    }
    return true;
}

bool qorbiterManager::setupNetworkSkins()
{

#ifdef __ANDROID__

#ifdef QT5
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/android"+buildType+"/qt5";
#endif
    qWarning()<< remoteDirectoryPath;
#elif defined Q_OS_MACX
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/qt5-desktop";
#elif defined for_desktop
#ifdef defined QT5
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/qt5-desktop";
#elif defined QT4
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/desktop";
#endif

#elif defined WIN32
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/desktop";
#elif defined RPI
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/qt5-desktop";
#elif defined for_harmattan
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins/harmattan";
#elif defined for_android


#else
    remoteDirectoryPath = "http://"+m_ipAddress+"/lmce-admin/skins"+qmlPath;
#endif

    QString qmlPath = adjustPath(QApplication::applicationDirPath().remove("bin"));
    finalPath = remoteDirectoryPath;
    QUrl base(finalPath);
    tskinModel = new SkinDataModel(base, new SkinDataItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("skinsList", tskinModel);
    QObject::connect(tskinModel, SIGNAL(currentSkinReady()), this, SLOT(showSkin()));
    QObject::connect(tskinModel, SIGNAL(skinsFinished(bool)), this, SLOT(setSkinStatus(bool)));

    setDceResponse("initializeManager()::Loading Skins");
#ifdef __ANDROID__
    setDceResponse("initializeManager()::For Android");
#ifdef QT5
    setDceResponse("initializeManager()::Loading Qt Quick 2 Skins for Qt5");

    finalPath = remoteDirectoryPath;
    setDceResponse("initializeManager()::"+finalPath);
#else
    setDceResponse("initializeManager()::Loading Qt Quick 1 Skins for Qt 4");
#endif

    if( !loadSkins(QUrl(finalPath))){
        emit skinIndexReady(false);
        b_skinReady = false;
        return false;
    }
#elif defined Q_OS_MACX

    if( !loadSkins(QUrl(finalPath)))
    {   emit skinIndexReady(false);
        b_skinReady = false;
        return false;
    }

#elif defined for_android
    setDceResponse("Loading skins");
    if( !loadSkins(QUrl(finalPath)))
    {   emit skinIndexReady(false);
        return false;
    }
#elif defined WIN32
    setDceResponse("Loading skins");
    if( !loadSkins(QUrl::fromLocalFile(finalPath)))
    {   emit skinIndexReady(false);
        return false;
    }
#elif defined for_desktop
    if( !loadSkins(QUrl(finalPath)))
    {
        emit skinIndexReady(false);
        return false;
    }

#elif for_harmattan
    if(b_localLoading == true)
    {
        if( !loadSkins(QUrl(finalPath)))
        {
            emit skinIndexReady(false);
            return false;
        }
    }
    else
    {
        if( !loadSkins(QUrl(finalPath)))
        {
            emit skinIndexReady(false);
            return false;
        }
    }

#elif defined RPI

    if( !loadSkins(QUrl(finalPath)))
    {
        emit skinIndexReady(false);
        return false;
    }
#else
    if( !loadSkins(QUrl(finalPath)))
    {
        emit skinIndexReady(false);
        return false;
    }
#endif
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

    if(!themeDoc.isObject())
        qDebug() << Q_FUNC_INFO << "invalid object ";

    QJsonObject themeObject = themeDoc.object();

    if(themeObject.isEmpty())
        return false;

    QVariantMap t = themeObject.toVariantMap();
    qDebug() << Q_FUNC_INFO << themeObject.value("themelist");

}

void qorbiterManager::handleViewError(QList<QQmlError>)
{


}

void qorbiterManager::reloadHandler()
{
    setConnectedState(false);
    setReloadStatus(true);
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
    qDebug() << "DCE Response" << response;
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
 * \brief qorbiterManager::createAndroidConfig
 *This function tries to determine the external storage location for a given android device so that it can read / write the user settings.
 * \return
 */
bool qorbiterManager::createMobileConfig()
{


    QFile droidConfig(mobileStorageLocation+"/config.xml");
    setDceResponse("Config File Path::"+droidConfig.fileName());
    if (droidConfig.exists() && droidConfig.size() != 0)
    {
        setDceResponse("Data exists, exiting 1st run");
        return true;
    }
    else
    {
        QDir filePath;
        if(filePath.mkpath(mobileStorageLocation)){
            setDceResponse("Made path");
        }

        QFile defaultConfig(":///config.xml");
        if(!defaultConfig.open(QFile::ReadOnly)){
            qDebug() << "Cannot find config in QRC";
            qDebug() << defaultConfig.errorString();
            return false;
        }

        if(defaultConfig.copy(mobileStorageLocation+"/config.xml"))
        {
            setDceResponse("file copied, verifying");

            droidConfig.setPermissions(QFile::WriteOther);
            setDceResponse(droidConfig.errorString());
            droidConfig.setPermissions(QFile::ReadOther);
            setDceResponse(droidConfig.errorString());
            droidConfig.setPermissions(QFile::ExeOwner);
            setDceResponse(droidConfig.errorString());

            if (droidConfig.exists() && droidConfig.size() !=0)
            {
                setDceResponse("config size: "+ QString::number(droidConfig.size()));
                return true;
            }
            else{
                setDceResponse("Config copying error.");
                setDceResponse(droidConfig.errorString());
            }
        }
        else{
            setDceResponse("Cannot install configuration!!");
            setDceResponse(defaultConfig.errorString());
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
void qorbiterManager::checkOrientation(QSize s)
{
    setDceResponse("checkOrientation(QSize)::start");
    //NOTE: Is this not handled by the window manager and Orientation change signals?
#ifndef QT5
    if(qorbiterUIwin->height() < qorbiterUIwin->width()){
        //setDceResponse("wide");

        appHeight = qorbiterUIwin->window()->rect().height();
        appWidth = qorbiterUIwin->window()->rect().width() ;
        setOrientation(false);
    } else {
        appHeight = qorbiterUIwin->window()->rect().height();
        appWidth = qorbiterUIwin->window()->rect().width() ;
        setOrientation( true);
    }

#else
    appHeight=s.height();
    appWidth = s.width();
    if(appHeight < appWidth){
        setOrientation(false);
    }
    else{
        setOrientation( true);
    }

#endif
    qDebug() <<"QOrbiter Window size::"<< qorbiterUIwin->size();

}

#ifdef QT5
void qorbiterManager::checkOrientation(Qt::ScreenOrientation o)
{
    Q_UNUSED(o);

    appHeight=qorbiterUIwin->size().height();
    appWidth=qorbiterUIwin->size().width();


    if(appHeight < appWidth){

        setOrientation(false);
    }
    else{
        setOrientation( true);
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

void qorbiterManager::setCurrentScreen(QString s)
{
    if(s!=currentScreen){
        currentScreen = s;
        emit screenChange(s);
    }

}

void qorbiterManager::setCurrentScreen(int s)
{
    QString i = QString("Screen_%1.qml").arg(QString::number(s));

    if(i!=currentScreen){
        currentScreen = i;
        emit screenChange(i);
    }
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

/*!
 * \brief qorbiterManager::setupMobileStorage
 * \return
 * this function saves the mobile storage devices external storage path.
 */
bool qorbiterManager::setupMobileStorage(QString externalStorage)
{

#ifdef ANDROID
    QStringList androidStorageLocation;
    androidStorageLocation <<"/mnt/sdcard"<< "/data"<< "/mnt/external"<< "/mnt/sdcard/ext_sd" << "/mnt/sdcard-ext"<< "/mnt/sdcard/external_sd"<< "/mnt/extSdCard"<< "/mnt/external1"<< "/Removable/MicroSD";

    QDir extLocation;

    extLocation.setPath(externalStorage+"/LinuxMCE");

    if(extLocation.exists() && extLocation.isReadable()){
        setMobileStorage(extLocation.path());
        return true;
    }
    else
    {
        qCritical("Falling back to guessing location method");
        for(int i = 0; i < androidStorageLocation.count(); i++)
        {
            extLocation.setPath(androidStorageLocation.at(i));

            if(extLocation.isReadable())
            {
                setMobileStorage(androidStorageLocation.at(i)+"/LinuxMCE");
                return true;
            }
        }
    }

    return false;
#elif defined(Q_OS_IOS)
    setMobileStorage(externalStorage);
    return true;

#endif

}

void qorbiterManager::setText(QString sDesignObj, QString sValue, int iPK_Text)
{
    emit textChanged(sDesignObj, sValue, iPK_Text);
}

void qorbiterManager::makeCall(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To) {
    emit CMD_makeCall(iPK_Users, sPhoneExtension, sPK_Device_From, iPK_Device_To);
}

QVariant qorbiterManager::systemFontList()
{
    qDebug() << m_fontDir.entryList();
    return QVariant(m_fontDir.entryList());
}

void qorbiterManager::handleScreenChanged(QScreen *screen)
{
    resetScreenSize();
    qDebug() << Q_FUNC_INFO << screen->name() << " changed";

}
