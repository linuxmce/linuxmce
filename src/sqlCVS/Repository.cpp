/*
 Repository
 
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
 * @file Repository.cpp
 * @brief source file for the Repository class
 *
 */
 
  
#include "PlutoUtils/CommonIncludes.h"	

#include "CommonFunctions.h"
#include <iostream>
#include <sstream>

#include "Database.h"
#include "Repository.h"
#include "Table.h"

#include "RA/RA_Processor.h"
#include "R_CommitChanges.h"
#include "R_UpdateRepository.h"
#include "R_CloseTransaction.h"
#include "R_ApproveBatch.h"
#include "A_UpdateSchema.h"
#include "ChangedRow.h"
#include "sqlCVSprocessor.h"

#include <iomanip>
#include <sstream>

using namespace sqlCVS;

void Repository::AddDefinitionTable( class Table *pTable )
{
	if( pTable->GetTrailingString( )=="repset" )
		m_pTable_Setting = pTable;
	else if( pTable->GetTrailingString( )=="bathdr" )
		m_pTable_BatchHeader = pTable;
	else if( pTable->GetTrailingString( )=="batuser" )
		m_pTable_BatchUser = pTable;
	else if( pTable->GetTrailingString( )=="batdet" )
		m_pTable_BatchDetail = pTable;
	else if( pTable->GetTrailingString( )=="tables" )
		m_pTable_Tables = pTable;
	else if( pTable->GetTrailingString( )=="schema" )
		m_pTable_Schema = pTable;
	else
	{
		cerr << "Cannot figure out what table: " << pTable->Name_get( ) << " is.";
		throw( "Schema problem" );
	}

	/**  We found a match, and this is a system table */
	pTable->SetRepository( this, true );
}

void Repository::MatchUpTables( )
{
	if( m_pTable_Setting==NULL || m_pTable_BatchHeader==NULL || m_pTable_BatchUser==NULL || m_pTable_BatchDetail==NULL || m_pTable_Tables==NULL || m_pTable_Schema==NULL )
	{
		cerr << "Could not find all the system tables for Repository: " << m_sName << endl
			<< "I need _repset, _bathdr, _batuser, _batdet, _tables" << endl
			<< "This repository has been corrupted, and will need to be rebuilt in order to continue." << endl;

		if( g_GlobalConfig.m_sCommand=="import" )
			return;

		if( !g_GlobalConfig.m_bNoPrompts && AskYNQuestion("Rebuild repository: " + m_sName,false) )
			ResetSystemTables();
		else
			throw "Repository corrupt";
	}

	string Tablename = "psc_" + m_sName + "_tables";

	ostringstream sql;
	sql << "SELECT Tablename,filter,frozen,anonymous FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			Table *pTable = m_pDatabase->m_mapTable_Find( row[0] );
			if( !pTable )
			{
				if( g_GlobalConfig.m_sCommand=="import" )
					continue;
				cerr << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it no longer exists in the database." << endl
					<< "I must delete it from the repository's database to continue." << endl;

				cout << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it no longer exists in the database." << endl
					<< "I must delete it from the repository's database to continue." << endl;

				if( !AskYNQuestion("Delete the bad data, and cleanup the repository table?",false) )
					throw "Repository corrupt";

				sql.str( "" );
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
			    m_pDatabase->threaded_mysql_query( sql.str( ) );
				continue; /**< Nothing more to do */
			}
			if( pTable->Repository_get( ) && pTable->Repository_get( )!=this )
			{
				if( g_GlobalConfig.m_sCommand=="import" )
					continue;
				cerr << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it already exists in the repository: " << pTable->Repository_get( )->Name_get( ) << endl
					<< "I must delete it from this repository's database to continue." << endl;

				cout << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it already exists in the repository: " << pTable->Repository_get( )->Name_get( ) << endl
					<< "I must delete it from this repository's database to continue." << endl;

				if( !AskYNQuestion("Delete the bad data, and cleanup the repository table?",false) )
					throw "Repository corrupt";

				sql.str( "" );
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
			    m_pDatabase->threaded_mysql_query( sql.str( ) );
				continue; /**< Nothing more to do */
			}
			/** Go ahead and add the table */
			m_mapTable[ pTable->Name_get( ) ] = pTable;
			pTable->SetRepository( this, false );
			pTable->m_sFilter = row[1] ? row[1] : "";
			pTable->m_bFrozen = row[2] && row[2][0]=='1';
			pTable->m_bAnonymous = row[3] && row[3][0]=='1';
		}
	}
}

void Repository::Setup( )
{
	if( !m_pTable_Setting )
		m_pTable_Setting = CreateSettingTable( );
	if( !m_pTable_BatchHeader )
		m_pTable_BatchHeader = CreateBatchHeaderTable( );
	if( !m_pTable_BatchUser )
		m_pTable_BatchUser = CreateBatchUserTable( );
	if( !m_pTable_BatchDetail )
		m_pTable_BatchDetail = CreateBatchDetailTable( );
	if( !m_pTable_Tables )
		m_pTable_Tables = CreateTablesTable( );
	if( !m_pTable_Schema )
		m_pTable_Schema = CreateSchemaTable( );

	string Tablename = "psc_" + m_sName + "_tables"; /**< Our _tables table */

	/** Go through all the tables in the map and be sure we have them in our tracking table, and also in our internal map */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		pTable->SetRepository( this, false );
		pTable->TrackChanges_set( true );
		m_mapTable[ pTable->Name_get( ) ] = pTable;
		ostringstream sql;

		sql << "SELECT PK_" << Tablename << " FROM `" << Tablename << "` WHERE Tablename='" << pTable->Name_get( ) << "'";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( !( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) || !( row = mysql_fetch_row( result_set.r ) ) )
		{
			sql.str( "" );
			sql << "INSERT INTO `" << Tablename << "` ( Tablename ) VALUES ( '" << pTable->Name_get( ) << "' )";
			if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
			{
				cerr << "SQL failed: " << sql.str( ) << endl;
				throw "Database Error";
			}
		}
	}

	/**
	 * If we have any tables in our existing internal map that aren't in this new list, remove them. 
	 * They're not part of the repository any more 
	 */
	
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( ); )
	{
		if( g_GlobalConfig.m_mapTable.find( ( *it ).first ) == g_GlobalConfig.m_mapTable.end( ) )
		{
			( *it ).second->TrackChanges_set( false );
			m_mapTable.erase( it++ ); /**< This isn't in the repository any more */
		}
		else
			it++;
	}

	/** If we have any tables in the _tables table that aren't in this new list, remove them. They're not part of the repository any more */
	
	ostringstream sql;
	sql << "SELECT Tablename FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			if( g_GlobalConfig.m_mapTable.find( row[0] ) == g_GlobalConfig.m_mapTable.end( ) )
			{
				sql.str( "" );
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
			    m_pDatabase->threaded_mysql_query( sql.str( ) );
			}
		}
	}
}

string Repository::GetSetting(string Setting,string Default)
{
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql	<< "SELECT Value FROM `" << Tablename << "` WHERE Setting='" + StringUtils::SQLEscape(Setting) + "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) && ( row = mysql_fetch_row( result_set.r ) ) )
		return row[0];

	if( Default.length() )
	{
		sql.str("");
		sql	<< "INSERT INTO `" << Tablename << "` (Setting,Value) VALUES ('" + StringUtils::SQLEscape(Setting) + "','"  + StringUtils::SQLEscape(Default) + "')";
		if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
		{
			cerr << "SQL Failed: " << sql.str( ) << endl;
			throw "Database error";
		}
	}

	return Default;
}

void Repository::SetSetting(string Setting,string Value)
{
	GetSetting(Setting,Value); // This will add the record if it's not already there

	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql	<< "UPDATE `" << Tablename << "` SET Setting='" + StringUtils::SQLEscape(Setting) + "',Value='"  + StringUtils::SQLEscape(Value) + "' WHERE Setting='" + StringUtils::SQLEscape(Setting) + "'";
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}
}

Table *Repository::CreateSettingTable( )
{
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Setting` varchar( 30 ) NOT NULL default '', " << endl
		<< "`Value` text default '', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

Table *Repository::CreateSchemaTable( )
{
	string Tablename = "psc_" + m_sName + "_schema";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Value` text NOT NULL default '', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}
class Table *Repository::CreateBatchHeaderTable( )
{
	string Tablename = "psc_" + m_sName + "_bathdr";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`IPAddress` varchar( 16 ), " << endl
		<< "`date` datetime, " << endl
		<< "`comments` text, " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

class Table *Repository::CreateBatchUserTable( )
{
	string Tablename = "psc_" + m_sName + "_batuser";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr` int( 11 ) NOT NULL, " << endl
		<< "`psc_user` int( 11 ) NOT NULL, " << endl
		<< "`is_sup` tinyint( 1 ) NOT NULL, " << endl
		<< "`no_pass` tinyint( 1 ) NOT NULL, " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

class Table *Repository::CreateBatchDetailTable( )
{
	string Tablename = "psc_" + m_sName + "_batdet";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr` int( 11 ) NOT NULL, " << endl
		<< "`Tablename` varchar( 60 ) NOT NULL default '', " << endl
		<< "`New` int( 11 ) NOT NULL, " << endl
		<< "`Deleted` int( 11 ) NOT NULL, " << endl
		<< "`Modified` int( 11 ) NOT NULL, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr_orig` int( 11 ) NOT NULL, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr_auth` int( 11 ) NOT NULL, " << endl
		<< "`FK_psc_" + m_sName + "_bathdr_unauth` int( 11 ) NOT NULL, " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

class Table *Repository::CreateTablesTable( )
{
	string Tablename = "psc_" + m_sName + "_tables";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	sql.str( "" );
	sql	<< "CREATE TABLE `" << Tablename << "`( " << endl
		<< "`PK_" << Tablename << "` int( 11 ) NOT NULL auto_increment, " << endl
		<< "`Tablename` varchar( 60 ) NOT NULL default '', " << endl
		<< "`filter` varchar( 255 ), " << endl
		<< "`frozen` TINYINT( 1 ) NOT NULL DEFAULT '0', " << endl
		<< "`anonymous` TINYINT( 1 ) NOT NULL DEFAULT '0', " << endl
		<< "`last_psc_id` INT( 11 ) NOT NULL DEFAULT '0', " << endl
		<< "`last_psc_batch` INT( 11 ) NOT NULL DEFAULT '0', " << endl
		<< "PRIMARY KEY ( `PK_" << Tablename << "` )" << endl
		<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Table *pTable = new Table( m_pDatabase, Tablename );
	pTable->SetRepository(this,true);
	return pTable;
}

void Repository::Remove( )
{
	ostringstream sql;
	string Tablename = "psc_" + m_sName + "_repset";
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_bathdr";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_batuser";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_batdet";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	Tablename = "psc_" + m_sName + "_tables";
	sql.str( "" );
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
	{
		cerr << "SQL Failed: " << sql.str( ) << endl;
		throw "Database error";
	}

	m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchDetail=m_pTable_Tables=m_pTable_Schema=NULL;
}

bool Repository::DetermineDeletions( )
{
	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!!  */
	RA_Processor ra_Processor( ClientID, SoftwareVersion, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );

	// Don't need to create a transaction since we're not changing anything.

	DCE::Socket *pSocket=NULL;

	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		/**
		 * Since we don't need to connect to the server for anything here, but we don't each table to make it's own connection, 
		 * Pass in the connection string and the NULL pointer to the socket so a connection will be made the
		 * first time and preserved for the other tables
		 */
		 
		if( pTable->Repository_get( )==this && !pTable->DetermineDeletions( ra_Processor, g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) )
		{
			delete pSocket;
			return false;
		}
	}

	// Don't need to close since we didn't change anything

	delete pSocket;
	return true; /**< We succeeded */
}

#pragma warning( "as you add a table to a repository, automatically add all related tables which depend on it, or it depends on them--no, what about constants that are shared???--put warnings next to them" )

bool Repository::CheckIn( )
{
	if( g_GlobalConfig.m_mapUsersPasswords.size()==0 )
		throw "No users"; // Shouldn't happen

	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );

	/** An exception will be thrown and a roll back called if this falls out of scope and hasn't been committed or rolled back */
	SafetyTransaction st( m_pDatabase );

	R_CommitChanges r_CommitChanges( m_sName, g_GlobalConfig.m_sDefaultUser, g_GlobalConfig.m_sComments, GetSchemaVersion() );
	for(MapStringString::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
	{
		// We don't care about user 0, or users who didn't log in
		if( (*it).first=="0" || (*it).second=="" )
			continue;
		r_CommitChanges.m_mapUsersPasswords[(*it).first]=(*it).second;
	}

	ra_Processor.AddRequest( &r_CommitChanges );
	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests( g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) );

	if( r_CommitChanges.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_CommitChanges ); /**< It's going to fall out of scope, so we don't want the processor to retry */
		
		cerr << "Unable to commit repository to server:" << (int) r_CommitChanges.m_cProcessOutcome << endl;
		delete pSocket;
		pSocket=NULL;
		if( r_CommitChanges.m_cProcessOutcome==LOGIN_FAILED )
		{
			cout << "The username or password was incorrect." << endl;
			if( !g_GlobalConfig.m_bNoPrompts && AskYNQuestion("Do you want to re-enter the passwords and try again?",false) )
			{
				for(MapStringString::iterator it=g_GlobalConfig.m_mapUsersPasswords.begin();it!=g_GlobalConfig.m_mapUsersPasswords.end();++it)
				{
					if( (*it).first=="0" || (*it).second=="" )
						continue;
					string Password;
					cout << "Enter the password for user " << (*it).first << ": ";
					cin >> Password;
					g_GlobalConfig.m_mapUsersPasswords[(*it).first]=Password;
				}
				return CheckIn();
			}
		}
		return false;
	}

	st.Commit();  // The schema may have been changed, causing the transaction to close.  Start a new one
	SafetyTransaction st2( m_pDatabase );

	/** First handle deletions, since the user may have deleted, and then re-added a row and we don't want to get a 'duplicate key' error on the server */
	for( map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		int psc_user = ( *it ).first;
		MapTable *pMapTable = ( *it ).second;
		for( MapTable::iterator itT=pMapTable->begin( );itT!=pMapTable->end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			if( pTable->Repository_get( )==this && !pTable->CheckIn( psc_user, ra_Processor, pSocket, toc_Delete ) )
			{
				cerr << "Table: " << pTable->Name_get() << " failed to deletes" << endl;
				delete pSocket;
				return false;
			}
		}
	}

	/** Then add all records */
	for( map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		int psc_user = (*it).first;
		MapTable *pMapTable = ( *it ).second;
		for( MapTable::iterator itT=pMapTable->begin( );itT!=pMapTable->end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			if( pTable->Repository_get( )==this && !pTable->CheckIn( psc_user, ra_Processor, pSocket, toc_New ) )
			{
				cerr << "Table: " << pTable->Name_get() << " failed to checkin adds" << endl;
				delete pSocket;
				return false;
			}
		}
	}
	
	/** Now scan for updates */
	for( map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		int psc_user = ( *it ).first;
		MapTable *pMapTable = ( *it ).second;
		for( MapTable::iterator itT=pMapTable->begin( );itT!=pMapTable->end( );++itT )
		{
			Table *pTable = ( *itT ).second;
			if( pTable->Repository_get( )==this && !pTable->CheckIn( psc_user, ra_Processor, pSocket, toc_Modify ) )
			{
				cerr << "Table: " << pTable->Name_get() << " failed to checkin updates" << endl;
				delete pSocket;
				return false;
			}
		}
	}
	
	R_CloseTransaction r_CloseTransaction;
	ra_Processor.AddRequest(&r_CloseTransaction);
	while( ra_Processor.SendRequests( g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) );

	delete pSocket;
	pSocket=NULL;
	if( r_CloseTransaction.m_cProcessOutcome==SUCCESSFULLY_PROCESSED )
	{
		// Do the commit first since the server already confirmed
		// TODO: This is a bit unsafe since it's possible the connection gets dropped the split second
		// after we send the close transaction and before getting the server's reply, meaning we would
		// roll back the transaction and try to re-commit.  Need a fail-safe reply
		st2.Commit();
		ostringstream sql;
		sql << "INSERT INTO `" << m_pTable_BatchHeader->Name_get() << "` (PK_" << m_pTable_BatchHeader->Name_get() << ",date) VALUES (" 
			<< r_CommitChanges.m_psc_batch << ",NOW())";
		if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
		{
			cerr << "Cannot write new batch to file" << endl;
			throw "Database write error";
		}
	}
	else
	{
		cerr << "Failed to close transaction.  Transaction will be rolled back!" << endl;
		return false;
	}

	return true;
}

bool Repository::Update( )
{
	if( !DetermineDeletions( ) )
		return false; /**< This will throw an exception with the transaction */

	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );

	/** An exception will be thrown and a roll back called if this falls out of scope and hasn't been committed or rolled back */
	SafetyTransaction st( m_pDatabase ); 

	R_UpdateRepository r_UpdateRepository( m_sName, GetSchemaVersion() );
	ra_Processor.AddRequest( &r_UpdateRepository );
	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests( g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) );

	if( r_UpdateRepository.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_UpdateRepository ); /**< It's going to fall out of scope, so we don't want the processor to retry */
		cerr << "Unable to update repository: " << (int) r_UpdateRepository.m_cProcessOutcome << endl;
		delete pSocket;
		pSocket=NULL;
		return false;
	}

	st.Commit();  // The schema may have been changed, causing the transaction to close.  Start a new one
	SafetyTransaction st2( m_pDatabase );

	/** Update Tables */
	
	for( MapTable::iterator it=g_GlobalConfig.m_mapTable.begin( );it!=g_GlobalConfig.m_mapTable.end( );++it )
	{
		Table *pTable = ( *it ).second;
		if( pTable->Repository_get( )==this && !pTable->Update( ra_Processor, pSocket ) )
		{
			delete pSocket;
			return false;
		}
	}

	R_CloseTransaction r_CloseTransaction;
	ra_Processor.AddRequest(&r_CloseTransaction);
	while( ra_Processor.SendRequests( g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) );

	delete pSocket;
	pSocket=NULL;

	if( r_CloseTransaction.m_cProcessOutcome==SUCCESSFULLY_PROCESSED )
		st2.Commit();
	else
		cerr << "Failed to close transaction.  Transaction will be rolled back!" << endl;
	return true;
}

void Repository::AddTablesToMap( )
{
	/** Be sure all tables in this repository are in the map we got */
	
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		g_GlobalConfig.m_mapTable[ ( *it ).first ] = ( *it ).second;
}

int Repository::CreateBatch( sqlCVSprocessor *psqlCVSprocessor, map<int,ValidatedUser *> *mapValidatedUsers )
{
	if( !m_pTable_BatchHeader || !m_pTable_BatchUser || !m_pTable_BatchDetail )
	{
		cerr << "Cannot create batch for repository " << m_sName << endl;
		throw "Invalid repository";
	}
	else
	{
		std::ostringstream sSQL;
		sSQL << "INSERT INTO `" << m_pTable_BatchHeader->Name_get( ) << "` ( IPAddress,date,comments ) VALUES ( '" << 
			(psqlCVSprocessor->m_pSocket ? psqlCVSprocessor->m_pSocket->m_sIPAddress : "")<< "',NOW(),'" << StringUtils::SQLEscape(psqlCVSprocessor->m_sComments) << "' )";
		int BatchID = m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) );
		if( !BatchID )
			cerr << "Failed to create batch: " << sSQL.str( ) << endl;
		else
		{
			cout << "Created Batch: " << BatchID << " in repository: " << m_sName << endl;
			for(map<int,ValidatedUser *>::iterator it=mapValidatedUsers->begin();it!=mapValidatedUsers->end();++it)
			{
				ValidatedUser *pValidatedUser = (*it).second;
				std::ostringstream sSQL;
				sSQL << "INSERT INTO `" << m_pTable_BatchUser->Name_get( ) << "` ( FK_" 
					<< m_pTable_BatchHeader->Name_get( ) << ",psc_user,is_sup,no_pass ) VALUES ( " << BatchID << "," << (*it).first << "," << 
					(pValidatedUser->m_bIsSupervisor ? "1" : "0") << "," << (pValidatedUser->m_bWithoutPassword ? "1" : "0") << " )";
				int BatchID = m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) );

			}
		}
		return BatchID;
	}
	return 0;
}

int Repository::psc_id_last_sync_get( Table *pTable )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		return -1;

	std::ostringstream sSQL;
	sSQL << "SELECT last_psc_id FROM " << m_pTable_Tables->Name_get( ) << " WHERE Tablename='" << pTable->Name_get( ) << "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) )==NULL || ( row = mysql_fetch_row( result_set.r ) )==NULL )
	{
		cerr << "Cannot get last_psc_id for " << pTable->Name_get( ) << endl;
		throw "Internal error";
	}
	return row[0] ? atoi( row[0] ) : 0;
}

void Repository::psc_id_last_sync_set( Table *pTable, int psc_id )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		throw "Internal error Repository::psc_id_set";

cout << "psc_id_last_sync_set: table: " << pTable->Name_get() << " psc_id: " << psc_id << endl;

	std::ostringstream sSQL;
	sSQL << "UPDATE `" << m_pTable_Tables->Name_get( ) << "` SET last_psc_id='" << psc_id << "' WHERE Tablename='" << pTable->Name_get( ) << "'";
	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "SQL failed: " << sSQL.str( );
		throw "Internal error Repository::psc_id_set #2";
	}
	pTable->m_psc_id_last_sync=psc_id;
}

int Repository::psc_batch_last_sync_get( Table *pTable )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
		return -1;

	std::ostringstream sSQL;
	sSQL << "SELECT last_psc_batch FROM " << m_pTable_Tables->Name_get( ) << " WHERE Tablename='" << pTable->Name_get( ) << "'";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) )==NULL || ( row = mysql_fetch_row( result_set.r ) )==NULL )
	{
		cerr << "Cannot get last_psc_batch for " << pTable->Name_get( ) << endl;
		throw "Internal error";
	}
	return row[0] ? atoi( row[0] ) : 0;
}

// See important notes in header
void Repository::psc_batch_last_sync_set( Table *pTable, int psc_batch )
{
	if( pTable->Repository_get( )!=this || !m_pTable_Tables || pTable->bIsSystemTable_get( ) )
	{
		cerr << "Error table: " << pTable->Name_get() << " psc_batch: " << psc_batch << " m_pTable_Tables " << m_pTable_Tables << " system: " << pTable->bIsSystemTable_get( ) << endl;
		throw "Internal error Repository::psc_batch_set";
	}

cout << "psc_batch_last_sync_get: table: " << pTable->Name_get() << " psc_batch: " << psc_batch << endl;

	std::ostringstream sSQL;
	sSQL << "UPDATE `" << m_pTable_Tables->Name_get( ) << "` SET last_psc_batch='" << psc_batch << "' WHERE Tablename='" << pTable->Name_get( ) << "'";
	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "SQL failed: " << sSQL.str( );
		throw "Internal error Repository::psc_batch_set #2";
	}
	pTable->m_psc_batch_last_sync = psc_batch;

	cout << "Setting psc_batch_last_sync for table: " << pTable->Name_get() << " to: " << psc_batch << endl;
}

void Repository::Dump( )
{
	SerializeableStrings str;
	if( (m_pTable_Setting && !m_pTable_Setting->Dump( str )) ||
		(m_pTable_BatchHeader && !m_pTable_BatchHeader->Dump( str )) ||
		(m_pTable_BatchUser && !m_pTable_BatchUser->Dump( str )) ||
		(m_pTable_BatchDetail && !m_pTable_BatchDetail->Dump( str )) ||
		(m_pTable_Tables && !m_pTable_Tables->Dump( str )) ||
		(m_pTable_Schema && !m_pTable_Schema->Dump( str )) )
	{
		cerr << "Failed to output system table data for dump!" << endl;
		return; /**< Just return without doing anything */
	}

	str.m_vectString.push_back( StringUtils::itos( ( int ) m_mapTable.size( ) ) );
	for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
	{
			Table *pTable = ( *it ).second;
		if( !pTable->Dump( str ) )
		{
			cerr << "Failed to output table data for dump!" << endl;
			return; /**< Just return without doing anything */
		}
	}

	str.SerializeWrite( );
	FILE *file = fopen( ( m_sName + ".sqlcvs" ).c_str( ), "wb" );
	if( !file )
		throw "Unable to write";

	size_t Size = str.CurrentSize( );
	size_t BytesOut = fwrite( str.m_pcDataBlock, 1, Size, file );
	str.FreeSerializeMemory( );
	fclose( file );
	if( BytesOut!=Size )
		throw "Out of disk space";
	return; /**< Just return without doing anything */
}

void Repository::ListTables( )
{
	cout << m_sName << endl;
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		cout << "\t" << pTable->Name_get() << endl;
	}
}

void Repository::ImportTable(string sTableName,SerializeableStrings &str,size_t &pos,Table *pTable,int ipsc_id_last,int ipsc_batch_last)
{
	int NumFields = atoi( str.m_vectString[pos++].c_str( ) );

	if( !pTable )
	{
		if( m_pDatabase->threaded_mysql_query( "DROP TABLE IF EXISTS `" + sTableName + "`" )<0 )
		{
			cerr << "Could not drop table " << sTableName << endl;
			throw "Database error";
		}
	}
	map<string,int> mapFields;
	list<string> listFields;
	int iField_psc_id = -1, iField_psc_batch = -1;
	int psc_id_last=0,psc_batch_last=0;

	// We will go through the create table process even if
	// bCleanImport is false and we're not going to use it because we need
	// to pull of the correct strings, and we need the mapFields member
	// populated so we can be sure the schema is correct
	std::ostringstream sSQL;
	sSQL << "CREATE TABLE `" << sTableName << "` ( ";
	string sPrimaryKey;
	bool bContainsAutoIncrement=false;
	for( int j=0;j<NumFields;++j )
	{
		string sField = str.m_vectString[pos++];
		string sType = str.m_vectString[pos++];
		string sNULL = str.m_vectString[pos++];
		string sIndex = str.m_vectString[pos++];
		string sDefault = str.m_vectString[pos++];
		string sExtra = str.m_vectString[pos++];

		// Don't allow defaulting to NULL when importing with NULL as not allowed.  This worked in 0710, but not in 0804
		if( sNULL!="YES" && sDefault == NULL_TOKEN )
			sDefault = "";

		if( sExtra=="auto_increment" )
			bContainsAutoIncrement=true;

		if( sField=="psc_id" )
			iField_psc_id = j;
		if( sField=="psc_batch" )
			iField_psc_batch = j;
		mapFields[sField]=j;
		listFields.push_back(sField);  // We need both a map and a list

		if( j!=0 )
			sSQL << ", ";
		if( sIndex=="PRI" )
			sPrimaryKey += ( sPrimaryKey.length( ) ? "," : "" ) + string( "`" ) + sField + "`";
		sSQL << "`" << sField << "` " << sType
			<< ( sNULL!="YES" ? " NOT NULL " : "" );

		if( sDefault.length( ) )
		{
			string sQuotedDefault;
			if (sDefault == "CURRENT_TIMESTAMP")
			{
				sQuotedDefault = "CURRENT_TIMESTAMP";
				if( sField=="psc_mod" )
					sQuotedDefault += " on update CURRENT_TIMESTAMP";
			}
			else
				sQuotedDefault = "'" + sDefault + "'";

			sSQL << " default " << ( sDefault==NULL_TOKEN ? "NULL" : sQuotedDefault );
		}
		// need to ensure that NOT NULL varchar fields have a default or db import will fail.
		else
		{
			if( sNULL!="YES" && sType.find("varchar")!=string::npos )
			{
				sSQL << " default ''";
			}
		}
		sSQL << " " << sExtra;
	}
	if( sPrimaryKey.length( ) )
		sSQL << ", PRIMARY KEY( " << sPrimaryKey << " )";

//	sSQL << " ) TYPE=InnoDB";
	sSQL << " ) ";

	map<int,int> map_id_mod,map_id_new;
	if( !pTable )
	{
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "SQL Failed: " << sSQL.str( ) << endl;
			throw "Database error";
		}
	}
	else
	{
		// Confirm the schema is correct
		for( map<string,int>::iterator it=mapFields.begin();it!=mapFields.end();++it )
		{
			string sField = (*it).first;
			if( !pTable->m_mapField_Find(sField) && sField!="psc_restrict" )  // temp hack since this was added later
			{
				cerr << "Cannot import.  Field: " << sField << " doesn't  exist in current table." << sTableName << endl;
				throw "Schema error";
			}
		}
		// Confirm the schema is correct
		for( MapField::iterator it=pTable->m_mapField.begin();it!=pTable->m_mapField.end();++it )
		{
			Field *pField = (*it).second;
			if( mapFields.find(pField->Name_get())==mapFields.end() && pField->Name_get()!="psc_restrict" )  // temp hack since this was added later
			{
				cerr << "Cannot import.  Field: " << pField->Name_get() << " doesn't  exist in import." << sTableName << endl;
				throw "Schema error";
			}
		}

		sSQL.str( "" );
		sSQL << "SELECT psc_id,psc_mod FROM `" << sTableName << "`";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		{
			while( (row = mysql_fetch_row( result_set.r ) ) )
				if( row[0] )  // Ignore records that were added on the local database
					map_id_mod[atoi(row[0])] = row[1] ? atoi(row[1]) : 0;
		}
	}

	int NumIndexes = atoi( str.m_vectString[pos++].c_str( ) );
	for( int i=0;i<NumIndexes;++i )
	{
		sSQL.str( "" );
		sSQL << "ALTER TABLE `" << sTableName << "` " << str.m_vectString[pos++] << ";";
		m_pDatabase->threaded_mysql_query( sSQL.str( ), true); // Don't worry about errors -- the index may already be in here
	}

	if( iField_psc_id!=-1 )
	{
		// If this has an auto-increment field, it's possible the user added
		// some records locally with an ID that will conflict with a new import.  If so
		// we have to relocate those records.  We'll keep a list of all such records
		if( pTable && pTable->m_pField_AutoIncrement )
		{
			sSQL.str( "" );
			sSQL << "SELECT " << pTable->m_pField_AutoIncrement->Name_get() << ",psc_id FROM `" << sTableName << "` WHERE `psc_id` IS NULL OR `psc_batch`<0";
			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
			{
				while( (row = mysql_fetch_row( result_set.r ) ) )
					map_id_new[atoi(row[0])] = row[1] ? atoi(row[1]) : 0;
			}
		}
	}

	bool bCheckForUpdate = map_id_mod.size()!=0;  // This is true if already have records in the database with psc_id's
	int i_psc_id_prior=0; // Keep track of the prior psc_id so we can see if any records were deleted on the server
	int NumRows = atoi( str.m_vectString[pos++].c_str( ) );
	cout << "Importing table: " << sTableName << " (" << m_sName << ") " << NumRows << " rows" << endl;
	for( int j=0;j<NumRows;++j )
	{
		if( j && j % 500 == 0 )
			cout << "Completed " << j << " rows out of " << NumRows << endl;

		bool bUpdate=false; // Make this an update rather than an insert
		int i_psc_id = iField_psc_id!=-1 ? atoi(str.m_vectString[pos+iField_psc_id].c_str()) : 0; // The psc_id of the row we're importing
if( sTableName=="DeviceTemplate" && (i_psc_id==54) )
{
int k=2;
}
		int i_psc_batch = iField_psc_batch!=-1 ? atoi(str.m_vectString[pos+iField_psc_batch].c_str()) : 0; // The psc_batch of the row we're importing
		if( i_psc_id>psc_id_last )
			psc_id_last=i_psc_id;
		if( i_psc_batch>psc_batch_last )
			psc_batch_last=i_psc_batch;

		// Delete any psc_id's that were since deleted on the server
		for(int ipsc_id_deleted=i_psc_id_prior+1;ipsc_id_deleted<i_psc_id;ipsc_id_deleted++)
		{
			// The user modified a row that we need to delete
			if( bCheckForUpdate && map_id_mod.find(ipsc_id_deleted)!=map_id_mod.end() && map_id_mod[ipsc_id_deleted] )
			{
				// First see if this is an unauthorized batch.  If so, don't bother asking
				sSQL.str( "" );
				sSQL << "SELECT psc_id FROM `" << sTableName << "` WHERE psc_id=" << ipsc_id_deleted 
					<< " AND psc_batch<0";
				PlutoSqlResult result_set;
				if( !( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) || result_set.r->row_count==0 )
				{
					cout << endl << "***Warning*** While importing into table: " << sTableName << " pscid: " << ipsc_id_deleted << endl;
					cout << "You modified the row that was deleted on the server." << endl;
					if( g_GlobalConfig.m_bNoPrompts )
					{
						cerr << "No prompts specified.  Not delete changes" << endl;
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"While importing into table: %s pscid: %d was deleted.  Skipping",sTableName.c_str(),i_psc_id);
						continue;
					}
					if( !AskYNQuestion("Delete anyway and lose your changes?",false) )
					{
						cerr << "Local database modified deleted rows on server" << endl;
						continue;
					}
				}
			}

			// Only if psc_batch is null (ie an original record) or >0 and <=last_batch from table
			// Don't want to delete stuff that's newer locally
			sSQL.str( "" );
			sSQL << "DELETE FROM `" << sTableName << "` WHERE psc_id=" << ipsc_id_deleted 
				<< " AND (psc_batch is NULL OR psc_batch=0 OR (psc_batch>0 AND psc_batch<=" << ipsc_batch_last << ")) AND " << g_GlobalConfig.GetRestrictionClause(sTableName);
			if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
			{
				cerr << "SQL Failed: " << sSQL.str( ) << endl;
				throw "Database error";
			}
			int iRows = mysql_affected_rows(m_pDatabase->m_pMySQL);
			if( iRows )
				cout << "Deleting row from import " << ipsc_id_deleted << " prior: " << i_psc_id_prior << " ipsc_id:" << i_psc_id << " last batch: " << ipsc_batch_last << endl;
		}

		if( bCheckForUpdate )
		{
			// See if a record with this psc_id already exists, only if we didn't create the table new
			if( map_id_mod.find(i_psc_id)!=map_id_mod.end() )
			{
				bUpdate=true;

				if( map_id_mod[i_psc_id] )
				{
					cout << endl << "***Warning*** While importing into table: " << sTableName << " pscid: " << i_psc_id << endl;
					cout << "You modified the row that is being re-imported." << endl;
					char Response;
					if( g_GlobalConfig.m_bNoPrompts )
					{
						cerr << "No prompts specified.  Not overwriting changes" << endl;
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"While importing into table: %s pscid: %d was modified.  Skipping",sTableName.c_str(),i_psc_id);
						Response='n';
					}
					else
						Response = AskMCQuestion("Overwrite your changes (yes,no,abort)?","yna");
					if( Response=='a' )
						exit (1);
					else if( Response=='n' )
					{
						pos += listFields.size();
						i_psc_id_prior = i_psc_id;
						continue;
					}
				}
			}
		}

		sSQL.str( "" );
		if( bUpdate )
			sSQL << "UPDATE `" << sTableName << "` SET ";
		else
		{
			sSQL << "INSERT INTO `" << sTableName << "` (";
			bool bFirst=true;
			for(list<string>::iterator it=listFields.begin();it!=listFields.end();++it)
			{
				if( bFirst )
					bFirst = false;
				else
					sSQL << ",";
				sSQL << "`" << *it << "`";
			}
			sSQL << ") VALUES ( ";
		}
		bool bFirst=true;
		for(list<string>::iterator it=listFields.begin();it!=listFields.end();++it)
		{
			if( bFirst )
				bFirst = false;
			else
				sSQL << ",";

			string Value = str.m_vectString[pos++];
			if ( *it == "psc_mod" )
				Value = NULL_TOKEN;

			if( bUpdate )
				sSQL << " `" << *it << "`=";
			else if( pTable && pTable->m_pField_AutoIncrement && map_id_new.size() && *it == pTable->m_pField_AutoIncrement->Name_get() && map_id_new.find( atoi(Value.c_str()) )!=map_id_new.end() )
			{
				int psc_id_local = map_id_new[atoi(Value.c_str())]; //i_psc_id
				// We're inserting a new row added from the master table, but there's already a row in the local table
				// with the same auto increment ID.  We're going to have to relocate our local one
				int PK_New = pTable->ReassignAutoIncrValue(atoi(Value.c_str()));
				map_id_new[PK_New] = psc_id_local;  // May need to move it again
			}

			if( Value==NULL_TOKEN )
				sSQL << "NULL";
			else
				sSQL << "'" << StringUtils::SQLEscape( Value ) << "'";
		}
		if( bUpdate )
		{
			// Only if psc_batch is null (ie an original record) or >0 and <=last_batch from table
			// Don't want to update stuff that's newer locally
			sSQL << " WHERE psc_id=" << i_psc_id
				<< " AND (psc_batch is NULL OR psc_batch=0 OR (psc_batch>0 AND psc_batch<=" << ipsc_batch_last << "))";
		}
		else
			sSQL << " )";
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "SQL Failed: " << sSQL.str( ) << endl;
			throw "Database error";
		}
		i_psc_id_prior = i_psc_id;
	}

	if( pTable && psc_batch_last>psc_batch_last_sync_get(pTable) )
		psc_batch_last_sync_set(pTable,psc_batch_last);
	if( pTable && psc_id_last>psc_id_last_sync_get(pTable) )
		psc_id_last_sync_set(pTable,psc_id_last);

}

bool Repository::UpdateSchema(int PriorSchema)
{
	cout << "Updating repository '" << m_sName << "' schema version from: " << PriorSchema << endl;
	std::ostringstream sSQL;
	sSQL << "SELECT PK_psc_" << m_sName << "_schema,Value FROM psc_" << m_sName << "_schema WHERE PK_psc_" << m_sName << "_schema>" << PriorSchema << " ORDER BY PK_psc_" << m_sName << "_schema";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
	{
		if( result_set.r->row_count==0 )
			return false; // nothing to update
		 while( ( row = mysql_fetch_row( result_set.r ) ) ) 
		 {
			 vector<string> vectCommands;
			 string Input=row[1];
			 string Tokens="\r\n";
			 StringUtils::Tokenize(Input,Tokens,vectCommands);
			 for(size_t s=0;s<vectCommands.size();++s)
			 {
				 if( vectCommands[s].length()==0 )
					 continue;
				if( !ProcessSchemaUpdate(vectCommands[s]) )
				{
					cout << "SQL failed: " << vectCommands[s] << endl;
					cerr << "The database is now corrupted, and the schema is out of sync" << endl
						<< "because the schema update failed!" << endl;
					throw "Schema update failed";
				}
			}
			SetSetting("schema",row[0]);
			cout << "Updated repository '" << m_sName << "' schema version to: " << row[0] << endl;
		}
	 	return true; // We updated the schema
	}
	throw "Database error"; // Can't get schema info??
}

bool Repository::ShowChanges()
{
	while(true)
	{
		cout << "    Table                                      New   Mod   Del" << endl;

		vector<Table *> vectTable; // So we can allow the user to pick a table by number
		for( MapTable::iterator it=m_mapTable.begin( );it!=m_mapTable.end( );++it )
		{
			Table *pTable = ( *it ).second;
			vectTable.push_back(pTable);
			cout << setw( 3 ) << vectTable.size() << " ";
			if( (int) pTable->Name_get().length() > 40 )
				cout << pTable->Name_get().substr(0,40);
			else
			{
				cout << pTable->Name_get() 
					<< StringUtils::RepeatChar( ' ', 40  - ( int )pTable->Name_get( ).length( ) );
			}

			int iNew=0,iMod=0,iDel=0;
			for(map<int, ListChangedRow *>::iterator itCR=pTable->m_mapUsers2ChangedRowList.begin();itCR!=pTable->m_mapUsers2ChangedRowList.end();++itCR)
			{
				ListChangedRow *pListChangedRow = (*itCR).second;
				for(ListChangedRow::iterator itLCR=pListChangedRow->begin();itLCR!=pListChangedRow->end();++itLCR)
				{
					ChangedRow *pChangedRow = *itLCR;
					if( pChangedRow->m_bReverted )
						continue;
					if( pChangedRow->m_eTypeOfChange==toc_New )
						iNew++;
					else if( pChangedRow->m_eTypeOfChange==toc_Modify )
						iMod++;
					else if( pChangedRow->m_eTypeOfChange==toc_Delete )
						iDel++;
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

		cout << "What table do you want more detail on?" << endl <<
			"Enter 'b' to go back, 'q' to quit," << endl << 
			"'a' for all tables, 'r' to revert all tables" << endl;

		string sTable;
		cin >> sTable;
		if( sTable=="b" || sTable=="B" )
			return true;
		if( sTable=="q" || sTable=="Q" )
			return false;

		int iTable = atoi(sTable.c_str());
		if( sTable=="a" || sTable=="A" )
		{
			for(size_t s=0;s<vectTable.size();++s)
			{
				Table *pTable = vectTable[ s ];
				if( !pTable->ShowChanges(true) )
					return false;
			}
		}
		else if( sTable=="r" || sTable=="R" )
		{
			if( AskYNQuestion("Revert all changes to all tables?  You will lose all your local changes?",false)==false )
				continue;

			for(size_t s=0;s<vectTable.size();++s)
			{
				Table *pTable = vectTable[ s ];
				if( !pTable->RevertAllChanges() )
					return false;
			}
		}
		else if( iTable < 1 || iTable > (int) vectTable.size() )
			cout << "That is not a valid selection" << endl;
		else
		{
			Table *pTable = vectTable[ iTable-1 ];
			if( !pTable->ShowChanges() )
				return false;
		}
	}
}

void Repository::ResetSystemTables()
{
	// First save the settings in this map
	MapStringString mapSettings;
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql	<< "SELECT Setting,Value FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set.r ) )
		{
			mapSettings[row[0]]=row[1];
		}
	}

	map<int,string> mapSchema;
	sql.str("");
	Tablename = "psc_" + m_sName + "_schema";
	sql	<< "SELECT PK_" << Tablename << ",`Value` FROM `" << Tablename << "`";
	PlutoSqlResult result_set2;
	if( ( result_set2.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set2.r ) )
			mapSchema[ atoi(row[0]) ] = row[1];
	}

	list<string> listTables;
	sql.str("");
	Tablename = "psc_" + m_sName + "_tables";
	sql	<< "SELECT Tablename FROM `" << Tablename << "`";
	PlutoSqlResult result_set3;
	if( ( result_set3.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set3.r ) )
			listTables.push_back(row[0]);
	}

	CreateSettingTable( );
	CreateBatchHeaderTable( );
	CreateBatchUserTable( );
	CreateBatchDetailTable( );
	CreateTablesTable( );
	CreateSchemaTable( );

	for(MapStringString::iterator it=mapSettings.begin();it!=mapSettings.end();++it)
		SetSetting((*it).first,(*it).second);  

	GetSetting("schema","1");  // Be sure we at least have a default schema of 1

	Tablename = "psc_" + m_sName + "_schema";
	for(map<int,string>::iterator it=mapSchema.begin();it!=mapSchema.end();++it)
	{
		sql.str("");
		sql	<< "INSERT INTO `" << Tablename << "` (PK_" << Tablename << ",`Value`) VALUES ("
			<< (*it).first << ",'" << StringUtils::SQLEscape((*it).second) << "');";
		if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
		{
			cerr << "SQL failed: " << sql.str( ) << endl;
			throw "Database Error";
		}
	}

	// We reset all the psc_rep_tables, so put back any tables in there
	for(list<string>::iterator it=listTables.begin();it!=listTables.end();++it)
	{
		Table *pTable = m_mapTable_Find(*it);
		string Tablename = "psc_" + m_sName + "_tables"; /**< Our _tables table */
		ostringstream sql;
		sql << "INSERT INTO `" << Tablename << "` ( Tablename,filter,frozen ) VALUES ( '" << *it << "','" << (pTable ? pTable->m_sFilter : "") 
			<< "'," << (pTable && pTable->m_bFrozen ? "1" : "0") << ")";
		if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
		{
			cerr << "SQL failed: " << sql.str( ) << endl;
			throw "Database Error";
		}
	}
}

bool Repository::ApproveBatch(R_ApproveBatch *pR_ApproveBatch,sqlCVSprocessor *psqlCVSprocessor)
{
	psqlCVSprocessor->m_pRepository=this;
	ostringstream sql;
	if( pR_ApproveBatch->m_bReject )
	{
		sql << "UPDATE psc_" << m_sName << "_batdet SET FK_psc_" << m_sName << "_bathdr_auth=-1"
			<< " WHERE FK_psc_" << m_sName << "_bathdr=" << pR_ApproveBatch->m_psc_batch;

		if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
		{
			cerr << "SQL failed: " << sql.str( ) << endl;
			throw "Database Error";
		}
		return true;
	}

	sql << "SELECT DISTINCT Tablename FROM psc_" << m_sName << "_batdet WHERE FK_psc_" 
		<< m_sName << "_bathdr=" << pR_ApproveBatch->m_psc_batch << " AND FK_psc_" << m_sName << "_bathdr_auth=0";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set.r ) )
		{
			Table *pTable = m_mapTable_Find(row[0]);
			if( !pTable )
			{
				cerr << "Cannot find table: " << row[0] << endl;
				return false;
			}
	
			psqlCVSprocessor->m_pTable=pTable;

			map<int,ChangedRow *> mapChangedRow;
			pTable->GetBatchContents(pR_ApproveBatch->m_psc_batch,mapChangedRow);
			for(map<int,ChangedRow *>::iterator it=mapChangedRow.begin();it!=mapChangedRow.end();++it)
			{
				int psc_id = (*it).first;
				ChangedRow *pChangedRow = (*it).second;

				// If the user is a supervisor, he can do anything, otherwise be sure we're allowed to do this
				if( !psqlCVSprocessor->m_bSupervisor )
				{
					std::ostringstream sSQL;
					sSQL << "SELECT psc_user,psc_frozen FROM " << pChangedRow->m_pTable->Name_get() << " WHERE psc_id=" << psc_id;
					PlutoSqlResult result_set;
					MYSQL_ROW row=NULL;

					// If the row isn't here, that's ok.  It means it's a new row
					if( ( result_set.r=m_pDatabase->mysql_query_result(sSQL.str()) ) && ( row = mysql_fetch_row( result_set.r ) ) )
					{
						if( row[1] && row[1][0]=='1' )
						{
							cout << "Row is frozen" << endl;
							pR_ApproveBatch->m_sMessage = "Not authorized to commit this batch--row is frozen";
							return false;
						}

						int i = row[0] ? atoi(row[0]) : 0;
						if( i && g_GlobalConfig.m_mapValidatedUsers.find(i)==g_GlobalConfig.m_mapValidatedUsers.end() )
						{
							cout << "Not authorized to commit this batch";
							pR_ApproveBatch->m_sMessage = "Not authorized to commit this batch";
							return false;
						}
					}
				}

				if( pChangedRow->m_eTypeOfChange==toc_New )
					psqlCVSprocessor->m_iNew++;
				if( pChangedRow->m_eTypeOfChange==toc_Delete )
					psqlCVSprocessor->m_iDel++;
				if( pChangedRow->m_eTypeOfChange==toc_Modify )
					psqlCVSprocessor->m_iMod++;

				pChangedRow->m_psc_batch = psqlCVSprocessor->m_i_psc_batch;

				pTable->ApplyChangedRow(pChangedRow);
				pTable->AddToHistory(pChangedRow);
			}

			psqlCVSprocessor->RecordChangesToTable();

			sql.str("");
			sql << "UPDATE psc_" << m_sName << "_batdet SET FK_psc_" << m_sName << "_bathdr_auth="
				<< psqlCVSprocessor->m_i_psc_batch << " WHERE FK_psc_" << m_sName << "_bathdr=" << pR_ApproveBatch->m_psc_batch;

			if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
			{
				cerr << "SQL failed: " << sql.str( ) << endl;
				throw "Database Error";
			}

			sql.str("");
			sql << "UPDATE psc_" << m_sName << "_batdet SET FK_psc_" << m_sName << "_bathdr_unauth="
				<< pR_ApproveBatch->m_psc_batch << " WHERE FK_psc_" << m_sName << "_bathdr=" << psqlCVSprocessor->m_i_psc_batch;

			if( m_pDatabase->threaded_mysql_query( sql.str( ) )<0 )
			{
				cerr << "SQL failed: " << sql.str( ) << endl;
				throw "Database Error";
			}
		}
	}
	return true;
}

bool Repository::HasChangedRecords()
{
	for( MapTable::iterator itT=m_mapTable.begin( );itT!=m_mapTable.end( );++itT )
	{
		Table *pTable = ( *itT ).second;
		if( pTable->m_mapUsers2ChangedRowList.size() )
			return true;
	}
	return false;
}

void Repository::UpdateClientSchema(RA_Request *pRA_Request,int iSchemaVersion)
{
	std::ostringstream sSQL;
	sSQL << "SELECT PK_psc_" << m_sName << "_schema,Value FROM psc_" << m_sName << "_schema WHERE PK_psc_" << m_sName << "_schema>" << iSchemaVersion << " ORDER BY PK_psc_" << m_sName << "_schema";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
	{
		if( result_set.r->row_count==0 )
			return; // nothing to update
		 while( ( row = mysql_fetch_row( result_set.r ) ) ) 
		 {
			cout << "UpdateClientSchema for " << m_sName << " to " << row[0] << endl;
			A_UpdateSchema *pA_UpdateSchema = new A_UpdateSchema(m_sName,atoi(row[0]),row[1]);
			pRA_Request->AddActionsToResponse( pA_UpdateSchema );
		}
	 	return; // We updated the schema
	}
	throw "Database error"; // Can't get schema info??
}

bool Repository::ProcessSchemaUpdate(string sSQLCommand)
{
	if( StringUtils::StartsWith(sSQLCommand,"alter table",true) )
	{
		string::size_type pos_start=11;
		while(sSQLCommand[pos_start]==' ')
			pos_start++;

		string sTable;
		string::size_type pos_end;
		if( sSQLCommand[pos_start]=='`' )
		{
			pos_start++;
			pos_end = sSQLCommand.find('`',pos_start);
			if( pos_end==string::npos )
			{
				cout << "ProcessSchemaUpdate can't find ending `" << endl;
				return false;
			}
			sTable=sSQLCommand.substr(pos_start,pos_end-pos_start);
		}
		else
		{
			pos_end = sSQLCommand.find(' ',pos_start);
			if( pos_end==string::npos )
			{
				cout << "ProcessSchemaUpdate can't find ending space" << endl;
				return false;
			}
			sTable=sSQLCommand.substr(pos_start,pos_end-pos_start);
		}

		if( m_mapTable_Find(sTable)==NULL )
		{
			cout << "ProcessSchemaUpdate - table " <<  sTable << " doesn't exist yet" << endl;
			return true; // It's ok - the next import will bring in the latest schema
		}
	}
	if( m_pDatabase->threaded_mysql_query( sSQLCommand )<0 )
	{
		cout << "ProcessSchemaUpdate failed: " << sSQLCommand << endl;
		return false;
	}

	cout << "ProcessSchemaUpdate OK: " << sSQLCommand << endl;
	return true; 
}

void Repository::RollbackBatch()
{
	m_pDatabase->StartTransaction();
	map<int,ValidatedUser *> mapValidatedUsers;
	sqlCVSprocessor c_sqlCVSprocessor(NULL);  // Simulate an incoming transaction so we can use all those functions
	c_sqlCVSprocessor.m_i_psc_batch = CreateBatch( &c_sqlCVSprocessor, &mapValidatedUsers );

	std::ostringstream sSQL;
	sSQL << "SELECT DISTINCT Tablename FROM psc_" << m_sName << "_batdet WHERE FK_psc_" << m_sName << "_bathdr=" << g_GlobalConfig.m_psc_batch;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
	{
		while ( row = mysql_fetch_row( result_set.r ) )
		{
			string sTable = row[0];
			if( g_GlobalConfig.m_mapTable.size() && g_GlobalConfig.m_mapTable.find(sTable)==g_GlobalConfig.m_mapTable.end() )
			{
				cout << "Skipping table: " << sTable << endl;
				continue;
			}

			Table *pTable = m_mapTable_Find(sTable);
			if( !pTable->m_pTable_History )
			{
				cout << "Cannot rollback table: " << sTable << ".  There is no history" << endl;
				if( !g_GlobalConfig.m_bNoPrompts && !AskYNQuestion("Continue",false) )
					throw "Rollback aborted";
				continue;
			}

			std::ostringstream sSQL2;
			sSQL2 << "SELECT psc_id,psc_toc FROM `" 
				<< pTable->m_pTable_History->Name_get() << "` WHERE psc_batch=" << g_GlobalConfig.m_psc_batch;

			PlutoSqlResult result_set2;
			MYSQL_ROW row2=NULL;
			cout << endl << "Table: " << sTable << endl << "=========================" << endl;
			if( ( result_set2.r=m_pDatabase->mysql_query_result( sSQL2.str( ) ) ) )
			{
				while ( row2 = mysql_fetch_row( result_set2.r ) )
					RollbackBatch(pTable,atoi(row2[0]),atoi(row2[1]),g_GlobalConfig.m_psc_batch,&c_sqlCVSprocessor); // Rollback this change
			}
			c_sqlCVSprocessor.m_pTable = pTable;
			c_sqlCVSprocessor.RecordChangesToTable();
		}
	}
	m_pDatabase->Commit();
}

void Repository::RollbackBatch(Table *pTable,int psc_id,int psc_toc,int psc_batch,sqlCVSprocessor *p_sqlCVSprocessor)
{
	int psc_batch_last=0;
	std::ostringstream sSQL;
	sSQL << "SELECT max(psc_batch) FROM `" << pTable->Name_get() << "_pschist` WHERE psc_id=" << psc_id << " AND psc_batch<>" << psc_batch;

	PlutoSqlResult result_batch;
	MYSQL_ROW row=NULL;
	if( ( result_batch.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) && ( row = mysql_fetch_row( result_batch.r ) ) && row[0] )
		psc_batch_last = atoi( row[0] );

	if( psc_batch_last>psc_batch )
	{
		cout << "Table: " << pTable->Name_get() << " psc_id: " << psc_id << " has been changed since batch " << psc_batch <<
			endl << "Skipping..." << endl;
		if( !g_GlobalConfig.m_bNoPrompts && !AskYNQuestion("Continue",false) )
			throw "Rollback aborted";
		return;
	}

	ChangedRow changedRow(pTable,toc_New,psc_id,p_sqlCVSprocessor->m_i_psc_batch);  // Treat this like a changed row
	changedRow.m_psc_id=psc_id;
	if( psc_toc==toc_New )
	{
		changedRow.m_eTypeOfChange=toc_Delete;
		p_sqlCVSprocessor->m_iDel++;
	}
	else
	{
		if( psc_toc==toc_Delete )
		{
			changedRow.m_eTypeOfChange=toc_New;
			p_sqlCVSprocessor->m_iNew++;
		}
		else
		{
			changedRow.m_eTypeOfChange=toc_Modify;
			p_sqlCVSprocessor->m_iMod++;
		}
		sSQL.str("");
		sSQL << "SELECT ";
		bool bFirst=true;
		for( MapField::iterator it=pTable->m_mapField.begin();it!=pTable->m_mapField.end();++it )
		{
			Field *pField = (*it).second;
			if( pField->Name_get()!="psc_batch" )  // temp hack since this was added later
			{
				if( bFirst )
					bFirst=false;
				else
					sSQL << ",";
				sSQL << "`" << pField->Name_get() << "`";
			}
		}
		sSQL << " FROM `" << pTable->m_pTable_History->Name_get() << "` WHERE psc_id=" << psc_id << " AND psc_batch=" << psc_batch_last;
		PlutoSqlResult result_hist;
		if( ( result_hist.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) && ( row = mysql_fetch_row( result_hist.r ) ) )
		{
			int iFieldCount=0;
			for( MapField::iterator it=pTable->m_mapField.begin();it!=pTable->m_mapField.end();++it )
			{
				Field *pField = (*it).second;
				if( pField->Name_get()!="psc_batch" )  // temp hack since this was added later
				{
					changedRow.m_mapFieldValues[pField->Name_get()] = (row[iFieldCount] ? row[iFieldCount] : NULL_TOKEN);
					iFieldCount++;
				}
			}
		}
		else
		{
			cerr << "Cannot retrieve history for rollback Table: " << pTable->Name_get() << " psc_id: " << psc_id << endl;
			throw "Rollback failure";
		}
	}

	pTable->ApplyChangedRow(&changedRow);
	pTable->AddToHistory(&changedRow);
}
