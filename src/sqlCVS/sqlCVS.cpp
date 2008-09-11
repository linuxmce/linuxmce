
/**
 * @file sqlCVS.cpp
 * @ brief  the main class for the sqlCVS
 *
*/


//#define C99_FORMAT_SPECIFIERS
#include "PlutoUtils/CommonIncludes.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#else
#include <conio.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Database.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"
#include "RA/RAServer.h"

#include "../include/version.h"

using namespace std;
using namespace sqlCVS;

vector<string> tables_list;


namespace sqlCVS
{
	GlobalConfig g_GlobalConfig;
}

using namespace DCE;
using namespace sqlCVS;

void ChangeLoginUsers();
bool ParseMaskFile(Database &database);

string GetCommand( )
{

	cout << "Use sqlCVS -? for command line options." << endl << endl
		<< "What would you like to do?" << endl
		<< "In the future you can also make your selections on the command line using the command shown in parenthesis." << endl
		<< "------Server-side functions------" << endl
		<< "1.	Create a new sqlCVS repository from an existing database ( create )" << endl
		<< "2.	Edit an existing repository ( edit )" << endl
		<< "3.	Start listening for incoming connections from clients ( listen )" << endl
		<< "4.	Permanently roll-back checkins ( rollback )" << endl
		<< "5.	Create a 'dump' file with the tables in the current repository ( dump )" << endl
		<< "6.	List all the tables and what repositories they are in ( list-tables )" << endl
		<< "7.	List all the repositories and what tables they have ( list-repositories )" << endl
		<< "8.	History on all tables ( history-all )" << endl
		<< "9.	History on no tables ( history-none )" << endl
		<< "10.	List unauthorized batches ( batches )" << endl
		<< "11.	List contents of batch ( batch-contents )" << endl
		<< "---The following are not normally used---" << endl
		<< "20.	Reset entire database--sqlCVS clients will be out of sync ( reset-all )" << endl
		<< "21.	Reset the system tables ( reset-sys )" << endl
		<< "22.	Update psc_id's ( update-psc )" << endl
		<< "23.	Update last_psc_batch/id **server only** ( update-last-psc )" << endl
		<< "------Client-side functions------" << endl
		<< "A.	Import a 'dump' file from a server and make a local, working copy ( import )" << endl
		<< "B.	Check-in changes you've made locally ( checkin )" << endl
		<< "C.	Update my local copy with changes from the server ( update )" << endl
		<< "D.	**disabled** Synchronize my database with the server. Same as checkin+update ( sync )" << endl
		<< "E.	View my local changes ( diff )" << endl
		<< "F.	Approve pending batch ( approve )" << endl
		<< "G.	Reject pending batch ( reject )" << endl
		<< "H.	Revert changes from a mask file ( revert )" << endl
		<< "I.	Changes a primary key and propagates ( change_key )" << endl
		<< endl
		<< "Z.	Change login or users" << endl
		<< endl
		<< "Q. Quit" << endl;

	string s;
	cin >> s;

	if( s=="1" )
		return "create";
	else if( s=="2" )
		return "edit";
	else if( s=="3" )
		return "listen";
	else if( s=="4" )
		return "rollback";
	else if( s=="5" )
		return "dump";
	else if( s=="6" )
		return "list-tables";
	else if( s=="7" )
		return "list-repositories";
	else if( s=="8" )
		return "history-all";
	else if( s=="9" )
		return "history-none";
	else if( s=="10" )
		return "batches";
	else if( s=="11" )
		return "batch-contents";
	else if( s=="20" )
		return "reset-all";
	else if( s=="21" )
		return "reset-sys";
	else if( s=="22" )
		return "update-psc";
	else if( s=="23" )
		return "update-last-psc";
	else if( s=="a" || s=="A" )
		return "import";
	else if( s=="b" || s=="B" )
		return "checkin";
	else if( s=="c" || s=="C" )
		return "update";
	else if( s=="d" || s=="D" )
		return "sync";
	else if( s=="e" || s=="E" )
		return "diff";
	else if( s=="f" || s=="F" )
		return "approve";
	else if( s=="g" || s=="G" )
		return "reject";
	else if( s=="h" || s=="H" )
		return "revert";
	else if( s=="i" || s=="I" )
		return "change_key";
	else if( s=="z" || s=="Z" )
		ChangeLoginUsers();  // This will loop back to the same menu
	else if( s=="q" || s=="Q" )
		exit( 0 );

	return "";
}

#ifndef WIN32
// handling signals
void signal_handler(int signal)
{
	switch (signal)
	{
		case SIGPIPE:
			cout << "Signal SIGPIPE received and ignored" << endl;
			break;
	}
}
#endif

int main( int argc, char *argv[] )
{
#ifndef WIN32
	signal(SIGPIPE, signal_handler);
#endif

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
			case 'h':
				g_GlobalConfig.m_sDBHost = argv[++optnum];
				break;
			case 'u':
				g_GlobalConfig.m_sDBUser = argv[++optnum];
				break;
			case 'p':
				g_GlobalConfig.m_sDBPassword = argv[++optnum];
				break;
			case 'D':
				g_GlobalConfig.m_sDBName = argv[++optnum];
				break;
			case 'P':
				g_GlobalConfig.m_iDBPort = atoi( argv[++optnum] );
				break;
			case 'H':
				g_GlobalConfig.m_sSqlCVSHost = argv[++optnum];
				break;
			case 'R':
				g_GlobalConfig.m_iSqlCVSPort = atoi( argv[++optnum] );
				break;
			case 'r':
				g_GlobalConfig.m_sRepository = argv[++optnum];
				break;
			case 's':
				g_GlobalConfig.m_sSkipVerification = argv[++optnum];
				break;
			case 'c':
				g_GlobalConfig.m_sComments = argv[++optnum];
				break;
			case 't':
				g_GlobalConfig.m_sTable = argv[++optnum];
				break;
			case 'd':
				g_GlobalConfig.m_sDefaultUser = argv[++optnum];
				break;
			case 'U':
				g_GlobalConfig.m_sUsers = argv[++optnum];
				break;
			case 'b':
				g_GlobalConfig.m_psc_batch = atoi( argv[++optnum] );
				break;
			case 'S':
				{
					string sRestrictions = argv[++optnum];
					string::size_type pos=0;
					while(pos!=string::npos && pos<sRestrictions.size())
						g_GlobalConfig.m_vectRestrictions.push_back( atoi(StringUtils::Tokenize(sRestrictions,",",pos).c_str()) );
				}
				break;
			case 'w':
				g_GlobalConfig.m_iScreenWidth = atoi( argv[++optnum] );
				g_GlobalConfig.dceConfig.AddInteger("ScreenWidth",g_GlobalConfig.m_iScreenWidth);
				g_GlobalConfig.dceConfig.WriteSettings();
				break;
			case 'v':
				g_GlobalConfig.m_bVerify = true;
				break;
			case 'V':
				g_GlobalConfig.m_iVerifyRestrictID = atoi( argv[++optnum] );
				break;
			case 'i':
				g_GlobalConfig.m_bVerifyID = true;
				break;
			case 'a':
				g_GlobalConfig.m_bAllowUnmetDependencies = true;
				break;
			case 'e':
				g_GlobalConfig.m_bCheckinEveryone = true;
				break;
			case 'A':
				g_GlobalConfig.m_bAllowAnonymousLogin = true;
				break;
			case 'n':
				g_GlobalConfig.m_bNoPrompts = true;
				break;
			case 'f':
				g_GlobalConfig.m_sOutputFile = argv[++optnum];
				break;
			case 'm':
				g_GlobalConfig.m_sMaskFile = argv[++optnum];
				break;
			case 'O':
				g_GlobalConfig.m_sPK_Old = argv[++optnum];
				break;
			case 'N':
				g_GlobalConfig.m_sPK_New = argv[++optnum];
				break;
			case 'C':
				g_GlobalConfig.m_iMaxConnectAttemptCount = atoi( argv[++optnum] );
				break;
			case 'T':
				g_GlobalConfig.m_iServerProcessorTimeout = atoi( argv[++optnum] );
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

//                123456789012345678901234567890123456789012345678901234567890
		cout << "sqlCVS, v." << VERSION << endl
			<< "Usage: sqlCVS [-h MySql hostname] [-u MySql username] " << endl
			<< "[-p MySql password] [-D MySql database] [-P mysql port] " << endl
			<< "[-H sqlCVS hostname] [-R sqlCVS port] " << endl
			<< "[-r Repository( -ies )] [-v] [-i] [-c comments]" << endl
			<< "[-t Table( s )] [-U Username[~Password][,...]] [-d username]" << endl
			<< "[-a] [-e everyone] [-w width]" << endl
			<< "[-s Skip Verification of fields] [-b batch] [-f] [-n]" << endl
			<< "-h hostname    -- address or DNS of database host," << endl
			<< "			default is `dcerouter`" << endl
			<< "-u username    -- username for database connection" << endl
			<< "-p password    -- password for database connection, "<< endl
			<< "			default is `` ( empty )" << endl
			<< "-D database    -- database name. default is pluto_main" << endl
			<< "-P port    	   -- port for database connection, " << endl
			<< "			default is 3306" << endl
			<< "-r output path -- Where to put the output files. "<< endl
			<< "			Default is ../[database name]" << endl
			<< "-t input path  -- Where to find the template files. " << endl
			<< "			Default is . then ../sqlCVS" << endl
			<< "-b batch       -- the batch number you want to approve" << endl
			<< "-U user~pass   -- the user(s) who are logged in and will be committing rows" << endl
			<< "-d username    -- the owner of any unclaimed new records" << endl
			<< "            Default is the first user checking in records" << endl
			<< "-v verify      -- Verifies the integrity of the database first (foreign keys)" << endl
			<< "-s Table:Field -- Comma delimted list of Fields to ignore when doing a verify" << endl
			<< "-i verify id   -- Verifies that all records have unique psc_id" << endl
			<< "-a allow       -- Allows checking in a row with a foreign key" << endl
			<< "            to a modified row in another table that is not" << endl
			<< "            being checked in" << endl
			<< "-e everyone    -- Checkin all records from every user" << endl
			<< "-n no prompts  -- Don't ever prompt, just exit if necessary" << endl
			<< "-c comments    -- Optional comments to be included with a checkin" << endl
			<< "-w screen width-- This value will 'stick' because it's stored in the config" << endl
			<< "-S restrictions-- Include the listed restrictions." << endl
			<< "                  If one restriction, other than 0," << endl
			<< "                  is specified with the -v option, rather than auto-deleting, it will" << endl
			<< "                  update orphaned rows to be in the same restriction" << endl
			<< "-f filename    -- Save the output of this command to a file, usually so" << endl
			<< "                  another program like a website can show the contents" << endl
			<< "                  normally used with -n" << endl
			<< "-O PK -N PK    -- For use with change_key command" << endl
			<< "-A anonymous   -- Support authorization witn anonymous~anonymous" << endl
			<< "                  username and password - all records are commited w/o quarantine" << endl
			<< "-C attempts count  -- Maximal attempts to connect server if it's busy, default is unlimited" << endl
			<< "-T processor timeout  -- Timeout for server-side processor (seconds, default is 15 min)" << endl;

		exit( 1 );
	}

	try
	{
#ifdef WIN32
		long err;
		WSADATA wsaData;
		err = WSAStartup( MAKEWORD( 1, 1 ), ( LPWSADATA ) &wsaData );
#endif
		cout << "Database host:" << g_GlobalConfig.m_sDBHost << " user:" << g_GlobalConfig.m_sDBUser
			<< " pass:" << g_GlobalConfig.m_sDBPassword << " name:" << g_GlobalConfig.m_sDBName << " port:" << g_GlobalConfig.m_iDBPort << endl
			<< "Users:" << g_GlobalConfig.m_sUsers << endl;

		Database database( g_GlobalConfig.m_sDBHost, g_GlobalConfig.m_sDBUser, g_GlobalConfig.m_sDBPassword, g_GlobalConfig.m_sDBName, g_GlobalConfig.m_iDBPort );
		if( !database.m_bConnected )
		{
			cerr << "***ERROR*** Cannot connect to database." << endl;
			cout << "Please modify the configuration settings and restart sqlCVS." << endl;
			if( !g_GlobalConfig.m_bNoPrompts )
	            ChangeLoginUsers();
			exit(1);
		}
		g_GlobalConfig.m_pDatabase=&database;

		/** Fill the lists with any repositories or tables that were passed in on the command line */
		database.GetRepositoriesTables( );

		string::size_type pos=0;
		string Token;
		while( ( Token=StringUtils::Tokenize( g_GlobalConfig.m_sUsers, ",", pos ) ).length( ) )
		{
			string::size_type pos2=0;
			string Username = StringUtils::Tokenize( Token, "~", pos2 );
			string Password = StringUtils::Tokenize( Token, "~", pos2 );
			g_GlobalConfig.m_mapUsersPasswords[Username]=Password;
		}

		if( !ParseMaskFile(database) )
			exit(1);

		while( true ) /** An endless loop processing commands */
		{
			while( !g_GlobalConfig.m_bNoPrompts && g_GlobalConfig.m_sCommand.length( )==0 )
			{
				g_GlobalConfig.m_sCommand=GetCommand( );
				database.m_bInteractiveMode=true;
			}
			if( database.bIsInvalid( ) && g_GlobalConfig.m_sCommand!="import" )
			{
				g_GlobalConfig.m_sCommand="";
				cerr << "Database is invalid. Import " << endl << "is the only available options" << endl;
				if( g_GlobalConfig.m_bNoPrompts )
					exit(1);
				continue;
			}
			if( g_GlobalConfig.m_sCommand=="create" )
				database.CreateRepository( );
			else if( g_GlobalConfig.m_sCommand=="checkin" )
			{
				if( g_GlobalConfig.m_sRepository.length( ) )
				{
					/** If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository */
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.CheckIn( true );
				// We may have updated some records because we added new records, which caused new primary keys to be
				// created and propagated, altering records we had already submitted.  We'll need to resubmit them again.
				database.CheckIn( false );
			}
			else if( g_GlobalConfig.m_sCommand=="update" )
			{
				if( g_GlobalConfig.m_sRepository.length( ) )
				{
					/** If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository */
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.Update( );
			}
			else if( g_GlobalConfig.m_sCommand=="listen" )
			{
				RAServer *pServer = new RAServer( );
				pServer->Initialize( );
				pServer->StartListening( g_GlobalConfig.m_iSqlCVSPort );
				pServer->Run( );
				delete pServer;
			}
			else if( g_GlobalConfig.m_sCommand=="dump" )
			{
				if( g_GlobalConfig.m_sRepository.length( ) )
				{
					/** If it was a valid repository, GetRepositoriesTables would have replaced it with a pointer to the repository in mapRepository */
					cerr << "Repository: " << g_GlobalConfig.m_sRepository << "is invalid";
					throw "Bad Arguments";
				}
				database.Dump( );
			}
			else if( g_GlobalConfig.m_sCommand=="import" )
			{
				database.Import( );
			}
			else if( g_GlobalConfig.m_sCommand=="list-tables" )
			{
				database.ListTables( );
			}
			else if( g_GlobalConfig.m_sCommand=="list-repositories" )
			{
				database.ListRepositories( );
			}
			else if( g_GlobalConfig.m_sCommand=="reset-all" )
			{
				database.Reset_all();
			}
			else if( g_GlobalConfig.m_sCommand=="reset-sys" )
			{
				database.Reset_sys();
			}
			else if( g_GlobalConfig.m_sCommand=="update-psc" )
			{
				database.Update_psc();
			}
			else if( g_GlobalConfig.m_sCommand=="update-last-psc" )
			{
				database.Update_lastpsc();
			}
			else if( g_GlobalConfig.m_sCommand=="history-all" )
			{
				database.HasFullHistory_set_all(true);
			}
			else if( g_GlobalConfig.m_sCommand=="history-none" )
			{
				database.HasFullHistory_set_all(false);
			}
			else if( g_GlobalConfig.m_sCommand=="batches" )
			{
				database.ListUnauthorizedBatches();
			}
			else if( g_GlobalConfig.m_sCommand=="batch-contents" )
			{
				database.ListBatchContents();
			}
			else if( g_GlobalConfig.m_sCommand=="diff" )
			{
				database.ShowChanges();
			}
			else if( g_GlobalConfig.m_sCommand=="approve" )
			{
				database.Approve();
			}
			else if( g_GlobalConfig.m_sCommand=="reject" )
			{
				database.Approve(true);
			}
			else if( g_GlobalConfig.m_sCommand=="revert" )
			{
				database.Revert();
			}
			else if( g_GlobalConfig.m_sCommand=="change_key" )
			{
				database.ChangeKey();
			}
			else if( g_GlobalConfig.m_sCommand=="rollback" )
			{
				database.RollbackBatch();
			}
			else
			{
				cerr << "Unknown command: " << g_GlobalConfig.m_sCommand << endl;
			}

			if( database.m_bInteractiveMode )
			{
				cout << "To do this non-interactively, use the following command:" << endl;
				cout << "sqlCVS -h \"" << g_GlobalConfig.m_sDBHost << "\" -u \"" << g_GlobalConfig.m_sDBUser
					<< "\" -p \"" << g_GlobalConfig.m_sDBPassword << "\" -D \"" << g_GlobalConfig.m_sDBName
					<< "\" -P \"" << g_GlobalConfig.m_iDBPort << "\" -r \"" << g_GlobalConfig.m_sRepository ;
				if( g_GlobalConfig.m_mapRepository.size() )
				{
					for(MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin();it!=g_GlobalConfig.m_mapRepository.end();++it)
						cout << (g_GlobalConfig.m_sRepository.size() || it!=g_GlobalConfig.m_mapRepository.begin() ? "," : "") << (*it).first;
				}
				cout << "\" -t \"" << g_GlobalConfig.m_sTable;
				if( g_GlobalConfig.m_mapTable.size()  )
				{
					for(MapTable::iterator it=g_GlobalConfig.m_mapTable.begin();it!=g_GlobalConfig.m_mapTable.end();++it)
						cout << (g_GlobalConfig.m_sTable.size() || it!=g_GlobalConfig.m_mapTable.begin() ? "," : "") << (*it).first;
				}

				cout << "\" -U \"" << g_GlobalConfig.m_sUsers
					<< "\" " << g_GlobalConfig.m_sCommand << endl;
			}
/** @test
			else if( Command=="add-tables" )
				database.AddTablesToRepository( sRepository, &listCommandParms );
			else if( Command=="remove-tables" )
				database.RemoveTablesFromRepository( sRepository, &listCommandParms );
			else if( Command=="listen" )
				rp.Listen( iListenPort, &database, sRepository );
			else if( Command=="dump" )
				database.DumpTables( sRepository, &listCommandParms );

*/
	return 0;
		};
	}
	catch( char *pException )
	{
		cerr << "Caught exception: " << pException << endl;
		exit(1);
	}
	catch( const char *pException )
	{
		cerr << "Caught exception: " << pException << endl;
		exit(1);
	}
	catch( string sException )
	{
		cerr << "Caught exception: " << sException << endl;
		exit(1);
		return 1;
	}
	catch( ... )
	{
		cerr << "Caught unknown exception" << endl;
		exit(1);
	}

	
#ifdef _WIN32
		WSACleanup( );
#endif
	cout << "Everything okay";
	return 0;
}

void ChangeLoginUsers()
{
	while(true)
	{
		cout << "MySql Database Login Info:" << endl
			<< "A.  Host: " << g_GlobalConfig.m_sDBHost << endl
			<< "B.  User: " << g_GlobalConfig.m_sDBUser << endl
			<< "C.  Password: " << g_GlobalConfig.m_sDBPassword << endl
			<< "D.  Database: " << g_GlobalConfig.m_sDBName << endl
			<< "E.  Port: " << g_GlobalConfig.m_iDBPort << endl
			<< endl
			<< "Automatically login users (username~password,...):" << endl
			<< g_GlobalConfig.m_sUsers << endl;

		cout << endl << endl
			<< "Q.  Quit" << endl
			<< "S.  Save changes to config file" << endl
			<< "Enter A-E,Q,S or a user name to add/remove a user for auto-login:";

		string Selection;
		cin >> Selection;
		cout << endl;

		if( Selection=="A" || Selection=="a" )
		{
			cout << "Enter the new MySql host: ";
			cin >> g_GlobalConfig.m_sDBHost;
			cout << endl;
		}
		else if( Selection=="B" || Selection=="b" )
		{
			cout << "Enter the MySql user: ";
			cin >> g_GlobalConfig.m_sDBUser;
			cout << endl;
		}
		else if( Selection=="C" || Selection=="c" )
		{
			cout << "Enter the MySql password: ";
			cin >> g_GlobalConfig.m_sDBPassword;
			cout << endl;
		}
		else if( Selection=="D" || Selection=="d" )
		{
			cout << "Enter the MySql database: ";
			cin >> g_GlobalConfig.m_sDBName;
			cout << endl;
		}
		else if( Selection=="E" || Selection=="e" )
		{
			cout << "Enter the MySql port: ";
			cin >> g_GlobalConfig.m_iDBPort;
			cout << endl;
		}
		else if( Selection=="Q" || Selection=="q" )
		{
			return;
		}
		else if( Selection=="S" || Selection=="s" )
		{
			g_GlobalConfig.dceConfig.AddString("sqlCVS_MySqlHost",g_GlobalConfig.m_sDBHost);
			g_GlobalConfig.dceConfig.AddString("sqlCVS_MySqlUser",g_GlobalConfig.m_sDBUser);
			g_GlobalConfig.dceConfig.AddString("sqlCVS_MySqlPassword",g_GlobalConfig.m_sDBPassword);
			g_GlobalConfig.dceConfig.AddString("sqlCVS_MySqlDatabase",g_GlobalConfig.m_sDBName);
			g_GlobalConfig.dceConfig.AddInteger("sqlCVS_MySqlPort",g_GlobalConfig.m_iDBPort);
			g_GlobalConfig.dceConfig.AddInteger("sqlCVS_Port",g_GlobalConfig.m_iSqlCVSPort);
			g_GlobalConfig.dceConfig.AddString("sqlCVS_Host",g_GlobalConfig.m_sSqlCVSHost);
			g_GlobalConfig.dceConfig.AddString("sqlCVS_Users",g_GlobalConfig.m_sUsers);
			g_GlobalConfig.dceConfig.WriteSettings();
			return;
		}
		else
		{
			// It must be a user
			vector<string> vectUsers;
			StringUtils::Tokenize(g_GlobalConfig.m_sUsers,",",vectUsers);
			int iRemoveUser=-1; // This will be >=0 if the user typed in an existing user name
			for(size_t s=0;s<vectUsers.size();s++)
			{
				// The vector may have a ~ password
				string::size_type pos=0;
				string Username = StringUtils::Tokenize(vectUsers[s],"~",pos);
				if( Username==Selection )
				{
					iRemoveUser=(int) s;
					break;
				}
			}
			if( iRemoveUser==-1 )
			{
				cout << "Enter the password for '" << Selection << "' or 0 to prompt for it each time: ";
				string Password;
				cin >> Password;
				if( Password=="0" )
					vectUsers.push_back(Selection);
				else
					vectUsers.push_back(Selection + "~" + Password);
			}
			g_GlobalConfig.m_sUsers="";
			for(size_t s=0;s<vectUsers.size();++s)
				if( s!=iRemoveUser )
					g_GlobalConfig.m_sUsers += (g_GlobalConfig.m_sUsers.length() ? "," : "") + vectUsers[s];
		}
	}
}

bool ParseMaskFile(Database &database)
{
	if( g_GlobalConfig.m_sMaskFile.size()==0 )
		return true; // OK

	vector<string> vectCommitLines;
	FileUtils::ReadFileIntoVector( g_GlobalConfig.m_sMaskFile, vectCommitLines );
	if( vectCommitLines.size()==0 )
	{
		cerr << "Mask file is empty" << endl;
		return false;
	}
	for(size_t s=0;s<vectCommitLines.size();++s)
	{
		string::size_type pos=0;
		string sRepository,sTable,sWhere,sToc;
		if( vectCommitLines[s].size()==0 )
			continue; // blank line
		while( pos<vectCommitLines[s].size() )
		{
			string sToken = StringUtils::Tokenize(vectCommitLines[s],"\t",pos);
			if( StringUtils::StartsWith(sToken,"REP:") && sToken.size()>4 )
				sRepository = sToken.substr(4);
			else if( StringUtils::StartsWith(sToken,"TABLE:") && sToken.size()>6 )
				sTable = sToken.substr(6);
			else if( StringUtils::StartsWith(sToken,"WHERE:") && sToken.size()>6 )
				sWhere = sToken.substr(6);
			else if( StringUtils::StartsWith(sToken,"CHANGE:") && sToken.size()>7 )
				sToc = sToken.substr(7);
		}

		Repository *pRepository = database.m_mapRepository_Find(sRepository);
		Table *pTable = database.m_mapTable_Find(sTable);
		if( !pRepository || !pTable || sWhere.size()==0 )
		{
			cerr << "Mask file has invalid format.  Use: REP:designer    TABLE:DesignObjVariation_DesignObjParameter     CHANGE:DEL      WHERE:1=1" << endl;
			return false;
		}

		enum TypeOfChange eToc;

		if( sToc=="NEW" )
			eToc=toc_New;
		else if( sToc=="MOD" )
			eToc=toc_Modify;
		else if( sToc=="DEL" )
			eToc=toc_Delete;
		else
		{
			cerr << "Mask file has invalid format.  Use: REP:designer    TABLE:DesignObjVariation_DesignObjParameter     CHANGE:DEL      WHERE:1=1" << endl;
			return false;
		}

		int psc_id=0;
		if( (pos=sWhere.find("psc_id="))!=string::npos )
			psc_id = atoi( sWhere.substr(pos+7).c_str() );

		g_GlobalConfig.m_mapMaskedChanges[ make_pair<string,string> (sRepository+":"+sTable,sWhere) ] =
			new MaskedChange(pTable,pRepository,eToc,psc_id);
	}

	return true;
}
