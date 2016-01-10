/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file Profiler.h
 Header file for ??? .
 */
#ifndef Profiler_h
#define Profiler_h

#include "DCE/Logger.h"

using namespace DCE;

#if !defined( WIN32 ) || !defined( DEBUG )

/** @class PlutoProfiler
 Class for Threading
 */
class PlutoProfiler
{
public:
	void Start(const char *pName) {}
	void Stop(const char *pName) {}
	void DumpResults() {}
	void Error(string sID,string sMessage) {}
	void ClearResults() { }
};
#else

typedef map<string, pair<LARGE_INTEGER,int> > MapProfiles;


/** @class PlutoProfiler
 Class for Threading
 */
class PlutoProfiler
{
public:
	MapProfiles m_MapProfiles;  // String = section name, LARGE_INTEGER = start time, int = cumulative time
	LARGE_INTEGER m_Freq;      // ticks per second

	PlutoProfiler()
	{
		QueryPerformanceFrequency(&m_Freq);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
	~PlutoProfiler()
	{
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}

	void Start(const char *pName)
	{
		LARGE_INTEGER tStart;
		QueryPerformanceCounter(&tStart);
		MapProfiles::iterator it = m_MapProfiles.find(pName);
		if (it == m_MapProfiles.end())
		{
#ifdef WIN32
			m_MapProfiles[pName] = make_pair (tStart, 0);
#else
			m_MapProfiles[pName] = make_pair<LARGE_INTEGER, int>(tStart, 0);
#endif
		}
		else if( it->second.first.QuadPart!=0 )
			Error(pName,"Already started");
		else
			it->second.first = tStart;
	}

	void Stop(const char *pName)
	{
		LARGE_INTEGER tStop;
		QueryPerformanceCounter(&tStop);

		MapProfiles::iterator it = m_MapProfiles.find(pName);
		if( it==m_MapProfiles.end() )
			Error(pName,"not started");
		else
		{
			LARGE_INTEGER diff;
			diff.QuadPart = ( (tStop.QuadPart-it->second.first.QuadPart) * 1000000 / m_Freq.QuadPart );
			it->second.second = it->second.second + (int) diff.QuadPart;
			it->second.first.QuadPart = 0;
		}
	}

	void DumpResults()
	{
		for(MapProfiles::iterator it = m_MapProfiles.begin();it != m_MapProfiles.end();++it)
			LoggerWrapper::GetInstance()->Write(LV_PROFILER,"PROFILE ID: %s used %d nanoseconds (ie %d seconds %d ms)",it->first.c_str(),it->second.second,it->second.second/1000000,it->second.second/1000);
	}

	void Error(string sID,string sMessage)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Profiling failed: %s %s",sID.c_str(),sMessage.c_str());
	}

	void ClearResults() { m_MapProfiles.clear(); }
};

#endif

extern PlutoProfiler *g_PlutoProfiler;

#endif
