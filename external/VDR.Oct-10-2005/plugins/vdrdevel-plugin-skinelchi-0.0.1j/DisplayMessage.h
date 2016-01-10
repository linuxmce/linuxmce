#ifndef __DISPLAYMESSAGE_H
#define __DISPLAYMESSAGE_H

#include <vdr/skins.h>
#include "setup.h"

class cSkinElchiDisplayMessage : public cSkinDisplayMessage {
private:
  cOsd *osd;
public:
  cSkinElchiDisplayMessage(void);
  virtual ~cSkinElchiDisplayMessage();
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

#endif //__DISPLAYMESSAGE_H
