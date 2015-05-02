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

/*!
 *\page main Main.cpp
 *
 * \brief main.cpp is where the application starts and is glued together with high quality Qt brand glue.
 *
 *main.cpp begins the main event loop of the application. Part of initializing this application involves starting QThreads, opening application
 *windows, and other activities to begin the LinuxMCE orbiter. Most important of these, are the signal and slot connections. These connect
 *the DCE server object to the Qt and thus QML GUI by emitting images and binary data to \ref qOrbiterManager.
 *Related objects include:
 *-# \ref qorbiterlogger - Local logging class
 *-# \ref listModel - QAbstractlistmodel used for media grid.
 *-# \ref orbiterwindow - Class to provide splash screen independant of other initialization
 *#- \ref timecodemanager - QTcpSocket based class for processing timecode from media players.
 *#- \ref qOrbiter - The server side of the equation
 *#- \ref qOrbiterManager - the Qt / QML object providing the GUI
 * \ingroup dce_controllers
 * \ingroup qt_controllers
 */

#ifdef Q_OS_IOS
# include <UIKit/UIKit.h>

#include "ioshelpers.h"
#include <QtGui/QApplication>
#include <QtCore/QtPlugin>
#include <QtDeclarative/QDeclarativeEngine>

Q_IMPORT_PLUGIN(UIKit)
#endif

#include <QtGlobal>
#ifdef QT5
#include <QtWidgets/QApplication>
#include <QQmlExtensionPlugin>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtCore/QThread>
#include <contextobjects/settinginterface.h>

#else
#include <QApplication>
#include <QtDeclarative/QDeclarativeEngine>
#endif

#include "logger/qorbiterlogger.h"
#include "datamodels/listModel.h"

#include "qOrbiter/qOrbiter.h"
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
#include <defineObjects/linuxmcedata.h>
#include <defineObjects/mediatypehelper.h>
#include <defineObjects/datagrids.h>
#include <RemoteCommands.h>
#ifdef debug
#include <QDebug>
#endif

#ifdef ANDROID
#include "plugins/AndroidInfo/androidsystem.h"
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

static QObject *mediatypes_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    MediaTypesHelper *helper = new MediaTypesHelper();
    return helper;
}
#ifdef Q_OS_IOS

Q_IMPORT_PLUGIN(DceScreenSaverPlugin)
Q_IMPORT_PLUGIN(AudioVisualPlugin)

#endif


#if defined(Q_OS_IOS)
extern "C" int qtmn(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    int deviceType =-1; /*!< deviceType can be 0 for osd(onscreen qorbiter), 1 for desktop(linux,mac,windows),2 for android, 3 for i0S */


#ifdef for_harmattan
    QApplication::setGraphicsSystem("meego");
#elif GLENABLED
#ifndef QT5
    // QApplication::setGraphicsSystem("opengl");
#endif
#else
    QApplication::setGraphicsSystem("raster");
#endif



#if(QT_VERSION >= 0x040800)
    Qt::AA_X11InitThreads;
#endif

#ifdef QT5
    QGuiApplication  a(argc, argv);
#else
    QApplication  a(argc, argv);
#endif
#ifdef Q_OS_IOS
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_DceScreenSaverPlugin().instance())->registerTypes("DceScreenSaver");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_AudioVisualPlugin().instance())->registerTypes("AudioVisual");
#endif

    QCoreApplication::setApplicationName("QOrbiter");
    QCoreApplication::setOrganizationDomain("org.linuxmce.QOrbiter");
    QCoreApplication::setOrganizationName("LinuxMCE");
   // qRegisterMetaType<SettingsInterfaceType>("SettingsInterfaceType");
   // qRegisterMetaType<SettingsKeyType >("SettingKeyType");


    SettingInterface settings;    

#ifdef __ANDROID__
    AndroidSystem androidHelper;
    deviceType=2;
#endif

    QOrbiterLogger localLogger;

    g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
    g_sBinaryPath = FileUtils::BasePath(argv[0]);
    cout << "qOrbiter, v." << VERSION << endl
         << "Visit www.linuxmce.org for source code and license information" << endl << endl;
    string sRouter_IP="";
    string graphicsmode="raster";
    string screen = "";
    int PK_Device=-1;
    int screenSize=-1;
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
            qDebug()<< "Command line hostname: " << sRouter_IP.c_str();
            break;
        case 'd':
            PK_Device = atoi(argv[++optnum]);
            qDebug() << "Command Line Device" << PK_Device;
            break;
        case 'L':
            bLocalMode = true;
            break;
        case 'l':
            sLogger = argv[++optnum];
            break;
        case 's':
            screen="fullscreen";
            deviceType=0;
            break;
        case 'o':
            screen="fullscreen";
            break;
        case 'z':
            screenSize=atoi(argv[++optnum]);
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
             << "-l -- Where to save the log files.  Specify 'dcerouter' to have the messages logged to the DCE Router.  Defaults to stdout." << endl
             << "-o --Switch for frameless MD and desktops." << endl
             << "-s --Switch for fullscreen." << endl
             << "-z --Screen size setting for testing 4 - small \n 7 - medium\n 10 - large" << endl;

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
    deviceType=1;
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
#ifdef Q_OS_IOS
        qDebug() << qt_static_plugin_DceScreenSaverPlugin().metaData();
        deviceType=3;
        //        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif

#ifdef GLENABLED
        bool glpresent= true;
#else
        bool glpresent = false;
#endif
        bool fs = false;
        bool fm = false;
        if(screen =="fullscreen"){
            fs=true;
            fm=true;
        }

        /*! Command impl based routine in here to execute needed dt determinations? */
        /*! Only required for x11 (right?), should consider android dt creation */
        /*! See line 4702 for idea on how to get data from router to execute selection */
        /*! UI will need to be created to allow the device to make the selections before the device tries to connect */

        QThread dceThread;

        qOrbiter pqOrbiter(PK_Device, sRouter_IP,true,bLocalMode );
        qmlRegisterType<MediaTypesHelper>("org.linuxmce.enums",1,0,"MediaTypes");
        //qmlRegisterSingletonType<MediaTypesHelper>("media.enums",1,0,"MediaTypesHelper", "Data Only for linuxMCE MediaTypes");
        qmlRegisterSingletonType<MediaTypesHelper>("enums.media", 1, 0, "MediaHelper", MediaTypesHelper::mediatypes_provider);
        qmlRegisterType<SubTypesHelper>("org.linuxmce.enums",1,0,"MediaSubtypes");
        qmlRegisterType<AttributeTypeHelper>("org.linuxmce.enums",1,0,"Attributes");
        qmlRegisterType<HostSystemData>("org.linuxmce.enums",1,0,"HostDevices");
        qmlRegisterType<RemoteCommands>("org.linuxmce.enums", 1,0, "RemoteCommands");
        qmlRegisterType<DataGrids>("org.linuxmce.grids", 1,0, "DataGrids");
        qmlRegisterType<UtilityCommands>("org.linuxmce.enums", 1,0, "UtilityCommands");
        qmlRegisterType<HouseModesHelper>("org.linuxmce.enums", 1,0, "HouseModes");
        qmlRegisterType<UserModesHelper>("org.linuxmce.enums",1,0,"HouseModes");
        qmlRegisterType<RoomTypeHelper>("org.linuxmce.enums", 1,0, "RoomTypes");
        qmlRegisterType<ScreenList>("org.linuxmce.screens", 1,0, "Screens");
        qmlRegisterType<ScreenData>("org.linuxmce.screeninfo", 1,0, "ScreenData");
        qmlRegisterType<SettingsInterfaceType>("org.linuxmce.settings", 1,0, "SettingsType");
        qmlRegisterType<SettingsKeyType>("org.linuxmce.settings", 1,0, "SettingsKey");
        qmlRegisterType<GenericFlatListModel>();

        orbiterWindow orbiterWin(PK_Device, sRouter_IP, fs, fm, screenSize);
        orbiterWin.mainView.rootContext()->setContextProperty("settings", &settings);
#ifdef __ANDROID__
        orbiterWin.mainView.rootContext()->setContextProperty("androidSystem", &androidHelper);
#endif
        orbiterWin.setMessage("Setting up Lmce");


#ifndef ANDROID

        qorbiterManager  w(&orbiterWin.mainView, screenSize, &settings);
        if(!sRouter_IP.empty()){
            w.setInternalIp(QString::fromStdString(sRouter_IP));
            orbiterWin.setRouterAddress(QString::fromStdString(sRouter_IP));
        }
        if(PK_Device!=-1){
            w.setDeviceNumber(PK_Device);
        }

        if(deviceType==0){
            w.setDeviceTemplate(DEVICETEMPLATE_OnScreen_qOrbiter_CONST);
            //pqOrbiter.m_pData->m_dwPK_DeviceTemplate=DEVICETEMPLATE_OnScreen_qOrbiter_CONST;
            pqOrbiter.m_bIsOSD=true;
            qDebug() <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!SETTING ON SCREEN FLAG!!!!!!!!!!!!!!!!!!!!!!!!";
        }
#else
        qorbiterManager w(&orbiterWin.mainView, &androidHelper, &settings);
        orbiterWin.mainView.rootContext()->setContextProperty("androidSystem", &androidHelper);
#endif
        orbiterWin.mainView.rootContext()->setContextProperty("manager", &w);
#if defined(Q_OS_IOS)
        w.setMobileStorage(localLogger.logLocation);
#endif


        AbstractImageProvider* modelimageprovider = new AbstractImageProvider(&w);
        orbiterWin.mainView.engine()->addImageProvider("listprovider", modelimageprovider);
        pqOrbiter.moveToThread(&dceThread);
        QObject::connect(&dceThread, SIGNAL(started()), &pqOrbiter, SLOT(beginSetup()));
        //epg listmodel, no imageprovider as of yet

        // ListModel *mediaModel = new ListModel(new gridItem);
        qDebug() << "!!!!!!!!!!!BREAK!!!!!!!!!!!!!!!!!!";
        TimeCodeManager *timecode = new TimeCodeManager();

        orbiterWin.mainView.rootContext()->setContextProperty("logger", &localLogger);
        orbiterWin.mainView.rootContext()->setContextProperty("dceTimecode", timecode);
        // orbiterWin.mainView.rootContext()->setContextProperty("dataModel", mediaModel);
        orbiterWin.mainView.rootContext()->setContextProperty("opengl", glpresent);
        qRegisterMetaType<QHash<int, QVariant> >("QHash<int, QVariant>");
        qRegisterMetaType<DCE::PreformedCommand>("DCE::PreformedCommand");


        // connnect local logger
        QObject::connect(&pqOrbiter, SIGNAL(commandResponseChanged(QString)), &localLogger, SLOT(logCommandMessage(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(mediaMessage(QString)), &localLogger, SLOT(logGuiMessage(QString)));

#ifdef QT4_8
        QObject::connect(orbiterWin.mainView.engine(), SIGNAL(warnings(QList<QDeclarativeError>)), &localLogger, SLOT(logQmlErrors(QList<QDeclarativeError>)));
#elif QT5
        QObject::connect(orbiterWin.mainView.engine(), SIGNAL(warnings(QList<QQmlError>)), &localLogger, SLOT(logQmlErrors(QList<QQmlError>)));
#endif
        QObject::connect(&w, SIGNAL(skinMessage(QString)), &localLogger, SLOT(logSkinMessage(QString)));
        QObject::connect(&w, SIGNAL(qtMessage(QString)) , &localLogger, SLOT(logQtMessage(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(newDceMessage(QString)), &localLogger, SLOT(logCommandMessage(QString)),Qt::QueuedConnection);
#ifdef ANDROID
        QObject::connect(&w, SIGNAL(mobileStorageChanged(QString)), &localLogger, SLOT(setLogLocation(QString)));
#endif

        //shutdown signals
        QObject::connect(&w, SIGNAL(destroyed()), &pqOrbiter, SLOT(deinitialize()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(closeOrbiter()), &pqOrbiter, SLOT(shutdown()));


        QObject::connect(&dceThread, SIGNAL(finished()), &dceThread, SLOT(deleteLater()));
        QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
        // QObject::connect(&dceThread, SIGNAL(finished()),&a, SLOT(quit()));

        // Generic DCE command sending signal/slots
        QObject::connect(&w, SIGNAL(sendDceCommand(DCE::PreformedCommand)), &pqOrbiter, SLOT(handleDceCommand(DCE::PreformedCommand)), Qt::QueuedConnection);
      //  NO! QObject::connect(&w, SIGNAL(sendDceCommandResponse(DCE::PreformedCommand&, string*)), &pqOrbiter, SLOT(sendDCECommandResponse(DCE::PreformedCommand&, string*)), Qt::QueuedConnection);


        QObject::connect(&pqOrbiter, SIGNAL(routerConnectionChanged(bool)), &w, SLOT(setConnectedState(bool)), Qt::QueuedConnection);


        // QObject::connect (&w, SIGNAL(orbiterClosing()), &dceThread, SLOT(quit()),Qt::QueuedConnection);
        //  QObject::connect (&w, SIGNAL(orbiterClosing()), mediaThread, SLOT(quit()),Qt::QueuedConnection);
        //  QObject::connect (&w, SIGNAL(orbiterClosing()), epgThread, SLOT(quit()),Qt::QueuedConnection);
        //  QObject::connect(&dceThread, SIGNAL(finished()), &a, SLOT(quit()),Qt::QueuedConnection);

        //tv epg signals
        //QObject::connect(&pqOrbiter, SIGNAL(addChannel(EPGItemClass*)), simpleEPGmodel, SLOT(appendRow(EPGItemClass*)), Qt::QueuedConnection );
        QObject::connect(&w, SIGNAL(newGridChannel(QString,QString)), &pqOrbiter, SLOT(TuneToChannel(QString,QString)), Qt::QueuedConnection );
        //QObject::connect(&w, SIGNAL(clearModel()), simpleEPGmodel, SLOT(clear()), Qt::QueuedConnection);
        //security video frames
        QObject::connect(&w, SIGNAL(newHouseMode(QString,int)), &pqOrbiter, SLOT(SetSecurityMode(QString,int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(getSingleCam(int,int,int)), &pqOrbiter, SLOT(GetSingleSecurityCam(int,int,int)));
        QObject::connect(&pqOrbiter, SIGNAL(securityImageReady(int, QImage)), w.SecurityVideo, SLOT(setCameraImage(int,QImage)),Qt::QueuedConnection);

        //filedetails
        QObject::connect(&pqOrbiter, SIGNAL(fd_imageUrlChanged(QString)), w.filedetailsclass, SLOT(setScreenshot(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(fd_titleChanged(QString)), w.filedetailsclass, SLOT(setTitle(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(fd_storageDeviceChanged(QString)), w.filedetailsclass, SLOT(setStorageDevice(QString)), Qt::QueuedConnection);
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
        QObject::connect(&pqOrbiter, SIGNAL(fd_trackChanged(QString)), w.filedetailsclass, SLOT(setTrack(QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(fd_ratingChanged(QString)), w.filedetailsclass, SLOT(setRating(QString)));
        QObject::connect(&pqOrbiter,SIGNAL(fd_studioChanged(QString)), w.filedetailsclass, SLOT(setStudio(QString)));
        QObject::connect(&w, SIGNAL(mediaSeperatorChanged(int)), &pqOrbiter, SLOT(setGridSeperator(int)), Qt::QueuedConnection);


        QObject::connect(&pqOrbiter,SIGNAL(resetNowPlaying()), w.nowPlayingButton, SLOT(resetData()));
        QObject::connect(&pqOrbiter, SIGNAL(setPlaylistPosition(int)), w.nowPlayingButton, SLOT(setPlaylistPostion(int)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(changeTrack(QString)), &pqOrbiter, SLOT(changedTrack(QString)), Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(clearPlaylist()), &w, SLOT(updatePlaylist()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(newPlaylistPosition(QString)), &pqOrbiter, SLOT(jumpToPlaylistPosition(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(movePlistEntry(QString,int)), &pqOrbiter, SLOT(movePlaylistEntry(QString,int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(savePlist(QString,bool)), &pqOrbiter, SLOT(saveCurrentPlaylist(QString,bool)), Qt::QueuedConnection);
        /*Misc*/

        QObject::connect(&pqOrbiter, SIGNAL(discreteAudioChanged(bool)), &w, SLOT(setDiscreteAudio(bool)));
        QObject::connect(&pqOrbiter, SIGNAL(commandResponseChanged(QString)), &w, SLOT(setCommandResponse(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(mediaResponseChanged(QString)), &w, SLOT(setMediaResponse(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(deviceResponseChanged(QString)), &w, SLOT(setDeviceResponse(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(eventResponseChanged(QString)), &w, SLOT(setEventResponse(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(liveAvPath(bool)), &w, SLOT(setLiveAvPath(bool)));
        QObject::connect(&pqOrbiter, SIGNAL(containsVideo(bool)), &w, SLOT(setContainsVideo(bool)));
        QObject::connect(&pqOrbiter, SIGNAL(isOsd(bool)), &w, SLOT(setOsd(bool)));
        QObject::connect(&pqOrbiter, SIGNAL(monitorStatusChanged(bool)), &w, SLOT(setMonitorStatus(bool)));
        QObject::connect(&w, SIGNAL(newMessageSend(QVariantMap)), &pqOrbiter, SLOT(executeMessageSend(QVariantMap)), Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(setText(QString,QString,int)), &w, SLOT(setText(QString,QString,int)), Qt::QueuedConnection);
        //timecodemanager signals / slots
        QObject::connect(&pqOrbiter, SIGNAL(updateTimeCode(QString,int)), timecode, SLOT(start(QString,int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(stopTimeCode()), timecode, SLOT(stop()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(connectionLost()), &w, SLOT(disconnectHandler()), Qt::QueuedConnection);

        //setup
        QObject::connect(&w, SIGNAL(switchIpAddress(QString)), &pqOrbiter,SLOT(changeAndRestart(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(usingExternalChanged(bool)), &orbiterWin, SLOT(setUsingExternal(bool)));
        QObject::connect(&w, SIGNAL(internalIpChanged(QString)), &orbiterWin, SLOT(setInternalIp(QString)));
        QObject::connect(&w, SIGNAL(externalIpChanged(QString)), &orbiterWin, SLOT(setExternalIp(QString)));
        QObject::connect(&w, SIGNAL(orbiterInitialized()), &orbiterWin, SLOT(setOrbiterInitialized()));
        QObject::connect(&w, SIGNAL(registerOrbiter(int,QString,int)), &pqOrbiter,SLOT(registerDevice(int,QString,int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(startManager(QString,QString)), &w, SLOT(qmlSetupLmce(QString,QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(addExistingOrbiter(ExistingOrbiter*)), w.myOrbiters, SLOT(appendRow(ExistingOrbiter*)));
        QObject::connect(&pqOrbiter, SIGNAL(deviceInvalid()), &orbiterWin, SLOT(prepareExistingOrbiters()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(clearExistingOrbiters()), w.myOrbiters, SLOT(clear()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(routerInvalid()), &orbiterWin, SIGNAL(showExternal()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(connectionValid(bool)), &orbiterWin, SLOT(setConnectionState(bool)), Qt::QueuedConnection);
        QObject::connect(&orbiterWin,SIGNAL(setupLmce(int, QString)), &pqOrbiter, SLOT(qmlSetup(int, QString)),Qt::QueuedConnection);
        QObject::connect(&w,SIGNAL(connectionValid(bool)), &orbiterWin, SLOT(setConnectionState(bool)));
        QObject::connect(&pqOrbiter,SIGNAL(deviceValid(bool)), &orbiterWin, SLOT(setDeviceState(bool)));
        QObject::connect(&w,SIGNAL(localConfigReady(bool)), &orbiterWin, SLOT(setLocalConfigState(bool)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(skinDataLoaded(bool)), &orbiterWin, SLOT(setSkinDataState(bool)));
        QObject::connect(&w,SIGNAL(skinIndexReady(bool)), &orbiterWin,SLOT(setSkinIndexState(bool)),Qt::QueuedConnection);
        QObject::connect(&w,SIGNAL(orbiterConfigReady(bool)), &orbiterWin, SLOT(setOrbiterConfigState(bool)));
        //QObject::connect(&pqOrbiter, SIGNAL(configReady(QByteArray)), &w, SLOT(processConfig(QByteArray)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(raiseSplash()), &orbiterWin, SLOT(showSplash()) );
        QObject::connect(&w,SIGNAL(showSetup()), &orbiterWin, SLOT( showSetup()) );

        QObject::connect(&w, SIGNAL(executeCMD(int)), &pqOrbiter, SLOT(executeCommandGroup(int)));

        QObject::connect(&pqOrbiter, SIGNAL(screenSaverImages(QStringList)), &w, SLOT(setScreenSaverImages(QStringList)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(screenSaverTimerOutChanged(int)), &w, SLOT(setScreensaverTimerout(int)));
        QObject::connect(&w, SIGNAL(updateDceScreenSaverTimeout(int)), &pqOrbiter, SLOT(updateScreenSaverTimeout(int)));

        QObject::connect(&pqOrbiter, SIGNAL(setMyIp(QString)), &w, SLOT(setInternalIp(QString)),Qt::QueuedConnection);
        QObject::connect(&orbiterWin, SIGNAL(beginSetupNewOrbiter()), &pqOrbiter,SLOT(populateSetupInformation()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(promptResponse(int,QList<QObject*>)), &orbiterWin, SLOT(displayPromptResponse(int, QList<QObject*> )), Qt::QueuedConnection);
        QObject::connect(&orbiterWin, SIGNAL(newOrbiterData(int , int , int , int , int , int )), &pqOrbiter, SLOT(setOrbiterSetupVars(int,int,int,int,int,int)), Qt::QueuedConnection);

        /*CHild Devices*/
        QObject::connect(&pqOrbiter, SIGNAL(qMediaPlayerIDChanged(int)), &w, SLOT(setMediaPlayerID(int)), Qt::QueuedConnection);

        //messaging
        //  QObject::connect(mediaModel, SIGNAL(statusMessage(QString)), &w, SLOT(setDceResponse(QString)),Qt::QueuedConnection);
        QObject::connect(w.nowPlayingButton, SIGNAL(statusMessage(QString)), &w, SLOT(setDceResponse(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(statusMessage(QString)), &w , SLOT(setDceResponse(QString)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(loadingMessage(QString)), &orbiterWin,SLOT(setMessage(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(commandComplete()), &w, SIGNAL(commandCompleted()), Qt::QueuedConnection);
        //operations
        QObject::connect(&pqOrbiter, SIGNAL(addScreenParam(QString,int)), w.ScreenParameters, SLOT(addParam(QString, int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(locationChanged(int,int)), &pqOrbiter, SLOT(setLocation(int,int)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(userChanged(int)), &pqOrbiter, SLOT(setUser(int)),Qt::QueuedConnection);
        //QObject::connect(w.ScreenSaver, SIGNAL(requestNewImage(QString)), &pqOrbiter, SLOT(getScreenSaverImage(QString)), Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter, SIGNAL(currentScreenSaverImage(QByteArray)), w.ScreenSaver, SLOT(setImageData(QByteArray)),Qt::QueuedConnection);

        // QObject::connect (&w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()));

        //sleeping alarms
        QObject::connect(&w, SIGNAL(setAlarm(QString,int,int,bool,int)), &pqOrbiter, SLOT(setAlarm(QString,int,int,bool,int)), Qt::QueuedConnection);

        //navigation
        QObject::connect(&pqOrbiter,SIGNAL(gotoQml(QString)), &w, SLOT(setCurrentScreen(QString)),Qt::QueuedConnection);

        //floorplans
        QObject::connect(&w, SIGNAL(floorplanTypeChanged(int)), &pqOrbiter, SLOT(ShowFloorPlan(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(floorPlanImageData(const uchar*,int)), w.floorplans, SLOT(setImageData(const uchar*,int)), Qt::QueuedConnection);
        QObject::connect(w.floorplans, SIGNAL(pageChanged(QString)), &pqOrbiter, SLOT(getFloorPlanImage(QString)), Qt::QueuedConnection);
        QObject::connect(w.floorplans, SIGNAL(requestNewFloorPlanData(QString)), &pqOrbiter, SLOT(updateFloorPlan(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(floorplanTypeChanged(int)), w.floorplans, SLOT(setCurrentFloorPlanType(int)),Qt::QueuedConnection);

        QObject::connect(&w, SIGNAL(populate_floorplan_device_commands(int)), &pqOrbiter, SLOT(getFloorplanDeviceCommand(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(addFloorplanDeviceCommand(QVariantMap)), &w, SLOT(setFloorPlanCommand(QVariantMap)), Qt::QueuedConnection);
        //mediagrid
        QObject::connect(&w, SIGNAL(gridStatus(bool)), &pqOrbiter, SLOT(setGridStatus(bool)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(resetSearchParams()), &pqOrbiter, SLOT(initializeGrid()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(cancelRequests()), &pqOrbiter, SLOT(cancelAllRequests()), Qt::QueuedConnection);
        // QObject::connect(mediaModel, SIGNAL(pagingCleared()), &pqOrbiter,SLOT(populateAdditionalMedia()), Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(clearPageGrid()), mediaModel, SLOT(clearForPaging()), Qt::QueuedConnection);
        // QObject::connect(mediaModel, SIGNAL(itemAdded(int)), &pqOrbiter, SLOT(setCurrentRow(int)),Qt::QueuedConnection);
        //  QObject::connect(&w, SIGNAL(clearModel()), mediaModel,SLOT(clear()), Qt::QueuedConnection);
        //  QObject::connect(&pqOrbiter,SIGNAL(addItem(gridItem*)), mediaModel, SLOT(appendRow(gridItem*)),Qt::QueuedConnection);
        //  QObject::connect(&pqOrbiter,SIGNAL(gridModelSizeChange(int)), mediaModel, SLOT(setTotalCells(int)), Qt::QueuedConnection);
        //  QObject::connect(&pqOrbiter, SIGNAL(clearModel()), mediaModel, SLOT(reset()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(clearFileDetails()), w.attribFilter, SLOT(clear()));

        // QObject::connect(&w, SIGNAL(gridTypeChanged(int)), mediaModel, SLOT(setGridType(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(authUserMedia(int,QString,int)), &pqOrbiter, SLOT(authorizePrivateMedia(int,QString,int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(mediaAuthChanged(int)), w.userList, SLOT(setCurrentPrivateUser(int)), Qt::QueuedConnection);
        //  QObject::connect(w.userList, SIGNAL(privateUserChanged(int, QString)), &pqOrbiter, SLOT(setStringParam(int,QString)),Qt::QueuedConnection);
        // QObject::connect(mediaModel,SIGNAL(gridTypeChanged(int)), w.userList, SLOT(unsetPrivate()));

        // QObject::connect(mediaModel, SIGNAL(pauseChanged(bool)), &pqOrbiter, SLOT(setGridPause(bool)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(keepLoading(bool)), &pqOrbiter,SLOT(setGridStatus(bool)),Qt::QueuedConnection);
        //  QObject::connect(&pqOrbiter, SIGNAL(modelPagesChanged(int)), mediaModel, SLOT(setTotalPages(int)),Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(pageSeperatorChanged(int)) , mediaModel, SLOT(setSeperator(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(requestDcePages(int)), &pqOrbiter, SLOT(requestPage(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(seekGrid(QString)), &pqOrbiter, SLOT(seekToGridPosition(QString)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(pageSeperatorChanged(int)), &w, SLOT(setGridSeperator(int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(mediaPageChanged(int)), &w, SLOT(setCurrentPage(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(dceGridSepChanged(int)), &pqOrbiter, SLOT(setGridSeperator(int)), Qt::QueuedConnection);

        // Media filter
        QObject::connect(&pqOrbiter, SIGNAL(showFileListMediaType(int)), &w.mediaFilter, SLOT(setMediaType(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(showFileListMediaType(int)), &w, SLOT(setGridMediaType(QString)), Qt::QueuedConnection);

        //        QObject::connect(&pqOrbiter, SIGNAL(showFileListMediaType(int)), &w, SLOT(prepareFileList()), Qt::QueuedConnection);
        //QObject::connect(&w.mediaFilter, SIGNAL(filterChanged(int)), mediaModel, SLOT(clearAndRequest(int)), Qt::QueuedConnection);
        QObject::connect(&w.mediaFilter, SIGNAL(genericOptionsChanged(QString)), &w, SLOT(genericFilterChanged(QString)), Qt::QueuedConnection);
        QObject::connect(&w.mediaFilter, SIGNAL(filterStringChanged(QString)), &w, SLOT(mediaFilterChanged(QString)), Qt::QueuedConnection);
        QObject::connect(&w.mediaFilter, SIGNAL(itemSelected(QString)), &pqOrbiter, SLOT(GetFileInfoForQml(QString)), Qt::QueuedConnection);
        QObject::connect(&w.mediaFilter, SIGNAL(itemSelected(QString)), &w, SLOT(mediaItemSelected(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(gridGoBack()), &w.mediaFilter, SLOT(goBack()), Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter, SIGNAL(noMediaFound()), &w.mediaFilter, SLOT(noMedia()),Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(newAttributeSort(AttributeSortItem*)), w.attribFilter, SLOT(appendRow(AttributeSortItem*)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(newMediaSubtype(AttributeSortItem*)), w.mediaTypeFilter, SLOT(appendRow(AttributeSortItem*)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(newFileFormatSort(AttributeSortItem*)), w.uiFileFilter, SLOT(appendRow(AttributeSortItem*)), Qt::QueuedConnection);

        // generic datagrid signals
        QObject::connect(&pqOrbiter,SIGNAL(prepareDataGrid(QString,QString,int,int)), &w, SLOT(prepareDataGrid(QString,QString,int,int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(addDataGridItem(QString,int,int,int,DataGridTable*)), &w, SLOT(addDataGridItem(QString,int,int,int,DataGridTable*)),Qt::QueuedConnection);
        QObject::connect(&w,SIGNAL(loadDataGrid(QString,int,QString)), &pqOrbiter, SLOT(loadDataGrid(QString,int,QString)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(updateItemData(QString,int,int,QVariant)), &w, SLOT(updateItemData(QString,int,int,QVariant)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(loadDataForDataGrid(QString,QString,int,QString,int,int,int,QString)), &pqOrbiter, SLOT(loadDataForDataGrid(QString,QString,int,QString,int,int,int,QString)), Qt::QueuedConnection);

        //now playing signals
        QObject::connect(&pqOrbiter, SIGNAL(nowPlayingDeviceListChanged(QVariantList)), w.nowPlayingButton, SLOT(setNowPlayingDeviceList(QVariantList)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(setNowPlaying(bool)), w.nowPlayingButton,SLOT(setStatus(bool)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(streamIdChanged(int)), w.nowPlayingButton, SLOT(setStreamID(int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(currentScreenChanged(QString)), w.nowPlayingButton, SLOT(setScreen(QString)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(screenChange(QString)), &pqOrbiter, SLOT(setCurrentScreen(QString)));
        QObject::connect(&w, SIGNAL(requestStreamImage()), &pqOrbiter, SLOT(getStreamingVideo()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(requestStoredMediaImage(QString)), &pqOrbiter, SLOT(grabScreenshot(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(requestVideoFrame()), &pqOrbiter, SLOT(getStreamingVideo()), Qt::QueuedConnection );
        QObject::connect(&pqOrbiter, SIGNAL(videoGrabReady(QImage)), &w, SLOT(setMediaScreenShot(QImage)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(screenShotReady(QImage)), &w, SLOT(setMediaScreenShot(QImage)), Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(mediaTypeChanged(int)), &w, SLOT(setMediaType(int)), Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter,SIGNAL(objectDataUpdate( char,int)), w.nowPlayingButton, SLOT(setDroidImageData(char,int)),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(objectUpdate(QByteArray)), w.nowPlayingButton, SLOT(setImageData(QByteArray)), Qt::QueuedConnection);


        //  QObject::connect(simpleEPGmodel, SIGNAL(channelNumberChanged(QString)), w.nowPlayingButton, SLOT(setChannel(QString)), Qt::QueuedConnection);
        // QObject::connect(simpleEPGmodel, SIGNAL(programChanged(QString)), w.nowPlayingButton, SLOT(setProgram(QString)), Qt::QueuedConnection);
        // QObject::connect(simpleEPGmodel, SIGNAL(networkChanged(QString)), w.nowPlayingButton, SLOT(setChannelID(QString)), Qt::QueuedConnection);
        QObject::connect(timecode, SIGNAL(seekToTime(QString)), &pqOrbiter, SLOT(JogStream(QString)), Qt::QueuedConnection );
        QObject::connect(&w, SIGNAL(seekPositionChanged(QString)), &pqOrbiter, SLOT(setPosition(QString)) );
        QObject::connect(&w, SIGNAL(jogToPosition(QString)), &pqOrbiter, SLOT(JogStream(QString)));

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

        QObject::connect(&pqOrbiter, SIGNAL(screenshotVariablesReady()), &w, SLOT(showScreenShotVariables()),Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(addScreenShotVar(screenshotAttributes*)), &w, SLOT(setScreenShotVariables(screenshotAttributes*)));
        QObject::connect(&pqOrbiter, SIGNAL(screenShotReady(QImage)), &w, SLOT(setMediaScreenShot(QImage)),Qt::QueuedConnection);
        QObject::connect (&w, SIGNAL(saveMediaScreenShot(QString)), &pqOrbiter, SLOT(saveScreenAttribute(QString)),Qt::QueuedConnection);

        // myth  now playing requires special handling
        //QObject::connect(&pqOrbiter, SIGNAL(mythTvUpdate(QString)), simpleEPGmodel, SLOT(setMythProgram(QString)),Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter, SIGNAL(epgDone()), simpleEPGmodel, SLOT(updatePosition()),Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(livetvDone()), simpleEPGmodel, SLOT(updateLivePosition()), Qt::QueuedConnection);
        // QObject::connect(simpleEPGmodel, SIGNAL(requestEpg()), &pqOrbiter, SLOT(requestLiveTvPlaylist()), Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter, SIGNAL(clearTVplaylist()), simpleEPGmodel, SLOT(populate()), Qt::QueuedConnection);
        //QObject::connect (&w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()),Qt::DirectConnection);
        // QObject::connect (&w, SIGNAL(managerPlaylistRequest()), storedVideoPlaylist,SLOT(populate()),Qt::QueuedConnection );

        //controls
        QObject::connect(&pqOrbiter,SIGNAL(newDeviceCommand(AvCommand*)), &w, SLOT(addCommandToList(AvCommand*)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(populateDeviceCommands(int)), &pqOrbiter, SLOT(GetAdvancedMediaOptions(int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(addDevice(AvDevice*)), &w, SLOT(addDeviceToList(AvDevice*)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(bookmarkList(QList<QObject*>)), &w, SLOT(showBookmarks(QList<QObject*>)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(extraButton(QString)), &pqOrbiter, SLOT(extraButtons(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(resendAvCodes()), &pqOrbiter, SLOT(showAdvancedButtons()), Qt::QueuedConnection );
        QObject::connect(&pqOrbiter, SIGNAL(deviceAudioLevelChanged(int)), &w, SLOT(setDeviceVolume(int)));

        QObject::connect(&w, SIGNAL(moveArrowDirection(int)), &pqOrbiter, SLOT(moveDirection(int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(signalGoBack()), &pqOrbiter, SLOT(osdBack()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(show_dvdMenu()), &pqOrbiter, SLOT(showMenu()));

        //so does live tv
        //QObject::connect(&w, SIGNAL(clearModel()), simpleEPGmodel, SLOT(empty()),Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(liveTvUpdate(QString)), simpleEPGmodel, SLOT(setProgram(QString)), Qt::QueuedConnection);
        //epg specific

        //storemediaplaylist specific

        /*Device control related*/

        QObject::connect(&w, SIGNAL(resendDeviceCode(int,int)), &pqOrbiter, SLOT(sendAvCommand(int,int)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(osdChanged(bool)), &pqOrbiter, SLOT(displayToggle(bool)),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(setDceVar(int,QString)), &pqOrbiter, SLOT(setVariable(int,QString)), Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter,SIGNAL(routerReloading(QString)), &w, SLOT(reloadHandler()) );
        QObject::connect(&w, SIGNAL(newLightLevel(QString)), &pqOrbiter, SLOT(adjustRoomLights(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(zoomLevelChanged(QString)), &pqOrbiter, SLOT(setZoom(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(aspectRatioChanged(QString)), &pqOrbiter, SLOT(setAspect(QString)), Qt::QueuedConnection);

        QObject::connect(&pqOrbiter, SIGNAL(routerConnectionChanged(bool)), &w, SLOT(setConnectedState(bool)), Qt::QueuedConnection);

        //FIXME: below emits error: QObject::connect: Attempt to bind non-signal orbiterWindow::close()
        //QObject::connect (&w,SIGNAL, &w, SLOT(closeOrbiter()), Qt::DirectConnection);
        QObject::connect(&w, SIGNAL(reloadRouter()), &pqOrbiter, SLOT(quickReload()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(routerReload()), &w, SLOT(reloadHandler()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(replaceDevice()), &w, SLOT(replaceHandler()), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(routerDisconnect()), &w, SLOT(disconnectHandler()),Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(reInitialize()), &pqOrbiter, SLOT(initialize()), Qt::QueuedConnection);
        //   QObject::connect(&w, SIGNAL(deviceNumberChanged(int)), &pqOrbiter, SLOT(setDeviceId(int)));
        QObject::connect(&w, SIGNAL(internalIpChanged(QString)), &orbiterWin, SLOT(setRouterAddress(QString)));

        /*Remote command signal */

        QObject::connect(&pqOrbiter, SIGNAL(dceGuiCommand(int)), &w, SLOT(handleDceGuiCommand(int)), Qt::QueuedConnection);
        dceThread.start();

        //#ifdef Q_OS_LINUX
        //        QProcess p;`
        //        p.start("awk", QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
        //        p.waitForFinished();
        //        QString memory = p.readAllStandardOutput();
        //        system_info.append(QString("; RAM: %1 MB").arg(memory.toLong() / 1024));
        //        p.close();
        //#endif


        if(sRouter_IP!=""){
            qDebug()<< "Command Line override. Using command line settings";

            if(PK_Device == -1){
                PK_Device = w.getDeviceNumber();
            }
            orbiterWin.setDeviceNumber(PK_Device); orbiterWin.setRouterAddress(QString::fromStdString(sRouter_IP));

        }else if(!w.getInternalIp().isEmpty() && w.getDeviceNumber() != -1){
            qDebug() << "No Command line opt set but config file located for device " << w.getDeviceNumber();
            sRouter_IP = w.getInternalIp().toStdString();                                          //use internal ip first
            PK_Device = w.getDeviceNumber();
            orbiterWin.setDeviceNumber(PK_Device);
            orbiterWin.setRouterAddress(w.getInternalIp());
        }
        else{
            qDebug() << "Nothing set, using defaults.";
            //orbiterWin.setDeviceNumber(settings.getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID).toInt());
            orbiterWin.setRouterAddress(w.getInternalIp());
            sRouter_IP = w.getInternalIp().toStdString();
            PK_Device=w.getDeviceNumber();
        }

        QList<QString*> myHosts;
        QString badMatch = orbiterWin.getInternalIp();
        int f = badMatch.lastIndexOf(".");
        qDebug() << badMatch.length() - f ;
        badMatch.remove(f, badMatch.length() - f);

        //        foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        //            QNetworkInterface t =  QNetworkInterface::interfaceFromIndex(0);
        //             pqOrbiter.m_localMac= t.hardwareAddress();
        //             qDebug() << "My Mac is:: " << pqOrbiter.m_localMac;
        //            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)){
        //                if(address.toString().contains(badMatch)){
        //                    qDebug() << "My Ip's" << address.toString() << ":: badMatch==>"<<badMatch;
        //                    w.setLocalAddress(address.toString());
        //                    pqOrbiter.setLocalIp(address.toString());
        //                    w.setHomeNetwork(true);
        //                    break;
        //                }
        //            }

        //        }

        orbiterWin.initView();
#ifdef __ANDROID__
        while(!w.orbiterInit){
            QApplication::processEvents(QEventLoop::AllEvents);
        }
#endif
       // pqOrbiter.qmlSetup(w.getDeviceNumber(), QString::fromStdString(sRouter_IP));
        a.exec();

        //        localLogger.deleteLater();
        //        timecode->deleteLater();
        //        mediaModel->deleteLater();
        //        storedVideoPlaylist->deleteLater();
        //        simpleEPGmodel->deleteLater();
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
