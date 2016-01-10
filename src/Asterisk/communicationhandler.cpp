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
// C++ Implementation: communicationhandler
//
// Description: 
//
//
// Author: Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "communicationhandler.h"

#include "DCE/Logger.h"

#include "asteriskconsts.h"
#include "asteriskmanager.h"
#include "runtimeconfig.h"
#include "utils.h"

using namespace std;
using namespace DCE;

#define ZOMBIE_CHANNEL_TOKEN "<ZOMBIE>"
#define PSEUDO_CHANNEL_TOKEN "Zap/pseudo"

namespace ASTERISK {

CommunicationHandler::CommunicationHandler()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CommunicationHandler created.");
};

CommunicationHandler::~CommunicationHandler() 
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CommunicationHandler destroyed.");
};

int CommunicationHandler::handleToken(Token* ptoken, bool& bIsResponseToken) 
{

	bIsResponseToken = false;
	if(ptoken->hasKey(TOKEN_RESPONSE)) {
		bIsResponseToken = true;
		return 1;
	}
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWEXTEN)
		return handleNewextenEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_HANGUP) 
		return handleHangupEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_LINK) 
		return handleLinkEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWCHANNEL)
		return handleNewChannelEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_NEWSTATE)
		return handleNewStateEvent(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_PEER_STATUS)
		return handlePeerStatusChanged(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_EXTEN_STATUS)
		return handleExtensionStatusChanged(ptoken);
	else if(ptoken->getKey(TOKEN_EVENT) == EVENT_DIAL)
		return handleDial(ptoken);

	return 0;
}

int CommunicationHandler::handleDial(Token* ptoken)
{	
	// ASTERISK 1.6 samples for IAX2 (Foxi352)
	//Event: Dial
	//Privilege: call,all
	//SubEvent: Begin
	//Channel: IAX2/306-3010
	//Destination: SIP/20408720-00000008
	//CallerIDNum: 306
	//CallerIDName: pl_554
	//UniqueID: 1317825362.74
	//DestUniqueID: 1317825362.75
	//Dialstring: 20408720/39872039

	//Event: Dial
	//Privilege: call,all
	//SubEvent: End
	//Channel: IAX2/306-8140
	//UniqueID: 1317826125.78
	//DialStatus: ANSWER

	//Event: Hangup
	//Privilege: call,all
	//Channel: IAX2/306-14139
	//Uniqueid: 1317826682.80
	//CallerIDNum: 306
	//CallerIDName: pl_554
	//Cause: 16
	//Cause-txt: Normal Clearing
	
	string sSrcChannel = ptoken->getKey(TOKEN_CHANNEL);
	string sDestChannel = ptoken->getKey(TOKEN_DESTINATION);

	string sSrcCallerID = ptoken->getKey(TOKEN_CALLERID);
	string sDestCallerID = map_callerid[sDestChannel];

	string sSrcExtension;
	string sDestExtension;

	if(!Utils::ParseChannel(sSrcChannel, &sSrcExtension) && !Utils::ParseChannel(sDestChannel, &sDestExtension)) 
	{
		if(sDestExtension != sSrcExtension)
		{
			//e.g. 202 -> 202
			map_ringext[sDestExtension] = sSrcChannel;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ringing from channel %s (%s) to channel %s (%s) ",
				sSrcChannel.c_str(), sSrcCallerID.c_str(), sDestChannel.c_str(), sDestCallerID.c_str());

			AsteriskManager::getInstance()->NotifyRing(sSrcChannel, sDestChannel, sSrcCallerID, sDestCallerID);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Ignoring event because exten is %s is channel is %s", 
				sDestExtension.c_str(), sSrcChannel.c_str());
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to parse channel %s", sSrcChannel.c_str());
	}

	return 0;
}

int CommunicationHandler::handleNewextenEvent(Token* ptoken) 
{
	if(ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_CONF || ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_MEETME || ptoken->getKey(TOKEN_APPLICATION) == APPLICATION_CONFBRIDGE)
	{
		//e.g.
		//Event: Newexten
		//Privilege: call,all
		//Channel: Local/201@trusted-860c,1
		//Context: trusted
		//Extension: 0000
		//Priority: 1
		//Application: Conference
		//AppData: 0000/S/1
		//Uniqueid: 1194014441.37

		string sSrcChannel = ptoken->getKey(TOKEN_CHANNEL);
		string sDestExtension = ptoken->getKey(TOKEN_EXTENSION);
		string sSrcExtension;

		if(!Utils::ParseChannel(sSrcChannel, &sSrcExtension)) 
		{
			//e.g. 201 -> room 0000
			map_ringext[sDestExtension] = sSrcChannel;

			string sSrcCallerID = map_callerid[sSrcChannel];

			string sDestChannel = sDestExtension; //TODO: is this correct ?
			string sDestCallerID = "Conference " + sDestExtension;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ringing from channel %s (%s) to channel %s (%s) ",
				sSrcChannel.c_str(), sSrcCallerID.c_str(), sDestExtension.c_str(), sDestCallerID.c_str());
			AsteriskManager::getInstance()->NotifyRing(sSrcChannel, sDestChannel, sSrcCallerID, sDestCallerID);
			AsteriskManager::getInstance()->NotifyLink(sSrcChannel, sDestChannel, sSrcCallerID, sDestCallerID);

		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to parse channel %s", sSrcChannel.c_str());
		}
	}

	return 0;
}

int CommunicationHandler::handleHangupEvent(Token* ptoken) 
{
	//e.g.
	//Event: Hangup
	//Privilege: call,all
	//Channel: SIP/200-0821dcc8
	//Uniqueid: 1194013971.30
	//Cause: 21
	//Cause-txt: Call Rejected

	string channel = ptoken->getKey(TOKEN_CHANNEL);
	string reason = ptoken->getKey(TOKEN_REASON);

	//Ignore channels like this:
	//AsyncGoto/SIP/202-08206080<ZOMBIE>
	//Zap/pseudo-47590674

	if(string::npos != channel.find(ZOMBIE_CHANNEL_TOKEN) || string::npos != channel.find(PSEUDO_CHANNEL_TOKEN))
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ignoring hangup for channel %s", channel.c_str());
		return 0;
	}

	string ringphoneid;
	if(!Utils::ParseChannel(channel, &ringphoneid))
		map_ringext.erase(ringphoneid);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup: %s, %s", channel.c_str(), reason.c_str());
	AsteriskManager::getInstance()->NotifyHangup(channel, reason);

	return 0;
}

int CommunicationHandler::handleLinkEvent(Token* ptoken) 
{
	string channel1 = ptoken->getKey(TOKEN_CHANNEL1);
	string channel2 = ptoken->getKey(TOKEN_CHANNEL2);
	string callerid1 = ptoken->getKey(TOKEN_CALLERID1);
	string callerid2 = ptoken->getKey(TOKEN_CALLERID2);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Linked channel %s (%s) to channel %s (%s) ",
		channel1.c_str(), callerid1.c_str(), channel2.c_str(), callerid2.c_str());
	AsteriskManager::getInstance()->NotifyLink(channel1, channel2, callerid1, callerid2);

	return 0;
}

int CommunicationHandler::handleNewStateEvent(Token* ptoken)
{
	if(ptoken->getKey(TOKEN_STATE) == STATE_RING)
	{
		//e.g.
		//Event: Newstate
		//Privilege: call,all
		//Channel: SIP/202-0821b2e0
		//State: Ring
		//CallerID: 202
		//CallerIDName: 202
		//Uniqueid: 1194011868.13

		string sSrcChannel = ptoken->getKey(TOKEN_CHANNEL);  
		string sSrcCallerID = ptoken->getKey(TOKEN_CALLERID);        
		map_callerid[sSrcChannel] = sSrcCallerID;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Caller id for channel %s is '%s'", 
			sSrcChannel.c_str(), sSrcCallerID.c_str());
	}

	return 0;
}

int CommunicationHandler::handleNewChannelEvent(Token* ptoken)
{
	if(ptoken->getKey(TOKEN_STATE) == STATE_RING)
	{
		//e.g.
		//Event: Newchannel
		//Privilege: call,all
		//Channel: Local/201@trusted-860c,2
		//State: Ring
		//CallerIDNum: <unknown>
		//CallerIDName: <unknown>
		//Uniqueid: 1194014441.38

		string sSrcChannel = ptoken->getKey(TOKEN_CHANNEL);  
		string sSrcCallerID = ptoken->getKey(TOKEN_CALLERID);  
		map_callerid[sSrcChannel] = sSrcCallerID;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Caller id for channel %s is '%s'", 
			sSrcChannel.c_str(), sSrcCallerID.c_str());
	}

	return 0;
}

int CommunicationHandler::handlePeerStatusChanged(Token* ptoken) 
{
	string sPeer = ptoken->getKey(TOKEN_PEER);
	string sStatus = ptoken->getKey(TOKEN_PEER_STATUS);

	AsteriskManager::getInstance()->NotifyExtensionsStatus(sPeer + ":" + sStatus);
	return 1;
}

int CommunicationHandler::handleExtensionStatusChanged(Token* ptoken) 
{
	string sExten = ptoken->getKey(TOKEN_EXTEN);
	string sRawStatus = ptoken->getKey(TOKEN_STATUS);
	string sStatus;

	switch(atoi(sRawStatus.c_str()))
	{
		case 0:
			sStatus = "Ready";
			break;

		case 1:
			sStatus = "On the phone";
			break;

		case 8:
			sStatus = "Ringing";
			break;

		default:
			sStatus = "Unknown(" + sRawStatus + ")";
			break;
	}

	AsteriskManager::getInstance()->NotifyExtensionsStatus(sExten + ":" + sStatus);
	return 1;
}

}; //namespace ASTERISK

