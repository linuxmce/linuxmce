/**
 * IdentifierBase - the base class for media identification
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> - class structure
 * Author: phenigma <phenigma@hotmail.com> - functionality
 */

#include "IdentifierBase.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

#include <sys/types.h>
#include <cdio/cdio.h>

using namespace std;
using namespace DCE;

/**
 * ctor - pass in file or device path to identify.
 */
IdentifierBase::IdentifierBase(string sPath, string sID)
{
  m_sPath = sPath;
  m_sDiskId = sID;
}

/**
 * dtor
 */
IdentifierBase::~IdentifierBase()
{
}

bool IdentifierBase::Init()
{
  return true;
}

bool IdentifierBase::Identify()
{
  if (!FileUtils::FileExists(m_sPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifierBase::Identify() - path %s does not exist. Bailing.",m_sPath.c_str());
      return false;
    }
  return true; // Only false if our sanity checks here fail.
}

string IdentifierBase::GetIdentifiedData()
{
  return "";
}

//IdentifierBase::eIdentityType IdentifierBase::GetIdentityType()
string IdentifierBase::GetIdentityType()
{
  return "NONE";
}

int IdentifierBase::GetMediaType()
{
  return 0; // unknown
}

string IdentifierBase::GetPictureURL()
{
  return ""; 
}

string IdentifierBase::GetPictureData()
{
  return "";
}
