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

#include "qorbitermanager.h"
#include <QDeclarativeProperty>
#include <QFile>
#include <QtXml/QtXml>
#include <QProcess>
#include <QDeclarativeEngine>
#include <QApplication>
#include <imageProviders/filedetailsimageprovider.h>

#include <contextobjects/epgchannellist.h>
#include <datamodels/skindatamodel.h>
#include <QTimer>

//#include "OrbiterData.h"


/*
  this file is responsible for the main connection between the dce thread and the qt thread.
   pqOrbiter->qmlUI = this; inside of setupLMCE initializes the dce thread, while other functions are
   responsible for signals and slots between dce and qml.
  */

using namespace DCE;

/*
  This is the latest incarnation of the orbiter constructor. its purpose is to quickly spawn a splash window, which can
  then (hopefully) notify us of background progress. A splash bar or loading indicator needs to be added, but a textual
  messaging system will be the initial method of communication
*/
qorbiterManager::qorbiterManager(QDeclarativeView *view, QObject *parent) :
    QObject(parent),qorbiterUIwin(view)
{

    m_bStartingUp= true;
    bAppError = false;
    //pqOrbiter->qmlUI = this;
    setDceResponse("Initializing...");

    if (readLocalConfig())
    {

        emit localConfigReady( true);
        QApplication::processEvents(QEventLoop::AllEvents);
    }
    else
    {
        emit localConfigReady(false);
    }

    QApplication::processEvents(QEventLoop::AllEvents);

    qorbiterUIwin->rootContext()->setContextProperty("srouterip", QString(qs_routerip) );
    qorbiterUIwin->rootContext()->setContextProperty("deviceid", QString::number((iPK_Device)) );
    qorbiterUIwin->rootContext()->setContextProperty("extip", qs_ext_routerip );
    qorbiterUIwin->rootContext()->setContextProperty("manager", this); //providing a direct object for qml to call c++ functions of this class
    qorbiterUIwin->rootContext()->setContextProperty("dcemessage", dceResponse); //file grids current media type


    appHeight = qorbiterUIwin->height() ;
    appWidth = qorbiterUIwin->width() ;

    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    qorbiterUIwin->rootContext()->setContextProperty("appH", appHeight); //file grids current media type
    qorbiterUIwin->rootContext()->setContextProperty("appW", appWidth); //file grids current media type

    item = qorbiterUIwin->rootObject();

    QObject::connect(qorbiterUIwin, SIGNAL(sceneResized(QSize)),  SLOT(checkOrientation(QSize)) );
    QObject::connect(this, SIGNAL(orbiterConfigReady(bool)), this, SLOT(showUI(bool)));

    ScreenSaver = new ScreenSaverClass();


#ifdef for_desktop
    buildType = "/qml/desktop";
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
#elif defined (ANDROID)
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

    QApplication::processEvents(QEventLoop::AllEvents);
    qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin"));
    localDir = qmlPath.append(buildType);

    initializeGridModel();  //begins setup of media grid listmodel and its properties
    initializeSortString(); //associated logic for navigating media grids
    QApplication::processEvents(QEventLoop::AllEvents);

    //managing where were are variables
    i_current_command_grp = 0;
    i_current_mediaType =0;
    videoDefaultSort = "13";
    audioDefaultSort = "2";
    photoDefaultSort = "13";
    gamesDefaultSort = "49";

    //goBack << ("|||1,2|0|13|0|2|");
    backwards = false;

    //avcode grid button list

    //file details object and imageprovider setup
    filedetailsclass = new FileDetailsClass();
    qorbiterUIwin->rootContext()->setContextProperty("filedetailsclass" ,filedetailsclass);
    filedetailsclass->clear();

    nowPlayingButton = new NowPlayingClass();
    qorbiterUIwin->rootContext()->setContextProperty("dcenowplaying" , nowPlayingButton);

    //screen parameters class that could be extended as needed to fetch other data
    ScreenParameters = new ScreenParamsClass;
    qorbiterUIwin->rootContext()->setContextProperty("screenparams", ScreenParameters);
    // timecodeThread = new QThread();
    timeCodeSocket = new QTcpSocket();
    // timeCodeSocket->moveToThread(timecodeThread);
    // timecodeThread->start();
    QApplication::processEvents(QEventLoop::AllEvents);
}


void qorbiterManager::gotoQScreen(QString s)
{

    if(s == "Screen_1.qml")
    {
        // qDebug() << "Setting load to false";
        bool t = false;
        emit keepLoading(t);
        emit clearModel();
        emit resetFilter();
       // emit bindMediaRemote(false);
    }

    //send the qmlview a request to go to a screen, needs error handling
    //This allows it to execute some transition or other if it wants to

    setDceResponse("Starting screen switch");

    QVariant screenname= s;
    currentScreen = s;
    QObject *item = qorbiterUIwin->rootObject();
    setDceResponse("About to call screenchange()");
    if (QMetaObject::invokeMethod(item, "screenchange", Qt::QueuedConnection, Q_ARG(QVariant, screenname))) {
        setDceResponse("Done call to screenchange()");
    } else {
        setDceResponse("screenchange() FAILED");
    }


}

//this block sets up the connection to linuxmce
bool qorbiterManager::initializeManager(string sRouterIP, int device_id)
{

    setDceResponse("Starting Manager");

    //QObject::connect(pqOrbiter, SIGNAL(gotoQml(QString)), this, SLOT(gotoQScreen(QString)));

    QObject::connect(this,SIGNAL(screenChange(QString)), this, SLOT(gotoQScreen(QString)));
    //QObject::connect(this,SIGNAL(executeCMD(int)), pqOrbiter, SLOT(executeCommandGroup(int)), Qt::QueuedConnection);


#ifdef ANDROID
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
    remoteDirectoryPath = "http://"+QString::fromStdString(sRouterIP)+"/lmce-admin/skins";
#endif

    QString qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin"));
    QString localDir = qmlPath.append(buildType);

    //loadSkins(QUrl(localDir));
#ifdef ANDROID
    if( !loadSkins(QUrl(remoteDirectoryPath)))
    {   emit skinIndexReady(true);
        return false;
    }
#elif for_android
    //qDebug() << "Loading skins";
    if( !loadSkins(QUrl(localDir)))
    {   emit skinIndexReady(false);
        return false;
    }
#else
    if( !loadSkins(QUrl(localDir)))
    {   emit skinIndexReady(false);
        return false;

    }
#endif


}

void qorbiterManager::refreshUI(QUrl url)
{
    qorbiterUIwin->rootContext()->setBaseUrl(skin->entryUrl());
    qorbiterUIwin->setSource(skin->entryUrl());
    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);

}


// get conf method that reads config file
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
    configData.setContent(tConf,false);
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
    // qDebug () << root.tagName();
    //------------DEFAULTS-FOR-ORBITER------------------------------------------------------------
    QDomElement defaults = root.firstChildElement("Default");
    QString sPK_User = defaults.attribute("sPK_User");
    iFK_Room = defaults.attribute("DefaultRoom").toInt();
    iea_area = defaults.attribute("DefaultEA").toInt();
    iPK_User = defaults.attribute("PK_User").toInt();

    setDceResponse("Defaults Set");

    //-floorplans-----------------------------------------------------------------------------------------------------
    QDomElement floorplanXml = root.firstChildElement("Floorplans");
    floorplans = new FloorPlanModel( new FloorPlanItem , this);
    QDomNodeList floorplanList = floorplanXml.childNodes();
    for(int index = 0; index < floorplanList.count(); index++)
    {
        QString m_installation= floorplanList.at(index).attributes().namedItem("Installation").nodeValue();
        QString m_iconpath= floorplanList.at(index).attributes().namedItem("Icon").nodeValue();
        QString m_description= floorplanList.at(index).attributes().namedItem("Description").nodeValue();
        int m_page= floorplanList.at(index).attributes().namedItem("Page").nodeValue().toInt();


        QImage m_image;// pqOrbiter->getfileForDG("/usr/pluto/orbiter/floorplans/inst"+m_installation.toStdString()+"/"+StringUtils::itos(m_page)+".png");
        m_image.load(":/icons/icon.png");
        QImage icon = m_image.scaledToHeight(100);
        floorplans->appendRow(new FloorPlanItem(m_installation,m_description, m_page, m_iconpath, m_image, icon,  userList));

        if (m_page == 1)
        {
            QString safe = m_installation;
            floorplans->setCurrentPage(safe.append("-").append(QString::number(m_page)));
        }

    }


    emit loadingMessage("floorplans");
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
        int m_val = roomListXml.at(index).attributes().namedItem("PK_Room").nodeValue().toInt();
        int m_iEA = roomListXml.at(index).attributes().namedItem("PK_EntertainArea").nodeValue().toInt();
        int m_iType = roomListXml.at(index).attributes().namedItem("FK_RoomType").nodeValue().toInt();
        QString m_title = roomListXml.at(index).attributes().namedItem("Description").nodeValue();
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

        m_lRooms->appendRow(new LocationItem(m_name, m_val, m_title, m_iEA, m_iType, imagePath, m_lRooms));
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

        int MroomMapNo = mScenarioList.at(index).attributes().namedItem("int").nodeValue().toInt();
        for (int innerIndex = 0; innerIndex < mScenarioRoom.count(); innerIndex++)
        {
            QString m_name = mScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_label = mScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QString m_param =mScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue() ;
            QString m_command = mScenarioRoom.at(innerIndex).attributes().namedItem("eaDescription").nodeValue();
            QString m_goto = mScenarioRoom.at(innerIndex).attributes().namedItem("FK_CommandGroup").nodeValue();
            QString imgName = mScenarioRoom.at(innerIndex).attributes().namedItem("Description").nodeValue();
            QImage m_image = QImage("Qrc:/icons/"+imgName);
            mediaModelHolder->appendRow(new MediaScenarioItem(m_name,m_label, m_param, m_command, m_goto, m_image, mediaModelHolder));
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
    //------------------------------------------context objects----------------------------------------------------
    qorbiterUIwin->rootContext()->setContextObject(this);
    // setActiveRoom(RroomMapping.value(QString::fromStdString(pqOrbiter->DATA_Get_FK_EntertainArea())), 0);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomLights", roomLights);                 //current room scenarios model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomMedia", roomMedia);                   //current room media model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomClimate", roomClimate);               //curent room climate model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomTelecom", roomTelecom);               //curret room telecom model
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomSecurity", roomSecurity);             //current room security model
    qorbiterUIwin->rootContext()->setContextProperty("floorplan", floorplans);
    qorbiterUIwin->rootContext()->setContextProperty("currentuser", sPK_User);
    qorbiterUIwin->rootContext()->setContextProperty("iPK_Device", QVariant::fromValue(iPK_Device));  //orbiter device number
    qorbiterUIwin->rootContext()->setContextProperty("currentroom", m_lRooms->sdefault_Ea);           //custom room list item provided
    qorbiterUIwin->rootContext()->setContextProperty("userList", userList);                           //custom user list provided
    qorbiterUIwin->rootContext()->setContextProperty("roomList", m_lRooms);                           //custom room list  provided
    qorbiterUIwin->rootContext()->setContextProperty("gmediaType", q_mediaType);
    qorbiterUIwin->rootContext()->setContextProperty("screenshotAttributes", QVariant::fromValue(screenshotVars));
    QObject::connect(this->nowPlayingButton, SIGNAL(mediaStatusChanged()), this, SLOT(updateTimecode()), Qt::QueuedConnection);
    setDceResponse("Properties Done");

    setDceResponse("Setting location");
    QApplication::processEvents(QEventLoop::AllEvents);
    //------------not sure if neccesary since it knows where we are.
    setActiveRoom(iFK_Room, iea_area);
    setCurrentUser(QString::number(iPK_User));
    emit registerOrbiter((userList->find(sPK_User)->data(4).toInt()), QString::number(iea_area), iFK_Room );

    //pqOrbiter->CMD_Set_Current_Room(m_lRooms->idefault_Ea);
    //pqOrbiter->CMD_Set_Entertainment_Area(m_lRooms->sdefault_Ea.toStdString());


    //-----setting up the FILEFORMAT model------------------------------------------------------------------------
    QImage attrimg(":/icons/Skin-Data.png");
    uiFileFilter = new FilterModel(new FilterModelItem, this);
    QDomElement fileFormatElement = root.firstChildElement("FileFormats");
    QDomNodeList fileformatlist = fileFormatElement.childNodes();

    for(int index = 0; index < fileformatlist.count(); index++)
    {
        QString name = fileformatlist.at(index).attributes().namedItem("Name").nodeValue();
        QString pk= fileformatlist.at(index).attributes().namedItem("PK_FileFormat").nodeValue();
        uiFileFilter->appendRow(new FilterModelItem(name,pk, attrimg,false,uiFileFilter));
    }
    this->qorbiterUIwin->rootContext()->setContextProperty("fileformatmodel", uiFileFilter); //custom fileformatmodel for selection filter item
    connect(uiFileFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));

    //-----setting up the MEDIASUBTYPE model------------------------------------------------------------------------
    QDomElement mediaTypeElement = root.firstChildElement("MediaSubTypes");
    QDomNodeList mediaTypeList = mediaTypeElement.childNodes();
    mediaTypeFilter = new MediaSubTypeModel(new MediaSubTypeItem, this);
    for(int index = 0; index < mediaTypeList.count(); index++)
    {
        QString name = mediaTypeList.at(index).attributes().namedItem("Name").nodeValue();
        QString pk= mediaTypeList.at(index).attributes().namedItem("PK_MediaSubType").nodeValue();
        mediaTypeFilter->appendRow(new MediaSubTypeItem(name,pk, attrimg,false,mediaTypeFilter));
    }
    this->qorbiterUIwin->rootContext()->setContextProperty("mediatypefilter", mediaTypeFilter); //custom mediatype selection model
    connect(mediaTypeFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));

    //-----setting up the GENRE model------------------------------------------------------------------------
    QDomElement genreElement = root.firstChildElement("GenreList");
    QDomNodeList genrelist = genreElement.childNodes();
    genreFilter = new GenreModel(new GenreItem, this);
    for(int index = 0; index < genrelist.count(); index++)
    {
        QString name = genrelist.at(index).attributes().namedItem("Name").nodeValue();
        QString pk= genrelist.at(index).attributes().namedItem("PK_Attribute").nodeValue();
        genreFilter->appendRow(new GenreItem(name,pk, attrimg,false,genreFilter));
    }
    this->qorbiterUIwin->rootContext()->setContextProperty("genrefilter", genreFilter); //custom mediatype selection model
    connect(genreFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
    QObject::connect(this, SIGNAL(resetFilter()), genreFilter, SLOT(resetStates()));

    //-----setting up the ATTRIBUTE model------------------------------------------------------------------------
    QDomElement attribElement = root.firstChildElement("AttributeList");
    QDomNodeList attriblist = attribElement.childNodes();
    attribFilter = new AttributeSortModel(new AttributeSortItem, this);
    for(int index = 0; index < attriblist.count(); index++)
    {
        QString name = attriblist.at(index).attributes().namedItem("Name").nodeValue();
        QString pk= attriblist.at(index).attributes().namedItem("PK_AttributeType").nodeValue();
        attribFilter->appendRow(new AttributeSortItem(name,pk, attrimg,false,attribFilter));
    }
    this->qorbiterUIwin->rootContext()->setContextProperty("attribfilter", attribFilter); //custom mediatype selection model
    connect(attribFilter, SIGNAL(SetTypeSort(int,QString)), this, SLOT(setStringParam(int,QString)));
    QObject::connect(this, SIGNAL(resetFilter()), attribFilter, SLOT(resetStates()) );

    //connect(nowPlayingButton, SIGNAL(mediaStatusChanged()), this , SLOT(updateTimecode()));
    binaryConfig.clear();

    //tConf.clear();

    //----------------Security Video setup
    SecurityVideo = new SecurityVideoClass();
    qorbiterUIwin->rootContext()->setContextProperty("securityvideo", SecurityVideo);

    //-alarms
    sleeping_alarms.clear();
    qorbiterUIwin->rootContext()->setContextProperty("alarms", QVariant::fromValue(sleeping_alarms) );
    //---update object image


    setDceResponse(" Remote Config Complete");

    configData.clear();
    tConf.clear();
#ifdef for_desktop
    activateScreenSaver();
#endif

    emit orbiterConfigReady(true);

}

bool qorbiterManager::OrbiterGen()
{
}

void qorbiterManager::swapSkins(QString incSkin)
{    
    setDceResponse("Setting Skin to:" + incSkin);
    skin = tskinModel->find(incSkin);
    setDceResponse("Got it from the model : " + skin->baseUrl().toString());
    //load the actual skin entry point
    currentSkin = incSkin;
    qorbiterUIwin->engine()->rootContext()->setContextProperty("style", skin->styleView());

    QObject::connect(qorbiterUIwin, SIGNAL(statusChanged(QDeclarativeView::Status)),
                     this, SLOT(skinLoaded(QDeclarativeView::Status)));

    QMetaObject::invokeMethod(this, "refreshUI", Qt::QueuedConnection, Q_ARG(QUrl, skin->entryUrl()));

}

void qorbiterManager::skinLoaded(QDeclarativeView::Status status) {
    setDceResponse("Skin appears to have finished loading ..");

    if (status == QDeclarativeView::Error) {
        qWarning() << "Skin loading has FAILED";
        qWarning() << qorbiterUIwin->errors();
        emit skinDataLoaded(false);
    } else {

        m_bStartingUp = false;
        emit skinDataLoaded(true);
        QApplication::processEvents(QEventLoop::AllEvents);

        startOrbiter();


    }
}


//takes care of un-registering the orbiter from the DCERouter and then shutting down
void qorbiterManager::closeOrbiter()
{
    setDceResponse("Shutting Down");
    QApplication::processEvents(QEventLoop::AllEvents);
#ifndef ANDROID
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Exiting, Unregistering 1st");
#endif
    emit unregisterOrbiter((userList->find(sPK_User)->data(4).toInt()), QString(iFK_Room), iea_area );
    this->~qorbiterManager();
    exit(0);
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
    roomLights = roomLightingScenarios.value(room);
    roomMedia = roomMediaScenarios.value(room);
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

    if(qs_routerip =="127.0.0.1")
    {

        //splashView->showFullScreen();
        qorbiterUIwin->close();
        QApplication::processEvents(QEventLoop::AllEvents);
        //qDebug("Onscreen orbiter");
        QProcess *regen = new QProcess(this);
        regen->start("/usr/bin/php /var/www/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device), QIODevice::ReadOnly);
        //qDebug() <<"status code:" << regen->errorString();
        QObject::connect(regen,SIGNAL(finished(int)), this, SLOT(regenComplete(int)));
        QObject::connect(regen,SIGNAL(error(QProcess::ProcessError)), this, SLOT(regenError(QProcess::ProcessError)));

    }
    else
    {
        emit screenChange("WebRegen.qml");
    }


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

#ifdef ANDROID
    return path+"droid";
#endif

#ifdef Q_OS_UNIX

#ifdef Q_OS_MAC
    return QCoreApplication::applicationDirPath().remove("MacOS").append("Resources");
#else
    const QString pathInShareDir = QCoreApplication::applicationDirPath()
            + QLatin1String("/../share/")
            + QFileInfo(QCoreApplication::applicationFilePath()).fileName()
            + QLatin1Char('/') + path;
    if (QFileInfo(pathInShareDir).exists())
        return pathInShareDir;
#endif
#endif
    return path;

}

void qorbiterManager::setSorting(int i)
{
    q_mediaType = QString::number(i);
    qorbiterUIwin->rootContext()->setContextProperty("gmediaType", q_mediaType);
    emit gridTypeChanged(i);
}

void qorbiterManager::setNowPlayingIcon(bool b)
{
    QColor color;
    color.fromRgb(255, 255, 215);
    qorbiterUIwin->rootContext()->setContextProperty("nowPlayingColor", color);
    item->setProperty("nowplayingtext", "null");
}

void qorbiterManager::nowPlayingChanged(bool b)
{
    nowPlayingButton->setStatus(b);
}

bool qorbiterManager::loadSkins(QUrl base)
{

    tskinModel = new SkinDataModel(base, new SkinDataItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("skinsList", tskinModel);

    /*
      TODO ASAP:
      Write feeder function that preceeds this call to creat a simple string list
      of all availible skins for the build. a simple parsing of the skins directory
      should suffice and improper skins are dealt with later.
      */

#ifdef for_harmattan
    tskinModel->addSkin("default");

#elif ANDROID
    tskinModel->addSkin("default");

#elif for_android
    tskinModel->addSkin("default");
#else
    tskinModel->addSkin("default");
    tskinModel->addSkin("aeon");
    tskinModel->addSkin("crystalshades");
#endif
    emit skinIndexReady(true);
    return true;
}

void qorbiterManager::quickReload()
{

    // pqOrbiter->QuickReload();
    //bool connected = pqOrbiter->m_bRouterReloading;
}

void qorbiterManager::showUI(bool b)
{
    if(b == true)
    {
        swapSkins(currentSkin);
    }
    else
    {
        emit raiseSplash();
    }
}

void qorbiterManager::displayModelPages(QList<QObject *> pages)
{
    qorbiterUIwin->rootContext()->setContextProperty("pageList", QVariant::fromValue(pages));
}

void qorbiterManager::setIpAddress(QString s)
{
    m_ipAddress = s;
}


void qorbiterManager::qmlSetupLmce(QString incdeviceid, QString incrouterip)
{
    setDceResponse("Triggering connection to LMCE Device ID [" + incdeviceid + "] port Router Address [" + incrouterip + "]") ;

    qs_routerip = incrouterip;
    iPK_Device = long(incdeviceid.toInt());
    setDceResponse("Connecting to LinuxMCE Core");
    initializeManager( qs_routerip.toStdString(), iPK_Device);
}


bool qorbiterManager::readLocalConfig()
{
    QApplication::processEvents(QEventLoop::AllEvents);
    QDomDocument localConfig;
#ifdef Q_OS_MAC
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("MacOS").append("Resources").append("/config.xml").toStdString());
#elif ANDROID
    QString xmlPath = "/mnt/sdcard/LinuxMCE/config.xml";
#else
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#endif
    QFile localConfigFile;
    //**todo!! - add function for 1st run on android that copies the file to the xml path, then performs checks. we cannot install directly there.

    localConfigFile.setFileName(xmlPath);

#ifdef ANDROID
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
        setDceResponse("config not found!");
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

            if(!configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue().isEmpty())
            {
                qs_routerip = configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue();
            }
            currentSkin = configVariables.namedItem("skin").attributes().namedItem("id").nodeValue();


            if(configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong() !=0)
            {
                iPK_Device = configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong();
            }

            if(!configVariables.namedItem("externalip").attributes().namedItem("id").nodeValue().isEmpty() )
            {
                qs_ext_routerip = configVariables.namedItem("externalip").attributes().namedItem("id").nodeValue();
            }
            else
            {
                qs_ext_routerip = "fill.me.in.com";
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
#elif ANDROID
    QString xmlPath = "/mnt/sdcard/LinuxMCE/config.xml";
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
            configVariables.namedItem("routerip").attributes().namedItem("id").setNodeValue(qs_routerip);
            configVariables.namedItem("skin").attributes().namedItem("id").setNodeValue(currentSkin);
            configVariables.namedItem("device").attributes().namedItem("id").setNodeValue(QString::number(iPK_Device));
            configVariables.namedItem("externalip").attributes().namedItem("id").setNodeValue(qs_ext_routerip);
            configVariables.namedItem("firstrun").attributes().namedItem("id").setNodeValue(QString("false"));


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
    setRequestMore(false);
    backwards= true;

    if(goBack.isEmpty())
    {
        initializeSortString();
        emit clearAndContinue();
    }
    else
    {
        goBack.removeLast();
        int back = goBack.count() - 1;
        //qDebug() << back;
        //qDebug() << "Going back to::" << goBack.at(back);

        QStringList reverseParams = goBack.at(back).split("|", QString::KeepEmptyParts);
        q_mediaType = reverseParams.first();
        q_subType = reverseParams.at(1);
        q_fileFormat = reverseParams.at(2);
        q_attribute_genres = reverseParams.at(3);
        q_mediaSources = reverseParams.at(4);
        q_usersPrivate = reverseParams.at(5);
        q_attributetype_sort = reverseParams.at(6);
        q_pk_users = reverseParams.at(7);
        q_last_viewed = reverseParams.at(8);
        q_pk_attribute = reverseParams.at(9);
        emit clearAndContinue();
    }

    //datagridVariableString.append(q_mediaType).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);



}

void qorbiterManager::showFileInfo(QString fk_file)
{

    // pqOrbiter->GetMediaAttributeGrid(fk_file);
}

void qorbiterManager::requestSecurityPic(int i_pk_camera_device, int h, int w)
{
    //  pqOrbiter->GetSingleSecurityCam( i_pk_camera_device,  h,  w);
}

void qorbiterManager::playMedia(QString FK_Media)
{
    //  pqOrbiter->playMedia(FK_Media);
    emit startPlayback(FK_Media);
}

void qorbiterManager::stopMedia()
{
    //  pqOrbiter->StopMedia();
}

void qorbiterManager::rw_media(int speed)
{
    //nowPlayingButton->setMediaSpeed(speed);
    //pqOrbiter->RwMedia();
}

void qorbiterManager::ff_media(int speed)
{
    //nowPlayingButton->setMediaSpeed(speed);
    //pqOrbiter->FfMedia();
}

void qorbiterManager::pauseMedia()
{
    //  pqOrbiter->PauseMedia();
}

void qorbiterManager::showfloorplan(int fptype)
{
    //  pqOrbiter->ShowFloorPlan(fptype);
}



void qorbiterManager::changedPlaylistPosition(QString position)
{


}

void qorbiterManager::setNowPlayingData()
{


}

void qorbiterManager::updateImageChanged(QImage img)
{

}

void qorbiterManager::setNowPlayingTv()
{
    emit bindMediaRemote(true);
    emit liveTVrequest();
}

void qorbiterManager::setScreenShotVariables(QList<QObject *> l)
{
    setDceResponse("Setting thumbnail attributes to screen");
    screenshotVars = l;
    qorbiterUIwin->rootContext()->setContextProperty("screenshotAttributes", QVariant::fromValue(screenshotVars));
}

void qorbiterManager::setMediaScreenShot(QByteArray data)
{
    if(mediaScreenShot.loadFromData(data))
    {
        emit mediaScreenShotReady();
        qDebug("Screen Shot loaded");
    }
}

void qorbiterManager::saveScreenShot(QString attribute)
{

    qDebug() << attribute;
    QByteArray bytes;

    QBuffer ba(&bytes);
    ba.open(QIODevice::WriteOnly);
    mediaScreenShot.save(&ba, "JPG");
    ba.close();
    qDebug()<<  bytes.size();
    emit saveMediaScreenShot(attribute, bytes);
     cleanupScreenie();
}

void qorbiterManager::changeChannels(QString chan)
{
    //  pqOrbiter->TuneToChannel(chan.toInt(), chan );
}

void qorbiterManager::getLiveTVPlaylist()
{   

}

void qorbiterManager::getStoredPlaylist()
{
 emit bindMediaRemote(true);
emit managerPlaylistRequest();

}

void qorbiterManager::gridChangeChannel(QString chan, QString chanid)
{
    ////    pqOrbiter->TuneToChannel(chan.toInt(), chanid);
}


void qorbiterManager::setHouseMode(int mode, int pass)
{
    // pqOrbiter->SetSecurityMode(pass, mode);
}

void qorbiterManager::setCurrentUser(QString inc_user)
{
    sPK_User = userList->find(sPK_User)->data(1).toString();
    int user = inc_user.toInt();
    // pqOrbiter->setUser(user);
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

void qorbiterManager::sleepingMenu(bool toggle, int grp)
{

    if(toggle == true)
    {   sleeping_alarms.clear();
        //  pqOrbiter->GetAlarms(toggle, grp);
    }
    else
    {
        sleeping_alarms.clear();
        // pqOrbiter->GetAlarms(false, 0);
    }
}



void qorbiterManager::updateTimecode(int port)
{

    if(!timeCodeSocket->isOpen())
    {
       qDebug() <<"opening connection to " << port;

        timeCodeSocket->connectToHost(qs_routerip, port, QFile::ReadOnly );
        if ( timeCodeSocket->isValid() )
        {
            setDceResponse("Time Code Socket connected! " + QString::fromStdString(m_ipAddress.toStdString()));
            QObject::connect(timeCodeSocket,SIGNAL(readyRead()), this, SLOT(showTimeCode()));
        }
        else
        {   qDebug("couldnt start timecode");
             qDebug() << timeCodeSocket->errorString();
        }
    }

}

void qorbiterManager::showTimeCode()
{

    QByteArray socketData = timeCodeSocket->readLine();
    QString tcData = QString::fromAscii(socketData.data(), socketData.size());

    if (tcData.length() > 0)
    {
        QStringList tcVars = tcData.split(",");
        QString tcClean = tcVars.at(1);
        tcClean.remove(QRegExp(".\\d\\d\\d|00:0|0:0|00:"));
        nowPlayingButton->setTimeCode(tcClean);

        QString playbackSpeed = tcVars.at(0);
        playbackSpeed.remove(QRegExp("000"));
        nowPlayingButton->setStringSpeed(playbackSpeed+"x");
        nowPlayingButton->setMediaSpeed(playbackSpeed.toInt());

        QString duration = tcVars.at(2);
        duration.remove(QRegExp(".\\d\\d\\d|00:0|0:0|00:"));
        nowPlayingButton->setDuration(duration);

    }

}

void qorbiterManager::checkConnection(QString s)
{
    /*
    setDceResponse(s);
    if(pqOrbiter->m_bQuit_get() == true)
    {
        setConnectedState(false);
        setDceResponse("Disconnected!");
    }
    emit raiseSplash();
*/

}

void qorbiterManager::jogPosition(QString jog)
{
    // pqOrbiter->JogStream(jog);
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

void qorbiterManager::setSeekLetter(QString letter)
{
    qs_seek = letter;
}

void qorbiterManager::showMessage(QString message, int duration, bool critical)
{

}

void qorbiterManager::startOrbiter()
{
    setDceResponse("Showing main orbiter window");
    QApplication::processEvents(QEventLoop::AllEvents);

    m_bStartingUp = false;
    qorbiterUIwin->setWindowTitle("LinuxMCE Orbiter " + QString::number(iPK_Device));

    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    QApplication::processEvents(QEventLoop::AllEvents);
    emit screenChange("Screen_1.qml");
    QApplication::processEvents(QEventLoop::AllEvents);

}

void qorbiterManager::processError(QString msg)
{
    emit raiseSplash();
    setDceResponse("Process Error Slot:" + msg) ;
}

void qorbiterManager::setActiveSkin(QString name)
{
    //qDebug("Setting Skin");
    swapSkins(name);
}



void qorbiterManager::cleanupScreenie()
{
    mediaScreenShot = QImage();
    screenshotVars.clear();
    setDceResponse("Cleaned up Screenshot model");
    QApplication::processEvents(QEventLoop::AllEvents);
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
    /*
    setDceResponse("Reloading Router");
    QApplication::processEvents(QEventLoop::AllEvents);
     sleep(15);
    if ( pqOrbiter->GetConfig() && pqOrbiter->Connect(pqOrbiter->PK_DeviceTemplate_get()))
    {

        setDceResponse("Reload Complete");
        swapSkins(currentSkin);
         QApplication::processEvents(QEventLoop::AllEvents);
    }
    else
    {
QApplication::processEvents(QEventLoop::AllEvents);

    }
    */

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
    return 1;
}

void qorbiterManager::activateScreenSaver()
{

    qorbiterUIwin->engine()->rootContext()->setContextProperty("screensaver", ScreenSaver);
}

void qorbiterManager::killScreenSaver()
{
}

bool qorbiterManager::createAndroidConfig()
{
    QFile droidConfig("/mnt/sdcard/LinuxMCE/config.xml");
    if (droidConfig.exists() && droidConfig.size() != 0)
    {   setDceResponse("Data exists, exiting 1st run");
        return true;
    }
    else
    {
        QDir filePath;
        if(filePath.mkpath("/mnt/sdcard/Linuxmce/"))
        {
            setDceResponse("Made path");
        }

        QFile defaultConfig(":main/config.xml");

        if(defaultConfig.copy("/mnt/sdcard/Linuxmce/config.xml"))
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

void qorbiterManager::checkOrientation(QSize)
{

    if(qorbiterUIwin->height() < qorbiterUIwin->width())
    {

        //setDceResponse("wide");
        appHeight = qorbiterUIwin->height() ;
        appWidth = qorbiterUIwin->width() ;
        setOrientation(false);

    }
    else
    {

        appHeight = qorbiterUIwin->height() ;
        appWidth = qorbiterUIwin->width() ;
        setOrientation( true);
    }

    setDceResponse("orientation change");
}

void qorbiterManager::getGrid(int i)
{
    setRequestMore(true);
    emit gridTypeChanged(i);
    emit mediaRequest(i);
}

void qorbiterManager::adjustVolume(int vol)
{
    //  pqOrbiter->adjustVolume( vol);
}

QString qorbiterManager::getCurrentScreen()
{
    return currentScreen;
}

void qorbiterManager::setCurrentScreen(QString s)
{
    currentSkin = s;
    emit screenChange(s);
}



