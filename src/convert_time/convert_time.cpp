/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <math.h>

#include <iostream>
#include <strstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <time.h>
using namespace std;

void ReportHumanTime(const char *pTime)
{
	struct tm timeptr;
	char *pMonth,*pDay,*pHour,*pMinute,*pSecond,*pDST;
	if( (pMonth=strchr(pTime,'-'))==NULL ||
		(pDay=strchr(pMonth+1,'-'))==NULL ||
		(pHour=strchr(pDay+1,' '))==NULL ||
		(pMinute=strchr(pDay+1,':'))==NULL ||
		(pSecond=strchr(pMinute+1,':'))==NULL ||
		(pDST=strchr(pSecond+1,':'))==NULL )
	{
		cerr << "Invalid time.  Use quotes and: \"yyyy-mm-dd hh:mm:ss:[Y/N]\" format" << endl
			<< "where Y/N = Is Daylight savings time in effect" << endl;
		return;
	}

	*pMonth=*pDay=*pHour=*pMinute=*pSecond=0;
	timeptr.tm_year = atoi( pTime )-1900;
	timeptr.tm_mon = atoi( pMonth+1 )-1;
	timeptr.tm_mday = atoi( pDay+1 );
	timeptr.tm_hour = atoi( pHour+1 );
	timeptr.tm_min = atoi( pMinute+1 );
	timeptr.tm_sec = atoi( pSecond+1 );
	if( *(pDST+1)=='Y' || *(pDST+1)=='y' )
		timeptr.tm_isdst = 1;
	else if( *(pDST+1)=='N' || *(pDST+1)=='n' )
		timeptr.tm_isdst = 0;
	else
	{
		timeptr.tm_isdst = -1;
		cout << "Invalid DST.  Assuming rules for US" << endl;
	}

	time_t tGMT = mktime(&timeptr);
	cout << "PCTime is: " << tGMT << endl;
}

void WriteTime(struct tm *timeptr)
{
	cout << timeptr->tm_year+1900 << "-" << timeptr->tm_mon+1 <<
		"-" << timeptr->tm_mday << " " << timeptr->tm_hour << ":" <<
		timeptr->tm_min << ":" << timeptr->tm_sec << ":";
	if( timeptr->tm_isdst==1 )
		cout << "Y";
	else if( timeptr->tm_isdst==0 )
		cout << "N";
	else
		cout << "?";
	cout << endl;
}

void ReportPCTime(const char *pTime)
{
	time_t tGMT = atoi(pTime);
	struct tm *timeptr = gmtime(&tGMT);
	cout << "GMT: ";
	WriteTime(timeptr);

	timeptr = localtime(&tGMT);
	cout << "LOCAL: " ;
	WriteTime(timeptr);
}

int main(int argc, char *argv[])
{
	if (argc<2)
	{
		cout << "conver_time [Time as Long Int]|[Time as \"yyyy-mm-dd hh:mm:ss:[Y/N]\"]" << endl
			<< "where Y/N = Is Daylight savings time in effect" << endl
			<< "current time: " << time(NULL) << endl;
		exit(0);
	}

	if( strchr(argv[1],'-') )
		ReportHumanTime(argv[1]);
	else
		ReportPCTime(argv[1]);
	return 0;
}

