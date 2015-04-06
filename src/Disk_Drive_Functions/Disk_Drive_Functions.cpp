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
#include "Disk_Drive_Functions.h"
#include "Logger.h"

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"

#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_DiscLocation.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Define_AttributeType.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "VFD_LCD/VFD_LCD_Base.h"
#include "../Media_Plugin/MediaAttributes_LowLevel.h"
#include "RipJob.h"

#include <fcntl.h>
#ifndef WIN32
#include <sys/ioctl.h>
#include <sys/mount.h>
#endif

#include <limits.h>

#include <sstream>
using namespace std;

Disk_Drive_Functions::Disk_Drive_Functions(int dwPK_Device,Command_Impl * pCommand_Impl, const string & sDrive,JobHandler *pJobHandler,
	Database_pluto_media *pDatabase_pluto_media,MediaAttributes_LowLevel *pMediaAttributes_LowLevel,bool bAutoIdentifyMedia)
: m_pCommand_Impl(pCommand_Impl), m_DiskMutex("disk drive"), m_mediaDiskStatus(DISCTYPE_NONE), m_discid(0),
	m_mediaInserted(false), m_bTrayOpen(false), m_sDrive(sDrive), m_pJobHandler(pJobHandler)
{
	m_pLockedPtr=NULL;
	m_dwPK_Device=dwPK_Device;
	m_pDatabase_pluto_media=pDatabase_pluto_media;
	m_pMediaAttributes_LowLevel=pMediaAttributes_LowLevel;
	m_eLocked=locked_available;
	m_bAutoIdentifyMedia=bAutoIdentifyMedia;

    pthread_mutexattr_init( &m_ThreadAttribute );
    pthread_mutexattr_settype( &m_ThreadAttribute, PTHREAD_MUTEX_RECURSIVE_NP );
	m_DiskMutex.Init(&m_ThreadAttribute);
	m_pDevice_MediaIdentifier = m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Media_Identifiers_CONST,m_pCommand_Impl,3);
	if( !m_pDevice_MediaIdentifier )
	{
		m_pDevice_MediaIdentifier = m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Media_Identifiers_CONST);  // Try again ignoring registration.  Maybe it will register later
		if( m_pDevice_MediaIdentifier )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Disk_Drive_Functions::Disk_Drive_Functions warning m_pDevice_MediaIdentifier %d isn't registered",m_pDevice_MediaIdentifier->m_dwPK_Device);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::Disk_Drive_Functions m_pDevice_MediaIdentifier %d",m_pDevice_MediaIdentifier ? m_pDevice_MediaIdentifier->m_dwPK_Device : 0);

	m_pDevice_AppServer = m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_App_Server_CONST);
}

Disk_Drive_Functions::~Disk_Drive_Functions()
{
}

void Disk_Drive_Functions::EVENT_Media_Inserted(int iFK_MediaType, string sMRL, string sID, string sName)
{
	m_pCommand_Impl->m_pEvent->SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 3, 4, 3, StringUtils::itos(iFK_MediaType).c_str(), 4, sMRL.c_str(), 7, sID.c_str(), 35, sName.c_str()));
}

void Disk_Drive_Functions::EVENT_Ripping_Progress(string sText, int iResult, string sValue, string sName, int iEK_Disc)
{
	m_pCommand_Impl->m_pEvent->SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 35, 5, 13, sText.c_str(), 20, StringUtils::itos(iResult).c_str(), 30, sValue.c_str(), 35, sName.c_str(), 43, StringUtils::itos(iEK_Disc).c_str()));
}

bool Disk_Drive_Functions::internal_monitor_step(bool bFireEvent)
{
	PLUTO_SAFETY_LOCK(dm,m_DiskMutex);
	int iPK_MediaType;
	string sDisks,sURL,sBlock_Device;
	if ( ! internal_reset_drive(bFireEvent,&iPK_MediaType,&sDisks,&sURL,&sBlock_Device) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Monitor drive returned false.");
		return false;
	}
	return true;
}

bool Disk_Drive_Functions::internal_reset_drive(bool bFireEvent,int *iPK_MediaType,string *sDisks,string *sURL,string *sBlock_Device,bool bRecheck)
{
	*sBlock_Device=m_sDrive;
#ifndef WIN32
	PLUTO_SAFETY_LOCK(dm,m_DiskMutex);

	int result = cdrom_checkdrive(m_sDrive.c_str(), &m_mediaDiskStatus, bFireEvent,bRecheck);

	//     LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disc Reset: checkdrive *iPK_MediaType: %d  result: %d", m_mediaDiskStatus, result);

	// we only care if a new CD was inserted in the meantime.
	if (bRecheck || (result >= 0 && m_mediaDiskStatus != DISCTYPE_NONE && m_mediaInserted == false) )
	{
		int fd = open( m_sDrive.c_str(), O_RDONLY | O_NONBLOCK );
		if (fd < 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to open device: %s", m_sDrive.c_str());
			// throw ("failed to open cdrom device" );
			return false;
		}

		*sURL = m_sDrive;
		switch (m_mediaDiskStatus)
		{
		case DISCTYPE_CD_MIXED: // treat a mixed CD as an audio CD for now.
		case DISCTYPE_CD_AUDIO:
			*sURL = getTracks("cdda://" + m_sDrive + "/").c_str();
			*iPK_MediaType = MEDIATYPE_pluto_CD_CONST;
			UpdateDiscLocation('c');  // We know it's media
			break;

		case DISCTYPE_DVD_VIDEO:
			*sURL = m_sDrive;
			*iPK_MediaType = MEDIATYPE_pluto_DVD_CONST;
			UpdateDiscLocation('d');  // We know it's media
			break;

		case DISCTYPE_HDDVD:
			*sURL = m_sDrive;
			*iPK_MediaType = MEDIATYPE_pluto_HDDVD_CONST;
			UpdateDiscLocation('H');  // We know it's media
			break;

		case DISCTYPE_BD:
			*sURL = m_sDrive;
			*iPK_MediaType = MEDIATYPE_pluto_BD_CONST;
			UpdateDiscLocation('R');  // We know it's media
			break;

		case DISCTYPE_BLANK:
			*iPK_MediaType = MEDIATYPE_misc_BlankMedia_CONST;
			UpdateDiscLocation('b');  // We know it's media
			break;

		case DISCTYPE_CD_VCD:
			*iPK_MediaType = MEDIATYPE_pluto_StoredVideo_CONST;
			UpdateDiscLocation('M');  // We know it's media
			break;

		default:
			UpdateDiscLocation('U');  // An unknown type
			*iPK_MediaType = 0;
			break;
		}
		close (fd);

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Disc of type %d was detected", *iPK_MediaType, sURL->c_str());

		m_discid=time(NULL);
		*sDisks = StringUtils::itos(m_discid);
		if ( bFireEvent && *iPK_MediaType )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "One Media Inserted event fired (%s) m_discid: %d", sURL->c_str(),m_discid);
			EVENT_Media_Inserted(*iPK_MediaType, *sURL,StringUtils::itos(m_discid),m_sDrive);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Not firing the event");
		}

		if( m_pDevice_MediaIdentifier && m_bAutoIdentifyMedia )
			IdDisk();

		m_mediaInserted = true;
	}

	// we mark media as not inserted on error or when the code tell us that no CD is in the unit.
	if (result < 0 || m_mediaDiskStatus == DISCTYPE_NONE )
	{
		if ( m_mediaInserted == true )
		{
			m_discid=0;
			m_mediaInserted = false;
			UpdateDiscLocation('E',0); // Now the drive is empty
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disk is not in the drive at the moment");
		}
	}
#else
	*iPK_MediaType=MEDIATYPE_pluto_DVD_CONST;
	*sDisks="1";
	*sURL=m_sDrive;
	// Pretend there's a dvd/cd in here for windows
	if( m_mediaInserted==false )
	{
		// DVD Block
		m_mediaDiskStatus = DISCTYPE_DVD_VIDEO;
		*iPK_MediaType = MEDIATYPE_pluto_DVD_CONST;
		*sURL=m_sDrive;

		// CD Block
		/*
		m_mediaDiskStatus = DISCTYPE_CD_AUDIO;
		*iPK_MediaType = MEDIATYPE_pluto_CD_CONST;
		*sURL=
			"cdda:///dev/cdrom/1\n"
			"cdda:///dev/cdrom/2\n"
			"cdda:///dev/cdrom/3\n"
			"cdda:///dev/cdrom/4\n"
			"cdda:///dev/cdrom/5\n"
			"cdda:///dev/cdrom/6\n"
			"cdda:///dev/cdrom/7\n"
			"cdda:///dev/cdrom/8\n"
			"cdda:///dev/cdrom/9\n"
			"cdda:///dev/cdrom/10\n"
			"cdda:///dev/cdrom/11";
		*/

		if ( bFireEvent )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "One Media Inserted event fired (%s) m_discid: %d", sURL->c_str(),m_discid);
			EVENT_Media_Inserted(*iPK_MediaType, *sURL,StringUtils::itos(m_discid),m_sDrive);
		}
		UpdateDiscLocation('d');  // We know it's media
		if( m_pDevice_MediaIdentifier && m_bAutoIdentifyMedia )
			IdDisk();

		m_mediaInserted = true;
	}
#endif
	return true;
	//  cout << "Need to implement command #47 - Reset Disk Drive" << endl;
}

int Disk_Drive_Functions::cdrom_has_dir(int fd, const char *directory)
{
	int ret = 0;        // return value
#ifndef WIN32
	unsigned short bs;  // the discs block size
	unsigned short ts;  // the path table size
	unsigned int tl;    // the path table location (in blocks)
	unsigned int i;
	unsigned int len_di = 0;    // length of the directory name in current path table entry
	unsigned int parent = 0;    // the number of the parent directory's path table entry
	char *dirname = NULL;   // filename for the current path table entry
	int pos = 0;        // our position into the path table
	int curr_record = 1;    // the path table record we're on

	// read the block size
	lseek (fd, 0x8080, SEEK_SET);
	int iFirstRead = read (fd, &bs, 2);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for folder: %s.", directory);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "FirstRead: %d fd: %d  directory: %s", iFirstRead, fd, directory);
	if( iFirstRead < 0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Try again - Cannot read from drive.");
		return -1;
	}

	// read in size of path table
	lseek (fd, 2, SEEK_CUR);
	read (fd, &ts, 2);

	// read in which block path table is in
	lseek (fd, 6, SEEK_CUR);
	read (fd, &tl, 4);

	// seek to the path table
	lseek (fd, ((int) (bs) * tl), SEEK_SET);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to loop through ts: %d.",ts);

	// loop through the path table entries
	while (pos < ts)
	{
		// get the length of the filename of the current entry
		int iRead = read (fd, &len_di, 1);

		// get the record number of this entry's parent
		// i'm pretty sure that the 1st entry is always the top directory
		int iSeek = lseek (fd, 5, SEEK_CUR);
		int iRead2 = read (fd, &parent, 2);

		// allocate and zero a string for the filename
		dirname = (char *) malloc (len_di + 1);
		for (i = 0; i < len_di + 1; i++)
			dirname[i] = '\0';

		// read the name
		int iRead3 = read (fd, dirname, len_di);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Directory: %s parent: %d (pos: %d  ts: %d read: %d read2: %d  read3: %d  seek: %d)",
			dirname, parent,
			pos, ts, iRead, iRead2, iRead3, iSeek);

		// if we found a folder that has the root as a parent, and the directory name matches
		// then return success
		if ((parent == 1) && (strcasecmp (dirname, directory) == 0))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "It's a match.");
			ret = 1;
			free (dirname);
			break;
		}

		free (dirname);

		// all path table entries are padded to be even, so if this is an odd-length table, seek a byte to fix it
		if (len_di % 2 == 1)
		{
			lseek (fd, 1, SEEK_CUR);
			pos++;
		}

		// update our position
		pos += 8 + len_di;
		curr_record++;
	}
#endif
	return ret;
}

int Disk_Drive_Functions::cdrom_lock(int lock)
{
	FILE *fp;

	if( (fp = fopen("/proc/sys/dev/cdrom/lock", "w")) != NULL )
	{
		fprintf(fp, "%d", lock);
		fclose(fp);
		return 1;
	}
	else
	{
		return 0;
	}
}

int Disk_Drive_Functions::cdrom_checkdrive(const char * filename, int * flag, bool bFireEvent,bool Recheck)
{
#ifdef WIN32
	* flag =DISCTYPE_DVD_VIDEO;
#else
	int fd;
	int status;
	struct cdrom_tochdr th;

	/*
	 * Workaround for ide-scsi bug
	 * ide-scsi reports the tray as being open when it has no disc in the drive, which messes up the eject toggle buttons
	 * it works fine when you put a disc in the drive (reports disc ok when the tray is closed)
	 */
	bool isIdeScsi = false;
	if (strstr(filename, "/dev/scd") == filename)
		isIdeScsi = true;

	// open the device
	fd = open(filename, O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error: couldn't open %s.", filename);
		return -1;
	}

	/*
		#define CDS_NO_INFO             0 
		#define CDS_NO_DISC             1
		#define CDS_TRAY_OPEN           2
		#define CDS_DRIVE_NOT_READY     3
		#define CDS_DISC_OK             4
	*/


	//     LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disk Drive %s file handle was opened!", DATA_Get_Drive().c_str());
	// read the drive status info
	time_t tTemp = time(NULL);
	status = ioctl(fd, CDROM_DRIVE_STATUS, CDSL_CURRENT);
	//     LoggerWrapper::GetInstance()->Write(LV_STATUS, "Current disk status %d", status);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Disk_Drive_Functions::cdrom_checkdrive status %d inserted %d flag: %d duration %d/%d open %d",status,(int) m_mediaInserted,*flag,(int) tTemp,(int) time(NULL),(int) m_bTrayOpen);

	switch (status)
	{
		// if there's a ok disc in there
	case CDS_DISC_OK:
		//LoggerWrapper::GetInstance()->Write(LV_WARNING, "Media inserted value here: %d", m_mediaInserted);
		//LoggerWrapper::GetInstance()->Write(LV_WARNING, "Disk type value here: %d", *flag);

		m_bTrayOpen = false;
		if (Recheck==false && (*flag != DISCTYPE_NONE || m_mediaInserted))
			break;

		if (bFireEvent)
			DisplayMessageOnOrbVFD("Disc detected in drive.");
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got a disc. Sleep a sec, then reopen. One hack to allow the disk to spin I think.");
		close(fd);
		sleep(1);
		fd = open(filename, O_RDONLY | O_NONBLOCK);
		if (fd < 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error: couldn't open %s.", filename);
			return -1;
		}

		// read the drive status info
		status = ioctl (fd, CDROM_DRIVE_STATUS, CDSL_CURRENT);
		if (status != CDS_DISC_OK)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Disc was detected, but the status is no longer disc ok.");
			close(fd);
			return 0;  // Don't change anything, we'll try again later
		}

		// see if we can read the disc's table of contents (TOC).
		status = ioctl(fd, CDROMREADTOCHDR, &th);
		if (status != 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Can't read disc TOC. The disc is either a blank, or has a broken TOC.");
			* flag = DISCTYPE_BLANK;
			break;
		}

		// read disc status info
		status = ioctl(fd, CDROM_DISC_STATUS, CDSL_CURRENT);

		switch (status)
		{
		case CDS_AUDIO:
			// found a audio cd
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Detected audio cd!");
			* flag = DISCTYPE_CD_AUDIO;
			break;

		case CDS_DATA_1:
		case CDS_DATA_2:
			{
				// Try up to 10 seconds to see what the directories are
				int result = -1;
				time_t timeout = time(NULL) + 10;
				while( result == -1 && timeout > time(NULL) )
				{
					result = cdrom_has_dir(fd, "video_ts");
					if (result == -1)
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Not ready to read the directories yet.");
						sleep(1);
					}
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Result of video_ts search: %d.", result);
				}

				if (result > 0)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "I think it's a DVD...");
					* flag =DISCTYPE_DVD_VIDEO;
				}
				else if (cdrom_has_dir(fd, "vcd") > 0)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "I think it's a VCD...");
					* flag = DISCTYPE_CD_VCD;
				}
				else if (cdrom_has_dir(fd, "svcd") > 0)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "I think it's a SVCD...");
					* flag = DISCTYPE_CD_SVCD;
				}
				else
				{
					// trying to mount the disk as UDF and check presence of
					// HD-DVD or Blu-ray system folders
					bool bDetected = false;
					
					char *mntDir = tempnam("/mnt", "diskid");
					if (mntDir)
					{
						if (mkdir(mntDir, S_IRUSR|S_IWUSR|S_IXUSR)==0)
						{
							if (mount(filename, mntDir, "udf", MS_RDONLY, NULL)==0)
							{
								string sFolder = mntDir;
								sFolder += "/HVDVD_TS";
								if (FileUtils::DirExists(sFolder))
								{
									LoggerWrapper::GetInstance()->Write(LV_STATUS, "I think it's a HD-DVD...");
									* flag =DISCTYPE_HDDVD;
									bDetected = true;
								}
								else
								{
									sFolder = mntDir;
									sFolder += "/BDMV";
									if (FileUtils::DirExists(sFolder))
									{
										LoggerWrapper::GetInstance()->Write(LV_STATUS, "I think it's a Blu-ray Disc...");
										* flag =DISCTYPE_BD;
										bDetected = true;
									}
								}
								
								umount(mntDir);
							}
							rmdir(mntDir);
						}
						free(mntDir);
                                        }
					
					if (!bDetected)
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Doesn't have any directories -- must be data.");
						* flag = DISCTYPE_DATA;
					}
				}
			}
			break;

		case CDS_MIXED:
			// found a mixed cd
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Detected mixed audio/data cd!");
			* flag = DISCTYPE_CD_MIXED;
			break;

		default:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not determine disc type: Doing nothing!");
			break;
		}
		break;

	case CDS_NO_INFO:
		if( * flag != DISCTYPE_NONE )
			UpdateDiscLocation('E',0); // Now the drive is empty
		// drive doesnt support querying, so this program will never work on that drive.
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "%s does not support status queries.", filename);
		* flag = DISCTYPE_NONE;
		break;

	case CDS_NO_DISC:
		if( * flag != DISCTYPE_NONE )
			UpdateDiscLocation('E',0); // Now the drive is empty
		m_bTrayOpen = false;
		* flag = DISCTYPE_NONE;
		break;

	case CDS_TRAY_OPEN:
		if( * flag != DISCTYPE_NONE )
			UpdateDiscLocation('E',0); // Now the drive is empty
		* flag = DISCTYPE_NONE;
		if (!isIdeScsi) // ide-scsi workaround
			m_bTrayOpen = true;
		break;

	default:
		// LoggerWrapper::GetInstance()->Write(LV_STATUS, "Nothing interesting hapened");
		// release the device
		if( * flag != DISCTYPE_NONE )
			UpdateDiscLocation('E',0); // Now the drive is empty
		* flag = DISCTYPE_NONE;
	}
	close(fd);
#endif
	return 0;
}

bool Disk_Drive_Functions::mountDVD(string fileName, string & strMediaUrl)
{
	// True if we're playing a physical dir, false if it's a file
	bool bDriveMount = StringUtils::StartsWith(fileName,"/dev/",true);

	string sDrive = bDriveMount ? fileName : m_sDrive;

	string cmd = "ln -sf " + sDrive + " /dev/dvd";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd drivemount: %d - %s",(int) bDriveMount,cmd.c_str());
	system(cmd.c_str());  // Don't care about the return.  Just making sure it's not loop 5 so we can delete it
	if (bDriveMount)
	{
		LoggerWrapper::GetInstance()->Write(LV_ACTION,"returning mounted drive");
		strMediaUrl = "dvd:/";
		Sleep(500); // TODO: HACK  -- sometimes xine can't play dvd's.  Throw a small delay in to see if it has an effect
		return true;
	}

	string cmdUnmount = "losetup -d /dev/loop5";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd: %s",cmdUnmount.c_str());
	system(cmdUnmount.c_str());  // Don't care about the return. 

	cmd = "losetup /dev/loop5 \"" + fileName + "\"";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd: %s",cmd.c_str());
	int iResult2 = 0, iResult = system(cmd.c_str());

	if (iResult != 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"first attempt to mount failed %s",cmd.c_str());
		Sleep(1000);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd: %s",cmdUnmount.c_str());
		system(cmdUnmount.c_str());  // Don't care about the return. 
		Sleep(500);
		iResult = system(cmd.c_str());
	}

	if (iResult == 0)
	{

		Sleep(500); // TODO: HACK  -- sometimes xine can't play dvd's.  Throw a small delay in to see if it has an effect
		cmd = "ln -sf /dev/loop5 /dev/dvd";
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd: %s",cmd.c_str());
		iResult2 = system(cmd.c_str());
	}

	if (iResult == 0 && iResult2 == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_ACTION,"returning mounted dvd");
		strMediaUrl = "dvd:/";
		Sleep(500); // TODO: HACK  -- sometimes xine can't play dvd's.  Throw a small delay in to see if it has an effect
		return true;
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to mount %d %d",iResult,iResult2);
	cmd = "ln -sf " + sDrive + " /dev/dvd";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd: %s",cmd.c_str());
	system(cmd.c_str());  // Can't do anything if it fails

	return false;
}

bool Disk_Drive_Functions::FixupRippingInfo(Disk_Drive_Functions *pDisk_Drive_Functions,int &PK_MediaType,string &sFilename,string &sTracks,int iEK_Disc,string &sDirectory)
{
	Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(iEK_Disc);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo disc %d PK_MediaType %d sFilename %s sTracks %s iEK_Disc %d sDirectory %s",
		pRow_Disc ? pRow_Disc->PK_Disc_get() : -1, PK_MediaType,sFilename.c_str(),sTracks.c_str(),iEK_Disc,sDirectory.c_str());

	if( pRow_Disc )
		PK_MediaType=pRow_Disc->EK_MediaType_get();
	else
	{
		vector<Row_DiscLocation *> vectRow_DiscLocation;
		m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(pDisk_Drive_Functions->m_dwPK_Device_get()),&vectRow_DiscLocation);
		if( vectRow_DiscLocation.size()!=1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Disk_Drive_Functions::FixupRippingInfo canot find disclocation for drive %d", pDisk_Drive_Functions->m_dwPK_Device_get());
			return false;
		}
		Row_DiscLocation *pRow_DiscLocation = vectRow_DiscLocation[0];
		if( pRow_DiscLocation->Type_get()=="d" )
			PK_MediaType=MEDIATYPE_pluto_DVD_CONST;
		else if( pRow_DiscLocation->Type_get()=="c" )
			PK_MediaType=MEDIATYPE_pluto_CD_CONST;
		else if ( pRow_DiscLocation->Type_get()=="H" )
		    	PK_MediaType=MEDIATYPE_pluto_HDDVD_CONST;
		else if ( pRow_DiscLocation->Type_get()=="R" )
		    	PK_MediaType=MEDIATYPE_pluto_BD_CONST;

		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Disk_Drive_Functions::FixupRippingInfo disclocation for drive %d has invalid type %s", 
				pDisk_Drive_Functions->m_dwPK_Device_get(), pRow_DiscLocation->Type_get().c_str());
			return false;
		}
	}

	if( PK_MediaType==MEDIATYPE_pluto_CD_CONST )
	{
		StringUtils::Replace(&sDirectory,"___audio___or___video___","audio");
		map<int,string> mapTracks;
		if( pRow_Disc )
			GetTracksForDisc(pRow_Disc,mapTracks);
		else
		{
			string sTemp = getTracks("");
			string::size_type pos=0;
			while(pos<sTemp.size())
			{
				string s = StringUtils::Tokenize(sTemp,"\n",pos);
				mapTracks[ atoi(s.c_str()) ] = "Track " + s;
			}
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo got %d tracks from: %s", (int) mapTracks.size(), sTemp.c_str());
		}

		string sNewTracks="";
		if( sTracks=="A" )
		{
			for(map<int,string>::iterator it=mapTracks.begin();it!=mapTracks.end();++it)
			{
				if( it->first<1 )
					continue;
				sNewTracks += StringUtils::itos(it->first) + ",";
				if( it->second.empty() )
					sNewTracks += "Track " + StringUtils::itos(it->first);
				else
					sNewTracks += FileUtils::ValidFileName(it->second,true,true);
				sNewTracks += "|";
			}
		}
		else
		{
			string::size_type pos=0;
			while( pos<sTracks.size() && pos!=string::npos )
			{
				string sTrack = StringUtils::Tokenize(sTracks,"|",pos);
				int iTrack = atoi(sTrack.c_str());
				sNewTracks += StringUtils::itos(iTrack) + ",";

				string sName = mapTracks[iTrack];
				if( sName.empty() )
					sName = "Track " + StringUtils::itos(iTrack);
				sNewTracks += FileUtils::ValidFileName(sName,true,true) + "|";
			}
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo Transformed %s into %s",sTracks.c_str(),sNewTracks.c_str());
		sTracks=sNewTracks;
	}
	else if( PK_MediaType==MEDIATYPE_pluto_DVD_CONST )
		StringUtils::Replace(&sDirectory,"___audio___or___video___","videos");
	else if( PK_MediaType==MEDIATYPE_pluto_HDDVD_CONST || PK_MediaType==MEDIATYPE_pluto_BD_CONST )
	    StringUtils::Replace(&sDirectory,"___audio___or___video___","videos");

	bool bUsingUnknownDiscName=false;
	// Validate the name and be sure it's unique
	if( sFilename.size()==0 )
	{
		// We have a disc.  Try to get a real name
		string sDirectory2;
		if( pRow_Disc )
		{
			m_pMediaAttributes_LowLevel->GetDefaultRippingName(pRow_Disc,&sFilename,&sDirectory2);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo GetDefaultRippingName got %s", sFilename.c_str());
		}
		if( sFilename.size()==0 )
		{
		    // default
		    sFilename = "Unknown CD";
		    bUsingUnknownDiscName=true;

		    if (PK_MediaType==MEDIATYPE_pluto_DVD_CONST)
			sFilename = "Unknown DVD";
		    else if (PK_MediaType==MEDIATYPE_pluto_HDDVD_CONST)
			sFilename = "Unknown HD-DVD";
		    else if (PK_MediaType==MEDIATYPE_pluto_BD_CONST)
			sFilename = "Unknown Blu-ray Disc";
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo fixed directory %s / %s / %s", sDirectory.c_str(), sFilename.c_str(), sDirectory2.c_str());
	}

	sFilename=FileUtils::ValidFileName(sFilename,true,true);
	sDirectory=FileUtils::ValidFileName(sDirectory,true,true);

	sFilename = sDirectory + sFilename;

	// This previously only checked for duplicate names if we were using the 'unknown' name.  However, doing bulk ripping on the jukebox, sometimes multi-series 
	// disks are identified with the same title and are given the same name.
	if( ( ((PK_MediaType==MEDIATYPE_pluto_CD_CONST||PK_MediaType==MEDIATYPE_pluto_HDDVD_CONST||PK_MediaType==MEDIATYPE_pluto_BD_CONST) && FileUtils::DirExists(sFilename)) || 
		(PK_MediaType==MEDIATYPE_pluto_DVD_CONST && FileUtils::FileExists(sFilename + ".dvd")) ) )  // Be sure the directory name is unique if we're using the default
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo file/dir exists PK_MediaType %d sFilename %s sTracks %s iEK_Disc %d sDirectory %s",
			PK_MediaType,sFilename.c_str(),sTracks.c_str(),iEK_Disc,sDirectory.c_str());

		int Counter=1;
		string sNewName = sFilename + "_" + StringUtils::itos(Counter++);
		while( ((PK_MediaType==MEDIATYPE_pluto_CD_CONST||PK_MediaType==MEDIATYPE_pluto_HDDVD_CONST||PK_MediaType==MEDIATYPE_pluto_BD_CONST) && FileUtils::DirExists(sNewName)) || (PK_MediaType==MEDIATYPE_pluto_DVD_CONST && FileUtils::FileExists(sNewName + ".dvd")) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo file/dir exists PK_MediaType %d sFilename %s sTracks %s iEK_Disc %d sDirectory %s",
				PK_MediaType,sNewName.c_str(),sTracks.c_str(),iEK_Disc,sDirectory.c_str());

			sNewName = sFilename + "_" + StringUtils::itos(Counter++);
		}
		sFilename = sNewName;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::FixupRippingInfo done PK_MediaType %d sFilename %s (%d) sTracks %s iEK_Disc %d sDirectory %s",
		PK_MediaType,sFilename.c_str(),(int) bUsingUnknownDiscName,sTracks.c_str(),iEK_Disc,sDirectory.c_str());

	return true;
}

string Disk_Drive_Functions::getTracks(string mrl)
{
	int fd = -1, status;
	string tracks = "";

#ifndef WIN32
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finding CD tracks for prefix %s",mrl.c_str());

	//     time_t startTime=time(NULL);

	try
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Opening drive first time.");

		fd = open(m_sDrive.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd < 0)
			throw string("Failed to open CD device") + strerror(errno);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Checking media with ioctl");
		status = ioctl(fd, CDROM_DISC_STATUS, CDSL_CURRENT);
		if (status != CDS_AUDIO && status != CDS_MIXED)
			throw string("Invalid media detected");

		/* Code inspired from cd-discid - Start */
		struct cdrom_tochdr hdr;
		if (ioctl(fd, CDROMREADTOCHDR, &hdr) < 0)
		{
			throw string("Failed to read CDROM TOC.");
		}
		/* Code inspired from cd-discid - End */

		ostringstream sb;
		for (int i = 1; i <= hdr.cdth_trk1; i++)
		{
			/* Code inspired from cd-discid - Start */
			cdrom_tocentry te;
			te.cdte_track = i;
			te.cdte_format = CDROM_LBA;
			if (ioctl(fd, CDROMREADTOCENTRY, &te) < 0)
			{
				throw string("Failed to read TOC entry for track " + StringUtils::itos(i));
			}
			/* Code inspired from cd-discid - End */

			if (te.cdte_ctrl & CDROM_DATA_TRACK)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ending track scan at track %d (data track)", i);
				break;
			}

			sb << mrl << i << endl;
		}

		tracks = sb.str();
	}
	catch (string err)
	{
		LoggerWrapper::GetInstance()->Write (LV_WARNING, "w1: %s",err.c_str());
	}
	catch (...)
	{
		LoggerWrapper::GetInstance()->Write (LV_WARNING, "Unknown error in getTracks()");
	}

	close(fd);
#endif
	return tracks;
}

void Disk_Drive_Functions::DisplayMessageOnOrbVFD(string sMessage)
{
	DeviceData_Base *pDevice_OSD = m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Orbiter_CONST);
	DeviceData_Base *pDevice_VFD = m_pCommand_Impl->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_LCDVFD_Displays_CONST);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Displaying on OSD: %d VFD: %d %s",
		(pDevice_OSD ? pDevice_OSD->m_dwPK_Device : 0),
		(pDevice_VFD ? pDevice_VFD->m_dwPK_Device : 0),
		sMessage.c_str());

	string sResponse; // Get Return confirmation so we know the message gets through before continuing
	if( pDevice_OSD )
	{
		DCE::CMD_Display_Message_DT CMD_Display_Message_DT(m_dwPK_Device,DEVICETEMPLATE_Orbiter_Plugin_CONST,BL_SameHouse,
			sMessage,"","","5",StringUtils::itos(pDevice_OSD->m_dwPK_Device));
		m_pCommand_Impl->SendCommand(CMD_Display_Message_DT, &sResponse);
	}
	if( pDevice_VFD )
	{
		DCE::CMD_Display_Message CMD_Display_Message(m_dwPK_Device,pDevice_VFD->m_dwPK_Device,
			sMessage,
			StringUtils::itos(VL_MSGTYPE_RUNTIME_NOTICES),"app serve","5","");
		m_pCommand_Impl->SendCommand(CMD_Display_Message, &sResponse);
	}
}

void Disk_Drive_Functions::UpdateDiscLocation(char cType,int PK_Disc)
{
	Row_DiscLocation *pRow_DiscLocation = m_pDatabase_pluto_media->DiscLocation_get()->GetRow(m_dwPK_Device,0);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::UpdateDiscLocation device %d type %c disc %d / %p", m_dwPK_Device,cType,PK_Disc,pRow_DiscLocation);
	if( pRow_DiscLocation )
	{
		if( cType=='E' )  // Shouldn't be a record if it's empty
		{
			pRow_DiscLocation->Delete();
			m_pDatabase_pluto_media->DiscLocation_get()->Commit();
			return;
		}
		pRow_DiscLocation->Reload();
	}
	else if( cType=='E' )
		return; // Nothing to do if it's empty
	else
	{
		pRow_DiscLocation = m_pDatabase_pluto_media->DiscLocation_get()->AddRow();
		pRow_DiscLocation->EK_Device_set(m_dwPK_Device);
	}

	if( PK_Disc>0 )
		pRow_DiscLocation->FK_Disc_set(PK_Disc);
	else if( PK_Disc==0 )
		pRow_DiscLocation->FK_Disc_setNull(true);

	char c[2]=" ";
	c[0]=cType;
	pRow_DiscLocation->Type_set(c);
	m_pDatabase_pluto_media->DiscLocation_get()->Commit();
}

bool Disk_Drive_Functions::isRipping()
{
	return false;
}

void Disk_Drive_Functions::GetTracksForDisc(Row_Disc *pRow_Disc,map<int,string> &mapTracks)
{
	string sAllTracks=getTracks("");
	string::size_type pos=0;
	while(pos<sAllTracks.size())
	{
		int Track = atoi( StringUtils::Tokenize( sAllTracks, "\n", pos ).c_str() );
		mapTracks[Track] = "";
	}

	string sSQL = "JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_Disc=" + StringUtils::itos(pRow_Disc->PK_Disc_get()) + " AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Title_CONST);
	vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
	m_pDatabase_pluto_media->Disc_Attribute_get()->GetRows(sSQL,&vectRow_Disc_Attribute);
	for(vector<Row_Disc_Attribute *>::iterator it=vectRow_Disc_Attribute.begin();it!=vectRow_Disc_Attribute.end();++it)
	{
		Row_Disc_Attribute *pRow_Disc_Attribute = *it;
		Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
		if( pRow_Attribute )
		{
			string sName = FileUtils::ValidFileName(pRow_Attribute->Name_get(),false,true);
			mapTracks[ pRow_Disc_Attribute->Track_get() ] = sName;
		}
	}
}

bool Disk_Drive_Functions::LockDrive(Locked locked,void *p_void)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::LockDrive m_eLocked %d locked %d %p", (int) m_eLocked, (int) locked,m_pLockedPtr);

	if( m_eLocked!=locked_available )
	{
		if( m_pLockedPtr==p_void )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::LockDrive already locked");
			return true;
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Disk_Drive_Functions::LockDrive cannot lock %p/%p", m_pLockedPtr, p_void);
		return true;  // temporarily letting locking problems through
	}

	m_pLockedPtr=p_void;
	m_eLocked=locked;
	return true;
}

void Disk_Drive_Functions::UnlockDrive()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Disk_Drive_Functions::UnlockDrive %p",m_pLockedPtr);
	m_eLocked=locked_available;
}

Disk_Drive_Functions::Locked Disk_Drive_Functions::m_eLocked_get(void **p_void)
{
	if( p_void )
		*p_void = m_pLockedPtr;

	return m_eLocked;
}

void Disk_Drive_Functions::IdDisk(bool bForce)
{
	if( m_pDevice_MediaIdentifier && (bForce || m_bAutoIdentifyMedia) )
	{
		DCE::CMD_Identify_Media CMD_Identify_Media(m_dwPK_Device,m_pDevice_MediaIdentifier->m_dwPK_Device,
			m_dwPK_Device,StringUtils::itos(m_discid),m_sDrive,m_dwPK_Device);
		m_pCommand_Impl->SendCommand(CMD_Identify_Media);
	}
}
