#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <zlib.h>
#include <sstream>

extern "C"
{
#include <mysql/mysql.h>
}

using namespace std;

MYSQL *mysqltribune;
MYSQL *mysqlmyth;

string CalcStartTime(string airdate, string airtime){
	string starttime;
	starttime = airdate.substr(0,4) + "-" + airdate.substr(4,2) + "-" + airdate.substr(6,2) + " " + airtime.substr(0,2) + ":" + airtime.substr(2,2) + ":" + "00";
	return starttime;
}

string CalcEndTime(string airdate, string airtime, string duration){
	int day, hour, mins, dur, month, year;
	year = atoi((airdate.substr(0,4)).c_str());
	month = atoi((airdate.substr(4,2)).c_str());
	day = atoi((airdate.substr(6,2)).c_str());
	hour = atoi((airtime.substr(0,2)).c_str());
	mins = atoi((airtime.substr(2,2)).c_str());
	dur = atoi(duration.c_str());

	if (dur >= 100){
		hour += (dur / 100);
		mins += (dur - ((dur/100)*100));
	} else {
		mins += dur;
	}

	if (mins >= 60){
		mins -= 60;
		hour++;
	}
	
	if (hour>=24){
		hour = 0;
		day ++;
	}
	switch (month){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
			if (day > 31){
				day=day-31;
				month++;
			}
			break;
		case 12:
			if (day > 31){
			day=day-31;
			month=1;
			year++;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if (day > 30){
				day=day-30;
				month++;
			}
			break;
		case 2:
			if (day > 28){
				day=day-28;
				month++;
			}
			break;
			
	}
	char newhour[12], newdate[25];
	string endtime;
	if (day < 10 && month < 10) {
		sprintf(newdate,"%d-0%d-0%d ",year,month,day);
	} else if (day >= 10 && month < 10){
		sprintf(newdate,"%d-0%d-%d ",year,month,day);
	} else if (day >= 10 && month >= 10){
		sprintf(newdate,"%d-%d-%d ",year,month,day);
	}
	sprintf(newhour,"%d:%d:00",hour, mins);
	endtime = (string)newdate + (string)newhour;

	return endtime;
}

float CalcStars(string stars){
	char* pIn = (char*) stars.c_str();
	float strs = 0;
	
	if( !pIn )
		return 0;

	while( *pIn=='*' && *pIn!=0 ){
		pIn++;
		strs++;
	}

	if( *pIn=='+' ){
            *pIn++=0;
		strs+=0.5;
	}
	
	return strs;
}
string GetCategory(string programid){
	MYSQL_RES *mrestribune;
	MYSQL_ROW rowtribune;
	std::ostringstream sSQL;
	string category="";

	cout << "categ for "<<programid<<endl;

	sSQL <<"select Genre.Description from Genre join ProgramRecord_Genre on ProgramRecord_Genre.FK_Genre=PK_Genre join ProgramRecord on ProgramRecord.PK_ProgramRecord=ProgramRecord_Genre.FK_ProgramRecord where ProgramRecord.PK_ProgramRecord="<<programid<<"'";

	if(mysql_real_query(mysqltribune,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mrestribune=mysql_store_result(mysqltribune))){ 

		while((rowtribune=mysql_fetch_row(mrestribune))){
			category = (string)rowtribune[0];
		}
		
	}

	return category;
}

void InsertChannel(){
	MYSQL_RES *mrestribune;
	MYSQL_ROW rowtribune;
	std::ostringstream sSQL;

	sSQL<<"delete from channel";
	if( mysql_query(mysqlmyth, (sSQL.str()).c_str()) ){
		cerr<<"query: "<<sSQL.str()<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}
		
	string query="select PK_Station, CallSign, Name from Station";

	sSQL.str("");
	if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))){ 
	
		while( (rowtribune=mysql_fetch_row(mrestribune)) ) {
			sSQL<<"insert into channel(chanid, channum, callsign, name) VALUES ("<<rowtribune[0]<<", "<<rowtribune[0]<<", \""<<rowtribune[1]<<"\", \""<<rowtribune[2]<<"\")";
			
			if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
				cerr<<"query: "<<sSQL.str()<<endl
						<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
				exit(1);
			}
			sSQL.str("");
		}
	}

// 	query="select sourceid from cardinput";
// 
// 	if(mysql_real_query(mysqlmyth,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqlmyth))){ 
// 		rowtribune=mysql_fetch_row(mrestribune);
// 		sSQL<<"update channel set sourceid="<<rowtribune[0];
// 			
// 		if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
// 			cerr<<"query: "<<sSQL.str()<<endl
// 					<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
// 			exit(1);
// 		}
// 		sSQL.str("");
// 	}
}

string GetRole(string actorid){
	MYSQL_RES *mrestribune;
	MYSQL_ROW rowtribune;
	std::ostringstream sSQL;
	string role="";

	cout << "AID: "<<actorid<<endl;

	sSQL <<"select distinct Role.Description from Role join Actor_Role on Actor_Role.FK_Role=PK_Role join Actor on Actor.PK_Actor=Actor_Role.FK_Actor where Actor.PK_Actor=\""<<actorid<<"\"";

	if(mysql_real_query(mysqltribune,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mrestribune=mysql_store_result(mysqltribune))){ 

		rowtribune=mysql_fetch_row(mrestribune);
		role = (string)rowtribune[0];
	}

	return role;
}

int IsPersonInDB(string name){
	MYSQL_RES *mresmyth;
	MYSQL_ROW rowmyth;
	std::ostringstream sSQL;
	int personid;

	sSQL <<"select person from people where name=\""<<name<<"\"";

	if(mysql_real_query(mysqlmyth,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mresmyth=mysql_store_result(mysqlmyth))){ 

		while ((rowmyth=mysql_fetch_row(mresmyth))){
			return atoi(rowmyth[0]);
		}
		
	}
	
	return -1;
}

void InsertPeople(string programid, string chanid, string startime){
	MYSQL_RES *mrestribune;
	MYSQL_ROW rowtribune;
	std::ostringstream sSQL;

	sSQL <<"select distinct Actor.FirstName, Actor.LastName, Actor.PK_Actor from Actor join Actor_ProgramRecord on Actor_ProgramRecord.FK_Actor=PK_Actor join ProgramRecord on ProgramRecord.PK_ProgramRecord=Actor_ProgramRecord.FK_ProgramRecord where ProgramRecord.PK_ProgramRecord='"<<programid<<"'";

	if(mysql_real_query(mysqltribune,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mrestribune=mysql_store_result(mysqltribune))){ 

		sSQL.str("");

		while( (rowtribune=mysql_fetch_row(mrestribune)) ) {
			string fullname=(string)rowtribune[0] + " " + (string)rowtribune[1];
			int id=IsPersonInDB(fullname);
			if (id==-1){
				sSQL<<"insert into people(name) VALUES (\""<<fullname<<"\")";
				if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
					cerr<<"query: "<<sSQL.str()<<endl
							<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
					exit(1);
				}
				sSQL.str("");
				id = mysql_insert_id(mysqlmyth);
			}

			string role = GetRole((string) rowtribune[2]); 
			sSQL<<"insert into credits (person,chanid,starttime,role) VALUES ("<<id<<", \""<<chanid<<"\", \""<<startime<<"\", \""<<role<<"\")";
			
			if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
				cerr<<"query: "<<sSQL.str()<<endl
						<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
				exit(1);
			}
			sSQL.str("");
		}
		
	}
}

void InsertProgram(){
	MYSQL_RES *mrestribune, *mresmyth;
	MYSQL_ROW rowtribune, rowmyth;
	std::ostringstream sSQL;

	sSQL<<"delete from program";
	if( mysql_query(mysqlmyth, (sSQL.str()).c_str()) ){
		cerr<<"query: "<<sSQL.str()<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}
	sSQL.str("");
	sSQL<<"delete from people";
	if( mysql_query(mysqlmyth, (sSQL.str()).c_str()) ){
		cerr<<"query: "<<sSQL.str()<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}
	sSQL.str("");
	sSQL<<"delete from credits";
	if( mysql_query(mysqlmyth, (sSQL.str()).c_str()) ){
		cerr<<"query: "<<sSQL.str()<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}
	sSQL.str("");

	string query="select chanid from channel";

	if(mysql_real_query(mysqlmyth,query.c_str(),(unsigned int) query.length())==0&&(mresmyth=mysql_store_result(mysqlmyth))){ 

		while( (rowmyth=mysql_fetch_row(mresmyth)) ) {

			sSQL <<"SELECT Schedule.AirDate, Schedule.AirTime, Schedule.Duration, ProgramRecord.ReducedTitle1, ProgramRecord.Title, ProgramRecord.ReducedDesc1, ProgramRecord.OrgAirDate, ProgramRecord.StarRating, Schedule.Stereo, ProgramRecord.OrgAirDate, ProgramRecord.SynEpiNum, ProgramRecord.PK_ProgramRecord  FROM Schedule JOIN ProgramRecord ON ProgramRecord.PK_ProgramRecord = Schedule.FK_ProgramRecord WHERE FK_Station ="<<rowmyth[0];

			if(mysql_real_query(mysqltribune,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mrestribune=mysql_store_result(mysqltribune))){ 

				sSQL.str("");

				while( (rowtribune=mysql_fetch_row(mrestribune)) ) {

					string starttime = CalcStartTime((string)rowtribune[0], (string)rowtribune[1]);
					string endtime = CalcEndTime((string)rowtribune[0], (string)rowtribune[1],(string)rowtribune[2]);
					string category = GetCategory((string)rowtribune[11]);
					float stars = CalcStars((string)rowtribune[7]);
					InsertPeople((string)rowtribune[11],(string)rowmyth[0],starttime);
					sSQL<<"insert into program(chanid,starttime,endtime,title,subtitle,description,category,category_type,airdate,stars,stereo,originalairdate,syndicatedepisodenumber,programid) VALUES ("<<rowmyth[0]<<", \""<<starttime<<"\", \""<<endtime<<"\",  \""<<rowtribune[4]<<"\", \""<<rowtribune[3]<<"\", \""<<rowtribune[5]<<"\", \""<<category<<"\", \""<<category<<"\", \""<<rowtribune[6]<<"\", \""<<stars<<"\", \""<<rowtribune[8]<<"\", \""<<rowtribune[9]<<"\", \""<<rowtribune[10]<<"\", \""<<rowtribune[11]<<"\")";
			
					if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
						cerr<<"query: "<<sSQL.str()<<endl
								<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
						exit(1);
					}
					sSQL.str("");
				}
			}
		}
	}
}

void DeleteProgram(string programid){
	MYSQL_RES *mresmyth;
	MYSQL_ROW rowmyth;
	std::ostringstream sSQL;
	string chanid, starttime;

	sSQL <<"select chanid, starttime from program where programid='"<<programid<<"'";

	if(mysql_real_query(mysqlmyth,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mresmyth=mysql_store_result(mysqlmyth))){ 

		while ((rowmyth=mysql_fetch_row(mresmyth))){
			chanid = rowmyth[0];
			starttime = rowmyth[1];
		}
	}
	sSQL.str("");

	sSQL<<"delete from program where programid='"<<programid<<"'";
	if( mysql_query(mysqlmyth, (sSQL.str()).c_str()) ){
		cerr<<"query: "<<sSQL.str()<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}
	sSQL.str("");

	sSQL<<"delete from credits where chanid='"<<chanid<<"' and starttime='"<<starttime<<"'";
	if( mysql_query(mysqlmyth, (sSQL.str()).c_str()) ){
		cerr<<"query: "<<sSQL.str()<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}
	sSQL.str("");
}

void UpdateProgram(string programid){
	
	MYSQL_RES *mrestribune, *mresmyth;
	MYSQL_ROW rowtribune, rowmyth;
	std::ostringstream sSQL;

	cout <<"Intru cu "<<programid<<endl;
	
	sSQL <<"SELECT Schedule.AirDate, Schedule.AirTime, Schedule.Duration, ProgramRecord.ReducedTitle1, ProgramRecord.Title, ProgramRecord.ReducedDesc1, ProgramRecord.OrgAirDate, ProgramRecord.StarRating, Schedule.Stereo, ProgramRecord.OrgAirDate, ProgramRecord.SynEpiNum, Schedule.FK_Station  FROM Schedule JOIN ProgramRecord ON ProgramRecord.PK_ProgramRecord = Schedule.FK_ProgramRecord WHERE PK_ProgramRecord ='"<<programid<<"'";

	if(mysql_real_query(mysqltribune,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mrestribune=mysql_store_result(mysqltribune))){ 

		sSQL.str("");

		while( (rowtribune=mysql_fetch_row(mrestribune)) ) {

			string starttime = CalcStartTime((string)rowtribune[0], (string)rowtribune[1]);
			string endtime = CalcEndTime((string)rowtribune[0], (string)rowtribune[1],(string)rowtribune[2]);
			string category = GetCategory(programid);
			float stars = CalcStars((string)rowtribune[7]);
			InsertPeople(programid,(string)rowtribune[11],starttime);
			

			sSQL<<"delete from credits where starttime=\""<<starttime<<"\" and chanid=\""<<rowtribune[11]<<"\"";
	
			if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
				cerr<<"query: "<<sSQL.str()<<endl
						<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
				exit(1);
			}
			sSQL.str("");

			sSQL<<"delete from program where programid=\""<<programid<<"\" and starttime=\""<<starttime<<"\"";
	
			if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
				cerr<<"query: "<<sSQL.str()<<endl
						<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
				exit(1);
			}
			sSQL.str("");
			
			cout<<"Title: "<<rowtribune[4]<<endl;

			sSQL<<"insert into program(chanid,starttime,endtime,title,subtitle,description,category,category_type,airdate,stars,stereo,originalairdate,syndicatedepisodenumber,programid) VALUES ("<<rowtribune[11]<<", \""<<starttime<<"\", \""<<endtime<<"\",  \""<<rowtribune[4]<<"\", \""<<rowtribune[3]<<"\", \""<<rowtribune[5]<<"\", \""<<category<<"\", \""<<category<<"\", \""<<rowtribune[6]<<"\", \""<<stars<<"\", \""<<rowtribune[8]<<"\", \""<<rowtribune[9]<<"\", \""<<rowtribune[10]<<"\", \""<<programid<<"\")";

// 			sSQL<<"update program set starttime=\""<<starttime<<"\", endtime=\""<<endtime<<"\", title=\""<<rowtribune[4]<<"\", subtitle=\""<<rowtribune[3]<<"\", description=\""<<rowtribune[5]<<"\", category=\""<<category<<"\", category_type=\""<<category<<"\", airdate=\""<<rowtribune[6]<<"\", stars=\""<<stars<<"\", stereo=\""<<rowtribune[8]<<"\", originalairdate=\""<<rowtribune[9]<<"\", syndicatedepisodenumber=\""<<rowtribune[10]<<"\" where programid=\""<<programid<<"\"";
	
			if( mysql_query( mysqlmyth, (sSQL.str()).c_str()) ){
				cerr<<"query: "<<sSQL.str()<<endl
						<<"Error executing query: "<<mysql_error(mysqlmyth)<<endl;
				exit(1);
			}
			sSQL.str("");
		}
	}
}

int main(int argc, char *argv[]){

	string hosttribune="localhost",usertribune="root",passwdtribune="",dbtribune="pluto_tribune";
	my_bool reconnect = true;

	mysqltribune=mysql_init(NULL);
	mysql_options(mysqltribune, MYSQL_OPT_RECONNECT, &reconnect);
	if(!mysql_real_connect(mysqltribune,hosttribune.c_str(),usertribune.c_str(),passwdtribune.c_str(),dbtribune.c_str(),0,NULL,0)){
		cerr<<"Error connecting to db:"<<mysql_error(mysqltribune)<<endl;
		return false;
	}

	string hostmyth="localhost",usermyth="root",passwdmyth="",dbmyth="mythconverg";

	mysqlmyth=mysql_init(NULL);
	mysql_options(mysqlmyth, MYSQL_OPT_RECONNECT, &reconnect);
	if(!mysql_real_connect(mysqlmyth,hostmyth.c_str(),usermyth.c_str(),passwdmyth.c_str(),dbmyth.c_str(),0,NULL,0)){
		cerr<<"Error connecting to db:"<<mysql_error(mysqlmyth)<<endl;
		return false;
	}

	MYSQL_RES *mresmyth;
	MYSQL_ROW rowmyth;
	std::ostringstream sSQL;
	int nrrec;

	sSQL <<"select count(*) from program";

	if(mysql_real_query(mysqlmyth,(sSQL.str()).c_str(),(unsigned int) (sSQL.str()).length())==0&&(mresmyth=mysql_store_result(mysqlmyth))){ 

		while ((rowmyth=mysql_fetch_row(mresmyth))){
			nrrec = atoi(rowmyth[0]);
		}
		
	}

	if (nrrec>0){

		ifstream deletefile("/tmp/deletefile");
		string line;

		if (!deletefile) {
			cerr<<"Error opening file /tmp/deletefile"<<endl;
			return false;
		}
	
		if (deletefile.is_open()) {
			
			while(! deletefile.eof())
			{
				getline (deletefile,line);
	
				if (line.length()!=0){
					cout << line << endl;
					DeleteProgram(line);
				}
				
				
			}
		}

		ifstream updatefile("/tmp/updatefile");
		
		if (!updatefile) {
			cerr<<"Error opening file /tmp/updatefile"<<endl;
			return false;
		}
	
		if (updatefile.is_open()) {
			
			while(! updatefile.eof())
			{
				getline (updatefile,line);
	
				if (line.length()!=0){
					cout << line << endl;
					UpdateProgram(line);
				}
				
				
			}
		}

	} else{
	
		InsertChannel();

		InsertProgram();

	}

	//cout<<CalcEndTime("20070131", "2315", "100") <<endl;
	//cout << CalcStars("+") <<endl;
	

}
