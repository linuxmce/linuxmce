
#include "xineCommon.h"

#include "xineOsd.h"
#include "xineDevice.h"
#include "xineLib.h"



namespace PluginXine
{

#if VDRVERSNUM < 10307
  
  bool cXineOsd::OpenWindow(cWindow *Window)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    m_windowVisible[ Window->Handle() ] = false;
    
    return m_xineLib.OpenWindow(this, Window);
  }

  void cXineOsd::CommitWindow(cWindow *Window)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    m_xineLib.CommitWindow(this, Window);
  }

  void cXineOsd::ShowWindow(cWindow *Window)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    m_windowVisible[ Window->Handle() ] = true;
    
    m_xineLib.ShowWindow(this, Window);
  }

  void cXineOsd::HideWindow(cWindow *Window, bool Hide)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    m_windowVisible[ Window->Handle() ] = !Hide;
    
    m_xineLib.HideWindow(this, Window, Hide);
  }

  void cXineOsd::MoveWindow(cWindow *Window, int x, int y)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    m_xineLib.MoveWindow(this, Window, x, y);
  }

  void cXineOsd::CloseWindow(cWindow *Window)
  {
    cMutexLock osdLock(&m_osdMutex);
    
    m_windowVisible[ Window->Handle() ] = false;
    
    m_xineLib.CloseWindow(this, Window);
  }

#endif


  static int vl = -1, vt = -1, vw = -1, vh = -1;

  
  void cXineOsd::ReshowCurrentOsd(const bool dontOptimize, const int frameLeft /* = -1 */, const int frameTop /* = -1 */, const int frameWidth /* = -1 */, const int frameHeight /* = -1 */)
  {
    cMutexLock osdLock(&m_osdMutex);
    
#if VDRVERSNUM < 10307    

    (void)vl;
    (void)vt;
    (void)vw;
    (void)vh;

    for (int i = 0; i < NumWindows(); i++)
    {
      cWindow *const window = GetWindowNr(i);
      if (!window)
        continue;
      
      if (dontOptimize)
        m_xineLib.OpenWindow(this, window);
      
      m_xineLib.CommitWindow(this, window, !dontOptimize);

      if (m_windowVisible[ i ])
        Show(window->Handle());
      else
        Hide(window->Handle());
    }

#else

#ifdef SET_VIDEO_WINDOW
    
    m_xineLib.SetVideoWindow(vidWin, dontOptimize);
    
#endif    
    
    int videoLeft   = frameLeft;
    int videoTop    = frameTop;
    int videoWidth  = frameWidth;
    int videoHeight = frameHeight;

    if (frameLeft < 0
        || frameTop < 0
        || frameWidth < 0
        || frameHeight < 0)
    {
      m_xineLib.execFuncVideoSize(videoLeft, videoTop, videoWidth, videoHeight);
    }

//    ::fprintf(stderr, "frame: %d x %d, %d x %d\n", videoLeft, videoTop, videoWidth, videoHeight);

    vl = videoLeft;
    vt = videoTop;
    vw = videoWidth;
    vh = videoHeight;
    
    for (int i = 0; i < MAXNUMWINDOWS; i++)
      m_xineLib.SendWindow(this, i, GetBitmap(i), videoLeft, videoTop, videoWidth, videoHeight, dontOptimize);

#endif

    m_xineLib.execFuncOsdFlush();
  }

  cXineOsd::cXineOsd(cXineDevice &xineDevice, int x, int y)
#if VDRVERSNUM < 10307    
    : cOsdBase(x, y)
#else
    : cOsd(x, y)
#endif
    , m_xineDevice(xineDevice)
    , m_xineLib(xineDevice.m_xineLib)
    , m_osdMutex(xineDevice.m_osdMutex)
  {
#if VDRVERSNUM < 10307    

    ::memset(m_windowVisible, 0, sizeof (m_windowVisible));

#endif
  }

  cXineOsd::~cXineOsd()
  {
    cMutexLock osdLock(&m_osdMutex);
    
#if VDRVERSNUM < 10307    

    for (int i = 0; i < MAXNUMWINDOWS; i++)
      m_xineLib.CloseWindow(this, i);

#else

    tArea defaultWindow;
    defaultWindow.x1 = 0;
    defaultWindow.y1 = 0;
    defaultWindow.x2 = 0;
    defaultWindow.y2 = 0;
    defaultWindow.bpp = 0;

    m_xineLib.SetVideoWindow(defaultWindow);
    
    for (int i = 0; i < MAXNUMWINDOWS; i++)
      m_xineLib.SendWindow(this, i);
    
#endif    

    m_xineDevice.OnFreeOsd(this);

    m_xineLib.execFuncOsdFlush();

//    m_xineLib.execFuncGrabImage("/tmp/grab.jpg", true, 50, 1000, 1000);
  }


  
#if VDRVERSNUM >= 10307
  
  eOsdError cXineOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
  {
//    fprintf(stderr, "cXineOsd::CanHandleAreas(%p, %d)\n", Areas, NumAreas);

    const eOsdError retVal = cOsd::CanHandleAreas(Areas, NumAreas);
    if (oeOk != retVal)
      return retVal;

    for (int i = 0; i < NumAreas; i++)
    {
      const tArea &a = Areas[ i ];
/*      
      fprintf(stderr, "Areas[ %d ]: (%d,%d)-(%d,%d)@%d\n"
              , i
              , a.x1
              , a.y1
              , a.x2
              , a.y2
              , a.bpp);
*/      
      assert(a.x1 <= a.x2);
      assert(a.y1 <= a.y2);

      if (1 != a.bpp
          && 2 != a.bpp
          && 4 != a.bpp
          && 8 != a.bpp)
      {
        return oeBppNotSupported;
      }
    }

    return oeOk;
  }

  eOsdError cXineOsd::SetAreas(const tArea *Areas, int NumAreas)
  {
    cMutexLock osdLock(&m_osdMutex);
    return cOsd::SetAreas(Areas, NumAreas);
  }
  
  void cXineOsd::SaveRegion(int x1, int y1, int x2, int y2)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::SaveRegion(x1, y1, x2, y2);
  }
  
  void cXineOsd::RestoreRegion(void)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::RestoreRegion();
  }
  
  eOsdError cXineOsd::SetPalette(const cPalette &Palette, int Area)
  {
    cMutexLock osdLock(&m_osdMutex);
    return cOsd::SetPalette(Palette, Area);
  }
  
  void cXineOsd::DrawPixel(int x, int y, tColor Color)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawPixel(x, y, Color);
  }
  
#if VDRVERSNUM < 10327    
  void cXineOsd::DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg /* = 0 */, tColor ColorBg /* = 0 */)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawBitmap(x, y, Bitmap, ColorFg, ColorBg);
//    fprintf(stderr, "drawbitmap\n");
  }
#else  
  void cXineOsd::DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg /* = 0 */, tColor ColorBg /* = 0 */, bool ReplacePalette /* = false */)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawBitmap(x, y, Bitmap, ColorFg, ColorBg, ReplacePalette);
//    fprintf(stderr, "drawbitmap\n");
  }
#endif
  
  void cXineOsd::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width /* = 0 */, int Height /* = 0 */, int Alignment /* = taDefault */)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawText(x, y, s, ColorFg, ColorBg, Font, Width, Height, Alignment);
  }
  
  void cXineOsd::DrawRectangle(int x1, int y1, int x2, int y2, tColor Color)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawRectangle(x1, y1, x2, y2, Color);
  }
  
  void cXineOsd::DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants /* = 0 */)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawEllipse(x1, y1, x2, y2, Color, Quadrants);
  }
  
  void cXineOsd::DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type)
  {
    cMutexLock osdLock(&m_osdMutex);
    cOsd::DrawSlope(x1, y1, x2, y2, Color, Type);
  }
  
  void cXineOsd::Flush(void)
  {
//    ::fprintf(stderr, "Flush ---: %s\n", ::ctime(&(const time_t &)::time(0)));
    cMutexLock osdLock(&m_osdMutex);
//    ::fprintf(stderr, "Flush +++: %s\n", ::ctime(&(const time_t &)::time(0)));

//    static int cnt = 0;
//    fprintf(stderr, "cXineOsd::Flush() %d\n", cnt++);

#ifdef SET_VIDEO_WINDOW
    
    m_xineLib.SetVideoWindow(vidWin);

#endif    
    
    int videoLeft   = -1;
    int videoTop    = -1;
    int videoWidth  = -1;
    int videoHeight = -1;

    m_xineLib.execFuncVideoSize(videoLeft, videoTop, videoWidth, videoHeight);

    if (vl != videoLeft
        || vt != videoTop
        || vw != videoWidth
        || vh != videoHeight)
    {
//      ::fprintf(stderr, "video: %d x %d, %d x %d\n", videoLeft, videoTop, videoWidth, videoHeight);

      vl = videoLeft;
      vt = videoTop;
      vw = videoWidth;
      vh = videoHeight;
    }
    
    for (int i = 0; i < MAXNUMWINDOWS; i++)
      m_xineLib.SendWindow(this, i, GetBitmap(i), videoLeft, videoTop, videoWidth, videoHeight);

    m_xineLib.execFuncOsdFlush();
  }

  
  
  cXineOsdProvider::cXineOsdProvider(cXineDevice &xineDevice)
    : cOsdProvider()
    , m_xineDevice(xineDevice)
  {
  }
  
  cOsd *cXineOsdProvider::CreateOsd(int Left, int Top)
  {
    return m_xineDevice.NewOsd(Left, Top);
  }
  
#endif


  
};
