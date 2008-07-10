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

#include "TimedEvent.h"

using namespace DCE;

class EventHandler
{
public:
	unsigned long m_PK_EventHander,m_PK_Event,m_PK_CannedEvents;
	int m_OncePerSeconds;
	class Criteria *m_pCriteria;
	time_t m_tLastFired;
	bool m_bDisabled;
	map<int, Command_Data> m_mapCommands;
	string m_sDescription;
	class Event_Plugin *m_pEvent_Plugin;

	EventHandler(unsigned long PK_EventHander,Event_Plugin *pEvent_Plugin,struct json_object *json_obj);
	void ParseCriteria(struct json_object *json_obj);
};

typedef list<EventHandler *> ListEventHandler;

#endif

