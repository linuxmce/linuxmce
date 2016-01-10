#ifndef __DISPLAYVOLUME_H
#define __DISPLAYVOLUME_H

#include <vdr/skins.h>
#include "setup.h"

class cSkinElchiDisplayVolume : public cSkinDisplayVolume {
private:
  cOsd *osd;
  static cBitmap bmMute, bmVolume;
  int mute, current;
public:
  cSkinElchiDisplayVolume(void);
  virtual ~cSkinElchiDisplayVolume();
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
  };

#endif //__DISPLAYVOLUME_H
