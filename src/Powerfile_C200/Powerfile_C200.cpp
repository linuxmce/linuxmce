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
#include "Powerfile_C200.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Define_AttributeType.h"
#include "Utils.h"

using namespace StringUtils;

#include "PowerfileJukebox.h"
#include "Disk_Drive_Functions/RipJob.h"
#include "Disk_Drive_Functions/RipTask.h"
#include "Disk_Drive_Functions/ProcessMediaIdentifiedJob.h"

namespace nsJobHandler
{
	int g_ID = 0;
}

using namespace nsJukeBox;

#ifdef WIN32
	#define WEXITSTATUS(a) 0
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Powerfile_C200::Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Powerfile_C200_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Powerfile_C200::~Powerfile_C200()
//<-dceag-dest-e->
{
#ifdef NOTDEF
	for (size_t i = 0; i < m_vectDDF.size(); i++)
		delete m_vectDDF[i];
	delete m_pDatabase_pluto_media;
#endif
}


//<-dceag-getconfig-b->
bool Powerfile_C200::GetConfig()
{
	if( !Powerfile_C200_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
//m_sIPAddress = ;  // todo
	
	m_pPowerfileJukebox = new nsJukeBox::PowerfileJukebox(this);
	if (!m_pPowerfileJukebox->Init())
		return false;
	
#ifdef NOTDEF
	PurgeInterceptors();
	// MessageInterceptorFn, int Device_From, int Device_To, int Device_Type, int Device_Category, int Message_Type, int Message_ID
	RegisterMsgInterceptor((MessageInterceptorFn)(&Powerfile_C200::RippingProgress), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Ripping_Progress_CONST);
#endif

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Powerfile_C200::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Powerfile_C200::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	Drive *pDrive = m_pPowerfileJukebox->m_mapDrive_FindByPK_Device(pMessage->m_dwPK_Device_To);
	if( !pDrive )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::ReceivedCommandForChild Unknown device: %d", pMessage->m_dwPK_Device_To);
	else
	{
		if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
		{
			sCMD_Result = "OK";
			switch( pMessage->m_dwID )
			{
			case COMMAND_Mass_identify_media_CONST:
				pDrive->IdDisk(true);
				return;

			case COMMAND_Eject_Disk_CONST:
				m_pPowerfileJukebox->Eject(pDrive,pMessage->m_dwPK_Device_From);
				return;

			case COMMAND_Unload_from_Drive_into_Slot_CONST:
				{
					int iSlot = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Slot_Number_CONST].c_str());
					Drive *pDrive = m_pPowerfileJukebox->m_mapDrive_FindByPK_Device(pMessage->m_dwPK_Device_To);
					if( !pDrive )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::ReceivedCommandForChild Unknown drive: %d", pMessage->m_dwPK_Device_To);
					else
						CMD_Unload_from_Drive_into_Slot(pDrive->m_dwPK_Device_get(),&iSlot,sCMD_Result,pMessage); // Forward this on with this is the drive
				}
				return;

			case COMMAND_Lock_CONST:
				{
					Drive *pDrive = m_pPowerfileJukebox->m_mapDrive_FindByPK_Device(pMessage->m_dwPK_Device_To);
					bool bIsSuccessful=false;
					bool bTurn_On=(pMessage->m_mapParameters[COMMANDPARAMETER_Turn_On_CONST]=="1" ? true : false);
					if( pDrive )
					{
						if( bTurn_On )
							bIsSuccessful = pDrive->LockDrive(Disk_Drive_Functions::locked_playback,this);
						else
						{
							bIsSuccessful = true;
							pDrive->UnlockDrive();
						}
					}

					if( !bIsSuccessful )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::ReceivedCommandForChild failed to %s %d", bTurn_On ? "lock" : "unlock", pMessage->m_dwPK_Device_To);

					if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage ) // Should always be the case
					{
						pMessage->m_bRespondedToMessage=true;
						Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[COMMANDPARAMETER_IsSuccessful_CONST]=(bIsSuccessful ? "1" : "0");
						pMessageOut->m_mapParameters[0]=sCMD_Result;
						SendMessage(pMessageOut);
					}
				}
				return;

			case COMMAND_Get_Disk_Info_CONST:
				{
					Drive *pDrive = m_pPowerfileJukebox->m_mapDrive_FindByPK_Device(pMessage->m_dwPK_Device_To);
					if( !pDrive )
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::ReceivedCommandForChild Unknown drive: %d", pMessage->m_dwPK_Device_To);
					else
					{
						int iPK_MediaType;
						string sDisks,sURL,sBlock_Device;
						pDrive->internal_reset_drive(false,&iPK_MediaType,&sDisks,&sURL,&sBlock_Device,true);

						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Powerfile_C200::ReceivedCommandForChild Get disk info media type %d disks %s url %s block %s",
							iPK_MediaType,sDisks.c_str(),sURL.c_str(),sBlock_Device.c_str());

						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[COMMANDPARAMETER_PK_MediaType_CONST]=StringUtils::itos(iPK_MediaType);
							pMessageOut->m_mapParameters[COMMANDPARAMETER_Disks_CONST]=sDisks;
							pMessageOut->m_mapParameters[COMMANDPARAMETER_URL_CONST]=sURL;
							pMessageOut->m_mapParameters[COMMANDPARAMETER_Block_Device_CONST]=sBlock_Device;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
							return;
						}
					}
				}

			case COMMAND_Rip_Disk_CONST:
			case COMMAND_Media_Identified_CONST:
			case COMMAND_Abort_Task_CONST:
			{
				// Just forward this to our own handler
				pMessage->m_mapParameters[COMMANDPARAMETER_PK_Device_CONST] = StringUtils::itos(pMessage->m_dwPK_Device_To);
				pMessage->m_dwPK_Device_To = m_dwPK_Device;
				ReceivedMessage(pMessage);
				return;
			}

			default:
				sCMD_Result = "UNHANDLED COMMAND";
				return;
			};
		}
	}
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Powerfile_C200::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	// Temp test to debug memory
	if (pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pMessage->m_dwID == 999)
	{
		for(map_int_Slotp::iterator it=m_pPowerfileJukebox->m_mapSlot.begin();it!=m_pPowerfileJukebox->m_mapSlot.end();++it)
		{
			Slot *pSlot = it->second;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Slot #%d st %d disc %d", 
				pSlot->m_SlotNumber, (int) pSlot->m_eStatus, pSlot->m_pRow_Disc ? pSlot->m_pRow_Disc->PK_Disc_get() : -1);
		}
		for(map_int_Drivep::iterator it=m_pPowerfileJukebox->m_mapDrive.begin();it!=m_pPowerfileJukebox->m_mapDrive.end();++it)
		{
			Drive *pDrive = it->second;
			void *p;;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Dive #%d st %d locked %d", 
				pDrive->m_dwPK_Device_get(),
				pDrive->m_mediaDiskStatus, (int) pDrive->m_eLocked_get(&p));
		}
	}

#ifdef NOTDEF
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unknown command: type %d, id %d", pMessage->m_dwMessage_Type, pMessage->m_dwID);
	if (pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT && pMessage->m_dwID == EVENT_Ripping_Progress_CONST)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received event Ripping_Progress as unknown command");
	}
	sCMD_Result = "UNKNOWN DEVICE";
#endif
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c701-b->

	/** @brief COMMAND: #701 - Load from Slot into Drive */
	/** Load disc from "Storage Element" (Slot) to "Data Transfer Element" (Drive).  This will lock the drive, which can be -1 for any drive */
		/** @param #2 PK_Device */
			/** The drive to load to.  Can pass in -1 and output will be the actual drive used and locked */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc from */

void Powerfile_C200::CMD_Load_from_Slot_into_Drive(int iSlot_Number,int *iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c701-e->
{
	if( m_pPowerfileJukebox->Load_from_Slot_into_Drive(iSlot_Number,iPK_Device,pMessage->m_dwPK_Device_From)!=JukeBox::jukebox_ok )
		sCMD_Result="JUKEBOX ERROR";
		
}

//<-dceag-c702-b->

	/** @brief COMMAND: #702 - Unload from Drive into Slot */
	/** Unload disc from "Data Transfer Element" (Drive) to "Storage Element" (Slot).  This will unlock the drive */
		/** @param #2 PK_Device */
			/** The drive */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc to.  Can pass in -1 and output will be the slot used */

void Powerfile_C200::CMD_Unload_from_Drive_into_Slot(int iPK_Device,int *iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c702-e->
{
	if( m_pPowerfileJukebox->Unload_from_Drive_into_Slot(iSlot_Number,iPK_Device,pMessage->m_dwPK_Device_From)!=JukeBox::jukebox_ok )
		sCMD_Result="JUKEBOX ERROR";
}

//<-dceag-c703-b->

	/** @brief COMMAND: #703 - Get Jukebox Status */
	/** Get jukebox slot and unit usage */
		/** @param #21 Force */
			/** If not empty, force=true. Else false. */
		/** @param #153 Jukebox Status */
			/** Jukebox Status in the following format: <Unit>=<State>[,<Unit>=<State>...]
where:
<Unit>=D0,D1 for the disc units, S1..200 for the disc storage slots
<State>=Empty or Full; occupied disc units may return Full-<number> where <number> is the source slot */

void Powerfile_C200::CMD_Get_Jukebox_Status(string sForce,string *sJukebox_Status,string &sCMD_Result,Message *pMessage)
//<-dceag-c703-e->
{
	bool bForce = sForce.size() != 0;
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Getting jukebox status");

	sCMD_Result = "FAILED";
	if (m_pPowerfileJukebox->Get_Jukebox_Status(sJukebox_Status,sForce=="1"))
		sCMD_Result = "OK";
}
//<-dceag-c45-b->

	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

void Powerfile_C200::CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage)
//<-dceag-c45-e->
{
}

//<-dceag-c46-b->

	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

void Powerfile_C200::CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage)
//<-dceag-c46-e->
{
}

//<-dceag-c47-b->

	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */

void Powerfile_C200::CMD_Reset_Disk_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c47-e->
{
#ifdef NOTDEF
	Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
	if (pDDF)
	{
		pDDF->m_mediaInserted = false;
		pDDF->m_mediaDiskStatus = DISCTYPE_NONE;
		pDDF->DisplayMessageOnOrbVFD("Checking disc...");

		int iPK_MediaType;
		string sDisks,sURL,sBlock_Device;
		pDDF->internal_reset_drive(true,&iPK_MediaType,&sDisks,&sURL,&sBlock_Device);
	}
#endif
}

//<-dceag-c48-b->

	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */
		/** @param #151 Slot Number */
			/** For jukeboxes, which slot to eject */

void Powerfile_C200::CMD_Eject_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c48-e->
{
	m_pPowerfileJukebox->Eject(iSlot_Number,0,pMessage->m_dwPK_Device_From);
}

//<-dceag-c49-b->

	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

void Powerfile_C200::CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c49-e->
{
}

//<-dceag-c50-b->

	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

void Powerfile_C200::CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c50-e->
{
}

//<-dceag-c51-b->

	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

void Powerfile_C200::CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c51-e->
{
}

//<-dceag-c52-b->

	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

void Powerfile_C200::CMD_Start_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c52-e->
{
}

//<-dceag-c53-b->

	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

void Powerfile_C200::CMD_Abort_Burning(string &sCMD_Result,Message *pMessage)
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

void Powerfile_C200::CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c54-e->
{
}

//<-dceag-c55-b->

	/** @brief COMMAND: #55 - Abort Ripping */
	/** Aborts ripping a DVD. */

void Powerfile_C200::CMD_Abort_Ripping(string &sCMD_Result,Message *pMessage)
//<-dceag-c55-e->
{
	m_pPowerfileJukebox->AbortAll();
}

//<-dceag-c56-b->

	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

void Powerfile_C200::CMD_Format_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c56-e->
{
}

//<-dceag-c57-b->

	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

void Powerfile_C200::CMD_Close_Tray(string &sCMD_Result,Message *pMessage)
//<-dceag-c57-e->
{
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

void Powerfile_C200::CMD_Rip_Disk(int iPK_Device,string sFilename,int iPK_Users,string sFormat,string sTracks,int iEK_Disc,int iSlot_Number,int iDriveID,string sDirectory,string &sCMD_Result,Message *pMessage)
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

	Drive *pDrive = m_pPowerfileJukebox->m_mapDrive_Find(iPK_Device);  // May be NULL if we're supposed to just pick a drive
	Slot *pSlot = m_pPowerfileJukebox->m_mapSlot_Find(iSlot_Number);

	if( !pDrive && !pSlot )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::CMD_Rip_Disk no drive and no slot");
		return;
	}
	RipJob *pRipJob = new RipJob(m_pPowerfileJukebox->m_pDatabase_pluto_media,
		m_pPowerfileJukebox->m_pJobHandler,pDrive,pSlot,iPK_Users,iEK_Disc,
		pMessage ? pMessage->m_dwPK_Device_From : 0,sFormat.empty() ? "ogg" : sFormat,sFilename,sDirectory,sTracks,true,this);
	m_pPowerfileJukebox->m_pJobHandler->AddJob(pRipJob);
}
//<-dceag-c720-b->

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

void Powerfile_C200::CMD_Bulk_Rip(string sFilename,int iPK_Users,string sFormat,string sDisks,string &sCMD_Result,Message *pMessage)
//<-dceag-c720-e->
{
	size_t i;

	vector<string> vect_sSlots;
	if (sDisks == "*" || sDisks.empty())
	{
		for(map_int_Slotp::iterator it=m_pPowerfileJukebox->m_mapSlot.begin();it!=m_pPowerfileJukebox->m_mapSlot.end();++it)
		{
			Slot *pSlot = it->second;
			if( pSlot->m_eStatus==Slot::slot_identified_disc || pSlot->m_eStatus==Slot::slot_unknown_medium )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Bulk_Rip; Adding slot %d/%d disc %d", pSlot->m_SlotNumber, (int) pSlot->m_eStatus, pSlot->m_pRow_Disc ? pSlot->m_pRow_Disc->PK_Disc_get() : 0);
				vect_sSlots.push_back(StringUtils::itos(pSlot->m_SlotNumber));
			}
		}
	}
	else
		StringUtils::Tokenize(sDisks, ",", vect_sSlots);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Bulk_Rip; sDisks='%s' slots %d/%d", sDisks.c_str(), m_pPowerfileJukebox->m_mapSlot.size(), (int) vect_sSlots.size());

	for (i = 0; i < vect_sSlots.size(); i++)
	{
		string sSlot = vect_sSlots[i];
		int iSlot = atoi(sSlot.c_str());
		Slot *pSlot = m_pPowerfileJukebox->m_mapSlot_Find(iSlot);
		if( !pSlot )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::CMD_Bulk_Rip Can't find slot %d", iSlot);
		else if( pSlot->m_eStatus==Slot::slot_identified_disc || pSlot->m_eStatus==Slot::slot_unknown_medium )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Bulk_Rip; Adding slot %d/%d disc %d", pSlot->m_SlotNumber, (int) pSlot->m_eStatus,  pSlot->m_pRow_Disc ? pSlot->m_pRow_Disc->PK_Disc_get() : 0);
			RipJob *pRipJob = new RipJob(m_pPowerfileJukebox->m_pDatabase_pluto_media,
				m_pPowerfileJukebox->m_pJobHandler,NULL,pSlot,iPK_Users,0,
				pMessage ? pMessage->m_dwPK_Device_From : 0,sFormat.empty() ? "ogg" : sFormat,"",sFilename,"A",false,this);
			m_pPowerfileJukebox->m_pJobHandler->AddJob(pRipJob);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Bulk_Rip; Skipping slot %d/%d disc %d", pSlot->m_SlotNumber, (int) pSlot->m_eStatus, pSlot->m_pRow_Disc ? pSlot->m_pRow_Disc->PK_Disc_get() : 0);
	}
}

//<-dceag-c738-b->

	/** @brief COMMAND: #738 - Play Disk */
	/** Play the disk from specified slot. Automatically do the behind logic. Eject a disk from drive if necessary. */
		/** @param #151 Slot Number */
			/** Slot number */

void Powerfile_C200::CMD_Play_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c738-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unimplemented: CMD_Play_Disk; iSlot_Number='%d'", iSlot_Number);
}
//<-dceag-c739-b->

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

void Powerfile_C200::CMD_Get_Bulk_Ripping_Status(string *sBulk_rip_status,string &sCMD_Result,Message *pMessage)
//<-dceag-c739-e->
{
#ifdef NOTDEF
#ifdef EMULATE_PF
	* sBulk_rip_status = "S1-F~S2-S~S3-R~S4-N";
#else
	* sBulk_rip_status = m_pJob->ToString();
#endif
	sCMD_Result = "OK";
#endif
}
//<-dceag-c740-b->

	/** @brief COMMAND: #740 - Mass identify media */
	/** Scan all loaded discs and identify each one */
		/** @param #157 Disks */
			/** Comma separated list of slots to identify. "*" for all */

void Powerfile_C200::CMD_Mass_identify_media(string sDisks,string &sCMD_Result,Message *pMessage)
//<-dceag-c740-e->
{
	m_pPowerfileJukebox->MassIdentify(sDisks,pMessage->m_dwPK_Device_From);
}

#ifdef NOTDEF
int Powerfile_C200::GetFreeDrive(int iSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	size_t i;

	for (i = 0; i < m_vectDriveStatus.size(); i++)
	{
		if (m_vectDriveStatus[i].second && (m_vectDriveStatus[i].first == 0 || abs(m_vectDriveStatus[i].first) == iSlot))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Free drive %d to slot %d", i, iSlot);
			m_vectDriveStatus[i].first = -iSlot;
			return i;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "No free drives for slot %d", iSlot);

	return -1;
}

void Powerfile_C200::ReleaseDrive(int iDrive_Number, int iSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	ReleaseDrive_NoMutex(iDrive_Number, iSlot);
}

void Powerfile_C200::ReleaseDrive_NoMutex(int iDrive_Number, int iSlot)
{
	if (abs(m_vectDriveStatus[iDrive_Number].first) != iSlot)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Slot %d tried to release drive %d which is in use by slot %d. Expect the application to mis-behave after this.",
			iSlot, iDrive_Number, abs(m_vectDriveStatus[iDrive_Number].first));
		return;
	}
	m_vectDriveStatus[iDrive_Number].first = 0;

	if (m_pJob)
	{
		Task * pTask = m_pJob->GetNextTask();
		if (pTask)
			pTask->Execute();
	}
}

string Powerfile_Job::ToString()
{
	string sResult;
	bool bComma = false;
	
	for(list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if (bComma)
			sResult += "~";
		else
			bComma = true;
		sResult += pTask->ToString();
	}
	return sResult;
}

string PowerfileRip_Task::ToString()
{
	static const char * textStatus[] = { "N", "R", "F", "S", "C" };
	string sResult = "S" + StringUtils::itos(m_iSlot) + "-" + textStatus[m_eTaskStatus] + " " + m_sResult;
	return sResult;
}

string PowerfileIdentify_Task::ToString()
{
	static const char * textStatus[] = { "N", "I", "F", "S", "C" };
	string sResult = "S" + StringUtils::itos(m_iSlot) + "-" + textStatus[m_eTaskStatus] + " " + m_sResult;
	return sResult;
}

void PowerfileRip_Task::Run()
{
	if (m_eTaskStatus == TASK_NOT_STARTED)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Task posponed. Exiting Run()");
		return; // task postponed
	}
	
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;

	listMediaAttribute listMediaAttribute_;
	int PK_Disc = pPowerfile_Job->m_pPowerfile_C200->m_pMediaAttributes_LowLevel->IsDiscAlreadyIdentified(pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot,listMediaAttribute_);
	Row_Disc *pRow_Disc;
	if( !PK_Disc || (pRow_Disc=pPowerfile_Job->m_pPowerfile_C200->m_pDatabase_pluto_media->Disc_get()->GetRow(PK_Disc))==NULL )
	{
		m_ePreTaskStatus=TASK_FAILED_CONTINUE;
		m_sResult="Disc has not been identified";
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot rip disk for device %d slot %d because it has not been identified",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot);
		return;
	}

	if( pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST )
		RipCD(pRow_Disc,listMediaAttribute_);
	else if( pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_DVD_CONST )
		RipDVD(pRow_Disc,listMediaAttribute_);
	else
	{
		m_ePreTaskStatus=TASK_FAILED_CONTINUE;
		m_sResult="Disc type is unknown";
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot rip disk for device %d slot %d because the type %d is unknown",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot,pRow_Disc->EK_MediaType_get());
	}
}

void PowerfileRip_Task::RipDVD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_)
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	string sTitle;

	for (listMediaAttribute::iterator it = listMediaAttribute_.begin(); it != listMediaAttribute_.end(); it++)
	{
		MediaAttribute * pMediaAttribute = * it;
		if (pMediaAttribute->m_Title_Track == 0 && pMediaAttribute->m_PK_AttributeType == ATTRIBUTETYPE_Title_CONST)
			sTitle = pMediaAttribute->m_sName;
	}

	if (! sTitle.size())
	{
		m_ePreTaskStatus = TASK_FAILED_CONTINUE;
		m_sResult = "DVD Title is not known";
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot rip disk for device %d slot %d because the title is unknown",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device, m_iSlot);
		return;
	}
	m_sPath = sTitle;

	string sPath;
	if (m_iPK_Users)
		sPath = "/home/user_" + StringUtils::itos(m_iPK_Users) + "/data/videos";
	else
		sPath = "/home/public/data/videos";
	if (m_sPath.size())
	{
		if (m_sPath[0] == '/')
			sPath = m_sPath;
		else
			sPath += "/" + m_sPath;
	}
	string sCMD_Result = "OK";

	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;
	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	m_pDDF->CMD_Rip_Disk(m_iPK_Users, m_sFormat, sPath, "", pRow_Disc->PK_Disc_get(), m_iDrive_Number, sCMD_Result, NULL);

	if (sCMD_Result == "OK")
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disc is ripping");
		while (! m_bStop)
		{
			Sleep(100);
		}
		if (m_ePreTaskStatus == TASK_CANCELED)
		{
			DCE::CMD_Kill_Application CMD_Kill_Application(iPK_Device, m_pDDF->m_pDevice_AppServer->m_dwPK_Device,
				"rip_" + StringUtils::itos(iPK_Device) + "_" + StringUtils::itos(m_iDrive_Number), true);
		}
		else if (m_ePreTaskStatus == TASK_COMPLETED)
		{
			pPowerfile_Job->m_pPowerfile_C200->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(pRow_Disc->PK_Disc_get(),
				MEDIATYPE_pluto_CD_CONST, sPath, "");
		}
	}
	else
	{
		m_bStop = true;
		m_ePreTaskStatus = TASK_FAILED_CONTINUE;
		m_sResult = "Ripping reported " + sCMD_Result;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disc failed to rip %s", sCMD_Result.c_str());
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Drive %d ripping task finished with status %d", m_iDrive_Number, m_ePreTaskStatus);
}

void PowerfileRip_Task::RipCD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_)
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;

	string sPerformer,sAlbum;
	bool bAllTracks = m_mapTracks.size()==0;
	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		if( pMediaAttribute->m_Title_Track==0 )
		{
			if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Performer_CONST )
				sPerformer = pMediaAttribute->m_sName;
			else if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
				sAlbum = pMediaAttribute->m_sName;
		}
		else if( bAllTracks || m_mapTracks.find(pMediaAttribute->m_Title_Track)!=m_mapTracks.end() )
		{
			if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Title_CONST )
				m_mapTracks[pMediaAttribute->m_Title_Track] = pMediaAttribute->m_sName;
		}
	}

	if( sPerformer.size()==0 || sAlbum.size()==0 )
	{
		m_ePreTaskStatus=TASK_FAILED_CONTINUE;
		m_sResult="CD Performer and Album are not known";
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot rip disk for device %d slot %d because the performer/album are unknown",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot);
		return;
	}

	DeviceData_Base * pDevice_MediaPlugin = pPowerfile_Job->m_pPowerfile_C200->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Media_Plugins_CONST);
	m_sPath = sPerformer + " - " + sAlbum;

	string sTracks;
	for(map<int,string>::iterator it=m_mapTracks.begin();it!=m_mapTracks.end();++it)
	{
		string sSong = it->second;
		if( sSong.size()==0 )
			sSong = "Track " + StringUtils::itos(it->first);
		sTracks += StringUtils::itos(it->first) + "," + sSong + "|";
	}

	string sPath;
	if( m_iPK_Users )
		sPath = "/home/user_" + StringUtils::itos(m_iPK_Users) + "/data/music";
	else
		sPath = "/home/public/data/music";
	if( m_sPath.size() )
	{
		if( m_sPath[0]=='/' )
			sPath = m_sPath;
		else
			sPath += "/" + m_sPath;
	}
	string sCMD_Result="OK";

	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;
	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	m_pDDF->CMD_Rip_Disk(m_iPK_Users, m_sFormat, sPath, sTracks, pRow_Disc->PK_Disc_get(), m_iDrive_Number, sCMD_Result, NULL);

	if( sCMD_Result=="OK" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disc is ripping");
		while (! m_bStop)
		{
			Sleep(100);
		}
		if (m_ePreTaskStatus == TASK_CANCELED)
		{
			DCE::CMD_Kill_Application CMD_Kill_Application(iPK_Device, m_pDDF->m_pDevice_AppServer->m_dwPK_Device,
				"rip_" + StringUtils::itos(iPK_Device) + "_" + StringUtils::itos(m_iDrive_Number), true);
		}
		else if (m_ePreTaskStatus == TASK_COMPLETED)
		{
			pPowerfile_Job->m_pPowerfile_C200->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(pRow_Disc->PK_Disc_get(),
				MEDIATYPE_pluto_CD_CONST, sPath, sTracks);
		}
	}
	else
	{
		m_bStop=true;
		m_ePreTaskStatus=TASK_FAILED_CONTINUE;
		m_sResult="Ripping reported " + sCMD_Result;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disc failed to rip %s",sCMD_Result.c_str());
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Drive %d ripping task finished with status %d", m_iDrive_Number, m_ePreTaskStatus);
}

void PowerfileIdentify_Task::Run()
{
	if (m_eTaskStatus == TASK_NOT_STARTED)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Task posponed. Exiting Run()");
		return; // task postponed
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PowerfileIdentify_Task::Run");
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	
	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;

	DCE::CMD_Identify_Media_Cat CMD_Identify_Media_Cat(iPK_Device, DEVICECATEGORY_Media_Identifiers_CONST,
		false, BL_SameComputer, iPK_Device, StringUtils::itos(m_iSlot), m_pDDF->m_sDrive,iPK_Device);
	pPowerfile_Job->m_pPowerfile_C200->SendCommand(CMD_Identify_Media_Cat);
	
	time_t TimeOut = time(NULL) + 60; // 60s timeout
	while (! m_bStop && time(NULL) < TimeOut)
	{
		Sleep(100);
	}
	if (m_bStop)
		m_ePreTaskStatus = TASK_COMPLETED;
	else // timeout
		m_ePreTaskStatus = TASK_FAILED_CONTINUE;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Drive %d identifying task finished with status %d", m_iDrive_Number, m_ePreTaskStatus);
}

void Powerfile_Task::ThreadStarted()
{
	m_eTaskStatus = TASK_IN_PROGRESS;
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	m_iDrive_Number = pPowerfile_Job->m_pPowerfile_C200->GetFreeDrive(m_iSlot);
	if (m_iDrive_Number == -1)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Asked to run, but no free drive. Task postponed");
		m_eTaskStatus = TASK_NOT_STARTED;
	}
	else
	{
		string sCmdResult = "FAILED";
		pPowerfile_Job->m_pPowerfile_C200->CMD_Load_from_Slot_into_Drive(m_iSlot, m_iDrive_Number, sCmdResult, NULL);
		if (sCmdResult != "OK")
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to load disc into drive. Task postponed");
			m_eTaskStatus = TASK_NOT_STARTED;
		}
	}
}

void Powerfile_Task::ThreadEnded()
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	if (m_iDrive_Number != -1)
		pPowerfile_Job->m_pPowerfile_C200->CMD_Unload_from_Drive_into_Slot(m_iSlot, m_iDrive_Number);
	m_eTaskStatus = m_ePreTaskStatus;
}

// returns the disc type
int Powerfile_Job::MediaIdentified(int iSlot)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received MediaIdentified event for slot %d", iSlot);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if ((pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_ePreTaskStatus == TASK_NOT_STARTED) && pTask->m_iSlot == iSlot)
		{
			pTask->m_bStop = true;
			return pTask->m_pDDF->m_mediaDiskStatus;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Slot %d not found in task list!", iSlot);
	return -1;
}

void Powerfile_Job::RippingProgress_End(int iDrive_Number, int iResult)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received RippingProgress message for drive %d, result %d", iDrive_Number, iResult);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if ((pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_ePreTaskStatus == TASK_NOT_STARTED) && pTask->m_iDrive_Number == iDrive_Number)
		{
			if (iResult == RIP_RESULT_SUCCESS)
			{
				pTask->m_ePreTaskStatus = TASK_COMPLETED;
			}
			else
			{
				pTask->m_sResult="Technical problem reading the disc";
				pTask->m_ePreTaskStatus = TASK_FAILED_CONTINUE;
			}
			pTask->m_bStop = true;
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Drive %d not found in task list!", iDrive_Number);
}

void Powerfile_Job::RippingProgress_Going(int iDrive_Number, int iPercent, string sName)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if ((pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_ePreTaskStatus == TASK_NOT_STARTED) && pTask->m_iDrive_Number == iDrive_Number)
		{
			pTask->m_sResult = sName + " " + StringUtils::itos(iPercent) + "%";
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Drive %d not found in task list!", iDrive_Number);
}

void Powerfile_Job::Remove_PowerfileTask_Slot(int iSlot)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Canceling job from slot %d", iSlot);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if (((pTask->m_eTaskStatus == TASK_IN_PROGRESS || pTask->m_eTaskStatus == TASK_NOT_STARTED) && pTask->m_ePreTaskStatus == TASK_NOT_STARTED)
			&& pTask->m_iSlot == iSlot)
		{
			pTask->m_sResult = "Canceled by user";
			pTask->m_ePreTaskStatus = TASK_CANCELED;
			pTask->m_bStop = true;

			if (pTask->m_eTaskStatus == TASK_NOT_STARTED)
				pTask->m_eTaskStatus = TASK_CANCELED;
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Slot %d not found in task list!", iSlot);
}
#endif
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

void Powerfile_C200::CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc,string &sCMD_Result,Message *pMessage)
//<-dceag-c742-e->
{
	ProcessMediaIdentifiedJob *pProcessMediaIdentifiedJob = new ProcessMediaIdentifiedJob(m_pPowerfileJukebox->m_pJobHandler,iPK_Device,sValue_To_Assign,sID,pData,iData_Size,sFormat,iPK_MediaType,sMediaURL,sURL,iEK_Disc,m_pPowerfileJukebox,NULL);
	m_pPowerfileJukebox->m_pJobHandler->AddJob(pProcessMediaIdentifiedJob);
}
//<-dceag-c743-b->

	/** @brief COMMAND: #743 - Cancel Pending Task */
	/** Cancels a pending task */
		/** @param #151 Slot Number */
			/** Slot to be canceled */

void Powerfile_C200::CMD_Cancel_Pending_Task(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c743-e->
{
#ifdef NOTDEF
	m_pJob->Remove_PowerfileTask_Slot(iSlot_Number);
#endif
	sCMD_Result = "OK";
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

void Powerfile_C200::CMD_Get_Default_Ripping_Info(int iEK_Disc,string *sFilename,bool *bUseDefault,string *sPath,int *iDriveID,string *sDirectory,string *sStorage_Device_Name,string &sCMD_Result,Message *pMessage)
//<-dceag-c817-e->
{
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

void Powerfile_C200::CMD_Update_Ripping_Status(string sText,string sFilename,string sTime,string sStatus,int iPercent,string sTask,string sJob,string &sCMD_Result,Message *pMessage)
//<-dceag-c871-e->
{
	PLUTO_SAFETY_LOCK(jm,*m_pPowerfileJukebox->m_pJobHandler->m_ThreadMutex_get());
	Task *pTask = m_pPowerfileJukebox->m_pJobHandler->FindTask(atoi(sJob.c_str()),atoi(sTask.c_str()));
	if( !pTask )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Powerfile_C200::CMD_Update_Ripping_Status invalid job %s task %s",sJob.c_str(),sTask.c_str());
		return;
	}

	RipTask *pRipTask = (RipTask *) pTask;
	pRipTask->UpdateProgress(sStatus,iPercent,atoi(sTime.c_str()),sText,sFilename);
}

bool Powerfile_C200::SafeToReload(string &sReason)
{
	PLUTO_SAFETY_LOCK(jm,*(m_pPowerfileJukebox->m_pJobHandler->m_ThreadMutex_get()));

	if(m_pPowerfileJukebox->m_pJobHandler->HasJobs()==false)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Powerfile_C200::SafeToReload true");
		return true;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Powerfile_C200::SafeToReload false");
	sReason = "See Pending Tasks.  Powerfile busy.";
	return false;
}

bool Powerfile_C200::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	return m_pPowerfileJukebox->m_pJobHandler->ReportPendingTasks(pPendingTaskList);
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

void Powerfile_C200::CMD_Lock(int iPK_Device,string sID,bool bTurn_On,string *sText,bool *bIsSuccessful,string &sCMD_Result,Message *pMessage)
//<-dceag-c872-e->
{
	// This is to lock a drive for playback.  We'll return the id of the drive we're locking
	if( bTurn_On )
	{
		Drive *pDrive = m_pPowerfileJukebox->LockAvailableDrive(Disk_Drive_Functions::locked_playback,NULL,this,true);
		if( pDrive )
		{
			*bIsSuccessful=true;
			*sText=StringUtils::itos( pDrive->m_dwPK_Device_get() );
		}
		else
		{
			*bIsSuccessful=false;
			*sText="NO DRIVES";
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Powerfile_C200::CMD_Lock unlock requests should be going to the drive, not the jukebox");
}
//<-dceag-c913-b->

	/** @brief COMMAND: #913 - Load Disk */
	/** Load a disk into the jukebox */
		/** @param #259 Multiple */
			/** If true, load multiple disks */

void Powerfile_C200::CMD_Load_Disk(bool bMultiple,string &sCMD_Result,Message *pMessage)
//<-dceag-c913-e->
{
	m_pPowerfileJukebox->LoadDiscs(bMultiple,pMessage->m_dwPK_Device_From);
}
//<-dceag-c914-b->

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

void Powerfile_C200::CMD_Get_Disk_Info(string *sAaronSpecial, int *iPK_MediaType, int *iAaronSpecial_EKID, string *sDisks,string *sURL,string *sBlock_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c914-e->
{
	// Should be sent to the children instead
}
//<-dceag-c14-b->

	/** @brief COMMAND: #14 - Refresh */
	/** Refresh the status of the drives */
		/** @param #15 DataGrid ID */
			/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

void Powerfile_C200::CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c14-e->
{
	m_pPowerfileJukebox->Get_Jukebox_Status(NULL,true);
	DCE::CMD_Refresh CMD_Refresh(m_dwPK_Device,pMessage->m_dwPK_Device_From,"*");
	SendCommand(CMD_Refresh);
}

//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void Powerfile_C200::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
	PLUTO_SAFETY_LOCK(jm,*m_pPowerfileJukebox->m_pJobHandler->m_ThreadMutex_get());
	Job *pJob = m_pPowerfileJukebox->m_pJobHandler->FindJob(iParameter_ID);
	if( !pJob )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Powerfile_C200::CMD_Abort_Task invalid job %d",iParameter_ID);
		return;
	}

	pJob->Abort();
}

void Powerfile_C200::PostConnect()
{
	DCE::CMD_Refresh_List_of_Online_Devices_Cat CMD_Refresh_List_of_Online_Devices_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,
		true,BL_SameHouse);
	SendCommand(CMD_Refresh_List_of_Online_Devices_Cat);
}
