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

#include "IdentifyJob.h"
#include "IdentifyTask.h"
#include "MoveDiscTask.h"
#include "JukeBox.h"
#include "DCE/Logger.h"
#include "DCE/Command_Impl.h"
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include "MoveDiscTask.h"

using namespace nsJobHandler;
using namespace DCE;

IdentifyJob::IdentifyJob(class JobHandler *pJobHandler,
			Disk_Drive_Functions *pDisk_Drive_Functions,
			Slot *pSlot,
			int PK_Orbiter,
			Command_Impl *pCommand_Impl)
	: Job(pJobHandler,"IdentifyJob",PK_Orbiter,pCommand_Impl)
{
	m_pDisk_Drive_Functions=pDisk_Drive_Functions;
	m_pSlot=pSlot;
}

IdentifyJob::~IdentifyJob()
{
	void *p_void;
	if( m_pDisk_Drive_Functions && m_pDisk_Drive_Functions->m_eLocked_get(&p_void)!=Disk_Drive_Functions::locked_available && p_void==(void *) this )
	{
		m_pDisk_Drive_Functions->UnlockDrive();
	}
}

bool IdentifyJob::ReadyToRun()
{
	if( m_pDisk_Drive_Functions )  // We're ready to go if we have a drive to do the ripping
	{
		if( !m_pDisk_Drive_Functions->LockDrive(Disk_Drive_Functions::locked_identify_job,this) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyJob::ReadyToRun cannot lock drive");
			return false; // We're not ready yet.  Somebody else is using the drive
		}

		if( !m_pDisk_Drive_Functions->m_pDevice_MediaIdentifier_get() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "IdentifyJob::ReadyToRun no media identifier");
			m_eJobStatus = job_Error;
			return false;
		}

		AddIdentifyTasks();
		return true;
	}

	if( !m_pSlot ) // If we don't have a m_pSlot, something is wrong, we have nothing and can't proceed
	{
		m_eJobStatus = job_Error;
		return false;
	}

	// Try to get an available drive from the jukebox to do the ripping
	m_pDisk_Drive_Functions = m_pSlot->m_pJukeBox->LockAvailableDrive(Drive::locked_identify_job,this,this,true);
	if( m_pDisk_Drive_Functions==NULL )  // We don't have a drive
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyJob::ReadyToRun no drive for slot %d",m_pSlot->m_SlotNumber);
		return false;
	}

	if( !m_pDisk_Drive_Functions->m_pDevice_MediaIdentifier_get() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "IdentifyJob::ReadyToRun no media identifier");
		m_eJobStatus = job_Error;
		return false;
	}

	AddTask(new MoveDiscTask(this,"SlotToDrive",MoveDiscTask::mdt_SlotToDrive,m_pSlot->m_pJukeBox,(Drive *) m_pDisk_Drive_Functions,m_pSlot));
	AddIdentifyTasks();
	AddTask(new MoveDiscTask(this,"DriveToSlot",MoveDiscTask::mdt_DriveToSlot,m_pSlot->m_pJukeBox,(Drive *) m_pDisk_Drive_Functions,m_pSlot));
	return true;
}

void IdentifyJob::AddIdentifyTasks()
{
	AddTask(new IdentifyTask(this,m_pDisk_Drive_Functions,m_pCommand_Impl,"Identify"));
}

bool IdentifyJob::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	if( m_eJobStatus==job_WaitingToStart || m_eJobStatus==job_InProgress )
	{
		if( pPendingTaskList )
		{
			int PK_Device = m_pDisk_Drive_Functions ? m_pDisk_Drive_Functions->m_pCommand_Impl_get()->m_dwPK_Device : m_pCommand_Impl->m_dwPK_Device;
			pPendingTaskList->m_listPendingTask.push_back(new PendingTask(m_iID,PK_Device,
				PK_Device,
				"identify",ToString(),(char) PercentComplete(),SecondsRemaining(),true));
		}

		return true;
	}
	else
		return false;
}

int IdentifyJob::PercentComplete()
{ 
	int iTaskNum=0;  // This will be which task we're working on
	Task *pTask_Current=NULL;
	for(list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
	{
		Task *pTask = *it;
		if( pTask->m_eTaskStatus_get()==TASK_NOT_STARTED )
			break;
		pTask_Current=pTask;
		iTaskNum++;  // All the tasks
	}

	if( pTask_Current==NULL || iTaskNum==0 )
		return 0; // We haven't started yet

	int RangeThisTask = 100 / m_listTask.size(); // What range each task occupies in the percentage of the total
	int ScaledPercent = pTask_Current->PercentComplete() * RangeThisTask / 100;  // What percentage this task is done, scaled to the range
	// if there are 3 tasks, the range is 33%.  if a task is 50% done, it represents 16.5% of the total (ScaledPercent)

	return (ScaledPercent*(iTaskNum-1)) /* for the tasks we've done so far */ + ScaledPercent /* plus the current task */;
}

int IdentifyJob::SecondsRemaining()
{ 
	return 0; 
}

string IdentifyJob::ToString()
{ 
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	int iTaskNum=0;  // This will be which task we're working on
	Task *pTask_Current=NULL;
	for(list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
	{
		Task *pTask = *it;
		if( pTask->m_eTaskStatus_get()==TASK_NOT_STARTED )
			break;
		pTask_Current=pTask;
		iTaskNum++;  // All the tasks
	}

	string sTimeLeft;
	if( pTask_Current )
		sTimeLeft = "(" + StringUtils::SecondsAsTime(pTask_Current->SecondsRemaining()) + ")";

	string sSlot = m_pSlot ? " Slot " + StringUtils::itos(m_pSlot->m_SlotNumber) : "";
	string sDrive = m_pDisk_Drive_Functions ? " Drive " + StringUtils::itos(m_pDisk_Drive_Functions->m_dwPK_Device_get()) : "";
		
	return "Identifying disc" + sSlot + sDrive + ".  Stage " + StringUtils::itos(iTaskNum) + " of " + StringUtils::itos((int) m_listTask.size()) + " " + sTimeLeft + ": ";
}
