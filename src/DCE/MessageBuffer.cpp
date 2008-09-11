/*
	MessageBuffer

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Logger.h"
#include "MessageBuffer.h"
#include "Command_Impl.h"
#include "pluto_main/Define_Command.h"

using namespace DCE;

/**
 * @brief entry point for the MessageBuffer thread
 */
void* MessageBufferThread( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	MessageBuffer* p = (MessageBuffer*)param;
	if( p->m_bThreadRunning )  // The constructor should have set this to true
		p->Run();
	p->m_bThreadRunning=false;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Exiting MessageBufferThread thread...");

	return NULL;
}

MessageBuffer::MessageBuffer(class Command_Impl *pCommand_Impl)
	: m_MessageBufferMutex("message buffer")
{
	m_pCommand_Impl=pCommand_Impl;
	pthread_cond_init( &m_MessageBufferCond, NULL );
	m_MessageBufferMutex.Init( NULL, &m_MessageBufferCond );

	pthread_t pt;
	m_bThreadRunning=true;
	if(pthread_create( &pt, NULL, MessageBufferThread, (void*)this) )
	{
		m_bThreadRunning=false;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create message processing queue" );
	}
	m_pMessage_In_Process=NULL;
}

bool MessageBuffer::BufferMessage(Message *pMessage)
{
	if( pMessage->m_eExpectedResponse!=ER_None )
		return false;

	PLUTO_SAFETY_LOCK(mb,m_MessageBufferMutex);

	for(list<Message *>::iterator it=m_listMessage.begin();it!=m_listMessage.end();++it)
	{
		Message *pMessageInQueue = *it;
		if( MessagesConflict(pMessageInQueue,pMessage) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MessageBuffer::BufferMessage dropping message %p",pMessageInQueue);
			m_listMessage.erase(it);
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MessageBuffer::BufferMessage adding to queue message %p",pMessage);

	m_listMessage.push_back(pMessage);
	pthread_cond_broadcast( &m_MessageBufferCond );
	return false;
}

void MessageBuffer::Run()
{
	while( !m_pCommand_Impl->m_bQuit_get() )
	{
		PLUTO_SAFETY_LOCK(mb,m_MessageBufferMutex);
		if( m_listMessage.size()==0 )
			mb.CondWait();

		// Now we have the lock
		if( m_listMessage.size() )
		{
			m_pMessage_In_Process = m_listMessage.front();
			m_listMessage.pop_front();
			mb.Release();
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MessageBuffer::Run processing message %p",m_pMessage_In_Process);
			m_pMessage_In_Process->m_bCanBuffer = false;
			m_pCommand_Impl->ReceivedMessage(m_pMessage_In_Process);
			delete m_pMessage_In_Process; // Since it wasn't auto-deleted like non-buffered messages
			m_pMessage_In_Process=NULL;
			mb.Relock();
		}
	}
	m_bThreadRunning=false;
}

bool MessageBuffer::MessagesConflict(Message *pMessage1,Message *pMessage2)
{
	if( pMessage1->m_dwPK_Device_To!=pMessage2->m_dwPK_Device_To || pMessage1->m_dwMessage_Type!=MESSAGETYPE_COMMAND || pMessage2->m_dwMessage_Type!=MESSAGETYPE_COMMAND )
		return false;

	if( pMessage1->m_dwID==pMessage2->m_dwID )
		return true;

	if( pMessage1->m_dwID!=COMMAND_Generic_On_CONST &&
		pMessage1->m_dwID!=COMMAND_Generic_Off_CONST &&
		pMessage1->m_dwID!=COMMAND_Set_Level_CONST &&
		pMessage1->m_dwID!=COMMAND_Set_Volume_CONST )
			return false;

	if( pMessage2->m_dwID!=COMMAND_Generic_On_CONST &&
		pMessage2->m_dwID!=COMMAND_Generic_Off_CONST &&
		pMessage2->m_dwID!=COMMAND_Set_Level_CONST &&
		pMessage2->m_dwID!=COMMAND_Set_Volume_CONST )
			return false;

	return true;
}

