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

#ifndef _REMOTETIMERS_MOVEDIR_H
#define _REMOTETIMERS_MOVEDIR_H

#include <vdr/thread.h>
#include <vdr/tools.h>

class cRecording;

class cMoveRec: public cThread {
	private:
		cString srcDir;
		cString dstDir;

		int CopyAttr(const struct stat* st, const char* dst);
		int CopyFile(const char* src, const char* dst);
		int CopyLink(const char* src, const char* dst);
		int MoveDir(cReadDir& dir, const char* srcdir, const char* dstdir);

		static cMoveRec* moveRec;
		cMoveRec();
	protected:
		virtual void Action();
	public:
		static bool IsMoving();
		static void Abort(int WaitSeconds = 0);
		static cMoveRec* GetInstance();
		bool Move(const cRecording* Recording, const char* DstDir);
};

#endif
