/**
 * IdentifyDisc - Routines to determine disc type for identification
 * mostly because we can't directly use the Disk_Drive_Functions..grrr...
 *
 * @Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @Version: 1.0
 */

#include "IdentifyDisc.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <linux/cdrom.h>
#include <limits.h>

namespace DCE
{

  IdentifyDisc::IdentifyDisc()
  {
  }
  
  IdentifyDisc::~IdentifyDisc()
  {
  }
  
  bool IdentifyDisc::IsDiscInDrive(string sDiscDevice)
  {
    int fd = open(sDiscDevice.c_str(),O_RDONLY,O_NONBLOCK);
    int drive_status = ioctl(fd, CDROM_DRIVE_STATUS, CDSL_CURRENT);
    bool bDiscInDrive=false;

    switch(drive_status)
      {
      case CDS_DISC_OK:
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::IsDiscInDrive(%s) - Disc in drive, Identifying.", sDiscDevice.c_str());
	bDiscInDrive=true;
	break;
      case CDS_NO_INFO:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::IsDiscInDrive(%s) - No status info from drive. Aborting. ", sDiscDevice.c_str());
	break;
      case CDS_NO_DISC:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::IsDiscInDrive(%s) - No disc in drive. Aborting.", sDiscDevice.c_str());
	break;
      case CDS_TRAY_OPEN:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::IsDiscInDrive(%s) - Drive tray is open. Aborting.", sDiscDevice.c_str());
	break;
      default:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::IsDiscInDrive(%s) - Fell through to default. Unknown state. Aborting.", sDiscDevice.c_str());
	break;
      }

    close(fd);
    sleep(1);
    
    return bDiscInDrive;
  }

  bool IdentifyDisc::HasValidTOC(string sDiscDevice)
  {
    int fd = open(sDiscDevice.c_str(),O_RDONLY|O_NONBLOCK);
    bool bRet=false;

    struct cdrom_tochdr th;
    int toc_status = ioctl(fd, CDROMREADTOCHDR, &th);
    
    if (toc_status != 0)
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::HasValidTOC(%s) - Could not read disc TOC. Returning unknown.",sDiscDevice.c_str());
	bRet=false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::HasValidTOC(%s) - Valid TOC",sDiscDevice.c_str());
	bRet=true;
      }
    
    return bRet;
  }

  bool IdentifyDisc::IsVideoDVD(string sDiscDevice)
  {
    bool result=false;
    time_t timeout = time(NULL) + 10;

    while(result==false && timeout > time(NULL))
      {
	if (!DirExists(sDiscDevice,"video_ts"))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::IsVideoDVD - not found, retrying.");
	    result=false;
	    sleep(1);
	  }
	else
	  {
	    result=true;
	  }
      }
    return result;
  }

  bool IdentifyDisc::IsBluRayVideo(string sDiscDevice)
  {
    bool result=false;
    time_t timeout = time(NULL) + 10;

    while(result==false && timeout > time(NULL))
      {
	if (!DirExists(sDiscDevice,"bdmv"))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::IsBluRayVideo - not found, retrying.");
	    result=false;
	    sleep(1);
	  }
	else
	  {
	    result=true;
	  }
      }
    return result;
  }

  IdentifyDisc::DiscType IdentifyDisc::GetDataTrackType(string sDiscDevice)
  {
    if (IsVideoDVD(sDiscDevice))
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::GetDataTrackType(%s) - disc is a Video DVD.", sDiscDevice.c_str());
	return DiscType::DVD;
      }

    if (IsBluRayVideo(sDiscDevice))
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::GetDataTrackType(%s) - disc is a Blu-Ray.", sDiscDevice.c_str());
	return DiscType::BluRay;
      }

      return DiscType::UNKNOWN;
  }

  IdentifyDisc::DiscType IdentifyDisc::GetDiscType(string sDiscDevice)
  {
    // Read disc status info
    int fd = open(sDiscDevice.c_str(),O_RDONLY|O_NONBLOCK);
    int disc_status = ioctl(fd, CDROM_DISC_STATUS, CDSL_CURRENT);
    IdentifyDisc::DiscType discType = IdentifyDisc::DiscType::UNKNOWN;

    switch(disc_status)
      {
      case CDS_AUDIO:
      case CDS_MIXED:  // Treat mixed discs as audio, for now.
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifyDisc::Identify(%s) - Identified an Audio CD",sDiscDevice.c_str());
	discType = DiscType::CD;
	break;
      case CDS_DATA_1:
      case CDS_DATA_2:
	discType = GetDataTrackType(sDiscDevice);
      case CDS_NO_INFO:
      case CDS_NO_DISC:
      case CDS_TRAY_OPEN:
      default:
	discType = DiscType::UNKNOWN;
      }

    return discType;

  }

  /**
   * Borrowed from Disk_Drive_Functions, cleaned up to use C++ types
   * and made to be a bit more self contained.
   */
  bool IdentifyDisc::DirExists(string sDevice, string sDirectory)
  {
    int fd=0;
    bool ret = false;       // return value
    unsigned short bs;  // the discs block size
    unsigned short ts;  // the path table size
    unsigned int tl;    // the path table location (in blocks)
    unsigned int i;
    unsigned int len_di = 0;    // length of the directory name in current path table entry
    unsigned int parent = 0;    // the number of the parent directory's path table entry
    char *dirname = NULL;   // filename for the current path table entry
    int pos = 0;        // our position into the path table
    int curr_record = 1;    // the path table record we're on
    
    fd = open(sDevice.c_str(),O_RDONLY|O_NONBLOCK);
    // read the block size
    lseek (fd, 0x8080, SEEK_SET);
    int iFirstRead = read (fd, &bs, 2);
    
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "IdentifyDisc::DirExists(%s, %s) - Looking for folder.", sDevice.c_str(),sDirectory.c_str());
    
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "FirstRead: %d fd: %d  directory: %s", iFirstRead, fd, sDirectory.c_str());
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
	if ((parent == 1) && (strcasecmp (dirname, sDirectory.c_str()) == 0))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "It's a match.");
	    ret=true;
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
    close(fd);
    return ret;
}

  IdentifyDisc::DiscType IdentifyDisc::Identify(string sDiscDevice)
  {

    /** Abort if disc device does not exist, for some reason. */
    if (!FileUtils::FileExists(sDiscDevice))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifyDisc::Identify(%s) - Could not find disc device. Aborting identify, and returning UNKNOWN.", sDiscDevice.c_str());
	return IdentifyDisc::DiscType::UNKNOWN;
      }

    if (!IsDiscInDrive(sDiscDevice))
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::Identify(%s) - Returning Unknown.", sDiscDevice.c_str());
	return IdentifyDisc::DiscType::UNKNOWN;
      }

    if (!HasValidTOC(sDiscDevice))
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifyDisc::Identify(%s) - Could not read disc TOC, disc is probably blank.");
	return IdentifyDisc::DiscType::UNKNOWN;
      }

    return GetDiscType(sDiscDevice);
  }
  
}
