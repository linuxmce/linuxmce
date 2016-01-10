/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCEMESSAGETRANSLATION_H
#define DCEMESSAGETRANSLATION_H

#include "DCE/Message.h"
#include "DCE/DeviceData_Base.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/ProcessUtils.h"

#include "pluto_main/Define_Command.h"

namespace DCE {

/**
@author Igor Spac,,,
*/

/*****************************************************************
MessageReplicator
*****************************************************************/
class MessageReplicator {
public:

	MessageReplicator(const Message& msg, int count = 1, int predelay = 0, int postdelay = 0)
		: msg_(msg),
		  count_(count),
		  predelay_(predelay),
		  postdelay_(postdelay),
		  timeStart_(0L),
		  replaceable_(false),
		  implemented_(false),
		  useless_(false)
	{
		// this should be something (Device Data) from database
		switch( msg_.m_dwID )
		{
			case COMMAND_Vol_Up_CONST :
			case COMMAND_Vol_Down_CONST :
			case COMMAND_Center_Volume_Down_CONST :
			case COMMAND_Center_Volume_Up_CONST :
				timeStart_ = ProcessUtils::GetMsTime();
				break;
			
			case COMMAND_Set_Volume_CONST :
				replaceable_ = true;
				break;
			
			default :
				break;
		}
	}
	
	MessageReplicator(const MessageReplicator& msg)
		: msg_(msg.msg_),
		  count_(msg.count_),
		  predelay_(msg.predelay_),
		  postdelay_(msg.postdelay_),
		  timeStart_(msg.timeStart_),
		  replaceable_(msg.replaceable_),
		  implemented_(msg.implemented_),
		  useless_(msg.useless_)
	{
	}

	Message& getMessage() {
		return msg_;
	}
	
	int getCount() {
		return count_;
	}
	void setCount(int count) {
		count_ = count;
	}
	
	int getPreDelay() {
		return predelay_;
	}
	void setPreDelay(int predelay) {
		predelay_ = predelay;
	}
	
	int getPostDelay() {
		return postdelay_;
	}
	void setPostDelay(int postdelay) {
		postdelay_ = postdelay;
	}
	
	unsigned long getTimeStart() const {
		return timeStart_;
	}
	void setTimeStart(unsigned long timeStart) {
		timeStart_ = timeStart;
	}
	
	bool isReplaceable() const
	{
		return replaceable_;
	}
	void setReplaceable(bool replaceable)
	{
		replaceable_ = replaceable;
	}

	bool isImplemented() const
	{
		return implemented_;
	}
	void setImplemented(bool implemented)
	{
		implemented_ = implemented;
	}

	bool isUseless() const
	{
		return useless_;
	}
	void setUseless(bool useless)
	{
		useless_ = useless;
	}

private:

	Message msg_;
	int count_;
	int predelay_;
	int postdelay_;
	unsigned long timeStart_;
	bool replaceable_;
	bool implemented_;
	bool useless_;
};

/*****************************************************************
MessageReplicatorList
*****************************************************************/
class MessageReplicatorList : public std::list<MessageReplicator> {
public:
	
	MessageReplicatorList();
	virtual ~MessageReplicatorList();

	/** Lock the access to the queue.*/
	void lock();
	
	/** Unlock the access to the queue.*/
	void unlock();
	
	/** Find a replaceable message in the queue.*/
	bool findReplaceable(MessageReplicator&);
		
private:
	
	pthread_mutex_t mutexid_;	
};

/*****************************************************************
TranslationBase
*****************************************************************/
class TranslationBase {
public:
	DeviceData_Base* FindTargetDevice(long devid);

protected:
	virtual AllDevices *getAllDevices() = 0;
	
private:
	static DeviceData_Base* FindTargetDevice(long devid, DeviceData_Base* pfromdev);
};

/*****************************************************************
MessageTranslator
*****************************************************************/

class MessageTranslator : public TranslationBase {
public:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) = 0;
};

/*****************************************************************
MessageDispatcher
*****************************************************************/

class MessageDispatcher : public TranslationBase {
public:
	virtual void DispatchMessage(MessageReplicator& inrepl) = 0;
};

/*****************************************************************
MessageTranslationManager
*****************************************************************/
class MessageTranslationManager {
public:
	MessageTranslationManager();
	MessageTranslationManager(MessageTranslator* ptranslator, MessageDispatcher* pdispatcher);
	virtual ~MessageTranslationManager();
	
public:
	bool ProcessMessage(Message* pmsg);

public:
	void setTranslator(MessageTranslator* ptranslator) {
		ptranslator_ = ptranslator;
	}
	
	MessageTranslator* getTranslator() {
		return ptranslator_;
	}
	
	void setDispatcher(MessageDispatcher* pdispatcher) {
		pdispatcher_ = pdispatcher;
	}
	
	MessageDispatcher* setDispatcher() {
		return pdispatcher_;
	}

protected:
	virtual void handleStart() {};
	virtual void handleStop() {};

	virtual void Start();

private:
/*	bool IsMessageProcessed(MessageReplicator& repl);
	void MarkMessageProcessed(MessageReplicator& repl);
*/
	bool ProcessReplicator(MessageReplicator& repl, MessageReplicatorList& replicators);
	
private:
	pthread_t threadid_;
	bool stopqueue_;
	void _QueueProc();
	static void* _queueproc(void *arg);

private:
	MessageTranslator* ptranslator_;
	MessageDispatcher* pdispatcher_;
	MessageReplicatorList msgqueue_;
};

/*****************************************************************
MessageProcessor
*****************************************************************/

template <class T = MessageTranslator, class D = MessageDispatcher>
class MessageProcessor : 
		public MessageTranslationManager,
		protected T, 
		protected D 
{
public:
	MessageProcessor() {
		setTranslator(this);
		setDispatcher(this);
	}
	
public:
	bool ProcessMessage(Message* pmsg) {
		return MessageTranslationManager::ProcessMessage(pmsg);
	}
};

};

#endif
