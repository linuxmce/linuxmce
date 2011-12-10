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
#include <datamodels/skindatamodel.h>

//#include "OrbiterData.h"


/*
  this file is responsible for the main connection between the dce thread and the qt thread.
   pqOrbiter->qmlUI = this; inside of setupLMCE initializes the dce thread, while other functions are
   responsible for signals and slots between dce and qml.
  */

using namespace DCE;

/*
  this is the constructor. it should be refactored to intantiatae the qorbiter class from the start to facilitate
  cleaner startup with more robust error checking.

  This class will evolve into a manager role, with the qml window handling being done elsewhere. we will see how this goes.
*/
qorbiterManager::qorbiterManager(int deviceno, QString routerip, QObject *parent) :
    QObject(parent),iPK_Device(deviceno), qs_routerip(routerip)
{
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::transparent);


    splashWindow = new QMainWindow(0, Qt::FramelessWindowHint);     //window for splash
    splashWindow->setAttribute(Qt::WA_TranslucentBackground, true); //setting transparent bg?

    splashView = new QDeclarativeView; //qdeclarativeview for splash.qml
    splashView->setAttribute(Qt::WA_TranslucentBackground, true);     //doing the same for qdeclarativeview

    splashWindow->setCentralWidget(splashView);                      //adding view to window

    splashView->setPalette(palette);
    splashView->setSource((QUrl("qrc:desktop/Splash.qml")));

    splashWindow->showNormal();

    QObject::connect(this, SIGNAL(orbiterReady()), splashWindow,SLOT(close()));



    // QObject * item=dynamic_cast<QObject*>(mainView.rootObject());

    qorbiterUIwin = new QDeclarativeView() ;  //initialize the declarative view to act upon its context
    qorbiterUIwin->rootContext()->setContextProperty("srouterip", QString(qs_routerip) );
    qorbiterUIwin->rootContext()->setContextProperty("deviceid", QString::number((iPK_Device)) );
    qorbiterUIwin->rootContext()->setContextProperty("manager", this); //providing a direct object for qml to call c++ functions of this class


    item=dynamic_cast<QObject*>(qorbiterUIwin->rootObject());
    QApplication::processEvents(QEventLoop::AllEvents);
    initializeConnections();

    //TODO, extract to a config
    //TODO, execute this already if we have config data.
    //setupLmce(iPK_Device, routerip.toStdString(), true, false);
}


void qorbiterManager::startSetup() {


    if (readLocalConfig())
    {
        emit localConfigReady("Success");

    }

    QString localDir = QApplication::applicationDirPath();

    loadSkins(QUrl(localDir+"/qml/desktop"));
    //loadSkins(QUrl("http://192.168.80.1/lmce-admin/skins/desktop"));
    //set it as a context property so the qml can read it. if we need to changed it,we just reset the context property

    initializeGridModel();  //begins setup of media grid listmodel and its properties
    initializeSortString(); //associated logic for navigating media grids

    //reference to the object for later use?


    //initial signal and slot connections
    //        QObject::connect(item, SIGNAL(close()), this, SLOT(closeOrbiter()));
    //        QObject::connect(this,SIGNAL(orbiterReady()), this,SLOT(startOrbiter()));
    //        QObject::connect(parent,SIGNAL(destroyed()), this, SLOT(close()));


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

    //QObject::connect(nowPlayingButton, SIGNAL(mediaStatusChanged()), this, SLOT(updateTimecode()), Qt::QueuedConnection );
    //iPK_Device= deviceno;

    setupLmce((int)iPK_Device, qs_routerip.toStdString(), false, false);

}


void qorbiterManager::gotoQScreen(QString s)
{
    //send the qmlview a request to go to a screen, needs error handling
    //This allows it to execute some transition or other if it wants to
    QVariant screenname= s;
    QObject *item = qorbiterUIwin->rootObject();
    QMetaObject::invokeMethod(item, "screenchange",  Q_ARG(QVariant, screenname));
    currentScreen = s;
}

//this block sets up the connection to linuxmce
void qorbiterManager::setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode)
{
    if(!pqOrbiter->m_bRunning)
    {
    pqOrbiter = new DCE::qOrbiter(iPK_Device, sRouterIP, true,false);
    }

    qDebug() << "Made orbiter";

    iPK_Device_DatagridPlugIn =  long(6);
    iPK_Device_OrbiterPlugin = long(9);
    iPK_Device_GeneralInfoPlugin = long(4);
    iPK_Device_SecurityPlugin = long(13);
    iPK_Device_LightingPlugin = long(8);
    m_dwIDataGridRequestCounter = 0;

    iPK_Device = long(PK_Device);
    iOrbiterPluginID = 9;
    iMediaPluginID = 10;
    iPK_Device_eventPlugin = 12;
    iSize = 0;
    m_pOrbiterCat = 5;
    s_onOFF = "1";

    /*
          this line ties the class variable in the dceGenerated code to the qt ui code
          this is how the two threads (dce and qt) communicate with each other and make it possible to connect
          qt GUI (qml or qobject based) signals to DCE slots and vice versa!
        */

    pqOrbiter->qmlUI = this;
     QObject::connect(pqOrbiter,SIGNAL(disconnected(QString)), this, SLOT(reloadHandler()));
    //connecting cross object slots - from dce/qt subclass(qorbiter) to pure qt (qorbitermanager)
    //QObject::connect(pqOrbiter,SIGNAL(disconnected(QString)), this, SLOT(processError(QString)));


    qDebug() << "Attempting connection" << iPK_Device;


    if ( pqOrbiter->GetConfig() && pqOrbiter->Connect(pqOrbiter->PK_DeviceTemplate_get()) )
    {
        qDebug() << "Orbiter connected ..";
        getConf(PK_Device);
        /*
              we get various variable here that we will need later. I store them in the qt object so it
              can pass them along without extra issues and so they can easily be passed to qml objects that
              are templated in
             */

        if (pqOrbiter->initialize()) //the dcethread initialization
        {
            pqOrbiter->CreateChildren();
            qorbiterUIwin->rootContext()->setContextProperty("dcerouter", pqOrbiter );
            setConnectedState(true);
            //return false;
        }
        else
        {
            qDebug() << "Orbiter Conf Hiccup!";
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device Failed to get configuration!");
            //return true;
        }
    }
    else
    {
        bAppError = true;

        if(pqOrbiter->m_pEvent && pqOrbiter->m_pEvent->m_pClientSocket && pqOrbiter->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
        {
            bAppError = false;
            bReload = false;
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "main loop No Router.  Will abort");
            qDebug() << " main loop- No Router, Aborting";
        }
        else
        {
            bAppError = true;
            if( pqOrbiter->m_pEvent&& pqOrbiter->m_pEvent->m_pClientSocket && pqOrbiter->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_BadDevice)
            {
                bAppError = false;
                bReload = false;
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Bad Device  Will abort");
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");

            }
        }
    }
}


void qorbiterManager::refreshUI()
{

splashWindow->showFullScreen();
qorbiterUIwin->close();
 QApplication::processEvents(QEventLoop::AllEvents);

 if (pqOrbiter->GetConfig())
 {
     qDebug("Regen success, loading...");
     getConf(iPK_Device);
 }
 else
 {
     qDebug("Cant Get Config!");
 }


}


// get conf method that reads config file
void qorbiterManager::getConf(int pPK_Device)
{

    QDomDocument configData;
    const QByteArray tConf = binaryConfig.data();
    configData.setContent(tConf,false);
    if(configData.isDocument() == false)
    {
        qDebug() << "Invalid config for device: " << iPK_Device;
        qDebug() << "Please run http://dcerouter/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device) ;

        emit error("Invalid Config");
    }
    else
    {
        qDebug() << "Attempting to write config";
        writeConfig();
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

    qorbiterUIwin->rootContext()->setContextProperty("screenshotAttributes", QVariant::fromValue(screenshotVars));
    QObject::connect(this->nowPlayingButton, SIGNAL(mediaStatusChanged()), this, SLOT(updateTimecode()));

    //epg listmodel, no imageprovider as of yet
    simpleEPGmodel = new EPGChannelList(new EPGItemClass, this );
    qorbiterUIwin->rootContext()->setContextProperty("simpleepg", simpleEPGmodel);
    processingThread = new QThread(this);
    simpleEPGmodel->moveToThread(processingThread);
    processingThread->start(QThread::LowPriority);
    QObject::connect(this,SIGNAL(liveTVrequest()), simpleEPGmodel,SLOT(populate()), Qt::QueuedConnection);
    //        qDebug () << "Orbiter Registered, starting";

    //qDebug() << "About to set room..";

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
    binaryConfig.clear();
    //tConf.clear();

    //----------------Security Video setup
    SecurityVideo = new SecurityVideoClass();
    qorbiterUIwin->rootContext()->setContextProperty("securityvideo", SecurityVideo);

    //-alarms
    sleeping_alarms.clear();
    qorbiterUIwin->rootContext()->setContextProperty("alarms", QVariant::fromValue(sleeping_alarms) );
    //---update object image
    updatedObjectImage.load(":/icons/videos.png");
    QObject::connect(this,SIGNAL(objectUpdated()), nowPlayingButton, SIGNAL(imageChanged()) );

    swapSkins(currentSkin);

}

void qorbiterManager::swapSkins(QString incSkin)
{
    qDebug() << "Initiating skin switch : " + incSkin;

    //get the skin URL from the skins model
    SkinDataItem* skin = tskinModel->find(incSkin);

    //load the skin style
    QDeclarativeComponent tskinData(qorbiterUIwin->engine(), skin->styleUrl());


    //wait for it to load up
    while (!tskinData.isReady())
    {
        if(tskinData.isError())
        {
            qDebug() << "error! " <<  tskinData.errors();
            break;
        }
    }
    //turning it into a qObject - this part actually loads it - the error should connect to a slot and not an exit
    QObject *styleObject = tskinData.create(qorbiterUIwin->rootContext());


    //load the actual skin entry point

    //qDebug() << "Skin switch url:" << skin->entryUrl();
    currentSkin = incSkin;
    qorbiterUIwin->engine()->rootContext()->setContextProperty("style", styleObject);
    qorbiterUIwin->setSource(skin->entryUrl());
    writeConfig();
    startOrbiter();
    //emit orbiterReady();

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


    qDebug() << "Regenerating..";
    qDebug() << qs_routerip;

    if(qs_routerip =="127.0.0.1")
    {

        splashWindow->showFullScreen();
        qorbiterUIwin->close();
        QApplication::processEvents(QEventLoop::AllEvents);
        qDebug("Onscreen orbiter");
        QProcess *regen = new QProcess(this);
        regen->start("/usr/bin/php /var/www/lmce-admin/qOrbiterGenerator.php?d="+QString::number(iPK_Device), QIODevice::ReadOnly);
        qDebug() <<"status code:" << regen->errorString();
        QObject::connect(regen,SIGNAL(finished(int)), this, SLOT(regenComplete(int)));
        QObject::connect(regen,SIGNAL(error(QProcess::ProcessError)), this, SLOT(regenError(QProcess::ProcessError)));

    }
    else
    {
       qDebug("remote orbiter");
       gotoQScreen("WebRegen.qml");
    }


}

void qorbiterManager::regenComplete(int i)
{
    if (i == 0)
    {
        getConf(iPK_Device);
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

void qorbiterManager::loadSkins(QUrl base)
{

    //qDebug() << base;
    tskinModel = new SkinDataModel(base, new SkinDataItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("skinsList", tskinModel);

    tskinModel->addSkin("default");
    tskinModel->addSkin("aeon");
}

void qorbiterManager::quickReload()
{

    //gotoQScreen("Splash.qml");
    bool connected = pqOrbiter->m_bRouterReloading;


}


void qorbiterManager::qmlSetupLmce(QString incdeviceid, QString incrouterip)
{
    //qDebug() << "Triggering connection to LMCE Device ID [" << incdeviceid << "] port Router Address [" << incrouterip << "]" ;
    qs_routerip = incrouterip;
    iPK_Device = incdeviceid.toInt();
    //writeConfig();
    setupLmce(incdeviceid.toInt(), incrouterip.toStdString(), false, false);
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
        emit error("config not found!");
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
            if (currentSkin == "")
            {
                currentSkin = "default";
            }

            iPK_Device = configVariables.namedItem("device").attributes().namedItem("id").nodeValue().toLong();


            if(!qs_routerip.isNull())
            {

            }
            else
            {
                qDebug() << "Could not read local, setting defaults.";
                // qDebug() << localConfig.toString();
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

        emit error("Error! Cant Save Config!");

    }
    else
    {

        if (!localConfig.setContent( &localConfigFile))
        {
            emit error("Local Config XML  ERROR!");
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
                emit error("write failed");
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
    qs_seek ="";

    datagridVariableString.append(q_mediaType).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);
    goBack.clear();
    // goBack.append(datagridVariableString);
    qDebug() << "Dg Variables Reset";
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
    //qDebug() << q_attributetype_sort;
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
    pqOrbiter->playMedia(FK_Media);

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



void qorbiterManager::changedPlaylistPosition(QString position)
{

    if(!position.contains(QRegExp("TITLE:")))
    {
        pqOrbiter->JumpToPlaylistPosition(position.toInt());
    }
    else
    {
        pqOrbiter->setPosition(position);
    }


}

void qorbiterManager::setNowPlayingData()
{
    pqOrbiter->BindMediaRemote(true);
    pqOrbiter->requestMediaPlaylist();
    updateTimecode();
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


void qorbiterManager::setHouseMode(int mode, int pass)
{
    pqOrbiter->SetSecurityMode(pass, mode);
}

void qorbiterManager::setCurrentUser(QString inc_user)
{
    sPK_User = userList->find(sPK_User)->data(1).toString();
    int user = inc_user.toInt();
    //qDebug() << "User name" << sPK_User << "::" << user;
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
                //qDebug() << sIPAddress.c_str();
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
                    //qDebug() << "Time Code Socket connected! " << sIPAddress.c_str();
                    QObject::connect(timeCodeSocket,SIGNAL(readyRead()), this, SLOT(showTimeCode()), Qt::QueuedConnection);
                }
            }
        }
    }
    else if (nowPlayingButton->b_mediaPlaying == false)
    {
        timeCodeSocket->disconnectFromHost();
        timeCodeSocket->close();
        //qDebug("Socket 12000 disconnect");
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

void qorbiterManager::jogPosition(QString jog)
{
    pqOrbiter->JogStream(jog);
}

void qorbiterManager::regenError(QProcess::ProcessError)
{
    qDebug("Error! Reloading existing config for device " + iPK_Device);
   getConf(iPK_Device);
}

bool qorbiterManager::cleanupData()
{
    roomLights= NULL;                 //current room scenarios model
    roomMedia=NULL;                   //current room media model
    /*    qorbiterUIwin->rootContext()->setContextProperty("currentRoomClimate", roomClimate);               //curent room climate model
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
*/
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
    qorbiterUIwin->setWindowTitle("LinuxMCE Orbiter " + QString::number(iPK_Device));
    qorbiterUIwin->setResizeMode(QDeclarativeView::SizeViewToRootObject);

emit orbiterReady();
//QApplication::processEvents(QEventLoop::AllEvents);
gotoQScreen("Screen_1.qml");
#ifdef Q_OS_SYMBIAN
    qorbiterUIwin->showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    qorbiterUIwin->showMaximized();
#elif defined(for_harmattan)
    qorbiterUIwin->showFullScreen();
#elif defined(for_desktop)
    qorbiterUIwin->showMaximized();
#else
    qorbiterUIwin->show();
#endif


    qDebug("Showing");

}

void qorbiterManager::processError(QString msg)
{
    gotoQScreen("Splash.qml");
    qDebug() << "Error:" << msg ;
}

void qorbiterManager::setActiveSkin(QString name)
{

}

void qorbiterManager::cleanupScreenie()
{
    mediaScreenShot = QImage();
    screenshotVars.clear();
    //qDebug("Cleaned up Screenshot model");
}

void qorbiterManager::initializeConnections()
{
    //connections to splash.qml slots
    QObject::connect(item,SIGNAL(splashLoaded()), this,SLOT(startSetup()));
    QObject::connect(this, SIGNAL(continueSetup()), this, SLOT(startSetup()));

    emit continueSetup();
}

void qorbiterManager::reloadHandler()
{
    QApplication::processEvents(QEventLoop::AllEvents);
   splashWindow->show();
    qorbiterUIwin->close();
    QApplication::processEvents(QEventLoop::AllEvents);
    pqOrbiter->Disconnect();
    sleep(15);

    emit continueSetup();
}



