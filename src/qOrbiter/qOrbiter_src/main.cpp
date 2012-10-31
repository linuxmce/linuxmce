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

#include <QtGlobal>
#ifdef QT5
#include <QtWidgets/QApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtCore/QThread>


#else
#include <QApplication>
#include <QtDeclarative/QDeclarativeEngine>
#endif

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
#include <imageProviders/abstractimageprovider.h>
#include <contextobjects/epgchannellist.h>
#include <contextobjects/playlistclass.h>
#include <contextobjects/timecodemanager.h>

#ifdef debug
#include <QDebug>
#endif

//--end includes to be made plugins

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

                qOrbiter *&pqOrbiter = new qOrbiter(PK_Device, "localhost",true,false,pRouter);
                if( &pqOrbiter->m_bQuit_get()|| !&pqOrbiter->GetConfig() )
                {
                        delete &pqOrbiter;
                        return NULL;
                }
                else
                {
                        g_pCommand_Impl=&pqOrbiter;
                        g_pDeadlockHandler=Plugin_DeadlockHandler;
                        g_pSocketCrashHandler=Plugin_SocketCrashHandler;
                }
                return &pqOrbiter;
        }
}
//<-dceag-plug-e->


//<-dceag-main-b->

*/
int main(int argc, char* argv[])
{
#ifndef QT5
#ifdef for_harmattan
    QApplication::setGraphicsSystem("meego");
#elif GLENABLED
    QApplication::setGraphicsSystem("opengl");
#else
    QApplication::setGraphicsSystem("raster");
#endif

#endif
#if(QT_VERSION >= 0x040800)
    Qt::AA_X11InitThreads;
#endif

#ifdef QT5
    QGuiApplication  a(argc, argv);
#else
    QApplication  a(argc, argv);
#endif

    QCoreApplication::setApplicationName("LinuxMCE QOrbiter");

#ifdef __ANDROID__ // workaround for 'text as boxes' issue.
    QFont f = a.font();
    f.setFamily("Droid Sans");
    f.setBold("true");
    a.setFont(f);
#endif

    g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
    g_sBinaryPath = FileUtils::BasePath(argv[0]);
    cout << "qOrbiter, v." << VERSION << endl
         << "Visit www.linuxmce.org for source code and license information" << endl << endl;
    string sRouter_IP="192.168.80.1";
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
#ifdef debug
            qDebug() << "From the command line input:" << PK_Device;
#endif
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
#ifndef __ANDROID__
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
#ifdef IOS
        //        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif

#if GLENABLED
        bool glpresent= true;
#else
        bool glpresent = false;
#endif

        QThread dceThread;
        qOrbiter pqOrbiter(PK_Device, sRouter_IP,true,bLocalMode );
        orbiterWindow orbiterWin(-1, sRouter_IP);
        pqOrbiter.moveToThread(&dceThread);
        QObject::connect(&dceThread, SIGNAL(started()), &pqOrbiter, SLOT(beginSetup()));

        // orbiterWin.mainView.rootContext()->setContextProperty("dcerouter", &pqOrbiter); //dcecontext object bad!
        typedef QMap <int, QString> myMap;
        int throwaway = qRegisterMetaType<myMap>("myMap");

        orbiterWin.setMessage("Setting up Lmce");
        qorbiterManager  w(&orbiterWin.mainView);
        AbstractImageProvider modelimageprovider(&w);
        orbiterWin.mainView.engine()->addImageProvider("listprovider", &modelimageprovider);
#ifndef QT5
        QThread *epgThread = new QThread; //for playlists and epg of all types. only one will be active a given time inthe app
#endif
        //stored video playlist for managing any media that isnt live broacast essentially
        PlaylistClass *storedVideoPlaylist = new PlaylistClass (new PlaylistItemClass);

        //epg listmodel, no imageprovider as of yet
        EPGChannelList *simpleEPGmodel = new EPGChannelList(new EPGItemClass);
#ifndef QT5
      //  simpleEPGmodel->moveToThread(dceThread);
#endif

#ifndef QT5
        QThread * mediaThread = new QThread();
#endif

        ListModel *mediaModel = new ListModel(new gridItem);
#ifndef QT5
     //   mediaModel->moveToThread(mediaThread);
#endif
        GridIndexProvider  advancedProvider(mediaModel , 6, 4);
        orbiterWin.mainView.engine()->addImageProvider("datagridimg",&advancedProvider);

#ifndef QT5
        //advancedProvider->moveToThread(mediaThread);
#endif

        TimeCodeManager *timecode = new TimeCodeManager();
        orbiterWin.mainView.rootContext()->setContextProperty("dceTimecode", timecode);
        orbiterWin.mainView.rootContext()->setContextProperty("dataModel", mediaModel);
        orbiterWin.mainView.rootContext()->setContextProperty("mediaplaylist", storedVideoPlaylist);
        orbiterWin.mainView.rootContext()->setContextProperty("simpleepg", simpleEPGmodel);
        orbiterWin.mainView.rootContext()->setContextProperty("opengl", glpresent);


#ifndef QT5
        //shutdown signals
        QObject::connect (&w, SIGNAL(orbiterClosing()), &dceThread, SLOT(quit()),Qt::QueuedConnection);
        QObject::connect (&w, SIGNAL(orbiterClosing()), mediaThread, SLOT(quit()),Qt::QueuedConnection);
        QObject::connect (&w, SIGNAL(orbiterClosing()), epgThread, SLOT(quit()),Qt::QueuedConnection);
        QObject::connect(&dceThread, SIGNAL(finished()), &a, SLOT(quit()),Qt::QueuedConnection);
#else
        QObject::connect(&w, SIGNAL(orbiterClosing()), &a, SLOT(quit()));
#endif
        //tv epg signals
        QObject::connect(&pqOrbiter, SIGNAL(addChannel(EPGItemClass*)), simpleEPGmodel, SLOT(appendRow(EPGItemClass*)), Qt::QueuedConnection );

        //security video frames
        QObject::connect(&w, SIGNAL(getSingleCam(int,int,int)), &pqOrbiter, SLOT(GetSingleSecurityCam(int,int,int)));
        QObject::connect(&pqOrbiter, SIGNAL(securityImageReady(int, QImage)), w.SecurityVideo, SLOT(setCameraImage(int,QImage)),Qt::QueuedConnection);

        //filedetails
        QObject::connect(&pqOrbiter, SIGNAL(fd_titleChanged(QString)), w.filedetailsclass, SLOT(setTitle(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(fd_storageDeviceChanged(QString)), w.filedetailsclass, SLOT(setStorageDevice(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(fd_titleImageChanged(QImage)), w.filedetailsclass, SLOT(setTitleImage(QImage)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_mediaTitleChanged(QString)), w.filedetailsclass, SLOT(setMediaTitle(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_directorChanged(QString)), w.filedetailsclass, SLOT(setDirector(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_programChanged(QString)), w.filedetailsclass, SLOT(setProgram(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_synopChanged(QString)), w.filedetailsclass, SLOT(setSynop(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_albumChanged(QString)), w.filedetailsclass, SLOT(setAlbum(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_performersChanged(QString)), w.filedetailsclass, SLOT(setPerformers(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_genreChanged(QString)), w.filedetailsclass, SLOT(setGenre(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_channelChanged(QString)), w.filedetailsclass, SLOT(setChannel(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_pathChanged(QString)), w.filedetailsclass, SLOT(setPath(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_fileChanged(QString)), w.filedetailsclass, SLOT(setFile(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_fileNameChanged(QString)), w.filedetailsclass, SLOT(setFilename(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_episodeChanged(QString)), w.filedetailsclass, SLOT(setEpisode(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_trackChanged(QString)), w.nowPlayingButton, SLOT(setTrack(QString)),Qt::QueuedConnection);

        QObject::connect(&w, SIGNAL(mediaSeperatorChanged(int)), &pqOrbiter, SLOT(setGridSeperator(int)), Qt::QueuedConnection);

        //stored media signal
        QObject::connect(&pqOrbiter,SIGNAL(playlistItemAdded(PlaylistItemClass*)), storedVideoPlaylist,SLOT(appendRow(PlaylistItemClass*)), Qt::QueuedConnection);

        QObject::connect(&pqOrbiter,SIGNAL(resetNowPlaying()), w.nowPlayingButton, SLOT(resetData()));
        QObject::connect(&pqOrbiter, SIGNAL(setPlaylistPosition(int)), w.nowPlayingButton, SLOT(setPlaylistPostion(int)),Qt::QueuedConnection);

        QObject::connect(w.nowPlayingButton, SIGNAL(playListPositionChanged(int)), storedVideoPlaylist, SLOT(setCurrentIndex(int)) ,Qt::QueuedConnection);

        //timecodemanager signals / slots
        QObject::connect(&pqOrbiter, SIGNAL(updateTimeCode(QString,int)), timecode, SLOT(start(QString,int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(stopTimeCode()), timecode, SLOT(restart()), Qt::QueuedConnection);

        //setup
        QObject::connect(&w, SIGNAL(registerOrbiter(int,QString,int)), &pqOrbiter,SLOT(registerDevice(int,QString,int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(startManager(QString,QString)), &w, SLOT(qmlSetupLmce(QString,QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(addExistingOrbiter(ExistingOrbiter*)), w.myOrbiters, SLOT(appendRow(ExistingOrbiter*)));
        QObject::connect(&pqOrbiter, SIGNAL(deviceInvalid()), &w, SIGNAL(showList()), Qt::AutoConnection);
        QObject::connect(&pqOrbiter,SIGNAL(routerInvalid()), &orbiterWin, SIGNAL(showExternal()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(connectionValid(bool)), &orbiterWin, SLOT(setConnectionState(bool)), Qt::QueuedConnection);
        QObject::connect(&orbiterWin,SIGNAL(setupLmce(QString,QString)), &pqOrbiter, SLOT(qmlSetup(QString,QString)),Qt::QueuedConnection);
        QObject::connect(&w,SIGNAL(connectionValid(bool)), &orbiterWin, SLOT(setConnectionState(bool)));
        QObject::connect(&pqOrbiter,SIGNAL(deviceValid(bool)), &orbiterWin, SLOT(setDeviceState(bool)));
        QObject::connect(&w,SIGNAL(localConfigReady(bool)), &orbiterWin, SLOT(setLocalConfigState(bool)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(skinDataLoaded(bool)), &orbiterWin, SLOT(setSkinDataState(bool)));
        QObject::connect(&w,SIGNAL(skinIndexReady(bool)), &orbiterWin,SLOT(setSkinIndexState(bool)),Qt::QueuedConnection);
        QObject::connect(&w,SIGNAL(orbiterConfigReady(bool)), &orbiterWin, SLOT(setOrbiterConfigState(bool)));
        QObject::connect(&pqOrbiter, SIGNAL(configReady(QByteArray)), &w, SLOT(processConfig(QByteArray)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(raiseSplash()), &orbiterWin, SLOT(showSplash()) );
        QObject::connect(&w,SIGNAL(showSetup()), &orbiterWin, SLOT( showSetup()) );

        QObject::connect(&w, SIGNAL(executeCMD(int)), &pqOrbiter, SLOT(executeCommandGroup(int)));

        QObject::connect(&pqOrbiter, SIGNAL(screenSaverImages(QStringList)), w.ScreenSaver, SLOT(setImageList(QStringList)),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(setMyIp(QString)), &w, SLOT(setInternalIp(QString)),Qt::QueuedConnection);
        QObject::connect(&orbiterWin, SIGNAL(setupNewOrbiter()), &pqOrbiter,SLOT(populateSetupInformation()));
        QObject::connect(&pqOrbiter,SIGNAL(promptResponse(int,QList<QObject*>)), &orbiterWin, SLOT(displayPromptResponse(int, QList<QObject*> )));
        QObject::connect(&orbiterWin, SIGNAL(newOrbiterData(int , int , int , int , int , int )), &pqOrbiter, SLOT(setOrbiterSetupVars(int,int,int,int,int,int)));

        //messaging
        QObject::connect(mediaModel, SIGNAL(statusMessage(QString)), &w, SLOT(setDceResponse(QString)),Qt::QueuedConnection);
        QObject::connect(w.nowPlayingButton, SIGNAL(statusMessage(QString)), &w, SLOT(setDceResponse(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(statusMessage(QString)), &w , SLOT(setDceResponse(QString)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(loadingMessage(QString)), &orbiterWin,SLOT(setMessage(QString)), Qt::DirectConnection);

        //operations
        QObject::connect(&pqOrbiter, SIGNAL(addScreenParam(QString,int)), w.ScreenParameters, SLOT(addParam(QString, int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(locationChanged(int,int)), &pqOrbiter, SLOT(setLocation(int,int)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(userChanged(int)), &pqOrbiter, SLOT(setUser(int)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(bindMediaRemote(bool)), &pqOrbiter, SLOT(BindMediaRemote(bool)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(startPlayback(QString)), &pqOrbiter, SLOT(playMedia(QString)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(pause()), &pqOrbiter, SLOT(PauseMedia()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(setStreamSpeed(int)), &pqOrbiter, SLOT(setPlaybackSpeed(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(stopPlayback()), &pqOrbiter,SLOT(StopMedia()),Qt::QueuedConnection);

        QObject::connect(w.ScreenSaver, SIGNAL(requestNewImage(QString)), &pqOrbiter, SLOT(getScreenSaverImage(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(currentScreenSaverImage(const uchar*,int)), w.ScreenSaver, SLOT(setImageData(const uchar*,int)),Qt::QueuedConnection);

        // QObject::connect (&w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()));

        //sleeping alarms
        QObject::connect(&pqOrbiter, SIGNAL(sleepingAlarmsReady(QList<QObject*>)), &w, SLOT(showSleepingAlarms(QList<QObject*>)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(setAlarm(bool,int)), &pqOrbiter, SLOT(GetAlarms(bool,int)), Qt::QueuedConnection);

        //navigation
        QObject::connect(&pqOrbiter,SIGNAL(gotoQml(QString)), &w, SLOT(gotoQScreen(QString)),Qt::QueuedConnection);

        //floorplans
        QObject::connect(&w, SIGNAL(floorplanTypeChanged(int)), &pqOrbiter, SLOT(ShowFloorPlan(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(floorPlanImageData(const uchar*,int)), w.floorplans, SLOT(setImageData(const uchar*,int)), Qt::QueuedConnection);
        QObject::connect(w.floorplans, SIGNAL(pageChanged(QString)), &pqOrbiter, SLOT(getFloorPlanImage(QString)), Qt::QueuedConnection);
        QObject::connect(w.floorplans, SIGNAL(requestNewFloorPlanData(QString)), &pqOrbiter, SLOT(updateFloorPlan(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(floorplanTypeChanged(int)), w.floorplans, SLOT(setCurrentFloorPlanType(int)),Qt::QueuedConnection);
        QObject::connect(w.floorplans, SIGNAL(adjustLevel(int,myMap)), &pqOrbiter, SLOT(adjustLighting(int,myMap)),Qt::QueuedConnection);

        //mediagrid
        QObject::connect(&w, SIGNAL(gridStatus(bool)), &pqOrbiter, SLOT(setGridStatus(bool)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(gridGoBack()), &pqOrbiter, SLOT(goBackGrid()), Qt::QueuedConnection);
        QObject::connect(mediaModel, SIGNAL(pagingCleared()), &pqOrbiter,SLOT(populateAdditionalMedia()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(clearPageGrid()), mediaModel, SLOT(clearForPaging()), Qt::QueuedConnection);
        QObject::connect(mediaModel, SIGNAL(itemAdded(int)), &pqOrbiter, SLOT(setCurrentRow(int)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(clearModel()), mediaModel,SLOT(clear()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(clearAndContinue(int)), mediaModel, SLOT(clearAndRequest(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(addItem(gridItem*)), mediaModel, SLOT(appendRow(gridItem*)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(gridModelSizeChange(int)), mediaModel, SLOT(setTotalCells(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(clearModel()), mediaModel, SLOT(reset()), Qt::DirectConnection);
        QObject::connect(mediaModel,SIGNAL(ready(int)), &pqOrbiter, SLOT(prepareFileList(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(gridTypeChanged(int)), mediaModel, SLOT(setGridType(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(setDceGridParam(int,QString)), &pqOrbiter, SLOT(setStringParam(int,QString)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(keepLoading(bool)), &pqOrbiter,SLOT(setGridStatus(bool)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(showFileInfo(bool)), w.filedetailsclass, SLOT(setVisible(bool)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(setFocusFile(QString)), w.filedetailsclass, SLOT(setFile(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(modelPagesChanged(int)), mediaModel, SLOT(setTotalPages(int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(pageSeperatorChanged(int)) , mediaModel, SLOT(setSeperator(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(requestDcePages(int)), &pqOrbiter, SLOT(requestPage(int)), Qt::QueuedConnection);
        //now playing signals
        QObject::connect(&pqOrbiter, SIGNAL(setNowPlaying(bool)), w.nowPlayingButton,SLOT(setStatus(bool)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(streamIdChanged(int)), w.nowPlayingButton, SLOT(setStreamID(int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(currentScreenChanged(QString)), w.nowPlayingButton, SLOT(setScreen(QString)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(requestStreamImage()), &pqOrbiter, SLOT(getStreamingVideo()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(videoGrabReady(QImage)), w.nowPlayingButton, SLOT(setStreamImage(QImage)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(mediaTypeChanged(int)), &w, SLOT(setMediaType(int)), Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter,SIGNAL(objectDataUpdate( char,int)), w.nowPlayingButton, SLOT(setDroidImageData(char,int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(objectUpdate(QImage)), w.nowPlayingButton, SLOT(setImageData(QImage)), Qt::QueuedConnection);

        QObject::connect(simpleEPGmodel, SIGNAL(channelNumberChanged(QString)), w.nowPlayingButton, SLOT(setChannel(QString)), Qt::QueuedConnection);
        QObject::connect(simpleEPGmodel, SIGNAL(programChanged(QString)), w.nowPlayingButton, SLOT(setProgram(QString)), Qt::QueuedConnection);
        QObject::connect(simpleEPGmodel, SIGNAL(networkChanged(QString)), w.nowPlayingButton, SLOT(setChannelID(QString)), Qt::QueuedConnection);
        QObject::connect(timecode, SIGNAL(seekToTime(QString)), &pqOrbiter, SLOT(JogStream(QString)), Qt::QueuedConnection );
        //attributes
        QObject::connect(&pqOrbiter, SIGNAL(np_storageDeviceChanged(QString)), w.nowPlayingButton, SLOT(setStorageDevice(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_pathChanged(QString)), w.nowPlayingButton, SLOT(setPath(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_filename(QString)), w.nowPlayingButton, SLOT(setFilePath(QString)),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(np_mediaTitleChanged(QString)), w.nowPlayingButton, SLOT(setMediaTitle(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_album(QString)), w.nowPlayingButton, SLOT(setAlbum(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_track(QString)), w.nowPlayingButton, SLOT(setTrack(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(np_channel(QString)), w.nowPlayingButton, SLOT(setChannel(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_network(QString)), w.nowPlayingButton, SLOT(setNetwork(QString)) ,Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_channelID(QString)), w.nowPlayingButton, SLOT(setChannelID(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_program(QString)), w.nowPlayingButton, SLOT(setProgram(QString)),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(np_director(QString)), w.nowPlayingButton, SLOT(setDirector(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(np_episode(QString)), w.nowPlayingButton, SLOT(setEpisode(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_filename(QString)), w.nowPlayingButton, SLOT(setFilePath(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_genre(QString)), w.nowPlayingButton, SLOT(setGenre(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_localpathChanged(QString)), w.nowPlayingButton, SLOT(setLocalPath(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_performer(QString)), w.nowPlayingButton, SLOT(setPerformers(QString)),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(np_program(QString)), w.nowPlayingButton, SLOT(setProgram(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_releaseDate(QString)), w.nowPlayingButton, SLOT(setRelease(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_title1Changed(QString)), w.nowPlayingButton, SLOT(setTitle(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_title2Changed(QString)), w.nowPlayingButton, SLOT(setTitle2(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(subtitleChanged(QString)), w.nowPlayingButton, SLOT(setSubTitle(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(np_synopsisChanged(QString)), w.nowPlayingButton, SLOT(setSynop(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(playlistPositionChanged(int)), w.nowPlayingButton, SLOT(setPlaylistPostion(int)),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(screenshotVariablesReady(QList<QObject*>)), &w, SLOT(setScreenShotVariables(QList<QObject*>)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(screenShotReady(QImage)), &w, SLOT(setMediaScreenShot(QImage)),Qt::QueuedConnection);
        QObject::connect (&w, SIGNAL(saveMediaScreenShot(QString,QImage)), &pqOrbiter, SLOT(saveScreenAttribute(QString,QImage)),Qt::QueuedConnection);
        // myth  now playing requires special handling
        QObject::connect(&pqOrbiter, SIGNAL(mythTvUpdate(QString)), simpleEPGmodel, SLOT(setMythProgram(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(epgDone()), simpleEPGmodel, SLOT(updatePosition()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(livetvDone()), simpleEPGmodel, SLOT(updateLivePosition()), Qt::QueuedConnection);

        QObject::connect(simpleEPGmodel, SIGNAL(requestEpg()), &pqOrbiter, SLOT(requestLiveTvPlaylist()), Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(clearTVplaylist()), simpleEPGmodel, SLOT(populate()), Qt::QueuedConnection);
        QObject::connect (&w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()),Qt::DirectConnection);
        QObject::connect (&w, SIGNAL(managerPlaylistRequest()), storedVideoPlaylist,SLOT(populate()),Qt::QueuedConnection );

        //controls
        QObject::connect(&pqOrbiter, SIGNAL(resendAvButtonList(QList<QObject*>)), &w, SLOT(showDeviceCodes(QList<QObject*>)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(deviceCommandList(QList<QObject*>)), &w, SLOT(setCommandList(QList<QObject*>)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(bookmarkList(QList<QObject*>)), &w, SLOT(showBookmarks(QList<QObject*>)), Qt::QueuedConnection);
        //so does live tv
        QObject::connect(&w, SIGNAL(clearModel()), simpleEPGmodel, SLOT(empty()),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(liveTvUpdate(QString)), simpleEPGmodel, SLOT(setProgram(QString)), Qt::QueuedConnection);
        //epg specific

        //storemediaplaylist specific
        QObject::connect(storedVideoPlaylist,SIGNAL(playlistReady()), &pqOrbiter,SLOT(requestMediaPlaylist()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(clearPlaylist()), storedVideoPlaylist,  SLOT(populate()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(playlistDone()), storedVideoPlaylist, SIGNAL(activeItemChanged()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(closeOrbiter()), &w, SLOT(closeOrbiter()),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(resendDeviceCode(int,int)), &pqOrbiter, SLOT(sendAvCommand(int,int)), Qt::QueuedConnection);

        QObject::connect(&pqOrbiter,SIGNAL(routerReloading(QString)), &w, SLOT(reloadHandler()) );
        //FIXME: below emits error: QObject::connect: Attempt to bind non-signal orbiterWindow::close()
        //QObject::connect (&w,SIGNAL, &w, SLOT(closeOrbiter()), Qt::DirectConnection);
        QObject::connect(&w, SIGNAL(reloadRouter()), &pqOrbiter, SLOT(quickReload()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(routerDisconnect()), &w, SLOT(reloadHandler()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(checkReload()), &w, SLOT(connectionWatchdog()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(reInitialize()), &pqOrbiter, SLOT(initialize()), Qt::QueuedConnection);



        dceThread.start();
        // tcThread->start();
#ifndef QT5
        //mediaThread->start();
        //epgThread->start();
#endif

        pqOrbiter.setDeviceId(w.iPK_Device);
        pqOrbiter.m_sHostName = w.qs_routerip.toStdString();
        pqOrbiter.m_sIPAddress = w.qs_routerip.toStdString();
        pqOrbiter.m_sExternalIP = w.qs_ext_routerip.toStdString();

        qDebug() << "Initializing connection";
        pqOrbiter.pingCore();
        PK_Device = pqOrbiter.m_dwPK_Device;
        a.exec();

        if( pqOrbiter.m_bReload )

        { bReload=true; }
        a.quit();
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
