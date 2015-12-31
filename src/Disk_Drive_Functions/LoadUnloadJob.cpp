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

#include "LoadUnloadJob.h"
#include "IdentifyTask.h"
#include "MoveDiscTask.h"
#include "JukeBox.h"
#include "DCE/Logger.h"
#include "DCE/Command_Impl.h"
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include "MoveDiscTask.h"

using namespace nsJobHandler;
using namespace DCE;

LoadUnloadJob::LoadUnloadJob(class JobHandler *pJobHandler,
			LoadUnloadJobType loadUnloadJobType,
			JukeBox *pJukeBox,
			Drive *pDrive,
			Slot *pSlot,
			int PK_Orbiter,
			Command_Impl *pCommand_Impl)
	: Job(pJobHandler,"LoadUnloadJob",PK_Orbiter,pCommand_Impl)
{
	m_pDrive=pDrive;
	m_pSlot=pSlot;
	m_pJukeBox=pJukeBox;
	m_eLoadUnloadJobType=loadUnloadJobType;
}

LoadUnloadJob::~LoadUnloadJob()
{
	void *p_void;
	if( m_pJukeBox->m_eLocked_get(&p_void)!=Disk_Drive_Functions::locked_available && p_void==(void *) this )
		m_pJukeBox->UnlockJukebox();

	if( m_pDrive && m_pDrive->m_eLocked_get(&p_void)!=Disk_Drive_Functions::locked_available && p_void==(void *) this )
		m_pDrive->UnlockDrive();
}

bool LoadUnloadJob::ReadyToRun()
{
	// If we're going to load/unload from a drive, be sure we can lock it
	if( m_pDrive && !m_pDrive->LockDrive(Disk_Drive_Functions::locked_move,(void *)this) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"LoadUnloadJob::ReadyToRun cannot lock drive");
		return false; // We're not ready yet.  Somebody else is using the drive
	}

	if( m_eLoadUnloadJobType==eMoveFromSlotToDrive || m_eLoadUnloadJobType==eMoveFromDriveToSlot ) // If we don't have a m_pSlot/drive, something is wrong, we have nothing and can't proceed
	{
		if( !m_pSlot || !m_pDrive )
		{
			m_eJobStatus = job_Error;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LoadUnloadJob::ReadyToRun no slot/drive for move operation");
			return false;
		}
		if( m_pDrive->LockDrive(Drive::locked_move,this)==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"LoadUnloadJob::ReadyToRun 1 drive is already locked");
			return false;
		}

		if( m_eLoadUnloadJobType==eMoveFromSlotToDrive )
			AddTask(new MoveDiscTask(this,"SlotToDrive",MoveDiscTask::mdt_SlotToDrive,m_pJukeBox,m_pDrive,m_pSlot));
		else
			AddTask(new MoveDiscTask(this,"DriveToSlot",MoveDiscTask::mdt_DriveToSlot,m_pJukeBox,m_pDrive,m_pSlot));
		return true;
	}

	if( m_eLoadUnloadJobType==eEjectOneDisc )
	{
		if( m_pDrive )  // We're ejecting the disk in a drive
		{
			if( m_pDrive->LockDrive(Drive::locked_move,this)==false )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"LoadUnloadJob::ReadyToRun 2 drive is already locked");
				return false;
			}

			PLUTO_SAFETY_LOCK(dm,m_pJukeBox->m_DriveMutex_get());
			Slot *pSlot = m_pJukeBox->m_mapSlot_Empty();  // Get an empty slot to move it to
			if( !pSlot )
			{
				m_eJobStatus = job_Error;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LoadUnloadJob::ReadyToRun 7 no slot for load operation");
				return false;
			}
			pSlot->m_eStatus = Slot::slot_intransit;

			AddTask(new MoveDiscTask(this,"DriveToSlot",MoveDiscTask::mdt_DriveToSlot,m_pJukeBox,m_pDrive,pSlot));
			AddTask(new MoveDiscTask(this,"SlotToEject",MoveDiscTask::mdt_SlotToEject,m_pJukeBox,NULL,pSlot));
			return true;
		}
		else if( !m_pSlot )
		{
			m_eJobStatus = job_Error;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LoadUnloadJob::ReadyToRun no slot for eject operation");
			return false;
		}
		else
		{
			AddTask(new MoveDiscTask(this,"SlotToEject",MoveDiscTask::mdt_SlotToEject,m_pJukeBox,NULL,m_pSlot));
			return true;
		}
	}
	else if( m_eLoadUnloadJobType==eEjectMultipleDiscs )
	{
		AddTask(new MoveDiscTask(this,"SlotToEject",MoveDiscTask::mdt_SlotToEject,m_pJukeBox,NULL,NULL));  // NULL Slot means all slots
		return true;
	}
	else if( m_eLoadUnloadJobType==eLoadOneDisc )
	{
		if( !m_pSlot )
		{
			PLUTO_SAFETY_LOCK(dm,m_pJukeBox->m_DriveMutex_get());
			m_pSlot = m_pJukeBox->m_mapSlot_Empty();
			if( !m_pSlot )
			{
				m_eJobStatus = job_Error;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LoadUnloadJob::ReadyToRun no slot for load operation");
				return false;
			}
			m_pSlot->m_eStatus = Slot::slot_intransit;
		}
		AddTask(new MoveDiscTask(this,"Load",MoveDiscTask::mdt_Load,m_pJukeBox,NULL,m_pSlot));
		return true;
	}
	else if( m_eLoadUnloadJobType==eLoadMultipleDiscs )
	{
		AddTask(new MoveDiscTask(this,"Load",MoveDiscTask::mdt_Load,m_pJukeBox,NULL,NULL)); // NULL slot means multiple
		return true;
	}

	m_eJobStatus = job_Error;
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LoadUnloadJob::ReadyToRun unknown operation");
	return false;
}

bool LoadUnloadJob::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	if( m_eJobStatus==job_WaitingToStart || m_eJobStatus==job_InProgress )
	{
		if( pPendingTaskList )
		{
			pPendingTaskList->m_listPendingTask.push_back(new PendingTask(m_iID,m_pJukeBox->m_pCommand_Impl->m_dwPK_Device,
				m_pJukeBox->m_pCommand_Impl->m_dwPK_Device,
				"identify",ToString(),(char) PercentComplete(),SecondsRemaining(),true));
		}

		return true;
	}
	else
		return false;
}

int LoadUnloadJob::PercentComplete()
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

int LoadUnloadJob::SecondsRemaining()
{ 
	return 0; 
}

string LoadUnloadJob::ToString()
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
	return "Identifying disc.  Stage " + StringUtils::itos(iTaskNum) + " of " + StringUtils::itos((int) m_listTask.size()) + " " + sTimeLeft + ": ";
}
