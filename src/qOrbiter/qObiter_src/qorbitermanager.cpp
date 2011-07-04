#include "qorbitermanager.h"
#include <QDeclarativeEngine>
#include <QDebug>

//#include "OrbiterData.h"
#include <QDeclarativeEngine>
#include <Gen_Devices/AllCommandsRequests.h>
#include <Command_Impl.h>
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "SerializeClass/SerializeClass.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

/*
  this file is responsible for the main connection between the dce thread and the qt thread.
   pqOrbiter->qmlUI = this; inside of setupLMCE initializes the dce thread, while other functions are
   responsible for signals and slots between dce and qml.
  */

using namespace DCE;

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
//<-dceag-plug-e->

qorbiterManager::qorbiterManager(QWidget *parent) :
    QWidget(parent)
{
    currentSkin = "default";
    currentSkinURL="qml/qObiter_src/main.qml";
    s_RouterIP="dcerouter";

    qorbiterUIwin = new QDeclarativeView;
    qorbiterUIwin->engine()->addImageProvider("datagridimg", new basicImageProvider);
    qorbiterUIwin->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());

    qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/main.qml"));
    qorbiterUIwin->engine()->addImportPath("qml/components");
    QObject *item= qorbiterUIwin->rootObject();
    qorbiterUIwin->showFullScreen();
    gotoQScreen("Splash.qml");
    qDebug() << "Showing Splash";



    //initial signal and slot connection
    //QObject::connect(item,SIGNAL(swapStyle()), this,SLOT(swapSkins()));
    QObject::connect(item, SIGNAL(close()), this, SLOT(closeOrbiter()));
   // QObject::connect(this,SIGNAL(destroyed()), this, SLOT(closeOrbiter()));

     iPK_Device_DatagridPlugIn =  long(6);
     m_dwIDataGridRequestCounter = 0;


}


bool qorbiterManager::gotoQScreen(QString s)
{
    QVariant screenname= s;
    QVariant returnedValue ="null";
  QObject *item = qorbiterUIwin->rootObject();
    if (QMetaObject::invokeMethod(item, "screenchange",  Q_ARG(QVariant, screenname)))
       { qDebug() << returnedValue; return true;}
                    else
        {qDebug () << "Failure!" << qPrintable(s) <<": " << returnedValue; return false;}

}

bool qorbiterManager::setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode)
{
    pqOrbiter = new DCE::qOrbiter(PK_Device, sRouterIP, true,bLocalMode);
    iPK_Device = long(PK_Device);

    if ( pqOrbiter->GetConfig() && pqOrbiter->Connect(pqOrbiter->PK_DeviceTemplate_get()) )
    {



            qDebug() << "Device Connect";
            g_pCommand_Impl=pqOrbiter;
            g_pDeadlockHandler=DeadlockHandler;
            g_pSocketCrashHandler=SocketCrashHandler;
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
            if ( OrbiterGen() )
            {
                qDebug () << "Config Recieved, starting";
                pqOrbiter->initialize();
                if (getConf(PK_Device))
                 {

                     gotoQScreen("Screen_1.qml");
                 return true;} else {


                      qDebug() << "Orbiter Conf Hiccup!";

                         LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device Failed to get configuration!");

                        return false;
                     }
                 }
                else
                 {
                     qDebug() << "Orbiter Could Not setup!" ;
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
                    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
            qDebug() << "Connect Failed";
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

/* This function serves to create the data needed to run orbiter. Its currently set to run automatically
  when it connect, but that will change
  */
bool qorbiterManager::OrbiterGen()
{

//setup the orbiter blob if the data has changed or by manual selection
    qOrbiterGenerator *orbiterConf = new qOrbiterGenerator(iPK_Device, "dcerouter","root", "", "pluto_main", 3306, this);


    if (orbiterConf->initializeRegen())
    {
        if (!orbiterConf->get_virtual_devices())
        {
            qDebug() << "Couldnt Get virtutal Devices!";
        }
 /*
   this block is responsible for setting up the list of rooms in the home and setting the initial location
   */
     QString fk_ea = QString::fromStdString(pqOrbiter->DATA_Get_FK_EntertainArea());
     m_lRooms = new LocationModel(new LocationItem, this);
     m_lRooms  = orbiterConf->get_locations(fk_ea);

       this->qorbiterUIwin->rootContext()->setContextProperty("roomlist", m_lRooms); //custom room list item provided
     this->qorbiterUIwin->rootContext()->setContextProperty("currentroom", m_lRooms->sdefault_Ea); //custom room list item provided

       /*
         this block sets up users and orbiter authorized users
         */

       UserModel *userList = new UserModel( new UserItem, this);
       userList = orbiterConf->get_users();
       userList->findDefault();

       QString sPK_User = userList->defaultUzer;

       this->qorbiterUIwin->rootContext()->setContextProperty("userList", userList); //custom list item provided
      qorbiterUIwin->rootContext()->setContextProperty("currentuser", sPK_User);

      // qDebug()<< userList->defaultUser;

       /*
         Lighting Scenarios
         */
      roomLightingScenarios = orbiterConf->get_lighting_scenarios();

      LightingScenarioModel *roomLights = new LightingScenarioModel(new LightingScenarioItem, this);

      roomLights = roomLightingScenarios.value(m_lRooms->rdefault_Ea);

     qorbiterUIwin->rootContext()->setContextProperty("currentRoomLights", roomLights); //custom list item provided

    }


 return true;
}

//temporary get conf method until qorbitergen does its purpose
bool qorbiterManager::getConf(int pPK_Device)
{
    qDebug() << "Getting Configuration" ;
    iPK_Device = long(pPK_Device);
    iOrbiterPluginID = 9;
    iSize = 0;
    m_pOrbiterCat = 5;
   s_onOFF = "1";
   qDebug() << "PK_Device No:" << iPK_Device;


  qorbiterUIwin->rootContext()->setContextObject(this);
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
bool qorbiterManager::requestDataGrid(QString s, QString sType)
{
   gridReqType = &sType;
    model = new ListModel(new gridItem, this);
    qorbiterUIwin->rootContext()->setContextProperty("dataModel", model);
    qDebug() << qPrintable(sType);

    /*
      the questionable custom provider that kinda worked once...*/

   // qorbiterUIwin->engine()->addImageProvider(QString("datagridimg"),  new GridIndexProvider(&model, 0, 0, parent));

    //notice this commented out code as well. it 'worked' to crash things. thats about it
   //QObject::connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), gridImageHandler,SLOT(dataUpdated(QModelIndex,QModelIndex)));

    qDebug() << qPrintable(s);
    m_dwIDataGridRequestCounter++;
    qDebug() << " Datagrid Request Counter: " << m_dwIDataGridRequestCounter;
    pqOrbiter->dataGridRequest( s.toStdString());

}

void qorbiterManager::setActiveRoom(int room,int ea)
{
    roomLights = roomLightingScenarios.value(room);
    qorbiterUIwin->rootContext()->setContextProperty("currentRoomLights", roomLights);
    qDebug() << "lights CHanged to room" << room;
}

void qorbiterManager::setCurrentUser()
{
}


