/*
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

#ifndef _REMOTETIMERS_CONFLICT__H
#define _REMOTETIMERS_CONFLICT__H

#include <vdr/tools.h>
#include "svdrp.h"
#include "include/epgsearch.h"

class cTimerConflict: public cListObject {
private:
	time_t time;
	int id;
	int percent;
	int* timerIds;
	cString text;
public:
	time_t Time() const { return time; }
	int Id() const { return id; }
	int Percent() const { return percent; }
	const int* With() const { return timerIds; }
	// Raw string as received from epgsearch.
	// In case of multiple conflicts at the same time, a string with all conflicts is returned with the first conflict.
	// It is NULL for all further conflicts.
	const char* Text() const { return text; }
	cTimerConflict(time_t Time, const char *Conflict, const char *Text);
	virtual ~cTimerConflict();
};

class cTimerConflicts: public cList<cTimerConflict> {
protected:
        bool needsUpdate;
	void AddConflicts(const char *Line);
public:
	bool NeedsUpdate() const { return needsUpdate; }
	void SetNeedsUpdate() { needsUpdate = true; }
	virtual void Update() = 0;
	bool HasConflict(int TimerId) const;
	cTimerConflicts();
};

class cLocalTimerConflicts: public cTimerConflicts {
private:
	bool hasEpgsearch;
	cPlugin* pluginEpgsearch;
public:
	virtual void Update();
	cLocalTimerConflicts();
};

class cRemoteTimerConflicts: public cTimerConflicts {
private:
	SvdrpCommand_v1_0 cmd;
public:
	virtual void Update();
	cRemoteTimerConflicts();
};

extern cLocalTimerConflicts LocalConflicts;
extern cRemoteTimerConflicts RemoteConflicts;

#endif //_REMOTETIMERS_CONFLICT__H
