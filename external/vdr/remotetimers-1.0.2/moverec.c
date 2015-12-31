/*
 * moverec.c: Move VDR recording to a different filesystem
 *
 * Copyright (C) 2011 Frank Schmirler <vdr@schmirler.de>
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

#include "moverec.h"
#include "setup.h"
#include "i18n.h"
#include "vdr/recording.h"

#include <fcntl.h>
#include <utime.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int cMoveRec::CopyAttr(const struct stat* st, const char* dst)
{
	struct utimbuf utbuf;
	utbuf.actime = st->st_atime;
	utbuf.modtime = st->st_mtime;
	return utime(dst, &utbuf) +
			 chmod(dst, st->st_mode) +
			 chown(dst, st->st_uid, st->st_gid);
}

int cMoveRec::CopyFile(const char* src, const char* dst)
{
	int in, out;
	int result = -1;

	if ((in = open(src, O_RDONLY)) >= 0) {
		if ((out = open(dst, O_WRONLY|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR)) >= 0) {
			// find optimal buffer size for copying
			struct stat stSrc, stDst;
			blksize_t blksize = BUFSIZ;
			if (stat(src, &stSrc) == 0 && stat(dst, &stDst) == 0)
				blksize = (stSrc.st_blksize > stDst.st_blksize ? stSrc.st_blksize : stDst.st_blksize);
			char buffer[blksize];

			cTimeMs timer;
			long throttle = 0UL;
			unsigned int byte_per_ms = (unsigned int) RemoteTimersSetup.moveBandwidth * 1024 * 1024 / 8 / 1000;
			while ((result = safe_write(out, buffer, safe_read(in, buffer, blksize))) > 0) {
				// thread aborted?
				if (!Running()) {
					result = -1;
					errno = EINTR;
					break;
				}
				// bandwidth throttle
				if (byte_per_ms) {
					throttle += timer.Elapsed() * byte_per_ms - result;
					timer.Set();
					if (throttle < blksize)
					    cCondWait::SleepMs((blksize - throttle) / byte_per_ms);
				}
			}

			if (result == 0) {
				struct stat st;
				if (fstat(in, &st) == 0) {
					if (CopyAttr(&st, dst) < 0)
						esyslog("remotetimers: Failed to copy file attributes from %s to %s: %m", src, dst);
				}
				else
					esyslog("remotetimers: Failed to read file attributes from %s: %m", src);

				if ((result = fsync(out)) == 0)
					result = close(out);
			}

			if (result < 0) {
				esyslog("remotetimers: An error occured while copying %s to %s: %m", src, dst);
				close(out);
				if (unlink(dst) < 0)
					esyslog("remotetimers: Cannot unlink %s: %m", dst);
			}
		}
		else
			esyslog("remotetimers: Failed to open %s for writing: %m", dst);
		close(in);
	}
	else
		esyslog("remotetimers: Failed to open %s for reading: %m", src);
	return result;
}

int cMoveRec::CopyLink(const char* src, const char* dst)
{
	char buffer[1024];

	ssize_t s = readlink(src, buffer, sizeof(buffer));
	if (s <= 0)
		esyslog("remotetimers: Failed to read link %s: %m", src);
	else if (s + 1 < (ssize_t) sizeof(buffer)) {
		buffer[s] = 0;
		if (symlink(buffer, dst) == 0) {
			struct stat st;
			if (stat(dst, &st) == 0)
				return 0;
			else
				esyslog("remotetimers: Failed to stat %s: %m", dst);
		}
		else
			esyslog("remotetimers: Failed to create symlink %s: %m", dst);
	}
	return -1;
}

int cMoveRec::MoveDir(cReadDir& dir, const char* srcdir, const char* dstdir)
{
	struct dirent *e = dir.Next();
	if (e != NULL) {
		cString srcfile = AddDirectory(srcdir, e->d_name);
		cString dstfile = AddDirectory(dstdir, e->d_name);

		struct stat st;
		if (strcmp(".", e->d_name) == 0 || strcmp("..", e->d_name) == 0)
			return MoveDir(dir, srcdir, dstdir);
		else if (!Running()) {
			isyslog("remotetimers: Copying %s to %s aborted", *srcfile, *dstfile);
			return -1;
		}
		else if (access(srcfile, R_OK|W_OK) == 0) {
			if (lstat(srcfile, &st) == 0) {
				if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
					if ((S_ISREG(st.st_mode) && CopyFile(srcfile, dstfile) == 0) ||
						(S_ISLNK(st.st_mode) && CopyLink(srcfile, dstfile) == 0)) {
						if (MoveDir(dir, srcdir, dstdir) == 0) {
							// success
							return 0;
						}
						else {
							// failure: remove dstfile
							if (unlink(dstfile) < 0)
								esyslog("remotetimers: Cannot unlink %s: %m", *dstfile);
							return -1;
						}
					}
					else
						return -1;
				}
				else {
					esyslog("remotetimers: Cannot copy %s: Not a plain file or symbolic link", *srcfile);
					return -1;
				}
			}
			else {
				esyslog("remotetimers: Failed to stat %s: %m", *srcfile);
				return -1;
			}
		}
		else {
			esyslog("remotetimers: File %s: %m", *srcfile);
			return -1;
		}
	}
	return 0;
}

cMoveRec* cMoveRec::moveRec = NULL;

cMoveRec::cMoveRec(): cThread("remotetimers MoveRec")
{}

cMoveRec* cMoveRec::GetInstance()
{
	if (!moveRec)
		moveRec = new cMoveRec();
	return moveRec;
}

bool cMoveRec::IsMoving()
{	return moveRec && moveRec->Active(); }

void cMoveRec::Abort(int WaitSeconds)
{
	if (IsMoving())
		moveRec->Cancel(WaitSeconds);
}

bool cMoveRec::Move(const cRecording* Recording, const char* DstDir)
{
	if (Running())
		return false;
	srcDir = Recording->FileName();
	dstDir = DstDir;
	return Start();
}

void cMoveRec::Action()
{
	SetPriority(19);
	SetIOPriority(7);
	cReadDir dir(srcDir);
	if (dir.Ok()) {
		if (MakeDirs(dstDir, true)) {
			if (MoveDir(dir, srcDir, dstDir) < 0)
				RemoveFileOrDir(dstDir);
			else {
				cThreadLock recordingsLock(&Recordings);
				cRecording* rec = Recordings.GetByName(srcDir);
				if (rec) {
					rec->Delete();
					Recordings.DelByName(srcDir);
				}
				Recordings.AddByName(dstDir);
				Skins.QueueMessage(mtInfo, trREMOTETIMERS("Finished moving recording"));
				return;
			}
		}
	}
	Skins.QueueMessage(mtError, trREMOTETIMERS("Failed to move recording!"));
}
