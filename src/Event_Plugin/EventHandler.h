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
#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

namespace DCE
{
	class CommandGroup;
}

using namespace DCE;

class EventHandler
{
public:
	unsigned long m_PK_EventHander,m_PK_Event;
	int m_OncePerSeconds;
	class Criteria *m_pCriteria;
	CommandGroup *m_pCommandGroup;
	time_t m_tLastFired;

	EventHandler(unsigned long PK_EventHander,unsigned long PK_Event,CommandGroup *pCommandGroup,class Criteria *pCriteria,int OncePerSeconds)
	{
		m_tLastFired=0;
		m_OncePerSeconds=OncePerSeconds;
		m_PK_EventHander = PK_EventHander;
		m_PK_Event = PK_Event;
		m_pCriteria = pCriteria;
		m_pCommandGroup = pCommandGroup;
	}
};

typedef list<EventHandler *> ListEventHandler;

#endif

