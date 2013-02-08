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

 /** @file EventList.h
For ???
 */
#ifndef EVENT_H
#define EVENT_H

/** @class Event
For ???
*/
class Event
{
public:
	int m_PK_Event;
	vector<class EventHandler *> m_vectEventHandlers;

	Event(int ID)
	{
		m_PK_Event = ID;
	}
};

#endif

