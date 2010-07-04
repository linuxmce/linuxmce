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
// C++ Interface: asteriskmanager
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKASTERISKMANAGER_H
#define ASTERISKASTERISKMANAGER_H

#include "tokenpool.h"
#include "communicationhandler.h"
#include "threadsafeobj.h"

#include <string>

namespace DCE {
	class Asterisk;
}

namespace ASTERISK {

/**
@author 
*/
class AsteriskManager : public ThreadSafeObj
{
    friend class DCE::Asterisk;
    
private:
    AsteriskManager();
    ~AsteriskManager();

    void Initialize(DCE::Asterisk* lpAsterisk);
    void Release();

public:
	static AsteriskManager* getInstance();

public:
	/*commands*/
	
	/*originates a call*/
	void Originate(const std::string sPhoneNumber, 
		const std::string sOriginatorNumber,
		const std::string sOriginatorType, 
		const std::string sCallerID);
	
	/*link an extension to a dialplan application*/
	void Application(const std::string sPhoneNumber,
		const std::string sApplication,
		const std::string sData_String);

	/*hangs up a call*/
	void Hangup(const std::string sChannel);

	/*transfers a call*/						
	void Transfer(const std::string sChannel1, 
		const std::string sChannel2, 
		const std::string sPhoneNumber);

	/* get status commands; the response will be events*/
	void GetExtensionsStatus();
	void GetCallsStatus();
	
	/*events - usually called by state machines*/
	void NotifyRing(const std::string sSourceChannel, const std::string sDestinationChannel, 
		const std::string sSourceCallerID, const std::string sDestinationCallerID);
	
	void NotifyResult(int iCommandID, int iResult, 
						const std::string Message);

	void NotifyHangup(const std::string sChannel, const std::string sReason);

	void NotifyLink(const std::string sSourceChannel, const std::string sDestinationChannel, 
		const std::string sSourceCallerID, const std::string sDestinationCallerID);

	void NotifyExtensionsStatus(std::string sStatus);

	void NotifyCallsStatus(std::string sStatus);

	void Close();

	void Set_Server_IP(std::string sIP) { m_sServerIP = sIP;  }
	std::string Get_Server_IP()         { return m_sServerIP; }
	
private:
	DCE::Asterisk* pAsterisk;
	ASTERISK::CommunicationHandler *m_pCommunicationHandler;
	std::string m_sServerIP;

// manager instance
private:
	static AsteriskManager* sinstance;
};

};

#endif
