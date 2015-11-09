/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file PlutoLockLogger.h
 Header file for the PlutoLockLogger class.
 @todo notcommented
 */
#ifndef PlutoLockLogger_h
#define PlutoLockLogger_h

extern const char *LoggerFileName;
using namespace ::std;

#ifndef WIN32
#include <sys/time.h>
#endif

#if defined(WINCE) && !defined(SMARTPHONE2005) && !defined(_VC80_UPGRADE)//--- CHANGED4WM5 ----//
	#include _STLP_NATIVE_C_HEADER(time.h)
	#include "wince.h"
#elif defined(SMARTPHONE2005)
	#include <time.h>
#endif

#include "PlutoUtils/MultiThreadIncludes.h"

// We need a special safety lock here since we can't log to ourselves if we're in a deadlock
#ifdef DEBUG
	#define PLUTO_SAFETY_LOCK_LOGGER(x,y) PlutoLockLogger x(&y,m_dwPK_Installation,__FILE__,__LINE__);
#else
	#define PLUTO_SAFETY_LOCK_LOGGER(x,y) PlutoLockLogger x(&y);
#endif

namespace DCE
{
    /**
    @class PlutoLockLogger
    This class ???
     */
	class PlutoLockLogger
	{
	public:
		string m_sFileName;
		int m_Line;
		bool m_bReleased,m_bLog,m_bLogErrorsOnly,m_bGotLock;
		pluto_pthread_mutex_t *m_pLock;

	public:
		PlutoLockLogger(pluto_pthread_mutex_t *pLock)
		{
			m_bReleased=false;
			m_bGotLock=false;
			m_pLock=pLock;
			pthread_mutex_lock(&m_pLock->mutex);
			m_sFileName = "*NONE*";
			m_Line = 9999999;
		}


		PlutoLockLogger(pluto_pthread_mutex_t *pLock,int /*PK_Installation*/,string File,int Line)
		{
			m_bReleased=false;
			m_pLock=pLock;
			m_sFileName=File;
			m_Line=Line;
			m_bGotLock = false;

			// AB 3/5/2004 - For some reason trylock doesn't work under Windows CE.  Do normal thread
			// logging instead
	#ifdef UNDER_CE
			pthread_mutex_lock(&m_pLock->mutex);
			m_pLock->m_Line=m_Line;
			m_pLock->m_sFileName=m_sFileName;
			m_pLock->m_thread=pthread_self();
			return;
	#endif
			time_t t = time(NULL);

			while( t+5>time(NULL) && !m_bGotLock)
			{
				m_bGotLock = (pthread_mutex_trylock(&m_pLock->mutex)!=EBUSY);
				if( m_bGotLock )
					break;
				Sleep(10);
			}
			if( !m_bGotLock )
			{
				timeval tv;
				char Message[2000];
				// Under Linux pthread_self is an int, under Windows it's a pointer
	#if defined(WIN32) || defined(__APPLE_CC__)
				snprintf(Message,sizeof(Message),"Logger failed to get lock(%p): %s:%d thread: %p used by: %s:%d thread:%p got: %d\n",
					m_pLock, m_sFileName.c_str(),m_Line, pthread_self(),m_pLock->m_sFileName.c_str(),m_pLock->m_Line,
					m_pLock->m_thread,(m_bGotLock ? 1 : 0));
	#else
				snprintf(Message,sizeof(Message),"Logger failed to get lock(%p): %s:%d thread: %d used by: %s:%d thread:%d got: %d\n",
					m_pLock, m_sFileName.c_str(),m_Line, (int) pthread_self(),m_pLock->m_sFileName.c_str(),m_pLock->m_Line,
					(int) m_pLock->m_thread,(m_bGotLock ? 1 : 0));
	#endif

	#ifndef WIN32
				gettimeofday(&tv, NULL);
	#ifdef ERRORS2PLUTOHOME
				FILE* f = popen(( string("wget -q -O - \"http://www.plutohome.com/utils/message.php?FK_Installation=") +
					StringUtils::itos(PK_Installation) + "&Severity=3&Message=" + StringUtils::URLEncode(Message) + "\"").c_str(), "r");
				if(f != NULL)
					fclose(f);
	/// @TODO -- need to somehow do this in a separate thread and not inline -- the whole system dies while this is happening
	#endif

	#else
				SYSTEMTIME lt;
				::GetLocalTime(&lt);
				///@TODO Need to fill tv
				tv.tv_sec = (long) time(NULL);
				tv.tv_usec = lt.wMilliseconds * 1000;
	#endif
				FILE *file = fopen(LoggerFileName,"ab");
				if( file )
				{

					struct tm t;
#ifdef WIN32
					_localtime64_s(&t, (time_t *)&tv.tv_sec);
#else
					localtime_r((time_t *)&tv.tv_sec,&t);
#endif
					char c[50];
					double sec = (double)(tv.tv_usec/1E6) + t.tm_sec;
					snprintf(c,sizeof(c),"%02d/%02d/%02d %d:%02d:%06.3f",(int) t.tm_mon+1,(int) t.tm_mday,(int) t.tm_year-100,(int) t.tm_hour,(int) t.tm_min, sec);

					fprintf(file, "%02d\t%s\t%s\t", LV_CRITICAL,c,"Logger");
					fwrite(Message, strlen(Message), 1, file);
	#if defined(WIN32) || defined(__APPLE_CC__)
					fprintf(file, " <%p>\n",pthread_self());
	#else
					fprintf(file, " <%d>\n",(int) pthread_self());
	#endif
					fwrite("\n", 1, 1, file);
					fflush(file);
					fclose(file);
				}
#ifndef WIN32
				else
					system( (string("lsof >>/var/log/pluto/lsof_") + StringUtils::itos((int) time(NULL)) + ".log").c_str() );
#endif
				pthread_mutex_lock(&m_pLock->mutex);
			}
			m_pLock->m_Line=m_Line;
			m_pLock->m_sFileName=m_sFileName;
			m_pLock->m_thread=pthread_self();
		}
		~PlutoLockLogger()
		{
			Release();
		}
		void Release()
		{
			if( !m_bReleased )
				pthread_mutex_unlock(&m_pLock->mutex);
			m_bReleased=true;
		}

	};
}

#endif
