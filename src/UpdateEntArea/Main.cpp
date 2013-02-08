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
#include "UpdateEntArea.h"

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
using namespace DefaultScenarios;
DCEConfig dceConfig;



extern bool g_bChangedScenarios;

int main(int argc, char *argv[])
{
	LoggerWrapper::SetType(LT_LOGGER_FILE,"/var/log/pluto/UpdateEntArea.log");

	bool bError=false;
	int iPK_EntertainArea=0;
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		c=argv[optnum][1];
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
		case 'i':
			dceConfig.m_iPK_Installation = atoi(argv[++optnum]);
			break;
		case 'e':
			iPK_EntertainArea = atoi(argv[++optnum]);
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		cout << "UpdateEntArea, v." << VERSION << endl
			<< "Usage: UpdateEntArea [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-i PK_Installation] [-e PK_EntertainArea]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "To create a Media scenario, pass in -e" << endl;

		exit(1);
	}

	UpdateEntArea updateEntArea;
	if( !updateEntArea.Connect(dceConfig.m_iPK_Installation,dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_sDBName,dceConfig.m_iDBPort) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot connect to database");
		return 1;
	}

	updateEntArea.FixMissingAutoCreateDevices();
	updateEntArea.GetMediaAndRooms();
	updateEntArea.SetEAInRooms();

	if( iPK_EntertainArea )
	{
		Row_EntertainArea *pRow_EntertainArea = updateEntArea.m_pDatabase_pluto_main->EntertainArea_get()->GetRow(iPK_EntertainArea);
		if( !pRow_EntertainArea )
		{
			cerr << "Bad entertainment area" << endl;
			return 1;
		}

		updateEntArea.AddDefaultMediaScenarios(pRow_EntertainArea);
	}
	else
		updateEntArea.AddDefaultScenarios();

	updateEntArea.UpdateOrbiterShortcuts();

	if( g_bChangedScenarios )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Scenarios were changed.  Updating Orbiters");
		string sSQL = "UPDATE Orbiter SET ScenariosFloorplans=NULL";
		updateEntArea.m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	}
	return 0;
}

