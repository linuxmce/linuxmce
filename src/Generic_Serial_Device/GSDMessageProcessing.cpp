/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "GSDMessageProcessing.h"
#include "RubyIOManager.h"
#include "DCE/Logger.h"
namespace DCE {

bool GSDMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	Message* pmsg = &inrepl.getMessage();
	if( pmsg == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "GSDMessageTranslator::Translate : null message");
		return false;
	}
	
	inrepl.setImplemented( pmanager->getCodeSupplier().isCmdImplemented(pmsg->m_dwPK_Device_To, pmsg->m_dwID) );
	
	if( inrepl.isImplemented() )
	{
		AVMessageTranslator::SetDelays(inrepl);
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "GSDMessageTranslator isCmdImplemented = true");
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "GSDMessageTranslator isCmdImplemented = false");
	if(AVMessageTranslator::Translate(inrepl, outrepls)) {
//		MessageReplicator *new_msg=&outrepls.back();
//		Message* pmsg = &new_msg->getMessage();
//		RubyIOManager* pmanager = RubyIOManager::getInstance();		
//		LoggerWrapper::GetInstance()->Write(LV_STATUS, "GSDMessageTranslator after translate.");
//		if(	!pmanager->getCodeSupplier().isCmdImplemented(pmsg->m_dwPK_Device_To, pmsg->m_dwID))
//		{
//			outrepls.pop_back();		
//			outrepls.push_back(inrepl);
//			LoggerWrapper::GetInstance()->Write(LV_STATUS, "GSDMessageTranslator will try to send untranslated.");
//		}
		return true;
	} else {
		/*in all cases we must route messages to GSD devices*/
		outrepls.push_back(inrepl);
		return true;
	}
};

//will try this later...
//bool GSDMessageTranslator::ProcessMessage(Message* pmsg) {
//	assert(ptranslator_);
//	if(ptranslator_ == NULL) {
//		return false;
//	}
//
//	if(threadid_ == 0) {
//		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GSDMessageTranslator::Start was never called");
//		return false;
//	}
//	bool ret = false;
//	MessageReplicator msgrepl(*pmsg);
//	msgqueue_.lock();
//	ret = ProcessReplicator(msgrepl, msgqueue_);
//	msgqueue_.unlock();
//	if( pmsg->m_eExpectedResponse==ER_ReplyMessage)
//	{
//		LoggerWrapper::GetInstance()->Write(LV_STATUS, "GSDMessageTranslator will try to wait until somebody sends response.");
//		time_t tTimeout=time(NULL)+DEFAULT_RESPONSE_TIME;
//		RubyIOManager* pmanager = RubyIOManager::getInstance();		
//		while( tTimeout>time(NULL) )
//		{
//			PLUTO_SAFETY_LOCK(mm,pmanager->m_MsgMutex);
//			if( pmsg->m_bRespondedToMessage )
//				return ret;
//			mm.Release();
//			Sleep(100);
//		}
//		PLUTO_SAFETY_LOCK(mm,pmanager->m_MsgMutex);
//		if( !pmsg->m_bRespondedToMessage )
//		{
//			return false;
//		}
//	}
//	return ret;
//};

};
