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
#include "DCE_Template.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCERouter.h"

#include "../include/version.cpp"

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
			return DCE_Template::PK_DeviceTemplate_get_static();
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

		DCE_Template *pDCE_Template = new DCE_Template(PK_Device, "localhost",true,false,pRouter);
		if( pDCE_Template->m_bQuit_get()|| !pDCE_Template->GetConfig() )
		{
			delete pDCE_Template;
			return NULL;
		}
		else
		{
			g_pCommand_Impl=pDCE_Template;
			g_pDeadlockHandler=Plugin_DeadlockHandler;
			g_pSocketCrashHandler=Plugin_SocketCrashHandler;
		}
		return pDCE_Template;
	}
}
//<-dceag-plug-e->

//<-dceag-main-b->
int main(int argc, char* argv[]) 
{
	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

	cout << "DCE_Template, v." << VERSION << endl
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
			<< "Usage: DCE_Template [-r Router's IP] [-d My Device ID] [-l dcerouter|stdout|null|filename]" << endl
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
			LoggerWrapper::SetInstance(new ServerLogger(PK_Device, DCE_Template::PK_DeviceTemplate_get_static(), sRouter_IP));
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
		DCE_Template *pDCE_Template = new DCE_Template(PK_Device, sRouter_IP,true,bLocalMode);
		if ( pDCE_Template->GetConfig() && pDCE_Template->Connect(pDCE_Template->PK_DeviceTemplate_get()) ) 
		{
			g_pCommand_Impl=pDCE_Template;
			g_pDeadlockHandler=DeadlockHandler;
			g_pSocketCrashHandler=SocketCrashHandler;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
			pDCE_Template->CreateChildren();
			if( bLocalMode )
				pDCE_Template->RunLocalMode();
			else
			{
				if(pDCE_Template->m_RequestHandlerThread)
					pthread_join(pDCE_Template->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
			}
			g_pDeadlockHandler=NULL;
			g_pSocketCrashHandler=NULL;
		} 
		else 
		{
			bAppError = true;
			if( pDCE_Template->m_pEvent && pDCE_Template->m_pEvent->m_pClientSocket && pDCE_Template->m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
			{
				bAppError = false;
				bReload = false;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No Router.  Will abort");
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
		}

		if( pDCE_Template->m_bReload )
			bReload=true;

		delete pDCE_Template;
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
