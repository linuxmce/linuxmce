/**
 * IdentifierMDR - Identifier to call QueryMDRDVD, ParseMDRDVD, and return (hopefully)
 * a fully parsed object of relevant metadata for an inserted video DVD.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#ifndef IDENTIFIERDVDMDR_H
#define IDENTIFIERDVDMDR_H

#include <string>
#include "IdentifierDVDBase.h"
#include "MDRDVDParser.h"
#include "MDRDVDQuery.h"
#include "DVDID.h"

using namespace std;

namespace DCE
{
  class IdentifierDVDMDR : public IdentifierDVDBase
  {

  private:
    MDRDVDQuery* m_pQuery;
    MDRDVDParser* m_pParser;
    void DumpReplyToLog(MDRDVDReply* pReply);
    void SplitActorsIntoVector(string sActors, vector<string>& vectActors);

  public:
    IdentifierDVDMDR(string sPath, string sID);
    virtual ~IdentifierDVDMDR();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetPictureData();
    virtual string GetPictureURL();
  };
}


#endif /* IDENTIFIERDVDMDR_H */
