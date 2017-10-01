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
#include "WMMCDI.h"

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
  return true;
}

string IdentifierGenericCD::GetIdentifiedData()
{
  WMMCDI* pWMMCDI = new WMMCDI(m_sPath);

  if (!pWMMCDI->calculate())
    return "";

  OutputMiscTab DiscData(pWMMCDI->WMMCDIId_get());

  DiscData.addAttribute(0, ATTRIBUTETYPE_Disc_ID_CONST, 0, pWMMCDI->WMMCDIId_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Album_CONST, 0, "Unknown Album");
  DiscData.addAttribute(0, ATTRIBUTETYPE_Album_Artist_CONST, 0, "Unknown Artist");
  DiscData.addAttribute(0, ATTRIBUTETYPE_Performer_CONST, 0, "Unknown Performer");

  for (int i=0; i<pWMMCDI->WMMCDINumtracks_get(); i++)
    {
      DiscData.addAttribute(i, ATTRIBUTETYPE_Track_CONST, 0, StringUtils::itos(i));
      DiscData.addAttribute(i, ATTRIBUTETYPE_Title_CONST, 0, "Track "+StringUtils::itos(i));
      DiscData.addAttribute(i, ATTRIBUTETYPE_Performer_CONST, 0, "Unknown Performer");
    }

  return DiscData.OutputAttributes();
}

string IdentifierGenericCD::GetPictureData()
{
  string sData="";
  FileUtils::ReadTextFile("/usr/pluto/share/audio_cd.jpg",sData);
  return sData;
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
