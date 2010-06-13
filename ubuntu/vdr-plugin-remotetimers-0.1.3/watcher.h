/*
 * watcher.h: monitor server's .update file
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _REMOTETIMERS_WATCHER__H
#define _REMOTETIMERS_WATCHER__H

#include <sys/types.h>
#include <sys/stat.h>
#include <vdr/thread.h>

class cUpdateWatcher: private cThread {
	private:
		static cUpdateWatcher* updateWatcher;
		cCondWait condWait;
		const char* serverUpdateFile;
		bool inSubDir;

		static dev_t DeviceId(const char* FileName);
	protected:
		virtual void Action();
	public:
		static cUpdateWatcher* GetInstance();
		static void DeleteInstance();

		void Reconfigure();
		cUpdateWatcher();
		virtual ~cUpdateWatcher();
};

#endif //_REMOTETIMERS_WATCHER__H
