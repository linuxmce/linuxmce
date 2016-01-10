/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-incl-b->
#include "Linuxmce_NVR.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"
#include <QCoreApplication>
#include "managerClasses/NvrManager.h"
#include "cameraClasses/abstractpicamera.h"
#include "cameraClasses/abstractcameraevent.h"
// include the main LMCE version file
#include "version.h"

using namespace DCE;


//<-dceag-main-b->
int main(int argc, char* argv[]) 
{
    QCoreApplication a(argc, argv);
     int i = qRegisterMetaType< NvrCameraBase*>("NvrCameraBase*");
   // qRegisterMetaType<CameraEvent>("CameraEvent");
    g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
    g_sBinaryPath = FileUtils::BasePath(argv[0]);

    cout << "Linuxmce_NVR, v." << VERSION << endl
         << "Visit www.plutohome.com for source code and license information" << endl << endl;

    string sRouter_IP="dcerouter";
    int PK_Device=0;
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
             << "Usage: Linuxmce_NVR [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
             << "-r -- the IP address of the DCE Router  Defaults to 'dcerouter'." << endl
             << "-d -- This device's ID number.  If not specified, it will be requested from the router based on our IP address." << endl
             << "-l -- Where to save the log files.  Specify 'dcerouter' to have the messages logged to the DCE Router.  Defaults to stdout." << endl;
        exit(1);
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
            LoggerWrapper::SetInstance(new ServerLogger(PK_Device, Linuxmce_NVR::PK_DeviceTemplate_get_static(), sRouter_IP));
        else if( sLogger=="null" )
            LoggerWrapper::SetType(LT_LOGGER_NULL);
        else if( sLogger!="stdout" )
            LoggerWrapper::SetType(LT_LOGGER_FILE,sLogger);

    }
    catch(...)
    {
        cerr << "Unable to create logger" << endl;
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device: %d starting.  Connecting to: %s",PK_Device,sRouter_IP.c_str());

    bool bAppError=false;
    bool bReload=false;
    try
    {
        Linuxmce_NVR *pLinuxmce_NVR = new Linuxmce_NVR(PK_Device, sRouter_IP,true,bLocalMode);


        if ( pLinuxmce_NVR->GetConfig() && pLinuxmce_NVR->Connect(pLinuxmce_NVR->PK_DeviceTemplate_get()) )
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
            pLinuxmce_NVR->CreateChildren();
          //  return a.exec();

            if( bLocalMode ) {
                pLinuxmce_NVR->RunLocalMode();
                return a.exec();
            }

            else
            {
                return a.exec();
                if(pLinuxmce_NVR->m_RequestHandlerThread)
                    pthread_join(pLinuxmce_NVR->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
            }


        }
        else
        {
            bAppError = true;
            if( pLinuxmce_NVR->m_pEvent && pLinuxmce_NVR->m_pEvent->m_pClientSocket && pLinuxmce_NVR->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
            {
                bAppError = false;
                bReload = false;
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No Router.  Will abort");
            }
            else
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
        }

        if( pLinuxmce_NVR->m_bReload )
            bReload=true;

        delete pLinuxmce_NVR;
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
