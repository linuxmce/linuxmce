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

/** @file Job.h
 Tasks.
 */
#ifndef Job_h
#define Job_h

#include <string>
#include <list>
#include "PlutoUtils/ThreadedClass.h"
#include "DCE/Command_Impl.h"

using namespace std;
using namespace nsThreadedClass;


/** @namespace DCE
For DCE
*/
namespace DCE
{
	class PendingTaskList;
}
using namespace DCE;


/** @namespace nsJobHandler
For DCE
*/
namespace nsJobHandler
{
	class Task;
	class JobHandler;

	typedef list<class Task *> ListTask;

    /** @class Job
    For tasks.
    */
	class Job : public ThreadedClass
	{
	public:
		enum JobStatus
		{
			job_WaitingToStart=0,
			job_InProgress,
			job_Error,
			job_Aborted,
			job_Done,
			job_WaitingForCallback  // Special case used in certain derived classes
		};

		enum TasklistPosition
		{
		    position_TasklistBegin,
		    position_TasklistEnd
		};

		enum enumReadyToRun
		{
			readyToRun_No,
			readyToRun_Yes
		};

	private:
		static int m_NextJobID;
		int m_NextTaskID;
		JobStatus m_eJobStatus;

	protected:
		friend class Task;
		friend class JobHandler;

		string m_sName;
		int m_iID;
		int m_iMaxTasks;
		class JobHandler *m_pJobHandler;
		ListTask m_listTask;
		bool m_bAutoDelete;
		unsigned char m_cPriority; // Priorities with lower numbers get run first if JobHandler::m_bRunByPriority is true

        /** Set this to non-zero if you want to your ReadyToRun() to be called no earlier than this time.
        Zero means don't call until something changes.
        */
		time_t m_tNextRunAttempt;
		timespec m_tsCreated, // The time this was added to the queue
			m_tsFirstRun; // The time this was first run

          /** If passed into the constructor, send this Orbiter a 'refresh' when something changes with this job.
          */
		int m_iPK_Orbiter;
		Command_Impl *m_pCommand_Impl;  //!< If passed, use this to refresh the orbiter above in RefreshOrbiter();

	public:
		Job(JobHandler *pJobHandler,string sName,int PK_Orbiter=0,Command_Impl *pCommand_Impl=NULL);
		virtual ~Job();

		// Override these for better reporting of the Job's progress
		virtual int PercentComplete() { return 0; }
		virtual int SecondsRemaining() { return 0; }
		virtual string ToString() { return "#" + StringUtils::itos(m_iID) + " P:" + StringUtils::itos((int) m_cPriority) + " " + GetType() + ":" + m_sName; }

		int m_iID_get() { return m_iID; }
		time_t m_tNextRunAttempt_get() { return m_tNextRunAttempt; }
		void m_tNextRunAttempt_set(time_t tNextRunAttempt) { m_tNextRunAttempt=tNextRunAttempt; }
		timespec m_tsCreated_get() { return m_tsCreated; }
		timespec m_tsFirstRun_get() { return m_tsFirstRun; }
		void m_tsFirstRun_set()	{ gettimeofday( &m_tsFirstRun, NULL ); }

		bool m_bAutoDelete_get() { return m_bAutoDelete; }
		void m_bAutoDelete_set( bool bAutoDelete ) { m_bAutoDelete=bAutoDelete; }

		int PendingTasks();
		virtual bool Abort(bool bRequeue=false);  // If bRequeue is true, then put it back in the queue so it can restart later

		void SetMaxTasks(int MaxTasks) { m_iMaxTasks=MaxTasks; }
		virtual bool StartThread();
		virtual bool StopThread(int iTimeout);
		virtual enumReadyToRun ReadyToRun() { return readyToRun_Yes; }

		virtual void JobDone() {} // Override to do something when the job is finished

		enum JobStatus m_eJobStatus_get() { return m_eJobStatus; }
#ifdef DEBUG
		void m_eJobStatus_set(enum JobStatus jobStatus,bool bLog=true);  // Set it to false when called from within a constructor so it doesn't error
#else
		void m_eJobStatus_set(enum JobStatus jobStatus) { m_eJobStatus=jobStatus; }
#endif

		unsigned char m_cPriority_get() { return m_cPriority; }
		void m_cPriority_set( unsigned char cPriority ) { m_cPriority=cPriority; }

		virtual bool CanHandleAnotherTask() { return m_iMaxTasks==0 || PendingTasks()<m_iMaxTasks; }

		virtual string GetType()=0;

		Task *GetNextTask();
		Task *FindTask(int taskID);
		void AddTask(Task *pTask, TasklistPosition position=position_TasklistEnd);
		void AddTasks(const vector<Task *> &vTasks, TasklistPosition position=position_TasklistEnd);

		string m_sName_get() { return m_sName; }
		void Reset(bool bDelete = true);

		virtual void Run();

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);  // override to accurately report this job

		void RefreshOrbiter();

		/** Be sure to grab a mutex before using this.
        like this: PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
        */
		const ListTask *m_listTask_get() { return &m_listTask; }
	};
};

#endif
