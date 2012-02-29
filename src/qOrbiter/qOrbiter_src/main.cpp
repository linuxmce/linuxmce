/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-incl-b->
#ifdef IOS
#import <UIKit/UIKit.h>

#include "ioshelpers.h"
#include <QtGui/QApplication>
#include <QtCore/QtPlugin>
#include <QtDeclarative/QDeclarativeEngine>


Q_IMPORT_PLUGIN(UIKit)
#endif

#include <QApplication>
#include <datamodels/listModel.h>
#include <datamodels/gridItem.h>
#include <qOrbiter/qOrbiter.h>
#include <orbiterwindow.h>
#include <qorbitermanager.h>
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include <QDeclarativeEngine>
#include <imageProviders/abstractimageprovider.h>


// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/

using namespace DCE;


// You can override this block if you don't want the app to reload in the event of a problem
/*
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


//<-dceag-main-b->

*/
int main(int argc, char* argv[])
{
#ifndef for_harmattan
    QApplication::setGraphicsSystem("raster");
#endif
    QApplication  a(argc, argv);

    g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
    g_sBinaryPath = FileUtils::BasePath(argv[0]);

    cout << "qOrbiter, v." << VERSION << endl
         << "Visit www.plutohome.com for source code and license information" << endl << endl;

    string sRouter_IP="DCEROUTER";
    int PK_Device;
    string sLogger="stdout";

    bool bLocalMode=false,bError=false; // An error parsing the command line
    char c;
    for(int optnum=1;optnum<argc;++optnum)
    {
        if( argv[optnum][0]!='-' )
        {
            cerr << "Unknown option " << argv[optnum] << endl;
            bError=true;
        }

        c=argv[optnum][1];
        switch (c)
        {
        case 'r':
            sRouter_IP = argv[++optnum];
            break;
        case 'd':
            PK_Device = atoi(argv[++optnum]);
            //qDebug() << "From the command line input:" << PK_Device;
            break;
        case 'L':
            bLocalMode = true;
            break;
        case 'l':
            sLogger = argv[++optnum];
            break;
        default:
            bError=true;
            break;
        };
    }

    if (bError)
    {
        cout << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
             << "Usage: qOrbiter [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
             << "-r -- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
             << "-d -- This device's ID number.  If not specified, it will be requested from the router based on our IP address." << endl
             << "-l -- Where to save the log files.  Specify 'dcerouter' to have the messages logged to the DCE Router.  Defaults to stdout." << endl;
#ifndef ANDROID
        exit(1);
#endif
    }

#ifdef WIN32
    WORD    wVersion;
    WSADATA wsaData;
    wVersion = MAKEWORD( 1, 1 );
    if (WSAStartup(wVersion, &wsaData)!=0)
    {
        int ec = WSAGetLastError();
        char s[91];
        sprintf(s, "WSAStartup err %d", ec);
        cerr << s << endl;
        exit(1);
    }
#endif

    try
    {
        if( sLogger=="dcerouter" )
            LoggerWrapper::SetInstance(new ServerLogger(PK_Device, qOrbiter::PK_DeviceTemplate_get_static(), sRouter_IP));
        else if( sLogger=="null" )
            LoggerWrapper::SetType(LT_LOGGER_NULL);
        else if( sLogger!="stdout" )
            LoggerWrapper::SetType(LT_LOGGER_FILE,sLogger);

    }
    catch(...)
    {
        cerr << "Unable to create logger" << endl;
    }


    bool bAppError=false;
    bool bReload=false;

    try
    {
        /*
          move main pqOrbiter initialization here.
          */
#ifdef IOS
        //        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif
        orbiterWindow orbiterWin(PK_Device, sRouter_IP);
        orbiterWin.setMessage("Setting up Lmce");
        qorbiterManager  *w= new qorbiterManager(&orbiterWin.mainView);
        AbstractImageProvider *modelimageprovider = new AbstractImageProvider(w);
        orbiterWin.mainView.engine()->addImageProvider("listprovider", modelimageprovider);

        QThread *dceThread = new QThread;
        qOrbiter *pqOrbiter = new qOrbiter(PK_Device, sRouter_IP,true,bLocalMode);
        pqOrbiter->moveToThread(dceThread);


        QThread * mediaThread = new QThread();
        ListModel *mediaModel = new ListModel(new gridItem);
        mediaModel->moveToThread(mediaThread);
        GridIndexProvider * advancedProvider = new GridIndexProvider(mediaModel , 6, 4);
       //QObject::connect(mediaModel,SIGNAL(dataChanged(QModelIndex,QModelIndex, int )), advancedProvider,SLOT(dataUpdated(QModelIndex,QModelIndex, int)), Qt::DirectConnection);
        //QObject::connect(mediaModel, SIGNAL(modelAboutToBeReset()), advancedProvider, SLOT(dataReset()), Qt::DirectConnection);

        orbiterWin.mainView.engine()->addImageProvider("datagridimg", advancedProvider);
       advancedProvider->moveToThread(mediaThread);

        orbiterWin.mainView.rootContext()->setContextProperty("dcerouter", pqOrbiter); //dcecontext object
        orbiterWin.mainView.rootContext()->setContextProperty("dataModel", mediaModel);

        //setup
        QObject::connect(w, SIGNAL(registerOrbiter(int,QString,int)), pqOrbiter,SLOT(registerDevice(int,QString,int)));
        QObject::connect(pqOrbiter,SIGNAL(startManager(QString,QString)), w, SLOT(qmlSetupLmce(QString,QString)));
        QObject::connect(pqOrbiter, SIGNAL(deviceInvalid(QList<QObject*>)), &orbiterWin,SLOT(prepareExistingOrbiters(QList<QObject *>)));
        QObject::connect(pqOrbiter,SIGNAL(routerInvalid()), &orbiterWin, SIGNAL(showExternal()));
        QObject::connect(&orbiterWin,SIGNAL(setupLmce(QString,QString)), pqOrbiter, SLOT(qmlSetup(QString,QString)),Qt::DirectConnection);
        QObject::connect(w,SIGNAL(connectionValid(bool)), &orbiterWin, SLOT(setConnectionState(bool)));
        QObject::connect(w,SIGNAL(deviceValid(bool)), &orbiterWin, SLOT(setDeviceState(bool)));
        QObject::connect(w,SIGNAL(localConfigReady(bool)), &orbiterWin, SLOT(setLocalConfigState(bool)));
        QObject::connect(w, SIGNAL(skinDataLoaded(bool)), &orbiterWin, SLOT(setSkinDataState(bool)));
        QObject::connect(w,SIGNAL(skinIndexReady(bool)), &orbiterWin,SLOT(setSkinIndexState(bool)));
        QObject::connect(w,SIGNAL(orbiterConfigReady(bool)), &orbiterWin, SLOT(setOrbiterConfigState(bool)));
        QObject::connect(pqOrbiter, SIGNAL(configReady(QByteArray)), w, SLOT(processConfig(QByteArray)));
        QObject::connect(w, SIGNAL(raiseSplash()), &orbiterWin, SLOT(showSplash()) );
        QObject::connect(w,SIGNAL(showSetup()), &orbiterWin, SLOT( showSetup()) );

        //messaging
        QObject::connect(mediaModel, SIGNAL(statusMessage(QString)), w, SLOT(setDceResponse(QString)),Qt::DirectConnection);
        QObject::connect(pqOrbiter, SIGNAL(statusMessage(QString)), w , SLOT(setDceResponse(QString)), Qt::DirectConnection);
        QObject::connect(w, SIGNAL(loadingMessage(QString)), &orbiterWin,SLOT(setMessage(QString)), Qt::DirectConnection);

        //operations
        QObject::connect(pqOrbiter,SIGNAL(objectUpdate(const uchar*,int)), w->nowPlayingButton, SLOT(setImageData(const uchar*,int)),Qt::DirectConnection);
        QObject::connect(pqOrbiter, SIGNAL(addScreenParam(QString,int)), w->ScreenParameters, SLOT(addParam(QString, int)));
        QObject::connect(pqOrbiter, SIGNAL(currentScreenChanged(QString)), w->nowPlayingButton, SLOT(setScreen(QString)));
        QObject::connect(w, SIGNAL(startPlayback(QString)), pqOrbiter, SLOT(checkLoadingStatus()));

        //navigation
        QObject::connect(pqOrbiter,SIGNAL(gotoQml(QString)), w, SLOT(gotoQScreen(QString)));

        //mediagrid
        QObject::connect(mediaModel, SIGNAL(itemAdded(int)), pqOrbiter, SLOT(setCurrentRow(int)), Qt::DirectConnection);
        QObject::connect(w, SIGNAL(clearModel()), mediaModel,SLOT(clear()));
        QObject::connect(pqOrbiter, SIGNAL(clearAndContinue(int)), mediaModel, SLOT(clearAndRequest(int)));
        QObject::connect(pqOrbiter, SIGNAL(clearGrid()), mediaModel, SLOT(clear()));
        QObject::connect(pqOrbiter,SIGNAL(addItem(gridItem*)), mediaModel, SLOT(appendRow(gridItem*)));
        QObject::connect(pqOrbiter,SIGNAL(gridModelSizeChange(int)), mediaModel, SLOT(setTotalCells(int)),Qt::QueuedConnection);
        // QObject::connect(pqOrbiter,SIGNAL(checkGridStatus()), mediaModel, SLOT(checkForMore()), Qt::QueuedConnection);
        // QObject::connect(mediaModel,SIGNAL(loadingStatusChanged(bool)), w, SLOT(setRequestMore(bool)), Qt::DirectConnection);
        // QObject::connect(mediaModel, SIGNAL(gimmieData(int)), pqOrbiter, SLOT(populateAdditionalMedia()), Qt::QueuedConnection);
        QObject::connect(mediaModel,SIGNAL(ready(int)), pqOrbiter, SLOT(prepareFileList(int)), Qt::DirectConnection);
        QObject::connect(w, SIGNAL(gridTypeChanged(int)), mediaModel, SLOT(setGridType(int)), Qt::QueuedConnection);
        QObject::connect(w, SIGNAL(setDceGridParam(int,QString)), pqOrbiter, SLOT(setStringParam(int,QString)));
        QObject::connect(w, SIGNAL(keepLoading(bool)), pqOrbiter,SLOT(setGridStatus(bool)), Qt::DirectConnection);
        QObject::connect(pqOrbiter, SIGNAL(showFileInfo(bool)), w->filedetailsclass, SLOT(setVisible(bool)));
        QObject::connect(pqOrbiter, SIGNAL(setFocusFile(QString)), w->filedetailsclass, SLOT(setFile(QString)));

        //now playing signals
        QObject::connect(pqOrbiter, SIGNAL(setNowPlaying(bool)), w->nowPlayingButton,SLOT(setStatus(bool)));
        QObject::connect(pqOrbiter,SIGNAL(streamIdChanged(int)), w->nowPlayingButton, SLOT(setStreamID(int)));

        //attributes
        QObject::connect(pqOrbiter, SIGNAL(np_mediaTitleChanged(QString)), w->nowPlayingButton, SLOT(setMediaTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_album(QString)), w->nowPlayingButton, SLOT(setAlbum(QString)));
        QObject::connect(pqOrbiter,SIGNAL(np_channel(QString)), w->nowPlayingButton, SLOT(setChannel(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_director(QString)), w->nowPlayingButton, SLOT(setDirector(QString)));
        QObject::connect(pqOrbiter,SIGNAL(np_episode(QString)), w->nowPlayingButton, SLOT(setEpisode(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_filename(QString)), w->nowPlayingButton, SLOT(setFilePath(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_genre(QString)), w->nowPlayingButton, SLOT(setGenre(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_imagePath(QString)), w->nowPlayingButton, SLOT(setImageUrl(QUrl)));
        QObject::connect(pqOrbiter, SIGNAL(np_performer(QString)), w->nowPlayingButton, SLOT(setPerformers(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_program(QString)), w->nowPlayingButton, SLOT(setProgram(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_releaseDate(QString)), w->nowPlayingButton, SLOT(setRelease(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_title1Changed(QString)), w->nowPlayingButton, SLOT(setTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_title2Changed(QString)), w->nowPlayingButton, SLOT(setTitle2(QString)));
        QObject::connect(pqOrbiter, SIGNAL(subtitleChanged(QString)), w->nowPlayingButton, SLOT(setSubTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_synopsisChanged(QString)), w->nowPlayingButton, SLOT(setSynop(QString)));

        mediaThread->start();
        dceThread->start();

        pqOrbiter->m_dwPK_Device = w->iPK_Device;
        pqOrbiter->m_sHostName = w->qs_routerip.toStdString();

        if ( pqOrbiter->initialize() )
        {

            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");


            pqOrbiter->CreateChildren();
            if( bLocalMode )
                pqOrbiter->RunLocalMode();
            else
            {
                /*
                if(pqOrbiter->m_RequestHandlerThread)
                    pthread_join(pqOrbiter->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
                */
            }

        }
        else
        {


        }

        if( pqOrbiter->m_bReload )
            bReload=true;
        a.exec();
        delete pqOrbiter;

    }

    catch(string s)
    {
        cerr << "Exception: " << s << endl;
    }
    catch(const char *s)
    {
        cerr << "Exception: " << s << endl;
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device: %d ending",PK_Device);
#ifdef WIN32
    WSACleanup();
#endif
#ifdef IOS
    //    [pool release];
#endif

    if( bAppError )
        return 1;

    if( bReload )
        return 2;
    else
        return 0;

}
//<-dceag-main-e->
