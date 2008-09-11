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
#include "JobHandler.h"
#include "Job.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Command_Impl.h"

using namespace nsJobHandler;
using namespace DCE;

JobHandler::JobHandler()
{
	m_bMultiThreadedJobs=true;  // Derived versions can override this if there should be only 1 job at a time
	m_bProcessingSuspended=m_bSuspendProcessing=false;
}

JobHandler::~JobHandler()
{
	WaitForJobsToFinish();
	StopThread();
}

bool JobHandler::AbortAllJobs()
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	bool bAbortedOk=true;
	for(list<Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		Job *pJob = *it;
		if( !pJob->Abort() )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"JobHandler::AbortAllJobs cannot abort %s", pJob->m_sName.c_str());
			bAbortedOk=false;
		}
	}

	PurgeCompletedJobs();
	BroadcastCond();
	return bAbortedOk;
}

bool JobHandler::WaitForJobsToFinish(bool bAbort,int iSeconds)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::WaitForJobsToFinish");
#endif
	if( bAbort )
		AbortAllJobs();

	time_t tTimeout = time(NULL) + iSeconds;
	while(true)
	{
		PurgeCompletedJobs();

		if( HasJobs()==false )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::WaitForJobsToFinish COMPLETED OK");
#endif
			return true;
		}

		if( tTimeout<=time(NULL) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"JobHandler::WaitForJobsToFinish failed to complete");
			return false;  // Jobs failed to end in designated amount of time
		}

		Sleep(500); // Wait 1/2 second and check again
	}
}

void JobHandler::PurgeCompletedJobs()
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::PurgeCompletedJobs purge job %d",
		m_listJob.size());
#endif

	for(list<Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();)
	{
		Job *pJob = *it;
		if( pJob->m_eJobStatus_get()==Job::job_Done || pJob->m_eJobStatus_get()==Job::job_Aborted || pJob->m_eJobStatus_get()==Job::job_Error )
		{
			if( pJob->m_eJobStatus_get()==Job::job_Error )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"JobHandler::PurgeCompletedJobs purge job %d %s status %d",
					pJob->m_iID_get(),pJob->m_sName_get().c_str(),(int) pJob->m_eJobStatus_get());
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::PurgeCompletedJobs purge job %d %s status %d",
				pJob->m_iID_get(),pJob->m_sName_get().c_str(),(int) pJob->m_eJobStatus_get());
#endif
			pJob->StopThread(5);  // Job should immediately, but give it 5 seconds to be sure
			m_listJob.erase(it++);
			if( pJob->m_bAutoDelete_get() )
				delete pJob;
		}
		else
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::PurgeCompletedJobs keep job %d %s status %d",
				pJob->m_iID_get(),pJob->m_sName_get().c_str(),(int) pJob->m_eJobStatus_get());
#endif
			++it;
		}
	}
}

bool JobHandler::HasJobs()
{ 
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	return m_listJob.empty()==false; 
}

void JobHandler::AddJob(Job *pJob,bool bFirst)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::AddJob job %d %s",pJob->m_iID_get(),pJob->m_sName_get().c_str());
#endif
	BroadcastCond();
	if( bFirst )
		m_listJob.push_front(pJob);
	else
		m_listJob.push_back(pJob);
}

bool JobHandler::MoveJobToEndOfQueue(Job *pJob)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::MoveJobToEndOfQueue job %d %s",pJob->m_iID_get(),pJob->m_sName_get().c_str());
#endif
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		if( pJob==*it )
		{
			m_listJob.erase(it);
			m_listJob.push_back(pJob);
			BroadcastCond();
			return true;
		}
	}
	return false;
}

string JobHandler::ToString()
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	string sResult = StringUtils::itos((int) m_listJob.size()) + "\t";

	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
		sResult += (*it)->ToString() + "\t";
	return sResult;
}

bool JobHandler::ContainsJob(string sName)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
		if( (*it)->m_sName_get()==sName )
			return true;
	return false;
}

void JobHandler::Run()
{
	while(!m_bQuit)
	{
		Job *pJob_Next_Timed=NULL,*pJob_SingleThreaded=NULL;
		PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);

		if( m_bSuspendProcessing )  // We need to suspend until ResumeProcessing is called
		{
			if( m_bProcessingSuspended==false )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::Run Suspending processing");
				m_bProcessingSuspended = true;
			}
			jm.Release();
			Sleep(500);
			continue;
		}
		PurgeCompletedJobs();

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::Run jobs %d", m_listJob.size());
		
		for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
		{
			Job *pJob = *it;
			if( pJob->m_eJobStatus_get()!=Job::job_WaitingToStart )
				continue;

			if( pJob->ReadyToRun() )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::Run ready to run job %d %s status %d <%p>",
					pJob->m_iID_get(),pJob->m_sName_get().c_str(),(int) pJob->m_eJobStatus_get(), pJob);
#endif
				if( m_bMultiThreadedJobs==false )
				{
					pJob_SingleThreaded = pJob;  // Only run this one job
					break;
				}
				else
				{
					pJob->StartThread();
				}
			}

			// This Job wants to be called no later than a certain time.  See what is the next job (ie nearest next runattempt)
			if( pJob->m_tNextRunAttempt_get() )
			{
				if( !pJob_Next_Timed || pJob_Next_Timed->m_tNextRunAttempt_get()>pJob->m_tNextRunAttempt_get() )
					pJob_Next_Timed=pJob;
			}
		}

		// We've got a job to run as single threaded
		if( pJob_SingleThreaded )
		{
			jm.Release();
			pJob_SingleThreaded->Run();
			pJob_SingleThreaded = NULL;
			jm.Relock();
			continue;  // Start the loop again at the beginning since it may have changed while the mutex was unlocked
		}

		if( m_bQuit )
			return;

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::Run ready to sleep %d jobs %p",
			(int) m_listJob.size(), pJob_Next_Timed );
#endif

		// See if there's a time limit by which we should check the jobs again
		if( pJob_Next_Timed )
		{
			time_t Seconds = pJob_Next_Timed->m_tNextRunAttempt_get() - time(NULL);
			if( Seconds<0 )
				Seconds=0;
			jm.TimedCondWait( (int) Seconds, 0 );  // Yes, wait only up until this job wants to be called again
		}
		else
			jm.CondWait();  // Nope, wait until something happens
	}
}

Job *JobHandler::FindJob(int jobID)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		Job *pJob = *it;
		if( pJob->m_iID_get()==jobID )
			return pJob;
	}
	return NULL;
}

Task *JobHandler::FindTask(int jobID,int taskID)
{
	Job *pJob = FindJob(jobID);
	if( !pJob )
		return NULL;

	return pJob->FindTask(taskID);
}

bool JobHandler::ReportPendingTasks(PendingTaskList *pPendingTaskList,string sType)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	bool bPendingTasks=false;
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		Job *pJob = *it;
		if( sType.empty()==false && pJob->GetType()!=sType )
			continue; // Skip this.  The user only cares about a specific type of job
		bPendingTasks |= pJob->ReportPendingTasks(pPendingTaskList);
	}

	return bPendingTasks;
}

bool JobHandler::SuspendProcessing(int iTimeoutInSeconds)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::SuspendProcessing");
	time_t tTimeout = time(NULL) + iTimeoutInSeconds;

	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	m_bProcessingSuspended=false;  // So we know when the Run loop gets into the 'idle' mode
	m_bSuspendProcessing=true;  // Only set here and in ResumeProcessing

	jm.Release();
	while( !m_bProcessingSuspended || NumberJobsWithStatus(Job::job_InProgress)>0 )
	{
		BroadcastCond();
		if( (iTimeoutInSeconds && tTimeout < time(NULL)) || m_bQuit )
			return false;  // We never were able to suspend.  Some job must be 'stuck'
		Sleep(500);  // Sleep half a second
	}
	return true;
}

void JobHandler::ResumeProcessing()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::ResumeProcessing");
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	m_bSuspendProcessing=false;  // Only set here and in SuspendProcessing
}

int JobHandler::NumberJobsWithStatus(Job::JobStatus eJobStatus)
{
	int iCount=0;
	for(list<Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		Job *pJob = *it;
		if( pJob->m_eJobStatus_get()==eJobStatus )
			iCount++;
	}
	return iCount;
}


