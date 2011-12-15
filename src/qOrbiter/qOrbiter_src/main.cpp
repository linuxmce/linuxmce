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
#include <QApplication>

#include <qOrbiter/qOrbiter.h>
#include <orbiterwindow.h>

#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
//#include "DCERouter.h"
#include <QDebug>

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/



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
/*
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
*/
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
}/*


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
}*/
//<-dceag-plug-e->


//<-dceag-main-b->


int main(int argc, char* argv[])
{

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
            qDebug() << "From the command line input:" << PK_Device;
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
#ifndef for_harmattan
        QApplication::setGraphicsSystem("raster");
#endif
        QApplication  a(argc, argv);

        orbiterWindow orbiterWin;
        QThread *processThread = new QThread();
        orbiterWin.moveToThread(processThread);
        processThread->start();

        QApplication::processEvents(QEventLoop::AllEvents);
        qorbiterManager  w(PK_Device,QString::fromStdString(sRouter_IP), &orbiterWin.mainView);
        QObject::connect(&w, SIGNAL(loadingMessage(QString)), &orbiterWin,SLOT(setMessage(QString)));
        QObject::connect(&orbiterWin,SIGNAL(setupLmce(QString,QString)), &w, SLOT(qmlSetupLmce(QString,QString)));

        if(w.setupLmce(PK_Device, sRouter_IP, true, false))
        {

        }
        else
        {
            bAppError = true;

            if(w.pqOrbiter->m_pEvent && w.pqOrbiter->m_pEvent->m_pClientSocket && w.pqOrbiter->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
            {
                bAppError = false;
                bReload = false;
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "main loop No Router.  Will abort");
               w.setDceResponse(" main loop- No Router, Aborting");
            }
            else
            {
                bAppError = true;
                if( w.pqOrbiter->m_pEvent&& w.pqOrbiter->m_pEvent->m_pClientSocket && w.pqOrbiter->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_BadDevice)
                {
                    bAppError = false;
                    bReload = false;
                    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Bad Device  Will abort");
                    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
                    //orbiterWin.mainView.setSource(QUrl("qrc:desktop/SetupNewOrbiter.qml"));
                    orbiterWin.setMessage("Connect Failed with Bad Device");
                }
            }


        }

    a.exec();

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

    if( bAppError )
        return 1;

    if( bReload )
        return 2;
    else
        return 0;
}
//<-dceag-main-e->
