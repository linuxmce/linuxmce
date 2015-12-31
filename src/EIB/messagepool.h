/***************************************************************************
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

/** @file messagepool.h
Header for all the MessagePool classes.
*/
#ifndef EIBMESSAGEPOOL_H
#define EIBMESSAGEPOOL_H

#include "thread.h"
#include "mutex.h"
#include "event.h"
#include "statemachine.h"
#include "telegrammessage.h"

#include <list>
#include "Serial/SerialPort.h"

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

class BusConnector;

/** @class MessagePoolInterceptor
For ???
*/
class MessagePoolInterceptor {
public:
	virtual void handleTelegram(const TelegramMessage *pt) = 0;
};

/** @class MessagePool
For ???
*/
class MessagePool : public Thread, protected StateMachine {
public:
    MessagePool(bool monitormode = true);
    ~MessagePool();

public:
	inline void setSerialPort(const char* serport, int serbps, enum eParityBitStop serparity) {
		serport_ = serport;
		serbps_ = serbps;
		serparity_ = serparity;
	}
	inline const char* getSerialPort() {
		return serport_.c_str();
	}

	int regInterceptor(MessagePoolInterceptor *pi);
	void unregInterceptor(MessagePoolInterceptor *pi);

public:
	int sendTelegram(const TelegramMessage *ptelegram, bool waitack = false);
	int readTelegram(TelegramMessage *ptelegram, int waitmilisecs = -1);

protected:
	virtual void* _Run();

	virtual bool handleStartup();
	virtual void handleTerminate();
	virtual void handleNewState();

private:
	std::string serport_;
	int serbps_;
	eParityBitStop serparity_;
	BusConnector* pbusconn_;

private:
	bool monitormode_;
	MessagePoolInterceptor* pi_;

private:
	Mutex msq_; /*lock on both queues*/
	Event esq_, erq_; /*event for arriving in send/recv queues*/
	std::list<TelegramMessage> recvqueue_;
	std::list<TelegramMessage> sendqueue_;

protected:
	/*BaseState*/
/** @class BaseState
For ???
*/
	friend class BaseState;
	class BaseState : public State {
	public:
		BaseState(StateMachine* psm)
			: State(psm) {}
	public:
		bool readAcknowledge();
		void logTelegram(const TelegramMessage *pt);
	};

	/*InitState*/
/** @class InitState
For ???
*/
	friend class InitState;
	class InitState : public BaseState {
	public:
		InitState(StateMachine* psm)
			: BaseState(psm) {}
	protected:
		virtual void Handle(void* p);
	} initstate_;

	/*RecvState*/
/** @class RecvState
For ???
*/
	friend class RecvState;
	class RecvState : public BaseState {
	public:
		RecvState(StateMachine* psm)
			: BaseState(psm) {}
	protected:
		virtual void Handle(void* p);
	} recvstate_;

	/*SendState*/
/** @class SendState
For ???
*/
	friend class SendState;
	class SendState : public BaseState {
	public:
		SendState(StateMachine* psm)
			: BaseState(psm), errcount_(0) {}
	protected:
		virtual void Handle(void* p);
	private:
		int errcount_;
	} sendstate_;

	/*ReadyState*/
/** @class ReadyState
For ???
*/
	friend class ReadyState;
	class ReadyState : public BaseState  {
	public:
		ReadyState(StateMachine* psm)
			: BaseState(psm) {}
	protected:
		virtual void Handle(void* p);
	} readystate_;
};


};

#endif
