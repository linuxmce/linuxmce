/*
SVNLog
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com		

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

See the GNU General Public License for more details.
*/

/**
*
* @file SVNLog.cpp
* @brief entry point for the SVNLog project
* @author Aaron
*/


#include "PlutoUtils/CommonIncludes.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#endif

#include "../version.h"



using namespace std;
using namespace DCE;

class Commit
{
public:
	string User;
	int Lines;
	int Revision;
	time_t date;
};

map<int,Commit *> mapCommits;
map<string,Commit *> mapUser;

typedef struct pair<string,int> PAIR_SI;

map<PAIR_SI,int> mapUsersWeeks;

int main( int argc, char *argv[] )
{
	chdir("/home/MakeRelease/trunk");
	unlink("output.all");
	cout << "About to execute command" << endl;
	system("svn log -q | grep '^r' > output.all");
	cout << "About to sleep" << endl;
	Sleep(1000);
	cout << "Slept" << endl;
	
	vector<string> vectLines;
	size_t size;
	char *ptr = FileUtils::ReadFileIntoBuffer("output.all",size);
//	cout << "File is: " << Lines.size() << " bytes." << endl;
	string Lines = ptr;

	StringUtils::Tokenize(Lines,"\r\n",vectLines);
	delete ptr;
	cout << "File is: " << vectLines.size() << " lines" << endl;

	int LowestWeek=0,HighestWeek=0;

	for(size_t s=0;s<vectLines.size();++s)
	{
		string Line = vectLines[s];
		int Revision = atoi(Line.substr(1).c_str());
		if( Revision==1 || Revision==2 || Revision==38 || Revision==98
			|| Revision==109 || Revision==111 
			|| Revision==219 
			|| Revision==249 || Revision==307 
			|| Revision==432
			|| Revision==487
			|| Revision==507 || Revision==534
			|| Revision==599
			|| Revision==1386 
			|| Revision==1522 || Revision==1895 || Revision==1889 
			|| Revision==4215 )
			continue;
		
		string::size_type start_pipe = Line.find('|');
		string::size_type stop_pipe = Line.find('|',start_pipe+1);
		string User = Line.substr(start_pipe+2,stop_pipe-start_pipe-3);
string x = Line.substr(stop_pipe+2);
string y = Line.substr(stop_pipe+7);
string z = Line.substr(stop_pipe+10);
		int Year = atoi(Line.substr(stop_pipe+2).c_str());
		int Month = atoi(Line.substr(stop_pipe+7).c_str());
		int Day = atoi(Line.substr(stop_pipe+10).c_str());

		struct tm tm_time;
		tm_time.tm_year = Year-1900;
		tm_time.tm_mon = Month-1;
		tm_time.tm_mday = Day;
		tm_time.tm_hour = 0;
		tm_time.tm_min = 0;
		tm_time.tm_sec = 0;
		time_t t = mktime(&tm_time);
		int Week = ((Year-2004)*52) + (tm_time.tm_yday / 7);

		if( Week > HighestWeek )
			HighestWeek=Week;
		if( Week < LowestWeek || LowestWeek==0 )
			LowestWeek=Week;
cout << Year << "/" << Month << "/" << Day << " yday: " << tm_time.tm_yday << " week: ";
cout << Week << " Highest: " << HighestWeek << " LOwest: " << LowestWeek << endl;
		int RetryCount=0;

retry:
		cout << "Line: " << s << " of " << vectLines.size() << " REV: " << Revision << " WEEK: " << Week << endl;
		string Cmd = "svn diff --revision " + StringUtils::itos(Revision-1) + ":" + StringUtils::itos(Revision) + " > output.diff";
		cout << "cmd: " << Cmd << endl;
		system(Cmd.c_str());
		Sleep(1000);
		vector<string> vectFiles;
		ptr = FileUtils::ReadFileIntoBuffer("output.diff",size);
		cout << "File is: " << size << " bytes." << endl;
		Lines = ptr;
		StringUtils::Tokenize(Lines,"\r\n",vectFiles);
		delete ptr;
		cout << "File is: " << vectFiles.size() << " lines" << endl;
		if( size==0 )
		{
			Cmd = "svn diff --revision " + StringUtils::itos(Revision-1) + ":" + StringUtils::itos(Revision) + " > output.diff";
			cout << "testing for death - cmd: " << Cmd << endl;
			system(Cmd.c_str());
			Sleep(1000);
			ptr = FileUtils::ReadFileIntoBuffer("output.diff",size);
			delete ptr;
			if( size==0 )
			{
				if( RetryCount>2 )
				{
					cout << "Tried restarting already.  Maybe there are no diffs?" << endl;
				}
				else
				{
					RetryCount++;
					cout << "SVN died again.  Retrying...." << endl;
					Sleep(1000);
					system("service inetd restart");
					Sleep(1000);
					goto retry;
				}
			}
			else
				cout << "SVN still alive.  Just nothing to report: size: " << size << endl;
		}

		int LinesChanged=0;
		string Filename;
		for(size_t s2=0;s2<vectFiles.size();++s2)
		{
			string File = vectFiles[s2];
			if( File.substr(0,6)=="Index:" )
			{
				Filename = File.substr(7);
				cout << "new file: " << File << "=" << Filename << endl;
				continue;
			}

			if( Filename.find("external/")!=string::npos || Filename.find("libs/")!=string::npos ||
				Filename.find("installers/")!=string::npos || Filename.find("misc_utils/")!=string::npos ||
				Filename.find("pluto_main/")!=string::npos || Filename.find("Gen_Devices/")!=string::npos ||
				Filename.find("pluto_media/")!=string::npos || Filename.find("Libraries/")!=string::npos ||
				Filename.find("pluto_security/")!=string::npos || Filename.find("pluto-support-modified/")!=string::npos ||
				Filename.find("utilities/")!=string::npos || Filename.find("from-mythtv/")!=string::npos ||
				Filename.find("CrossPlatform-Libs/")!=string::npos )
   			{
          	   continue;
    		}

			cout << "line: " << File << endl;

			if( File[0]=='-' && File[1]!='-' )
				LinesChanged++;
			else if( File[0]=='+' && File[1]!='+' )
				LinesChanged++;
		}

		cout << "============LOGGING COMMIT===============" << endl <<
			"user: " << User << " lines: " << LinesChanged << " week: " << Week << endl <<
			"===========================================" << endl;

		Commit *pCommit = new Commit();
		pCommit->date = t;
		pCommit->Lines = LinesChanged;
		pCommit->Revision = Revision;
		pCommit->User = User;
		mapCommits[Revision] = pCommit;
		mapUser[User] = pCommit;

		PAIR_SI psi(User,Week);
		int LinesSoFar = mapUsersWeeks[psi];
		LinesSoFar += LinesChanged;
		mapUsersWeeks[psi] = LinesSoFar;
cout << "pair: " << User << " week: " << Week << " lines: " << LinesSoFar << " lineschanged: " << LinesChanged << "  map: " << mapUsersWeeks.size() << endl;			
	}

	cout << "\t";
	for(map<string,Commit *>::iterator it=mapUser.begin();it!=mapUser.end();++it)
		cout << (*it).first << "\t";
	cout << endl;
cout << "we Highest: " << HighestWeek << " LOwest: " << LowestWeek << endl;
	for(int i=LowestWeek;i<=HighestWeek;++i)
	{
		cout << i << "\t";
		for(map<string,Commit *>::iterator it=mapUser.begin();it!=mapUser.end();++it)
		{
			PAIR_SI psi( (*it).first,i);
bool bFound = mapUsersWeeks.find(psi)!=mapUsersWeeks.end();
			int Lines = mapUsersWeeks[psi];
//cout << "found: " << bFound << " pair: " << (*it).first << " week: " << i << " lines: " << Lines << endl;			
			cout << Lines << "\t";
		}
		cout << endl;
	}
}
