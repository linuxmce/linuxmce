/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     This main.cpp has be heavily modified from the DCE Generated main.cpp for Qt integration.

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
#include <contextobjects/epgchannellist.h>
#include <contextobjects/playlistclass.h>
#include <QtTest/QSignalSpy>

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
#ifdef for_harmattan
    QApplication::setGraphicsSystem("meego");
#elif for_desktop
    QApplication::setGraphicsSystem("opengl");
#elif WIN32

#elif for_android
    QApplication::setGraphicsSystem("raster");
#else
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

        QThread *epgThread = new QThread; //for playlists and epg of all types. only one will be active a given time inthe app
        //stored video playlist for managing any media that isnt live broacast essentially
        PlaylistClass *storedVideoPlaylist = new PlaylistClass (new PlaylistItemClass);
        storedVideoPlaylist->moveToThread(epgThread);

        //epg listmodel, no imageprovider as of yet
        EPGChannelList *simpleEPGmodel = new EPGChannelList(new EPGItemClass);
        simpleEPGmodel->moveToThread(epgThread);

        QThread * mediaThread = new QThread();
        ListModel *mediaModel = new ListModel(new gridItem);
        mediaModel->moveToThread(mediaThread);
        GridIndexProvider * advancedProvider = new GridIndexProvider(mediaModel , 6, 4);
        orbiterWin.mainView.engine()->addImageProvider("datagridimg", advancedProvider);
        advancedProvider->moveToThread(mediaThread);

        orbiterWin.mainView.rootContext()->setContextProperty("dcerouter", pqOrbiter); //dcecontext object
        orbiterWin.mainView.rootContext()->setContextProperty("dataModel", mediaModel);
        orbiterWin.mainView.rootContext()->setContextProperty("mediaplaylist", storedVideoPlaylist);
        orbiterWin.mainView.rootContext()->setContextProperty("simpleepg", simpleEPGmodel);
        //shutdown signals

        QObject::connect(w, SIGNAL(orbiterClosing()), dceThread, SLOT(quit()));
        QObject::connect(w, SIGNAL(orbiterClosing()), mediaThread, SLOT(quit()));
        QObject::connect(w, SIGNAL(orbiterClosing()), epgThread, SLOT(quit()));
        QObject::connect(dceThread, SIGNAL(finished()), &a, SLOT(quit()));

        //tv epg signals
        QObject::connect(pqOrbiter, SIGNAL(addChannel(EPGItemClass*)), simpleEPGmodel, SLOT(appendRow(EPGItemClass*)), Qt::QueuedConnection );

        //filedetails
        QObject::connect(pqOrbiter,SIGNAL(fd_titleImageChanged(QImage)), w->filedetailsclass, SLOT(setTitleImage(QImage)));
        QObject::connect(pqOrbiter, SIGNAL(fd_mediaTitleChanged(QString)), w->filedetailsclass, SLOT(setMediaTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_directorChanged(QString)), w->filedetailsclass, SLOT(setDirector(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_programChanged(QString)), w->filedetailsclass, SLOT(setProgram(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_synopChanged(QString)), w->filedetailsclass, SLOT(setSynop(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_albumChanged(QString)), w->filedetailsclass, SLOT(setAlbum(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_performersChanged(QString)), w->filedetailsclass, SLOT(setPerformers(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_genreChanged(QString)), w->filedetailsclass, SLOT(setGenre(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_channelChanged(QString)), w->filedetailsclass, SLOT(setChannel(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_pathChanged(QString)), w->filedetailsclass, SLOT(setPath(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_fileChanged(QString)), w->filedetailsclass, SLOT(setFile(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_episodeChanged(QString)), w->filedetailsclass, SLOT(setEpisode(QString)));
        QObject::connect(pqOrbiter, SIGNAL(fd_trackChanged(QString)), w->nowPlayingButton, SLOT(setTrack(QString)));

        //stored media signal
        QObject::connect(pqOrbiter,SIGNAL(playlistItemAdded(PlaylistItemClass*)), storedVideoPlaylist,SLOT(appendRow(PlaylistItemClass*)), Qt::QueuedConnection);

        QObject::connect(pqOrbiter,SIGNAL(resetNowPlaying()), w->nowPlayingButton, SLOT(resetData()));
        QObject::connect(pqOrbiter, SIGNAL(setPlaylistPosition(int)), w->nowPlayingButton, SLOT(setPlaylistPostion(int)));
        QObject::connect(pqOrbiter,SIGNAL(updateMediaSpeed(int)), w->nowPlayingButton, SLOT(setMediaSpeed(int)));
        QObject::connect(pqOrbiter,SIGNAL(mediaLengthChanged(QString)), w->nowPlayingButton, SLOT(setDuration(QString)));
        QObject::connect(pqOrbiter,SIGNAL(tcUpdated(QString)), w->nowPlayingButton, SLOT(setTimeCode(QString)));
        QObject::connect(w->nowPlayingButton, SIGNAL(playListPositionChanged(int)), storedVideoPlaylist, SLOT(setCurrentIndex(int)) );


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
        QObject::connect(pqOrbiter, SIGNAL(screenSaverImages(QStringList)), w->ScreenSaver, SLOT(setImageList(QStringList)));
        QObject::connect(pqOrbiter, SIGNAL(setMyIp(QString)), w, SLOT(setIpAddress(QString)));

        //messaging
        QObject::connect(mediaModel, SIGNAL(statusMessage(QString)), w, SLOT(setDceResponse(QString)),Qt::DirectConnection);
        QObject::connect(pqOrbiter, SIGNAL(statusMessage(QString)), w , SLOT(setDceResponse(QString)), Qt::DirectConnection);
        QObject::connect(w, SIGNAL(loadingMessage(QString)), &orbiterWin,SLOT(setMessage(QString)), Qt::DirectConnection);

        //operations
        QObject::connect(pqOrbiter, SIGNAL(newTCport(int)), w, SLOT(updateTimecode(int)));
        QObject::connect(pqOrbiter, SIGNAL(addScreenParam(QString,int)), w->ScreenParameters, SLOT(addParam(QString, int)));
        QObject::connect(w, SIGNAL(locationChanged(int,int)), pqOrbiter, SLOT(setLocation(int,int)));
        QObject::connect(w, SIGNAL(userChanged(int)), pqOrbiter, SLOT(setUser(int)));
        QObject::connect(w, SIGNAL(bindMediaRemote(bool)), pqOrbiter, SLOT(BindMediaRemote(bool)), Qt::DirectConnection);
        QObject::connect(w, SIGNAL(startPlayback(QString)), pqOrbiter, SLOT(playMedia(QString)));

        // QObject::connect(w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()));

        QObject::connect(w->nowPlayingButton, SIGNAL(newMediaSpeed(int)), pqOrbiter,SLOT(setMediaSpeed(int)));

        //navigation
        QObject::connect(pqOrbiter,SIGNAL(gotoQml(QString)), w, SLOT(gotoQScreen(QString)));

        //mediagrid
        QObject::connect(mediaModel, SIGNAL(pagingCleared()), pqOrbiter,SLOT(populateAdditionalMedia()), Qt::QueuedConnection);
        QObject::connect(pqOrbiter, SIGNAL(clearPageGrid()), mediaModel, SLOT(clearForPaging()), Qt::DirectConnection);
        QObject::connect(mediaModel, SIGNAL(itemAdded(int)), pqOrbiter, SLOT(setCurrentRow(int)));
        QObject::connect(w, SIGNAL(clearModel()), mediaModel,SLOT(clear()));
        QObject::connect(pqOrbiter, SIGNAL(clearAndContinue(int)), mediaModel, SLOT(clearAndRequest(int)), Qt::QueuedConnection);
        QObject::connect(pqOrbiter,SIGNAL(addItem(gridItem*)), mediaModel, SLOT(appendRow(gridItem*)));
        QObject::connect(pqOrbiter,SIGNAL(gridModelSizeChange(int)), mediaModel, SLOT(setTotalCells(int)), Qt::QueuedConnection);


        QObject::connect(mediaModel,SIGNAL(ready(int)), pqOrbiter, SLOT(prepareFileList(int)), Qt::QueuedConnection);
        QObject::connect(w, SIGNAL(gridTypeChanged(int)), mediaModel, SLOT(setGridType(int)), Qt::QueuedConnection);
        QObject::connect(w, SIGNAL(setDceGridParam(int,QString)), pqOrbiter, SLOT(setStringParam(int,QString)), Qt::DirectConnection);
        QObject::connect(w, SIGNAL(keepLoading(bool)), pqOrbiter,SLOT(setGridStatus(bool)));
        QObject::connect(pqOrbiter, SIGNAL(showFileInfo(bool)), w->filedetailsclass, SLOT(setVisible(bool)));
        QObject::connect(pqOrbiter, SIGNAL(setFocusFile(QString)), w->filedetailsclass, SLOT(setFile(QString)));
        QObject::connect(pqOrbiter, SIGNAL(modelPageCount(QList<QObject*>)), w, SLOT(displayModelPages(QList<QObject*>)));

        //now playing signals
        QObject::connect(pqOrbiter, SIGNAL(setNowPlaying(bool)), w->nowPlayingButton,SLOT(setStatus(bool)));
        QObject::connect(pqOrbiter,SIGNAL(streamIdChanged(int)), w->nowPlayingButton, SLOT(setStreamID(int)));
        QObject::connect(pqOrbiter, SIGNAL(currentScreenChanged(QString)), w->nowPlayingButton, SLOT(setScreen(QString)));
        QObject::connect(pqOrbiter,SIGNAL(objectUpdate(const uchar*,int)), w->nowPlayingButton, SLOT(setImageData(const uchar*,int)),Qt::DirectConnection);
        QObject::connect(simpleEPGmodel, SIGNAL(channelNumberChanged(QString)), w->nowPlayingButton, SLOT(setChannel(QString)), Qt::QueuedConnection);
        QObject::connect(simpleEPGmodel, SIGNAL(programChanged(QString)), w->nowPlayingButton, SLOT(setProgram(QString)), Qt::QueuedConnection);
        QObject::connect(simpleEPGmodel, SIGNAL(networkChanged(QString)), w->nowPlayingButton, SLOT(setChannelID(QString)), Qt::QueuedConnection);

        //attributes
        QObject::connect(pqOrbiter, SIGNAL(np_filename(QString)), w->nowPlayingButton, SLOT(setFilePath(QString)));

        QObject::connect(pqOrbiter, SIGNAL(np_mediaTitleChanged(QString)), w->nowPlayingButton, SLOT(setMediaTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_album(QString)), w->nowPlayingButton, SLOT(setAlbum(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_track(QString)), w->nowPlayingButton, SLOT(setTrack(QString)));
        QObject::connect(pqOrbiter,SIGNAL(np_channel(QString)), w->nowPlayingButton, SLOT(setChannel(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_network(QString)), w->nowPlayingButton, SLOT(setNetwork(QString)) );
        QObject::connect(pqOrbiter, SIGNAL(np_channelID(QString)), w->nowPlayingButton, SLOT(setChannelID(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_program(QString)), w->nowPlayingButton, SLOT(setProgram(QString)));

        QObject::connect(pqOrbiter, SIGNAL(np_director(QString)), w->nowPlayingButton, SLOT(setDirector(QString)));
        QObject::connect(pqOrbiter,SIGNAL(np_episode(QString)), w->nowPlayingButton, SLOT(setEpisode(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_filename(QString)), w->nowPlayingButton, SLOT(setFilePath(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_genre(QString)), w->nowPlayingButton, SLOT(setGenre(QString)));

        QObject::connect(pqOrbiter, SIGNAL(np_performer(QString)), w->nowPlayingButton, SLOT(setPerformers(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_program(QString)), w->nowPlayingButton, SLOT(setProgram(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_releaseDate(QString)), w->nowPlayingButton, SLOT(setRelease(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_title1Changed(QString)), w->nowPlayingButton, SLOT(setTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_title2Changed(QString)), w->nowPlayingButton, SLOT(setTitle2(QString)));
        QObject::connect(pqOrbiter, SIGNAL(subtitleChanged(QString)), w->nowPlayingButton, SLOT(setSubTitle(QString)));
        QObject::connect(pqOrbiter, SIGNAL(np_synopsisChanged(QString)), w->nowPlayingButton, SLOT(setSynop(QString)));
        QObject::connect(pqOrbiter,SIGNAL(playlistPositionChanged(int)), w->nowPlayingButton, SLOT(setPlaylistPostion(int)));

        QObject::connect(pqOrbiter, SIGNAL(screenshotVariablesReady(QList<QObject*>)), w, SLOT(setScreenShotVariables(QList<QObject*>)));
        QObject::connect(pqOrbiter, SIGNAL(screenShotReady(QByteArray)), w, SLOT(setMediaScreenShot(QByteArray)));
        QObject::connect(w, SIGNAL(saveMediaScreenShot(QString,QByteArray)), pqOrbiter, SLOT(saveScreenAttribute(QString,QByteArray)));
        // myth  now playing requires special handling
        QObject::connect(pqOrbiter, SIGNAL(mythTvUpdate(QString)), simpleEPGmodel, SLOT(setMythProgram(QString)),Qt::QueuedConnection);
        QObject::connect(pqOrbiter, SIGNAL(epgDone()), simpleEPGmodel, SLOT(updatePosition()),Qt::QueuedConnection);
        QObject::connect(pqOrbiter, SIGNAL(livetvDone()), simpleEPGmodel, SLOT(updateLivePosition()), Qt::QueuedConnection);

        QObject::connect(simpleEPGmodel, SIGNAL(requestEpg()), pqOrbiter, SLOT(requestLiveTvPlaylist()), Qt::QueuedConnection);
        QObject::connect(pqOrbiter, SIGNAL(clearTVplaylist()), simpleEPGmodel, SLOT(populate()), Qt::QueuedConnection);
        QObject::connect(w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()),Qt::DirectConnection);
        QObject::connect(w, SIGNAL(managerPlaylistRequest()), storedVideoPlaylist,SLOT(populate()) );

        //controls
        QObject::connect(pqOrbiter, SIGNAL(resendAvButtonList(QList<QObject*>)), w, SLOT(showDeviceCodes(QList<QObject*>)), Qt::QueuedConnection);

        //so does live tv
        QObject::connect(w, SIGNAL(clearModel()), simpleEPGmodel, SLOT(empty()));

        QObject::connect(pqOrbiter, SIGNAL(liveTvUpdate(QString)), simpleEPGmodel, SLOT(setProgram(QString)), Qt::QueuedConnection);
        //epg specific

        //storemediaplaylist specific
        QObject::connect(storedVideoPlaylist,SIGNAL(playlistReady()), pqOrbiter,SLOT(requestMediaPlaylist()));
        QObject::connect(pqOrbiter, SIGNAL(clearPlaylist()), storedVideoPlaylist,  SLOT(populate()));
        QObject::connect(pqOrbiter, SIGNAL(playlistDone()), storedVideoPlaylist, SIGNAL(activeItemChanged()));

        QObject::connect(pqOrbiter,SIGNAL(routerReloading(QString)), w, SLOT(reloadHandler()) );
        QObject::connect(pqOrbiter, SIGNAL(routerDisconnect()), w, SLOT(reloadHandler()));
        QObject::connect(pqOrbiter, SIGNAL(closeOrbiter()), w, SLOT(closeOrbiter()));

        dceThread->start();
        mediaThread->start();
        epgThread->start();

        pqOrbiter->m_dwPK_Device = w->iPK_Device;
        pqOrbiter->m_sHostName = w->qs_routerip.toStdString();

        if ( pqOrbiter->initialize() == true )
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
        a.exec();

        if( pqOrbiter->m_bReload )
            bReload=true;
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
