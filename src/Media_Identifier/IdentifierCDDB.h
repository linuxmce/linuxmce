/**
 * IdentifierCDDB - identify an inserted disc against FreeDB
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef IDENTIFIERCDDB_H
#define IDENTIFIERCDDB_H

#include "IdentifierBase.h"
#include <cdio/cdio.h>
#include <vector>

namespace DCE
{
  class IdentifierCDDB : public IdentifierBase
  {
  public:
    IdentifierCDDB(string sPath);
    ~IdentifierCDDB();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual eIdentityType GetIdentityType();
  private:
    CdIo_t* pCDIO; // The libCDIO pointer.
    track_t iNumTracks, iCurrentTrack; // CD track counters
    lba_t iCurrentTrackLBA; // Current Track Logical Block Address (LBA)
    vector<int> vectFrameOffsets; // A list of frame offsets for each track.    
  };
}

#endif
