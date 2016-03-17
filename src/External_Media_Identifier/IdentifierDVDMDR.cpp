/**
 * IdentifierMDR - Identifier to call QueryMDRDVD, ParseMDRDVD, and return (hopefully)
 * a fully parsed object of relevant metadata for an inserted video DVD.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#include "IdentifierDVDMDR.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "../pluto_media/Define_AttributeType.h"
#include "OutputMiscTab.h"
#include "HTTPPicture.h"

using namespace DCE;
using namespace std;

IdentifierDVDMDR::IdentifierDVDMDR(string sPath, string sID) : IdentifierDVDBase(sPath, sID)
{
  m_pQuery=NULL;
  m_pParser=NULL;
}

IdentifierDVDMDR::~IdentifierDVDMDR()
{
  if (m_pQuery)
    delete m_pQuery;
  
  if (m_pParser)
    delete m_pParser;
}

bool IdentifierDVDMDR::Init()
{
  if (!IdentifierDVDBase::Init())
    return false;
  return true;
}

bool IdentifierDVDMDR::Identify()
{
  if (!IdentifierDVDBase::Identify())
    return false;

  m_pQuery=new MDRDVDQuery(DVDID_get());

  if (!m_pQuery)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDMDR::Identify() - could not instantiate a query object. Aborting");
      return false;
    }

  if (!m_pQuery->execute())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDMDR::Identify() - query execution failed. Aborting.");
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierDVDMDR::Identify() - Received output from MDR. Length: %d bytes.",m_pQuery->Reply_get().size());
    }

  m_pParser=new MDRDVDParser(m_pQuery->Reply_get());

  if (!m_pParser)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDMDR::Identify() - Could not instantiate parser");
      return false;
    }

  if (!m_pParser->parse())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDMDR::Identify() - Could not parse reply from MDR. Aborting");
      return false;
    }
  else
    {
      DumpReplyToLog(m_pParser->Reply_get());
    }

  return true;
}

void IdentifierDVDMDR::SplitActorsIntoVector(string sActors, vector<string>& vectActors)
{
  StringUtils::Tokenize(sActors,";",vectActors);
}

string IdentifierDVDMDR::GetIdentifiedData()
{
  OutputMiscTab DiscData(m_pQuery->DVDId_get());
  MDRDVDReply* r = m_pParser->Reply_get();

  DiscData.addAttribute(0, ATTRIBUTETYPE_Disc_ID_CONST, 0, m_pQuery->DVDId_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Title_CONST, 0, r->DVDTitle_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Studio_CONST, 0, r->Studio_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Director_CONST, 0, r->Director_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Rating_CONST, 0, r->Rating_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Genre_CONST, 0, r->Genre_get());
  
  vector<string> vectActors;
  SplitActorsIntoVector(r->Actors_get(),vectActors);

  // actors -> performers
  for (vector<string>::iterator it=vectActors.begin(); it!=vectActors.end(); ++it)
    {
      string sActor=*it;
      DiscData.addAttribute(0, ATTRIBUTETYPE_Performer_CONST, 0, sActor);
    }

  // now we iterate through titles and chapters
  for (vector<MDRTitle*>::iterator it=r->Titles_get().begin(); it!=r->Titles_get().end(); ++it)
    {
      MDRTitle* t = *it;
      for (vector<MDRChapter*>::iterator cit=t->Chapters_get().begin(); cit!=t->Chapters_get().end(); ++cit)
	{
	  MDRChapter* c = *cit;
	  DiscData.addAttribute(atoi(t->TitleNum_get().c_str()),
				ATTRIBUTETYPE_Chapter_CONST,
				atoi(c->ChapterNum_get().c_str()),
				c->ChapterTitle_get());
	}
    }
  
  return DiscData.OutputAttributes();

}

string IdentifierDVDMDR::GetPictureData()
{
  HTTPPicture* p = new HTTPPicture(m_pParser->Reply_get()->DVDCoverURL_get());

  if (!p->get())
    return "";

  return p->Data_get(); 
}

string IdentifierDVDMDR::GetPictureURL()
{
  return m_pParser->Reply_get()->DVDCoverURL_get();
}

void IdentifierDVDMDR::DumpReplyToLog(MDRDVDReply* pReply)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Title: %s",pReply->DVDTitle_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Studio: %s",pReply->Studio_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Actors: %s",pReply->Actors_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Director: %s",pReply->Director_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Rating: %s",pReply->Rating_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Genre: %s",pReply->Genre_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Number of Titles: %s",pReply->Titles_get().size());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- DVD Cover URL %s",pReply->DVDCoverURL_get().c_str());

  for (vector<MDRTitle*>::iterator it = pReply->Titles_get().begin(); it!=pReply->Titles_get().end(); ++it)
    {
      MDRTitle* t = *it;
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Title #%s",t->TitleNum_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Title: %s",t->Title_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Studio: %s",t->Studio_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Director: %s",t->Director_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Actors: %s",t->Actors_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Rating: %s",t->Rating_get().c_str());
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"++ Genre: %s",t->Genre_get().c_str());
      for (vector<MDRChapter*>::iterator cit = t->Chapters_get().begin(); cit!=t->Chapters_get().end();++cit)
	{
	  MDRChapter* c = *cit;
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"== Chapter #%s",c->ChapterNum_get().c_str());
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"== Chapter Name: %s",c->ChapterTitle_get().c_str());
	}
    }
}
