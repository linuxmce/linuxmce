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
#include "QQmlApplicationEngine"
#include <QtCore/QThread>
#include <contextobjects/settinginterface.h>

#else
#include <QApplication>
#include <QtDeclarative/QDeclarativeEngine>
#endif

#include "logger/qorbiterlogger.h"
#include "datamodels/listModel.h"

#include <DCECommand.h>
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
#include "../version.h"

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
Q_IMPORT_PLUGIN(AudioVisualPlugin)
Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
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
    //qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_DceScreenSaverPlugin().instance())->registerTypes("DceScreenSaver");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_AudioVisualPlugin().instance())->registerTypes("AudioVisual");
#endif

    QCoreApplication::setApplicationName("QOrbiter");
    QCoreApplication::setOrganizationDomain("org.linuxmce.QOrbiter");
    QCoreApplication::setOrganizationName("LinuxMCE");

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
    QString overrideDir="";
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
            screenSize=0;
            break;
        case 'o':
            //used for md type devices. a work around at the moment
            screen="md-fullscreen";
            screenSize=-1;
            deviceType=0;
            break;
        case 'z':
            screenSize=atoi(argv[++optnum]);
            break;
        case 'x':
            overrideDir= QString::fromStdString( argv[++optnum]);
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
             << "-o --Switch for frameless MD and desktops. " << endl
             << "-s --Switch for fullscreen. " << endl
             << "-z --Screen size setting for testing 4 - small \n 7 - medium\n 10 - large " << endl
             << "-x --Point to custom skins directory. " << endl;

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
        //qDebug() << qt_static_plugin_DceScreenSaverPlugin().metaData();
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
        bool isOsd = screen=="md-fullscreen"; //qOrbiter::PK_DeviceTemplate_get_static()==DEVICETEMPLATE_OnScreen_qOrbiter_CONST;
        if(isOsd){
            qDebug() << "Starting qMd";
            fs=true;
            fm=true;
        } else {
            qDebug() << "Starting client qorbiter " << qOrbiter::PK_DeviceTemplate_get_static();
        }
        if(screen =="fullscreen"){
            fs=true;
            fm=true;
        }

        /*! Command impl based routine in here to execute needed dt determinations? */
        /*! Only required for x11 (right?), should consider android dt creation */
        /*! See line 4702 for idea on how to get data from router to execute selection */
        /*! UI will need to be created to allow the device to make the selections before the device tries to connect */

        QThread dceThread;

        QString badMatch = settings.getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Router).toString();
        int f = badMatch.lastIndexOf(".");
        qDebug() << badMatch.length() - f ;
        badMatch.remove(f, badMatch.length() - f);

        bool isHomeNetwork=false;

        foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces()) {
            qDebug() << "Network Interface Scan-----------------------------------------------------------------";
            qDebug() << QString("Interface : %1").arg(iface.humanReadableName());
            qDebug() << QString("Ip address:: %1").arg(iface.addressEntries().size() > 0 ? iface.addressEntries().at(0).ip().toString() : "<none>");

            qDebug() << QString("Mac Address:: %1").arg(iface.hardwareAddress());
            //pqOrbiter.m_localMac= t.hardwareAddress();
            //                     qDebug() << "My Mac is:: " << t.hardwareAddress();
            if(iface.addressEntries().size() > 0 && iface.addressEntries().at(0).ip().toString().contains(badMatch)){
                qDebug() << "!!!!!!!!!!!!!!!!!!!USING HOME NETWORK!!!!!!!!!!!!!";
                isHomeNetwork = true;
                // pqOrbiter.m_sMacAddress=iface.hardwareAddress().toStdString();
                // qDebug() << QString("Mac Address Set to %1").arg(iface.hardwareAddress());
                break;
            }

        }

        if(!isHomeNetwork)
            sRouter_IP = settings.getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_ExternalHostname).toString().toStdString();

        QString name = settings.getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_DeviceName).toString();

        qOrbiter pqOrbiter(name, PK_Device, sRouter_IP,true,bLocalMode );
        pqOrbiter.setOsd(isOsd);

        qmlRegisterType<FloorplanDevice>("org.linuxmce.floorplans",1,0,"FloorplanDevice");
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
        qRegisterMetaType<QHash<int, QVariant> >("QHash<int, QVariant>");
        qRegisterMetaType<DCE::PreformedCommand>("DCE::PreformedCommand");
        qRegisterMetaType<QHash<int, string*> >("QHash<int, string*>");
        qmlRegisterType<GenericFlatListModel>();
        qRegisterMetaType<QList<SystemFontItem*>>();
        qmlRegisterType<SystemFontItem>("org.linuxmce.fonts", 1,0, "SystemFontItem");

        QQmlApplicationEngine engine;


        orbiterWindow orbiterWin(PK_Device, sRouter_IP, fs, fm, screenSize, &engine, isOsd );

        engine.rootContext()->setContextProperty("settings", &settings);
        engine.rootContext()->setContextProperty("orbiterVersion", QString::fromLocal8Bit(VERSION));
        engine.rootContext()->setContextProperty("buildDate", QString::fromLocal8Bit(g_szCompile_Date));

#ifdef __ANDROID__
        engine.rootContext()->setContextProperty("androidSystem", &androidHelper);

#endif
        orbiterWin.setMessage("Setting up Lmce");

#ifndef ANDROID

        qorbiterManager  w(&pqOrbiter, &orbiterWin.mainView, &engine, screenSize, &settings, overrideDir, isOsd);
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
        qorbiterManager w(&pqOrbiter, &orbiterWin.mainView, &engine, &androidHelper, &settings, overrideDir);
        w.setUsingExternal(isHomeNetwork);
        engine.rootContext()->setContextProperty("androidSystem", &androidHelper);
#endif
        engine.rootContext()->setContextProperty("manager", &w);

#if defined(Q_OS_IOS)
        w.setMobileStorage(localLogger.logLocation);
#endif

        AbstractImageProvider* modelimageprovider = new AbstractImageProvider(&w);
        engine.addImageProvider("listprovider", modelimageprovider);
        pqOrbiter.moveToThread(&dceThread);
        QObject::connect(&dceThread, SIGNAL(started()), &pqOrbiter, SLOT(beginSetup()));
        //epg listmodel, no imageprovider as of yet

        // ListModel *mediaModel = new ListModel(new gridItem);
        qDebug() << "!!!!!!!!!!!BREAK!!!!!!!!!!!!!!!!!!";
        TimeCodeManager *timecode = new TimeCodeManager();

        engine.rootContext()->setContextProperty("logger", &localLogger);
        engine.rootContext()->setContextProperty("dceTimecode", timecode);
        engine.rootContext()->setContextProperty("opengl", glpresent);
        // orbiterWin.mainView.rootContext()->setContextProperty("dataModel", mediaModel);

        // connnect local logger
        QObject::connect(&pqOrbiter, SIGNAL(commandResponseChanged(QString)), &localLogger, SLOT(logCommandMessage(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(mediaMessage(QString)), &localLogger, SLOT(logGuiMessage(QString)));

#ifdef QT4_8
        QObject::connect(orbiterWin.mainView.engine(), SIGNAL(warnings(QList<QDeclarativeError>)), &localLogger, SLOT(logQmlErrors(QList<QDeclarativeError>)));
#elif QT5
        QObject::connect(&engine, SIGNAL(warnings(QList<QQmlError>)), &localLogger, SLOT(logQmlErrors(QList<QQmlError>)));
#endif
        QObject::connect(&w, SIGNAL(skinMessage(QString)), &localLogger, SLOT(logSkinMessage(QString)));
        QObject::connect(&w, SIGNAL(qtMessage(QString)) , &localLogger, SLOT(logQtMessage(QString)));
        QObject::connect(&pqOrbiter, SIGNAL(newDceMessage(QString)), &localLogger, SLOT(logCommandMessage(QString)),Qt::QueuedConnection);
#ifdef ANDROID
        QObject::connect(&w, SIGNAL(mobileStorageChanged(QString)), &localLogger, SLOT(setLogLocation(QString)));
#endif

        //connect font stuff because qApp->setFont() crashes elsewhere

        QObject::connect(w.m_fontsHelper, &FontsHelper::currentFontChanged, [&a, &w](QString newFont){
            qDebug() << Q_FUNC_INFO << newFont;
            a.setFont(QFont(newFont));
          w.qmlReload();
        });

        //shutdown signals order is important to prevent crashes on close

        //all 'exit' functions point to qOrbiterManager::closeOrbiter, which closes the window. This emits a signal to stop tell the dce object to cleanly exit.
        QObject::connect(&a, SIGNAL(lastWindowClosed()), &pqOrbiter, SLOT(deinitialize()), Qt::QueuedConnection);
        //When dce object emits destroyed (handy qt feature!) it then tells its parent thread to stop. We wait for that thread to properly shut down, then exit the event loop allows other automatic destructors to run
        QObject::connect(&pqOrbiter, SIGNAL(destroyed(QObject*)), &dceThread, SLOT(quit()));
        //Thread finished, a, the event loop quits and bobs your uncle.
        QObject::connect(&dceThread, SIGNAL(finished()), &a, SLOT(quit()));

        // QObject::connect(&w, SIGNAL(destroyed()), &pqOrbiter, SLOT(deinitialize()), Qt::QueuedConnection);
        //  QObject::connect(&pqOrbiter, SIGNAL(closeOrbiter()), &w, SLOT(closeOrbiter()));
        //  QObject::connect(&dceThread, SIGNAL(finished()), &dceThread, SLOT(deleteLater()));
        //  QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
        // QObject::connect(&dceThread, SIGNAL(finished()),&a, SLOT(quit()));


        // Generic DCE command sending signal/slots
        QObject::connect(&w, SIGNAL(sendDceCommand(DCE::PreformedCommand)), &pqOrbiter, SLOT(handleDceCommand(DCE::PreformedCommand)), Qt::QueuedConnection); //this somehow still blocks btw. this is why i had a painful but explict bridge. the must be a middle ground. possibly look at posting the event instead of connections.
        QObject::connect(&w, SIGNAL(sendDceCommandResponse(DCECommand*)), &pqOrbiter, SLOT(handleDceCommandResp(DCECommand*)), Qt::QueuedConnection); //this somehow still blocks btw. this is why i had a painful but explict bridge. the must be a middle ground. possibly look at posting the event instead of connections.
        //  NO! QObject::connect(&w, SIGNAL(sendDceCommandResponse(DCE::PreformedCommand&, string*)), &pqOrbiter, SLOT(sendDCECommandResponse(DCE::PreformedCommand&, string*)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(routerConnectionChanged(bool)), &w, SLOT(setConnectedState(bool)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, &qOrbiter::routerFound, &orbiterWin, &orbiterWindow::setConnectionState, Qt::QueuedConnection );
        // QObject::connect (&w, SIGNAL(orbiterClosing()), &dceThread, SLOT(quit()),Qt::QueuedConnection);
        //  QObject::connect (&w, SIGNAL(orbiterClosing()), mediaThread, SLOT(quit()),Qt::QueuedConnection);
        //  QObject::connect (&w, SIGNAL(orbiterClosing()), epgThread, SLOT(quit()),Qt::QueuedConnection);
        //  QObject::connect(&dceThread, SIGNAL(finished()), &a, SLOT(quit()),Qt::QueuedConnection);

        //tv epg signals
        //QObject::connect(&pqOrbiter, SIGNAL(addChannel(EPGItemClass*)), simpleEPGmodel, SLOT(appendRow(EPGItemClass*)), Qt::QueuedConnection );

        /*Misc*/

        QObject::connect(&pqOrbiter, SIGNAL(setText(QString,QString,int)), &w, SLOT(setText(QString,QString,int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(connectionLost()), &w, SLOT(disconnectHandler()), Qt::QueuedConnection);
        //timecodemanager signals / slots
        QObject::connect(&pqOrbiter, SIGNAL(updateTimeCode(QString,int)), timecode, SLOT(start(QString,int)), Qt::QueuedConnection);
        QObject::connect(&pqOrbiter, SIGNAL(stopTimeCode()), timecode, SLOT(stop()), Qt::QueuedConnection);
        //setup
        QObject::connect(&w, SIGNAL(usingExternalChanged(bool)), &orbiterWin, SLOT(setUsingExternal(bool)));
        QObject::connect(&w, SIGNAL(internalIpChanged(QString)), &orbiterWin, SLOT(setInternalIp(QString)));
        QObject::connect(&w, SIGNAL(externalIpChanged(QString)), &orbiterWin, SLOT(setExternalIp(QString)));
        QObject::connect(&w, SIGNAL(orbiterInitialized()), &orbiterWin, SLOT(setOrbiterInitialized()));
        QObject::connect(&pqOrbiter, SIGNAL(deviceInvalid()), &orbiterWin, SLOT(prepareExistingOrbiters()), Qt::QueuedConnection);
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
        QObject::connect(&pqOrbiter, &qOrbiter::creationComplete, &orbiterWin, &orbiterWindow::creationComplete, Qt::QueuedConnection);
        QObject::connect(&pqOrbiter,SIGNAL(promptResponse(int,QList<PromptData*>*)), &orbiterWin, SLOT(displayPromptResponse(int, QList<PromptData*>* )), Qt::QueuedConnection);
        QObject::connect(&orbiterWin, SIGNAL(newOrbiterData(int , int , int , int , int , int, QString )), &pqOrbiter, SLOT(setOrbiterSetupVars(int,int,int,int,int,int, QString)), Qt::QueuedConnection);
        QObject::connect(&orbiterWin, SIGNAL(beginSetupNewOrbiter()), &pqOrbiter,SLOT(populateSetupInformation()), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(loadingMessage(QString)), &orbiterWin,SLOT(setMessage(QString)), Qt::QueuedConnection);
        QObject::connect(&w, SIGNAL(internalIpChanged(QString)), &orbiterWin, SLOT(setRouterAddress(QString)));
        //QObject::connect(&pqOrbiter,SIGNAL(objectDataUpdate( char,int)), w.nowPlayingButton, SLOT(setDroidImageData(char,int)),Qt::QueuedConnection);
        //  QObject::connect(simpleEPGmodel, SIGNAL(channelNumberChanged(QString)), w.nowPlayingButton, SLOT(setChannel(QString)), Qt::QueuedConnection);
        // QObject::connect(simpleEPGmodel, SIGNAL(programChanged(QString)), w.nowPlayingButton, SLOT(setProgram(QString)), Qt::QueuedConnection);
        // QObject::connect(simpleEPGmodel, SIGNAL(networkChanged(QString)), w.nowPlayingButton, SLOT(setChannelID(QString)), Qt::QueuedConnection);
        QObject::connect(timecode, SIGNAL(seekToTime(QString)), &pqOrbiter, SLOT(JogStream(QString)), Qt::QueuedConnection );

        // myth  now playing requires special handling
        //QObject::connect(&pqOrbiter, SIGNAL(mythTvUpdate(QString)), simpleEPGmodel, SLOT(setMythProgram(QString)),Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter, SIGNAL(epgDone()), simpleEPGmodel, SLOT(updatePosition()),Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(livetvDone()), simpleEPGmodel, SLOT(updateLivePosition()), Qt::QueuedConnection);
        // QObject::connect(simpleEPGmodel, SIGNAL(requestEpg()), &pqOrbiter, SLOT(requestLiveTvPlaylist()), Qt::QueuedConnection);
        //QObject::connect(&pqOrbiter, SIGNAL(clearTVplaylist()), simpleEPGmodel, SLOT(populate()), Qt::QueuedConnection);
        //QObject::connect (&w, SIGNAL(liveTVrequest()), simpleEPGmodel, SLOT(populate()),Qt::DirectConnection);
        // QObject::connect (&w, SIGNAL(managerPlaylistRequest()), storedVideoPlaylist,SLOT(populate()),Qt::QueuedConnection );
        //so does live tv
        //QObject::connect(&w, SIGNAL(clearModel()), simpleEPGmodel, SLOT(empty()),Qt::QueuedConnection);
        // QObject::connect(&pqOrbiter, SIGNAL(liveTvUpdate(QString)), simpleEPGmodel, SLOT(setProgram(QString)), Qt::QueuedConnection);
        //epg specific

        dceThread.start();

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
            pqOrbiter.qmlSetup(PK_Device,w.getInternalIp());
        }
        else{
            qDebug() << "Nothing set, using defaults.";
            //orbiterWin.setDeviceNumber(settings.getOption(SettingsInterfaceType::Settings_Network, SettingsKeyType::Setting_Network_Device_ID).toInt());
            orbiterWin.setRouterAddress(w.getInternalIp());
            sRouter_IP = w.getInternalIp().toStdString();
            PK_Device=w.getDeviceNumber();
        }


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
