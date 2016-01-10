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

#include "IdentifyTask.h"
#include "IdentifyJob.h"
#include "JukeBox.h"
#include "Disk_Drive_Functions.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "Media_Plugin/Media_Plugin.h"

using namespace nsJobHandler;

#define	ID_TIMEOUT	45 // Maximum seconds to wait for ID

IdentifyTask::IdentifyTask(Job *pJob,Disk_Drive_Functions *pDisk_Drive_Functions,Command_Impl *pCommand_Impl,string sName)
	: Task(pJob,sName)
{
	m_pDisk_Drive_Functions=pDisk_Drive_Functions;
	m_pCommand_Impl=pCommand_Impl;
	m_tSentIdCommand=0;
}

int IdentifyTask::Run()
{
	// JukeBox::Media_Identified will set this to completed when the job is id'd
	if( m_tSentIdCommand )
	{
		if( m_tSentIdCommand + ID_TIMEOUT < time(NULL) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "IdentifyTask::Run didn't get a valid response");
			m_eTaskStatus_set(TASK_FAILED_CONTINUE);  // Keep going and put the disc back in its slot
			return 0;
		}
		return 1000; // Keep going
	}
	if( m_pDisk_Drive_Functions && m_pDisk_Drive_Functions->m_pDevice_MediaIdentifier_get() && m_pCommand_Impl )
	{
		string sResponse;
		DCE::CMD_Identify_Media CMD_Identify_Media(m_pCommand_Impl->m_dwPK_Device,m_pDisk_Drive_Functions->m_pDevice_MediaIdentifier_get()->m_dwPK_Device,
			m_pDisk_Drive_Functions->m_dwPK_Device_get(),StringUtils::itos(m_pDisk_Drive_Functions->m_discid),
			m_pDisk_Drive_Functions->m_sDrive,
			m_pCommand_Impl->m_dwPK_Device);
		if( m_pCommand_Impl->SendCommand(CMD_Identify_Media,&sResponse) && sResponse=="OK" )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "IdentifyTask::Run waiting for identification");
			m_eTaskStatus_set(TASK_IN_PROGRESS);
			m_tSentIdCommand=time(NULL);
			return 1000;
		}
	}

	m_eTaskStatus_set(TASK_FAILED_CONTINUE);  // Keep going and put the disc back in its slot
	return 0;
}

int IdentifyTask::RunAlreadySpawned()
{
	return 1000;
}

void IdentifyTask::UpdateProgress(string sStatus,int iPercent,int iTime,string sText,string sFilename)
{
}

bool IdentifyTask::Abort()
{
	return Task::Abort();
}

void IdentifyTask::ReportProgress()
{
}

void IdentifyTask::ReportFailure()
{
}

void IdentifyTask::ReportSuccess()
{
}

int IdentifyTask::SecondsRemaining()
{ 
	return 30;  // Typical ID time
}
