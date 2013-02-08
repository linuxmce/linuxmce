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

#include "MoveDiscTask.h"
#include "IdentifyJob.h"
#include "Disk_Drive_Functions.h"
#include "JukeBox.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "Media_Plugin/Media_Plugin.h"

using namespace nsJobHandler;

MoveDiscTask::MoveDiscTask(class Job *pJob,string sName,MoveDiscTaskType MoveDiscTaskType,JukeBox *pJukeBox,Drive *pDrive,Slot *pSlot)
	: Task(pJob,sName)
{
	m_pDrive=pDrive;
	m_pSlot=pSlot;
	m_pJukeBox=pJukeBox;
	m_MoveDiscTaskType=MoveDiscTaskType;
}

int MoveDiscTask::Run()
{
	JukeBox::JukeBoxReturnCode jukeBoxReturnCode = JukeBox::jukebox_ok;
	if( m_MoveDiscTaskType==mdt_SlotToDrive && m_pSlot && m_pDrive )
	{
		if( !m_pJukeBox->LockJukebox(Disk_Drive_Functions::locked_move,this) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveDiscTask::Run 1 cannot lock jukebox");
			return 1;  // Try again in a sec
		}
		jukeBoxReturnCode = m_pJukeBox->MoveFromSlotToDrive(m_pSlot,m_pDrive);
		m_pJukeBox->UnlockJukebox();
	}
	else if( m_MoveDiscTaskType==mdt_DriveToSlot && m_pSlot && m_pDrive )
	{
		if( !m_pJukeBox->LockJukebox(Disk_Drive_Functions::locked_move,this) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveDiscTask::Run 2 cannot lock jukebox");
			return 1;  // Try again in a sec
		}
		jukeBoxReturnCode = m_pJukeBox->MoveFromDriveToSlot(m_pSlot,m_pDrive);
		m_pJukeBox->UnlockJukebox();
	}
	else if( m_MoveDiscTaskType==mdt_SlotToEject )
	{
		Slot *pSlot = m_pSlot;
		if( !pSlot )
		{
			// This is an eject all slots
			pSlot = m_pJukeBox->m_mapSlot_NotEmpty();
			if( !pSlot )  // We ejected all slots
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveDiscTask::Run no slots to move %d",(int) m_pJukeBox->m_mapSlot.size());
				m_eTaskStatus_set(TASK_COMPLETED);
				return 0;
			}
		}
		if( !m_pJukeBox->LockJukebox(Disk_Drive_Functions::locked_move,this) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveDiscTask::Run 3 cannot lock jukebox");
			return 1;  // Try again in a sec
		}
		jukeBoxReturnCode = m_pJukeBox->Eject(pSlot);
		m_pJukeBox->UnlockJukebox();

		if( !m_pSlot && jukeBoxReturnCode==JukeBox::jukebox_ok )
		{
			m_pJob->RefreshOrbiter();
			return 1;  // This is a multiple eject, so keep going until we run out
		}
	}
	else if( m_MoveDiscTaskType==mdt_Load )
	{
		Slot *pSlot = m_pSlot;
		if( !pSlot )
		{
			// This is a load all slots
			pSlot = m_pJukeBox->m_mapSlot_Empty();
			if( !pSlot )  // We have no more slots
			{
				m_eTaskStatus_set(TASK_COMPLETED);
				return 0;
			}
		}
		if( !m_pJukeBox->LockJukebox(Disk_Drive_Functions::locked_move,this) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveDiscTask::Run 4 cannot lock jukebox");
			return 1;  // Try again in a sec
		}
		jukeBoxReturnCode = m_pJukeBox->Load(pSlot);
		m_pJukeBox->UnlockJukebox();

		if( !m_pSlot && jukeBoxReturnCode==JukeBox::jukebox_ok )
		{
			m_pJob->RefreshOrbiter();
			return 1;  // This is a multiple eject, so keep going until we run out
		}
	}

	m_pJob->RefreshOrbiter();
	m_eTaskStatus_set(jukeBoxReturnCode==JukeBox::jukebox_ok ? TASK_COMPLETED : TASK_FAILED_ABORT);
	return 0;
}


void MoveDiscTask::UpdateProgress(string sStatus,int iPercent,int iTime,string sText,string sFilename)
{
	/*
	m_sText=sText;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MoveDiscTask::UpdateProgress message %s", m_sText.c_str());

	if( sStatus=="p" )
	{
		m_iPercent=iPercent;
		m_iTime=iTime;
		ReportProgress();
	}
	else if( sStatus=="e" )
	{
		ReportFailure();
		m_eTaskStatus_set(TASK_FAILED);
	}
	else if( sStatus=="s" )
	{
		m_pIdentifyJob->m_pDisk_Drive_Functions->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(m_pIdentifyJob->m_iEK_Disc,
			m_pIdentifyJob->m_iPK_MediaType,m_pIdentifyJob->m_sFileName,m_pIdentifyJob->m_sTracks);
		ReportSuccess();
		m_eTaskStatus_set(TASK_COMPLETED);
	}
	*/
}

bool MoveDiscTask::Abort()
{
	/*
	DCE::CMD_Kill_Application CMD_Kill_Application
		(m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device,
		m_pIdentifyJob->m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
		m_sSpawnName,false);
    m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(CMD_Kill_Application);

	if( FileUtils::DirExists(m_pIdentifyJob->m_sName) )
	{
		string::size_type pos=0;
		while(pos<m_pIdentifyJob->m_sTracks.size())
		{
			string sTrack = StringUtils::Tokenize( m_pIdentifyJob->m_sTracks, "|", pos );
			string::size_type pos_name = sTrack.find(",");
			if( pos_name!=string::npos )
			{
				FileUtils::DelFile(m_pIdentifyJob->m_sName + "/" + sTrack.substr(pos_name+1) + "*");
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::RippingProgress deleting %s", (m_pIdentifyJob->m_sName + "/" + sTrack.substr(pos_name+1) + "*").c_str() );
			}
		}
		list<string> listFiles;
		FileUtils::FindFiles(listFiles,m_pIdentifyJob->m_sName,"*",true,false,1);
		FileUtils::FindDirectories(listFiles,m_pIdentifyJob->m_sName,false);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"It's a directory %s with %d files",m_pIdentifyJob->m_sName.c_str(),(int) listFiles.size());
		if( listFiles.size()==0 )  // There's nothing else in that directory.  Delete it
		{
			FileUtils::DelDir(m_pIdentifyJob->m_sName);
			string sParent = FileUtils::BasePath(m_pIdentifyJob->m_sName);
			FileUtils::FindFiles(listFiles,sParent,"*",true,false,1);
			FileUtils::FindDirectories(listFiles,sParent,false);
		}
	}
	else
	{
		FileUtils::DelFile(m_pIdentifyJob->m_sName + ".*");  // Delete any temporary or in progress
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::RippingProgress deleting %s", (m_pIdentifyJob->m_sName + ".*").c_str() );
	}
*/
	return Task::Abort();
}

void MoveDiscTask::ReportProgress()
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

void MoveDiscTask::ReportFailure()
{
	/*
	SCREEN_PopupMessage SCREEN_PopupMessage(m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device, 
		m_pIdentifyJob->m_iPK_Orbiter,
		"Ripping failed", // Main message
		"", // Command Line
		"ripping_failed", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
	*/
}

void MoveDiscTask::ReportSuccess()
{
	/*
	SCREEN_PopupMessage SCREEN_PopupMessage(m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device, 
		m_pIdentifyJob->m_iPK_Orbiter,
		"Ripping complete", // Main message
		"", // Command Line
		"ripping_complete", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
	*/
}

int MoveDiscTask::SecondsRemaining()
{ 
	return 30;  // Typical ID time
}
