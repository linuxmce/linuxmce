/*
    Logger

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU Lesser General Public License for more details.
*/

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

#include "Logger.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "Message.h"
#include "DCEConfig.h"

#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
#include "PlutoUtils/ConsoleColor.h"
#endif

// some environment specific stuff
#ifndef WIN32
    const char *LoggerFileName="/var/log/pluto/PlutoServer.Logger.log";
#else
    #ifdef UNDER_CE
        char *LoggerFileName="\\Storage Card\\PlutoServer.Logger.txt";
    #else
        const char *LoggerFileName="\\pluto\\PlutoServer.Logger.txt";
    #endif
#endif

using namespace ::std;
using namespace DCE;

Logger *LoggerWrapper::m_pPlutoLogger=NULL;
int LoggerWrapper::m_iType=LT_LOGGER_FILE;
string LoggerWrapper::m_sFilename;


#ifdef WINCE
    #include "wince.h"
#endif

#ifndef WINCE
	bool g_bFlushLog = getenv("ImmediatelyFlushLog") && atoi(getenv("ImmediatelyFlushLog"))==1;
#else
	bool g_bFlushLog = false;
#endif

#ifdef __APPLE_CC__
	#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
#endif

string DCE::g_sBinary,DCE::g_sBinaryPath;

Logger::Logger( const char* pcName ) : m_Lock( "logger" )
{
    if( pcName ) m_Name = pcName;

//  m_dwPK_Installation = atoi( StringUtils::get_pluto_parameter( "INSTALLATION" ).c_str() );

	m_bLogLevels=NULL;

    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_Lock.Init( &m_MutexAttr );

	ReloadLogLevels();
}

Logger::~Logger()
{
	delete m_bLogLevels;
	m_bLogLevels=NULL;
	m_iMaxLogLevel=0;
    pthread_mutex_destroy(&m_Lock.mutex);
    pthread_mutexattr_destroy(&m_MutexAttr);
}

void Logger::SetInstallation(int iInstallation)
{
    m_dwPK_Installation = iInstallation;
}

void Logger::SetName( const char* pcName )
{
    m_Name = pcName;
}

void Logger::LogAll()
{
    PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures
	delete m_bLogLevels;
	m_bLogLevels=NULL;
}

void Logger::ReloadLogLevels()
{
    PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures
	delete m_bLogLevels;
	m_bLogLevels=NULL;
	m_iMaxLogLevel=0;

	DCEConfig _DCEConfig;
	string sLevels = _DCEConfig.m_mapParameters_Find("LogLevels");
	g_bFlushLog = _DCEConfig.m_mapParameters_Find("ImmediatelyFlushLog")=="1";
	string::size_type pos=0;
	list<int> TempList;
	while(pos<sLevels.size())
	{
		int iLevel = atoi( StringUtils::Tokenize(sLevels,",",pos).c_str() );
		if( iLevel>0 )
		{
			if( iLevel>m_iMaxLogLevel )
				m_iMaxLogLevel = iLevel;
			TempList.push_back(iLevel);
		}
	}
	if( m_iMaxLogLevel<1 || TempList.empty() )
		return;

	m_bLogLevels = new bool[m_iMaxLogLevel+1];
	for(int i=0;i<=m_iMaxLogLevel;++i)
		m_bLogLevels[i]=false;

	string sLogs;
	for(list<int>::iterator it=TempList.begin();it!=TempList.end();++it)
	{
		m_bLogLevels[*it]=true;
		sLogs += StringUtils::itos(*it) + ",";
	}
}
                void LoggerWrapper::SetType(int Type,string sFilename)
                {
                        m_iType=Type;
                        m_sFilename=sFilename;
                        Logger *pPlutoLogger=m_pPlutoLogger;
                        m_pPlutoLogger=NULL;
                        delete pPlutoLogger;
}

void Logger::Write( int iLevel, const char *pcFormat, ... )
{
#ifdef ERROR_LOGGING_ONLY
	if( iLevel != LV_CRITICAL && iLevel != LV_WARNING )
		return;
#endif

	// See if we're not supposed to log this level
	if( m_bLogLevels && iLevel>=0 && (iLevel>m_iMaxLogLevel || m_bLogLevels[iLevel]==false) )
		return;

    timeval tv;
    gettimeofday( &tv, NULL );
    PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures

    static char s[1500];  // Max size of a log message
    memset(s, 0, sizeof(s));
    va_list argList;
    va_start( argList, pcFormat );
#ifdef WIN32
    _vsnprintf( s, sizeof(s), pcFormat, argList );
#else
    vsnprintf( s, sizeof(s), pcFormat, argList );
#endif
    va_end( argList );

    if( iLevel == LV_CRITICAL )
    {
#ifdef ERRORS2PLUTOHOME
#ifndef WIN32
        FILE* f = popen(( string("wget -q -O - \"http://www.plutohome.com/utils/message.php?FK_Installation=")
            + StringUtils::itos( m_dwPK_Installation ) + "&Severity=3&Message="
            + StringUtils::URLEncode(s) + "\"").c_str(), "r");

        if( f != NULL ) fclose(f);
        /** @todo -- need to somehow do this in a separate thread and not inline
        the whole system dies while this is happening */
#endif
#endif
    }

    string sData;
#ifndef ANDROID
    if( iLevel == LV_CRITICAL )
        sData = string( "\x1b[31;1m" ) + string(s) + string( "\x1b[0m" ); // setting the text color to red and back to black
    else if( iLevel == LV_WARNING )
        sData = string( "\x1b[33;1m" ) + string(s) + string( "\x1b[0m" ); // setting the text color to yellow and back to black
    else
#endif
        sData = s;

    Entry e( iLevel, tv, m_Name, sData, 0 ); // reating the entry

    WriteEntry( e ); // writting it
}

FileLogger::FileLogger( FILE* log )
{
    m_LogFile = log;
    m_bNeedToClose = false;
}

FileLogger::FileLogger( const char* file )
{
    m_LogFile = fopen(file, "a");
    m_bNeedToClose = true;
    if( m_LogFile == NULL )
    {
        m_bNeedToClose = false;
        m_LogFile = stderr;
    }
	else
		m_sFilename = file;
}

FileLogger::~FileLogger()
{
    if( m_bNeedToClose ) fclose( m_LogFile );
}

void FileLogger::SetColor( int iColor )
{
    // This takes an ANSI color value (the number before the m)
#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
    static int iBold = 0, iFgColor=7, iBgColor=0;
    int aiColortbl[] = { 0, 4, 2, 6, 1, 5, 3, 7 };

    if ( iColor==1 )
    {
        iBold = 1;
    }
    if ( iColor==0 )
    {
        iBold = 0;
        iFgColor = 7;
        iBgColor = 0;
    }
    if ( iColor >= 30 && iColor <= 37 )
    {
        iFgColor = aiColortbl[iColor-30];
    }
    if ( iColor >= 40 && iColor <= 47 )
    {
        iBgColor = aiColortbl[iColor-40];
    }
    setcolor( iFgColor + (iBold ? 8 : 0) + (iBgColor << 4) );
#endif
}

void FileLogger::ClearConsole()
{
#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
    clrscr();
#endif
}

void FileLogger::Rotate()
{
	PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures.

	if(!m_sFilename.empty() && m_LogFile != stderr)
	{
		//the file has already been renamed, but we still have it open
		//closing the old file (which has a new name at this point, from logrotate) and opening a new file (with the original name)
		fclose(m_LogFile);
		m_LogFile = fopen(m_sFilename.c_str(), "a");
	}
}

void FileLogger::WriteBlock( const char *pBlock, size_t sBlockLen )
{
    PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures

	fwrite( pBlock, 1, sBlockLen, m_LogFile );
#ifdef DEBUG
    fflush( m_LogFile );  // Try leaving this out of release builds to get faster disk performance.  For debug, we want to see the logs in real-time
#endif
}

void FileLogger::WriteEntry( Entry& Entry )
{
    PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures

    struct tm t;
#ifdef WIN32
	_localtime64_s(&t, (time_t *)&Entry.m_TimeStamp.tv_sec);
#else
	localtime_r((time_t *)&Entry.m_TimeStamp.tv_sec,&t);
#endif
	char acBuff[50];
    double dwSec = (double)(Entry.m_TimeStamp.tv_usec/1E6) + t.tm_sec;
    snprintf( acBuff, sizeof(acBuff), "%02d/%02d/%02d %d:%02d:%06.3f", (int)t.tm_mon + 1, (int)t.tm_mday, (int)t.tm_year - 100, (int)t.tm_hour, (int)t.tm_min, dwSec );

#ifdef LOG_MOUSE_CLICKS
    static time_t tLastTouch=0;
    if( Entry.m_iLevel == LV_MOUSE_CLICKS ) // print the interval passed since the last writting of the mouse clicks entry
    {
        // I use these to automate sending messages to a remote system
        // and don't want the time and date stuff
        if( tLastTouch == 0 )
        {
            tLastTouch = time( NULL );
            fprintf( m_LogFile, "%s", Entry.m_sName.c_str() );
        }
        else
        {
            fprintf( m_LogFile, "sleep %d\n%s", time(NULL) - tLastTouch, Entry.m_sName.c_str() );
            tLastTouch = time( NULL );
        }
    }
    else
#endif
        fprintf( m_LogFile, "%02d\t%s\t%s\t", Entry.m_iLevel, acBuff, Entry.m_sName.c_str() );

    char *pcLp = (char*)Entry.m_sData.c_str();
#ifdef WIN32
    if ( m_LogFile == stdout )
    {
        char *pc, *pcMp;
        while( pcLp && *pcLp && (pc = strstr( pcLp, "\x1b[" )) )
        {
            fwrite( pcLp, pc - pcLp, 1, m_LogFile );
            pc += 2;
            pcMp = strchr( pc, 'm' );
            if ( pcMp )
            {
                do
                {
                    SetColor( atoi(pc) );
                    pc = strchr( pc, ';' );
                } while( pc++ && *pc && pc < pcMp );
                pcLp = pcMp + 1;
            }
        }
    }
#endif
    fwrite( pcLp, strlen( pcLp ), 1, m_LogFile );
#ifdef DEBUG

    fprintf( m_LogFile, " <%p>\n", reinterpret_cast<void *>(pthread_self()) );

#else
    fwrite( "\n", 1, 1, m_LogFile );
#endif

#ifdef DEBUG
    fflush( m_LogFile );
#else
	if( g_bFlushLog )
	    fflush( m_LogFile );   // Try leaving this out of release builds to get faster disk performance.  For debug, we want to see the logs in real-time
#endif
}

#ifdef WIN32
void WinOrbiterLogger::WriteEntry( Entry& entry )
{
    PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures

	struct tm *t = localtime((time_t *)&entry.m_TimeStamp.tv_sec);
	char acBuff[50];
	double dwSec = (double)(entry.m_TimeStamp.tv_usec/1E6) + t->tm_sec;
	snprintf(
		acBuff,
		sizeof(acBuff), //"%02d/%02d/%02d %d:%02d:%06.3f",
		"%d:%02d:%02.3f",
		/*(int)t->tm_mon + 1, (int)t->tm_mday, (int)t->tm_year - 100,*/
		(int)t->tm_hour, (int)t->tm_min, dwSec );

    string str = entry.m_sData;
	if(entry.m_iLevel == LV_CRITICAL)
		str = "CRITICAL : " + str;
	else if(entry.m_iLevel == LV_WARNING)
		str = "WARNING : " + str;

	str = string(acBuff) + " " + str;

	StringUtils::Replace(&str, "\x1b[31;1m", "");
	StringUtils::Replace(&str, "\x1b[0m", "");
	StringUtils::Replace(&str, "\x1b[33;1m", "");
	StringUtils::Replace(&str, char(0x1B) + string("[33m"), "");

	string s = str + "\n";
	FILE* f = fopen("orbiter_logger.out", "a+");
	if( f )
	{
		fseek(f, 0, SEEK_END);
		fwrite(s.c_str(), s.length(), 1, f);
		fclose(f);
	}

    //TODO : use entry.m_iLevel to set a color for the output string

#ifdef WINCE
	////--- CHANGED4WM5 ----//
    const int MAX_STRING_LEN = 4096;
    wchar_t wTextBuffer[MAX_STRING_LEN];
    mbstowcs(wTextBuffer, str.c_str(), MAX_STRING_LEN);
    #define MESSAGE wTextBuffer
#else
    #define MESSAGE str.c_str()
#endif
	//--- CHANGED4WM5 ----//
    const int cTimeOutInterval = 50; //miliseconds
    DWORD Count;
    if(0 == ::SendMessageTimeout(m_hWndList, LB_ADDSTRING, 0L, (LPARAM)MESSAGE, SMTO_NORMAL, cTimeOutInterval, &Count))
        return; //send message timed out

    DWORD Result;
    ::SendMessageTimeout(m_hWndList, LB_SETTOPINDEX, Count - 1, 0L, SMTO_NORMAL, cTimeOutInterval, &Result);
}

#endif

#ifdef ANDROID
#include <QDebug>
void QtLogger::WriteEntry( Entry& entry )
{
    QString s;

    /*  Android/Qt provides their own timestamp
    struct tm t;
    localtime_r((time_t *)&entry.m_TimeStamp.tv_sec,&t);
    char acBuff[50];
    double dwSec = (double)(entry.m_TimeStamp.tv_usec/1E6) + t.tm_sec;
    snprintf( acBuff, sizeof(acBuff), "%02d/%02d/%02d %d:%02d:%06.3f", (int)t.tm_mon + 1, (int)t.tm_mday, (int)t.tm_year - 100, (int)t.tm_hour, (int)t.tm_min, dwSec );

    s += acBuff;*/
    s += entry.m_sName.c_str();
    s += entry.m_sData.c_str();

    if (entry.m_iLevel == LV_CRITICAL)
    {
      qCritical() << s;
    } else if (entry.m_iLevel == LV_WARNING)
    {
      qWarning() << s;
    } else {
      qDebug() << s;
    }
}

#endif
