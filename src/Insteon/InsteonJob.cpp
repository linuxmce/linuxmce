/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/


#include "InsteonJob.h"

#include "main.h"
#include "LMCEInsteonAPI.h"

class InsteonJob::Private
{
	public:
	
		Private(LMCEInsteonAPI *);
		~Private();
		
		LMCEInsteonAPI * handler;
		InsteonJob::JobState state;
		InsteonJob::INSTEON_JOB type;
		time_t receivingTimeout;
		time_t runningTimeout;
		time_t queueTimeout;
		time_t startTime;
		time_t answerTime;
		bool again;
		
	private:
};


InsteonJob::Private::Private(LMCEInsteonAPI * insteonAPI)
	: handler(insteonAPI),
	  state(InsteonJob::IDLE),
	  type(InsteonJob::NOTHING),
	  receivingTimeout(0),
	  runningTimeout(20),
	  queueTimeout(20),
	  startTime(0),
	  answerTime(0),
	  again(false)
{
}


InsteonJob::Private::~Private()
{
}

InsteonJob::InsteonJob(LMCEInsteonAPI * insteonAPI)
{
	d = new Private(insteonAPI);
	if( d == NULL )
	{
		// error message
	}
}


InsteonJob::~InsteonJob(void)
{
	delete d;
	d = NULL;
}

InsteonJob::JobState InsteonJob::state() const
{
	return d->state;
}

void InsteonJob::setState(InsteonJob::JobState st)
{
	d->state = st;
}
InsteonJob::INSTEON_JOB InsteonJob::type() const
{
	return d->type;
}

void InsteonJob::setType(InsteonJob::INSTEON_JOB type)
{
	d->type = type;
}

LMCEInsteonAPI* InsteonJob::handler() const
{
	return d->handler;
}

bool InsteonJob::runAgain()
{
	d->again = true;
	bool bRet = run();
	d->again = false;
	
	return bRet;
}

bool InsteonJob::again() const
{
	return d->again;
}

