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
#include <map>
#include <fstream>
#include <time.h>
#include <iostream>
extern "C"
{
#include <mysql/mysql.h>
}
#include "Tribunexml.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#define WORK "work"
string URL="http://192.168.125.1/tribune/";


using namespace DCE;
using namespace std;
string device="empty";

int getURL(const string filename){
	string command=(string) "wget -O - \""+URL+filename+".tar.bz2\"|tar xjf - -C /tmp";
	cout<<command<<endl;
	return system(command.c_str());
}

int patchFile(const string &sfilename, const string &sminusname, const string &splusname){
	string command="/usr/pluto/bin/rmprogs "+sminusname+" < "+sfilename+">"+sfilename+";cat "+sfilename+" "+splusname+"|sort>"+sfilename;
	cout<<command<<endl;
	return system(command.c_str());
}

int fillStation(ifstream &f, Station *s) {
	if(f.eof())
		return false;
	string str="",buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	int field=1;
	f.getline(tmp, sizeof(tmp));
	if(*tmp==0)
		return false;
	buf=(string)tmp+"|";
	int num;
	while((num=buf.find("|"))>=0){
		str=buf.substr(0,num);
		switch( field ){
			case 1:
				s->tf_station_num=str;
				break;
			case 2:
				s->tf_station_time_zone=str;
				break;
			case 3:
				s->tf_station_name=str;
				break;
			case 4:
				s->tf_station_call_sign=str;
				break;
			case 5:
				s->tf_station_affil=str;
				break;
			case 6:
				s->tf_station_city=str;
				break;
			case 7:
				s->tf_station_zip_code=str;
				break;
			case 8:
				s->tf_station_state=str;
				break;
			case 9:
				s->tf_station_country=str;
				break;
			case 10:
				s->tf_dma_name=str;
				break;
			case 11:
				s->tf_dma_num=str;
				break;
			case 12:
				s->tf_fcc_channel_num=str;
				break;
		}
		str="";
		field++;
		if((unsigned)num+1<=buf.size())
			buf.erase(0,num+1);
	}
	return true;
}

void postStation(ofstream &fxml,const Station *s){
	fxml<<"<station ";
	fxml<<"id=\""+s->tf_station_num+"\">"<<endl;
	fxml<<"<callSign>"<<s->tf_station_call_sign<<"</callSign>"<<endl;
	fxml<<"<name>"<<s->tf_station_name<<"</name>"<<endl;
	if(s->tf_station_affil.length())
		fxml<<"<affiliate>"<<s->tf_station_affil<<"</affiliate>"<<endl;
	if(s->tf_fcc_channel_num.length())
		fxml<<"<fccChannelNumber>"<<s->tf_fcc_channel_num<<"</fccChannelNumber>"<<endl;
	fxml<<"</station>"<<endl;
}

void postLineup(const ChannelLineup &l, ofstream &fxml){
	if(device!=l.device){
		if(device!="empty"){
			fxml<<"</lineup>"<<endl;
		}
		fxml<<"<lineup name=\""<<l.name<<"\" ";
		if(l.device.length())
			fxml<<"device=\""<<l.device<<"\" ";
		fxml<<"id=\""<<l.name<<"\" postalCode=\""<<l.postalCode<<"\">"<<endl;
		device=l.device;
	}
	fxml<<"<map station=\""<<l.station_num<<"\" channel=\""<<l.channel<<"\" ";
	if(l.effective_date.length())
		fxml<<"from=\""<<l.effective_date<<"\" ";
	if(l.expiration_date.length())
		fxml<<"to=\""<<l.expiration_date<<"\" ";
	fxml<<"/>"<<endl;
}

int fillSchedule(ifstream &f, Schedule *s){
	if(f.eof()) return false;
	string str="",buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	int field=1;
	f.getline(tmp, sizeof(tmp));
	if(*tmp==0)
		return false;
	buf=(string)tmp+"|";
	int num;
	while((num=buf.find("|"))>=0){
		str=buf.substr(0,num);
		switch( field ){
			case 1:
				s->tf_station_num=str;
				break;
			case 2:
				s->tf_database_key=str;
				break;
			case 3:
				if(str.length()){
					str.insert(6,"-");
					str.insert(4,"-");
				}
				s->tf_air_date=str;
				break;
			case 4:
				if(str.length()){
					str.insert(2,":");
					s->tf_air_time="T"+str+":00Z";
				}else
					s->tf_air_time=str;
				break;
			case 5:
				if(str.length()){
					str.insert(2,"H");
					s->tf_duration="PT"+str+"M";
				}else
					s->tf_duration=str;
				break;
			case 6:
				s->tf_part_num=str;
				break;
			case 7:
				s->tf_num_of_parts=str;
				break;
			case 8:
				s->tf_cc=(str=="Y"?"true":"false");
				break;
			case 9:
				s->tf_stereo=(str=="Y"?"true":"false");
				break;
			case 10:
				s->tf_repeat=(str=="Y"?"true":"false");
				break;
			case 11:
				s->tf_live_tape_delay=str;
				break;
			case 12:
				s->tf_subtitled=(str=="Y"?"true":"false");
				break;
			case 13:
				s->tf_premiere_finale=str;
				break;
			case 14:
				s->tf_joined_in_progress=(str=="Y"?"true":"false");
				break;
			case 15:
				s->tf_cable_in_the_classroom=(str=="Y"?"true":"false");
				break;
			case 16:
				s->tf_tv_raiting=str;
				break;
			case 17:
				s->tf_sap=(str=="Y"?"true":"false");
				break;
			case 18:
				s->tf_blackout=(str=="Y"?"true":"false");
				break;
			case 19:
				s->tf_sex_raiting=(str=="Y"?"true":"false");
				break;
			case 20:
				s->tf_violence_raiting=(str=="Y"?"true":"false");
				break;
			case 21:
				s->tf_language_raiting=(str=="Y"?"true":"false");
				break;
			case 22:
				s->tf_dialog_raiting=(str=="Y"?"true":"false");
				break;
			case 23:
				s->tf_fv_raiting=(str=="Y"?"true":"false");
				break;
			case 24:
				s->tf_enhanced=(str=="Y"?"true":"false");
				break;
			case 25:
				s->tf_three_d=(str=="Y"?"true":"false");
				break;
			case 26:
				s->tf_letterbox=(str=="Y"?"true":"false");
				break;
			case 27:
				s->tf_hdtv=(str=="Y"?"true":"false");
				break;
			case 28:
				s->tf_dolby=str;
				break;
			case 29:
				s->tf_dvs=(str=="Y"?"true":"false");
				break;
		}
		str="";
		field++;
		if((unsigned)num+1<=buf.size())
			buf.erase(0,num+1);
	}
	return true;
}

void postSchedule(ofstream &fxml,const Schedule *s){
	fxml<<"<schedule program=\""<<s->tf_database_key<<"\" station=\""<<s->tf_station_num<<"\" time=\""<<s->tf_air_date<<s->tf_air_time<<"\" duration=\""<<s->tf_duration<<"\" repeat=\""<<s->tf_repeat<<"\" stereo=\""<<s->tf_stereo<<"\" subtitled=\""<<s->tf_subtitled<<"\" hdtv=\""<<s->tf_hdtv<<"\" closeCaptioned=\""<<s->tf_cc<<"\" ";
	if(s->tf_tv_raiting.length())
		fxml<<"tvRaiting=\""<<s->tf_tv_raiting<<"\" ";
	if(s->tf_dolby.length())
		fxml<<"dolby=\""<<s->tf_dolby<<"\" ";
	if(s->tf_part_num.length())
		fxml<<"number=\""<<s->tf_part_num<<"\" ";
	if(s->tf_num_of_parts.length())
		fxml<<"total=\""<<s->tf_num_of_parts<<"\" ";
	fxml<<"/>"<<endl;
}

int fillProgram(ifstream &f, Program *p){
	if(f.eof()) return false;
	string str="",buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	int field=1;
	f.getline(tmp, sizeof(tmp));
	if(*tmp==0)
		return false;
	buf=(string)tmp+"|";
	int num;
	while((num=buf.find("|"))>=0){
		str=buf.substr(0,num);
		switch( field ){
			case 1:
				p->tf_database_key=str;
				break;
			case 2:
				p->tf_title=str;
				break;
			case 3:case 4:case 5: case 6:
				p->tf_reduced_title[field-3]=str;
				break;
			case 7:
				p->tf_alt_title=str;
				break;
			case 8: case 9: case 10:
				p->tf_reduced_desc[field-8]=str;
				break;
			case 11: case 12: case 13: case 14: case 15: case 16:
				p->tf_advisory_desc[field-11]=str;
				break;
			case 17:
				p->tf_desc[0]=str;
				break;
			case 18:
				p->tf_year=str;
				break;
			case 19:
				p->tf_mpaa_rating=str;
				break;
			case 20:
				p->tf_star_rating=str;
				break;
			case 21:
				if(str.length()){
					str.insert(2,"H");
					p->tf_run_time="PT"+str+"M";
				}else
					p->tf_run_time="";
				break;
			case 22:
				p->tf_color_code=str;
				break;
			case 23:
				p->tf_program_language=str;
				break;
			case 24:
				p->tf_org_country=str;
				break;
			case 25:
				p->tf_made_for_tv=(str=="Y"?"true":"false");
				break;
			case 26:
				p->tf_source_type=str;
				break;
			case 27:
				p->tf_show_type=str;
				break;
			case 28:
				p->tf_holiday=str;
				break;
			case 29:
				p->tf_syn_epi_num=str;
				break;
			case 30:
				p->tf_alt_syn_epi_num=str;
				break;
			case 31:
				p->tf_epi_title=str;
				break;
			case 32:
				p->tf_net_syn_source=str;
				break;
			case 33:
				p->tf_net_syn_type=str;
				break;
			case 34:
				p->tf_desc[1]=str;
				break;
			case 35:
				p->tf_reduced_desc[3]=str;
				break;
			case 36:
				p->tf_org_studio=str;
				break;
			case 37:
				p->tf_game_date=str;
				break;
			case 38:
				p->tf_game_time=str;
				break;
			case 39:
				p->tf_game_time_xone=str;
				break;
			case 40:
				p->tf_org_air_date=str;
				break;
			case 41:
				p->tf_unique_id=str;
				break;
			default:
				break;
		}
		str="";
		field++;
		if((unsigned)num+1<=buf.size())
			buf.erase(0,num+1);
	}
	return true;
}

void postProgram(ofstream &fxml,const Program *p){
	fxml<<"<program id=\""<<p->tf_database_key<<"\">"<<endl;
	if(p->tf_database_key.substr(0,2)=="EP")
		fxml<<"<series>SH"<<p->tf_database_key.substr(2,6)<<"</series>"<<endl;
	fxml<<"<title>"<<p->tf_title<<"</title>"<<endl;
	if(p->tf_alt_title.length())
		fxml<<"<subtitle>"<<p->tf_alt_title<<"</subtitle>"<<endl;
	if(p->tf_desc[0].length())
		fxml<<"<description>"<<p->tf_desc[0]<<"</description>"<<endl;
	if(p->tf_mpaa_rating.length())
		fxml<<"<mpaaRating>"<<p->tf_mpaa_rating<<"</mpaaRating>"<<endl;
	if(p->tf_star_rating.length())
		fxml<<"<starRating>"<<p->tf_star_rating<<"</starRating>"<<endl;
	if(p->tf_run_time.length())
		fxml<<"<runTime>"<<p->tf_run_time<<"</runTime>"<<endl;
	if(p->tf_year.length())
		fxml<<"<year>"<<p->tf_year<<"</year>"<<endl;
	if(p->tf_show_type.length())
		fxml<<"<showType>"<<p->tf_show_type<<"</showType>"<<endl;
	if(p->tf_color_code.length())
		fxml<<"<colorCode>"<<p->tf_color_code<<"</colorCode>"<<endl;
	if(p->tf_org_air_date.length())
		fxml<<"<originalAirDate>"<<p->tf_org_air_date<<"</originalAirDate>"<<endl;
	if(p->tf_syn_epi_num.length())
		fxml<<"<syndicatedEpisodeNumber>"<<p->tf_syn_epi_num<<"</syndicatedEpisodeNumber>"<<endl;
	fxml<<"<advisories>"<<endl;
	int num=0;
	while(p->tf_advisory_desc[num].length())
		fxml<<"<advisory>"<<p->tf_advisory_desc[num++]<<"</advisory>"<<endl;
	fxml<<"</advisories>"<<endl;
	fxml<<"</program>"<<endl;
}

void postCrew(ifstream &f, ofstream &fxml){
	string str[4],buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	int num;
	string prog="empty";
	while(!f.eof()){
		int field=0;
		f.getline(tmp, sizeof(tmp));
		if(*tmp==0)
			return;
		buf=(string)tmp;
		while((num=buf.find("|"))>=0){
			str[field++]=buf.substr(0,num);
			if((unsigned)num+1<=buf.size())
				buf.erase(0,num+1);
		}
		if(prog!=str[0]){
			if(prog!="empty")
				fxml<<"</crew>"<<endl;
			fxml<<"<crew program=\""<<str[0]<<"\">"<<endl;
			prog=str[0];
		}
		fxml<<"<member>"<<endl
				<<"<role>"<<str[3]<<"</role>"<<endl
				<<"<givenname>"<<str[1]<<"</givenname>"<<endl
				<<"<surname>"<<str[2]<<"</surname>"<<endl
				<<"</member>"<<endl;
	}
}

void postGenre(ifstream &f, ofstream &fxml){
	string str[2],buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	int num;
	string prog="empty";
	while(!f.eof()){
		int field=0;
		f.getline(tmp, sizeof(tmp));
		if(*tmp==0)
			return;
		buf=(string)tmp+"|";
		while((num=buf.find("|"))>=0){
			str[field++]=buf.substr(0,num);
			if((unsigned)num+1<=buf.size())
				buf.erase(0,num+1);
		}
		if(prog!=str[0]){
			if(prog!="empty")
				fxml<<"</programGenre>"<<endl;
			fxml<<"<programGenre program=\""<<str[0]<<"\">"<<endl;
			prog=str[0];
		}
		fxml<<"<genre>"<<endl
				<<"<class>"<<str[1]<<"</class>"<<endl
				<<"<relevance>0</relevance>"<<endl
				<<"</genre>"<<endl;
	}
}

void fillNext(const string station_id,const int funcNo,ofstream &fxml){
	string fileName="";
	switch (funcNo){
		case 1:fileName=(string)"/tmp/"+station_id+"/statrec.txt";break;
		case 2:fileName=(string)"/tmp/"+station_id+"/skedrec.txt";break;
	}
	ifstream f(fileName.c_str());
	if (!f){
		cerr<<"Error opening file "<<fileName<<endl;
		return;
	}
	Station stat;
	Schedule s;
	string command;
	switch(funcNo){
		case 1:
			while(fillStation(f,&stat)){
				postStation(fxml,&stat);
			}
			break;
		case 2:
			while(fillSchedule(f,&s)){
				postSchedule(fxml,&s);
			}
			command=(string)"cut -f2 -d'|' "+fileName+" | sort | uniq | sed 's/$/|/' >>  /tmp/programs.txt";
			system(command.c_str());
			break;
	}
	f.close();
}

int Replace( const string &sFileInput, const string &sSearch, const string &sReplace ){
	string command="replace \""+sSearch+"\" \""+sReplace+"\" -- \""+sFileInput+"\"";
	return system(command.c_str());
}

bool ReplaceFromFile(const string sFileNameFrom, const string &sFileNameTo){
	vector<string> vSearchList;
	ifstream f( sFileNameFrom.c_str() );
	FileUtils::ReadFileIntoVector(sFileNameFrom,vSearchList);
	char num2[20];
	memset( num2, 0, sizeof(num2) );
	for( uint i=0;i<vSearchList.size();i++ ){
		sprintf( num2, "|%d|", i+1);
		Replace( sFileNameTo, num2, (string)"|"+vSearchList[i]+"|");
	}
	return true;
}

void CreateXML(const ChannelLineup clineup){
	ofstream fxml("/tmp/station.xml",std::ios_base::app);
	getURL((string)"stations/"+clineup.station_num);
	cout<<"Writing station"<<endl;
	fillNext(clineup.station_num,1,fxml);
	fxml.close();
	cout<<"Writing schedules"<<endl;
	fxml.open("/tmp/schedule.xml",std::ios_base::app);
	fillNext(clineup.station_num,2,fxml);
	fxml.close();
	string command=(string)"rm -rf /tmp/"+clineup.station_num;
	system(command.c_str());
}

void CreateProgramXML(){
	ofstream fxml("/tmp/progs.xml");
	if(FileUtils::FileExists("/tmp/programs.txt")){
		string command=(string)"sort /tmp/programs.txt|uniq >/tmp/programs.txt;cat "+WORK+"/progrec.txt| /usr/pluto/bin/filterprogs /tmp/programs.txt > /tmp/progrec.txt;cat "+WORK+"/crew.txt| /usr/pluto/bin/filterprogs /tmp/programs.txt > /tmp/crew.txt;cat "+WORK+"/genres.txt| /usr/pluto/bin/filterprogs /tmp/programs.txt > /tmp/genres.txt";
		cout<<command<<endl;
		system(command.c_str());

		ifstream fp("/tmp/progrec.txt");
		cout<<"Writing programs"<<endl;
		fxml<<"<programs>"<<endl;
		Program p;
		while(fillProgram(fp,&p)){
			postProgram(fxml,&p);
		}
		fxml<<"</programs>"<<endl;
		fp.close();
		cout<<"Writing crew"<<endl;
		ifstream fc("/tmp/crew.txt");
		fxml<<"<productionCrew>"<<endl;
		postCrew(fc, fxml);
		fxml<<"</crew>"<<endl
				<<"</productionCrew>"<<endl;
		fc.close();
		cout<<"Writing genres"<<endl;
		ifstream fg("/tmp/genres.txt");
		fxml<<"<genres>"<<endl;
		postGenre(fg, fxml);
		fg.close();
		fxml<<"</programGenre>"<<endl
				<<"</genres>"<<endl;
	}
	fxml<<"</xtvd>"<<endl;
}

void DoXMLCommonPart(vector<ChannelLineup> &chanLineupList){
	ofstream fxml("/tmp/head.xml");
	struct tm *date;
	time_t tTime=time(NULL);
	date=gmtime(&tTime);

	fxml<<"<?xml version='1.0' encoding='utf-8'?>"<<endl<<"<xtvd from=\""<<1900+date->tm_year<<"-"<<(date->tm_mon<9?"0":"")<<1+date->tm_mon<<"-"<<(date->tm_mday<10?"0":"")<<date->tm_mday<<"T00:00:00Z\"";
	tTime+=14*3600*24;
	date=gmtime(&tTime);
	fxml<<" to=\""<<1900+date->tm_year<<"-"<<(date->tm_mon<9?"0":"")<<1+date->tm_mon<<"-"<<(date->tm_mday<10?"0":"")<<date->tm_mday<<"T23:59:59Z\" xmlns=\"urn:TMSWebServices\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:TMSWebServices http://docs.tms.tribune.com/tech/xml/schemas/tmsxtvd.xsd\" schemaVersion=\"1.3\">"<<endl;
	cout<<"Writing lineups"<<endl;
	fxml.close();
	fxml.open("/tmp/lineup.xml");
	fxml<<"<lineups>"<<endl;
	device="empty";
	for( vector<ChannelLineup>::iterator it=chanLineupList.begin(); it!=chanLineupList.end(); it++ ){
		postLineup(*it,fxml);
	}
	fxml<<"</lineup>"<<endl<<"</lineups>"<<endl;
	fxml.close();
	fxml.close();
}

void postXMLTomythconverg(const string sourceid, const string lineupname){
	CreateProgramXML();
	ofstream fxml("/tmp/station.xml",std::ios_base::app);
	fxml<<"</stations>"<<endl;
	fxml.close();
	fxml.open("/tmp/schedule.xml",std::ios_base::app);
	fxml<<"</schedules>"<<endl;
	fxml.close();

	system("cat /tmp/head.xml /tmp/station.xml /tmp/lineup.xml /tmp/schedule.xml /tmp/progs.xml |replace \"&\" \"&amp;\">/tmp/temp.xml");
	string command=(string)"/usr/bin/mythfilldatabase --dd-file "+sourceid+" -1 \""+lineupname+"\" /tmp/temp.xml";
	cout<<"Command: "<<command<<endl;
	system(command.c_str());
	command=(string)"rm -rf /tmp/temp.xml /tmp/schedule.xml /tmp/station.xml /tmp/programs.txt /tmp/crew.txt /tmp/genres.txt /tmp/progrec.txt /tmp/progs.xml";
	cout<<"Command: "<<command<<endl;
	system(command.c_str());
}

int main(int argc, char *argv[]){
	bool bError=false;
	char c;
	string sourceid="";
	string hostmyth="dcerouter",usermyth="root",passwdmyth="",dbmyth="mythconverg";
	string hosttribune="dcerouter",usertribune="root",passwdtribune="",dbtribune="pluto_myth";
	for(int i=1;i<argc;i++){
		if(argv[i][0]!='-'){
			cerr<<"Unknown option"<<endl;
			bError=true;
			break;
		}
		c=argv[i][1];
		switch(c){
			case 'h':hostmyth=argv[++i];break;
			case 'u':usermyth=argv[++i];break;
			case 'p':passwdmyth=argv[++i];break;
			case 'd':dbmyth=argv[++i];break;
			case 'H':hosttribune=argv[++i];break;
			case 'U':usertribune=argv[++i];break;
			case 'P':passwdtribune=argv[++i];break;
			case 'D':dbtribune=argv[++i];break;
			case 's':sourceid=argv[++i];break;
			case 'R':URL=argv[++i];break;
			default:cerr<<"Unknown option"<<endl;
							bError=true;
		}
	}
	if(bError){
		cout << "Usage: "<< argv[0]
				<< " [-h MySql hostname of myth database] [-u MySql username of myth database] " << endl
				<< " [-p MySql password of myth database] [-d MySql database of myth database] " << endl
				<< " [-H MySql hostname of tribune database] [-U MySql username of tribune database] " << endl
				<< " [-P MySql password of tribune database] [-D MySql database of tribune database] " << endl
				<< " [-s sourceid] [-R URL to download parsed files]" << endl;
		return false;
	}
	string wheresql="";
	if(sourceid.length()){
		wheresql=(string)" AND sourceid="+sourceid;
	}
	MYSQL *mysqlmyth,*mysqltribune;
	MYSQL_RES *mresmyth,*mrestribune;
	MYSQL_ROW rowmyth,rowtribune;
	my_bool reconnect = true;
	mysqlmyth=mysql_init(NULL);
	mysql_options(mysqlmyth, MYSQL_OPT_RECONNECT, &reconnect);
	mysqltribune=mysql_init(NULL);
	mysql_options(mysqltribune, MYSQL_OPT_RECONNECT, &reconnect);
	if(!mysql_real_connect(mysqlmyth,hostmyth.c_str(),usermyth.c_str(),passwdmyth.c_str(),dbmyth.c_str(),0,NULL,0)){
		cerr<<"Error connecting to db:"<<mysql_error(mysqlmyth)<<endl;
		return false;
	}
	if(!mysql_real_connect(mysqltribune,hosttribune.c_str(),usertribune.c_str(),passwdtribune.c_str(),dbtribune.c_str(),0,NULL,0)){
		cerr<<"Error connecting to db:"<<mysql_error(mysqltribune)<<endl;
		return false;
	}
	cout<<"Getting common part"<<endl;
	string str;
	if( FileUtils::FileExists((string)WORK+"/progrec.txt") ){
		getURL("diffs");
		ReplaceFromFile((string)WORK+"/uniqueroles.txt","/tmp/diffs/crew.diff");
		patchFile((string)WORK+"/progrec.txt","/tmp/diffs/progminus.diff","/tmp/diffs/progplus.diff");
		patchFile((string)WORK+"/crew.txt","/tmp/diffs/progminus.diff","/tmp/diffs/crew.diff");
		patchFile((string)WORK+"/genres.txt","/tmp/diffs/progminus.diff","/tmp/diffs/genres.diff");
		system("rm -rf /tmp/diffs");
	}else{
		getURL("progs");
		getURL("uniqueroles");
		FileUtils::PUCopyFile("/tmp/progs/progrec.txt",(string)WORK+"/progrec.txt");
		FileUtils::PUCopyFile("/tmp/uniqueroles.txt",(string)WORK+"/uniqueroles.txt");
		ReplaceFromFile((string)WORK+"/uniqueroles.txt","/tmp/progs/crew.txt");
		FileUtils::PUCopyFile("/tmp/progs/crew.txt",(string)WORK+"/crew.txt");
		FileUtils::PUCopyFile("/tmp/progs/genres.txt",(string)WORK+"/genres.txt");
		system("rm -rf /tmp/progs /tmp/uniqueroles.txt");
	}
	string query=(string)"SELECT sourceid FROM videosource WHERE xmltvgrabber='tribunetv'"+wheresql;
	cout<<"Getting sourceid"<<endl;
	if(mysql_real_query(mysqlmyth,query.c_str(),(unsigned int) query.length())==0&&(mresmyth=mysql_store_result(mysqlmyth))) {
		vector<ChannelLineup> chanLineupList;
		while( (rowmyth=mysql_fetch_row(mresmyth)) ){
			query=(string)"SELECT l.name, h.zip_code, l.FK_Stations, l.device, l.tms_chan, l.effective_date, l.expiration_date "
					"FROM UserLineups l "
					"LEFT JOIN Headends h on l.FK_Headends=h.PK_Headends "
					"WHERE l.sourceid="+rowmyth[0]+
					" ORDER BY l.name, l.device";
			cout<<"Getting lineups"<<endl;
			if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0 &&(mrestribune=mysql_store_result(mysqltribune)))
			{
				while( (rowtribune=mysql_fetch_row(mrestribune)) ){
					ChannelLineup newlineup;
					newlineup.name=rowtribune[0];
					newlineup.postalCode=rowtribune[1];
					newlineup.station_num=rowtribune[2];
					newlineup.device=rowtribune[3];
					newlineup.channel=rowtribune[4];
					newlineup.effective_date=rowtribune[5];
					newlineup.expiration_date=rowtribune[6];
					chanLineupList.push_back(newlineup);
				}
			}else{
				cerr<<"Error getting user lineup"<<endl;
				cerr<<"Error query:"<<query<<endl<<mysql_error(mysqltribune)<<endl;
				continue;
			}
			if (!chanLineupList.size()){
				cerr<<"User lineup has no cannel"<<endl;
				continue;
			}
			DoXMLCommonPart(chanLineupList);
			string lineupname=chanLineupList[0].name;
			ofstream fxml("/tmp/station.xml");
			fxml<<"<stations>"<<endl;
			fxml.close();
			fxml.open("/tmp/schedule.xml");
			fxml<<"<schedules>"<<endl;
			fxml.close();
			for( vector<ChannelLineup>::iterator it=chanLineupList.begin(); it!=chanLineupList.end(); it++ ){
				if(lineupname!=it->name){
					postXMLTomythconverg(rowmyth[0],lineupname);
					lineupname=it->name;
					fxml.open("/tmp/station.xml");
					fxml<<"<stations>"<<endl;
					fxml.close();
					fxml.open("/tmp/schedule.xml");
					fxml<<"<schedules>"<<endl;
					fxml.close();
				}
				CreateXML(*it);
			}
			postXMLTomythconverg(rowmyth[0],lineupname);
			system("rm -rf /tmp/head.xml /tmp/lineup.xml");
		}
	}else{
		cerr<<"Error query:"<<query<<endl<<mysql_error(mysqlmyth)<<endl;
		return false;
	}
	cout<<"Finished"<<endl;
	return true;
}
