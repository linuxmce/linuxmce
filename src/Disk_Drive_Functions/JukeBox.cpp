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
#include "JukeBox.h"
#include "IdentifyJob.h"
#include "LoadUnloadJob.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_media/Table_DiscLocation.h"
#include "pluto_media/Table_Disc.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "Media_Plugin/Media_Plugin.h"

using namespace nsJobHandler;
using namespace nsJukeBox;

#include "DCE/DCEConfig.h"
DCEConfig g_DCEConfig_JB;

JukeBox::JukeBox(Command_Impl *pCommand_Impl)
	: m_DriveMutex("Jukebox drive mutex", true)
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_DriveMutex.Init( &m_MutexAttr );

	m_pCommand_Impl = pCommand_Impl;
	m_pJobHandler = NULL;
	m_pDatabase_pluto_media = NULL;
	m_pMediaAttributes_LowLevel = NULL;
	m_eLocked=Disk_Drive_Functions::locked_available;
	m_pLockedPtr=NULL;
}

JukeBox::~JukeBox()
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);
	map_int_Slotp::iterator itSlot;
	map_int_Drivep::iterator itDrive;

	for (itSlot = m_mapSlot.begin(); itSlot != m_mapSlot.end(); itSlot++)
	{
		delete itSlot->second;
	}

	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		delete itDrive->second;
	}

	delete m_pJobHandler;
	delete m_pDatabase_pluto_media;
	delete m_pMediaAttributes_LowLevel;
}

bool JukeBox::Init()
{
	// The derived class should first populate the mapSlot and mapDrive

	m_pJobHandler = new JobHandler();
	m_pJobHandler->StartThread();
	m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());

	string sIP = g_DCEConfig_JB.m_mapParameters_Find("MySqlHost");
	if( sIP.empty() )
		sIP = m_pCommand_Impl->m_sIPAddress;

	if (!m_pDatabase_pluto_media->Connect(sIP, "root", "", "pluto_media", 3306))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_pCommand_Impl->m_bQuit_set(true);
		return false;
	}
	m_pMediaAttributes_LowLevel = new MediaAttributes_LowLevel(m_pDatabase_pluto_media);
	return true;
}

void JukeBox::UpdateDrivesSlotsFromDatabase()
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);

	// This is called on startup, so we're not ripping anything.  Clear stale values
	string sDeviceList = StringUtils::itos(m_pCommand_Impl->m_dwPK_Device);
	for (map_int_Drivep::iterator itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
		sDeviceList += "," + StringUtils::itos(itDrive->second->m_dwPK_Device_get());
	string sSQL = "UPDATE DiscLocation SET EK_Device_Ripping=NULL,RipJob=NULL WHERE EK_Device_Ripping IN (" + sDeviceList + ")";
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeBox::UpdateDrivesSlotsFromDatabase SQL: %s", sSQL.c_str());
	m_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL);

	vector<Row_DiscLocation *> vectRow_DiscLocation;
	m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(m_pCommand_Impl->m_dwPK_Device),&vectRow_DiscLocation);
	for(vector<Row_DiscLocation *>::iterator it=vectRow_DiscLocation.begin();it!=vectRow_DiscLocation.end();++it)
	{
		Row_DiscLocation *pRow_DiscLocation = *it;
		if( pRow_DiscLocation->Type_get()=="U" )
			continue;  // It's got something in it, but we don't know what it is

		Row_Disc *pRow_Disc = pRow_DiscLocation->FK_Disc_getrow();
		Slot *pSlot = m_mapSlot_Find( pRow_DiscLocation->Slot_get() );
		if( !pSlot )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::UpdateDrivesSlotsFromDatabase no disc %d/%p slot %d/%p", 
				pRow_DiscLocation->FK_Disc_get(), pRow_Disc, pRow_DiscLocation->Slot_get(), pSlot);
			pRow_DiscLocation->Delete(); // Something is wrong with this
			m_pDatabase_pluto_media->DiscLocation_get()->Commit();
		}
		pSlot->m_pRow_Disc = pRow_Disc;
	}
}

Drive *JukeBox::LockAvailableDrive(Disk_Drive_Functions::Locked eLocked,Job *pJob,void *p_void,bool bEmptyOnly)
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);

	map_int_Drivep::iterator itDrive;
	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		Drive *pDrive = itDrive->second;
		if( pDrive->m_eLocked_get(NULL)==Disk_Drive_Functions::locked_available )
		{
			if( pDrive->m_mediaInserted && bEmptyOnly )
				continue;

			if( pDrive->LockDrive(eLocked,p_void) )
			{
				pDrive->m_pJob = pJob;
				return pDrive;
			}
		}
	}

	return NULL;
}

void JukeBox::MassIdentify(string sSlots,int PK_Orbiter)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeBox::MassIdentify: sSlots: %s", sSlots.c_str());

	if( sSlots=="*" || sSlots.empty() )
	{
		for(map_int_Drivep::iterator it=m_mapDrive.begin();it!=m_mapDrive.end();++it)
		{
			Drive *pDrive = it->second;
			if( pDrive->m_mediaInserted )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,pDrive,NULL,PK_Orbiter,m_pCommand_Impl);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
		for(map_int_Slotp::iterator it=m_mapSlot.begin();it!=m_mapSlot.end();++it)
		{
			Slot *pSlot = it->second;
			if( pSlot->m_eStatus!=Slot::slot_defective && pSlot->m_eStatus!=Slot::slot_empty )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,NULL,pSlot,PK_Orbiter,m_pCommand_Impl);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
	}
	else
	{
		string::size_type pos=0;
		while(pos<sSlots.size())
		{
			int iSlot = atoi( StringUtils::Tokenize(sSlots,",",pos).c_str() );
			Slot *pSlot = m_mapSlot_Find(iSlot);

			if(NULL != pSlot && pSlot->m_eStatus!=Slot::slot_defective && pSlot->m_eStatus!=Slot::slot_empty )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,NULL,pSlot,PK_Orbiter,m_pCommand_Impl);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
	}
}

bool JukeBox::LockJukebox(Disk_Drive_Functions::Locked locked,void *p_void)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::LockDrive m_eLocked %d locked %d %p", (int) m_eLocked, (int) locked,m_pLockedPtr);

	if( m_eLocked!=Disk_Drive_Functions::locked_available )
	{
		if( m_pLockedPtr==p_void )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"LockJukebox::LockJukebox already locked");
			return true;
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::LockJukebox cannot lock %p/%p", m_pLockedPtr, p_void);
		return false;
	}

	m_pLockedPtr=p_void;
	m_eLocked=locked;
	return true;
}

void JukeBox::UnlockJukebox()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::UnlockJukebox %p",m_pLockedPtr);
	m_eLocked=Disk_Drive_Functions::locked_available;
}

void JukeBox::AssertJukeboxIsLocked()
{
	if( m_eLocked==Disk_Drive_Functions::locked_available )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"JukeBox::AssertJukeboxIsLocked");  // Report that something is wrong since we're using this without locking it

}

Disk_Drive_Functions::Locked JukeBox::m_eLocked_get(void **p_void)
{
	if( p_void )
		*p_void = m_pLockedPtr;

	return m_eLocked;
}

void JukeBox::UpdateDiscLocation(int PK_Device_Original,int Slot_Original,int PK_Device_New,int Slot_New)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::UpdateDiscLocation PK_Device_Original: %d Slot_Original: %d PK_Device_New: %d Slot_New: %d", PK_Device_Original,Slot_Original,PK_Device_New,Slot_New);
	string sSQL = "UPDATE DiscLocation SET EK_Device=" + StringUtils::itos(PK_Device_New) + ",Slot=" + (Slot_New==-1 ? "NULL" : StringUtils::itos(Slot_New)) +
		" WHERE EK_Device=" + StringUtils::itos(PK_Device_Original) + (Slot_Original==-1 ? "" : " AND Slot=" + StringUtils::itos(Slot_Original));
	if( m_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL)==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::UpdateDiscLocation Nothing in the db for dev %d slot %d, adding",PK_Device_New,Slot_New);
		AddDiscToDb(PK_Device_New,Slot_New,'U');
	}
}

void JukeBox::RemoveDiscFromDb(int PK_Device,int Slot)
{
	string sSQL = "DELETE FROM DiscLocation WHERE EK_Device=" + StringUtils::itos(PK_Device);
	if( Slot!=-1 )
		sSQL += " AND Slot=" + StringUtils::itos(Slot);

	m_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL);
}

void JukeBox::AddDiscToDb(int PK_Device,int Slot,char Type)
{
	string sSQL = "INSERT INTO DiscLocation(EK_Device,Slot,Type) VALUES(" + StringUtils::itos(PK_Device) + "," + (Slot ? StringUtils::itos(Slot) : "NULL") + ",'" + Type + "')";
	m_pDatabase_pluto_media->threaded_db_wrapper_query(sSQL);
}

void JukeBox::AbortAll()
{
	PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
	const ListJob *plistJob = m_pJobHandler->m_listJob_get();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PowerfileJukebox::AbortAll %d jobs", plistJob->size());

	for(ListJob::const_iterator it=plistJob->begin();it!=plistJob->end();++it)
	{
		Job *pJob = *it;
		pJob->Abort();
	}
}

void JukeBox::Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc)
{
	DCE::CMD_Media_Identified_DT CMD_Media_Identified_DT(m_pCommand_Impl->m_dwPK_Device,DEVICETEMPLATE_Media_Plugin_CONST,
		BL_SameHouse,iPK_Device,sValue_To_Assign,sID,pData,iData_Size,sFormat,iPK_MediaType,sMediaURL,sURL,iEK_Disc);
	m_pCommand_Impl->SendCommand(CMD_Media_Identified_DT);

	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);
	Drive *pDrive = NULL;
	for(map_int_Drivep::iterator it=m_mapDrive.begin();it!=m_mapDrive.end();++it)
	{
		Drive *pD = it->second;
		if( pD->m_dwPK_Device_get()==iPK_Device )
		{
			pDrive = pD;
			break;
		}
	}

	if( !pDrive )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PowerfileJukebox::Media_Identified - no drive: %d",iPK_Device);
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PowerfileJukebox::Media_Identified disc is %d",*iEK_Disc);
	if( *iEK_Disc )
	{
		char cMediaType='M'; // The default
		if( iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
			cMediaType='c';
		else if( iPK_MediaType==MEDIATYPE_pluto_DVD_CONST )
			cMediaType='d';
		pDrive->UpdateDiscLocation(cMediaType,*iEK_Disc);
	}

	PLUTO_SAFETY_LOCK(jm,*m_pJobHandler->m_ThreadMutex_get());
	const ListJob *plistJob = m_pJobHandler->m_listJob_get();

	for(ListJob::const_iterator it=plistJob->begin();it!=plistJob->end();++it)
	{
		Job *pJob = *it;
		if( pJob->GetType()=="IdentifyJob" )
		{
			IdentifyJob *pIdentifyJob = (IdentifyJob *) pJob;
			if( pIdentifyJob->m_pDisk_Drive_Functions && pIdentifyJob->m_pDisk_Drive_Functions->m_dwPK_Device_get()==iPK_Device )
			{
				// Found the job.  Now get the pending task
				Task *pTask = pIdentifyJob->GetNextTask();
				if( pTask && pTask->GetType()=="IdentifyTask" )
				{
					pTask->m_eTaskStatus_set(TASK_COMPLETED);
					return;
				}
			}
		}
		else if( pJob->GetType()=="RipJob" )
		{
			RipJob *pRipJob = (RipJob *) pJob;
			if( pRipJob->m_pDisk_Drive_Functions && pRipJob->m_pDisk_Drive_Functions->m_dwPK_Device_get()==iPK_Device )
			{
				// Found the job.  Now get the pending task
				Task *pTask = pRipJob->GetNextTask();
				if( pTask && pTask->GetType()=="IdentifyTask" )
				{
					Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(*iEK_Disc);
					if( pRow_Disc )
						pRipJob->m_iEK_Disc = *iEK_Disc;

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"PowerfileJukebox::Media_Identified updated bulk rip to %d %p", *iEK_Disc, pRow_Disc);
					pTask->m_eTaskStatus_set(TASK_COMPLETED);
					return;
				}
			}
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::Media_Identified cannot find job/task for %d",iPK_Device);
}

/*virtual*/ JukeBox::JukeBoxReturnCode JukeBox::LoadDiscs(bool bMultiple,int PK_Orbiter)
{
	PLUTO_SAFETY_LOCK(pf,m_DriveMutex_get());
	Slot *pSlot=NULL;
	if( bMultiple==false )
	{
		pSlot = m_mapSlot_Empty();
		if( !pSlot )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeBox::CMD_Load_Disk -- no available slots");
			return JukeBox::jukebox_transport_failure;
		}
		pSlot->m_eStatus=Slot::slot_intransit;
	}

	LoadUnloadJob *pLoadUnloadJob = new LoadUnloadJob(m_pJobHandler,pSlot==NULL ? LoadUnloadJob::eLoadMultipleDiscs : LoadUnloadJob::eLoadOneDisc,this,NULL,pSlot,PK_Orbiter,m_pCommand_Impl);
	m_pJobHandler->AddJob(pLoadUnloadJob);
	return JukeBox::jukebox_ok;
}

/*virtual*/ JukeBox::JukeBoxReturnCode JukeBox::Load_from_Slot_into_Drive(int iSlot_Number,int *iPK_Device,int iPK_Orbiter)
{
	PLUTO_SAFETY_LOCK(pf,m_DriveMutex_get());
	Slot *pSlot=m_mapSlot_Find(iSlot_Number);
	if( !pSlot || pSlot->m_eStatus==Slot::slot_empty || pSlot->m_eStatus==Slot::slot_intransit )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Load_from_Slot_into_Drive -- bad slot %d status %d", iSlot_Number, pSlot ? (int) pSlot->m_eStatus : -1);
		return JukeBox::jukebox_transport_failure;
	}

	Drive *pDrive = NULL;
	LoadUnloadJob *pLoadUnloadJob = new LoadUnloadJob(m_pJobHandler,LoadUnloadJob::eMoveFromSlotToDrive,this,NULL,pSlot,iPK_Orbiter,m_pCommand_Impl);

	if( *iPK_Device!=-1 )
	{
		pDrive = m_mapDrive_FindByPK_Device(*iPK_Device);
		if( !pDrive || !pDrive->LockDrive(Disk_Drive_Functions::locked_move,pLoadUnloadJob) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Load_from_Slot_into_Drive -- bad drive %d or cannot lock drive %p", *iPK_Device, pDrive);
			delete pLoadUnloadJob;
			return JukeBox::jukebox_transport_failure;
		}
	}
	else
	{
		pDrive = LockAvailableDrive(Disk_Drive_Functions::locked_move,pLoadUnloadJob,pLoadUnloadJob,true);
		if( !pDrive )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Load_from_Slot_into_Drive -- no available drives");
			delete pLoadUnloadJob;
			return JukeBox::jukebox_transport_failure;
		}
		*iPK_Device = pDrive->m_dwPK_Device_get();
	}

	pSlot->m_eStatus=Slot::slot_intransit;
	pLoadUnloadJob->m_pDrive=pDrive;
	m_pJobHandler->AddJob(pLoadUnloadJob);
	return JukeBox::jukebox_ok;
}

/*virtual*/ JukeBox::JukeBoxReturnCode JukeBox::Unload_from_Drive_into_Slot(int *iSlot_Number,int iPK_Device,int iPK_Orbiter)
{
	PLUTO_SAFETY_LOCK(pf,m_DriveMutex_get());
	Slot *pSlot=NULL;
	if( *iSlot_Number!=-1 )
	{
		pSlot = m_mapSlot_Find(*iSlot_Number);
		if( !pSlot || pSlot->m_eStatus!=Slot::slot_empty )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Unload_from_Drive_into_Slot -- bad slot %d status %d", *iSlot_Number, pSlot ? (int) pSlot->m_eStatus : -1);
			return JukeBox::jukebox_transport_failure;
		}
	}
	else
	{
		pSlot = m_mapSlot_Empty();
		if( !pSlot )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Unload_from_Drive_into_Slot -- no empty slots");
			return JukeBox::jukebox_transport_failure;
		}
		*iSlot_Number = pSlot->m_SlotNumber;
	}

	Drive *pDrive = m_mapDrive_FindByPK_Device(iPK_Device);
	if( !pDrive )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Unload_from_Drive_into_Slot -- bad drive %d", iPK_Device);
		return JukeBox::jukebox_transport_failure;
	}

	LoadUnloadJob *pLoadUnloadJob = new LoadUnloadJob(m_pJobHandler,LoadUnloadJob::eMoveFromDriveToSlot,this,pDrive,pSlot,iPK_Orbiter,m_pCommand_Impl);
	if( !pDrive->LockDrive(Disk_Drive_Functions::locked_move,pLoadUnloadJob) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "JukeBox::Unload_from_Drive_into_Slot -- cannot lock drive %d", iPK_Device);
		delete pLoadUnloadJob;
		return JukeBox::jukebox_transport_failure;
	}

	pSlot->m_eStatus=Slot::slot_intransit;
	m_pJobHandler->AddJob(pLoadUnloadJob);
	return JukeBox::jukebox_ok;
}
