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
#include "USB_UIRT_0038.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

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
			return USB_UIRT_0038::PK_DeviceTemplate_get_static();
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

		USB_UIRT_0038 *pUSB_UIRT_0038 = new USB_UIRT_0038(PK_Device, "localhost",true,false,pRouter);
		if( pUSB_UIRT_0038->m_bQuit_get()|| !pUSB_UIRT_0038->GetConfig() )
		{
			delete pUSB_UIRT_0038;
			return NULL;
		}
		else
		{
			g_pCommand_Impl=pUSB_UIRT_0038;
			g_pDeadlockHandler=Plugin_DeadlockHandler;
			g_pSocketCrashHandler=Plugin_SocketCrashHandler;
		}
		return pUSB_UIRT_0038;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->!
int main(int argc, char* argv[]) 
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout << "USB_UIRT_0038, v." << VERSION << endl
		<< "Visit www.plutohome.com for source code and license information" << endl << endl;

	string sRouter_IP="dcerouter";
	int PK_Device=0;
	string sLogger="stdout";
	string sPort;
	string sAVWHost;
	int iAVWPort = 0;

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
		case 'p':
            bLocalMode = true;
			sPort = argv[++optnum];
			break;
		case 'P':
			iAVWPort = atoi(argv[++optnum]);
			break;
		case 'H':
			sAVWHost = argv[++optnum];
			break;
		default:
			bError=true;
			break;
		};
	}

	if (bError)
	{
		cout << "A Pluto DCE Device.  See www.plutohome.com/dce for details." << endl
			<< "Usage: USB_UIRT_0038 [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
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
			LoggerWrapper::SetInstance(new ServerLogger(PK_Device, USB_UIRT_0038::PK_DeviceTemplate_get_static(), sRouter_IP));
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

	cout << "LocalMode = " << bLocalMode << endl;
	bool bAppError=false;
	bool bReload=false;
	try
	{
		USB_UIRT_0038 *pUSB_UIRT_0038 = new USB_UIRT_0038(PK_Device, sRouter_IP,true,bLocalMode);
		pUSB_UIRT_0038->m_sPort = sPort;
		pUSB_UIRT_0038->m_iAVWPort = iAVWPort;
		pUSB_UIRT_0038->m_sAVWHost = sAVWHost;
		if ( pUSB_UIRT_0038->GetConfig() && 
			 pUSB_UIRT_0038->Connect(pUSB_UIRT_0038->PK_DeviceTemplate_get()) ) 
		{
			g_pCommand_Impl=pUSB_UIRT_0038;
			g_pDeadlockHandler=DeadlockHandler;
			g_pSocketCrashHandler=SocketCrashHandler;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
			if( bLocalMode )
				pUSB_UIRT_0038->RunLocalMode();
			else
			{
				pUSB_UIRT_0038->CreateChildren();
				if( PK_Device==DEVICEID_MESSAGESEND )  // If this is running for the a/v wizard block here so it doesn't exit when the server exits
					while(true) Sleep(10000);
				pthread_join(pUSB_UIRT_0038->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
			}
			g_pDeadlockHandler=NULL;
			g_pSocketCrashHandler=NULL;
		} 
		else 
		{
			bAppError = true;
			if( pUSB_UIRT_0038->m_pEvent && pUSB_UIRT_0038->m_pEvent->m_pClientSocket && pUSB_UIRT_0038->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
			{
				bAppError = false;
				bReload = false;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No Router.  Will abort");
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
		}

		if( pUSB_UIRT_0038->m_bReload )
			bReload=true;

		delete pUSB_UIRT_0038;
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

	if(bAppError)
		return 1;

	if( bReload )
		return 2;
	else
		return 0;
}
//<-dceag-main-e->
