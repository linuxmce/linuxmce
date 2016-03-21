/**
 * IdentifierBase - the base class for media identification
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> - class structure
 * Author: phenigma <phenigma@hotmail.com> - functionality
 */

#ifndef IDENTIFIERBASE_H
#define IDENTIFIERBASE_H

#include <string>

using namespace std;

namespace DCE 
{
  class IdentifierBase
  {
  public:
    string m_sPath; // The path to identify
    string m_sDiskId; // LMCE internal reference for unidentified disc
    bool m_bIsIdentified; // Media Identifier was able to identify media
    bool m_bultipleMatches; // There were multiple matches.
    enum eIdentityType {NONE,CDDB_TAB,MISC_TAB};
    IdentifierBase(string sPath, string sID);
    virtual ~IdentifierBase();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetPictureURL();
    virtual string GetPictureData();
    virtual string GetIdentityType();
    virtual int GetMediaType();
  };
}

#endif
