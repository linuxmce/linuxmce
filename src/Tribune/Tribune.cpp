#include "PlutoUtils/CommonIncludes.h"

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Database.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"
#include "RA/RAServer.h"
#include "R_GetLineups.h"
#include "R_GetChannels.h"
#include "RA/RA_Processor.h"
#include "ClientFunctions.h"
#include "ServerManagement.h"
// #include "inotify/FileNotifier.h"

#include <iostream>
#include <sstream>
#include <fstream>

//#include <cstdlib>

#include <string>
#include <vector>
#include <map>
#include <list>

using namespace std;
using namespace DCE;
using namespace Tribune;


namespace Tribune
{
	GlobalConfig g_GlobalConfig;
}

string GetCommand( )
{

	cout << "Use Tribune -? for command line options." << endl << endl
		<< "What would you like to do?" << endl
		<< "------Server-side functions------" << endl
		<< "1.	Start listening for incoming connections from clients ( listen )" << endl
		<< "------Client-side functions------" << endl
		<< "A.	Choose a lineup and its channels providing the postal code" << endl
		<< "B.	Daily update" << endl
		<< endl
		<< "Q. Quit" << endl;

	string s;
	cin >> s;

	if( s=="1" )
		return "listen";
	else if( s=="a" || s=="A" )
		return "lineup";
	else if( s=="b" || s=="B" )
		return "dailyupdate";
	else if( s=="q" || s=="Q" )
		exit( 0 );

	return "";
}

void OnCreateFiles(list<string> &listFiles)
{
	
}

void OnDeleteFiles(list<string> &listFiles)
{
	g_GlobalConfig.m_bImportTable=true;
}

//void* FileNotifier_Thread(void *p){
// 
// 	FileNotifier *fn = (FileNotifier *) p;
// 	if( fn != NULL )
// 	{
// 		fn->Run();
// 	}
// 	
// 	return NULL;
// }


int main(int argc, char *argv[]){

	cout << " Copyright (C) 2004 Pluto, Inc., a Florida Corporation " << endl
	    	<< " www.plutohome.com " << endl
	    	<< " Phone: +1 (877) 758-8648 " << endl
			<< "This program is distributed according to the terms " << endl
			<< "of the Pluto Public License, available at:" << endl
			<< " http://plutohome.com/index.php?section=public_license" << endl
			<< "This program is distributed in the hope that it will be useful,"  <<endl
			<< "but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY"<<endl
			<< "or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details."<<endl
			<< endl << "-------" << endl << endl;


	cout << "Database host:" << g_GlobalConfig.m_sDBHost << " user:" << g_GlobalConfig.m_sDBUser
			<< " pass:" << g_GlobalConfig.m_sDBPassword << " name:" << g_GlobalConfig.m_sDBName << " port:" << g_GlobalConfig.m_iDBPort << endl;

	Database database( g_GlobalConfig.m_sDBHost, g_GlobalConfig.m_sDBUser, g_GlobalConfig.m_sDBPassword, g_GlobalConfig.m_sDBName, g_GlobalConfig.m_iDBPort );
	if( !database.m_bConnected )
	{
		cerr << "***ERROR*** Cannot connect to database." << endl;
		cout << "Please modify the configuration settings and restart sqlCVS." << endl;
		exit(1);
	}
	g_GlobalConfig.m_pDatabase=&database;

	bool bError=false; /** An error parsing the command line */

	try
	{
		char c;
		for( int optnum=1;optnum<argc;++optnum )
		{
			if( argv[optnum][0]!='-' )
			{
				g_GlobalConfig.m_sCommand = argv[optnum];
				if( optnum<argc-1 )
				{
					cerr << "If " << argv[optnum] << " is the command it must be at the end of the line" << endl;
					bError=true;	/** The command must be the last thing on the line */
				}
				break;
			}

			c=argv[optnum][1];
			switch ( c )
			{
			case 'H':
				g_GlobalConfig.m_sTribuneHost = argv[++optnum];
				break;
			case 'P':
				g_GlobalConfig.m_iTribunePort = atoi( argv[++optnum] );
				break;
			case '?':
				bError=true;
				break;
			default:
				cerr << "Unknown option: " << argv[optnum] << endl;
				bError=true;
				break;
			};
		}
	}
	catch(...)
	{
		bError=true;
	}

	if ( bError )
	{
		cout << "Tribune"<<endl
			<< "Usage: Tribune [-H Tribune hostname] [-P Tribune port] " << endl
			<< "-P port    	   -- port for tribune connection, " << endl
			<< "			default is 9003" << endl
			<< "-H hostname    -- address or DNS of Tribune host," << endl
			<< "			default is `localhost`" << endl;
		exit( 1 );
	}


		while( g_GlobalConfig.m_sCommand.length( )==0 )
		{
			g_GlobalConfig.m_sCommand=GetCommand( );
		}
		/** Server Mode */
		if( g_GlobalConfig.m_sCommand=="listen" )
		{
			/** there are 3 threads: */

			/** thread that watch Tribune dir, when daily import is done global variable m_bImportTable
			is set true (there is still some code to write here)*/
// 			FileNotifier* pfileNotifier = new FileNotifier();
// 			pfileNotifier->RegisterCallbacks(OnCreateFiles,OnDeleteFiles);
//   			pfileNotifier->Watch("/var/Tribune");
// 			pthread_t trf;
//                      pthread_create(&trf, NULL, FileNotifier_Thread, (void*)pfileNotifier);

			/**thread that reset the server maps after daily import */
			g_GlobalConfig.m_bImportTable = true;	

			ServerManagement* pSM = ServerManagement::getInstance();
			pthread_t tr;
			pthread_create(&tr, NULL, ServerManagement::ServerManagement_Thread, (void*)pSM);

			/**thread that is the server itself */
			RAServer *pServer = new RAServer( );
			pServer->Initialize( );
			pServer->StartListening( g_GlobalConfig.m_iTribunePort );
			pServer->Run( );
			delete pServer;

		}
		/** Client Mode*/

		/** client run this to choose his lineup inserting a zip code and display the channels*/
		else if (g_GlobalConfig.m_sCommand=="lineup")
		{
			string command;
			int zipcode;
			DCE::Socket *pSocket=NULL;

			ClientFunctions *clientFunct = new ClientFunctions(&pSocket);
		
			while (command.length( )==0){
				cout << endl << "Please provide a zip code to list the lineups: ";
				cin >> command;
			}

			zipcode = atoi(command.c_str());
			command = "";

			map <int,pair<string,string> > mapIndexLineupKey_To_Name;
			clientFunct->GetLineups(zipcode, mapIndexLineupKey_To_Name);

			if (mapIndexLineupKey_To_Name.empty()  ){
				cout<<endl<<"There are no lineups for the zip code provided"<<endl;
				exit(0);
			}

			map<int,pair<string,string> >::iterator iter;

			while (command.length( )==0){

				cout << endl << endl << "-------------------------" << endl;
				cout << "Choose a lineup from list" << endl;
				cout << "-------------------------" << endl << endl;

				for(iter = mapIndexLineupKey_To_Name.begin(); iter != mapIndexLineupKey_To_Name.end(); iter++ ) {
					cout << "[ "<<iter->first << " ].  " << iter->second.second << endl;
				}

				cout << "[ q ].  " << "Quit" << endl;				

				cin >> command;

				if( command=="q" || command=="Q" ){
					exit( 0 );
				}

				int key = atoi(command.c_str());

				iter = mapIndexLineupKey_To_Name.find(key);
		
				if( iter == mapIndexLineupKey_To_Name.end() ) {
						
					cout << "Please choose a valid key!";
					command = "";

				} else if (iter != mapIndexLineupKey_To_Name.end() ) {

					clientFunct->DeleteClientLineup();

					clientFunct->SetClientLineup(iter->second.first,iter->second.second);
					
					cout << endl << endl << "-------------------------" << endl;
					cout << "The list of channels (Please wait...)" << endl;
					cout << "-------------------------" << endl << endl;
					map <string,string> mapChannelKey_To_Name;
					int key = atoi((iter->second.first).c_str());
					string extra_cond = clientFunct->GetBlackListChannels();
					clientFunct->GetChannels(key, extra_cond, mapChannelKey_To_Name);
					map<string,string>::iterator it;
					for(it = mapChannelKey_To_Name.begin(); it != mapChannelKey_To_Name.end(); it++){
						cout<< it->second <<endl;
					}
					cout<<endl;
				}
			}

 		}
		/** this will run daily to update the client database */
		else if (g_GlobalConfig.m_sCommand=="dailyupdate"){

			DCE::Socket *pSocket=NULL;
			ClientFunctions *clientFunct = new ClientFunctions(&pSocket);
			string blacklist = clientFunct->GetBlackListChannels();
			string lineup = clientFunct->GetClientLineup();
			
			/** populate the client maps*/

			map<string,string> mapProgramRecord;
			if (! MapManagement::GetProgramRecordMap( mapProgramRecord ) ){
				cerr << "Cannot fill the program record map: " << endl;
				exit(1);
			}
			cout << "Program record map: " << mapProgramRecord.size() << endl;
			
			map<int,string> mapStation;
			if (! MapManagement::GetStationMap( mapStation ) ){
				cerr << "Cannot fill the station map: " << endl;
				exit(1);
			}
			cout << "Station map: " << mapStation.size() << endl;
			
			map<u_int64_t,string> mapSchedule;
			if (! MapManagement::GetScheduleMap( mapSchedule ) ){
				cerr << "Cannot fill the schedule map: " << endl;
				exit(1);
			}
			cout << "Schedule map: " << mapSchedule.size() << endl;
			
			map<string,string> mapActor;
			if (! MapManagement::GetActorMap( mapActor ) ){
				cerr << "Cannot fill the actor map: " << endl;
				exit(1);
			}
			cout << "Actor map: " << mapActor.size() << endl;
						
			map<string,string> mapGenre;
			if (! MapManagement::GetGenreMap( mapGenre ) ){
				cerr << "Cannot fill the genre map: " << endl;
				exit(1);
			}
			cout << "Genre map: " << mapGenre.size() << endl;
			
			map<string,string> mapRole;
			if (! MapManagement::GetRoleMap( mapRole ) ){
				cerr << "Cannot fill the role map: " << endl;
				exit(1);
			}
			cout << "Role map: " << mapRole.size() << endl;

			map<string,string> mapActorRole;
			if (! MapManagement::GetActorRoleMap( mapActorRole ) ){
				cerr << "Cannot fill the actor_role map" << endl;
				exit(1);
			}
			cout << "Actor role map: " << mapActorRole.size() << endl;
			
			/** send the client maps to the server and receive a file with all the changes that must
			be done */
			string clientfile = clientFunct->CalcDiffs(lineup,blacklist,mapProgramRecord,mapStation,mapSchedule,mapActor,mapGenre,mapRole,mapActorRole);

			string path = "/tmp/testfile";
 			ofstream testfile(path.c_str());
 			testfile << clientfile;
 			testfile.close();

			/** modify the client database*/
 			clientFunct->ModifyClientDatabase(path);

			string updatelist = clientFunct->getUpdateList();
			string deletelist = clientFunct->getDeleteList();

			path = "/tmp/updatefile";
 			ofstream updatefile(path.c_str());
 			updatefile << updatelist;
 			updatefile.close();

			path = "/tmp/deletefile";
 			ofstream deletefile(path.c_str());
 			deletefile << deletelist;
 			deletefile.close();

			//string command = "/usr/pluto/bin/FillMythDatabase";
			//system(command.c_str());
		}
		else {
			cerr << "Unknown command: " << g_GlobalConfig.m_sCommand << endl;
		}


}


