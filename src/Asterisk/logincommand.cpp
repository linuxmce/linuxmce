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
// C++ Implementation: LoginCommand
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "logincommand.h"
#include "DCE/Logger.h"
#include "asteriskconsts.h"

using namespace DCE;

namespace ASTERISK {

LoginCommand::LoginCommand() 
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Login commmand created.");
}

LoginCommand::~LoginCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Login command destroyed.");
}

void 
LoginCommand::setUserName(std::string username) {
	this->username = username;
}

void 
LoginCommand::setSecret(std::string secret) {
	this->secret = secret;
}

void 
LoginCommand::handleStartup() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Login SM created.!!!!!");
	
	m_token.setKey("Action", "login");
	m_token.setKey("Username", username);
	m_token.setKey("Secret", secret);
}

bool
LoginCommand::handleResponse(Token &token) {
	//TODO: process response
	return true;
}

void
LoginCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Login completed.");	
}

};
