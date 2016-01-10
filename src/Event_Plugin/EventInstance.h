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
#ifndef EVENTINSTANCE_H
#define EVENTINSTANCE_H

#include "Message.h"

class EventInstance
{
public:
	class EventHandler *m_ptrEventHandler;
	Message *pMessage;
	int m_ID,m_ActionNum,m_PKID_Device_OriginatedThisEvent;
	clock_t m_tTime;
//	clock_t m_ctWaitUntil;
	
	int m_TimerID;

	EventInstance(int ID,class EventHandler *ptrEventHandler)
	{
		pMessage=NULL;
		m_ActionNum=0;
		m_ptrEventHandler=ptrEventHandler;
		m_ID = ID;
		m_PKID_Device_OriginatedThisEvent=0;
	}

	~EventInstance()
	{
		if( pMessage!=NULL )
			delete pMessage;
	}
};

#endif

