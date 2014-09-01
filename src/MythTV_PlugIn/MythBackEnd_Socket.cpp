 /**
 *
 * @file MythBackEnd_Socket.cpp
 * @brief source file for the MythTvStream, MythBackEnd_Socket classes
 *
 */

#include "MythBackEnd_Socket.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "MythTV_PlugIn.h"
#include "../Media_Plugin/Media_Plugin.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

MythBackEnd_Socket::MythBackEnd_Socket(MythTV_PlugIn *pMythTV_PlugIn,string sIPAddress)
{
	m_pSocketBuffer = new char[MYTH_SOCKET_BUFFER_SIZE];
	m_pMythTV_PlugIn = pMythTV_PlugIn;
	m_sIPAddress = sIPAddress;
	m_bConnected=false;
	m_pSocket = NULL;
	m_iCharsInBuffer=m_iSizeIncomingMessage=0;
}

MythBackEnd_Socket::~MythBackEnd_Socket()
{
	delete m_pSocket;
	delete[] m_pSocketBuffer;
}

bool MythBackEnd_Socket::SendMythString(string sValue,string *sResponse,string sContaining)
{
	if( InternalSendMythString(sValue,sResponse,sContaining) )
		return true;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythBackEnd_Socket::SendMythString trying to reconnect");

	bool bResult = Connect();
	if( bResult )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythBackEnd_Socket::SendMythString failed -- reconnected");
		return InternalSendMythString(sValue,sResponse,sContaining);
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString failed to reconnect");
	return false;
}

bool MythBackEnd_Socket::InternalSendMythString(string sValue,string *sResponse,string sContaining)
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	if( !m_bConnected || m_pSocket==NULL || m_pSocket->m_Socket == INVALID_SOCKET )
	{
		sSM.Release();
		if( !Connect() )
			return false;
		sSM.Relock();
	}

	// There might be stray stuff left in the buffer
	PurgeSocketBuffer();

	// Send our string
	char szSize[20];
	sprintf(szSize,"%d          ",sValue.size());
	szSize[8]=0;
	if( !m_pSocket->SendData(8,szSize) || !m_pSocket->SendData((int) sValue.size(),sValue.c_str()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket cannot send %s", sValue.c_str());
		DeleteSocket();
		return false;
	}

	// The user may not care about the response
	if( sResponse==NULL )
		return true;

	time_t tTimeout = time(NULL) + MYTH_RESPONSE_TIMEOUT;

	while(time(NULL)<tTimeout)
	{
		eReceiveCharResult _eReceiveCharResult = ReceiveChar();
		if( _eReceiveCharResult==eReceiveCharResult_String )
		{
			// We got a string.  If sContaining is empty, that's what we'll use, otherwise, use it only if it contains sContaining
			if( sContaining.empty()==true || m_sStringBuffer.find(sContaining)!=string::npos )
			{
				*sResponse = m_sStringBuffer;
				m_sStringBuffer="";
				return true;
			}
			else
				ProcessIncomingString();  // It's not what we're looking for, process it
		}
		else if( _eReceiveCharResult==eReceiveCharResult_Error )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString sent %s but error getting response", sValue.c_str());
			return false;
		}
		else if( _eReceiveCharResult==eReceiveCharResult_Nothing )
			Sleep(50);  // Don't hog the cpu.  There's nothing coming in, so give Myth a chance to reply
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythString sent %s but timeout getting response containing %s", 
		sValue.c_str(), sContaining.c_str());
	return false;
}

bool MythBackEnd_Socket::SendMythStringGetOk(string sValue)
{
	string sResponse;
	if( !SendMythString(sValue,&sResponse,"OK") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::SendMythStringGetOk couldn't send and get OK");
		DeleteSocket();
		return false;
	}

	return true;
}

bool MythBackEnd_Socket::Connect( )
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythBackEnd_Socket::Connect m_bConnected %d m_Socket %p",
		(int) m_bConnected, m_pSocket);

	if( m_bConnected && m_pSocket && m_pSocket->m_Socket != INVALID_SOCKET )
		return true;

	m_iSizeIncomingMessage=0;

	if( m_pSocket==NULL || m_pSocket->m_Socket == INVALID_SOCKET )
	{
		sSM.Release();
		MythBackEnd_Socket_Wrapper *pSocket = new MythBackEnd_Socket_Wrapper(this,m_sIPAddress + ":6543");
		if( pSocket->Connect(0,"")==false )
		{
			sSM.Relock();
			DeleteSocket();
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect failed m_bConnected=false");
			return false;
		}
		sSM.Relock();
		DeleteSocket();
		m_pSocket = pSocket;
	}

	m_bConnected=true;

	string sResponse;
	if( !InternalSendMythString("MYTH_PROTO_VERSION " + m_pMythTV_PlugIn->m_sMyth_Protocol + " " + m_pMythTV_PlugIn->m_sMyth_Token,&sResponse,"ACCEPT") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect couldn't send MYTH_PROTO_VERSION m_bConnected=false");
		DeleteSocket();
		return false;
	}

	sResponse="";
	if( !InternalSendMythString("ANN Playback mythtv_plugin 1",&sResponse,"OK") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Connect couldn't send ANN m_bConnected=false");
		DeleteSocket();
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythBackEnd_Socket::Connect m_bConnected=true");
	return true;
}

bool MythBackEnd_Socket::ProcessSocket()
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	eReceiveCharResult _eReceiveCharResult;
	
	// Process all incoming characters
	while( (_eReceiveCharResult=ReceiveChar())==eReceiveCharResult_Char );

	if( _eReceiveCharResult==eReceiveCharResult_String )
	{
		ProcessIncomingString();  // Process it
		return true;
	}
	else if( _eReceiveCharResult==eReceiveCharResult_Error )
	{
		delete m_pSocket;
		m_pSocket=NULL;
		bool bResult = Connect();
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::ProcessSocket error reconnect %d", (int) bResult);
	}
	return false;
}

void MythBackEnd_Socket::PurgeSocketBuffer()
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	if( !m_pSocket || m_pSocket->m_Socket == INVALID_SOCKET )
		return;

	while(true)
	{
		eReceiveCharResult _eReceiveCharResult = ReceiveChar();
		if( _eReceiveCharResult==eReceiveCharResult_String )
			ProcessIncomingString();
		else if( _eReceiveCharResult!=eReceiveCharResult_Char )
			return;
	}
}

void MythBackEnd_Socket::Close()
{
	PLUTO_SAFETY_LOCK(sSM,m_pMythTV_PlugIn->m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythBackEnd_Socket::Close");
	m_bConnected=false;
}

eReceiveCharResult MythBackEnd_Socket::ReceiveChar()
{
	// Don't bother locking the mutex, since it will always be locked by the calling app and this is looped a lot
	if( (!m_bConnected || m_pSocket==NULL || m_pSocket->m_Socket == INVALID_SOCKET) )  // Don't bother reconnecting at this low-level.  The high level will
		return eReceiveCharResult_Error;

	char pcBuf[1];
	if ( !m_pSocket->ReceiveData( 1, pcBuf, -2 ) )
		return eReceiveCharResult_Nothing;

	// Unless we're receiving a fixed size message, treat \n as the end of the message.
	// If we're only expecting 1 more character in the message, this is it
	if( (m_iSizeIncomingMessage==0 && pcBuf[0]=='\n') || m_iSizeIncomingMessage==1 )
	{
		if( m_iSizeIncomingMessage==1 )
		{
			m_pSocketBuffer[m_iCharsInBuffer++] = pcBuf[0];  // It's not a \n
			m_pSocketBuffer[m_iCharsInBuffer] = 0;
			m_sStringBuffer += m_pSocketBuffer;
			m_sStringBuffer = m_sStringBuffer.substr(8);  // Strip the initial size
		}
		else
		{
			m_pSocketBuffer[m_iCharsInBuffer] = 0;
			m_sStringBuffer += m_pSocketBuffer;
		}
		m_iCharsInBuffer=0;
		m_iSizeIncomingMessage=0;
		return eReceiveCharResult_String;
	}
	
	m_pSocketBuffer[m_iCharsInBuffer++] = pcBuf[0];
	if( m_iSizeIncomingMessage )  // We're expecting a fixed size message.  Decrement the amount we're still expecting
		m_iSizeIncomingMessage--;

	if( m_iCharsInBuffer+2>=MYTH_SOCKET_BUFFER_SIZE )  // Allow room for the final numm term, plus a last char on the next loop
	{
		m_pSocketBuffer[m_iCharsInBuffer] = 0;
		m_sStringBuffer += m_pSocketBuffer;
		m_iCharsInBuffer=0;
	}
	else if( m_iCharsInBuffer==8 && m_sStringBuffer.empty()==true )
	{
		// If we've received 8 characters, and nothing more, see if they're all numeric, and if so, this is a fixed-size message
		for(int i=0;i<8;++i)
		{
			if( m_pSocketBuffer[i]!=' ' && (m_pSocketBuffer[i]<'0' || m_pSocketBuffer[i]>'9') ) 
				return eReceiveCharResult_Char;
		}

		// It's a fixed size message
		m_pSocketBuffer[8]=0;
		m_iSizeIncomingMessage=atoi(m_pSocketBuffer);
	}
	return eReceiveCharResult_Char;
}

void MythBackEnd_Socket::ProcessIncomingString(string sResponse)
{
	if( sResponse.empty()==true )
	{
		sResponse=m_sStringBuffer;
		m_sStringBuffer="";
	}

	if( sResponse.substr(0,15)=="BACKEND_MESSAGE" )
	{
		if( sResponse.substr(20,15)=="SCHEDULE_CHANGE" )
			m_pMythTV_PlugIn->m_pAlarmManager->AddRelativeAlarm(0,m_pMythTV_PlugIn,CHECK_FOR_SCHEDULED_RECORDINGS,NULL);
		else if( sResponse.substr(20,14)=="DONE_RECORDING" )
			m_pMythTV_PlugIn->m_pAlarmManager->AddRelativeAlarm(0,m_pMythTV_PlugIn,CHECK_FOR_NEW_RECORDINGS,NULL);
	}
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"MythBackEnd_Socket::ProcessIncomingString %s", 
		sResponse.c_str());
}

void MythBackEnd_Socket_Wrapper::Close()
{ 
	m_pMythBackEnd_Socket->Close(); 
	ClientSocket::Close(); 
}

