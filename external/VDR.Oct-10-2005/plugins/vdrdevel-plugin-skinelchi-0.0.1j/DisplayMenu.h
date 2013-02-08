#ifndef __DISPLAYMENU_H
#define __DISPLAYMENU_H

#include <vdr/skins.h>
#include "setup.h"

#ifdef HAVE_IMAGEMAGICK
#ifdef HAVE_YAEPG_PATCH
class cBackgroundImage : public cThread {
private:
	virtual void Action(void);
	cTimeMs ShowTime;
public:
	cBackgroundImage(void);
	~cBackgroundImage();
    };
#endif
//for epgimages
class cEpgImage : public cThread {
private:
	virtual void Action(void);
	cTimeMs ShowTime;
public:
	cEpgImage(void);
	~cEpgImage();
    };
//
#endif

class cSkinElchiDisplayMenu : public cSkinDisplayMenu {
private:
  cOsd *osd;
  void Show(void);
//for epgimages
#ifdef HAVE_IMAGEMAGICK
  void ShowEpgImage(void);
//
#endif
  int roundness;
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;
  int y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;
  int lineHeight;
  int menuTop, menuHeight;
  int currentIndex;
  int area;
  bool epgimages;
  bool vidwin;
  bool bit;
  void SetScrollbar(void);

public:
  cSkinElchiDisplayMenu(void);
  virtual ~cSkinElchiDisplayMenu();
  virtual void Scroll(bool Up, bool Page);
  virtual int MaxItems(void);
  virtual void Clear(void);
  virtual void SetTitle(const char *Title);
  virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
  virtual void SetEvent(const cEvent *Event);
  virtual void SetRecording(const cRecording *Recording);
  virtual void SetText(const char *Text, bool FixedFont);
  virtual void Flush(void);

#ifdef HAVE_IMAGEMAGICK
#ifdef HAVE_YAEPG_PATCH
  cBackgroundImage *makeimage;
#endif
//for epgimages
  cEpgImage    *epgimage;
//
#endif

};

#endif //__DISPLAYMENU_H
