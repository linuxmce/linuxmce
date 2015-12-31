
#ifndef __XINEOSD_H
#define __XINEOSD_H



#include "xineCommon.h"

#if VDRVERSNUM < 10307
#include <vdr/osdbase.h>
#else
#include <vdr/osd.h>
#endif

#include <vdr/thread.h>



namespace PluginXine
{

  class cXineDevice;
  class cXineLib;
  
#if VDRVERSNUM < 10307
  class cXineOsd : public cOsdBase
#else
  class cXineOsd : public cOsd
#endif
  {
    cXineDevice &m_xineDevice;
    cXineLib &m_xineLib;
    cMutex &m_osdMutex;

#if VDRVERSNUM < 10307    
    bool m_windowVisible[ MAXNUMWINDOWS ];
    
    virtual bool OpenWindow(cWindow *Window);
    virtual void CommitWindow(cWindow *Window);
    virtual void ShowWindow(cWindow *Window);
    virtual void HideWindow(cWindow *Window, bool Hide);
    virtual void MoveWindow(cWindow *Window, int x, int y);
    virtual void CloseWindow(cWindow *Window);
#else
    virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);
    virtual void SaveRegion(int x1, int y1, int x2, int y2);
    virtual void RestoreRegion(void);
    virtual eOsdError SetPalette(const cPalette &Palette, int Area);
    virtual void DrawPixel(int x, int y, tColor Color);
#if VDRVERSNUM < 10327    
    virtual void DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0);
#else
    virtual void DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0, bool ReplacePalette = false);
#endif    
    virtual void DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width = 0, int Height = 0, int Alignment = taDefault);
    virtual void DrawRectangle(int x1, int y1, int x2, int y2, tColor Color);
    virtual void DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants = 0);
    virtual void DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type);    
    virtual void Flush(void);
#endif
    
  public:
    cXineOsd(cXineDevice &device, int x, int y);
    virtual ~cXineOsd();

    void ReshowCurrentOsd(const bool dontOptimize, const int frameLeft = -1, const int frameTop = -1, const int frameWidth = -1, const int frameHeight = -1);

    friend class cXineLib;
  };


  
#if VDRVERSNUM >= 10307
  
  class cXineOsdProvider : public cOsdProvider
  {
    cXineDevice &m_xineDevice;
    
  public:
    cXineOsdProvider(cXineDevice &xineDevice);
    virtual cOsd *CreateOsd(int Left, int Top);
  };

#endif

  
  
};



#endif //__XINEOSD_H
