/*
 * remotetimers.h: Public interface of the plugin's services
 *
 * Copyright (C) 2008-2013 Frank Schmirler <vdr@schmirler.de>
 *
 * This file is part of VDR Plugin remotetimers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */

#ifndef _SERVICE__H
#define _SERVICE__H

#include <vdr/timers.h>
#include <vdr/epg.h>
#include <vdr/osdbase.h>

/*
 * If the Data argument is NULL, all service calls return true.
 * Otherwise the return value indicates success or failure of the service call.
 *
 * The service calls are not thread safe and must be called from the VDR main loop.
 */

/*
 * RemoteTimers::InstantRecording-v1.0
 * Start an instant recording or pause live TV. VDR needs to be patched to support this.
 * The service returns false if a local timer should be used. An error occured if true is returned but the out parameters name and fileName are NULL.
 * Data points to the following structure where pause indicates if it is an instant recording or an attempt to pause live TV.
 */

struct RemoteTimers_InstantRecording_v1_0 {
//in
	const cTimer	*timer;
	bool		pause;
	const cEvent	*event;
//out
	cString		name;
	cString		fileName;
};

/* 
 * RemoteTimers::RefreshTimers-v1.0
 * Fetch timer list from remote VDR. You must call this service before you can use one of the service calls below.
 * Data points to a cString which in case of failure (service call returns false) contains an error message.
 */

//out
//	cString errorMsg;

/*
 * RemoteTimers::ForEachConflict-v1.0
 * Iterates the list of remote timer conflicts.
 * The service call always returns true.
 * Data points to a const char* which must be NULL to return the first conflict. Pass the previously returned conflict to get the next one until const char* is NULL.
 *
 */
//in+out
//	const char* conflict;

/*
 * RemoteTimers::ForEach-v1.0
 * Iterates the list of remote timers.
 * The service call always returns true.
 * Data points to a cTimer* which must be NULL to return the first timer. Pass the previously returned timer to get the next one until cTimer* is NULL.
 *
 * RemoteTimers::GetTimer-v1.0
 * Test if the timer exists as either a remote or a local timer. 
 * The service call always returns true.
 * Data points to a cTimer* which points to the timer you are looking for. If found, cTimer* will point to the timer, otherwise it will be NULL.
 */

//in+out
//	cTimer* timer;

/*
 * RemoteTimers::GetMatch-v1.0
 * Find the remote or local timer which matches the event best.
 * The service call always returns true.
 * Data points to the following structure:
 */

struct RemoteTimers_GetMatch_v1_0 {
//in
	const cEvent	*event;
//out
	cTimer		*timer;
	int		timerMatch;
	int		timerType;
	bool		isRemote;
};

/*
 * RemoteTimers::GetTimerByEvent-v1.0
 * Find the remote or local timer matching the event.
 * If no timer matches, the service call returns false.
 * Data points to a RemoteTimers_Event_v1_0 struct.
 *
 * RemoteTimers::NewTimerByEvent-v1.0
 * Add a new timer for an event.
 * In case of an error, the service call returns false and the structure includes an error message.
 * Data points to a RemoteTimers_Event_v1_0 struct.
 */

struct RemoteTimers_Event_v1_0 {
//in
        const cEvent    *event;
//out
	cTimer		*timer;
	cString		errorMsg;
};

/*
 * RemoteTimers::NewTimer-v1.0
 * Add a new timer.
 * In case of an error, the service call returns false and the structure includes an error message.
 * Data points to a RemoteTimers_Timer_v1_0 struct.
 *
 * RemoteTimers::ModTimer-v1.0
 * Change an existing timer.
 * In case of an error, the service call returns false and the structure includes an error message.
 * Data points to a RemoteTimers_Timer_v1_0 struct.
 *
 * RemoteTimers::DelTimer-v1.0
 * Delete an existing timer.
 * In case of an error, the service call returns false and the structure includes an error message.
 * Data points to a RemoteTimers_Timer_v1_0 struct.
 */

struct RemoteTimers_Timer_v1_0 {
//in+out
	cTimer		*timer;
//out
	cString		errorMsg;
};

/*
 * RemoteTimers::GetTimerById-v1.0
 * Get a remote timer by its id (i.e. timer->Index()+1 on remote machine).
 * The service call always returns true.
 * Data must point to a RemoteTimers_Timer_v1_1 structure. errorMsg is unused.
 * NULL is returned as timer if no remote timer exists for the given id locally.
 * Note that a timer with this id may exist remotely. This can happen if the
 * remote timer's channel doesn't exist on the local machine.
 */

struct RemoteTimers_Timer_v1_1 {
//in+out
	cTimer		*timer;
	int		id;
//out
	cString		errorMsg;
};

/*
 * RemoteTimers::Menu-v1.0
 * Depending on the state parameter, open the Timers or Schedule menu.
 * In case of an error, menu is NULL.
 * Data points to a RemoteTimers_Menu_v1_0 struct.
 */
struct RemoteTimers_Menu_v1_0 {
//in
	const char	*serverIp;
	unsigned short	serverPort;
	eOSState	state;
//out
	cOsdMenu	*menu;
};
#endif //_SERVICE__H
