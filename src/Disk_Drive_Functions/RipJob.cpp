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

#include "RipJob.h"
#include "RipTask.h"
#include "DCE/Logger.h"
#include "DCE/Command_Impl.h"
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include "JukeBox.h"
#include "MoveDiscTask.h"
#include "FixupRippingInfoTask.h"
#include "AddRippingTasksTask.h"
#include "pluto_media/Table_DiscLocation.h"
#include "IdentifyTask.h"
#include "Gen_Devices/AllScreens.h"

using namespace nsJobHandler;
using namespace DCE;

RipJob::RipJob(Database_pluto_media *pDatabase_pluto_media,
			class JobHandler *pJobHandler,
			Disk_Drive_Functions *pDisk_Drive_Functions,
			Slot *pSlot,
			int iPK_Users, int iEK_Disc,
			int iPK_Orbiter,
			string sFormat, string sFileName, string sDirectory, string sTracks,
			bool bReportResult,
			Command_Impl *pCommand_Impl)
	: Job(pJobHandler,"RipJob",iPK_Orbiter,pCommand_Impl)
{
	m_bReportResult=bReportResult;
	m_pDatabase_pluto_media=pDatabase_pluto_media;
	m_pDisk_Drive_Functions=pDisk_Drive_Functions;
	m_pSlot=pSlot;
	m_iPK_Users=iPK_Users;
	m_iEK_Disc=iEK_Disc;
	m_iPK_MediaType=0;  // Fix ripping in the disc drive will fill this in
	m_sFormat=sFormat;
	m_sFileName=sFileName;
	m_sDirectory=sDirectory;
	m_sTracks=sTracks;
	m_nTracksFailedToRip = 0;
	m_bHasErrors=false;


	string sWhere;
	if( m_pDisk_Drive_Functions )
	{
		sWhere = "EK_Device = " + StringUtils::itos(m_pDisk_Drive_Functions->m_dwPK_Device_get());
		if( m_pSlot )
			sWhere += " AND Slot = " + StringUtils::itos(m_pSlot->m_SlotNumber);
	}
	else if( m_pSlot )
		sWhere = "EK_Device = " + StringUtils::itos(m_pSlot->m_pJukeBox->m_pCommand_Impl->m_dwPK_Device) +
			" AND Slot = " + StringUtils::itos(m_pSlot->m_SlotNumber);

	if( sWhere.empty() )
		m_pRow_DiscLocation=NULL;
	else
	{
		vector<Row_DiscLocation *> vectRow_DiscLocation;
		m_pDatabase_pluto_media->DiscLocation_get()->GetRows(sWhere,&vectRow_DiscLocation);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipJob::RipJob %d/%s", (int) vectRow_DiscLocation.size(), sWhere.c_str());
		if( vectRow_DiscLocation.size() )
			m_pRow_DiscLocation = vectRow_DiscLocation[0];
		else
			m_pRow_DiscLocation=NULL;
	}

	if( m_pRow_DiscLocation )
	{
		m_pRow_DiscLocation->RipJob_set(m_iID);

		if( m_pDisk_Drive_Functions )
			m_pRow_DiscLocation->EK_Device_Ripping_set(m_pDisk_Drive_Functions->m_dwPK_Device_get());
		else if( m_pSlot )
			m_pRow_DiscLocation->EK_Device_Ripping_set(m_pSlot->m_pJukeBox->m_pCommand_Impl->m_dwPK_Device);
		m_pRow_DiscLocation->Table_DiscLocation_get()->Commit();
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipJob::RipJob %d drive %d slot %d %s / %s m_pRow_DiscLocation %p m_pDisk_Drive_Functions %p m_pSlot %p", m_iID, m_pDisk_Drive_Functions ? m_pDisk_Drive_Functions->m_dwPK_Device_get() : 0,
		m_pSlot ? m_pSlot->m_SlotNumber : 0, m_sDirectory.c_str(), m_sFileName.c_str(),m_pRow_DiscLocation,m_pDisk_Drive_Functions,m_pSlot);
}

RipJob::~RipJob()
{
	string sDevices;
	if( m_pDisk_Drive_Functions )
		sDevices = StringUtils::itos(m_pDisk_Drive_Functions->m_dwPK_Device_get());
	if( m_pSlot )
		sDevices += (sDevices.size() ? "," : "") +  StringUtils::itos(m_pSlot->m_pJukeBox->m_pCommand_Impl->m_dwPK_Device);

	string sWhere = "EK_Device_Ripping in (" + sDevices + ") AND RipJob=" + StringUtils::itos(m_iID);

	string sSQL = "UPDATE DiscLocation SET EK_Device_Ripping=NULL,RipJob=NULL WHERE " + sWhere;

	if( sDevices.size() )
		m_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipJob::~RipJob %d drive %d slot %d %s / %s m_pRow_DiscLocation %d/%d query: %s", 
		m_iID, m_pDisk_Drive_Functions ? m_pDisk_Drive_Functions->m_dwPK_Device_get() : 0,
		m_pSlot ? m_pSlot->m_SlotNumber : 0, m_sDirectory.c_str(), m_sFileName.c_str(),
		m_pRow_DiscLocation ? m_pRow_DiscLocation->EK_Device_get() : 0, m_pRow_DiscLocation ? m_pRow_DiscLocation->Slot_get() : 0, sSQL.c_str());

	string sMessage;
	if(m_nTracksFailedToRip == m_listTask.size())
		sMessage = "Ripping failed";
	else if (m_bHasErrors)
		sMessage = "Ripping completed with errors";
	else
		sMessage = "Ripping completed";

	sMessage += ": " + m_sFileName;

	if(m_listTask.size() > 1)
		sMessage += m_nTracksFailedToRip > 0 ? (". Failed to rip " + StringUtils::ltos(m_nTracksFailedToRip) + " tracks.") : "";

	SCREEN_PopupMessage SCREEN_PopupMessage(m_pCommand_Impl->m_dwPK_Device, 
		m_iPK_Orbiter,
		sMessage, // Main message
		"", // Command Line
		"ripping_complete", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
}

bool RipJob::ReadyToRun()
{
	if( m_pDisk_Drive_Functions )  // We're ready to go if we have a drive to do the ripping
	{
	    	// if fixup failed, we should also fail
		if (!m_pDisk_Drive_Functions->FixupRippingInfo(m_pDisk_Drive_Functions,m_iPK_MediaType,m_sFileName,m_sTracks,m_iEK_Disc,m_sDirectory))
		{
		    LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipJob::RipJob::ReadyToRun fixup failed, aborting this job");
		    m_eJobStatus = job_Error;
		    return false;
		}

		AddRippingTasks();
		return true;
	}

	if( !m_pSlot ) // If we don't have a jukebox, something is wrong, we have nothing and can't proceed
	{
		m_eJobStatus = job_Error;
		return false;
	}

	// Try to get an available drive from the jukebox to do the ripping
	Drive *pDrive = m_pSlot->m_pJukeBox->LockAvailableDrive(Disk_Drive_Functions::locked_rip_job,this,NULL,true);
	if( pDrive==NULL )  // We don't have a drive
		return false;

	m_pDisk_Drive_Functions=pDrive;
	AddTask(new MoveDiscTask(this,"SlotToDrive",MoveDiscTask::mdt_SlotToDrive,m_pSlot->m_pJukeBox,pDrive,m_pSlot));
	AddTask(new IdentifyTask(this,m_pDisk_Drive_Functions,m_pCommand_Impl,"Identify"));
	AddTask(new FixupRippingInfoTask(this,"FixupRippingInfo"));
	AddTask(new AddRippingTasksTask(this,pDrive,"AddRippingTasks"));
	AddTask(new MoveDiscTask(this,"DriveToSlot",MoveDiscTask::mdt_DriveToSlot,m_pSlot->m_pJukeBox,pDrive,m_pSlot));
	return true;
}

void RipJob::JobDone()
{
	if( m_pDisk_Drive_Functions )
		m_pDisk_Drive_Functions->UnlockDrive();
}

void RipJob::AddRippingTasks(Drive *pDrive, TasklistPosition position)
{
	vector<string> vectTracks;
	StringUtils::Tokenize(m_sTracks, "|", vectTracks);

	if (vectTracks.empty())
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding ripping task for the whole disk");
	    AddTask(new RipTask(this,"Rip",m_bReportResult, "",pDrive), position);
	}
	else
	{
	    	vector<Task *> vTasks;
		for(vector<string>::iterator it = vectTracks.begin(), end = vectTracks.end(); it != end; ++it)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding ripping task for track %s", it->c_str());
			vTasks.push_back( new RipTask(this,"Rip",m_bReportResult,*it,pDrive) );
		}
		AddTasks(vTasks, position);
	}
}

bool RipJob::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
	if( m_eJobStatus==job_WaitingToStart || m_eJobStatus==job_InProgress )
	{
		if( pPendingTaskList )
		{
			if( m_pDisk_Drive_Functions )
			{
				pPendingTaskList->m_listPendingTask.push_back(new PendingTask(m_iID,m_pDisk_Drive_Functions->m_pCommand_Impl_get()->m_dwPK_Device,
					m_pDisk_Drive_Functions->m_pCommand_Impl_get()->m_dwPK_Device,
					"rip",ToString(),(char) PercentComplete(),SecondsRemaining(),true));
			}
			else
			{
				// We don't have a drive yet
				pPendingTaskList->m_listPendingTask.push_back(new PendingTask(m_iID,m_pCommand_Impl->m_dwPK_Device,
					m_pCommand_Impl->m_dwPK_Device,
					"rip",ToString(),(char) PercentComplete(),SecondsRemaining(),true));
			}
		}

		return true;
	}
	else
		return false;
}

int RipJob::PercentComplete()
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

	return (RangeThisTask * (iTaskNum-1)) /* for the tasks we've done so far */ + ScaledPercent /* plus the current task */;
}

int RipJob::SecondsRemaining()
{ 
	return 0; 
}

string RipJob::ToString()
{ 
	if( !m_pDisk_Drive_Functions )
		return "Waiting... (" + m_sFileName + ")" + (m_pSlot ? " Slot: " + StringUtils::itos(m_pSlot->m_SlotNumber) : "");

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

	string sTxt = "Ripping " + StringUtils::itos(iTaskNum) + " of " + StringUtils::itos((int) m_listTask.size()) + " " + sTimeLeft + ": " + m_sFileName;
	if (m_bHasErrors)
		sTxt = "(!!)" + sTxt;
	return sTxt;
}

int RipJob::Get_PK_Orbiter()
{
	return m_iPK_Orbiter;
}
