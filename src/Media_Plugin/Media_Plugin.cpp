/*
 Media_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

//<-dceag-d-b->
#include "Media_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#include <sstream>
#include <iterator>     // std::ostream_iterator
#include <algorithm>    // std::copy

using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <algorithm>

#include "PlutoUtils/DatabaseUtils.h"
#include "PlaylistParser.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "Generic_NonPluto_Media.h"
#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "File_Grids_Plugin/File_Grids_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_RemoteControl.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Define_AttributeType.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_DiscLocation.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_AttributeType.h"
#include "pluto_media/Table_MediaType_AttributeType.h"
#include "pluto_media/Table_MediaProvider.h"
#include "pluto_media/Table_ProviderSource.h"
#include "pluto_media/Table_LongAttribute.h"
#include "pluto_media/Table_RipStatus.h"
#include "pluto_media/Table_MediaSubType.h"
#include "pluto_media/Table_FileFormat.h"
#include "pluto_media/Table_MediaType_FileFormat.h"
#include "pluto_media/Table_MediaType_MediaSubType.h"
#include "Gen_Devices/AllScreens.h"
#include "pluto_main/Define_Pipe.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "pluto_main/Define_DataGrid.h"
#include "DataGrid.h"
#include "File_Grids_Plugin/FileListGrid.h"
#include "SerializeClass/ShapesColors.h"
#include "../VFD_LCD/VFD_LCD_Base.h"

#include "Media_Plugin/BoundRemote.h"

#include "DCE/DCEConfig.h"

// Alarm Types
#define MEDIA_PLAYBACK_TIMEOUT					1
#define CHECK_FOR_NEW_FILES						2
#define WAITING_FOR_JUKEBOX						3
#define UPDATE_VIEW_DATE						4
#define UPDATE_SEARCH_TOKENS					5
#define UPDATE_ATTRIBUTE_CACHE 6

#define TIMEOUT_JUKEBOX							60

using namespace nsJobHandler;

int UniqueColors[MAX_MEDIA_COLORS];

// Job Handlers /////////////////////////////////////////////////////////////////////////////

MoveJob::MoveJob(Database_pluto_media *pDatabase_pluto_media,
		 class JobHandler *pJobHandler,
		 int iPK_Orbiter,
		 Row_File *pRow_File,
		 string sFileName,
		 string sDestinationFileName,
		 bool bReportResult,
		 Command_Impl *pCommand_Impl)
  : Job(pJobHandler, "MoveJob", iPK_Orbiter, pCommand_Impl)
{
  m_bReportResult=bReportResult;
  m_pDatabase_pluto_media=pDatabase_pluto_media;
  m_pRow_File=pRow_File;
  m_sFileName=sFileName;
  m_sDestinationFileName=sDestinationFileName;
  m_iPK_Orbiter=iPK_Orbiter;

  LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveJob::MoveJob constructor - %s to %s ",m_sFileName.c_str(),m_sDestinationFileName.c_str());
}

MoveJob::~MoveJob()
{
  SCREEN_PopupMessage SCREEN_PopupMessage(m_pCommand_Impl->m_dwPK_Device,
					  m_iPK_Orbiter,
					  "Move Done!",        // Message to display
					  "",              // Command Line
					  "move_complete", // Description
					  "0",             // do not prompt to reset router.
					  "0",             // no timeout
					  "1");            // Can't Go back.
  m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
}

bool MoveJob::ReadyToRun()
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MoveJob::ReadyToRun - Ready - Adding Move Tasks");
  AddMoveTasks();
  return true;
}

void MoveJob::AddMoveTasks(TasklistPosition position)
{
  vector <Task *> vTasks;
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX AddMoveTasks - from %s to %s",m_sFileName.c_str(),m_sDestinationFileName.c_str());
  // Move original filename.
  m_sDestinationFileName+="/"+FileUtils::FilenameWithoutPath(m_sFileName);
  if (FileUtils::FileExists(m_sFileName))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX Adding Move Task for Main File");
      vTasks.push_back(new MoveTask(this, "Move Media",m_bReportResult,m_sFileName,m_sDestinationFileName));
      vTasks.push_back(new MoveDBTask(this, "Move Media Entry in DB",m_bReportResult));
    }
  if (FileUtils::FileExists(m_sFileName+".id3"))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX Adding Move Task for ID3 File");
      vTasks.push_back(new MoveTask(this, "Move ID3",m_bReportResult,m_sFileName+".id3",m_sDestinationFileName+".id3"));
    }
  if (FileUtils::FileExists(m_sFileName+".dvd.keys.tar.gz"))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX Adding Move Task for DVD Keys");
      vTasks.push_back(new MoveTask(this, "Move DVD Keys",m_bReportResult,m_sFileName+".dvd.keys.tar.gz",m_sDestinationFileName+".dvd.keys.tar.gz"));
    }
  AddTasks(vTasks, position);
}

bool MoveJob::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
  // am totally guessing here. hopefully this will work.
  PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
  if (m_eJobStatus==job_WaitingToStart || m_eJobStatus==job_InProgress)
    {
      if (pPendingTaskList)
	{
	  pPendingTaskList->m_listPendingTask.push_back(new PendingTask(m_iID,
									m_iPK_Orbiter,
									m_pCommand_Impl->m_dwPK_Device,
									"move",ToString(),
									(char)PercentComplete(),
									SecondsRemaining(),
									true));
	}
      return true;
    }
  else
    return false;
}

int MoveJob::PercentComplete()
{
  int iTaskNum=0;
  Task *pTask_Current=NULL;
  for (list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
    {
      Task *pTask = *it;
      if ( pTask->m_eTaskStatus_get()==TASK_NOT_STARTED )
	break;
      pTask_Current=pTask;
      iTaskNum++;
    }

  if ( pTask_Current==NULL || iTaskNum==0 )
    return 0; // hasn't started yet.

  int RangeThisTask = 100 / m_listTask.size();
  int ScaledPercent = pTask_Current->PercentComplete()*RangeThisTask / 100;

  return (RangeThisTask * (iTaskNum-1)) + ScaledPercent;

}

int MoveJob::SecondsRemaining()
{
  return 0;
}

string MoveJob::ToString()
{
  PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
  int iTaskNum=0;
  Task *pTask_Current=NULL;
  for (list<class Task *>::iterator it=m_listTask.begin();it!=m_listTask.end();++it)
    {
      Task *pTask = *it;
      if (pTask->m_eTaskStatus_get() == TASK_NOT_STARTED )
	break;
      pTask_Current=pTask;
      iTaskNum++;
    }

  string sTimeLeft;

  if ( pTask_Current )
    sTimeLeft = "(" + StringUtils::SecondsAsTime(pTask_Current->SecondsRemaining()) + ")";

  string sTxt = "Moving " + m_sFileName + " to " + m_sDestinationFileName + " " + sTimeLeft;

  return sTxt;

}

int MoveJob::Get_PK_Orbiter()
{
  return m_iPK_Orbiter; // From the superclass.
}

// MoveTask /////////////////////////////////////////////////////////////////////////////////

MoveTask::MoveTask(class MoveJob *pMoveJob,
		   string sName,
		   bool bReportResult,
		   string sFileName,
		   string sDestinationFileName) : Task(pMoveJob,sName)
{
  m_bAlreadySpawned=false;
  m_pMoveJob=pMoveJob;
  m_bReportResult=bReportResult;
  m_sFileName=sFileName;
  m_sDestinationFileName=sDestinationFileName;

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MoveTask::MoveTask constructor");

}

MoveTask::~MoveTask()
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MoveTask::MoveTask destructor");
  // Just here because...
}

int MoveTask::Run()
{

   if (m_bAlreadySpawned)
    {
      return RunAlreadySpawned();
    }

  DeviceData_Base *pDevice_App_Server = m_pMoveJob->m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST, m_pMoveJob->m_pCommand_Impl);

  if (!pDevice_App_Server)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MoveTask::Run() - Cannot move, no App Server found.");
      return 0;
    }
  else
    {
      m_pDevice_App_Server=pDevice_App_Server;
    }

  // Pass these parameters to the MoveWrapper.sh
  string strParameters =
    StringUtils::itos(m_pMoveJob->m_pCommand_Impl->m_dwPK_Device) + "\t" +
    StringUtils::itos(m_pJob->m_iID_get()) + "\t" +
    StringUtils::itos(m_iID_get()) + "\t" +
    m_sFileName + "\t" +
    m_sDestinationFileName;

  string sResultMessage =
    StringUtils::itos(m_pMoveJob->m_pCommand_Impl->m_dwPK_Device) + " " +   // From device
    StringUtils::itos(m_pMoveJob->m_pCommand_Impl->m_dwPK_Device) + " " +   // To Device
    StringUtils::itos(MESSAGETYPE_COMMAND) + " " +                          // 1 = COMMAND
    StringUtils::itos(COMMAND_Update_Move_Status_CONST) + " " +             // CMD_Update_Move_Status
    StringUtils::itos(COMMANDPARAMETER_Task_CONST) + " " +                  //
    StringUtils::itos(m_iID) + " " +                                  // Task ID
    StringUtils::itos(COMMANDPARAMETER_Job_CONST) + " " +                   //
    StringUtils::itos(m_pJob->m_iID_get()) + " " +                          // Job ID
    StringUtils::itos(COMMANDPARAMETER_Status_CONST) + " ";  // The Rest will be filled in below.

  // Passed to the App Server as a unique name that can be killed later.
  m_sSpawnName = "move_job_" +
    StringUtils::itos(m_pJob->m_iID_get()) +
    "_task_" +
    StringUtils::itos(m_iID);

  DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pMoveJob->m_pCommand_Impl->m_dwPK_Device,
						   pDevice_App_Server->m_dwPK_Device,
						   "/usr/pluto/bin/moveWrapper.sh",
						   m_sSpawnName,
						   strParameters,
						   sResultMessage + "e",
						   sResultMessage + "s",
						   false, false, false, false);

  string sResponse;

  if (!m_pMoveJob->m_pCommand_Impl->SendCommand(CMD_Spawn_Application,&sResponse) || sResponse != "OK")
    {
      // App server failed somehow.
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MoveTask::Run() - Trying to move - App Server returned %s",sResponse.c_str());
      return 0; // This is done. Do not trigger the run again.
    }

  m_bAlreadySpawned=true;

  /* if (m_sName == "Move Media")
     {
       // Set the database location appropriately.
       string sDestinationPath = FileUtils::BasePath(m_sDestinationFileName);
       string sDestinationBaseName = FileUtils::FilenameWithoutPath(m_sDestinationFileName);

       m_pMoveJob->m_pRow_File->Path_set(sDestinationPath);
       m_pMoveJob->m_pRow_File->Path_set(sDestinationBaseName);
       m_pMoveJob->m_pDatabase_pluto_media->File_get()->Commit();
     } */

  return 1000; // check again in 1 second
}

bool MoveTask::Abort()
{
  DCE::CMD_Kill_Application CMD_Kill_Application
    (m_pMoveJob->m_pCommand_Impl->m_dwPK_Device,
     m_pDevice_App_Server->m_dwPK_Device,
     m_sSpawnName,
     false);
  m_pMoveJob->m_pCommand_Impl->SendCommand(CMD_Kill_Application);

  return Task::Abort(); // superclass

}

int MoveTask::RunAlreadySpawned()
{
  return 1000; // Re-check in 1 second...
}

void MoveTask::UpdateProgress(string sStatus, int iPercent, int iTime, string sText)
{
  if( m_eTaskStatus_get()==TASK_COMPLETED )
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveTask::UpdateProgress ignoring because we're done status %s", sStatus.c_str());
      return;
    }

  m_sText=sText; // expose new status text to task so other parts can use it.

  LoggerWrapper::GetInstance()->Write(LV_WARNING,"MoveTask::UpdateProgress id %d m_eTaskStatus get %d status %s message %s sFilename %s sDestinationFileName %s",
				    m_pMoveJob->m_iID_get(),
				    m_eTaskStatus_get(),
				    sStatus.c_str(),
				    sText.c_str(),
				    m_sFileName.c_str(),
				    m_sDestinationFileName.c_str());

  if ( sStatus=="p" )
    {
      m_iPercent=iPercent;
      m_iTime=iTime;
      ReportProgress();
    }
  else if ( sStatus=="e" )
    {
      if ( m_bReportResult )
	ReportFailure();
      m_eTaskStatus_set(TASK_FAILED_ABORT); // mark it failed so the job will be aborted.
    }
  else if ( sStatus=="s" )
    {
      if (m_bReportResult)
	ReportSuccess();
      m_eTaskStatus_set(TASK_COMPLETED);
    }
}

void MoveTask::ReportFailure()
{
    // It's here in case we need to report on a task by task basis.
}

void MoveTask::ReportSuccess()
{
  // It's here in case we need to report on a task by task basis.
}

void MoveTask::ReportProgress()
{
  // It's here in case we need to do something.
}

int MoveTask::SecondsRemaining()
{
  if ( m_iTime )
    return m_iTime;

  return Task::SecondsRemaining();

}

/////////////////////////////////////////////////////////////////////////////////////////////

MoveDBTask::MoveDBTask(class MoveJob *pMoveJob,
		       string sName,
		       bool bReportResult) : Task(pMoveJob,sName)
{
  m_bAlreadySpawned=false;
  m_pMoveJob=pMoveJob;
  m_bReportResult=bReportResult;
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX MoveDBTask Added for sName %s",sName.c_str());
}

MoveDBTask::~MoveDBTask()
{
  // destructor.
}

int MoveDBTask::Run()
{

  string sDestinationPath = FileUtils::BasePath(m_pMoveJob->m_sDestinationFileName);
  string sDestinationBaseName = FileUtils::FilenameWithoutPath(m_pMoveJob->m_sDestinationFileName);

  if (FileUtils::FileExists(m_pMoveJob->m_sDestinationFileName))
    {
      string sOldPath = m_pMoveJob->m_pRow_File->Path_get();
      string sUpdOldCmd = "/usr/pluto/bin/UpdateMedia -d "+sOldPath;
      string sUpdNewCmd = "/usr/pluto/bin/UpdateMedia -d "+FileUtils::BasePath(m_pMoveJob->m_sDestinationFileName);
      m_pMoveJob->m_pRow_File->Path_set(m_pMoveJob->m_sDestinationFileName);
      m_pMoveJob->m_pDatabase_pluto_media->File_get()->Commit();
      system(sUpdOldCmd.c_str());
      system(sUpdNewCmd.c_str());
      m_eTaskStatus_set(TASK_COMPLETED);
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database entry for file %d set to Path %s Filename %s",m_pMoveJob->m_pRow_File->PK_File_get(),sDestinationPath.c_str(),sDestinationBaseName.c_str());
    }
  else
    m_eTaskStatus_set(TASK_FAILED_ABORT);  // If this didn't work, no point in doing other tasks.

  return 0; // Done.
}

bool MoveDBTask::Abort()
{
  // Might be implemented later, but honestly, this will happen so fast...
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Transcode job and task classes ///////////////////////////////////////////////////////////
TranscodeJob::TranscodeJob(class JobHandler *pJobHandler,
			   int iPK_Orbiter,
			   string sFileName,
			   string sDestinationFileName,
			   map<string, string> mapParametersAndValues,
			   bool bDeleteSourceAfterTranscode,
			   Command_Impl *pCommand_Impl)
  : Job(pJobHandler, "TranscodeJob", iPK_Orbiter, pCommand_Impl)
{
  m_sFileName=sFileName;
  m_bDeleteSourceAfterTranscode=bDeleteSourceAfterTranscode;
  m_sDestinationFileName=sDestinationFileName;
  m_mapParametersAndValues=mapParametersAndValues;
  m_iPK_Orbiter=iPK_Orbiter;
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranscodeJob - start");
}

TranscodeJob::~TranscodeJob()
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranscodeJob - stop");
  if (m_bDeleteSourceAfterTranscode)
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Deleting %s after transcode.");
      FileUtils::DelFile(m_sFileName);
    }

  SCREEN_PopupMessage SCREEN_PopupMessage(m_pCommand_Impl->m_dwPK_Device,
					  m_iPK_Orbiter,
					  "Transcode Complete!",
					  "",
					  "transcode_complete",
					  "0",
					  "10",
					  "1");
  m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
}

bool TranscodeJob::ReadyToRun()
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranscodeJob - ReadyToRun");
  AddTranscodeTask();
  return true;
}

void TranscodeJob::AddTranscodeTask()
{
  vector <Task *> vTasks;
  if (FileUtils::FileExists(m_sFileName))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Task Added!");
      vTasks.push_back(new TranscodeTask(this,"Transcode File",m_sFileName,m_sDestinationFileName));
      AddTasks(vTasks,position_TasklistEnd);
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Task Not Added! %s does not exist",m_sFileName.c_str());
    }
}

bool TranscodeJob::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
  PLUTO_SAFETY_LOCK(jm,m_ThreadMutex);
  if (m_eJobStatus==job_WaitingToStart || m_eJobStatus==job_InProgress)
    {
      if (pPendingTaskList)
	{
	  pPendingTaskList->m_listPendingTask.push_back(new PendingTask(m_iID,
									m_iPK_Orbiter,
									m_pCommand_Impl->m_dwPK_Device,
									"transcode",ToString(),
									(char)PercentComplete(),
									SecondsRemaining(),
									true));
	}
      return true;
    }
  else
    return false;
}

int TranscodeJob::PercentComplete()
{
  return 1;
}

int TranscodeJob::SecondsRemaining()
{
  return 255;
}

string TranscodeJob::ToString()
{
  return "Transcoding "+m_sFileName+" to "+m_sDestinationFileName;
}

int TranscodeJob::Get_PK_Orbiter()
{
  return m_iPK_Orbiter;
}

TranscodeTask::TranscodeTask(class TranscodeJob *pTranscodeJob,
			     string sName,
			     string sFileName,
			     string sDestinationFileName) : Task(pTranscodeJob,sName)
{
  m_bAlreadySpawned=false;
  m_pTranscodeJob=pTranscodeJob;
  m_sFileName=sFileName;
  m_sDestinationFileName=sDestinationFileName;
  m_mapParametersAndValues=pTranscodeJob->m_mapParametersAndValues;
}

TranscodeTask::~TranscodeTask()
{
  // does nothing.
}

string TranscodeTask::GetParameters()
{
  string sParameters="";
  for (map<string, string>::iterator it=m_mapParametersAndValues.begin();
       it!=m_mapParametersAndValues.end();++it)
    {
      sParameters += "-"+it->first+"\t"+it->second+"\t";
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX added parameter: -%s %s",it->first.c_str(),it->second.c_str());
    }

  return sParameters;

}

int TranscodeTask::Run()
{
  if (m_bAlreadySpawned)
    {
      return RunAlreadySpawned();
    }

  DeviceData_Base *pDevice_App_Server = m_pTranscodeJob->m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST, m_pTranscodeJob->m_pCommand_Impl);

  if (!pDevice_App_Server)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranscodeTask::Run() - Cannot transcode. No App Server found.");
      return 0;
    }

  m_pDevice_App_Server=pDevice_App_Server;
  //  string sParameters = "-i\t"+m_sFileName+"\t"+GetParameters()+m_sDestinationFileName; // ffmpeg
  string sParameters=GetParameters()+"-o\t"+m_sDestinationFileName+"\t"+m_sFileName;     // mencoder
  string sResultMessage =
    StringUtils::itos(m_pTranscodeJob->m_pCommand_Impl->m_dwPK_Device) + " " +   // From device
    StringUtils::itos(m_pTranscodeJob->m_pCommand_Impl->m_dwPK_Device) + " " +   // To Device
    StringUtils::itos(MESSAGETYPE_COMMAND) + " " +                          // 1 = COMMAND
    StringUtils::itos(COMMAND_Update_Transcode_Status_CONST) + " " +             // CMD_Update_Transcode_Status
    StringUtils::itos(COMMANDPARAMETER_Task_CONST) + " " +                  //
    StringUtils::itos(m_iID) + " " +                                  // Task ID
    StringUtils::itos(COMMANDPARAMETER_Job_CONST) + " " +                   //
    StringUtils::itos(m_pJob->m_iID_get()) + " " +                          // Job ID
    StringUtils::itos(COMMANDPARAMETER_Status_CONST) + " ";  // The Rest will be filled in below.

  // Passed to the App Server as a unique name that can be killed later.
  m_sSpawnName = "transcode_job_" +
    StringUtils::itos(m_pJob->m_iID_get()) +
    "_task_" +
    StringUtils::itos(m_iID);

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXX PARAMETERS IS: %s",sParameters.c_str());

  DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pTranscodeJob->m_pCommand_Impl->m_dwPK_Device,
						   pDevice_App_Server->m_dwPK_Device,
						   "/usr/bin/mencoder",
						   m_sSpawnName,
						   sParameters,
						   sResultMessage + "e",
						   sResultMessage + "s",
						   false, false, false, false);

  string sResponse;

  if (!m_pTranscodeJob->m_pCommand_Impl->SendCommand(CMD_Spawn_Application,&sResponse) || sResponse != "OK")
    {
      // App server failed somehow.
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranscodeTask::Run() - Trying to move - App Server returned %s",sResponse.c_str());
      return 0; // This is done. Do not trigger the run again.
    }

  m_bAlreadySpawned=true;

  string sText = "Your game recording is being processed and stored. I will let you know when it is finished.";
  string sOptions = "Ok|";

  DCE::CMD_Display_Dialog_Box_On_Orbiter_Cat db(m_pTranscodeJob->m_pCommand_Impl->m_dwPK_Device,
						DEVICECATEGORY_Orbiter_Plugins_CONST,
						false, BL_SameHouse,
						sText,
						sOptions,
						StringUtils::itos(m_pTranscodeJob->Get_PK_Orbiter()));

  m_pTranscodeJob->m_pCommand_Impl->SendCommand(db);

  return 1000;
}

bool TranscodeTask::Abort()
{
  DCE::CMD_Kill_Application CMD_Kill_Application(m_pTranscodeJob->m_pCommand_Impl->m_dwPK_Device,
						 m_pDevice_App_Server->m_dwPK_Device,
						 m_sSpawnName,
						 false);
  m_pTranscodeJob->m_pCommand_Impl->SendCommand(CMD_Kill_Application);
  
  return Task::Abort();

}

int TranscodeTask::RunAlreadySpawned()
{
  return 1000;
}

void TranscodeTask::UpdateProgress(string sStatus, int iPercent, int iTime, string sText)
{
  if( m_eTaskStatus_get()==TASK_COMPLETED )
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS, "MoveTask::UpdateProgress ignoring because we're done status %s", sStatus.c_str());
      return;
    }
  
  m_sText=sText; // expose new status text to task so other parts can use it.

  if ( sStatus=="p" )
    {
      m_iPercent=iPercent;
      m_iTime=iTime;
      ReportProgress();
    }
  else if ( sStatus=="e" )
    {
	ReportFailure();
      m_eTaskStatus_set(TASK_FAILED_ABORT); // mark it failed so the job will be aborted.
    }
  else if ( sStatus=="s" )
    {
	ReportSuccess();
      m_eTaskStatus_set(TASK_COMPLETED);
    }

}

void TranscodeTask::ReportFailure()
{
    // It's here in case we need to report on a task by task basis.
}

void TranscodeTask::ReportSuccess()
{
  // It's here in case we need to report on a task by task basis.
}

void TranscodeTask::ReportProgress()
{
  // It's here in case we need to do something.
}

int TranscodeTask::SecondsRemaining()
{
  if ( m_iTime )
    return m_iTime;

  return Task::SecondsRemaining();

}
/////////////////////////////////////////////////////////////////////////////////////////////
static bool MediaFileComparer(MediaFile *a, MediaFile *b)
{
	if(a->m_sAlbum != b->m_sAlbum)
	{
		if(a->m_sAlbum.empty())
			return false;
		else if(b->m_sAlbum.empty())
			return true;
		else
			return strcmp(a->m_sAlbum.c_str(), b->m_sAlbum.c_str()) < 0;
	}

	if(a->m_iTrack != b->m_iTrack)
	{
		if(a->m_iTrack == 0)
			return false;
		else if(b->m_iTrack == 0)
			return true;
		else
			return a->m_iTrack < b->m_iTrack;
	}

	return strcmp(a->m_sFilename.c_str(), b->m_sFilename.c_str()) < 0;
}

// For sorting by priority
static bool MediaHandlerComparer(MediaHandlerInfo *x, MediaHandlerInfo *y)
{
	// Reverse since we want highest priorities first
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Priority X %d and Y %d", x->m_pMediaHandlerBase->m_iPriority, y->m_pMediaHandlerBase->m_iPriority);
	return x->m_pMediaHandlerBase->m_iPriority>y->m_pMediaHandlerBase->m_iPriority;
}

MediaDevice::MediaDevice( class Router *pRouter, class Row_Device *pRow_Device,class Media_Plugin *pMediaPlugin )
{
	m_pRouter=pRouter;
	m_pMediaPlugin=pMediaPlugin;
	m_iLastPlaybackSpeed = 1000;
	m_pDeviceData_Router = pRouter->m_mapDeviceData_Router_Find( pRow_Device->PK_Device_get( ) );
	m_pOH_Orbiter_OSD = NULL;
	m_bDontSendOffIfOSD_ON=false;
	m_pOH_Orbiter_Reset=NULL;
	m_pCommandGroup=NULL;
	m_tReset=0;
	m_pDevice_Audio=m_pDevice_Video=m_pDevice_Media_ID=NULL;
	m_dwPK_Command_Audio=m_dwPK_Command_Video=0;
	m_iPK_MediaProvider=atoi(m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_EK_MediaProvider_CONST).c_str());
	m_bCaptureCardActive=false;
	m_bPreserveAspectRatio=m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_Preserve_Aspect_Ratio_CONST)=="1";
	m_dwPK_Command_LastAdjustment_Audio=m_dwPK_Command_LastAdjustment_Video=m_dwPK_Command_LastAdjustment_Command=0;
	string sAdjustRules=m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_AV_Adjustment_Rules_CONST);
	vector<string> vectAdjustRules;
	StringUtils::Tokenize(sAdjustRules,"\r\n",vectAdjustRules);
	for(vector<string>::iterator it=vectAdjustRules.begin();it!=vectAdjustRules.end();++it)
	{
		string::size_type pos = it->find('=');
		if( pos!=string::npos )
			m_mapAdjustmentRules[it->substr(0,pos)]=atoi(it->substr(pos+1).c_str());
	}
	m_iDelayForCaptureCard=0;
	m_iLastVolume=-1;
	m_bMute=false;

	int PK_Device_CaptureCard = atoi(m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_FK_Device_Capture_Card_Port_CONST).c_str());
	if( PK_Device_CaptureCard )
        m_pDevice_CaptureCard = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_CaptureCard);
	else
		m_pDevice_CaptureCard = NULL;

	m_pRow_MediaProvider = NULL;
	m_tLastPowerCommand=0;
	m_dwPK_Command_LastPower=m_dwPK_Command_LastPower=0;

	DeviceData_Router *pDeviceData_Router_Source = m_pDeviceData_Router;  // The source is ourselves unless we're a software source and then it's the MD
	if( (m_pDeviceData_Router->m_pDevice_ControlledVia && m_pDeviceData_Router->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Media_Director_CONST)) ||
		(m_pDeviceData_Router->m_pDevice_ControlledVia && m_pDeviceData_Router->m_pDevice_ControlledVia->m_pDevice_ControlledVia && m_pDeviceData_Router->m_pDevice_ControlledVia->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Media_Director_CONST)) )
			if( m_pDeviceData_Router->m_pDevice_MD )
				pDeviceData_Router_Source = (DeviceData_Router *) m_pDeviceData_Router->m_pDevice_MD;

	// and has a pipe for audio -- if so, we won't adjust the PC's volume
	Pipe *pPipe_Audio = pDeviceData_Router_Source->m_mapPipe_Available_Find(1);
	Pipe *pPipe_Video = pDeviceData_Router_Source->m_mapPipe_Available_Find(2);

	if( !pPipe_Audio )
	{
		if( pDeviceData_Router_Source->WithinCategory(DEVICECATEGORY_Media_Director_CONST) )
		{
			// Since there's no other a/v device for volume adjustments, we'll send it to an app server
			vector<DeviceData_Router *> vectDeviceData_Router;
			if( pDeviceData_Router_Source->m_pDevice_ControlledVia )
				( (DeviceData_Router *)pDeviceData_Router_Source->m_pDevice_ControlledVia)->FindChildrenWithinCategory(DEVICECATEGORY_App_Server_CONST,vectDeviceData_Router);
			else
				pDeviceData_Router_Source->FindChildrenWithinCategory(DEVICECATEGORY_App_Server_CONST,vectDeviceData_Router);
			if( vectDeviceData_Router.size() )
				m_pDevice_Audio = vectDeviceData_Router[0];
		}
	}
	else
		m_pDevice_Audio = pRouter->m_mapDeviceData_Router_Find(FindUltimateDestinationViaPipe(pPipe_Audio,1,m_dwPK_Command_Audio));

	if( pPipe_Video )
		m_pDevice_Video = pRouter->m_mapDeviceData_Router_Find(FindUltimateDestinationViaPipe(pPipe_Video,2,m_dwPK_Command_Video));

	if( !m_pDevice_Audio )
		m_pDevice_Audio = m_pDeviceData_Router;
	if( !m_pDevice_Video )
		m_pDevice_Video = m_pDeviceData_Router;
}

int MediaDevice::FindUltimateDestinationViaPipe(Pipe *pPipe,int PK_Pipe,int &PK_Command_Input)
{
	string sSQL = "FK_Device_From=" + StringUtils::itos(pPipe->m_pDevice_To->m_dwPK_Device) + " AND FK_Pipe=" + StringUtils::itos(PK_Pipe);
	vector<Row_Device_Device_Pipe *> vectRow_Device_Device_Pipe;
	m_pMediaPlugin->m_pDatabase_pluto_main->Device_Device_Pipe_get()->GetRows(sSQL,&vectRow_Device_Device_Pipe);
	if( vectRow_Device_Device_Pipe.size() )
	{
		Row_Device_Device_Pipe *pRow_Device_Device_Pipe = vectRow_Device_Device_Pipe[0];
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find( pRow_Device_Device_Pipe->FK_Device_To_get() );
		if( pDeviceData_Router )
		{
			Pipe *pPipe2 = pDeviceData_Router->m_mapPipe_Available_Find(PK_Pipe);
			if( pPipe2 )
				return FindUltimateDestinationViaPipe(pPipe2,PK_Pipe,PK_Command_Input);
			else
			{
				PK_Command_Input = pRow_Device_Device_Pipe->FK_Command_Input_get();
				return pRow_Device_Device_Pipe->FK_Device_To_get();
			}
		}
	}
	PK_Command_Input = pPipe->m_pCommand_Input ? pPipe->m_pCommand_Input->m_dwPK_Command : 0;
	return pPipe->m_pDevice_To->m_dwPK_Device;
}

//<-dceag-const-b->! custom// i get
Media_Plugin::Media_Plugin( int DeviceID, string ServerAddress, bool bConnectEventHandler, bool bLocalMode, class Router *pRouter )
    : Media_Plugin_Command( DeviceID, ServerAddress, bConnectEventHandler, bLocalMode, pRouter ),
    m_MediaMutex( "Media Plugin" )
//<-dceag-const-e->
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_MediaMutex.Init( &m_MutexAttr );

	srand((int) time(NULL)); // Shuffle uses a random generator

	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;

	m_iStreamID=1000; // Start with a higher number in case temporary stuff like video wizard uses low numbers
	m_iPK_File_Last_Scanned_For_New=0;
	m_pDatabase_pluto_main=NULL;
	m_pDatabase_pluto_media=NULL;
	m_pMediaAttributes=NULL;
	m_pGeneric_NonPluto_Media=NULL;
    m_pGenericMediaHandlerInfo=NULL;
	UniqueColors[0] = PlutoColor(128,0,0).m_Value;
	UniqueColors[1] = PlutoColor(0,128,0).m_Value;
	UniqueColors[2] = PlutoColor(0,0,128).m_Value;
	UniqueColors[3] = PlutoColor(0,128,128).m_Value;
	UniqueColors[4] = PlutoColor(128,128,0).m_Value;
	m_pAlarmManager=NULL;
	m_pJobHandler = new JobHandler();
}

//<-dceag-getconfig-b->
bool Media_Plugin::GetConfig()
{
	if( !Media_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit_set(true);
        return false;
    }

    m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_media->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit_set(true);
        return false;
    }

	vector<Row_MediaType_AttributeType *> vectMediaType_AttributeType;
	m_pDatabase_pluto_media->MediaType_AttributeType_get()->GetRows("Identifier=1",&vectMediaType_AttributeType);
	for(vector<Row_MediaType_AttributeType *>::iterator it=vectMediaType_AttributeType.begin();it!=vectMediaType_AttributeType.end();++it)
        m_mapMediaType_AttributeType_Identifier[ make_pair<int,int> ((*it)->EK_MediaType_get(),(*it)->FK_AttributeType_get()) ] = true;

    m_pMediaAttributes = new MediaAttributes( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ), m_pRouter->iPK_Installation_get() );

    Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get( )->GetRow( m_pRouter->iPK_Installation_get( ) );
    vector<Row_Room *> vectRow_Room; // Ent Areas are specified by room. Get all the rooms first
    pRow_Installation->Room_FK_Installation_getrows( &vectRow_Room );
    for( size_t iRoom=0;iRoom<vectRow_Room.size( );++iRoom )
    {
        Row_Room *pRow_Room=vectRow_Room[iRoom];
        vector<Row_EntertainArea *> vectRow_EntertainArea;
        pRow_Room->EntertainArea_FK_Room_getrows( &vectRow_EntertainArea );
        for( size_t s=0;s<vectRow_EntertainArea.size( );++s )
        {
            Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[s];
            EntertainArea *pEntertainArea = new EntertainArea( pRow_EntertainArea->PK_EntertainArea_get( ),
				( pRow_EntertainArea->Only1Stream_get( )==1 ), pRow_EntertainArea->Description_get(),
				m_pRouter->m_mapRoom_Find(pRow_Room->PK_Room_get()) );
            m_mapEntertainAreas[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
            // Now find all the devices in the ent area
            vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
            pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows( &vectRow_Device_EntertainArea );
            for( size_t s2=0;s2<vectRow_Device_EntertainArea.size( );++s2 )
            {
                Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s2];
if( !pRow_Device_EntertainArea || !pRow_Device_EntertainArea->FK_Device_getrow( ) )
{
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device_EntertainArea refers to a NULL device %d %d",pRow_Device_EntertainArea->FK_EntertainArea_get(),pRow_Device_EntertainArea->FK_Device_get());
//pRow_Device_EntertainArea->Delete();
//TODO: add the delete above and also do a commit.
continue;
}


                // This will recurse all the other devices in the path, and add them too
                AddDeviceToEntertainArea( pEntertainArea, pRow_Device_EntertainArea->FK_Device_getrow( ) );

                // If this is a media director, be sure all the device's children, like the media players, disc drives, etc. are also in the same entertainment area
                // We don't do this for other devices because some devices, like whole house amps, have children that are distinct zones in their own entertainment areas
                if( pRow_Device_EntertainArea->FK_Device_getrow( )->FK_DeviceTemplate_getrow( )->FK_DeviceCategory_get( )==DEVICECATEGORY_Media_Director_CONST )
                {
                    vector<Row_Device *> vectRow_Device;
                    pRow_Device_EntertainArea->FK_Device_getrow( )->Device_FK_Device_ControlledVia_getrows( &vectRow_Device ); // all it's children
                    for( size_t child=0;child<vectRow_Device.size( );++child )
                        AddDeviceToEntertainArea( pEntertainArea, vectRow_Device[child] );
                }
            }
        }
    }

	// Streamers are often not in entertainment areas.  But we need them to have an entry as
	// a MediaDevice so they can be a media source
	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Media_Streamers_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDeviceData_Router = (*it);
		    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pDeviceData_Router->m_dwPK_Device );
			if( !pMediaDevice && m_mapEntertainAreas.size() )
			{
				EntertainArea *pEntertainArea = (*m_mapEntertainAreas.begin()).second;
				AddDeviceToEntertainArea(pEntertainArea,pDeviceData_Router->m_pRow_Device);
			}
		}
	}

	// Now go through all the devices in all the ent areas, and add the reverse match so all devices have a list of the entertain areas they're in
    for( map<int, EntertainArea *>::iterator itEntArea=m_mapEntertainAreas.begin( );itEntArea!=m_mapEntertainAreas.end( );++itEntArea )
    {
        class EntertainArea *pEntertainArea = ( *itEntArea ).second;
        for( map<int, class MediaDevice *>::iterator itDevice=pEntertainArea->m_mapMediaDevice.begin( );itDevice!=pEntertainArea->m_mapMediaDevice.end( );++itDevice )
        {
            class MediaDevice *pMediaDevice = ( *itDevice ).second;
            pMediaDevice->m_mapEntertainArea[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
        }
    }

	// Go through all the command groups, looking for ones that are used to turn a device on.  If the device
	// is turned on manually, we'll execute that command group
    for(map<int,CommandGroup *>::const_iterator it=m_pRouter->m_mapCommandGroup_get()->begin();it!=m_pRouter->m_mapCommandGroup_get()->end();++it)
    {
		CommandGroup *pCommandGroup = it->second;
		for(size_t s=0;s<pCommandGroup->m_vectCommandGroup_Command.size();++s)
		{
			CommandGroup_Command *pCommandGroup_Command = pCommandGroup->m_vectCommandGroup_Command[s];
			if( pCommandGroup_Command->m_pCommand->m_dwPK_Command==COMMAND_MH_Play_Media_CONST &&
				pCommandGroup_Command->m_mapParameter.find(COMMANDPARAMETER_PK_Device_CONST)!=pCommandGroup_Command->m_mapParameter.end() )
			{
				int PK_Device = atoi(pCommandGroup_Command->m_mapParameter[COMMANDPARAMETER_PK_Device_CONST].c_str());
				MediaDevice *pMediaDevice = m_mapMediaDevice_Find(PK_Device);
				if( pMediaDevice )
				{
					pMediaDevice->m_pCommandGroup = pCommandGroup;
					break;
				}
			}
		}
	}

	// See which devices have different output zones, and save them in a map indexed by the ent area
	for(map<int, class MediaDevice *>::iterator it=m_mapMediaDevice.begin();it!=m_mapMediaDevice.end();++it)
	{
		MediaDevice *pMediaDevice = it->second;
		if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Output_Zone_CONST) )
		{
			// The output zones should only be in 1 EA.  Map it
			MediaDevice *pMediaDevice_Parent = m_mapMediaDevice_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device_ControlledVia);
			if( pMediaDevice_Parent )
				for(map<int,class EntertainArea *>::iterator itEA=pMediaDevice->m_mapEntertainArea.begin();itEA!=pMediaDevice->m_mapEntertainArea.end();++itEA)
				{
					EntertainArea *pEntertainArea = itEA->second;
					pMediaDevice_Parent->m_mapOutputZone[pEntertainArea->m_iPK_EntertainArea] = pMediaDevice;

					// Now see what other devices feed into the parent.  All of them can thus feed into us as well
					RecursivelyAddSendingDevice(pMediaDevice,pMediaDevice_Parent);
				}
		}
	}

	m_pGeneric_NonPluto_Media = new Generic_NonPluto_Media(this);
    m_pGenericMediaHandlerInfo = new MediaHandlerInfo(m_pGeneric_NonPluto_Media,this,0,0,false,false);

	DCEConfig dceconfig;
	string sMediaType = dceconfig.m_mapParameters_Find("Bookmark_Media");
	string::size_type pos=0;
	while( pos<sMediaType.size() && pos!=string::npos )
	{
		int PK_MediaType = atoi(StringUtils::Tokenize(sMediaType,",",pos).c_str());
		m_mapMediaType_Bookmarkable[PK_MediaType]=true;
	}

#ifdef SIM_JUKEBOX
	{
		string sSQL = "SELECT FK_File,Name FROM File_Attribute JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Purchase_Info_CONST);
		PlutoSqlResult result;
		DB_ROW row;
		if( (result.r = m_pDatabase_pluto_media->db_wrapper_query_result(sSQL))  )
			while( (row = db_wrapper_fetch_row( result.r )) )
				m_mapPK_FilesForSimulatedPurchase[ atoi(row[0]) ] = row[1];
	}
#endif

	DeviceData_Base *pDevice = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Media_Identifiers_CONST);
	if( pDevice )
		m_dwPK_Device_MediaIdentification = pDevice->m_dwPK_Device;

	string sLastSearchTokenUpdate = dceconfig.m_mapParameters_Find("LastSearchTokenUpdate");
	m_tLastSearchTokenUpdate = atoi(sLastSearchTokenUpdate.c_str());

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      // number of worker threads
	CMD_Refresh_List_of_Online_Devices();
	m_pAlarmManager->AddRelativeAlarm(15,this,CHECK_FOR_NEW_FILES,NULL);
	m_pAlarmManager->AddRelativeAlarm(600,this,UPDATE_SEARCH_TOKENS,NULL);
	m_pAlarmManager->AddRelativeAlarm(30,this,UPDATE_ATTRIBUTE_CACHE,NULL);

	// and finally, start the Job Handler thread.
	bool bResult = m_pJobHandler->StartThread();

	return true;
}


//<-dceag-const2-b->!

//<-dceag-dest-b->
Media_Plugin::~Media_Plugin()
//<-dceag-dest-e->
{

	delete m_pJobHandler;  // Delete this first since it can reference the others
	m_pJobHandler = NULL;

	{
		PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

		for(map<int,MediaDevice *>::iterator it=m_mapMediaDevice.begin();it!=m_mapMediaDevice.end();++it)
			delete (*it).second;
		for(map<int,EntertainArea *>::iterator it=m_mapEntertainAreas.begin();it!=m_mapEntertainAreas.end();++it)
			delete (*it).second;

		for(size_t s=0;s<m_vectMediaHandlerInfo.size();++s)
		{
			MediaHandlerInfo *pMediaHandlerInfo = m_vectMediaHandlerInfo[s];
			delete pMediaHandlerInfo;
		}

		delete m_pGenericMediaHandlerInfo;
		delete m_pGeneric_NonPluto_Media;

		for(int i = 0; i < NUM_UI_VERSIONS; ++i)
		{
			for(map< int, class RemoteControlSet *>::iterator itm = m_mapMediaType_RemoteControl[i].begin();
				itm != m_mapMediaType_RemoteControl[i].end(); ++itm)
			{
				delete itm->second;
			}
			m_mapMediaType_RemoteControl[i].clear();

			map< pair<int,int>, class RemoteControlSet *>::iterator itp;
			for(itp = m_mapOrbiter_MediaType_RemoteControl[i].begin();
				itp != m_mapOrbiter_MediaType_RemoteControl[i].end(); ++itp)
			{
				delete itp->second;
			}
			m_mapOrbiter_MediaType_RemoteControl[i].clear();

			for(itp = m_mapDeviceTemplate_MediaType_RemoteControl[i].begin();
				itp != m_mapDeviceTemplate_MediaType_RemoteControl[i].end(); ++itp)
			{
				delete itp->second;
			}
			m_mapDeviceTemplate_MediaType_RemoteControl[i].clear();
		}
	}

	delete m_pMediaAttributes;
	m_pMediaAttributes = NULL;
	delete m_pDatabase_pluto_main;
	m_pDatabase_pluto_main = NULL;
	delete m_pDatabase_pluto_media;
	m_pDatabase_pluto_media = NULL;

	pthread_mutex_destroy(&m_MediaMutex.mutex);
    pthread_mutexattr_destroy(&m_MutexAttr);
}

void Media_Plugin::PrepareToDelete()
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Media_Plugin::Register()
//<-dceag-reg-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
    m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
    m_pFile_Grids_Plugin=( File_Grids_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_File_Grids_Plugin_CONST);

	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin || !m_pFile_Grids_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to media plugin");
		return false;
	}

	for( map<int, class MediaDevice *>::iterator itDevice=m_mapMediaDevice.begin( );itDevice!=m_mapMediaDevice.end( );++itDevice )
    {
        class MediaDevice *pMediaDevice = ( *itDevice ).second;

		if( pMediaDevice->m_pDevice_CaptureCard )
		{
			DeviceData_Base *pDevice_Orbiter = pMediaDevice->m_pDevice_CaptureCard->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Orbiter_CONST);
			if( pDevice_Orbiter )
				pMediaDevice->m_pOH_Orbiter_OSD = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pDevice_Orbiter->m_dwPK_Device);
		}
		else if ( pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia &&
			pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Orbiter_CONST)
			)
		{
			// store the orbiter which is directly controlling the target device as the target on-screen display.
			pMediaDevice->m_pOH_Orbiter_OSD = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device_ControlledVia);
		}
		else if( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate==DEVICETEMPLATE_OnScreen_Orbiter_CONST )
			for(map<int,class EntertainArea *>::iterator it=pMediaDevice->m_mapEntertainArea.begin();it!=pMediaDevice->m_mapEntertainArea.end();++it)
				it->second->m_pOH_Orbiter_OSD = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);
	}

    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaInserted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Inserted_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::PlaybackCompleted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::PlaybackStarted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Started_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaFollowMe ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Follow_Me_Media_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::DeviceOnOff ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_OnOff_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::AvInputChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_AV_Input_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::MediaDescriptionChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Description_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::VolumeChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Volume_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::VolumeChanged ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Set_Volume_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::VolumeChanged ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Mute_CONST );

    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::SubtitleTrackChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Subtitle_Track_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::AudioTrackChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Audio_Track_Changed_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::VideoTrackChanged ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Video_Track_Changed_CONST );


    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaBrowser) )
        , DATAGRID_Media_Browser_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::CurrentMediaSections) )
        , DATAGRID_Current_Media_Sections_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::CDTracks) )
        , DATAGRID_CD_Tracks_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::AvailablePlaylists) )
        , DATAGRID_Playlists_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaSearchAutoCompl ) )
        , DATAGRID_Media_Search_Auto_Compl_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrFiles ) )
        , DATAGRID_Media_Attr_Files_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrCollections ) )
        , DATAGRID_Media_Attr_Collections_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrXref ) )
        , DATAGRID_Media_Attr_Xref_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaItemAttr ) )
        , DATAGRID_Media_Item_Attr_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DevicesPipes ) )
        , DATAGRID_Devices__Pipes_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrCurStream ) )
        , DATAGRID_Media_Attr_Cur_Stream_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::MediaAttrFile ) )
        , DATAGRID_Media_Attributes_For_File_CONST,PK_DeviceTemplate_get() );

	// datagrids to support the floorplans
    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::FloorplanMediaChoices ) )
        , DATAGRID_Floorplan_Media_Choices_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::ActiveMediaStreams ), true )
        , DATAGRID_Floorplan_Media_Streams_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DVDSubtitles ))
        , DATAGRID_DVD_Subtitles_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DVDAudioTracks ))
        , DATAGRID_DVD_Audio_Tracks_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DVDAngles ))
        , DATAGRID_DVD_Angles_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::Bookmarks ))
        , DATAGRID_Bookmarks_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::BookmarksByMediaType ))
        , DATAGRID_Bookmarks_by_MediaType_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::CaptureCardPorts ))
        , DATAGRID_Capture_Card_Ports_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
	new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::CaptureCardAudioPorts ))
	, DATAGRID_Capture_Card_Audio_Ports_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DevicesForCaptureCardPort ))
        , DATAGRID_Capture_Card_Port_Devices_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::DevicesNeedingProviders ))
        , DATAGRID_Devices_Needing_Providers_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::ProvidersForDevice ))
        , DATAGRID_Providers_For_Device_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::ThumbnailableAttributes ))
        , DATAGRID_Thumbnailable_Attributes_CONST,PK_DeviceTemplate_get() );

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Media_Plugin::ThumbnailableAttributes ))
        , DATAGRID_Thumbnailable_Attributes_CONST,0 );  // Register as 0 also, because some media plugins may implement this, but not all.  See: Datagrid_PluginGetCallBack

	PopulateRemoteControlMaps();
	RestoreMediaResumePreferences();

    return Connect(PK_DeviceTemplate_get());
}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Media_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Media_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

void Media_Plugin::AddDeviceToEntertainArea( EntertainArea *pEntertainArea, Row_Device *pRow_Device )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    if( pEntertainArea->m_mapMediaDevice_Find( pRow_Device->PK_Device_get( ) ) || m_pRouter->m_mapDeviceData_Router_Find( pRow_Device->PK_Device_get( ) )==NULL )
        return; // We already added this one, or it's not in our installation

    // See if we already have this media device in our master list
    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pRow_Device->PK_Device_get( ) );
    if( !pMediaDevice )
    {
        pMediaDevice = new MediaDevice( m_pRouter, pRow_Device,this ); // Nope, create it
		if( pMediaDevice->m_iPK_MediaProvider )
			pMediaDevice->m_pRow_MediaProvider = m_pDatabase_pluto_media->MediaProvider_get()->GetRow( pMediaDevice->m_iPK_MediaProvider );

        m_mapMediaDevice[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device]=pMediaDevice;

		vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
		pMediaDevice->m_pDeviceData_Router->m_pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);

		// See if this is an Disc Drive, if so, get the media identifier
		if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Disc_Drives_CONST) )
		{
			vector<DeviceData_Router *> vectDeviceData_Router;
			pMediaDevice->m_pDeviceData_Router->FindSibblingsWithinCategory( DEVICECATEGORY_Media_Identifiers_CONST, vectDeviceData_Router);
			if( vectDeviceData_Router.size() )
				pMediaDevice->m_pDevice_Media_ID = vectDeviceData_Router[0];
		}

		if( pMediaDevice->m_pDeviceData_Router->m_pDevice_RouteTo )  // If it's an embedded device like a tuner, add the master's device types too
			pMediaDevice->m_pDeviceData_Router->m_pDevice_RouteTo->m_pRow_Device->FK_DeviceTemplate_getrow()->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);

		for(size_t mt=0;mt<vectRow_DeviceTemplate_MediaType.size();++mt)
		{
			Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[mt];
			pMediaDevice->m_mapMediaType[pRow_DeviceTemplate_MediaType->FK_MediaType_get()]=pRow_DeviceTemplate_MediaType->FK_MediaType_getrow();
		}
	}
    pEntertainArea->m_mapMediaDevice[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device]=pMediaDevice;

	if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Media_Director_CONST) )
		pEntertainArea->m_pMediaDevice_MD = pMediaDevice;

LoggerWrapper::GetInstance()->Write( LV_STATUS, "adding device %d %s to map ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),
					  pEntertainArea->m_iPK_EntertainArea,pEntertainArea->m_sDescription.c_str());

    map<int, ListMediaDevice *> m_mapMediaDeviceByTemplate;  /** All the media devices in the area by device template */
    ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(pRow_Device->FK_DeviceTemplate_get());
    if( !pListMediaDevice )
    {
        pListMediaDevice = new ListMediaDevice();
        pEntertainArea->m_mapMediaDeviceByTemplate[pRow_Device->FK_DeviceTemplate_get()]=pListMediaDevice;
    }
    pListMediaDevice->push_back(pMediaDevice);

	if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_LCDVFD_Displays_CONST) )
		pEntertainArea->m_listVFD_LCD_Displays.push_back(pMediaDevice);
}

// Our message interceptor
bool Media_Plugin::MediaInserted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int PK_MediaType = atoi( pMessage->m_mapParameters[EVENTPARAMETER_FK_MediaType_CONST].c_str( ) );
	int discid = atoi( pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST].c_str( ) );
	// We will embed into the MRL \t(disk drive)\t so that in the plugin the system can know if we're playing a drive other than our local one
	string MRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST] + "\t(" + StringUtils::itos(pMessage->m_dwPK_Device_From) + ")\t";

    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got a media inserted at drive %d, of type %d ", pDeviceFrom->m_dwPK_Device, PK_MediaType );

    // First figure out what entertainment area this corresponds to. We are expecting that whatever media player is running on this pc will have
    // added the disc drive to it's entertainment area when it registered
    MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pDeviceFrom->m_dwPK_Device );
    if( !pMediaDevice )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Got a media inserted event from %d -- a drive that isn't a media device", pDeviceFrom->m_dwPK_Device );
        return false; // Let someone else handle it
    }

    if( !pMediaDevice->m_mapEntertainArea.size( ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Got a media inserted event from a drive that doesn't exist in an entertainment area" );
        return false; // Let someone else handle it
    }

/*
    if ( PK_MediaType == MEDIATYPE_pluto_HDDVD_CONST || PK_MediaType == MEDIATYPE_pluto_BD_CONST )
    {
	    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Detected HD-DVD/Bluray disk inserted at drive %d, asking user if he wants to rip it", pDeviceFrom->m_dwPK_Device );

	    EntertainArea *pEntertainArea = pMediaDevice->m_mapEntertainArea.begin( )->second;

	    // this block is a duplicate, with only difference it iterates over the remote Orbiter
	    for (map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin(); it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end(); ++it)
	    {
		if (  (!pEntertainArea->m_pRoom) || (it->second->m_dwPK_Room != pEntertainArea->m_pRoom->m_dwPK_Room) )
			continue;

		int iPK_Device_Orbiter = it->second->m_pDeviceData_Router->m_dwPK_Device;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "DEBUG: asking user at Orbiter %d", iPK_Device_Orbiter);

		string sDescription = "HD-DVD/Bluray disk is inserted, do you want to rip it?";
		string sRipDescription = "The default name to save this media under is shown below.  You may change it.  Then, touch 'Save Public' to make this media public, or, confirm that you are the current user and touch 'Save Private'.  Use / for subdirectories, like: dir/filename";
		string sButton1 = "Yes";

		string sCommand1 = StringUtils::itos(m_dwPK_Device) + " " +
			StringUtils::itos(iPK_Device_Orbiter) + " " +
			StringUtils::itos(MESSAGETYPE_COMMAND) + " " +
			StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " +
			StringUtils::itos(COMMANDPARAMETER_PK_MediaType_CONST)
			+ " " + "0" + " " +
			StringUtils::itos(COMMANDPARAMETER_EK_Disc_CONST) + " "
			+ "0" + " " +
			StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " "
			+ "19" + " " +
			StringUtils::itos(COMMANDPARAMETER_Caption_CONST) + " "
			+ "\"" + sRipDescription + "\"" + " " +
			StringUtils::itos(COMMANDPARAMETER_Command_CONST) + " "
			+
			"\""
			+ StringUtils::itos(iPK_Device_Orbiter) + " " + StringUtils::itos(m_dwPK_Device) + " 1 337 "
			"17 <%=EE#34%><%=EEU%><%=EE#34%> "
			"13 <%=EE#34%><%=EE17%><%=EE#34%> "
			"233 <%=EE#34%><%=EE45%><%=EE#34%> "
			"234 <%=EE#34%><%=EE9%><%=EE#34%> "
			"131 0 "
			"2 " + "<%=EE#34%>" + StringUtils::itos(pDeviceFrom->m_dwPK_Device) + "<%=EE#34%>"
			"\"" + " " +
			StringUtils::itos(COMMANDPARAMETER_Advanced_options_CONST)
			+ " " + "1" + " " +
			StringUtils::itos(COMMANDPARAMETER_Interruption_CONST)
			+ " " + "0" + " " +
			StringUtils::itos(COMMANDPARAMETER_Turn_On_CONST) + " "
			+ "0" + " " +
			StringUtils::itos(COMMANDPARAMETER_Queue_CONST) + " "
			+ "0" + " ";

		string sButton2 = "No";
		string sCommand2 = "";


		DCE::SCREEN_PopupMessage cmd(m_dwPK_Device, iPK_Device_Orbiter,
				sDescription + "|" + sButton1 + "|" + sButton2,
				sCommand1 + "|" + sCommand2, "hdbd-ripping", "0", "0", "1");

		SendCommand(cmd);
	    }


	    return true;
    }
*/

    // If there are more than one entertainment areas for this drive there's nothing we can do since we can't know the
    // destination based on the media inserted event. No matter what, we'll just pick the first one
    EntertainArea *pEntertainArea = pMediaDevice->m_mapEntertainArea.begin( )->second;

    // See if there's a media handler for this type of media in this area
    List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_MediaType_Find( PK_MediaType,0 );
    if( !pList_MediaHandlerInfo || pList_MediaHandlerInfo->size( )==0 )
    {
        LoggerWrapper::GetInstance()->Write( LV_WARNING, "Media inserted from device %d but nothing to handle it", pDeviceFrom->m_dwPK_Device );
        return false; // Let someone else handle it
    }

    // We've got all the plug-ins that can handle this type of media
    for( List_MediaHandlerInfo::iterator itMPI=pList_MediaHandlerInfo->begin( );itMPI!=pList_MediaHandlerInfo->end( );++itMPI )
    {
        MediaHandlerInfo *pMediaHandlerInfo = *itMPI;
        if( pMediaHandlerInfo->m_bUsesRemovableMedia ) // todo - hack --- this should be a list of removable media devices
        {
            deque<MediaFile *> dequeMediaFile;

			// we split the MRL by \n first and then pass this to the StartMedia.
			deque<string> dequeFileNames;
			deque<string>::const_iterator itFileNames;

			StringUtils::Tokenize(MRL, "\n", dequeFileNames, false);
			for(size_t s=0;s<dequeFileNames.size();++s)
			{
				MediaFile *pMediaFile = new MediaFile(dequeFileNames[s]);
				pMediaFile->m_dwPK_Device_Disk_Drive=pMessage->m_dwPK_Device_From;
				if( dequeFileNames.size()>1 )
					pMediaFile->m_sDescription = "<%=T" + StringUtils::itos(TEXT_Unknown_Disc_CONST) + "%> " + StringUtils::itos(s+1);
				else
					pMediaFile->m_sDescription = "<%=T" + StringUtils::itos(TEXT_Unknown_Disc_CONST) + "%>";

				dequeMediaFile.push_back(pMediaFile);
			}

			vector<EntertainArea *> vectEntertainArea;
			vectEntertainArea.push_back(pEntertainArea);
			int PK_Orbiter=0;
			if( pMediaDevice->m_pOH_Orbiter_Reset && time(NULL)-pMediaDevice->m_tReset<60 )
				PK_Orbiter = pMediaDevice->m_pOH_Orbiter_Reset->m_pDeviceData_Router->m_dwPK_Device;

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from Media Inserted with orbiter %d",PK_Orbiter);

			MediaStream *pMediaStream = StartMedia(pMediaHandlerInfo,0,PK_Orbiter,vectEntertainArea,pDeviceFrom->m_dwPK_Device,&dequeMediaFile,false,false,0,"", false);
			if( pMediaStream )
				pMediaStream->m_discid = discid;
/*
			if( pMediaDevice->m_pDevice_Media_ID )
			{
				// Notify the identifiers that it's time to ID what this is
				DCE::CMD_Identify_Media CMD_Identify_Media(m_dwPK_Device,pMediaDevice->m_pDevice_Media_ID->m_dwPK_Device,
					pDeviceFrom->m_dwPK_Device,StringUtils::itos(discid),pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST]);
				SendCommand(CMD_Identify_Media);
			}
*/
			return false;
        }
    }

    LoggerWrapper::GetInstance()->Write( LV_WARNING, "Nothing knows how to handle removable media here. We have %d Plugins registered", ( int ) pList_MediaHandlerInfo->size( ) );
    return false; // Couldn't handle it//                     LoggerWrapper::GetInstance()->Write(LV_STATUS, "Filename: %s", sFilename.c_str());

}


bool Media_Plugin::PlaybackCompleted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
    string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::PlaybackCompleted stream id %d",iStreamID);
    MediaStream * pMediaStream = NULL;
	if( iStreamID==0 )  // This is just informational that nothing is playing on this stream
	{
		while(true)
		{
			bool bLoopAgain=false;  // If we delete a stream mid-way, just loop again through the whole list
			for( MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it )
			{
				MediaStream *pMS = (*it).second;
				if( pMS->m_pMediaDevice_Source && pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device==pMessage->m_dwPK_Device_From )
				{
					if( !pMS->m_bStopped )
						pMS->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia(pMS);
					StreamEnded(pMS);
					bLoopAgain=true;
					break;
				}
			}
			if( bLoopAgain )
				continue;
			break;
		}
		return false;
	}
	else
	    pMediaStream = m_mapMediaStream_Find( iStreamID, pMessage->m_dwPK_Device_From );
	bool bWithErrors = pMessage->m_mapParameters[EVENTPARAMETER_With_Errors_CONST]=="1";

    if ( pMediaStream == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Stream ID %d is not mapped to a media stream object", iStreamID);
        return false;
    }

	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( pMediaFile && pMediaFile->m_dwPK_CommandGroup_Stop )
		QueueMessageToRouter(new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,
			MESSAGETYPE_EXEC_COMMAND_GROUP,pMediaFile->m_dwPK_CommandGroup_Stop,0));

	if(
		NULL != pMediaStream->m_pMediaDevice_Source &&
		pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device != pMessage->m_dwPK_Device_From
	)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Ignoring 'Playback completed' event because it's from a slave. Stream %d, MRL : %s",
			iStreamID, sMRL.c_str());
		return false;
	}
/*
    if ( pMediaStream->m_pOH_Orbiter_StartedMedia == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "The stream object mapped to the stream ID %d does not have an associated orbiter object.", iStreamID);
        return false;
    }
*/

//     EntertainArea *pEntertainArea = pMediaStream->m_pOH_Orbiter_StartedMedia->m_pEntertainArea;
//     if ( pEntertainArea == NULL )
//     {
//         LoggerWrapper::GetInstance()->Write(LV_WARNING, "The orbiter %d which created this stream %d is not in a valid entertainment area",
//                     pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,
//                     iStreamID );
//         return false;
//     }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::PlaybackCompleted() Checking conditions: canPlayMore: %d, shouldResume %d parked: %d  # of eas %d", pMediaStream->CanPlayMore(), pMediaStream->m_bResume, (int) pMediaStream->m_tTime_Parked, (int) pMediaStream->m_mapEntertainArea.size() );
    if ( !bWithErrors && pMediaStream->CanPlayMore() && !pMediaStream->m_bResume && !pMediaStream->m_tTime_Parked )
    {
		ReleaseDriveLock(pMediaStream);

        pMediaStream->ChangePositionInPlaylist(1);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from playback completed");
		string sError;
        pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pMediaStream,sError);
		CheckStreamForTimeout(pMediaStream);
		MediaInfoChanged(pMediaStream,true);
    }
    else
    {
		if( !pMediaStream->m_bStopped )
			pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia(pMediaStream);

		StreamEnded(pMediaStream,true,true,NULL,NULL,true);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Playback completed. The stream can't play anything more.");
    }

    return false;
}

bool Media_Plugin::PlaybackStarted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
	string sSectionDescription = pMessage->m_mapParameters[EVENTPARAMETER_SectionDescription_CONST];
	string sAudio = pMessage->m_mapParameters[EVENTPARAMETER_Audio_CONST];
	string sVideo = pMessage->m_mapParameters[EVENTPARAMETER_Video_CONST];
    MediaStream * pMediaStream = m_mapMediaStream_Find( iStreamID, pMessage->m_dwPK_Device_From );

    if ( pMediaStream == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::PlaybackStarted Stream ID %d is not mapped to a media stream object", iStreamID);
        return false;
    }

	pMediaStream->m_bPlaybackStarted = true;

	string::size_type pos=0;
	while(pos<sSectionDescription.size())
	{
		string sSection = StringUtils::Tokenize(sSectionDescription,"\n",pos);
		string::size_type pos2=0;
		string sDescription = StringUtils::Tokenize(sSection,"\t",pos2);
		int iChapter = atoi(StringUtils::Tokenize(sSection,"\t",pos2).c_str());
		int iTitle = atoi(StringUtils::Tokenize(sSection,"\t",pos2).c_str());

		pMediaStream->m_mapSections[ make_pair<int,int> (iChapter-1,iTitle-1) ] = sDescription;
	}

	HandleAVAdjustments(pMediaStream,sAudio,sVideo);

	return false;
}

void Media_Plugin::HandleAVAdjustments(MediaStream *pMediaStream,string sAudio,string sVideo)
{
	Row_MediaType *pRow_MediaType = m_pDatabase_pluto_main->MediaType_get()->GetRow(pMediaStream->m_iPK_MediaType);

	// We need to get the current rendering devices so that we can see what have rules
	map<int,MediaDevice *> mapMediaDevice_Current;
	for(map<int,class EntertainArea *>::iterator it=pMediaStream->m_mapEntertainArea.begin();it!=pMediaStream->m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&mapMediaDevice_Current);
		AddOtherDevicesInPipesToRenderDevices(pRow_MediaType ? pRow_MediaType->FK_Pipe_get() : 0,&mapMediaDevice_Current,pEntertainArea);
	}

	if( pMediaStream->m_sAudioSettings.empty()==false )
		sAudio = pMediaStream->m_sAudioSettings;
	if( pMediaStream->m_sVideoSettings.empty()==false )
		sVideo = pMediaStream->m_sVideoSettings;

	for(map<int,MediaDevice *>::iterator it=mapMediaDevice_Current.begin();it!=mapMediaDevice_Current.end();++it)
	{
		// Go through each device and find rules for these audio or video settings
		MediaDevice *pMediaDevice = it->second;
		int PK_Command=0; // Find a command
		if( pMediaDevice->m_bPreserveAspectRatio==false )  // The user wants full screen rules, which will be preceeded with a !
			PK_Command = pMediaDevice->m_mapAdjustmentRules_Find("!" + sVideo);
		if( PK_Command==0 )  // Either the user doesn't want full screen, or there was no full screen rule
			PK_Command = pMediaDevice->m_mapAdjustmentRules_Find(sVideo);

		if( PK_Command && pMediaDevice->m_dwPK_Command_LastAdjustment_Video!=PK_Command )
		{
			pMediaDevice->m_dwPK_Command_LastAdjustment_Video=PK_Command;
			DCE::Message *pMessage = new DCE::Message(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,PK_Command,0);
			QueueMessageToRouter(pMessage);

			if( pMediaStream->m_pMediaDevice_Source )
			{
				Command *pCommand = m_pRouter->m_mapCommand_Find(PK_Command);
				string sCommand = pCommand ? pCommand->m_sDescription : "CMD #" + StringUtils::itos(PK_Command);
				DCE::CMD_Display_Alert CMD_Display_Alert(m_dwPK_Device,pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_ControlledVia,
					"Blackbird data: " + sVideo + " = " + sCommand,"bbvid","60",interuptAlways);
				SendCommand(CMD_Display_Alert);
			}
		}

		PK_Command = pMediaDevice->m_mapAdjustmentRules_Find(sAudio);
		if( PK_Command && pMediaDevice->m_dwPK_Command_LastAdjustment_Audio!=PK_Command )
		{
			pMediaDevice->m_dwPK_Command_LastAdjustment_Audio=PK_Command;
			DCE::Message *pMessage = new DCE::Message(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,PK_Command,0);
			QueueMessageToRouter(pMessage);

			if( pMediaStream->m_pMediaDevice_Source )
			{
				Command *pCommand = m_pRouter->m_mapCommand_Find(PK_Command);
				string sCommand = pCommand ? pCommand->m_sDescription : "CMD #" + StringUtils::itos(PK_Command);
				DCE::CMD_Display_Alert CMD_Display_Alert(m_dwPK_Device,pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_ControlledVia,
					"Blackbird data: " + sAudio + " = " + sCommand,"bbaud","60",interuptAlways);
				SendCommand(CMD_Display_Alert);
			}
		}

		string::size_type pos=0;
		while(pos<pMediaStream->m_sCommands.size())
		{
			PK_Command = atoi( StringUtils::Tokenize(pMediaStream->m_sCommands,",",pos).c_str() );
			if( PK_Command && pMediaDevice->m_dwPK_Command_LastAdjustment_Command!=PK_Command )
			{
				pMediaDevice->m_dwPK_Command_LastAdjustment_Command=PK_Command;
				DCE::Message *pMessage = new DCE::Message(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,PK_Command,0);
				QueueMessageToRouter(pMessage);
			}
		}
	}
}

void Media_Plugin::StartMedia( int iPK_MediaType, int iPK_MediaProvider, unsigned int iPK_Device_Orbiter, vector<EntertainArea *>  &vectEntertainArea, int iPK_Device, int iPK_DeviceTemplate, deque<MediaFile *> *p_dequeMediaFile, bool bQueue, bool bResume, int iRepeat, string sStartingPosition, bool bBypass_Event, vector<MediaStream *> *p_vectMediaStream)
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia - Main function start");
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::StartMedia iPK_MediaType %d iPK_MediaProvider %d iPK_Device_Orbiter %d vectEntertainArea %d iPK_Device %d iPK_DeviceTemplate %d p_dequeMediaFile %p bResume %d iRepeat %d sStartingPosition %s p_vectMediaStream %p",
		iPK_MediaType, iPK_MediaProvider, iPK_Device_Orbiter, (int) vectEntertainArea.size(), iPK_Device, iPK_DeviceTemplate, p_dequeMediaFile, (int) bResume, iRepeat, sStartingPosition.c_str(), p_vectMediaStream);

	if( !iPK_MediaType && p_dequeMediaFile->size() )
	{
		iPK_MediaType = GetMediaTypeForFile(p_dequeMediaFile,vectEntertainArea);

		if( !iPK_MediaType )
		{
			MediaFile *pMediaFile = (*p_dequeMediaFile)[0];
			// This could be a DVD in a directory we're supposed to play as a file
			if( pMediaFile->m_dwPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST && FileUtils::DirExists( pMediaFile->FullyQualifiedFile() ) )
			{
				pMediaFile->m_sPath = "dvd:/" + pMediaFile->m_sPath;
				iPK_MediaType=MEDIATYPE_pluto_StoredVideo_CONST;
			}
			else if( StringUtils::StartsWith(pMediaFile->m_sFilename,"!P",true) )
			{
				string sPK_EntertainArea;
				for(vector<EntertainArea *>::iterator it=vectEntertainArea.begin();it!=vectEntertainArea.end();++it)
					sPK_EntertainArea += StringUtils::itos( (*it)->m_iPK_EntertainArea ) + ",";
				Message m(iPK_Device_Orbiter,m_dwPK_Device,0,0,0,0);  // fake message since the command looks for the from
				string sResponse;
				CMD_Load_Playlist(sPK_EntertainArea,atoi(pMediaFile->m_sFilename.substr(2).c_str()),sResponse,&m);
				return;
			}
		}
	}

	// Find the media handlers we're going to need.  If everything can be handled by one
	// handler, vectEA_to_MediaHandler will only have 1 element.  Otherwise it will have an
	// element for each handler we need to use (which will translate to a stream for each)
	// and the list of entertainment areas
	vector< pair< MediaHandlerInfo *,vector<EntertainArea *> > > vectEA_to_MediaHandler;
	if( !iPK_MediaType )
	{
		if( iPK_Device_Orbiter )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%>");
			SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device, iPK_Device_Orbiter, "");
			SendCommand(SCREEN_DialogCannotPlayMedia);
		}
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartMedia - MediaType==0");
		return;
	}

	// This is really complicated.  We need to keep track of what output zones are being used for which entertainment areas
	// So when we go to create the pipes we'll know what our target destinations are
	map<int, pair<MediaDevice *,MediaDevice *> > mapEntertainmentArea_OutputZone;
	GetMediaHandlersForEA(iPK_MediaType, iPK_MediaProvider, iPK_Device, iPK_DeviceTemplate, vectEntertainArea, vectEA_to_MediaHandler, mapEntertainmentArea_OutputZone);
	if( vectEA_to_MediaHandler.size()==0 )
	{
		if( iPK_Device )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't find any media handlers for type %d device %d trying without",iPK_MediaType, iPK_Device);
			int iPK_Device_2=0;
			GetMediaHandlersForEA(iPK_MediaType, iPK_MediaProvider, iPK_Device_2, iPK_DeviceTemplate, vectEntertainArea, vectEA_to_MediaHandler, mapEntertainmentArea_OutputZone);
			iPK_Device = iPK_Device_2;
		}
		if( vectEA_to_MediaHandler.size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't find any media handlers for type %d", iPK_MediaType);
			SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device, iPK_Device_Orbiter, "");
			SendCommand(SCREEN_DialogCannotPlayMedia);
		}
	}

	// If there are 2 or more stream and we have a deque of mediafiles, make copies of them
	// so each stream will have it's own and won't share, causing duplicate deletes when the stream
	// is closed
	deque<MediaFile *> *p_dequeMediaFile_Copy=NULL;
	if( vectEA_to_MediaHandler.size()>1 && p_dequeMediaFile && p_dequeMediaFile->size() )
		p_dequeMediaFile_Copy = p_dequeMediaFile;

	for(size_t s=0;s<vectEA_to_MediaHandler.size();++s)
	{
		MediaHandlerInfo *pMediaHandlerInfo = vectEA_to_MediaHandler[s].first;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from MH Play Media2");

		if( p_dequeMediaFile_Copy )
		{
			p_dequeMediaFile = new deque<MediaFile *>;
			for(size_t sFile=0;sFile<p_dequeMediaFile_Copy->size();++sFile)
				(*p_dequeMediaFile).push_back( new MediaFile((*p_dequeMediaFile_Copy)[sFile]) );

		}

		MediaStream *pMediaStream = StartMedia(pMediaHandlerInfo,iPK_MediaProvider,iPK_Device_Orbiter,vectEA_to_MediaHandler[s].second,
						       iPK_Device,p_dequeMediaFile,bQueue,bResume,iRepeat,sStartingPosition,bBypass_Event,0,&mapEntertainmentArea_OutputZone);  // We'll let the plug-in figure out the source, and we'll use the default remote

		//who will take care of pMediaStream ?

		if( p_dequeMediaFile_Copy )
			delete p_dequeMediaFile;
	}

	if( p_dequeMediaFile_Copy )
		for(size_t s=0;s<p_dequeMediaFile_Copy->size();++s)
			delete (*p_dequeMediaFile_Copy)[s];
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia main constructor end.");
}

int Media_Plugin::GetMediaTypeForFile(deque<MediaFile *> *p_dequeMediaFile,vector<EntertainArea *>  &vectEntertainArea)
{
	MediaFile *pMediaFile = (*p_dequeMediaFile)[0];
	if( pMediaFile->m_dwPK_Disk )  // Is it a disk?
	{
		Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(pMediaFile->m_dwPK_Disk);
		if( pRow_Disc )
			return pRow_Disc->EK_MediaType_get();
	}

	if( pMediaFile->m_dwPK_MediaType>0 )
		return pMediaFile->m_dwPK_MediaType;

	string Extension = StringUtils::ToUpper(FileUtils::FindExtension(pMediaFile->m_sFilename));

	map<int,MediaHandlerInfo *> mapMediaHandlerInfo;

	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea=vectEntertainArea[s];
		List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find(Extension);
		if( pList_MediaHandlerInfo && pList_MediaHandlerInfo->size() )
		{
			MediaHandlerInfo *pMediaHandlerInfo = pList_MediaHandlerInfo->front();
			return pMediaHandlerInfo->m_PK_MediaType;
		}
	}

	// Dig through all the media handler's to find it
	for(map<int,EntertainArea *>::iterator it=m_mapEntertainAreas.begin();it!=m_mapEntertainAreas.end();++it)
	{
		EntertainArea *pEntertainArea=it->second;
		List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find(Extension);
		if( pList_MediaHandlerInfo && pList_MediaHandlerInfo->size() )
		{
			MediaHandlerInfo *pMediaHandlerInfo = pList_MediaHandlerInfo->front();
			return pMediaHandlerInfo->m_PK_MediaType;
		}
	}

	// It's possible that it's an unidentified slot in a jukebox.  If so, just assume it's a dvd for now.  WaitingForJukebox will fill in the real info
	if( pMediaFile->m_dwPK_Device_Disk_Drive )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(pMediaFile->m_dwPK_Device_Disk_Drive);
		if( pDevice && pDevice->WithinCategory(DEVICECATEGORY_CDDVD_Jukeboxes_CONST) )
		{
			return MEDIATYPE_pluto_DVD_CONST;
		}
	}
	return 0;
}

MediaStream *Media_Plugin::StartMedia( MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, unsigned int PK_Device_Orbiter, vector<EntertainArea *> &vectEntertainArea, int PK_Device_Source, deque<MediaFile *> *dequeMediaFile, bool bQueue, bool bResume,int iRepeat, string sStartingPosition, bool bBypass_Event, int iPK_Playlist,map<int, pair<MediaDevice *,MediaDevice *> > *p_mapEntertainmentArea_OutputZone)
{
    PLUTO_SAFETY_LOCK(mm,m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia OutputZone begin.");
	// Be sure all the files are in the database
	for(size_t s=0;s<dequeMediaFile->size();++s)
	{
		if( !(*dequeMediaFile)[s]->m_dwPK_File )
			AddFileToDatabase( (*dequeMediaFile)[s],pMediaHandlerInfo->m_PK_MediaType);
	}

	MediaDevice *pMediaDevice=NULL;
    if( PK_Device_Source )
        pMediaDevice = m_mapMediaDevice_Find(PK_Device_Source);
	else if( p_mapEntertainmentArea_OutputZone && p_mapEntertainmentArea_OutputZone->size() )
		pMediaDevice = p_mapEntertainmentArea_OutputZone->begin()->second.first;

    // If this pointer is still valid, then we'll just add this file to the queue
    MediaStream *pMediaStream = NULL;

	bool bContainsTitlesOrSections=false;
	if( pMediaHandlerInfo->m_PK_MediaType==MEDIATYPE_pluto_DVD_CONST ||
		(dequeMediaFile->size() &&
			((*dequeMediaFile)[0]->m_sExtension=="DVD" || (*dequeMediaFile)[0]->m_sExtension=="ISO" || StringUtils::StartsWith((*dequeMediaFile)[0]->m_sPath,"DVD:",true))	) )
				bContainsTitlesOrSections=true;
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::StartMedia type %d size %d extension %s path %s cont %d",
pMediaHandlerInfo->m_PK_MediaType,dequeMediaFile->size(),
dequeMediaFile->size() ? (*dequeMediaFile)[0]->m_sExtension.c_str() : "NO",
dequeMediaFile->size() ? (*dequeMediaFile)[0]->m_sPath.c_str() : "NO",
(int) bContainsTitlesOrSections);
#endif

    // If all EA's are playing the same stream, it might be possible to queue it
    MediaStream *pMediaStream_AllEAsPlaying = NULL;
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		if( !vectEntertainArea[s]->m_pMediaStream ||
			(pMediaStream_AllEAsPlaying && vectEntertainArea[s]->m_pMediaStream!=pMediaStream_AllEAsPlaying) )
		{
			pMediaStream_AllEAsPlaying=NULL;  // they're not all playing the same stream
			break;
		}
		pMediaStream_AllEAsPlaying=vectEntertainArea[s]->m_pMediaStream;
	}

    OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Device_Orbiter);

	// See if we can queue it 

    if( !bResume && pMediaStream_AllEAsPlaying && !pMediaStream_AllEAsPlaying->m_bResume &&
	pMediaStream_AllEAsPlaying->m_pMediaHandlerInfo->m_pMediaHandlerBase == pMediaHandlerInfo->m_pMediaHandlerBase &&
	pMediaStream_AllEAsPlaying->m_iPK_MediaType == pMediaHandlerInfo->m_PK_MediaType &&
	!pMediaStream_AllEAsPlaying->m_bContainsTitlesOrSections &&
	!bContainsTitlesOrSections &&
	pMediaStream_AllEAsPlaying->m_dequeMediaFile.size() &&
	pMediaStream_AllEAsPlaying->m_dwPK_Disc==0 && pMediaStream_AllEAsPlaying->m_discid==0 /* don't resume disks */ )
      {
        pMediaStream = pMediaStream_AllEAsPlaying;
        pMediaStream->m_dequeMediaFile += *dequeMediaFile;
	if (pMediaStream->m_dequeMediaFile.size() == dequeMediaFile->size() || !bQueue)
	  // If deque sizes are the same(we just started playing) or we don't queue(play newly added file), set new starting position
	  pMediaStream->m_iDequeMediaFile_Pos = pMediaStream->m_dequeMediaFile.size()-dequeMediaFile->size();
	pMediaStream->m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
      }
    else if (!bResume && pMediaStream_AllEAsPlaying && !pMediaStream_AllEAsPlaying->m_bResume &&
	     pMediaStream_AllEAsPlaying->m_pMediaHandlerInfo->m_pMediaHandlerBase == pMediaHandlerInfo->m_pMediaHandlerBase && 
	     IsGameMediaType(pMediaHandlerInfo->m_PK_MediaType) &&
	     pMediaStream_AllEAsPlaying->m_dequeMediaFile.size())
      {
	// We want the same behavior, as above, IF the game being added is a game media type
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_PlugIn::StartMedia() - Special StartMedia case for Adding Games to queue.");
	pMediaStream = pMediaStream_AllEAsPlaying;
	pMediaStream->m_dequeMediaFile += *dequeMediaFile;
	if (pMediaStream->m_dequeMediaFile.size() == dequeMediaFile->size() || !bQueue)
	  // If deque sizes are the same(we just started playing) or we don't queue(play newly added file), set new starting position
	  pMediaStream->m_iDequeMediaFile_Pos = pMediaStream->m_dequeMediaFile.size()-dequeMediaFile->size();
	pMediaStream->m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
      }
    else
    {
		if ( (pMediaStream = pMediaHandlerInfo->m_pMediaHandlerBase->CreateMediaStream(pMediaHandlerInfo,iPK_MediaProvider,vectEntertainArea,pMediaDevice,(pOH_Orbiter ? pOH_Orbiter->PK_Users_get() : 0),dequeMediaFile,++m_iStreamID)) == NULL )
		{
			if( PK_Device_Orbiter )
			{
				//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(PK_Device_Orbiter,"<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%>");
				SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device, PK_Device_Orbiter, "");
				SendCommand(SCREEN_DialogCannotPlayMedia);
			}
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The plugin %s (%d) returned a NULL media stream object",
													pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_sName.c_str(),
													pMediaHandlerInfo->m_pMediaHandlerBase->m_pMedia_Plugin->m_dwPK_Device);
			return pMediaStream;
		}

		// ContainsVideo needs this too
	    pMediaStream->m_iPK_MediaType = pMediaHandlerInfo->m_PK_MediaType;
		pMediaStream->m_sStartPosition = sStartingPosition;
		pMediaStream->m_sAppName = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_Name_CONST);

        pMediaStream->m_dequeMediaFile += *dequeMediaFile;
    }

	if (pMediaStream->m_dequeMediaFile.size() != dequeMediaFile->size() && bQueue) {
		// If we want to queue and there already was items in the deque, return here
		// Load stream attributes for new files
		m_pMediaAttributes->LoadStreamAttributes(pMediaStream);
		// Update playlist datagrid
		pMediaStream->UpdateDescriptions(true);
		MediaInfoChanged(pMediaStream,true);
		return pMediaStream;
	}
	if( !pOH_Orbiter )
	{
		pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_ControlledVia );
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::StartMedia assuming parent of %d started media", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_ControlledVia);
#endif
	}

	if( pOH_Orbiter )
	{
	    pMediaStream->m_pOH_Orbiter_StartedMedia = pOH_Orbiter;
		pMediaStream->m_dwPK_Device_Remote = pOH_Orbiter->m_dwPK_Device_CurrentRemote;
		if( pOH_Orbiter->m_pOH_User )
			pMediaStream->m_iPK_Users = pOH_Orbiter->m_pOH_User->m_iPK_Users;
	}

	pMediaStream->m_pMediaHandlerInfo = pMediaHandlerInfo;
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
	    pMediaStream->m_mapEntertainArea[pEntertainArea->m_iPK_EntertainArea] = pEntertainArea;
	}
	pMediaStream->m_iRepeat=iRepeat;
	pMediaStream->m_iPK_Playlist=iPK_Playlist;
	if( iPK_Playlist )
	{
		Row_Playlist *pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(iPK_Playlist);
		if( pRow_Playlist )
			pMediaStream->m_sPlaylistName = pRow_Playlist->Name_get();
	}

    // HACK: get the user if the message originated from an orbiter!

	pMediaStream->m_bResume=bResume;
	pMediaStream->m_bContainsTitlesOrSections=bContainsTitlesOrSections;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::StartMedia stream %d orbiter %d user %d remote %d",
		pMediaStream->m_iStreamID_get(),pMediaStream->m_pOH_Orbiter_StartedMedia ? pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device : 0,
		pMediaStream->m_iPK_Users,pMediaStream->m_dwPK_Device_Remote);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia OutputZone end.");

	if( StartMedia(pMediaStream,p_mapEntertainmentArea_OutputZone, bBypass_Event) )
		return pMediaStream;
	// delete pMediaStream; // can't do this here because I'm not sure we always still own the pointer at this point
	return NULL;
}

bool Media_Plugin::StartMedia(MediaStream *pMediaStream,map<int, pair<MediaDevice *,MediaDevice *> > *p_mapEntertainmentArea_OutputZone, bool bBypass_Event)
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia OutputZone - MediaStream - Begin");
	if( !pMediaStream->m_pMediaDevice_Source )
	{
		if( pMediaStream->m_pOH_Orbiter_StartedMedia )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,"<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%>");
			SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device, pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device, "");
			SendCommand(SCREEN_DialogCannotPlayMedia);
		}
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot start media without a source");
		return false;
	}

	for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
	{
		EntertainArea *pEntertainArea = ( *it ).second;

		if( pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream!=pMediaStream )
		{
			OldStreamInfo *pOldStreamInfo = new OldStreamInfo(pEntertainArea);
			pMediaStream->m_mapOldStreamInfo[pEntertainArea->m_iPK_EntertainArea]=pOldStreamInfo;
			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&pOldStreamInfo->m_mapMediaDevice_Prior);
			pOldStreamInfo->m_PK_MediaType_Prior = pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType;

			// this will reset the current media stream if the pMediaHandlerBase is different from the original media Handler.
			// We should also look at the media types. If the current Media type is different then we will also do a new media stream.
			if ( pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase == pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase &&
					pEntertainArea->m_pMediaStream->m_iPK_MediaType == pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType &&
					pEntertainArea->m_pMediaStream->m_pMediaDevice_Source == pMediaStream->m_pMediaDevice_Source )
				pOldStreamInfo->m_bNoChanges = true;

			// If Resume is set, then this media is just a temporary stream, like an announcement, and if something
			// is currently playing, we will store that stream here and resume playing it when the temporary
			// stream is done.  But don't resume another stream that also has a resume, since it too was just
			// an announcement.
			if( pMediaStream->m_bResume && !pEntertainArea->m_pMediaStream->m_bResume )
				pEntertainArea->m_vectMediaStream_Interrupted.push_back(pEntertainArea->m_pMediaStream);

			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pEntertainArea->m_pMediaStream );
			if( pEntertainArea->m_pMediaStream->m_bPlaybackStarted==false )
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StartMedia(): Calling Stream ended for a stream %d that never started", pEntertainArea->m_pMediaStream->m_iStreamID_get());
			else
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::StartMedia(): Calling Stream ended after the Stop Media");

			// If the new stream has m_bResume set we don't want to delete the prior stream.  Unless the prior stream never actually started yet
			StreamEnded(pEntertainArea->m_pMediaStream,false,pMediaStream->m_bResume && pEntertainArea->m_pMediaStream->m_bPlaybackStarted ? false : true,pMediaStream,NULL,false,true,false);  // Don't delete it if we're going to resume
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::StartMedia(): Call completed.");

		}

		pEntertainArea->m_pMediaStream=pMediaStream;
		pEntertainArea->m_pMediaDevice_OutputZone=NULL;
		if (!bBypass_Event)
		{
			if( pMediaStream->ContainsVideo() )
				EVENT_Watching_Media(pEntertainArea->m_pRoom->m_dwPK_Room);
			else
				EVENT_Listening_to_Media(pEntertainArea->m_pRoom->m_dwPK_Room);
		}
	}

	FindActiveDestination(pMediaStream,p_mapEntertainmentArea_OutputZone);

	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( pMediaFile && (pMediaFile->m_dwPK_Disk || pMediaFile->m_dwPK_Device_Disk_Drive) )
	{
		if( !AssignDriveForDisc(pMediaStream,pMediaFile) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::StartMedia AssignDriveForDisc failed");
			return false;
		}
		if( pMediaFile->m_bWaitingForJukebox )
		{
			int StreamID = pMediaStream->m_iStreamID_get( );
			m_pAlarmManager->AddRelativeAlarm(1,this,WAITING_FOR_JUKEBOX,(void *) StreamID);
			return true;
		}
	}

	if( pMediaFile && pMediaFile->m_dwPK_Device_Disk_Drive )
	{
		string sText="STREAM " + StringUtils::itos(pMediaStream->m_iStreamID_get());
		bool bIsSuccessful=false;
		DCE::CMD_Lock CMD_Lock(m_dwPK_Device,pMediaFile->m_dwPK_Device_Disk_Drive,m_dwPK_Device,
			"",true,&sText,&bIsSuccessful);
		if( !SendCommand(CMD_Lock) || bIsSuccessful==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::StartMedia Cannot lock drive %d", pMediaFile->m_dwPK_Device_Disk_Drive);
			if( pMediaStream->m_pOH_Orbiter_StartedMedia )
			{
				SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device,
					pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device, "Drive is not available");
				SendCommand(SCREEN_DialogCannotPlayMedia);
			}
			StreamEnded(pMediaStream);
			return false;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia - OutputZone - MediaStream - end.");
	return StartMedia(pMediaStream);
}

bool Media_Plugin::StartMedia(MediaStream *pMediaStream)
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia pMediaStream Start");
#ifdef SIM_JUKEBOX
	static bool bToggle=false;
	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( pMediaFile )
	{
		string sExtension = FileUtils::FindExtension(pMediaFile->m_sFilename);
		if( StringUtils::ToUpper(sExtension)=="DVD" && pMediaStream->m_pOH_Orbiter_StartedMedia )
		{
			bToggle=!bToggle;
			DCE::CMD_Spawn_Application_DT CMD_Spawn_Application_DT(m_dwPK_Device,DEVICETEMPLATE_App_Server_CONST,BL_SameHouse,"/home/JukeBoxSimulate.sh","simjukebox",
				bToggle ? "100" : "2", "", "", false, false, false, true);
			SendCommand(CMD_Spawn_Application_DT);
			DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device,pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,
				"Please wait up to 60 seconds while I load that disc.  Be patient.  Playback will start when the disc is loaded","","load_jukebox","0","10","1");
			SendCommand(SCREEN_PopupMessage);
			Sleep(5000);   // Not good.  We're holding the mutex, but it's a temporary simulation
		}
	}
#endif

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ready to call plugin's startmedia");
	int iPK_Orbiter_PromptingToResume = 0;	string::size_type queue_pos;
	if( pMediaStream->m_sStartPosition.size()==0 &&
		(pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST || pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST || pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_HDDVD_CONST || pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_BD_CONST || pMediaStream->m_iPK_MediaType==MEDIATYPE_lmce_Game_CONST) &&  // Don't bother asking for music
			(pMediaStream->m_iDequeMediaFile_Pos<0 || pMediaStream->m_iDequeMediaFile_Pos>=pMediaStream->m_dequeMediaFile.size() ||
			pMediaStream->GetCurrentMediaFile()->m_sStartPosition.size()==0) )
		iPK_Orbiter_PromptingToResume = CheckForAutoResume(pMediaStream);
	else if( pMediaStream->m_sStartPosition.size() && (queue_pos = pMediaStream->m_sStartPosition.find(" QUEUE_POS:"))!=string::npos )
	{
		unsigned int pos = atoi( pMediaStream->m_sStartPosition.substr(queue_pos+11).c_str() );
		if( pos>=0 && pos<pMediaStream->m_dequeMediaFile.size() )
			pMediaStream->m_iDequeMediaFile_Pos=pos;
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia - pMediaStream - LoadStreamAttributes begin");
	m_pMediaAttributes->LoadStreamAttributes(pMediaStream);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia - pMediaStream - LoadStreamAttributes end");

	string sError;
	if( pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pMediaStream,sError) )
	{
		int StreamID = pMediaStream->m_iStreamID_get( );
		pMediaStream->m_sStartPosition = "";

		if(NULL != m_pAlarmManager)
			m_pAlarmManager->AddRelativeAlarm(1,this,UPDATE_VIEW_DATE,(void *) StreamID);  // Do this off-line so we don't block the mutex doing a database update

		CheckStreamForTimeout(pMediaStream);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Plug-in started media");

		pMediaStream->m_pMediaDevice_Source->m_bCaptureCardActive = false;  // We will set it to true in StartCaptureCard

		for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
		{
			EntertainArea *pEntertainArea = ( *it ).second;

			CheckForAlternatePipes(pMediaStream,pEntertainArea);

			// Only start it once, we set it to false above, and will set it to true in StartCaptureCard.  We may be streaming to multiple EA's
			if( pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard && pMediaStream->m_pMediaDevice_Source->m_bCaptureCardActive == false)
				StartCaptureCard(pMediaStream);

			map<int,class OldStreamInfo *>::iterator itFind = pMediaStream->m_mapOldStreamInfo.find(pEntertainArea->m_iPK_EntertainArea);
			OldStreamInfo *pOldStreamInfo = NULL;
			if( itFind != pMediaStream->m_mapOldStreamInfo.end() )
			{
				pOldStreamInfo = (*itFind).second;
			}
			if( !pOldStreamInfo || !pOldStreamInfo->m_bNoChanges )
			{
				// We need to get the current rendering devices so that we can send on/off commands
				map<int,MediaDevice *> mapMediaDevice_Current;
				// only do stuff with valid objects
				pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&mapMediaDevice_Current);

				HandleOnOffs(pOldStreamInfo ? pOldStreamInfo->m_PK_MediaType_Prior : 0,
					pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType,
					pOldStreamInfo ? &pOldStreamInfo->m_mapMediaDevice_Prior : NULL,
					&mapMediaDevice_Current,pMediaStream,pEntertainArea);

				// Reset the values used to keep track of live/non-live viewing, and adjusts for all devices involved in this stream
				Row_MediaType *pRow_MediaType = m_pDatabase_pluto_main->MediaType_get()->GetRow(pMediaStream->m_iPK_MediaType);
				AddOtherDevicesInPipesToRenderDevices(pRow_MediaType ? pRow_MediaType->FK_Pipe_get() : 0,&mapMediaDevice_Current, pEntertainArea);
				for(map<int,MediaDevice *>::iterator it=mapMediaDevice_Current.begin();it!=mapMediaDevice_Current.end();++it)
				{
					MediaDevice *pMediaDevice = (*it).second;
					pMediaStream->m_pMediaDevice_Source->m_dwPK_Command_LastAdjustment_Audio=pMediaStream->m_pMediaDevice_Source->m_dwPK_Command_LastAdjustment_Video=pMediaStream->m_pMediaDevice_Source->m_dwPK_Command_LastAdjustment_Command=0;
				}
			}
		}

		// If this is just an announcement don't bother sending the orbiters to the remote screen
		if( pMediaStream->m_bResume )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StartMedia() done - not sending to remote since stream is marked resumte");
			return true;
		}

		pMediaStream->UpdateDescriptions(true);
		MediaInfoChanged( pMediaStream, pMediaStream->m_dequeMediaFile.size()>1 );

		for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
		{
			OH_Orbiter *pOH_Orbiter = (*it).second;
			if( iPK_Orbiter_PromptingToResume==pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device )
				continue;  // This orbiter is displaying a resume? message

			if( !pMediaStream->OrbiterIsOSD(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device) &&
				(!pOH_Orbiter->m_pEntertainArea || pMediaStream->m_mapEntertainArea.find(pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea)==pMediaStream->m_mapEntertainArea.end()) )
				continue;  // Don't send an orbiter to the remote if it's not linked to an entertainment area where we're playing this stream unless it's the OSD

			// We don't want to change to the remote screen on the orbiter that started playing this if it's audio, so that they can build a playlist, unless this was a playlist
			if( pOH_Orbiter && pOH_Orbiter == pMediaStream->m_pOH_Orbiter_StartedMedia && pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && pMediaStream->m_iPK_Playlist<1 )
				continue;

			WaitForMessageQueue();  // Be sure all the Set Now Playing's are set
			EntertainArea *pEntertainArea_OSD=NULL;
			int PK_Orbiter = pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device;
			int PK_Screen = pMediaStream->GetRemoteControlScreen(PK_Orbiter);
			bool bIsOSD=pMediaStream->OrbiterIsOSD(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,&pEntertainArea_OSD);
			if( bIsOSD || pOH_Orbiter == pMediaStream->m_pOH_Orbiter_StartedMedia )
			{
				DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,PK_Orbiter,"",PK_Screen,interuptAlways,true,false);

				if( bIsOSD && pEntertainArea_OSD && pOH_Orbiter->m_pEntertainArea!=pEntertainArea_OSD )
				{
					DCE::CMD_Set_Entertainment_Area CMD_Set_Entertainment_Area(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
						StringUtils::itos(pEntertainArea_OSD->m_iPK_EntertainArea));
					pMediaStream->SetNowPlaying( pOH_Orbiter, false, false, CMD_Set_Entertainment_Area.m_pMessage );

					string sResponse;
					SendCommand(CMD_Set_Entertainment_Area,&sResponse);  // Get a confirmation so we're sure it goes through before the goto screen
				}
				SendCommand(CMD_Goto_Screen);
			}
			else
			{
				DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,PK_Orbiter,"",PK_Screen,interuptAlways,true,false);
				SendCommand(CMD_Goto_Screen);
			}
		}
	}
	else
	{
		if( pMediaStream->m_pOH_Orbiter_StartedMedia )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,
			//	"<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%> " + sError );
			SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device,
				pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device, sError);
			SendCommand(SCREEN_DialogCannotPlayMedia);
		}
		StreamEnded(pMediaStream);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media Plug-in's call to Start Media failed 2.");
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StartMedia() function call completed with honors!");
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT StartMedia - pMediaStream - end.");
	return true;
}

void Media_Plugin::FindActiveDestination(MediaStream *pMediaStream,map<int, pair<MediaDevice *,MediaDevice *> > *p_mapEntertainmentArea_OutputZone)
{
	// Find the active destinations.  If the media player in the living room is playing locally and streaming to the bedroom,
	// then in the living room the media player is both the source and the destination, and in the bedroom, that media player is the
	// destination if the device supports streaming.
	// If the source device is standard a/v equipment connected to a capture card, and the destination is an EA away from the capture card,
	// then the dest is a md in the target EA
	for( map<int,EntertainArea *>::iterator it=pMediaStream->m_mapEntertainArea.begin();it!=pMediaStream->m_mapEntertainArea.end();++it )
	{
		EntertainArea *pEntertainArea = it->second;
		MediaDevice *pMediaDevice_Audio=NULL,*pMediaDevice_Video=NULL;

		if( pMediaStream->m_pMediaDevice_Source->m_pDevice_Audio )
			pMediaDevice_Audio = m_mapMediaDevice_Find(pMediaStream->m_pMediaDevice_Source->m_pDevice_Audio->m_dwPK_Device);
		if( pMediaStream->m_pMediaDevice_Source->m_pDevice_Video )
			pMediaDevice_Video = m_mapMediaDevice_Find(pMediaStream->m_pMediaDevice_Source->m_pDevice_Video->m_dwPK_Device);

		if( p_mapEntertainmentArea_OutputZone && p_mapEntertainmentArea_OutputZone->find( pEntertainArea->m_iPK_EntertainArea ) != p_mapEntertainmentArea_OutputZone->end() )
		{
			pEntertainArea->m_pMediaDevice_ActiveDest = (*p_mapEntertainmentArea_OutputZone)[ pEntertainArea->m_iPK_EntertainArea ].first;
			pEntertainArea->m_pMediaDevice_OutputZone = (*p_mapEntertainmentArea_OutputZone)[ pEntertainArea->m_iPK_EntertainArea ].second;
			continue;
		}

		if( pEntertainArea->m_pMediaDevice_ActiveDest )
			continue;  // We already assigned this

		if( pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo )
		{
			// If we've got destination audio zones and we're not in there, then we need to find another one
			if( (pMediaDevice_Audio && pMediaDevice_Audio->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)==pMediaDevice_Audio->m_mapEntertainArea.end())
				|| (pMediaDevice_Video && pMediaDevice_Video->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)==pMediaDevice_Video->m_mapEntertainArea.end()) )
			{
				CheckForAlternatePipes(pMediaStream,pEntertainArea);
				if( pEntertainArea->m_pMediaDevice_ActiveDest )  // Got one
					continue;
			}
			if( (pMediaDevice_Audio && pMediaDevice_Audio!=pMediaStream->m_pMediaDevice_Source && pMediaDevice_Audio->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaDevice_Audio->m_mapEntertainArea.end())
				|| (pMediaDevice_Video && pMediaDevice_Video!=pMediaStream->m_pMediaDevice_Source && pMediaDevice_Video->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaDevice_Video->m_mapEntertainArea.end()) )
			{
				pEntertainArea->m_pMediaDevice_ActiveDest=pMediaStream->m_pMediaDevice_Source;  // It's the same as the source
				continue;  // We have destination audio/video zones, and they're in the ea we want
			}
		}

		// If this is a generic device using a capture card, what matters is that the capture card is in the same ea
		if( pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard )
		{
			MediaDevice *pMediaDevice_CaptCard = m_mapMediaDevice_Find( pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_dwPK_Device);
			if( pMediaDevice_CaptCard )
			{
				// We won't need to use the capture card if it's in the same EA
				if( pMediaDevice_CaptCard->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaDevice_CaptCard->m_mapEntertainArea.end() )
				{
					pEntertainArea->m_pMediaDevice_ActiveDest=pMediaStream->m_pMediaDevice_Source;  // It's the same as the source
					continue;
				}
				else if( pEntertainArea->m_pMediaDevice_MD )
				{
					pEntertainArea->m_pMediaDevice_ActiveDest=pEntertainArea->m_pMediaDevice_MD; // Going to the destination md
					continue;
				}
			}

		}
		else if( pMediaStream->m_pMediaDevice_Source->m_mapEntertainArea.find( pEntertainArea->m_iPK_EntertainArea )!=
			pMediaStream->m_pMediaDevice_Source->m_mapEntertainArea.end() )
		{
			pEntertainArea->m_pMediaDevice_ActiveDest=pMediaStream->m_pMediaDevice_Source;  // It's the same as the source
			continue;
		}

		if( pMediaStream->m_pMediaHandlerInfo->m_bMultipleDestinations )
		{
			// It can stream.  Find a destination in the EA
			for(list<class MediaDevice *>::iterator itMD=pMediaStream->m_pMediaHandlerInfo->m_listMediaDevice.begin();itMD!=pMediaStream->m_pMediaHandlerInfo->m_listMediaDevice.end();++itMD)
			{
				MediaDevice *pMediaDevice = *itMD;
				if( pMediaDevice->m_mapEntertainArea.find( pEntertainArea->m_iPK_EntertainArea )!=pMediaDevice->m_mapEntertainArea.end() )
				{
					// We found another device in this EA that can be the active destination
					pEntertainArea->m_pMediaDevice_ActiveDest=pMediaDevice;
					break;
				}
			}
		}

		if( !pEntertainArea->m_pMediaDevice_ActiveDest )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::StartMedia no ActiveDest for EA %d", pEntertainArea->m_iPK_EntertainArea);
	}
}

void Media_Plugin::StartCaptureCard(MediaStream *pMediaStream)
{
	pMediaStream->m_pDevice_CaptureCard = m_pRouter->m_mapDeviceData_Router_Find( pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_dwPK_Device );
	if( !pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard || !pMediaStream->m_pDevice_CaptureCard || !pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_pDevice_ControlledVia )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::StartCaptureCard -- error");
		return; // Shouldn't happen
	}

	// Since this is only used here, I am not changing the MediaDevice/MediaStream structures
	// to accomodate an audio port for the capture card. There is no point. -TSCHAK

	DeviceData_Router *pDevice_CaptureCard_AudioPort = m_pRouter->m_mapDeviceData_Router_Find( atoi(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_FK_Device_Capture_Card_Audio_P_CONST).c_str()));

	if (pDevice_CaptureCard_AudioPort)
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartCaptureCard(): Audio Port Device is %d",pDevice_CaptureCard_AudioPort->m_dwPK_Device);

	// We check this later.

	// Find the media player to play this capture card
	DeviceData_Router *pDevice_MediaPlayer = (DeviceData_Router *) ((DeviceData_Router *) pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard)->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);

	// Find the device
	string sDevice = pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_mapParameters_Find(DEVICEDATA_Block_Device_CONST);
	if( sDevice.empty() && pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_pDevice_ControlledVia )
		sDevice = ((DeviceData_Router *) pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_pDevice_ControlledVia)->m_mapParameters_Find(DEVICEDATA_Block_Device_CONST);

	if( sDevice.empty() || !pDevice_MediaPlayer )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::StartCaptureCard - Device is empty or no media player for %d (%s,%p)",
			pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_dwPK_Device,sDevice.c_str(),pDevice_MediaPlayer);
		return;
	}

	pMediaStream->m_sAppName = pDevice_MediaPlayer->m_mapParameters_Find(DEVICEDATA_Name_CONST);

	// See if there's a utility needed to switch to this port
	string sPortSelectUtility = pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_mapParameters_Find(DEVICEDATA_File_Name_and_Path_CONST);
	if( sPortSelectUtility.empty() && pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_pDevice_ControlledVia )
		sPortSelectUtility = ( (DeviceData_Router *) pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_pDevice_ControlledVia)->m_mapParameters_Find(DEVICEDATA_File_Name_and_Path_CONST);

	if( sPortSelectUtility.empty()==false )
	{
		// Find the App Server
		DeviceData_Base *pDevice_App_Server = ((DeviceData_Router *) pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard)->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
		if( pDevice_App_Server )
		{
			string sArguments = pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_mapParameters_Find(DEVICEDATA_Extra_Parameters_CONST);
			string sArguments2;

			if (pDevice_CaptureCard_AudioPort) // look above for this pointer.
			{
				sArguments2 = pDevice_CaptureCard_AudioPort->m_mapParameters_Find(DEVICEDATA_Extra_Parameters_CONST);
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartCaptureCard(): Arguments2 is %s",sArguments2.c_str());
			}

			sArguments = sArguments + " " + sArguments2;
			StringUtils::Replace(&sArguments,"<%=BLOCK%>",sDevice);
			StringUtils::Replace(&sArguments," ","\t");  // The database uses spaces to separate the arguments, we need tabs
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
				sPortSelectUtility,"setinput",sArguments,"","",false,false,false,true);
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
			SendCommand(CMD_Spawn_Application);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::StartCaptureCard -- no app server to set port for %d",pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_dwPK_Device);
	}

	if( pMediaStream->StreamingRequired() )
	{
		// For now we're not able to have a xine that renders to a NULL window and can do dvd's.  They require
		// a live window with events.  So for the moment this function will confirm that if we're playing a dvd disc remotely that we make the
		// source be one of the destinations, and change the mrl to reference the source disk

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StartCaptureCard id %d streaming to %d",
			pMediaStream->m_iStreamID_get(),
			pDevice_MediaPlayer->m_dwPK_Device);

		string sTargets = pMediaStream->GetTargets(DEVICETEMPLATE_Xine_Player_CONST);
		DCE::CMD_Start_Streaming cmd(m_dwPK_Device,
								pDevice_MediaPlayer->m_dwPK_Device,
								pMediaStream->m_iPK_MediaType,
								pMediaStream->m_iStreamID_get( ),
								"",
								"fifo://" + sDevice,
								sTargets);

		// No handling of errors (it will in some cases deadlock the router.)
		SendCommand(cmd);

		// In any ea's with the target devices, those are the active destinations
		string::size_type pos=0;
		while(pos<sTargets.size())
		{
			int PK_Device_Streaming_Dest = atoi( StringUtils::Tokenize(sTargets,",",pos).c_str() );
			MediaDevice *pMediaDevice = m_mapMediaDevice_Find(PK_Device_Streaming_Dest);
			if( pMediaDevice )
			{
				for(map<int,EntertainArea *>::iterator itEA=pMediaDevice->m_mapEntertainArea.begin();itEA!=pMediaDevice->m_mapEntertainArea.end();++itEA)
				{
					EntertainArea *pEntertainArea = itEA->second;
					if( pMediaStream->m_mapEntertainArea.find( pEntertainArea->m_iPK_EntertainArea ) != pMediaStream->m_mapEntertainArea.end() && pEntertainArea->m_pMediaDevice_ActiveDest==NULL )
						pEntertainArea->m_pMediaDevice_ActiveDest = pMediaDevice;
				}
			}
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StartCaptureCard id %d playing to %d",
			pMediaStream->m_iStreamID_get(),
			pDevice_MediaPlayer->m_dwPK_Device);
		DCE::CMD_Play_Media cmd(m_dwPK_Device,
								pDevice_MediaPlayer->m_dwPK_Device,
								pMediaStream->m_iPK_MediaType,
								pMediaStream->m_iStreamID_get( ),
								"",
								"fifo://" + sDevice);

		// No handling of errors (it will in some cases deadlock the router.)
		SendCommand(cmd);
	}

	// We're using a capture card.  Make it active
	pMediaStream->m_pMediaDevice_Source->m_bCaptureCardActive = true;
}

void Media_Plugin::StopCaptureCard(MediaStream *pMediaStream)
{
	// Find the media player to play this capture card
	DeviceData_Base *pDevice_MediaPlayer = ((DeviceData_Router *) pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard->m_pDevice_ControlledVia)->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::StopCaptureCard id %d to %p", pMediaStream->m_iStreamID_get(), pDevice_MediaPlayer );
	if( pDevice_MediaPlayer )
	{
		string sMediaPosition;
		DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,pDevice_MediaPlayer->m_dwPK_Device,pMediaStream->m_iStreamID_get(),&sMediaPosition);
		SendCommand(CMD_Stop_Media);
	}
}

ReceivedMessageResult Media_Plugin::ReceivedMessage( class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media plug in received message id: %d", pMessage->m_dwID );
    // Give it to our base class to see if we have a handler
    if( Media_Plugin_Command::ReceivedMessage( pMessage )!=rmr_Processed )
    {
        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media plug base class didn't handle message id: %d", pMessage->m_dwID );

		// Find the EA this message corresponds to.  If one is passed as a parameter that takes precedence
        class EntertainArea *pEntertainArea=NULL;
		if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
		{
			map<long, string>::iterator itEA = pMessage->m_mapParameters.find(COMMANDPARAMETER_PK_EntertainArea_CONST);
			if( itEA!=pMessage->m_mapParameters.end() )
				pEntertainArea = m_mapEntertainAreas_Find( atoi(itEA->second.c_str()) );
		}
		// otherwise if the message comes from an orbiter, assume it's the ea for that device
		if( !pEntertainArea )
		{
			OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );
			if( !pOH_Orbiter ) // It could be a remote control
				pOH_Orbiter = m_pOrbiter_Plugin->m_mapRemote_2_Orbiter_Find( pMessage->m_dwPK_Device_From );

			if( !pOH_Orbiter || !pOH_Orbiter->m_pEntertainArea )
			{
				if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && (pMessage->m_dwID==COMMAND_Stop_CONST || pMessage->m_dwID==COMMAND_Stop_Media_CONST)
					&& pMessage->m_mapParameters[COMMANDPARAMETER_Eject_CONST]=="1" )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got a stop with no media.  Will eject 1");
					DCE::CMD_Eject_Disk_Cat CMD_Eject_Disk_Cat(pMessage->m_dwPK_Device_From,DEVICECATEGORY_Disc_Drives_CONST,true,BL_SameComputer, 0);
					SendCommand(CMD_Eject_Disk_Cat);
					return rmr_Processed;
				}

				// Worst case just find an entertainment area in the same room.  It may be a new remote that we haven't reloaded yet, and it may not have a room but at least have a parent
				Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pMessage->m_dwPK_Device_From);
				Row_Device *pRow_Device_Parent = NULL;
				int PK_Room=0;
				if( pRow_Device && ((PK_Room=pRow_Device->FK_Room_get()) || ((pRow_Device_Parent=pRow_Device->FK_Device_ControlledVia_getrow())!=NULL && (PK_Room=pRow_Device_Parent->FK_Room_get()))) )
				{
					for( map<int, EntertainArea *>::iterator itEntArea=m_mapEntertainAreas.begin( );itEntArea!=m_mapEntertainAreas.end( );++itEntArea )
					{
						EntertainArea *pE = ( *itEntArea ).second;
						if( pE->m_pRoom && pE->m_pRoom->m_dwPK_Room==PK_Room )
						{
							pEntertainArea = pE;
							break;
						}
					}
					if( !pEntertainArea )
					{
						// Could be a timing issue that the stream finished and Orbiter didn't change the screen yet
						LoggerWrapper::GetInstance()->Write( LV_WARNING, "An orbiter sent the media handler message type: %d id: %d, but it's not for me and I can't find a entertainment area", pMessage->m_dwMessage_Type, pMessage->m_dwID );
						return rmr_NotProcessed;
					}
				}
				else
				{
					// Could be a timing issue that the stream finished and Orbiter didn't change the screen yet
					LoggerWrapper::GetInstance()->Write( LV_WARNING, "An orbiter sent the media handler message type: %d id: %d, but it's not for me and I can't find a entertainment area", pMessage->m_dwMessage_Type, pMessage->m_dwID );
					return rmr_NotProcessed;
				}
			}
			else
				pEntertainArea=pOH_Orbiter->m_pEntertainArea;
		}

		if( !pEntertainArea->m_pMediaStream )
		{
			// If it's a volume command, let it go through to the m/d (which will get to the tv/stereo) anyway
			if( pEntertainArea->m_pOH_Orbiter_OSD && pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND &&
				(pMessage->m_dwID==COMMAND_Set_Volume_CONST || pMessage->m_dwID==COMMAND_Vol_Up_CONST || pMessage->m_dwID==COMMAND_Vol_Down_CONST) )
			{
				Message *pNewMessage = new Message( pMessage );
				pNewMessage->m_dwPK_Device_To = pEntertainArea->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_dwPK_Device_ControlledVia;
				QueueMessageToRouter( pNewMessage );
				return rmr_Processed;
			}
			else if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && (pMessage->m_dwID==COMMAND_Stop_CONST || pMessage->m_dwID==COMMAND_Stop_Media_CONST)
					&& pMessage->m_mapParameters[COMMANDPARAMETER_Eject_CONST]=="1" )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got a stop with no media.  Will eject 4");
				DCE::CMD_Eject_Disk_Cat CMD_Eject_Disk_Cat(pMessage->m_dwPK_Device_From,DEVICECATEGORY_Disc_Drives_CONST,true,BL_SameComputer, 0);
				SendCommand(CMD_Eject_Disk_Cat);
				return rmr_Processed;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "An orbiter sent the media handler message type: %d id: %d, but there's no stream in ea %d", pMessage->m_dwMessage_Type, pMessage->m_dwID, pEntertainArea->m_iPK_EntertainArea );
				return rmr_NotProcessed;
			}
		}

		// Add any custom pipe if this is going through another output zone
		CheckForCustomPipe(pEntertainArea,pMessage);

		// Add some stuff to the message parameters
		pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST] = StringUtils::itos(pEntertainArea->m_pMediaStream->m_iStreamID_get());

		if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && pMessage->m_dwID==COMMAND_Eject_Disk_CONST )
		{
			DCE::CMD_Eject_Disk_Cat CMD_Eject_Disk_Cat(pMessage->m_dwPK_Device_From,DEVICECATEGORY_Disc_Drives_CONST,true,BL_SameComputer, 0);
			SendCommand(CMD_Eject_Disk_Cat);
			return rmr_Processed;
		}

		// If it's a stop, treat it like a stop media, unless we're using a generic external device where it goes to that device
		// and we should let it pass, and have the user use 'power' to stop the media
		if( (pMessage->m_dwID==COMMAND_Stop_CONST || pMessage->m_dwID==COMMAND_Stop_Media_CONST) &&
			(!pEntertainArea || !pEntertainArea->m_pMediaStream || pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo!=m_pGenericMediaHandlerInfo) )
		{
			string sResult;
			CMD_MH_Stop_Media(0,0,0,StringUtils::itos(pEntertainArea->m_iPK_EntertainArea),false,sResult,pMessage);  // It expects to get a valid message
			return rmr_NotProcessed;
		}
		else if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source &&
			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->m_bPreProcessSpeedControl &&
			pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND &&
			(pMessage->m_dwID==COMMAND_Change_Playback_Speed_CONST ||
			pMessage->m_dwID==COMMAND_Pause_Media_CONST ||
			pMessage->m_dwID==COMMAND_Play_Media_CONST) )
		{
			MediaDevice *pMediaDevice = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source;
			if( pMessage->m_dwID==COMMAND_Pause_Media_CONST && pMediaDevice->m_iLastPlaybackSpeed!=0 )
			{
				MediaInfoChanged(pEntertainArea->m_pMediaStream,false);
				pMediaDevice->m_iLastPlaybackSpeed = 0;
			}
			else if( pMessage->m_dwID==COMMAND_Pause_Media_CONST )
			{
				// It's the second pause in a row.  Change it to a play
				pMessage->m_dwID=COMMAND_Change_Playback_Speed_CONST;
				pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST] = "1000";
				pMediaDevice->m_iLastPlaybackSpeed = 1000;
				MediaInfoChanged(pEntertainArea->m_pMediaStream,false);
			}
			else if( pMessage->m_dwID==COMMAND_Play_Media_CONST && pMediaDevice->m_iLastPlaybackSpeed!=1000 )
			{
				MediaInfoChanged(pEntertainArea->m_pMediaStream,false);
				pMessage->m_dwID=COMMAND_Change_Playback_Speed_CONST;
				pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST] = "1000";
				pMediaDevice->m_iLastPlaybackSpeed = 1000;
			}
			else if (pMessage->m_dwID==COMMAND_Play_Media_CONST)
			{
				// It's the second play in a row.  Change it to a pause
				// If we need a discrete "play" command, we should create a new constant
				// for "play/pause" combo buttons.
				pMessage->m_dwID=COMMAND_Change_Playback_Speed_CONST;
				pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST] = "0";
				pMediaDevice->m_iLastPlaybackSpeed = 0;
				MediaInfoChanged(pEntertainArea->m_pMediaStream,false);
			}
			else if( pMessage->m_dwID==COMMAND_Change_Playback_Speed_CONST )
			{
				string sValue = pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST];
				if( sValue.size() )
				{
					int iValue=1;
					if( sValue[0]=='+' )
						iValue = (sValue.size()>1 ? atoi(sValue.substr(1).c_str()) : 1);
					else if( sValue[0]=='-' )
						iValue = (sValue.size()>1 ? atoi(sValue.substr(1).c_str()) : 1) * -1;
					else
						iValue = atoi(sValue.c_str());

					// Relative speed
					if( abs(iValue)<10 && iValue )
					{
						// Nothing does more than 64x, so if we're already more than 32, go back to normal
						if( abs(pMediaDevice->m_iLastPlaybackSpeed)>32000 )
							pMediaDevice->m_iLastPlaybackSpeed = 1000;

						// We're changing directions to reverse, start at -2000
						else if( iValue<0 && pMediaDevice->m_iLastPlaybackSpeed>0 )
							pMediaDevice->m_iLastPlaybackSpeed=-2000;

						// We're changing directions to forward, start at 2000
						else if( iValue>0 && pMediaDevice->m_iLastPlaybackSpeed<0 )
							pMediaDevice->m_iLastPlaybackSpeed=2000;

						// We were paused and now got a forward.  Start out in slow motion
						else if( iValue>0 && pMediaDevice->m_iLastPlaybackSpeed==0 )
							pMediaDevice->m_iLastPlaybackSpeed=250;

						// We were paused and now got a reverse.  Start out in slow motion
						else if( iValue<0 && pMediaDevice->m_iLastPlaybackSpeed==0 )
							pMediaDevice->m_iLastPlaybackSpeed=-250;

						else if( iValue<0 )
							pMediaDevice->m_iLastPlaybackSpeed =
								-1 * abs(pMediaDevice->m_iLastPlaybackSpeed * iValue);
						else
							pMediaDevice->m_iLastPlaybackSpeed =
								abs(pMediaDevice->m_iLastPlaybackSpeed * iValue);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"relative Playback speed now %d for device %d %s",
pMediaDevice->m_iLastPlaybackSpeed,
pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

						pMessage->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST] =
							StringUtils::itos(pMediaDevice->m_iLastPlaybackSpeed);
					}
					else
						pMediaDevice->m_iLastPlaybackSpeed = iValue;
				}
			}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Playback speed now %d for device %d %s",
pMediaDevice->m_iLastPlaybackSpeed,
pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

		}

        // We know it's derived from CommandImpl
        class Command_Impl *pPlugIn = pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl;
        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Checking to see if the plugin %s will handle it!", pPlugIn->m_sName.c_str());
        pMessage->m_dwPK_Device_To=pPlugIn->m_dwPK_Device;
		// Don't forward to the generic handler/capture card--it's just ourself
        if( pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo || pEntertainArea->m_pMediaStream->m_pDevice_CaptureCard || pPlugIn->ReceivedMessage( pMessage )!=rmr_Processed )
        {
            LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media plug in did not handled message id: %d forwarding to %d",
				pMessage->m_dwID, pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device );

			// TODO "Warning: received dcemessage should take a bool but don't delete in or something so we don't need to copy the message!"

			// If it's a command, it could be something for the media player (like a Pause), or something for the a/v equipment
			// that is connected to the media director (like vol up/down going to the receiver).  Don't bother with generic media
			if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo!=m_pGenericMediaHandlerInfo )
			{
			    Command *pCommand = m_pRouter->m_mapCommand_Find(pMessage->m_dwID);
				if( !pCommand )
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Got a command in media plugin that we can't identify");
				else
				{
				    if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_MD &&
						(pCommand->m_dwPK_Command==COMMAND_Generic_Off_CONST || pCommand->m_dwPK_Command==COMMAND_Generic_On_CONST || pCommand->m_listPipe.size()) )
					{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"It's an on/off");
						pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_MD;
						Message *pNewMessage = new Message( pMessage );
						QueueMessageToRouter( pNewMessage );
					}

					if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_mapCommands.find(pMessage->m_dwID) !=
						pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_mapCommands.end() )
					{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"It's a valid command");
						pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
						Message *pNewMessage = new Message( pMessage );
						QueueMessageToRouter( pNewMessage );
					}

					if( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD &&  // then it's going to the media director
						(pCommand->m_dwPK_Command==COMMAND_Vol_Up_CONST || pCommand->m_dwPK_Command==COMMAND_Vol_Down_CONST || pCommand->m_dwPK_Command==COMMAND_Set_Volume_CONST
						|| pCommand->m_dwPK_Command==COMMAND_Mute_CONST ) // It's a volume command
						)
					{
						MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD); // We have an app server to control the volume
						if( pMediaDevice && pMediaDevice->m_pDevice_Audio && pMediaDevice->m_pDevice_Audio->m_dwPK_DeviceTemplate==DEVICETEMPLATE_App_Server_CONST )  // We have an MD and it uses appserver for the volume
						{
							Message *pNewMessage = new Message( pMessage );
							pNewMessage->m_dwPK_Device_To = pMediaDevice->m_pDevice_Audio->m_dwPK_Device;
							QueueMessageToRouter( pNewMessage );
						}
					}

					// For streaming media the source and destination are often different.  Send it to the dest as well
				    if( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest!=pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
					{
						Message *pNewMessage = new Message( pMessage );
						pNewMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device;
						QueueMessageToRouter( pNewMessage );

						// It could be an a/v command that is routed through the m/d to a receiver/tv
						if( pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD &&
							pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD != pNewMessage->m_dwPK_Device_To )
						{
							pNewMessage = new Message( pMessage );
							pNewMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD;
							QueueMessageToRouter( pNewMessage );
						}
					}
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Just send it to the media device");
				// Just send it to the media device.  We don't know what it is
				pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;

				// Look for special cases
				if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo )
				{
					Command *pCommand = m_pRouter->m_mapCommand_Find(pMessage->m_dwID);
					if( !pCommand )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Got a command in media plugin that we can't identify");
					else if( pEntertainArea->m_pMediaStream->m_pDevice_CaptureCard && pCommand->m_dwPK_Command==COMMAND_Set_Zoom_CONST )
					{
						DeviceData_Router *pDevice_MediaPlayer = (DeviceData_Router *) ((DeviceData_Router *) pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDevice_CaptureCard)->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
						if( pDevice_MediaPlayer )
							pMessage->m_dwPK_Device_To = pDevice_MediaPlayer->m_dwPK_Device;
					}
					else if( ( pCommand->m_dwPK_Command==COMMAND_Vol_Up_CONST ||
						   pCommand->m_dwPK_Command==COMMAND_Vol_Down_CONST ||
						   pCommand->m_dwPK_Command==COMMAND_Set_Volume_CONST ||
						   pCommand->m_dwPK_Command==COMMAND_Mute_CONST
						 ) && !pEntertainArea->m_bViewingLiveAVPath
                                               ) // It's a volume command and we're not on the LiveAV path
					{
						// See if we can send the volume request to a MD
						if( pEntertainArea->m_pMediaDevice_MD && pEntertainArea->m_pMediaDevice_MD->m_pDevice_Audio )
                                                        pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaDevice_MD->m_pDevice_Audio->m_dwPK_Device;
						else
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "Cannot find media director or audio device missing for volume commands %p", pEntertainArea->m_pMediaDevice_MD);

					}
				}

	            Message *pNewMessage = new Message( pMessage );
				QueueMessageToRouter( pNewMessage );
			}
        }

        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media plug in done with message id: %d", pMessage->m_dwID );
    }

    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media plug base done with id: %d", pMessage->m_dwID );

    return rmr_Processed;
}

void Media_Plugin::MediaInfoChanged( MediaStream *pMediaStream, bool bRefreshScreen )
{
	pMediaStream->UpdateDescriptions();
	pMediaStream->SetPicture(NULL, 0);

	if( pMediaStream->m_dwPK_Disc )
	{
		vector<Row_Picture_Disc *> vectRow_Picture_Disc;
		m_pDatabase_pluto_media->Picture_Disc_get()->GetRows("FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc),&vectRow_Picture_Disc);

LoggerWrapper::GetInstance()->Write(LV_STATUS, "Getting m_pPictureData for disc %d size %d",pMediaStream->m_dwPK_Disc,(int) vectRow_Picture_Disc.size());
		if( vectRow_Picture_Disc.size() )
		{
			size_t iSize = 0;
			char *pData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(vectRow_Picture_Disc[0]->FK_Picture_get()) + ".jpg", iSize);
			pMediaStream->SetPicture(pData, iSize);
		}
	}

    if( !pMediaStream->m_pPictureData_get() && pMediaStream->m_dequeMediaFile.size() )
    {
LoggerWrapper::GetInstance()->Write(LV_STATUS, "We have %d media entries in the playback list", pMediaStream->m_dequeMediaFile.size());
		MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
		int PK_Picture=0;

		/*
#ifdef WIN32
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got 2 picture data %p (FK_File: %d)", pMediaStream->m_pPictureData, pMediaFile->m_dwPK_File);
		if( pMediaFile->m_dwPK_File )
		{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking got media database file with ID: %d", pMediaFile->m_dwPK_File );
			Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(pMediaFile->m_dwPK_File);
			vector<Row_Picture_File *> vectRow_Picture_File;
			if ( pRow_File )
			{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "We found a file row %p", pRow_File );
				pRow_File->Picture_File_FK_File_getrows(&vectRow_Picture_File);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "We got %d rows.", vectRow_Picture_File.size() );
				if( vectRow_Picture_File.size() )
					PK_Picture = vectRow_Picture_File[0]->FK_Picture_get();
				else
				{
					vector<Row_File_Attribute *> vectRow_File_Attribute;
					pRow_File->File_Attribute_FK_File_getrows(&vectRow_File_Attribute);
					for(size_t s=0;s<vectRow_File_Attribute.size();++s)
					{
						vector<Row_Picture_Attribute *> vectRow_Picture_Attribute;
						vectRow_File_Attribute[s]->FK_Attribute_getrow()->Picture_Attribute_FK_Attribute_getrows(&vectRow_Picture_Attribute);
						if( vectRow_Picture_Attribute.size() )
						{
							PK_Picture = vectRow_Picture_Attribute[0]->FK_Picture_get();
							break;
						}
					}
				}
			}
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found PK_Picture to be: %d.", PK_Picture);
		}
#else
		*/

		string sFilePath = FileUtils::BasePath(pMediaFile->FullyQualifiedFile());
		string sFileName = FileUtils::FilenameWithoutPath(pMediaFile->FullyQualifiedFile(), true);
		m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(pMediaFile->m_dwPK_File,&PK_Picture);
//#endif

		if(PK_Picture)
		{
			size_t iSize = 0;
			char *pData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + ".jpg", iSize);
			pMediaStream->SetPicture(pData, iSize);
		}

    }
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to update bound remotes with %p %d",pMediaStream->m_pPictureData_get(),pMediaStream->m_iPictureSize_get());

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
        for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
        {
            BoundRemote *pBoundRemote = ( *itBR ).second;
			if( pEntertainArea!=pBoundRemote->m_pOH_Orbiter->m_pEntertainArea )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter %d is bound to an ent area it isn't in",pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			else
	            pBoundRemote->UpdateOrbiter( pMediaStream, bRefreshScreen );
        }
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
            if( pOH_Orbiter->m_pEntertainArea==pEntertainArea && // UpdateOrbiter will have already set the now playing
					pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)==pEntertainArea->m_mapBoundRemote.end() )
			{
				pMediaStream->SetNowPlaying( pOH_Orbiter, bRefreshScreen );
			}
        }
		for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
		{
			MediaDevice *pMediaDevice = *itVFD;
			DCE::CMD_Display_Message CMD_Display_Message_TC(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				(pMediaStream->m_sMediaDescription.find("<%=")==string::npos ? pMediaStream->m_sMediaDescription : "Unknown Disc"), // For now VFD's don't have the language table to do these lookups
				StringUtils::itos(VL_MSGTYPE_NOW_PLAYING_MAIN),"np","","");

			DCE::CMD_Display_Message CMD_Display_Message_Section(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				(pMediaStream->m_sSectionDescription.find("<%=")==string::npos ? pMediaStream->m_sSectionDescription : "Unknown"), // For now VFD's don't have the language table to do these lookups
				StringUtils::itos(VL_MSGTYPE_NOW_PLAYING_SECTION),"np","","");
			CMD_Display_Message_Section.m_pMessage->m_vectExtraMessages.push_back(CMD_Display_Message_TC.m_pMessage);
			SendCommand(CMD_Display_Message_Section);
		}
    }
	pMediaStream->LoadDefaultAvSettings();  // Handle special a/v settings and commands, like zoom modes, dsp, etc.
}

/*

    COMMANDS TO IMPLEMENT

*/
//<-dceag-c44-b->

	/** @brief COMMAND: #44 - MH Stop Media */
	/** Stop media. All parameters are optional. The Media handler must find out what media to stop. */
		/** @param #2 PK_Device */
			/** The specific device to stop media on. */
		/** @param #29 PK_MediaType */
			/** The type of media to stop. */
		/** @param #44 PK_DeviceTemplate */
			/** The type of device to stop the media on. */
		/** @param #45 PK_EntertainArea */
			/** This is the location on which we need to stop the media. This is optional. If not specified the orbiter will decide the location based on the controlled area. */
		/** @param #254 Bypass Event */
			/** If true, the usual event for 'Stopped Watching Media' won't be fired */

void Media_Plugin::CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bBypass_Event,string &sCMD_Result,Message *pMessage)
//<-dceag-c44-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to play media
    DetermineEntArea( pMessage->m_dwPK_Device_From, iPK_Device, sPK_EntertainArea, vectEntertainArea );
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
		for(vector<class MediaStream  *>::iterator it=pEntertainArea->m_vectMediaStream_Interrupted.begin();it!=pEntertainArea->m_vectMediaStream_Interrupted.end();++it)
			delete *it;
		pEntertainArea->m_vectMediaStream_Interrupted.clear();
		if( !pEntertainArea->m_pMediaStream )
			continue; // Don't know what area it should be played in, or there's no media playing there

		pEntertainArea->m_pMediaStream->m_sLastPosition = ""; // Be sure we get a real position
		pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pEntertainArea->m_pMediaStream );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Called StopMedia" );

		StreamEnded(pEntertainArea->m_pMediaStream,true,true,NULL,NULL,false,true,!bBypass_Event);
	}
}

void Media_Plugin::StreamEnded(MediaStream *pMediaStream,bool bSendOff,bool bDeleteStream,MediaStream *pMediaStream_Replacement,vector<EntertainArea *> *p_vectEntertainArea,bool bNoAutoResume,bool bTurnOnOSD,bool bFireEvent)
{
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"debug_stream_end Media_Plugin::StreamEnded ID %d/%p delete %d auto resume %d resume: %c",
pMediaStream->m_iStreamID_get(),pMediaStream, (int) bDeleteStream, (int) bNoAutoResume,m_mapPromptResume[make_pair<int,int> (pMediaStream->m_iPK_Users,pMediaStream->m_iPK_MediaType)]);


	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StreamEnded() called with NULL MediaStream in it! Ignoring");
		return;
	}

	if( bDeleteStream )   // Release the drive if the stream is really over and not just being moved
		ReleaseDriveLock(pMediaStream);

	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::StreamEnded ID %d delete %d auto resume %d resume: %c",pMediaStream->m_iStreamID_get(),(int) bDeleteStream, (int) bNoAutoResume,m_mapPromptResume[make_pair<int,int> (pMediaStream->m_iPK_Users,pMediaStream->m_iPK_MediaType)]);
#endif

	if( bNoAutoResume )
	{
		bool bError=false;
		string sWhere = "EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) + " AND Description<>'START' AND ";
		if( pMediaStream->m_dwPK_Disc )
			sWhere += "FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc) + " AND IsAutoResume=1";
        else if( pMediaStream->m_iPK_Playlist )
			sWhere += "FK_Playlist=" + StringUtils::itos(pMediaStream->m_iPK_Playlist) + " AND IsAutoResume=1";
		else if( pMediaStream->m_iDequeMediaFile_Pos>=0 && pMediaStream->m_iDequeMediaFile_Pos<pMediaStream->m_dequeMediaFile.size() )
		{
			MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
			if( pMediaFile->m_dwPK_File )
				sWhere += "FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND IsAutoResume=1";
			else
				bError=true;
		}
		else
			bError=true; // Don't do this if there's nothing to reference this

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::StreamEnded() no auto resume %s",sWhere.c_str());
		if( !bError )
			m_pDatabase_pluto_media->threaded_db_wrapper_query("DELETE FROM Bookmark WHERE " + sWhere);
	}
	// Unless this user has specified he doesn't ever want to resume this type of media, we should prompt him
	else if( m_mapPromptResume[make_pair<int,int> (pMediaStream->m_iPK_Users,pMediaStream->m_iPK_MediaType)]!='N' )
	{
		if( pMediaStream->m_dwPK_Disc )
			SaveLastDiscPosition(pMediaStream);

		if( pMediaStream->m_iPK_Playlist )
			SaveLastPlaylistPosition(pMediaStream);

		if( pMediaStream->m_iDequeMediaFile_Pos>=0 && pMediaStream->m_iDequeMediaFile_Pos<pMediaStream->m_dequeMediaFile.size() )
			SaveLastFilePosition(pMediaStream);
	}

	// This could have been playing in lots of entertainment areas
    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream %s ended with %d ent areas", pMediaStream->m_sMediaDescription.c_str(), (int) pMediaStream->m_mapEntertainArea.size() );
    for( MapEntertainArea::iterator it=pMediaStream->m_mapEntertainArea.begin( );it!=pMediaStream->m_mapEntertainArea.end( );++it )
    {
        EntertainArea *pEntertainArea = ( *it ).second;
		map<int,MediaDevice *> mapMediaDevice_Prior;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Getting Render Devices" );
		pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->GetRenderDevices(pEntertainArea,&mapMediaDevice_Prior);
		Row_MediaType *pRow_MediaType_Prior = m_pDatabase_pluto_main->MediaType_get()->GetRow(pMediaStream->m_iPK_MediaType);
		int PK_Pipe_Prior = pRow_MediaType_Prior && pRow_MediaType_Prior->FK_Pipe_isNull()==false ? pRow_MediaType_Prior->FK_Pipe_get() : 0;
		AddOtherDevicesInPipesToRenderDevices(PK_Pipe_Prior,&mapMediaDevice_Prior,pEntertainArea);

		if( pMediaStream_Replacement &&
				pMediaStream_Replacement->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaStream_Replacement->m_mapEntertainArea.end() &&
				pMediaStream_Replacement->ContainsVideo()==pMediaStream->ContainsVideo() )
			bFireEvent=false;

		for(size_t s=0;p_vectEntertainArea && s<p_vectEntertainArea->size();++s)
			if( (*p_vectEntertainArea)[s]==pEntertainArea )
			{
				bFireEvent=false;
				break;
			}

		MediaInEAEnded(pEntertainArea,bFireEvent);

		// If the stream ended because we're starting a new one, don't bother resuming any pending streams.
		// We'll do that when this replacement stream ends
		if( pEntertainArea->m_vectMediaStream_Interrupted.size() && !pMediaStream_Replacement )
		{
			MediaStream *pMediaStream_Resume = pEntertainArea->m_vectMediaStream_Interrupted.back();
			// Don't pay attention if it's the stream we just interrupted that ended
			if( pMediaStream_Resume!=pMediaStream )
			{
				pEntertainArea->m_vectMediaStream_Interrupted.pop_back();
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from StreamEnded");
				StartMedia(pMediaStream_Resume);
			}
		}

		// Need to switch inputs on any pipes back to the orbiter.  Use the MediaType from the device that is
		// turing off so that if it was an audio only device we only switch the audio pipes to the OSD orbiter
		// and not any of the video pipes.
		int iPK_MediaType_Current = 0;
		map<int,MediaDevice *> mapMediaDevice_Current;
		if( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_MD && bTurnOnOSD )
		{
			int iPK_Device_MD = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_MD->m_dwPK_Device;
			iPK_MediaType_Current = pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType;
			mapMediaDevice_Current[iPK_Device_MD] = m_mapMediaDevice_Find(iPK_Device_MD);
		}

		if( bSendOff )
			HandleOnOffs(pMediaStream->m_pMediaHandlerInfo->m_PK_MediaType,iPK_MediaType_Current,&mapMediaDevice_Prior,&mapMediaDevice_Current,pMediaStream,pEntertainArea);
	}

	if( pMediaStream->m_pDevice_CaptureCard )
		StopCaptureCard(pMediaStream);

	if( bDeleteStream )
	{
		// Just be sure this stream isn't still pending somewhere
		for( MapEntertainArea::iterator it=m_mapEntertainAreas.begin( );it!=m_mapEntertainAreas.end( );++it )
		{
			EntertainArea *pEntertainArea = ( *it ).second;

			for(vector<MediaStream *>::iterator itMSI=pEntertainArea->m_vectMediaStream_Interrupted.begin();itMSI!=pEntertainArea->m_vectMediaStream_Interrupted.end();)
			{
				if( *itMSI==pMediaStream )
				{
					itMSI=pEntertainArea->m_vectMediaStream_Interrupted.erase(itMSI);
				}
				else
					itMSI++;
			}
		}
	    delete pMediaStream;
	}
	else
		pMediaStream->m_bStopped=true;
}

void Media_Plugin::MediaInEAEnded(EntertainArea *pEntertainArea,bool bFireEvent)
{
	if( bFireEvent )
	{
		if( pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->ContainsVideo() )
			EVENT_Stopped_Watching_Media(pEntertainArea->m_pRoom->m_dwPK_Room);
		else
			EVENT_Stopped_Listening_To_Medi(pEntertainArea->m_pRoom->m_dwPK_Room);
	}

LoggerWrapper::GetInstance()->Write( LV_STATUS, "Stream in ea %s ended %d remotes bound bFireEvent %d", pEntertainArea->m_sDescription.c_str(), (int) pEntertainArea->m_mapBoundRemote.size(), (int) bFireEvent );
	pEntertainArea->m_pMediaStream = NULL;
	pEntertainArea->m_pMediaDevice_ActiveDest=NULL;

	if( !bFireEvent )
		return;  // If this is false, it means the calling class will be starting something else, which will create all it's own 'now playing's' and goto remotes.  Just leave this

	// Set all the now playing's to nothing
    for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        if( pOH_Orbiter->m_pEntertainArea==pEntertainArea )
		{
LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter %d %s in this ea to stop", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_sDescription.c_str());
			SetNothingNowPlaying( pOH_Orbiter, false );
		}
    }

	// Clear the VFD displays
	for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
	{
		MediaDevice *pMediaDevice = *itVFD;
		DCE::CMD_Display_Message CMD_Display_Message_TC(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
			"",
			StringUtils::itos(VL_MSGTYPE_NOW_PLAYING_MAIN),"np","","");
		SendCommand(CMD_Display_Message_TC);
	}

	// Send all bound remotes back home
	for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
    {
        BoundRemote *pBoundRemote = ( *itBR ).second;

		if( pBoundRemote->m_sPK_DesignObj_GraphicImage.size() )
		{
			DCE::CMD_Update_Object_Image CMD_Update_Object_Image( 0, pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pBoundRemote->m_sPK_DesignObj_GraphicImage, "jpg", NULL, 0, "0" );
			SendCommand(CMD_Update_Object_Image);
		}

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter %d %s is bound", pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_sDescription.c_str());
		//DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,"<%=M%>","","",false, false);
		//SendCommand(CMD_Goto_DesignObj);
		DCE::SCREEN_Main SCREEN_Main(m_dwPK_Device,pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,"<%=L%>"); //current location
		SendCommand(SCREEN_Main);

    }
	ShowMediaPlaybackState(pEntertainArea);
}

//<-dceag-c74-b->

	/** @brief COMMAND: #74 - Bind to Media Remote */
	/** When an orbiter goes to a media remote control screen, it fires this command so that the media plug-in knows it is sitting at a remote, and needs to be notified when the media changes or the cover art changes.  This should be in the onLoad commands of eve */
		/** @param #2 PK_Device */
			/** The device the orbiter is controlling. */
		/** @param #3 PK_DesignObj */
			/** The object where the remote displays the graphical image of the cover art.  It will get update object images when the cover art changes. */
		/** @param #8 On/Off */
			/** If 1, bind (the Orbiter is sitting at the remote screen).  If 0, the orbiter has left the remote screen, and does not need media changed commands. */
		/** @param #39 Options */
			/** Miscellaneous options.  These are not pre-defined, but are specific to a remote and the plug-in.  For example, the PVR plug-in needs to know what tuning device is active. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area the orbiter is controlling. */
		/** @param #63 PK_Text_Synopsis */
			/** The text object for the synopsis, a full description.  Examples are a DVD synopsis, or a description of a tv show. */
		/** @param #159 PK_Screen */
			/** The current screen. */

void Media_Plugin::CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis,int iPK_Screen,string &sCMD_Result,Message *pMessage)
//<-dceag-c74-e->
{
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CMD_Bind_to_Media_Remote(). Binding (%s) orbiter %d to device %d with cover art on the object: %s",
			sOnOff.c_str(),
			pMessage->m_dwPK_Device_From,
			iPK_Device, sPK_DesignObj.c_str());

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Only an Orbiter will send this
    OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );

	vector<EntertainArea *> vectEntertainArea;
	DetermineEntArea( pMessage->m_dwPK_Device_From, iPK_Device, sPK_EntertainArea, vectEntertainArea );
    if( vectEntertainArea.size()!=1 || !pOH_Orbiter )
    {
        sCMD_Result="Can't find ent area/orbiter";
        return; // Don't know what area it should be played in, or there's no media playing there
    }

	EntertainArea *pEntertainArea = vectEntertainArea[0];

	if( !atoi( sOnOff.c_str( ) ) ) // Off
        pEntertainArea->m_mapBoundRemote_Remove( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device );
    else
    {
		if( !pEntertainArea->m_pMediaStream )
		{
			sCMD_Result="No media stream";
			if( sOptions.find("X")==string::npos )  // Means don't send me to the main menu if there's no media playing
			{
				SetNothingNowPlaying(pOH_Orbiter,true);
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Attempt to bind to a remote in an entertainment area with no media stream");
			}
			return; // Don't know what area it should be played in, or there's no media playing there
		}

        BoundRemote *pBoundRemote=pEntertainArea->m_mapBoundRemote_Find( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device );
        if( !pBoundRemote )
        {
            pBoundRemote = new BoundRemote( this );
            pEntertainArea->m_mapBoundRemote[pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device]=pBoundRemote;
        }
        pBoundRemote->m_pOH_Orbiter = pOH_Orbiter;
        pBoundRemote->m_pEntertainArea = pEntertainArea;
        pBoundRemote->m_sPK_DesignObj_GraphicImage = sPK_DesignObj;
        pBoundRemote->m_PK_Screen = iPK_Screen;
        pBoundRemote->m_sOptions = sOptions;

        pBoundRemote->m_iPK_Text_Synopsis = iPK_Text_Synopsis;
		if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"embedding set now playing");
			Message *pMessage_Response=NULL;
	        pBoundRemote->UpdateOrbiter( pEntertainArea->m_pMediaStream, false, &pMessage_Response ); // So that it will put the picture back on this remote
			if( pMessage_Response )
			{
				pMessage->m_bRespondedToMessage=true;
				SendMessage(pMessage_Response);
			}
		}
		else
	        pBoundRemote->UpdateOrbiter( pEntertainArea->m_pMediaStream, false ); // So that it will put the picture back on this remote
LoggerWrapper::GetInstance()->Write(LV_STATUS,"EA %d %s bound %d remotes",pEntertainArea->m_iPK_EntertainArea,pEntertainArea->m_sDescription.c_str(),(int) pEntertainArea->m_mapBoundRemote.size());

		int PK_Screen_ShouldBe = pEntertainArea->m_pMediaStream->GetRemoteControlScreen(pMessage->m_dwPK_Device_From);
		if( iPK_Screen && PK_Screen_ShouldBe && iPK_Screen != PK_Screen_ShouldBe )
		{
			DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,pMessage->m_dwPK_Device_From,"",PK_Screen_ShouldBe,interuptAlways,true,false);
			SendCommand(CMD_Goto_Screen);
LoggerWrapper::GetInstance()->Write(LV_WARNING,"EA %d %s bound %d remotes was at screen %d sending to %d",
					  pEntertainArea->m_iPK_EntertainArea,pEntertainArea->m_sDescription.c_str(),(int) pEntertainArea->m_mapBoundRemote.size(),iPK_Screen,PK_Screen_ShouldBe);
		}
    }
}

void Media_Plugin::DetermineEntArea( int iPK_Device_Orbiter, int iPK_Device, string sPK_EntertainArea, vector<EntertainArea *> &vectEntertainArea, int *p_iStreamID )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	MediaStream *pMediaStream = NULL;

    // See if we need to figure out the entertainment area on our own. If so, the only way to do this is if the message came from an orbiter
    if( sPK_EntertainArea.size()==0 || sPK_EntertainArea=="0" )  // if the ent area isn't specified
    {
        OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( iPK_Device_Orbiter );
		if( !pOH_Orbiter ) // It could be a remote control
			pOH_Orbiter = m_pOrbiter_Plugin->m_mapRemote_2_Orbiter_Find( iPK_Device_Orbiter );
		MediaDevice *pMediaDevice;
		if( iPK_Device && (pMediaDevice = m_mapMediaDevice_Find(iPK_Device))!=NULL )
		{
			if( NULL != pOH_Orbiter && pOH_Orbiter->m_pEntertainArea && pMediaDevice->m_mapEntertainArea.find( pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea )!=pMediaDevice->m_mapEntertainArea.end() )
				vectEntertainArea.push_back(pOH_Orbiter->m_pEntertainArea);
			else
			{
				// Maybe there's a specific media device we should use
				for( MapEntertainArea::iterator it=pMediaDevice->m_mapEntertainArea.begin( );it!=pMediaDevice->m_mapEntertainArea.end( );++it )
				{
					EntertainArea *pEntertainArea = it->second;
					if( pEntertainArea->m_pMediaStream )
					{
						if( !pMediaStream )
							pMediaStream = pEntertainArea->m_pMediaStream;
						else if( pMediaStream!=pEntertainArea->m_pMediaStream )
							LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::DetermineEntArea found stream %d and now %d",pMediaStream->m_iStreamID_get(),pEntertainArea->m_pMediaStream->m_iStreamID_get());
					}
					vectEntertainArea.push_back(( *it ).second);
				}
			}
		}
        else if( !pOH_Orbiter )
        {
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Received a play media with no entertainment area/device from a non-orbiter %d %d %s",iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea.c_str() );
			return; // Don't know what area it should be played in
        }
        else if( !pOH_Orbiter->m_pEntertainArea )
        {
			// It could be an orbiter that was recently added
			for( map<int, EntertainArea *>::iterator itEntArea=m_mapEntertainAreas.begin( );itEntArea!=m_mapEntertainAreas.end( );++itEntArea )
			{
				class EntertainArea *pEntertainArea = ( *itEntArea ).second;
				if( pEntertainArea->m_pRoom && pEntertainArea->m_pRoom->m_dwPK_Room == pOH_Orbiter->m_dwPK_Room )
				{
					if( pEntertainArea->m_pMediaStream )
					{
						if( !pMediaStream )
							pMediaStream = pEntertainArea->m_pMediaStream;
						else if( pMediaStream!=pEntertainArea->m_pMediaStream )
							LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::DetermineEntArea 2 found stream %d and now %d",pMediaStream->m_iStreamID_get(),pEntertainArea->m_pMediaStream->m_iStreamID_get());
					}
		            vectEntertainArea.push_back(pEntertainArea);
					break;
				}
			}

			if( vectEntertainArea.size()==0 )
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Received a DetermineEntArea from an orbiter %p %d with no entertainment area (%p) %d %d %s",
					pOH_Orbiter,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pEntertainArea,
					iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea.c_str() );
				return; // Don't know what area it should be played in
			}
        }
		else
		{
			if( pOH_Orbiter->m_pEntertainArea->m_pMediaStream )
			{
				if( !pMediaStream )
					pMediaStream = pOH_Orbiter->m_pEntertainArea->m_pMediaStream;
				else if( pMediaStream!=pOH_Orbiter->m_pEntertainArea->m_pMediaStream )
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::DetermineEntArea 3 found stream %d and now %d",pMediaStream->m_iStreamID_get(),pOH_Orbiter->m_pEntertainArea->m_pMediaStream->m_iStreamID_get());
			}
			vectEntertainArea.push_back(pOH_Orbiter->m_pEntertainArea);
		}
    }

	string::size_type pos=0;
	while( pos<sPK_EntertainArea.size() )
	{
		string s = StringUtils::Tokenize(sPK_EntertainArea,",",pos);
		EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( abs(atoi(s.c_str())) );  // abs() since orbiters may pass them in as negative since floorplans do that to differentiate between ea's and devices
		if( !pEntertainArea )
		{
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Received a play media for an invalid entertainment area %d %d %s",iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea.c_str() );
			return; // Don't know what area it should be played in
		}
		if( pEntertainArea->m_pMediaStream )
		{
			if( !pMediaStream )
				pMediaStream = pEntertainArea->m_pMediaStream;
			else if( pMediaStream!=pEntertainArea->m_pMediaStream )
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::DetermineEntArea 2 found stream %d and now %d",pMediaStream->m_iStreamID_get(),pEntertainArea->m_pMediaStream->m_iStreamID_get());
		}
		vectEntertainArea.push_back(pEntertainArea);
	}

	if( pMediaStream && p_iStreamID )
	{
		if( *p_iStreamID && *p_iStreamID!=pMediaStream->m_iStreamID_get() )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::DetermineEntArea past in stream id %d but found %d",*p_iStreamID,pMediaStream->m_iStreamID_get());
		*p_iStreamID = pMediaStream->m_iStreamID_get();
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found the proper ent area: %d", (int) vectEntertainArea.size());
#endif
}

int Media_Plugin::DetermineUserOnOrbiter(int iPK_Device_Orbiter)
{
    if ( !m_pOrbiter_Plugin )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "There is no valid orbiter_plugin in the media plugin. (While trying to find a user on the orbiter: %d)", iPK_Device_Orbiter );
        return 0;
    }

    class OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(iPK_Device_Orbiter);
    if ( ! pOH_Orbiter )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Not orbiter found for id %d while trying to find the active user.", iPK_Device_Orbiter);
        return 0;
    }

    return pOH_Orbiter->PK_Users_get();
}

bool Media_Plugin::IgnoreFileForMediaType(string sFilename, int iPK_MediaType)
{
  if (iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST)
    {
      if (FileUtils::DirExists(sFilename)) // This is a directory.
        {	
		return true;
	}
      if (!FileUtils::FileExists(sFilename)) // File does not exist somehow.
	{
		return true;
	}
    }
  return false;
}

//<-dceag-c43-b->

	/** @brief COMMAND: #43 - MH Play Media */
	/** The Orbiters send the play media command to the actual media handler. rnrnThe Orbiter can send anyone or a combination of parameters. rnrnIt's up to media handler to figure out how to handle it. The media handler must find out if the media is already pla */
		/** @param #2 PK_Device */
			/** The ID of the actual device to start playing. */
		/** @param #13 Filename */
			/** The filename to play or a pipe delimited list of filenames. */
		/** @param #29 PK_MediaType */
			/** The ID of the media type descriptor (if it is TV, movie, audio, etc ..  ) */
		/** @param #44 PK_DeviceTemplate */
			/** The DeviceTemplate ID. */
		/** @param #45 PK_EntertainArea */
			/** The desired target area for the playback. If this is missing then the orbiter should decide the target based on his controlled area. */
		/** @param #116 Resume */
			/** If true, when this media finishes, resume whatever was playing previously.  Useful for making announcements and similar. */
		/** @param #117 Repeat */
			/** 0=default for media type, 1=loop, -1=do not loop */
		/** @param #253 Queue */
			/** If true, the file won't be played unless the queue is empty.  It will be appended to the queue only */
		/** @param #254 Bypass Event */
			/** If true, the 'listening to media' event won't be fire. */
		/** @param #276 Dont Setup AV */
			/** If true, the on/input selects won't be sent to the a/v gear */

void Media_Plugin::CMD_MH_Play_Media(int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,bool bQueue,bool bBypass_Event,bool bDont_Setup_AV,string &sCMD_Result,Message *pMessage)
//<-dceag-c43-e->
{
    PLUTO_SAFETY_LOCK(mm,m_MediaMutex);

	MediaDevice *pMediaDevice_Source = iPK_Device ? m_mapMediaDevice_Find(iPK_Device) : NULL;

	int iPK_Device_Orbiter = pMessage ? pMessage->m_dwPK_Device_From : 0;
	vector<EntertainArea *> vectEntertainArea;

	// Only an Orbiter will tell us to play media
    DetermineEntArea(iPK_Device_Orbiter,iPK_Device,sPK_EntertainArea,vectEntertainArea);
    if( vectEntertainArea.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got a play media for no ent area");
        return;  // Don't know what area it should be played in
	}

	// i get the media type here when it's global
    deque<MediaFile *> dequeMediaFile;
    if( sFilename.length() > 0 )
 	{
		if ( sFilename[sFilename.length() - 1] == '*' )
		{
			list<string> allFilesList;
			list<string>::const_iterator itFileName;

			if ( iPK_MediaType == 0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "When you want a recursive listing of a folder you should also provide a target media type.");
				return;
			}

			Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(iPK_MediaType);
			if( !pRow_MediaType )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid media type PK provided: %d", iPK_MediaType);
				return;
			}

			string Extensions = pRow_MediaType->Extensions_get();
			StringUtils::Replace(&Extensions,",","\t");
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using there extensions to filter the results: %s", Extensions.c_str());
			sFilename = sFilename.substr(0, sFilename.length()-1);
			string::size_type posDirs=0;
			string sDirectory;
			while( posDirs<sFilename.size() )
			{
				sDirectory=StringUtils::Tokenize(sFilename,"\t",posDirs);
				if ( FileUtils::FindFiles(allFilesList, sDirectory, Extensions.c_str(), true, true) ) // we have hit the bottom
				{
					SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device, iPK_Device_Orbiter,
						"You have more than 100 files in this folder. Only the first 100 of them have been added to the queue.", // Main message
						"", // Command Line
						"too_many_files", // Description
						"0", // sPromptToResetRouter
						"30", // sTimeout
						"1"); // sCannotGoBack
					SendCommand(SCREEN_PopupMessage);
				}
			}

			itFileName = allFilesList.begin();
			while ( itFileName != allFilesList.end() )
				dequeMediaFile.push_back(new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel, *itFileName++));
		}
		else if ( sFilename.size()>2 && sFilename[0]=='!' && (sFilename[1] == 'v' || sFilename[1] == 'V')) // Special case, means play all the voicemails of the current user.
		  {
		    string sDataGridID="Voicemail_"+sFilename.substr(2);
		    DataGridTable *pDataGridTable = m_pDatagrid_Plugin->DataGridTable_get(sDataGridID);
		    if (pDataGridTable)
		      {
			for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
			  {
			    DataGridCell *pCell = it->second;
			    const char *pValue = pCell->GetValue();
			    LoggerWrapper::GetInstance()->Write(LV_WARNING,"XXXXXX Reading Cell value of %s",pValue);
			    if ( pCell && pValue && FileUtils::FileExists(string(pValue)))
			      {
				MediaFile *pMediaFile = new MediaFile(string(pValue));
				dequeMediaFile.push_back(pMediaFile);
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXX Added %s to MediaFile Deque",pValue);
			      }
			  }
		      }
		  }
		else if( sFilename.size()>2 && sFilename[0]=='!' && (sFilename[1] == 'g' || sFilename[1] == 'G') )  // Special case, means use all the files in this grid
		{
			string sDataGridID = "MediaFile_" + sFilename.substr(2);
			DataGridTable *pDataGridTable = m_pDatagrid_Plugin->DataGridTable_get(sDataGridID);
			vector<int> vectFiles;
			vector<int> vectAttributes;

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT !G BEGIN!");

			DCE::CMD_Display_Alert pleasewait(m_dwPK_Device, pMessage->m_dwPK_Device_From,
							  string("Adding all of these files to playlist."),string("queue start"),string("5"),interuptAlways);
			SendCommand(pleasewait);

			// This is so there is always at least one entry in here, so the queries won't fail. It will not affect the overall query at all.
			// vectFiles.push_back(0);
			// vectAttributes.push_back(0);

			if( pDataGridTable )
			{
			  // This whole block of code has been rewritten, replacing the sql2cpp calls, with something
			  // far more optimized for this case. If it sucks, make it better! -tschak
			  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT DataGridTable suck begin!");
			  for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
			    {
			      // Traverse the datagrid, grabbing all the cell values, and shoving them into file or attribute vectors
			      // respectively.
			      DataGridCell *pCell = it->second;
			      const char *pValue = pCell->GetValue();
			      if ( pCell && pValue )
				{
				  if ( pValue[0]=='!' && pValue[1]=='F' )
				    {
				      int PK_File = atoi(&pValue[2]);
				      vectFiles.push_back(PK_File);
				    }
				  else if ( pValue[0]=='!' && pValue[1]=='A' )
				    {
				      int PK_Attribute = atoi(&pValue[2]);
				      vectAttributes.push_back(PK_Attribute);
				    }
				}
			    }
			 
			  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT DataGridTable suck end!");
			  
			  // At this point, we should have two vectors of files and attributes, that we can literally just plop right into
			  // an IN() set.

			  string sFiles, sAttributes;
			  stringstream ssFiles, ssAttributes;

			  if (!vectFiles.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Stringifying files.");
			      copy(vectFiles.begin(), vectFiles.end(), ostream_iterator<int>(ssFiles, ",")); // both of these have a trailing comma.
			      sFiles=ssFiles.str();
			    }

			  if (!vectAttributes.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Stringifying Attributes.");
			      copy(vectAttributes.begin(), vectAttributes.end(), ostream_iterator<int>(ssAttributes, ","));
			      sAttributes=ssAttributes.str();
			    }

			  if (!sFiles.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Making string for Files.");
			      sFiles = sFiles.substr(0, sFiles.length()-1); // Get rid of the trailing comma.
			    }
			  
			  if (!sAttributes.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Making string for attributes.");
			      sAttributes = sAttributes.substr(0, sAttributes.length()-1); // huzzah!
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Attributes are %s",sAttributes.c_str());
			    }

			  string sSQL;

			  if (!sFiles.empty() && !sAttributes.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Files AND Attributes query!");
			      sSQL = "SELECT File.PK_File, CONCAT(File.Path, '/', File.Filename), File.EK_MediaType from File, File_Attribute "
				"WHERE File.PK_File = File_Attribute.FK_File AND FK_Attribute IN (" + sAttributes  +") AND IsDirectory=0 AND Missing=0 "
				"UNION "
				"SELECT File.PK_File, CONCAT(File.Path, '/', File.Filename), EK_MediaType from File "
				"WHERE File.PK_File IN (" + sFiles + ") AND IsDirectory=0 AND Missing=0";
			    }
			  else if (!sFiles.empty() && sAttributes.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Files Query!");
			      sSQL = "SELECT File.PK_File, CONCAT(File.Path, '/', File.Filename), EK_MediaType from File "
				"WHERE File.PK_File IN (" + sFiles + ") AND IsDirectory=0 AND Missing=0";
			    }
			  else if (sFiles.empty() && !sAttributes.empty())
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Attributes Query!");
			      sSQL = "SELECT File.PK_File, CONCAT(File.Path, '/', File.Filename), File.EK_MediaType from File, File_Attribute "
				"WHERE File.PK_File = File_Attribute.FK_File AND FK_Attribute IN (" + sAttributes  +") AND IsDirectory=0 AND Missing=0 ";
			    }
			  else
			    {
			      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Oops! This should never have happened!");
			    }
			    
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Query is %s",sSQL.c_str());

			  PlutoSqlResult result;
			  DB_ROW row;

			  if (!sSQL.empty())
			    {
			  
			      if ( (result.r = m_pDatabase_pluto_media->db_wrapper_query_result(sSQL)) )
				{
				  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT populate dequeMediaFile begin");
				  while ( (row = db_wrapper_fetch_row( result.r )) )
				    {
				      int PK_File = atoi(row[0]);
				      string sFilename = row[1];
				      int PK_MediaType = atoi(row[2]);
				      MediaFile *pMediaFile = new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel, PK_File, sFilename);
				      pMediaFile->m_dwPK_MediaType=PK_MediaType;
				      dequeMediaFile.push_back(pMediaFile);
				    }
				  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT populate dequeMediaFile end");
				}
			      
			    }

			}
			if( dequeMediaFile.empty() )
			  return; // Nothing to do; the grid was empty
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT !G END!");
		}
		else
			TransformFilenameToDeque(sFilename, dequeMediaFile);  // This will convert any !A, !F, !B etc.
 	}

	int iPK_MediaProvider=0;
	string sStartPosition;
	// See if it's a bookmark to a channel or something
	if( !iPK_MediaType && dequeMediaFile.size()==0 && sFilename.size() && sFilename[0]=='!' && sFilename[1]=='B' )
	{
		Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow(atoi(sFilename.substr(2).c_str()));
		if( pRow_Bookmark )
		{
			sStartPosition = pRow_Bookmark->Position_get();
			iPK_MediaType = pRow_Bookmark->EK_MediaType_get();
			iPK_MediaProvider = pRow_Bookmark->FK_MediaProvider_get();
		}
	}

	// What is the media?  It must be a Device, DeviceTemplate, or a media type, or filename
	// If there's a filename, we'll use that to determine the media type
    if( !iPK_MediaType && (pMediaDevice_Source || iPK_DeviceTemplate) && !sFilename.size() )
    {
		vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
		Row_DeviceTemplate *pRow_DeviceTemplate=NULL;
		if( pMediaDevice_Source )
			pRow_DeviceTemplate = pMediaDevice_Source->m_pDeviceData_Router->m_pRow_Device->FK_DeviceTemplate_getrow();

		if( !pRow_DeviceTemplate && iPK_DeviceTemplate )
			pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);

		if( pRow_DeviceTemplate )
			pRow_DeviceTemplate->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);

		if( vectRow_DeviceTemplate_MediaType.size()!=1 )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Cannot decisively determine media type for %d/%d, rows: %d",
				iPK_Device,iPK_DeviceTemplate,(int) vectRow_DeviceTemplate_MediaType.size());

		if( vectRow_DeviceTemplate_MediaType.size() )
		{
			Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[0];
			iPK_MediaType = pRow_DeviceTemplate_MediaType->FK_MediaType_get();
		}
    }

    if( vectEntertainArea.size()==1 &&
		( pMediaDevice_Source && pMediaDevice_Source->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Disc_Drives_CONST) )
		 || (dequeMediaFile.size()==0 && iPK_MediaType==0) ) // We got nothing -- find a disk drive within the entertainment area and send it a reset
    {
		EntertainArea *pEntertainArea = vectEntertainArea[0];

		if( !pMediaDevice_Source )
		{
			for(map<int,class MediaDevice *>::iterator itDevice=pEntertainArea->m_mapMediaDevice.begin();itDevice!=pEntertainArea->m_mapMediaDevice.end();++itDevice)
			{
				class MediaDevice *pMediaDevice = (*itDevice).second;
				if( pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceCategory==DEVICECATEGORY_Disc_Drives_CONST )
				{
					pMediaDevice_Source = pMediaDevice;
					break;
				}
			}
		}

		if( pMediaDevice_Source && DiskDriveIsRipping(pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device) )
		{
			SCREEN_DialogRippingInProgress SCREEN_DialogRippingInProgress(m_dwPK_Device, pMessage->m_dwPK_Device_From,
				StringUtils::itos(pMessage->m_dwPK_Device_From), StringUtils::itos(pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device));
			SendCommand(SCREEN_DialogRippingInProgress);
			return;
		}

		if( !pMediaDevice_Source )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find source device in EA %d",pEntertainArea->m_iPK_EntertainArea);
			SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device, pMessage->m_dwPK_Device_From, "");
			SendCommand(SCREEN_DialogCannotPlayMedia);
			return;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Final Sort!");
	//sort them by album, track, filename
	std::sort(dequeMediaFile.begin(), dequeMediaFile.end(), MediaFileComparer);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TTTT Final Sort End!");

	if (bQueue)
	  {
	    size_t iNumFiles = dequeMediaFile.size();
	    DCE::CMD_Display_Alert CMD_Display_Alert(m_dwPK_Device, pMessage->m_dwPK_Device_From,
						     StringUtils::itos(iNumFiles) + " file(s) added to queue.","queue added","5",interuptAlways);
	    SendCommand(CMD_Display_Alert);
	  }

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_MH_Play_Media playing MediaType: %d Provider %d Orbiter %d Device %d Template %d",
		iPK_MediaType,iPK_MediaProvider,iPK_Device_Orbiter,iPK_Device,iPK_DeviceTemplate);
	StartMedia(iPK_MediaType,iPK_MediaProvider,iPK_Device_Orbiter,vectEntertainArea,iPK_Device,iPK_DeviceTemplate,&dequeMediaFile,bQueue,bResume,iRepeat,"", bBypass_Event);  // We'll let the plug-in figure out the source, and we'll use the default remote
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** This will allow an orbiter to change the current playing position in the playlist */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Media_Plugin::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    int iPK_Device_Orbiter = pMessage->m_dwPK_Device_From;

	vector<EntertainArea *> vectEntertainArea;
    // Only an Orbiter will tell us to jump to next position
    DetermineEntArea( iPK_Device_Orbiter, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()!=1 )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Got jump position but i couldn't find an entertainment area for it!");
        return;
    }

	EntertainArea *pEntertainArea = vectEntertainArea[0];

    if ( pEntertainArea->m_pMediaStream == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Can't jump in an empty queue");
        return;
    }

	pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST] = StringUtils::itos(pEntertainArea->m_pMediaStream->m_iStreamID_get());

	if( pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->m_pData->m_dwPK_Device!=m_dwPK_Device )
	{
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->m_pData->m_dwPK_Device);
		if( pDeviceData_Router && pDeviceData_Router->m_mapCommands.find(pMessage->m_dwID)!=pDeviceData_Router->m_mapCommands.end() )
		{
			// The plugin has it's own method for handling this.  Give it the message instead
			pMessage->m_mapParameters[COMMANDPARAMETER_PK_Device_CONST] = StringUtils::itos(pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
			pMessage->m_dwPK_Device_To=pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->m_dwPK_Device;
			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->ReceivedMessage(pMessage);
			return;
		}
	}

	if( sValue_To_Assign.find("CHAPTER:")!=string::npos || sValue_To_Assign.find("POS:")!=string::npos )
	{
		DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_dwPK_Device,pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
			pEntertainArea->m_pMediaStream->m_iStreamID_get(),sValue_To_Assign);
		SendCommand(CMD_Set_Media_Position);
		return;
	}

	// If we don't have multiple files in the queue, this is treated as a skip forward/back to jump through chapters.
	// Ditto for generic media devices
	if( pEntertainArea->m_pMediaStream->m_dequeMediaFile.size()<2 || pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo )
	{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "It's a generic stream--just forward it to the device");
        pMessage->m_dwPK_Device_To = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_To);
		// Generic IR devices generally don't know how to process a jump command, and it should be converted to a 'skip'
		if( pDeviceData_Router && pDeviceData_Router->m_mapCommands.find( pMessage->m_dwID ) == pDeviceData_Router->m_mapCommands.end() )
			pMessage->m_dwID = sValue_To_Assign.size() && sValue_To_Assign[0]=='-' ? COMMAND_Skip_Back_ChannelTrack_Lower_CONST : COMMAND_Skip_Fwd_ChannelTrack_Greater_CONST;
	    Message *pNewMessage = new Message( pMessage );
		QueueMessageToRouter( pNewMessage );
		return;
	}

	// update the orbiter object to be the one that commanded this.
	pEntertainArea->m_pMediaStream->m_pOH_Orbiter_StartedMedia = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( iPK_Device_Orbiter );

	// Made the Jump to position to be the responsibility of hte Media stream. Most of them will just inherit the previous behaviour
	// +1 next in playlist
	// -1 prev in playlist
	// number select that channel
	//
	// but a few specific ones (like the MythTV MediaStream need to so special handling here)
	if ( pEntertainArea->m_pMediaStream->ProcessJumpPosition(sValue_To_Assign) ) // if the specification was valid for this stream then continue processing in the media plugin
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from JumpPos (handler)");
		string sError;
    	pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pEntertainArea->m_pMediaStream,sError);
		CheckStreamForTimeout(pEntertainArea->m_pMediaStream);
	}

	MediaInfoChanged(pEntertainArea->m_pMediaStream,true);  // Refresh the screen and re-draw the grid
//  StartMediaByPositionInPlaylist(pEntertainArea, pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos, 0, 0);
}
//<-dceag-c214-b->

	/** @brief COMMAND: #214 - Save playlist */
	/** This will instruct the device to save the currently playing list */
		/** @param #17 PK_Users */
			/** The user that will own the new playlist. Can be missing. It will pick the current user then. */
		/** @param #45 PK_EntertainArea */
			/** Which playlist to save. You can direct the command to save a specific entertainment area's playlist or you can leave it blank to pick the current entertainment area's playlist */
		/** @param #50 Name */
			/** It will use the this name when saving. If it is not specified it will either use the name of the loaded playlist in the database or it will generate a new one. */
		/** @param #77 Save as new */
			/** Save the playlist as a new playlist. This will override the default behaviour. (If this playlist was not loaded from the database it will be saved as new. If it was loaded it will be overridded). This will make it always save it as new. */

void Media_Plugin::CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,Message *pMessage)
//<-dceag-c214-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Find the corrent entertainment area to use.
	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea );

    if ( vectEntertainArea.size()!=1 || vectEntertainArea[0]->m_pMediaStream == NULL)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't find a valid entertainment area with a valid stream in it. Can't save the playlist");
        return;
    }
LoggerWrapper::GetInstance()->Write(LV_WARNING, "pl1 = %s",sName.c_str());

	EntertainArea *pEntertainArea = vectEntertainArea[0];
    if ( sName == "" )
        sName = pEntertainArea->m_pMediaStream->m_sPlaylistName;

    if ( sName == "" )
        sName = "New Playlist";

LoggerWrapper::GetInstance()->Write(LV_WARNING, "pl2 = %s",sName.c_str());
    int iPK_Playlist = 0;
	if( !bSave_as_new && pEntertainArea->m_pMediaStream->m_iPK_Playlist &&
			(sName.size()==0 || pEntertainArea->m_pMediaStream->m_sPlaylistName==sName) )
		iPK_Playlist = pEntertainArea->m_pMediaStream->m_iPK_Playlist;  // Save as a new playlist if the user specified a new save to name

	if( !m_pMediaAttributes->SavePlaylist(pEntertainArea->m_pMediaStream->m_dequeMediaFile, iPK_Users, iPK_Playlist, sName) )
    {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to save playlist");
        //m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"Unable to save playlist",false,10,true);
		SCREEN_DialogUnableToSavePlaylist SCREEN_DialogUnableToSavePlaylist(m_dwPK_Device, pMessage->m_dwPK_Device_From);
		SendCommand(SCREEN_DialogUnableToSavePlaylist);
        return;
    }

    pEntertainArea->m_pMediaStream->m_iPK_Playlist = iPK_Playlist;
    pEntertainArea->m_pMediaStream->m_sPlaylistName = sName;
	//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(StringUtils::itos(pMessage->m_dwPK_Device_From),
	//	"<%=T" + StringUtils::itos(TEXT_Playlist_Saved_CONST) + "%>");
	SCREEN_DialogPlaylistSaved SCREEN_DialogPlaylistSaved(m_dwPK_Device, pMessage->m_dwPK_Device_From);
	SendCommand(SCREEN_DialogPlaylistSaved);
}
//<-dceag-c231-b->

	/** @brief COMMAND: #231 - Load Playlist */
	/** This will instruct the device to load the specific playlist. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area in which to load the  playlist. By defualt it will be the entertainment in which the current orbiter is running. */
		/** @param #78 EK_Playlist */
			/** The id of the playlist to load */

void Media_Plugin::CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,Message *pMessage)
//<-dceag-c231-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    // Find the corrent entertainment area to use.
	vector<EntertainArea *> vectEntertainArea;

	DetermineEntArea( pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea );
    if ( vectEntertainArea.size()==0 )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't find a valid entertainment area to load the playlist into.");
        return;
    }

    string sPlaylistName;
    deque<MediaFile *> dequeMediaFile;
    if( !m_pMediaAttributes->LoadPlaylist( iEK_Playlist, dequeMediaFile, sPlaylistName) || dequeMediaFile.size()==0 )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "I was unable to load playlist entries");
        //m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"Unable to load playlist",false,10,true);
		SCREEN_DialogUnableToLoadPlaylist SCREEN_DialogUnableToLoadPlaylist(m_dwPK_Device, pMessage->m_dwPK_Device_From);
		SendCommand(SCREEN_DialogUnableToLoadPlaylist);
        return;
    }

    string Extension = StringUtils::ToUpper(FileUtils::FindExtension(dequeMediaFile[0]->m_sFilename));
	map<int,MediaHandlerInfo *> mapMediaHandlerInfo;

	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea=vectEntertainArea[s];

		List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_Extension_Find(Extension);
		if( !pList_MediaHandlerInfo || pList_MediaHandlerInfo->size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Play playlist %s in entertain area %s but nothing to handle it",sPlaylistName.c_str(),sPK_EntertainArea.c_str());
			return;
		}
		MediaHandlerInfo *pMediaHandlerInfo = pList_MediaHandlerInfo->front();
		if( pMediaHandlerInfo )
			mapMediaHandlerInfo[pMediaHandlerInfo->m_MediaHandlerID]=pMediaHandlerInfo;
	}
	for(map<int,MediaHandlerInfo *>::iterator it=mapMediaHandlerInfo.begin();it!=mapMediaHandlerInfo.end();++it)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from Load Playlist");
		MediaStream *pMyStream = StartMedia(it->second,0,pMessage->m_dwPK_Device_From,vectEntertainArea,0,&dequeMediaFile,false,false,0,"",iEK_Playlist);  // We'll let the plug-in figure out the source, and we'll use the default remote
		// delete pMyStream; // looks like a leak, but apparently we don't own this pointer over here - all kinds of things start crashing if we remove this
	}
}

void Media_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
	if( pEntertainArea && pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->m_iOrder>=0 && pEntertainArea->m_pMediaStream->m_iOrder<MAX_MEDIA_COLORS )
	{
		Color = UniqueColors[pEntertainArea->m_pMediaStream->m_iOrder];
		sDescription = pEntertainArea->m_pMediaStream->m_sMediaDescription;
	}
	else
	{
		Color = PlutoColor::Black().m_Value;
		sDescription = "off";
	}
}
//<-dceag-c241-b->

	/** @brief COMMAND: #241 - MH Move Media */
	/** Moves an existing media stream to the specified entertainment areas. */
		/** @param #41 StreamID */
			/** The ID of the media stream to move */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area(s) to send the Media Stream to. */

void Media_Plugin::CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c241-e->
{
	MediaStream *pMediaStream;

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	if ( (pMediaStream = m_mapMediaStream_Find(iStreamID,pMessage ? pMessage->m_dwPK_Device_From : 0)) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CMD_MH_Move_Media No media stream with ID %d available", iStreamID );
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_MH_Move_Media Move Media, stream %d  ea: %s",iStreamID,sPK_EntertainArea.c_str());
	// Note: The entertainment area is probably a negative number, because orbiter does thata to differentiate between devices and EA

	vector<EntertainArea *> vectEntertainArea;
	if( sPK_EntertainArea.size() )  // If it's empty, we're just parking the media
	    DetermineEntArea(0,0,sPK_EntertainArea,vectEntertainArea);

	bool bNothingMoreToPlay = vectEntertainArea.size()==0;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Calling StopMedia parked %d vectea: %d", (int) pMediaStream->m_tTime_Parked, (int) vectEntertainArea.size());

	if( pMediaStream->m_tTime_Parked==0 )
	{
		// Don't bother stopping the media if it's already parked.  This media is not parked
		pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pMediaStream );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_MH_Move_Media Called StopMedia" );
		StreamEnded(pMediaStream,true,false,NULL,&vectEntertainArea,false,false);  // In the case of a move the user likely doesn't want to still use this system, so the final false means dont leave the osd on
	}
	else if( vectEntertainArea.empty() )
	{
		// If the user parked the stream, and is still moving it to no where, probably he just wants it to go away
		StreamEnded(pMediaStream,true,true,NULL,&vectEntertainArea,false,false);  // In the case of a move the user likely doesn't want to still use this system, so the final false means dont leave the osd on
		return;
	}

	if( bNothingMoreToPlay )
	{
		pMediaStream->m_tTime_Parked = time(NULL);
		pMediaStream->m_mapEntertainArea.clear();
	}
	else
	{
LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_MH_Move_Media ready to restart %d eas",(int) vectEntertainArea.size());
		pMediaStream->m_tTime_Parked = 0;
		// Be sure all outgoing stop messages are flushed before we proceed
		WaitForMessageQueue();
		pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->WaitForMessageQueue();

		if( pMediaStream->m_dequeMediaFile.size()>1 )
			pMediaStream->m_sLastPosition += " QUEUE_POS:" + StringUtils::itos(pMediaStream->m_iDequeMediaFile_Pos);

		// Preserve the source device so when moving media on generic media devices we keep using the same source
		// If moving media on software media players, we will find another source that can resume the media
		int PK_Device = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
		// Is the source device also a target device
		int i = 0;
		MediaDevice* pSourceMediaDevice = NULL;
		while (pSourceMediaDevice == NULL && i < vectEntertainArea.size())
		{
			pSourceMediaDevice = vectEntertainArea[i]->m_mapMediaDevice_Find(PK_Device);
			i++;
		}
		if (pSourceMediaDevice == NULL)
		{
			// Source device not in destination, let StartMedia figure out where to play it
			PK_Device = 0;
		}
		StartMedia( pMediaStream->m_iPK_MediaType, pMediaStream->m_iPK_MediaProvider, (pMediaStream->m_pOH_Orbiter_StartedMedia ? pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device : 0),
			vectEntertainArea, PK_Device, 0,
			    &pMediaStream->m_dequeMediaFile, false, pMediaStream->m_bResume, pMediaStream->m_iRepeat,pMediaStream->m_sLastPosition, false);

		pMediaStream->m_dequeMediaFile.clear();  // We don't want to delete the media files since we will have re-used the same pointers above
		delete pMediaStream; // We will have started with new copies
	}
}

bool Media_Plugin::CheckForAlternatePipes(MediaStream *pMediaStream,EntertainArea *pEntertainArea)
{
	if( !pEntertainArea->m_pMediaDevice_ActiveDest )
	{
		DeviceData_Router *pDevice_Dest=NULL;
		bool bResult = CheckForAlternatePipes(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router,pDevice_Dest,pEntertainArea);
		if( pDevice_Dest )
		{
			MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pDevice_Dest->m_dwPK_Device );
			if( !pMediaDevice )
				return false;

			if( pDevice_Dest->WithinCategory(DEVICECATEGORY_Output_Zone_CONST)==false && pMediaStream->m_pMediaDevice_Source->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaStream->m_pMediaDevice_Source->m_mapEntertainArea.end() )
			{
				// The destination is in the same EA as the source, so there's no output zone involved
				pEntertainArea->m_pMediaDevice_ActiveDest = m_mapMediaDevice_Find( pDevice_Dest->m_dwPK_Device );
			}
			else
			{
				// This is likely a GSD device with an output zone, so the active destination is the source, and the output zone is this path
				pEntertainArea->m_pMediaDevice_ActiveDest = pMediaStream->m_pMediaDevice_Source;
				pEntertainArea->m_pMediaDevice_OutputZone = m_mapMediaDevice_Find( pDevice_Dest->m_dwPK_Device );
			}
		}
		return bResult;
	}
	else if( !pEntertainArea->m_pMediaDevice_OutputZone  )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CheckForAlternatePipes no destination for EA %d %s",
			pEntertainArea->m_iPK_EntertainArea,pEntertainArea->m_sDescription.c_str());
		return false;
	}

	// If this is a software media player, the md it's on is the source
	MediaDevice *pMediaDevice=NULL;
	if( pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_DCE_Software_Wrappers_CONST) )
		pMediaDevice = m_mapMediaDevice_Find( pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD );
	else
		pMediaDevice = pEntertainArea->m_pMediaDevice_ActiveDest;

	if( pMediaDevice && pMediaDevice->m_pDeviceData_Router->m_mapPipe_Temporary.empty() )
		return CheckForAlternatePipes(pMediaDevice->m_pDeviceData_Router,pEntertainArea->m_pMediaDevice_OutputZone->m_pDeviceData_Router,pEntertainArea);
	return false;
}

// Recursively check a particular device
bool Media_Plugin::CheckForAlternatePipes(DeviceData_Router *pDevice_From,DeviceData_Router *&pDevice_To,EntertainArea *pEntertainArea)
{
	// We've got to find a way to get from the source to the dest
	for(MapPipe::iterator it=pDevice_From->m_mapPipe_Available.begin();
		it!=pDevice_From->m_mapPipe_Available.end();++it)
	{
		Pipe *pPipe = it->second;
		MediaDevice *pMediaDevice_Pipe_To = m_mapMediaDevice_Find(pPipe->m_pDevice_To->m_dwPK_Device);
		if( (pDevice_To && pPipe->m_pDevice_To->m_dwPK_Device==pDevice_To->m_dwPK_Device) || (!pDevice_To && pMediaDevice_Pipe_To && pMediaDevice_Pipe_To->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaDevice_Pipe_To->m_mapEntertainArea.end()) )
		{
			if( !pDevice_To )
				pDevice_To = pPipe->m_pDevice_To;
			// Got our route
			AddAlternativeRoute(pDevice_From,pPipe->m_pDevice_To,pPipe,pEntertainArea);
			return true;
		}
		else if( CheckForAlternatePipes(pPipe->m_pDevice_To,pDevice_To,pEntertainArea) )
		{
			AddAlternativeRoute(pDevice_From,pPipe->m_pDevice_To,pPipe,pEntertainArea);
			return true;
		}

		if( pMediaDevice_Pipe_To )
		{
			for(map<int,MediaDevice *>::iterator it=pMediaDevice_Pipe_To->m_mapOutputZone.begin();it!=pMediaDevice_Pipe_To->m_mapOutputZone.end();++it)
			{
				MediaDevice *pMediaDevice_To = it->second;
				if( (pDevice_To && pMediaDevice_To->m_pDeviceData_Router->m_dwPK_Device==pDevice_To->m_dwPK_Device) || (!pDevice_To && pMediaDevice_To->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaDevice_To->m_mapEntertainArea.end()) )
				{
					if( !pDevice_To )
						pDevice_To = pMediaDevice_To->m_pDeviceData_Router;
					AddAlternativeRoute(pDevice_From,pMediaDevice_To->m_pDeviceData_Router,pPipe,pEntertainArea);
					return true;
				}
			}
		}
	}
	return false;
}

void Media_Plugin::AddAlternativeRoute(DeviceData_Router *pDevice_From,DeviceData_Router *&pDevice_To,Pipe *pPipe,EntertainArea *pEntertainArea)
{
	Pipe *pPipe_New = new Pipe(pDevice_From,pDevice_To,pPipe->m_PK_Pipe,pPipe->m_pCommand_Input,pPipe->m_pCommand_Output,true);
	MapPipe *pMapPipe;
	map<int,MapPipe *>::iterator it=pDevice_From->m_mapPipe_Temporary.find( pEntertainArea->m_iPK_EntertainArea );
	if( it==pDevice_From->m_mapPipe_Temporary.end() )
	{
		pMapPipe = new MapPipe();
		pDevice_From->m_mapPipe_Temporary[ pEntertainArea->m_iPK_EntertainArea ] = pMapPipe;
	}
	else
		pMapPipe = it->second;

	(*pMapPipe)[ pPipe_New->m_PK_Pipe ] = pPipe_New;
}

void Media_Plugin::RecursivelyAddSendingDevice(MediaDevice *pMediaDevice_FeedInto,MediaDevice *pMediaDevice_FeedFrom)
{
	pMediaDevice_FeedInto->m_mapMediaDevice_FedInto_OutsideZone[ pMediaDevice_FeedFrom->m_pDeviceData_Router->m_dwPK_Device ] = pMediaDevice_FeedFrom;

	// If we've got a media director feeding in, any software media players in that ea are also feeding in via the md
	if( pMediaDevice_FeedFrom->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Media_Director_CONST) )
	{
		for(map<int,EntertainArea *>::iterator it=pMediaDevice_FeedFrom->m_mapEntertainArea.begin();it!=pMediaDevice_FeedFrom->m_mapEntertainArea.end();++it)
		{
			EntertainArea *pEntertainArea = it->second;
			for(map<int,MediaDevice *>::iterator itMD = pEntertainArea->m_mapMediaDevice.begin();itMD != pEntertainArea->m_mapMediaDevice.end();++itMD)
			{
				MediaDevice *pMediaDevice = itMD->second;
				if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Media_Players_CONST) )
					pMediaDevice_FeedInto->m_mapMediaDevice_FedInto_OutsideZone[ pMediaDevice->m_pDeviceData_Router->m_dwPK_Device ] = pMediaDevice;
			}
		}
	}
	for(vector<DeviceData_Router *>::iterator it=pMediaDevice_FeedFrom->m_pDeviceData_Router->m_vectDevices_SendingPipes.begin();it!=pMediaDevice_FeedFrom->m_pDeviceData_Router->m_vectDevices_SendingPipes.end();++it)
	{
		DeviceData_Router *pDevice = *it;
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDevice->m_dwPK_Device);
		if( pMediaDevice )
			RecursivelyAddSendingDevice(pMediaDevice_FeedInto,pMediaDevice);
	}
}

void Media_Plugin::HandleOnOffs(int PK_MediaType_Prior,int PK_MediaType_Current, map<int,MediaDevice *> *pmapMediaDevice_Prior,map<int,MediaDevice *> *pmapMediaDevice_Current,MediaStream *pMediaStream,EntertainArea *pEntertainArea)
{
	// Is a specific pipe used?  If this is an audio stream only, the media type will have the pipe set to 1
	Row_MediaType *pRow_MediaType_Prior = PK_MediaType_Prior ? m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType_Prior) : NULL;
	Row_MediaType *pRow_MediaType_Current = PK_MediaType_Current ? m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType_Current) : NULL;

	int PK_Pipe_Prior = pRow_MediaType_Prior && pRow_MediaType_Prior->FK_Pipe_isNull()==false ? pRow_MediaType_Prior->FK_Pipe_get() : 0;

	int PK_Pipe_Current = pRow_MediaType_Current && pRow_MediaType_Current->FK_Pipe_isNull()==false ? pRow_MediaType_Current->FK_Pipe_get() : 0;

	map<int,MediaDevice *> mapMediaDevice_Current;
	// We don't want just the top-level devices, we want all the devices in the 'current' map so we don't
	// turn off something we're going to turn on again.  But we don't want to add them to pmapMediaDevice_Current
	// and then have to handle the pipes ourselves.  But this only matters if there are prior devices to turn off
	if( pmapMediaDevice_Prior && pmapMediaDevice_Prior->size() && pmapMediaDevice_Current && pmapMediaDevice_Current->size() )
	{
		mapMediaDevice_Current = *pmapMediaDevice_Current;
		AddOtherDevicesInPipesToRenderDevices(PK_Pipe_Current,&mapMediaDevice_Current,pEntertainArea);
	}

	if( pmapMediaDevice_Prior )
	{
		// If we watching a DVD, and the pipe was from the dvd player to a video scaler to the tv, and we are now watching
		// TV with the path from the media director to the tv, we want to turn off the scaler, but not the tv.  We will just
		// send the media director an on and let the framework propagate the input selection automatically.  However, we don't
		// want to just send the DVD player an off because then the framework will turn the tv off too.
		for(map<int,MediaDevice *>::iterator it=pmapMediaDevice_Prior->begin();it!=pmapMediaDevice_Prior->end();++it)
		{
			// We need a recursive function to propagate the off's along the pipe, but not turning off any devices
			// that we're still going to use in the current map
			if ( (*it).second )
				TurnDeviceOff(PK_Pipe_Prior,(*it).second->m_pDeviceData_Router,&mapMediaDevice_Current,pEntertainArea);
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::HandleOnOffs Found a NULL associated device in the HandleOnOff: %d", (*it).first);
			}
		}
	}

	if( !pmapMediaDevice_Current || !pmapMediaDevice_Current->size() )
		return; // Nothing to turn on -- we already turned everything off

	for(map<int,MediaDevice *>::iterator it=pmapMediaDevice_Current->begin();it!=pmapMediaDevice_Current->end();++it)
	{
		MediaDevice *pMediaDevice = (*it).second;

		if ( pMediaDevice == NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::HandleOnOffs() There is a null device associated with the deviceID: %d. Ignoring device in HandleOnOff", (*it).first);
			continue;
		}

		// Reset this
		pEntertainArea->m_bViewingLiveAVPath=false;
		for(map<int,Pipe *>::iterator it=pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.begin();it!=pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.end();++it)
		{
			it->second->m_bDontSendInputs=false;  // Reset this in case the device was previously in use and had this set to true
			it->second->m_bDontSendOn=false;  // Reset this in case the device was previously in use and had this set to true
		}

		// If this is using a capture card and it's active, then we are only going to turn on the m/d
		if( pMediaDevice->m_pDevice_CaptureCard && pMediaDevice->m_bCaptureCardActive && pMediaDevice->m_pDevice_CaptureCard )
		{
			// See if we're viewing it in the same room where the device is located.  If so the user may want to have live and non-live paths
			bool bViewingInRoom=false;

			MediaDevice *pMediaDevice_CaptureCard = m_mapMediaDevice_Find(pMediaDevice->m_pDevice_CaptureCard->m_dwPK_Device);
			if( pMediaDevice_CaptureCard )
			{
				for( map<int, class EntertainArea *>::iterator it=pMediaStream->m_mapEntertainArea.begin();
					it!=pMediaStream->m_mapEntertainArea.end();++it)
				{
					EntertainArea *pEntertainArea = it->second;
					if( pMediaDevice_CaptureCard->m_mapEntertainArea.find(pEntertainArea->m_iPK_EntertainArea)!=pMediaDevice_CaptureCard->m_mapEntertainArea.end() )
					{
						bViewingInRoom=true;
						break;
					}
				}
			}

			// We don't want to be setting the inputs to the 'live' a/v path because we're using the capture card
			for(map<int,Pipe *>::iterator it=pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.begin();it!=pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.end();++it)
			{
				it->second->m_bDontSendInputs=true;
				if( !bViewingInRoom )
					it->second->m_bDontSendOn=true;  // Additionally, if we're not viewing it in the room the device is in, don't send the live paths either
			}

			for( map<int, class EntertainArea *>::iterator it=pMediaStream->m_mapEntertainArea.begin();
				it!=pMediaStream->m_mapEntertainArea.end();++it)
			{
				EntertainArea *pEntertainArea = it->second;
				MediaDevice *pMediaDevice_MD = pEntertainArea->m_pMediaDevice_MD;

				if( pMediaDevice_MD  )
				{
					if( pMediaDevice_MD->m_dwPK_Command_LastPower==COMMAND_Generic_On_CONST && time(NULL)-pMediaDevice_MD->m_tLastPowerCommand < DONT_RESEND_POWER_WITHIN_X_SECONDS && pMediaDevice_MD->m_dwPK_EntertainArea_LastPower==pEntertainArea->m_iPK_EntertainArea )
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::HandleOnOffs Not resending power command (2)");
					else
					{
						if( pMediaDevice_MD )
						{
							pMediaDevice_MD->m_tLastPowerCommand=time(NULL);
							pMediaDevice_MD->m_dwPK_Command_LastPower=COMMAND_Generic_On_CONST;
							pMediaDevice_MD->m_dwPK_EntertainArea_LastPower=pEntertainArea->m_iPK_EntertainArea;
						}

						// Send it to the MD
						DCE::CMD_On CMD_On(m_dwPK_Device,pMediaDevice_MD->m_pDeviceData_Router->m_pDevice_MD->m_dwPK_Device,PK_Pipe_Current,"");
						CheckForCustomPipe(pEntertainArea,CMD_On.m_pMessage);
						SendCommand(CMD_On);
					}
				}
			}
		}
		// If this is on a media director and it's a child of the OSD, then turn the media director on
		// Also turn on the media director if this is an audio only device to prevent the media director
		// from powering on later and switching inputs on pipes away from the device.
		else if( pMediaDevice->m_pDeviceData_Router->m_pDevice_MD &&
				pMediaDevice->m_pDeviceData_Router!=pMediaDevice->m_pDeviceData_Router->m_pDevice_MD &&
				( (
				     pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia &&
				     pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia->m_dwPK_DeviceTemplate==DEVICETEMPLATE_OnScreen_Orbiter_CONST
                                  ) || PK_Pipe_Current == PIPE_Audio_CONST
				)
		       )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::HandleOnOffs Also turning on MD and OSD");
#endif

			MediaDevice *pMediaDevice_MD = m_mapMediaDevice_Find(pMediaDevice->m_pDeviceData_Router->m_pDevice_MD->m_dwPK_Device);

			if( pMediaDevice_MD && pMediaDevice_MD->m_dwPK_Command_LastPower==COMMAND_Generic_On_CONST && time(NULL)-pMediaDevice_MD->m_tLastPowerCommand < DONT_RESEND_POWER_WITHIN_X_SECONDS && pMediaDevice_MD->m_dwPK_EntertainArea_LastPower==pEntertainArea->m_iPK_EntertainArea )
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::HandleOnOffs Not resending power command");
			else
			{
				if( pMediaDevice_MD )
				{
					pMediaDevice_MD->m_tLastPowerCommand=time(NULL);
					pMediaDevice_MD->m_dwPK_Command_LastPower=COMMAND_Generic_On_CONST;
					pMediaDevice_MD->m_dwPK_EntertainArea_LastPower=pEntertainArea->m_iPK_EntertainArea;
				}

				// Send it to the MD
				DCE::CMD_On CMD_On(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_pDevice_MD->m_dwPK_Device,PK_Pipe_Current,"");
				CheckForCustomPipe(pEntertainArea,CMD_On.m_pMessage);
				SendCommand(CMD_On);
			}

			// Send it to the on screen orbiter
			DCE::CMD_On CMD_On2(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_pDevice_ControlledVia->m_dwPK_Device,PK_Pipe_Current,"");
			CheckForCustomPipe(pEntertainArea,CMD_On2.m_pMessage);
			SendCommand(CMD_On2);
		}

		// See if it's a generic media stream, and it's using it's own pipes
		if( pMediaStream->m_pMediaHandlerInfo==m_pGenericMediaHandlerInfo &&
		    pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.size() &&
		    pEntertainArea->m_pMediaDevice_MD->m_pDeviceData_Router->m_dwPK_Device != pMediaDevice->m_pDeviceData_Router->m_dwPK_Device // It isn't LiveAV if the MediaDevice is the MD
		  )
			pEntertainArea->m_bViewingLiveAVPath=true;

		DCE::CMD_On CMD_On(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,PK_Pipe_Current,"");
		CheckForCustomPipe(pEntertainArea,CMD_On.m_pMessage);
		SendCommand(CMD_On);
	}
}

void Media_Plugin::TurnDeviceOff(int PK_Pipe,DeviceData_Router *pDeviceData_Router,map<int,MediaDevice *> *pmapMediaDevice_Current,EntertainArea *pEntertainArea,vector<int> *p_vectDevice)
{
	bool bCreatedVect=false;
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDeviceData_Router->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDeviceData_Router->m_dwPK_Device);

	// If there are no current devices, or there are but this one isn't in the list and this isn't an embedded device for one in the list
	if( !pmapMediaDevice_Current ||
		(pmapMediaDevice_Current->find( pDeviceData_Router->m_dwPK_Device ) == pmapMediaDevice_Current->end()
		&& (pDeviceData_Router->m_pDevice_RouteTo==NULL ||
			pmapMediaDevice_Current->find( pDeviceData_Router->m_pDevice_RouteTo->m_dwPK_Device ) == pmapMediaDevice_Current->end()) ) )
	{
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDeviceData_Router->m_dwPK_Device);

		// Don't turn the device off the OSD needs it on
		if( !pMediaDevice || !pMediaDevice->m_bDontSendOffIfOSD_ON )
		{
			DCE::CMD_Off CMD_Off(m_dwPK_Device,pDeviceData_Router->m_dwPK_Device,-1);  // -1 means don't propagate to any pipes
			CheckForCustomPipe(pEntertainArea,CMD_Off.m_pMessage);
			SendCommand(CMD_Off);
		}
	}

//  AB dec 3, 2005 -- This meant that i was watching a vcr and switched to the sat box, since the vcr
// has m_pDevice_MD it would shut the m/d off.
//	if( pDeviceData_Router->m_pDevice_MD && pDeviceData_Router!=pDeviceData_Router->m_pDevice_MD )
//		TurnDeviceOff(PK_Pipe,(DeviceData_Router *) pDeviceData_Router->m_pDevice_MD,pmapMediaDevice_Current,p_vectDevice);

    for(map<int,Pipe *>::iterator it=pDeviceData_Router->m_mapPipe_Available.begin();it!=pDeviceData_Router->m_mapPipe_Available.end();++it)
    {
        Pipe *pPipe = (*it).second;
		if( (PK_Pipe && pPipe->m_PK_Pipe!=PK_Pipe) || pPipe->m_bDontSendOff )
			continue;

		DeviceData_Router *pDeviceData_RouterChild = m_pRouter->m_mapDeviceData_Router_Find( pPipe->m_pDevice_To->m_dwPK_Device );
		if( pDeviceData_RouterChild )
			TurnDeviceOff(PK_Pipe,pDeviceData_RouterChild,pmapMediaDevice_Current,pEntertainArea,p_vectDevice);
	}

	if( bCreatedVect )
		delete p_vectDevice;
}
//<-dceag-createinst-b->!

//<-dceag-c269-b->

	/** @brief COMMAND: #269 - Move Playlist entry Up */
	/** Moves a entry up in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved up. */

void Media_Plugin::CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c269-e->
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to Mode media
    DetermineEntArea(pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()==0 )
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Up() got a message from a device that is not a orbiter in an ent area. Ignoring!");
		return;  // Don't know what area it should be played in
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];
	if ( pEntertainArea->m_pMediaStream == NULL )
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Up(): There is no media stream in the detected ent area %d.!");
		return;  // There is no stream in the target ent area.
	}

	// the playlist is reversed
	pEntertainArea->m_pMediaStream->MoveEntryInPlaylist(iValue, -1);
	MediaInfoChanged(pEntertainArea->m_pMediaStream,true);
}

//<-dceag-c270-b->

	/** @brief COMMAND: #270 - Move Playlist entry Down */
	/** Moves a entry down in the current playlist. */
		/** @param #48 Value */
			/** The id of the entry that needs to be moved down in the playlist. */

void Media_Plugin::CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c270-e->
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Only an Orbiter will tell us to Mode media
	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea(pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()==0 )
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Down() got a message from a device that is not a orbiter in an ent area. Ignoring!");
		return;  // Don't know what area it should be played in
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];
	if ( pEntertainArea->m_pMediaStream == NULL )
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Move_Playlist_entry_Down(): There is no media stream in the detected ent area %d.!");
		return;  // There is no stream in the target ent area.
	}

	// the playlist is reversed
	pEntertainArea->m_pMediaStream->MoveEntryInPlaylist(iValue, +1);
	MediaInfoChanged(pEntertainArea->m_pMediaStream,true);
}

//<-dceag-c271-b->

	/** @brief COMMAND: #271 - Remove playlist entry. */
	/** Removes an entry from the playlist. */
		/** @param #48 Value */
			/** The Id of the entry that needs to be removed from the playlist. */

void Media_Plugin::CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c271-e->
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Only an Orbiter will tell us to Mode media
	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea(pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
    if( vectEntertainArea.size()!=1 )
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Remove_playlist_entry() got a message from a device that is not a orbiter in an ent area. Ignoring!");
		return;  // Don't know what area it should be played in
	}

	EntertainArea *pEntertainArea = vectEntertainArea[0];
	if ( pEntertainArea->m_pMediaStream == NULL )
	{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Remove_playlist_entry(): There is no media stream in the detected ent area %d.!");
		return;  // There is no stream in the target ent area.
	}

	pEntertainArea->m_pMediaStream->DeleteEntryFromPlaylist(iValue);
	if( pEntertainArea->m_pMediaStream->m_dequeMediaFile.size()==0 )
	{
		pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pEntertainArea->m_pMediaStream );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Called StopMedia after delete playlist" );
		StreamEnded(pEntertainArea->m_pMediaStream);
	}
	else
	{
		if( pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos>0 && pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos>=pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() )
			pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos--;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from JumpPos (handler)");
		string sError;
    	pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pEntertainArea->m_pMediaStream,sError);
		CheckStreamForTimeout(pEntertainArea->m_pMediaStream);
		MediaInfoChanged(pEntertainArea->m_pMediaStream,true);  // we need the true to refresh the grid
	}
}
//<-dceag-c331-b->

	/** @brief COMMAND: #331 - Get EntAreas For Device */
	/** Returns the EntArea(s) that a given device is in. */
		/** @param #2 PK_Device */
			/** The Device */
		/** @param #9 Text */
			/** A comma delimted list of EntAreas */

void Media_Plugin::CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c331-e->
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	MediaDevice *pMediaDevice = NULL;
	while( pDevice && !pMediaDevice )
	{
		pMediaDevice = m_mapMediaDevice_Find( pDevice->m_dwPK_Device );
		pDevice = (DeviceData_Router *) pDevice->m_pDevice_ControlledVia;
	}

	if( !pMediaDevice )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot get ent area for non media device: %d", iPK_Device);
		return;
	}

	for(map<int,class EntertainArea *>::iterator it=pMediaDevice->m_mapEntertainArea.begin();it!=pMediaDevice->m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		(*sText) += StringUtils::itos(pEntertainArea->m_iPK_EntertainArea) + ",";
	}
}

void Media_Plugin::FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
	// See if we have any pending media for this user
	MediaStream *pMediaStream = NULL,*pMediaStream_User = NULL;  // Matching the device (ie remote control) has first priority
	time_t tStarted = 0;

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	MediaStream *pMediaStream_Time = NULL;  // The best match is whatever one the user last started
	// Find the last media stream this user started
	for( MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it )
	{
		MediaStream *pMS = (*it).second;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::FollowMe_EnteredRoom stream %d time %d use %d device %d, comp to %d %d",
			pMS->m_iStreamID_get(),(int) pMS->m_tTime,pMS->m_iPK_Users,pMS->m_dwPK_Device_Remote,iPK_Users,iPK_Device);
#endif
		if( iPK_Device && pMS->m_dwPK_Device_Remote==iPK_Device )
		{
			if( pMediaStream_Time==NULL || pMediaStream_Time->m_tTime<pMS->m_tTime )
				pMediaStream_Time = pMS;
			break;
		}
		if( pMS->m_iPK_Users && pMS->m_iPK_Users==iPK_Users && pMS->m_tTime > tStarted )
		{
			if( pMediaStream_User==NULL || pMediaStream_User->m_tTime<pMS->m_tTime )
				pMediaStream_User = pMS;
		}
	}

	if( pMediaStream_Time )
		pMediaStream = pMediaStream_Time;
	else  // If a device (ie remote control) wasn't specified, or wasn't associated with the stream, find whatever this user was consuming
		pMediaStream = pMediaStream_User;

	if( !pMediaStream )
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Move Media, but user %d isn't listening to anything.  Open Streams: %d",iPK_Users,(int) m_mapMediaStream.size());
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Move Media, ea %d/%d user %d -- stream %d %s",
			iPK_RoomOrEntArea, iPK_RoomOrEntArea_Left, iPK_Users,pMediaStream->m_iStreamID_get(),pMediaStream->m_sMediaDescription.c_str());
		// Only move the media if it's not already there.  Maybe the user just turned follow me on
		if( pMediaStream->m_mapEntertainArea.find(iPK_RoomOrEntArea)==pMediaStream->m_mapEntertainArea.end() )
			CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),StringUtils::itos(iPK_RoomOrEntArea));
	}
}

void Media_Plugin::FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
	if( iPK_RoomOrEntArea && iPK_RoomOrEntArea_Left )
		return; // It's just a move, we're going to get EnteredRoom called immediately hereafter

	// See if we have any pending media for this user
	MediaStream *pMediaStream = NULL;
	time_t tStarted = 0;

    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// Find the last media stream this user started
	for( MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it )
	{
		MediaStream *pMS = (*it).second;
		if( pMS->m_iPK_Users && pMS->m_iPK_Users==iPK_Users && pMS->m_tTime > tStarted )
			pMediaStream = pMS;
	}

	if( !pMediaStream )
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Move Media left, but user %d isn't listening to anything.  Open Streams: %d",iPK_Users,(int) m_mapMediaStream.size());
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Before Move Media, user %d -- parking stream %d %s",
			iPK_Users,pMediaStream->m_iStreamID_get(),pMediaStream->m_sMediaDescription.c_str());
		CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),"");  // Park it for the time being
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"After Move Media");
	}
}

bool Media_Plugin::MediaFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	return HandleFollowMe(pMessage);
}
//<-dceag-c337-b->

	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #2 PK_Device */
			/** The ID of the disk drive or jukebox */
		/** @param #13 Filename */
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #20 Format */
			/** wav, flac, ogg, etc. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */
		/** @param #131 EK_Disc */
			/** The ID of the disc to rip.  If not specified this will be whatever disc is currently playing the entertainment area. */
		/** @param #151 Slot Number */
			/** The slot if this is a jukebox */
		/** @param #233 DriveID */
			/** The PK_Device ID of the storage drive that will be ripped to. Can be the ID of the core to store in /home */
		/** @param #234 Directory */
			/** The relative directory for the file to rip */

void Media_Plugin::CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory,string &sCMD_Result,Message *pMessage)
//<-dceag-c337-e->
{
	// we only have the sources device. This should be an orbiter
	// but it should be ok if we get this message from a different device in the same ent area.
	// (this is not here yet, we can put this to have the above functionality. Don't know about the usefullness yet.
	//
	// EntertainArea *pEntertainArea = DetermineEntArea( pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_From, 0);
	// is this required here ?!
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	// The disk drive we're ripping could have been passed in.  Otherwise we'll find one with the given disc
	DeviceData_Base *pDevice_Disk=m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	Row_DiscLocation *pRow_DiscLocation = NULL;

	if( pDevice_Disk )
	{
		vector<Row_DiscLocation *> vectRow_DiscLocation;
		string sSQL = "EK_Device=" + StringUtils::itos(pDevice_Disk->m_dwPK_Device);
		if( iSlot_Number )
			sSQL += " AND Slot=" + StringUtils::itos(iSlot_Number);
		m_pDatabase_pluto_media->DiscLocation_get()->GetRows(sSQL, &vectRow_DiscLocation);
		if( vectRow_DiscLocation.size()==1 )
			pRow_DiscLocation=vectRow_DiscLocation[0];
	}

	// Figure out what disc to rip
	Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(iEK_Disc);
	if( !pRow_Disc && pRow_DiscLocation )
		pRow_Disc = pRow_DiscLocation->FK_Disc_getrow();

	MediaStream *pMediaStream = NULL;
	MediaFile *pMediaFile = NULL;

	if( !pRow_Disc && !pRow_DiscLocation )  // If we have a location and not a disk, it's an unidentified disc
	{
		vector<EntertainArea *> vectEntertainArea;
		DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
		if ( vectEntertainArea.size()!=1 )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"<%=T" + StringUtils::itos(TEXT_problem_ripping_CONST) + "%>");
			SCREEN_DialogRippingError SCREEN_DialogRippingError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
				"<%=T" + StringUtils::itos(TEXT_problem_ripping_CONST) + "%>", "0");
			SendCommand(SCREEN_DialogRippingError);

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Rip_Disk(): The source device ID in the message is not in an ent area or is not an orbiter. Ignoring request");
			return;
		}

		EntertainArea *pEntertainArea = vectEntertainArea[0];

		if( !pEntertainArea->m_pMediaStream )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"<%=T" + StringUtils::itos(TEXT_problem_ripping_CONST) + "%>");
			SCREEN_DialogRippingError SCREEN_DialogRippingError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
				"<%=T" + StringUtils::itos(TEXT_problem_ripping_CONST) + "%>", "0");
			SendCommand(SCREEN_DialogRippingError);

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Rip_Disk(): no media stream");
			return;
		}

		pMediaStream = pEntertainArea->m_pMediaStream;
		pMediaFile = pMediaStream->GetCurrentMediaFile();

		if( pMediaFile && pMediaFile->m_dwPK_Disk )
			pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(pMediaFile->m_dwPK_Disk);
		else
			pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(pEntertainArea->m_pMediaStream->m_dwPK_Disc);

		if( pMediaFile && pMediaFile->m_dwPK_Device_Disk_Drive && !pDevice_Disk )
			pDevice_Disk = m_pRouter->m_mapDeviceData_Router_Find(pMediaFile->m_dwPK_Device_Disk_Drive);

		if( !pDevice_Disk && (!pMediaFile || !pRow_Disc) )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"<%=T" + StringUtils::itos(TEXT_Only_rip_drives_CONST) + "%>");
			SCREEN_DialogRippingError SCREEN_DialogRippingError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
				"<%=T" + StringUtils::itos(TEXT_Only_rip_drives_CONST) + "%>", "0");
			SendCommand(SCREEN_DialogRippingError);
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Rip_Disk(): not a drive");
			return;
		}
	}

	if( !pDevice_Disk && !pRow_DiscLocation && pRow_Disc  )
	{
		vector<Row_DiscLocation *> vectRow_DiscLocation;
		pRow_Disc->DiscLocation_FK_Disc_getrows(&vectRow_DiscLocation);
		for(vector<Row_DiscLocation *>::iterator it=vectRow_DiscLocation.begin();it!=vectRow_DiscLocation.end();++it)
		{
			Row_DiscLocation *p = *it;
			pDevice_Disk = m_pRouter->m_mapDeviceData_Router_Find( p->EK_Device_get() );
			if( pDevice_Disk )
			{
				pRow_DiscLocation=p;
				break;
			}
		}
	}

	if( !pDevice_Disk )
	{
		SCREEN_DialogRippingError SCREEN_DialogRippingError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
			"<%=T" + StringUtils::itos(TEXT_Only_rip_drives_CONST) + "%>", "0");
		SendCommand(SCREEN_DialogRippingError);
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media_Plugin::CMD_Rip_Disk(): no drive for disk");
		return;
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_Rip_Disk disc: %d mt: %d type: %s tracks: %s",
		pRow_Disc ? pRow_Disc->PK_Disc_get() : -1, pRow_Disc ? pRow_Disc->EK_MediaType_get() : -1,
		pRow_DiscLocation ? pRow_DiscLocation->Type_get().c_str() : "*NULL*", sTracks.c_str() );

	// If it's a cd and no tracks were specified, prompt the user, otherwise fill in the file names
	if( sTracks.size()==0 &&
		((pRow_Disc && pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST) || (pRow_DiscLocation && pRow_DiscLocation->Type_get()=="C") ||
		(pMediaStream && pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST) || (pMediaFile && pMediaFile->m_dwPK_MediaType==MEDIATYPE_pluto_CD_CONST)) )
	{
		SCREEN_CDTrackCopy SCREEN_CDTrackCopy(m_dwPK_Device,pMessage->m_dwPK_Device_From, iPK_Users,sFormat,sFilename,iEK_Disc,iSlot_Number,iDriveID);
		SendCommand(SCREEN_CDTrackCopy);
		return;
	}

	MediaStream *pMediaStream_Using=NULL; // See if there's a media stream using this disc or this device
	for( MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it )
	{
		MediaStream *pMS = (*it).second;
		MediaFile *pMediaFile = pMS->GetCurrentMediaFile();
		if( pRow_Disc && pMS->m_dwPK_Disc==pRow_Disc->PK_Disc_get() )
		{
			pMediaStream_Using = pMS;
			break;
		}
		if( pDevice_Disk && pMediaFile && pMediaFile->m_dwPK_Device_Disk_Drive==pDevice_Disk->m_dwPK_Device )
		{
			pMediaStream_Using = pMS;
			break;
		}
	}
	if( pMediaStream_Using )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Sending stop media before rip" );
		pMediaStream_Using->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pMediaStream_Using );
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Called StopMedia begfore rip" );
		StreamEnded(pMediaStream_Using);
	}

	if( sFormat.size()==0 )
		sFormat = DATA_Get_Type();
	if( sFormat.size()==0 )
		sFormat = "flac";
	string sResponse;
	DCE::CMD_Rip_Disk cmdRipDisk(pMessage->m_dwPK_Device_From, pDevice_Disk->m_dwPK_Device, pDevice_Disk->m_dwPK_Device, sFilename, iPK_Users,
		sFormat, sTracks, pRow_Disc ? pRow_Disc->PK_Disc_get() : 0, iSlot_Number, iDriveID, sDirectory);  // Send it from the Orbiter so disk drive knows who requested it
	if( !SendCommand(cmdRipDisk,&sResponse) || sResponse!="OK" )
	{
		SCREEN_DialogRippingError SCREEN_DialogRippingError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
			"Cannot copy disk " + sResponse, "40");
		SendCommand(SCREEN_DialogRippingError);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Rip_Disk %s",sResponse.c_str());
		return;
	}

	SCREEN_DialogRippingInstructions SCREEN_DialogRippingInstructions(m_dwPK_Device, pMessage->m_dwPK_Device_From);
	SendCommand(SCREEN_DialogRippingInstructions);

//	m_mapRippingJobs[pDiskDriveMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = new RippingJob(pDiskDriveMediaDevice,
//		pMessage->m_dwPK_Device_From, PK_Disc, PK_MediaType, iPK_Users, sName, sTracks);
	return;
}

bool Media_Plugin::DeviceOnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	bool bIsOn = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";
	MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pMessage->m_dwPK_Device_From);
	if( !pMediaDevice )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got an on/off from a non-media device");
		return false;  // It's not for us
	}

	return HandleDeviceOnOffEvent(pMediaDevice,bIsOn);
}

bool Media_Plugin::HandleDeviceOnOffEvent(MediaDevice *pMediaDevice,bool bIsOn)
{
	// First figure out if this device is involved in any streams
	MediaDevice *pMediaDevice_Source=NULL,*pMediaDevice_Dest=NULL;
	EntertainArea *pEntertainArea=NULL;
	MediaStream *pMediaStream=NULL;

	int iIsSource_OrDest = DetermineInvolvement(pMediaDevice, pMediaDevice_Source,pMediaDevice_Dest,
		pEntertainArea,pMediaStream);

	if( bIsOn && iIsSource_OrDest )  // Nothing to report, we turned on something we already knew was on
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::HandleDeviceOnOffEvent Nothing to report, we turned on something we already knew was on");
		return false;
	}
	if( !bIsOn && !iIsSource_OrDest )  // Nothing to report, we turned off something we already knew was off
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::HandleDeviceOnOffEvent Nothing to report, we turned off something we already knew was off");
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::HandleDeviceOnOffEvent IsOn %d iSource_OrDest %d",
		(int) bIsOn,iIsSource_OrDest);

	if( bIsOn==false )
	{
		// We're only playing in 1 EA.  Just shut it off
		if( pMediaStream->m_mapEntertainArea.size()<2 )
		{
			pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pMediaStream );
			StreamEnded(pMediaStream,true,true,NULL,NULL,false,false);  // Final false = don't turn back on the a/v equipment to re-activate the OSD
		}

		// We turned off the destination in an entertainment area
		else if( pEntertainArea )
			CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),pMediaStream->GetEntAreasWithout(pEntertainArea));

		// We're streaming to multiple destinations, and the device we turned off isn't the source
		// so we'll just do a move media removing this
		else if( pMediaDevice_Source && pMediaStream->m_mapEntertainArea.size()>1 )
			CMD_MH_Move_Media(pMediaStream->m_iStreamID_get(),pMediaStream->GetEntAreasWithout(&pMediaDevice->m_mapEntertainArea));

		// We can't turn off just 1 area -- shut down the whole stream
		else
		{
			pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StopMedia( pMediaStream );
			StreamEnded(pMediaStream,true,true,NULL,NULL,false,false);  // Final false = don't turn back on the a/v equipment to re-activate the OSD
		}
	}
	else if( pMediaDevice->m_pCommandGroup )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::HandleDeviceOnOffEvent Turned On %d (%s) executing command group %d (%s)",
			pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str(),
			pMediaDevice->m_pCommandGroup->m_PK_CommandGroup,pMediaDevice->m_pCommandGroup->m_Description.c_str());

		QueueMessageToRouter(new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,
			MESSAGETYPE_EXEC_COMMAND_GROUP,pMediaDevice->m_pCommandGroup->m_PK_CommandGroup,0));
	}
	else
	{
		// We don't have a specific command to do this, but since watching tv is such a common task
		// we've got a special case for it.  Don't bother if it's not just a simple tv in a single
		// entertainment area
		if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST) &&
			pMediaDevice->m_mapEntertainArea.size()==1 )
		{
			vector<EntertainArea *> vectEntertainArea;
			vectEntertainArea.push_back( pMediaDevice->m_mapEntertainArea.begin()->second );
/*
			map<int,MediaHandlerInfo *> mapMediaHandlerInfo;
			GetMediaHandlersForEA(MEDIATYPE_pluto_LiveTV_CONST, vectEntertainArea, mapMediaHandlerInfo);

			for(map<int,MediaHandlerInfo *>::iterator it=mapMediaHandlerInfo.begin();it!=mapMediaHandlerInfo.end();++it)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from Auto start TV");
				StartMedia(it->second,0,vectEntertainArea,0, 0,NULL,false,0);  // We'll let the plug-in figure out the source, and we'll use the default remote
			}
*/
		}
	}
	return false;
}

bool Media_Plugin::MediaDescriptionChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pMessage->m_dwPK_Device_From);
	if( !pMediaDevice )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got an on/off from a non-media device");
		return false;  // It's not for us
	}

	// First figure out if this device is involved in any streams
	MediaDevice *pMediaDevice_Source=NULL,*pMediaDevice_Dest=NULL;
	EntertainArea *pEntertainArea=NULL;
	MediaStream *pMediaStream=NULL;

	int iIsSource_OrDest = DetermineInvolvement(pMediaDevice, pMediaDevice_Source,pMediaDevice_Dest,
		pEntertainArea,pMediaStream);

	if( pMediaStream )
	{
		pMediaStream->m_sMediaDescription = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
		MediaInfoChanged(pMediaStream,false);
	}
	return false;
}

bool Media_Plugin::AvInputChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( !pDeviceFrom )
		return false;

	DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceFrom;
	int PK_Command = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Command_CONST].c_str());
	Command *pCommand = m_pRouter->m_mapCommand_Find(PK_Command);

	// See what media device is using that input
	for(size_t s=0;s<pDevice->m_vectDevices_SendingPipes.size();++s)
	{
		DeviceData_Router *pDevice_Child = pDevice->m_vectDevices_SendingPipes[s];
        for(map<int,Pipe *>::iterator it=pDevice_Child->m_mapPipe_Available.begin();it!=pDevice_Child->m_mapPipe_Available.end();++it)
        {
            Pipe *pPipe = (*it).second;
			if( pPipe->m_pDevice_To->m_dwPK_Device==pDevice->m_dwPK_Device &&
				pPipe->m_pCommand_Input && pPipe->m_pCommand_Input->m_dwPK_Command==PK_Command )
			{
				MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pPipe->m_pDevice_From->m_dwPK_Device);
				if( pMediaDevice )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::AvInputChanged Device %d (%s) changed to input %d (%s) the source is %d (%s)",
						pDevice->m_dwPK_Device,pDevice->m_sDescription.c_str(),
						PK_Command,pCommand ? pCommand->m_sDescription.c_str() : "unknown",
						pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
					HandleDeviceOnOffEvent(pMediaDevice,true);
				}
			}
		}
	}

	return false;
}

bool Media_Plugin::SubtitleTrackChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string sTrackID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];
	int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	string sLanguage = pMessage->m_mapParameters[EVENTPARAMETER_Language_CONST];

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::SubtitleTrackChanged to Track: %s, Lang: '%s', StreamID: %i", sTrackID.c_str(), sLanguage.c_str(), iStreamID);
	return true;
}

bool Media_Plugin::AudioTrackChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string sTrackID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];
	int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	string sLanguage = pMessage->m_mapParameters[EVENTPARAMETER_Language_CONST];

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::AudioTrackChanged to Track: %s, Lang: '%s', StreamID: %i", sTrackID.c_str(), sLanguage.c_str(), iStreamID);
	return true;
}

bool Media_Plugin::VideoTrackChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string sTrackID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];
	int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	string sLanguage = pMessage->m_mapParameters[EVENTPARAMETER_Language_CONST];

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::VideoTrackChanged to Track: %s, Lang: '%s', StreamID: %i", sTrackID.c_str(), sLanguage.c_str(), iStreamID);
	return true;
}

bool Media_Plugin::VolumeChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	MediaDevice *pMediaDevice = m_mapMediaDevice_Find( pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT ? pMessage->m_dwPK_Device_From : pMessage->m_dwPK_Device_To );
	if( !pMediaDevice )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::VolumeChanged message from %d to %d involves unknown media device",
			pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To);
		return false;
	}

	if( pMessage->m_dwID == COMMAND_Mute_CONST )
		pMediaDevice->m_bMute=!pMediaDevice->m_bMute;
	else if( pMessage->m_dwID == EVENT_Volume_Changed_CONST )
	{
		string sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
		if( sValue=="MUTE" )
			pMediaDevice->m_bMute=true;
		else
		{
			pMediaDevice->m_bMute=false;
			pMediaDevice->m_iLastVolume=atoi(pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST].c_str());;
		}
	}
	else if( pMessage->m_dwID == COMMAND_Set_Volume_CONST )
	{
		pMediaDevice->m_bMute=false;
		pMediaDevice->m_iLastVolume=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());;
	}

	for(map<int,class EntertainArea *>::iterator it=pMediaDevice->m_mapEntertainArea.begin();it!=pMediaDevice->m_mapEntertainArea.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_listVFD_LCD_Displays.size() )
			ShowMediaPlaybackState(pEntertainArea);
	}
	return false;
}

void Media_Plugin::ShowMediaPlaybackState(EntertainArea *pEntertainArea)
{
	string sVolume,sLastPlaybackState;
	if( pEntertainArea->m_pMediaStream )
	{
		if( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDevice_Audio )
		{
			MediaDevice *pMediaDevice_Audio = m_mapMediaDevice_Find( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDevice_Audio->m_dwPK_Device );
			if( pMediaDevice_Audio )
			{
				if( pMediaDevice_Audio->m_bMute )
					sVolume = "MUTE";
				else
					sVolume = StringUtils::itos(pMediaDevice_Audio->m_iLastVolume);
			}
		}
		sLastPlaybackState = StringUtils::itos( pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_iLastPlaybackSpeed );
	}

	for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
	{
		MediaDevice *pMediaDevice = *itVFD;

		DCE::CMD_Show_Media_Playback_State CMD_Show_Media_Playback_State(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
			sLastPlaybackState, pEntertainArea->m_pMediaStream ? pEntertainArea->m_pMediaStream->m_iPK_MediaType : 0, sVolume);
		SendCommand(CMD_Show_Media_Playback_State);
	}
}

bool Media_Plugin::DiskDriveIsRipping(int iPK_Device)
{
//	map<int, class RippingJob *>::const_iterator itRippingJobs = m_mapRippingJobs.find(iPK_Device);
//	return itRippingJobs!=m_mapRippingJobs.end();
	return false;
}

bool Media_Plugin::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
  return m_pJobHandler->ReportPendingTasks(pPendingTaskList);
}

void Media_Plugin::AddOtherDevicesInPipesToRenderDevices(int PK_Pipe, map<int,MediaDevice *> *pmapMediaDevice,EntertainArea *pEntertainArea)
{
	for(map<int,MediaDevice *>::iterator it=pmapMediaDevice->begin();it!=pmapMediaDevice->end();++it)
		AddOtherDevicesInPipes_Loop(PK_Pipe,(*it).second->m_pDeviceData_Router,pmapMediaDevice,pEntertainArea);
}

void Media_Plugin::AddOtherDevicesInPipes_Loop(int PK_Pipe, DeviceData_Router *pDevice,map<int,MediaDevice *> *pmapMediaDevice,EntertainArea *pEntertainArea,vector<int> *p_vectDevice)
{
	if( !pDevice )
		return;

	bool bCreatedVect=false;  // The vect is a list of device we've checked already so we don't check the same one twice if the user made a recursive loop
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries - just exit and don't check the same device twice
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDevice->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDevice->m_dwPK_Device);


	MapPipe *pMapPipe = NULL;
	if( pEntertainArea && pEntertainArea->m_pMediaDevice_OutputZone )
		pMapPipe = pDevice->m_mapPipe_Temporary_Find(pEntertainArea->m_iPK_EntertainArea);
	else
		pMapPipe = &pDevice->m_mapPipe_Available;

	if( pMapPipe )
	{
		for(MapPipe::iterator it=pMapPipe->begin();it!=pMapPipe->end();++it)
		{
			Pipe *pPipe = (*it).second;
			if( !PK_Pipe || pPipe->m_PK_Pipe==PK_Pipe )
			{
				DeviceData_Router *pDevice_Pipe = m_pRouter->m_mapDeviceData_Router_Find(pPipe->m_pDevice_To->m_dwPK_Device);
				if( pDevice_Pipe )
				{
					MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDevice_Pipe->m_dwPK_Device);
					if( pMediaDevice )
						(*pmapMediaDevice)[pDevice_Pipe->m_dwPK_Device] = pMediaDevice;
					AddOtherDevicesInPipes_Loop(PK_Pipe,pDevice_Pipe,pmapMediaDevice,pEntertainArea,p_vectDevice);
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AddOtherDevicesInPipes_Loop - Device %d isn't a media device",pPipe->m_pDevice_To->m_dwPK_Device);
			}
		}
	}

	if( pDevice->m_pDevice_MD && pDevice!=pDevice->m_pDevice_MD &&
		pDevice->m_pDevice_ControlledVia &&
		pDevice->m_pDevice_ControlledVia->m_dwPK_DeviceTemplate==DEVICETEMPLATE_OnScreen_Orbiter_CONST )
	{
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pDevice->m_pDevice_MD->m_dwPK_Device);
		if( pMediaDevice )
			(*pmapMediaDevice)[pDevice->m_pDevice_MD->m_dwPK_Device] = pMediaDevice;
		AddOtherDevicesInPipes_Loop( PK_Pipe, (DeviceData_Router *) pDevice->m_pDevice_MD, pmapMediaDevice, pEntertainArea, p_vectDevice );
	}
	if( bCreatedVect )
		delete p_vectDevice;
}

void Media_Plugin::GetMediaHandlersForEA(int iPK_MediaType,int iPK_MediaProvider,int &iPK_Device, int iPK_DeviceTemplate, vector<EntertainArea *> &vectEntertainArea, vector< pair< MediaHandlerInfo *,vector<EntertainArea *> > > &vectEA_to_MediaHandler, map<int, pair<MediaDevice *,MediaDevice *> > &mapEntertainmentArea_OutputZone)
{
	bool bUsingGenericHandler=false;  // A flag if we're using the generic handler
	// This function needs to find a media handler for every entertainment area.  This map will store all our possibilities
	// of handlers and what entertainment areas they can support.  We'll first populate the map, then pick the best matches
	// to store in vectEA_to_MediaHandler
	map<MediaHandlerInfo *, vector<EntertainArea *> > mapMediaHandlerInfo;
	map<int,EntertainArea *> mapEntertainArea_NoHandlers;
	LoggerWrapper::GetInstance()->Write( LV_DEBUG, "Start of GetMediaHandlersForEA");
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea=vectEntertainArea[s];
		// See if there's a media handler for this type of media in this area
		List_MediaHandlerInfo *pList_MediaHandlerInfo = pEntertainArea->m_mapMediaHandlerInfo_MediaType_Find(iPK_MediaType,iPK_MediaProvider);
		if( !pList_MediaHandlerInfo || pList_MediaHandlerInfo->size()==0 )
		{
			Row_MediaType *pRow_MediaType = m_pDatabase_pluto_main->MediaType_get()->GetRow(iPK_MediaType);
			if( pRow_MediaType && pRow_MediaType->DCEAware_get()==0 )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Play media type %d in entertain area %d with generic handler", iPK_MediaType, pEntertainArea->m_iPK_EntertainArea);
				m_pGenericMediaHandlerInfo->m_PK_MediaType = iPK_MediaType; // Just temporary for start media.  We're in a mutex
				mapMediaHandlerInfo[m_pGenericMediaHandlerInfo].push_back(pEntertainArea);
				bUsingGenericHandler=true;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Play media type %d in entertain area %d but nothing to handle it", iPK_MediaType, pEntertainArea->m_iPK_EntertainArea);
				mapEntertainArea_NoHandlers[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
			}
		}
		else
		{
			for( List_MediaHandlerInfo::iterator it=pList_MediaHandlerInfo->begin();it!=pList_MediaHandlerInfo->end();++it )
			{
				MediaHandlerInfo *pMediaHandlerInfo = *it;
				if( (iPK_Device && !pMediaHandlerInfo->ControlsDevice(iPK_Device)) ||
					(iPK_DeviceTemplate && pMediaHandlerInfo->m_PK_DeviceTemplate!=iPK_DeviceTemplate) )
						continue;
				mapMediaHandlerInfo[pMediaHandlerInfo].push_back(pEntertainArea);
			}
		}
	}
	if( mapMediaHandlerInfo.size()==1 && mapEntertainArea_NoHandlers.size()==0 && (vectEntertainArea.size()==1 || mapMediaHandlerInfo.begin()->first->m_bMultipleDestinations) )
	{
		// It's easy, save ourselves the trouble of searching we've got 1 handler and it can take care of it
		vectEA_to_MediaHandler.push_back( make_pair< MediaHandlerInfo *,vector<EntertainArea *> >(mapMediaHandlerInfo.begin()->first,mapMediaHandlerInfo.begin()->second) );
        	LoggerWrapper::GetInstance()->Write( LV_DEBUG, "We exit out of GetMediaHandlersForEA as we have a media handler");
		return;
	}

	// It's possible that all the devices are generic media devices...
	if( mapMediaHandlerInfo.size()==1 && bUsingGenericHandler )
	{
		// and that the source device has a capture card, then it's ok to use one stream because it will be broadcast throughout the house via the capture card
		MediaDevice *pMediaDevice = m_mapMediaDevice_Find(iPK_Device);
		if( pMediaDevice && pMediaDevice->m_pDevice_CaptureCard )
		{
			vectEA_to_MediaHandler.push_back( make_pair< MediaHandlerInfo *,vector<EntertainArea *> >(mapMediaHandlerInfo.begin()->first,mapMediaHandlerInfo.begin()->second) );
			return;
		}
	}

	// If we didn't find any handlers, see if there any devices in other entertainment areas
	// with output zones feeding the one(s) we want to be in.  This is complicated.  If the source device was a generic media device
	// it would have matched above already.
	for(map<int,EntertainArea *>::iterator it=mapEntertainArea_NoHandlers.begin();it!=mapEntertainArea_NoHandlers.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		// We need to play in this entertainment area, are there any output zones in it?
		for(map<int, class MediaDevice *>::iterator itMD=pEntertainArea->m_mapMediaDevice.begin();itMD!=pEntertainArea->m_mapMediaDevice.end();++itMD)
		{
			MediaDevice *pMediaDevice = itMD->second;
			if( pMediaDevice->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_Output_Zone_CONST) )
			{
				// See what devices feed into it
				for(map<int,MediaDevice *>::iterator itMDFO=pMediaDevice->m_mapMediaDevice_FedInto_OutsideZone.begin();itMDFO!=pMediaDevice->m_mapMediaDevice_FedInto_OutsideZone.end();++itMDFO)
				{
					MediaDevice *pMediaDevice_FeedsIn = itMDFO->second;
					// See if there's a media handler that can control this
					for(vector<MediaHandlerInfo *>::iterator itMHI=m_vectMediaHandlerInfo.begin();itMHI!=m_vectMediaHandlerInfo.end();++itMHI)
					{
						MediaHandlerInfo *pMediaHandlerInfo = *itMHI;
						if( pMediaHandlerInfo->m_PK_MediaType==iPK_MediaType && pMediaHandlerInfo->ControlsDevice(pMediaDevice_FeedsIn->m_pDeviceData_Router->m_dwPK_Device) )
						{
							mapEntertainmentArea_OutputZone[ pEntertainArea->m_iPK_EntertainArea ] = make_pair<MediaDevice *,MediaDevice *> (pMediaDevice_FeedsIn,pMediaDevice);
							mapMediaHandlerInfo[pMediaHandlerInfo].push_back(pEntertainArea);
							break;
						}
					}
				}
			}
		}
	}

	// It's more work, we need to prioritize the handlers and start making assignments from the top
	list<MediaHandlerInfo *> listMediaHandlerInfo;
	for( map<MediaHandlerInfo *, vector<EntertainArea *> >::iterator it=mapMediaHandlerInfo.begin();it!=mapMediaHandlerInfo.end();++it )
		listMediaHandlerInfo.push_back( it->first );
	listMediaHandlerInfo.sort(MediaHandlerComparer);

	// Debug loop to see what we have atm with regards to priorities.
	
	for (list<MediaHandlerInfo *>::iterator it=listMediaHandlerInfo.begin(); it!=listMediaHandlerInfo.end(); ++it)
	{
                MediaHandlerInfo *pMediaHandlerInfo = *it;
                LoggerWrapper::GetInstance()->Write(LV_DEBUG,"After sort DT #%d - MediaType #%d",pMediaHandlerInfo->m_PK_DeviceTemplate,pMediaHandlerInfo->m_PK_MediaType);
        }


	// Now a map to keep track of which EA's we have matched
	map<int,bool> mapMatched;
	for(size_t s=0;s<vectEntertainArea.size();++s)
		mapMatched[ vectEntertainArea[s]->m_iPK_EntertainArea ] = false;

	vector<EntertainArea *> vEmpty;  // Just an empty vect since we're creating new ones on the stack

	// Now, we're going to make 2 passes if we have more than 1 EA.  First we'll find handlers that can play multiple
	// areas, reducing the number of redundant streams, and then on the 2nd pass pick up EA's not yet serviced
	bool bMultipleEA = vectEntertainArea.size()>1;
	for(int pass=0;pass< ( bMultipleEA ? 2 : 1); ++pass)
	{
		for( list<MediaHandlerInfo *>::iterator it=listMediaHandlerInfo.begin();it!=listMediaHandlerInfo.end();++it )
		{
			MediaHandlerInfo *pMediaHandlerInfo = *it;
			if( bMultipleEA && pass==0 && !pMediaHandlerInfo->m_bMultipleDestinations )
				continue;

			bool bAlreadyUsed=false;  // We'll set this to true when we assign a media handler to allow for splitting into multiple entries if the device can't handle more than 1 EA
			bool bNeedToStartWithEmpty=true;  // The first time we find a match we know we need to start with a new empty vect

			// Get all the possible EA's
			vector<EntertainArea *> *pvect_EntertainArea = &mapMediaHandlerInfo[pMediaHandlerInfo];
			for(size_t s=0;s<pvect_EntertainArea->size();++s)
			{
				// And fill in the ones we didn't match
				if( !mapMatched[ (*pvect_EntertainArea)[s]->m_iPK_EntertainArea ] )
				{
					if( bAlreadyUsed || bNeedToStartWithEmpty ) // Start with a new empty vector if we already used it and can't handle multiple EA's
					{
						vectEA_to_MediaHandler.push_back( make_pair< MediaHandlerInfo *,vector<EntertainArea *> >(
							pMediaHandlerInfo,vEmpty) );
						bNeedToStartWithEmpty=false;
					}

					// Mark it as matched, and add it to the vect we just created
					mapMatched[ (*pvect_EntertainArea)[s]->m_iPK_EntertainArea ]=true;
					vectEA_to_MediaHandler.back().second.push_back( (*pvect_EntertainArea)[s] );
					if( !pMediaHandlerInfo->m_bMultipleDestinations )
						bAlreadyUsed=true;  // Don't re-use this it can't handle multiple destinations

				}
			}
		}
	}
}

MediaDevice *Media_Plugin::GetMediaDeviceForEA(int iPK_MediaType,EntertainArea *pEntertainArea)
{
	for(map<int, class MediaDevice *>::iterator it=pEntertainArea->m_mapMediaDevice.begin();it!=pEntertainArea->m_mapMediaDevice.end();++it)
	{
		MediaDevice *pMediaDevice = it->second;
		if( pMediaDevice->m_mapMediaType.find(iPK_MediaType)!=pMediaDevice->m_mapMediaType.end() )
			return pMediaDevice;
	}
	return NULL;
}

//<-dceag-c372-b->

	/** @brief COMMAND: #372 - MH Set Volume */
	/** Set the volume */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment Area(s) */
		/** @param #76 Level */
			/** The level as an abolute value from 0-100, or a relative value like -1 or +1. */

void Media_Plugin::CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c372-e->
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_MH_Set_Volume");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string::size_type pos=0;
	while(pos<sPK_EntertainArea.size() && pos!=string::npos)
	{
		string s = StringUtils::Tokenize(sPK_EntertainArea,",",pos);
		EntertainArea *pEntertainArea = m_mapEntertainAreas_Find(abs(atoi(s.c_str())));  // abs because orbiter floorplan sends ent areas as - to differentiate between devices
		if( !pEntertainArea )
		{
			OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
			if( pOH_Orbiter )
				pEntertainArea = pOH_Orbiter->m_pEntertainArea;
		}

		int PK_Device_Dest = m_dwPK_Device;
		if( pEntertainArea->m_pMediaDevice_ActiveDest )
		{
			if( pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->WithinCategory(DEVICECATEGORY_DCE_Software_Wrappers_CONST) && pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_pDevice_MD )
				PK_Device_Dest = pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_pDevice_MD->m_dwPK_Device;
			else
				PK_Device_Dest = pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device;
		}
		else if( pEntertainArea->m_pMediaDevice_MD )
			PK_Device_Dest = pEntertainArea->m_pMediaDevice_MD->m_pDeviceData_Router->m_dwPK_Device;

LoggerWrapper::GetInstance()->Write(LV_STATUS,"For EA %s found active device %d",s.c_str(),PK_Device_Dest);

		long dwApp_Server = 0;
		if(NULL != pEntertainArea->m_pMediaDevice_ActiveDest && NULL != pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router)
		{
			MediaDevice *pMediaDevice = m_mapMediaDevice_Find(pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device_MD); // We have an app server to control the volume
			if(NULL != pMediaDevice && NULL != pMediaDevice->m_pDevice_Audio && pMediaDevice->m_pDevice_Audio->m_dwPK_DeviceTemplate==DEVICETEMPLATE_App_Server_CONST )  // We have an MD and it uses appserver for the volume
				dwApp_Server = pMediaDevice->m_pDevice_Audio->m_dwPK_Device;
		}

		if( sLevel=="-1" )
		{
			DCE::CMD_Vol_Down CMD_Vol_Down(pMessage->m_dwPK_Device_From,PK_Device_Dest,1);
			CheckForCustomPipe(pEntertainArea,CMD_Vol_Down.m_pMessage);
			SendCommand(CMD_Vol_Down);

			if(dwApp_Server != 0)
			{
				DCE::CMD_Vol_Down CMD_Vol_Down_App_Server(pMessage->m_dwPK_Device_From,dwApp_Server,1);
				CheckForCustomPipe(pEntertainArea,CMD_Vol_Down_App_Server.m_pMessage);
				SendCommand(CMD_Vol_Down_App_Server);
			}
		}
		else if( sLevel=="+1" )
		{
			DCE::CMD_Vol_Up CMD_Vol_Up(pMessage->m_dwPK_Device_From,PK_Device_Dest,1);
			CheckForCustomPipe(pEntertainArea,CMD_Vol_Up.m_pMessage);
			SendCommand(CMD_Vol_Up);

			if(dwApp_Server != 0)
			{
				DCE::CMD_Vol_Up CMD_Vol_Up_App_Server(pMessage->m_dwPK_Device_From,dwApp_Server,1);
				CheckForCustomPipe(pEntertainArea,CMD_Vol_Up_App_Server.m_pMessage);
				SendCommand(CMD_Vol_Up_App_Server);
			}
		}
		else
		{
			DCE::CMD_Set_Volume CMD_Set_Volume(pMessage->m_dwPK_Device_From,PK_Device_Dest,sLevel);
			CheckForCustomPipe(pEntertainArea,CMD_Set_Volume.m_pMessage);
			SendCommand(CMD_Set_Volume);

			if(dwApp_Server != 0)
			{
				DCE::CMD_Set_Volume CMD_Set_Volume_App_Server(pMessage->m_dwPK_Device_From,dwApp_Server,sLevel);
				CheckForCustomPipe(pEntertainArea,CMD_Set_Volume_App_Server.m_pMessage);
				SendCommand(CMD_Set_Volume_App_Server);
			}
		}
	}
}

//<-dceag-c388-b->

	/** @brief COMMAND: #388 - Set Media Private */
	/** Indicate if the media in an entertainment area is private or not. */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #119 True/False */
			/** True if the media in this area is public (the default), false if it's not */

void Media_Plugin::CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c388-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(sPK_EntertainArea.c_str()) );
	if( !pEntertainArea )
	{
		//LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Tried to set private flag on non-existant ea (%s)%i",sPK_EntertainArea,bTrueFalse);
		return;
	}
	pEntertainArea->m_bMediaIsPrivate=!bTrueFalse;

	for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = (*it).second;
		if( pOH_Orbiter->m_pEntertainArea != pEntertainArea )
			continue;

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconl(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
			StringUtils::itos((int) pEntertainArea->m_bMediaIsPrivate),"","media_private");
		SendCommand(CMD_Set_Bound_Iconl);
	}
}

//<-dceag-c391-b->

	/** @brief COMMAND: #391 - Add Media Attribute */
	/** Changes the media attribute */
		/** @param #5 Value To Assign */
			/** The new value of the attribute */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #121 Tracks */
			/** If empty, the attribute is for the disc.  If specified, it is for this track number */
		/** @param #122 EK_AttributeType */
			/** The type of attribute to set */
		/** @param #123 EK_Attribute */
			/** The ID of the attribute */
		/** @param #135 Section */
			/** If specified the attribute is added for this section only */
		/** @param #145 EK_File */
			/** The file to add the attribute for.  If not specified, then a stream ID must be specified and the current file in that stream will be used */

void Media_Plugin::CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType,string sSection,int iEK_File,int *iEK_Attribute,string &sCMD_Result,Message *pMessage)
//<-dceag-c391-e->
{
	*iEK_Attribute=0;
	MediaStream *pMediaStream = NULL;
	if( !iEK_File )
	{
		pMediaStream = m_mapMediaStream_Find(iStreamID,pMessage ? pMessage->m_dwPK_Device_From : 0);
		if( !pMediaStream )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CMD_Add_Media_Attribute cannot find stream %d",iStreamID);
			sCMD_Result="BAD STREAM";
			return;
		}
		MediaFile *pMediaFile=NULL;
		if( pMediaStream->m_iDequeMediaFile_Pos<0 || pMediaStream->m_iDequeMediaFile_Pos>=pMediaStream->m_dequeMediaFile.size() ||
			(pMediaFile=pMediaStream->GetCurrentMediaFile())==NULL || (iEK_File=pMediaFile->m_dwPK_File)==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CMD_Add_Media_Attribute no valid file in stream %d",iStreamID);
			sCMD_Result="STREAM HAS NO FILE";
			return;
		}
	}
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(iEK_File);
	if( !pRow_File )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CMD_Add_Media_Attribute bad file %d",iEK_File);
		sCMD_Result="BAD FILE";
		return;
	}

	Row_Attribute *pRow_Attribute = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromDescription(pRow_File->EK_MediaType_get(),iEK_AttributeType,sValue_To_Assign);
	if( pRow_Attribute )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "CMD_Add_Media_Attribute: pRow_Attribute found!");
		*iEK_Attribute=pRow_Attribute->PK_Attribute_get();
		Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->GetRow(iEK_File,pRow_Attribute->PK_Attribute_get(),atoi(sTracks.c_str()),atoi(sSection.c_str()));
		if( !pRow_File_Attribute )
		{
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "CMD_Add_Media_Attribute: Add and commit changes!");
			pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
			pRow_File_Attribute->FK_File_set(iEK_File);
			pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
			pRow_File_Attribute->Track_set(atoi(sTracks.c_str()));
			pRow_File_Attribute->Section_set(atoi(sSection.c_str()));
			m_pDatabase_pluto_media->File_Attribute_get()->Commit();
		} else {
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "CMD_Add_Media_Attribute: COULD NOT Add and commit changes!");
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "CMD_Add_Media_Attribute: pRow_Attribute NOT found!");
	}
}

//<-dceag-c392-b->

	/** @brief COMMAND: #392 - Set Media Attribute Text */
	/** Updates the text for an attribute */
		/** @param #5 Value To Assign */
			/** The new value.  If it's a name, LastName^Firstname format */
		/** @param #123 EK_Attribute */
			/** The attribute */

void Media_Plugin::CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iEK_Attribute,string &sCMD_Result,Message *pMessage)
//<-dceag-c392-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(iEK_Attribute);
	if( pRow_Attribute )
	{
		string sOldValue = pRow_Attribute->Name_get();

		pRow_Attribute->Name_set(sValue_To_Assign);
		pRow_Attribute->Table_Attribute_get()->Commit();

		for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
		{
			MediaInfoChanged(it->second,true);
		}
	}
}
//<-dceag-c393-b->

	/** @brief COMMAND: #393 - Get Attribute */
	/** Returns the attribute text for the given ID */
		/** @param #9 Text */
			/** The value */
		/** @param #123 EK_Attribute */
			/** The attribute */

void Media_Plugin::CMD_Get_Attribute(int iEK_Attribute,string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c393-e->
{
	Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(iEK_Attribute);
	if( pRow_Attribute )
	{
		(*sText) = pRow_Attribute->Name_get();
		StringUtils::Replace(sText,"\t","&");
	}
}

int Media_Plugin::DetermineInvolvement(MediaDevice *pMediaDevice, MediaDevice *&pMediaDevice_Source,MediaDevice *&pMediaDevice_Dest,
	EntertainArea *&pEntertainArea,MediaStream *&pMediaStream)
{
	if( !pMediaDevice )
		return 0;

    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMS = (*it).second;

		// See if the device turned off was the source for some media
		if( pMS->m_pMediaDevice_Source==pMediaDevice || pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_MD==pMediaDevice->m_pDeviceData_Router )
		{
			pMediaStream = pMS;
			return 1;  // This is the source device
		}

		// If not, maybe it's being fed the signal from the source
		for(size_t s=0;s<pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes.size();++s)
		{
			DeviceData_Base *pDevice = pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes[s];
			if( pMS->m_pMediaDevice_Source->m_pDeviceData_Router==pDevice || pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_MD==pDevice )
			{
				pMediaStream = pMS;
				pMediaDevice_Source = pMS->m_pMediaDevice_Source;
				return 1;
			}
		}

		for( MapEntertainArea::iterator it=pMS->m_mapEntertainArea.begin( );it!=pMS->m_mapEntertainArea.end( );++it )
		{
			EntertainArea *pEA = ( *it ).second;
			if( !pEA->m_pMediaDevice_ActiveDest )
				continue;

			if( pEA->m_pMediaDevice_ActiveDest==pMediaDevice || pEA->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_pDevice_MD==pMediaDevice->m_pDeviceData_Router )
			{
				pMediaStream = pMS;
				pEntertainArea = pEA;
				return 2;  // This is the dest device
			}
			// If not, maybe it's being fed the signal from the dest
			for(size_t s=0;s<pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes.size();++s)
			{
				DeviceData_Base *pDevice = pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes[s];
				if( pEA->m_pMediaDevice_ActiveDest->m_pDeviceData_Router==pDevice || pEA->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_pDevice_MD==pDevice )
				{
					pMediaStream = pMS;
					pEntertainArea = pEA;
					pMediaDevice_Dest = pEntertainArea->m_pMediaDevice_ActiveDest;
					return 2;  // This is the dest device
				}
			}
		}
	}
	return 0; // No involvement
}

int Media_Plugin::GetStorageDeviceWithMostFreeSpace(string& sFullDescription, string& sMountedPath)
{
	PlutoSqlResult result;
	DB_ROW row;
	string sSQL =
		"SELECT PK_Device, Device.Description, Device_DeviceData.IK_DeviceData, FK_DeviceCategory "
		"FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN Device_DeviceData ON FK_Device = PK_Device "
		"WHERE FK_DeviceCategory IN (" +
		StringUtils::ltos(DEVICECATEGORY_Core_CONST) + ", " +
		StringUtils::ltos(DEVICECATEGORY_Hard_Drives_CONST) + ", " +
		StringUtils::ltos(DEVICECATEGORY_Storage_Devices_CONST) + ", " +
		StringUtils::ltos(DEVICECATEGORY_Network_Storage_CONST) + ", " +
		StringUtils::ltos(DEVICECATEGORY_Raid_Devices_CONST) + ", " +
		StringUtils::ltos(DEVICECATEGORY_Raid_Generic_CONST) +
		+ ") AND FK_DeviceData = " +
		StringUtils::ltos(DEVICEDATA_Free_Disk_Space_in_MBytes_CONST) + " " +
		"ORDER BY CAST(Device_DeviceData.IK_DeviceData AS UNSIGNED) DESC ";

	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL.c_str())) )
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			if(NULL != row[0] && NULL != row[1] && NULL != row[3])
			{
				int PK_Device = atoi(row[0]);
				string sOnline = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,PK_Device,DEVICEDATA_Online_CONST);
				if( sOnline.empty()==false && atoi(sOnline.c_str())==0 )
					continue;  // This disc isn't online

				string sReadOnly = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,PK_Device,DEVICEDATA_Readonly_CONST);
				if( sReadOnly.empty()==false && atoi(sReadOnly.c_str())==1 )
					continue;  // This disc isn't online

				string sFreeSpace;
				if(0 == row[2] || string(row[2]).empty())
					sFreeSpace = "0";
				else
					sFreeSpace = row[2];

				if(atoi(row[3]) == DEVICECATEGORY_Core_CONST)
					sMountedPath = "/home/";
				else
					sMountedPath = string("/mnt/device/") + row[0] + "/";

				sFullDescription = string(row[1]) + " (#" + row[0] + ") " + sFreeSpace + "MB";
				return PK_Device;
			}
		}
	}

	return 0;
}

void Media_Plugin::RegisterMediaPlugin(class Command_Impl *pCommand_Impl,class MediaHandlerBase *pMediaHandlerBase,vector<int> &vectPK_DeviceTemplate,bool bUsesDCE)
{
	for(size_t s=0;s<vectPK_DeviceTemplate.size();++s)
	{
		int iPK_DeviceTemplate = vectPK_DeviceTemplate[s];
		Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);
		if( !pRow_DeviceTemplate )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid device template %d as plugin",iPK_DeviceTemplate);
			return;  // Nothing we can do
		}

		int iPKDevice = pCommand_Impl->m_dwPK_Device;
		int iPKDeviceTemplate = pRow_DeviceTemplate->PK_DeviceTemplate_get();
		string strDescription = pRow_DeviceTemplate->Description_get();

		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(iPKDeviceTemplate,DEVICEDATA_Media_Catalog_CONST);
		if( pRow_DeviceTemplate_DeviceData )
		{
			string sData = pRow_DeviceTemplate_DeviceData->IK_DeviceData_get();
			string::size_type pos=0;
			while(true)
			{
				string sLine = StringUtils::Tokenize(sData,"\r\n",pos);
				if(sLine.empty())
					break;
				string::size_type pos2 = 0;
				int PK_MediaType = atoi( StringUtils::Tokenize(sLine,",",pos2).c_str() );
				string sToken = StringUtils::Tokenize(sLine,",",pos2);
				m_mapMediaCatalog[sToken]=pMediaHandlerBase;
			}
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Registered media plug in #%d (Template %d) %s (adress %p, plugin base address %p)",iPKDevice,iPKDeviceTemplate,strDescription.c_str(), pCommand_Impl, pMediaHandlerBase);
		vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
		pRow_DeviceTemplate->DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(&vectRow_DeviceTemplate_MediaType);
		for(size_t mt=0;mt<vectRow_DeviceTemplate_MediaType.size();++mt)
		{
			Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[mt];
			MediaHandlerInfo *pMediaHandlerInfo =
				new MediaHandlerInfo(pMediaHandlerBase,pCommand_Impl,pRow_DeviceTemplate_MediaType->FK_MediaType_get(),
					iPK_DeviceTemplate,pRow_DeviceTemplate_MediaType->CanSetPosition_get()==1,bUsesDCE);

			m_vectMediaHandlerInfo.push_back(pMediaHandlerInfo);
			pMediaHandlerBase->m_vectMediaHandlerInfo.push_back(pMediaHandlerInfo);
		}
	}
}

void Media_Plugin::PopulateRemoteControlMaps()
{
	vector<Row_DeviceTemplate_MediaType_DesignObj *> vectRow_DeviceTemplate_MediaType_DesignObj;
	m_pDatabase_pluto_main->DeviceTemplate_MediaType_DesignObj_get()->GetRows("1=1",&vectRow_DeviceTemplate_MediaType_DesignObj);
	for(size_t s=0;s<vectRow_DeviceTemplate_MediaType_DesignObj.size();++s)
	{
		Row_DeviceTemplate_MediaType_DesignObj *pRow_DeviceTemplate_MediaType_DesignObj = vectRow_DeviceTemplate_MediaType_DesignObj[s];
		if( !pRow_DeviceTemplate_MediaType_DesignObj->FK_Skin_isNull() )
			continue;  // This applies to a particular skin.  Come back and check these against the orbiters below
		m_mapDeviceTemplate_MediaType_RemoteControl[pRow_DeviceTemplate_MediaType_DesignObj->UIVersion_get()-1][
			make_pair<int,int> ( pRow_DeviceTemplate_MediaType_DesignObj->FK_DeviceTemplate_MediaType_getrow()->FK_DeviceTemplate_get(),
				pRow_DeviceTemplate_MediaType_DesignObj->FK_DeviceTemplate_MediaType_getrow()->FK_MediaType_get() )
			]
			= new RemoteControlSet(pRow_DeviceTemplate_MediaType_DesignObj);
	}

	vector<Row_MediaType_DesignObj *> vectRow_MediaType_DesignObj;
	m_pDatabase_pluto_main->MediaType_DesignObj_get()->GetRows("1=1",&vectRow_MediaType_DesignObj);
	for(size_t s=0;s<vectRow_MediaType_DesignObj.size();++s)
	{
		Row_MediaType_DesignObj *pRow_MediaType_DesignObj = vectRow_MediaType_DesignObj[s];
		if( !pRow_MediaType_DesignObj->FK_Skin_isNull() )
			continue;  // This applies to a particular skin.  Come back and check these against the orbiters below
		m_mapMediaType_RemoteControl[pRow_MediaType_DesignObj->UIVersion_get()-1][pRow_MediaType_DesignObj->FK_MediaType_get()]
			= new RemoteControlSet(pRow_MediaType_DesignObj);
	}

	// Now check each orbiter to see if there is something specific for a skin that overwrites these settings
    for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
		int UiVersion=1;
		if( pOH_Orbiter->m_dwPK_UI )
		{
			Row_UI *Row_UI = m_pDatabase_pluto_main->UI_get()->GetRow(pOH_Orbiter->m_dwPK_UI);
			if( Row_UI )
				UiVersion = Row_UI->Version_get();
		}
		m_mapOrbiterUiVersion[pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device] = UiVersion;
		for(size_t s=0;s<vectRow_DeviceTemplate_MediaType_DesignObj.size();++s)
		{
			Row_DeviceTemplate_MediaType_DesignObj *pRow_DeviceTemplate_MediaType_DesignObj = vectRow_DeviceTemplate_MediaType_DesignObj[s];
			if( pRow_DeviceTemplate_MediaType_DesignObj->FK_Skin_isNull() || pRow_DeviceTemplate_MediaType_DesignObj->FK_Skin_get()!=pOH_Orbiter->m_dwPK_Skin )
				continue;  // No skin to overwrite for this orbiter

			m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl[pRow_DeviceTemplate_MediaType_DesignObj->UIVersion_get()-1][
				make_pair< int, pair<int,int> > ( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
					make_pair< int, int > (
						pRow_DeviceTemplate_MediaType_DesignObj->FK_DeviceTemplate_MediaType_getrow()->FK_DeviceTemplate_get(),
						pRow_DeviceTemplate_MediaType_DesignObj->FK_DeviceTemplate_MediaType_getrow()->FK_MediaType_get() ) )
				]
				= new RemoteControlSet(pRow_DeviceTemplate_MediaType_DesignObj);
		}

		for(size_t s=0;s<vectRow_MediaType_DesignObj.size();++s)
		{
			Row_MediaType_DesignObj *pRow_MediaType_DesignObj = vectRow_MediaType_DesignObj[s];
			if( pRow_MediaType_DesignObj->FK_Skin_isNull() || pRow_MediaType_DesignObj->FK_Skin_get()!=pOH_Orbiter->m_dwPK_Skin )
				continue;  // No skin to overwrite for this orbiter
int k=2;
			m_mapOrbiter_MediaType_RemoteControl[pRow_MediaType_DesignObj->UIVersion_get()-1][
				make_pair<int,int> (pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
					pRow_MediaType_DesignObj->FK_MediaType_get() )
				]
				= new RemoteControlSet(pRow_MediaType_DesignObj);
		}
	}

	// Do these last since these are the customer's individual preferences that should override the defaults
	vector<Row_RemoteControl *> vectRow_RemoteControl;
	m_pDatabase_pluto_main->RemoteControl_get()->GetRows("1=1",&vectRow_RemoteControl);
	for(size_t s=0;s<vectRow_RemoteControl.size();++s)
	{
		Row_RemoteControl *pRow_RemoteControl = vectRow_RemoteControl[s];
		int UIVersion=1;
		if( pRow_RemoteControl->FK_Orbiter_isNull() )
		{
			if( pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_isNull() )
			{
				if( !pRow_RemoteControl->FK_MediaType_DesignObj_isNull() ) // Be sure they're not both null
					m_mapMediaType_RemoteControl[UIVersion-1][pRow_RemoteControl->FK_MediaType_DesignObj_getrow()->FK_MediaType_get()]
						= new RemoteControlSet(pRow_RemoteControl->FK_MediaType_DesignObj_getrow());
			}
			else
				m_mapDeviceTemplate_MediaType_RemoteControl[UIVersion-1][
					make_pair<int,int> ( pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_getrow()->FK_DeviceTemplate_MediaType_getrow()->FK_DeviceTemplate_get(),
						pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_getrow()->FK_DeviceTemplate_MediaType_getrow()->FK_MediaType_get() )
					]
					= new RemoteControlSet(pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_getrow());
		}
		else
		{
			UIVersion = m_mapOrbiterUiVersion[pRow_RemoteControl->FK_Orbiter_get()-1];
			if( UIVersion==0 )
				UIVersion=1;
			if( pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_isNull() )
			{
				if( !pRow_RemoteControl->FK_MediaType_DesignObj_isNull() ) // Be sure they're not both null
					m_mapOrbiter_MediaType_RemoteControl[UIVersion-1][
						make_pair<int,int> (pRow_RemoteControl->FK_Orbiter_get(),
							pRow_RemoteControl->FK_MediaType_DesignObj_getrow()->FK_MediaType_get() )
						]
						= new RemoteControlSet(pRow_RemoteControl->FK_MediaType_DesignObj_getrow());
			}
			else
				m_mapOrbiter_DeviceTemplate_MediaType_RemoteControl[UIVersion-1][
					make_pair< int, pair<int,int> > ( pRow_RemoteControl->FK_Orbiter_get(),
						make_pair< int, int > (
							pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_getrow()->FK_DeviceTemplate_MediaType_getrow()->FK_DeviceTemplate_get(),
							pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_getrow()->FK_DeviceTemplate_MediaType_getrow()->FK_MediaType_get() ) )
					]
					= new RemoteControlSet(pRow_RemoteControl->FK_DeviceTemplate_MediaType_DesignObj_getrow());
		}
	}

	vector<Row_MediaType *> vectRow_MediaType;
	m_pDatabase_pluto_main->MediaType_get()->GetRows("1=1",&vectRow_MediaType);
	for(size_t s=0;s<vectRow_MediaType.size();++s)
	{
		Row_MediaType *pRow_MediaType = vectRow_MediaType[s];
        m_mapMediaType_2_Directory[pRow_MediaType->PK_MediaType_get()]=pRow_MediaType->Subdirectory_get();
	}
}

//<-dceag-c409-b->

	/** @brief COMMAND: #409 - Save Bookmark */
	/** Save the current position as a bookmark */
		/** @param #17 PK_Users */
			/** The user to save this under, or 0 for public */
		/** @param #19 Data */
			/** The picture to save as the thumbnail, in jpg format.  If not specified the plugin will try to grab a frame from the media player */
		/** @param #29 PK_MediaType */
			/** The media type, if not specified it will get it from the stream */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */
		/** @param #163 Description */
			/** The description for this bookmark. */
		/** @param #188 Position */
			/** The position to save as.  For TV, CHAN: or PROG: or SERIES:.  Leave blank and the plugin will grab the current value */
		/** @param #225 Always */
			/** If true, then this is the start position */

void Media_Plugin::CMD_Save_Bookmark(int iPK_Users,char *pData,int iData_Size,int iPK_MediaType,string sPK_EntertainArea,string sDescription,string sPosition,bool bAlways,string &sCMD_Result,Message *pMessage)
//<-dceag-c409-e->
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	bool bDescriptionWasEmpty = sDescription.empty();

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(sPK_EntertainArea.c_str()) );

	if( pEntertainArea && pEntertainArea->m_pMediaStream && pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->m_pData->m_dwPK_Device!=m_dwPK_Device )
	{
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->m_pData->m_dwPK_Device);
		if( pDeviceData_Router && pDeviceData_Router->m_mapCommands.find(pMessage->m_dwID)!=pDeviceData_Router->m_mapCommands.end() )
		{
			// The plugin has it's own method for handling this.  Give it the message instead
			pMessage->m_dwPK_Device_To=pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->m_dwPK_Device;
			pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl->ReceivedMessage(pMessage);
			return;
		}
	}

	MediaStream *pMediaStream = pEntertainArea ? pEntertainArea->m_pMediaStream : NULL;
	if( pMediaStream && !iPK_MediaType )
		iPK_MediaType = pMediaStream->m_iPK_MediaType;

	MediaFile *pMediaFile=NULL;
	if( pMediaStream && !pMediaStream->m_dwPK_Disc && iPK_MediaType!=MEDIATYPE_pluto_LiveTV_CONST )
	{
		if( pMediaStream->m_dequeMediaFile.size()==0 ||
			pMediaStream->m_iDequeMediaFile_Pos<0 ||
			pMediaStream->m_iDequeMediaFile_Pos>=pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() ||
			(pMediaFile=pMediaStream->GetCurrentMediaFile())==NULL ||
			pMediaFile->m_dwPK_File==0)
		{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"size %d pos %d file %p %d",(int) pMediaStream->m_dequeMediaFile.size(),
						pMediaStream->m_iDequeMediaFile_Pos,pMediaFile,pMediaFile ? pMediaFile->m_dwPK_File : 0);
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(StringUtils::itos(pMessage->m_dwPK_Device_From),"Cannot identify the disc or file being played");
			SCREEN_DialogGenericError SCREEN_DialogGenericError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
				"Cannot identify the disc or file being played", "0", "0", "0");
			SendCommand(SCREEN_DialogGenericError);

			return;
		}
	}
/*  allow all bookmarking for now
	if( m_mapMediaType_Bookmarkable_Find(pMediaStream->m_iPK_MediaType)==false )
	{
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(StringUtils::itos(pMessage->m_dwPK_Device_From),"<%=T" + StringUtils::itos(TEXT_Cannot_bookmark_CONST) + "%> (MT:" + StringUtils::itos(pMediaStream->m_iPK_MediaType) + ")");
		SCREEN_DialogCannotBookmark SCREEN_DialogCannotBookmark(m_dwPK_Device, pMessage->m_dwPK_Device_From,
			"(MT:" + StringUtils::itos(pMediaStream->m_iPK_MediaType) + ")");
		SendCommand(SCREEN_DialogCannotBookmark);

		return;
	}
*/
	string sText;
	if( sPosition.empty() && pMediaStream && pEntertainArea )
	{
		DCE::CMD_Report_Playback_Position CMD_Report_Playback_Position(m_dwPK_Device,pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
			pMediaStream->m_iStreamID_get(),&sText,&sPosition);

		if( !SendCommand(CMD_Report_Playback_Position) || sPosition.size()==0 )
		{
			//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(StringUtils::itos(pMessage->m_dwPK_Device_From),"<%=T" + StringUtils::itos(TEXT_Cannot_bookmark_CONST) + "%>");
			SCREEN_DialogCannotBookmark SCREEN_DialogCannotBookmark(m_dwPK_Device, pMessage->m_dwPK_Device_From, "");
			SendCommand(SCREEN_DialogCannotBookmark);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot get current position from %d",pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
			return;
		}
	}

	string sFormat="JPG";
	if( pData==NULL && pMediaStream && pEntertainArea )
	{
		DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame(m_dwPK_Device,pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
			"0",pMediaStream->m_iStreamID_get(),800,800,&pData,&iData_Size,&sFormat);
		SendCommand(CMD_Get_Video_Frame);
	}

	if( sPosition.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Save_Bookmark no position");
		return;
	}

	Row_Picture *pRow_Picture = NULL;
	if( pData && iData_Size )
	{
		pRow_Picture = m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(pData,iData_Size,sFormat,"");
	}

	Row_Bookmark *pRow_Bookmark;
	if( bAlways )
	{
		vector<Row_Bookmark *> vectRow_Bookmark;
		if( pMediaStream && pMediaStream->m_dwPK_Disc )
			m_pDatabase_pluto_media->Bookmark_get()->GetRows(
				"Description='START' AND FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc),&vectRow_Bookmark);
		else if( pMediaFile )
			m_pDatabase_pluto_media->Bookmark_get()->GetRows(
				"Description='START' AND FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File),&vectRow_Bookmark);

		if( vectRow_Bookmark.size() )
			pRow_Bookmark = vectRow_Bookmark[0];
		else
			pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->AddRow();

		pRow_Bookmark->IsAutoResume_set(1);
		sDescription = "START";
	}
	else
	{
		if( pMediaStream && sDescription.empty() )
			sDescription = pMediaStream->m_sMediaDescription + " " + sText;
		// Be sure we don't already have this same thing bookmarked
		string sWhere = "Position='" + sPosition + "'";

		if( iPK_Users )
			sWhere += " AND EK_Users=" + StringUtils::itos(iPK_Users);
		else
			sWhere += " AND EK_Users IS NULL";

		if( pMediaStream && pMediaStream->m_dwPK_Disc )
			sWhere += " AND FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc);
		else if( pMediaFile )
			sWhere += " AND FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File);

		if( iPK_MediaType )
			sWhere += " AND EK_MediaType=" + StringUtils::itos(iPK_MediaType);

		vector<Row_Bookmark *> vectRow_Bookmark;
		m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);

		if( vectRow_Bookmark.size() )
			pRow_Bookmark = vectRow_Bookmark[0];
		else
			pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->AddRow();
	}

	pRow_Bookmark->Description_set( sDescription );

	if( pMediaStream && pMediaStream->m_dwPK_Disc )
		pRow_Bookmark->FK_Disc_set(pMediaStream->m_dwPK_Disc);
	else if( pMediaFile )
		pRow_Bookmark->FK_File_set(pMediaFile->m_dwPK_File);
	pRow_Bookmark->EK_MediaType_set(iPK_MediaType);
	if( pRow_Picture )
		pRow_Bookmark->FK_Picture_set(pRow_Picture->PK_Picture_get());
	pRow_Bookmark->Description_set(sDescription);
	pRow_Bookmark->Position_set(sPosition);
	if( iPK_Users )
		pRow_Bookmark->EK_Users_set(iPK_Users);
	m_pDatabase_pluto_media->Bookmark_get()->Commit();

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Save_Bookmark stream %p description %s start %d",
		pMediaStream,sDescription.c_str(),(int) bAlways);
#endif

	if( pMediaStream )
	{
		int PK_Screen = pMediaStream->GetRemoteControlScreen(pMessage->m_dwPK_Device_From);
		if( bDescriptionWasEmpty && !bAlways )
		{
			string sCmdToRenameBookmark= "<%=!%> -300 1 741 159 " + StringUtils::itos(PK_Screen) +
				"\n<%=!%> <%=V-106%> 1 411 5 \"<%=17%>\" 129 " + StringUtils::itos(pRow_Bookmark->PK_Bookmark_get());

			DCE::SCREEN_FileSave SCREEN_FileSave(m_dwPK_Device,pMessage->m_dwPK_Device_From,
				pMediaStream->m_iPK_MediaType,0, "<%=T" + StringUtils::itos(TEXT_Name_Bookmark_CONST) + "%>", sCmdToRenameBookmark, false);
			SendCommand(SCREEN_FileSave);
		}
	}
}

//<-dceag-c410-b->

	/** @brief COMMAND: #410 - Delete Bookmark */
	/** Delete a bookmark */
		/** @param #129 EK_Bookmark */
			/** The bookmark to delete */

void Media_Plugin::CMD_Delete_Bookmark(int iEK_Bookmark,string &sCMD_Result,Message *pMessage)
//<-dceag-c410-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow( iEK_Bookmark );
	if( pRow_Bookmark )
	{
		pRow_Bookmark->Delete();
		m_pDatabase_pluto_media->Bookmark_get()->Commit();
	}
}

//<-dceag-c411-b->

	/** @brief COMMAND: #411 - Rename Bookmark */
	/** Rename a bookmark */
		/** @param #5 Value To Assign */
			/** The new name */
		/** @param #17 PK_Users */
			/** The user, if this is a private bookmark */
		/** @param #129 EK_Bookmark */
			/** The bookmark */

void Media_Plugin::CMD_Rename_Bookmark(string sValue_To_Assign,int iPK_Users,int iEK_Bookmark,string &sCMD_Result,Message *pMessage)
//<-dceag-c411-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow(iEK_Bookmark);
	if( !pRow_Bookmark )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot retrieve bookmark");
		return;
	}

	pRow_Bookmark->Description_set(sValue_To_Assign);
	if( !iPK_Users )
		pRow_Bookmark->EK_Users_setNull(true);
	else
		pRow_Bookmark->EK_Users_set(iPK_Users);
	m_pDatabase_pluto_media->Bookmark_get()->Commit();
}

void Media_Plugin::AddFileToDatabase(MediaFile *pMediaFile,int PK_MediaType)
{
	if( StringUtils::StartsWith(pMediaFile->m_sFilename,"/dev/",true) ||
		StringUtils::StartsWith(pMediaFile->m_sFilename,"cdda:/",true) ||
		StringUtils::StartsWith(pMediaFile->m_sFilename,"http://",true) ||
		pMediaFile->m_sFilename.empty() )
			return;  // Don't add it if it's just a drive or a http link

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::AddFileToDatabase %s",pMediaFile->m_sFilename.c_str());

	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" +
		StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(pMediaFile->m_sPath)) +
		"' AND Filename='" + StringUtils::SQLEscape(pMediaFile->m_sFilename) + "'",
		&vectRow_File);

	if(vectRow_File.size()) //the file already exists
		return;

	int nEK_Users_Private = MediaAttributes_LowLevel::GetOwnerForPath(FileUtils::ExcludeTrailingSlash(pMediaFile->m_sPath));

	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
	pRow_File->DateAdded_set(StringUtils::SQLDateTime(time(NULL)));
	pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(pMediaFile->m_sPath));
	pRow_File->Filename_set(pMediaFile->m_sFilename);
	pRow_File->EK_MediaType_set(PK_MediaType);
	pRow_File->INode_set( FileUtils::GetInode( pMediaFile->m_sPath + "/" + pMediaFile->m_sFilename ) );

	if(nEK_Users_Private != 0)
		pRow_File->EK_Users_Private_set(nEK_Users_Private);
	else
		pRow_File->EK_Users_Private_setNull(true);

	pRow_File->Table_File_get()->Commit();
	pMediaFile->m_dwPK_File = pRow_File->PK_File_get();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::AddFileToDatabase %s PK_File %d Inode %d",
		pMediaFile->m_sFilename.c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Position the current media to a bookmark */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void Media_Plugin::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to play media
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea, &iStreamID );
	Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow( atoi(sMediaPosition.c_str()) );
	EntertainArea *pEntertainArea;
	if( vectEntertainArea.size()==0 || (pEntertainArea=vectEntertainArea[0])==NULL || !pEntertainArea->m_pMediaStream || !pRow_Bookmark )
	{
		// Could be a timing issue that the stream finished and Orbiter didn't change the screen yet
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't go to media position -- no EA or stream");
		return;
	}

	DCE::CMD_Set_Media_Position CMD_Set_Media_Position(m_dwPK_Device,pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
		iStreamID,pRow_Bookmark->Position_get());
	SendCommand(CMD_Set_Media_Position);
}
//<-dceag-c417-b->

	/** @brief COMMAND: #417 - Set Auto Resume Options */
	/** Specify if the user should be prompted to resume the given media type or not */
		/** @param #5 Value To Assign */
			/** Valid values are: NEVER, ALWAYS, PROMPT */
		/** @param #17 PK_Users */
			/** The user to set */
		/** @param #29 PK_MediaType */
			/** The media type */

void Media_Plugin::CMD_Set_Auto_Resume_Options(string sValue_To_Assign,int iPK_Users,int iPK_MediaType,string &sCMD_Result,Message *pMessage)
//<-dceag-c417-e->
{
	m_mapPromptResume[ make_pair<int,int> (iPK_Users,iPK_MediaType) ] = (sValue_To_Assign.size() ? sValue_To_Assign[0] : 'P');
	SaveMediaResumePreferences();
}

void Media_Plugin::SaveLastPlaylistPosition(MediaStream *pMediaStream)
{
	string sWhere = "EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) + " AND FK_Playlist=" + StringUtils::itos(pMediaStream->m_iPK_Playlist) + " AND IsAutoResume=1 AND Description<>'START'";

	Row_Bookmark *pRow_Bookmark=NULL;
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);
	if( vectRow_Bookmark.size() ) // Should only be 1
		pRow_Bookmark=vectRow_Bookmark[0];
	else
	{
		pRow_Bookmark=m_pDatabase_pluto_media->Bookmark_get()->AddRow();
		pRow_Bookmark->FK_Playlist_set(pMediaStream->m_iPK_Playlist);
		pRow_Bookmark->EK_Users_set(pMediaStream->m_iPK_Users);
		pRow_Bookmark->Description_set("Auto resume");
		pRow_Bookmark->IsAutoResume_set(1);
	}

	pRow_Bookmark->Position_set(" QUEUE_POS:" + StringUtils::itos(pMediaStream->m_iDequeMediaFile_Pos) + " " + pMediaStream->m_sLastPosition);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::SaveLastPlaylistPosition %s QUEUE_POS: %d %s",sWhere.c_str(),pMediaStream->m_iDequeMediaFile_Pos,pMediaStream->m_sLastPosition.c_str());
#endif
	m_pDatabase_pluto_media->Bookmark_get()->Commit();
}

void Media_Plugin::SaveLastDiscPosition(MediaStream *pMediaStream)
{
	string sWhere = "EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) + " AND FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc) + " AND IsAutoResume=1 AND Description<>'START'";

	Row_Bookmark *pRow_Bookmark=NULL;
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);
	if( vectRow_Bookmark.size() ) // Should only be 1
		pRow_Bookmark=vectRow_Bookmark[0];
	else
	{
		pRow_Bookmark=m_pDatabase_pluto_media->Bookmark_get()->AddRow();
		pRow_Bookmark->FK_Disc_set(pMediaStream->m_dwPK_Disc);
		pRow_Bookmark->EK_Users_set(pMediaStream->m_iPK_Users);
		pRow_Bookmark->Description_set("Auto resume");
		pRow_Bookmark->IsAutoResume_set(1);
	}

	pRow_Bookmark->Position_set(" QUEUE_POS:" + StringUtils::itos(pMediaStream->m_iDequeMediaFile_Pos) + " " + pMediaStream->m_sLastPosition);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::SaveLastDiscPosition %s QUEUE_POS: %d %s",sWhere.c_str(),pMediaStream->m_iDequeMediaFile_Pos,pMediaStream->m_sLastPosition.c_str());
#endif
	m_pDatabase_pluto_media->Bookmark_get()->Commit();
}

void Media_Plugin::SaveLastFilePosition(MediaStream *pMediaStream)
{
	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( !pMediaFile->m_dwPK_File )
		return; // The file isn't in the database

	string sWhere = "EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) + " AND FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND IsAutoResume=1 AND Description<>'START'";

	Row_Bookmark *pRow_Bookmark=NULL;
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);
	if( vectRow_Bookmark.size() ) // Should only be 1
		pRow_Bookmark=vectRow_Bookmark[0];
	else
	{
		pRow_Bookmark=m_pDatabase_pluto_media->Bookmark_get()->AddRow();
		pRow_Bookmark->FK_File_set(pMediaFile->m_dwPK_File);
		pRow_Bookmark->EK_Users_set(pMediaStream->m_iPK_Users);
		pRow_Bookmark->Description_set("Auto resume");
		pRow_Bookmark->IsAutoResume_set(1);
	}

	pRow_Bookmark->Position_set(pMediaStream->m_sLastPosition);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::SaveLastDiscPosition %s %s",sWhere.c_str(),pMediaStream->m_sLastPosition.c_str());
#endif
	m_pDatabase_pluto_media->Bookmark_get()->Commit();
}

void Media_Plugin::RestoreMediaResumePreferences()
{
	DCEConfig dceconfig;
	for(map<string,string>::const_iterator it=dceconfig.Parameters().begin();it!=dceconfig.Parameters().end();++it)
	{
		if( StringUtils::StartsWith(it->first,"auto_resume_user_") )
		{
			int PK_Users = atoi( it->first.substr(17).c_str() );
			string::size_type pos=0;
			while(pos<it->second.length())
			{
				string sInput = it->second;
				string sToken = StringUtils::Tokenize(sInput,",",pos);
				string::size_type pos_equal = sToken.find('-');
				if( pos_equal==string::npos )
					return;
				int PK_MediaType = atoi(sToken.c_str());
				m_mapPromptResume[ make_pair<int,int> (PK_Users,PK_MediaType) ] = sToken[pos_equal+1];
			}
		}
	}
}

void Media_Plugin::SaveMediaResumePreferences()
{
	map<int,string> mapSettings;
	for( map< pair<int,int>,char >::iterator it=m_mapPromptResume.begin();it!=m_mapPromptResume.end();++it )
	{
		mapSettings[ it->first.first ] = mapSettings[ it->first.first ] + StringUtils::itos(it->first.second) + "-" + it->second + ",";
        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::SaveMediaResumePreferences m_mapPromptResume it->first.first %d %d-%c",
			it->first.first,it->first.second,it->second);
	}

	DCEConfig dceconfig;

	for(map<int,string>::iterator it=mapSettings.begin();it!=mapSettings.end();++it)
	{
		string s = "auto_resume_user_" + StringUtils::itos(it->first);
		dceconfig.AddString(s,it->second);
        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::SaveMediaResumePreferences add string %s=%s",
			s.c_str(),it->second.c_str());
	}
	dceconfig.WriteSettings();
}

int Media_Plugin::CheckForAutoResume(MediaStream *pMediaStream)
{
	string sPosition;
	Row_Bookmark *pRow_Bookmark=NULL;
	vector<Row_Bookmark *> vectRow_Bookmark;
	// See if this is a disc to resume
	if( pMediaStream->m_dwPK_Disc )
	{
		m_pDatabase_pluto_media->Bookmark_get()->GetRows(
			"EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) +
			" AND FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc) + " AND IsAutoResume=1 AND Description<>'START'",
			&vectRow_Bookmark);

		if( vectRow_Bookmark.size() )
		{
			sPosition = vectRow_Bookmark[0]->Position_get();
			vectRow_Bookmark.clear();
		}
	}

	// Or a playlist
	if( pMediaStream->m_iPK_Playlist )
	{
		m_pDatabase_pluto_media->Bookmark_get()->GetRows(
			"EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) +
			" AND FK_Playlist=" + StringUtils::itos(pMediaStream->m_iPK_Playlist) + " AND IsAutoResume=1 AND Description<>'START'",
			&vectRow_Bookmark);

		if( vectRow_Bookmark.size() )
		{
			sPosition = vectRow_Bookmark[0]->Position_get() + sPosition;  // Just append in case we already got one
			vectRow_Bookmark.clear();
		}
	}

	// Either one of those could have set a position in the queue to resume
	string::size_type queue_pos = sPosition.find(" QUEUE_POS:");
	if( queue_pos!=string::npos )
	{
		int pos = atoi( sPosition.substr(queue_pos+11).c_str() );
		if( pos>=0 && pos<pMediaStream->m_dequeMediaFile.size() )
			pMediaStream->m_iDequeMediaFile_Pos=pos;
	}

	MediaFile *pMediaFile=pMediaStream->GetCurrentMediaFile();
	if( pMediaFile )
	{
		if( StringUtils::StartsWith(pMediaFile->m_sPath,"/home/public/data/samples/",true) )  // Don't resume the samples
			return 0;

		if( pMediaFile->m_dwPK_File )
		{
			m_pDatabase_pluto_media->Bookmark_get()->GetRows(
				"EK_Users=" + StringUtils::itos(pMediaStream->m_iPK_Users) +
				" AND FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND IsAutoResume=1 AND Description<>'START'",
				&vectRow_Bookmark);

			if( vectRow_Bookmark.size() )
			{
				sPosition = vectRow_Bookmark[0]->Position_get() + sPosition;  // Just append in case we already got one
				vectRow_Bookmark.clear();
			}
		}
	}

	if( sPosition.size()==0 || m_mapPromptResume[make_pair<int,int> (pMediaStream->m_iPK_Users,pMediaStream->m_iPK_MediaType)]=='N' )
		return 0; // Nothing to resume anyway, we only restored so far the queue if this was a playlist
	if( m_mapPromptResume[make_pair<int,int> (pMediaStream->m_iPK_Users,pMediaStream->m_iPK_MediaType)]=='A' )
	{
		if( pMediaFile )
			pMediaFile->m_sStartPosition = sPosition;
		return 0; // Resume automatically
	}

	int iPK_Device_Orbiter=0;
	if( !pMediaStream->m_pOH_Orbiter_StartedMedia )
		return 0; // can't ask if there's no orbiter
	else
		iPK_Device_Orbiter = pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device;

	/*
	string sMessageToResume = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device)
		+ " 1 " + StringUtils::itos(COMMAND_Set_Media_Position_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_StreamID_CONST) + " " + StringUtils::itos(pMediaStream->m_iStreamID_get()) + " "
		+ StringUtils::itos(COMMANDPARAMETER_MediaPosition_CONST) + " \"" + sPosition + "\"";

	string sMessageToGoToRemote = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(iPK_Device_Orbiter)
		+ " 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " <%=NP_R%>";

	string sMessageToSetPreference = StringUtils::itos(iPK_Device_Orbiter) + " " + StringUtils::itos(m_dwPK_Device)
		+ " 1 " + StringUtils::itos(COMMAND_Set_Auto_Resume_Options_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_Users_CONST) + " " + StringUtils::itos(pMediaStream->m_iPK_Users) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_MediaType_CONST) + " " + StringUtils::itos(pMediaStream->m_iPK_MediaType) + " "
		+ StringUtils::itos(COMMANDPARAMETER_Value_To_Assign_CONST);

	// Prompt the user
	m_pOrbiter_Plugin->DisplayMessageOnOrbiter(iPK_Device_Orbiter,"<%=T" + StringUtils::itos(TEXT_Ask_to_resume_CONST) + "%>",
		false,30,true,
		"<%=T" + StringUtils::itos(TEXT_YES_CONST) + "%>",sMessageToResume + "\n" + sMessageToGoToRemote,
		"<%=T" + StringUtils::itos(TEXT_NO_CONST) + "%>",sMessageToGoToRemote,
		"<%=T" + StringUtils::itos(TEXT_Always_Resume_CONST) + "%>",sMessageToResume + "\n" + sMessageToGoToRemote + "\n" + sMessageToSetPreference + " A",
		"<%=T" + StringUtils::itos(TEXT_Never_Resume_CONST) + "%>",sMessageToGoToRemote + "\n" + sMessageToSetPreference + " N");
	*/

	RemoteControlSet *pRemoteControlSet = PickRemoteControlMap(
		iPK_Device_Orbiter,
		pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceTemplate,
		pMediaStream->m_iPK_MediaType);
	if( !pRemoteControlSet )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CheckForAutoResume Cannot find remote controls for Orbiter %d",iPK_Device_Orbiter);
		return 0;
	}
	pMediaStream->m_mapRemoteControlSet[iPK_Device_Orbiter]=pRemoteControlSet;
	EntertainArea *pEntertainArea_OSD=NULL;
	bool bIsOSD=pMediaStream->OrbiterIsOSD(iPK_Device_Orbiter,&pEntertainArea_OSD);

	// HACK this should be refactored, but duplicating code for now with Xine_Plugin to make bookmarks work
	string mediaURL = pMediaStream->GetFilenameToPlay("Empty file name");
	bool bRedirectToMPlayer = false;
/*
	bRedirectToMPlayer = ( pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_HDDVD_CONST ) ||
		( pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_BD_CONST ) ||
		( (pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_StoredVideo_CONST) &&
		  ( StringUtils::EndsWith(mediaURL, ".EVO", true) || StringUtils::EndsWith(mediaURL, ".M2TS", true) ) );
*/
	MediaDevice *pMediaDevice_MPlayer = NULL;
	if (bRedirectToMPlayer)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Movie is known to be HD-DVD or Bluray, trying to redirect to MPlayer it on resume");

		pMediaDevice_MPlayer = m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_MPlayer_Player_CONST, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device));
		if (!pMediaDevice_MPlayer)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to find MPlayer for redirect, cancelling redirect :(");
			bRedirectToMPlayer = false;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found MPlayer for redirect, device #%i", pMediaDevice_MPlayer->m_pDeviceData_Router->m_dwPK_Device);
		}
	}


	SCREEN_DialogAskToResume SCREEN_DialogAskToResume(m_dwPK_Device, iPK_Device_Orbiter,
		StringUtils::ltos(m_dwPK_Device),
		StringUtils::ltos( bRedirectToMPlayer?(pMediaDevice_MPlayer->m_pDeviceData_Router->m_dwPK_Device):(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device) ),
		StringUtils::itos(pMediaStream->m_iStreamID_get()),
		sPosition,
		StringUtils::itos(pMediaStream->m_iPK_Users),
		StringUtils::itos(pMediaStream->m_iPK_MediaType),
		bIsOSD && pRemoteControlSet->m_iPK_Screen_OSD ? pRemoteControlSet->m_iPK_Screen_OSD : pRemoteControlSet->m_iPK_Screen_Remote);
	SendCommand(SCREEN_DialogAskToResume);

	return iPK_Device_Orbiter;
}

//<-dceag-sample-b->!

//<-dceag-c623-b->

	/** @brief COMMAND: #623 - Shuffle */
	/** Randomizes the order of the current playlist. */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment Area to send the shuffle command to. If not specified, This is assumed to come from an orbiter, and will be automatically determined. */

void Media_Plugin::CMD_Shuffle(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c623-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, (sPK_EntertainArea.empty() ? "" : sPK_EntertainArea), vectEntertainArea );
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
		if( !pEntertainArea->m_pMediaStream )
			continue; // Don't know what area it should be played in, or there's no media playing there

		int NumFiles=pEntertainArea->m_pMediaStream->m_dequeMediaFile.size();
		if( NumFiles<2 )
			return;

		for(size_t sPos=0;sPos<NumFiles;++sPos)
		{
			int NewPos = (int) ((double) rand() * NumFiles / RAND_MAX);
			if( NewPos>=NumFiles )
				NewPos=NumFiles-1;
			if( NewPos==sPos )
				continue;  // Don't move this
			// Swap positions
			MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->m_dequeMediaFile[sPos];
			pEntertainArea->m_pMediaStream->m_dequeMediaFile[sPos] = pEntertainArea->m_pMediaStream->m_dequeMediaFile[NewPos];
			pEntertainArea->m_pMediaStream->m_dequeMediaFile[NewPos] = pMediaFile;
		}
		pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos=0;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Calling Start Media from shuffle");
		string sError;
		pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pMediaHandlerBase->StartMedia(pEntertainArea->m_pMediaStream,sError);
		CheckStreamForTimeout(pEntertainArea->m_pMediaStream);
		MediaInfoChanged(pEntertainArea->m_pMediaStream,true);
	}
}
//<-dceag-c742-b->

	/** @brief COMMAND: #742 - Media Identified */
	/** A disc has been identified */
		/** @param #2 PK_Device */
			/** The disk drive */
		/** @param #5 Value To Assign */
			/** The identified data */
		/** @param #10 ID */
			/** The ID of the disc */
		/** @param #19 Data */
			/** The picture/cover art */
		/** @param #20 Format */
			/** The format of the data */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #59 MediaURL */
			/** The URL for the disc drive */
		/** @param #131 EK_Disc */
			/** If a disc was added accordingly, this reports the disc id */
		/** @param #193 URL */
			/** The URL for the picture */

void Media_Plugin::CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc,string &sCMD_Result,Message *pMessage)
//<-dceag-c742-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media was identified id %s device %d format %s",sID.c_str(),iPK_Device,sFormat.c_str());

	int Priority=1;
	DeviceData_Router *pDevice_ID = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From);
	if( pDevice_ID )
		Priority = atoi( pDevice_ID->m_mapParameters_Find(DEVICEDATA_Priority_CONST).c_str() );

	int PK_File=0;

	DeviceData_Router *pDevice_Disk_Drive = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( !pDevice_Disk_Drive )
	{
		// There's no disk drive involved.  See if there's a file
		PK_File = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetFileIDFromFilePath( sMediaURL );
		if( !PK_File )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find the disk drive device identified, or a matching file");
			return;
		}
	}

	// Find the media stream
	MediaStream *pMediaStream=NULL;
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMS = (*it).second;
		// If the disk matches, and the drive is either the source, or a sibbling of the source, this is the stream
		if( pMS->m_discid==atoi(sID.c_str()) &&
			(pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device==iPK_Device ||
			pMS->m_pMediaDevice_Source->m_pDeviceData_Router->m_pDevice_ControlledVia==pDevice_Disk_Drive->m_pDevice_ControlledVia ||
			pMS->m_pMediaDevice_Source->m_pDeviceData_Router->GetTopMostDevice() == pDevice_Disk_Drive->GetTopMostDevice() // The disc drive is no longer a sibbling of the media player under Orbiter, it may be 1 level higher
			) )
		{
			pMediaStream=pMS;
			break;
		}
	}

	listMediaAttribute listMediaAttribute_;
	int PK_Disc=0;
	if( sFormat=="CDDB-TAB" && pMediaStream && pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
		PK_Disc=m_pMediaAttributes->m_pMediaAttributes_LowLevel->Parse_CDDB_Media_ID(iPK_MediaType,listMediaAttribute_,sValue_To_Assign);
	if( sFormat=="MISC-TAB" )
		PK_Disc=m_pMediaAttributes->m_pMediaAttributes_LowLevel->Parse_Misc_Media_ID(iPK_MediaType,listMediaAttribute_,sValue_To_Assign,PK_File);
	*iEK_Disc = PK_Disc;

	if( pData && iData_Size )
	{
		if( PK_File )
			m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToFile(PK_File,pData,iData_Size,sURL);
		else
			m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToDisc(PK_Disc,pData,iData_Size,sURL);
	}

	if( !pMediaStream )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Media_Identified Disc is not currently playing");
		return;  // Nothing more to do
	}

	if( pMediaStream->m_IdentifiedPriority && (pMediaStream->m_IdentifiedPriority >= Priority) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media already identified by a higher priority %d (%d)",pMediaStream->m_IdentifiedPriority,Priority);
		return;
	}
	pMediaStream->m_IdentifiedPriority=Priority;

	if( PK_Disc )
	{
		pMediaStream->m_dwPK_Disc = PK_Disc;
		m_pMediaAttributes->LoadStreamAttributesForDisc(pMediaStream);
		int iPK_Orbiter_PromptingToResume = CheckForAutoResume(pMediaStream);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Media_Identified disc %d resume on orbiter %d",PK_Disc,iPK_Orbiter_PromptingToResume);
#endif
                // notifying player that disk ID changed
                string sMediaID = "D"+StringUtils::itos(PK_Disc);
                DCE::CMD_Set_Media_ID cmd(m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, sMediaID, pMediaStream->m_iStreamID_get());
                SendCommand(cmd);
	}

	m_pMediaAttributes->m_pMediaAttributes_LowLevel->PurgeListMediaAttribute(listMediaAttribute_);
	if( pMediaStream->m_mapPK_Attribute.find(ATTRIBUTETYPE_Performer_CONST)!=pMediaStream->m_mapPK_Attribute.end() ||
		pMediaStream->m_mapPK_Attribute.find(ATTRIBUTETYPE_Album_CONST)!=pMediaStream->m_mapPK_Attribute.end() ||
		pMediaStream->m_mapPK_Attribute.find(ATTRIBUTETYPE_Title_CONST)!=pMediaStream->m_mapPK_Attribute.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Media_Identified stream is now identified disc");
		pMediaStream->m_bIdentifiedDisc=true;
	}
	pMediaStream->UpdateDescriptions(true);
	MediaInfoChanged(pMediaStream,true);
}

void Media_Plugin::AlarmCallback(int id, void* param)
{
	PLUTO_SAFETY_LOCK(sm,m_MediaMutex);
	if( id==MEDIA_PLAYBACK_TIMEOUT )
	{
		MediaStream *pMediaStream = m_mapMediaStream_Find((int)(long) param,0);
		if( pMediaStream )
			ProcessMediaFileTimeout(pMediaStream);
	}
	else if( id==CHECK_FOR_NEW_FILES )
		CMD_Check_For_New_Files();
	else if( id==UPDATE_ATTRIBUTE_CACHE )
	  m_pMediaAttributes->m_pMediaAttributes_LowLevel->UpdateAttributeCache();
	else if( id==WAITING_FOR_JUKEBOX )
	{
		MediaStream *pMediaStream = m_mapMediaStream_Find((int)(long) param,0);
		if( pMediaStream )
			WaitingForJukebox( pMediaStream);
	}
	else if( id==UPDATE_VIEW_DATE )
	{
		MediaStream *pMediaStream = m_mapMediaStream_Find((int)(long) param,0);
		if( pMediaStream )
			UpdateViewDate(pMediaStream);
	}
	else if( id==UPDATE_SEARCH_TOKENS )
		UpdateSearchTokens();
}

void Media_Plugin::ProcessMediaFileTimeout(MediaStream *pMediaStream)
{
	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( !pMediaFile || pMediaFile->m_tTimeout>time(NULL) )
	{
time_t k = time(NULL);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring timeout for file %p",pMediaFile);
		return;
	}

	string sPosition;
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
		pMediaStream->m_iStreamID_get(),&sPosition);  // Ignore the position since we are forcing the stop
	SendCommand(CMD_Stop_Media);
}

void Media_Plugin::CheckStreamForTimeout(MediaStream *pMediaStream)
{
	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( !pMediaFile )
		return;

	if( pMediaFile->m_dwPK_CommandGroup_Start )
		QueueMessageToRouter(new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,
			MESSAGETYPE_EXEC_COMMAND_GROUP,pMediaFile->m_dwPK_CommandGroup_Start,0));

	if( pMediaFile->m_dwDuration==0 )
		return;

	pMediaFile->m_tTimeout = time(NULL) + pMediaFile->m_dwDuration -1; // Subtract 1 to prevent a rounding issue
	int StreamID = pMediaStream->m_iStreamID_get( );
	m_pAlarmManager->AddRelativeAlarm(pMediaFile->m_dwDuration,this,MEDIA_PLAYBACK_TIMEOUT,(void *) StreamID);
}

void Media_Plugin::UpdateViewDate(MediaStream *pMediaStream)
{
	for(deque<MediaFile *>::iterator it=pMediaStream->m_dequeMediaFile.begin();it!=pMediaStream->m_dequeMediaFile.end();++it)
	{
		MediaFile *pMediaFile = *it;
		int PK_File=0,PK_Disc=0; // Bookmarks may be used
		if( pMediaFile->m_dwPK_Bookmark && !pMediaFile->m_dwPK_File && !pMediaFile->m_dwPK_Disk )
		{
			Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow(pMediaFile->m_dwPK_Bookmark);
			if( pRow_Bookmark )
			{
				PK_File=pRow_Bookmark->FK_File_get();
				PK_Disc=pRow_Bookmark->FK_Disc_get();
			}
		}

		if( pMediaFile->m_dwPK_File || PK_File )
		{
			Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow( pMediaFile->m_dwPK_File ? pMediaFile->m_dwPK_File : PK_File );
			if( pRow_File )
				pRow_File->DateLastViewed_set( StringUtils::SQLDateTime(time(NULL)) );
		}
		if( pMediaFile->m_dwPK_Disk || PK_Disc )
		{
			Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow( pMediaFile->m_dwPK_Disk ? pMediaFile->m_dwPK_Disk : PK_Disc );
			if( pRow_Disc )
				pRow_Disc->DateLastViewed_set( StringUtils::SQLDateTime(time(NULL)) );
		}
		m_pDatabase_pluto_media->File_get()->Commit();
		m_pDatabase_pluto_media->Disc_get()->Commit();
	}
}

//<-dceag-c780-b->

	/** @brief COMMAND: #780 - Remove playlist */
	/** Removes a playlist from database */
		/** @param #78 EK_Playlist */
			/** The id of the playlist to be removed. */

void Media_Plugin::CMD_Remove_playlist(int iEK_Playlist,string &sCMD_Result,Message *pMessage)
//<-dceag-c780-e->
{
	vector<Row_PlaylistEntry *> vectPlaylistEntry;
	m_pDatabase_pluto_media->PlaylistEntry_get()->GetRows("FK_Playlist = " + StringUtils::ltos(iEK_Playlist),
		&vectPlaylistEntry);
	for(vector<Row_PlaylistEntry *>::iterator it = vectPlaylistEntry.begin(); it != vectPlaylistEntry.end(); ++it)
	{
		Row_PlaylistEntry* pRow_PlaylistEntry = *it;
		pRow_PlaylistEntry->Delete();
	}

	m_pDatabase_pluto_media->PlaylistEntry_get()->Commit();

	Row_Playlist *pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(iEK_Playlist);
	if(NULL != pRow_Playlist)
	{
		pRow_Playlist->Delete();
		m_pDatabase_pluto_media->Playlist_get()->Commit();
	}
}
//<-dceag-c807-b->

	/** @brief COMMAND: #807 - Get Attributes For Media */
	/** Get the attributes for a file */
		/** @param #5 Value To Assign */
			/** A tab delimited list of attributes: Attribute type \t Name \t type ... */
		/** @param #13 Filename */
			/** The file to retrieve the attributes for.  Can be a filename, or !F + PK_File, or a URL.  If not specified, specify the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** Gets the currently active media in this entertainment area.  If not specified, specify file */

void Media_Plugin::CMD_Get_Attributes_For_Media(string sFilename,string sPK_EntertainArea,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c807-e->
{
	MediaFile *pMediaFile = NULL;
	if( sFilename.empty() )
	{
		EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(sPK_EntertainArea.c_str()) );
		if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Get_Attributes_For_Media no filename nor valid EA w/ stream %s (%p) specified",sPK_EntertainArea.c_str(),pEntertainArea);
			return;
		}
		pMediaFile = pEntertainArea->m_pMediaStream->GetCurrentMediaFile();
	}
	else if( StringUtils::StartsWith(sFilename,"!P") )  // Playlists are handled differently
	{
		Row_Playlist *pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(atoi(sFilename.substr(2).c_str()));
		if( pRow_Playlist )
		{
			*sValue_To_Assign = "FILE\t" + sFilename +
			"\tTITLE\t" + pRow_Playlist->Name_get() +
			"\t";
			if( pRow_Playlist->FK_Picture_get() )
				*sValue_To_Assign += "PICTURE\t/home/mediapics/" + StringUtils::itos(pRow_Playlist->FK_Picture_get()) + ".jpg\t";
		}
		else
			*sValue_To_Assign = "FILE\t" + sFilename + "\tTITLE\t" + sFilename + "\t";
		return;
	}
	else if( StringUtils::StartsWith(sFilename,"!E") || StringUtils::StartsWith(sFilename,"!o") )  // external media
	{
		string::size_type pos=0;
		string sType = StringUtils::Tokenize(sFilename,",",pos);
		string sPK_MediaSource = StringUtils::Tokenize(sFilename,",",pos);
		MediaHandlerBase *pMediaHandlerBase = m_mapMediaCatalog_Find(sPK_MediaSource);
		if( pMediaHandlerBase )
			pMediaHandlerBase->GetExtendedAttributes(sType, sPK_MediaSource, sFilename.substr(pos), sValue_To_Assign);
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Get_Attributes_For_Media Bad media source %s", sPK_MediaSource.c_str());
		return;

	}
    deque<MediaFile *> dequeMediaFile;
	TransformFilenameToDeque(sFilename, dequeMediaFile);  // This will convert any !A, !F, !B etc.
	if( dequeMediaFile.size() )
		pMediaFile = dequeMediaFile[0];

	if( !pMediaFile )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Get_Attributes_For_Media no valid file found %s",sFilename.c_str());
		return;
	}

	int PK_Picture=0;
	string sExtension;
	if( pMediaFile->m_dwPK_Disk )
	{
		*sValue_To_Assign = "FILE\tDisc #" + StringUtils::itos(pMediaFile->m_dwPK_Disk) +
			"\tTITLE\t" + m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetDefaultDescriptionForMediaFile(pMediaFile) +
			"\t";
		sExtension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromDiscID(pMediaFile->m_dwPK_Disk, &PK_Picture);
	}
	else
	{
		*sValue_To_Assign = "FILE\t" + pMediaFile->HumanReadableFullyQualifiedFile() +
		  "\tPATH\t" + pMediaFile->m_sPath +
		  "\tFILENAME\t" + pMediaFile->m_sFilename +
			"\tTITLE\t" + m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetDefaultDescriptionForMediaFile(pMediaFile) +
			"\t";
		// Get attribute values for this media
		vector<Row_Attribute *> vectRow_Attribute;
		m_pDatabase_pluto_media->Attribute_get()->GetRows("JOIN File_Attribute ON FK_Attribute=PK_Attribute WHERE FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File),&vectRow_Attribute);
		if( vectRow_Attribute.size() )
		{
			for (int i = 0; i < vectRow_Attribute.size(); i++)
			{
				*sValue_To_Assign += StringUtils::ToUpper(vectRow_Attribute[i]->FK_AttributeType_getrow()->Description_get()) + "\t" +
					vectRow_Attribute[i]->Name_get() + "\t";
			}
		}
		
		vector<Row_LongAttribute *> vectRow_LongAttribute;
		m_pDatabase_pluto_media->LongAttribute_get()->GetRows("FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Synopsis_CONST),&vectRow_LongAttribute);
		if( vectRow_LongAttribute.size() )
		{
			string sSynopsis = vectRow_LongAttribute[0]->Text_get();
			StringUtils::Replace(&sSynopsis,"\t","");
			*sValue_To_Assign += "SYNOPSIS\t" + sSynopsis + "\t";
		}
		sExtension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(pMediaFile->m_dwPK_File, &PK_Picture);
	}

	if (pMediaFile->m_dwPK_MediaType == MEDIATYPE_pluto_Pictures_CONST)
	{
		*sValue_To_Assign += "PICTURE\t"+pMediaFile->FullyQualifiedFile()+ "\t";
	}
	else
	{
		if( PK_Picture )
			*sValue_To_Assign += "PICTURE\t/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + (sExtension.empty() ? "jpg" : sExtension) + "\t";
	}
	// cleanup
	for (deque<MediaFile *>::iterator it = dequeMediaFile.begin(); it != dequeMediaFile.end(); ++it)
		delete *it;

}
//<-dceag-c817-b->

	/** @brief COMMAND: #817 - Get Default Ripping Info */
	/** Get default ripping info: default filename, id and name of the storage device with most free space. */
		/** @param #13 Filename */
			/** Default ripping name. */
		/** @param #53 UseDefault */
			/** If true, use the default directory structure for public/private */
		/** @param #131 EK_Disc */
			/** The disc to rip.  If not specified, it will be whatever is playing in the entertainment area that sent this */
		/** @param #219 Path */
			/** Base path for ripping. */
		/** @param #233 DriveID */
			/** The id of the storage device with most free space. */
		/** @param #234 Directory */
			/** The directory for this, such as video, audio, etc. */
		/** @param #235 Storage Device Name */
			/** The name of the storage device with most free space. */

void Media_Plugin::CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name,string &sCMD_Result,Message *pMessage)
//<-dceag-c817-e->
{
	*sFilename = "Unknown disk";

	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	if( iEK_Disc && iEK_Disc!=-999 )
	{
		Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(iEK_Disc);
		if( !pRow_Disc )
		{
	        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Media_Plugin::CMD_Get_Default_Ripping_Info - invalid disc %d",iEK_Disc );
			return;
		}
		*sFilename=""; // May not have been empty
		m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetDefaultRippingName(pRow_Disc,sFilename,sDirectory);
	}
	else
	{
		vector<EntertainArea *> vectEntertainArea;
		DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea);
		if(vectEntertainArea.size() == 1)
		{
			EntertainArea *pEntertainArea = vectEntertainArea[0];
			if(NULL != pEntertainArea && NULL != pEntertainArea->m_pMediaStream)
			{
				MediaStream *pMediaStream = pEntertainArea->m_pMediaStream;
				if( pMediaStream->m_bIdentifiedDisc )
				{
					if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
					{
						list_int *listPK_Attribute_Performer = pMediaStream->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Performer_CONST);
						list_int *listPK_Attribute_Album = pMediaStream->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Album_CONST);
						int PK_Attribute_Performer = listPK_Attribute_Performer && listPK_Attribute_Performer->size() ? *(listPK_Attribute_Performer->begin()) : 0;
						int PK_Attribute_Album = listPK_Attribute_Album && listPK_Attribute_Album->size() ? *(listPK_Attribute_Album->begin()) : 0;

						*sFilename = FileUtils::ValidFileName(m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(PK_Attribute_Performer));
						if(sFilename->size())
							*sFilename += "/"; // We got a performer

						*sFilename += FileUtils::ValidFileName(m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(PK_Attribute_Album));
						*sDirectory = "audio";
					}
					else if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST )
					{
						*sFilename = FileUtils::ValidFileName(pMediaStream->m_sMediaDescription);
						*sDirectory = "videos";
					}

				}
				else if( pMediaStream->m_iPK_Playlist )
					*sFilename = FileUtils::ValidFileName(pMediaStream->m_sPlaylistName);
			}
		}
	}

	mm.Release();

	*iDriveID = GetStorageDeviceWithMostFreeSpace(*sStorage_Device_Name, *sPath);

	// See if this is one where we use Pluto's directory structure (incl the Core).  If not, we won't save this in a sub-directory except what the user chooses
	DeviceData_Router *pDevice_Drive = m_pRouter->m_mapDeviceData_Router_Find(*iDriveID);
	if( pDevice_Drive && pDevice_Drive->m_mapParameters_Find(DEVICEDATA_PK_Users_CONST)!="-1" && pDevice_Drive->WithinCategory(DEVICECATEGORY_Core_CONST)==false )
		*bUseDefault=false;
	else
		*bUseDefault=true;
}

//<-dceag-c819-b->

	/** @brief COMMAND: #819 - Get ID from Filename */
	/** Given a filename, get the ID for the file.  The ID will be 0 if the indicated file does not exist */
		/** @param #13 Filename */
			/** The file to get the ID for */
		/** @param #145 EK_File */
			/** The file id */

void Media_Plugin::CMD_Get_ID_from_Filename(string sFilename,int *iEK_File,string &sCMD_Result,Message *pMessage)
//<-dceag-c819-e->
{
	bool bIsDirectory = FileUtils::DirExists(sFilename);
	if( !bIsDirectory && !FileUtils::FileExists(sFilename) )
	{
		*iEK_File=0;
		return;
	}

	string sPath = FileUtils::BasePath(sFilename);
	string sFile = FileUtils::FilenameWithoutPath(sFilename);
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows(
		"Path='" + StringUtils::SQLEscape(sPath) + "' AND Filename='" + StringUtils::SQLEscape(sFile) + "'",
		&vectRow_File);
	if( vectRow_File.size() )
	{
		*iEK_File = vectRow_File[0]->PK_File_get();
		return;
	}

	string sExtension = FileUtils::FindExtension(sFile);
	vector<Row_MediaType *> vectRow_MediaType;
	if( sExtension.empty()==false )
		m_pDatabase_pluto_main->MediaType_get()->GetRows("Extensions like '%" + sExtension + "%'",&vectRow_MediaType);

	int nEK_Users_Private = MediaAttributes_LowLevel::GetOwnerForPath(sPath);

	int PK_MediaType = vectRow_MediaType.size() ? vectRow_MediaType[0]->PK_MediaType_get() : 0;
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
	pRow_File->EK_MediaType_set(PK_MediaType);
	pRow_File->Path_set(sPath);
	pRow_File->Filename_set(sFile);
	pRow_File->IsDirectory_set(bIsDirectory);
	pRow_File->INode_set( FileUtils::GetInode( sPath + "/" + sFile ) );

	if(nEK_Users_Private != 0)
		pRow_File->EK_Users_Private_set(nEK_Users_Private);
	else
		pRow_File->EK_Users_Private_setNull(true);

	m_pDatabase_pluto_media->File_get()->Commit();
	*iEK_File = pRow_File->PK_File_get();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_Get_ID_from_Filename %s PK_File %d Inode %d",
		(sPath + "/" + sFile).c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );
}

//<-dceag-c823-b->

	/** @brief COMMAND: #823 - Specify Media Provider */
	/** Specify the media provider for a device */
		/** @param #2 PK_Device */
			/** The device to set the provider for */
		/** @param #9 Text */
			/** The media providers information */
		/** @param #163 Description */
			/** A description for this provider */

void Media_Plugin::CMD_Specify_Media_Provider(int iPK_Device,string sText,string sDescription,string &sCMD_Result,Message *pMessage)
//<-dceag-c823-e->
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_Specify_Media_Provider iPK_Device %d sText %s sDescription %s",
		iPK_Device,sText.c_str(),sDescription.c_str() );

	// No provider for this.
	if( sText=="NONE" )
	{
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_EK_MediaProvider_CONST,sText);
		return;
	}

	string::size_type pos = 0;
	string sUsername = StringUtils::Tokenize(sText,"\t",pos);
	string sPassword = StringUtils::Tokenize(sText,"\t",pos);
	int PK_DeviceTemplate_MediaType = atoi(StringUtils::Tokenize(sText,"\t",pos).c_str());
	int PK_ProviderSource = atoi(StringUtils::Tokenize(sText,"\t",pos).c_str());

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = m_pDatabase_pluto_main->DeviceTemplate_MediaType_get()->GetRow(PK_DeviceTemplate_MediaType);
	Row_ProviderSource *pRow_ProviderSource = m_pDatabase_pluto_media->ProviderSource_get()->GetRow(PK_ProviderSource);
	if( !pRow_Device || !pRow_DeviceTemplate_MediaType || !pRow_ProviderSource )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Specify_Media_Provider can't find records for device %d text: %s",iPK_Device,sText.c_str());
		return; // shouldn't happen
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_Specify_Media_Provider ready to get rows" );

	vector<Row_MediaProvider *> vectRow_MediaProvider;
	m_pDatabase_pluto_media->MediaProvider_get()->GetRows("ID='" + StringUtils::SQLEscape(sText) + "'",&vectRow_MediaProvider);

	Row_MediaProvider *pRow_MediaProvider = NULL;
	if( vectRow_MediaProvider.size() )
		pRow_MediaProvider = vectRow_MediaProvider[0];
	else
	{
		pRow_MediaProvider = m_pDatabase_pluto_media->MediaProvider_get()->AddRow();
		pRow_MediaProvider->Description_set(sDescription);
		pRow_MediaProvider->EK_MediaType_set( pRow_DeviceTemplate_MediaType->FK_MediaType_get() );
		pRow_MediaProvider->FK_ProviderSource_set( PK_ProviderSource );
		pRow_MediaProvider->ID_set( sText );
		m_pDatabase_pluto_media->MediaProvider_get()->Commit();
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_Specify_Media_Provider ready to set data" );
	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_EK_MediaProvider_CONST,StringUtils::itos(pRow_MediaProvider->PK_MediaProvider_get()));
	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_Source_CONST,"");  // Got to get this again
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media_Plugin::CMD_Specify_Media_Provider ready to sync" );

	DCE::CMD_Sync_Providers_and_Cards_Cat CMD_Sync_Providers_and_Cards_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Player_Plugins_CONST,false,BL_SameHouse,0,pMessage ? pMessage->m_dwPK_Device_From : 0);
	SendCommand(CMD_Sync_Providers_and_Cards_Cat);
}
//<-dceag-c825-b->

	/** @brief COMMAND: #825 - Specify Capture Card Port */
	/** Specify the capture card port for a device */
		/** @param #2 PK_Device */
			/** The device to set the port for */
		/** @param #201 PK_Device_Related */
			/** The capture card port */

void Media_Plugin::CMD_Specify_Capture_Card_Port(int iPK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage)
//<-dceag-c825-e->
{
	// Don't allow 2 devices to use the same capture card port
	string sSQL = "UPDATE Device_DeviceData SET IK_DeviceData=NULL WHERE IK_DeviceData=" + StringUtils::itos(iPK_Device_Related) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_FK_Device_Capture_Card_Port_CONST);
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_FK_Device_Capture_Card_Port_CONST,StringUtils::itos(iPK_Device_Related));
}

//<-dceag-c831-b->

	/** @brief COMMAND: #831 - Refresh List of Online Devices */
	/** Send this command when the list of devices that are online/offline changes */

void Media_Plugin::CMD_Refresh_List_of_Online_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c831-e->
{
	m_sPK_Devices_Online="";
	string sSQL = "SELECT PK_Device,FK_DeviceCategory,FK_DeviceTemplate,FK_Device_ControlledVia FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_Online_CONST) " "
		"WHERE FK_DeviceCategory IN ("
			TOSTRING(DEVICECATEGORY_Hard_Drives_CONST) ","
			TOSTRING(DEVICECATEGORY_Network_Storage_CONST) ","
			TOSTRING(DEVICECATEGORY_Disc_Drives_CONST) ","
			TOSTRING(DEVICECATEGORY_CDDVD_Jukeboxes_CONST) ","
			TOSTRING(DEVICECATEGORY_Raid_Devices_CONST) ","
			TOSTRING(DEVICECATEGORY_Raid_Generic_CONST)
			") "
		"AND (IK_DeviceData IS NULL OR IK_DeviceData<>0)";

	PlutoSqlResult result;
	DB_ROW row;

	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			if( atoi(row[2])==DEVICETEMPLATE_Disc_Drive_Embedded_CONST )
			{
				if( !row[3] || m_pRouter->DeviceIsRegistered( atoi(row[3]) )==false )
					continue; // It's an embedded device and the parent isn't online
			}
			else if( atoi(row[1])==DEVICECATEGORY_Disc_Drives_CONST && m_pRouter->DeviceIsRegistered( atoi(row[0]) )==false )
				continue; // This disc drive isn't online
			if( m_sPK_Devices_Online.empty()==false )
				m_sPK_Devices_Online+=",";
			m_sPK_Devices_Online += row[0];
		}
	}

	// Delete files on devices that have since been removed
	sSQL = "DELETE `pluto_media`.File FROM `pluto_media`.File "
		"LEFT JOIN `pluto_main`.Device ON EK_Device=PK_Device "
		"WHERE EK_Device IS NOT NULL AND EK_Device>0 AND PK_Device IS NULL";
	m_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL);

	m_tLastScanOfOnlineDevices=time(NULL);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Refresh_List_of_Online_Devices now %s",m_sPK_Devices_Online.c_str());
#endif
}

string Media_Plugin::GetMRLFromDiscID( int PK_Disc,int PK_Device_Drive )
{
	if( PK_Device_Drive==0 )
	{
		string SQL = "SELECT EK_Device FROM DiscLocation WHERE FK_Disc=" + StringUtils::itos( PK_Disc );
		PlutoSqlResult result;
		DB_ROW row;
		if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && NULL != row[0] )
			PK_Device_Drive = atoi(row[0]);
	}

	if( PK_Device_Drive )
	{
		string sDrive = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,PK_Device_Drive,DEVICEDATA_Drive_CONST);
		if( sDrive.empty() )
			sDrive = "/dev/cdrom";
		return sDrive + "\t(" + StringUtils::itos(PK_Device_Drive) + ")\t";  // This \t(xxx)\t is used to know which drive it is.  See xine plugin
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::GetMRLFromDiscID no drive");
    return "/dev/cdrom";
}

//<-dceag-c839-b->

	/** @brief COMMAND: #839 - Check For New Files */
	/** Check to see if there are any new files that have been picked up by UpdateMedia that we should do some post processing on */

void Media_Plugin::CMD_Check_For_New_Files(string &sCMD_Result,Message *pMessage)
//<-dceag-c839-e->
{
	if( !m_dwPK_Device_MediaIdentification || !m_pRouter->DeviceIsRegistered(m_dwPK_Device_MediaIdentification) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Check_For_New_Files no media id device %d", m_dwPK_Device_MediaIdentification);
#endif
		return;
	}
	int PK_File_Last=0;
	string sSQL = "SELECT max(PK_File) FROM File";
    PlutoSqlResult result;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && row[0] )
		PK_File_Last = atoi(row[0]);

	string sFileList;
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("PK_File>" + StringUtils::itos(m_iPK_File_Last_Scanned_For_New) + " AND EK_MediaType=" TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST) + " AND IsNew=1",&vectRow_File);
	if( vectRow_File.size()==0 )
		return; // Nothing to do
	for(vector<Row_File *>::iterator it=vectRow_File.begin();it!=vectRow_File.end();++it)
	{
		Row_File *pRow_File = *it;
		sFileList += pRow_File->Path_get() + "/" + pRow_File->Filename_get() + "\n";
	}

	DCE::CMD_Identify_Media CMD_Identify_Media(m_dwPK_Device,m_dwPK_Device_MediaIdentification,
		0,"",sFileList,m_dwPK_Device);
	if( !SendCommand(CMD_Identify_Media) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Check_For_New_Files Failed to send ID command");
		return;
	}
	else
	{
		for(vector<Row_File *>::iterator it=vectRow_File.begin();it!=vectRow_File.end();++it)
			(*it)->IsNew_set(false);
	}
	m_pDatabase_pluto_media->File_get()->Commit();
	m_iPK_File_Last_Scanned_For_New=PK_File_Last;
}

//<-dceag-c846-b->

	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail a file or attribute */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

void Media_Plugin::CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c846-e->
{
	if( !pData || iData_Size==0 || sFilename.size()<3 )
		return;

    OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );
	if( !pOH_Orbiter ) // It could be a remote control
		pOH_Orbiter = m_pOrbiter_Plugin->m_mapRemote_2_Orbiter_Find( pMessage->m_dwPK_Device_From );

	if( pOH_Orbiter && pOH_Orbiter->m_pEntertainArea && pOH_Orbiter->m_pEntertainArea->m_pMediaStream )
	{
        class Command_Impl *pPlugIn = pOH_Orbiter->m_pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl;
        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Checking to see if the plugin %s will handle it!", pPlugIn->m_sName.c_str());
        pMessage->m_dwPK_Device_To=pPlugIn->m_dwPK_Device;
		// Don't forward to the generic handler/capture card--it's just ourself
        if( pOH_Orbiter->m_pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo!=m_pGenericMediaHandlerInfo && pPlugIn->ReceivedMessage( pMessage )==rmr_Processed )
			return; // The plugin handled this
	}

	Row_Picture *pRow_Picture = m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(pData,iData_Size,"JPG","");
	if( !pRow_Picture )
		return;
	Row_File *pRow_File = NULL;

	if( sFilename[0]=='!' )
	{
		if( sFilename[1]=='F' )
			pRow_File = m_pDatabase_pluto_media->File_get()->GetRow( atoi( sFilename.substr(2).c_str() ) );
		else if( sFilename[1]=='A' )
		{
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow( atoi( sFilename.substr(2).c_str() ) );
			if( !pRow_Attribute )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Make_Thumbnail can't find attriute %s", sFilename.c_str());
				return;
			}

			// Delete any existing pictures for this attribute
			vector<Row_Picture_Attribute *> vectRow_Picture_Attribute;
			pRow_Attribute->Picture_Attribute_FK_Attribute_getrows(&vectRow_Picture_Attribute);
			for(vector<Row_Picture_Attribute *>::iterator it=vectRow_Picture_Attribute.begin();it!=vectRow_Picture_Attribute.end();++it)
			{
				Row_Picture_Attribute *pRow_Picture_Attribute = *it;
				pRow_Picture_Attribute->Delete();
			}

			Row_Picture_Attribute *pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->AddRow();
			pRow_Picture_Attribute->FK_Attribute_set( pRow_Attribute->PK_Attribute_get() );
			pRow_Picture_Attribute->FK_Picture_set( pRow_Picture->PK_Picture_get() );
			m_pDatabase_pluto_media->Picture_Attribute_get()->Commit();
			return;
		}
		else
			return; // Don't know how to handle
	}
	else
	{
		int PK_File = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetFileIDFromFilePath(sFilename);
		if( PK_File )
			pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
	}

	if( !pRow_File )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Make_Thumbnail can't thumbnail %s", sFilename.c_str());
		return;
	}

	// Delete any existing pictures for this file
	vector<Row_Picture_File *> vectRow_Picture_File;
	pRow_File->Picture_File_FK_File_getrows(&vectRow_Picture_File);
	for(vector<Row_Picture_File *>::iterator it=vectRow_Picture_File.begin();it!=vectRow_Picture_File.end();++it)
	{
		Row_Picture_File *pRow_Picture_File = *it;
		pRow_Picture_File->Delete();
	}

	Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
	pRow_Picture_File->FK_File_set( pRow_File->PK_File_get() );
	pRow_Picture_File->FK_Picture_set( pRow_Picture->PK_Picture_get() );
	m_pDatabase_pluto_media->Picture_File_get()->Commit();
}
//<-dceag-c847-b->

	/** @brief COMMAND: #847 - Live AV Path */
	/** Switch the given a/v device to use the live a/v path */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #252 Turn On */
			/** If true, the audio/video inputs for direct viewing, not through the capture card, will be used */

void Media_Plugin::CMD_Live_AV_Path(string sPK_EntertainArea,bool bTurn_On,string &sCMD_Result,Message *pMessage)
//<-dceag-c847-e->
{
	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to play media
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea );
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
		if( !pEntertainArea->m_pMediaStream )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_Live_AV_Path Ent area %d has no media", pEntertainArea->m_iPK_EntertainArea);
			DCE::CMD_Display_Alert CMD_Display_Alert(m_dwPK_Device,pMessage->m_dwPK_Device_From,
				"Cannot switch to live a/v path.  No media playing","no live","5",interuptAlways);
			SendCommand(CMD_Display_Alert);
			continue;
		}

		MediaStream *pMediaStream = pEntertainArea->m_pMediaStream;
		MediaDevice *pMediaDevice = pMediaStream->m_pMediaDevice_Source;
		Row_MediaType *pRow_MediaType = m_pDatabase_pluto_main->MediaType_get()->GetRow(pMediaStream->m_iPK_MediaType);
		int PK_Pipe = pRow_MediaType->FK_Pipe_get();

		if( pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.empty() )
		{
			DCE::CMD_Display_Alert CMD_Display_Alert(m_dwPK_Device,pMessage->m_dwPK_Device_From,
				pMediaDevice->m_pDeviceData_Router->m_sDescription + " has no direct a/v path","no live","5",interuptAlways);
			SendCommand(CMD_Display_Alert);
			continue;
		}

		// We don't want to be setting the inputs to the 'live' a/v path because we're using the capture card
		for(map<int,Pipe *>::iterator it=pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.begin();it!=pMediaDevice->m_pDeviceData_Router->m_mapPipe_Available.end();++it)
			it->second->m_bDontSendInputs=(bTurn_On==false);  // If Turn on is true, then we want to send the inputs

		pEntertainArea->m_bViewingLiveAVPath = bTurn_On;

		// Send the on command to the m/d if 'view live turn on'==false, if it's true, it goes to the destination device
		int PK_Device;
		if( bTurn_On )
			PK_Device = pMediaDevice->m_pDeviceData_Router->m_dwPK_Device;
		else if( pEntertainArea->m_pOH_Orbiter_OSD && pEntertainArea->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_pDevice_ControlledVia )
			// The MD
			PK_Device = pEntertainArea->m_pOH_Orbiter_OSD->m_pDeviceData_Router->m_pDevice_ControlledVia->m_dwPK_Device;
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Live_AV_Path Ent area %d has no osd",pEntertainArea->m_iPK_EntertainArea);
			continue;
		}

		DCE::CMD_On CMD_On(m_dwPK_Device,PK_Device,PK_Pipe,"");
		CheckForCustomPipe(pEntertainArea,CMD_On.m_pMessage);
		SendCommand(CMD_On);

		if( pEntertainArea->m_pOH_Orbiter_OSD )
			pMediaStream->SetNowPlaying( pEntertainArea->m_pOH_Orbiter_OSD,false );
	}
}

//<-dceag-c845-b->

	/** @brief COMMAND: #845 - Delete File */
	/** Delete a file.  Can be a fully qualified filename, or a !F syntax */
		/** @param #13 Filename */
			/** The file to delete */

void Media_Plugin::CMD_Delete_File(string sFilename,string &sCMD_Result,Message *pMessage)
//<-dceag-c845-e->
{
	if( sFilename.size()>2 && sFilename[0]=='!' && sFilename[1]=='F' )
	{
		int PK_File = atoi( sFilename.substr(2).c_str() );
		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
		if( !pRow_File )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Delete_File cannot find %s",sFilename.c_str());
		else
		{
			pRow_File->Reload();
			sFilename = pRow_File->Path_get() + "/" + pRow_File->Filename_get();
			if( FileUtils::DelFile(sFilename) )
			{
				pRow_File->Missing_set(1);
				m_pDatabase_pluto_media->File_get()->Commit();
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File deleted Row_File id %d %s ",
					pRow_File->PK_File_get(),sFilename.c_str());
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Delete_File cannot delete %d %s %s/%s",
					pRow_File->PK_File_get(),sFilename.c_str(),pRow_File->Path_get().c_str(),pRow_File->Filename_get().c_str());
		}
	}
	else if ( sFilename.size()>2 && sFilename[0]=='!' && sFilename[1]=='V' )
	{
		// It is a voicemail of the form !Vxxx,yyyy[,O]
		// xxx = extension, yyyy = voicemail number, O = Old message.
		string::size_type tokenPos = 0;
		string sVoiceMailID = sFilename.substr(2);	// xxx,yyyy
		string sExtension = StringUtils::Tokenize(sVoiceMailID,",",tokenPos);
		string sMsg = StringUtils::Tokenize(sVoiceMailID,",",tokenPos);
		string sOld = StringUtils::Tokenize(sVoiceMailID,",",tokenPos);
		bool bIsOld = ( sOld[0] == 'O' );

		string sFileToDelete = "/var/spool/asterisk/voicemail/default/" + sExtension + "/INBOX/" + ( bIsOld ? "Old/"  : "" ) + "msg" + sMsg;

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Deleting Voicemail %s",sFileToDelete.c_str());

		// Delete the files.
		if ( FileUtils::DelFile(sFileToDelete + ".txt") )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File deleted voicemail info file %s.txt",sFileToDelete.c_str());
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File couldn't delete voicemail info file %s.txt",sFileToDelete.c_str());
		}

                if ( FileUtils::DelFile(sFileToDelete + ".gsm") )
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File deleted voicemail raw GSM file %s.gsm",sFileToDelete.c_str());
                }
                else
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File couldn't delete voicemail GSM file %s.gsm",sFileToDelete.c_str());
                }

                if ( FileUtils::DelFile(sFileToDelete + ".WAV") )
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File deleted voicemail WAVE (GSM) file %s.WAV",sFileToDelete.c_str());
                }
                else
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File couldn't delete voicemail WAVE (GSM) file %s.WAV",sFileToDelete.c_str());
                }

                if ( FileUtils::DelFile(sFileToDelete + ".wav") )
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File deleted voicemail WAVE (PCM) file %s.wav",sFileToDelete.c_str());
                }
                else
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Delete_File couldn't delete voicemail WAVE (PCM) file %s.wav",sFileToDelete.c_str());
                }


	}
	else if (sFilename.size()>2 && sFilename[0]=='!' && sFilename[1]=='P')
	  {
	    // Request to delete a playlist.
	    string sPK_Playlist = sFilename.substr(2);
	    string sEntriesSQL="DELETE FROM PlaylistEntry WHERE FK_Playlist = '"+sPK_Playlist+"'";
	    string sPlaylistSQL="DELETE FROM Playlist WHERE PK_Playlist = '"+sPK_Playlist+"'";

	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Deleting Playlist entries for PK_Playlist %s",sPK_Playlist.c_str());
	    m_pDatabase_pluto_media->threaded_db_wrapper_query(sEntriesSQL);
	    Sleep(100);
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Deleting Playlist %s",sPK_Playlist.c_str());
	    m_pDatabase_pluto_media->threaded_db_wrapper_query(sPlaylistSQL);
	    Sleep(100);
	  }
	else
	{
		// It is none of these things, error out and ignore the request.
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Delete_File will not delete arbitrary file %s",sFilename.c_str());
		sCMD_Result = "ERROR";
	}
}
//<-dceag-c868-b->

	/** @brief COMMAND: #868 - Retransmit A/V Commands */
	/** Resend a/v commands to the active video or audio output device */
		/** @param #9 Text */
			/** AP=Audio Power
AI=Audio Input
VP=Video Power
VI=Video Input */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */

void Media_Plugin::CMD_Retransmit_AV_Commands(string sText,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c868-e->
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	if( sText.size()<2 )
		return;  // Invalid parameter

	bool bFoundMatching=false;  // This way if we don't ever find a matching stream, we'll go ahead and resend on's to the m/d
	vector<EntertainArea *> vectEntertainArea;
	// Only an Orbiter will tell us to do this.  should only be one stream
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea );
	for(size_t s=0;s<vectEntertainArea.size();++s)
	{
		EntertainArea *pEntertainArea = vectEntertainArea[s];
		if( !pEntertainArea->m_pMediaStream )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_Retransmit_AV_Commands no media stream");
			continue;
		}

		bFoundMatching=true;
		// If we're not viewing a live path, it's running through the media director, so use those inputs
		MediaDevice *pMediaDevice;
		if( pEntertainArea->m_pMediaDevice_MD && !pEntertainArea->m_bViewingLiveAVPath )
			pMediaDevice = pEntertainArea->m_pMediaDevice_MD;
		else
			pMediaDevice = pEntertainArea->m_pMediaStream->m_pMediaDevice_Source;

		HandleRetransmitOnOff(sText[0],sText[1],pMediaDevice,pMessage->m_dwPK_Device_From,pEntertainArea);
	}

	if( bFoundMatching==false )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From);
		MediaDevice *pMediaDevice = pDevice ? m_mapMediaDevice_Find(pDevice->m_dwPK_Device_ControlledVia) : NULL;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Retransmit_AV_Commands Forwarding to %d %p", pMessage->m_dwPK_Device_From, pMediaDevice);
		if( pMediaDevice )
			HandleRetransmitOnOff(sText[0],sText[1],pMediaDevice,pMessage->m_dwPK_Device_From,NULL);
		// Find any md's in the ea's and reset their paths
		for(size_t s=0;s<vectEntertainArea.size();++s)
		{
			EntertainArea *pEntertainArea = vectEntertainArea[s];
			if( pEntertainArea->m_pMediaDevice_MD && pEntertainArea->m_pMediaDevice_MD!=pMediaDevice )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Retransmit_AV_Commands Forwarding to %d", pEntertainArea->m_pMediaDevice_MD->m_pDeviceData_Router->m_dwPK_Device );
				HandleRetransmitOnOff(sText[0],sText[1],pEntertainArea->m_pMediaDevice_MD,pMessage->m_dwPK_Device_From,NULL);
			}
		}
	}
}

void Media_Plugin::HandleRetransmitOnOff(char A_or_V,char P_or_I,MediaDevice *pMediaDevice,int PK_Device_From,EntertainArea *pEntertainArea)
{
	if( A_or_V=='A' )
	{
		if( !pMediaDevice->m_pDevice_Audio )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_Retransmit_AV_Commands no audio device");
		else
		{
			if( P_or_I=='P' )
			{
				DCE::CMD_On CMD_On(PK_Device_From,pMediaDevice->m_pDevice_Audio->m_dwPK_Device,
					0,"");
				CMD_On.m_pMessage->m_mapParameters[COMMANDPARAMETER_Retransmit_CONST]="1";
				if( pEntertainArea )
					CheckForCustomPipe(pEntertainArea,CMD_On.m_pMessage);
				SendCommand(CMD_On);
			}
			else if( P_or_I=='I' )
			{
				if( pMediaDevice->m_dwPK_Command_Audio )
				{
					DCE::CMD_Input_Select CMD_Input_Select(PK_Device_From,pMediaDevice->m_pDevice_Audio->m_dwPK_Device,
						pMediaDevice->m_dwPK_Command_Audio);
					CMD_Input_Select.m_pMessage->m_mapParameters[COMMANDPARAMETER_Retransmit_CONST]="1";
					if( pEntertainArea )
						CheckForCustomPipe(pEntertainArea,CMD_Input_Select.m_pMessage);
					SendCommand(CMD_Input_Select);
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_Retransmit_AV_Commands no input on audio device");
			}
		}
	}
	else if( A_or_V=='V' )
	{
		if( !pMediaDevice->m_pDevice_Video )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_Retransmit_AV_Commands no Video device");
		else
		{
			if( P_or_I=='P' )
			{
				DCE::CMD_On CMD_On(PK_Device_From,pMediaDevice->m_pDevice_Video->m_dwPK_Device,
					0,"");
				CMD_On.m_pMessage->m_mapParameters[COMMANDPARAMETER_Retransmit_CONST]="1";
				if( pEntertainArea )
					CheckForCustomPipe(pEntertainArea,CMD_On.m_pMessage);
				SendCommand(CMD_On);
			}
			else if( P_or_I=='I' )
			{
				if( pMediaDevice->m_dwPK_Command_Video )
				{
					DCE::CMD_Input_Select CMD_Input_Select(PK_Device_From,pMediaDevice->m_pDevice_Video->m_dwPK_Device,
						pMediaDevice->m_dwPK_Command_Video);
					CMD_Input_Select.m_pMessage->m_mapParameters[COMMANDPARAMETER_Retransmit_CONST]="1";
					if( pEntertainArea )
						CheckForCustomPipe(pEntertainArea,CMD_Input_Select.m_pMessage);
					SendCommand(CMD_Input_Select);
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::CMD_Retransmit_AV_Commands no input on Video device");
			}
		}
	}
}

void Media_Plugin::CheckForCustomPipe(EntertainArea *pEntertainArea,Message *pMessage)
{
	if( !pEntertainArea || !pMessage || pMessage->m_dwMessage_Type!=MESSAGETYPE_COMMAND )
		return;
	if( pEntertainArea->m_pMediaDevice_OutputZone )
		pMessage->m_mapParameters[COMMANDPARAMETER_PipeID_CONST]=StringUtils::itos(pEntertainArea->m_iPK_EntertainArea);
}

//<-dceag-c871-b->

	/** @brief COMMAND: #871 - Update Ripping Status */
	/** Update the status of a ripping job */
		/** @param #9 Text */
			/** A text message */
		/** @param #13 Filename */
			/** The filename being ripped */
		/** @param #102 Time */
			/** How much longer in seconds it will take */
		/** @param #199 Status */
			/** The status: [p] in progress, [e]rror, [s]uccess */
		/** @param #256 Percent */
			/** The percentage of completion */
		/** @param #257 Task */
			/** The task id */
		/** @param #258 Job */
			/** The job id */

void Media_Plugin::CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage)
//<-dceag-c871-e->
{
}
//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void Media_Plugin::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
}

void Media_Plugin::ReleaseDriveLock(MediaStream *pMediaStream)
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( pMediaFile && pMediaFile->m_dwPK_Device_Disk_Drive )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(pMediaFile->m_dwPK_Device_Disk_Drive);
		if( pDevice && pDevice->WithinCategory(DEVICECATEGORY_CDDVD_Jukeboxes_CONST)==false )  // If the lock failed and there were no avail drives, m_dwPK_Device_Disk_Drive will be a jukebox, and not a drive
		{
			string sText;
			bool bIsSuccessful=false;
			DCE::CMD_Lock CMD_Lock(m_dwPK_Device,pMediaFile->m_dwPK_Device_Disk_Drive,m_dwPK_Device,
				"",false,&sText,&bIsSuccessful);
			if( !SendCommand(CMD_Lock) || bIsSuccessful==false )
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::ReleaseDriveLock Cannot unlock drive %d", pMediaFile->m_dwPK_Device_Disk_Drive);

			// If this is within a jukebox, then we're done with the disc so put it back in a lot
			if( pDevice && pDevice->m_pDevice_ControlledVia && pDevice->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_CDDVD_Jukeboxes_CONST) )
			{
				int i=-1;
				DCE::CMD_Unload_from_Drive_into_Slot CMD_Unload_from_Drive_into_Slot(m_dwPK_Device,pDevice->m_dwPK_Device,pDevice->m_dwPK_Device,&i);
				SendCommand(CMD_Unload_from_Drive_into_Slot);
			}
		}
	}
}

bool Media_Plugin::AssignDriveForDisc(MediaStream *pMediaStream,MediaFile *pMediaFile)
{
	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	if( pMediaFile->m_dwPK_Disk && !pMediaFile->m_dwPK_Device_Disk_Drive )
	{
		int Slot=0;
		Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(pMediaFile->m_dwPK_Disk);
		if( pRow_Disc )
		{
			vector<Row_DiscLocation *> vectRow_DiscLocation;
			pRow_Disc->DiscLocation_FK_Disc_getrows(&vectRow_DiscLocation);
			DeviceData_Router *pDevice_Backup=NULL; // We'll try to find a drive on the same PC, otherwise we'll take any we can find with this disc
			for(vector<Row_DiscLocation *>::iterator it=vectRow_DiscLocation.begin();it!=vectRow_DiscLocation.end();++it)
			{
				Row_DiscLocation *pRow_DiscLocation = *it;
				DeviceData_Router *pDevice_Drive = m_pRouter->m_mapDeviceData_Router_Find(pRow_DiscLocation->EK_Device_get());
				if( pDevice_Drive )
				{
					if( pDevice_Drive->GetTopMostDevice() == pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->GetTopMostDevice() )
					{
						pMediaFile->m_dwPK_Device_Disk_Drive = pDevice_Drive->m_dwPK_Device;
						pMediaFile->m_Slot = pRow_DiscLocation->Slot_get();
						break;
					}
					Slot = pRow_DiscLocation->Slot_get();
					pDevice_Backup = pDevice_Drive;
				}
			}
			if( !pMediaFile->m_dwPK_Device_Disk_Drive && pDevice_Backup )
			{
				pMediaFile->m_dwPK_Device_Disk_Drive = pDevice_Backup->m_dwPK_Device;
				pMediaFile->m_Slot = Slot;
			}
		}
	}

	// See if the drive is actually a jukebox.  If so, we need to get an actual drive to play it and load the disc
	DeviceData_Router *pDevice_Jukebox = m_pRouter->m_mapDeviceData_Router_Find(pMediaFile->m_dwPK_Device_Disk_Drive);
	if( pDevice_Jukebox && pDevice_Jukebox->WithinCategory(DEVICECATEGORY_CDDVD_Jukeboxes_CONST) )
	{
		string sText="STREAM " + StringUtils::itos(pMediaStream->m_iStreamID_get());
		bool bIsSuccessful=false;

		int PK_Device_Disk=-1;  // This will give us the drive and lock it
		DCE::CMD_Load_from_Slot_into_Drive CMD_Load_from_Slot_into_Drive(m_dwPK_Device,pMediaFile->m_dwPK_Device_Disk_Drive,pMediaFile->m_Slot,&PK_Device_Disk);  // Figure out the drive id based on the destination
		if( !SendCommand(CMD_Load_from_Slot_into_Drive) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::AssignDriveForDisc Cannot lock drive %d", pMediaFile->m_dwPK_Device_Disk_Drive);
			if( pMediaStream->m_pOH_Orbiter_StartedMedia )
			{
				SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device,
					pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device, "Drive is not available");
				SendCommand(SCREEN_DialogCannotPlayMedia);
			}
			StreamEnded(pMediaStream);
			return false;
		}

		if( pMediaStream->m_pOH_Orbiter_StartedMedia )
		{
			DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device,pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device,
				"Please wait up to 60 seconds while I load that disc.  Playback will start when the disc is loaded.","","load_jukebox","0","10","1");
			SendCommand(SCREEN_PopupMessage);
		}
		pMediaFile->m_dwPK_Device_Disk_Drive = PK_Device_Disk; // This is the disc we'll be using now
		pMediaFile->m_bWaitingForJukebox=true;
	}

	return true;
}

void Media_Plugin::WaitingForJukebox( MediaStream *pMediaStream )
{
	MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
	if( !pMediaFile )  // Shouldn't ever happen
	{
		StreamEnded(pMediaStream);
		return;
	}

	vector<Row_DiscLocation *> vectRow_DiscLocation;
	m_pDatabase_pluto_media->DiscLocation_get()->GetRows( "EK_Device=" + StringUtils::itos(pMediaFile->m_dwPK_Device_Disk_Drive), &vectRow_DiscLocation );  // This should now be the drive, not the jukebox, since AssignDriveForDisc converts it
	if( vectRow_DiscLocation.size() )  // There should only be 1 record for a drive since only 1 disc can be there.  When this is not empty, the move has finished
	{
		// It's possible we were playing a slot without knowing what type of disc was in it.  Fill out the missing info
		int iPK_MediaType=0;
		string sDisks,sURL,sBlock_Device;
		string sAaronSpecial="*grml*"; // TODO hack to get sqlCVS updates back to work.
		int iAaronSpecial_EKID=0;
		DCE::CMD_Get_Disk_Info CMD_Get_Disk_Info(m_dwPK_Device,pMediaFile->m_dwPK_Device_Disk_Drive, &sAaronSpecial, &iPK_MediaType, &iAaronSpecial_EKID, &sDisks,&sURL,&sBlock_Device);
		if( SendCommand(CMD_Get_Disk_Info) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::WaitingForJukebox stream %d.  Disc loaded in drive.  Now ready.  Got MT %d disks %s url %s block %s",
				pMediaStream->m_iStreamID_get(),iPK_MediaType,sDisks.c_str(),sURL.c_str(),sBlock_Device.c_str());
			if( iPK_MediaType )
			{
				pMediaFile->m_dwPK_MediaType=iPK_MediaType;
                                pMediaStream->m_iPK_MediaType=iPK_MediaType;
			}
			if( sURL.empty()==false )
				pMediaFile->m_sFilename=sURL;
		}

		if( pMediaFile->m_dwPK_Device_Disk_Drive )
		{
			string sText="STREAM " + StringUtils::itos(pMediaStream->m_iStreamID_get());
			bool bIsSuccessful=false;
			DCE::CMD_Lock CMD_Lock(m_dwPK_Device,pMediaFile->m_dwPK_Device_Disk_Drive,m_dwPK_Device,
				"",true,&sText,&bIsSuccessful);
			if( !SendCommand(CMD_Lock) || bIsSuccessful==false )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Media_Plugin::WaitingForJukebox Cannot lock drive %d", pMediaFile->m_dwPK_Device_Disk_Drive);
				if( pMediaStream->m_pOH_Orbiter_StartedMedia )
				{
					SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device,
						pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device, "Jukebox Drive is not available");
					SendCommand(SCREEN_DialogCannotPlayMedia);
				}
				StreamEnded(pMediaStream);
				return;
			}
		}

		pMediaFile->m_bWaitingForJukebox=false;
		StartMedia(pMediaStream);
		return;
	}

	if( pMediaStream->m_tTime + TIMEOUT_JUKEBOX < time(NULL) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::WaitingForJukebox stream %d.  failed to load",
			pMediaStream->m_iStreamID_get());
		if( pMediaStream->m_pOH_Orbiter_StartedMedia )
		{
			SCREEN_DialogCannotPlayMedia SCREEN_DialogCannotPlayMedia(m_dwPK_Device,
				pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device, "Jukebox is not available");
			SendCommand(SCREEN_DialogCannotPlayMedia);
		}
		StreamEnded(pMediaStream);
		return;
	}
	else // Try again
	{
		int StreamID = pMediaStream->m_iStreamID_get( );
		m_pAlarmManager->AddRelativeAlarm(1,this,WAITING_FOR_JUKEBOX,(void *) StreamID);
	}
}

void Media_Plugin::TransformFilenameToDeque(string sFilename,deque<MediaFile *> &dequeFilenames)
{
	if( sFilename.length()==0 )
		return;

	if( sFilename[0] != '!' || sFilename.length()<3 )
	{
		dequeFilenames.push_back(new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel,sFilename));  // Just a normal file
		return;
	}

	if( sFilename[1] == 'A' || sFilename[1] == 'a' )
	{
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow( atoi(sFilename.substr(2).c_str()) );
		if( !pRow_Attribute )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::TransformFilenameToDeque Attribute lookup on %s is invalid",sFilename.c_str());
			return;
		}

		vector<Row_File_Attribute *> vectRow_File_Attribute;
		pRow_Attribute->File_Attribute_FK_Attribute_getrows(&vectRow_File_Attribute);
		for(size_t s=0;s<vectRow_File_Attribute.size();++s)
		{
			Row_File_Attribute *pRow_File_Attribute = vectRow_File_Attribute[s];
			dequeFilenames.push_back(new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel,pRow_File_Attribute->FK_File_get(),m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetFilePathFromFileID(pRow_File_Attribute->FK_File_get())));
		}
	}
	else if( sFilename[1] == 'F' || sFilename[1] == 'f' )
	{
		int PK_MediaType=0;
		MediaFile *pMediaFile = new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel,atoi(sFilename.substr(2).c_str()),m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetFilePathFromFileID(atoi(sFilename.substr(2).c_str()),&PK_MediaType));
		pMediaFile->m_dwPK_MediaType=PK_MediaType;
		dequeFilenames.push_back(pMediaFile);
	}
	else if( sFilename[1] == 'B' || sFilename[1] == 'b' )
	{
		Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow(atoi(sFilename.substr(2).c_str()));
		if( pRow_Bookmark )
		{
			Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();
			if( pRow_File )
			{
				MediaFile *pMediaFile = new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel,pRow_File->PK_File_get(),pRow_File->Path_get() + "/" + pRow_File->Filename_get());
				pMediaFile->m_sStartPosition = pRow_Bookmark->Position_get();
				dequeFilenames.push_back(pMediaFile);
			}
		}
	}
	else if( sFilename[1] == 'r' || sFilename[1] == 'R' )  // Removable media (ie a disc) as !r[PK_Disc][.track]:[PK_Device_Disk]:[Slot]  This can be followed by a : and a disk drive to prevent ambiguity if there are multiple sources, and another : and a slot
	{
		int PK_Device_Disk_Drive=0;
		int Slot=-1;
		string::size_type pos=sFilename.find(':');
		if( pos!=string::npos )
		{
			PK_Device_Disk_Drive = atoi( sFilename.substr(pos+1).c_str() );
			pos=sFilename.find(':',pos+1);
			if( pos!=string::npos )
				Slot = atoi( sFilename.substr(pos+1).c_str() );
		}

		Row_DiscLocation *pRow_DiscLocation = NULL;

		if( PK_Device_Disk_Drive )
			pRow_DiscLocation = m_pDatabase_pluto_media->DiscLocation_get()->GetRow(PK_Device_Disk_Drive,0);

		int PK_MediaType = pRow_DiscLocation ? m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetMediaType(pRow_DiscLocation) : 0;

		pos = sFilename.find('.');
		if( pos!=string::npos || PK_MediaType!=MEDIATYPE_pluto_CD_CONST || !PK_Device_Disk_Drive )
		{
			MediaFile *pMediaFile = new MediaFile(atoi(sFilename.substr(2).c_str()),PK_Device_Disk_Drive,Slot);
			if( pos!=string::npos )
				pMediaFile->m_iTrack = atoi( sFilename.substr(pos+1).c_str() );
			if( PK_MediaType )
				pMediaFile->m_dwPK_MediaType = PK_MediaType;

			dequeFilenames.push_back(pMediaFile);
		}
		else  // It's a cd.  Need to add tracks
		{
			string sDisks,sURL,sBlock_Device;
			string sAaronSpecial="*grml*"; // TODO hack to get sqlCVS updates back to work.
			int iAaronSpecial_EKID=0;
			DCE::CMD_Get_Disk_Info CMD_Get_Disk_Info(m_dwPK_Device,PK_Device_Disk_Drive,&sAaronSpecial, &PK_MediaType, &iAaronSpecial_EKID, &sDisks,&sURL,&sBlock_Device);
			if( SendCommand(CMD_Get_Disk_Info) )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::TransformFilenameToDeque Got cd info MT %d disks %s url %s block %s",
					PK_MediaType,sDisks.c_str(),sURL.c_str(),sBlock_Device.c_str());

				vector<string> Tracks;
				StringUtils::Tokenize(sURL,"\n",Tracks);
				for(vector<string>::iterator it=Tracks.begin();it!=Tracks.end();++it)
				{
					string sTrack = *it;
					MediaFile *pMediaFile = new MediaFile(sTrack);
					pMediaFile->m_dwPK_Disk = atoi(sFilename.substr(2).c_str());
					pMediaFile->m_dwPK_Device_Disk_Drive = PK_Device_Disk_Drive;
					pMediaFile->m_Slot = Slot;
                                        // media type defaults to CD, if it is not returned by the disk drive
                                        pMediaFile->m_dwPK_MediaType = (PK_MediaType!=0) ? PK_MediaType : MEDIATYPE_pluto_CD_CONST;
					dequeFilenames.push_back(pMediaFile);
				}
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::TransformFilenameToDeque unable to get cd info from %d",PK_Device_Disk_Drive);
		}
	}
	else if( sFilename[1] == 'E' || sFilename[1] == 'o')  // external media
	{
	        vector<string> parts;
		StringUtils::Tokenize(sFilename,",",parts);
		string url = parts[2];
		string sPK_MediaSource = parts[1];
		int MediaType = 4;
		if (parts[1] == "shoutcast_audio") {
		        MediaType = 4;
		}
		ExpandPlaylist(url, MediaType, dequeFilenames);

	}
	else
		dequeFilenames.push_back(new MediaFile(m_pMediaAttributes->m_pMediaAttributes_LowLevel,sFilename));  // Just a normal file
}

bool Media_Plugin::ExpandPlaylist(string sFilename, int MediaType, deque<MediaFile *> &dequeFilenames) {
	// Detect playlists
	if (StringUtils::EndsWith(sFilename, ".pls"))
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::ExpandPlaylist fileName indicates playlist");
		// Parse playlist
		PlaylistParser playlistParser(sFilename);
		if (playlistParser.isPlaylist()) {
			deque<PlaylistItem> playlistItems = playlistParser.getItems();
			for(deque<PlaylistItem>::iterator it=playlistItems.begin();it!=playlistItems.end();++it)
			{
				PlaylistItem item = *it;
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::ExpandPlaylist adding playlist item %s, title %s", item.url.c_str(), item.title.c_str());

				MediaFile *pMediaFile = new MediaFile(item.url);
				pMediaFile->m_dwPK_MediaType = MediaType;
				pMediaFile->m_sTitle = item.title;
				dequeFilenames.push_back(pMediaFile);
			}
			return true;
		}
	}
	return false;
}

void Media_Plugin::UpdateSearchTokens()
{
	string sSQL = "SELECT max(psc_mod) FROM Attribute";

	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pDatabase_pluto_media->db_wrapper_query_result(sSQL))==NULL || ( (row = db_wrapper_fetch_row( result.r )) )==NULL )
		return; // Shouldn't happen

	time_t tLastAttribute = 0;

	if(NULL != row[0])
		tLastAttribute = StringUtils::SQLDateTime(row[0]);

	vector<Row_Attribute *> vectRow_Attribute;
	string sWhere = "psc_mod>'" + StringUtils::SQLDateTime(m_tLastSearchTokenUpdate ? m_tLastSearchTokenUpdate : 1) + "'";
	m_pDatabase_pluto_media->Attribute_get()->GetRows( sWhere, &vectRow_Attribute );

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::UpdateSearchTokens m_tLastAttribute %d tLastAttribute %d size %d",
		m_tLastSearchTokenUpdate,tLastAttribute,vectRow_Attribute.size());

	for( vector<Row_Attribute *>::iterator it=vectRow_Attribute.begin();it!=vectRow_Attribute.end();++it )
	{
		Row_Attribute *pRow_Attribute = *it;
		m_pMediaAttributes->m_pMediaAttributes_LowLevel->UpdateSearchTokens(pRow_Attribute);
	}

	m_tLastSearchTokenUpdate = tLastAttribute;
	DCEConfig dceconfig;
	dceconfig.AddString("LastSearchTokenUpdate",StringUtils::itos(m_tLastSearchTokenUpdate));
	dceconfig.WriteSettings();
	m_pAlarmManager->AddRelativeAlarm(600,this,UPDATE_SEARCH_TOKENS,NULL);  // Do this every 10 minutes
}
//<-dceag-c942-b->

	/** @brief COMMAND: #942 - Get Ripping Status */
	/** Get ripping status */
		/** @param #199 Status */
			/** Ripping status */

void Media_Plugin::CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage)
//<-dceag-c942-e->
{
	vector<Row_RipStatus *> vectRow_RipStatus;
	m_pDatabase_pluto_media->RipStatus_get()->GetRows("1=1 ORDER BY PK_RipStatus DESC LIMIT 1", &vectRow_RipStatus);

    if(vectRow_RipStatus.empty())
	{
		*sStatus = "No rip in progress";
	}
	else
	{
		Row_RipStatus *pRow_RipStatus = vectRow_RipStatus[0];
		pRow_RipStatus->Reload();

		string sRipStatus = pRow_RipStatus->Status_get();
		string sFile = pRow_RipStatus->File_get();
		string sMessage = pRow_RipStatus->Message_get();

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Get_Ripping_Status: file '%s', status '%s'",
			sFile.c_str(), sRipStatus.c_str());

		string sFullStatus = "preparing";
		if(sRipStatus == "p")
			sFullStatus = sMessage;
		else if(sRipStatus == "e")
			sFullStatus = "failed";

		 *sStatus = "Ripping '" + FileUtils::FilenameWithoutPath(sFile) + "': " + sFullStatus;

		 //change description for rip completed
		 if(sRipStatus == "s")
			 *sStatus = "Rip completed!";
		 else if(sRipStatus == "b")
			 *sStatus = "Rip completed with errors!";

		 //if the disk was ejected, reset the status
		 if(sMessage == "disk ejected")
			 *sStatus = "";
	}
}
//<-dceag-c955-b->

	/** @brief COMMAND: #955 - Specify Repeat Options */
	/** Change the repeat option for an entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area */
		/** @param #117 Repeat */
			/** 0=none, 1=repeat queue, 2=repeat track */

void Media_Plugin::CMD_Specify_Repeat_Options(string sPK_EntertainArea,int iRepeat,string &sCMD_Result,Message *pMessage)
//<-dceag-c955-e->
{
}
//<-dceag-c1083-b->

	/** @brief COMMAND: #1083 - Move File */
	/** Move a file from one place to another. */
		/** @param #13 Filename */
			/** !F notation referring to PK_File entry in the pluto_media File table. */
		/** @param #219 Path */
			/** The fully qualified destination Path and Filename */

void Media_Plugin::CMD_Move_File(string sFilename,string sPath,string &sCMD_Result,Message *pMessage)
//<-dceag-c1083-e->
{

  if ( sFilename.size()>2 && sFilename[0]=='!' && sFilename[1]=='F' )
    {
      int PK_File = atoi(sFilename.substr(2).c_str());
      Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
      if (!pRow_File)
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Move_File cannot find %s decoded to %d",sFilename.c_str(),PK_File);
      else
	{
	  pRow_File->Reload();
	  sFilename=pRow_File->Path_get()+"/"+pRow_File->Filename_get();
	  string sDestinationFileName = sPath;

	  // Oddness follows, I have to ask the General Info Plugin for the symlink.
	  CMD_Get_Home_Symlink_DT CMD_Get_Home_Symlink(m_dwPK_Device, (long)DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, sPath, &sDestinationFileName);
	  if (!SendCommand(CMD_Get_Home_Symlink))
	    {
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send CMD_Get_Home_Symlink to General_Info_Plugin. Aborting CMD_Move_File.");
	      sCMD_Result="ERROR";
	      return;
	    }
	  else
	    {
	      LoggerWrapper::GetInstance()->Write(LV_WARNING,"");
	    }

	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX Moving file %s to %s",sFilename.c_str(),sDestinationFileName.c_str());

	  MoveJob *pMoveJob = new MoveJob(m_pDatabase_pluto_media,
					  m_pJobHandler,
					  pMessage->m_dwPK_Device_From,
					  pRow_File,
					  sFilename,
					  sDestinationFileName,
					  true,this);
	  m_pJobHandler->AddJob(pMoveJob);
	}
    }

  // // Check that filename is a media reference. We do not deal with anything else!
  // if( sFilename.size()>2 && sFilename[0]=='!' && sFilename[1]=='F' )
  //   {
  //     int PK_File = atoi( sFilename.substr(2).c_str() );
  //     Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
  //     if( !pRow_File )
  // 	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Move_File cannot find %s decoded to %d",sFilename.c_str(),PK_File);
  //     else
  // 	{
  // 	  pRow_File->Reload();
  // 	  sFilename = pRow_File->Path_get() + "/" + pRow_File->Filename_get();
  // 	  string sDestinationFileName = sPath;
  // 	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXX Moving file %s to %s ",sFilename.c_str(),sDestinationFileName.c_str());
  // 	  if( FileUtils::MoveFile(sFilename,sDestinationFileName,true) )
  // 	    {
  // 	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File moved Row_File id %d %s ",
  // 						  pRow_File->PK_File_get(),sFilename.c_str());

  // 	      string sDestinationPath = FileUtils::BasePath(sDestinationFileName);
  // 	      string sDestinationBaseName = FileUtils::FilenameWithoutPath(sDestinationFileName);

  // 	      pRow_File->Path_set(sDestinationPath);
  // 	      pRow_File->Filename_set(sDestinationBaseName);
  // 	      m_pDatabase_pluto_media->File_get()->Commit();

  // 	      // Beginning the snippet
  // 	      // Move the .id3 file, if it exists.
  // 	      string sID3File = sFilename + ".id3";
  // 	      string sDestinationID3File = sDestinationFileName + ".id3";
  // 	      if ( FileUtils::FileExists(sID3File) )
  // 		{
  // 		  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File moving existing ID3 file%s",sID3File.c_str());
  // 		  if ( FileUtils::MoveFile(sID3File,sDestinationID3File,true) )
  // 		    {
  // 		      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File moved ID3 file %s to %s",sID3File.c_str(),sDestinationID3File.c_str());
  // 		    }
  // 		  else
  // 		    {
  // 		      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Move_File Could not move existing ID3 file %s to %s",sID3File.c_str(),sDestinationID3File.c_str());
  // 		    }
  // 		}
  // 	      else
  // 		{
  // 		  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File no ID3 File found.");
  // 		}

  // 	      // Move the .dvd.keys.tar.gz if it exists.
  // 	      string sDVDFile = sFilename + ".keys.tar.gz";
  // 	      string sDestinationDVDFile = sDestinationFileName + ".keys.tar.gz";
  // 	      if ( FileUtils::FileExists(sDVDFile) )
  // 		{
  // 		  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File moving existing DVD Keys file%s",sDVDFile.c_str());
  // 		  if ( FileUtils::MoveFile(sDVDFile,sDestinationDVDFile,true) )
  // 		    {
  // 		      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File moved DVD Keys file %s to %s",sDVDFile.c_str(),sDestinationDVDFile.c_str());
  // 		    }
  // 		  else
  // 		    {
  // 		      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Move_File Could not move existing DVD Keys file %s to %s",sDVDFile.c_str(),sDestinationDVDFile.c_str());
  // 		    }
  // 		}
  // 	      else
  // 		{
  // 		  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CMD_Move_File no DVD Keys File found.");
  // 		}
  // 	      // Ending the snippet

  // 	    }
  // 	  else
  // 	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Move_File cannot move %d %s %s/%s",
  // 						pRow_File->PK_File_get(),sFilename.c_str(),pRow_File->Path_get().c_str(),pRow_File->Filename_get().c_str());

  // 	}
  //   }

}
//<-dceag-c1088-b->

	/** @brief COMMAND: #1088 - Specify Capture Card Audio Port */
	/** Specify the capture card audio port for a device */
		/** @param #2 PK_Device */
			/** The Device to set the capture card audio port */
		/** @param #201 PK_Device_Related */
			/** The Device to set the capture card audio port to */

void Media_Plugin::CMD_Specify_Capture_Card_Audio_Port(int iPK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage)
//<-dceag-c1088-e->
{
  // Don't allow 2 devices to use the same capture card audio port
  string sSQL = "UPDATE Device_DeviceData SET IK_DeviceData=NULL WHERE IK_DeviceData=" + StringUtils::itos(iPK_Device_Related) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_FK_Device_Capture_Card_Audio_P_CONST);
  m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
  DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_FK_Device_Capture_Card_Audio_P_CONST,StringUtils::itos(iPK_Device_Related));
}
//<-dceag-c1089-b->

	/** @brief COMMAND: #1089 - Update Move Status */
	/** Update the Status of a Move Task */
		/** @param #9 Text */
			/** A Text message to return to the Task. */
		/** @param #102 Time */
			/** Time in Seconds remaining. */
		/** @param #199 Status */
			/** The status: [p] in progress, [e]rror, [s]uccess */
		/** @param #256 Percent */
			/** The Percentage to update. */
		/** @param #257 Task */
			/** Task ID to update */
		/** @param #258 Job */
			/** Job ID to update */

void Media_Plugin::CMD_Update_Move_Status(string sText,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage)
//<-dceag-c1089-e->
{
  PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
  Task *pTask = m_pJobHandler->FindTask(atoi(sJob.c_str()),atoi(sTask.c_str()));
  if (!pTask)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Update_Move_Status invalid job %s task %s",sJob.c_str(),sTask.c_str());
      return;
    }

  MoveTask *pMoveTask = (MoveTask *) pTask;
  pMoveTask->UpdateProgress(sStatus, iPercent, atoi(sTime.c_str()), sText);

}
//<-dceag-c1090-b->

	/** @brief COMMAND: #1090 - Get Attribute Image */
	/** Returns PK_Image file for supplied !A */

void Media_Plugin::CMD_Get_Attribute_Image(string &sCMD_Result,Message *pMessage)
//<-dceag-c1090-e->
{
}

//<-dceag-c1091-b->

	/** @brief COMMAND: #1091 - Transcode File */
	/** Transcode a file */
		/** @param #13 Filename */
			/** Source Filename */
		/** @param #20 Format */
			/** Format parameters key=value separated by | */
		/** @param #50 Name */
			/** Destination Filename */
		/** @param #234 Directory */
			/** Destination Directory */

void Media_Plugin::CMD_Transcode_File(string sFilename,string sFormat,string sName,string sDirectory,string &sCMD_Result,Message *pMessage)
//<-dceag-c1091-e->
{
  // Parse the filenames out.
  bool bDeleteSourceAfterTranscode=true;
  size_t pos=0;
  string sSourceFilename = sFilename;
  string sDestinationFilename = sDirectory + "/" + sName;
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Transcode_File() - sDirectory is %s and sName is %s",sDirectory.c_str(),sName.c_str());

  // Parse the format parameters
  vector<string> vectParameters;
  map<string, string> mapParametersAndValues;
  StringUtils::Tokenize(sFormat,"|",vectParameters);
  for (vector<string>::iterator it=vectParameters.begin();it!=vectParameters.end();++it)
    {
      pos=0;
      string sCurrentParameter = *it;
      string sKey=StringUtils::Tokenize(sCurrentParameter,"=",pos);
      string sValue=StringUtils::Tokenize(sCurrentParameter,"=",pos);
      if (sKey=="deleteSourceAfterTranscode")
	{
	  bDeleteSourceAfterTranscode=true;
	}
      if (sKey=="xvidbitrate")
	{
	  mapParametersAndValues["xvidencopts"] = "bitrate="+sValue;
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Transcode_File - sFormat: sKey is %s sValue is %s",sKey.c_str(),sValue.c_str());
	  mapParametersAndValues[sKey]=sValue;
	}
    }
  
  // Now we should have everything we need to fire a job.
  
  TranscodeJob *pTranscodeJob = new TranscodeJob(m_pJobHandler,
						 pMessage->m_dwPK_Device_From,
						 sSourceFilename,
						 sDestinationFilename,
						 mapParametersAndValues,
						 bDeleteSourceAfterTranscode,
						 this);

   m_pJobHandler->AddJob(pTranscodeJob);
}

//<-dceag-c1092-b->

	/** @brief COMMAND: #1092 - Update Transcode Status */
	/** Update the status of a transcode job. */
		/** @param #9 Text */
			/** STatus text to return to task */
		/** @param #102 Time */
			/** Time remaining */
		/** @param #199 Status */
			/** Status e s f */
		/** @param #256 Percent */
			/** Percent update for task */
		/** @param #257 Task */
			/** Task number */
		/** @param #258 Job */
			/** Job Number */

void Media_Plugin::CMD_Update_Transcode_Status(string sText,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage)
//<-dceag-c1092-e->
{
  PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
  Task *pTask = m_pJobHandler->FindTask(atoi(sJob.c_str()),atoi(sTask.c_str()));
  if (!pTask)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::CMD_Update_Transcode_Status invalid job %s task %s",sJob.c_str(),sTask.c_str());
      return;
    }

  TranscodeTask *pTranscodeTask = (TranscodeTask *) pTask;
  pTranscodeTask->UpdateProgress(sStatus, iPercent, atoi(sTime.c_str()), sText);
}
//<-dceag-c1095-b->

	/** @brief COMMAND: #1095 - Get Attribute Types */
	/** Get  attribute types */
		/** @param #9 Text */
			/** The attribute types in the format of: id:name<newline>id:name<newline> */
		/** @param #29 PK_MediaType */
			/** Media type to find attribute types for. If 0, will return all. */

void Media_Plugin::CMD_Get_Attribute_Types(int iPK_MediaType,string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c1095-e->
{
	string result = "";
	if ( iPK_MediaType > 0 )
	{
		// Look up attribute types for specific media type
		vector<Row_MediaType_AttributeType *> vectRow_MediaType_AttributeType;
		m_pDatabase_pluto_media->MediaType_AttributeType_get()->GetRows("EK_MediaType="+StringUtils::itos(iPK_MediaType),
									 &vectRow_MediaType_AttributeType);
		for(vector<Row_MediaType_AttributeType *>::iterator it=vectRow_MediaType_AttributeType.begin();it!=vectRow_MediaType_AttributeType.end();++it)
		{
			result += StringUtils::itos((*it)->FK_AttributeType_get()) + ":" + StringUtils::itos((*it)->MediaSortOption_get()) + ":" + (*it)->FK_AttributeType_getrow()->Description_get() + "\n";
		}
	} else {
		// Look up all attribute types
		vector<Row_AttributeType *> vectRow_AttributeType;
		m_pDatabase_pluto_media->AttributeType_get()->GetRows("1=1", &vectRow_AttributeType);
		for(vector<Row_AttributeType *>::iterator it=vectRow_AttributeType.begin();it!=vectRow_AttributeType.end();++it)
		{
			result += StringUtils::itos((*it)->PK_AttributeType_get()) + ":" + (*it)->Description_get() + "\n";
		}

	}
	(*sText) = result;
}
//<-dceag-c1096-b->

	/** @brief COMMAND: #1096 - Get Attributes For Type */
	/** Get the attributes for attribute type */
		/** @param #9 Text */
			/** The attributes for the specified attribute type in the format: id:name<newline>id:name<newline> */
		/** @param #122 EK_AttributeType */
			/** The attribute type to get the attributes for */

void Media_Plugin::CMD_Get_Attributes_For_Type(int iEK_AttributeType,string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c1096-e->
{
	string result = "";
	// Look up attribute for specific attribute type
	vector<Row_Attribute *> vectRow_Attribute;
	m_pDatabase_pluto_media->Attribute_get()->GetRows("FK_AttributeType="+StringUtils::itos(iEK_AttributeType),
									 &vectRow_Attribute);
	for(vector<Row_Attribute *>::iterator it=vectRow_Attribute.begin();it!=vectRow_Attribute.end();++it)
	{
		result += StringUtils::itos((*it)->PK_Attribute_get()) + ":" + (*it)->Name_get() + "\n";
	}
	(*sText) = result;
}
//<-dceag-c1097-b->

	/** @brief COMMAND: #1097 - Get File Formats */
	/** Get file formats for the specified media type. */
		/** @param #20 Format */
			/** Comma-delimited list of file formats. */
		/** @param #29 PK_MediaType */
			/** Media Type to get file formats for. If 0, get all. */

void Media_Plugin::CMD_Get_File_Formats(int iPK_MediaType,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c1097-e->
{
	string result = "";
	if ( iPK_MediaType > 0 )
	{
		// Look up file formats for specific media type
		vector<Row_MediaType_FileFormat *> vectRow_MediaType_FileFormat;
		m_pDatabase_pluto_media->MediaType_FileFormat_get()->GetRows("EK_MediaType="+StringUtils::itos(iPK_MediaType),
									 &vectRow_MediaType_FileFormat);
		for(vector<Row_MediaType_FileFormat *>::iterator it=vectRow_MediaType_FileFormat.begin();it!=vectRow_MediaType_FileFormat.end();++it)
		{
			result += StringUtils::itos((*it)->FK_FileFormat_get()) + ":" + (*it)->FK_FileFormat_getrow()->Description_get() + "\n";
		}
	} else {
		// Look up all file format
		vector<Row_FileFormat *> vectRow_FileFormat;
		m_pDatabase_pluto_media->FileFormat_get()->GetRows("1=1", &vectRow_FileFormat);
		for(vector<Row_FileFormat *>::iterator it=vectRow_FileFormat.begin();it!=vectRow_FileFormat.end();++it)
		{
			result += StringUtils::itos((*it)->PK_FileFormat_get()) + ":" + (*it)->Description_get() + "\n";
		}

	}
	(*sFormat) = result;
}


//<-dceag-c1098-b->

	/** @brief COMMAND: #1098 - Get Media Sub Type */
	/** Get media sub types for specified media type. */
		/** @param #29 PK_MediaType */
			/** The Media Type to get sub types for. If 0, gets all. */
		/** @param #50 Name */
			/** Comma delimited list of media sub types. */

void Media_Plugin::CMD_Get_Media_Sub_Type(int iPK_MediaType,string *sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c1098-e->
{
	string result = "";
	if ( iPK_MediaType > 0 )
	{
		// Look up media sub type for specific media type
		vector<Row_MediaType_MediaSubType *> vectRow_MediaType_MediaSubType;
		m_pDatabase_pluto_media->MediaType_MediaSubType_get()->GetRows("EK_MediaType="+StringUtils::itos(iPK_MediaType),
									 &vectRow_MediaType_MediaSubType);
		for(vector<Row_MediaType_MediaSubType *>::iterator it=vectRow_MediaType_MediaSubType.begin();it!=vectRow_MediaType_MediaSubType.end();++it)
		{
			result += StringUtils::itos((*it)->FK_MediaSubType_get()) + ":" + (*it)->FK_MediaSubType_getrow()->Description_get() + "\n";
		}
	} else {
		// Look up all media sub types
		vector<Row_MediaSubType *> vectRow_MediaSubType;
		m_pDatabase_pluto_media->MediaSubType_get()->GetRows("1=1", &vectRow_MediaSubType);
		for(vector<Row_MediaSubType *>::iterator it=vectRow_MediaSubType.begin();it!=vectRow_MediaSubType.end();++it)
		{
			result += StringUtils::itos((*it)->PK_MediaSubType_get()) + ":" + (*it)->Description_get() + "\n";
		}

	}
	(*sName) = result;
}
