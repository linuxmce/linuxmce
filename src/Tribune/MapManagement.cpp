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
#include "MapManagement.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "CommonFunctions.h"
#include "Database.h"

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <string>

using namespace Tribune;

bool MapManagement::GetProgramRecordMap(map<string,string> & mapPrimaryKey_Timestam_ProgramRecord){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_ProgramRecord, TimestampF FROM ProgramRecord";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_ProgramRecord[ row[0] ] = row[1];
		}
	}

	return true;
}

bool MapManagement::GetStationMap(map<int,string> & mapPrimaryKey_Timestam_Station){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Station, TimestampF FROM Station";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
			}
			mapPrimaryKey_Timestam_Station[ atoi(row[0]) ] = row[1];
		}
	}

	return true;
}

bool MapManagement::GetScheduleMap(map<u_int64_t,string> & mapPrimaryKey_Timestam_Schedule){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Schedule, TimestampF FROM Schedule";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Schedule[ atoll(row[0]) ] = row[1];
		}
	}

	return true;
}

bool MapManagement::GetActorMap(map<string,string> & mapPrimaryKey_Timestam_Actor){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Actor FROM Actor";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Actor[ row[0] ] = "";
		}
	}

	return true;
}

bool MapManagement::GetGenreMap(map<string,string> & mapPrimaryKey_Timestam_Genre){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Genre FROM Genre";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Genre[ row[0] ] = "";
		}
	}

	return true;
}

bool MapManagement::GetRoleMap(map<string,string> & mapPrimaryKey_Timestam_Role){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_Role FROM Role";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_Role[ row[0] ] = "";
		}
	}

	return true;
}

bool MapManagement::GetActorRoleMap(map<string,string> & mapPrimaryKey_Timestam_ActorRole){

	std::ostringstream sSQL;
	
	sSQL << "SELECT PK_ActorRole FROM Actor_Role";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				return false;
				
			}
			mapPrimaryKey_Timestam_ActorRole[ row[0] ] = "";
		}
	}

	return true;
}
