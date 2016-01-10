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
/* Linphone SIP (LS) */

#ifndef SIP_THREAD_H
#define SIP_THREAD_H

#include "SimplePhone.h"
using namespace DCE;

/* C Object, public methods - forward declaration */
void * LS_Thread(void * arg);

bool LS_ActiveCall();
void LS_SentDTMF(char cDTMF);
void LS_InitiateCall(string sNumber);
bool LS_DropCall();
bool LS_AcceptCall();

/* C Object, public members - forward declaration */
extern bool LS_bQuit;

/* Configuration class */
class SimplePhoneConf
{
public:
	static SimplePhoneConf& Instance() { return m_Instance; }

	void Set_Server_IP(string sServerIP) { m_sServerIP = sServerIP; }
	string Get_Server_IP()               { return m_sServerIP;      }

private:
	SimplePhoneConf() {}

	static SimplePhoneConf m_Instance;
	string m_sServerIP;
};

#endif /* SIP_THREAD_H */
