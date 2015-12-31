/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "messagepool.h"
#include "busconnector.h"
#include "timetracker.h"

#include "DCE/Logger.h"

#include "bauinitmessage.h"
#include "monitorrequestmessage.h"
#include "ackmessage.h"


using namespace std;
using namespace DCE;

#ifndef _WINDOWS
	#define DEFAULT_EIB_SER_PORT 	"ttyS0"
#else
	#define DEFAULT_EIB_SER_PORT 	"COM1"
#endif

#define SLEEP_IO_ERROR			5000
#define SLEEP_WAIT_MESSAGES		10
#define SLEEP_WAIT_ACKNOWLEDGE	3000

/* states */
#define STATE_READY				1
#define STATE_INIT				2
#define STATE_ACK				3
#define STATE_SEND				4
#define STATE_READ				5

#define MAX_RECVQUEUE_SIZE		20
#define MAX_RESEND_RETRIES		3

#define INTER_SEND_DELAY		100

extern long g_dwTelegramDelay;

namespace EIBBUS {

std::string FormatHexBuffer(const unsigned char* buff, unsigned int size);

MessagePool::MessagePool(bool monitormode) 
	: monitormode_(monitormode),
		initstate_(this),
		recvstate_(this),
		sendstate_(this),
		readystate_(this)
{
	serport_ = DEFAULT_EIB_SER_PORT;
	
	pbusconn_ = BusConnector::getInstance();
}


MessagePool::~MessagePool() {
	Wait(true);
}

int 
MessagePool::regInterceptor(MessagePoolInterceptor *pi) {
	pi_ = pi;
	return 0;
}

void 
MessagePool::unregInterceptor(MessagePoolInterceptor *pi) {
	pi_ = NULL;
}

bool 
MessagePool::handleStartup()
{
	return true;
}

void 
MessagePool::handleTerminate()
{
}

void* 
MessagePool::_Run() {
	while(!isStopRequested()) {
		/*check if we are connected*/
		if(!pbusconn_->isOpened()) {
			if(pbusconn_->Open(serport_.c_str(),serbps_,serparity_)) {
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Could not connect to EIB Bus. Sleeping for %d second...", SLEEP_IO_ERROR);
				sleep(SLEEP_IO_ERROR / 1000);
				continue;
			}
			setState(&initstate_);
		} 
		
		Handle();
	}	
	return 0;
}


void 
MessagePool::handleNewState() {
/*
	std::string statestr;
	if(getState() == &readystate_)
		statestr = "READY";
	else 
	if(getState() == &initstate_)
		statestr = "INIT";
	else 
	if(getState() == &sendstate_)
		statestr = "SEND";
	else 
	if(getState() == &recvstate_)
		statestr = "RECV";
	else 
		statestr = "UNKNOWN";
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Now in %s state.", statestr.c_str());
*/
}

int 
MessagePool::sendTelegram(const TelegramMessage *ptelegram, bool waitack) {
	msq_.Lock();
	sendqueue_.push_back(*ptelegram);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "New telegram in Send Queue.");
	esq_.Signal();
	msq_.Unlock();
	return 0;
}

int 
MessagePool::readTelegram(TelegramMessage *ptelegram, int waitmilisecs) {
	int ret = -1;
	
	msq_.Lock();
	if(recvqueue_.size() > 0) {
		*ptelegram = *recvqueue_.begin();
		recvqueue_.pop_front();
		msq_.Unlock();
		ret = 0;
	} else {
		msq_.Unlock();
		if(erq_.Wait(waitmilisecs)) {
			msq_.Lock();
			if(recvqueue_.size() > 0) {
				*ptelegram = *recvqueue_.begin();
				recvqueue_.pop_front();
				ret = 0;
			}
			msq_.Unlock();
		}
	}
	return ret;
}

/*state machines*/

#define MPSM(member) (((MessagePool*)getStateMachine())->member)
#define BUSCONNECTOR MPSM(pbusconn_)
#define MONITORMODE MPSM(monitormode_)

#define INITSTATE (&MPSM(initstate_))
#define READYSTATE (&MPSM(readystate_))
#define SENDSTATE (&MPSM(sendstate_))
#define RECVSTATE (&MPSM(recvstate_))

#define POOLINTERCEPTOR (MPSM(pi_))

#define EVENTSENDQUEUE (MPSM(esq_))
#define EVENTRECVQUEUE (MPSM(erq_))

#define MSGLOCK (MPSM(msq_))
#define RECVQUEUE (MPSM(recvqueue_))
#define SENDQUEUE (MPSM(sendqueue_))

bool MessagePool::BaseState::readAcknowledge() {
	int ret;
	TimeTracker tk(true);
	
	AckMessage ackmsg;
	while((ret = ackmsg.Recv(BUSCONNECTOR)) != 0) {
		if(tk.getCurrentPeriodInMiliSecs() > SLEEP_WAIT_ACKNOWLEDGE) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Timeout receiving Acknowledge message.");
			return false;
		}
		if(ret == Message::RECV_UNKNOWN){
			BUSCONNECTOR->Skip(1);
		}
	}
	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Acknowledge received.");
	return true;
}

void 
MessagePool::BaseState::logTelegram(const TelegramMessage *pt) {
	unsigned char usrdata[MAX_STRING_DATA_LEN];
	pt->getUserData(usrdata, sizeof(usrdata));
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Telegram: graddr: %s, LENGTH: %d, SUD: %d, UD: <%s>.", pt->getGroupAddress(), 
									pt->getDataLength(), pt->getShortUserData(), FormatHexBuffer(usrdata, pt->getDataLength()).c_str());
}


void MessagePool::InitState::Handle(void* p) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending BAU Initialization.");

	BauInitMessage bimsg;
	if(bimsg.Send(BUSCONNECTOR)) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error sending BAU Initialization. Waiting for next retry.");
		return;
	}

	if(MONITORMODE) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Requesting Monitor mode.");

		MonitorRequestMessage mrmsg;
		if(mrmsg.Send(MPSM(pbusconn_))) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error requesting Monitor mode. Waiting for next retry.");
			return;
		}
		if(readAcknowledge()) {
			setState(READYSTATE);
		}
	} else {
		setState(READYSTATE);
	}
}

void MessagePool::ReadyState::Handle(void* p) {
	if(BUSCONNECTOR->isDataAvailable()) {
		setState(RECVSTATE);
	} else {
		bool dosend = false;
		MSGLOCK.Lock();
		if(SENDQUEUE.size() > 0) {
			dosend = true;
		}
		MSGLOCK.Unlock();
		if(!dosend && EVENTSENDQUEUE.Wait(SLEEP_WAIT_MESSAGES)) {
			dosend = true;
		}
		if(dosend) {
			setState(SENDSTATE);
		}
	}
}

void MessagePool::RecvState::Handle(void* p) {
	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Data available. Checking for telegrams...");
	
	TelegramMessage tlmsg;
	int ret = tlmsg.Recv(BUSCONNECTOR);
	if(ret == Message::RECV_INVALID) {
		/*incalid message*/
	} else
	if(ret == Message::RECV_UNKNOWN) {
		/*unknown message*/
		BUSCONNECTOR->Skip(1);
	} else {
		logTelegram(&tlmsg);
		if(POOLINTERCEPTOR != NULL) {
			POOLINTERCEPTOR->handleTelegram(&tlmsg);
		}

		/* put telegram into recv queue and unlock readers */
		MSGLOCK.Lock();
		if(RECVQUEUE.size() >= MAX_RECVQUEUE_SIZE) {
			/* LoggerWrapper::GetInstance()->Write(LV_STATUS, "Recv queue FULL. Removing front message.");*/
			RECVQUEUE.pop_front();
		}
		RECVQUEUE.push_back(tlmsg);
		MSGLOCK.Unlock();
		EVENTRECVQUEUE.Signal();

		AckMessage ackmsg;
		if(ackmsg.Send(BUSCONNECTOR)) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error sending Acknowledge.");
		}
	}
	
	setState(READYSTATE);
}

void MessagePool::SendState::Handle(void* p) {
	if(errcount_ >= MAX_RESEND_RETRIES) {
		errcount_ = 0;
		setState(READYSTATE);
		
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Acknowledge not received. Abandoning message send.");
		return;
	}
	
	MSGLOCK.Lock();
	if(SENDQUEUE.size() > 0) {
		bool waitack = true;
		TelegramMessage& tlmsg = *SENDQUEUE.begin();
	
		if(errcount_ == 0) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending telegram...");
		} else {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending telegram. Retry %d.", errcount_);
		}
	
		TimeTracker tk;
		tk.Start();
		if(tlmsg.Send(BUSCONNECTOR)) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error sending telegram.");
			setState(READYSTATE);
		} else {
			tk.Stop();
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Telegram sent in %.04f seconds", tk.getTotalPeriodInSecs());
	
			if(!waitack || readAcknowledge()) {
				SENDQUEUE.pop_front();
				errcount_ = 0;
				
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Acknowledge received.");
				
				setState(READYSTATE);
			} else {
				errcount_++;
			}
		}
	} else {
		setState(READYSTATE);
	}
	MSGLOCK.Unlock();
	usleep(g_dwTelegramDelay * 1000);
}

std::string FormatHexBuffer(const unsigned char* buff, unsigned int size) {
	std::string logstr;
	if(size <= 0) {
		logstr = "EMPTY BUFFER";
	} else {
		char hxbuff[5];
		for(unsigned int i = 0; i < size; i++) {
			sprintf(hxbuff, "0x%0hhx", buff[i]);
			logstr += ((i > 0) ? " " : "");
			logstr += hxbuff;
		}
	}
	return logstr;
}

}
;
