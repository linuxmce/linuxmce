/**
 * IdentifierDVDBase - common code for Identifying DVDs by a DVD ID CRC64 Checksum
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> 
 */

#include "IdentifierDVDBase.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include <stdio.h>
#include <sys/mount.h>

#define MOUNT_PATH_PREFIX "/tmp/"

using namespace DCE;
using namespace std;

IdentifierDVDBase::IdentifierDVDBase(string sPath, string sID) : IdentifierBase(sPath, sID)
{
  m_sPath=sPath;
  m_sDiskId=sID;
  m_pDVDID=NULL;
  m_bIsMounted=false;
}

IdentifierDVDBase::~IdentifierDVDBase()
{
  if (m_pDVDID)
    delete m_pDVDID;

  if (m_bIsMounted)
    UnmountDVD();

}

void IdentifierDVDBase::UnmountDVD()
{
  if (!m_bIsMounted || m_sTempPath.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifierDVDBase::UnmountDVD() - Called when nothing was mounted?");
      return;
    }

  umount(m_sTempPath.c_str());
  unlink(m_sTempPath.c_str());

  m_sTempPath="";
  m_bIsMounted=false;

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierDVDBase::UnmountDVD() - Unmounted %s at path %s",m_sPath.c_str(),m_sTempPath.c_str());
}

bool IdentifierDVDBase::MountDVD(string sPath)
{
  char* cPath=tempnam("/tmp/","Identifier");

  if (mount(m_sPath.c_str(),cPath,"udf",0,NULL))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDBase::MountDVD() - Could not mount device %s",sPath.c_str());
      return false;
    }

  m_sTempPath=string(cPath);
  m_bIsMounted=true;
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierDVDBase::MountDVD() - Disc %s mounted at path %s",sPath.c_str(),m_sTempPath.c_str());
  return true;
}

bool IdentifierDVDBase::Init()
{
  if (!IdentifierBase::Init())
    return false;

  if (!FileUtils::FileExists(m_sPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDBase::Init() - Could not find device path %s - Aborting.",m_sPath.c_str());
      return false;
    }

  if (!MountDVD(m_sPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDBase::Init() - Could not mount DVD device %s at path %s",m_sPath.c_str(),m_sTempPath.c_str());
      return false;
    }

  m_pDVDID = new DVDID(m_sTempPath);
  
  if (!m_pDVDID)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDBase::Init() - m_pDVDID is NULL. Possible initialization failure.");
      return false;
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierDVDBase::Init() - DVD %s mounted and ready to identify at path %s.",m_sPath.c_str(),m_sTempPath.c_str());
  return true;
}

bool IdentifierDVDBase::Identify()
{
  m_sDVDID=m_pDVDID->DVDID_get();
  
  if (m_sDVDID.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierDVDBase::Identify() no DVDID returned for inserted disc in %s at path %s",m_sPath.c_str(),m_sTempPath.c_str());
      return false;
    }

  return true;
}

string IdentifierDVDBase::GetIdentifiedData()
{
  return "";
}

string IdentifierDVDBase::GetIdentityType()
{
  return "MISC-TAB";
}

int IdentifierDVDBase::GetMediaType()
{
  return 3;
}
