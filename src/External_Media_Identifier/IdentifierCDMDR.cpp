/**
 * IdentifierMDR - Identifier to call QueryMDRCD, ParseMDRCD, and return (hopefully)
 * a fully parsed object of relevant metadata for an inserted video CD.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#include "IdentifierCDMDR.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "../pluto_media/Define_AttributeType.h"
#include "OutputMiscTab.h"
#include "HTTPPicture.h"

using namespace DCE;
using namespace std;

IdentifierCDMDR::IdentifierCDMDR(string sPath, string sID) : IdentifierBase(sPath, sID)
{
  m_pQuery=NULL;
  m_pParser=NULL;
  m_pWMMCDI=NULL;
  m_sPath=sPath;
}

IdentifierCDMDR::~IdentifierCDMDR()
{
  if (m_pQuery)
    delete m_pQuery;
  
  if (m_pParser)
    delete m_pParser;

  if (m_pWMMCDI)
    delete m_pWMMCDI;
}

bool IdentifierCDMDR::Init()
{
  m_pWMMCDI = new WMMCDI(m_sPath);
  
  if (!m_pWMMCDI)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierCDMDR::Init() - Could not initialize WMMCDI class. Aborting");
      return false;
    }

  return true;
}

bool IdentifierCDMDR::Identify()
{
  if (!m_pWMMCDI->calculate())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierCDMDR::Identify() - Could not calculate disk ID.");
      return false;
    }

  m_pQuery=new MDRCDQuery(m_pWMMCDI->WMMCDIId_get());

  if (!m_pQuery)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierCDMDR::Identify() - could not instantiate a query object. Aborting");
      return false;
    }

  if (!m_pQuery->execute())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierCDMDR::Identify() - query execution failed. Aborting.");
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierCDMDR::Identify() - Received output from MDR. Length: %d bytes.",m_pQuery->Reply_get().size());
    }

  m_pParser=new MDRCDParser(m_pQuery->Reply_get());

  if (!m_pParser)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierCDMDR::Identify() - Could not instantiate parser");
      return false;
    }

  if (!m_pParser->parse())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierCDMDR::Identify() - Could not parse reply from MDR. Aborting");
      return false;
    }
  else
    {
      DumpReplyToLog(m_pParser->Reply_get());
    }

  return true;
}

string IdentifierCDMDR::GetIdentifiedData()
{
  OutputMiscTab DiscData(m_pWMMCDI->WMMCDIId_get());
  MDRCDReply* r = m_pParser->Reply_get();

  DiscData.addAttribute(0, ATTRIBUTETYPE_Disc_ID_CONST, 0, m_pWMMCDI->WMMCDIId_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Album_CONST, 0, r->AlbumTitle_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Album_Artist_CONST, 0, r->AlbumArtist_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Performer_CONST, 0, r->AlbumArtist_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Genre_CONST, 0, r->Genre_get());

  vector<MDRTrack*> vectTracks = r->Tracks_get();

  for (vector<MDRTrack*>::iterator it=vectTracks.begin(); it!=vectTracks.end(); ++it)
    {
      MDRTrack* t = *it;
      int trackNum = atoi(t->TrackNumber_get().c_str());
      DiscData.addAttribute(trackNum, ATTRIBUTETYPE_Track_CONST, 0, t->TrackNumber_get());
      DiscData.addAttribute(trackNum, ATTRIBUTETYPE_Title_CONST, 0, t->TrackTitle_get());
      DiscData.addAttribute(trackNum, ATTRIBUTETYPE_Performer_CONST, 0, r->AlbumArtist_get());
      // DiscData.addAttribute(trackNum, ATTRIBUTETYPE_Album_CONST, 0, r->AlbumTitle_get());
      // DiscData.addAttribute(trackNum, ATTRIBUTETYPE_Genre_CONST, 0, r->Genre_get());
    }
  
  return DiscData.OutputAttributes();

}

string IdentifierCDMDR::GetPictureData()
{
  HTTPPicture* p = new HTTPPicture(m_pParser->Reply_get()->CoverURL_get());

  if (!p->get())
    return "";

  return p->Data_get(); 
}

string IdentifierCDMDR::GetPictureURL()
{
  return m_pParser->Reply_get()->CoverURL_get();
}

void IdentifierCDMDR::DumpReplyToLog(MDRCDReply* pReply)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Album Title: %s",pReply->AlbumTitle_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Album Artist: %s",pReply->AlbumArtist_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Release Date: %s",pReply->ReleaseDate_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Label: %s",pReply->Label_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Genre: %s",pReply->Genre_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Cover URL: %s",pReply->CoverURL_get().c_str());

  vector<MDRTrack*> vectTracks = pReply->Tracks_get();

  for (vector<MDRTrack*>::iterator it=vectTracks.begin(); it!=vectTracks.end(); ++it)
    {
      MDRTrack* t = *it;
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Track Number: %s",t->TrackNumber_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Track Title: %s",t->TrackTitle_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"-------------------------------------------");
    }
}

string IdentifierCDMDR::GetIdentityType()
{
  return "MISC-TAB";
}
