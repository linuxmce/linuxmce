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

 /** @file EventInfo.h
For ???
 */
#ifndef EVENTINFO_H
#define EVENTINFO_H

#include "EventHandler.h"


/** @class EventInfo
For ???
*/
class EventInfo
{
public:
	int m_iPK_Event;
	class Event *pEvent;
	class PlutoEvents *m_pPlutoEvents;
	class Message *pMessage;

	class EventHandler *pEventHandler;
	class DeviceData_Router *pDevice;
	int PK_C_Mode_Room,PK_C_Mode_House;
	vector<class EventHandler *> m_vectEventHandlers;

	EventInfo(class PlutoEvents *plutoEvents,class Event *eventlist,class Message *message,class DeviceData_Router *device,int C_Mode_Room,int C_Mode_House) :
		pEvent(eventlist), m_pPlutoEvents(plutoEvents), pMessage(message),
		pEventHandler(NULL), pDevice(device),PK_C_Mode_Room(C_Mode_Room),PK_C_Mode_House(C_Mode_House)
	{};
	~EventInfo()
	{
		for(int i=0;i<(int) m_vectEventHandlers.size();++i)
		{
			delete m_vectEventHandlers[i];
		}
	}
};

#endif

