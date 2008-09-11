/* 
	AlarmManager
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include <pthread.h>
#ifndef UNDER_CE
#include <errno.h>
#endif
#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#include "AlarmManager.h"

using namespace DCE;

void* AlarmThread(void* param)
{
	AlarmManager* am = (AlarmManager*)param;
	am->Run();

	return NULL;
}


AlarmManager::AlarmManager() : m_Mutex("alarm")
{
	m_LastID = 0;
	pthread_cond_init(&m_Cond, NULL);
	m_Mutex.Init(NULL,&m_Cond);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"AlarmManager::AlarmManager %p",this);
#endif
}

AlarmManager::~AlarmManager()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"AlarmManager::~AlarmManager start %p",this);
#endif
	Stop();
	
	pthread_mutex_destroy(&m_Mutex.mutex);
	pthread_cond_destroy(&m_Cond);

	AlarmIDMap::iterator i;
	for(i=m_AlarmIDMap.begin();i!=m_AlarmIDMap.end();i++)
		delete i->second;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"AlarmManager::~AlarmManager done %p",this);
#endif
}

/// Start up the internal processing thread
bool AlarmManager::Start(int threads)
{
	Stop();
	m_Run = true;
	pthread_t thread_id;
	for(int i=0;i<threads;i++)
	{
		if(pthread_create(&thread_id, NULL, AlarmThread, (void*)this))
			goto err;
		m_Threads.push_back(thread_id);
	}
	return true;
err:
	
		LoggerWrapper::GetInstance()->Write(LV_ALARM, "AlarmManager::Start - ran into error - stopping");
	Stop();
	return false;
}

/// Stop the internal processing thread
void AlarmManager::Stop()
{
	PLUTO_SAFETY_LOCK(mm,m_Mutex);
	m_Run = false;
	pthread_cond_broadcast(&m_Cond);
	mm.Release();

	size_t len = m_Threads.size();
	for(size_t i=0;i<len;i++)
		pthread_join(m_Threads[i], NULL);
		
	m_Threads.clear();
}

/// Loop forever processing alarms
void AlarmManager::Run()
{
	PLUTO_SAFETY_LOCK(mm,m_Mutex);
	mm.m_bIgnoreDeadlock=true;
	AlarmEntry* entry;
	while(m_Run)
	{
		// find an alarm that we aren't already waiting on
		SortedAlarmMap::iterator i;
		
		entry = NULL;
		for(i=m_SortedAlarms.begin();i!=m_SortedAlarms.end();i++)
		{
			if(i->second->waiting == false)
			{
				entry = i->second;
				break;
			}
		}
		//If we have alarms, wait until they expire, or just wait until we actually have some
		if(entry == NULL)
		{
			mm.CondWait();
		}
		else
		{
			entry->waiting = true;
			
			timespec timeout;
			timeout.tv_sec = (long)entry->when;
			timeout.tv_nsec = 0;
			int ret;
			ret = mm.TimedCondWait(timeout);
			if(ret == ETIMEDOUT)
			{	// Time to actually execute the event
				int id = entry->id;
				void* param = entry->param;
				mm.Release();
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_ALARM, "Calling callback for alarm %p id %d param=%p entry->when: %d time: %d",this,id,param,entry->when,time(NULL));
#endif
				entry->callback->AlarmCallback(id, param);
				mm.Relock();
				// remove it from the sorted list
				m_SortedAlarms.erase(entry->it);

				// and from the id map
				m_AlarmIDMap.erase(entry->alarm_dwID);

				delete entry;
			}
			else
			{	// Oops, we were notified of a change in the alarms
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_ALARM, "Alarm manager notified of a change");
#endif
				if(entry->deleted)
				{
					// remove it from the sorted list
					m_SortedAlarms.erase(entry->it);

					// and from the id map
					m_AlarmIDMap.erase(entry->alarm_dwID);
					delete entry;
				}
				else
					entry->waiting = false;
			}
		}
	}
}

/** Fire an alarm when the date/time is 'when'
 * @return An ID for this alarm or -1 if the add failed
 */
int AlarmManager::AddAbsoluteAlarm(time_t when, AlarmEvent* callback, int type, void* param)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AlarmManager::AddAbsoluteAlarm time %d type %d", (int) when, type);
#endif

	AlarmEntry* entry = new AlarmEntry;
	if(entry == NULL)
		return -1;
	
	PLUTO_SAFETY_LOCK(mm,m_Mutex);
	
	// fille the struct
	entry->alarm_dwID = m_LastID++;
	entry->when = when;
	entry->callback = callback;
	entry->id = type;
	entry->param = param;
	entry->waiting = false;
	entry->deleted = false;
	
	SortedAlarmMap::iterator iterator;

	iterator = m_SortedAlarms.insert(SortedAlarmMap::value_type(when, entry));
	entry->it = iterator;
	m_AlarmIDMap.insert(AlarmIDMap::value_type(entry->alarm_dwID, entry));

	//TODO: only release threads that end up being affected.
	pthread_cond_broadcast(&m_Cond);

	return entry->alarm_dwID;
}

/** Fire an alarm after 'delay' seconds
 * @return  An ID for this alarm or -1 if the add failed
 */
int AlarmManager::AddRelativeAlarm(long delay, AlarmEvent* callback, int type, void* param)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AlarmManager::AddRelativeAlarm current time %d delay %d type %d", (int) time(NULL), (int) delay, type);
#endif
	return AddAbsoluteAlarm(time(NULL)+delay, callback, type, param);
}

/** Find an alarm by it's type
 * @return The ID of the FIRST alarm to match 'type' or -1 if there is no match
 */
int AlarmManager::FindAlarmByType(int type)
{
	SortedAlarmMap::iterator i;

	int id = -1;
	
	PLUTO_SAFETY_LOCK(mm,m_Mutex);
	for(i=m_SortedAlarms.begin();i!=m_SortedAlarms.end();i++)
	{
		if(i->second->id == type)
		{
			id = i->second->alarm_dwID;
			break;
		}
	}
	
	return id;
}
/** Get an alarms info from it's ID
 * 
 * @return true if the ID exists, false on failure
 */
bool AlarmManager::GetAlarmInfo(int id, long* when, int* type, void** param)
{
	AlarmEntry* entry;
	
	PLUTO_SAFETY_LOCK(mm,m_Mutex);
	entry = m_AlarmIDMap[id];
	if(entry == NULL)
	{
		return false;
	}

	if(when)
		*when = (long) entry->when;
	if(type)
		*type = entry->id;
	if(param)
		*param = entry->param;

	return true;
}


/// Cancel an alarm
bool AlarmManager::CancelAlarm(int id)
{
#ifdef DEBUG
	
		LoggerWrapper::GetInstance()->Write(LV_ALARM, "Cancel Alarm: %d",id);
#endif
	AlarmEntry* entry;

	PLUTO_SAFETY_LOCK(mm,m_Mutex);

	entry = m_AlarmIDMap[id];
	if(entry == NULL)
	{
		return false;
	}

	if(entry->waiting)
		entry->deleted = true;
	else
	{
		// remove it from the sorted list
		m_SortedAlarms.erase(entry->it);

		// and from the id map
		m_AlarmIDMap.erase(id);

		delete entry;
	}
		
	pthread_cond_broadcast(&m_Cond);
	
	return true;
}
/// Cancel an alarm by type
bool AlarmManager::CancelAlarmByType(int type)
{
#ifdef DEBUG
	
		LoggerWrapper::GetInstance()->Write(LV_ALARM, "Cancel Alarm by type: %d",type);
#endif
	AlarmEntry* entry;

	PLUTO_SAFETY_LOCK(mm,m_Mutex);

	SortedAlarmMap::iterator i;
		
	entry = NULL;
	
	for(i=m_SortedAlarms.begin();i!=m_SortedAlarms.end();i++)
	{
		if(i->second->id == type)
		{
			entry = i->second;
			break;
		}
	}

	if(entry == NULL)
	{
		return false;
	}

	if(entry->waiting)
		entry->deleted = true;
	else
	{
		// remove it from the sorted list
		m_SortedAlarms.erase(entry->it);

		// and from the id map
		m_AlarmIDMap.erase(type);

		delete entry;
	}
		
	pthread_cond_broadcast(&m_Cond);
	
	return true;
}


void AlarmManager::Clear()
{
#ifdef DEBUG
	
		LoggerWrapper::GetInstance()->Write(LV_ALARM, "clear alarm");
#endif
	AlarmEntry* entry;
	
	PLUTO_SAFETY_LOCK(mm,m_Mutex);
	
	AlarmIDMap::iterator i;
	for(i=m_AlarmIDMap.begin();i!= m_AlarmIDMap.end();i++)
	{
		entry = i->second;
		if(entry && entry->waiting)
			entry->deleted = true;
		else
		{
			// remove it from the sorted list
			if(entry)
				m_SortedAlarms.erase(entry->it);

			// and from the id map
			m_AlarmIDMap.erase(i);

			delete entry;
		}
	}
		
	pthread_cond_broadcast(&m_Cond);
}

