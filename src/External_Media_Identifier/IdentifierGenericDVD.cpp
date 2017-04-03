/**
 * IdentifierGenericDVD - A fallback for video DVD media, to present generic unidentifiable media.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#include "IdentifierGenericDVD.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "../pluto_media/Define_AttributeType.h"
#include "../pluto_main/Define_MediaType.h"
#include "OutputMiscTab.h"
#include "HTTPPicture.h"

using namespace DCE;
using namespace std;

IdentifierGenericDVD::IdentifierGenericDVD(string sPath, string sID) : IdentifierBase(sPath, sID)
{
}

IdentifierGenericDVD::~IdentifierGenericDVD()
{
}

bool IdentifierGenericDVD::Init()
{
  // nothing really to initialize.
  return true;
}

bool IdentifierGenericDVD::Identify()
{
  // TODO: Implement
}

string IdentifierGenericDVD::GetIdentifiedData()
{
  // TODO: Implement
  return "";
}

string IdentifierGenericDVD::GetPictureData()
{
  // TODO: Implement
  return "";
}

string IdentifierGenericDVD::GetPictureURL()
{
  // N/A
  return "";
}

string IdentifierGenericDVD::GetIdentityType()
{
  return "MISC-TAB";
}

int IdentifierGenericDVD::GetMediaType()
{
  return MEDIATYPE_pluto_DVD_CONST;
}
