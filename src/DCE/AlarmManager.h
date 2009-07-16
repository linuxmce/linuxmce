/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file AlarmManager.h
 *@brief The Alarm Manager.
 */
#ifndef __ALARM_MANAGER_H__87f105
#define __ALARM_MANAGER_H__87f105

#include <pthread.h>

#include <map>
#include <vector>

namespace DCE
{


/**
@class AlarmEvent
This class is Called by AlarmManager when an alarm expires.
*/
	class AlarmEvent
	{
	public:
		/// Called by AlarmManager when an alarm expires
		virtual void AlarmCallback(int id, void* param) = 0;
		virtual ~AlarmEvent() {};
	};


/**
@class AlarmManager
This class manages a list of alarms.
*/
	class AlarmManager
	{
	public:
		AlarmManager();
		virtual ~AlarmManager();

		/// Start up the internal processing threads
		bool Start( int threads=2);

		/// Stop the internal processing thread
		void Stop();

		/// Loop forever processing alarms
		void Run();

		/** Fire an alarm when the date/time is 'when'
		* @return An ID for this alarm or -1 if the add failed
		*/
		int AddAbsoluteAlarm(time_t when, AlarmEvent* callback, int type, void* param);

		/** Fire an alarm after 'delay' seconds
		* @return  An ID for this alarm or -1 if the add failed
		*/
		int AddRelativeAlarm(long delay, AlarmEvent* callback, int type, void* param);

		/** Find an alarm by it's type
		* @return The ID of the FIRST alarm to match 'type' or -1 if there is no match
		*/
		int FindAlarmByType(int type);

		/** Get an alarms info from it's ID
		*
		* @return true if the ID exists, false on failure
		*/
		bool GetAlarmInfo(int id, long* when=NULL, int* type=NULL, void** param=NULL);

		/// Cancel an alarm
		bool CancelAlarm(int id);

		/// Cancel an alarm by type
		bool CancelAlarmByType(int type);

		/// Remove all alarms
		void Clear();

		struct AlarmEntry;
		typedef ::std::multimap<time_t, AlarmEntry*> SortedAlarmMap;
		typedef ::std::map<int, AlarmEntry*> AlarmIDMap;

		pluto_pthread_mutex_t *m_Mutex_get() { return &m_Mutex; }
		SortedAlarmMap *m_SortedAlarms_get() { return &m_SortedAlarms; }

		struct AlarmEntry
		{
			int alarm_dwID;
			time_t when;
			AlarmEvent* callback;
			int id;
			void* param;
			SortedAlarmMap::iterator it;
			bool waiting;
			bool deleted;
		};

	protected:
		SortedAlarmMap m_SortedAlarms;
		AlarmIDMap m_AlarmIDMap;

		pluto_pthread_mutex_t m_Mutex;

		::std::vector<pthread_t> m_Threads;
		pthread_cond_t m_Cond;

		int m_LastID;
		bool m_Run;
	};
}
#endif
