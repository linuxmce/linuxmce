/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"

#include "TimedEvent.h"

#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_CriteriaParm.h"
#include "pluto_main/Table_CriteriaParmNesting.h"
#include "pluto_main/Table_CriteriaParmList.h"
#include "pluto_main/Table_EventHandler.h"

TimedEvent::TimedEvent(Row_EventHandler *pRow_EventHandler, class Criteria *pCriteria)
{
	m_tTime=0;
	m_pRow_EventHandler=pRow_EventHandler;
	m_iTimedEventType=m_pRow_EventHandler->TimedEvent_get();
	m_pCriteria = pCriteria;
	m_pCommandGroup=NULL;
	vector<Row_CriteriaParm *> vectRow_CriteriaParm;
	pRow_EventHandler->FK_Criteria_getrow()->FK_CriteriaParmNesting_getrow()->CriteriaParm_FK_CriteriaParmNesting_getrows(&vectRow_CriteriaParm);
	for(size_t s=0;s<vectRow_CriteriaParm.size();++s)
	{
		Row_CriteriaParm *pRow_CriteriaParm = vectRow_CriteriaParm[s];
		if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Day_Of_Week_CONST )
			m_sDaysOfWeek=pRow_CriteriaParm->Value_get();
		else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Time_of_day_CONST )
			m_sTimes=StringUtils::ToUpper(pRow_CriteriaParm->Value_get());
		else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Day_Of_Month_CONST )
			m_sDaysOfMonth=pRow_CriteriaParm->Value_get();
		else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Specific_Date_CONST )
			m_tTime=StringUtils::SQLDateTime(pRow_CriteriaParm->Value_get());
	}

	CalcNextTime();
}

void TimedEvent::CalcNextTime()
{
	switch(m_iTimedEventType)
	{
	case INTERVAL_EVENT:
		{
			if( m_tTime && m_tTime>time(NULL) )
				return; // It's valid, it just hasn't been tripped yet
			int i = atoi(m_sTimes.c_str());
			if( i )
			{
				time_t tCurrent = m_tTime==0 ? time(NULL) : m_tTime;  // Relative to the current time if we've not set it before
				if( m_sTimes.find('H')!=string::npos )
					m_tTime = tCurrent + i * 3600; // 3600 seconds per hour
				else
					m_tTime = tCurrent + i * 60; // 60 seconds per minute
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Interval timer %s has no time: %s",
					m_pRow_EventHandler->Description_get().c_str(),m_sTimes.c_str());
				m_tTime=0;
				return;
			}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Added interval timer %s at %d now %d seconds %d",
	m_pRow_EventHandler->Description_get().c_str(),(int) m_tTime,(int) time(NULL), (int) m_tTime-time(NULL));
		}
		break;
	case DAY_OF_WEEK:
		{
			m_tTime=0;
			time_t t=time(NULL);
			struct tm tm_Now;
			localtime_r(&t,&tm_Now);
			int CurrentDow = tm_Now.tm_wday==0 ? 7 : tm_Now.tm_wday; // We use M=1, Sunday=7
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::CalcNextTime about to see if another time occurs today %d  after %d Checking %d/%d/%d", CurrentDow, t, tm_Now.tm_mday,tm_Now.tm_mon+1,tm_Now.tm_year);

			if( (m_sDaysOfWeek.length() && !m_sDaysOfWeek.find(StringUtils::itos(CurrentDow))==string::npos) ||
				!SetNextTime(&tm_Now,&tm_Now,m_sTimes) )
			{
				// Either the current day is not in the list, or there are no times in m_sTimes that are after tm_Now.
				// We need to find the next day that matches this
				int iDowFirst=8,iDowNext=8;
				
				string::size_type pos=0;
				while(pos<m_sDaysOfWeek.length())
				{
					int Dow = atoi( StringUtils::Tokenize(m_sDaysOfWeek,",",pos).c_str() );
					if( Dow>0 && Dow<iDowFirst )
						iDowFirst = Dow;
					if( Dow>0 && Dow>CurrentDow && Dow<iDowNext )
						iDowNext = Dow;
				}
				if( iDowNext<8 )
					tm_Now.tm_mday += iDowNext-CurrentDow;
				else if( iDowFirst<8 )
					tm_Now.tm_mday += (7-CurrentDow+iDowFirst);  // wrap around to the next week
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Day of week timer %s has no day of week: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sDaysOfWeek.c_str());
					return;
				}

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::CalcNextTime no other time today.  Checking %d %d/%d/%d", iDowNext, tm_Now.tm_mday,tm_Now.tm_mon+1,tm_Now.tm_year);
				if( !SetNextTime(NULL,&tm_Now,m_sTimes) )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Day of week timer %s has no next time: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sTimes.c_str());
					return;
				}
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::CalcNextTime no stage 2.  Checking %d %d/%d/%d", iDowNext, tm_Now.tm_mday,tm_Now.tm_mon+1,tm_Now.tm_year);
			}
			m_tTime = mktime(&tm_Now);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::CalcNextTime dow done using %d %d/%d/%d %d:%d:%d", 
					  m_tTime, tm_Now.tm_mday,tm_Now.tm_mon+1,tm_Now.tm_year,tm_Now.tm_hour,tm_Now.tm_min,tm_Now.tm_sec);
		}
		break;
	case DAY_OF_MONTH:
		{
			m_tTime=0;
			time_t t=time(NULL);
			struct tm tm_Now;
			localtime_r(&t,&tm_Now);
			if( (m_sDaysOfMonth.length() && !m_sDaysOfMonth.find(StringUtils::itos(tm_Now.tm_mday+1))) ||
				!SetNextTime(&tm_Now,&tm_Now,m_sTimes) )
			{
				// Either the current day is not in the list, or there are no times in m_sTimes that are after tm_Now.
				// We need to find the next day that matches this
				int iDomFirst=99,iDomNext=99;
				string::size_type pos=0;
				while(pos<m_sDaysOfMonth.length())
				{
					int Dom = atoi( StringUtils::Tokenize(m_sDaysOfMonth,",",pos).c_str() );
					if( Dom<iDomFirst )
						iDomFirst = Dom;
					if( Dom>tm_Now.tm_mday+1 && Dom<iDomNext )
						iDomNext = Dom;
				}
				if( iDomNext<99 )
					tm_Now.tm_mday = iDomNext;
				else if( iDomFirst<99 )
				{
					tm_Now.tm_mday = iDomFirst;
					tm_Now.tm_mon++;
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Day of week timer %s has no day of week: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sDaysOfMonth.c_str());
					return;
				}

				if( !SetNextTime(NULL,&tm_Now,m_sTimes) )  // First time that day
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Day of week timer %s has no next time: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sTimes.c_str());
					return;
				}
			}
			m_tTime = mktime(&tm_Now);
		}
		break;
	case ABSOLUTE_TIME:
		break;
	}
	if( m_tTime && m_tTime < time(NULL) && m_iTimedEventType!=ABSOLUTE_TIME )
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::CalcNextTime Adjusting relative time because %d < %d ", m_tTime, time(NULL));
		m_tTime = time(NULL);
	}
struct tm tmLocal;
localtime_r(&m_tTime,&tmLocal);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::CalcNextTime Timer: %s set for %d/%d/%d %d:%d:%d in %d seconds",
	m_pRow_EventHandler->Description_get().c_str(),tmLocal.tm_mon+1,tmLocal.tm_mday,tmLocal.tm_year-100,tmLocal.tm_hour,tmLocal.tm_min,tmLocal.tm_sec,m_tTime - time(NULL));
}

class TimeOfDay
{
public:
	int m_H,m_M,m_S;
	TimeOfDay() { m_H=m_M=m_S=-1; };
	TimeOfDay(string sTime) 
	{
		string::size_type pos=0;
		m_H = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());
		m_M = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());
		m_S = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());

		if( sTime.find('p')!=string::npos || sTime.find('P')!=string::npos )
		{
			m_H += 12;
			if( m_H==24 )
				m_H=0;
		}
	};

	bool operator < (TimeOfDay &time1) 
	{ 
		if( time1.m_H!=m_H )
			return m_H<time1.m_H;
		if( time1.m_M!=m_M )
			return m_M<time1.m_M;
		return m_S<time1.m_S;
	}
	bool operator >= (struct tm *tm) 
	{
		if( tm->tm_hour!=m_H )
			return m_H>=tm->tm_hour;
		if( tm->tm_min!=m_M )
			return m_M>=tm->tm_min;
		return m_S>=tm->tm_hour;
	};
};

// Given a comma delimited list of times, like 14:20, 13:22, etc., find the one that occurs next
bool TimedEvent::SetNextTime(tm *tmAfter,tm *tmOutput,string sTimes)
{
  	TimeOfDay tod;
LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::SetNextTime starting %d/%d/%d %d:%d:%d",
	tmOutput->tm_mday,tmOutput->tm_mon+1,tmOutput->tm_year,
	tmOutput->tm_hour,tmOutput->tm_min,tmOutput->tm_sec);

	string::size_type pos=0;
	while(pos<sTimes.length())
	{
		TimeOfDay tod2(StringUtils::Tokenize(sTimes,",",pos));
		if( (tod.m_H==-1 /* first iteration */ || tod2<tod /* found an earlier one */ ) && (tmAfter==NULL || tod2>=tmAfter /* must be after */) )
			tod = tod2;
	}

	if( tod.m_H==-1 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::SetNextTime didn't find a valid time");
		return false; // We didn't find a valid time
	}

LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::SetNextTime ready to assign %d/%d/%d %d:%d:%d",
	tmOutput->tm_mday,tmOutput->tm_mon+1,tmOutput->tm_year,
	tmOutput->tm_hour,tmOutput->tm_min,tmOutput->tm_sec);

	tmOutput->tm_hour = tod.m_H;
	tmOutput->tm_min = tod.m_M;
	tmOutput->tm_sec = tod.m_S;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"TimedEvent::SetNextTime returning %d/%d/%d %d:%d:%d",
		tmOutput->tm_mday,tmOutput->tm_mon+1,tmOutput->tm_year,
		tod.m_H,tod.m_M,tod.m_S);
	return true;
}
