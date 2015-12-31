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
#include "R_GetDiffs.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Tribuneprocessor.h"
#include "Database.h"
#include "R_GetChannels.h"
//#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Tribune;
using namespace std;

R_GetDiffs::R_GetDiffs(string lineup, string blacklist, map <string,string> mapProgramRecord, map <int,string> mapStation, map <u_int64_t,string> mapSchedule, map <string,string> mapActor, map <string,string> mapGenre, map <string,string> mapRole, map <string,string> mapActorRole)
{
	m_lineup = lineup;
	m_blacklist = blacklist;
	m_mapProgramRecord = mapProgramRecord;
        m_mapStation = mapStation;
	m_mapSchedule = mapSchedule;
	m_mapActor = mapActor;
	m_mapGenre = mapGenre;
	m_mapRole = mapRole;
	m_mapActorRole = mapActorRole;
}

bool R_GetDiffs::UpdateProgramRecord(string pk_programrecord, bool conversion){

	std::ostringstream sSQL;
	sSQL << "SELECT * FROM ProgramRecord WHERE PK_ProgramRecord='" << pk_programrecord <<"'";
	string timestamp;
		
	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return false;
	}
	else
	{
		row = db_wrapper_fetch_row( res.r );

		if( !row[0] )
		{
			cerr << "Found NULL in query: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
			return false; /**<< Request successfully processed */
		}

		if (conversion){
			timestamp = ChangedLongFormatTime(row[42]);
		} else{
			timestamp = row[42];
		}
		
		clientfile<<"UPDATE ProgramRecord SET Title=\""<<row[1]<<"\", ReducedTitle1=\""<<row[2]<<"\", ReducedTitle2=\""<<row[3]<<"\", ReducedTitle3=\""<<row[4]<<"\", ReducedTitle4=\""<<row[5]<<"\", AltTitle=\""<<row[6]<<"\", ReducedDesc1=\""<<row[7]<<"\", ReducedDesc2=\""<<row[8]<<"\", ReducedDesc3=\""<<row[9]<<"\", AdvisoryAdultContent=\""<<row[10]<<"\", AdvisoryExplicitLanguage=\""<<row[11]<<"\", AdvisoryNudity=\""<<row[12]<<"\", AdvisoryViolence=\""<<row[13]<<"\", AdvisorySexualContent=\""<<row[14]<<"\", AdvisoryRapeContent=\""<<row[15]<<"\", Description=\""<<row[16]<<"\", RealYear=\""<<row[17]<<"\", MpaaRating=\""<<row[18]<<"\", StarRating=\""<<row[19]<<"\", RunTime=\""<<row[20]<<"\", ColorCode=\""<<row[21]<<"\", ProgramLanguage=\""<<row[22]<<"\", OrgCountry=\""<<row[23]<<"\", MadeForTV=\""<<row[24]<<"\", SourceType=\""<<row[25]<<"\", ShowType=\""<<row[26]<<"\", Holiday=\""<<row[27]<<"\", SynEpiNum=\""<<row[28]<<"\", AltSynEpiNum=\""<<row[29]<<"\", EpiTitle=\""<<row[30]<<"\", NetSynSource=\""<<row[31]<<"\", NetSynType=\""<<row[32]<<"\", DescWithActors=\""<<row[33]<<"\", ReducedDescWithActors=\""<<row[34]<<"\", OrgStudio=\""<<row[35]<<"\", Gamedate=\""<<row[36]<<"\", GameTime=\""<<row[37]<<"\", GameTimeZone=\""<<row[38]<<"\", OrgAirDate=\""<<row[39]<<"\", UniqueID=\""<<row[40]<<"\", Checksum=\""<<row[41]<<"\", TimestampF=\""<<timestamp<<"\" WHERE PK_ProgramRecord=\""<<pk_programrecord<<"\"\n";
		
		updatelist<<pk_programrecord<<"\n";

		//m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}
	return true;
}

bool R_GetDiffs::UpdateProgramRecord_Genre(string pk_programrecord){
	
	std::ostringstream sSQL;
	sSQL << "SELECT FK_Genre FROM ProgramRecord_Genre WHERE FK_ProgramRecord='" << pk_programrecord<<"'";
		
	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );

	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			std::ostringstream sSQL1;

			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			clientfile<<"UPDATE ProgramRecord_Genre SET FK_Genre=\""<<row[0]<<"\" WHERE FK_ProgramRecord=\""<<pk_programrecord<<"\"\n";

			
		}
	}
	return true;
}

bool R_GetDiffs::UpdateActor_ProgramRecord(string pk_programrecord){

	std::ostringstream sSQL;
	sSQL << "SELECT FK_Actor FROM Actor_ProgramRecord WHERE FK_ProgramRecord='" << pk_programrecord<<"'";
	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );

	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
		return false;
	}
	else
	{
		while( ( row = db_wrapper_fetch_row( res.r ) ) )
		{
			std::ostringstream sSQL1;

			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			clientfile<<"UPDATE Actor_ProgramRecord SET FK_Actor=\""<<row[0]<<"\" WHERE FK_ProgramRecord=\""<<pk_programrecord<<"\"\n";


			}
			
	}
	
	return true;
}

bool R_GetDiffs::InsertSchedule(string pk_station){
	
	std::ostringstream sSQL;
	sSQL << "SELECT * from Schedule where FK_Station='"<<pk_station<<"'";
	PlutoSqlResult res1;
	DB_ROW row1=NULL;
	res1.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res1.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while ( ( row1 = db_wrapper_fetch_row( res1.r ) ) ){

			if( !row1[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
		
			map<u_int64_t,string>::iterator it=m_mapSchedule.find(atoll(row1[0]));

			if( it == m_mapSchedule.end() ){
					
				cout << "Insert into schedule" << row1[0] << endl;

				clientfile<<"INSERT INTO Schedule(PK_Schedule, FK_Station, FK_ProgramRecord, AirDate, AirTime, Duration, PartNum, NumOfParts, CCaptioning, Stereo, Repeating, LiveTapeDelay, Subtitled, PremiereFinale, JoinedInProgress, CableInTheClassroom, TVRating, Sap, Blackout, SexRating, ViolenceRating, LanguageRating, DialogRating, FvRating, Enhanced, ThreeD, LetterBox, Hdtv, Dolby, Dvs, Checksum, TimestampF) VALUES (\""<<row1[0]<<"\",\""<<row1[1]<<"\",\""<<row1[2]<<"\",\""<<row1[3]<<"\",\""<<row1[4]<<"\",\""<<row1[5]<<"\",\""<<row1[6]<<"\",\""<<row1[7]<<"\",\""<<row1[8]<<"\",\""<<row1[9]<<"\",\""<<row1[10]<<"\",\""<<row1[11]<<"\",\""<<row1[12]<<"\",\""<<row1[13]<<"\",\""<<row1[14]<<"\",\""<<row1[15]<<"\",\""<<row1[16]<<"\",\""<<row1[17]<<"\",\""<<row1[18]<<"\",\""<<row1[19]<<"\",\""<<row1[20]<<"\",\""<<row1[21]<<"\",\""<<row1[22]<<"\",\""<<row1[23]<<"\",\""<<row1[24]<<"\",\""<<row1[25]<<"\",\""<<row1[26]<<"\",\""<<row1[27]<<"\",\""<<row1[28]<<"\",\""<<row1[29]<<"\",\""<<row1[30]<<"\",\""<<row1[31]<<"\") \n";
			}
			
		}
	}
	return true;
}

bool R_GetDiffs::InsertProgramRecord_Genre(string pk_programrecord){
	
	std::ostringstream sSQL;
	
	sSQL << "SELECT FK_Genre FROM ProgramRecord_Genre WHERE FK_ProgramRecord='" << pk_programrecord<<"'";

	PlutoSqlResult res3;
	DB_ROW row3=NULL;
	res3.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );

	if( !res3.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while( ( row3 = db_wrapper_fetch_row( res3.r ) ) )
		{
			clientfile<<"INSERT INTO ProgramRecord_Genre(FK_ProgramRecord, FK_Genre) VALUES(\""<<pk_programrecord<<"\",\""<<row3[0]<<"\") \n";

			
		}
	}
	return true;
}

bool R_GetDiffs::InsertActor_ProgramRecord(string pk_programrecord){

	std::ostringstream sSQL1;
	
	sSQL1 << "SELECT FK_Actor FROM Actor_ProgramRecord WHERE FK_ProgramRecord='" << pk_programrecord<<"'";
	PlutoSqlResult res3;
	DB_ROW row3=NULL;
	res3.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL1.str( ) );

	if( !res3.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL1.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while( ( row3 = db_wrapper_fetch_row( res3.r ) ) )
		{

			clientfile<<"INSERT INTO Actor_ProgramRecord(FK_Actor, FK_ProgramRecord) VALUES(\""<<row3[0]<<"\",\""<<pk_programrecord<<"\") \n";


			
		}
	}
	return true;
}

bool R_GetDiffs::InsertProgramRecord(string pk_station){

	std::ostringstream sSQL;
	sSQL<<"SELECT ProgramRecord.* FROM Schedule JOIN ProgramRecord ON ProgramRecord.PK_ProgramRecord = Schedule.FK_ProgramRecord WHERE FK_Station = "<<pk_station;
	PlutoSqlResult res1;
	DB_ROW row1=NULL;
	res1.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	if( !res1.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while ( ( row1 = db_wrapper_fetch_row( res1.r ) ) ){

			if( !row1[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return false; /**<< Request successfully processed */
			}
			
			map<string,string>::iterator it=m_mapProgramRecord.find(row1[0]);

			if( it == m_mapProgramRecord.end() ){

				cout << "Insert into ProgramRecord " << row1[0] << endl;

				clientfile<<"INSERT INTO ProgramRecord(PK_ProgramRecord, Title, ReducedTitle1, ReducedTitle2, ReducedTitle3, ReducedTitle4, AltTitle, ReducedDesc1, ReducedDesc2, ReducedDesc3, AdvisoryAdultContent, AdvisoryExplicitLanguage, AdvisoryNudity, AdvisoryViolence, AdvisorySexualContent, AdvisoryRapeContent, Description, RealYear, MpaaRating, StarRating, RunTime, ColorCode, ProgramLanguage, OrgCountry, MadeForTV, SourceType, ShowType, Holiday, SynEpiNum, AltSynEpiNum, EpiTitle, NetSynSource, NetSynType, DescWithActors, ReducedDescWithActors, OrgStudio, Gamedate, GameTime, GameTimeZone, OrgAirDate, UniqueID, Checksum, TimestampF) VALUES (\""<<row1[0]<<"\",\""<<row1[1]<<"\",\""<<row1[2]<<"\",\""<<row1[3]<<"\",\""<<row1[4]<<"\",\""<<row1[5]<<"\",\""<<row1[6]<<"\",\""<<row1[7]<<"\",\""<<row1[8]<<"\",\""<<row1[9]<<"\",\""<<row1[10]<<"\",\""<<row1[11]<<"\",\""<<row1[12]<<"\",\""<<row1[13]<<"\",\""<<row1[14]<<"\",\""<<row1[15]<<"\",\""<<row1[16]<<"\",\""<<row1[17]<<"\",\""<<row1[18]<<"\",\""<<row1[19]<<"\",\""<<row1[20]<<"\",\""<<row1[21]<<"\",\""<<row1[22]<<"\",\""<<row1[23]<<"\",\""<<row1[24]<<"\",\""<<row1[25]<<"\",\""<<row1[26]<<"\",\""<<row1[27]<<"\",\""<<row1[28]<<"\",\""<<row1[29]<<"\",\""<<row1[30]<<"\",\""<<row1[31]<<"\",\""<<row1[32]<<"\",\""<<row1[33]<<"\",\""<<row1[34]<<"\",\""<<row1[35]<<"\",\""<<row1[36]<<"\",\""<<row1[37]<<"\",\""<<row1[38]<<"\",\""<<row1[39]<<"\",\""<<row1[40]<<"\",\""<<row1[41]<<"\",\""<<row1[42]<<"\") \n";

				if (! InsertProgramRecord_Genre((string)row1[0])){
					return false;
				}
				
				if (! InsertActor_ProgramRecord((string)row1[0])){
					return false;
				}

				m_mapProgramRecord[row1[0]] = row1[43];

			}
			
		}
	}
	return true;
}

string R_GetDiffs::ChangedTime(string time){

	string newtime;
	
	newtime = time.substr(0,4)+time.substr(5,2)+time.substr(8,2)+time.substr(11,2)+time.substr(14,2)+time.substr(17,2);

	return newtime;
}

string R_GetDiffs::ChangedLongFormatTime(string time){
	
	string newtime = time.substr(0,4)+"-"+time.substr(4,2)+"-"+time.substr(6,2)+" "+time.substr(8,2)+":"+time.substr(10,2)+":"+time.substr(12,2);

	return newtime;
}

bool R_GetDiffs::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( Tribuneprocessor * ) pRA_Processor)->LogActivityTime();

	bool conversion;
	string convtime;
	cout << "r_GetDiffs"<< endl;
	std::ostringstream sSQL;

	cout << "Pe server ajung: " << endl;
	cout << "Program record size " << m_mapProgramRecord.size()<<endl; 
        cout << "Station size "<< m_mapStation.size()<<endl;
	cout << "Schedule size "<<m_mapSchedule.size()<<endl;
	cout << "Actor size " <<m_mapActor.size()<<endl;
	cout << "Genre sieze " <<m_mapGenre.size()<<endl;
	cout << "Role size " <<m_mapRole.size()<<endl;

	cout << "------------Cele de pe server sunt: " << endl;
	cout << "Program record size " << (g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).size()<<endl; 
        cout << "Station size "<< (g_GlobalConfig.mapPrimaryKey_Timestam_Station).size()<<endl;
	cout << "Schedule size "<<(g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).size()<<endl;
	cout << "Actor size " <<(g_GlobalConfig.mapPrimaryKey_Actor).size()<<endl;
	cout << "Genre sieze " <<(g_GlobalConfig.mapPrimaryKey_Genre).size()<<endl;
	cout << "Role size " <<(g_GlobalConfig.mapPrimaryKey_Role).size()<<endl;
	
	/** update or delete program record table*/
	map<string,string>::iterator iter;
	for( iter = m_mapProgramRecord.begin(); iter != m_mapProgramRecord.end(); iter++ ) {

		conversion = false;

		map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).find(iter->first);

		if ((iter->second).length()>14){
			convtime = ChangedTime(iter->second);
			conversion = true;
		} else {
			convtime = iter->second;
		}

		if( it == (g_GlobalConfig.mapPrimaryKey_Timestam_ProgramRecord).end() ){

			clientfile << "DELETE FROM ProgramRecord WHERE PK_ProgramRecord='"<<iter->first<<"'\n";	
			clientfile << "DELETE FROM ProgramRecord_Genre WHERE FK_ProgramRecord='"<<iter->first<<"'\n";	
			clientfile << "DELETE FROM Actor_ProgramRecord WHERE FK_ProgramRecord='"<<iter->first<<"'\n";	

			deletelist<<iter->first<<"\n";

		} else if ( ( (convtime).compare(it->second) ) != 0 ) { 

			if (! UpdateProgramRecord(it->first,conversion)){
				return true;
			}
		
			if (! UpdateProgramRecord_Genre(it->first)){
				return true;
			}

			if (! UpdateActor_ProgramRecord(it->first)){
				return true;
			}
			
			
		}
	}
	sSQL.str("");
	/** update or delete genre table*/
	if (m_mapGenre.empty()){
		cout << "Tabela genre e goala" << endl;
		//std::ostringstream sSQL;
		sSQL << "SELECT PK_Genre, Description, Level FROM Genre";
		PlutoSqlResult res;
		DB_ROW row=NULL;
		res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
		
		if( !res.r )
		{
			cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
		}
		else
		{
			while ( ( row = db_wrapper_fetch_row( res.r ) ) ){
				clientfile<<"INSERT INTO Genre(PK_Genre, Description, Level) VALUES (\""<<row[0]<<"\",\""<<row[1]<<"\",\""<<row[2]<<"\")\n";
			}
		}
	} else {

		for( iter = m_mapGenre.begin(); iter != m_mapGenre.end(); iter++ ) {
		
			map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Genre).find(iter->first);
		
		
			if( it == (g_GlobalConfig.mapPrimaryKey_Genre).end() ){
		
				clientfile << "DELETE FROM Genre WHERE PK_Genre='"<<iter->first<<"'\n";	
		
			} 
		}
		
		for( iter = (g_GlobalConfig.mapPrimaryKey_Genre).begin(); iter != (g_GlobalConfig.mapPrimaryKey_Genre).end(); iter++ ) {
		
			map<string,string>::iterator it=m_mapGenre.find(iter->first);
		
			sSQL.str("");
			if( it == m_mapGenre.end() ){
				//std::ostringstream sSQL;
				sSQL << "SELECT Description, Level FROM Genre WHERE PK_Genre='" << iter->first<<"'";
				
				PlutoSqlResult res1;
				DB_ROW row1=NULL;
				res1.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
		
				if( !res1.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row1 = db_wrapper_fetch_row( res1.r );
					clientfile<<"INSERT INTO Genre(PK_Genre, Description, Level) VALUES (\""<<iter->first<<"\",\""<<row1[0]<<"\",\""<<row1[1]<<"\")\n";
					m_mapGenre[iter->first]="";
				}
		
			} 
		}
	}
	
	sSQL.str("");
	/** update or delete role table*/
	if (m_mapRole.empty()){
		cout << "Tabela role e goala" << endl;
		//std::ostringstream sSQL;
		sSQL << "SELECT PK_Role, Description FROM Role";
		PlutoSqlResult res;
		DB_ROW row=NULL;
		res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
		
		if( !res.r )
		{
			cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
		}
		else
		{
			while ( ( row = db_wrapper_fetch_row( res.r ) ) ){
				clientfile<<"INSERT INTO Role(PK_Role, Description) VALUES (\""<<row[0]<<"\",\""<<row[1]<<"\")\n";
			}
		}
	} else {
		for( iter = m_mapRole.begin(); iter != m_mapRole.end(); iter++ ) {
	
			map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Role).find(iter->first);
	
	
			if( it == (g_GlobalConfig.mapPrimaryKey_Role).end() ){
				
				cout << "nu-l gasesc pe "<< iter->first <<endl;
				clientfile << "DELETE FROM Role WHERE PK_Role='"<<iter->first<<"'\n";	
	
			} 
		}
	
		for( iter = (g_GlobalConfig.mapPrimaryKey_Role).begin(); iter != (g_GlobalConfig.mapPrimaryKey_Role).end(); iter++ ) {
	
			map<string,string>::iterator it=m_mapRole.find(iter->first);
	
			sSQL.str("");
			if( it == m_mapRole.end() ){
				//std::ostringstream sSQL;
				sSQL << "SELECT Description FROM Role WHERE PK_Role='" << iter->first<<"'";
							
				PlutoSqlResult res;
				DB_ROW row=NULL;
				res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	
				if( !res.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row = db_wrapper_fetch_row( res.r );
					clientfile<<"INSERT INTO Role(PK_Role, Description) VALUES (\""<<iter->first<<"\",\""<<row[0]<<"\") \n";
					m_mapRole[iter->first]="";
	
				}	
	
			} 
		}
	}
	sSQL.str("");
	/** update or delete actor table*/
	if (m_mapActor.empty()){
		cout << "Tabela actor e goala" << endl;
		//std::ostringstream sSQL;
		sSQL << "SELECT PK_Actor, FirstName, LastName FROM Actor";
		PlutoSqlResult res;
		DB_ROW row=NULL;
		res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
		
		if( !res.r )
		{
			cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
		}
		else
		{
			while ( ( row = db_wrapper_fetch_row( res.r ) ) ){
				clientfile<<"INSERT INTO Actor(PK_Actor, FirstName, LastName) VALUES (\""<<row[0]<<"\",\""<<row[1]<<"\",\""<<row[2]<<"\")\n";
			}
		}
	} else {
		for( iter = m_mapActor.begin(); iter != m_mapActor.end(); iter++ ) {
	
			conversion = false;
	
			map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Actor).find(iter->first);
	
	
			if( it == (g_GlobalConfig.mapPrimaryKey_Actor).end() ){
	
				clientfile << "DELETE FROM Actor WHERE PK_Actor=\""<<iter->first<<"\"\n";	
	
			} 
		}
	
		for( iter = (g_GlobalConfig.mapPrimaryKey_Actor).begin(); iter != (g_GlobalConfig.mapPrimaryKey_Actor).end(); iter++ ) {
	
			map<string,string>::iterator it=m_mapActor.find(iter->first);
	
			sSQL.str("");
			if( it == m_mapActor.end() ){
				//std::ostringstream sSQL;
				sSQL << "SELECT FirstName, LastName FROM Actor WHERE PK_Actor=\"" << iter->first<<"\"";
					
				PlutoSqlResult res1;
				DB_ROW row1=NULL;
				res1.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	
				if( !res1.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row1 = db_wrapper_fetch_row( res1.r );
					clientfile<<"INSERT INTO Actor(PK_Actor, FirstName, LastName) VALUES (\""<<iter->first<<"\",\""<<row1[0]<<"\",\""<<row1[1]<<"\")\n";
					m_mapActor[iter->first]="";
				}
	
			} 
		}
	}
	
	sSQL.str("");
	/** update or delete actor_role table*/
	if (m_mapActorRole.empty()){
		sSQL << "SELECT PK_ActorRole, FK_Actor, FK_Role FROM Actor_Role";
		PlutoSqlResult res;
		DB_ROW row=NULL;
		res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
		
		if( !res.r )
		{
			cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
		}
		else
		{
			while ( ( row = db_wrapper_fetch_row( res.r ) ) ){
				clientfile<<"INSERT INTO Actor_Role(PK_ActorRole, FK_Actor, FK_Role) VALUES (\""<<row[0]<<"\",\""<<row[1]<<"\",\""<<row[2]<<"\")\n";
			}
		}
	} else {
		for( iter = m_mapActorRole.begin(); iter != m_mapActorRole.end(); iter++ ) {
	
			map<string,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_ActorRole).find(iter->first);
	
	
			if( it == (g_GlobalConfig.mapPrimaryKey_ActorRole).end() ){
				
				clientfile << "DELETE FROM Actor_Role WHERE PK_ActorRole='"<<iter->first<<"'\n";	
	
			} 
		}
	
		for( iter = (g_GlobalConfig.mapPrimaryKey_ActorRole).begin(); iter != (g_GlobalConfig.mapPrimaryKey_ActorRole).end(); iter++ ) {
	
			map<string,string>::iterator it=m_mapActorRole.find(iter->first);
	
			sSQL.str("");
			if( it == m_mapActorRole.end() ){
				//std::ostringstream sSQL;
				sSQL << "SELECT FK_Actor, FK_Role FROM Actor_Role WHERE PK_ActorRole='" << iter->first<<"'";
							
				PlutoSqlResult res;
				DB_ROW row=NULL;
				res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	
				if( !res.r )
				{
					cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
				}
				else
				{
					row = db_wrapper_fetch_row( res.r );
					clientfile<<"INSERT INTO Actor_Role(PK_ActorRole, FK_Actor, FK_Role) VALUES (\""<<iter->first<<"\",\""<<row[0]<<"\", \""<<row[1]<<"\") \n";
					m_mapActorRole[iter->first]="";
	
				}	
	
			} 
		}
	}
	/** update or delete schedule table*/

	/** this must not be commented after the serialization bug is fixed*/
	map<u_int64_t,string>::iterator iter_uint;
	for( iter_uint = m_mapSchedule.begin(); iter_uint != m_mapSchedule.end(); iter_uint++ ) {

		conversion = false;
		sSQL.str("");
		map<u_int64_t,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).find(iter_uint->first);

		if ((iter_uint->second).length()>14){
			convtime = ChangedTime(iter_uint->second);
			conversion = true;
		} else {
			convtime = iter_uint->second;
		}

		if( it == (g_GlobalConfig.mapPrimaryKey_Timestam_Schedule).end() ){

			clientfile << "DELETE FROM Schedule WHERE PK_Schedule='"<<iter_uint->first<<"'\n";	

		} else if ( ( (iter_uint->second).compare(it->second) ) != 0 ) {
			
			sSQL << "SELECT * FROM Schedule WHERE PK_Schedule=" << it->first;
	
			PlutoSqlResult res;
			DB_ROW row=NULL;
			res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
			if( !res.r )
			{
				cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
			}
			else
			{
				row = db_wrapper_fetch_row( res.r );
				string timestamp;

				if( !row[0] )
				{
					cerr << "Found NULL in query: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
					return true; /**<< Request successfully processed */
				}
				if (conversion){
					timestamp = ChangedLongFormatTime(row[31]);
				} else{
					timestamp = row[31];
				}
				clientfile<<"UPDATE Schedule SET FK_Station=\""<<row[1]<<"\", FK_ProgramRecord=\""<<row[2]<<"\", AirDate=\""<<row[3]<<"\", AirTime=\""<<row[4]<<"\", Duration=\""<<row[5]<<"\", PartNum=\""<<row[6]<<"\", NumOfParts=\""<<row[7]<<"\", CCaptioning=\""<<row[8]<<"\", Stereo=\""<<row[9]<<"\", Repeating=\""<<row[10]<<"\", LiveTapeDelay=\""<<row[11]<<"\", Subtitled=\""<<row[12]<<"\", PremiereFinale=\""<<row[13]<<"\", JoinedInProgress=\""<<row[14]<<"\", CableInTheClassroom=\""<<row[15]<<"\", TVRating=\""<<row[16]<<"\", Sap=\""<<row[17]<<"\", Blackout=\""<<row[18]<<"\", SexRating=\""<<row[19]<<"\", ViolenceRating=\""<<row[20]<<"\", LanguageRating=\""<<row[21]<<"\", DialogRating=\""<<row[22]<<"\", FvRating=\""<<row[23]<<"\", Enhanced=\""<<row[24]<<"\", ThreeD=\""<<row[25]<<"\", LetterBox=\""<<row[26]<<"\", Hdtv=\""<<row[27]<<"\", Dolby=\""<<row[28]<<"\", Dvs=\""<<row[29]<<"\", Checksum=\""<<row[30]<<"\", TimestampF=\""<<timestamp<<"\" WHERE PK_Schedule=\""<<it->first<<"\"\n";

			}

		}
	}

	/** update or delete station table*/
	map<int,string>::iterator iter1;

	for( iter1 = m_mapStation.begin(); iter1 != m_mapStation.end(); iter1++ ) {

		conversion = false;
		sSQL.str("");
		map<int,string>::iterator it=(g_GlobalConfig.mapPrimaryKey_Timestam_Station).find(iter1->first);

		if ((iter1->second).length()>14){
			convtime = ChangedTime(iter1->second);
			conversion = true;
		} else {
			convtime = iter1->second;
		}

		if( it == (g_GlobalConfig.mapPrimaryKey_Timestam_Station).end() ){

			clientfile << "DELETE FROM Station WHERE PK_Station='"<<iter1->first<<"'\n";	

		} else if ( ( (convtime).compare(it->second) ) != 0 ) {
			
			sSQL << "SELECT * FROM Station WHERE PK_Station=" << it->first;
	
			PlutoSqlResult res;
			DB_ROW row=NULL;
			string timestamp;
			res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
			if( !res.r )
			{
				cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
			}
			else
			{
				row = db_wrapper_fetch_row( res.r );

				if( !row[0] )
				{
					cerr << "Found NULL in query: " << sSQL.str() << endl;
					m_cProcessOutcome=INTERNAL_ERROR;
					return true; /**<< Request successfully processed */
				}
				if (conversion){
					timestamp = ChangedLongFormatTime(row[13]);
				} else{
					timestamp = row[13];
				}
				clientfile<<"UPDATE Station SET TimeZone=\""<<row[1]<<"\", Name=\""<<row[2]<<"\", CallSign=\""<<row[3]<<"\", Affil=\""<<row[4]<<"\", City=\""<<row[5]<<"\", State=\""<<row[6]<<"\", ZipCode=\""<<row[7]<<"\", Country=\""<<row[8]<<"\", DmaName=\""<<row[9]<<"\", DmaNum=\""<<row[10]<<"\", FccChannelNum=\""<<row[11]<<"\", Checksum=\""<<row[12]<<"\", TimestampF=\""<<timestamp<<"\" WHERE PK_Station=\""<<it->first<<"\"\n";

			}

		}
	}

	/** insert for station table*/
	int statnr =0;
	sSQL.str("");
	if (m_blacklist.length( )==0){
		sSQL << "SELECT s.* FROM Lineup l LEFT JOIN Headend h ON h.HeadendID=l.HeadendID LEFT JOIN Station s ON s.PK_Station=l.FK_Station WHERE h.PK_Headend=" << m_lineup;
	} else {
		sSQL << "SELECT s.* FROM Lineup l LEFT JOIN Headend h ON h.HeadendID=l.HeadendID LEFT JOIN Station s ON s.PK_Station=l.FK_Station WHERE h.PK_Headend=" << m_lineup << " AND s.Name NOT IN (" << m_blacklist << ")";
	}

	PlutoSqlResult res;
	DB_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->db_wrapper_query_result( sSQL.str( ) );
	
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while ( ( row = db_wrapper_fetch_row( res.r ) ) ){

			statnr++;
			cout << "[ "<< statnr << " ]" <<"----> Inserting station: " << row[2] <<endl;

			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return true; /**<< Request successfully processed */
			}
		
			/** insert for Station*/
			map<int,string>::iterator it=m_mapStation.find(atoi(row[0]));

			if( it == m_mapStation.end() ){
				clientfile<<"INSERT INTO Station(PK_Station, TimeZone, Name, CallSign, Affil, City, State, ZipCode, Country, DmaName, DmaNum, FccChannelNum, Checksum, TimestampF) VALUES (\""<<row[0]<<"\",\""<<row[1]<<"\",\""<<row[2]<<"\",\""<<row[3]<<"\",\""<<row[4]<<"\",\""<<row[5]<<"\",\""<<row[6]<<"\",\""<<row[7]<<"\",\""<<row[8]<<"\",\""<<row[9]<<"\",\""<<row[10]<<"\",\""<<row[11]<<"\",\""<<row[12]<<"\",\""<<row[13]<<"\") \n";

				m_mapStation.insert( make_pair( atoi(row[0]), row[13] ) ); 
			}
			
			/** insert for Schedule for every station*/
			//insert for Schedule
			if (!InsertSchedule((string)row[0])){
				return true;
			}
			
			/** insert for ProgramRecord for every station*/
			if (!InsertProgramRecord((string)row[0])){
				return true;
			}

		}
	}

	m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	
	m_clientfile = clientfile.str();
	m_deletelist = deletelist.str();
	m_updatelist = updatelist.str(); 

	return true; /**<< Request successfully processed */
}
