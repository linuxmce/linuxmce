/*
 * Copyright (C) 2008-2011 Frank Schmirler <vdr@schmirler.de>
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

#ifndef _REMOTETIMERS_WATCHER__H
#define _REMOTETIMERS_WATCHER__H

#include <sys/types.h>
#include <sys/stat.h>
#include <vdr/thread.h>
#include <vdr/tools.h>

class cUpdateWatcher: private cThread {
		friend class cPluginRemotetimers;
	private:
		static cUpdateWatcher* updateWatcher;

		cCondWait condWait;
		cString   serverUpdateFile;
		time_t    serverLastModifiedTime;
		cString   clientUpdateFile;
		dev_t     clientLastDev;
		bool      inSubDir;

		static dev_t DeviceId(const char* FileName);
	protected:
		virtual void Action();
	public:
		static cUpdateWatcher* GetInstance();
		static void DeleteInstance();

		void Initialize();
		void Reconfigure();
		cUpdateWatcher();
		virtual ~cUpdateWatcher();
};

#endif //_REMOTETIMERS_WATCHER__H
