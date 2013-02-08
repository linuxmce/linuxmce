#ifndef __DISPLAYREPLAY_H
#define __DISPLAYREPLAY_H

#include <vdr/skins.h>
#include "setup.h"

class cSkinElchiDisplayReplay : public cSkinDisplayReplay {
private:
  cOsd *osd;
  static cBitmap bmFfwd, bmFfwd1, bmFfwd2, bmFfwd3, bmFfwd4, bmFfwd5, bmFfwd6, bmFfwd7, bmFfwd8, bmFrew,
	 bmFrew1, bmFrew2, bmFrew3, bmFrew4, bmFrew5, bmFrew6, bmFrew7, bmFrew8, bmPause, bmPlay, bmSfwd,
	 bmSfwd1, bmSfwd2, bmSfwd3, bmSfwd4, bmSfwd5, bmSfwd6, bmSfwd7, bmSfwd8, bmSrew, bmSrew1, bmSrew2,
	 bmSrew3, bmSrew4, bmSrew5, bmSrew6, bmSrew7, bmSrew8;
  int x0, x1, x2, x3, x4, x5;
  int y0, y1, y2, y3;
  int lastCurrentWidth;
  bool message;
  bool modeonly;
  char *title;
  char *oldtitle;
  cTimeMs DelayTime;

public:
  cSkinElchiDisplayReplay(bool ModeOnly);
  virtual ~cSkinElchiDisplayReplay();
  virtual void SetTitle(const char *Title);
  virtual void SetMode(bool Play, bool Forward, int Speed);
  virtual void SetProgress(int Current, int Total);
  virtual void SetCurrent(const char *Current);
  virtual void SetTotal(const char *Total);
  virtual void SetJump(const char *Jump);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

#endif //__DISPLAYREPLAY_H
