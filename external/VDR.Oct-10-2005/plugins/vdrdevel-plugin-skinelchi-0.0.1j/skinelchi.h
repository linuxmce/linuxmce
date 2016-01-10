/*
 * $Id:
 */

#ifndef __SKINELCHI_H
#define __SKINELCHI_H

#include <vdr/skins.h>
#include "setup.h"

extern const char *OSDSKIN;

#ifdef VIDEOSTATUS
#include <linux/dvb/video.h>
extern video_status videostatus;
extern int fd_video;
extern video_size_t vs;
#endif

class cSkinElchi : public cSkin {
public:
  cSkinElchi(void);
  virtual const char *Description(void);
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual cSkinDisplayMessage *DisplayMessage(void);
  };

#endif //__SKINELCHI_H
