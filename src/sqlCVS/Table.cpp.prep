/*
 Table

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
 * @file Table.cpp
 * @brief source file for the Table class
 *
 */


#include "PlutoUtils/CommonIncludes.h"
#include "Table.h"

#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

#include <iterator>
#include <algorithm>

#include "Field.h"
#include "Database.h"

#include "RA/RA_Processor.h"
#include "R_CommitTable.h"
#include "R_CommitRow.h"
#include "R_GetAll_psc_id.h"
#include "R_GetRow.h"
#include "R_UpdateTable.h"
#include "R_GetHashedTableStats.h"
#include "R_GetConditional_psc_id.h"
#include "A_UpdateRow.h"
#include "sqlCVSprocessor.h"

#include <iomanip>
#include <sstream>

using namespace sqlCVS;

Table::Table( class Database *pDatabase, string sName )
{
itmp_RowsToDelete=0;
	m_pDatabase=pDatabase;
	m_sName=sName;
	m_pRepository=NULL;
	m_pField_id=m_pField_batch=m_pField_user=m_pField_frozen=m_pField_mod=m_pField_restrict=NULL;
	m_pTable_History=m_pTable_History_Mask=m_pTable_WeAreHistoryFor=m_pTable_WeAreHistoryMaskFor=NULL;
	m_pField_AutoIncrement=NULL;
	m_bIsSystemTable=false;
	ValidateTable( );
	GetFields( );
	m_psc_id_next=1;
	m_psc_id_last_sync=m_psc_batch_last_sync=-1;
	if( m_pField_id )
	{
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result("SELECT max( psc_id ) FROM `" + m_sName + "`") ) && ( row = mysql_fetch_row( result_set.r ) ) )
			m_psc_id_next = row[0] ? atoi( row[0] )+1 : 1;
	}
}

void Table::GetFields( )
{
	m_pField_id=m_pField_batch=m_pField_user=m_pField_frozen=m_pField_mod=m_pField_restrict=NULL;

	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
		delete ( *it ).second;
	m_mapField.clear( );
	m_listField_PrimaryKey.clear();

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	if( res.r=mysql_list_fields( m_pDatabase->MYSQL_get( ), m_sName.c_str( ), NULL ) )
	{
		int fields_count = mysql_num_fields( res.r );

		MYSQL_FIELD *mysql_fields = mysql_fetch_fields( res.r );

		for ( int i=0; i<fields_count; i++ )
		{
			Field *pField = new Field( this, mysql_fields+i );
			m_mapField[pField->Name_get( )]=pField;

			if( pField->m_iFlags & PRI_KEY_FLAG )
				m_listField_PrimaryKey.push_back( pField );

			if( pField->m_iFlags & AUTO_INCREMENT_FLAG )
				m_pField_AutoIncrement = pField;

			if( pField->Name_get( )=="psc_id" )
				m_pField_id = pField;
			else if( pField->Name_get( )=="psc_batch" )
				m_pField_batch = pField;
			else if( pField->Name_get( )=="psc_user" )
				m_pField_user = pField;
			else if( pField->Name_get( )=="psc_frozen" )
				m_pField_frozen = pField;
			else if( pField->Name_get( )=="psc_mod" )
				m_pField_mod = pField;
			else if( pField->Name_get( )=="psc_restrict" )
				m_pField_restrict = pField;
		}
	}

	// Temporary code since restrict was added after the fact
	if( !m_pField_restrict && m_pField_id )
	{
		m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_restrict` INT( 11 );" );
		GetFields();
	}
}

string Table::DefinesRepository( )
{
	if( m_sName.substr( 0, 4 )!="psc_" )
		return "";

	return m_sName.substr( 4, m_sName.length( ) - GetTrailingString( ).length( ) - 5 );
}

string Table::GetTrailingString( string sName )
{
	if( sName.length( )==0 )
		sName = m_sName;

	string::size_type lastpos=string::npos, pos=0;
	while( ( pos = sName.find( '_', pos ) ) != string::npos )
		lastpos = pos++; /**< Increment pos after assigning the value */
	return lastpos==string::npos ? "" : sName.substr( lastpos+1 );
}

Table::~Table( )
{
	int k=2;
}

void Table::TrackChanges_set( bool bOn )
{
	if( bOn )
	{
		if( !m_pField_id )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_id` INT( 11 );" );
		if( !m_pField_batch )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_batch` INT( 11 );" );
		if( !m_pField_user )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_user` INT( 11 );" );
		if( !m_pField_frozen )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_frozen` TINYINT( 1 ) default '0';" );
		if( !m_pField_mod )
		{
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_mod` timestamp( 14 ) default 0;" );
			m_pDatabase->threaded_mysql_query( "UPDATE `" + m_sName + "` SET `psc_mod`=0;" );
		}
		if( !m_pField_restrict )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` ADD `psc_restrict` INT( 11 );" );
	}
	else
	{
		if( m_pField_id )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_id`;" );
		if( m_pField_batch )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_batch`;" );
		if( m_pField_user )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_user`;" );
		if( m_pField_frozen )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_frozen`;" );
		if( m_pField_mod )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_mod`;" );
		if( m_pField_restrict )
			m_pDatabase->threaded_mysql_query( "ALTER TABLE `" + m_sName + "` DROP `psc_restrict`;" );
	}
	GetFields( );
}

void Table::HasFullHistory_set( bool bOn )
{
	if( m_bIsSystemTable )
		return; // We don't track these

	if( bOn )
	{
		if( m_pTable_History || m_pTable_History_Mask)
		{
			/** This shouldn't happen */
			cerr << "There is already a history table for: " << m_sName << endl;
			if( !AskYNQuestion("Drop it and make a new one?  You will lose all your history data.",false) )
				return;
			if( m_pTable_History )
			{
				m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschist`;" );
				m_pDatabase->m_mapTable_Remove( m_pTable_History->m_sName );
				delete m_pTable_History;
			}
			if( m_pTable_History_Mask )
			{
				m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschist`;" );
				m_pDatabase->m_mapTable_Remove( m_pTable_History_Mask->m_sName );
				delete m_pTable_History_Mask;
			}
		}
		TrackChanges_set( true ); /** This must be on */
		ostringstream sqlHistory,sqlHistoryMask;
		sqlHistory	<< "CREATE TABLE `" << m_sName << "_pschist`( " << endl;
		sqlHistoryMask	<< "CREATE TABLE `" << m_sName << "_pschmask`( " << endl;

		PlutoSqlResult result_set2;
		MYSQL_ROW row=NULL;
		if( !( result_set2.r=m_pDatabase->mysql_query_result( ( "DESCRIBE `" +m_sName+"`" ).c_str( ) ) ) )
			throw "Error getting table info";

		while( ( row = mysql_fetch_row( result_set2.r ) ) )
		{
			string sField = row[0];
			string sType = row[1];

			sqlHistory << "`" << sField << "` " << sType << ", ";
			if( sField=="psc_batch" || sField=="psc_id" )
				sqlHistoryMask << "`" << sField << "` int(11), ";
			else
				sqlHistoryMask << "`" << sField << "` tinyint(1), ";
		}

		sqlHistory << "psc_toc tinyint(1) DEFAULT 0, ";

		sqlHistory << "PRIMARY KEY ( psc_id, psc_batch )" << endl
			<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
		sqlHistoryMask << "PRIMARY KEY ( psc_id, psc_batch )" << endl
			<< " ) TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;

		if( m_pDatabase->threaded_mysql_query( sqlHistory.str( ) )<0 || m_pDatabase->threaded_mysql_query( sqlHistoryMask.str( ) )<0 )
		{
			cerr << "sqlHistory Failed: " << sqlHistory.str( ) << endl;
			throw "Database error";
		}

		m_pTable_History = new Table( m_pDatabase, m_sName + "_pschist" );
		m_pTable_History->m_pTable_WeAreHistoryFor=this;
		m_pDatabase->m_mapTable_Add( m_pTable_History->m_sName, m_pTable_History );

		m_pTable_History_Mask = new Table( m_pDatabase, m_sName + "_pschmask" );
		m_pTable_History_Mask->m_pTable_WeAreHistoryMaskFor=this;
		m_pDatabase->m_mapTable_Add( m_pTable_History_Mask->m_sName, m_pTable_History_Mask );

		// Now populate it with the initial values
		PlutoSqlResult result_set3;
		if( !( result_set3.r=m_pDatabase->mysql_query_result( ("SELECT * FROM `" + m_sName + "`").c_str( ) ) ) )
			throw "Error getting table history";

		while( ( row = mysql_fetch_row( result_set3.r ) ) )
		{
			ostringstream sSql,sSqlMask;
			sSql << "INSERT INTO `" << m_sName << "_pschist` (";
			sSqlMask << "INSERT INTO `" << m_sName << "_pschmask` (";
			for(int i=0;i<(int) result_set3.r->field_count;++i)
			{
				if( i!=0 )
				{
					sSql << ",";
					sSqlMask << ",";
				}
				sSql << "`" << result_set3.r->fields[i].name << "`";
				sSqlMask << "`" << result_set3.r->fields[i].name << "`";
			}
			sSql << ") VALUES (";
			sSqlMask << ") VALUES (";
			for(int i=0;i<(int) result_set3.r->field_count;++i)
			{
				if( i!=0 )
				{
					sSql << ",";
					sSqlMask << ",";
				}
				if( strcmp(result_set3.r->fields[i].name,"psc_id")==0 || strcmp(result_set3.r->fields[i].name,"psc_batch")==0 )
				{
					if( row[i] )
						sSqlMask << row[i];
					else
						sSqlMask << "0";
				}
				else
					sSqlMask << "1";

				if( row[i] )
					sSql << "'" << StringUtils::SQLEscape(row[i]) << "'";
				else
				{
					if( strcmp(result_set3.r->fields[i].name,"psc_id")==0 || strcmp(result_set3.r->fields[i].name,"psc_batch")==0 )
						sSql << "0";
					else
						sSql << "NULL";
				}
			}
			sSql << ")";
			sSqlMask << ")";
			if( m_pDatabase->threaded_mysql_query(sSql.str())<0 || m_pDatabase->threaded_mysql_query(sSqlMask.str())<0 )
			{
				cerr << "Unable to add current values to history table" << endl;
				throw "Database error";
			}
		}
	}
	else
	{
		if( !g_GlobalConfig.m_bNoPrompts )
		{
			cout << "Are you sure you want to delete your history for " << m_sName << "?" << endl;
			if( !AskYNQuestion("The data will be permanently removed unless you made a backup.",false) )
				return;
		}
		m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschist`;" );
		m_pDatabase->m_mapTable_Remove( m_pTable_History->m_sName );
		delete m_pTable_History;
		m_pTable_History=NULL;

		m_pDatabase->threaded_mysql_query( "DROP TABLE `" + m_sName + "_pschmask`;" );
		m_pDatabase->m_mapTable_Remove( m_pTable_History_Mask->m_sName );
		delete m_pTable_History_Mask;
		m_pTable_History_Mask=NULL;
	}
}

void Table::MatchUpHistory( )
{
	if( GetTrailingString( )=="pschist" )
	{
		string BaseName = m_sName.substr( 0, m_sName.length( )-8 );
		Table *pMainTable = m_pDatabase->m_mapTable_Find( BaseName );
		if( !pMainTable )
		{
			cerr << "Found a history table: " << m_sName << " with no corresponding data table." << endl;
			throw "Database corrupt";
		}

		pMainTable->m_pTable_History = this;
		m_pTable_WeAreHistoryFor = pMainTable;
		m_bIsSystemTable = true;

		// It's possible that we added some new rows that have not yet been authorized, we'll need to
		// be sure we don't re-use those id's
		if( m_psc_id_next > m_pTable_WeAreHistoryFor->m_psc_id_next )
		{
			m_pTable_WeAreHistoryFor->m_psc_id_next = m_psc_id_next;
			cout << "Larger psc_id in history table for: " << m_pTable_WeAreHistoryFor->Name_get()
				<< " setting it to: " << m_psc_id_next << endl;
		}
	}
	if( GetTrailingString( )=="pschmask" )
	{
		string BaseName = m_sName.substr( 0, m_sName.length( )-9 );
		Table *pMainTable = m_pDatabase->m_mapTable_Find( BaseName );
		if( !pMainTable )
		{
			cerr << "Found a history mask table: " << m_sName << " with no corresponding data table." << endl;
			throw "Database corrupt";
		}

		pMainTable->m_pTable_History_Mask = this;
		m_pTable_WeAreHistoryMaskFor = pMainTable;
		m_bIsSystemTable = true;
	}

	if( m_pTable_WeAreHistoryFor || m_pTable_WeAreHistoryMaskFor )
	{
		// Go through all the columns in the other table and be sure we have columns for them in this one too
		Table *pTable = m_pTable_WeAreHistoryFor ? m_pTable_WeAreHistoryFor : m_pTable_WeAreHistoryMaskFor;
		for(MapField::iterator it=pTable->m_mapField.begin();it!=pTable->m_mapField.end();++it)
		{
			Field *pField = (*it).second;
			if( !m_mapField_Find(pField->Name_get()) )
			{
				// We need to add this field to our table
				ostringstream sql;
				if( m_pTable_WeAreHistoryMaskFor )
					sql << "ALTER TABLE `" + m_sName + "` ADD `" << pField->Name_get() << "`" << "tinyint(1)";
				else
				{
					// This is a bit round-about way of getting the field type for the add, but the
					// advantage of using DESCRIBE is that if MySql adds field types we don't need
					// to worry about keeping our own map to go from FieldInfo to a valid descriptor.
					PlutoSqlResult result_set;
					MYSQL_ROW row=NULL;
					if( !( result_set.r=m_pDatabase->mysql_query_result( ( "DESCRIBE `" + pTable->Name_get() + "`" ).c_str( ) ) ) )
						throw "Error getting table info";

					while( ( row = mysql_fetch_row( result_set.r ) ) )
					{
						string sField = row[0];
						string sType = row[1];
						if( sField == pField->Name_get() )
						{
							sql << "ALTER TABLE `" + m_sName + "` ADD `" << sField << "` " << sType;
							break;
						}
					}
				}
				if( m_pDatabase->threaded_mysql_query( sql.str() )<0 )
				{
					cerr << "Could not synchronize history table schema";
					throw "Database error";
				}
			}
		}
	}
}

void Table::GetDependencies( )
{
if( m_sName=="CommandGroup_Command_CommandParameter")
{
int k=2;
}
	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
	{
		Field *pField = ( *it ).second;
		if( pField->Name_get( ).substr( 0, 3 )=="FK_" || pField->Name_get( ).substr( 0, 3 )=="IK_" )
		{
			Table *pTable = m_pDatabase->GetTableFromForeignKey( pField );
			if( !pTable )
			{
				cerr << "Table: " << m_sName << " has a foreign key " << pField->Name_get( ) << " but I cannot find the table it is referring to." << endl;
				throw "Bad Schema";
			}

			/**
			 * See if this is an indirect foreign key, meaning we have an IV_[identifier] field, which refers to a table
			 * [identifier], which has a field Description, which, if it starts with PK_ means our IK_ field refers
			 * to a foreign key in that table
			*/

			if( pField->Name_get( ).substr( 0, 3 )=="IK_" )
			{
				Field *pField_Indirect=NULL;
				pField_Indirect = m_mapField_Find( "IV_" + pField->Name_get( ).substr( 3 ) );

				ostringstream sql;
				sql << "SELECT Description FROM `" << pField->Name_get( ).substr( 3 ) << "` WHERE Description LIKE 'PK_%'";
				PlutoSqlResult result_set;
				MYSQL_ROW row=NULL;
				if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
				{
					while( ( row = mysql_fetch_row( result_set.r ) ) )
					{
						string FieldName = row[0];
						class Table *pTable = m_pDatabase->GetTableFromForeignKeyString( this, FieldName );
						Field *pField_PrimaryKey = pTable->m_mapField_Find( "PK_" + pTable->Name_get( ) );
						pField->m_listField_IReferTo_Indirectly.push_back( make_pair<string,Field *> (FieldName,pField_PrimaryKey) );
						pField_PrimaryKey->m_listField_ReferringToMe_Indirectly.push_back(  make_pair<string,Field *> (FieldName,pField) );
					}
				}
			}
			else
			{
				Field *pField_PrimaryKey = pTable->m_mapField_Find( "PK_" + pTable->Name_get( ) );
				if( !pField_PrimaryKey )
				{
					cerr << "Table: " << m_sName << " has a foreign key " << pField->Name_get( ) << " but I cannot find the primary key in the table it is referring to." << endl;
					throw "Bad Schema";
				}

				pField->m_pField_IReferTo_Directly = pField_PrimaryKey;
				pField_PrimaryKey->m_listField_ReferringToMe_Directly.push_back( pField );
			}
		}
	}
}

#pragma warning( "todo -- warn if other tables already have a timestamp that we can't use them" )

// Executed on the client side to update a table
bool Table::Update( RA_Processor &ra_Processor, DCE::Socket *pSocket )
{
	vector<string> vectFields;
	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
	{
		Field *pField = ( *it ).second;
		if( pField==m_pField_id || pField==m_pField_batch || pField==m_pField_user || pField==m_pField_mod )
			continue;
		vectFields.push_back( ( *it ).first );
	}

	// First delete any rows in our vect that we found during the DetermineDeletions phase so we reduce the 'duplicate key' errors
	if( m_vectRowsToDelete.size() )
	{
		cout << "Deleting " << (int) m_vectRowsToDelete.size() << " rows from table: " << m_sName << endl;
		ostringstream sSql;
		sSql << "DELETE FROM `" << m_sName << "` WHERE psc_id IN (";

		for(size_t s=0;s<m_vectRowsToDelete.size();++s)
			sSql << (s ? "," : "") << m_vectRowsToDelete[s];

		sSql << ")";
cout << sSql.str() << endl;
if( !g_GlobalConfig.m_bNoPrompts && !AskYNQuestion("Proceed with delete?",false) )
throw "problem with delete";
		if( m_pDatabase->threaded_mysql_query( sSql.str() )<0 )
		{
			cerr << "Unable to delete local rows" << endl;
			throw "Cannot delete rows";
		}
	}

	R_UpdateTable r_UpdateTable( m_sName, m_psc_batch_last_sync, m_psc_id_last_sync, &vectFields, &g_GlobalConfig.m_vectRestrictions );

	// The server will send us all updates after m_psc_batch_last_sync, but we need to tell it to
	// explicitly exclude any batches that we sent ourselves, since have them already.  Create a list
	// of the batches we created since our last sync

	cout << "Requesting update table: " << m_sName << " last batch: " << m_psc_batch_last_sync << " last id: " << m_psc_id_last_sync << " extra batches:";

	ostringstream sSQL;
	sSQL << "SELECT PK_psc_" << m_pRepository->Name_get() << "_bathdr FROM psc_" << m_pRepository->Name_get() << "_bathdr "
		<< " WHERE PK_psc_" << m_pRepository->Name_get() << "_bathdr>" << m_psc_batch_last_sync;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			r_UpdateTable.m_vect_psc_batch.push_back( atoi(row[0]) );
			cout << row[0] << ",";
		}

	cout << endl;

	ra_Processor.SendRequest( &r_UpdateTable, pSocket );
	if( r_UpdateTable.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		/**
		 * It's going to fall out of scope, so we don't want the processor to retry sending a deleted request
		 */
		ra_Processor.RemoveRequest( &r_UpdateTable );
		cerr << "Failed to update table: " << m_sName << ":" << (int) r_UpdateTable.m_cProcessOutcome << endl;
		return false;
	}

	cout << "Requested update" << endl;

	if( r_UpdateTable.m_psc_id_last_sync > m_psc_id_last_sync )
		m_pRepository->psc_id_last_sync_set(this,r_UpdateTable.m_psc_id_last_sync);
	if( r_UpdateTable.m_psc_batch_last_sync > m_psc_batch_last_sync )
		m_pRepository->psc_batch_last_sync_set(this,r_UpdateTable.m_psc_batch_last_sync);
	return true;
}

// Executed on the server side in repsonse to update table
void Table::GetChanges( R_UpdateTable *pR_UpdateTable )
{
#pragma warning( "take into account filter" )
	ostringstream sSql;
	sSql << "SELECT ";
	for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
		sSql << "`" << m_sName << "`.`" <<  ( *pR_UpdateTable->m_pvectFields )[s] << "`, ";

	sSql << "`" << m_sName << "`.`psc_id`, `" << m_sName << "`.`psc_batch`,"
		<< "`" << m_sName << "`.`psc_user` FROM `" << m_sName << "`";

	if( m_sFilter.length() )
	{
		if( StringUtils::ToUpper(m_sFilter).find("WHERE")==string::npos )
			sSql << " WHERE";
		StringUtils::Replace(&m_sFilter,"<%=U%>",g_GlobalConfig.csvUserID());
		sSql << " " << m_sFilter << " AND ";
	}
	else
		sSql << " WHERE ";

	sSql << "`" << m_sName << "`.`psc_batch`>" << pR_UpdateTable->m_psc_batch_last_sync;
	if( pR_UpdateTable->m_vect_psc_batch.size() )
	{
		sSql << " AND `" << m_sName << "`.`psc_batch` NOT IN (";
		for(size_t s=0;s<pR_UpdateTable->m_vect_psc_batch.size();++s)
			sSql << (s>0 ? "," : "") << pR_UpdateTable->m_vect_psc_batch[s];
		sSql << ")";
	}

	sSql << " AND	" << g_GlobalConfig.GetRestrictionClause( m_sName,&pR_UpdateTable->m_vectRestrictions );
	sSql << " ORDER BY psc_id,psc_batch";

	/* 10 May 05 - this used to be order by psc_batch,psc_id.  But it happened that a row was
	added by a user (1230), then he added another row (1232), then he updated the first row (1230).
	Now 1230 has a higher psc_batch.  So, we return the row 1232 first, set the last sync:
	if( pA_UpdateRow->m_psc_id>m_psc_id_last_sync )
		m_pRepository->psc_id_last_sync_set(this,pA_UpdateRow->m_psc_id);
	and then when 1230 comes in, it's treated as an update not a new row:
		if( pA_UpdateRow->m_psc_id>m_psc_id_last_sync && !psc_id_exists(pA_UpdateRow->m_psc_id) )
	It's never added, and therefore repored as deleted the next time.  So we'll do this
	in order of psc_id, not psc_batch.  It doesn't appear that the order of psc_batch is important,
	so I just changed the sort order */

	int i_psc_id_field = ( int ) pR_UpdateTable->m_pvectFields->size( );
	int i_psc_batch_field = i_psc_id_field+1;
	int i_psc_user_field = i_psc_batch_field+1;

	cout << "GetChanges() query: " << sSql.str() << endl;

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( !( result_set.r=m_pDatabase->mysql_query_result( sSql.str( ) ) ) )
	{
		cerr << "Cannot get changes from SQL: " << sSql.str( ) << endl;
		throw "Database error";
	}

	cout << "Sending changes to " << m_sName << " " << result_set.r->row_count << " rows." << endl;

	while( ( row = mysql_fetch_row( result_set.r ) ) )
	{
		int psc_id=atoi( row[i_psc_id_field] );
		int psc_batch=atoi( row[i_psc_batch_field] );
		int psc_user=row[i_psc_user_field] ? atoi( row[i_psc_user_field] ) : 0;
		if( psc_id>pR_UpdateTable->m_psc_id_last_sync )
			pR_UpdateTable->m_psc_id_last_sync = psc_id;
		if( psc_batch>pR_UpdateTable->m_psc_batch_last_sync )
			pR_UpdateTable->m_psc_batch_last_sync = psc_batch;

		cout << "Sending psc_id: " << psc_id << " psc_batch: " << psc_batch << " psc_id_last_sync: " <<
			pR_UpdateTable->m_psc_id_last_sync << " m_psc_batch_last_sync: " << pR_UpdateTable->m_psc_batch_last_sync << endl;

		A_UpdateRow *pa_UpdateRow = new A_UpdateRow( psc_id, psc_batch, psc_user );

		for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
			pa_UpdateRow->m_vectValues.push_back( row[s] ? row[s] : NULL_TOKEN );
		cout << "Sending psc_id: " << psc_id << " psc_batch: " << psc_batch << " psc_id_last_sync: " <<
			pR_UpdateTable->m_psc_id_last_sync << " m_psc_batch_last_sync: " << pR_UpdateTable->m_psc_batch_last_sync <<
			" # of fields: " << pR_UpdateTable->m_pvectFields->size( ) << " values: " << pa_UpdateRow->m_vectValues.size() << endl;
		pR_UpdateTable->AddActionsToResponse( pa_UpdateRow );
	}
}

void Table::GetChanges( )
{
	if( m_bIsSystemTable )
		return;

	int iAutoIncrField=-1; /** If there is an auto increment field in the key, we will point to it's "slot" here */
	ostringstream sql;
	sql << "SELECT psc_id, psc_batch, psc_user, psc_restrict";

	int FieldNum=4;
	for( ListField::iterator it=m_listField_PrimaryKey.begin( );it!=m_listField_PrimaryKey.end( );++it )
	{
		Field *pField = *it;
		sql << ", `" << pField->Name_get( ) << "`";
		if( pField==m_pField_AutoIncrement )
			iAutoIncrField=FieldNum;
		FieldNum++;
	}

	sql << " FROM `" << m_sName << "` WHERE psc_mod>0 AND " << g_GlobalConfig.GetRestrictionClause(m_sName);

	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
	{
		while( ( row = mysql_fetch_row( result_set.r ) ) )
		{
			TypeOfChange eTypeOfChange;
			if( row[0] == NULL )
				eTypeOfChange = toc_New; /** It's a new row if psc_id hasn't been assigned */
			else
				eTypeOfChange = toc_Modify;

			/** Add all the primary key values   */
			vector<string> vectPrimaryKeys;
			for( size_t s=0;s<m_listField_PrimaryKey.size( );++s )
			{
cout << "pk: " << s << " " << row[s+4];
				vectPrimaryKeys.push_back( row[s+4] );
			}
cout << " psc_id: " << (row[0] ? atoi( row[0] ) : 0) << endl;
			int iAutoIncrValue = iAutoIncrField>=0 ? atoi( row[iAutoIncrField] ) : 0;

			if( row[1] && atoi(row[1])<1 )
			{
				cout << "Not checking in changes to table: " << m_sName << " psc_id: " << (row[0]?row[0]:"NULL") << " because it's batch: " << row[1] << " is still unauthorized" << endl;
				continue;
			}

			ChangedRow *pChangedRow = new ChangedRow( this, eTypeOfChange, row[0] ? atoi( row[0] ) : 0, row[1] ? atoi( row[1] ) : 0, row[2] ? atoi( row[2] ) : 0, row[3] ? atoi( row[3] ) : 0, iAutoIncrValue, vectPrimaryKeys );
			AddChangedRow( pChangedRow );
		}
	}
}

void Table::AddChangedRow( ChangedRow *pChangedRow )
{
	ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[ pChangedRow->m_psc_user ];
	if( !pListChangedRow )
	{
		pListChangedRow = new ListChangedRow( );
		m_mapUsers2ChangedRowList[ pChangedRow->m_psc_user ] = pListChangedRow;
	}
	pListChangedRow->push_back( pChangedRow );

	MapTable *pMapTable = g_GlobalConfig.m_mapUsersTables[ pChangedRow->m_psc_user ];
	if( !pMapTable )
	{
		pMapTable = new MapTable( );
		g_GlobalConfig.m_mapUsersTables[ pChangedRow->m_psc_user ] = pMapTable;
	}
	( *pMapTable )[m_sName]=this;
}

bool Table::ConfirmDependencies( )
{
	if( g_GlobalConfig.m_bAllowUnmetDependencies )
		return true;

	bool bDependenciesMet=true;

	for( map<int, MapTable *>::iterator it=g_GlobalConfig.m_mapUsersTables.begin( );it!=g_GlobalConfig.m_mapUsersTables.end( );++it )
	{
		int PK_Users = ( *it ).first;
		if( m_mapUsers2ChangedRowList.find( PK_Users ) != m_mapUsers2ChangedRowList.end( ) )
		{
			ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[PK_Users];
			for( ListChangedRow::iterator itCR=pListChangedRow->begin( );itCR!=pListChangedRow->end( );++itCR )
			{
				ChangedRow *pChangedRow = *itCR;

				/**
				 * Go through all our fields. If any are foreign keys to another table,
				 * that row in the other table must be checked in as well
				 */

				for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
				{
					Field *pField = ( *it ).second;
					if( pField->m_pField_IReferTo_Directly && !ConfirmDependency( pChangedRow, pField, pField->m_pField_IReferTo_Directly ) )
					{
						bDependenciesMet=false;
						cerr << "Field: " << m_sName << ":" << pField->Name_get( ) << " Refers to: " << pField->m_pField_IReferTo_Directly->m_pTable->Name_get( ) << " which is not being checked in." << endl;
					}
/*
TODO -- This won't work for indirect fields -- we can't match any field to any field.  For the moment comment it out and don't verify dependencies for indirect fields
					for( ListField::iterator itList=pField->m_listField_IReferTo_Indirectly.begin( );itList!=pField->m_listField_IReferTo_Indirectly.end( );++itList )
					{
						Field *pField_Indirect = *itList;
						if( !ConfirmDependency( pChangedRow, pField, pField_Indirect ) )
						{
							bDependenciesMet=false;
							cerr << "Field: " << m_sName << ":" << pField->Name_get( ) << " indirectly refers to: " << pField_Indirect->m_pTable->Name_get( ) << " which is not being checked in." << endl;
						}
					}
*/
				}
			}
		}
	}
	return bDependenciesMet;
}

bool Table::ConfirmDependency( ChangedRow *pChangedRow, Field *pField_Referring, Field *pField_ReferredTo )
{
	/** If we're checking the table that we're referring to, then we're fine. Any changes will be propagated. */

	if( g_GlobalConfig.m_mapTable.find( pField_ReferredTo->m_pTable->Name_get( ) )!=g_GlobalConfig.m_mapTable.end( ) )
		return true;

	/** We refer to a table that's not being checked in. That's okay if the row we're referncing has not been modified.
	 * If it has changed, then we must check in that table too, otherwise we will be uploading a row to the server that references
	 * a row the server doesn't have, and the server's database will become corrupted. First we have to get the value of the field here.
	 */

	if( !pField_ReferredTo->m_pTable->m_pField_mod )
	{
		cerr << "Table: " << pField_ReferredTo->m_pTable->Name_get( ) << " does not have a psc_mod field. I can't tell if the dependency is met" << endl;
		return false;
	}
#pragma warning( "THIS ISN'T GOING TO WORK WITH INDIRECT FIELDS LIKE THIS" )
	std::ostringstream sSQL;
	sSQL << "SELECT `" << pField_Referring->Name_get( ) << "` FROM " << m_sName << pChangedRow->GetWhereClause( );
	PlutoSqlResult res, res2;
	MYSQL_ROW row=NULL;
	res.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r || res.r->row_count!=1 )
	{
		cerr << "Problem retrieving row with query2: " << sSQL.str() << endl;
		return false;
	}
	row = mysql_fetch_row( res.r );

	if( !row[0] )
		return true; /** It's a null value */

	sSQL.str( "" );
	sSQL << "SELECT psc_mod FROM " << pField_ReferredTo->m_pTable->Name_get( ) << " WHERE `" << pField_ReferredTo->Name_get( ) << "`=" << row[0];
	res2.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res2.r || res2.r->row_count!=1 )
	{
		cerr << "Problem retrieving row with query3: " << sSQL.str() << endl;
		return false;
	}
	row = mysql_fetch_row( res2.r );
	if( !row[0] || atoi( row[0] )==0 )
		return true;

	cerr << "Table: " << pField_ReferredTo->m_pTable->Name_get( ) << " needs to be checked in." << endl;
	return false;
}

bool Table::DetermineDeletions( RA_Processor &ra_Processor, string Connection, DCE::Socket **ppSocket )
{
	cout << "DetermineDeletions for table: " << m_sName << " from psc_id: " << m_psc_id_last_sync << " batch: " << m_psc_batch_last_sync << endl;
	if( m_psc_id_last_sync<1 )
	{
		cout << "We haven't synced any records with the server yet, exiting" << endl;
		return true; /** We haven't synced any records with the server anyway */
	}
		
	if (m_sFilter != "")
	{
		cout << "Table has non-empty filter, skipping this stage {" << m_sFilter << "}" << endl;
		return true;	
	}

	//number of intervals to divide
	const int intervals_count = 10;

	//this will be ranges queue for search of changes
	vector < pair<int, int> > ranges_queue;

	//initial seed: up to 10 mln of records
	ranges_queue.push_back(pair<int, int> (0, 99999999));

	//these arrays wll contain resulting ranges of local deletions and remote deletions
	vector<pair<int, int> > local_deletions, remote_deletions;

	while (ranges_queue.size())
	{
		// collecting all requests to send them together
		vector<R_GetHashedTableStats*> hash_requests;

		for (int i=0; i<ranges_queue.size(); i++)
		{
			int range_begin = ranges_queue[i].first;
			int range_end = ranges_queue[i].second;
			int interval_length = (range_end-range_begin)/intervals_count+1;

			cout << "Scanning range of psc_id: [" << range_begin << ", " << range_end << "] with interval_length=" << interval_length << endl;

			R_GetHashedTableStats* pReq = new R_GetHashedTableStats(m_sName, &g_GlobalConfig.m_vectRestrictions, range_begin, range_end, interval_length);
			hash_requests.push_back(pReq);

			ra_Processor.AddRequest( pReq );
		}

		// sending requests
		while( ra_Processor.SendRequests( Connection, ppSocket ) );

		// processing responses
		for (int req=0; req<hash_requests.size(); req++)
		{

			if( hash_requests[req]->m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
			{
				cerr << "Cannot get table stats from server:" << (int) hash_requests[req]->m_cProcessOutcome << endl;
				while (hash_requests.size()) {delete hash_requests[0]; hash_requests.erase(hash_requests.begin());}
				throw "Communication error";
			}

			int range_begin = ranges_queue[0].first;
			int range_end = ranges_queue[0].second;
			int interval_length = (range_end-range_begin)/intervals_count+1;

			// requesting stats for the same range from local client
			std::ostringstream sSQL;
			// BIT_XOR and DIV are not implemented for MySQL 4.0.x
			/*
			sSQL << "SELECT (psc_id-" << range_begin << ") DIV " << interval_length << " , count(*),BIT_XOR(POW(psc_id-"<<range_begin<<", FLOOR(63/CEIL(LOG2("<<(1+range_end-range_begin)<<"))))) FROM " << m_sName << " WHERE (psc_id IS NOT NULL AND psc_id>0)  AND (psc_id BETWEEN " << range_begin << " AND " << range_end << ") AND " << g_GlobalConfig.GetRestrictionClause(m_sName,&g_GlobalConfig.m_vectRestrictions) << " GROUP BY (psc_id-"<< range_begin<< ") DIV " << interval_length << " ORDER BY (psc_id-"<< range_begin<<") DIV " << interval_length;
			*/

			sSQL << "SELECT FLOOR( (psc_id-" << range_begin << ") / " << interval_length << ") , count(*), MD5(CAST(SUM(POW(1+(psc_id-"<<range_begin<<")%"<<interval_length<<", 3)) AS UNSIGNED)) FROM `" << m_sName << "` WHERE (psc_id IS NOT NULL AND psc_id>0)  AND (psc_id BETWEEN " << range_begin << " AND " << range_end << ") AND " << g_GlobalConfig.GetRestrictionClause(m_sName,&g_GlobalConfig.m_vectRestrictions) << " GROUP BY FLOOR((psc_id-"<< range_begin<< ") / " << interval_length << ") ORDER BY FLOOR((psc_id-"<< range_begin<<") / " << interval_length << ")";

			PlutoSqlResult res;
			MYSQL_ROW row=NULL;
			res.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
			if( !res.r )
			{
				cerr << "Problem retrieving rows with query: " << sSQL.str() << endl;
				while (hash_requests.size()) {delete hash_requests[0]; hash_requests.erase(hash_requests.begin());}
				throw "Database error";
			}

			map<int, pair<int,string> >  server_stats, client_stats;
			vector<int> server_intervals, client_intervals;

			// adding server stats into map
			for (int i=0; i<hash_requests[req]->m_hashedCount.size(); i++)
			{
				server_stats[hash_requests[req]->m_hashedCount[i].first] = pair<int,string> (hash_requests[req]->m_hashedCount[i].second, hash_requests[req]->m_hashedCheckSum[i]);

				server_intervals.push_back(hash_requests[req]->m_hashedCount[i].first);
			}

			while( ( row = mysql_fetch_row( res.r ) ) )
			{
				if (row[0])
				{
					client_stats[atoi(row[0])] = pair<int, string> (atoi(row[1]), row[2]);

					client_intervals.push_back(atoi(row[0]));
				}
			}

			// determining deletions and intersections
			vector<int> deleted_locally, deleted_remotely, common;

			set_intersection(client_intervals.begin(), client_intervals.end(), server_intervals.begin(), server_intervals.end(), inserter(common, common.end()));

			set_difference(client_intervals.begin(), client_intervals.end(), server_intervals.begin(), server_intervals.end(), inserter(deleted_remotely, deleted_remotely.end()));

			set_difference(server_intervals.begin(), server_intervals.end(), client_intervals.begin(), client_intervals.end(), inserter(deleted_locally, deleted_locally.end()));

			// checking common intervals
			for (int i=0; i<common.size(); i++)
			{
				int interval_no = common[i];
				if ((client_stats[interval_no].first!=server_stats[interval_no].first)||(client_stats[interval_no].second!=server_stats[interval_no].second))
				{
					ranges_queue.push_back(pair<int, int> (range_begin+interval_length*interval_no, range_begin+interval_length*(interval_no+1)-1));
					cout << "Interval: " << interval_no << " is different = range [" << (range_begin+interval_length*interval_no) << ", " << (range_begin+interval_length*(interval_no+1)-1) << "] " << endl;
				}
			}

			// deleted locally - processing
			for (int i=0; i<deleted_locally.size(); i++)
			{
				int interval_no = deleted_locally[i];
				local_deletions.push_back(pair<int, int> (range_begin+interval_length*interval_no, range_begin+interval_length*(interval_no+1)-1));
			}

			// deleted remotely - processing
			for (int i=0; i<deleted_remotely.size(); i++)
			{
				int interval_no = deleted_remotely[i];
				remote_deletions.push_back(pair<int, int> (range_begin+interval_length*interval_no, range_begin+interval_length*(interval_no+1)-1));
			}

			ranges_queue.erase(ranges_queue.begin());

		}

		while (hash_requests.size()) {delete hash_requests[0]; hash_requests.erase(hash_requests.begin());}
	}

	cout << "Local deletions: ";
	for (int i=0; i<local_deletions.size(); i++)
	{
		cout << "[" << local_deletions[i].first << ", " << local_deletions[i].second << "] ";
	}
	cout << endl;

	cout << "Remote deletions: ";
	for (int i=0; i<remote_deletions.size(); i++)
	{
		cout << "[" << remote_deletions[i].first << ", " << remote_deletions[i].second << "] ";
	}
	cout << endl;


	//processing local deletions
	std::ostringstream lSQL;
	lSQL << "(0 ";
	for (int i=0; i<local_deletions.size(); i++)
	{
		lSQL << "OR (psc_id>="<< local_deletions[i].first << " AND psc_id<=" << local_deletions[i].second << ") ";
	}
	lSQL << ")";

	//requesting from server information about locally deleted records
	R_GetConditional_psc_id r_GetConditional_psc_id( m_sName, &g_GlobalConfig.m_vectRestrictions,  lSQL.str());
	ra_Processor.AddRequest( &r_GetConditional_psc_id );
	while( ra_Processor.SendRequests( Connection, ppSocket ) );

	if( r_GetConditional_psc_id.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetConditional_psc_id.m_cProcessOutcome << endl;
		throw "Communication error";
	}

	for (int i=0; i<r_GetConditional_psc_id.m_vectAll_psc_id.size(); i++)
	{
			cout << "Row deleted locally: " <<  r_GetConditional_psc_id.m_vectAll_psc_id[i].first << "(" << r_GetConditional_psc_id.m_vectAll_psc_id[i].second << ")" << endl;

		// if record is newer than our last-sync and it is not our batch - skipping it
		if ( r_GetConditional_psc_id.m_vectAll_psc_id[i].first>m_psc_id_last_sync &&  !DoWeHaveBatch(r_GetConditional_psc_id.m_vectAll_psc_id[i].second) )
		{
			cout << "Record is newer than our last-sync and it is not our batch - skipping it" << endl;
			continue;
		}

		if( r_GetConditional_psc_id.m_vectAll_psc_id[i].second>m_psc_batch_last_sync )
		{
			if( !DoWeHaveBatch(r_GetConditional_psc_id.m_vectAll_psc_id[i].second) )
			{
				cout << "Record is newer than what we've synced" << endl;
				continue;
			}
			else
				cout << "Record is newer than what we've synced, but we added it" << endl;
		}
		else
			cout << "Marking for deletion" << endl;

		itmp_RowsToDelete++;
		ChangedRow *pChangedRow = new ChangedRow( this, r_GetConditional_psc_id.m_vectAll_psc_id[i].first);
		AddChangedRow( pChangedRow);
	}


	//processing remote deletions
	std::ostringstream rSQL;
	rSQL << "SELECT psc_id, psc_batch FROM `" << m_sName << "` WHERE 0 ";
	for (int i=0; i<remote_deletions.size(); i++)
	{
		rSQL << "OR (psc_id>="<< remote_deletions[i].first << " AND psc_id<=" << remote_deletions[i].second << ") ";
	}

	// walking thru records deleted remotely - if they have non-zero batch, mark them as to be deleted
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = m_pDatabase->mysql_query_result( rSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query: " << rSQL.str() << endl;
		throw "Database error";
	}

	while( ( row = mysql_fetch_row( res.r ) ) )
	{
		cout << "Server deleted row psc_id=" << row[0];
		if ( !row[1] || atoi(row[1])>=0 )
		{
			cout << " marking it for deletion too" << endl;
			m_vectRowsToDelete.push_back(atoi(row[0]));
		}
		else
		{
			cout << " it is unauthorized, skipping" << endl;
		}
	}

	return true;
}

bool Table::CheckIn( int psc_user, RA_Processor &ra_Processor, DCE::Socket *pSocket, enum TypeOfChange toc )
{
if( m_sName=="CommandCategory" )
{
int k=2;
}
	vector<string> vectFields;
	for( MapField::iterator it=m_mapField.begin( );it!=m_mapField.end( );++it )
	{
		Field *pField = ( *it ).second;

		/** If this a new record, don't add the auto increment field */

		if( pField==m_pField_id || pField==m_pField_batch || pField==m_pField_user || pField==m_pField_mod || pField==m_pField_frozen || pField==m_pField_restrict || ( pField==m_pField_AutoIncrement && toc==toc_New ) )
			continue;
		vectFields.push_back( ( *it ).first );
	}

	R_CommitTable r_CommitTable( m_sName, &vectFields );
	ra_Processor.SendRequest( &r_CommitTable, pSocket );
	if( r_CommitTable.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		ra_Processor.RemoveRequest( &r_CommitTable ); /** It's going to fall out of scope, so we don't the processor to retry */
		cerr << "Failed to commit table: " << m_sName << ":" << (int) r_CommitTable.m_cProcessOutcome << endl;
		return false;
	}

	cout << "Checking in: " << m_sName << " user: " << psc_user << endl;
	if( m_mapUsers2ChangedRowList.find( psc_user ) != m_mapUsers2ChangedRowList.end( ) )
	{
		ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList[psc_user];
		cout << "Checking in: " << m_sName << " user: " << psc_user << " rows: " << (int) pListChangedRow->size() << endl;
		for( ListChangedRow::iterator itCR=pListChangedRow->begin( );itCR!=pListChangedRow->end( );++itCR )
		{
			ChangedRow *pChangedRow = *itCR;
			if( pChangedRow->m_eTypeOfChange != toc )
				continue;

			// If we have a mask file, only check in the changes in that file
			if( g_GlobalConfig.m_mapMaskedChanges.size() &&
					g_GlobalConfig.m_mapMaskedChanges.find( make_pair<string,string> (m_pRepository->Name_get()+":"+m_sName,pChangedRow->GetWhereClause()) )
					== g_GlobalConfig.m_mapMaskedChanges.end() )
				continue;

			R_CommitRow r_CommitRow( pChangedRow );

			cout << "Sending row table: " << m_sName << " toc: " << pChangedRow->m_eTypeOfChange << " psc_id: " << pChangedRow->m_psc_id << " " << pChangedRow->GetWhereClause() << endl;

			std::ostringstream sSQL;
			if( pChangedRow->m_eTypeOfChange!=toc_Delete )
			{
				sSQL << "SELECT ";

				for( size_t s=0;s<vectFields.size( );++s )
				{
					if( s>0 )
						sSQL << ", ";
					sSQL << "`" << vectFields[s] << "`";
				}

				sSQL << " FROM `" << m_sName << "` ";

				sSQL << pChangedRow->GetWhereClause( );

				PlutoSqlResult res;
				MYSQL_ROW row=NULL;
				res.r = m_pDatabase->mysql_query_result( sSQL.str( ) );
				if( !res.r || res.r->row_count!=1 )
				{
					cerr << "Problem retrieving row with query5: " << sSQL.str() << endl;
					return false;
				}
				row = mysql_fetch_row( res.r );
				for( size_t s=0;s<vectFields.size( );++s )
				{
					r_CommitRow.m_vectValues.push_back( row[s] ? row[s] : NULL_TOKEN );
				}
			}

			ra_Processor.SendRequest( &r_CommitRow, pSocket );
#pragma warning( "take into account return value of 'record frozen'" )
			if( r_CommitRow.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
			{
				if( r_CommitRow.m_cProcessOutcome==SKIPPING_ROW_DELETED )
				{
					cout << "Changes to table: " << m_sName << " psc_id: " << pChangedRow->m_psc_id
						<< " were rejected because the row has been deleted by another user " << endl;
					if( !g_GlobalConfig.m_bNoPrompts && !AskYNQuestion("Continue?",false) )
						return false;
					continue;
				}

				/** It's going to fall out of scope, so we don't want the processor to retry sending a deleted request */
				ra_Processor.RemoveRequest( &r_CommitRow );
				cerr << "Failed to commit row in table: " << m_sName  << ":" << (int) r_CommitRow.m_cProcessOutcome << endl << r_CommitRow.m_sResponseMessage << endl;
				return false;
			}

			pChangedRow->m_bFrozen = r_CommitRow.m_bFrozen;
			pChangedRow->m_psc_user_needs_to_authorize = r_CommitRow.m_psc_user_needs_to_authorize;
			pChangedRow->m_psc_batch_new = r_CommitRow.m_psc_batch_new;
			pChangedRow->m_psc_id_new = r_CommitRow.m_psc_id_new;
			if( pChangedRow->m_bFrozen || pChangedRow->m_psc_user_needs_to_authorize )
			{
				ListChangedRow *pListChangedRow = g_GlobalConfig.m_mapBatch_ChangedRow[pChangedRow->m_psc_batch_new];
				if( !pListChangedRow )
				{
					pListChangedRow = new ListChangedRow( );
					g_GlobalConfig.m_mapBatch_ChangedRow[pChangedRow->m_psc_batch_new] = pListChangedRow;
				}
				pListChangedRow->push_back( pChangedRow );

			}

			if( toc==toc_New )
			{
				if( !r_CommitRow.m_psc_id_new )
				{
					cerr << "Didn't get back a valid psc_id for: " << m_sName << endl;
					return false;
				}
				sSQL.str( "" );
				sSQL << "UPDATE `" << m_sName << "` SET psc_id=" << r_CommitRow.m_psc_id_new << pChangedRow->GetWhereClause( );

				if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
				{
					cerr << "SQL failed: " << sSQL.str( );
					return false;
				}
			}

			if( r_CommitRow.m_psc_batch_new && r_CommitRow.m_psc_batch_new!=r_CommitRow.m_psc_batch )
			{
				sSQL.str( "" );
				sSQL << "UPDATE `" << m_sName << "` SET psc_batch=" << r_CommitRow.m_psc_batch_new << pChangedRow->GetWhereClause( );
				if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
				{
					cerr << "SQL failed: " << sSQL.str( );
					return false;
				}
			}

			if( r_CommitRow.m_psc_user_new && r_CommitRow.m_psc_user_new!=r_CommitRow.m_psc_user )
			{
				sSQL.str( "" );
				sSQL << "UPDATE `" << m_sName << "` SET psc_user=" << r_CommitRow.m_psc_user_new << pChangedRow->GetWhereClause( );
				if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
				{
					cerr << "SQL failed: " << sSQL.str( );
					return false;
				}
			}

			/** Do this last since the 2 above may be relying on the old primary key */

			if( r_CommitRow.m_iNewAutoIncrID && r_CommitRow.m_iNewAutoIncrID!=r_CommitRow.m_iBeforeTransmit_iAutoIncrID )
			{
				cout << "Need to check for changed primary key r_CommitRow.m_iNewAutoIncrID: " << r_CommitRow.m_iNewAutoIncrID
					<< " m_iBeforeTransmit_iAutoIncrID: " << r_CommitRow.m_iBeforeTransmit_iAutoIncrID << endl;

				if( !m_pField_AutoIncrement )
					throw ( "Internal error: Trying to set a non existant auto increment in table: " + m_sName ).c_str( );

				/*
					There's a problem here...  The new primary key may already be used by another row which is still waiting to be checked in.
					Once all the new rows are checked in there should be no overlapping of primary keys since we will be using all the id's
					from the server.  But until then, the server may be giving out id's that we're already using for another row.  So we
					have to do a quick check, and if that's the case, move that other row to a different id.
				*/

				sSQL.str( "" );
				sSQL << "SELECT PK_" << m_sName << " FROM `" << m_sName << "` WHERE PK_" << m_sName << "=" << r_CommitRow.m_iNewAutoIncrID;
				PlutoSqlResult result_set;
				MYSQL_ROW row=NULL;
				if( ( result_set.r=m_pDatabase->mysql_query_result(sSQL.str()) ) && ( row = mysql_fetch_row( result_set.r ) ) )
				{
					// We're going to have to move the key.  This doesn't happen too often, so we'll just find the conflicting row the 'brute force' way
					// It's possible the row that's conflicting is either a ChangedRow that we're checking in now,
					// or an un-authorized one we checked in already.  This loop will see if it's one we're checking in
					ChangedRow *pChangedRowToMove = NULL;

					int iHighestUsedID=0; // We have to find the highest id so we can move the row outside
					std::ostringstream sSQL2;
					sSQL2 << "SELECT max(" << m_pField_AutoIncrement->Name_get() << ") FROM `" << m_sName << "`";
					PlutoSqlResult result_set_hi;
					MYSQL_ROW row_hi=NULL;
					if( ( result_set_hi.r=m_pDatabase->mysql_query_result( sSQL2.str( ) ) ) && (row_hi = mysql_fetch_row(result_set_hi.r) ) )
						iHighestUsedID = atoi(row_hi[0]);

					cout << "Checking:Reassigning Auto Incr Value for table: " << m_sName << " determined highest used value is: " << row_hi[0] << " m_psc_id_next: " << m_psc_id_next << " " << row_hi << " highestid: " << iHighestUsedID << endl;

					for(map<int, ListChangedRow *>::iterator it1=m_mapUsers2ChangedRowList.begin();it1!=m_mapUsers2ChangedRowList.end();++it1)
					{
						ListChangedRow *pListChangedRow = (*it1).second;
						for( ListChangedRow::iterator itCR=pListChangedRow->begin( );itCR!=pListChangedRow->end( );++itCR )
						{
							ChangedRow *pChangedRow = *itCR;

							if( pChangedRow->m_iNewAutoIncrID>iHighestUsedID )
							{
								iHighestUsedID = pChangedRow->m_iNewAutoIncrID;
cout << "Found higher value: " << pChangedRow->m_iNewAutoIncrID << " orig: " << pChangedRow->m_iBeforeTransmit_iAutoIncrID << " psc_id: " << pChangedRow->m_psc_id << " highestid: " << iHighestUsedID << endl;
							}
							if( pChangedRow->m_iBeforeTransmit_iAutoIncrID!=r_CommitRow.m_iNewAutoIncrID )
								continue;

							pChangedRowToMove=pChangedRow;  // We're going to keep going because we need to determine iHighestUsedID
						}
					}
					// We found it -- quick sanity check.  If it's one we're checking in, it must be a new record that we haven't committed yet,
					// with 1 single auto increment primary key
					if( pChangedRowToMove &&
						(pChangedRowToMove->m_bCommitted || pChangedRowToMove->m_eTypeOfChange!=toc_New || pChangedRowToMove->m_vectPrimaryKey.size()!=1) )
					{
						cerr << "Something is wrong with re-allocating an auto incr primary key: " << r_CommitRow.m_iNewAutoIncrID << "-" << r_CommitRow.m_iBeforeTransmit_iAutoIncrID << endl;
						throw "Internal error--data unexpected";
					}

					cout << "Checking:Reassigning Auto Incr Value for table (2): " << m_sName << " determined highest used value is: " << row_hi << " m_psc_id_next: " << m_psc_id_next << " pChangedRowToMove "
						<< (pChangedRowToMove==NULL ? " is null" : " is not null") << " highestid: " << iHighestUsedID << endl;
if( pChangedRowToMove )
cout << "pChangedRowToMove original auto incr id: " << pChangedRowToMove->m_iBeforeTransmit_iAutoIncrID << " psc_id: " << pChangedRowToMove->m_psc_id << endl;

					iHighestUsedID++;
					if( m_psc_id_next>iHighestUsedID )
						iHighestUsedID = m_psc_id_next;

					m_psc_id_next=iHighestUsedID+1;

					sSQL.str("");
					sSQL << "UPDATE `" << m_sName << "` SET PK_" << m_sName << "=" << iHighestUsedID << " WHERE PK_" << m_sName << "=" << r_CommitRow.m_iNewAutoIncrID;
					if( m_pDatabase->threaded_mysql_query( sSQL.str() )<0 )
						throw "Internal error--query failed moving auto incr key";

					PropagateUpdatedField( m_pField_AutoIncrement, StringUtils::itos( iHighestUsedID ), StringUtils::itos( r_CommitRow.m_iNewAutoIncrID ), pChangedRowToMove );
					if( pChangedRowToMove )
					{
						// These only apply if it's another row we're checking in
						m_mapUncommittedAutoIncrChanges[pChangedRowToMove->m_iOriginalAutoIncrID] = iHighestUsedID;
						pChangedRowToMove->m_iBeforeTransmit_iAutoIncrID = iHighestUsedID;
cout << "pChangedRowToMove now original auto incr id: " << pChangedRowToMove->m_iBeforeTransmit_iAutoIncrID << " psc_id: " << pChangedRowToMove->m_psc_id << endl;
						pChangedRowToMove->m_vectPrimaryKey[0] = StringUtils::itos(iHighestUsedID);
					}
				}

				sSQL.str( "" );
				sSQL << "UPDATE `" << m_sName << "` SET " << m_pField_AutoIncrement->Name_get() << "=" << r_CommitRow.m_iNewAutoIncrID << pChangedRow->GetWhereClause( );
				if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
				{
					cerr << "SQL failed: " << sSQL.str( );
					return false;
				}
			}

			sSQL.str( "" );
			sSQL << "UPDATE `" << m_sName << "` SET psc_mod=0 WHERE psc_id=" << ( toc==toc_New ? r_CommitRow.m_psc_id_new : r_CommitRow.m_psc_id );
			if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
			{
				cerr << "SQL failed: " << sSQL.str( );
				return false;
			}

			/**
				* First update this table's auto increment field before setting the psc_mod to 0. Then, handle the
				* propagate. That way if this table contains a foreign key to itself, or is somehow modified by
				* the propagate, it's modification flag will be set again
				*/

			if( r_CommitRow.m_iNewAutoIncrID && r_CommitRow.m_iNewAutoIncrID!=r_CommitRow.m_iBeforeTransmit_iAutoIncrID )
			{
				m_mapUncommittedAutoIncrChanges[pChangedRow->m_iOriginalAutoIncrID] = r_CommitRow.m_iNewAutoIncrID;
				PropagateUpdatedField( m_pField_AutoIncrement, StringUtils::itos( r_CommitRow.m_iNewAutoIncrID ), StringUtils::itos( r_CommitRow.m_iBeforeTransmit_iAutoIncrID ), pChangedRow );
			}
			pChangedRow->m_bCommitted=true;
		}
	}

	return true;
}

void Table::PropagateUpdatedField( Field *pField_Changed, string NewValue, string OldValue, ChangedRow *pChangedRow )
{
	for( ListField::iterator itList=pField_Changed->m_listField_ReferringToMe_Directly.begin( );itList!=pField_Changed->m_listField_ReferringToMe_Directly.end( );++itList )
	{
		Field *pField_FK = *itList;
		PropagateUpdatedField( pField_Changed, NewValue, OldValue, pChangedRow, pField_FK );
	}
	for( list< pair<string,Field *> >::iterator itList=pField_Changed->m_listField_ReferringToMe_Indirectly.begin( );itList!=pField_Changed->m_listField_ReferringToMe_Indirectly.end( );++itList )
	{
		Field *pField_FK = itList->second;
		PropagateUpdatedField( pField_Changed, NewValue, OldValue, pChangedRow, pField_FK, itList->first );
	}
}

void Table::PropagateUpdatedField( Field *pField_Changed, string NewValue, string OldValue, ChangedRow *pChangedRow, Field *pField_FK, string sIndirectFieldName )
{
	std::ostringstream sSQL;
	sSQL << "UPDATE `" << pField_FK->m_pTable->Name_get( ) << "` SET " << pField_FK->Name_get( ) << "='" << NewValue << "'" << " WHERE " << pField_FK->Name_get( ) << "='" << OldValue << "'";
	if( pField_FK->Name_get( ).substr(0,3)=="IK_" )
	{
		// It's an indirect key
		std::ostringstream sSQL_IK;
		sSQL_IK << "SELECT `PK_" << pField_FK->Name_get( ).substr(3) << "` FROM `" << pField_FK->Name_get( ).substr(3) << "` WHERE Description='" << sIndirectFieldName << "'";

		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result(sSQL_IK.str()) ) && ( row = mysql_fetch_row( result_set.r ) ) )
			sSQL << " AND `FK_" << pField_FK->Name_get( ).substr(3) << "`=" << row[0];
		else
		{
			cerr << "Failed to propagate indirect change: " << sSQL_IK.str( ) << endl;
			throw "Failed to propagate indirect change";
		}
	}
	if( pField_FK->m_pTable->m_s_psc_id_new_this_update.size() )
		sSQL << " AND psc_id NOT IN(" << pField_FK->m_pTable->m_s_psc_id_new_this_update << ")";

	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "Failed to propagate change: " << sSQL.str( ) << endl;
		throw "Failed to propagate change";
	}
}

void Table::UpdateRow( A_UpdateRow *pA_UpdateRow, R_UpdateTable *pR_UpdateTable, sqlCVSprocessor *psqlCVSprocessor )
{
	// This will be true if we skip updating this row, likely because we already modified it locally and haven't checked in
	bool bSkippingUpdate=false;

	std::ostringstream sSQL;
if( pA_UpdateRow->m_psc_id==1617 )
{
int k=2;
}
	// See if this is a new row, or an updated one
	// It's possible that we created the row originally, checked it in, then someone else modified it
	// In that case, m_psc_id_last_sync will be < since we haven't yet pulled updates.  When we pull
	// an update now, we will get the row and pA_UpdateRow->m_psc_id>m_psc_id_last_sync will be true
	// So we need to also double check that we don't already have it !psc_id_exists(pA_UpdateRow->m_psc_id)

	// AB 2005-08-17 it was: if( pA_UpdateRow->m_psc_id>m_psc_id_last_sync && !psc_id_exists(pA_UpdateRow->m_psc_id) )
	// but that means transactions rolled back never get included because the psc_id < sync
	if( !psc_id_exists(pA_UpdateRow->m_psc_id) )
	{
		if( pA_UpdateRow->m_psc_id<=m_psc_id_last_sync )
			cout << "WARNING: Adding back a row that should have already been there.  Probably was a rolled back batch" << endl;

		cout << "Updating (insert) row id " << pA_UpdateRow->m_psc_id << " last sync: " << m_psc_id_last_sync << endl;

		// It's possible that an incoming row is going to use the same primary key as one
		// we added locally but haven't checked in yet, or isn't approved
		int iAutoIncrementKeyValue=0;
		sSQL << "INSERT INTO `" << m_sName << "` ( ";

		for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
		{
			sSQL << "`" << ( *pR_UpdateTable->m_pvectFields )[s] << "`, ";
			if( m_pField_AutoIncrement && m_pField_AutoIncrement->Name_get()==( *pR_UpdateTable->m_pvectFields )[s] )
				iAutoIncrementKeyValue = atoi(pA_UpdateRow->m_vectValues[s].c_str());
		}

		sSQL << "psc_id, psc_batch, psc_user ) VALUES( ";

		for( size_t s=0;s<pA_UpdateRow->m_vectValues.size( );++s )
		{
			if( pA_UpdateRow->m_vectValues[s]==NULL_TOKEN )
				sSQL << "NULL, ";
			else
				sSQL << "'" << StringUtils::SQLEscape( pA_UpdateRow->m_vectValues[s] ) << "', ";
		}

		sSQL << pA_UpdateRow->m_psc_id << ", " << pA_UpdateRow->m_psc_batch << ", " << (pA_UpdateRow->m_psc_user ? StringUtils::itos(pA_UpdateRow->m_psc_user) : "NULL") << " )";

		if( m_s_psc_id_new_this_update.size() )
			m_s_psc_id_new_this_update += ",";
		m_s_psc_id_new_this_update += StringUtils::itos(pA_UpdateRow->m_psc_id);

		// First do a quick check to be sure there's no other record using this primary key
		if( iAutoIncrementKeyValue )
		{
			std::ostringstream sSQL2;
			sSQL2 << "SELECT `" << m_pField_AutoIncrement->Name_get() << "` FROM `" << m_sName
				<< "` WHERE `" << m_pField_AutoIncrement->Name_get() << "`=" << iAutoIncrementKeyValue;

			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			if( (result_set.r=m_pDatabase->mysql_query_result(sSQL2.str())) &&
				(row = mysql_fetch_row(result_set.r)) )
			{
				// We've got a conflict
				ReassignAutoIncrValue(iAutoIncrementKeyValue);
			}
		}

		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "Failed to insert row: " << sSQL.str( ) << endl;
			throw "Failed to insert row";
		}
	}
	else
	{
		cout << "Updating (update) row id " << pA_UpdateRow->m_psc_id << " last sync: " << m_psc_id_last_sync << endl;
		sSQL << "UPDATE `" << m_sName << "` SET ";
		if( ModifiedRow(pA_UpdateRow->m_psc_id) &&
				(g_GlobalConfig.m_bNoPrompts || !AskYNQuestion("In Table " + m_sName + " psc_id: " + StringUtils::itos(pA_UpdateRow->m_psc_id)
				+ " was modified on the server and locally.\nDelete your local changes?",false)) )
		{
			cout << "WARNING: *** Skipping locally updated row" << endl;
			bSkippingUpdate=true;
		}

		// It's possible that an incoming row is going to use the same primary key as one
		// we added locally but haven't checked in yet, or isn't approved
		int iAutoIncrementKeyValue=0;

		// If SkippingUpdate is true, then we've modified local rows.  We will need to change only the psc_batch and psc_user
		// and any auto increment primary key, since those are maintained by the server side
		for( size_t s=0;s<pR_UpdateTable->m_pvectFields->size( );++s )
		{
			if( m_pField_AutoIncrement && (*pR_UpdateTable->m_pvectFields)[s]==m_pField_AutoIncrement->Name_get() )
				iAutoIncrementKeyValue = atoi(pA_UpdateRow->m_vectValues[s].c_str());
			else if( bSkippingUpdate )
				continue;

			sSQL << "`" << ( *pR_UpdateTable->m_pvectFields )[s] << "`=";
			if( pA_UpdateRow->m_vectValues[s]==NULL_TOKEN )
				sSQL << "NULL, ";
			else
				sSQL << "'" << StringUtils::SQLEscape( pA_UpdateRow->m_vectValues[s] ) << "', ";
		}

		sSQL << "psc_batch=" << pA_UpdateRow->m_psc_batch << ", psc_user=" << (pA_UpdateRow->m_psc_user ? StringUtils::itos(pA_UpdateRow->m_psc_user) : "NULL") << " WHERE psc_id=" << pA_UpdateRow->m_psc_id;

		// First do a quick check to be sure there's no other record using this primary key
		if( iAutoIncrementKeyValue )
		{
			std::ostringstream sSQL2;
			sSQL2 << "SELECT `" << m_pField_AutoIncrement->Name_get() << "` FROM `" << m_sName
				<< "` WHERE `" << m_pField_AutoIncrement->Name_get() << "`=" << iAutoIncrementKeyValue;

			PlutoSqlResult result_set;
			MYSQL_ROW row=NULL;
			if( (result_set.r=m_pDatabase->mysql_query_result(sSQL2.str())) &&
				(row = mysql_fetch_row(result_set.r)) && atoi(row[0])!=iAutoIncrementKeyValue )
			{
				// We've got a conflict
				ReassignAutoIncrValue(iAutoIncrementKeyValue);
			}
		}


		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "Failed to update row: " << sSQL.str( ) << endl;
			throw "Failed to insert row";
		}
	}

	if( !bSkippingUpdate )
	{
		// Reset the mod flag
		sSQL.str( "" );
		sSQL << "UPDATE `" << m_sName << "` SET psc_mod=0 WHERE psc_id=" << pA_UpdateRow->m_psc_id;
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "SQL failed: " << sSQL.str( );
			throw "cannot reset psc_mod";
		}
	}

	if( pA_UpdateRow->m_psc_batch>m_psc_batch_last_sync )
		m_pRepository->psc_batch_last_sync_set(this,pA_UpdateRow->m_psc_batch);
	if( pA_UpdateRow->m_psc_id>m_psc_id_last_sync )
		m_pRepository->psc_id_last_sync_set(this,pA_UpdateRow->m_psc_id);
}

void Table::DeleteRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen, int &psc_user )
{
	// If the user is a supervisor, he can do anything, otherwise we queuing the batch
	if( !psqlCVSprocessor->m_bSupervisor )
	{
		std::ostringstream sSQL;
		sSQL << "SELECT 0 as psc_user, psc_frozen FROM `" << m_sName << "` WHERE psc_id=" << pR_CommitRow->m_psc_id;
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result(sSQL.str()) ) && ( row = mysql_fetch_row( result_set.r ) ) )
		{
			psqlCVSprocessor->m_i_psc_batch = pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->UnauthorizedBatch(psc_user);
			AddToHistory( pR_CommitRow, psqlCVSprocessor );
			return;
		}
		else
		{
			pR_CommitRow->m_sResponseMessage = "Row not found to delete";
			throw "Error: row not found to delete";
		}
	}

	std::ostringstream sSQL;
	sSQL << "DELETE FROM `" << m_sName << "` WHERE psc_id=" << pR_CommitRow->m_psc_id;

	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "Failed to delete row: " << sSQL.str( ) << endl;
		pR_CommitRow->m_sResponseMessage = "Failed to delete row: " + sSQL.str( ) + "-" + mysql_error(m_pDatabase->m_pMySQL);
		throw "Failed to delete row";
	}
	pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->m_i_psc_batch;
	AddToHistory( pR_CommitRow, psqlCVSprocessor );
}

void Table::UpdateRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen, int &psc_user )
{
	cout << "UpdateRow table: " << m_sName << " psc_id: " << pR_CommitRow->m_psc_id << " user: " << pR_CommitRow->m_psc_user << " is sup: " << psqlCVSprocessor->m_bSupervisor << endl;
	// If the user is a supervisor, he can do anything, otherwise be sure we're allowed to do this
	if( !psqlCVSprocessor->m_bSupervisor )
	{
		std::ostringstream sSQL;
		sSQL << "SELECT 0 as psc_user,psc_frozen FROM `" << m_sName << "` WHERE psc_id=" << pR_CommitRow->m_psc_id;
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( (result_set.r=m_pDatabase->mysql_query_result(sSQL.str())) && (row = mysql_fetch_row(result_set.r)) )
		{
			psqlCVSprocessor->m_i_psc_batch = pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->UnauthorizedBatch(psc_user);
			AddToHistory( pR_CommitRow, psqlCVSprocessor );
			return;
		}
		else
		{
			// The row is not here.  It must have been deleted.  Alternately it's possible that it
			// was a row checked in as unauthorized, never approved, and therefore only exists in the
			// history.  But that shouldn't happen since we no longer checkin modifications to unauthorized
			// rows
			cout << "Row not found " << sSQL.str() << endl;
			cout << "Skipping update -- row was deleted on the server" << endl;
			pR_CommitRow->m_cProcessOutcome=SKIPPING_ROW_DELETED;
			return;
		}
	}

	std::ostringstream sSQL;
	sSQL << "UPDATE `" << m_sName << "` SET ";

	for( size_t s=0;s<psqlCVSprocessor->m_pvectFields->size( );++s )
	{
		sSQL << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`=";
		if( pR_CommitRow->m_vectValues[s]==NULL_TOKEN )
			sSQL << "NULL, ";
		else
			sSQL << "'" << StringUtils::SQLEscape( pR_CommitRow->m_vectValues[s] ) << "', ";
	}
	sSQL << "psc_batch=" << psqlCVSprocessor->m_i_psc_batch << ",psc_restrict=";
	if( pR_CommitRow->m_psc_restrict )
		sSQL << pR_CommitRow->m_psc_restrict;
	else
		sSQL << "NULL";
	sSQL << " WHERE psc_id=" << pR_CommitRow->m_psc_id;

	// Add the history before we commit the change.  That way we can compare what the old values were
	pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->m_i_psc_batch;
	AddToHistory( pR_CommitRow, psqlCVSprocessor );

	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "Failed to update row: " << sSQL.str( ) << endl;
		pR_CommitRow->m_sResponseMessage = "Failed to update row: " + sSQL.str( ) + "-" + mysql_error(m_pDatabase->m_pMySQL);
		throw "Failed to update row";
	}
}

void Table::AddRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen )
{
	bool bUnauthorizedRow=false;
	std::ostringstream sSQL;

	//cout << "Debug: " << m_bAnonymous << " " << psqlCVSprocessor->m_bAllAnonymous << " " << psqlCVSprocessor->m_bSupervisor << " " << m_bFrozen << endl;
	if( (!m_bAnonymous && psqlCVSprocessor->m_bAllAnonymous) || (!psqlCVSprocessor->m_bSupervisor && m_bFrozen) )
	{
		cout << "Adding unauthorized row" << endl;
		// This row is unauthorized, so we're going to insert into the history instead.  But we'll insert it here
		// rather than in AddToHistory because AddToHistory is intended to add rows that already were added to the database,
		// but in this case, we're not putting the row in the database--only in the history file
		bFrozen=true;
		psqlCVSprocessor->m_i_psc_batch = pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->UnauthorizedBatch(psqlCVSprocessor->m_ipsc_user_default);
		bUnauthorizedRow=true;
		sSQL << "INSERT INTO `" << m_pTable_History->Name_get( ) << "` ( ";
	}
	else
	{
		cout << "Adding authorized row" << endl;
		sSQL << "INSERT INTO `" << m_sName << "` ( ";
	}

	for( size_t s=0;s<psqlCVSprocessor->m_pvectFields->size( );++s )
		sSQL << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`, ";

	sSQL << "psc_id, psc_batch, psc_user, psc_restrict ) VALUES( ";

	for( size_t s=0;s<pR_CommitRow->m_vectValues.size( );++s )
	{
		if( pR_CommitRow->m_vectValues[s]==NULL_TOKEN )
			sSQL << "NULL, ";
		else
			sSQL << "'" << StringUtils::SQLEscape( pR_CommitRow->m_vectValues[s] ) << "', ";
	}

	// Even if the row is unauthorized, it will need a unique psc_id so we can change it later
	sSQL << m_psc_id_next << ", " << psqlCVSprocessor->m_i_psc_batch << ", ";

	if( pR_CommitRow->m_psc_user )
		sSQL << pR_CommitRow->m_psc_user;
	else if( psqlCVSprocessor->m_ipsc_user_default )
	{
		sSQL << psqlCVSprocessor->m_ipsc_user_default;
		pR_CommitRow->m_psc_user_new = psqlCVSprocessor->m_ipsc_user_default;
	}
	else
		sSQL << "NULL";

	sSQL << ",";
	if( pR_CommitRow->m_psc_restrict )
		sSQL << pR_CommitRow->m_psc_restrict;
	else
		sSQL << "NULL";

	sSQL << " )"; /** @warning batch # todo - hack */

	cout << "Adding new row with id: " << m_psc_id_next << endl;

	pR_CommitRow->m_psc_id_new = m_psc_id_next;
	pR_CommitRow->m_psc_batch_new = psqlCVSprocessor->m_i_psc_batch;

	if( !bUnauthorizedRow && m_pField_AutoIncrement )
	{
		if( !( pR_CommitRow->m_iNewAutoIncrID=m_pDatabase->threaded_mysql_query_withID( sSQL.str( ) ) ) )
		{
			cerr << "Failed to insert row1: " << sSQL.str( ) << endl;
			pR_CommitRow->m_sResponseMessage = "Failed to insert row #1: " + sSQL.str( ) + "-" + mysql_error(m_pDatabase->m_pMySQL);
			throw "Failed to insert row";
		}
	}
	else
	{
		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "Failed to insert row: " << sSQL.str( ) << endl;
			pR_CommitRow->m_sResponseMessage = "Failed to insert row #2: " + sSQL.str( ) + "-" + mysql_error(m_pDatabase->m_pMySQL);
			throw "Failed to insert row";
		}
		pR_CommitRow->m_iNewAutoIncrID=0;
	}

	if( !bUnauthorizedRow ) // See notes above
		AddToHistory( pR_CommitRow, psqlCVSprocessor );

	m_psc_id_next++;
}

void Table::GetCurrentValues(int psc_id,MapStringString *mapCurrentValues,int psc_batch_in_history)
{
	std::ostringstream sSql;
	if( psc_batch_in_history )
		sSql << "SELECT * FROM " << m_pTable_History->Name_get() << " WHERE psc_id=" << psc_id << " AND psc_batch=" << psc_batch_in_history;
	else
		sSql << "SELECT * FROM `" << m_sName << "` WHERE psc_id=" << psc_id;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSql.str() ) ) && ( row = mysql_fetch_row( result_set.r ) ) )
	{
		for(int i=0;i<(int) result_set.r->field_count;++i)
		{
			if( row[i] )
				(*mapCurrentValues)[result_set.r->fields[i].name] = row[i];
			else
				(*mapCurrentValues)[result_set.r->fields[i].name] = NULL_TOKEN;
		}
	}
	else
	{
		cerr << "Unable to find record to retrieve current values (gcv) for table: " << m_sName << " psc_id: " << psc_id << endl;
		throw "Canot locate record";
	}
}

void Table::GetBatchContents(int psc_batch,map<int,ChangedRow *> &mapChangedRow)
{
	std::ostringstream sSql;
	sSql << "SELECT psc_id,psc_toc,`" << m_sName << "_pschist`.* FROM " << m_sName << "_pschist WHERE psc_batch=" << psc_batch;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSql.str() ) ) )
	{
		while (( row = mysql_fetch_row( result_set.r ) ) )
		{
			int psc_id = atoi(row[0]);
			TypeOfChange psc_toc = (TypeOfChange) atoi(row[1]);

			cout << "Retrieving change to table: " << m_sName << " batch: " << psc_batch << " id: " << psc_id << " toc: " << (int) psc_toc << endl;

			if( psc_toc==toc_Delete )
			{
				ChangedRow *pChangedRow = new ChangedRow(this,psc_toc,psc_id,psc_batch);
				mapChangedRow[psc_id] = pChangedRow;
				continue;
			}

			ChangedRow *pChangedRow = new ChangedRow( this, psc_toc, psc_id, psc_batch );

			map<string,bool> mapChangedFields;  // We only need a list of fields, but use a map for faster lookup

			// Now we have to see what fields were changed
			sSql.str("");
			sSql << "SELECT * FROM " << m_sName << "_pschmask WHERE psc_batch=" << psc_batch;
			PlutoSqlResult result_set2;
			MYSQL_ROW row2=NULL;
			if( psc_toc!=toc_Modify || ( result_set2.r=m_pDatabase->mysql_query_result( sSql.str() ) )==NULL || ( row2 = mysql_fetch_row( result_set2.r ) )==NULL )
			{
				cout << "It's either not a modify (2) or it's not in pschist"  << endl;
				/* This shouldn't happen, but since it did, just add all fields */
				if( psc_toc==toc_Modify )
					cerr << "***ERROR*** Cannot find value in history mask table:" << sSql.str() << endl;

				for(MapField::iterator it=m_mapField.begin();it!=m_mapField.end();++it)
				{
					string Field = (*it).first;
					if( Field=="psc_id" || Field=="psc_batch" || Field=="psc_toc" )
						continue;
					mapChangedFields[Field]=true;
				}
			}
			else
			{
				cout << "It's a modify and it's in pschist"  << endl;
				for(int i=0;i<(int) result_set2.r->field_count;++i)
				{
					string Field = result_set2.r->fields[i].name;
					if( Field=="psc_id" || Field=="psc_batch" || Field=="psc_toc" || !row2[i] || !atoi(row2[i]) )
						continue;
					mapChangedFields[Field]=true;
				}
			}
			for(int i=0;i<(int) result_set.r->field_count;++i)
			{
				string Field = result_set.r->fields[i].name;
				if( mapChangedFields.find(Field)==mapChangedFields.end() )
					continue;

				pChangedRow->m_mapFieldValues[Field] = (row[i] ? row[i] : NULL_TOKEN);
			}

			cout << "Row has %d changed fields: " << (int) pChangedRow->m_mapFieldValues.size() << endl;
			mapChangedRow[psc_id] = pChangedRow;
		}
	}
	else
	{
		cerr << "Unable to find record to retrieve current values (gbc) for table: " << m_sName << " psc_batch: " << psc_batch << endl;
		throw "Cannot locate record";
	}
}

void Table::AddToHistory( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor,int psc_batch_in_history )
{
	if( !m_pTable_History || !m_pTable_History_Mask )
		return;

	if( pR_CommitRow->m_eTypeOfChange==( int ) sqlCVS::toc_Delete )
	{
		std::ostringstream sSqlHistory;
		sSqlHistory << "INSERT INTO `" << m_pTable_History->Name_get( ) << "` (psc_id,psc_batch,psc_toc,psc_restrict) VALUES ("
			<< pR_CommitRow->m_psc_id << ", " << psqlCVSprocessor->m_i_psc_batch << "," << pR_CommitRow->m_eTypeOfChange << ",";
		if( pR_CommitRow->m_psc_restrict )
			sSqlHistory << pR_CommitRow->m_psc_restrict;
		else
			sSqlHistory << "NULL";
		sSqlHistory << ")";
		if( m_pDatabase->threaded_mysql_query( sSqlHistory.str( ) )<0 )
		{
			cerr << "Failed to insert history delete row: " << sSqlHistory.str( ) << endl;
			pR_CommitRow->m_sResponseMessage = "Failed to iinsert history delete row: " + sSqlHistory.str( ) + "-" + mysql_error(m_pDatabase->m_pMySQL);
			throw "Failed to insert history delete row";
		}
		return;
	}

	std::ostringstream sSqlHistory,sSqlMask;
	sSqlHistory << "INSERT INTO `" << m_pTable_History->Name_get( ) << "`( ";
	sSqlMask << "INSERT INTO `" << m_pTable_History_Mask->Name_get( ) << "`( ";

	MapStringString mapCurrentValues;
	GetCurrentValues((pR_CommitRow->m_eTypeOfChange==toc_New ? pR_CommitRow->m_psc_id_new : pR_CommitRow->m_psc_id),&mapCurrentValues,psc_batch_in_history);
	int NumChangedFields=0;

	for( size_t s=0;s<psqlCVSprocessor->m_pvectFields->size( );++s )
		sSqlHistory << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`, ";

	/** If this isn't a new row, any auto-increment fields are passed like normal  */

	if( m_pField_AutoIncrement && pR_CommitRow->m_eTypeOfChange==toc_New )
		sSqlHistory << m_pField_AutoIncrement->Name_get( ) << ", ";

	sSqlHistory << "psc_toc, psc_id, psc_batch, psc_user,psc_restrict ) VALUES( ";

	for( size_t s=0;s<pR_CommitRow->m_vectValues.size( );++s )
	{
		if( pR_CommitRow->m_vectValues[s]==NULL_TOKEN )
			sSqlHistory << "NULL, ";
		else
			sSqlHistory << "'" << StringUtils::SQLEscape( pR_CommitRow->m_vectValues[s] ) << "', ";

		if( mapCurrentValues[( *psqlCVSprocessor->m_pvectFields )[s]] != pR_CommitRow->m_vectValues[s] )
		{
			sSqlMask << "`" << ( *psqlCVSprocessor->m_pvectFields )[s] << "`, ";
			NumChangedFields++;
		}
	}

	sSqlMask << "psc_id, psc_batch ) VALUES( ";
	for(int i=0;i<NumChangedFields;++i)
		sSqlMask << "1,";

	if( m_pField_AutoIncrement && pR_CommitRow->m_eTypeOfChange==toc_New )
		sSqlHistory << pR_CommitRow->m_iNewAutoIncrID << ", ";

	sSqlHistory << pR_CommitRow->m_eTypeOfChange << "," << ( pR_CommitRow->m_eTypeOfChange==toc_New ? pR_CommitRow->m_psc_id_new : pR_CommitRow->m_psc_id ) << ", " << pR_CommitRow->m_psc_batch_new << ", "
		<< (pR_CommitRow->m_psc_user ? StringUtils::itos(pR_CommitRow->m_psc_user) : "NULL") << ","
		<< (pR_CommitRow->m_psc_restrict ? StringUtils::itos(pR_CommitRow->m_psc_restrict) : "NULL") << ")"; // batch # todo - hack
	sSqlMask << ( pR_CommitRow->m_eTypeOfChange==toc_New ? pR_CommitRow->m_psc_id_new : pR_CommitRow->m_psc_id ) << ", " << psqlCVSprocessor->m_i_psc_batch << " )"; // batch # todo - hack
	if( m_pDatabase->threaded_mysql_query( sSqlHistory.str( ) )<0 || m_pDatabase->threaded_mysql_query( sSqlMask.str( ) )<0 )
	{
		cerr << "Failed to insert history row: " << sSqlHistory.str( ) << endl;
		pR_CommitRow->m_sResponseMessage = "Failed to history row #2: " + sSqlHistory.str( ) + "-" + mysql_error(m_pDatabase->m_pMySQL);
		throw "Failed to insert row";
	}
}

void Table::AddToHistory( ChangedRow *pChangedRow )
{
	if( !m_pTable_History || !m_pTable_History_Mask )
		return;

	if( pChangedRow->m_eTypeOfChange==toc_Delete )
	{
		std::ostringstream sSqlHistory;
		sSqlHistory << "INSERT INTO `" << m_pTable_History->Name_get( ) << "` (psc_id,psc_batch,psc_toc,psc_restrict) VALUES ("
			<< pChangedRow->m_psc_id << ", " << pChangedRow->m_psc_batch << "," << pChangedRow->m_eTypeOfChange << ","
			<< (pChangedRow->m_psc_restrict ? StringUtils::itos(pChangedRow->m_psc_restrict) : "NULL") << ")";
		if( m_pDatabase->threaded_mysql_query( sSqlHistory.str( ) )<0 )
		{
			cerr << "Failed to insert history delete row: " << sSqlHistory.str( ) << endl;
			throw "Failed to insert row";
		}
		return;
	}

	std::ostringstream sSqlHistory,sSqlMask;
	sSqlHistory << "INSERT INTO `" << m_pTable_History->Name_get( ) << "` ( ";
	sSqlMask << "INSERT INTO `" << m_pTable_History_Mask->Name_get( ) << "` ( ";

	int NumChangedFields=0;

	bool b_psc_user_passedin=false;

	for( MapStringString::iterator it=pChangedRow->m_mapFieldValues.begin();it!=pChangedRow->m_mapFieldValues.end();++it )
	{
		// When we're approving a batch, GetBatchContents will have retrieved the auto increment key.  However,
		// we don't want to use it, since it will be NULL.  We're going to use the NewAutoIncrValue
		if( m_pField_AutoIncrement && pChangedRow->m_eTypeOfChange==toc_New && (*it).first==m_pField_AutoIncrement->Name_get() )
			continue;

		if( it->first=="psc_toc" || it->first=="psc_id" || it->first=="psc_batch" || it->first=="psc_restrict" )
			continue;
		sSqlHistory << "`" << (*it).first << "`, ";
		if( (*it).first=="psc_user" )
			b_psc_user_passedin=true;
	}

	/** If this isn't a new row, any auto-increment fields are passed like normal  */

	if( m_pField_AutoIncrement && pChangedRow->m_eTypeOfChange==toc_New )
		sSqlHistory << m_pField_AutoIncrement->Name_get( ) << ", ";

	sSqlHistory << "psc_toc, psc_id, psc_batch,psc_restrict ";
	if( !b_psc_user_passedin )
		sSqlHistory << ", psc_user ";

	sSqlHistory << ") VALUES( ";

	for( MapStringString::iterator it=pChangedRow->m_mapFieldValues.begin();it!=pChangedRow->m_mapFieldValues.end();++it )
	{
		// When we're approving a batch, GetBatchContents will have retrieved the auto increment key.  However,
		// we don't want to use it, since it will be NULL.  We're going to use the NewAutoIncrValue
		if( m_pField_AutoIncrement && pChangedRow->m_eTypeOfChange==toc_New && (*it).first==m_pField_AutoIncrement->Name_get() )
			continue;

		if( it->first=="psc_toc" || it->first=="psc_id" || it->first=="psc_batch" || it->first=="psc_restrict" )
			continue;

		if( (*it).second==NULL_TOKEN )
			sSqlHistory << "NULL, ";
		else
			sSqlHistory << "'" << StringUtils::SQLEscape( (*it).second ) << "', ";

		if( (*it).second.length() )
		{
			sSqlMask << "`" << (*it).first << "`, ";
			NumChangedFields++;
		}
	}

	sSqlMask << "psc_id, psc_batch ) VALUES( ";
	for(int i=0;i<NumChangedFields;++i)
		sSqlMask << "1,";

	if( m_pField_AutoIncrement && pChangedRow->m_eTypeOfChange==toc_New )
		sSqlHistory << pChangedRow->m_iNewAutoIncrID << ", ";

	sSqlHistory << pChangedRow->m_eTypeOfChange << "," << pChangedRow->m_psc_id << ", " << pChangedRow->m_psc_batch
		<< "," << (pChangedRow->m_psc_restrict ? StringUtils::itos(pChangedRow->m_psc_restrict) : "NULL");

	if( !b_psc_user_passedin )
		sSqlHistory << ", " << (pChangedRow->m_psc_user ? StringUtils::itos(pChangedRow->m_psc_user) : "NULL");
	sSqlHistory << " )"; // batch # todo - hack
	sSqlMask << pChangedRow->m_psc_id << ", " << pChangedRow->m_psc_batch << " )"; // batch # todo - hack
	if( m_pDatabase->threaded_mysql_query( sSqlHistory.str( ) )<0 || m_pDatabase->threaded_mysql_query( sSqlMask.str( ) )<0 )
	{
		cerr << "Failed to insert history row: " << sSqlHistory.str( ) << endl;
		throw "Failed to insert row";
	}
}

bool Table::Dump( SerializeableStrings &str )
{
	int num_psc_id=-1,num_psc_mod=-1;

	str.m_vectString.push_back( m_sName );

	std::ostringstream sSQL;
	sSQL << "DESCRIBE `" << m_sName << "`";
	PlutoSqlResult result_set2;
	MYSQL_ROW row=NULL;
	if( !( result_set2.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		return false;

	int FieldCount=0;
	string sFieldList;
	str.m_vectString.push_back( StringUtils::itos( ( int ) ( result_set2.r->row_count ) ) );
	while( ( row = mysql_fetch_row( result_set2.r ) ) )
	{
		if( sFieldList.length( ) )
			sFieldList += ", ";

		sFieldList += "`" + m_sName + "`.`" + row[0] + "`";

		if( strcmp(row[0],"psc_id")==0 )
			num_psc_id = FieldCount;

		if( strcmp(row[0],"psc_mod")==0 )
			num_psc_mod = FieldCount;

		FieldCount++;

		for( size_t s=0;s<6;++s )
			str.m_vectString.push_back( row[s] ? row[s] : NULL_TOKEN );
	}

	sSQL.str( "" );
	sSQL << "SHOW INDEX FROM `" << m_sName << "`";
	PlutoSqlResult result_set3;
	if( !( result_set3.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		return false;

	vector<string> vectIndex;
	row = mysql_fetch_row( result_set3.r );
	while( row )
	{
		if( strcmp(row[2],"PRIMARY")==0 )
		{
			row = mysql_fetch_row( result_set3.r );
			continue;
		}

		string sIndex;
		if( row[1] && row[1][0]!='1' )
			sIndex = "add unique ";
		else
			sIndex = "add index ";

		string sName = row[2];
		sIndex += "`" + sName + "` (`" + row[4] + "`";

		while(true) // All subsequent rows with the same name are part of the same index
		{
			row = mysql_fetch_row( result_set3.r );
			if( !row || sName!=row[2] )
				break;

			// It's another field in the same index
			sIndex += string(",`") + row[4] + "`";
		}

		sIndex += ")";
		vectIndex.push_back(sIndex);
	}

	str.m_vectString.push_back( StringUtils::itos( ( int ) vectIndex.size() ) );
	for(size_t sVI=0;sVI<vectIndex.size();++sVI)
		str.m_vectString.push_back( vectIndex[sVI] );

	StringUtils::Replace(&m_sFilter,"<%=U%>",g_GlobalConfig.csvUserID());
	sSQL.str( "" );
	sSQL << "SELECT " << sFieldList << " FROM " << "`" << m_sName << "`" << (m_sFilter.length() && StringUtils::ToUpper(m_sFilter).find("WHERE")==string::npos ? " WHERE " : " " ) << m_sFilter;
	if( TrackChanges_get() )
		sSQL << (m_sFilter.length() ? " AND " : " WHERE ") << g_GlobalConfig.GetRestrictionClause(m_sName);

	if( num_psc_id!=-1 )
		sSQL << " ORDER BY psc_id";
	PlutoSqlResult result_set;
	if( !( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) )
		return false;

	cout << "Table: " << m_sName << "\t\tDumping " << result_set.r->row_count << " rows" << endl;

	str.m_vectString.push_back( StringUtils::itos( ( int ) result_set.r->row_count ) );
	while( ( row = mysql_fetch_row( result_set.r ) ) )
	{
		for( size_t s=0;s<m_mapField.size( );++s )
		{
			if( s==num_psc_id && (!row[s] || !atoi(row[s])) )
			{
				cerr << "Not all the records in table: " << m_sName << " have an ID." << endl
					<< "You can run sqlCVS with the 'update-psc' command to correct this," << endl
					<< "however it should never have happened." << endl;
				throw "Missing psc_id";
			}
			if( s==num_psc_mod )
				str.m_vectString.push_back( "0" );
			else
				str.m_vectString.push_back( row[s] ? row[s] : NULL_TOKEN );
		}
	}

	return true;
}

bool Table::RevertAllChanges()
{
	for(map<int, ListChangedRow *>::iterator itCR=m_mapUsers2ChangedRowList.begin();itCR!=m_mapUsers2ChangedRowList.end();++itCR)
	{
		ListChangedRow *pListChangedRow = (*itCR).second;
		for(ListChangedRow::iterator itLCR=pListChangedRow->begin();itLCR!=pListChangedRow->end();++itLCR)
		{
			ChangedRow *pChangedRow = *itLCR;
			RevertChange(pChangedRow);
		}
	}
	return true;
}

bool Table::RevertChange(ChangedRow *pChangedRow)
{
	if( pChangedRow->m_bReverted )
		return false;
	if( pChangedRow->m_eTypeOfChange==toc_New )
	{
		std::ostringstream sSQL;
		if( !g_GlobalConfig.m_bNoPrompts && AskYNQuestion("Delete the new row you added?",false)==false )
			return false;
		sSQL << "DELETE FROM " << m_sName << pChangedRow->GetWhereClause();

		if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
		{
			cerr << "Revert change: " << sSQL.str( ) << endl;
			throw "Database error";
		}
	}
	else
	{
		if( RevertChange(pChangedRow->m_psc_id,pChangedRow->m_eTypeOfChange) )
			pChangedRow->m_bReverted=true;
	}
	return true;
}

bool Table::RevertChange(int psc_id,enum TypeOfChange toc)
{
	std::ostringstream sSQL;
	// We need the row from the server
	R_GetRow r_GetRow( m_sName, psc_id );

	int ClientID=1, SoftwareVersion=1; /** @warning HACK!!! */
	RA_Processor ra_Processor( ClientID, SoftwareVersion, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );
	ra_Processor.AddRequest( &r_GetRow );

	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests( g_GlobalConfig.m_sSqlCVSHost + ":" + StringUtils::itos(g_GlobalConfig.m_iSqlCVSPort), &pSocket ) );

	if( r_GetRow.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get changed record from server:" << (int) r_GetRow.m_cProcessOutcome << endl;
		throw "Communication error";
	}

	if( toc==toc_Delete )
	{
		sSQL << "INSERT INTO " << m_sName << "(";
		for( MapStringString::iterator it=r_GetRow.m_mapCurrentValues.begin();it!=r_GetRow.m_mapCurrentValues.end();++it)
			sSQL << (it!=r_GetRow.m_mapCurrentValues.begin() ? "," : "") << "`" << (*it).first << "`";
		sSQL << ") VALUES (";
		for( MapStringString::iterator it=r_GetRow.m_mapCurrentValues.begin();it!=r_GetRow.m_mapCurrentValues.end();++it)
		{
			if( it!=r_GetRow.m_mapCurrentValues.begin() )
				sSQL << ",";

			if( (*it).first=="psc_mod" )
				sSQL << "0";
			else if( (*it).second==NULL_TOKEN )
				sSQL << "NULL";
			else
				sSQL << "'" << StringUtils::SQLEscape((*it).second) << "'";
		}
		sSQL << ")";
	}
	else if( toc==toc_Modify )
	{
		sSQL << "UPDATE " << m_sName << " SET ";
		for( MapStringString::iterator it=r_GetRow.m_mapCurrentValues.begin();it!=r_GetRow.m_mapCurrentValues.end();++it)
		{
			if( it!=r_GetRow.m_mapCurrentValues.begin() )
				sSQL << ",";

			sSQL << "`" << (*it).first << "`=";

			if( (*it).first=="psc_mod" )
				sSQL << "0";
			else if( (*it).second==NULL_TOKEN )
				sSQL << "NULL";
			else
				sSQL << "'" << StringUtils::SQLEscape((*it).second) << "'";
		}
		sSQL << " WHERE psc_id=" << psc_id;
	}
	delete pSocket;

	if( m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "Revert change: " << sSQL.str( ) << endl;
		throw "Database error";
	}

	return true;
}

bool Table::ShowChanges(bool bAllUsers)
{
	while(true)
	{
		if( !bAllUsers )
		{
			cout << "Changes for table: " << m_sName << endl;
			cout << "    User     New   Mod   Del" << endl;
		}

		for(map<int, ListChangedRow *>::iterator itCR=m_mapUsers2ChangedRowList.begin();itCR!=m_mapUsers2ChangedRowList.end();++itCR)
		{
			if( bAllUsers )
			{
				if( !ShowChanges( (*itCR).first ) )
					return false;
				continue;
			}
			int iUser = (*itCR).first;
			ListChangedRow *pListChangedRow = (*itCR).second;

			cout << setw( 7 ) << iUser << " ";

			int iNew=0,iMod=0,iDel=0;
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
			cout << setw( 6 ) << iNew << setw( 6 ) << iMod << setw( 6 ) << iDel << endl;
		}
		if( bAllUsers )
			return true;

		cout << "What user do you want more detail on?" << endl <<
			"Enter 'b' to go back, 'q' to quit" << endl <<
			"Enter 'a' to show all, 'r' to revert all" << endl;
		string sUser;
		cin >> sUser;
		if( sUser=="b" || sUser=="B" )
			return true;
		if( sUser=="q" || sUser=="Q" )
			return false;
		if( sUser=="a" || sUser=="A" )
			return ShowChanges(true);
		if( sUser=="r" || sUser=="R" )
			return RevertAllChanges();

		if( !ShowChanges( atoi(sUser.c_str()) ) )
			return false;
	}
}

bool Table::ShowChanges(int psc_user)
{
	ListChangedRow *pListChangedRow = m_mapUsers2ChangedRowList_Find(psc_user);
	while(true)
	{
		vector<ChangedRow *> vectChangedRow; // So we can revert
		cout << "Changes to table: " << m_sName << " by user: " << psc_user << endl;

		for(ListChangedRow::iterator it=pListChangedRow->begin();it!=pListChangedRow->end();++it)
		{
			ChangedRow *pChangedRow = *it;
			if( pChangedRow->m_bReverted )
				continue;
			vectChangedRow.push_back(pChangedRow); // We need something to reference these numerically
			cout << setw(4) << vectChangedRow.size();
			if( pChangedRow->m_eTypeOfChange==toc_Delete )
				 cout << " DEL psc_id=" << pChangedRow->m_psc_id << endl;
			else if( pChangedRow->m_eTypeOfChange==toc_Modify )
				cout << " MOD psc_id=" << pChangedRow->m_psc_id << endl;
			else if( pChangedRow->m_eTypeOfChange==toc_New )
			{
				cout << " NEW ";
				size_t sF=0;
				for(ListField::iterator itF=m_listField_PrimaryKey.begin();itF!=m_listField_PrimaryKey.end();++itF)
				{
					Field *pField = *itF;
					cout << pField->Name_get() << "=" << pChangedRow->m_vectPrimaryKey[sF++];
				}
				cout << endl;
			}
		}

		cout << "Enter 'b' to go back, 'q' to quit, 'rX' to revert change X" << endl;
		string sRow;
		cin >> sRow;
		if( sRow=="b" || sRow=="B" )
			return true;
		else if( sRow=="q" || sRow=="Q" )
			return false;
		if( sRow.size()>1 && (sRow[0]=='r' || sRow[0]=='R') )
		{
			int iChangeNum=atoi( sRow.substr(1).c_str() );
			if( iChangeNum<1 || iChangeNum>vectChangedRow.size() )
				cout << "Bad number" << endl;
			else
				RevertChange( vectChangedRow[iChangeNum-1] );
		}
	}
}

void Table::ApplyChangedRow(ChangedRow *pChangedRow)
{
	cout << "Apply changed row for id: " << pChangedRow->m_psc_id << " type: " << (int) pChangedRow->m_eTypeOfChange << " fields: "
		<< "Row has changed fields: " << (int) pChangedRow->m_mapFieldValues.size() << endl;

	ostringstream sSql;
	if( pChangedRow->m_eTypeOfChange==toc_Delete )
	{
		sSql << "DELETE FROM `" << m_sName << "` WHERE psc_id=" << pChangedRow->m_psc_id;
	}
	else if( pChangedRow->m_eTypeOfChange==toc_Modify && pChangedRow->m_mapFieldValues.size() )
	{
		bool bFound_psc_batch=false;
		sSql << "UPDATE `" << m_sName << "` SET ";
		bool bFirst=true;
		for(MapStringString::iterator it=pChangedRow->m_mapFieldValues.begin();it!=pChangedRow->m_mapFieldValues.end();++it)
		{
			if( bFirst )
				bFirst=false;
			else
				sSql << ",";
			sSql << "`" << (*it).first << "`=";
			if( it->second==NULL_TOKEN )
				sSql << "NULL";
			else
				sSql << "'" << StringUtils::SQLEscape((*it).second) << "'";
			if( (*it).first=="psc_batch" )
				bFound_psc_batch=true;
		}
		if( !bFound_psc_batch )
		{
			if( !bFirst )
				sSql << ",";
			sSql << "psc_batch=" << pChangedRow->m_psc_batch;
		}
		sSql << " WHERE psc_id=" << pChangedRow->m_psc_id;
	}
	else if( pChangedRow->m_eTypeOfChange==toc_New && pChangedRow->m_mapFieldValues.size() )
	{
		sSql << "INSERT INTO `" << m_sName << "` (";
		bool bFirst=true;
		bool bFound_psc_id=false,bFound_psc_batch=false;
		for(MapStringString::iterator it=pChangedRow->m_mapFieldValues.begin();it!=pChangedRow->m_mapFieldValues.end();++it)
		{
			if( bFirst )
				bFirst=false;
			else
				sSql << ",";
			sSql << "`" << (*it).first << "`";
			if( (*it).first=="psc_id" )
				bFound_psc_id=true;
			else if( (*it).first=="psc_batch" )
				bFound_psc_batch=true;
		}

		if( !bFound_psc_id )
			sSql << ",psc_id";
		if( !bFound_psc_batch )
			sSql << ",psc_batch";

		sSql << ") VALUES(";
		bFirst=true;
		for(MapStringString::iterator it=pChangedRow->m_mapFieldValues.begin();it!=pChangedRow->m_mapFieldValues.end();++it)
		{
			if( bFirst )
				bFirst=false;
			else
				sSql << ",";
			if( it->second==NULL_TOKEN )
				sSql << "NULL";
			else
				sSql << "'" << StringUtils::SQLEscape((*it).second) << "'";
		}
		if( !bFound_psc_id )
			sSql << "," << pChangedRow->m_psc_id;
		if( !bFound_psc_batch )
			sSql << "," << pChangedRow->m_psc_batch;
		sSql << ")";
	}

	if( pChangedRow->m_eTypeOfChange==toc_New && m_pField_AutoIncrement && sSql.str().size()!=0 )
	{
		if( !( pChangedRow->m_iNewAutoIncrID=m_pDatabase->threaded_mysql_query_withID( sSql.str( ) ) ) )
		{
			cerr << "Failed to insert row: " << sSql.str( ) << endl;
			throw "Failed to insert row";
		}

	}
	else
	{
		if( sSql.str().size()==0 )
		{
			cout << "WARNING: Nothing to do applying this change table: " << m_sName << " psc_id: " << pChangedRow->m_psc_id << endl;
		}
		else if( m_pDatabase->threaded_mysql_query( sSql.str( ) )<0 )
		{
			cerr << "Error applying changed row: " << sSql.str() << endl;
			throw "Database error applying changed row";
		}
		pChangedRow->m_iNewAutoIncrID=0;
	}
}

void Table::VerifyIntegrity()
{
	if( m_pRepository && !m_bIsSystemTable )
	{
		ostringstream sql;
		PlutoSqlResult result_set,result_set2,result_set3;
		MYSQL_ROW row=NULL;

		sql << "SELECT psc_id FROM `" << m_sName << "` ORDER BY psc_id";

		if( g_GlobalConfig.m_bVerifyID )
		{
			int psc_id_last=0;

			if( g_GlobalConfig.m_sCommand!="update-psc" && ( result_set.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
			{
				while( ( row = mysql_fetch_row( result_set.r ) ) )
				{
					if( !row[0] || !atoi(row[0]) || atoi(row[0])==psc_id_last )
					{
						cerr << "In table: " << m_sName << " all rows don't have unique psc_id's: " << (row[0] ? row[0] : "blank") << endl;
						throw "Database integrity failed";
					}
					psc_id_last = atoi(row[0]);
				}
			}
		}

		string sRestrictions_R = g_GlobalConfig.GetRestrictionClause("R");
		string sRestrictions_L = g_GlobalConfig.GetRestrictionClause("L");
		string sRestrictions_T3 = g_GlobalConfig.GetRestrictionClause("T3");
		string sRestrictions_T1 = g_GlobalConfig.GetRestrictionClause("T1");

		if( g_GlobalConfig.m_bVerify )
		{
			for(MapField::iterator it=m_mapField.begin();it!=m_mapField.end();++it)
			{
				Field *pField = (*it).second;

				if( g_GlobalConfig.m_sSkipVerification.find( m_sName + ":" + pField->Name_get() )!=string::npos )
					continue;

				if( pField->m_pField_IReferTo_Directly && pField->m_pField_IReferTo_Directly->m_pTable && pField->m_pField_IReferTo_Directly->m_pTable->m_pRepository )
				{
					sql.str("");
					sql << "SELECT L.`" << pField->Name_get() << "`,R.`" << pField->m_pField_IReferTo_Directly->Name_get() << "` FROM " <<
						"`" << Name_get() << "` As L LEFT JOIN `" << pField->m_pField_IReferTo_Directly->Table_get()->Name_get() << "` As R " <<
						" ON L.`" << pField->Name_get() << "`=" <<
						"R.`" << pField->m_pField_IReferTo_Directly->Name_get() << "` AND " << sRestrictions_R << " WHERE " <<
						"L.`" << pField->Name_get() << "` IS NOT NULL " <<
						"AND R.`" << pField->m_pField_IReferTo_Directly->Name_get() << "` IS NULL AND " << sRestrictions_L;

					if( ( result_set2.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
					{
						string Keys="";
						while( (row = mysql_fetch_row(result_set2.r)) )
						{
							cout << "In table: " << m_sName << ":" << pField->Name_get() << ":";
							cout << (row[0] ? row[0] : "NULL") << " ";
							cout << pField->m_pField_IReferTo_Directly->Table_get()->Name_get() << ":" << pField->m_pField_IReferTo_Directly->Name_get() << ":" << (row[1] ? row[1] : "NULL") << endl;
							if( Keys.length() )
								Keys += ",";
							Keys += "'" + StringUtils::SQLEscape(row[0]) + "'";
						}
						if( Keys.length() )
						{
							if( g_GlobalConfig.m_bNoPrompts )
							{
								cerr << "Database integrity check failed" << endl;
								throw "Database integrity check failed";
							}
							else if( AskYNQuestion( (g_GlobalConfig.m_iVerifyRestrictID ? "Update resriction on " : "Delete ") + string("the offending records?  (BE CAREFUL!)"),false) )
							{
								sql.str("");
								if( !g_GlobalConfig.m_iVerifyRestrictID )
									sql << "DELETE FROM `" << Name_get() << "` WHERE `" << pField->Name_get() << "` IN (" << Keys << ")";
								else
									sql << "UPDATE `" << Name_get() << "` SET psc_restrict=" << g_GlobalConfig.m_iVerifyRestrictID << " WHERE `" << pField->Name_get() << "` IN (" << Keys << ")";

								m_pDatabase->threaded_mysql_query(sql.str());

							}
						}
					}
					else
					{
						cerr << "In table: " << m_sName << " query: " << sql.str() << " failed." << endl;
						throw "Database error";
					}
				}
				for( list< pair<string,Field *> >::iterator it=pField->m_listField_IReferTo_Indirectly.begin();it!=pField->m_listField_IReferTo_Indirectly.end();++it )
				{
					string sName = it->first;
					Field *pField_IReferTo_Indirectly = it->second;
					Table *pTable = m_pDatabase->GetTableFromForeignKey( pField );
					if(	pField_IReferTo_Indirectly->m_pTable->m_pRepository==NULL )
						continue;  // The foreign field isn't under our control anyway
					
					sql.str("");
					sql << "SELECT T1.FK_" << pField->Name_get().substr(3) << ",T1." << pField->Name_get() << " FROM `" << m_sName << "` AS T1 " <<
						"LEFT JOIN " << pField->Name_get().substr(3) << " As T2 ON T1.FK_" << pField->Name_get().substr(3) << " = T2.PK_" << pField->Name_get().substr(3) <<
						" LEFT JOIN " << pField_IReferTo_Indirectly->m_pTable->Name_get() << " As T3 ON T1." << pField->Name_get() << "=T3." << pField_IReferTo_Indirectly->Name_get() <<
						" AND " << sRestrictions_T3 <<
						" WHERE T2.Description = \"" << sName << "\" AND T3." << pField_IReferTo_Indirectly->Name_get() <<
						" IS NULL AND T1." << pField->Name_get() << " IS NOT NULL AND T1." << pField->Name_get() << "<>'' " <<
						" AND " << sRestrictions_T1;

					if( ( result_set3.r=m_pDatabase->mysql_query_result( sql.str( ) ) ) )
					{
						string Keys="";
						while( (row = mysql_fetch_row(result_set3.r)) )
						{
							if( !row[1] )
								continue;
							string sValue = row[1];
							if( StringUtils::itos( atoi(row[1]) )!=StringUtils::TrimSpaces( sValue ) )
								continue; // It's not numeric
							cout << "In table: " << m_sName << ":" << pField->Name_get() << ": ";
							cout << (row[0] ? row[0] : "NULL") << "-" << row[1];
							cout << " " << pField_IReferTo_Indirectly->Table_get()->Name_get() << ":" << pField_IReferTo_Indirectly->Name_get() << endl;
							if( Keys.length() )
								Keys += " OR ";
							Keys += "(FK_" + pField->Name_get().substr(3) + "='" + StringUtils::SQLEscape(row[0]) + "' AND " + pField->Name_get() +
								"='" + StringUtils::SQLEscape(row[1]) + "')" ;
						}
						if( Keys.length() )
						{
							if( g_GlobalConfig.m_bNoPrompts )
							{
								cerr << "Database integrity check failed" << endl;
								throw "Database integrity check failed";
							}
							else if( AskYNQuestion((g_GlobalConfig.m_iVerifyRestrictID ? "Update resriction on " : "Delete ") + string("the offending records?  (BE CAREFUL!)"),false) )
							{
								sql.str("");
								if( !g_GlobalConfig.m_iVerifyRestrictID )
									sql << "DELETE FROM `" << m_sName << "` WHERE " << Keys;
								else
									sql << "UPDATE `" << m_sName << "` SET psc_restrict=" << g_GlobalConfig.m_iVerifyRestrictID << " WHERE " << Keys;
								m_pDatabase->threaded_mysql_query(sql.str());
							}
						}
					}
					else
					{
						cerr << "In table: " << m_sName << " query: " << sql.str() << " failed." << endl;
						throw "Database error";
					}
				}
			}
		}
	}
}

int Table::ReassignAutoIncrValue(int iPrimaryKey)
{
	std::ostringstream sSQL2;
	sSQL2 << "SELECT max(" << m_pField_AutoIncrement->Name_get() << ") FROM `" << m_sName << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL2.str( ) ) ) && (row = mysql_fetch_row(result_set.r) ) )
	{
		int iNextID = atoi(row[0])+1;
		cout << "ReassignAutoIncrValue for table: " << m_sName << " determined highest used value is: " << row[0] << " m_psc_id_next: " << m_psc_id_next << endl;
		if( m_psc_id_next>iNextID )
			iNextID = m_psc_id_next;

		m_psc_id_next=iNextID+1;

		PropagateUpdatedField( m_pField_AutoIncrement, StringUtils::itos(iNextID), StringUtils::itos(iPrimaryKey), NULL );
		sSQL2.str("");
		sSQL2 << "UPDATE `" << m_sName << "` SET `" << m_pField_AutoIncrement->Name_get() << "`=" << iNextID <<
			" WHERE `" << m_pField_AutoIncrement->Name_get() << "`=" << iPrimaryKey;
		if( m_pDatabase->threaded_mysql_query( sSQL2.str( ) )<0 )
		{
			cerr << "SQL Failed: " << sSQL2.str( ) << endl;
			throw "Database error";
		}

		return iNextID;
	}
	cerr << "ReassignAutoIncrValue cannot get new value";
	throw "Database error";
}

bool Table::psc_id_exists(int psc_id)
{
	std::ostringstream sSQL2;
	sSQL2 << "SELECT psc_id FROM `" << m_sName << "` WHERE psc_id=" << psc_id;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL2.str( ) ) ) && (row = mysql_fetch_row(result_set.r) ) )
		return true;
	else
		return false;
}

bool Table::ModifiedRow(int psc_id)
{
	std::ostringstream sSQL2;
	sSQL2 << "SELECT psc_mod FROM `" << m_sName << "` WHERE psc_id=" << psc_id;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL2.str( ) ) ) && (row = mysql_fetch_row(result_set.r) ) && row[0] )
		return atoi(row[0])!=0;
	else
		return false;
}

bool Table::DoWeHaveBatch( int psc_batch )
{
	std::ostringstream sSQL2;
	sSQL2 << "SELECT * FROM `psc_" << m_pRepository->Name_get() << "_bathdr` WHERE "
		<< "PK_psc_" << m_pRepository->Name_get() << "_bathdr"
		<< "=" << psc_batch;
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL2.str( ) ) ) && (row = mysql_fetch_row(result_set.r) ) && row[0] )
		return true;
	else
		return false;
}

void Table::ValidateTable()
{
//<-mkr_b_ubuntu_b->
	// Do any fixups if something has changed in the schema
	if( mysql_get_server_version(m_pDatabase->m_pMySQL)>50000 ) // In Mysql 5 they require an ON UPDATE CURRENT_TIMESTAMP for the psc_mod
	{
		std::ostringstream sSQL;
		sSQL << "show create table `" << m_sName << "`";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( ( result_set.r=m_pDatabase->mysql_query_result( sSQL.str( ) ) ) && (row = mysql_fetch_row(result_set.r) ) && row[0] && row[1] )
		{
			vector<string> vectString;
			string s = row[1];
			StringUtils::Tokenize(s,"\n",vectString);
			for(vector<string>::iterator it=vectString.begin();it!=vectString.end();++it)
			{
				if( it->find("`psc_mod`")!=string::npos && it->find("ON UPDATE CURRENT_TIMESTAMP")==string::npos )
				{
					std::ostringstream sSQL;
					sSQL << "ALTER TABLE `" << m_sName << "` change `psc_mod` `psc_mod` timestamp NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP";
					m_pDatabase->threaded_mysql_query( sSQL.str() );
					break;
				}
			}
		}
	}
//<-mkr_b_ubuntu_e->
}

