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
#include "duplicatefilterpoolinterceptor.h"
#include "DCE/Logger.h"

#include <unistd.h>
#include <stdio.h>

#define DUPLICATE_TIME_DELAY	1000

using namespace DCE;

namespace EIBBUS {

DuplicateFilterPoolInterceptor::DuplicateFilterPoolInterceptor()
{
}


DuplicateFilterPoolInterceptor::~DuplicateFilterPoolInterceptor()
{
}

void 
DuplicateFilterPoolInterceptor::handleTelegram(const TelegramMessage *pt) {
	struct timespec arrivedtm;
	clock_gettime(CLOCK_REALTIME, &arrivedtm);
	
	bool bSend = true;
	
	std::list<TIMEDMESSAGE>::iterator it = messages_.begin(); 
	while(it != messages_.end()) {
		if((arrivedtm.tv_sec - (*it).atm.tv_sec) * 1000  
				+ ((arrivedtm.tv_nsec - (*it).atm.tv_nsec) / 1000000) >= DUPLICATE_TIME_DELAY) {
			it = messages_.erase(it);
		} else {
			if((*it).msg == *pt) {
				bSend = false;
			}
			it++;
		}
	}
	
	if(bSend) {
		TIMEDMESSAGE tmsg(*pt, arrivedtm);
		messages_.push_back(tmsg);
		handleUniqueTelegram(&tmsg.msg);
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Message is a duplicate.");
	}
}

};
