/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "PlutoDHCP.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;



int main(int argc, char *argv[])
{
#ifndef WIN32
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/var/log/pluto/PlutoDHCP.log");
#else
	LoggerWrapper::SetType(LT_LOGGER_FILE,"c:/temp/PlutoDHCP.log");
#endif

	int iPK_Device=0;
	bool bAssignIP=false,bError=false;

	for(int optnum=1;optnum<argc;++optnum)
	{
		char c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			dceConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			dceConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			dceConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			dceConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			dceConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'd':
			iPK_Device = atoi(argv[++optnum]);
			break;
		case 'i':
			dceConfig.m_iPK_Installation = atoi(argv[++optnum]);
			break;
		case 'a':
			bAssignIP=true;
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		cout << "PlutoDHCP, v." << VERSION << endl
			<< "Usage: PlutoDHCP [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-d PK_Device] [-i PK_Installation] [-a]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "If -a is not specified, it will echo the dchpd.conf" << endl
			<< "If -a is specified it will echo an available IP" << endl
			<< "Note use -d with -a to assign that IP to a device" << endl;

		exit(1);
	}

	string args;
	for(int optnum=1;optnum<argc;++optnum)
		args += string(argv[optnum]) + " ";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Called with: %s",args.c_str());

	PlutoDHCP PlutoDHCP(dceConfig.m_iPK_Installation,dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort);
	if( !PlutoDHCP.IsConnected() )
		exit(1);

	PlutoDHCP.ReadAllIPs();

	if( bAssignIP )
	{
		if( !iPK_Device )
		{
			cout << "You must specify a device ID" << endl;
			exit(1);
		}
		else
		{
			string sIPAddress = PlutoDHCP.AssignIP(iPK_Device);
			if( sIPAddress.size() )
			{
				cout << sIPAddress << endl;
				exit(0);
			}
			else
			{
				cout << "Error Assigning IP Address" << endl;
				exit(1);
			}
		}
	}
	else
	{
		cout << PlutoDHCP.GetDHCPConfig() << endl;
	}

	return 0;
}
