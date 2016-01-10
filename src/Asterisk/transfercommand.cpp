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
#include "transfercommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

TransferCommand::TransferCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transfer command created.");
};

TransferCommand::~TransferCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transfer command destroyed.");
};

void 
TransferCommand::setExtenNum(std::string extennum) {
	this->extennum = extennum;
}

void 
TransferCommand::setChannel1(std::string channel) {
	this->channel1 = channel;
}

void 
TransferCommand::setChannel2(std::string channel) {
	this->channel2 = channel;
}

void
TransferCommand::handleStartup() {
	m_token.setKey(TOKEN_ACTION, ACTION_REDIRECT);
	m_token.setKey(TOKEN_CHANNEL, channel1 );
        m_token.setKey(TOKEN_EXTEN, extennum);
        m_token.setKey(TOKEN_CONTEXT, "trusted");
        m_token.setKey(TOKEN_PRIORITY, "1");

	if(channel2 != "") {
		m_token.setKey(TOKEN_EXTRACHANNEL, channel2 );
		m_token.setKey(TOKEN_EXTRACHANCONTEXT, "trusted");
		m_token.setKey(TOKEN_EXTRAEXTEN, extennum);
		m_token.setKey(TOKEN_EXTRAPRIORITY, "1");
	}
};

bool 
TransferCommand::handleResponse(Token &token) {
	return true;
}

void 
TransferCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transfer completed.");	
}

};
