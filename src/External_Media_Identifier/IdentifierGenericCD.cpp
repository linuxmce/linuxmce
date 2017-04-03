/**
 * IdentifierGenericCD - A fallback for audio CD media, to present generic unidentifiable media.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#include "IdentifierGenericCD.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "../pluto_media/Define_AttributeType.h"
#include "../pluto_main/Define_MediaType.h"
#include "OutputMiscTab.h"
#include "HTTPPicture.h"

using namespace DCE;
using namespace std;

IdentifierGenericCD::IdentifierGenericCD(string sPath, string sID) : IdentifierBase(sPath, sID)
{
}

IdentifierGenericCD::~IdentifierGenericCD()
{
}

bool IdentifierGenericCD::Init()
{
  // nothing really to initialize.
  return true;
}

bool IdentifierGenericCD::Identify()
{
  // TODO: Implement
  return false;
}

string IdentifierGenericCD::GetIdentifiedData()
{
  // TODO: Implement
  return "";
}

string IdentifierGenericCD::GetPictureData()
{
  // TODO: Implement
  return "";
}

string IdentifierGenericCD::GetPictureURL()
{
  // N/A
  return "";
}

string IdentifierGenericCD::GetIdentityType()
{
  return "MISC-TAB";
}

int IdentifierGenericCD::GetMediaType()
{
  return MEDIATYPE_pluto_CD_CONST;
}
