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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "EPG.h"

#include <iostream>
using namespace std;
using namespace DCE;
using namespace VDREPG;

static bool ChannelComparer(Channel *x, Channel *y)
{
	return x->m_ChannelID<y->m_ChannelID;
}

EPG::~EPG()
{
	for(list<Channel *>::iterator it=m_listChannel.begin();it!=m_listChannel.end();++it)
		delete *it;
	for(map<string,Program *>::iterator it=m_mapProgram.begin();it!=m_mapProgram.end();++it)
		delete it->second;
	for(map<int,Event *>::iterator it=m_mapEvent.begin();it!=m_mapEvent.end();++it)
		delete it->second;
}

bool EPG::ReadFromFile(string sFileEPG,string sFileChannels)
{
	size_t size,size_channels;
	char *pBuffer = FileUtils::ReadFileIntoBuffer(sFileEPG,size);
	char *pBuffer_Channels = FileUtils::ReadFileIntoBuffer(sFileChannels,size_channels);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PG::ReadFromFile %s=%d %s=%d bytes %p %p",sFileEPG.c_str(),(int) size,
		sFileChannels.c_str(),(int) size_channels,pBuffer,pBuffer_Channels);
	if( !pBuffer || !pBuffer_Channels )
	{
		delete pBuffer;  // in case we got one and not the other
		delete pBuffer_Channels;
		return false;
	}

	m_pChannel_Reading=NULL;
	m_pEvent_Reading=NULL;
	m_pProgram_Reading=NULL;

	char *pPtr=pBuffer,*pEnd=pBuffer+size,*pPtrLine=pBuffer;
	while( pPtr<pEnd )
	{
		while( pPtr<pEnd && *pPtr!='\n' )
			pPtr++;

		if( *pPtr=='\n' )
		{
			*pPtr=0; // Terminate the line
			ProcessLine(pPtrLine);
			pPtr++;
			pPtrLine=pPtr;
		}
	}
	delete[] pBuffer;

	map<string,int> mapChannelsFrequencies;
	pPtr = pBuffer_Channels,pPtrLine=pBuffer_Channels;
	char *pPtrColon1=NULL,*pPtrColon2=NULL,*pPtrColon3=NULL,*pPtrColon4=NULL,*pPtrColon5=NULL,*pPtrColon6=NULL,*pPtrColon7=NULL,*pPtrColon8=NULL,*pPtrColon9=NULL,*pPtrColon10=NULL,*pPtrColon11=NULL,*pPtrColon12=NULL;
	int counter=1;
	while( *pPtr )
	{
		if( *pPtr=='\n' || *pPtr==0 )  // We got the end of the line
		{
			// Process the line only if it doesn't start with a : and all our pointers are good
			if( *pPtrLine!=':' && pPtrColon12 )
			{
				//                                colons:     1     2    3      4     5   6       9   8 9    10 11   12
				// Channel.conf = RTL Television,RTL;RTL World:12187:hC34:S19.2E:27500:163:104=deu:105:0:12003:1:1089:0
				// epg data = S19.2E-1-1089-12003
				// it is colon3-10-11-9
				*pPtrColon4=*pPtrColon11=*pPtrColon12=*pPtrColon10=0; // Terminate
				string sFrequency = string(pPtrColon3+1) + "-" + string(pPtrColon10+1) + "-" + string(pPtrColon11+1) + "-" + string(pPtrColon9+1);
				mapChannelsFrequencies[sFrequency] = counter++;
			}
			pPtrColon1=pPtrColon2=pPtrColon3=pPtrColon4=pPtrColon5=pPtrColon6=pPtrColon7=pPtrColon8=pPtrColon9=pPtrColon10=pPtrColon11=pPtrColon12=NULL;
			pPtrLine = pPtr+1;
		}
		else if( *pPtr==':' && !pPtrColon12 )
		{
			if( pPtrColon11 )
				pPtrColon12 = pPtr;
			else if( pPtrColon10 )
				pPtrColon11 = pPtr;
			else if( pPtrColon9 )
				pPtrColon10 = pPtr;
			else if( pPtrColon8 )
				pPtrColon9 = pPtr;
			else if( pPtrColon7 )
				pPtrColon8 = pPtr;
			else if( pPtrColon6 )
				pPtrColon7 = pPtr;
			else if( pPtrColon5 )
				pPtrColon6 = pPtr;
			else if( pPtrColon4 )
				pPtrColon5 = pPtr;
			else if( pPtrColon3 )
				pPtrColon4 = pPtr;
			else if( pPtrColon2 )
				pPtrColon3 = pPtr;
			else if( pPtrColon1 )
				pPtrColon2 = pPtr;
			else
				pPtrColon1 = pPtr;
		}

		if( *pPtr==0 )
			break;

		pPtr++;
	}

	delete[] pBuffer_Channels;
	for(list<Channel *>::iterator it=m_listChannel.begin();it!=m_listChannel.end();++it)
	{
		Channel *pChannel = *it;
		pChannel->m_ChannelID = mapChannelsFrequencies[pChannel->m_sFrequency];
		if( !pChannel->m_ChannelID )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Channel %s isn't mapped to a number",pChannel->m_sFrequency.c_str());
		m_mapChannelNumber[pChannel->m_ChannelID]=pChannel;
	}

	m_listChannel.sort(ChannelComparer);
	Channel *pChannel_Prior = (*m_listChannel.rbegin());
	for(list<Channel *>::iterator it=m_listChannel.begin();it!=m_listChannel.end();++it)
	{
		if( !(*it)->m_ChannelID )
			continue; // Skip over the ones without a channel number

		m_vectChannel.push_back(*it);
		pChannel_Prior->m_pChannel_Next = *it;
		(*it)->m_pChannel_Prior=pChannel_Prior;
		pChannel_Prior = *it;
	}
	return true;
}

void EPG::ProcessLine(char *szLine)
{
	if( *szLine=='C' )
	{
		m_pChannel_Reading = new Channel(szLine+2);
		m_listChannel.push_back(m_pChannel_Reading);
		m_mapChannelName[m_pChannel_Reading->m_sChannelName]=m_pChannel_Reading;
		m_pEvent_Reading = NULL;
		m_pProgram_Reading = NULL;
	}
	else if( *szLine=='E' )
	{
		m_pEvent_Reading = new Event(szLine+2,m_pChannel_Reading);
		if( m_mapEvent.find(m_pEvent_Reading->m_EventID)!=m_mapEvent.end() )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Line %s has duplicate event number",szLine);
			m_pEvent_Reading->m_EventID = m_Event_DuplicateID++;
		}

		m_mapEvent[m_pEvent_Reading->m_EventID] = m_pEvent_Reading;
		m_pEvent_Reading->m_pProgram = m_pProgram_Reading;  // If the program comes after the event, this will be NULL and we'll catch it below
		if( !m_tTime_First || m_pEvent_Reading->m_tStartTime<m_tTime_First )
			m_tTime_First = m_pEvent_Reading->m_tStartTime;
		if( m_pEvent_Reading->m_tStopTime>m_tTime_Last )
			m_tTime_Last = m_pEvent_Reading->m_tStopTime;
		if( m_pChannel_Reading && (!m_pChannel_Reading->m_pEvent_First || m_pChannel_Reading->m_pEvent_First->m_tStartTime>m_pEvent_Reading->m_tStartTime) )
		{
			if( m_pChannel_Reading->m_pEvent_First )
				m_pChannel_Reading->m_pEvent_First->m_pEvent_Prior = m_pEvent_Reading;
			m_pEvent_Reading->m_pEvent_Next = m_pChannel_Reading->m_pEvent_First;
			m_pChannel_Reading->m_pEvent_First = m_pEvent_Reading;  // This is now the first, and will point to the last
			if( !m_pChannel_Reading->m_pEvent_Last )
				m_pChannel_Reading->m_pEvent_Last = m_pEvent_Reading;  // This was totally empty, so this is now the last too
			m_pEvent_Reading->m_pEvent_Prior = m_pChannel_Reading->m_pEvent_Last;
		}
		else if( m_pChannel_Reading->m_pEvent_Last->m_tStartTime < m_pEvent_Reading->m_tStartTime )
		{
			m_pEvent_Reading->m_pEvent_Prior = m_pChannel_Reading->m_pEvent_Last;
			m_pEvent_Reading->m_pEvent_Next = m_pChannel_Reading->m_pEvent_First;
			m_pChannel_Reading->m_pEvent_Last->m_pEvent_Next = m_pEvent_Reading;
			m_pChannel_Reading->m_pEvent_Last = m_pEvent_Reading;
		}
		else
		{
			// I don't think this should happen, but we're not reading events in order.  Insert this in the middle somewhere
			Event *pEvent_Next = m_pChannel_Reading->m_pEvent_First;
			while( pEvent_Next->m_tStartTime < m_pEvent_Reading->m_tStartTime )  // pEvent_Next will point to the show right after us
				pEvent_Next = pEvent_Next->m_pEvent_Next;
			m_pEvent_Reading->m_pEvent_Next = pEvent_Next;
			m_pEvent_Reading->m_pEvent_Prior = pEvent_Next->m_pEvent_Prior;
			m_pEvent_Reading->m_pEvent_Prior->m_pEvent_Next = m_pEvent_Reading;
			m_pEvent_Reading->m_pEvent_Prior->m_pEvent_Prior = m_pEvent_Reading;
		}
	}
	else if( *szLine=='T' )
	{
		m_pProgram_Reading = m_mapProgram_Find(szLine+2);
		if( !m_pProgram_Reading )
		{
			m_pProgram_Reading = new Program(szLine+2);
			m_mapProgram[ m_pProgram_Reading->m_sTitle ] = m_pProgram_Reading;
		}
		if( m_pEvent_Reading )
		{
			m_pEvent_Reading->m_pProgram = m_pProgram_Reading;  
			m_pProgram_Reading->m_listEvent.push_back(m_pEvent_Reading);
		}
	}
	else if( *szLine=='S' && m_pEvent_Reading )  // m_pEvent_Reading should always be non-null here
		m_pEvent_Reading->m_sDescription_Short = szLine+2;
	else if( *szLine=='D' && m_pEvent_Reading )  // m_pEvent_Reading should always be non-null here
		m_pEvent_Reading->m_sDescription_Long = szLine+2;
	else if( *szLine=='X' && m_pEvent_Reading )  // m_pEvent_Reading should always be non-null here
		m_pEvent_Reading->m_listStream.push_back( new Stream(szLine+2) );
	else if( *szLine=='e' ) 
	{
		m_pEvent_Reading = NULL;
		m_pProgram_Reading = NULL;
	}
	else if( *szLine=='c' )
	{
		if( !m_pChannel_Reading )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"got a c, but not reading a channel");
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Read channel %s with events from %d-%d",
				m_pChannel_Reading->m_sChannelName.c_str(),
				(m_pChannel_Reading->m_pEvent_First ? m_pChannel_Reading->m_pEvent_First->m_tStartTime : 0),
				(m_pChannel_Reading->m_pEvent_Last ? m_pChannel_Reading->m_pEvent_Last->m_tStopTime : 0));

		m_pChannel_Reading = NULL;
	}
}

void EPG::ReadLogos(string sPath)
{
	for(size_t s=0;s<m_vectChannel.size();++s)
	{
		Channel *pChannel = m_vectChannel[s];

#ifndef WIN32
		// VDR usually stores in mng files.  We want jpg's.
		// If there is an mng and no jpg, or the mng was modified after the jpg, do a convert
		time_t tModTime_mng=0,tModTime_jpg=0;
		struct stat64 dirEntryStat;
		if ( stat64( (sPath + "/" + pChannel->m_sChannelName + ".mng").c_str(), &dirEntryStat) == 0 )
			tModTime_mng = dirEntryStat.st_mtime;

		if ( stat64( (sPath + "/" + pChannel->m_sChannelName + ".jpg").c_str(), &dirEntryStat) == 0 )
			tModTime_jpg = dirEntryStat.st_mtime;

		if( tModTime_mng && tModTime_mng>tModTime_jpg )
		{
			// Either the jpg doesn't exist or it's older than the mng
			string sCmd = "convert \"" + sPath + "/" + pChannel->m_sChannelName + ".mng\" \"" + sPath + "/" + pChannel->m_sChannelName + ".jpg\"";
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Will convert %s",sCmd.c_str());
			system(sCmd.c_str());
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Will not convert %s %d %d",
				pChannel->m_sChannelName.c_str(),tModTime_mng,tModTime_jpg);
#endif

		size_t sImage=0;
		pChannel->m_pImage = FileUtils::ReadFileIntoBuffer(sPath + "/" + pChannel->m_sChannelName + ".jpg",
			pChannel->m_sizeImage);
	}
}

void EPG::ReadTimers(string sPath)
{
}

Event::Event(char *szLine,Channel *pChannel)
{
	m_pChannel=pChannel; m_pProgram=NULL; m_pEvent_Next=NULL; m_pEvent_Prior=NULL; 
	char *pDuration,*pTableID,*pStartTime;
	if( (pStartTime=strchr(szLine,' '))!=NULL && (pDuration=strchr(pStartTime+1,' '))!=NULL && (pTableID=strchr(pDuration+1,' '))!=NULL )
	{
		*pDuration=*pTableID=*pStartTime=0;
		pDuration++; pTableID++; pStartTime++;
		m_EventID = atoi(szLine);
		m_tStartTime = atoi(pStartTime);
		m_tStopTime = m_tStartTime + atoi(pDuration);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Event::Event EPG data is malformed");
		return;
	}
}

Event::~Event()
{
	for(list<Stream *>::iterator it=m_listStream.begin();it!=m_listStream.end();++it)
		delete *it;
}

Event *Event::ConfirmCurrentProgram()
{
	time_t tNow = time(NULL);
	if( tNow>=m_tStartTime && tNow<=m_tStopTime )
		return this;
	return m_pChannel->GetCurrentEvent();
}

string Event::GetShortShowtime()
{
	struct tm *tmptr = localtime(&m_tStartTime);
	return StringUtils::itos(tmptr->tm_mon) + "/" + StringUtils::itos(tmptr->tm_mday) + " " + 
		StringUtils::itos(tmptr->tm_hour) + ":" + (tmptr->tm_min<10 ? "0" : "") + StringUtils::itos(tmptr->tm_min);
}

Event *Program::GetNextEvent()
{
	time_t tNow = time(NULL);
	Event *pEvent = NULL;
	for(list<Event *>::iterator it=m_listEvent.begin();it!=m_listEvent.end();++it)
	{
		Event *pEvent_Test = *it;
		if( pEvent_Test->m_tStopTime>tNow &&
			(pEvent==NULL || pEvent->m_tStartTime>pEvent_Test->m_tStartTime) )
				pEvent = pEvent_Test;
	}
	return pEvent;
}

Channel::Channel(char *szLine)
{
	char *pSpace = strchr(szLine,' ');
	if( !pSpace )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Channel::Channel EPG data is malformed");
		return;
	}
	*pSpace=0;
	m_ChannelID = 0; // Must assign elsewhere
	m_sChannelName = pSpace+1;
	m_sFrequency = szLine;
	m_pEvent_First = NULL;
	m_pEvent_Last = NULL;
	m_pChannel_Prior = m_pChannel_Next = NULL;
	char *m_pImage=NULL;
	m_sizeImage=0;
}

Channel::~Channel()
{
}

Event *Channel::GetCurrentEvent()
{
	time_t t = time(NULL);
	Event *pEvent = m_pEvent_First;
	while(pEvent && pEvent->m_tStopTime<t && pEvent->m_pEvent_Next!=m_pEvent_First )
		pEvent = pEvent->m_pEvent_Next;
	if( !pEvent )
		return m_pEvent_Last;
	return pEvent;
}

string Event::GetProgram()
{ 
	return m_pProgram ? m_pProgram->m_sTitle : ""; 
}

