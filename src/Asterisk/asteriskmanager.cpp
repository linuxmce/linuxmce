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
// C++ Implementation: asteriskmanager
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "asteriskmanager.h"
#include "DCE/Logger.h"

#include "Asterisk.h"

#include "logincommand.h"
#include "originatecommand.h"
#include "applicationcommand.h"
#include "hangupcommand.h"
#include "transfercommand.h"
//#include "conferencecommand.h"
#include "callsstatuscommand.h"
#include "sipextensionsstatuscommand.h"
#include "iaxextensionsstatuscommand.h"

#define DEFAUL_LOGIN_NAME "admin"
#define DEFAUL_LOGIN_SECRET "adminsecret"

#define KEY_RESPONSE "Response"
#define KEY_MESSAGE "Message"

#define RESPONSE_SUCCESS "Success"
#define RESPONSE_ERROR "Error"

using namespace DCE;

namespace ASTERISK {

AsteriskManager* AsteriskManager::sinstance = 0;


AsteriskManager::AsteriskManager() {
	m_pCommunicationHandler = new CommunicationHandler();
}

AsteriskManager::~AsteriskManager() 
{
	Close();
	Release();
}

void AsteriskManager::Close()
{
	if(NULL != m_pCommunicationHandler)
	{
		m_pCommunicationHandler->Wait();

		delete m_pCommunicationHandler;
		m_pCommunicationHandler = NULL;
	}
}

void AsteriskManager::Originate(const string sPhoneNumber, const string sOriginatorNumber,
				const string sOriginatorType, const string sCallerID) 
{
	/*originate*/
	OriginateCommand *pCommand = new OriginateCommand();
	pCommand->setPhoneNum(sOriginatorNumber);
	pCommand->setPhoneType(sOriginatorType);
	pCommand->setExtenNum(sPhoneNumber);
	pCommand->setCallerID(sCallerID);

	m_pCommunicationHandler->sendCommand(pCommand);
}

void AsteriskManager::Application(const string sPhoneNumber, const string sApplication, const string sData_String)
{
	/*application*/
	ApplicationCommand *pCommand = new ApplicationCommand();
	pCommand->setPhoneNum(sPhoneNumber);
	pCommand->setApplication(sApplication);
	pCommand->setApplicationData(sData_String);	
	m_pCommunicationHandler->sendCommand(pCommand);
}

void AsteriskManager::Hangup(const std::string sChannel) 
{
	/*hangup*/
	HangupCommand *pCommand = new HangupCommand();
	pCommand->setChannel(sChannel);

	m_pCommunicationHandler->sendCommand(pCommand);
}

void AsteriskManager::Transfer(const std::string sChannel1, const std::string sChannel2, 
	const string sPhoneNumber)
{
	/*transfer*/
	TransferCommand *pCommand = new TransferCommand();
	pCommand->setChannel1(sChannel1);
	pCommand->setChannel2(sChannel2);
	pCommand->setExtenNum(sPhoneNumber);

	m_pCommunicationHandler->sendCommand(pCommand);
}

void AsteriskManager::GetExtensionsStatus()
{
	m_pCommunicationHandler->sendCommand(new SipExtensionStatusCommand());
	m_pCommunicationHandler->sendCommand(new IaxExtensionsStatusCommand());
}

void AsteriskManager::GetCallsStatus()
{
	m_pCommunicationHandler->sendCommand(new CallsStatusCommand());
}

void AsteriskManager::NotifyRing(const std::string sSourceChannel, const std::string sDestinationChannel, 
								 const std::string sSourceCallerID, const std::string sDestinationCallerID) 
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, 
		"NotifyRing from channel %s, callerid %s to channel %s, callerid %s",
		sSourceChannel.c_str(), sSourceCallerID.c_str(), sDestinationChannel.c_str(), sDestinationCallerID.c_str());

	if(NULL != pAsterisk) 
		pAsterisk->EVENT_PBX_Ring(sSourceChannel, sDestinationChannel, sSourceCallerID, sDestinationCallerID);
}

void AsteriskManager::NotifyHangup(const std::string sChannel, const std::string sReason) 
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "NotifyHangup channel %s, reason %s", 
		sChannel.c_str(), sReason.c_str());

	if(NULL != pAsterisk) 
		pAsterisk->EVENT_PBX_Hangup(sChannel, sReason);
}

void AsteriskManager::NotifyLink(const std::string sSourceChannel, const std::string sDestinationChannel, 
								 const std::string sSourceCallerID, const std::string sDestinationCallerID)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, 
		"NotifyLink from channel %s, callerid %s to channel %s, callerid %s",
		sSourceChannel.c_str(), sSourceCallerID.c_str(), sDestinationChannel.c_str(), sDestinationCallerID.c_str());

	if(NULL != pAsterisk) 
		pAsterisk->EVENT_PBX_Link(sSourceChannel, sDestinationChannel, sSourceCallerID, sDestinationCallerID);
}

void AsteriskManager::NotifyExtensionsStatus(std::string sStatus)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "NotifyPeersStatus status: \n%s", sStatus.c_str());

	if(NULL != pAsterisk) 
		pAsterisk->EVENT_Extensions_Status(sStatus);
}

void AsteriskManager::NotifyCallsStatus(std::string sStatus)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "NotifyCallsStatus status: \n%s", sStatus.c_str());

	if(NULL != pAsterisk) 
		pAsterisk->EVENT_Calls_Status(sStatus);
}

void AsteriskManager::NotifyResult(int iCommandID, int iResult, 
						const std::string Message) 
{	
	if(NULL != pAsterisk) 
		pAsterisk->EVENT_PBX_CommandResult(iCommandID, iResult, Message);
}

AsteriskManager* AsteriskManager::getInstance() 
{
	static AsteriskManager manager;
	
	LOCK_OBJ(manager);
	
	if(sinstance == 0) {
		sinstance = &manager;
	}
	
	UNLOCK_OBJ(manager);
	
	return sinstance;
}

void AsteriskManager::Initialize(DCE::Asterisk* lpAsterisk) {
	this->pAsterisk = lpAsterisk;

    /*initialize persistent SMs*/
	m_pCommunicationHandler->Run(false);
}

void AsteriskManager::Release() {
	this->pAsterisk = 0;
}
		

};
