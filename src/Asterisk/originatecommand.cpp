/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//
// C++ Implementation: originatestatemachine
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "originatecommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "runtimeconfig.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

OriginateCommand::OriginateCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate command created.");
};

OriginateCommand::~OriginateCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate command destroyed.");
};

void 
OriginateCommand::setPhoneNum(std::string phonenum) {
	this->phonenum = phonenum;
}

void 
OriginateCommand::setPhoneType(std::string phonetype) {
	this->phonetype = phonetype;
	// SIP5061 -> SIP
	if( 3 < this->phonetype.size() &&
		"SIP" == this->phonetype.substr(0, 3) )
	{
		this->phonetype = "SIP";
	}
}

void 
OriginateCommand::setExtenNum(std::string extennum) {
	this->extennum = extennum;
}

void 
OriginateCommand::setCallerID(std::string callerid) {
	this->callerid = callerid;
}

void
OriginateCommand::handleStartup() {

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Registered phone %s as originator of the call. ", phonenum .c_str());
	RuntimeConfig::getInstance()->regOriginateCall(phonenum);
	
	m_token.setKey(TOKEN_ACTION, ACTION_ORIGINATE);
	m_token.setKey(TOKEN_CHANNEL, phonetype + "/" + phonenum );
	m_token.setKey(TOKEN_EXTEN, extennum);
	m_token.setKey(TOKEN_CONTEXT, "trusted");
	m_token.setKey(TOKEN_PRIORITY, "1");
	m_token.setKey(TOKEN_CALLERID, callerid);
};

bool 
OriginateCommand::handleResponse(Token& token) {

	//TODO: do we still need this?

	if(token.getKey(TOKEN_RESPONSE) != RESPONSE_SUCCESS) {
		/*error*/
		//AsteriskManager::getInstance()->NotifyResult(commandid, -1, destchannel);
		return true;
	}

	return true;
}

void 
OriginateCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Originate completed.");
}

};
