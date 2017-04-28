/**
 * IdentifierGenericCD - A fallback for audio CD media, to present generic unidentifiable media.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#ifndef IDENTIFIERGENERICCD_H
#define IDENTIFIERGENERICCD_H

#include <string>
#include "IdentifierBase.h"

using namespace std;

namespace DCE
{
  class IdentifierGenericCD : public IdentifierBase
  {

  public:
    IdentifierGenericCD(string sPath, string sID);
    virtual ~IdentifierGenericCD();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetPictureData();
    virtual string GetPictureURL();
    virtual string GetIdentityType();
    virtual int GetMediaType();
  };
}


#endif /* IDENTIFIERGENERICCD_H */
