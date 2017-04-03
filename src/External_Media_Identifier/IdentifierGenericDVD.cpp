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

IdentifierGenericDVD::IdentifierGenericDVD(string sPath, string sID) : IdentifierDVDBase(sPath, sID)
{
}

IdentifierGenericDVD::~IdentifierGenericDVD()
{
}

bool IdentifierGenericDVD::Init()
{
  // nothing really to initialize.
  return IdentifierDVDBase::Init();
}

bool IdentifierGenericDVD::Identify()
{
  if (!IdentifierDVDBase::Identify())
    return false;

  return true;
}

string IdentifierGenericDVD::GetIdentifiedData()
{

  OutputMiscTab DiscData(DVDID_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Disc_ID_CONST, 0, DVDID_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Title_CONST, 0, "Unknown Disc");
  
  return DiscData.OutputAttributes();
}

string IdentifierGenericDVD::GetPictureData()
{
  string sData="";
  FileUtils::ReadTextFile("/usr/pluto/share/video_cd.jpg",sData);
  return sData;
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
