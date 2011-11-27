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
#include <QDebug>
#include <QFile>
#include <QtXml/QtXml>
#include <QProcess>
#include <QDeclarativeEngine>
#include <QApplication>
#include <imageProviders/filedetailsimageprovider.h>
#include <imageProviders/abstractimageprovider.h>
#include <contextobjects/epgchannellist.h>


//#include "OrbiterData.h"


/*
  this file is responsible for the main connection between the dce thread and the qt thread.
   pqOrbiter->qmlUI = this; inside of setupLMCE initializes the dce thread, while other functions are
   responsible for signals and slots between dce and qml.
  */

using namespace DCE;

/*
  this is the constructor. it should be refactored to intantiatae the qorbiter class from the start to facilitate
  cleaner startup with more robust error checking
*/
qorbiterManager::qorbiterManager(int deviceno, QString routerip,QWidget *parent) :
    QWidget(parent), iPK_Device(deviceno), qs_routerip(routerip)
{
    /*
    this block is for the purposes of exposing the proper qml skins for particular target class
    its current reflective more of devices as target classes but this may change
            */
    QString buildType;
#ifdef for_desktop
    buildType = "/qml/desktop/";
#elif defined (for_freemantle)
    buildType = "/qml/freemantle/";
#elif defined (for_harmattan)
    buildType="/qml/harmattan/";
#elif defined (Q_OS_MACX)
    buildType="/qml/desktop/";
#elif defined (ANDROID)
    buildType = "/qml/android/";
#elif defined (for_droid)
    buildType = "/qml/android/phone/";
#endif

    qorbiterUIwin = new QDeclarativeView; //initialize the declarative view to act upon its context

    if (readLocalConfig())
    {
        //qDebug () << "Local confing processing";
    }

    //  currentSkin = "default";

   // s_RouterIP="DCERouter";

    //adjusting runtime paths also based on platform and build type
    QString qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin"));
    const QString test = buildType;
    QString finalPath = qmlPath+buildType+currentSkin;
    QString skinPath= finalPath+"/Style.qml";
    QString skinsPath = qmlPath+test;

    /*
      this represent the beginning of the processing area for the skins availible for the device
      we read the directory, and in turn read the file information to present it in the user interface
      */
    //qDebug () << "QML import path for build: " << qmlPath;
    m_SkinsDirectoryPath = qmlPath+buildType.toLatin1().constData();
    QDir directoryMap(skinsPath);
    directoryMap.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QStringList skinList = directoryMap.entryList();
    getcurrentSkins(skinList);

    //loading the style from the current set skin directory
#ifdef ANDROID
    QDeclarativeComponent skinData(qorbiterUIwin->engine(),QUrl("qrc:main/Style.qml"));
#elif for_droid
    QDeclarativeComponent skinData(qorbiterUIwin->engine(),QUrl("qrc:main/Style.qml"));
#else
    QDeclarativeComponent skinData(qorbiterUIwin->engine(),QUrl::fromLocalFile(finalPath+"/Style.qml"));
#endif

    //turning it into a qObject - this part actually loads it - the error should connect to a slot and not an exit
    QObject *styleObject = skinData.create(qorbiterUIwin->rootContext());

    //wait for it to load up
    while (!skinData.isReady())
    {
        if(skinData.isError())
        {
            qDebug() << skinData.errors();
            exit(0);
        }
        // qDebug() << " loading";
    }

    if(skinData.isReady())
    {
        //set it as a context property so the qml can read it. if we need to changed it,we just reset the context property
        qorbiterUIwin->engine()->rootContext()->setContextProperty("style", styleObject);
        qorbiterUIwin->rootContext()->setContextProperty("srouterip", QString(qs_routerip) );
        qorbiterUIwin->rootContext()->setContextProperty("deviceid", QString::number((iPK_Device)) );
        initializeGridModel();  //begins setup of media grid listmodel and its properties
        initializeSortString(); //associated logic for navigating media grids
        qorbiterUIwin->rootContext()->setContextObject(this); //providing a direct object for qml to call c++ functions of this class


        //setting engine import path
        qorbiterUIwin->engine()->setBaseUrl(qmlPath+buildType);
#ifdef ANDROID
        qorbiterUIwin->setSource(QUrl("qrc:main/main.qml"));
#elif for_droid
        qorbiterUIwin->setSource(QUrl("qrc:main/main.qml"));
#else
        qorbiterUIwin->setSource(QUrl::fromLocalFile(finalPath+"/main.qml"));
#endif

        //reference to the object for later use?
        item= qorbiterUIwin->rootObject();


        //initial signal and slot connections
        QObject::connect(item, SIGNAL(close()), this, SLOT(closeOrbiter()));


        //managing where were are variables
        int i_current_command_grp;
        i_current_command_grp = 0;
        QStringList goBack;
        goBack << ("|||1,2|0|13|0|2|");
        backwards = false;

        //file details object and imageprovider setup
        filedetailsclass = new FileDetailsClass();
        qorbiterUIwin->rootContext()->setContextProperty("filedetailsclass" ,filedetailsclass);

        // connect(filedetailsclass, SIGNAL(FileChanged(QString)), this, SLOT(showFileInfo(QString)));

        //non functioning screen saver module
        ScreenSaver = new ScreenSaverModule;
        qorbiterUIwin->engine()->rootContext()->setContextProperty("screensaver", ScreenSaver);

        /* now playing button todo - make it appear and dissapear if in a room with no media / media director
            embed in room model or other location based marker
*/
        nowPlayingButton = new NowPlayingClass();
        qorbiterUIwin->rootContext()->setContextProperty("dcenowplaying" , nowPlayingButton);



        //screen parameters class that could be extended as needed to fetch other data
        ScreenParameters = new ScreenParamsClass;
        qorbiterUIwin->rootContext()->setContextProperty("screenparams", ScreenParameters);

        //stored video playlist for managing any media that isnt live broacast essentially

        storedVideoPlaylist = new PlaylistClass (new PlaylistItemClass, this);
        qorbiterUIwin->rootContext()->setContextProperty("mediaplaylist", storedVideoPlaylist);

        //initializing threading for timecode to prevent blocking
        timecodeThread = new QThread;
        timeCodeSocket = new QTcpSocket(timecodeThread);
        timecodeThread->start();

        //showing the qml screen depending on device / platform / etc
#ifdef Q_OS_SYMBIAN
        qorbiterUIwin->showFullScreen();
#elif defined(Q_WS_MAEMO_5)
        qorbiterUIwin->showMaximized();
#elif defined(for_harmattan)
        qorbiterUIwin->showFullScreen();
#elif defined(for_desktop)
        qorbiterUIwin->show();
#else
        qorbiterUIwin->show();
#endif

        // qorbiterUIwin->showFullScreen();

        //     qDebug() << "Showing Splash";
        gotoQScreen("Splash.qml");

        QObject::connect(item,SIGNAL(setupStart(int, QString)), this,SLOT(qmlSetupLmce(int,QString)));
        QObject::connect(nowPlayingButton, SIGNAL(mediaStatusChanged()), this, SLOT(updateTimecode()), Qt::QueuedConnection );
    }
    else
    {
        qDebug() << "Couldnt get skin data!";
        exit(15);
    }
    iPK_Device= deviceno;
    pqOrbiter = new DCE::qOrbiter(iPK_Device, routerip.toStdString(), true,false);
}


void qorbiterManager::gotoQScreen(QString s)
{
    //send the qmlview a request to go to a screen, needs error handling

#ifdef for_droid

#else
    QVariant screenname= s;
    QVariant returnedValue;
    QObject *item = qorbiterUIwin->rootObject();
    QMetaObject::invokeMethod(item, "screenchange",  Q_ARG(QVariant, screenname));
    currentScreen = s;
#endif
}

//this block sets up the connection to linuxmce
bool qorbiterManager::setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode)
{

pqOrbiter->qmlUI = this;

    if ( pqOrbiter->GetConfig() && pqOrbiter->Connect(pqOrbiter->PK_DeviceTemplate_get()) )
    {

        /*
              we get various variable here that we will need later. I store them in the qt object so it
              can pass them along without extra issues and so they can easily be passed to qml objects that
              are templated in
             */


        /*
              this line ties the class variable in the dceGenerated code to the qt ui code
              this is how the two threads (dce and qt) communicate with each other and make it possible to connect
              qt GUI (qml or qobject based) signals to DCE slots and vice versa!
            */


        if (getConf(PK_Device) == true )        //getting configuration from qOrbiterGen xml file
        {
            setConnectedState(true);

            if (pqOrbiter->initialize()) //the dcethread initialization
            {
                pqOrbiter->CreateChildren();
                qorbiterUIwin->rootContext()->setContextProperty("dcerouter", pqOrbiter );
                gotoQScreen("Screen_1.qml");
                return false;
            }
            else
            {
                qDebug() << "Orbiter Conf Hiccup!";
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device Failed to get configuration!");
                return true;
            }
        }
        else
        {
            qDebug() << "Config invalid";
            return true;
        }
    }
    else
    {
        bAppError = true;

        if( pqOrbiter->m_pEvent && pqOrbiter->m_pEvent->m_pClientSocket && pqOrbiter->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
        {
            bAppError = false;
            bReload = false;
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No Router.  Will abort");
            qDebug() << " orbiter -setup No Router, Aborting";
            pqOrbiter->Disconnect();

            return true;

        }
        else if( pqOrbiter->m_pEvent && pqOrbiter->m_pEvent->m_pClientSocket && pqOrbiter->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_BadDevice )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Invalid Device ID - Please run setup in webadmin");
            qDebug() << "Invalid Device ID - Please run setup in webadmin";
            gotoQScreen("Splash.qml");
            pqOrbiter->Disconnect();
            return true;
        }
    }

}

bool qorbiterManager::refreshUI()
{

    //  qDebug() << "Launching UI";
    return true;
}


// get conf method that reads config file
bool qorbiterManager::getConf(int pPK_Device)
{

    //device variables
    iPK_Device_DatagridPlugIn =  long(6);
    iPK_Device_OrbiterPlugin = long(9);
    iPK_Device_GeneralInfoPlugin = long(4);
    iPK_Device_SecurityPlugin = long(13);
    iPK_Device_LightingPlugin = long(8);
    m_dwIDataGridRequestCounter = 0;

    sleeping_alarms.clear();
    qorbiterUIwin->rootContext()->setContextProperty("alarms", QVariant::fromValue(sleeping_alarms) );

    updatedObjectImage.load(":/icons/videos.png");
    QObject::connect(this,SIGNAL(objectUpdated()), nowPlayingButton, SIGNAL(imageChanged()) );

    //----------------Security Video setup
    SecurityVideo = new SecurityVideoClass();
    qorbiterUIwin->rootContext()->setContextProperty("securityvideo", SecurityVideo);
    // qDebug() << "Reading Config" ;
    iPK_Device = long(pPK_Device);
    iOrbiterPluginID = 9;
    iMediaPluginID = 10;
    iPK_Device_eventPlugin = 12;
    iSize = 0;
    m_pOrbiterCat = 5;
    s_onOFF = "1";
    // qDebug() << "PK_Device No:" << iPK_Device;

    QDomDocument configData;
    const QByteArray tConf = binaryConfig.data();
    configData.setContent(tConf,false);
    if(configData.isDocument() == false)
    {
        qDebug() << "Invalid config for device: " << iPK_Device;
        qDebug() << "Please run http://dcerouter/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device) ;

        return false;
    }

    QDomElement root = configData.documentElement();        //represent configuration in memeory
    // qDebug () << root.tagName();
    //------------DEFAULTS-FOR-ORBITER------------------------------------------------------------
    QDomElement defaults = root.firstChildElement("Default");
    QString sPK_User = defaults.attribute("sPK_User");
    iFK_Room = defaults.attribute("DefaultRoom").toInt();
    iea_area = defaults.attribute("DefaultEA").toInt();
    iPK_User = defaults.attribute("PK_User").toInt();

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

        qDebug () << "/usr/pluto/orbiter/floorplans/inst" << m_installation.toStdString().c_str()<<"/"<<StringUtils::itos(m_page).c_str()<<".png";
        QImage m_image = pqOrbiter->getfileForDG("/usr/pluto/orbiter/floorplans/inst"+m_installation.toStdString()+"/"+StringUtils::itos(m_page)+".png");
        QImage icon = m_image.scaledToHeight(100);
        floorplans->appendRow(new FloorPlanItem(m_installation,m_description, m_page, m_iconpath, m_image, icon,  userList));
        // qDebug() << m_description;
        if (m_page == 1)
        {
            QString safe = m_installation;
            floorplans->setCurrentPage(safe.append("-").append(QString::number(m_page)));
        }

    }
    modelimageprovider = new AbstractImageProvider(this);
    qorbiterUIwin->engine()->addImageProvider("listprovider", modelimageprovider);


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
            imagePath = QUrl("../../../img/icons/backgrounds/livingroom.png");
            break;
        case 3:
            imagePath = QUrl("../../../img/icons/backgrounds/bedroom.png");
            break;
        case 5:
            imagePath = QUrl("../../../img/icons/backgrounds/kitchen.png");
            break;
        case 11:
            imagePath = QUrl("../../../img/icons/backgrounds/mstrbedroom.png");
            break;
        default:
            imagePath = QUrl("../../../img/lmcesplash.png");
            break;
        }

        RroomMapping.insert(m_name, m_val);

        m_lRooms->appendRow(new LocationItem(m_name, m_val, m_title, m_iEA, m_iType, imagePath, m_lRooms));
    }
    m_lRooms->sdefault_Ea = defaults.attribute("DefaultLocation");
    m_lRooms->idefault_Ea = RroomMapping.value(m_lRooms->sdefault_Ea);


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
    qorbiterUIwin->rootContext()->setContextProperty("dcemessage", dceResponse); //file grids current media type
    QObject::connect(this->nowPlayingButton, SIGNAL(mediaStatusChanged()), this, SLOT(updateTimecode()));

    //epg listmodel, no imageprovider as of yet
    simpleEPGmodel = new EPGChannelList(new EPGItemClass, this );
    qorbiterUIwin->rootContext()->setContextProperty("simpleepg", simpleEPGmodel);
    processingThread = new QThread(this);
    simpleEPGmodel->moveToThread(processingThread);
    processingThread->start(QThread::LowPriority);
    QObject::connect(this,SIGNAL(liveTVrequest()), simpleEPGmodel,SLOT(populate()), Qt::QueuedConnection);
    //        qDebug () << "Orbiter Registered, starting";

    //------------not sure if neccesary since it knows where we are.
    setActiveRoom(iFK_Room, iea_area);

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

    connect(nowPlayingButton, SIGNAL(mediaStatusChanged()), this , SLOT(updateTimecode()));

    qDebug() << "Cleanup config - tconf:" << tConf.size() << "|| binaryConfig:" << binaryConfig.size() << "|| configData:" << configData.childNodes().size();
    return true;
}

//experimental skin swappping method. what should happen here is a call to qDeclarative engine to change
//source import directory, thereby changing the effective skin. This is why the base compnents need to be
//seperate from the screens them selves. the screen can and will refer to base standard object and if people
//want to create their own, the can and simple change the component import directory, all without compiling.

void qorbiterManager::swapSkins(QString incSkin)
{
#ifdef Q_OS_MAC
    QDir skinsDir(QApplication::applicationDirPath().remove("MacOS").append("Resources"));
#else
    QDir skinsDir(QApplication::applicationDirPath().remove("bin"));
#endif

    if(skinsDir.cd("qml/"))
    {
        //qDebug() << skinsDir.path();

        QStringList platform = skinsDir.entryList();

        // qDebug() << "Switching to:" << platform.last();


        if(platform.isEmpty())
        {
            //  qDebug() << "couldnt locate platform";
        }
        else
        {

            if(skinsDir.cd(platform.last()))
            {
                incSkin.toLower() = skinsDir.path();
                //qDebug() << "Looking for skins in " + skinsDir.path() ;
                QDeclarativeComponent skinData(qorbiterUIwin->engine(),QUrl::fromLocalFile(skinsDir.path()+"/"+incSkin.toLower()+"/Style.qml"));
                qDebug() << skinsDir.path()+"/"+incSkin.toLower()+"/Style.qml" ;
                //turning it into a qObject - this part actually loads it - the error should connect to a slot and not an exit
                QObject *styleObject = skinData.create(qorbiterUIwin->rootContext());
                //wait for it to load up
                while (!skinData.isReady())
                {
                    if(skinData.isError())
                    {
                        qDebug() << skinData.errors();
                        break;
                    }

                    qDebug() << skinData.status();
                }

                qorbiterUIwin->engine()->rootContext()->setContextProperty("style", styleObject);

                qorbiterUIwin->setSource(skinsDir.path()+"/"+incSkin.toLower()+"/main.qml");
                currentSkin = incSkin;
                gotoQScreen("Screen_1.qml");
                writeConfig();
            }
        }
    }
}


//takes care of un-registering the orbiter from the DCERouter and then shutting down
void qorbiterManager::closeOrbiter()
{
    qDebug() << "Shutting Down";
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Exiting, Unregistering 1st");
    // processingThread->quit();
    pqOrbiter->deinitialize();
    pqOrbiter->~qOrbiter();
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
    /* */
    qDebug() << "CHanged to room" << room;
    setLocation(room, ea);
}


void qorbiterManager::execGrp(int grp)
{
    pqOrbiter->executeCommandGroup(grp);
    i_current_command_grp = grp;
    qorbiterUIwin->rootContext()->setContextProperty("currentcommandgrp", i_current_command_grp);
}


bool qorbiterManager::addMediaItem(QString mText, QString temp, QImage cell)
{

    //const char *tcell = reinterpret_cast<char*>(cell.bits());
    if (cell.isNull())
    {
        //qDebug() << mText << " is null img";
    }
    //this->model->appendRow(new gridItem(mText, temp, cell , model));
    emit modelChanged();
    // qorbiterUIwin->rootContext()->setContextProperty("dataModel", model);
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
   // qDebug() << "Moving to Location:" << room << "with ea: " << ea;
    emit locationChanged(room, ea);
    pqOrbiter->setLocation(room, ea);

}

int qorbiterManager::getlocation() const
{
    return iFK_Room;
}

void qorbiterManager::regenOrbiter(int deviceNo)
{
    gotoQScreen("Splash.qml");
    qDebug() << "Regenerating..";
    qDebug() << qs_routerip;

    if(qs_routerip =="192.168.80.1")
    {
       gotoQScreen("LoadError.qml");
        qDebug("remote orbiter");
    }
    else
    {
        qDebug("local orbiter");
    QProcess *regen = new QProcess(this);
    regen->start("/usr/bin/php /var/www/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device), QIODevice::ReadOnly);
    qDebug() <<"status code:" << regen->errorString();
    QObject::connect(regen,SIGNAL(finished(int)), this, SLOT(regenComplete(int)));
    QObject::connect(regen,SIGNAL(error(QProcess::ProcessError)), this, SLOT(regenError(QProcess::ProcessError)));
    }


}

void qorbiterManager::regenComplete(int i)
{
    if (i == 0)
    {
        if (getConf(iPK_Device))
        {   qDebug() << "Regen Complete!";
            // qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/qOrbiter_src/main.qml"));
            gotoQScreen("Screen_1.qml");
        }
    }
    else
    {
        gotoQScreen("LoadError.qml");
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
    //qDebug() << "Setting grid mediatype to :" << i;
    q_mediaType = QString::number(i);
    this->qorbiterUIwin->rootContext()->setContextProperty("gmediaType", q_mediaType);
    emit gridTypeChanged();
}

void qorbiterManager::setNowPlayingIcon(bool b)
{
    QColor color;
    color.fromRgb(255, 255, 215);
    qorbiterUIwin->rootContext()->setContextProperty("nowPlayingColor", color);
    item->setProperty("nowplayingtext", "null");
}

void qorbiterManager::getcurrentSkins(QStringList skinPaths)
{
    QImage skinPic(":/icons/Skin-Data.png");
    SkinDataModel* tskinModel = new SkinDataModel(new SkinDataItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("skinsList", tskinModel);
    QString skins;

#ifdef Q_OS_MAC
    QDir skinsDir(QApplication::applicationDirPath().remove("MacOS").append("Resources"));
#else
    QDir skinsDir(QApplication::applicationDirPath().remove("bin"));
#endif
    if(skinsDir.cd("qml/"))
    {
       qDebug() << skinsDir.path();

        QStringList platform = skinsDir.entryList();

        qDebug() << "Switching to:" << platform.last();


        if(platform.isEmpty())
        {
            qDebug() << "Could not locate skins for platform!";
            exit(10);
        }
        else
        {

            if(skinsDir.cd(platform.last()))
            {

                skins = skinsDir.path();
                 qDebug() << "Looking for skins in " + skins ;

            }

        }

        skins.append("/");
        QStringList::const_iterator constIterator;
        for (constIterator = skinPaths.constBegin(); constIterator != skinPaths.constEnd(); ++constIterator)
        {

            if ((*constIterator) == "js"||(*constIterator)=="components"||(*constIterator) =="screens")
            {
                // qDebug() << "System Path" << (*constIterator);
            }
            else
            {
                QDeclarativeComponent skinData(qorbiterUIwin->engine(),QUrl::fromLocalFile(skins+(*constIterator)+"/Style.qml"));
                if (skinData.isError()) {
                    // this dir does not contain a Style.qml; ignore it
                    break;
                }
                //turning it into a qObject - this part actually loads it - the error should connect to a slot and not an exit
                QObject *styleObject = skinData.create(qorbiterUIwin->rootContext());
                //wait for it to load up
                while (!skinData.isReady())
                {
                    if(skinData.isError())
                    {
                        qDebug() << skinData.errors();
                        break;
                    }

                    qDebug() << " loading";
                }
                //Importing the document data for the skins themselves
                QString s_title = styleObject->property("skinname").toString();
                QString s_creator = styleObject->property("skincreator").toString();
                QString s_description = styleObject->property("skindescription").toString();
                QString s_version = styleObject->property("skinversion").toString();
                QString s_target = styleObject->property("skinvariation").toString();
                QString s_path = styleObject->property("skindir").toString();
                QString s_mainc = styleObject->property("maincolor").toString();
                QString s_accentc = styleObject->property("accentcolor").toString();
                // qDebug() << "Adding skin to list" << s_title;
                tskinModel->appendRow(new SkinDataItem(s_title, s_creator, s_description, s_version, s_target, skinPic, s_path, s_mainc, s_accentc, tskinModel ));
            }
        }
    }
}

void qorbiterManager::quickReload()
{

    //gotoQScreen("Splash.qml");
    bool connected = pqOrbiter->m_bRouterReloading;

    if(setupLmce(iPK_Device, s_RouterIP, false, false))
    {
        qDebug() << "Reload complete, starting";

    }
    else
    {
        qDebug() << "Router not up, waiting";
        sleep(5);
        quickReload();
    }
}

void qorbiterManager::qmlSetupLmce(int incdeviceid, QString incrouterip)
{
    qs_routerip = incrouterip;
    iPK_Device = incdeviceid;
    writeConfig();
    setupLmce(incdeviceid, incrouterip.toStdString(), false, false);

}


bool qorbiterManager::readLocalConfig()
{
    QDomDocument localConfig;
#ifdef Q_OS_MAC
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("MacOS").append("Resources").append("/config.xml").toStdString());
#else
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#endif
    QFile localConfigFile;

    localConfigFile.setFileName(xmlPath);
   // qDebug() << xmlPath;
    if (!localConfigFile.open(QFile::ReadWrite))
    {
        return false;

    }
    else
    {

        if (!localConfig.setContent( &localConfigFile))
        {
            qDebug() << "Local Config Missing!";
            qDebug() <<"Enter information to continue, press connect when ready.";
            localConfigFile.close();
            return false;
        }
        else
        {

            QDomElement configVariables = localConfig.documentElement().toElement();

            if(qs_routerip.isEmpty())
            {
            qs_routerip = configVariables.namedItem("routerip").attributes().namedItem("id").nodeValue();
            }
            currentSkin = configVariables.namedItem("skin").attributes().namedItem("id").nodeValue();
            iPK_Device = configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong();


            if(!qs_routerip.isNull())
            {

            }
            else
            {
                qDebug() << "Could not read local, setting defaults.";
                qDebug() << localConfig.toString();               
                localConfigFile.close();
                return false;
            }
        }
        localConfigFile.close();
        return true;
    }
}



void qorbiterManager::writeConfig()
{
    QDomDocument localConfig;
#ifdef Q_OS_MAC
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().remove("MacOS").append("Resources").append("/config.xml").toStdString());
#else
    QString xmlPath = QString::fromStdString(QApplication::applicationDirPath().toStdString())+"/config.xml";
#endif
    QFile localConfigFile;
   // qDebug() << "Writing Config";
   // qDebug() << xmlPath;
    localConfigFile.setFileName(xmlPath);
    if (!localConfigFile.open(QFile::ReadOnly))
    {

        qDebug() << "Error! Cant Save Config!";

    }
    else
    {

        if (!localConfig.setContent( &localConfigFile))
        {
            qDebug() << "Local Config XML  ERROR!";
        }
        else
        {
            localConfigFile.close();
            QDomElement configVariables = localConfig.documentElement().toElement();

            configVariables.namedItem("routerip").attributes().namedItem("id").setNodeValue(qs_routerip);
            configVariables.namedItem("skin").attributes().namedItem("id").setNodeValue(currentSkin);
            configVariables.namedItem("device").attributes().namedItem("id").setNodeValue(QString::number(iPK_Device));

            QByteArray output = localConfig.toByteArray();
            localConfigFile.open(QFile::WriteOnly);
            if (!localConfigFile.write(output))
            {
                qDebug() << "write failed";
            }
            localConfigFile.close();

        }


    }
    localConfigFile.close();
}

void qorbiterManager::setStringParam(int paramType, QString param)
{
    /*
    QString q_mediaType;           //1 passed in inital dg request
    QString q_subType;             //2
    QString q_fileFormat;          //3
    QString q_attribute_genres;    //4
    QString q_mediaSources;         //5 needs comma seperator

    QString q_usersPrivate;        //6
    QString q_attributetype_sort;  //7
    QString q_pk_users;             //8
    QString q_last_viewed;        //9
    QString q_pk_attribute;        //10
    QString *datagridVariableString;
    */
    QStringList longassstring;
    QString datagridVariableString;
    requestMore = false;
    switch (paramType)
    {
    case 1:
        q_subType = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        //qDebug() << datagridVariableString;
        execGrp(i_current_command_grp);

        break;
    case 2:
        q_fileFormat = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        // qDebug() << datagridVariableString;
        execGrp(i_current_command_grp);

        break;
    case 3:
        q_attribute_genres = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
//        qDebug() << datagridVariableString;

        execGrp(i_current_command_grp);


        break;
    case 4:
     //   qDebug() << "Filter Parameter: " << param;
        if (q_mediaSources != param.remove("!D"))
        {
            if(param.contains("!F"))
            {
                filedetailsclass->setVisible(true);
                filedetailsclass->setFile(param);
                showFileInfo(param);
                qDebug () << "info tripped";
                break;

                //    pqOrbiter->GetMediaAttributeGrid(param);

            }
            else if (param.contains("!P"))
            {
                filedetailsclass->setVisible(true);
                filedetailsclass->setFile(param);
                showFileInfo(param);
                break;
            }

            else
            {
                q_pk_attribute = param.remove("!A");
               longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
                datagridVariableString = longassstring.join("|");
                //   qDebug() << datagridVariableString;
                execGrp(i_current_command_grp);
            }
        }
        //goBack<<longassstring;

        break;
    case 5:
        q_usersPrivate = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        // qDebug() << datagridVariableString;
        execGrp(i_current_command_grp);

        break;
    case 6:

        if (param.contains("!P"))
        {
            filedetailsclass->setVisible(true);
            filedetailsclass->setFile(param);
            break;
        }
        else
        {
            if(backwards ==false)
            {
            q_attributetype_sort = param;
            }
            longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
            datagridVariableString = longassstring.join("|");
          //  qDebug() << datagridVariableString;
           // goBack<<longassstring;
            execGrp(i_current_command_grp);

            break;

        }
    case 7:
        q_pk_users = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        //  qDebug() << datagridVariableString;
        execGrp(i_current_command_grp);

        break;
    case 8:
        q_last_viewed = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        //   qDebug() << datagridVariableString;
        execGrp(i_current_command_grp);

        break;
    case 9:
        if(param.contains("!F"))
        {

            filedetailsclass->setVisible(true);
            filedetailsclass->setFile(param);
            break;
            //   pqOrbiter->GetMediaAttributeGrid(param);

        }
        else if (param.contains("!P"))
        {
            filedetailsclass->setVisible(true);
            filedetailsclass->setFile(param);
            break;
        }

        else
        {
            q_pk_attribute = param.remove("!A");

            longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
            datagridVariableString = longassstring.join("|");
            //    qDebug() << datagridVariableString;
            execGrp(i_current_command_grp);
            break;
        }

    default:
        cout << "no type";

    }



    // initializeSortString();
}

void qorbiterManager::initializeSortString()
{
    QString datagridVariableString ;
    //datagrid option variables
    //  QString q_mediaType;           //1
    q_subType="";             //2
    q_fileFormat="";          //3
    q_attribute_genres="";    //4
    q_mediaSources ="1,2";         //5 need comma delineation
    q_usersPrivate = "0";        //6
    q_attributetype_sort="13";  //7
    q_pk_users="0";             //8
    q_last_viewed="2";        //9
    q_pk_attribute="";        //10


    datagridVariableString.append(q_mediaType).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);
    goBack.clear();
   // goBack.append(datagridVariableString);
    qDebug() << "Dg Variables Reset";

}

void qorbiterManager::initializeContexts()
{

}

void qorbiterManager::initializeGridModel()
{

    playlistModel = new ListModel(new gridItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("playlistmodel", playlistModel);

    //datagrid model setup with image provider for grid
    model = new ListModel(new gridItem, this);
    basicProvider = new basicImageProvider();
    advancedProvider = new GridIndexProvider(model , 6, 4);
    QObject::connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex, int )), advancedProvider,SLOT(dataUpdated(QModelIndex,QModelIndex, int)), Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(requestMoreGridData()), model,SLOT(checkForMore()));
    //adding important data and objects to qml now that they have been setup
    qorbiterUIwin->rootContext()->setContextProperty("dataModel", model);
    qorbiterUIwin->engine()->addImageProvider("datagridimg", advancedProvider);
    qorbiterUIwin->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());
}

void qorbiterManager::goBackGrid()
{
    setRequestMore(false);
    backwards = true;
    qDebug() << q_attributetype_sort;
    execGrp(i_current_command_grp);

}

void qorbiterManager::showFileInfo(QString fk_file)
{

    pqOrbiter->GetMediaAttributeGrid(fk_file);
}

void qorbiterManager::requestSecurityPic(int i_pk_camera_device, int h, int w)
{
    pqOrbiter->GetSingleSecurityCam( i_pk_camera_device,  h,  w);
}

void qorbiterManager::playMedia(QString FK_Media)
{
    pqOrbiter->StartMedia(FK_Media);

}

void qorbiterManager::stopMedia()
{
    pqOrbiter->StopMedia();
}

void qorbiterManager::rw_media(int speed)
{
    nowPlayingButton->setMediaSpeed(speed);
    pqOrbiter->RwMedia();
}

void qorbiterManager::ff_media(int speed)
{
    nowPlayingButton->setMediaSpeed(speed);
    pqOrbiter->FfMedia();
}

void qorbiterManager::pauseMedia()
{
    pqOrbiter->PauseMedia();
}

void qorbiterManager::showfloorplan(int fptype)
{
    pqOrbiter->ShowFloorPlan(fptype);
}

void qorbiterManager::bindMediaRemote(bool state)
{

}

void qorbiterManager::changedPlaylistPosition(QString position)
{

    if(!position.contains(QRegExp("TITLE:")))
    {
    pqOrbiter->JumpToPlaylistPosition(position.toInt());
    }
    else
    {
        setPosition(position);
    }


}

void qorbiterManager::setNowPlayingData()
{

    pqOrbiter->BindMediaRemote(true);
    pqOrbiter->requestMediaPlaylist();

    updateTimecode();

}

void qorbiterManager::getMediaPlaylist()
{

}

void qorbiterManager::updateImageChanged(QImage img)
{
    updatedObjectImage = img;
    emit objectUpdated();
}

void qorbiterManager::setNowPlayingTv()
{
    pqOrbiter->BindMediaRemote(true);
    pqOrbiter->requestMediaPlaylist();


}

void qorbiterManager::changeChannels(QString chan)
{
    pqOrbiter->TuneToChannel(chan.toInt(), chan );
}

void qorbiterManager::getLiveTVPlaylist()
{
   // qDebug() << "Orbiter Manager slot called";
    //emit liveTVrequest();

}

void qorbiterManager::gridChangeChannel(QString chan, QString chanid)
{
    pqOrbiter->TuneToChannel(chan.toInt(), chanid);


}

void qorbiterManager::adjustLighting(int l)
{

    pqOrbiter->SetLightingLevel(l);

}

void qorbiterManager::adjustVolume(int vol)
{
    if (vol < 0)
    {
        pqOrbiter->VolumeUp(vol);
    }
    else
    {
        pqOrbiter->VolumeDown(vol);
    }
}

void qorbiterManager::mute()
{
    pqOrbiter->mute();
}

void qorbiterManager::changedTrack(QString increment)
{
    pqOrbiter->ChangedTrack(increment);
}

void qorbiterManager::setHouseMode(int mode, int pass)
{
    pqOrbiter->SetSecurityMode(pass, mode);
}

void qorbiterManager::setCurrentUser(QString inc_user)
{
    sPK_User = userList->find(sPK_User)->data(1).toString();
    int user = inc_user.toInt();
    qDebug() << "User name" << sPK_User << "::" << user;
    pqOrbiter->setUser(user);
    emit userChanged();
}

void qorbiterManager::setRequestMore(bool state)
{
    requestMore = state;
    emit requestMoreGridData();
}

bool qorbiterManager::getRequestMore()
{
    return requestMore;
}

void qorbiterManager::sleepingMenu(bool toggle, int grp)
{
   // qDebug() << grp;
    if(toggle == true)
    {   sleeping_alarms.clear();
        pqOrbiter->GetAlarms(toggle, grp);
    }
    else
    {
        sleeping_alarms.clear();
        pqOrbiter->GetAlarms(false, 0);
    }

}

void qorbiterManager::setZoom(QString qs_zoom)
{
    pqOrbiter->SetZoom(qs_zoom);
}

void qorbiterManager::setAspect(QString qs_aspect)
{
    pqOrbiter->SetAspectRatio(qs_aspect);
}

void qorbiterManager::updateTimecode()
{
    string sIPAddress;
    if(nowPlayingButton->b_mediaPlaying == true && !timeCodeSocket->isValid())
    {
        //qDebug("media playing with no timecode!");
        if(!timeCodeSocket->isOpen())

        {
            //qDebug("socket is closed, opening!");
            DeviceData_Base *pDevice = pqOrbiter->m_dwPK_Device_NowPlaying ? pqOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pqOrbiter->m_dwPK_Device_NowPlaying) : NULL;
            if( NULL != pDevice &&
                    (
                        pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_Xine_Player_CONST ||
                        pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_MPlayer_Player_CONST
                        )
                    )
            {
                sIPAddress = pDevice->m_sIPAddress;
                qDebug() << sIPAddress.c_str();
                if( sIPAddress.empty() )
                {
                    //qDebug() << "ip empty...";
                    if( pDevice->m_pDevice_MD && !pDevice->m_pDevice_MD->m_sIPAddress.empty() )
                        sIPAddress = pDevice->m_pDevice_MD->m_sIPAddress;
                    else if( pDevice->m_pDevice_Core && !pDevice->m_pDevice_Core->m_sIPAddress.empty() )
                        sIPAddress = pDevice->m_pDevice_Core->m_sIPAddress;
                    else
                    {
                        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter::CMD_Set_Now_Playing  Xine has no IP address");
                        return;
                    }
                }
            }
            QString port = QString::fromStdString(pqOrbiter->GetCurrentDeviceData(pqOrbiter->m_dwPK_Device_NowPlaying, 171));
            if (sIPAddress.length() == 0 && i_current_mediaType == 11)
            {
               qDebug() << "IpAddress empty, error!: " <<  sIPAddress.c_str();
                return;
            }
            else
            {
                timeCodeSocket->connectToHost(QString::fromStdString(sIPAddress), port.toInt(), QFile::ReadOnly );
                if ( timeCodeSocket->isValid() )
                {
                    qDebug() << "Time Code Socket connected! " << sIPAddress.c_str();
                    QObject::connect(timeCodeSocket,SIGNAL(readyRead()), this, SLOT(showTimeCode()), Qt::QueuedConnection);
                }
            }
        }
    }
    else if (nowPlayingButton->b_mediaPlaying == false)
    {
        timeCodeSocket->disconnectFromHost();
        timeCodeSocket->close();
        qDebug("Socket 12000 disconnect");
    }
}

void qorbiterManager::showTimeCode()
{
    QByteArray socketData = timeCodeSocket->readLine();
    QString tcData = QString::fromAscii(socketData.data(), socketData.size());
    //qDebug() << tcData;
    if (tcData.length() > 0)
    {
        QStringList tcVars = tcData.split(",");
        QString tcClean = tcVars.at(1);
        tcClean.remove(QRegExp(".\\d\\d\\d|00:0|0:0|00:"));
        nowPlayingButton->setTimeCode(tcClean);

        QString playbackSpeed = tcVars.at(0);
        playbackSpeed.remove(QRegExp("000"));
        nowPlayingButton->setStringSpeed(playbackSpeed+"x");

        QString duration = tcVars.at(2);
        duration.remove(QRegExp(".\\d\\d\\d|00:0|0:0|00:"));
        nowPlayingButton->setDuration(duration);
    }
}

void qorbiterManager::checkConnection()
{
    setDceResponse("Connection Problem");
    if(pqOrbiter->m_bQuit_get() == true)
    {
        setConnectedState(false);
    }

    if( pqOrbiter->GetConfig() && pqOrbiter->Connect(pqOrbiter->PK_DeviceTemplate_get()))
    {
        if(!pqOrbiter->GetConfig())
        {
            gotoQScreen("Splash.qml");
        }

    }
}

void qorbiterManager::setPosition(QString position)
{
     qDebug() << position;
     pqOrbiter->SetMediaPosition(position);
}

void qorbiterManager::showMenu()
{
    pqOrbiter->DvdMenu();
}

void qorbiterManager::moveDirection(QString direction)
{
    pqOrbiter->NavigateScreen(direction);
}

void qorbiterManager::enter()
{

}

void qorbiterManager::jogPosition(QString jog)
{
    pqOrbiter->JogStream(jog);
}

void qorbiterManager::regenError(QProcess::ProcessError)
{
    qDebug("Error!");

}




