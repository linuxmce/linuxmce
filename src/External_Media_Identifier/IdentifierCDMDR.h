/**
 * IdentifierMDR - Identifier to call QueryMDRCD, ParseMDRCD, and return (hopefully)
 * a fully parsed object of relevant metadata for an inserted video CD.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#ifndef IDENTIFIERCDMDR_H
#define IDENTIFIERCDMDR_H

#include <string>
#include "IdentifierBase.h"
#include "MDRCDParser.h"
#include "MDRCDQuery.h"
#include "WMMCDI.h"

using namespace std;

namespace DCE
{
  class IdentifierCDMDR : public IdentifierBase
  {

  private:
    MDRCDQuery* m_pQuery;
    MDRCDParser* m_pParser;
    WMMCDI* m_pWMMCDI;
    void DumpReplyToLog(MDRCDReply* pReply);

  public:
    IdentifierCDMDR(string sPath, string sID);
    virtual ~IdentifierCDMDR();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetPictureData();
    virtual string GetPictureURL();
    virtual string GetIdentityType();
  };
}


#endif /* IDENTIFIERCDMDR_H */
