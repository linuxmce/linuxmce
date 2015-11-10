/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "MessageTranslation.h"

#include <assert.h>
#include "DCE/Logger.h"

#define POOL_IDLE_SLEEP					50
#define MARKPARAM_MESSAGE_PROCESS		((unsigned long)-1)

#ifdef WIN32
#define usleep(x) Sleep((x) / 1000)
#endif

namespace DCE {

MessageReplicatorList::MessageReplicatorList() {
	pthread_mutex_init(&mutexid_, 0);
}

MessageReplicatorList::~MessageReplicatorList() {
	pthread_mutex_destroy(&mutexid_);
}

void 
MessageReplicatorList::lock() {
	pthread_mutex_lock(&mutexid_);
}

void 
MessageReplicatorList::unlock() {
	pthread_mutex_unlock(&mutexid_);
}

// TODO: it should be const reference as parameter
bool
MessageReplicatorList::findReplaceable(MessageReplicator & msgReplicator)
{
	bool found = false;
	
	lock();
	for(std::list<MessageReplicator>::iterator it=begin(); it!=end(); ++it)
	{
		if( (*it).isReplaceable() && (*it).getMessage().m_dwID == msgReplicator.getMessage().m_dwID )
		{
			found = true;
			break;
		}
	}
	unlock();
	
	return found;
}

/*****************************************************************
TranslationBase
*****************************************************************/
DeviceData_Base * 
TranslationBase::FindTargetDevice(long devid) {
	assert(getAllDevices());
	if(!getAllDevices()) {
		return NULL;
	}
	return getAllDevices()->m_mapDeviceData_Base_Find(devid);
}
		
/*****************************************************************
MessageTranslationManager
*****************************************************************/

MessageTranslationManager::MessageTranslationManager()
	: stopqueue_(false),
	  ptranslator_(NULL),
	  pdispatcher_(NULL)
{
	threadid_.p = 0;
	ProcessUtils::ResetMsTime();
}

MessageTranslationManager::MessageTranslationManager(MessageTranslator* ptranslator, MessageDispatcher* pdispatcher)
	: stopqueue_(false)
{
	threadid_.p = 0;
	ProcessUtils::ResetMsTime();
	setTranslator(ptranslator);
	setDispatcher(pdispatcher);
}

MessageTranslationManager::~MessageTranslationManager()
{
	stopqueue_ = true;
	if(threadid_.p != 0) {
		pthread_join(threadid_, 0);
	}
	
	//TODO
	// to delete the translator and dispatcher ??
}

bool 
MessageTranslationManager::ProcessMessage(Message* pmsg) {
	assert(ptranslator_);
	if(ptranslator_ == NULL) {
		return false;
	};

	if(threadid_.p == 0) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MessageTranslationManager::Start was never called");
		return false;
	}

	bool ret = false;
	
	msgqueue_.lock();
	MessageReplicator msgrepl(*pmsg);
	ret = ProcessReplicator(msgrepl, msgqueue_);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "#### Pre-Process Queue = %u", msgqueue_.size());
	msgqueue_.unlock();
	
	return ret;
}

void 
MessageTranslationManager::Start() {
	if(threadid_.p == 0) {
		pthread_create(&threadid_, NULL, _queueproc, (void*)this);
	}
}

bool
MessageTranslationManager::ProcessReplicator(MessageReplicator& msgrepl, MessageReplicatorList& replicators) {
	MessageReplicatorList lrepls;
	if(!ptranslator_->Translate(msgrepl, lrepls)) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessReplicator : not translatable");
		// Eugen C. - forwarding all not translatable messages into the Translator queue
		// to avoid mixing the messages into the dispatcher queue
		replicators.push_back( msgrepl );
		return true;
	} else {
		MessageReplicatorList::iterator it = lrepls.begin();
		while(it != lrepls.end()) {
			if(msgrepl.getMessage().m_dwID == (*it).getMessage().m_dwID) {
				replicators.push_back(*it);
			} else {
				MessageReplicatorList prepls;
				ProcessReplicator(*it, prepls);
				for(int i = 0; i < msgrepl.getCount(); i++)
				{
					if( !prepls.empty() )
					{
						for(MessageReplicatorList::iterator itPrepls = prepls.begin(); itPrepls != prepls.end(); ++itPrepls)
						{
							// we don't allow to avoid commands at this moment
							(*itPrepls).setUseless(false);
							replicators.push_back(*itPrepls);
						}
					}
					else
					{
						// we don't allow to avoid commands at this moment
						(*it).setUseless(false);
						replicators.push_back(*it);
					}
				}
			}
			it++;
		}
		
		return (replicators.size() > 0);
	}
}
/*
bool 
MessageTranslationManager::IsMessageProcessed(MessageReplicator& repl) {
	return repl.getMessage().m_mapParameters.count(MARKPARAM_MESSAGE_PROCESS) > 0;
}

void 
MessageTranslationManager::MarkMessageProcessed(MessageReplicator& repl) {
	repl.getMessage().m_mapParameters[MARKPARAM_MESSAGE_PROCESS] = "TRUE";
}
*/

/*
void 
MessageTranslationManager::_QueueProc() {
	int nextdelay = 0;
	
	handleStart();
	while(!stopqueue_) {
		if(nextdelay > 0) {
			usleep(nextdelay * 1000);
			nextdelay = 0;
		}

		bool msgpopped = false;
		
		msgqueue_.lock();
		int delay = 0;
		MessageReplicatorList::iterator it = msgqueue_.begin();
		if(it != msgqueue_.end()) {
			MessageReplicator& replmsg = (*it);
			delay = replmsg.getPreDelay();
			nextdelay = replmsg.getPostDelay();
			
			replmsg.setCount(replmsg.getCount() - 1);
			
			assert(pdispatcher_);
			if(pdispatcher_) {
				MarkMessageProcessed(replmsg);
				pdispatcher_->DispatchMessage(replmsg);
			}
			
			if(replmsg.getCount() <= 0) {
				msgqueue_.erase(it);
			}
			msgpopped = true;
		}
		msgqueue_.unlock();
		
		if(msgpopped && delay > 0) {
			usleep(delay * 1000);
		} else {
			usleep(POOL_IDLE_SLEEP * 1000);
		}
	}
	handleStop();
}

*/

void 
MessageTranslationManager::_QueueProc() {
	handleStart();
	while(!stopqueue_) {
		msgqueue_.lock();
		MessageReplicatorList::iterator it = msgqueue_.begin();
		if(it != msgqueue_.end()) {
			MessageReplicator replmsg = (*it);
			msgqueue_.erase(it);
			msgqueue_.unlock();
			
			// If it's less than 1 sec since it's in the queue
			// then it's still available
			// If NOT, it's ignored !
			// Eugen C. + Aaron B.
			if( 0L == replmsg.getTimeStart() ||
				1000 > ProcessUtils::GetMsTime() - replmsg.getTimeStart() ) {
				// if we find another replaceable message, then we can ignore the current one
				if( replmsg.isReplaceable() && msgqueue_.findReplaceable(replmsg) )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"---- _QueueProc Replaced - %d", replmsg.getMessage().m_dwID);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"_QueueProc Pre - %d : %d", replmsg.getMessage().m_dwID, replmsg.getPreDelay());
					assert(pdispatcher_);
					if(pdispatcher_) {
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"_QueueProc ------- %d", replmsg.getMessage().m_dwID);
						pdispatcher_->DispatchMessage(replmsg);
					}
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"_QueueProc Post - %d : %d", replmsg.getMessage().m_dwID, replmsg.getPostDelay());
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"---- _QueueProc Ignored - %d", replmsg.getMessage().m_dwID);
			}
		} else {
			ProcessUtils::ResetMsTime();
			msgqueue_.unlock();
			usleep(POOL_IDLE_SLEEP* 1000);
		}
	}
	handleStop();
}
		
void* 
MessageTranslationManager::_queueproc(void *arg) {
	MessageTranslationManager* pme = (MessageTranslationManager*)arg;
	pme->_QueueProc();
	return 0;
}

};
