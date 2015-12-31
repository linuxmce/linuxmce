/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file duplicatefilterpoolinterceptor.h
Header for DuplicateFilterPoolInterceptor class.
*/
//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSDUPLICATEFILTERPOOLINTERCEPTOR_H
#define EIBBUSDUPLICATEFILTERPOOLINTERCEPTOR_H

#include "messagepool.h"

#include <list>

/** @namespace EIBBUS
For EIB (European Installation Bus) Lighting control.
*/
namespace EIBBUS {

/** @class DuplicateFilterPoolInterceptor
For ???
*/
class DuplicateFilterPoolInterceptor : public MessagePoolInterceptor {
public:
    DuplicateFilterPoolInterceptor();
    virtual ~DuplicateFilterPoolInterceptor();

public:
	virtual void handleUniqueTelegram(const TelegramMessage *pt) = 0;

protected:
	virtual void handleTelegram(const TelegramMessage *pt);

private:
	typedef struct _TIMEDMESSAGE {
		TelegramMessage msg;
		struct timespec atm;
		_TIMEDMESSAGE(const TelegramMessage& msg, struct timespec& atm) {
			this->msg = msg;
			this->atm = atm;
		}
	} TIMEDMESSAGE;

	std::list<TIMEDMESSAGE> messages_;
};

};

#endif
