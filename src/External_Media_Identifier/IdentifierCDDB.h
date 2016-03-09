/**
 * IdentifierCDDB - identify an inserted disc against FreeDB
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> - class structure
 * Author: phenigma <phenigma@hotmail.com> - functionality
 */

#ifndef IDENTIFIERCDDB_H
#define IDENTIFIERCDDB_H

#include "IdentifierBase.h"
#include <cddb/cddb.h>
#include <vector>

namespace DCE
{
  class IdentifierCDDB : public IdentifierBase
  {
  public:
    IdentifierCDDB(string sPath, string sID);
    virtual ~IdentifierCDDB();
    virtual bool Init();
    virtual bool Identify();
    virtual string GetIdentifiedData();
    virtual string GetIdentityType();
    virtual int GetMediaType();
  private:
    cddb_conn_t *conn; // = NULL; /* libcddb connection structure */
    cddb_disc_t *disc; // = NULL; /* libcddb disc structure */
    char *charset; // = NULL;     /* requested character set encoding */
    int use_cd; // = 0;           /* use CD-ROM to retrieve disc data */
    const char *device; // = NULL;      /* device to use if use_cd == 1. NULL means to find a suitable CD-ROM drive. */
    char *category; // = NULL;    /* category command-line argument */
    unsigned int discid; //  = 0; /* disc ID command-line argument or calculated */

//    CdIo_t* pCDIO; // The libCDIO pointer.
//    track_t iNumTracks, iCurrentTrack; // CD track counters
//    lba_t iCurrentTrackLBA; // Current Track Logical Block Address (LBA)
//    vector<int> vectFrameOffsets; // A list of frame offsets for each track.    
  };
}

#endif
