/*
 sqlCVSprocessor.cpp
 
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
 * @file sqlCVSprocessor.cpp
 * @brief source file for the sqlCVSProcessor class
 *
 */
 
#include "PlutoUtils/CommonIncludes.h"
#include "sqlCVSprocessor.h"
#include "Repository.h"
#include "Database.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

sqlCVSprocessor *g_psqlCVSprocessor = NULL;  // There can only be 1 running at a time

int sqlCVSprocessor::UnauthorizedBatch(int psc_user_needs_to_authorize)
{
	// See if we started an unauthorized batch and it's still the same user.  If so, we'll just re-use it
	if( m_psc_bathdr_unauth && psc_user_needs_to_authorize==m_psc_user_needs_to_authorize )
		return m_psc_bathdr_unauth;

	RecordChangesToTable();

	std::ostringstream sSQL;

	sSQL << "SELECT min(PK_psc_" << m_pRepository->Name_get() << "_bathdr) FROM psc_" << m_pRepository->Name_get() << "_bathdr";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;

	// The new batch is going to be a negative number, find the lowest used number, or -1, whichever is less
	m_psc_bathdr_unauth=-1;
	if( (result_set.r=m_pRepository->m_pDatabase->mysql_query_result(sSQL.str())) && (row = mysql_fetch_row( result_set.r ) ) )
		m_psc_bathdr_unauth = min(m_psc_bathdr_unauth,atoi(row[0])-1);

	sSQL.str("");
	sSQL << "INSERT INTO `psc_" << m_pRepository->Name_get( ) << "_bathdr` (PK_psc_" << m_pRepository->Name_get() << "_bathdr, IPAddress, date, comments ) VALUES ( "
		<< m_psc_bathdr_unauth << ",'" << (m_pSocket ? m_pSocket->m_sIPAddress : "") << "', NOW(), '" << StringUtils::SQLEscape(m_sComments) << "' )";
	if( m_pRepository->m_pDatabase->threaded_mysql_query( sSQL.str( ) )<0 )
	{
		cerr << "Cannot create unauthorized batch" << endl;
		throw "Database error";
	}

	m_psc_user_needs_to_authorize = psc_user_needs_to_authorize;
	return m_psc_bathdr_unauth;
}

void sqlCVSprocessor::RecordChangesToTable()
{
	if( !m_pTable || !m_pRepository || (m_iNew==0 && m_iDel==0 && m_iMod==0) )
		return; // Nothing to do

	ostringstream sSql;
	sSql << "INSERT INTO `psc_" << m_pRepository->Name_get() << "_batdet` ("
		<< "FK_psc_" << m_pRepository->Name_get() << "_bathdr,"
		<< "Tablename,New,Deleted,Modified,"
		<< "FK_psc_" << m_pRepository->Name_get() << "_bathdr_orig,"
		<< "FK_psc_" << m_pRepository->Name_get() << "_bathdr_auth,"
		<< "FK_psc_" << m_pRepository->Name_get() << "_bathdr_unauth"
		<< ") VALUES ("
		<< m_i_psc_batch << ","
		<< "'" << m_pTable->Name_get() << "'," << m_iNew << "," << m_iDel << "," << m_iMod << ","
		<< m_psc_bathdr_orig << ","
		<< m_psc_bathdr_auth << ","
		<< m_psc_bathdr_unauth 
		<< ")";

	if( m_pRepository->m_pDatabase->threaded_mysql_query( sSql.str() )<0 )
	{
		cerr << "Error recording changes to batdet" << endl;
		throw "Error writing to database";
	}
	m_iNew=m_iMod=m_iDel=0;
}


