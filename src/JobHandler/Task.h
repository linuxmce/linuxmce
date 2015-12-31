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

 /** @file Task.h
 Tasks.
 */
#ifndef Task_h
#define Task_h

#include <string>
using namespace std;
#include "PlutoUtils/MultiThreadIncludes.h"

/** @namespace nsJobHandler
For job handler
*/
namespace nsJobHandler
{
	typedef enum {
		TASK_NOT_STARTED = 0,
		TASK_IN_PROGRESS,
		TASK_FAILED_ABORT,  // Failed, and stop all the rest of the tasks
		TASK_FAILED_CONTINUE, // Failed, but keep doing the rest of the tasks
		TASK_COMPLETED,
		TASK_CANCELED
	} TaskStatus;


    /** @class Task
    For tasks.
    */
	class Task
	{
		friend class Job;
		TaskStatus m_eTaskStatus;

	protected:
		string m_sName;
		int m_iID;
		class Job *m_pJob;
		time_t m_tStarted;
		int m_iPercent;

	public:
		string m_sResult;

		Task(Job *pJob,string sName);
		virtual ~Task() {}

		virtual bool Abort();

		int m_iID_get() { return m_iID; }
		virtual string ToString() { return m_sName; }
		virtual string GetType()=0; // Should override
		virtual int Run()=0;  //!< Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms
		TaskStatus m_eTaskStatus_get() { return m_eTaskStatus; }
		void m_eTaskStatus_set(TaskStatus taskStatus);
		virtual int PercentComplete() { return m_iPercent; }
		virtual int SecondsRemaining()
		{
			if( m_iPercent==0 || m_tStarted==0 )
				return 0;

			int Duration = (int) (time(NULL) - m_tStarted);
			int TotalTime = Duration * 100 / m_iPercent;
			return TotalTime - Duration;
		}
	};
};

#endif
