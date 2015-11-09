/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/MySQLHelper_moved.h"
#include "DCEConfig.h"
#include "Logger.h"

//MySqlHelper g_MySqlHelper("192.168.80.1","root","","mantis");
MySqlHelper g_MySqlHelper("localhost","root","moscow70bogata","mantis");
map<int,bool> g_mapBugs;

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "../version.h"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;



void AssignWorkDays(string sUserID);
void AssignWorkDaysForUser(string sUserID);
int GetDuration(string sTaskID);
time_t GetEndTime(string sTaskID,time_t tStartTime);
time_t AssignTaskIfFits( string sTaskID, time_t tStartTime, time_t tEndTime );
void AssignTasksFollowingID(time_t tStartTime,time_t tEndTime,list<int> &listNextTasks);
void AssignTaskByDate(string sFirstTask,string sUserID,time_t tFirstDate,time_t tEndTime,list<int> &listNextTasks);
time_t GetActualStartTimeAndWorkHours(string sUser,time_t tStartTime,int &Hours);
string GetUserForTask(string sTaskID);
void RecurseAllFollowingTasks( string sTaskID, string sUserID,list<int> &listNextTasks );
void Report(string sUserID,string sStartDate,string EndDate);
void OutputTask(time_t time,int MantisID,int Severity,int Status,int Resolution,string summary,int hours_estimate,int hours_actual,time_t DateTodo,int ID_After_Todo);
void WriteDay(string sDate);
void OutputUser(string sUser,int iHourStart,int iHourStop,string sComment);

int main(int argc, char *argv[])
{
	LoggerWrapper::SetType(LT_LOGGER_FILE,"MantisAssign.log");

	if( argc!=5 )
	{
		cout << "Usage: MantisAssign [assign or report or both] [user id or 'all'] [start date] [end date]" << endl
			<< "dates are: yyyy-mm-dd, and are ignored if only assign is specified" << endl;
		exit(1);
	}

	bool bAssign = string(argv[1])=="assign";
	string sUserID = StringUtils::ToUpper(argv[2]);
	string sStartDate = argv[3];
	string sEndDate = argv[4];

	if( string(argv[1])=="assign" || string(argv[1])=="both" )
		AssignWorkDays(sUserID);

	if( string(argv[1])=="report" || string(argv[1])=="both" )
		Report(sUserID,sStartDate,sEndDate);

	return 0;
}

void Report(string sUserID,string sStartDate,string EndDate)
{
	string sSQL = "select workday,user_id,hour_start,hour_stop,username,comment FROM work_day "
		" join mantis_user_table ON mantis_user_table.id=user_id "
		" where workday>='"
		+ sStartDate + "' and workday<='" + EndDate + "'";

	if( StringUtils::ToUpper(sUserID)!="ALL" )
		sSQL += " AND user_id in (" + sUserID + ")";

	sSQL += " ORDER BY workday,username";
	string sLastDate;

	PlutoSqlResult result_days;
	MYSQL_ROW row;
	if( ( result_days.r=g_MySqlHelper.mysql_query_result( sSQL ) ) )
		while( row=mysql_fetch_row( result_days.r ) )
		{
			if( sLastDate!=row[0] )
			{
				sLastDate=row[0];
				WriteDay(sLastDate);
			}

			OutputUser(row[4],row[2] ? atoi(row[2]) : 0,row[3] ? atoi(row[3]) : 0,row[5] ? row[5] : "");
			sUserID=row[1];

			sSQL = "select assigned_time,assigned_time.id,severity,status,resolution,summary,hours_estimate,hours_actual,date_todo,id_after_todo FROM assigned_time "
				"JOIN mantis_bug_table on assigned_time.id=mantis_bug_table.id "
				"WHERE handler_id=" + sUserID + " and date_format(assigned_time,'%Y-%m-%d')='" + sLastDate + "'"
				" ORDER BY assigned_time";

			PlutoSqlResult result;
			MYSQL_ROW row;
			if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) )
				while( row=mysql_fetch_row( result.r ) )
					OutputTask(StringUtils::SQLDateTime(string(row[0])),
						atoi(row[1]),atoi(row[2]),atoi(row[3]),atoi(row[4]),row[5],
						row[6] ? atoi(row[6]) : 0,row[7] ? atoi(row[7]) : 0,
						row[8] ? StringUtils::SQLDateTime(row[8]) : 0,row[9] ? atoi(row[9]) : 0);
		}
}

void OutputTask(time_t time,int MantisID,int Severity,int Status,int Resolution,string summary,int hours_estimate,int hours_actual,time_t DateTodo,int ID_After_Todo)
{
	struct tm *ptm = localtime(&time);

	string color="white";
	if( Status==90 )
		color="green";
	if( Status==80 )
		color="yellow";

	cout << "<p><div style=\"background-color: " << color << "\"><h3>" << ptm->tm_hour << ":" << ptm->tm_min << " <a href=\"http://plutohome.com/support/mantis/view.php?id=" << MantisID << "\">" << summary << "</a></div></h3>"
		<< endl << "<br>hours estimated: " << hours_estimate << " actual: " << hours_actual << "  after id: " << ID_After_Todo;

	if( DateTodo && DateTodo>1 )
	{
		ptm = localtime(&DateTodo);
		cout << "  date: " << asctime(ptm);
	}

	cout << endl
		<< "<br>Severity: ";

	switch(Severity)
	{
	case 50:
		cout << "severe";
		break;
	default:
		cout << Severity;
	}

	cout << " Resolution: ";
	
	switch(Resolution)
	{
	case 10:
		cout << "severe";
		break;
	default:
		cout << Severity;
	}
	
	cout << " Status: ";
	
	switch(Status)
	{
	case 90:
		cout << "closed";
		break;
	case 80:
		cout << "resolved";
		break;
	default:
		cout << Status;
	}	
	cout << "</p>" << endl;
}

void OutputUser(string sUser,int iHourStart,int iHourStop,string sComment)
{
	cout << "<p><strong>" << sUser << "</strong> " << sComment << " hours: " << iHourStart << "-" << iHourStop << "</p>" << endl;
}

void WriteDay(string sDate)
{
	cout << "<p><h1>" << sDate << "<h1></p>" << endl;
}

void AssignWorkDays(string sUserID)
{
	string sSQL = "DELETE FROM assigned_time";
	g_MySqlHelper.threaded_mysql_query(sSQL);

	if( sUserID=="ALL" )
	{
		sSQL = "select distinct handler_id from mantis_bug_table";
		PlutoSqlResult result;
		MYSQL_ROW row;
		if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) )
			while( row=mysql_fetch_row( result.r ) )
				AssignWorkDaysForUser(row[0]);
	}
	else
		AssignWorkDaysForUser(sUserID);
}

void AssignWorkDaysForUser(string sUserID)
{
	list<int> listNextTasks; // This will be the list of tasks which need to be assigned because they follow
	// a task that was already assigned.  As they are assigned, 
	string sSQL = "select id,date_todo from mantis_bug_table where handler_id=" + sUserID + " and date_todo is not null and date_todo>0 order by date_todo";
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
	{
		string sFirstTask = row[0];
		string sFirstDate = row[1];

		while(true)
		{
			row=mysql_fetch_row( result.r );
			AssignTaskByDate(sFirstTask,sUserID,StringUtils::SQLDateTime(sFirstDate),row ? StringUtils::SQLDateTime(row[1]) : 0,listNextTasks); // row[1] is the stop date, ie the first date of the subsequent task
			if( !row )
				return;  // We've done all the tasks with a specific date in them
			sFirstTask = row[0];
			sFirstDate = row[1];
		}
	}
}

void AssignTaskByDate(string sFirstTask,string sUserID,time_t tFirstDate,time_t tEndTime,list<int> &listNextTasks)
{
	int Hours;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"AssignTaskByDate task %s user %s number %d",sFirstTask.c_str(),sUserID.c_str(),(int) listNextTasks.size());
	tFirstDate = GetActualStartTimeAndWorkHours(GetUserForTask(sFirstTask),tFirstDate,Hours);

	if( g_mapBugs[atoi(sFirstTask.c_str())] )
	{
		cout << "******ERROR*******  Bug " << sFirstTask << " assigned twice." << endl;
		exit(1);
	}

	g_mapBugs[atoi(sFirstTask.c_str())]=true;
	cout << "Bug " << sFirstTask << " assigned" << endl;
	
	g_mapBugs[atoi(sFirstTask.c_str())]=true;
	string sSQL = "INSERT INTO assigned_time(id,assigned_time) VALUES(" + sFirstTask + ",'" + StringUtils::SQLDateTime(tFirstDate) + "')";
	g_MySqlHelper.threaded_mysql_query(sSQL);

	// The next task will start after this one ends
	time_t tStartTime = GetEndTime(sFirstTask,tFirstDate);

	RecurseAllFollowingTasks( sFirstTask,sUserID,listNextTasks );
	AssignTasksFollowingID(tStartTime,tEndTime,listNextTasks);
}

void AssignTasksFollowingID(time_t tStartTime,time_t tEndTime,list<int> &listNextTasks)
{
	for(list<int>::iterator it=listNextTasks.begin();it!=listNextTasks.end();)
	{
		string sTask = StringUtils::itos(*it);

		int Hours;  // Get the correct start time
		tStartTime = GetActualStartTimeAndWorkHours(GetUserForTask(sTask),tStartTime,Hours);

		tStartTime = AssignTaskIfFits( sTask, tStartTime, tEndTime );
		if( tStartTime==0 )
			return;  // Doesn't fit
		listNextTasks.erase(it++);  // We already assigned it
	}
}

void RecurseAllFollowingTasks( string sTaskID, string sUserID, list<int> &listNextTasks )
{
    // Add in the tasks that should follow this one
	string sSQL = "SELECT id,handler_id from mantis_bug_table where id_after_todo=" + sTaskID;

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) )
		while ( row=mysql_fetch_row( result.r ) )
		{
			if( !row[1] || atoi(row[1])!=atoi(sUserID.c_str()) )
			{
				cout << "******ERROR*******  Bug " << row[0] << " follows a bug assigned to a different user than: " << sUserID << endl;
				exit(1);
			}
			listNextTasks.push_back(atoi(row[0]));
			RecurseAllFollowingTasks( row[0], sUserID, listNextTasks );
		}
}

time_t AssignTaskIfFits( string sTaskID, time_t tStartTime, time_t tEndTime )
{
	time_t tMyEndTime = GetEndTime(sTaskID,tStartTime);
	if( tEndTime && tMyEndTime>tEndTime )
		return 0;
	
	if( g_mapBugs[atoi(sTaskID.c_str())] )
	{
		cout << "******ERROR*******  Bug " << sTaskID << " assigned twice." << endl;
		return 0;
	}
	g_mapBugs[atoi(sTaskID.c_str())]=true;

	cout << "Bug " << sTaskID << " assigned" << endl;
if( sTaskID=="1978" )
int k=2;

	string sSQL = "INSERT INTO assigned_time(id,assigned_time) VALUES(" + sTaskID + ",'" + StringUtils::SQLDateTime(tStartTime) + "')";
	g_MySqlHelper.threaded_mysql_query(sSQL);

	return tMyEndTime;
}

time_t GetEndTime(string sTaskID,time_t tStartTime)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"GetEndTime %s",sTaskID.c_str());
	int Duration = GetDuration(sTaskID);
	if( Duration<1 )
		Duration=1;

	while(true)
	{
		int Hours;
		tStartTime = GetActualStartTimeAndWorkHours(GetUserForTask(sTaskID),tStartTime,Hours);
		time_t tStopTime = tStartTime + Hours * 3600;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"GetEndTime %s duration is %d  start %d  stop %d Hours %d",sTaskID.c_str(),Duration,(int) tStartTime, (int) tStopTime, Hours);
		if( (tStopTime - tStartTime)/3600 >= Duration )
			return tStartTime + Duration * 3600;
		Duration -= Hours; // Subtract the hours for this day, and continue to the next
		tStartTime = tStopTime;
	}
}

int GetDuration(string sTaskID)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"GetDuration %s",sTaskID.c_str());
	string sSQL = "select hours_estimate,hours_actual from mantis_bug_table where id=" + sTaskID;
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
	{
		if( row[1] && atoi(row[1]) )
			return atoi(row[1]);

		if( row[0] && atoi(row[0]) )
			return atoi(row[0]);
	}
	return 0;
}

time_t GetActualStartTimeAndWorkHours(string sUser,time_t tStartTime,int &Hours)
{
	// Find the first day that is after that time
	string sSQL = "select workday,hour_start,hour_stop from work_day where user_id=" + sUser +
		" and workday>='" + StringUtils::SQLDateTime(tStartTime) + "' order by workday";
	
	Hours=8;
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) )
	{
		while ( row=mysql_fetch_row( result.r ) )
		{
			if( !row[1] || !row[2] || !atoi(row[2]) || !atoi(row[2]) )
				continue; // This isn't a work day
			time_t tTime = StringUtils::SQLDateTime(row[0]);
			struct tm *ptm = localtime(&tTime);

			// See if we are already after the stop time
			ptm->tm_hour=atoi(row[2]);
			time_t tEndOfDay = mktime(ptm);
			if( tStartTime>=tEndOfDay )
				continue; // Go to the next day

			ptm->tm_hour=atoi(row[1]);
			time_t tStartOfDay = mktime(ptm);
			if( tStartTime>=tStartOfDay ) // The start time is during the day
			{
				Hours = (tEndOfDay-tStartTime)/3600;
				if( Hours<1 )
					Hours=8;
				return tStartTime;
			}
			Hours = atoi(row[2])-atoi(row[1]);
			if( Hours<1 )
				Hours=8;
			return tStartOfDay;
		}
	}
	if( Hours<1 )
		Hours=8;
	return tStartTime;
}

string GetUserForTask(string sTaskID)
{
	string sSQL = "select handler_id from mantis_bug_table where id=" + sTaskID;
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=g_MySqlHelper.mysql_query_result( sSQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
		return row[0];
	else
		return "";
}
