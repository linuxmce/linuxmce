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
//<-dceag-d-b->
#ifndef Powerfile_C200_h
#define Powerfile_C200_h

//	DCE Implemenation for #1737 Powerfile C200

#include "Gen_Devices/Powerfile_C200Base.h"
//<-dceag-d-e->
#ifdef NOTDEF
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include <JobHandler/Job.h>
#include <JobHandler/Task.h>
#include "Media_Plugin/MediaAttributes_LowLevel.h"
using namespace nsJobHandler;
#endif
#include <vector>

class Row_Disc;

namespace nsJukeBox
{
	class PowerfileJukebox;
}

//<-dceag-decl-b->
namespace DCE
{
	class Powerfile_C200 : public Powerfile_C200_Command
	{
//<-dceag-decl-e->
		// Private member variables
		friend class PowerfileRip_Task;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Powerfile_C200();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		bool SafeToReload(string &sReason);
		bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
		void PostConnect();

//<-dceag-const2-b->!
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Defective_Units();
	string DATA_Get_Hacks();

			*****EVENT***** accessors inherited from base class
	void EVENT_Media_Inserted(int iFK_MediaType,string sMRL,string sID,string sName);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #14 - Refresh */
	/** Refresh the status of the drives */
		/** @param #15 DataGrid ID */
			/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

	virtual void CMD_Refresh(string sDataGrid_ID) { string sCMD_Result; CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

	virtual void CMD_Disk_Drive_Monitoring_ON() { string sCMD_Result; CMD_Disk_Drive_Monitoring_ON(sCMD_Result,NULL);};
	virtual void CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

	virtual void CMD_Disk_Drive_Monitoring_OFF() { string sCMD_Result; CMD_Disk_Drive_Monitoring_OFF(sCMD_Result,NULL);};
	virtual void CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */

	virtual void CMD_Reset_Disk_Drive() { string sCMD_Result; CMD_Reset_Disk_Drive(sCMD_Result,NULL);};
	virtual void CMD_Reset_Disk_Drive(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */
		/** @param #151 Slot Number */
			/** For jukeboxes, which slot to eject */

	virtual void CMD_Eject_Disk(int iSlot_Number) { string sCMD_Result; CMD_Eject_Disk(iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Eject_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

	virtual void CMD_Start_Burn_Session() { string sCMD_Result; CMD_Start_Burn_Session(sCMD_Result,NULL);};
	virtual void CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

	virtual void CMD_Start_Ripping_Session() { string sCMD_Result; CMD_Start_Ripping_Session(sCMD_Result,NULL);};
	virtual void CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

	virtual void CMD_Add_File_To_Burning_Session() { string sCMD_Result; CMD_Add_File_To_Burning_Session(sCMD_Result,NULL);};
	virtual void CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

	virtual void CMD_Start_Burning() { string sCMD_Result; CMD_Start_Burning(sCMD_Result,NULL);};
	virtual void CMD_Start_Burning(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

	virtual void CMD_Abort_Burning() { string sCMD_Result; CMD_Abort_Burning(sCMD_Result,NULL);};
	virtual void CMD_Abort_Burning(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #54 - Mount Disk Image */
	/** Will mount a disk image as a disk. */
		/** @param #13 Filename */
			/** What to mount. If it get's the Device name it will mount the actual disk in the drive. */
		/** @param #59 MediaURL */
			/** The URL which can be used to play the mounted media. */

	virtual void CMD_Mount_Disk_Image(string sFilename,string *sMediaURL) { string sCMD_Result; CMD_Mount_Disk_Image(sFilename.c_str(),sMediaURL,sCMD_Result,NULL);};
	virtual void CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #55 - Abort Ripping */
	/** Aborts ripping a DVD. */

	virtual void CMD_Abort_Ripping() { string sCMD_Result; CMD_Abort_Ripping(sCMD_Result,NULL);};
	virtual void CMD_Abort_Ripping(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

	virtual void CMD_Format_Drive() { string sCMD_Result; CMD_Format_Drive(sCMD_Result,NULL);};
	virtual void CMD_Format_Drive(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

	virtual void CMD_Close_Tray() { string sCMD_Result; CMD_Close_Tray(sCMD_Result,NULL);};
	virtual void CMD_Close_Tray(string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory) { string sCMD_Result; CMD_Rip_Disk(iPK_Device,sFilename.c_str(),iPK_Users,sFormat.c_str(),sTracks.c_str(),iEK_Disc,iSlot_Number,iDriveID,sDirectory.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #701 - Load from Slot into Drive */
	/** Load disc from "Storage Element" (Slot) to "Data Transfer Element" (Drive).  This will lock the drive, which can be -1 for any drive */
		/** @param #2 PK_Device */
			/** The drive to load to.  Can pass in -1 and output will be the actual drive used and locked */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc from */

	virtual void CMD_Load_from_Slot_into_Drive(int iSlot_Number,int *iPK_Device) { string sCMD_Result; CMD_Load_from_Slot_into_Drive(iSlot_Number,iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Load_from_Slot_into_Drive(int iSlot_Number,int *iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #702 - Unload from Drive into Slot */
	/** Unload disc from "Data Transfer Element" (Drive) to "Storage Element" (Slot).  This will unlock the drive */
		/** @param #2 PK_Device */
			/** The drive */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc to.  Can pass in -1 and output will be the slot used */

	virtual void CMD_Unload_from_Drive_into_Slot(int iPK_Device,int *iSlot_Number) { string sCMD_Result; CMD_Unload_from_Drive_into_Slot(iPK_Device,iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Unload_from_Drive_into_Slot(int iPK_Device,int *iSlot_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #703 - Get Jukebox Status */
	/** Get jukebox slot and unit usage */
		/** @param #21 Force */
			/** If not empty, force=true. Else false. */
		/** @param #153 Jukebox Status */
			/** Jukebox Status in the following format: <Unit>=<State>[,<Unit>=<State>...]
where:
<Unit>=D0,D1 for the disc units, S1..200 for the disc storage slots
<State>=Empty or Full; occupied disc units may return Full-<number> where <number> is the source slot */

	virtual void CMD_Get_Jukebox_Status(string sForce,string *sJukebox_Status) { string sCMD_Result; CMD_Get_Jukebox_Status(sForce.c_str(),sJukebox_Status,sCMD_Result,NULL);};
	virtual void CMD_Get_Jukebox_Status(string sForce,string *sJukebox_Status,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #720 - Bulk Rip */
	/** Mass ripping */
		/** @param #13 Filename */
			/** A path where to put the files.  By default it will be /home/[public | user_xxx]/data/[music|movies].

If this parameter starts with a "/" it is considered absolute.  Otherwise it is considered a sub-directory under the aforementioned default */
		/** @param #17 PK_Users */
			/** 0 to rip as public, or the user id to rip as private */
		/** @param #20 Format */
			/** For cd's only, flac,mp3,wav,ogg */
		/** @param #157 Disks */
			/** Comma delimited list of slot number. For all, use "*". */

	virtual void CMD_Bulk_Rip(string sFilename,int iPK_Users,string sFormat,string sDisks) { string sCMD_Result; CMD_Bulk_Rip(sFilename.c_str(),iPK_Users,sFormat.c_str(),sDisks.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Bulk_Rip(string sFilename,int iPK_Users,string sFormat,string sDisks,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #738 - Play Disk */
	/** Play the disk from specified slot. Automatically do the behind logic. Eject a disk from drive if necessary. */
		/** @param #151 Slot Number */
			/** Slot number */

	virtual void CMD_Play_Disk(int iSlot_Number) { string sCMD_Result; CMD_Play_Disk(iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Play_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #739 - Get Bulk Ripping Status */
	/** Get status/progress of bulk ripping operation */
		/** @param #158 Bulk rip status */
			/** Returns:
S1-F,S2-S,S3-R,S4-N
F = failed
S = succeeded
R = ripping
N = not processed yet
empty string when idle or reset; last status when all disks were ripped
only slots that were scheduled for ripping will appear in the string */

	virtual void CMD_Get_Bulk_Ripping_Status(string *sBulk_rip_status) { string sCMD_Result; CMD_Get_Bulk_Ripping_Status(sBulk_rip_status,sCMD_Result,NULL);};
	virtual void CMD_Get_Bulk_Ripping_Status(string *sBulk_rip_status,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #740 - Mass identify media */
	/** Scan all loaded discs and identify each one */
		/** @param #157 Disks */
			/** Comma separated list of slots to identify. "*" for all */

	virtual void CMD_Mass_identify_media(string sDisks) { string sCMD_Result; CMD_Mass_identify_media(sDisks.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Mass_identify_media(string sDisks,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc) { string sCMD_Result; CMD_Media_Identified(iPK_Device,sValue_To_Assign.c_str(),sID.c_str(),pData,iData_Size,sFormat.c_str(),iPK_MediaType,sMediaURL.c_str(),sURL.c_str(),iEK_Disc,sCMD_Result,NULL);};
	virtual void CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #743 - Cancel Pending Task */
	/** Cancels a pending task */
		/** @param #151 Slot Number */
			/** Slot to be canceled */

	virtual void CMD_Cancel_Pending_Task(int iSlot_Number) { string sCMD_Result; CMD_Cancel_Pending_Task(iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Cancel_Pending_Task(int iSlot_Number,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name) { string sCMD_Result; CMD_Get_Default_Ripping_Info(iEK_Disc,sFilename,bUseDefault,sPath,iDriveID,sDirectory,sStorage_Device_Name,sCMD_Result,NULL);};
	virtual void CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob) { string sCMD_Result; CMD_Update_Ripping_Status(sText.c_str(),sFilename.c_str(),sTime.c_str(),sStatus.c_str(),iPercent,sTask.c_str(),sJob.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Lock(int iPK_Device,string sID,bool bTurn_On,string *sText,bool *bIsSuccessful) { string sCMD_Result; CMD_Lock(iPK_Device,sID.c_str(),bTurn_On,sText,bIsSuccessful,sCMD_Result,NULL);};
	virtual void CMD_Lock(int iPK_Device,string sID,bool bTurn_On,string *sText,bool *bIsSuccessful,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

	virtual void CMD_Abort_Task(int iParameter_ID) { string sCMD_Result; CMD_Abort_Task(iParameter_ID,sCMD_Result,NULL);};
	virtual void CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #913 - Load Disk */
	/** Load a disk into the jukebox */
		/** @param #259 Multiple */
			/** If true, load multiple disks */

	virtual void CMD_Load_Disk(bool bMultiple) { string sCMD_Result; CMD_Load_Disk(bMultiple,sCMD_Result,NULL);};
	virtual void CMD_Load_Disk(bool bMultiple,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #914 - Get Disk Info */
	/**  */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #157 Disks */
			/** The disk id */
		/** @param #193 URL */
			/** The URL/MRL to play */
		/** @param #223 Block Device */
			/** The block device for the drive */

	virtual void CMD_Get_Disk_Info(string *sAaronSpecial, int *iPK_MediaType, int *iAaronSpecial_EKID, string *sDisks,string *sURL,string *sBlock_Device) { string sCMD_Result; CMD_Get_Disk_Info(sAaronSpecial, iPK_MediaType, iAaronSpecial_EKID, sDisks,sURL,sBlock_Device,sCMD_Result,NULL);};
	virtual void CMD_Get_Disk_Info(string *sAaronSpecial, int *iPK_MediaType, int *iAaronSpecial_EKID, string *sDisks,string *sURL,string *sBlock_Device,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
		private:
			nsJukeBox::PowerfileJukebox * m_pPowerfileJukebox;

		public:
			bool RippingProgress(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo);

			int GetFreeDrive(int iSlot);
#ifdef NOTDEF
			Disk_Drive_Functions * GetDDF(int iDrive_Number);
#endif

		private:
			void ReleaseDrive(int iDrive_Number, int iSlot);
			void ReleaseDrive_NoMutex(int iDrive_Number, int iSlot);
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
/*
namespace DCE
{
	class Powerfile_Job : public Job
	{
		public:
			Powerfile_Job(string sName, Powerfile_C200 * pPowerfile_C200) : Job(sName), m_pPowerfile_C200(pPowerfile_C200), m_JobMutex("Powerfile Job Mutex", true) { }
			int PercentComplete() { return 0; }
			int SecondsRemaining() { return 0; }
			string ToString();

			void Remove_PowerfileTask_Slot(int iSlot);
			
			int MediaIdentified(int iSlot);
			void RippingProgress_End(int iDrive_Number, int iResult);
			void RippingProgress_Going(int iDrive_Number, int iPercent, string sName);

			Powerfile_C200 * m_pPowerfile_C200;
			pluto_pthread_mutex_t m_JobMutex;
	};

	class Powerfile_Task : public Task
	{
		public:
			Powerfile_Task(string sName, int iPriority, Job * pJob) : Task(sName, iPriority, pJob),
				m_bStop(false), m_pDDF(NULL), m_ePreTaskStatus(TASK_NOT_STARTED) {}
			
			bool m_bStop;
			int m_iDrive_Number;
			int m_iSlot;
			TaskStatus m_ePreTaskStatus;
			Disk_Drive_Functions * m_pDDF;

		protected:
			void ThreadStarted();
			void ThreadEnded();
	};
	
	class PowerfileRip_Task : public Powerfile_Task
	{
		public:
			int m_iPK_Users;
			string m_sFormat,m_sPath;
			map<int,string> m_mapTracks;

			PowerfileRip_Task(int PK_Users, string sFormat, string sPath, string sName, int iPriority, Job * pJob) : Powerfile_Task(sName, iPriority, pJob) { m_iPK_Users=PK_Users; m_sFormat=sFormat; m_sPath=sPath; }
			string Type() { return "Rip"; }
			string ToString();
			void Run();

			void RipCD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_);
			void RipDVD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_);
	};

	class PowerfileIdentify_Task : public Powerfile_Task
	{
		public:
			PowerfileIdentify_Task(string sName, int iPriority, Job * pJob) : Powerfile_Task(sName, iPriority, pJob) {}
			string Type() { return "Identify"; }
			string ToString();
			void Run();
	};
}
	*/
