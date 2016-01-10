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
#include "PlutoUtils/CommonIncludes.h"
#include "ClientFunctions.h"
#include "Database.h"
#include "CommonFunctions.h"
#include "R_GetLineups.h"
#include "R_GetChannels.h"
#include "R_GetDiffs.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <map>
#include <string>

bool ClientFunctions::GetLineups(int zipcode, map <int,pair<string,string> > & mapIndexLineupKey_To_Name){

	R_GetLineups r_GetLineups( "Headend", zipcode);
	ra_Processor->AddRequest( &r_GetLineups );
							
	while( ra_Processor->SendRequests(Connection, m_pSocket) );

	if( r_GetLineups.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetLineups.m_cProcessOutcome << endl;
	}

	map<string,string>::iterator iter;
	int i = 0;
	//map <int,pair<string,string> > mapIndexLineupKey_To_Name;
	if (r_GetLineups.m_mapPrimaryKey_LineupName.empty()){
		return false;
	}
	for(iter = r_GetLineups.m_mapPrimaryKey_LineupName.begin(); iter != r_GetLineups.m_mapPrimaryKey_LineupName.end(); iter++ ) {
    		//cout << "Id-ul lineupului"<<iter->first << " numele lineupului " << iter->second << endl;
		mapIndexLineupKey_To_Name[i] = make_pair(iter->first,iter->second);
		i++;
  	}

	return true;
}

bool ClientFunctions::GetChannels(int key, string extra_cond, map <string,string> & mapChannelKey_To_Name)
{
	R_GetChannels r_GetChannels( key, extra_cond );

	ra_Processor->AddRequest( &r_GetChannels );
							
	while( ra_Processor->SendRequests(Connection, m_pSocket) );

	if( r_GetChannels.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetChannels.m_cProcessOutcome << endl;
		return false;
	}

	mapChannelKey_To_Name = r_GetChannels.m_mapPrimaryKey_ChannelName;

	return true;
}

string ClientFunctions::GetBlackListChannels() {

	std::ostringstream sSQL;
	string blacklist;
	
	sSQL << "SELECT ChannelName FROM BlackListChannels";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return blacklist;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
			}
			if (blacklist.length( )==0){
				blacklist = "'"+(string)row[0]+"'";
			} else {
				blacklist = blacklist+","+"'"+(string)row[0]+"'";
			}
		}
	}

	return blacklist;
}

void ClientFunctions::DeleteClientLineup() {

	std::ostringstream sSQL;
	
	sSQL << "DELETE FROM ClientLineup";

	g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );

}

void ClientFunctions::SetClientLineup(string key, string lineup) {

	std::ostringstream sSQL;
	
	sSQL << "INSERT INTO ClientLineup(PK_ClientLineup, LineupName) VALUES (\""+key+"\",\""+lineup+"\")";

	g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );

}

string ClientFunctions::GetClientLineup() {

	std::ostringstream sSQL;
	string id;
	
	sSQL << "SELECT PK_ClientLineup FROM ClientLineup";

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		return id;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
			}
			id = (string) row[0];
		}
	}

	return id;
	
}

string ClientFunctions::CalcDiffs(string lineup,string blacklist, map <string,string> &mapProgramRecord, map <int,string> &mapStation, map <u_int64_t,string> &mapSchedule, map <string,string> &mapActor, map <string,string> &mapGenre, map <string,string> &mapRole, map <string,string> &mapActorRole) {

	R_GetDiffs r_GetDiffs(lineup, blacklist, mapProgramRecord, mapStation, mapSchedule, mapActor, mapGenre, mapRole, mapActorRole);

	ra_Processor->AddRequest( &r_GetDiffs );
							
	while( ra_Processor->SendRequests(Connection, m_pSocket) );

	if( r_GetDiffs.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Cannot get list of records from server (clarifying local deletions destiny):" << (int) r_GetDiffs.m_cProcessOutcome << endl;
		return r_GetDiffs.m_clientfile;
	}
	
	update_list = r_GetDiffs.m_updatelist;
	delete_list = r_GetDiffs.m_deletelist;

	return r_GetDiffs.m_clientfile;
}

bool ClientFunctions::ModifyClientDatabase(string path){
	
	ifstream mysqlfile(path.c_str());
        string line;
	
	if (!mysqlfile) {
		cerr<<"Error opening file "<<path<<endl;
		return false;
	}

	if (mysqlfile.is_open()) {
		
		while(! mysqlfile.eof())
		{
			getline (mysqlfile,line);

			if (line.length()!=0){
				cout << line << endl;
				g_GlobalConfig.m_pDatabase->db_wrapper_query_result( line );
			}
			
			
		}
	}
		
	return true;
}

string ClientFunctions::getUpdateList(){
	return this->update_list;
}


string ClientFunctions::getDeleteList(){
	return this->delete_list;
}






