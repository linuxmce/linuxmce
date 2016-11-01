/*
 Database
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

 
 
/**
 *
 * @file Database.cpp
 * @brief source file for the Database, SafetyDBLock, SafetyTransaction classes
 *
 */
 
   
#include "PlutoUtils/CommonIncludes.h"	
#include "Database.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "RA/RA_Processor.h"
#include "R_ApproveBatch.h"
#include "CommonFunctions.h"
#include <iomanip>
#include <sstream>

using namespace sqlCVS;

Database::Database( string db_host, string db_user, string db_pass, string db_name, int db_port )
	: MySqlHelper( db_host, db_user, db_pass, db_name, db_port )
{
	if( !m_bConnected )
	{
		// Try connecting without a database
		m_sMySQLDBName = "";
		MySQLConnect(true);
		if( !m_bConnected )
			return;  // Nope, the connection is no good

		g_GlobalConfig.m_bNewDatabase=true;
		// Try creating the dabase
		cout << "Creating database " << db_name << endl;
		threaded_mysql_query("CREATE DATABASE " + db_name);
		m_sMySQLDBName = db_name; // Set it back and try again
		MySQLConnect(true);
		if( !m_bConnected )
			return;
	}

	m_bInvalid=false;
	try
	{
		/** Establishing database connection	*/
		m_bInteractiveMode=false;

		LoadDatabaseStructure();
		bool bSchemaChanged = false;

		// See if the schema has changed for any of the repositories
		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			int PriorSchema = atoi(pRepository->GetSetting("schema","1").c_str());

			if( pRepository->UpdateSchema(PriorSchema) )
				bSchemaChanged = true;
		}

		if( bSchemaChanged )
		{
			cout << "Schema changed.  Reloading database structure." << endl;
			for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
			{
				Repository *pRepository = ( *it ).second;
				delete pRepository;
			}
			m_mapRepository.clear();
			for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
			{
				Table *pTable = ( *it ).second;
				delete pTable;
			}
			m_mapTable.clear();
			LoadDatabaseStructure();
		}
	}
	catch( const char *pException )
	{
		cerr << "Caught exception: " << pException << endl << "***DATABASE SCHEMA INVALID. IMPORT IS THE ONLY VALID OPTION***" << endl;
		m_bInvalid=true;
	}
}

void Database::LoadDatabaseStructure()
{
	/** And build our list of tables and repositories	*/
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	if( res.r=mysql_list_tables( m_pMySQL, NULL ) )
	{
		while ( ( row = mysql_fetch_row( res.r ) ) )
		{
			Table *pTable = new Table( this, row[0] );
			m_mapTable[row[0]] = pTable;
			/** Does this table help define a repository ( ie table is psc_repositoryname_[x] */
			string sRepository = pTable->DefinesRepository( );
			if( sRepository.size( ) )
			{
				Repository *pRepository = m_mapRepository[sRepository];
				if( !pRepository )
				{
					pRepository = new Repository( this, sRepository );
					m_mapRepository[sRepository] = pRepository;
				}
				pRepository->AddDefinitionTable( pTable );
			}
		}
	}

	for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
		( *it ).second->MatchUpTables( );
	for( MapTable::iterator itT=m_mapTable.begin( );itT!=m_mapTable.end( );++itT )
	{
		Table *pTable = ( *itT ).second;

		pTable->MatchUpHistory( );
		pTable->GetDependencies( );
		if( pTable->Repository_get( ) )
		{
			if( g_GlobalConfig.m_bVerify || g_GlobalConfig.m_bVerifyID )
				pTable->VerifyIntegrity();  // Check all the psc_id's to be sure they are assigned & unique
			
			if( !pTable->m_bIsSystemTable )
			{
				pTable->m_psc_id_last_sync = ( pTable->Repository_get( )->psc_id_last_sync_get( pTable ) );
				pTable->m_psc_batch_last_sync = ( pTable->Repository_get( )->psc_batch_last_sync_get( pTable ) );
			}
		}
	}
	
	if( g_GlobalConfig.m_bVerify )
		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
			( *it ).second->VerifyIntegrity( );
}

void Database::GetRepositoriesTables( )
{
	string s, sUnknownRepository;
	string::size_type pos=0;

	while( ( s=StringUtils::Tokenize( g_GlobalConfig.m_sRepository, ",", pos ) ).length( ) )
	{
		Repository *pRepository = m_mapRepository_Find( s );
		if( !pRepository )
			sUnknownRepository += ( sUnknownRepository.length( ) ? "," : "" ) + s;
		else
			g_GlobalConfig.m_mapRepository[s]=pRepository;
	}
	g_GlobalConfig.m_sRepository=sUnknownRepository;

	pos=0;
	while( ( s=StringUtils::Tokenize( g_GlobalConfig.m_sTable, ",", pos ) ).length( ) )
	{
		Table *pTable = m_mapTable[s];
		if( !pTable )
		{
			cerr << "Unknown Table: " << s << endl;
			throw "Unknown Table";
		}
		g_GlobalConfig.m_mapTable[s]=pTable;
	}
}

void Database::CreateRepository( )
{
	vector<string> vect_sRepository;
	StringUtils::Tokenize( g_GlobalConfig.m_sRepository, ",", vect_sRepository );

	if( vect_sRepository.size( )>1 && g_GlobalConfig.m_mapTable.size( )>0 )
	{
		cerr << "You cannot specify more than 1 repository to create at a time if you explicitly list tables" << endl;
		throw "Bad arguments";
	}

	if( vect_sRepository.size( )==0 )
	{
		while( g_GlobalConfig.m_sRepository.length( )==0 )
		{
			cout << "What do you want to call this repository? ( enter q to quit ): ";
			cin >> g_GlobalConfig.m_sRepository;
			if( g_GlobalConfig.m_sRepository=="q" )
				return; /**< Do nothing */
			vect_sRepository.push_back( g_GlobalConfig.m_sRepository );
		}
	}

	/** We must have at least 1 repository specified, maybe more if the user passed them in the command line		*/
	for( size_t s=0;s<vect_sRepository.size( );++s )
	{
		Repository *pRepository = m_mapRepository_Find( vect_sRepository[s] );
		if( pRepository )
		{

			if( !AskYNQuestion( "That repository exists. Edit the exiting one?", false ) )
				return; /**< Nope, just abort */
		}
		else
			pRepository = new Repository( this, vect_sRepository[s] );

		int Result;
		
		/** If the user didn't specify tables on the command line, prompt for them */
		if( g_GlobalConfig.m_mapTable.size( )==0 && ( Result=PromptForTablesInRepository( pRepository, g_GlobalConfig.m_mapTable ) )<1 ) 
		{
			if( Result<0 )
				return; /**< Just return without doing anything */
			pRepository->Remove( ); /**< Remove this repository if there are no tables */
			MapRepository::iterator it = m_mapRepository.find( pRepository->Name_get( ) );
			
			/** It could be that, even though the user chose create, he typed in the name of an existing repository  */
			if( it!=m_mapRepository.end( ) ) 
				m_mapRepository.erase( it );
			delete pRepository;

			return; /**< PromptForTablesInRepository returns the number of tables the user selected, or -1 to exit */
		}

		pRepository->Setup( ); /**< Sets up the repository */
		m_mapRepository[ pRepository->Name_get( ) ] = pRepository;
	}
	Update_psc();
}



int Database::PromptForTablesInRepository( Repository *pRepository, MapTable &mapTable )
{
	m_bInteractiveMode=true;

	int iNumColumns = g_GlobalConfig.m_iScreenWidth / ( ( int )MaxTablenameLength( )+8 );
	int iColumnWidth = g_GlobalConfig.m_iScreenWidth/iNumColumns;

	/**
	 * We'll use this map to keep track of what tables the user wants history tracking for
	 * This will be pre-populated with the current settings, and then modified as the user
	 * makes changes.  We'll write out the changes when the user is done.
	 */
	map<string,bool> mapTableHistory;

	/**
	 * Since the user may have called this with an existing repository, do a quick run 
	 * first to be sure we add any tables that are already in the repository to the map
	 * The user can always remove them in the main loop
	 */
	 
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( pTable->bIsSystemTable_get() )
			continue; /**< It's a system table */

		/**
		 * If the table is not in any repository, that's okay -- the user will be adding it. But if it already is in a repository, 
		 * it must point to this one if it's already in the list
		 */
		 
		if( pTable->Repository_get( ) )
		{
			/** 
			 * If it's in the list, and points to us, that's ok. If it's not in the list and points to another, that's also ok. 
			 * But if it's in the list and points to another, that's an error
			 */
			 
			if( mapTable.find( pTable->Name_get( ) )!=mapTable.end( ) && pTable->Repository_get( )!=pRepository )
			{
				/**
				 * We would never allow this to happen, but maybe the user specified everything in the command line and 
				 * is trying to put a table into a repository that is already in another repository
				 */
				 
				cerr << "Table: " << pTable->Name_get( ) << " is already in the repository " << pTable->Repository_get( )->Name_get( ) << endl;
				cerr << "It cannot be added to repository: " << pRepository->Name_get( ) << endl;
				throw "Bad arguments";
			}
			
			/**
			 * and if it's not in the list, but points to us, we need to add it, since, by default, it will be selected. 
			 * The user can remove it
			 */
			 
			else if( mapTable.find( pTable->Name_get( ) )==mapTable.end( ) && pTable->Repository_get( )==pRepository )
				mapTable[ pTable->Name_get( ) ] = pTable;
		}

		mapTableHistory[pTable->Name_get()]=pTable->HasFullHistory_get();
	}

	string sError="";
	while( true )
	{
		NewSection( );
		int iColumnNum=0;
		vector<Table *> vectTable; /**< A numeric reference to the tables the user can change */
		for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		{
			Table *pTable = ( *it ).second;
			if( StringUtils::StartsWith( pTable->Name_get( ), "psc_" ) || pTable->bIsSystemTable_get() )
				continue; /**< It's a system table */

			bool bTableAlreadyInList = mapTable.find( pTable->Name_get( ) )!=mapTable.end( );

			/** 001 *H TableName   */
			if( pTable->Repository_get( ) && pTable->Repository_get( )!=pRepository )
				cout << "XXX"; /**< It's in another repository, we can't touch it */
			else
			{
				vectTable.push_back( pTable );
				cout << setw( 3 ) << ( int ) vectTable.size( );
			}

			cout << ( bTableAlreadyInList ? " *" : " " ) << ( mapTableHistory[pTable->Name_get()] ? "H " : " " );
			if( ( int ) pTable->Name_get( ).length( )>iColumnWidth )
				cout << pTable->Name_get( ).substr( 0, iColumnWidth );
			else
				cout << pTable->Name_get( );

			if( ++iColumnNum >= iNumColumns )
			{
				iColumnNum=0;
				cout << endl;
			}
			else 
				cout << StringUtils::RepeatChar( ' ', iColumnWidth - ( int )pTable->Name_get( ).length( ) );
		}

		cout << endl << "Tables marked with * are to be included in the repository: " << pRepository->Name_get( ) << endl
			<< "Enter the table numbers, separated by commas, to toggle the * flag." << endl
			<< "XXX means the table is already in another repository, and cannot be added" << endl
			<< "Tables with an H in front will have history tracking." << endl
			<< "Enter H followed by the table numbers to toggle the history flag." << endl
			<< "Enter W[NumColumns], [ColumnWidth] to change layout" << endl
			<< "Enter D when you have finished, Q to quit." << endl;
		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
		{
			// Write out any changes to the history settings
			for(map<string,bool>::iterator it=mapTableHistory.begin();it!=mapTableHistory.end();++it)
			{
				Table *pTable = m_mapTable[(*it).first];
				if( pTable && !pTable->bIsSystemTable_get() && pTable->HasFullHistory_get()!=(*it).second )
					pTable->HasFullHistory_set((*it).second);
			}
			return ( int ) mapTable.size( );
		}
		if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort  */
		if( sAction[0]=='W' || sAction[0]=='w' )
		{
			string::size_type pos=1; /**< Skip the w */
			string sNumColumns = StringUtils::Tokenize( sAction, ",", pos );
			string sWidth = StringUtils::Tokenize( sAction, ",", pos );
			if( atoi( sNumColumns.c_str( ) ) )
				iNumColumns = atoi( sNumColumns.c_str( ) );
			if( atoi( sWidth.c_str( ) ) )
				iColumnWidth = atoi( sWidth.c_str( ) );
		}

		bool bHistory = sAction[0]=='h' || sAction[0]=='H';
		string::size_type pos = bHistory ? 1 : 0;
		string sNumber;
		while( ( sNumber=StringUtils::Tokenize( sAction, ",", pos ) ).length( ) )
		{
			int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
			if( iNumber<0 || iNumber>( int ) vectTable.size( )-1 )
			{
				sError ="Invalid table number";
				break;  /**< stops the while */
			}
			Table *pTable = vectTable[iNumber];
			if( bHistory )
				mapTableHistory[pTable->Name_get()]=!mapTableHistory[pTable->Name_get()];
			else if( mapTable.find( pTable->Name_get( ) )!=mapTable.end( ) )
				mapTable.erase( pTable->Name_get( ) );
			else
				mapTable[pTable->Name_get( )] = pTable;
		}
	}
	return ( int ) mapTable.size( );
}

size_t Database::MaxTablenameLength( )
{
	size_t MaxLength=0;
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
	{
		if( ( *it ).first.length( )>MaxLength )
			MaxLength = ( *it ).first.length( );
	}

	return MaxLength;
}

class Table *Database::GetTableFromForeignKey( Field *pField )
{
	string sTable = pField->Name_get( ).substr( 3 );

	while( sTable.length( )>0 )
	{
		/** See if table is a valid Table Name */
		Table *pTable = m_mapTable_Find( sTable );
		if( pTable )
			return pTable;

		string Tail = pField->Table_get( )->GetTrailingString( sTable );	/**< Use the helper function in table */
		if( Tail.length()==0 )
			break;
		sTable = sTable.substr( 0, sTable.length( ) - Tail.length( ) - 1 );	/**< Strip off the last part */
	}

	cerr << "I cannot find the table referred to in the field: " << pField->Table_get( )->Name_get( ) << ":" << pField->Name_get( ) << endl;
	throw "Bad Schema";
}

class Table *Database::GetTableFromForeignKeyString( Table *pTable, string sField )
{
	string sTable = sField.substr( 3 );

	while( sTable.length( )>0 )
	{
		/** See if table is a valid Table Name  */
		Table *pTable = m_mapTable_Find( sTable );
		if( pTable )
			return pTable;

		string Tail = pTable->GetTrailingString( sTable );			/**< Use the helper function in table */
		sTable = sTable.substr( 0, sTable.length( ) - Tail.length( ) - 1 );	/**< Strip off the last part */
	}

	cerr << "I cannot find the table referred to in the table: " << pTable->Name_get( ) << " by " << sField << endl;
	throw "Bad Schema";
}

void Database::ShowChanges()
{
	{  // So ra_Processor will fall out of scope before we browse changes
		int ClientID=1, SoftwareVersion=1; /** @warning HACK!!!  */
		RA_Processor ra_Processor( ClientID, SoftwareVersion, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );

		// Don't need to create a transaction since we're not changing anything.

		DCE::Socket *pSocket=NULL;

		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( g_GlobalConfig.m_mapRepository.size() && 
					g_GlobalConfig.m_mapRepository.find( pRepository->Name_get() )==g_GlobalConfig.m_mapRepository.end() )
				continue;  // If we specified repositories, only show the tables in those repositories -- not all tables

			for( MapTable::iterator it=pRepository->m_mapTable.begin( );it!=pRepository->m_mapTable.end( );++it )
			{
				Table *pTable = ( *it ).second;
				
				if( g_GlobalConfig.m_mapTable.size() && 
						g_GlobalConfig.m_mapTable.find( pTable->Name_get() )==g_GlobalConfig.m_mapTable.end() )
					continue;

				/**
				* Since we don't need to connect to the server for anything here, but we don't each table to make it's own connection, 
				* Pass in the connection string and the NULL pointer to the socket so a connection will be made the
				* first time and preserved for the other tables
				*/
				 
				if( pTable->Repository_get( ) )
				{
					pTable->GetChanges(); // This will get the adds and modifies
					if( !pTable->DetermineDeletions( ra_Processor, g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) )
					{
						delete pSocket;
						cerr << "Unable to contact the server to determine what records were deleted" << endl;
						throw "database error";
					}
				}
			}
		}
		delete pSocket;
	}

	bool bOutputToFile=g_GlobalConfig.m_sOutputFile.size()>0;
	fstream fstr_Diff;
	if( bOutputToFile )
		fstr_Diff.open(g_GlobalConfig.m_sOutputFile.c_str(),fstream::out);

	while(true)
	{
		cout << "    Repository                                 New   Mod   Del" << endl;

		vector<Repository *> vectRepository; // So we can allow the user to pick a repository by number
		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( g_GlobalConfig.m_mapRepository.size() && 
					g_GlobalConfig.m_mapRepository.find( pRepository->Name_get() )==g_GlobalConfig.m_mapRepository.end() )
				continue;  // If we specified repositories, only show the tables in those repositories -- not all tables
			vectRepository.push_back(pRepository);
			cout << setw( 3 ) << (int) vectRepository.size() << " ";
			if( pRepository->Name_get().length() > 40 )
				cout << pRepository->Name_get().substr(0,40);
			else
			{
				cout << pRepository->Name_get() 
					<< StringUtils::RepeatChar( ' ', 40  - ( int )pRepository->Name_get( ).length( ) );
			}

			int iNew=0,iMod=0,iDel=0;
			for(MapTable::iterator itT = pRepository->m_mapTable.begin();itT != pRepository->m_mapTable.end();itT++)
			{
				Table *pTable = (*itT).second;
				for(map<int, ListChangedRow *>::iterator itCR=pTable->m_mapUsers2ChangedRowList.begin();itCR!=pTable->m_mapUsers2ChangedRowList.end();++itCR)
				{
					ListChangedRow *pListChangedRow = (*itCR).second;
					for(ListChangedRow::iterator itLCR=pListChangedRow->begin();itLCR!=pListChangedRow->end();++itLCR)
					{
						ChangedRow *pChangedRow = *itLCR;
						if( pChangedRow->m_eTypeOfChange==toc_New )
							iNew++;
						else if( pChangedRow->m_eTypeOfChange==toc_Modify )
							iMod++;
						else if( pChangedRow->m_eTypeOfChange==toc_Delete )
							iDel++;

						// Output instead
						if( bOutputToFile )
						{
							fstr_Diff << "REP:" << pRepository->Name_get() << "\t" 
								<< "TABLE:" << pTable->Name_get() << "\t"
								<< "USER:" << pChangedRow->m_psc_user << "\t"
								<< "CHANGE:";
							if( pChangedRow->m_eTypeOfChange==toc_New )
								fstr_Diff << "NEW\t";
							else if( pChangedRow->m_eTypeOfChange==toc_Modify )
								fstr_Diff << "MOD\t";
							else if( pChangedRow->m_eTypeOfChange==toc_Delete )
								fstr_Diff << "DEL\t";
							fstr_Diff << "WHERE:" << pChangedRow->GetWhereClause() << "\n";
						}
					}
				}
			}
			if (iNew != 0)
				cout << setw(6) << iNew;
			else
				cout << setw(6) << "-";
			if (iMod != 0)
				cout << setw(6) << iMod;
			else
				cout << setw(6) << "-";
			if (iDel != 0)
				cout << setw(6) << iDel;
			else
				cout << setw(6) << "-";
			cout << endl;
		}

		if( bOutputToFile )
		{
			fstr_Diff.close();
			return; // Nothing more to do--we only output the diffs to a file
		}

		cout << "What repository do you want more detail on?  Enter 'q' to quit" << endl;
		string sRepository;
		cin >> sRepository;
		if( sRepository=="q" || sRepository=="Q" )
			return;

		int iRepository = atoi(sRepository.c_str());
		if( iRepository < 1 || iRepository > (int) vectRepository.size() )
			cout << "That is not a valid selection" << endl;
		else
		{
			Repository *pRepository = vectRepository[ iRepository-1 ];
			if( !pRepository->ShowChanges() )
				return;
		}
	}
}

void Database::CheckIn( bool bFirstLoop )
{
	g_GlobalConfig.m_mapUsersTables.clear(); // Be sure we start clean, since this could be the second loop

	if( bFirstLoop )
		GetTablesToCheckIn( );
	
	if( g_GlobalConfig.m_mapTable.size( )==0 )
	{
		cerr << "No tables found to add. Aborting." << endl;
		return;
	}

	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		pTable->m_mapUsers2ChangedRowList.clear(); // Be sure we start clean, since this could be the second loop
		pTable->m_vectRowsToDelete.clear();
		pTable->itmp_RowsToDelete=0;
		pTable->GetChanges( );
	}

	if( bFirstLoop )
	{
		bool bDependenciesMet=true;
		if( !g_GlobalConfig.m_bAllowUnmetDependencies )
		{
			for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
			{
				if( !( *it ).second->ConfirmDependencies( ) )
					bDependenciesMet=false;
			}
		}

		if( !bDependenciesMet )
		{
			cerr << "Aborting checkin due to unmet dependencies" << endl;
			throw "Unmet dependencies";
		}

		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( !pRepository->DetermineDeletions( ) )
				return; /**< This will throw an exception with the transaction */
		}

		/** Now mapTable has all the tables we need to check in. Confirm the users if none were passed in on the command line   */
		if( g_GlobalConfig.m_mapUsersPasswords.size( )==0 && ConfirmUsersToCheckIn( )<1 )
			return;

		// If the only user in the map is the 0 user (ie new, unclaimed records), then at least 1 real user must be logged in
		if( (*(g_GlobalConfig.m_mapUsersPasswords.begin())).first=="0" && g_GlobalConfig.m_mapUsersPasswords.size()==1 )
		{
			while( g_GlobalConfig.m_mapUsersPasswords.size()==1 )
			{
				cout << "Type in user names, or id's who will be doing the checkin." << endl
					<< "You can separate multiple ones with a comma." << endl
					<< "Enter 'q' to quit." << endl;
				
				string sUsers;
				cin >> sUsers;

				if( sUsers=="q" || sUsers=="Q" )
					return;
				string::size_type pos=0;
				string Token;
				while( (Token=StringUtils::Tokenize(sUsers,",",pos)).length() )
					g_GlobalConfig.m_mapUsersPasswords[Token]="";  // Add the user with no password.  We will prompt for the password below
			}
		}

		// If no default user is specified, we will pick one.  This will be the user that is assigned ownership of any new, unclaimed records
		if( g_GlobalConfig.m_sDefaultUser.size()==0 )
			g_GlobalConfig.m_sDefaultUser = (*(g_GlobalConfig.m_mapUsersPasswords.begin())).first;

		for(MapStringString::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
		{
			if( (*it).first=="0" )
				continue; // We don't care about the '0' user.  This is for unclaimed new records

			string md5;
			string Password = (*it).second;
			while( Password.length()==0 )
			{
				cout << "Enter the password for user " << (*it).first << " or 0 if you don't know it or "
					<< "'nopass' to checkin without his password: ";
				cin >> Password;
			}

			// It requires the this->????  Guess there's another md5 function
			if( Password=="0" )
				g_GlobalConfig.m_mapUsersPasswords[(*it).first]="";
			else
				g_GlobalConfig.m_mapUsersPasswords[(*it).first]=this->md5(Password);
		}

		/** If any of the records to be checked in refer to a record with an auto-increment field that is new and is not being checked in,
		* this will fail
		*/
		if( !ConfirmRecordsToCheckIn( ) )
			return;
	}

// Temporary hack to avoid accidental deletions while trying to sort out what sometimes goes wrong
for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
{
Table *pTable = ( *it ).second;
if( pTable->m_vectRowsToDelete.size() )
{
cout << "Table: " << pTable->Name_get() <<  " has " << pTable->m_vectRowsToDelete.size() << " rows to delete from the server" << endl;
if( !g_GlobalConfig.m_bNoPrompts && !AskYNQuestion("Continue",false) )
	throw "Checkin aborted";
}

if( pTable->itmp_RowsToDelete )
{
cout << "Table: " << pTable->Name_get() <<  " has " << pTable->itmp_RowsToDelete << " rows deleted locally" << endl;
if( !g_GlobalConfig.m_bNoPrompts && !AskYNQuestion("Continue",false) )
	throw "Checkin aborted";
}
}

	try
	{
		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( !pRepository->HasChangedRecords() )
			{
				if( bFirstLoop )
					cout << "Repository: " << pRepository->Name_get() << " has no changes.  Skipping..." << endl;
			}
			else if( !pRepository->CheckIn( ) )
			{
				cerr << "Checkin failed" << endl;
				throw "Checkin failed";
				return;
			}
		}
	}
	catch( const char *pException )
	{
		cerr << "Checkin threw exception: " << pException << endl;
		throw "Checkin failed";
	}

	if( !bFirstLoop && g_GlobalConfig.m_mapBatch_ChangedRow.size() )
	{
		cout << "Some records have been checked-in, but are pending" << endl
			<< "because you are not authorized to modify the table." << endl
			<< "You will need to have the record owner or a supervisor" << endl
			<< "approve the batch.  Refer to the batch ID's below:" << endl;

		for( map<int, ListChangedRow *>::iterator it=g_GlobalConfig.m_mapBatch_ChangedRow.begin();it!=g_GlobalConfig.m_mapBatch_ChangedRow.end();++it )
		{
			cout << "----------------------------------------------" << endl
				<< "Batch #" << (*it).first << endl;
			ListChangedRow *pListChangedRow = (*it).second;
			for(ListChangedRow::iterator it2=pListChangedRow->begin();it2!=pListChangedRow->end();++it2)
			{
				ChangedRow *pChangedRow = *it2;
				cout << "Table: " << pChangedRow->m_pTable->Name_get() << " psc_id: " << pChangedRow->m_psc_id_new;
				if( pChangedRow->m_bFrozen )
					cout << " frozen";
				if( pChangedRow->m_psc_user_needs_to_authorize )
					cout << " owner: " << pChangedRow->m_psc_user_needs_to_authorize;
				cout << endl;
			}
		}
	}
}

void Database::Update( )
{
	GetTablesToCheckIn( );
	
	if( g_GlobalConfig.m_mapTable.size( )==0 )
	{
		cerr << "No tables found to add. Aborting." << endl;
		return;
	}

	/** Now mapTable has all the tables we need to check in. Confirm the users if none were passed in on the command line */
	if( g_GlobalConfig.m_mapUsersPasswords.size( )==0 && ConfirmUsersToUpdate( )<1 )
		return;

	try
	{
		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			if( !pRepository->Update( ) )
			{
				cerr << "ERROR: Update returned false" << endl;
				return; 
			}
		}
	}
	catch( const char *pException )
	{
		cerr << "Update threw exception: " << pException << endl;
		return;
	}
}

void Database::GetTablesToCheckIn( )
{
	/** If the user passed specific tables on the command line, that's okay. Otherwise we need to add the tables from the repository  */
	
	int flag = 0;
	MapTable mapTableToCheckIn;
	MapTable::iterator itToChkIn = g_GlobalConfig.m_mapTable.begin( );

	if( g_GlobalConfig.m_mapTable.size( )==0 )
	{
		if( g_GlobalConfig.m_mapRepository.size( )==0 )
			if( PromptForRepositories( )<1 )
				return;

		/** Since the user didn't specify tables explicitly, but rather repositories, just add all tables in the repository  */
		for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			pRepository->AddTablesToMap( ); /**< Be sure all the tables in the repositories are added */
		}
	}

	return;
}

int Database::PromptForRepositories( bool bOneOnly )
{
	m_bInteractiveMode=true;

	string sError="";
	while( true )
	{
		NewSection( );
		vector<Repository *> vectRepository; /**< A numeric reference to the tables the user can change */
		for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
		{
			Repository *pRepository = ( *it ).second;
			vectRepository.push_back( pRepository );
			cout << setw( 3 ) << ( int ) vectRepository.size( );
			cout << ( g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) ? " *" : " " ) << pRepository->Name_get( ) << endl;
		}

		if( bOneOnly )
			cout << "Enter the Repository number or Q to quit:" << endl;
		else
			cout << "Repositories marked with * are to be included." << endl
				<< "Enter the Repository numbers, separated by commas, to toggle the * flag." << endl
				<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return ( int ) g_GlobalConfig.m_mapRepository.size( );
		if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort  */

		string::size_type pos=0;
		string sNumber;
		while( ( sNumber=StringUtils::Tokenize( sAction, ",", pos ) ).length( ) )
		{
			int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
			if( iNumber<0 || iNumber>( int ) vectRepository.size( )-1 )
			{
				sError ="Invalid Repository number";
				break;
			}
			Repository *pRepository = vectRepository[iNumber];
			if( g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) )
				g_GlobalConfig.m_mapRepository.erase( pRepository->Name_get( ) );
			else
				g_GlobalConfig.m_mapRepository[pRepository->Name_get( )] = pRepository;

			if( bOneOnly )
				return ( int ) g_GlobalConfig.m_mapRepository.size( );
		}
	}
}

int Database::PromptForSqlCvsFiles( )
{
	m_bInteractiveMode=true;

	string sError="";
	while( true )
	{
		NewSection( );

		vector<string> vectFiles; /**< A numeric reference to the tables the user can change */

#ifdef WIN32  /** Windows */

		intptr_t ptrFileList;
		_finddata_t finddata;

		ptrFileList = _findfirst( "*.sqlcvs", & finddata );
		while ( ptrFileList != -1 )
		{
			if ( finddata.attrib == _A_SUBDIR || finddata.name[0] == '.' )
				continue;
			
			vectFiles.push_back( finddata.name );

			if ( _findnext( ptrFileList, & finddata ) < 0 )
				break;
		}
		_findclose( ptrFileList );
#else /** Linux */
#endif
		for( size_t s=0;s<vectFiles.size( );++s )
		{
			string sRepository = FileUtils::FilenameWithoutPath( vectFiles[s], false );
			Repository *pRepository = ( g_GlobalConfig.m_mapRepository.find( sRepository )!=g_GlobalConfig.m_mapRepository.end( ) ? g_GlobalConfig.m_mapRepository[sRepository] : NULL );
			Repository *pRepository_Valid = m_mapRepository_Find( sRepository );
			bool bFound = g_GlobalConfig.m_sRepository.find( "," + sRepository + "," )!=string::npos;
			cout << setw( 3 ) << ( int ) s+1;
			cout << ( pRepository || bFound ? " *" : " " )
				<< ( pRepository_Valid ? "!" : " " )
				<< vectFiles[s] << endl;
		}

		cout << "Repositories marked with * are to be included. ! are existing repositories." << endl
			<< "Enter the Repository numbers, separated by commas, to toggle the * flag." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			 /** We don't know exactly how many since it could be a combination of comma delimited, plus existing repositories */
			return 1;
		if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort */

		string::size_type pos=0;
		string sNumber;
		while( ( sNumber=StringUtils::Tokenize( sAction, ",", pos ) ).length( ) )
		{
			int iNumber=atoi( sNumber.c_str( ) )-1; /**< Our vector is 0 based, subtract 1 */
			if( iNumber<0 || iNumber>( int ) vectFiles.size( )-1 )
			{
				sError ="Invalid Repository number";
				break;
			}
			string sRepository = FileUtils::FilenameWithoutPath( vectFiles[iNumber], false );
			Repository *pRepository = m_mapRepository_Find( sRepository );
			if( pRepository )
			{
				/** The file is an existing repository */
				if( g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) )
					g_GlobalConfig.m_mapRepository.erase( pRepository->Name_get( ) );
				else
					g_GlobalConfig.m_mapRepository[pRepository->Name_get( )] = pRepository;
			}
			else
			{
				/** The file is not an existing repository */
				if( g_GlobalConfig.m_sRepository.find( "," + sRepository + "," )!=string::npos )
					StringUtils::Replace( &g_GlobalConfig.m_sRepository, "," + sRepository + ",","" );
				else
				{
					if( g_GlobalConfig.m_sRepository.length( )==0 )
						g_GlobalConfig.m_sRepository = ",";
					g_GlobalConfig.m_sRepository += sRepository + ",";
				}
			}
		}
	}
}

int Database::ConfirmUsersToCheckIn( )
{
	if( g_GlobalConfig.m_bCheckinEveryone )
	{
		for ( map<int, MapTable*>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
			g_GlobalConfig.m_mapUsersPasswords[StringUtils::itos(( *it ).first)]="";
		return ( int ) g_GlobalConfig.m_mapUsersPasswords.size( );
	}

	string sError="";
	while( true )
	{
		NewSection( );
		for( map<int, MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
		{
			MapTable *pMapTable = ( *it ).second;
			cout << ( g_GlobalConfig.m_mapUsersPasswords.find( StringUtils::itos(( *it ).first) )!=g_GlobalConfig.m_mapUsersPasswords.end( ) ? "* " : " " )
				<< "User ID: " << ( *it ).first << " modified the following tables: " << endl
				<< "\t\t\t";
			int ColumnPosition=0;
			for( MapTable::iterator it=pMapTable->begin( );it!=pMapTable->end( );++it )
			{
				Table *pTable = ( *it ).second;
				if( ColumnPosition && ColumnPosition + pTable->Name_get( ).length( ) > g_GlobalConfig.m_iScreenWidth )
				{
					cout << endl;
					ColumnPosition=0;
				}
				else
				{
					if( ColumnPosition )
						cout << ", ";
					++ColumnPosition;
				}
				cout << pTable->Name_get( );
			}
			if( ColumnPosition )
				cout << endl;
		}

		cout << "Users marked with * are to be included." << endl
			<< "Enter the users ID's, separated by commas, to toggle the * flag." << endl
			<< "Enter + to select all users, - to deselect all users." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length( ) )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return ( int ) g_GlobalConfig.m_mapUsersPasswords.size( );
		else if( sAction=="Q" || sAction=="q" )
			return -1; /**< means abort */

		if ( sAction == "-" )
			g_GlobalConfig.m_mapUsersPasswords.clear( );
		else 
			if ( sAction == "+" )
				for ( map<int, MapTable*>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
					g_GlobalConfig.m_mapUsersPasswords[StringUtils::itos(( *it ).first)]="";

			else
			{
				string::size_type pos=0;
				string sNumber;
				while( ( sNumber=StringUtils::Tokenize( sAction, ",", pos ) ).length( ) )
				{
					int iNumber=atoi( sNumber.c_str( ) );
					if( g_GlobalConfig.m_mapUsersTables.find( iNumber )==g_GlobalConfig.m_mapUsersTables.end( ) )
					{
						sError ="Invalid User ID";
						break;
					}
					if( g_GlobalConfig.m_mapUsersPasswords.find( StringUtils::itos(iNumber) )!=g_GlobalConfig.m_mapUsersPasswords.end( ) )
						g_GlobalConfig.m_mapUsersPasswords.erase( StringUtils::itos(iNumber) );
					else
						g_GlobalConfig.m_mapUsersPasswords[StringUtils::itos(iNumber)] = "";
				}
			}
	}
}

bool Database::ConfirmRecordsToCheckIn( )
{
/** If any of the records to be checked in refer to a record with an auto-increement field that is new and is not being checked in, this will fail */

	return true;
}

void Database::Dump( )
{
	bool bSupervisor;
	if( !g_GlobalConfig.ValidateUsers(bSupervisor,false) )
	{
		cerr << "Failed to validate any users" << endl;
		throw "No Users";
	}
	if( g_GlobalConfig.m_mapRepository.size( )==0 )
		if( PromptForRepositories( )<1 )
			return; /**< abort*/

	for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
	{
		Repository *pRepository = ( *it ).second;
		pRepository->Dump( );
	}
}

void Database::Import( )
{
	if( g_GlobalConfig.m_mapRepository.size( )==0 && g_GlobalConfig.m_sRepository.length( )==0 )
	{
		if( PromptForSqlCvsFiles( )<0 )
			return;  /**< abort */
	}

	for( MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin( );it!=g_GlobalConfig.m_mapRepository.end( );++it )
		Import( ( *it ).first, ( *it ).second );

	string::size_type pos=0;
	string sRepository;
	while( true )
	{
		sRepository=StringUtils::Tokenize( g_GlobalConfig.m_sRepository, ",", pos );
		if( sRepository.length( ) )
		{
			Repository *pRepository = new Repository(this,sRepository);
			m_mapRepository[sRepository] = pRepository;
			Import( sRepository, pRepository );
		}
		if( pos==string::npos || pos>=g_GlobalConfig.m_sRepository.length( ) )
			break;
	}
}

class SerializeableStrings : public SerializeClass
{
public:
	vector<string> m_vectString;
	void SetupSerialization( ) {	StartSerializeList( ) + m_vectString; 	}
};

void Database::Import( string sRepository, Repository *pRepository )
{
	SerializeableStrings str;
	if( !str.SerializeRead( sRepository + ".sqlcvs" ) )
		throw "Cannot read file";

	size_t pos=0;

	// Pass in NULL for all the tables since we will just rebuild them anyway
	string sTable = str.m_vectString[pos++];

	/*  IMPORT THE NEW REPOSITORY SETTINGS -- SAVE OUR LOCAL SETTINGS FIRST */
	// First save the settings in this map
	MapStringString mapSettings;
	if( pRepository->m_pTable_Setting )
	{
		string Tablename = "psc_" + pRepository->Name_get() + "_repset";
		ostringstream sql;

		sql	<< "SELECT Setting,Value FROM `" << Tablename << "`";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
			while ( row = mysql_fetch_row( result_set.r ) )
				mapSettings[row[0]]=row[1];
	}

	if( sTable != "psc_" + pRepository->Name_get() + "_repset" )
		throw "Import schema error";

	pRepository->ImportTable(sTable,str,pos,NULL,0,0);

	MapStringString::iterator it;
	for(it=mapSettings.begin();it!=mapSettings.end();++it)
	{
		if( (*it).first!="schema" )  // Don't import this setting.  We just updated the schema
			pRepository->SetSetting((*it).first,(*it).second);
	}

	int PriorSchema = atoi(mapSettings["schema"].c_str());

	int CurrentSchema = atoi(pRepository->GetSetting("schema","1").c_str());
	if( !CurrentSchema )
		CurrentSchema = 1;

	if( CurrentSchema<PriorSchema )
	{
		pRepository->SetSetting("schema",StringUtils::itos(PriorSchema));  // Set it back to what it was
		cout << "WARNING:  Not importing repository " << pRepository->Name_get() << " with PriorSchema " << PriorSchema << " because it had a newer schema than " << CurrentSchema << endl;
		return;
	}

	sTable = str.m_vectString[pos++];
	if( sTable != "psc_" + pRepository->Name_get() + "_bathdr" )
		throw "Import schema error";
	pRepository->ImportTable(sTable,str,pos,NULL,0,0);

	sTable = str.m_vectString[pos++];
	if( sTable != "psc_" + pRepository->Name_get() + "_batuser" )
		throw "Import schema error";
	pRepository->ImportTable(sTable,str,pos,NULL,0,0);

	sTable = str.m_vectString[pos++];
	if( sTable != "psc_" + pRepository->Name_get() + "_batdet" )
		throw "Import schema error";
	pRepository->ImportTable(sTable,str,pos,NULL,0,0);

	/*  IMPORT THE NEW REPOSITORY TABLES -- SAVE OUR LOCAL SETTINGS FIRST */
	map< string,pair<int,int> > mapLast_psc;   // pair<last_psc_id,last_psc_batch>
	if( pRepository->m_pTable_Tables )
	{
		ostringstream sql;
		sql	<< "SELECT Tablename,last_psc_id,last_psc_batch FROM `" << pRepository->m_pTable_Tables->Name_get() << "`";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
			while ( row = mysql_fetch_row( result_set.r ) )
				mapLast_psc[row[0]]=pair<int,int>(atoi(row[1]),atoi(row[2]));
	}

	sTable = str.m_vectString[pos++];
	if( sTable != "psc_" + pRepository->Name_get() + "_tables" )
		throw "Import schema error";
	pRepository->ImportTable(sTable,str,pos,NULL,0,0);

	/*  Before we restore our local settings back, the table will now have the 
	values the server had when it did the dump.  We need these so that when we do 
	an import on the table, we know which records we can delete.  It's possible that
	a record in the local database is either still pending, or might be newer than
	what's in the import file.  If so, we should keep the local one.  Also when
	doing an update, only update if the batch locally is not > than the one in the file */
	map< string,pair<int,int> > mapLast_psc_FromServer;   // pair<last_psc_id,last_psc_batch>
	if( pRepository->m_pTable_Tables )
	{
		ostringstream sql;
		sql	<< "SELECT Tablename,last_psc_id,last_psc_batch FROM `" << pRepository->m_pTable_Tables->Name_get() << "`";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
			while ( row = mysql_fetch_row( result_set.r ) )
				mapLast_psc_FromServer[row[0]]=pair<int,int>(atoi(row[1]),atoi(row[2]));
	}


	for(map< string,pair<int,int> >::iterator it=mapLast_psc.begin();it!=mapLast_psc.end();++it)
	{
		string sTable = (*it).first;
		int last_psc_id = (*it).second.first;
		int last_psc_batch = (*it).second.second;
		ostringstream sql;
		std::ostringstream sSQL;
		sSQL << "UPDATE `" << pRepository->m_pTable_Tables->Name_get( ) << "` SET last_psc_batch='" << 
			last_psc_batch << "',last_psc_ID='" << last_psc_id << "' WHERE Tablename='" << sTable << "'";
		if( threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "SQL failed: " << sSQL.str( );
			throw "Internal error Repository::psc_last_id,last_batch set";
		}
	}

	sTable = str.m_vectString[pos++];
	if( sTable != "psc_" + pRepository->Name_get() + "_schema" )
		throw "Import schema error";
	pRepository->ImportTable(sTable,str,pos,NULL,0,0);

	if( CurrentSchema>PriorSchema && PriorSchema && !g_GlobalConfig.m_bNewDatabase )
	{
		pRepository->UpdateSchema(PriorSchema);
		// Get all the fields again since this could have changed things.  Don't worry about matching
		// up until we get to re-assigning primary keys
		for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		{
			Table *pTable = ( *it ).second;
			pTable->GetFields();
		}
		for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		{
			Table *pTable = ( *it ).second;
			pTable->MatchUpHistory( );
			// we are doing an import - should continue even if GetDependencies is invalid
			try
			{
				pTable->GetDependencies( );
			}
			catch( const char *pException )
			{
				cerr << "Suppresed exception: \"" << pException << endl << "\" because we are in the middle of IMPORT. Hope the schema will be OK after all" << endl;
				m_bInvalid=true;
			}
		}
	}

	int NumTables = atoi( str.m_vectString[pos++].c_str( ) );
	for( int i=0;i<NumTables;++i )
	{
		std::ostringstream sSQL;
		string sTable = str.m_vectString[pos++];
		Table *pTable = m_mapTable_Find(sTable);

		// pair<last_psc_id,last_psc_batch>
		pair<int,int> psc_last = mapLast_psc_FromServer[sTable];
		pRepository->ImportTable(sTable,str,pos,pTable,psc_last.first,psc_last.second);
	}

	str.FreeSerializeMemory( );
}

void Database::ListTables( )
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->m_bIsSystemTable )
			continue;
		cout << "Table: " << pTable->Name_get();
		if( pTable->Repository_get() )
			cout << "\t\tRepository: " << pTable->Repository_get()->Name_get();
		else
			cout << "\t\t**no repository**";
		cout << endl;
	}
}

void Database::ListRepositories( )
{
	for(MapRepository::iterator it=m_mapRepository.begin();it!=m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		pRepository->ListTables();
	}
}

void Database::HasFullHistory_set_all( bool bOn )
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get() && pTable->HasFullHistory_get()!=bOn )
			pTable->HasFullHistory_set( bOn );
	}
}

void Database::Reset_all()
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get() )
		{
			pTable->TrackChanges_set(false);
			pTable->TrackChanges_set(true);

			// We need to update all the psc_id's to have a sequential number.  First figure out the primary keys for this table
			if( pTable->m_listField_PrimaryKey.size()==0 )
			{
				cout << "**Error**  Table: " << pTable->Name_get() << " has no primary key.  Can't update it." << endl;
				throw "Database error";
			}
			ostringstream sql;
			sql << "SELECT ";
			bool bFirst=true;
			for(ListField::iterator it=pTable->m_listField_PrimaryKey.begin();it!=pTable->m_listField_PrimaryKey.end();++it)
			{
				Field *pField = *it;
				if( bFirst )
					bFirst=false;
				else
					sql << ",";

				sql << "`" << pField->Name_get( ) << "`";
			}
			
			sql << " FROM `" << pTable->Name_get() << "`";
			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			int RowCount=1;
			if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
			{
				while ( row = mysql_fetch_row( result_set.r ) )
				{
					sql.str("");
					sql << "UPDATE `" << pTable->Name_get() << "` SET psc_mod=NULL,psc_id=" << RowCount++ << " WHERE ";
					int FieldCount=0;
					bool bFirst=true;
					for(ListField::iterator it=pTable->m_listField_PrimaryKey.begin();it!=pTable->m_listField_PrimaryKey.end();++it)
					{
						Field *pField = *it;
						if( bFirst )
							bFirst=false;
						else
							sql << " AND ";

						sql << "`" << pField->Name_get( ) << "`='";
						if( row[FieldCount] )
							sql << row[FieldCount];
						sql << "'";
						FieldCount++;
					}
					if( threaded_mysql_query(sql.str())<0 )
					{
						cerr << "Could not update table " << pTable->Name_get() << endl;
						throw "Database error";
					}
				}
			}
		}
		else
			pTable->TrackChanges_set(false);
	}

	Reset_sys();
}

void Database::Reset_sys()
{
	for(MapRepository::iterator it=m_mapRepository.begin();it!=m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		pRepository->ResetSystemTables();
	}
}

void Database::Update_lastpsc()
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		Repository *pRepository = pTable->Repository_get();
		if( pRepository && pRepository->m_pTable_Tables && !pTable->m_bIsSystemTable )
		{
			int last_psc_id=0,last_psc_batch=0;

			ostringstream sql;
			PlutoSqlResult result_id;
			MYSQL_ROW row=NULL;
			sql << "SELECT max(psc_id),max(psc_batch) FROM `" << pTable->Name_get() << "`";
			if( ( result_id.r=mysql_query_result( sql.str( ) ) ) && ( row = mysql_fetch_row( result_id.r ) ) )
			{
				if( row[0] )
					last_psc_id = atoi( row[0] );
				
				if( row[1] )
					last_psc_batch = atoi( row[1] );
			}

			sql.str("");
			sql << "UPDATE `" << pRepository->m_pTable_Tables->Name_get() << "` SET last_psc_id=" << last_psc_id
				<< ",last_psc_batch=" << last_psc_batch << " WHERE Tablename='" << pTable->Name_get() << "'";
			if( threaded_mysql_query( sql.str( ) )<0 )
			{
				cerr << "SQL failed: " << sql.str( );
				throw "Internal error Database::Update_lastpsc";
			}
		}
	}
}


void Database::Update_psc()
{
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get() && !pTable->m_bIsSystemTable )
		{
			// We need to update all the psc_id's to have a sequential number.  First figure out the primary keys for this table
			if( pTable->m_listField_PrimaryKey.size()==0 )
			{
				cout << "**Error**  Table: " << pTable->Name_get() << " has no primary key.  Can't update it." << endl;
				throw "Database error";
			}

			int psc_id=1;
			ostringstream sql;
			PlutoSqlResult result_id;
			MYSQL_ROW row=NULL;
			sql << "SELECT max(psc_id) FROM `" << pTable->Name_get() << "`";
			if( ( result_id.r=mysql_query_result( sql.str( ) ) ) && ( row = mysql_fetch_row( result_id.r ) ) && row[0] )
				psc_id = atoi( row[0] ) +1;

			sql.str("");
			sql << "SELECT ";
			bool bFirst=true;
			for(ListField::iterator it=pTable->m_listField_PrimaryKey.begin();it!=pTable->m_listField_PrimaryKey.end();++it)
			{
				Field *pField = *it;
				if( bFirst )
					bFirst=false;
				else
					sql << ",";

				sql << "`" << pField->Name_get( ) << "`";
			}
			
			sql << " FROM `" << pTable->Name_get() << "` WHERE psc_id is null OR psc_id=0";
			PlutoSqlResult result_set;

			if( ( result_set.r=mysql_query_result( sql.str( ) ) ) )
			{
				while ( row = mysql_fetch_row( result_set.r ) )
				{
					sql.str("");
					sql << "UPDATE `" << pTable->Name_get() << "` SET psc_id=" << psc_id++ << " WHERE ";
					int FieldCount=0;
					bool bFirst=true;
					for(ListField::iterator it=pTable->m_listField_PrimaryKey.begin();it!=pTable->m_listField_PrimaryKey.end();++it)
					{
						Field *pField = *it;
						if( bFirst )
							bFirst=false;
						else
							sql << " AND ";

						sql << "`" << pField->Name_get( ) << "`='";
						if( row[FieldCount] )
							sql << row[FieldCount];
						sql << "'";
						FieldCount++;
					}
					if( threaded_mysql_query(sql.str())<0 )
					{
						cerr << "Could not update table " << pTable->Name_get() << endl;
						throw "Database error";
					}
				}
			}
		}
	}
}

void Database::StartTransaction( ) 
{
	if( threaded_mysql_query("START TRANSACTION")<0 )
	{
		cerr << "Could not start transaction" << endl;
		throw "Database error";
	}
}

void Database::Commit( )
{
	cout << "Doing a Commit" << endl;
	if( threaded_mysql_query("COMMIT")<0 )
	{
		cerr << "Could not commit transaction" << endl;
		throw "Database error";
	}
}

void Database::Rollback( )
{
	cout << "Doing a rollback" << endl;
	if( threaded_mysql_query("ROLLBACK")<0 )
	{
		cerr << "Could not rollback transaction" << endl;
		throw "Database error";
	}
}

void Database::Approve( bool bReject )
{
	if( g_GlobalConfig.m_mapRepository.size()==0 && PromptForRepositories(true)!=1 )
		return;  // User must have chosen quit

	while( !g_GlobalConfig.m_psc_batch )
	{
		cout << "What is the batch number (enter q to quit)? ";
		string sBatch;
		cin >> sBatch;
		if( sBatch=="q" || sBatch=="Q" )
			exit(1);
		g_GlobalConfig.m_psc_batch = atoi(sBatch.c_str());
	}

	/** Now mapTable has all the tables we need to check in. 
	  Confirm the users if none were passed in on the command line or put in the .conf file  */
	if( g_GlobalConfig.m_mapUsersPasswords.size( )==0 && ConfirmUsersToUpdate()==0 )
		return;

	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );

	for(MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin();it!=g_GlobalConfig.m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		R_ApproveBatch r_ApproveBatch( pRepository->Name_get(), g_GlobalConfig.m_psc_batch, bReject );
		for(MapStringString::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
		{
			// We don't care about user 0, or users who didn't log in
			if( (*it).first=="0" || (*it).second=="" )
				continue;
			r_ApproveBatch.m_mapUsersPasswords[(*it).first]=(*it).second;
		}

		ra_Processor.AddRequest( &r_ApproveBatch );
		DCE::Socket *pSocket=NULL;
		while( ra_Processor.SendRequests( g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) );
		delete pSocket;
		if( r_ApproveBatch.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
		{
			ra_Processor.RemoveRequest( &r_ApproveBatch ); /**< It's going to fall out of scope, so we don't want the processor to retry */
			cerr << "Unable to approve batch:" << (int) r_ApproveBatch.m_cProcessOutcome << " " << r_ApproveBatch.m_sMessage << endl;
			if( r_ApproveBatch.m_cProcessOutcome==LOGIN_FAILED )
				cerr << "Login failed" << endl;

			return;
		}
		if( bReject )
			cout << "Rejected batch in repository: " << pRepository->Name_get() << endl;
		else
			cout << "Approved batch in repository: " << pRepository->Name_get() << endl;
	}
}

int Database::ConfirmUsersToUpdate()
{
	while(true)
	{
		cout << "The following users will be logged in: " << endl;
		for(MapStringString::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
		{
			cout << (*it).first << endl;
		}

		cout << endl << "Enter a username to add to or remove from the list or 'd' for done, 'q' to quit: ";
		string sUser;
		cin >> sUser;

		if( sUser=="d" || sUser=="D" )
			return (int) g_GlobalConfig.m_mapUsersPasswords.size();
		else if( sUser=="u" || sUser=="U" )
			exit(1);

		if( g_GlobalConfig.m_mapUsersPasswords.find(sUser)!=g_GlobalConfig.m_mapUsersPasswords.end() )
			g_GlobalConfig.m_mapUsersPasswords.erase(sUser);
		else
		{
			cout << "Enter the password for " << sUser << ":";
			string sPassword;
			cin >> sPassword;
			g_GlobalConfig.m_mapUsersPasswords[sUser]=sPassword;
		}
	}
}

void Database::ListUnauthorizedBatches()
{
	for( MapRepository::iterator it=m_mapRepository.begin( );it!=m_mapRepository.end( );++it )
	{
		Repository *pRepository = ( *it ).second;
		if( g_GlobalConfig.m_mapRepository.size()==0 || 
			g_GlobalConfig.m_mapRepository.find( pRepository->Name_get( ) )!=g_GlobalConfig.m_mapRepository.end( ) )
		{
			cout << "Repository: " << pRepository->Name_get() << endl;
			std::ostringstream sSQL;
			sSQL << "SELECT FK_psc_" << pRepository->Name_get() << "_bathdr,Tablename,New,Deleted,Modified,IPAddress,`date` FROM " <<
				pRepository->m_pTable_BatchDetail->Name_get() << " JOIN " << pRepository->m_pTable_BatchHeader->Name_get() <<
				" ON FK_psc_"  << pRepository->Name_get() << "_bathdr=PK_psc_"  << pRepository->Name_get() << "_bathdr " <<
				" WHERE FK_psc_" << pRepository->Name_get() << "_bathdr_unauth<1" <<
				" AND FK_psc_" << pRepository->Name_get() << "_bathdr_unauth=FK_psc_" << pRepository->Name_get() << "_bathdr " <<
				" AND FK_psc_" << pRepository->Name_get() << "_bathdr_auth=0";

			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			if( ( result_set.r=mysql_query_result( sSQL.str( ) ) ) )
			{
				if( result_set.r->row_count==0 )
					cout << "** None **" << endl;
				while ( ( row = mysql_fetch_row( result_set.r ) ) )
				{
					cout << "Batch: " << row[0] << " New: " << row[2] << " Del: " << row[3] << " Mod: " << row[4] << " " << row[5] << " " << row[6] << " " << row[1] << endl;
				}
			}
		}
	}
}

void Database::ListBatchContents()
{
	if( g_GlobalConfig.m_mapRepository.size()==0 && PromptForRepositories(true)!=1 )
		return;  // User must have chosen quit

	if( g_GlobalConfig.m_psc_batch==0 )
	{
		cout << "What batch number? (0 to quit) ";
		cin >> g_GlobalConfig.m_psc_batch;
		if( g_GlobalConfig.m_psc_batch==0 )
			return; /**< Do nothing */
	}

	if( g_GlobalConfig.m_mapRepository.size( )!=1 )
	{
		cerr << "You must specify 1 and only 1 repository for the batch to rollback" << endl;
		throw "Parameter error";
	}

	Repository *pRepository = (*g_GlobalConfig.m_mapRepository.begin()).second;

	std::ostringstream sSQL;
	sSQL << "SELECT IPAddress,`date` FROM " << pRepository->m_pTable_BatchHeader->Name_get() <<
		" WHERE PK_" << pRepository->m_pTable_BatchHeader->Name_get() << "=" << g_GlobalConfig.m_psc_batch;
	PlutoSqlResult result_hdr;
	MYSQL_ROW row=NULL;
	if( ( result_hdr.r=mysql_query_result( sSQL.str( ) ) ) && ( row = mysql_fetch_row( result_hdr.r ) ) )
	{
		cout << "Batch: " << g_GlobalConfig.m_psc_batch << " Repository: " << pRepository->Name_get() <<
			" IP: " << row[0] << " Date: " << row[1] << endl;
	}

	sSQL.str("");
	sSQL << "SELECT DISTINCT Tablename FROM psc_" << pRepository->Name_get() << "_batdet WHERE FK_psc_" << pRepository->Name_get() << "_bathdr=" << g_GlobalConfig.m_psc_batch;
	PlutoSqlResult result_set;
	if( ( result_set.r=mysql_query_result( sSQL.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set.r ) )
		{
			string sTable = row[0];
			std::ostringstream sSQL2;
			sSQL2 << "SELECT psc_id,psc_toc,`" << sTable << "_pschist`.* FROM `" << sTable << "_pschist` WHERE psc_batch=" << g_GlobalConfig.m_psc_batch;
			PlutoSqlResult result_set2;
			MYSQL_ROW row2=NULL;
			cout << endl << "Table: " << sTable << endl << "=========================" << endl;
			if( ( result_set2.r=mysql_query_result( sSQL2.str( ) ) ) )
			{
				while ( row2 = mysql_fetch_row( result_set2.r ) )
				{
					std::ostringstream sSQL3;
					PlutoSqlResult result_set3;
					MYSQL_ROW row3=NULL;
					if( atoi(row2[1])==toc_Modify )
					{
						sSQL3 << "SELECT * FROM `" << sTable << "_pschmask` WHERE psc_batch=" << g_GlobalConfig.m_psc_batch << " AND psc_id=" << row2[0];
						result_set3.r=mysql_query_result(sSQL3.str());
						row3=mysql_fetch_row( result_set3.r );
						if( !row3 )
						{
							cout << "Cannot find record in hmask table: " << sSQL3 << endl;
							cerr << "Cannot find record in hmask table: " << sSQL3 << endl;
							throw "Database error";
						}
					}

					if( atoi(row2[1])==toc_New )
						cout << "NEW- ";
					else if( atoi(row2[1])==toc_Modify )
						cout << "MODIFY- ";
					else if( atoi(row2[1])==toc_Delete )
						cout << "DELETE- ";

					for(int iField=2;iField<result_set2.r->field_count;iField++)
					{
						if( atoi(row2[1])==toc_Modify && (!row3[iField-2] || row3[iField-2][0]=='0') )
							continue;
						if( atoi(row2[1])==toc_Delete && strcmp(result_set2.r->fields[iField].name,"psc_id")!=0 )
							continue;
						if( strcmp(result_set2.r->fields[iField].name,"psc_batch")==0 ||
//								strcmp(result_set2.r->fields[iField].name,"psc_mod")==0 ||
								strcmp(result_set2.r->fields[iField].name,"psc_toc")==0 )
							continue;

						cout << result_set2.r->fields[iField].name << ":";
						if( !row2[iField] )
							cout << "(NULL) ";
						else
						{
							if( strlen(row2[iField])>30 )
								row2[iField][30]=0;
							char *pCRLF = strchr(row2[iField],'\n');
							if( pCRLF )
								*pCRLF = 0;
							cout << row2[iField] << " ";
						}
					}
					cout << endl;
				}
			}
		}
	}
	cout << endl;
}

void Database::Revert()
{
	StartTransaction();
	for(map< pair<string,string>, MaskedChange *>::iterator it=g_GlobalConfig.m_mapMaskedChanges.begin();
		it!=g_GlobalConfig.m_mapMaskedChanges.end();++it)
	{
		MaskedChange *pMaskedChange = it->second;
		if( pMaskedChange->m_eTypeOfChange==toc_New )
		{
			std::ostringstream sSQL;
			sSQL << "DELETE FROM " << pMaskedChange->m_pTable->Name_get() << it->first.second;

			if( threaded_mysql_query( sSQL.str( ) )<0 )
			{
				cerr << "Revert change: " << sSQL.str( ) << endl;
				throw "Database error";
			}
		}
		else
			pMaskedChange->m_pTable->RevertChange(pMaskedChange->m_psc_id,pMaskedChange->m_eTypeOfChange);
	}
	Commit();  // an exception would have been thrown if something went wrong
}

void Database::ChangeKey()
{
	Table *pTable = NULL;
	if( g_GlobalConfig.m_mapTable.size( )!=1 )
	{
		string sTable;
		cout << "What is the table?";
		cin >> sTable;
		pTable = m_mapTable_Find(sTable);
	}
	else
		pTable = g_GlobalConfig.m_mapTable.begin()->second;

	if( !pTable )
		throw "Invalid table";

	if( pTable->m_listField_PrimaryKey.size()!=1 )
		throw "Table does not have 1 sole primary key";

	if( g_GlobalConfig.m_sPK_Old.size()==0 )
	{
		cout << "What is the current value? ";
		cin >> g_GlobalConfig.m_sPK_Old;
	}

	if( g_GlobalConfig.m_sPK_New.size()==0 )
	{
		cout << "What is the new value? ";
		cin >> g_GlobalConfig.m_sPK_New;
	}

	if( g_GlobalConfig.m_sPK_Old.size()==0 || g_GlobalConfig.m_sPK_New.size()==0 )
		throw "Bad primary keys";

	Field *pField = *(pTable->m_listField_PrimaryKey.begin());

	string sSQL = "SELECT psc_id FROM `" + pTable->m_sName + "` WHERE `" + pField->Name_get() + "`='" + StringUtils::SQLEscape(g_GlobalConfig.m_sPK_Old) + "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=mysql_query_result(sSQL) )==0 || result_set.r->row_count!=1 )
		throw "Primary key invalid";

	StartTransaction();
	sSQL = "UPDATE `" + pTable->m_sName + "` SET `" + pField->Name_get() + "`='" + StringUtils::SQLEscape(g_GlobalConfig.m_sPK_New) + "' WHERE `" + pField->Name_get() + "`='" + StringUtils::SQLEscape(g_GlobalConfig.m_sPK_Old) + "'";
	int iRows=threaded_mysql_query(sSQL);
	if( iRows!=1 )
		throw "Could not update primary key";

	pTable->PropagateUpdatedField(pField,g_GlobalConfig.m_sPK_New,g_GlobalConfig.m_sPK_Old,NULL);
	Commit();  // an exception would have been thrown if something went wrong
}

void Database::RollbackBatch()
{
	if( g_GlobalConfig.m_mapRepository.size()==0 && PromptForRepositories(true)!=1 )
		return;  // User must have chosen quit

	if( g_GlobalConfig.m_psc_batch==0 )
	{
		cout << "What batch number? (0 to quit) ";
		cin >> g_GlobalConfig.m_psc_batch;
		if( g_GlobalConfig.m_psc_batch==0 )
			return; /**< Do nothing */
	}

	if( g_GlobalConfig.m_mapRepository.size( )!=1 )
	{
		cerr << "You must specify 1 and only 1 repository for the batch to rollback" << endl;
		throw "Parameter error";
	}

	Repository *pRepository = (*g_GlobalConfig.m_mapRepository.begin()).second;
	pRepository->RollbackBatch();
}
