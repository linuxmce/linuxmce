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

#ifndef JukeBox_h
#define JukeBox_h

#include "Disk_Drive_Functions.h"

namespace nsJukeBox
{
	// A slot is one of the slots in the Jukebox to hold discs.  A Drive is a physical drive
	// for playing media.  Init() should populate m_mapSlot with all the slots where the key
	// is the slot number.  In other words for a 200 disc jukebox, there will be 200 entries.
	// Init() should also populate m_mapDrive for all the drives, where the key is the drive number.

	class Slot
	{
	public:
		int m_SlotNumber;
		enum Status
		{
			slot_empty=0,
			slot_identified_disc,
			slot_unknown_medium,
			slot_defective,
			slot_intransit
		} m_eStatus;
		Row_Disc *m_pRow_Disc;
		class JukeBox *m_pJukeBox;

		Slot(int SlotNumber, Status status,JukeBox *pJukeBox)
		{
			m_SlotNumber=SlotNumber;
			m_eStatus=status;
			m_pRow_Disc=NULL;
			m_pJukeBox=pJukeBox;
		}

		virtual ~Slot() {};
	};

	class Drive : public Disk_Drive_Functions
	{
	public:
		Job *m_pJob;  // If it's locked by a job, this is the job

		int m_iSourceSlot;  // Slot where the disk came from originally, or -1 if there is no disc or it's unknown
		int m_DriveNumber;
		bool m_bFunctional;
		string m_sSRdev; // The /dev/sgX entry

		Row_Disc *m_pRow_Disc;
		JukeBox *m_pJukeBox;

		Drive(int dwPK_Device,Command_Impl * pCommand_Impl, const string & sDrive,JobHandler *pJobHandler,Database_pluto_media *pDatabase_pluto_media,MediaAttributes_LowLevel *pMediaAttributes_LowLevel,
            int DriveNumber, string sSRdev, JukeBox *pJukeBox, bool bFunctional)
			: Disk_Drive_Functions(dwPK_Device,pCommand_Impl, sDrive, pJobHandler, pDatabase_pluto_media,pMediaAttributes_LowLevel,false)
		{
			m_DriveNumber=DriveNumber;
			m_sSRdev=sSRdev;
			m_bFunctional=bFunctional;
			m_iSourceSlot=0;
			m_pRow_Disc=NULL;
			m_pJob=NULL;
			m_pJukeBox=pJukeBox;
		}

		virtual ~Drive() {};
	};

	typedef map<int,Slot *> map_int_Slotp;
	typedef map<int,Drive *> map_int_Drivep;

	class JukeBox
	{
	protected:
		pluto_pthread_mutex_t m_DriveMutex;
		pthread_mutexattr_t m_MutexAttr;
		Disk_Drive_Functions::Locked m_eLocked; // Indicates if the drive is in use, if so for what, or available
		void *m_pLockedPtr; // What locked it

	public:
		map_int_Slotp m_mapSlot;
		map_int_Drivep m_mapDrive;
		Drive *m_mapDrive_Find( int Drive ) { map_int_Drivep::iterator it = m_mapDrive.find( Drive ); return it == m_mapDrive.end() ? NULL : (*it).second; }
		Drive *m_mapDrive_FindByPK_Device( int PK_Device )
		{ 
			for(map_int_Drivep::iterator it = m_mapDrive.begin(); it != m_mapDrive.end(); ++it)
			{
				if( it->second->m_dwPK_Device_get()==PK_Device )
					return it->second;
			}
			return NULL;
		}
		Slot *m_mapSlot_Find( int Slot ) { map_int_Slotp::iterator it = m_mapSlot.find( Slot ); return it == m_mapSlot.end() ? NULL : (*it).second; }
		Slot *m_mapSlot_Empty()
		{ 
			for(map_int_Slotp::iterator it = m_mapSlot.begin(); it != m_mapSlot.end(); ++it)
			{
				if( it->second->m_eStatus == Slot::slot_empty )
					return it->second;
			}
			return NULL;
		}
		Slot *m_mapSlot_NotEmpty()
		{ 
			for(map_int_Slotp::iterator it = m_mapSlot.begin(); it != m_mapSlot.end(); ++it)
			{
				if( it->second->m_eStatus != Slot::slot_empty )
					return it->second;
			}
			return NULL;
		}

		string m_sChangerDev;
		JobHandler * m_pJobHandler;
		Database_pluto_media * m_pDatabase_pluto_media;
		MediaAttributes_LowLevel * m_pMediaAttributes_LowLevel;
		Command_Impl *m_pCommand_Impl;
		pluto_pthread_mutex_t &m_DriveMutex_get() { return m_DriveMutex; }

	public:
		enum JukeBoxReturnCode
		{
			jukebox_ok,
			jukebox_transport_failure,
			jukebox_communication_failure
		};

		JukeBox(Command_Impl *pCommand_Impl);
		virtual ~JukeBox();

		virtual bool Init();
		void UpdateDrivesSlotsFromDatabase(); // Udpate the m_pRow_Disc's in slot's and drives

		// If a drive is available, it locks it and returns the drive.  Otherwise returns NULL
		Drive *LockAvailableDrive(Disk_Drive_Functions::Locked eLocked,Job *pJob,void *p_void,bool bEmptyOnly);

		/*
			Don't call these directly.  Call from a job as they may block.  Be sure the Jukebox and any Drive involved are locked first (Drive, then Jukebox)
		*/
		virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)=0;
		virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)=0;
		virtual JukeBoxReturnCode Eject(Slot *pSlot)=0;  // Elect the disc in pSlot.
		virtual JukeBoxReturnCode Load(Slot *pSlot=NULL)=0;  // If NULL, just allow bulk inserting into slots.

		// High level functions
		void MassIdentify(string sSlots,int PK_Orbiter);

		virtual void AbortAll();
		virtual void Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc);
		virtual JukeBoxReturnCode LoadDiscs(bool bMultiple,int PK_Orbiter);
		virtual JukeBoxReturnCode Load_from_Slot_into_Drive(int iSlot_Number,int *iPK_Device,int iPK_Orbiter);
		virtual JukeBoxReturnCode Unload_from_Drive_into_Slot(int *iSlot_Number,int iPK_Device,int iPK_Orbiter);

		bool LockJukebox(Disk_Drive_Functions::Locked locked,void *p_void); // returns false if the jukebox is already locked, or true if it set it to locked
		void UnlockJukebox();  // release the jukebox
		Disk_Drive_Functions::Locked m_eLocked_get(void **p_void);
	
		void AssertJukeboxIsLocked();

		// Update the database
		void UpdateDiscLocation(int PK_Device_Original,int Slot_Original,int PK_Device_New,int Slot_New);
		void RemoveDiscFromDb(int PK_Device,int Slot);
		void AddDiscToDb(int PK_Device,int Slot,char Type);

	};
}

#endif
