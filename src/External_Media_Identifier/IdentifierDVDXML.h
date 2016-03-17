/**
 * IdentifierDVDXML - Identifier to call QueryDVDXML, ParseDVDXML, and return (hopefully)
 * a fully parsed object of relevant metadata for an inserted video DVD.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#ifndef IDENTIFIERDVDXML_H
#define IDENTIFIERDVDXML_H

#include <string>
#include <vector>
#include "IdentifierDVDBase.h"
#include "DVDXMLParser.h"
#include "DVDXMLQuery.h"
#include "DVDID.h"

using namespace std;

namespace DCE
{
  class IdentifierDVDXML : public IdentifierDVDBase
  {

  private:
    DVDXMLQuery* m_pQuery;
    DVDXMLParser* m_pParser;
    void DumpReplyToLog(DVDXMLReply* pReply);
    void SplitActorsIntoVector(string sActors, vector<string>& vectActors);

  public:
    IdentifierDVDXML(string sPath, string sID);
    virtual ~IdentifierDVDXML();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();

  };
}


#endif /* IDENTIFIERDVDXML_H */
