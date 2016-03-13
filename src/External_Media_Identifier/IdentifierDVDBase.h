/**
 * IdentifierDVDBase - common code for Identifying DVDs by a DVD ID CRC64 Checksum
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> 
 */

#ifndef IDENITIFERDVDBASE_H
#define IDENTIFIERDVDBASE_H

#include <string>
#include "IdentifierBase.h"
#include "DVDID.h"

using namespace std;

namespace DCE
{
  class IdentifierDVDBase : public IdentifierBase
  {

  private:
    string m_sTempPath;
    bool m_bIsMounted;
    DVDID* m_pDVDID;
    string m_sDVDID;

    bool MountDVD(string sPath);
    void UnmountDVD();

  public:
    IdentifierDVDBase(string sPath, string sID);
    virtual ~IdentifierDVDBase();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetIdentityType();
    virtual int GetMediaType();
    virtual string DVDID_get() {return m_sDVDID;}

  };
}

#endif /* IDENTIFIERDVDBASE_H */
