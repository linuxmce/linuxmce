/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Disk_Drive_h
#define Disk_Drive_h

//	DCE Implemenation for #11 Disk Drive

#include "Gen_Devices/Disk_DriveBase.h"
//<-dceag-d-e->

#include <pthread.h>
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include "JobHandler/JobHandler.h"

using namespace nsJobHandler;

class Database_pluto_media;

//<-dceag-decl-b->
namespace DCE
{
	class Disk_Drive : public Disk_Drive_Command
	{
//<-dceag-decl-e->
		Database_pluto_media *m_pDatabase_pluto_media;
		MediaAttributes_LowLevel *m_pMediaAttributes_LowLevel;
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Disk_Drive(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Disk_Drive();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		
		// Private member variables

		JobHandler *m_pJobHandler;
		bool m_monitorEnabled;
		int  m_serverPid;
		int  m_serverPort;

		char *m_args[100];

public:
    // Public member variables
	virtual void PostConnect();

    virtual void RunMonitorLoop();

	virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);

	void VerifyDriveIsNotEmbedded(string &sDrive);

	bool SafeToReload(string &sReason);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Drive();
	void DATA_Set_Drive(string Value);
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();
	bool DATA_Get_Send_Events();

			*****EVENT***** accessors inherited from base class
	void EVENT_Media_Inserted(int iFK_MediaType,string sMRL,string sID,string sName);

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Get_Disk_Info(string *sRippingStatus, int *iPK_MediaType, int *iEK_Disc, string *sDisks,string *sURL,string *sBlock_Device) { string sCMD_Result; CMD_Get_Disk_Info(sRippingStatus,iPK_MediaType,iEK_Disc,sDisks,sURL,sBlock_Device,sCMD_Result,NULL);};
	virtual void CMD_Get_Disk_Info(string *sRippingStatus, int *iPK_MediaType, int *iEK_Disc, string *sDisks,string *sURL,string *sBlock_Device,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	private:
		Disk_Drive_Functions * m_pDisk_Drive_Functions;
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
