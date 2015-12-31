/*
 R_GetHashedTableStats

 Copyright (C) 2006 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */


/**
 *
 * @file R_GetHashedTableStats.cpp
 * @brief source file for the R_GetHashedTableStats class
 *
 */

#include "R_GetHashedTableStats.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "sqlCVSprocessor.h"
#include "Database.h"
#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

R_GetHashedTableStats::R_GetHashedTableStats(string sTable, vector<int> *p_vectRestrictions, int range_begin, int range_end, int interval_length)
{
	m_sTable=sTable;
	m_vectRestrictions=*p_vectRestrictions;
	m_iRange_Begin = range_begin;
	m_iRange_End = range_end;
	m_iInterval_Length = interval_length;
}

bool R_GetHashedTableStats::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "R_GetHashedTableStats" << endl;
	std::ostringstream sSQL;
	// See notes in Table::DetermineDeletions

	// BIT_XOR and DIV are not implemented for MySQL 4.0.x
/*
	sSQL << "SELECT (psc_id-" << m_iRange_Begin << ") DIV " << m_iInterval_Length << " , count(*), BIT_XOR(POW(psc_id-"<<m_iRange_Begin<<", FLOOR(63/CEIL(LOG2("<<(1+m_iRange_End-m_iRange_Begin)<<"))))) FROM " << m_sTable << " WHERE (psc_id IS NOT NULL AND psc_id>0) AND (psc_id BETWEEN " << m_iRange_Begin << " AND " << m_iRange_End << ") AND " << g_GlobalConfig.GetRestrictionClause(m_sTable,&m_vectRestrictions) << " GROUP BY (psc_id-"<< m_iRange_Begin<< ") DIV " << m_iInterval_Length << " ORDER BY (psc_id-"<< m_iRange_Begin<<") DIV " << m_iInterval_Length;
*/

	sSQL << "SELECT FLOOR((psc_id-" << m_iRange_Begin << ") / " << m_iInterval_Length << ") , count(*), MD5(CAST(SUM(POW(1+(psc_id-"<<m_iRange_Begin<<")%"<<m_iInterval_Length<<", 3)) AS UNSIGNED)) FROM `" << m_sTable << "` WHERE (psc_id IS NOT NULL AND psc_id>0) AND (psc_id BETWEEN " << m_iRange_Begin << " AND " << m_iRange_End << ") AND " << g_GlobalConfig.GetRestrictionClause(m_sTable,&m_vectRestrictions) << " GROUP BY FLOOR( (psc_id-"<< m_iRange_Begin<< ") / " << m_iInterval_Length << ") ORDER BY FLOOR( (psc_id-"<< m_iRange_Begin<<") / " << m_iInterval_Length << ")";

	//cout << "Query: " << sSQL.str() << endl;

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return true; /**<< Request successfully processed */
			}

			m_hashedCount.push_back(pair<int, int> (atoi(row[0]), atoi(row[1])) );
			m_hashedCheckSum.push_back(row[2] );
			//m_hashedCheckSum.push_back(pair<int, int> (atoi(row[0]), atoi(row[2])) );
		}
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}

	return true; /**<< Request successfully processed */
}
