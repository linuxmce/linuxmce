/*
 Disk_Drive

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.
*/

//<-dceag-d-b->
#include "Disk_Drive.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_RipStatus.h"
#include "PlutoUtils/ProcessUtils.h"
#include "Media_Plugin/MediaAttributes_LowLevel.h"
#include "JobHandler/Job.h"
#include "DCE/DCEConfig.h"
DCEConfig g_DCEConfig;

/*
FIXME: refactor for udev to support local jukeboxes again.
#include "HAL/HalTree.h"
*/

#include "Disk_Drive_Functions/RipJob.h"
#include "Disk_Drive_Functions/RipTask.h"
#include "Disk_Drive_Functions/ProcessMediaIdentifiedJob.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef WIN32
//#include <stropts.h>
#include <sys/wait.h>
extern "C"
{
    #include <linux/cdrom.h>
}
#endif

using namespace nsJobHandler;

#define SERVER_PORT 5150

//<-dceag-const-b->! custom
Disk_Drive::Disk_Drive(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Disk_Drive_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
        m_monitorEnabled(true), m_serverPid(-1), m_serverPort(SERVER_PORT)
//<-dceag-const-e->
{
	m_pJobHandler = new JobHandler();
	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;
	m_pDatabase_pluto_media = NULL;
	m_pMediaAttributes_LowLevel = NULL;
	m_pDisk_Drive_Functions = NULL;
}

//<-dceag-getconfig-b->
bool Disk_Drive::GetConfig()
{
	if( !Disk_Drive_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	string sDrive;
	
	system("modprobe ide-generic");
	system("modprobe ide-cd");
	sDrive = DATA_Get_Drive();
	if (sDrive == "")
	{
		sDrive = "/dev/cdrom";
		if (!FileUtils::FileExists(sDrive))
			sDrive = "/dev/cdrom1";
	}
	
	m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
	string sIP = g_DCEConfig.m_mapParameters_Find("MySqlHost");
	if( sIP.empty() )
		sIP = m_sIPAddress;

	VerifyDriveIsNotEmbedded(sDrive);

	if( !m_pDatabase_pluto_media->Connect(sIP,"root","","pluto_media") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Disk_Drive::GetConfig cannot connect to database on %s", sIP.c_str());
		delete m_pDatabase_pluto_media;
		m_pDatabase_pluto_media=NULL;
	}
	
	m_pMediaAttributes_LowLevel = new MediaAttributes_LowLevel(m_pDatabase_pluto_media);

	m_pDisk_Drive_Functions = new Disk_Drive_Functions(m_dwPK_Device,this, sDrive, m_pJobHandler,m_pDatabase_pluto_media,m_pMediaAttributes_LowLevel);
	m_pDisk_Drive_Functions->UpdateDiscLocation('E',0); // For now say the drive is empty, when the script starts it will get set again

	bool bResult = m_pJobHandler->StartThread();
	LoggerWrapper::GetInstance()->Write(LV_SOCKET,"Disk_Drive::GetConfig StartThread %d", (int) bResult);

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Disk_Drive::~Disk_Drive()
//<-dceag-dest-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::~Disk_Drive");
	m_pJobHandler->StopThread(5); // Give it at most 5 seconds to stop 
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::~Disk_Drive Stopped Job Handler");
	delete m_pJobHandler;  // Delete this first since it can reference the others
	m_pJobHandler = NULL;
	delete m_pDatabase_pluto_media;
	m_pDatabase_pluto_media = NULL;
	delete m_pMediaAttributes_LowLevel;
	m_pMediaAttributes_LowLevel = NULL;
	delete m_pDisk_Drive_Functions;
	m_pDisk_Drive_Functions = NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Disk_Drive::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will
	get all	commands for your children in ReceivedCommandForChild, where
	pDeviceData_Base is the child device.  If you handle the message, you
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Disk_Drive::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Disk_Drive::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

void Disk_Drive::PostConnect()
{
	DCE::CMD_Refresh_List_of_Online_Devices_Cat CMD_Refresh_List_of_Online_Devices_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,
		true,BL_SameHouse);
	SendCommand(CMD_Refresh_List_of_Online_Devices_Cat);
}

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c45-b->

	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

void Disk_Drive::CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage)
//<-dceag-c45-e->
{
    LoggerWrapper::GetInstance()->Write(LV_ACTION,"Turning ON the disk drive monitoring.");
    m_monitorEnabled = true;
    m_pDisk_Drive_Functions->cdrom_lock(0);
}

//<-dceag-c46-b->

	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

void Disk_Drive::CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage)
//<-dceag-c46-e->
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Turning OFF the Disk Drive Monitoring.");
    m_monitorEnabled = false;
    m_pDisk_Drive_Functions->cdrom_lock(1);
}

//<-dceag-c47-b->

	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */

void Disk_Drive::CMD_Reset_Disk_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c47-e->
{
	PLUTO_SAFETY_LOCK(dm,m_pDisk_Drive_Functions->m_DiskMutex);
	m_pDisk_Drive_Functions->m_mediaInserted = false;
	m_pDisk_Drive_Functions->m_mediaDiskStatus = DISCTYPE_NONE;
	m_pDisk_Drive_Functions->DisplayMessageOnOrbVFD("Checking disc...");

	int iPK_MediaType;
	string sDisks,sURL,sBlock_Device;
	m_pDisk_Drive_Functions->internal_reset_drive(true,&iPK_MediaType,&sDisks,&sURL,&sBlock_Device);
}

//<-dceag-c48-b->

	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */
		/** @param #151 Slot Number */
			/** For jukeboxes, which slot to eject */

void Disk_Drive::CMD_Eject_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c48-e->
{
	if( !m_pDisk_Drive_Functions )
		return;  // shouldn't happen
	static time_t tLastEject=0;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::CMD_Eject_Disk  tLastEject %d (%d) tray open: %d",
		(int) tLastEject, (int) time(NULL), (int) m_pDisk_Drive_Functions->m_bTrayOpen);

	if( time(NULL)-tLastEject<=2 )  // It can take the drive a while to spin down and the user hits eject multiple times
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::CMD_Eject_Disk skipping eject within last 2 seconds");
		return;
	}
	if( m_pDisk_Drive_Functions->m_bTrayOpen )
	{
		string sCmd = "eject -t " + m_pDisk_Drive_Functions->m_sDrive;
		m_pDisk_Drive_Functions->m_bTrayOpen=false;
		m_pDisk_Drive_Functions->DisplayMessageOnOrbVFD("Closing tray...");
		system( sCmd.c_str() );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::CMD_Eject_Disk %s",sCmd.c_str() );
	}
	else
	{
		string sCmd = "eject " + m_pDisk_Drive_Functions->m_sDrive;
		m_pDisk_Drive_Functions->m_bTrayOpen=true;
		m_pDisk_Drive_Functions->DisplayMessageOnOrbVFD("Opening tray...");
		system( sCmd.c_str() );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::CMD_Eject_Disk %s",sCmd.c_str() );
	}

	m_pDisk_Drive_Functions->m_mediaInserted = false;  // Be sure we re-identify any media in there
	m_pDisk_Drive_Functions->m_mediaDiskStatus = DISCTYPE_NONE;
	tLastEject = time(NULL); // Put this after the system call so we know when it's been less than 2 seconds since a successful one

	if(m_pDisk_Drive_Functions->m_bTrayOpen)
	{
	        vector<Row_RipStatus *> vectRow_RipStatus;
        	m_pDatabase_pluto_media->RipStatus_get()->GetRows("EK_Device = " + StringUtils::ltos(m_dwPK_Device) + " ORDER BY PK_RipStatus DESC LIMIT 1",
                        &vectRow_RipStatus);

	        if(!vectRow_RipStatus.empty())
        	{
                	Row_RipStatus *pRow_RipStatus = vectRow_RipStatus[0];
	                pRow_RipStatus->Message_set("disk ejected");
			m_pDatabase_pluto_media->RipStatus_get()->Commit(); 
        	}
	}

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Disk_Drive::CMD_Eject_Disk  tLastEject %d (%d) tray open: %d",
	(int) tLastEject, (int) time(NULL), (int) m_pDisk_Drive_Functions->m_bTrayOpen);
}

//<-dceag-c49-b->

	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

void Disk_Drive::CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c49-e->
{
    cout << "Need to implement command #49 - Start Burn Session" << endl;
}

//<-dceag-c50-b->

	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

void Disk_Drive::CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c50-e->
{
    cout << "Need to implement command #50 - Start Ripping Session" << endl;
}

//<-dceag-c51-b->

	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

void Disk_Drive::CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c51-e->
{
    cout << "Need to implement command #51 - Add File To Burning Session" << endl;
}

//<-dceag-c52-b->

	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

void Disk_Drive::CMD_Start_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c52-e->
{
    cout << "Need to implement command #52 - Start Burning" << endl;
}

//<-dceag-c53-b->

	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

void Disk_Drive::CMD_Abort_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c53-e->
{
}

//<-dceag-c54-b->

	/** @brief COMMAND: #54 - Mount Disk Image */
	/** Will mount a disk image as a disk. */
		/** @param #13 Filename */
			/** What to mount. If it get's the Device name it will mount the actual disk in the drive. */
		/** @param #59 MediaURL */
			/** The URL which can be used to play the mounted media. */

void Disk_Drive::CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c54-e->
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got a mount media request %s", sFilename.c_str());
    string stringMRL;

	if (! m_pDisk_Drive_Functions->mountDVD(sFilename, stringMRL))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error executing the dvd mounting script (message: %s). Returning error.", stringMRL.c_str());
		sCMD_Result = "NOT_OK";
		*sMediaURL = stringMRL;
		return;
	}

	*sMediaURL += stringMRL;
	sCMD_Result = "OK";
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning new media URL: %s", sMediaURL->c_str());
}

//<-dceag-c55-b->

	/** @brief COMMAND: #55 - Abort Ripping */
	/** Aborts ripping a DVD. */

void Disk_Drive::CMD_Abort_Ripping(string &sCMD_Result,Message *pMessage)
//<-dceag-c55-e->
{
	if (! m_pDisk_Drive_Functions->m_pDevice_AppServer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot Abort rip -- no app server");
		sCMD_Result="NO App_Server";
		return;
	}

	PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
	const ListJob *plistJob = m_pJobHandler->m_listJob_get();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::AbortAll %d jobs", plistJob->size());

	for(ListJob::const_iterator it=plistJob->begin();it!=plistJob->end();++it)
	{
		Job *pJob = *it;
		pJob->Abort();
	}
}

//<-dceag-c56-b->

	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

void Disk_Drive::CMD_Format_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c56-e->
{
    cout << "Need to implement command #56 - Format Drive" << endl;
}

//<-dceag-c57-b->

	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

void Disk_Drive::CMD_Close_Tray(string &sCMD_Result,Message *pMessage)
//<-dceag-c57-e->
{
    cout << "Need to implement command #57 - Close Tray" << endl;
}


void Disk_Drive::RunMonitorLoop()
{
    m_pDisk_Drive_Functions->internal_monitor_step(false); // ignore any drive that is in the drive at the start.

    bool done = false;
    
    bool bSendEvents = DATA_Get_Send_Events();
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disk_Drive:::RunMonitorLoop - configured %s Media Inserted events", bSendEvents?"to send":"not to send");

    while ( ! done  && !m_bQuit_get())
    {
        done = ! m_pDisk_Drive_Functions->internal_monitor_step(bSendEvents);
        Sleep(3000); // Sleep 3 seconds
    }
}

//<-dceag-sample-b->! no sample
//<-dceag-createinst-b->!
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

void Disk_Drive::CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory,string &sCMD_Result,Message *pMessage)
//<-dceag-c337-e->
{
	string sCorrectedDirectory;
	DCE::CMD_Get_Home_Symlink_DT cmd_Get_Home_Symlink_DT(0, DEVICETEMPLATE_General_Info_Plugin_CONST,
		BL_SameHouse, sDirectory, &sCorrectedDirectory);
	SendCommand(cmd_Get_Home_Symlink_DT);

	if(!sCorrectedDirectory.empty())
	{
		sDirectory = sCorrectedDirectory;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Translated directory '%s' => '%s'",
			sDirectory.c_str(), sCorrectedDirectory.c_str());

		sDirectory = FileUtils::IncludeTrailingSlash(sDirectory);
	}

	RipJob *pRipJob = new RipJob(m_pDatabase_pluto_media,m_pJobHandler,m_pDisk_Drive_Functions,NULL,iPK_Users,iEK_Disc,
		pMessage ? pMessage->m_dwPK_Device_From : 0,sFormat,sFilename,sDirectory,sTracks,true,this);
	m_pJobHandler->AddJob(pRipJob);
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

void Disk_Drive::CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name,string &sCMD_Result,Message *pMessage)
//<-dceag-c817-e->
{
}
//<-dceag-c742-b->

	/** @brief COMMAND: #742 - Media Identified */
	/** Media has been identified */
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

void Disk_Drive::CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc,string &sCMD_Result,Message *pMessage)
//<-dceag-c742-e->
{
	ProcessMediaIdentifiedJob *pProcessMediaIdentifiedJob = new ProcessMediaIdentifiedJob(m_pJobHandler,iPK_Device,sValue_To_Assign,sID,pData,iData_Size,sFormat,iPK_MediaType,sMediaURL,sURL,iEK_Disc,NULL,m_pDisk_Drive_Functions);
	m_pJobHandler->AddJob(pProcessMediaIdentifiedJob);
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

void Disk_Drive::CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage)
//<-dceag-c871-e->
{
	PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
	Task *pTask = m_pJobHandler->FindTask(atoi(sJob.c_str()),atoi(sTask.c_str()));
	if( !pTask )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Disk_Drive::CMD_Update_Ripping_Status invalid job %s task %s",sJob.c_str(),sTask.c_str());
		return;
	}

	RipTask *pRipTask = (RipTask *) pTask;
	pRipTask->UpdateProgress(sStatus,iPercent,atoi(sTime.c_str()),sText,sFilename);
}
//<-dceag-c872-b->

	/** @brief COMMAND: #872 - Lock */
	/** Lock the drive for use by something else, normally the player */
		/** @param #2 PK_Device */
			/** The device requesting the lock */
		/** @param #9 Text */
			/** A description of the lock for incoming.  On failure (IsSuccesful=false), the description of whatever currently has the lock, on success info about the lock (for jukeboxes, the drive) */
		/** @param #10 ID */
			/** The ID of what needs to be locked.  For a jukebox, this would be the slot. */
		/** @param #40 IsSuccessful */
			/** returns true if the lock was succesfull.  If not, it puts the current lock in Text */
		/** @param #252 Turn On */
			/** True to set the lock, false to release it */

void Disk_Drive::CMD_Lock(int iPK_Device,string sID,bool bTurn_On,string *sText,bool *bIsSuccessful,string &sCMD_Result,Message *pMessage)
//<-dceag-c872-e->
{
	if( !m_pDisk_Drive_Functions )
	{
		*bIsSuccessful = false;
		return;
	}

	// This is to lock a drive for playback.  We'll return the id of the drive we're locking
	if( bTurn_On )
		*bIsSuccessful = m_pDisk_Drive_Functions->LockDrive(Disk_Drive_Functions::locked_playback,this);
	else
	{
		*bIsSuccessful = true;
		m_pDisk_Drive_Functions->UnlockDrive();
	}
}

bool Disk_Drive::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	return m_pJobHandler->ReportPendingTasks(pPendingTaskList);
}
//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void Disk_Drive::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
	PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
	Job *pJob = m_pJobHandler->FindJob(iParameter_ID);
	if( !pJob )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Disk_Drive::CMD_Abort_Task invalid job %d",iParameter_ID);
		return;
	}

	pJob->Abort();
}
//<-dceag-c914-b->

	/** @brief COMMAND: #914 - Get Disk Info */
	/** Retrieve the information on the current disk */
		/** @param #9 sRippingStatus */
			/** If ripping going on, this will contain the ripping status */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #131 EK_Dics */
			/** PK_Disc from pluto_media */
		/** @param #157 Disks */
			/** The disk id */
		/** @param #193 URL */
			/** The URL/MRL to play */
		/** @param #223 Block Device */
			/** The block device for the drive */

void Disk_Drive::CMD_Get_Disk_Info(string *sRippingStatus,int *iPK_MediaType,int *iEK_Disc,string *sDisks,string *sURL,string *sBlock_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c914-e->
{
	m_pDisk_Drive_Functions->internal_reset_drive(false,iPK_MediaType,sDisks,sURL,sBlock_Device,true);
}

void Disk_Drive::VerifyDriveIsNotEmbedded(string &sDrive)
{
/*******************************************************************************************/
/* FIXME: to support local jukeboxes this code needs to be refactored to use udev not hal. */
/* FIXME: phenigma - 27/03/2015                                                            */
/*******************************************************************************************/
/*
	// There's a problem in Linux that it often creates the /dev/cdrom symlinc
	// to a drive that's really embedded in one of the jukeboxes, and not the main cdrom.
	// Go through all embedded disk drives, and if this is a symlinc to one, change it to
	// something else

	int iDrive = FileUtils::GetLinuxDeviceId(sDrive);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::VerifyDriveIsNotEmbedded %d: %s",  iDrive, sDrive.c_str());

	bool bNeedToChangeDrive=false; // Will set to true if we're using the same thing as an embedded
	list<int> listEmbeddedDrives;
	for(Map_DeviceData_Base::iterator it=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();it!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++it)
	{
		DeviceData_Base *pDeviceData_Base = (*it).second;
		if( pDeviceData_Base->m_dwPK_DeviceTemplate==DEVICETEMPLATE_Disc_Drive_Embedded_CONST )
		{
			string s = m_pEvent->GetDeviceDataFromDatabase(pDeviceData_Base->m_dwPK_Device,DEVICEDATA_Drive_CONST);
			if( s.size() )
			{
				int iDrive2 = FileUtils::GetLinuxDeviceId(s);
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::VerifyDriveIsNotEmbedded found embedded %d - %s", iDrive2, s.c_str());
				if( iDrive==iDrive2 )
					bNeedToChangeDrive=true;
				listEmbeddedDrives.push_back(iDrive2);
			}
		}
	}

	// Go through the whole hal tree and find a cd-rom drive that's not embedded
	if( bNeedToChangeDrive )
	{
		HalTree halTree;
		halTree.Populate();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::VerifyDriveIsNotEmbedded need to change scanning %d items",halTree.m_mapHalDevice.size());
		for(map<int,HalDevice *>::iterator it=halTree.m_mapHalDevice.begin();it!=halTree.m_mapHalDevice.end();++it)
		{
			HalDevice *pHalDevice = it->second;
			HalValue *pHalValue = pHalDevice->m_mapHalValue_Find("storage.drive_type");
			if( !pHalValue || !pHalValue->GetType()==HalValue::hvt_string || ((HalValue_string *) pHalValue)->m_sValue!="cdrom" )
				continue; // Not of interest to us

			pHalValue = pHalDevice->m_mapHalValue_Find("block.device");
			if( !pHalValue || !pHalValue->GetType()==HalValue::hvt_string )
				continue; // Not of interest to us

			string sValue = ((HalValue_string *) pHalValue)->m_sValue;
			int iDrive2 = FileUtils::GetLinuxDeviceId(sValue);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::VerifyDriveIsNotEmbedded checking %s against %d",sValue.c_str(),listEmbeddedDrives.size());
			bool bIsOk=true;
			for(list<int>::iterator it=listEmbeddedDrives.begin();it!=listEmbeddedDrives.end();++it)
			{
				if( iDrive2==*it )
				{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::VerifyDriveIsNotEmbedded skipping %d,%s is already in use", iDrive2,sValue.c_str());
					bIsOk=false;
					break;
				}
			}
			if( bIsOk )
			{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive::VerifyDriveIsNotEmbedded %d,%s is a go", iDrive2,sValue.c_str());
				SetDeviceDataInDB(m_dwPK_Device,DEVICEDATA_Drive_CONST,sValue);
				sDrive=sValue;
				return;
			}
		}
	}
*/
}

bool Disk_Drive::SafeToReload(string &sReason)
{
	PLUTO_SAFETY_LOCK(dm,m_pDisk_Drive_Functions->m_DiskMutex);

	if(m_pJobHandler->HasJobs()==false)
		return true;

	sReason = "See Pending Tasks.  Disk_Drive is busy.";
	return false;
}
