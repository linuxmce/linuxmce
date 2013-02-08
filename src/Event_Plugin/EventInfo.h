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
#ifndef EVENTINFO_H
#define EVENTINFO_H

#include "EventHandler.h"

namespace DCE
{
	class DeviceData_Router;
	class Message;
}

using namespace DCE;

class EventInfo
{
public:
	int m_iPK_Event;
	Message *m_pMessage;
	class EventHandler *m_pEventHandler;
	DeviceData_Router *m_pDevice;
	int m_PK_HouseMode;
	vector<class EventHandler *> m_vectEventHandlers;
	bool m_bTimedEvent;

	EventInfo(int PK_Event,Message *pMessage,DeviceData_Router *pDevice,int PK_HouseMode) :
		m_iPK_Event(PK_Event), m_pMessage(pMessage), m_pEventHandler(NULL), m_pDevice(pDevice),
		m_PK_HouseMode(PK_HouseMode), m_bTimedEvent(false)
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

