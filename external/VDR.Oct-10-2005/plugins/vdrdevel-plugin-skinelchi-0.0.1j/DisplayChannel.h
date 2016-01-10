#ifndef __DISPLAYCHANNEL_H
#define __DISPLAYCHANNEL_H

#include <vdr/skins.h>
#include "setup.h"

class cSkinElchiDisplayChannel : public cSkinDisplayChannel {
private:
  cOsd *osd;
  int logox, logoy;
  int x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  int y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;
  bool withInfo;
  int lineHeight;
  tColor bg;
  int timeWidth;
  bool message, sleeptimermessage;
  const cEvent *present;
  const tTrackId *track;
  int lastSeen;
  time_t oldpresentvps;
  static cBitmap bmVps, bmTeletext, bmRadio, bmAudio, bmDolbyDigital, bmEncrypted, bmRecording;
  int setchannel_x;

public:
  cSkinElchiDisplayChannel(bool WithInfo);
  virtual ~cSkinElchiDisplayChannel();
  virtual void SetChannel(const cChannel *Channel, int Number);
  virtual void SetEvents(const cEvent *Present, const cEvent *Following);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
};

#endif //__DISPLAYCHANNEL_H
