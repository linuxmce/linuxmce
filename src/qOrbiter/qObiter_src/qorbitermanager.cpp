#include "qorbitermanager.h"
#include <QDeclarativeProperty>
#include <QDebug>
#include <QFile>
#include <QtXml/QtXml>
#include <QProcess>
#include <QDeclarativeEngine>
#include <QApplication>
//#include "OrbiterData.h"
//#include "DCERouter.h"

/*
  this file is responsible for the main connection between the dce thread and the qt thread.
   pqOrbiter->qmlUI = this; inside of setupLMCE initializes the dce thread, while other functions are
   responsible for signals and slots between dce and qml.
  */

using namespace DCE;

/*
// You can override this block if you don't want the app to reload in the event of a problem
extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
void DeadlockHandler(PlutoLock *pPlutoLock)
{
        // This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
        if( g_pCommand_Impl )
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Deadlock problem.  %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
                g_pCommand_Impl->OnReload();
        }
}
void SocketCrashHandler(Socket *pSocket)
{
        // This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
        if( g_pCommand_Impl )
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Socket problem. %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
                g_pCommand_Impl->OnReload();
        }
}
void Plugin_DeadlockHandler(PlutoLock *pPlutoLock)
{
        // This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
        if( g_pCommand_Impl && g_pCommand_Impl->m_pRouter )
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Plugin Deadlock problem.  %d Going to reload",g_pCommand_Impl->m_dwPK_Device);
                g_pCommand_Impl->m_pRouter->CrashWithinPlugin(g_pCommand_Impl->m_dwPK_Device);
        }
}
void Plugin_SocketCrashHandler(Socket *pSocket)
{
        if( g_pCommand_Impl && g_pCommand_Impl->m_pRouter )
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Plugin Socket problem.  %d",g_pCommand_Impl->m_dwPK_Device);
                // g_pCommand_Impl->m_pRouter->CrashWithinPlugin(g_pCommand_Impl->m_dwPK_Device);  // Don't reload plugins since sockets can fail
        }
}
//<-dceag-incl-e->

extern "C" {
        int IsRuntimePlugin()
        {
                // If you want this plug-in to be able to register and be used even if it is not in the Device table, set this to true.
                // Then the Router will scan for all .so or .dll files, and if found they will be registered with a temporary device number
                bool bIsRuntimePlugin=false;
                if( bIsRuntimePlugin )
                        return qOrbiter::PK_DeviceTemplate_get_static();
                else
                        return 0;
        }
}


//<-dceag-plug-b->
extern "C" {
        class Command_Impl *RegisterAsPlugIn(class Router *pRouter,int PK_Device,Logger *pPlutoLogger)
        {
                LoggerWrapper::SetInstance(pPlutoLogger);
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device: %d loaded as plug-in",PK_Device);

                qOrbiter *pqOrbiter = new qOrbiter(PK_Device, "localhost",true,false,pRouter);
                if( pqOrbiter->m_bQuit_get()|| !pqOrbiter->GetConfig() )
                {
                        delete pqOrbiter;
                        return NULL;
                }
                else
                {
                        g_pCommand_Impl=pqOrbiter;
                        g_pDeadlockHandler=Plugin_DeadlockHandler;
                        g_pSocketCrashHandler=Plugin_SocketCrashHandler;
                }
                return pqOrbiter;
        }
}
*/
//<-dceag-plug-e->

qorbiterManager::qorbiterManager(QWidget *parent) :
    QWidget(parent)
{


    currentSkin = "default";
    currentSkinURL="/qml/qObiter_src/";
    s_RouterIP="192.168.80.1";

    qorbiterUIwin = new QDeclarativeView;
    ScreenSaverModule ScreenSaver;
    qmlRegisterType<ScreenSaverModule>("ScreenSaverModule",1,0,"ScreenSaverModule");    
    ScreenSaver.setImage(QUrl("../../img/lmcesplash.jpg"));

    qorbiterUIwin->engine()->rootContext()->setContextProperty("screensaver", &ScreenSaver);

    model = new ListModel(new gridItem, this);

    //basicProvider = new basicImageProvider();
    //advancedProvider = new GridIndexProvider(model, 1, 4);

    qorbiterUIwin->rootContext()->setContextProperty("dataModel", model);

    qorbiterUIwin->engine()->addImageProvider("datagridimg",  new GridIndexProvider(model, 1,4 ));
    qorbiterUIwin->rootContext()->setContextProperty("currentSkinUrl" , currentSkinURL);
    qorbiterUIwin->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());

    QString qmlPath = adjustPath(QApplication::applicationDirPath().remove("/bin")) +currentSkinURL;

    qDebug () << "QML import path for build: " << qmlPath;
    qorbiterUIwin->engine()->setBaseUrl(qmlPath);
    qorbiterUIwin->setSource(QUrl::fromLocalFile(qmlPath+"main.qml"));

    QObject *item= qorbiterUIwin->rootObject();
#ifdef Q_OS_SYMBIAN
    qorbiterUIwin->showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    qorbiterUIwin->showMaximized();
#else
   qorbiterUIwin->show();
#endif
   // qorbiterUIwin->showFullScreen();
    gotoQScreen("Splash.qml");
    qDebug() << "Showing Splash";

    //initial signal and slot connection
    //QObject::connect(item,SIGNAL(swapStyle()), this,SLOT(swapSkins()));
    QObject::connect(item, SIGNAL(close()), this, SLOT(closeOrbiter()));
   // QObject::connect(this,SIGNAL(destroyed()), this, SLOT(closeOrbiter()));

     iPK_Device_DatagridPlugIn =  long(6);
     iPK_Device_OrbiterPlugin = long(9);
     m_dwIDataGridRequestCounter = 0;

     QString *q_mediaType = new QString("5");
     QString *q_publicUsers = NULL;
     QString *q_fileFormat=NULL;
     QString *q_subType=NULL;
     QString *q_attribute = NULL;
}


bool qorbiterManager::gotoQScreen(QString s)
{
    QVariant screenname= s;
    QVariant returnedValue ="null";
    QObject *item = qorbiterUIwin->rootObject();
    if (QMetaObject::invokeMethod(item, "screenchange",  Q_ARG(QVariant, screenname)))
       {
           qDebug() << returnedValue; return true;
       }
                    else
       {
            qDebug () << "Failure!" << qPrintable(s) <<": " << returnedValue.toString(); return false;
       }
}

bool qorbiterManager::setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode)
{
    pqOrbiter = new DCE::qOrbiter(PK_Device, sRouterIP, true,bLocalMode);
    iPK_Device = long(PK_Device);

    if ( pqOrbiter->GetConfig() && pqOrbiter->Connect(pqOrbiter->PK_DeviceTemplate_get()) )
    {
        qDebug() << "Device: " << PK_Device <<" Connect";
        //g_pCommand_Impl=pqOrbiter;
        //g_pDeadlockHandler=DeadlockHandler;
       // g_pSocketCrashHandler=SocketCrashHandler;
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
            /*
              we get various variable here that we will need later. I store them in the qt object so it
              can pass them along without extra issues and so they can easily be passed to qml objects that
              are templated in
             */

        pqOrbiter->CreateChildren();
            /*
              this line ties the class variable in the dceGenerated code to the qt ui code
              this is how the two threads (dce and qt) communicate with each other and make it possible to connect
              qt GUI (qml or qobject based) signals to DCE slots and vice versa!
            */
            pqOrbiter->qmlUI = this;

            /*
              here is where we setup orbiter variables that are going to be used.
             */


         if ( pqOrbiter->initialize())
            {

                if (getConf(PK_Device))
                 {
                     qDebug () << "Config Recieved, starting";
                     gotoQScreen("Screen_1.qml");
                     return true;
                 }
                else
                {
                     qDebug() << "Orbiter Conf Hiccup!";
                     LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device Failed to get configuration!");
                     return false;
                 }

          /*  i dont know what this does, but since orbter should not be local, it is commented out but not removed.
            if( bLocalMode )
                    pqOrbiter->RunLocalMode();
            else
            {
                    if(pqOrbiter->m_RequestHandlerThread)
                            pthread_join(pqOrbiter->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down

            }
            g_pDeadlockHandler=NULL;
            g_pSocketCrashHandler=NULL; */
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
                    qDebug() << "No Router, Aborting";
                    return false;
            }
            else
            {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
            qDebug() << "Connect Failed";
            }
    }

    if( pqOrbiter->m_bReload )
            bReload=true;
    delete pqOrbiter;

}

bool qorbiterManager::refreshUI()
{

   qDebug() << "Launching UI";
   return true;
}


// get conf method that reads config file
bool qorbiterManager::getConf(int pPK_Device)
{

    qDebug() << "Reading Config" ;
    iPK_Device = long(pPK_Device);
    iOrbiterPluginID = 9;
    iSize = 0;
    m_pOrbiterCat = 5;
    s_onOFF = "1";
    qDebug() << "PK_Device No:" << iPK_Device;

    QDomDocument configData;

    const QByteArray tConf = binaryConfig.data();
    configData.setContent(tConf,false);

    QDomElement root = configData.documentElement();        //orbiterXX
   // qDebug () << root.tagName();

    QDomElement defaults = root.firstChildElement("Default");
    QString sPK_User = defaults.attribute("sPK_User");


    QDomElement userXml = root.firstChildElement("Users"); //users
    //qDebug () << userXml.tagName();

    userList = new UserModel( new UserItem, this);
    QDomNodeList userXmlList = userXml.childNodes();
//-----users
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

    //---------rooms
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

        RroomMapping.insert(m_name, m_val);
        m_lRooms->appendRow(new LocationItem(m_name, m_val, m_title, m_iEA, m_iType, m_lRooms));
    }
        m_lRooms->sdefault_Ea = defaults.attribute("DefaultLocation");
        m_lRooms->idefault_Ea = RroomMapping.value(m_lRooms->sdefault_Ea);


    //-----lighting scenarios

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
    //-----media scenarios

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


    //climate---------------------

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
    //telecom---------------------

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
    //security---------------------

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


  qorbiterUIwin->rootContext()->setContextObject(this);

 // setActiveRoom(RroomMapping.value(QString::fromStdString(pqOrbiter->DATA_Get_FK_EntertainArea())), 0);
  qorbiterUIwin->rootContext()->setContextProperty("currentRoomLights", roomLights); //custom list item provided
  qorbiterUIwin->rootContext()->setContextProperty("currentRoomMedia", roomMedia);
  qorbiterUIwin->rootContext()->setContextProperty("currentRoomClimate", roomClimate);
  qorbiterUIwin->rootContext()->setContextProperty("currentRoomTelecom", roomTelecom);
   qorbiterUIwin->rootContext()->setContextProperty("currentRoomSecurity", roomSecurity);
   qorbiterUIwin->rootContext()->setContextProperty("currentuser", sPK_User);
    qorbiterUIwin->rootContext()->setContextProperty("iPK_Device", QVariant::fromValue(iPK_Device));
    pqOrbiter->CMD_Set_Current_Room(m_lRooms->idefault_Ea);
  pqOrbiter->CMD_Set_Entertainment_Area(m_lRooms->sdefault_Ea.toStdString());


  this->qorbiterUIwin->rootContext()->setContextProperty("currentroom", m_lRooms->sdefault_Ea); //custom room list item provided
  this->qorbiterUIwin->rootContext()->setContextProperty("userList", userList); //custom user list provided
  this->qorbiterUIwin->rootContext()->setContextProperty("roomList", m_lRooms); //custom room list  provided

  this->qorbiterUIwin->rootContext()->setContextProperty("gmediaType", q_mediaType);

   // binaryConfig.clear();
   //configData.clear();

   qDebug() << "Cleanup config - tconf:" << tConf.size() << "|| binaryConfig:" << binaryConfig.size() << "|| configData:" << configData.childNodes().size();

  return true;

}

//experimental skin swappping method. what should happen here is a call to qDeclarative engine to change
//source import directory, thereby changing the effective skin. This is why the base compnents need to be
//seperate from the screens them selves. the screen can and will refer to base standard object and if people
//want to create their own, the can and simple change the component import directory, all without compiling.

void qorbiterManager::swapSkins()
{
    if (currentSkin == "default")
    {



//  qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/skins/scheme/classic/Main.qml"));

    currentSkin="classic";
    }
    else
    {
       // qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/qObiter_src/main.qml"));
        currentSkin="default";
    }
    qDebug() << "Yes! Switched to: " << qPrintable(currentSkin);
}


//takes care of un-registering the orbiter from the DCERouter and then shutting down
void qorbiterManager::closeOrbiter()
{
    qDebug() << "Shutting Down";
     LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Exiting, Unregistering 1st");
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
    /*
      the questionable custom provider that kinda worked once...*/

   // qorbiterUIwin->engine()->addImageProvider(QString("datagridimg"),  new GridIndexProvider(&model, 0, 0, parent));

    //notice this commented out code as well. it 'worked' to crash things. thats about it
   //QObject::connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), gridImageHandler,SLOT(dataUpdated(QModelIndex,QModelIndex)));


    m_dwIDataGridRequestCounter++;
  //  qDebug() << " Datagrid Request Counter: " << m_dwIDataGridRequestCounter;
  //  pqOrbiter->dataGridRequest( s.toStdString());

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

void qorbiterManager::setCurrentUser()
{
}

void qorbiterManager::execGrp(int grp)
{
    model->clear();
    pqOrbiter->executeCommandGroup(grp);
}


bool qorbiterManager::addMediaItem(QString mText, QString temp, QImage cell)
{
    this->model->appendRow(new gridItem(mText, temp, cell , model));
    emit modelChanged();
   // qorbiterUIwin->rootContext()->setContextProperty("dataModel", model);
}

void qorbiterManager::updateModel()
{

}

void qorbiterManager::clearMediaModel()
{
    model->clear();

}

void qorbiterManager:: setLocation(const int &room, const int &ea)
{

    iFK_Room = room;
    iea_area = ea;

   emit locationChanged(room, ea);
}

int qorbiterManager::getlocation() const
{
    return iFK_Room;
}

void qorbiterManager::regenOrbiter(int deviceNo)
{
    qorbiterUIwin->hide();
   // qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/qObiter_src/Splash.qml"));

    qDebug() << "Regenerating..";
    QProcess *regen = new QProcess(this);
    regen->start("sudo /var/www/lmce-admin/qOrbiterGen.php?d="+QString::number(iPK_Device), QIODevice::ReadOnly);
    qDebug() <<"status code:" << regen->error();

    QObject::connect(regen,SIGNAL(finished(int)), this, SLOT(regenComplete(int)));

}

void qorbiterManager::regenComplete(int i)
{
    if (i == 0)
    {


    if (getConf(iPK_Device))
    {   qDebug() << "Regen Complete!";
       // qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/qObiter_src/main.qml"));
        gotoQScreen("Screen_1.qml");
#ifdef Q_OS_SYMBIAN
    qorbiterUIwin->showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    qorbiterUIwin->showMaximized();
#else
   qorbiterUIwin->show();
#endif
    }
}
    else
    {
        gotoQScreen("LoadError.qml");
#ifdef Q_OS_SYMBIAN
    qorbiterUIwin->showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    qorbiterUIwin->showMaximized();
#else
   qorbiterUIwin->show();
#endif
    }
}

QString qorbiterManager::adjustPath(const QString &path)
{

#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
   if (!QDir::isAbsolutePath(path))
         return QCoreApplication::applicationDirPath()
                 + QLatin1String("/../Resources/") + path;
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
    qDebug() << "Setting grid mediatype to :" << i;
    q_mediaType = "videomodel";
    this->qorbiterUIwin->rootContext()->setContextProperty("gmediaType", q_mediaType);
    emit gridTypeChanged();
}

