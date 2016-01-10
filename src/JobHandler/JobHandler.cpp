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

void JobHandler::AddJob(Job *pJob)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::AddJob job %d %s",pJob->m_iID_get(),pJob->m_sName_get().c_str());
#endif
	BroadcastCond();
	m_listJob.push_back(pJob);
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
		Job *pJob_Next_Timed=NULL;
		PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
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
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"JobHandler::Run ready to run job %d %s status %d",
					pJob->m_iID_get(),pJob->m_sName_get().c_str(),(int) pJob->m_eJobStatus_get());
#endif
				pJob->StartThread();
			}

			// This Job wants to be called no later than a certain time.  See what is the next job (ie nearest next runattempt)
			if( pJob->m_tNextRunAttempt_get() )
			{
				if( !pJob_Next_Timed || pJob_Next_Timed->m_tNextRunAttempt_get()>pJob->m_tNextRunAttempt_get() )
					pJob_Next_Timed=pJob;
			}
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
			int Seconds = pJob_Next_Timed->m_tNextRunAttempt_get() - time(NULL);
			if( Seconds<0 )
				Seconds=0;
			jm.TimedCondWait( Seconds, 0 );  // Yes, wait only up until this job wants to be called again
		}
		else
			jm.CondWait();  // Nope, wait until something happens
	}
}

Job *JobHandler::FindJob(int jobID)
{
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

bool JobHandler::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	bool bPendingTasks=false;
	for(list<class Job *>::iterator it=m_listJob.begin();it!=m_listJob.end();++it)
	{
		Job *pJob = *it;
		bPendingTasks |= pJob->ReportPendingTasks(pPendingTaskList);
	}

	return bPendingTasks;
}

