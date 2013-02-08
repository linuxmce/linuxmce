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

#include "RipTask.h"
#include "RipJob.h"
#include "Disk_Drive_Functions.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_media/Table_RipStatus.h"
#include "pluto_media/Table_DiscLocation.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "Media_Plugin/Media_Plugin.h"
#include "JukeBox.h"

using namespace nsJobHandler;

RipTask::RipTask(RipJob *pRipJob,string sName,bool bReportResult,string sTrack,Drive *pDrive)
	: Task(pRipJob,sName)
{
	m_sTrack=sTrack;
	m_iTime=0;
	m_bSpawnedRip=false;
	m_pRipJob=pRipJob; // A duplicate of m_pJob, but we don't need to keep recasting
	m_pRow_RipStatus=NULL;
	m_bReportResult=bReportResult;
	m_pDrive=pDrive;
}

int RipTask::Run()
{
	if( !m_pRow_RipStatus )
	{
		m_pRow_RipStatus = m_pRipJob->m_pDatabase_pluto_media->RipStatus_get()->AddRow();
		if( m_pRipJob->m_pDisk_Drive_Functions )
			m_pRow_RipStatus->EK_Device_set(m_pRipJob->m_pDisk_Drive_Functions->m_dwPK_Device_get());
		if( m_pRipJob->m_pSlot )
			m_pRow_RipStatus->Slot_set(m_pRipJob->m_pSlot->m_SlotNumber);
		if( m_pRipJob->m_pRow_DiscLocation )
		{
			m_pRipJob->m_pRow_DiscLocation->Reload();
			m_pRow_RipStatus->Type_set(m_pRipJob->m_pRow_DiscLocation->Type_get());
			if( m_pRipJob->m_pRow_DiscLocation->FK_Disc_get() )
				m_pRow_RipStatus->FK_Disc_set(m_pRipJob->m_pRow_DiscLocation->FK_Disc_get());

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipTask::Run m_pRipJob %p jobslot %d dl: device %d slot %d disc %d",
				m_pRipJob, m_pRipJob->m_pSlot ? m_pRipJob->m_pSlot->m_SlotNumber : -1,
				m_pRipJob->m_pRow_DiscLocation->EK_Device_get(), m_pRipJob->m_pRow_DiscLocation->Slot_get(), m_pRipJob->m_pRow_DiscLocation->FK_Disc_get());
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipTask::Run m_pRipJob %p jobslot %d",
				m_pRipJob, m_pRipJob->m_pSlot ? m_pRipJob->m_pSlot->m_SlotNumber : -1);


		m_pRow_RipStatus->DateTime_set(StringUtils::SQLDateTime());
		m_pRow_RipStatus->RipJob_set( m_pRipJob->m_iID_get() );
		m_pRow_RipStatus->File_set( m_pRipJob->m_sFileName );
		m_pRipJob->m_pDatabase_pluto_media->RipStatus_get()->Commit();
	}

	if( m_bSpawnedRip )
		return RunAlreadySpawned();  // We already spawned.  Another run which checks to see if the task is still active

	if (!m_pRipJob->m_pDisk_Drive_Functions->m_pDevice_AppServer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot rip -- no appserver");
		return 0;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Going to rip %s; drive number: %d", m_sName.c_str(), m_pRipJob->m_pDisk_Drive_Functions->m_dwPK_Device);

	if (!m_pRipJob->m_pDisk_Drive_Functions->m_mediaInserted)
	{
		m_pRipJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("",RIP_RESULT_NO_DISC, "", m_sName, m_pRipJob->m_iEK_Disc);
		return 0;
	}

	if (m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_AUDIO && 
		m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_DVD_VIDEO && 
		m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_HDDVD && 
		m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_BD && 
		m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_MIXED && 
		m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_VCD)
	{
		m_pRipJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("", RIP_RESULT_INVALID_DISC_TYPE, "", m_sName, m_pRipJob->m_iEK_Disc);
		return 0;
	}

	if (m_pRipJob->m_pDisk_Drive_Functions->isRipping())
	{
		m_pRipJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("", RIP_RESULT_ALREADY_RIPPING, "", m_sName, m_pRipJob->m_iEK_Disc);
		return 0;
	}

	string strParameters;
	strParameters = StringUtils::itos(m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + "\t" 
		+ StringUtils::itos(m_pJob->m_iID_get()) + "\t" 
		+ StringUtils::itos(m_iID_get()) + "\t"
		+ ((RipJob *) m_pJob)->m_sFileName + "\t" + m_pRipJob->m_pDisk_Drive_Functions->m_sDrive + "\t" 
		+ StringUtils::itos(m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus) + "\t" 
		+ StringUtils::itos(m_pRipJob->m_iPK_Users) + "\t" 
		+ m_pRipJob->m_sFormat + "\t" + m_sTrack;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Launching ripping job2 with name \"%s\" for disk with type \"%d\" parms %s", 
		m_sName.c_str(), m_pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus, strParameters.c_str());

	string sResultMessage =
		StringUtils::itos(m_pRipJob->m_pDisk_Drive_Functions->m_dwPK_Device) + " " + StringUtils::itos(m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) +
			" " + StringUtils::itos(MESSAGETYPE_COMMAND) +
			" " + StringUtils::itos(COMMAND_Update_Ripping_Status_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_Task_CONST) + " " + StringUtils::itos(m_iID_get()) +
			" " + StringUtils::itos(COMMANDPARAMETER_Job_CONST) + " " + StringUtils::itos(m_pJob->m_iID_get()) + " " +
			StringUtils::itos(COMMANDPARAMETER_Status_CONST) + " ";

	m_sSpawnName="rip_job_" + StringUtils::itos(m_pJob->m_iID_get()) + "_task_" + StringUtils::itos(m_iID);

	DCE::CMD_Spawn_Application
		spawnApplication(m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device,
						m_pRipJob->m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
						"/usr/pluto/bin/ripDiskWrapper.sh", 
						m_sSpawnName,
						strParameters,
						sResultMessage + "e",
						sResultMessage + "s",
						false, false, false, false);

	string sResponse;
    if (! m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(spawnApplication,&sResponse) || sResponse != "OK")
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Trying to rip - App server returned %s",sResponse.c_str());
		return 0;
	}

	m_bSpawnedRip=true;
	return 1000;  // Check again in 1 second
}

int RipTask::RunAlreadySpawned()
{
	return 1000;
}

void RipTask::UpdateProgress(string sStatus,int iPercent,int iTime,string sText,string sFilename)
{
	if( m_eTaskStatus_get()==TASK_COMPLETED )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipTask::UpdateProgress ignoring because we're done status %s message %s file %s", sStatus.c_str(), m_sText.c_str(),sFilename.c_str());
		return;
	}

	if( sStatus=="b" )
	{
		if (!m_pRipJob->m_bHasErrors)
		{
			string sMessage = "Errors were encountered while ripping the disk. The bad sectors will be skipped and replaced with zeros, but if the disk is very scratched, it may take a very long while for the operation to complete, up to a full day or more. If you don't want to wait, you can cancel the operation from the 'Pending tasks' screen|OK|Pending tasks";

			int iPK_Orbiter = m_pRipJob->Get_PK_Orbiter();
			string sPK_Orbiter = StringUtils::itos(iPK_Orbiter);
			string sCmdLine = "-targetType device 0 " + sPK_Orbiter +" 1 4|"
				"-targetType device 0 " + sPK_Orbiter + " 1 741 159 82";

			SCREEN_PopupMessage SCREEN_PopupMessage(0, iPK_Orbiter,
				sMessage, // Main message
				sCmdLine, // Command Line
				"ripping", // Description
				"0", // sPromptToResetRouter
				"0", // sTimeout
				"1"); // sCannotGoBack
			m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
		}
		m_pRipJob->m_bHasErrors=true;
		return;
	}

	if( m_pRow_RipStatus )
	{
		m_pRow_RipStatus->File_set(sFilename); //make sure the filename is complete
		m_pRow_RipStatus->Status_set(sStatus);
		m_pRow_RipStatus->Message_set(sText.empty() && sStatus == "p" ? StringUtils::ltos(m_iPercent) + "%" : sText);
		m_pRipJob->m_pDatabase_pluto_media->RipStatus_get()->Commit();
	}

	m_sText=sText;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "RipTask::UpdateProgress id %d m_eTaskStatus_get %d status %s message %s file %s (%s)", 
		m_pRipJob->m_iID_get(), m_eTaskStatus_get(), sStatus.c_str(), sText.c_str(),m_pRipJob->m_sFileName.c_str(), sFilename.c_str());

	if( sStatus=="p" )
	{
		m_iPercent=iPercent;
		m_iTime=iTime;
		ReportProgress();
	}
	else if( sStatus=="e" )
	{
		if( m_bReportResult )
			ReportFailure();
		m_eTaskStatus_set(TASK_COMPLETED);  // Mark it completed, not failure, so if there's another task to unload this slot it still gets done
	}
	else if( sStatus=="s" )
	{
		if (m_pRipJob->m_bHasErrors && m_pRow_RipStatus)
		{
			m_pRow_RipStatus->Status_set("b");
			m_pRipJob->m_pDatabase_pluto_media->RipStatus_get()->Commit();
		}

		int PK_File = m_pRipJob->m_pDisk_Drive_Functions->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(m_pRipJob->m_iEK_Disc,
			m_pRipJob->m_iPK_MediaType,m_pRipJob->m_sFileName,m_pRipJob->m_sTracks);

		if( m_pRow_RipStatus && PK_File )
			m_pRow_RipStatus->FK_File_set(PK_File);

		if( m_bReportResult )
			ReportSuccess();
		m_eTaskStatus_set(TASK_COMPLETED);

		m_pRipJob->m_pDatabase_pluto_media->Commit();
	}
}

bool RipTask::Abort()
{
	DCE::CMD_Kill_Application CMD_Kill_Application
		(m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device,
		m_pRipJob->m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
		m_sSpawnName,false);
    m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(CMD_Kill_Application);

	if( FileUtils::DirExists(m_pRipJob->m_sFileName) )
	{
		string::size_type pos=0;
		while(pos<m_pRipJob->m_sTracks.size())
		{
			string sTrack = StringUtils::Tokenize( m_pRipJob->m_sTracks, "|", pos );
			string::size_type pos_name = sTrack.find(",");
			if( pos_name!=string::npos )
			{
				FileUtils::DelFile(m_pRipJob->m_sFileName + "/" + sTrack.substr(pos_name+1) + "*");
LoggerWrapper::GetInstance()->Write(LV_STATUS,"RipTask::Abort deleting %s", (m_pRipJob->m_sFileName + "/" + sTrack.substr(pos_name+1) + "*").c_str() );
			}
		}
		list<string> listFiles;
		FileUtils::FindFiles(listFiles,m_pRipJob->m_sFileName,"*",true,false,1);
		FileUtils::FindDirectories(listFiles,m_pRipJob->m_sFileName,false);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"It's a directory %s with %d files",m_pRipJob->m_sFileName.c_str(),(int) listFiles.size());
		if( listFiles.size()==0 )  // There's nothing else in that directory.  Delete it
		{
			FileUtils::DelDir(m_pRipJob->m_sFileName);
			string sParent = FileUtils::BasePath(m_pRipJob->m_sFileName);
			FileUtils::FindFiles(listFiles,sParent,"*",true,false,1);
			FileUtils::FindDirectories(listFiles,sParent,false);
		}
	}
	else
	{
		FileUtils::DelFile(m_pRipJob->m_sFileName + ".*");  // Delete any temporary or in progress
LoggerWrapper::GetInstance()->Write(LV_STATUS,"RipTask::Abort deleting %s", (m_pRipJob->m_sFileName + ".*").c_str() );
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"RipTask::Abort %s job %p slot %p jukebox %p drive %p",
		ToString().c_str(),m_pRipJob, m_pRipJob ? m_pRipJob->m_pSlot : NULL,
		(m_pRipJob && m_pRipJob->m_pSlot) ? m_pRipJob->m_pSlot->m_pJukeBox : NULL,
		m_pDrive);
	
	if( m_pRipJob && m_pRipJob->m_pSlot && m_pRipJob->m_pSlot->m_pJukeBox && m_pDrive )
	{
		if( !m_pRipJob->m_pSlot->m_pJukeBox->LockJukebox(Disk_Drive_Functions::locked_move,this) )
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipTask::Abort cannot lock jukebox");
		else
		{
			m_pRipJob->m_pSlot->m_pJukeBox->MoveFromDriveToSlot(m_pRipJob->m_pSlot, m_pDrive);
			m_pRipJob->m_pSlot->m_pJukeBox->UnlockJukebox();
		}
	}

	if (m_pRow_RipStatus) {
	    m_pRow_RipStatus->Status_set("a");
	    m_pRow_RipStatus->Message_set("aborted by user");
	    m_pRipJob->m_pDatabase_pluto_media->RipStatus_get()->Commit();
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipTask::Abort marking task as aborted");
	}
	else {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "RipTask::Abort not marking task as aborted - m_pRow_RipStatus is NULL");
	}

	return Task::Abort();
}

void RipTask::ReportProgress()
{
	/*
	for( MapEntertainArea::iterator it=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.begin( );it!=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.end( );++it )
	{
		EntertainArea *pEntertainArea = ( *it ).second;
		for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
		{
			MediaDevice *pMediaDevice = *itVFD;
			DCE::CMD_Display_Message CMD_Display_Message_Name(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pRippingJob->m_sStatus,
				StringUtils::itos(VL_MSGTYPE_RIPPING_NAME),"tc","","");

			DCE::CMD_Display_Message CMD_Display_Message_SP(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pRippingJob->m_sPercentage,
				StringUtils::itos(VL_MSGTYPE_RIPPING_PROGRESS),"tc","","");
			CMD_Display_Message_SP.m_pMessage->m_vectExtraMessages.push_back(CMD_Display_Message_Name.m_pMessage);
			SendCommand(CMD_Display_Message_SP);
		}
	}
	*/
}

void RipTask::ReportFailure()
{
	SCREEN_PopupMessage SCREEN_PopupMessage(m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device, 
		m_pRipJob->m_iPK_Orbiter,
		"Ripping failed " + m_sText + ": " + ((RipJob *) m_pJob)->m_sFileName, // Main message
		"", // Command Line
		"ripping_failed", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
	m_pRipJob->m_nTracksFailedToRip++;
}

void RipTask::ReportSuccess()
{
	/*
	SCREEN_PopupMessage SCREEN_PopupMessage(m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device, 
		m_pRipJob->m_iPK_Orbiter,
		"Ripping complete: " + ((RipJob *) m_pJob)->m_sFileName, // Main message
		"", // Command Line
		"ripping_complete", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
	*/
}




/*
	for( MapEntertainArea::iterator it=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.begin( );it!=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.end( );++it )
	{
		EntertainArea *pEntertainArea = ( *it ).second;
		for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
		{
			MediaDevice *pMediaDevice = *itVFD;
			DCE::CMD_Display_Message CMD_Display_Message(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				sMessage,
				StringUtils::itos(VL_MSGTYPE_RUNTIME_NOTICES),"tc","","");

			DCE::CMD_Display_Message CMD_Display_Message_Name(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				"",
				StringUtils::itos(VL_MSGTYPE_RIPPING_NAME),"tc","","");
			CMD_Display_Message_Name.m_pMessage->m_vectExtraMessages.push_back(CMD_Display_Message.m_pMessage);
			SendCommand(CMD_Display_Message);
		}
	}

*/

int RipTask::SecondsRemaining()
{ 
	if( m_iTime )
		return m_iTime;

	return Task::SecondsRemaining();
}
