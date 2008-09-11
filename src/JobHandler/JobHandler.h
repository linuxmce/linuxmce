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

/** @file JobHandler.h
 Tasks.

A JobHandler lets you add job's and tasks which need to be done asynchronously.

A Job can be something like Rip a Disc, Generate an Orbiter's GUI, etc.
Derive a class from Job for your particular type of job.  The JobHandler
will check your Job to see if it's ready to do something, and if so, make the
Job start.  The Job will be started in it's own thread.  Several Jobs may
run in parallel up to the maximum specified.

Within a job are a series of Tasks which much be run in order.  You do not need to
use the task mechanism, and can instead handle the job yourself directly by overriding the
Run() function to do whatever you like.

However, you can also create classes drived from Task to do all the tasks that need to be done
in order and add them to Job, and let Job::Run() run them in order.  Note that the Job's ReadyToRun
function can return true, meaning it's ready to go and Run() will be called, or return false, meaning
it's not ready, check it later.  If ReadyRoRun() sets m_tNextRunAttempt to a time then it will be checked
again at that time.  Otherwise it won't be checked again until JobHandler::SomethingChanged is called.

Both Job::Run and Task::Run need to keep checking m_bAbort and if it's true,
return and set the status to ABORTED.

*/

#ifndef JobHandler_h
#define JobHandler_h

#include <string>
using namespace std;
#include "PlutoUtils/ThreadedClass.h"

#include "Job.h"

/** @namespace DCE
For DCE
*/
namespace DCE
{
	class PendingTaskList;
}
using namespace DCE;


/** @namespace nsJobHandler
For JobHandler
*/
namespace nsJobHandler
{
	typedef list<class Job *> ListJob;

    /** @class JobHandler
    For deriving jobs.
    @see file JobHandler.h.
    */
	class JobHandler : public nsThreadedClass::ThreadedClass
	{
	private:
		ListJob m_listJob;
		bool m_bProcessingSuspended, m_bSuspendProcessing;

	protected:
		/* If this m_bMultiThreadedJobs is true, the default, then StartThread() will 
		be called so all jobs can run in parallel.  Set it to false to run only 1 job
		at a time in the same thread as the JobHandler's main Run() thread
		*/
		bool m_bMultiThreadedJobs;

	public:
		JobHandler();
		virtual ~JobHandler();

		bool AbortAllJobs();
		void PurgeCompletedJobs();
		bool WaitForJobsToFinish(bool bAbort=true,int iSeconds=5);  //!< By default abort and wait 5 seconds for jobs to finish

		void AddJob(Job *pJob,bool bFirst=false);
		bool MoveJobToEndOfQueue(Job *pJob);
		string ToString();
		bool ContainsJob(string sName);
		void StateChanged();

		bool HasJobs();

		virtual void Run();

		/** Be sure to grab a mutex before using this.
        like this: PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
        */
		Job *FindJob(int jobID);

		// Returns the number of jobs with the indicated status
		int NumberJobsWithStatus(Job::JobStatus eJobStatus);

        /** Be sure to grab a mutex before using this.
        like this: PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
        */
		class Task *FindTask(int jobID,int taskID);

        /** Be sure to grab a mutex before using this.
        like this: PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
        */
		const ListJob *m_listJob_get() { return &m_listJob; }

		bool ReportPendingTasks(PendingTaskList *pPendingTaskList,string sType="");

		// This function will block until all jobs in progress are not running anymore and the Run() loop is in a suspended state, and return true
		// If it doesn't happen within iTimeoutInSeconds seconds, it will return false.  iTimeoutInSeconds==0 means wait forever
		bool SuspendProcessing(int iTimeoutInSeconds);

		// Resume processing after calling suspend processing
		void ResumeProcessing();
	};
};

#endif
