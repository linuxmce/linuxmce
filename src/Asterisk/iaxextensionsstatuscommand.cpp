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
#include "iaxextensionsstatuscommand.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "utils.h"

using namespace DCE;

namespace ASTERISK {

IaxExtensionsStatusCommand::IaxExtensionsStatusCommand()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Peers status command created.");
};

IaxExtensionsStatusCommand::~IaxExtensionsStatusCommand() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Peers status command destroyed.");
};

void
IaxExtensionsStatusCommand::handleStartup() {
	m_token.setKey(TOKEN_ACTION, ACTION_COMMAND);
	m_token.setKey(TOKEN_COMMAND, "iax2 show peers");
};

bool 
IaxExtensionsStatusCommand::handleResponse(Token &token){

	string sResponse = token.getKey(TOKEN_DATA);

	vector<string> vectLines;
	StringUtils::Tokenize(sResponse, "\n", vectLines);

	enum Fields
	{
		fName,
		fHost,
		fDyn,
		fMask,
		fPort,
		fStatus,
		fNumFields
	};

	string sStatus;

	if(vectLines.size() > 3)
	{
		//skip the header and the footers
		for(size_t i = 1; i < vectLines.size() - 2; ++i)
		{
			string sLine = vectLines[i];

			vector<string> vectTokens;
			StringUtils::Tokenize(sLine, " ", vectTokens);

			if(vectTokens.size() >= fNumFields)
			{
				string sName = vectTokens[fName];
				string sHost = vectTokens[fHost];

				vector<string> vectTokens;
				StringUtils::Tokenize(sName, "/", vectTokens);

				if(!vectTokens.empty())
					sName = vectTokens[0];

				sStatus += "IAX2/" + sName + ":" + (sHost == "(Unspecified)" ? "Unregistered" : "Registered") + "\n";
			}
		}
	}

	if(!sStatus.empty())
		AsteriskManager::getInstance()->NotifyExtensionsStatus(sStatus);
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Not sending status for IAX because we don't have any peers");

	return true;
}

void 
IaxExtensionsStatusCommand::handleTerminate() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Iax extensions status completed.");	
}

};
