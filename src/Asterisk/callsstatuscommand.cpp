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
#include "callsstatuscommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

CallsStatusCommand::CallsStatusCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Calls status command created.");
};

CallsStatusCommand::~CallsStatusCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Calls status command destroyed.");
};

void
CallsStatusCommand::handleStartup() {
	m_token.setKey(TOKEN_ACTION, ACTION_COMMAND);
	m_token.setKey(TOKEN_COMMAND, "core show channels concise");
	//Response is like: 
	//SIP/20408720-00000008!from-trunk!!1!Up!AppDial!(Outgoing Line)!039872039!!3!15!IAX2/306-3010!1317825362.75
	//IAX2/306-3010!macro-dialout-trunk!s!23!Up!Dial!SIP/20408720/39872039,300,!306!!3!15!SIP/20408720-00000008!1317825362.74
};

bool 
CallsStatusCommand::handleResponse(Token &token){
	string sResponse = token.getKey(TOKEN_DATA);

	vector<string> vectLines;
	StringUtils::Tokenize(sResponse, "\n", vectLines);

	string sStatus;
	if(vectLines.size() > 1)
	{
		//skip the footer
		for(size_t i = 0; i < vectLines.size() - 1; ++i)
			sStatus += vectLines[i] + "\n";
	}

	AsteriskManager::getInstance()->NotifyCallsStatus(sStatus);
	return true;
}

void 
CallsStatusCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Calls status completed.");	
}

};
