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
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "VFD_LCD_Base.h"

void *VL_Thread(void *p)
{
	VFD_LCD_Base *pVFD_LCD_Base = (VFD_LCD_Base *) p;
	pVFD_LCD_Base->RunThread();
	pVFD_LCD_Base->m_bVL_ThreadRunning=false;
	return NULL;
}

VFD_LCD_Base::VFD_LCD_Base(int iNumColumns,int iNumLines,int iNumVisibleColumns) : m_VL_MessageMutex("m_VL_MessageMutex")
{
	m_iNumLines=iNumLines;
	m_iNumColumns=iNumColumns;
	m_iNumVisibleColumns=iNumVisibleColumns;
	m_bQuit_VL=m_bVL_ThreadRunning=false;
	m_pVFD_LCD_Message_new=NULL;
	m_pMenuStructure=NULL;
	m_pMenuNode_Current=NULL;

	pthread_cond_init(&m_VL_MessageCond, NULL);
	m_VL_MessageMutex.Init(NULL,&m_VL_MessageCond);

	m_bVL_ThreadRunning=true;
	if( pthread_create(&m_ptVL_Thread, NULL, VL_Thread, (void*)this) )
	{
		m_bVL_ThreadRunning=false;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VFD_LCD_Base::VFD_LCD_Base failed to create thread");
	}
}

VFD_LCD_Base::~VFD_LCD_Base()
{
	m_bQuit_VL=true;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VFD_LCD_Base::~VFD_LCD_Base waiting for thread to exit");
	while( m_bVL_ThreadRunning )
	{
		pthread_cond_broadcast(&m_VL_MessageCond);
		Sleep(100);
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VFD_LCD_Base::~VFD_LCD_Base joining thread");
	if( m_ptVL_Thread )
		pthread_join( m_ptVL_Thread, NULL );
	delete m_pMenuStructure;
	m_pMenuStructure=NULL;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VFD_LCD_Base::~VFD_LCD_Base done");
}

void VFD_LCD_Base::RunThread()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	int iSecondsToSleep=10;
	while( !m_bQuit_VL )
	{
		vl.TimedCondWait(iSecondsToSleep ? iSecondsToSleep : 10 ,0);
		iSecondsToSleep=UpdateDisplay();
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sleeping %d ",iSecondsToSleep);
	}
}

void VFD_LCD_Base::NewMessage(int iMessageType,string sName,string sMessage,int ExpiresSeconds)
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Message %d=%s",iMessageType,sMessage.c_str());
	MapMessages *pMapMessages = m_mapMessages_Get(iMessageType);

	if( sMessage.size() )
	{
		if( iMessageType==VL_MSGTYPE_NOW_PLAYING_MAIN || iMessageType==VL_MSGTYPE_NOW_PLAYING_SECTION ||
				iMessageType==VL_MSGTYPE_RIPPING_NAME )
		{
			string::size_type s=0;
			while( ( s=sMessage.find( '\n', s ) ) != string::npos ) // search for the first apperence of a char return
				sMessage[s++] = ' ';
		}

		MapMessages::iterator it=pMapMessages->find(sName);
		if( it!=pMapMessages->end() )
			delete it->second;
		VFD_LCD_Message *pVFD_LCD_Message = new VFD_LCD_Message(iMessageType,sMessage,ExpiresSeconds);
		m_pVFD_LCD_Message_new = pVFD_LCD_Message;
		(*pMapMessages)[sName]=pVFD_LCD_Message;
	}
	else
		pMapMessages->erase(sName);

	vl.Release();
	pthread_cond_broadcast(&m_VL_MessageCond);
}

int VFD_LCD_Base::UpdateDisplay()
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateDisp");
	if( m_pMenuNode_Current )
	{
		vector<string> str;
		m_pMenuNode_Current->Render(&str);
		DoUpdateDisplay(&str);
		return 60;
	}

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateDisp");
	if( m_pVFD_LCD_Message_new && (m_pVFD_LCD_Message_new->m_iMessageType==VL_MSGTYPE_RUNTIME_ERRORS || m_pVFD_LCD_Message_new->m_iMessageType==VL_MSGTYPE_RUNTIME_NOTICES ) )
	{
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateDisp 2");
		DisplayMessage(m_pVFD_LCD_Message_new);
		m_pVFD_LCD_Message_new=NULL;
		return 5;  // After 5 seconds we'll see if something else belongs on the display
	}
	int SecondsToRedisplay;
	if(	SecondsToRedisplay=DisplayErrorMessages() )
		return SecondsToRedisplay;
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateDisp 3");

	if(	SecondsToRedisplay=DisplayNoticesMessages() )
		return SecondsToRedisplay;
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateDisp 4");

	if(	SecondsToRedisplay=DisplayNowPlayingRippingMessages() )
		return SecondsToRedisplay;
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateDisp 5");

	if(	SecondsToRedisplay=DisplayStatusMessages() )
		return SecondsToRedisplay;
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DispDate");
	DisplayDate();
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DispDate done");
	return 60;
}

int VFD_LCD_Base::DisplayErrorMessages()
{
	return DisplayStandardMessages(VL_MSGTYPE_RUNTIME_ERRORS);
}


int VFD_LCD_Base::DisplayNoticesMessages()
{
	return DisplayStandardMessages(VL_MSGTYPE_RUNTIME_NOTICES);
}

int VFD_LCD_Base::DisplayNowPlayingRippingMessages()
{
	MapMessages *pMapMessages_NowPlaying = m_mapMessages_Find(VL_MSGTYPE_NOW_PLAYING_MAIN);
	MapMessages *pMapMessages_Ripping = m_mapMessages_Find(VL_MSGTYPE_RIPPING_NAME);

	vector<string> str;
	if( pMapMessages_NowPlaying && pMapMessages_NowPlaying->size() && 
		pMapMessages_Ripping && pMapMessages_Ripping->size() )
	{
		// We've got to split between now playing and ripping, can split between 2 lines
		// if we have them, otherwise we'll rotate
		if( m_iNumLines>1 )
		{
			int iLinesNP = m_iNumLines/2;
			GetNowPlaying(&str,iLinesNP);
			GetRipping(&str,m_iNumLines-iLinesNP);
		}
		else
		{
			if( m_mapLastMessageByType[VL_MSGTYPE_NOW_PLAYING_MAIN]=="NP" )
			{
				GetRipping(&str,m_iNumLines);
				m_mapLastMessageByType[VL_MSGTYPE_NOW_PLAYING_MAIN]="RIP";
			}
			else
			{
				GetNowPlaying(&str,m_iNumLines);
				m_mapLastMessageByType[VL_MSGTYPE_NOW_PLAYING_MAIN]="NP";
			}
		}
	}
	else if( pMapMessages_NowPlaying && pMapMessages_NowPlaying->size() )
		GetNowPlaying(&str,m_iNumLines);
	else if( pMapMessages_Ripping && pMapMessages_Ripping->size() )
		GetRipping(&str,m_iNumLines);

	if( str.size() )
	{
		DoUpdateDisplay(&str);
		return 3;
	}
	else
		return 0;
}

int VFD_LCD_Base::DisplayStatusMessages()
{
	return DisplayStandardMessages(VL_MSGTYPE_STATUS);
}

void VFD_LCD_Base::DisplayDate()
{
	vector<string> str;
	time_t t=time(NULL);
	struct tm *ptm = localtime(&t);

	char *Months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	string sDate = StringUtils::itos(ptm->tm_mday) + " " + Months[ptm->tm_mon] + " " + StringUtils::itos(ptm->tm_year+1900);
	string sTime = StringUtils::itos(ptm->tm_hour) + ":" + (ptm->tm_min<10 ? "0" : "") + StringUtils::itos(ptm->tm_min);
	if( m_iNumVisibleColumns-2 > sDate.size() )
		sDate = string("                                        ").substr(0,(m_iNumVisibleColumns-sDate.size())/2) + sDate;
	if( m_iNumVisibleColumns-2 > sTime.size() )
		sTime = string("                                        ").substr(0,(m_iNumVisibleColumns-sTime.size())/2) + sTime;
	str.push_back(sDate);
	str.push_back(sTime);
	
	DoUpdateDisplay(&str);
}

int VFD_LCD_Base::DisplayStandardMessages(int iMessageType)
{
	MapMessages *pMapMessages = m_mapMessages_Find(iMessageType);
	if( !pMapMessages || pMapMessages->size()==0 )
		return 0;

	VFD_LCD_Message *pVFD_LCD_Message=NULL;
	MapMessages::iterator it;
	if( pMapMessages->size()>1 )
	{
		string sLastMessage = m_mapLastMessageByType[iMessageType];
		for( it = pMapMessages->begin();it!=pMapMessages->end();++it )
		{
			if( it->first == sLastMessage )
				break;
		}
		if( it==pMapMessages->end() || ++it==pMapMessages->end() )
			it = pMapMessages->begin();
	}
	else
			it = pMapMessages->begin();

	pVFD_LCD_Message = it->second;
	m_mapLastMessageByType[iMessageType] = it->first;

	time_t tNow=time(NULL);
	if( pVFD_LCD_Message->m_tExpireTime && pVFD_LCD_Message->m_tExpireTime<tNow )
	{
		delete pVFD_LCD_Message;
		pMapMessages->erase(it);
		return DisplayStandardMessages(iMessageType);
	}

	pVFD_LCD_Message->m_tDisplayTime=tNow;
	DisplayMessage(pVFD_LCD_Message);

	if( pVFD_LCD_Message->m_tExpireTime && pVFD_LCD_Message->m_tExpireTime-tNow<5 )
		return pVFD_LCD_Message->m_tExpireTime-tNow;
    return 5;
}


void VFD_LCD_Base::DisplayMessage(VFD_LCD_Message *pVFD_LCD_Message)
{
	vector<string> str;
	// See if we can fit this in the visible space
	StringUtils::BreakIntoLines(pVFD_LCD_Message->m_sMessage,&str,m_iNumVisibleColumns);

	if( str.size()>m_iNumLines && m_iNumColumns>m_iNumVisibleColumns )
	{
		str.clear();
		StringUtils::BreakIntoLines(pVFD_LCD_Message->m_sMessage,&str,m_iNumColumns);
	}

	DoUpdateDisplay(&str);
}

void VFD_LCD_Base::GetNowPlaying(vector<string> *vectString,int iNumLines)
{
	static int iLastNP=1;
	MapMessages *pMapMessages_NowPlaying = m_mapMessages_Find(VL_MSGTYPE_NOW_PLAYING_MAIN);
	MapMessages *pMapMessages_Section = m_mapMessages_Find(VL_MSGTYPE_NOW_PLAYING_SECTION);
	MapMessages *pMapMessages_TimeCode = m_mapMessages_Find(VL_MSGTYPE_NOW_PLAYING_TIME_CODE);
	MapMessages *pMapMessages_Speed = m_mapMessages_Find(VL_MSGTYPE_NOW_PLAYING_SPEED);

	if( pMapMessages_NowPlaying && pMapMessages_NowPlaying->size() && (iNumLines>1 || iLastNP==1) )
	{
		(*vectString).push_back( pMapMessages_NowPlaying->begin()->second->m_sMessage );
		iLastNP=2;
		if( iNumLines==1 )
			return; // Nothing more to display
	}
	iLastNP=1;

	if( pMapMessages_Section && pMapMessages_Section->size() )
	{
		if( ( (vectString->size() && iNumLines<3) || iNumLines==1 ) &&  // We're already displaying the title, if there's only 1 line left & a section/time code we have to split it
			( (pMapMessages_TimeCode && pMapMessages_TimeCode->size()) || (pMapMessages_Speed && pMapMessages_Speed->size()) )  )
		{
			string strTimeCode;
			if( pMapMessages_TimeCode && pMapMessages_TimeCode->size() )
				strTimeCode = pMapMessages_TimeCode->begin()->second->m_sMessage.substr(0,8);

			if( pMapMessages_Speed && pMapMessages_Speed->size() )
				strTimeCode += " " + pMapMessages_Speed->begin()->second->m_sMessage;

			int iSectionWidth=m_iNumVisibleColumns - strTimeCode.size() -1;
			string str = pMapMessages_Section->begin()->second->m_sMessage.substr(0,iSectionWidth);
			if( str.size()<iSectionWidth )
				str += string("                                  ").substr(0, iSectionWidth-str.size());
			string strSpeed;
			(*vectString).push_back(str + " " + strTimeCode);
			return; // Nothing more to display
		}
		vectString->push_back( pMapMessages_Section->begin()->second->m_sMessage );
	}
	if( (pMapMessages_TimeCode && pMapMessages_TimeCode->size()) || (pMapMessages_Speed && pMapMessages_Speed->size()) )
	{
		int WidthTimeCode = m_iNumVisibleColumns-4;
		string str;
		if( pMapMessages_TimeCode && pMapMessages_TimeCode->size() )
			str = pMapMessages_TimeCode->begin()->second->m_sMessage.substr(0,WidthTimeCode);
		if( str.size()<WidthTimeCode )
			str += string("                                                                  ").substr(0, WidthTimeCode-str.size());
		if( pMapMessages_Speed && pMapMessages_Speed->size() )
			str += pMapMessages_Speed->begin()->second->m_sMessage.substr(0,4);
		(*vectString).push_back(str);
	}
}

void VFD_LCD_Base::GetRipping(vector<string> *vectString,int iNumLines)
{
	static int iLastRip=1;
	MapMessages *pMapMessages_RippingName = m_mapMessages_Find(VL_MSGTYPE_RIPPING_NAME);
	MapMessages *pMapMessages_RippingProgress = m_mapMessages_Find(VL_MSGTYPE_RIPPING_PROGRESS);
	
	if( iNumLines==1 && pMapMessages_RippingName && pMapMessages_RippingName->size() &&
		pMapMessages_RippingProgress && pMapMessages_RippingProgress->size() )
	{
		if( iLastRip==1 )
			(*vectString).push_back( pMapMessages_RippingName->begin()->second->m_sMessage );
		else
			(*vectString).push_back( pMapMessages_RippingProgress->begin()->second->m_sMessage );
		iLastRip = !iLastRip;
		return;
	}

	if( pMapMessages_RippingName && pMapMessages_RippingName->size() )
		(*vectString).push_back( pMapMessages_RippingName->begin()->second->m_sMessage );
	if( pMapMessages_RippingProgress && pMapMessages_RippingProgress->size() )
		(*vectString).push_back( pMapMessages_RippingProgress->begin()->second->m_sMessage );
}

void VFD_LCD_Base::Up()
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"UP");
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;
	m_pMenuNode_Current->Up();
}

void VFD_LCD_Base::Down()
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"dOWN");
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;
	m_pMenuNode_Current->Down();
}

void VFD_LCD_Base::Left()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;
	m_pMenuNode_Current->Left();
}

void VFD_LCD_Base::Right()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;
	m_pMenuNode_Current->Right();
}

void VFD_LCD_Base::Enter()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;
	m_pMenuNode_Current->Enter();
}

void VFD_LCD_Base::Back()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;
}

void VFD_LCD_Base::Home()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	if( CheckActivateMenu() || !m_pMenuNode_Current )
		return;

	if( m_pMenuStructure->m_pMenuNode==m_pMenuNode_Current )
		SetMenuNode_Current(NULL);  // Already at the top, exit the menu
	else
		SetMenuNode_Current(m_pMenuStructure->m_pMenuNode);
	pthread_cond_broadcast(&m_VL_MessageCond);
}

bool VFD_LCD_Base::CheckActivateMenu()
{
	if( !m_pMenuNode_Current && m_pMenuStructure && m_pMenuStructure->m_pMenuNode )
	{
		SetMenuNode_Current(m_pMenuStructure->m_pMenuNode);
		pthread_cond_broadcast(&m_VL_MessageCond);
		return true;
	}
	return false;
}

void VFD_LCD_Base::WakeUpThread()
{
	pthread_cond_broadcast(&m_VL_MessageCond);
}

