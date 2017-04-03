/**
 * IdentifierGenericDVD - A fallback for video DVD media, to present generic unidentifiable media.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
 */

#ifndef IDENTIFIERGENERICDVD_H
#define IDENTIFIERGENERICDVD_H

#include <string>
#include "IdentifierBase.h"

using namespace std;

namespace DCE
{
  class IdentifierGenericDVD : public IdentifierBase
  {

  public:
    IdentifierGenericDVD(string sPath, string sID);
    virtual ~IdentifierGenericDVD();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetPictureData();
    virtual string GetPictureURL();
    virtual string GetIdentityType();
    virtual int GetMediaType();
  };
}


#endif /* IDENTIFIERGENERICDVD_H */
