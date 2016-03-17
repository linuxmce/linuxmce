/**
 * IdentifierMDR - Identifier to call QueryDVDXML, ParseDVDXML, and return (hopefully)
 * a fully parsed object of relevant metadata for an inserted video DVD.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#include "IdentifierDVDXML.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "../pluto_media/Define_AttributeType.h"
#include "OutputMiscTab.h"

using namespace DCE;
using namespace std;

IdentifierDVDXML::IdentifierDVDXML(string sPath, string sID) : IdentifierDVDBase(sPath, sID)
{
  m_pQuery=NULL;
  m_pParser=NULL;
}

IdentifierDVDXML::~IdentifierDVDXML()
{
  if (m_pQuery)
    delete m_pQuery;
  
  if (m_pParser)
    delete m_pParser;
}

bool IdentifierDVDXML::Init()
{
  if (!IdentifierDVDBase::Init())
    return false;
  return true;
}

bool IdentifierDVDXML::Identify()
{
  if (!IdentifierDVDBase::Identify())
    return false;

  m_pQuery=new DVDXMLQuery(DVDID_get());

  if (!m_pQuery)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDXML::Identify() - could not instantiate a query object. Aborting");
      return false;
    }

  if (!m_pQuery->execute())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDXML::Identify() - query execution failed. Aborting.");
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"IdentifierDVDXML::Identify() - Received output from MDR. Length: %d bytes.",m_pQuery->Reply_get().size());
    }

  m_pParser=new DVDXMLParser(m_pQuery->Reply_get());

  if (!m_pParser)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDXML::Identify() - Could not instantiate parser");
      return false;
    }

  if (!m_pParser->parse())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IdentifierDVDXML::Identify() - Could not parse reply from MDR. Aborting");
      return false;
    }
  else
    {
      DumpReplyToLog(m_pParser->Reply_get());
    }

  return true;
}

void IdentifierDVDXML::SplitActorsIntoVector(string sActors, vector<string>& vectActors)
{
  StringUtils::Tokenize(sActors,";",vectActors);
}

string IdentifierDVDXML::GetIdentifiedData()
{
  OutputMiscTab DiscData(m_pQuery->DVDId_get());
  DVDXMLReply* r = m_pParser->Reply_get();

  DiscData.addAttribute(0, ATTRIBUTETYPE_Disc_ID_CONST, 0, m_pQuery->DVDId_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Title_CONST, 0, r->Title_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Studio_CONST, 0, r->Studio_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Rating_CONST, 0, r->Rating_get());
  DiscData.addAttribute(0, ATTRIBUTETYPE_Genre_CONST, 0, r->Genre_get());
  
  vector<string> vectActors;
  SplitActorsIntoVector(r->Cast_get(),vectActors);

  // actors -> performers
  for (vector<string>::iterator it=vectActors.begin(); it!=vectActors.end(); ++it)
    {
      string sActor=*it;
      DiscData.addAttribute(0, ATTRIBUTETYPE_Performer_CONST, 0, sActor);
    }
  
  return DiscData.OutputAttributes();

}

void IdentifierDVDXML::DumpReplyToLog(DVDXMLReply* pReply)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Title: %s",pReply->Title_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Studio: %s",pReply->Studio_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Actors: %s",pReply->Cast_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Rating: %s",pReply->Rating_get().c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"-- Genre: %s",pReply->Genre_get().c_str());
}
