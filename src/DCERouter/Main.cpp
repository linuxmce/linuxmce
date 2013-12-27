/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the GNU Public License, available at:
 http://www.fsf.org/licensing/licenses/gpl.html

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.

 */


 /** @file Main.cpp
Usage: Router [-i installation] [-d device] [-h hostname]

[-u username] [-p password] [-D database] [-P mysql port]

[-t tcp/ip port to listen on] [-f Base Path][-l Logger]

-h hostname  -- address or DNS of database host, default is `dcerouter`

-u username  -- username for database connection

-p password  -- password for database connection, default is `` (empty)

-D database  -- database name

-P port      -- port for database connection, default is 3306

-f base path -- device requests for files will be relative to this path

-l logger    -- where to put the log
 */
/*
 * $Id: Main.cpp,v 1.3 2004/10/16 12:36:39 aaron Exp $
 */
#include "PlutoUtils/CommonIncludes.h"
#include <signal.h>
#include <iostream>
#include "DCERouter.h"
#include "DCE/DCEConfig.h"
#include "DeviceData_Router.h"
#include "../pluto_main/Table_DeviceData.h"
#include "../pluto_main/Table_DesignObj.h"

#include "version.h"

using namespace DCE;

namespace DCE
{
	class Router *g_pRouter;
}

DCEConfig g_DCEConfig;

#define SIGINT_NOTIFY_MESSAGE	"Got SIGINT, exiting\n"
void sig_int(int sig)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, SIGINT_NOTIFY_MESSAGE);

	if(g_pRouter) {
		g_pRouter->Quit();
	}
}

#ifdef LL_DEBUG
void sig_usr1(int sig)
{
	if (g_pRouter != NULL)
		g_pRouter->ShowSockets();
}
#endif

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
int main(int argc, char *argv[])
{
#endif

	system("touch /usr/pluto/var/DCERouter.running");

	g_sBinary = FileUtils::FilenameWithoutPath(argv[0]);
	g_sBinaryPath = FileUtils::BasePath(argv[0]);

		cout<<"Copyright (C) 2004 Pluto, Inc., a Florida Corporation"<<endl
		    <<"www.plutohome.com"<<endl
		    <<"Phone: +1 (877) 758-8648"<<endl
		    <<"This program is distributed according to the terms of the GNU Public License, available at: "<<endl
		    <<"http://www.fsf.org/licensing/licenses/gpl.html "<<endl
		    <<"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "<<endl
		    <<"without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "<<endl
		    <<"See the GNU Public License for more details."<<endl << "---------------" << endl << endl;

	srand((int) time(NULL));

	string DBHost="dcerouter",DBUser="root",DBPassword="",DBName="pluto_main",BasePath="";
	int DBPort=3306,ListenPort=3450,Installation=0,PK_Device=0;
	string sLogger="stdout";

	bool bError=false; // An error parsing the command line
    char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			exit(1);
		}

		c=argv[optnum][1];
        switch (c)
        {
            case 'i':
                Installation = atoi(argv[++optnum]);
                break;
            case 'h':
                DBHost = argv[++optnum];
                break;
            case 'u':
                DBUser = argv[++optnum];
                break;
            case 'p':
                DBPassword = argv[++optnum];
                break;
            case 'D':
                DBName = argv[++optnum];
                break;
            case 'P':
                DBPort = atoi(argv[++optnum]);
                break;
            case 't':
                ListenPort = atoi(argv[++optnum]);
                break;
            case 'd':
                PK_Device = atoi(argv[++optnum]);
                break;
	    case 'f':
		BasePath = argv[++optnum];
		break;
	    case 'l':
		sLogger = argv[++optnum];
		break;
            default:
		bError=true;
		break;
        };
	}

    if ( bError)
    {
//                123456789012345678901234567890123456789012345678901234567890
	cout << "Router, v." << VERSION << endl
                << "Usage: Router [-i installation] [-d device] [-h hostname]"<<endl
		<< "[-u username] [-p password] [-D database] [-P mysql port]"<<endl
		<< "[-t tcp/ip port to listen on] [-f Base Path][-l Logger]" << endl
                << "-h hostname  -- address or DNS of database host, default is `dcerouter`" << endl
                << "-u username  -- username for database connection" << endl
                << "-p password  -- password for database connection, default is `` (empty)" << endl
                << "-D database  -- database name" << endl
                << "-P port      -- port for database connection, default is 3306" << endl
                << "-f base path -- device requests for files will be relative to this path" << endl
		<< "-l logger	 -- where to put the log" << endl;

        exit(1);
    }



	try
	{
		if( sLogger=="null" )
			LoggerWrapper::SetType(LT_LOGGER_NULL);
		else if( sLogger!="stdout" )
			LoggerWrapper::SetType(LT_LOGGER_FILE,sLogger);
	}
	catch(...)
	{
		cerr << "Unable to create logger" << endl;
	}

#ifndef _WIN32
	/* install interrupt handler*/
	signal(SIGINT, sig_int);
#ifdef LL_DEBUG
	signal(SIGUSR1, sig_usr1);
#endif
#endif

	if( BasePath.length()>0 && BasePath[ BasePath.length()-1 ]!='/' )
		BasePath += "/";

	g_pRouter = new Router(PK_Device,Installation,BasePath,DBHost,DBUser,DBPassword,DBName,DBPort,ListenPort);
	bool bResult = g_pRouter->Run();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoServer: Exited with %d",(int) bResult);

#ifdef _WIN32
	WSACleanup();
#endif

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to delete router");
	delete g_pRouter;
	g_pRouter = NULL;

	MutexTracking::Delete();

	system("rm /usr/pluto/var/DCERouter.running");
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlutoServer: terminating now with %d",(int) bResult);

	if( bResult )
		return 2;
	else
		return 0;
}
