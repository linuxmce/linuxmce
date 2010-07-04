/*
     Written for LinuxMCE. (c) LinuxMCE Foundation.
     Licensed under the GNU GPL v3, see COPYING for more details.

     Author: Adam Shortland <los93sol@gmail.com>
*/

#include "applicationcommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "runtimeconfig.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

ApplicationCommand::ApplicationCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Application command created.");
};

ApplicationCommand::~ApplicationCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Application command destroyed.");
};

void 
ApplicationCommand::setPhoneNum(std::string phonenum) {
	this->phonenum = phonenum;
}

void
ApplicationCommand::setApplication(std::string application) {
	this->application = application;
}

void
ApplicationCommand::setApplicationData(std::string application_data) {
	this->application_data = application_data;
}

void
ApplicationCommand::handleStartup() {

	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Registered phone %s as originator of the call. ", phonenum .c_str());
	//RuntimeConfig::getInstance()->regOriginateCall(phonenum);
	
	m_token.setKey(TOKEN_ACTION, ACTION_ORIGINATE);
	m_token.setKey(TOKEN_CHANNEL, "Local/" + phonenum + "@trusted" );
	m_token.setKey(TOKEN_APPLICATION, application);
	m_token.setKey(TOKEN_APPDATA, application_data);
	//m_token.setKey(TOKEN_PRIORITY, "1");
};

bool 
ApplicationCommand::handleResponse(Token& token) {

	//TODO: do we still need this?

	if(token.getKey(TOKEN_RESPONSE) != RESPONSE_SUCCESS) {
		/*error*/
		//AsteriskManager::getInstance()->NotifyResult(commandid, -1, destchannel);
		return true;
	}

	return true;
}

void 
ApplicationCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Application completed.");
}

};
