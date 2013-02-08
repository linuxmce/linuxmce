/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com


 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the GNU Public License, available at:
 http://www.fsf.org/licensing/licenses/gpl.html

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.

 */

 /** @file EventHandler.h
For ???
 */
#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

typedef vector<class Command *> CommandVector;


/** @class EventHndler
For ???
*/
class EventHandler
{
public:
	bool m_bExecuted;
	int m_PK_EventHander;
	class EventHandler_CriteriaList *m_ptrCriteria;
	class EventInstance *m_ptrEventInstance;
	class Event *m_ptrEvent;
	class CommandGroup *m_ptrCommandGroup;
	vector<class CriteriaParm *> m_vectCriteriaParm;

	EventHandler(int ID,class Event *ptrEvent,class CommandGroup *ptrCommandGroup,class EventHandler_CriteriaList *ptrCriteria)
	{
		m_PK_EventHander = ID;
		m_ptrCriteria = ptrCriteria;
		m_ptrEvent = ptrEvent;
		m_ptrCommandGroup = ptrCommandGroup;
		m_ptrEventInstance = NULL;
		m_bExecuted=false;
	}
};

#endif

