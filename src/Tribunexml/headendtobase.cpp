/*
	Tribunexml
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include <vector>
#include <fstream>
#include <time.h>
#include <iostream>
#include <string.h>
#include "DCE/Logger.h"
#include "PlutoUtils/StringUtils.h"

extern "C"
{
#include <mysql/mysql.h>
}


using namespace DCE;
using namespace std;
string URL="http://192.168.125.1/tribune/";

MYSQL *mysql=NULL;

int getURL(const string filename){
	string command=(string) "wget -O - \""+URL+filename+".tar.bz2\"|tar xjf - -C /tmp";
	cout<<command<<endl;
	return system(command.c_str());
}

int fillInsert(ifstream &f, string &result,short table=0) {
	if(f.eof())
		return false;
	string str="",buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	f.getline(tmp, sizeof(tmp));
	int field=0;
	if(*tmp==0)
		return false;
	buf=(string)tmp+"|";
	int num=buf.find("|");
	result+="('";
	while(num>=0){
		if ((1==table)&&(field>11))
			break;
		str=buf.substr(0,num);
		result+=StringUtils::SQLEscape(str)+"','";
		str="";
		if((unsigned)num+1<=buf.size())
			buf.erase(0,num+1);
		num=buf.find("|");
		field++;
	}
	result.erase(result.size()-2,2);
	result+=")";
	return true;
}

bool sendQuery(const string &query){
	if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
		cerr<<"query: "<<query<<endl
				<<"Error executing query: "<<mysql_error(mysql)<<endl;
		return true;
	}
	return false;
}

void createATempTable(const string &ptablename, const string &ptablestruct){
	string query = (string)"CREATE TEMPORARY TABLE IF NOT EXISTS "+ptablename+" "+ptablestruct;
	if( sendQuery(query)){
		return ;
	}
	query = (string)"TRUNCATE TABLE " + ptablename + ";";
	sendQuery(query);
}

void createTempTables(){
	string table;
	table = "( headend_id char(7),community_name varchar(28),county_name varchar(25),county_size char(1),st_county_code char(5),state_served char(2),zip_code char(5),dma_code int(3),dma_name varchar(70),mso_code char(5),dma_rank char(4),headend_name varchar(42),headend_location varchar(28),mso_name varchar(42),time_zone_code int(1),stationid char(12), callsign char(10), stationname varchar(40) )";
	createATempTable("tmp_Headends", table);

	table = "( PK_Stations int(12),station_time_zone varchar(30),station_name varchar(40),station_call_sign varchar(10),station_affil varchar(25),station_city varchar(20),station_state varchar(15),station_zip_code varchar(12),station_country varchar(15),dma_name varchar(70),dma_num int(10),fcc_channel_num int(8) )";
	createATempTable("tmp_Stations", table);
}

int main(int argc, char *argv[]) {
	string host="dcerouter",user="root",passwd="",db="pluto_myth";
	bool bError=false;
	char c;
	for(int i=1;i<argc;i++){
		if(argv[i][0]!='-'){
			cerr<<"Unknown option"<<endl;
			bError=true;
			break;
		}
		c=argv[i][1];
		switch(c){
			case 'h':host=argv[++i];break;
			case 'u':user=argv[++i];break;
			case 'p':passwd=argv[++i];break;
			case 'd':db=argv[++i];break;
			case 'U':URL=argv[++i];break;
			default:cerr<<"Unknown option"<<endl;
			bError=true;
		}
	}
	if(bError){
		cout << "Usage: "<< argv[0]
				<< " [-h MySql hostname] [-u MySql username] " << endl
				<< "[-p MySql password] [-d MySql database] " << endl
				<< "[-U URL to download headend and lineup]" << endl;
		return true;
	}
	string query="";
	string head="INSERT INTO tmp_Headends( headend_id, community_name, county_name, county_size, st_county_code, state_served, zip_code, dma_code, dma_name, mso_code, dma_rank, headend_name, headend_location, mso_name, time_zone_code ) VALUES ";
	my_bool reconnect = true;
	mysql=mysql_init(NULL);
	mysql_options(mysql, MYSQL_OPT_RECONNECT, &reconnect);
	if(!mysql_real_connect(mysql,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),0,NULL,0)){
		cout<<"Error connecting to db:"<<mysql_error(mysql)<<endl;
		return true;
	}
	createTempTables();
	if(!getURL("headend.fsf")){
		ifstream f("/tmp/headend.fsf");
		if (!f){
			cerr<<"Error opening file /tmp/headend.fsf"<<endl;
			return true;
		}
		while(fillInsert(f, query)){
			if(query.size()>=12800){
				query=head+query;
				if( sendQuery(query) ){
					return true;
				}
				query="";
			}else
				query+=",";
		}
		if(query.size()>0){
			query.erase(query.size()-1,1);
			query=head+query;
			if( sendQuery(query) ){
				return true;
			}
		}
		query="UPDATE Headends H,tmp_Headends t SET H.county_name=t.county_name, H.county_size=t.county_size, H.st_county_code=t.st_county_code, H.state_served=t.state_served, H.dma_code=t.dma_code, H.dma_name=t.dma_name, H.mso_code=t.mso_code, H.dma_rank=t.dma_rank, H.headend_name=t.headend_name, H.headend_location=t.headend_location, H.mso_name=t.mso_name, H.time_zone_code=t.time_zone_code WHERE H.headend_id=t.headend_id AND H.community_name=t.community_name AND H.zip_code=t.zip_code";
		if( sendQuery(query) ){
			return true;
		}
		query="DELETE FROM tmp_Headends USING tmp_Headends t,Headends H WHERE H.headend_id=t.headend_id AND H.community_name=t.community_name AND H.zip_code=t.zip_code";
		if( sendQuery(query) ){
			return true;
		}
		query="INSERT INTO Headends( headend_id, community_name, county_name, county_size, st_county_code, state_served, zip_code, dma_code, dma_name, mso_code, dma_rank, headend_name, headend_location, mso_name, time_zone_code) SELECT headend_id, community_name, county_name, county_size, st_county_code, state_served, zip_code, dma_code, dma_name, mso_code, dma_rank, headend_name, headend_location, mso_name, time_zone_code from tmp_Headends";
		if( sendQuery(query) ){
			return true;
		}
		query="";
		system("rm /tmp/headend.fsf");
	}
	head="INSERT INTO Lineups( row_id,headend_id, device, FK_Stations, tms_chan, service_tier, effective_date, expiration_date ) VALUES ";
	if(!getURL("lineup.fsf")){
		ifstream f("/tmp/lineup.fsf");
		if (!f){
			cerr<<"Error opening file /tmp/lineup.fsf"<<endl;
			return true;
		}
		query="DELETE FROM Lineups";
		if( sendQuery(query) ){
			return true;
		}
		query="";
		while(fillInsert(f, query)){
			if(query.size()>=12800){
				query=head+query;
				if( sendQuery(query) ){
					return true;
				}
				query="";
			}else
				query+=",";
		}
		if(query.size()>0){
			query.erase(query.size()-1,1);
			query=head+query;
			if( sendQuery(query) ){
				return true;
			}
		}
		query="";
		system("rm /tmp/lineup.fsf");
	}
	head="INSERT INTO tmp_Stations( PK_Stations, station_time_zone, station_name, station_call_sign, station_affil, station_city, station_state, station_zip_code, station_country, dma_name, dma_num, fcc_channel_num ) VALUES ";
	if(!getURL("statrec.txt")){
		ifstream f("/tmp/statrec.txt");
		if (!f){
			cerr<<"Error opening file /tmp/statrec.txt"<<endl;
			return true;
		}
		while(fillInsert(f, query,1)){
			if(query.size()>=12800){
				query=head+query;
				if( sendQuery(query) ){
					return true;
				}
				query="";
			}else
				query+=",";
		}
		if(query.size()>0){
			query.erase(query.size()-1,1);
			query=head+query;
			if( sendQuery(query) ){
				return true;
			}
		}
		query="UPDATE Stations S,tmp_Stations t SET S.station_time_zone=t.station_time_zone, S.station_name=t.station_name, S.station_call_sign=t.station_call_sign, S.station_affil=t.station_affil, S.station_city=t.station_city, S.station_state=t.station_state, S.station_zip_code=t.station_zip_code, S.station_country=t.station_country, S.dma_name=t.dma_name, S.dma_num=t.dma_num, S.fcc_channel_num=t.fcc_channel_num WHERE S.PK_Stations=t.PK_Stations";
		if( sendQuery(query) ){
			return true;
		}
		query="DELETE FROM tmp_Stations USING tmp_Stations t,Stations S WHERE S.PK_Stations=t.PK_Stations";
		if( sendQuery(query) ){
			return true;
		}
		query="INSERT INTO Stations( PK_Stations, station_time_zone, station_name, station_call_sign, station_affil, station_city, station_state, station_zip_code, station_country, dma_name, dma_num, fcc_channel_num ) SELECT PK_Stations, station_time_zone, station_name, station_call_sign, station_affil, station_city, station_state, station_zip_code, station_country, dma_name, dma_num, fcc_channel_num from tmp_Stations";
		if( sendQuery(query) ){
			return true;
		}
		system("rm /tmp/statrec.txt");
	}
	return false;
}
