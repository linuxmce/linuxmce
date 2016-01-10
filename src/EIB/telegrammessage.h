/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file telegrammessage.h
Header for TelegramMessage class.
*/
//
// C++ Interface: telegrammessage
//
// Description:
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSTELEGRAMMESSAGE_H
#define EIBBUSTELEGRAMMESSAGE_H

#include "message.h"

#include <cstring>
#include <string>

#define MAX_STRING_DATA_LEN		23

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

/** @class TelegramMessage
For telegrams???

@todo Exactly what does this do?
It sends some sort of message to some device.

*/
class TelegramMessage : public PeiMessage
{
	friend class MessagePool;
public:
    TelegramMessage();
	TelegramMessage(const TelegramMessage& tlmsg);
    ~TelegramMessage();

	bool operator ==(const TelegramMessage& tlmsg);
	TelegramMessage& operator=(const TelegramMessage& tlmsg);

public:
	virtual int Send(BusConnector *pbusconn);
	virtual int Recv(BusConnector *pbusconn);

public:
	enum ACTIONTYPE {
		READ,
		ANSWER,
		WRITE/*,
		SUBSCRIBE,
		UNSUBSCRIBE,
		UNKNOWN*/
	};

public:
	inline void setActionType(ACTIONTYPE acttype) {
		acttype_ = acttype;
	}
	inline ACTIONTYPE getActionType() const {
		return acttype_;
	};

	inline void setGroupAddress(const char* gaddr) {
		gaddr_ = gaddr;
	}
	inline const char* getGroupAddress() const {
		return gaddr_.c_str();
	};


	inline unsigned int getDataLength() const {
		return length_;
	}

	inline void setShortUserData(unsigned char usrdata) {
		shortusrdata_ = usrdata;
		length_ = 0;
	}
	inline unsigned char getShortUserData() const {
		return shortusrdata_;
	}

	inline void setUserData(const unsigned char* usrdata, unsigned int length) { /*in string format, which is then converted to apropriate format*/
		memcpy(usrdata_, usrdata, length);
		length_ = length;
	}
	inline unsigned int getUserData(unsigned char* usrdata, unsigned int length) const {
		if(length >= length_) {
			memcpy(usrdata, usrdata_, length_);
			return length_;
		} else {
			return 0;
		}
	};

private:
	ACTIONTYPE acttype_;
	std::string gaddr_;

	unsigned int length_;
	unsigned char shortusrdata_;
	unsigned char usrdata_[MAX_STRING_DATA_LEN];
};

};

#endif
