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
#ifndef DISK_DRIVE_FUNCTIONS
#define DISK_DRIVE_FUNCTIONS

#include "Command_Impl.h"
#include "pluto_main/Define_MediaType.h"
#include "JobHandler/JobHandler.h"

#ifndef WIN32
extern "C"
{
    #include <linux/cdrom.h>
}
#endif

#include <string>

using namespace std;
using namespace DCE;

namespace nsJobHandler
{
	class RipTask;
}
using namespace nsJobHandler;

typedef enum {
    MEDIA_NOT_LOADED        = 0x00,
    MEDIA_CD_ROM            = 0x08, /* also CD-ROM and Finalized CD-R */
    MEDIA_CD_R              = 0x09,
    MEDIA_CD_RW             = 0x0A,
    MEDIA_DVD_ROM           = 0x10, /* also Finalized DVD-R and DVD+R */
    MEDIA_DVD_MINUS_R       = 0x11,
    MEDIA_DVD_MINUS_RW      = 0x13,
    MEDIA_DVD_MINUS_RW_RO   = 0x14, /* _RO = restricted overwrite */
    MEDIA_DVD_PLUS_RW       = 0x1A,
    MEDIA_DVD_PLUS_R        = 0x1B,
    MEDIA_TYPE_UNKNOWN      = 0x98,
    MEDIA_TYPE_ERROR        = 0x99
} discMediaType;

// This should be in sync with the typedef in the Media_Plugin.cpp
typedef enum {
	RIP_RESULT_BEGIN_ENUM		 = 0x01,
	RIP_RESULT_ALREADY_RIPPING	 = 0x02,
	RIP_RESULT_NO_DISC			 = 0x03,
	RIP_RESULT_INVALID_DISC_TYPE = 0x04,
	RIP_RESULT_SUCCESS			 = 0x05,
	RIP_RESULT_FAILURE			 = 0x06,
	RIP_RESULT_STILLGOING		 = 0x07,
	RIP_RESULT_END_ENUM			 = 0x08
} rippingResult;

enum DiscTypes {
    DISCTYPE_NONE       = -1,
    DISCTYPE_CD_AUDIO   =  0,
    DISCTYPE_CD         =  1,
    DISCTYPE_DVD_VIDEO  =  2,
    DISCTYPE_DVD        =  3,
    DISCTYPE_BLANK      =  4,
    DISCTYPE_DATA       =  5,
    DISCTYPE_CD_MIXED   =  6,
    DISCTYPE_CD_VCD     =  7,
    DISCTYPE_CD_SVCD    =  8,
    DISCTYPE_DVD_AUDIO  =  9,
    DISCTYPE_HDDVD	= 10,
    DISCTYPE_BD		= 11	// Blu-ray Disc
};

class Disk_Drive_Functions
{
public:
	friend class nsJobHandler::RipTask;

	enum Locked
	{
		locked_available,
		locked_move,
		locked_identify_job,
		locked_rip_job,
		locked_playback
	};
	protected:
		Command_Impl * m_pCommand_Impl;
	    pthread_mutexattr_t m_ThreadAttribute;
		bool m_bAutoIdentifyMedia;
		DeviceData_Base *m_pDevice_MediaIdentifier;
		JobHandler *m_pJobHandler;
		class Database_pluto_media *m_pDatabase_pluto_media;
		class MediaAttributes_LowLevel *m_pMediaAttributes_LowLevel;
		Locked m_eLocked; // Indicates if the drive is in use, if so for what, or available
		void *m_pLockedPtr; // Whatever did the lock
		int m_dwPK_Device;

		void EVENT_Media_Inserted(int iFK_MediaType,string sMRL,string sID,string sName);
		void EVENT_Ripping_Progress(string sText,int iResult,string sValue,string sName,int iEK_Disc);
		
	public:
	    pluto_pthread_mutex_t m_DiskMutex;

		Disk_Drive_Functions(int dwPK_Device,Command_Impl * pCommand_Impl, const string & sDrive,JobHandler *pJobHandler,Database_pluto_media *pDatabase_pluto_media,MediaAttributes_LowLevel *pMediaAttributes_LowLevel,bool bAutoIdentifyMedia=true);
		~Disk_Drive_Functions();
		bool internal_monitor_step(bool bFireEvent);
		bool internal_reset_drive(bool bFireEvent,int *iPK_MediaType,string *sDisks,string *sURL,string *sBlock_Device,bool bRecheck=false);
		int cdrom_has_dir(int fd, const char *directory);
		int cdrom_lock(int lock);
		int cdrom_checkdrive(const char * filename, int * flag, bool bFireEvent,bool Recheck=false);
		bool mountDVD(string fileName, string & strMediaUrl);
		bool FixupRippingInfo(Disk_Drive_Functions *pDisk_Drive_Functions,int &PK_MediaType,string &sFilename,string &sTracks,int iEK_Disc,string &sDirectory);
		string getTracks(string mrl); // We use cddb for nothing other than determining how many tracks there are
		void DisplayMessageOnOrbVFD(string sMessage);
		bool isRipping();
		void UpdateDiscLocation(char cType,int PK_Disc=-1);  // An unknown type
		void GetTracksForDisc(class Row_Disc *pRow_Disc,map<int,string> &mapTracks);

		// TODO: write accessors for these
		DeviceData_Base *m_pDevice_AppServer;
	    int m_mediaDiskStatus;
	    int m_discid;		// A unique ID to indicate the insertion of this disc, will be set to the time(NULL) at insertion
	    bool m_mediaInserted;
		int m_what_is_ripping;
		bool m_bTrayOpen;
		string m_sDrive;
		Command_Impl *m_pCommand_Impl_get() { return m_pCommand_Impl; }
		DeviceData_Base *m_pDevice_MediaIdentifier_get() { return m_pDevice_MediaIdentifier; }

		bool LockDrive(Locked locked,void *p_void); // returns false if the drive is already locked, or true if it set it to locked.  p_void is the pointer to whatever locked it so we can check if it was us
		void UnlockDrive();  // release the drive
		Locked m_eLocked_get(void **p_void);
		int m_dwPK_Device_get() { return m_dwPK_Device; }

		class Database_pluto_media *m_pDatabase_pluto_media_get() { return m_pDatabase_pluto_media; }

		void IdDisk(bool bForce=false); // Send the command to id this disc
};

#endif
