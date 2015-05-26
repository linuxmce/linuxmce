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

#include <stdio.h>
#include <stdlib.h>
#include <vdr/tools.h>
#include <vdr/plugin.h>
#include "conflict.h"
#include "i18n.h"

cLocalTimerConflicts LocalConflicts;
cRemoteTimerConflicts RemoteConflicts;

// cTimerConflict -------------------------------------------------

cTimerConflict::cTimerConflict(time_t Time, const char *Conflict, const char *Text):
		time(Time), text(Text) {
	char* ids = NULL;
	if (sscanf(Conflict, "%d|%d|%a[0-9#]", &id, &percent, &ids) == 3) {
		char *p;

		// number of IDs is one more than number of '#'
		int count = 1;
		for (p = ids; *p; ++p) {
			if (*p == '#')
				++count;
		}

		timerIds = (int*) malloc(sizeof(int) * (count + 1));
		// convert IDs into numbers
		p = ids;
		for (int i = 0; i < count; ++i) {
			timerIds[i] = strtol(p, &p, 10);
			++p;
		}
		timerIds[count] = 0;

		free(ids);
	}
	else {
		id = 0;
		percent = 0;
		timerIds = NULL;
	}
}

cTimerConflict::~cTimerConflict()
{	free(timerIds); }

// cTimerConflicts -------------------------------------------------
cTimerConflicts::cTimerConflicts() {
	needsUpdate = false;
}

void cTimerConflicts::AddConflicts(const char *Line) {
	char *p = NULL;
	const char *l = Line;
	time_t time = (time_t) strtol(l, &p, 10);
	while (p && p != l && *p == ':') {
		l = ++p;
		p = strchr(p, ':');
		Add(new cTimerConflict(time, l, Line));
		// multiple conflicts for same time: store Line only once
		Line = NULL;
	}
}

bool cTimerConflicts::HasConflict(int TimerId) const
{
	for (cTimerConflict *c = First(); c; c = Next(c)) {
		if (c->Id() == TimerId)
			return true;
	}
	return false;
}

// cLocalTimerConflicts -------------------------------------------------
cLocalTimerConflicts::cLocalTimerConflicts() {
	hasEpgsearch = true;
	pluginEpgsearch = NULL;
}

void cLocalTimerConflicts::Update() {
	Epgsearch_services_v1_1 epgsearch;
	if (hasEpgsearch) {
		if (!pluginEpgsearch) {
			pluginEpgsearch = cPluginManager::CallFirstService("Epgsearch-services-v1.1", &epgsearch);
			hasEpgsearch = pluginEpgsearch != NULL;
		}
		else
			pluginEpgsearch->Service("Epgsearch-services-v1.1", &epgsearch);
	}
	if (hasEpgsearch) {
		
		needsUpdate = false;
		Clear();
		std::list<std::string> conflicts = epgsearch.handler->TimerConflictList();
		for (std::list<std::string>::iterator it = conflicts.begin(); it != conflicts.end(); it++)
			AddConflicts((*it).c_str());
	}
}

// cRemoteTimerConflicts -------------------------------------------------
cRemoteTimerConflicts::cRemoteTimerConflicts() {
	cmd.command = "PLUG epgsearch LSCC\r\n";
}

void cRemoteTimerConflicts::Update() {
	needsUpdate = false;
	Clear();
	if (!cSvdrp::GetInstance()->Offline())
	{
		cSvdrp::GetInstance()->Send(&cmd);
		if (cmd.responseCode == 900) {
			for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line))
				AddConflicts(line->Text());
		}
	}
}
