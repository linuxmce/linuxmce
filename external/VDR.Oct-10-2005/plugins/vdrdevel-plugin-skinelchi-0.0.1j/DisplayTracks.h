#ifndef __DISPLAYTRACKS_H
#define __DISPLAYTRACKS_H

#include <vdr/skins.h>
#include "setup.h"

class cSkinElchiDisplayTracks : public cSkinDisplayTracks {
private:
  cOsd *osd;
  static cBitmap bmAudioLeft, bmAudioRight, bmAudioStereo, bmArrowDown, bmArrowUp;
  int x0, x1, x2, x3;
  int y0, y1, y2, y3, y4, y5;
  int lineHeight;
  int currentIndex;
  int index;
  int offset;
  int numTracks;
  void SetItem(const char *Text, int Index, bool Current);
public:
  cSkinElchiDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual ~cSkinElchiDisplayTracks();
  virtual void SetTrack(int Index, const char * const *Tracks);
  virtual void SetAudioChannel(int AudioChannel);
  virtual void Flush(void);
  virtual int MaxItems(void);
  virtual void Clear(void);
  };

#endif //__DISPLAYTRACKS_H
