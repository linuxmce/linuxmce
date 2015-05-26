/*
 * osd.c: Xinelib On Screen Display control
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: osd.c,v 1.54 2015-02-20 16:04:16 phintuka Exp $
 *
 */

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/thread.h>

#include "logdefs.h"
#include "config.h"
#include "device.h"

#include "tools/osd_command.h"

#include "osd.h"

#ifndef OSD_LEVEL_TTXTSUBS
#define OSD_LEVEL_TTXTSUBS 20 // from ttxtsubs plugin
#endif

//#define LIMIT_OSD_REFRESH_RATE

#define LOGOSD(x...)

//
// tools
//

static inline int saturate(int x, int min, int max)
{
  return x < min ? min : (x > max ? max : x);
}

static inline void prepare_palette(osd_clut_t *clut, const unsigned int *palette, int colors, bool top, bool rgb)
{
  if (colors) {
    int c;

    // Apply alpha layer correction and convert ARGB -> AYCrCb

    for(c=0; c<colors; c++) {
      int A = (palette[c] & 0xff000000) >> 24;
      int R = (palette[c] & 0x00ff0000) >> 16;
      int G = (palette[c] & 0x0000ff00) >>  8;
      int B = (palette[c] & 0x000000ff);
      A = A + xc.alpha_correction*A/100 + xc.alpha_correction_abs;
      if(rgb) {
        clut[c].r     = R;
        clut[c].g     = G;
        clut[c].b     = B;
	clut[c].alpha = saturate( A,  0, 255);
      } else {
        int Y         = (( +  66*R + 129*G +  25*B + 0x80) >> 8) +  16;
        int CR        = (( + 112*R -  94*G -  18*B + 0x80) >> 8) + 128;
        int CB        = (( -  38*R -  74*G + 112*B + 0x80) >> 8) + 128;
        clut[c].y     = saturate( Y, 16, 235);
        clut[c].cb    = saturate(CB, 16, 240);
        clut[c].cr    = saturate(CR, 16, 240);
        clut[c].alpha = saturate( A,  0, 255);
      }
    }

    // Apply OSD mixer settings

    if(!top) {
      if(xc.osd_mixer & OSD_MIXER_ALPHA)
	for(c=0; c<colors; c++)
	  clut[c].alpha >>= 1; /* fade */
      if(xc.osd_mixer & OSD_MIXER_GRAY)
	for(c=0; c<colors; c++) {
	  if(rgb)
	    clut[c].r = clut[c].g = clut[c].b = (clut[c].r + clut[c].g + clut[c].b)/3;
	  else
	    clut[c].cb = clut[c].cr = 0x80;
	}
    }
  }
}

//
// cXinelibOsd
//

class cXinelibOsd : public cOsd, public cListObject 
{
  private:
    cXinelibOsd();
    cXinelibOsd(cXinelibOsd&); // no copy

    cXinelibDevice *m_Device;

    void CloseWindows(void);
    void CmdSize(int Width, int Height);
    void CmdVideoWindow(int X, int Y, int W, int H);
    void CmdArgb(int X, int Y, int W, int H,
                 const unsigned char *Data, int DataLen);
    void CmdLut8(int Wnd, int X0, int Y0,
                 int W, int H, unsigned char *Data,
                 int Colors, unsigned int *Palette,
                 osd_rect_t *DirtyArea);
    void CmdPalette(int Wnd, int Colors, unsigned int *Palette);
    void CmdMove(int Wnd, int Width, int Height);
    void CmdClose(int Wnd);
    void CmdFlush(void);

    void SetCmdFlags(osd_command_t& cmd);

    /* map single OSD window indexes to unique xine-side window handles */
    static uint64_t  m_HandlesBitmap;
    int             *m_WindowHandles;
    int  AllocWindowHandles(int NumWindows);
    void FreeWindowHandles(void);

  protected:
    static cMutex             m_Lock;
    static cList<cXinelibOsd> m_OsdStack;

    bool     m_IsVisible;
    bool     m_Refresh;
    uint     m_Layer;
    uint16_t m_ExtentWidth;
    uint16_t m_ExtentHeight;

    virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);
    virtual void Flush(void);

    // Messages from cXinelibOsdProvider
    void Show(void);
    void Hide(void);
    void Refresh(void);
    void Detach(void);

    friend class cXinelibOsdProvider;

  public:
    cXinelibOsd(cXinelibDevice *Device, int x, int y, uint Level = 0);
    virtual ~cXinelibOsd();
};

cList<cXinelibOsd> cXinelibOsd::m_OsdStack;
cMutex             cXinelibOsd::m_Lock;
uint64_t           cXinelibOsd::m_HandlesBitmap;

int cXinelibOsd::AllocWindowHandles(int NumWindows)
{
  uint64_t bit = 1;
  int index = 0, wnd = 0;

  FreeWindowHandles();
  m_WindowHandles = new int[NumWindows+1];

  for (index = 0; index < MAX_OSD_OBJECT; index++) {
    if (! (m_HandlesBitmap & bit)) {
      m_WindowHandles[wnd++] = index;
      m_HandlesBitmap |= bit;
    }
    if (wnd >= NumWindows)
      break;
    bit <<= 1;
  }
  m_WindowHandles[NumWindows] = -1;

  if (wnd < NumWindows) {
    LOGMSG("cXinelibOsd::AllocOsdHandles(): Too many open OSD windows !");
    while(wnd < NumWindows) m_WindowHandles[wnd++] = -1;
    return 0;
  }

  return NumWindows;
}

void cXinelibOsd::FreeWindowHandles(void)
{
  if (m_WindowHandles) {
    int wnd = 0;
    while (m_WindowHandles[wnd] >= 0) {
      m_HandlesBitmap &= ~( ((uint64_t)1) << m_WindowHandles[wnd]);
      wnd++;
    }
    delete [] m_WindowHandles;
    m_WindowHandles = NULL;
  }
}

void cXinelibOsd::CmdSize(int Width, int Height)
{
  TRACEF("cXinelibOsd::CmdSize");

  if (m_Device && m_WindowHandles) {
    osd_command_t osdcmd = {0};

    osdcmd.cmd = OSD_Size;
    osdcmd.w   = Width;
    osdcmd.h   = Height;

    if (Prev() == NULL)
      osdcmd.flags |= OSDFLAG_TOP_LAYER;

    for (int Wnd = 0; m_WindowHandles[Wnd] >= 0; Wnd++) {
      osdcmd.wnd = m_WindowHandles[Wnd];

      m_Device->OsdCmd((void*)&osdcmd);
    }
  }
}

void cXinelibOsd::CmdVideoWindow(int X, int Y, int W, int H)
{
  TRACEF("cXinelibOsd::CmdVideoWindow");

  if (m_Device && m_WindowHandles) {
    osd_command_t osdcmd = {0};

    for (int Wnd = 0; m_WindowHandles[Wnd] >= 0; Wnd++) {
      osdcmd.cmd = OSD_VideoWindow;
      osdcmd.wnd = m_WindowHandles[Wnd];
      osdcmd.x   = X;
      osdcmd.y   = Y;
      osdcmd.w   = W;
      osdcmd.h   = H;

      m_Device->OsdCmd((void*)&osdcmd);
    }
  }
}

void cXinelibOsd::CmdMove(int Wnd, int NewX, int NewY)
{
  TRACEF("cXinelibOsd::CmdMove");

  if (m_Device && m_WindowHandles) {
    osd_command_t osdcmd = {0};

    osdcmd.cmd = OSD_Move;
    osdcmd.wnd = m_WindowHandles[Wnd];
    osdcmd.x   = NewX;
    osdcmd.y   = NewY;

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::CmdPalette(int Wnd, int Colors, unsigned int *Palette)
{
  TRACEF("cXinelibOsd::CmdPalette");

  if (m_Device) {
    osd_clut_t    clut[Colors];
    osd_command_t osdcmd = {0};

    osdcmd.cmd     = OSD_SetPalette;
    osdcmd.wnd     = m_WindowHandles[Wnd];
    osdcmd.palette = clut;
    osdcmd.colors  = Colors;

    prepare_palette(&clut[0], Palette, Colors, /*Top*/(Prev() == NULL), true);

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::CmdClose(int Wnd)
{
  TRACEF("cXinelibOsd::CmdClose");

  if (m_Device) {
    osd_command_t osdcmd = {0};

    osdcmd.cmd = OSD_Close;
    osdcmd.wnd = m_WindowHandles[Wnd];

    if (m_Refresh)
      osdcmd.flags |= OSDFLAG_REFRESH;
    if (Prev() == NULL)
      osdcmd.flags |= OSDFLAG_TOP_LAYER;

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::CmdFlush(void)
{
  TRACEF("cXinelibOsd::CmdFlush");

  if (m_Device) {
    osd_command_t osdcmd = {0};

    osdcmd.cmd = OSD_Flush;

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::SetCmdFlags(osd_command_t& cmd)
{
  if (m_Refresh)
    cmd.flags |= OSDFLAG_REFRESH;
  if (xc.osd_blending == OSD_BLENDING_HARDWARE)
    cmd.flags |= OSDFLAG_UNSCALED;
  if (xc.osd_blending_lowresvideo == OSD_BLENDING_HARDWARE)
    cmd.flags |= OSDFLAG_UNSCALED_LOWRES;
  if (Prev() == NULL)
    cmd.flags |= OSDFLAG_TOP_LAYER;

  cmd.scaling = xc.osd_scaling;

  if (m_Layer == OSD_LEVEL_TTXTSUBS)
    cmd.scaling = xc.osd_spu_scaling;
}

void cXinelibOsd::CmdArgb(int X, int Y, int W, int H,
                          const unsigned char *Data, int DataLen)
{
  TRACEF("cXinelibOsd::CmdArgb");

  if (m_Device) {

    osd_command_t osdcmd = {0};

    osdcmd.cmd   = OSD_Set_ARGB;
    osdcmd.wnd   = m_WindowHandles[0];
    osdcmd.layer = saturate(m_Layer, 0, 0xffff);
    osdcmd.x     = X;
    osdcmd.y     = Y;
    osdcmd.w     = W;
    osdcmd.h     = H;

    osdcmd.dirty_area.x2 = W - 1;
    osdcmd.dirty_area.y2 = H - 1;

    SetCmdFlags(osdcmd);

    osdcmd.raw_data = (uint8_t*)Data;
    osdcmd.num_rle  = 0;
    osdcmd.datalen  = DataLen;

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::CmdLut8(int Wnd, int X0, int Y0,
                          int W, int H, unsigned char *Data,
                          int Colors, unsigned int *Palette,
                          osd_rect_t *DirtyArea)
{
  TRACEF("cXinelibOsd::CmdLut8");

  if (m_Device) {

    osd_clut_t    clut[Colors];
    osd_command_t osdcmd = {0};

    osdcmd.cmd   = OSD_Set_LUT8;
    osdcmd.wnd   = m_WindowHandles[Wnd];
    osdcmd.layer = saturate(m_Layer, 0, 0xffff);
    osdcmd.x   = X0;
    osdcmd.y   = Y0;
    osdcmd.w   = W;
    osdcmd.h   = H;
    osdcmd.colors  = Colors;
    osdcmd.palette = clut;

    if (DirtyArea)
      memcpy(&osdcmd.dirty_area, DirtyArea, sizeof(osd_rect_t));

    SetCmdFlags(osdcmd);

    prepare_palette(&clut[0], Palette, Colors, /*Top*/(Prev() == NULL), true);

    osdcmd.raw_data = Data;
    osdcmd.num_rle  = 0;
    osdcmd.datalen  = W * H;

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

cXinelibOsd::cXinelibOsd(cXinelibDevice *Device, int x, int y, uint Level)
    : cOsd(x, y, Level)
{
  TRACEF("cXinelibOsd::cXinelibOsd");

  m_Device       = Device;
  m_Refresh      = false;
  m_IsVisible    = true;
  m_Layer        = Level;
  m_ExtentWidth  = 720;
  m_ExtentHeight = 576;

  m_WindowHandles = NULL;
}

cXinelibOsd::~cXinelibOsd()
{
  TRACEF("cXinelibOsd::~cXinelibOsd");

  cMutexLock ml(&m_Lock);

  CloseWindows();
  FreeWindowHandles();

  m_OsdStack.Del(this, false);

  if(m_OsdStack.First())
    m_OsdStack.First()->Show();
}

eOsdError cXinelibOsd::SetAreas(const tArea *Areas, int NumAreas)
{
  TRACEF("cXinelibOsd::SetAreas");
  cMutexLock ml(&m_Lock);

  LOGOSD("cXinelibOsd::SetAreas");

  // Close all existing windows
  CloseWindows();
  FreeWindowHandles();

  eOsdError Result = cOsd::SetAreas(Areas, NumAreas);

  if (Result != oeOk)
    return Result;

  // Allocate xine OSD window handles
  if (!AllocWindowHandles(NumAreas)) {
    FreeWindowHandles();
    return oeTooManyAreas;
  }

  if (xc.osd_spu_scaling && m_Layer == OSD_LEVEL_TTXTSUBS) {
    m_ExtentWidth  = 720;
    m_ExtentHeight = 576;
  } else {
    double Aspect;
    int    W, H;
    m_Device->GetOsdSize(W, H, Aspect);
    m_ExtentWidth  = W;
    m_ExtentHeight = H;
  }

  CmdSize(m_ExtentWidth, m_ExtentHeight);

  return Result;
}

eOsdError cXinelibOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
{
  TRACEF("cXinelibOsd::CanHandleAreas");

  eOsdError Result = cOsd::CanHandleAreas(Areas, NumAreas);

  if (Result != oeOk)
    return Result;

  if (NumAreas > MAX_OSD_OBJECT)
    return oeTooManyAreas;

  // bpp

  if (NumAreas == 1 && Areas[0].bpp == 32) {
    if (!m_Device->SupportsTrueColorOSD()) {
      LOGDBG("cXinelibOsd::CanHandleAreas(): Device does not support ARGB");
      return oeBppNotSupported;
    }

  } else {

    for (int i = 0; i < NumAreas; i++) {
      if (Areas[i].bpp < 1 || Areas[i].bpp > 8) {
        LOGMSG("cXinelibOsd::CanHandleAreas(): invalid bpp (%d)", Areas[i].bpp);
        return oeBppNotSupported;
      }
    }
  }

  // enough free xine OSD windows ?
  uint64_t bit     = 1;
  int      windows = NumAreas;
  for (int index = 0; index < MAX_OSD_OBJECT && windows > 0; index++) {
    if (! (m_HandlesBitmap & bit))
      windows--;
    bit <<= 1;
  }
  if (windows > 0) {
    LOGMSG("cXinelibOsd::CanHandleAreas(): not enough free window handles !");
    return oeTooManyAreas;
  }

  return oeOk;
}

void cXinelibOsd::Flush(void)
{
  TRACEF("cXinelibOsd::Flush");

  cMutexLock ml(&m_Lock);

  cBitmap *Bitmap;

  if(!m_IsVisible)
    return;

  const cRect& videoWindow = m_Device->GetVideoWindow();
  if (videoWindow != cRect::Null) {
    CmdVideoWindow(videoWindow.X(), videoWindow.Y(), videoWindow.Width(), videoWindow.Height());
  }

  if (IsTrueColor()) {

    LOCK_PIXMAPS;
    while (cPixmapMemory *pm = dynamic_cast<cPixmapMemory*>(RenderPixmaps())) {
      int w = pm->ViewPort().Width();
      int h = pm->ViewPort().Height();
      int d = w * sizeof(tColor);
      CmdArgb(Left() + pm->ViewPort().X(), Top() + pm->ViewPort().Y(), w, h, pm->Data(), h * d);
#if VDRVERSNUM >= 20200
      DestroyPixmap(pm);
#else
      delete pm;
#endif
    }

    return;
  }

  int SendDone = 0, XOffset = 0, YOffset = 0;

  if (!xc.osd_spu_scaling && m_Layer == OSD_LEVEL_TTXTSUBS) {
    double Aspect;
    int    W, H;
    m_Device->GetOsdSize(W, H, Aspect);
    YOffset = (H - 576) > 0 ? (H - 576) : 0;
    XOffset = ((W - 720) / 2) ? ((W - 720) / 2) : 0;
  }

  for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) {
    int x1 = 0, y1 = 0, x2 = x1+Bitmap->Width()-1, y2 = y1+Bitmap->Height()-1;
    if (m_Refresh || Bitmap->Dirty(x1, y1, x2, y2)) {

      /* XXX what if only palette has been changed ? */
      int NumColors;
      const tColor *Colors = Bitmap->Colors(NumColors);
      if (Colors) {
	osd_rect_t DirtyArea = {x1:(uint16_t)x1, y1:(uint16_t)y1, x2:(uint16_t)x2, y2:(uint16_t)y2};
        CmdLut8(i,
                Left() + Bitmap->X0() + XOffset, Top() + Bitmap->Y0() + YOffset,
                Bitmap->Width(), Bitmap->Height(),
                (unsigned char *)Bitmap->Data(0,0),
                NumColors, (unsigned int *)Colors,
                &DirtyArea);
	SendDone++;
      }
    }
    Bitmap->Clean();
  }

#ifdef LIMIT_OSD_REFRESH_RATE
  if(SendDone) {
    static int64_t last_refresh = 0LL;
    int64_t now = cTimeMs::Now();
    if(now - last_refresh < 100) {
      /* too fast refresh rate, delay ... */
      cCondWait::SleepMs(40); /* Can't update faster anyway ... */
# if 0
      LOGDBG("cXinelibOsd::Flush: OSD refreshing too fast ! (>10Hz) -> Sleeping 50ms");
# endif
    }
    last_refresh = now;
  }
#endif
}

void cXinelibOsd::Refresh(void)
{
  TRACEF("cXinelibOsd::Refresh");

  cMutexLock ml(&m_Lock);

  m_Refresh = true;
  CloseWindows();
  CmdSize(m_ExtentWidth, m_ExtentHeight);
  Flush();
  m_Refresh = false;
}

void cXinelibOsd::Show(void)
{
  TRACEF("cXinelibOsd::Show");

  cMutexLock ml(&m_Lock);

  m_IsVisible = true;
  Refresh();
}

void cXinelibOsd::CloseWindows(void)
{
  TRACEF("cXinelibOsd::CloseWindows");

  if (m_IsVisible && m_WindowHandles) {
    for (int i = 0; m_WindowHandles[i] >= 0; i++) {
      LOGOSD("Close OSD %d.%d", Index(), i);
      CmdClose(i);
    }
  }

  if (!m_Refresh)
    CmdFlush();
}

void cXinelibOsd::Hide(void)
{
  TRACEF("cXinelibOsd::Hide");

  cMutexLock ml(&m_Lock);

  CloseWindows();
  m_IsVisible = false;
}

void cXinelibOsd::Detach(void)
{
  TRACEF("cXinelibOsd::Detach");

  cMutexLock ml(&m_Lock);

  Hide();
  m_Device = NULL;
}

//
// cXinelibOsdProvider
//

cXinelibOsdProvider::cXinelibOsdProvider(cXinelibDevice *Device)
{
  m_Device = Device;
}

cXinelibOsdProvider::~cXinelibOsdProvider()
{
  LOGMSG("cXinelibOsdProvider: shutting down !");

  cMutexLock ml(&cXinelibOsd::m_Lock);

  m_Device = NULL;

  if(cXinelibOsd::m_OsdStack.First()) {
    LOGMSG("cXinelibOsdProvider: OSD open while OSD provider shutting down !");

    // Detach all OSD instances from device
    cXinelibOsd *osd;
    while(NULL != (osd = cXinelibOsd::m_OsdStack.First())) {
      osd->Detach();
      cXinelibOsd::m_OsdStack.Del(osd, false);
    }
  }
}

bool cXinelibOsdProvider::ProvidesTrueColor(void)
{
  return m_Device && m_Device->SupportsTrueColorOSD();
}

cOsd *cXinelibOsdProvider::CreateOsd(int Left, int Top, uint Level)
{
  TRACEF("cXinelibOsdProvider::CreateOsd");

  cMutexLock ml(&cXinelibOsd::m_Lock);

  cXinelibOsd *m_OsdInstance = new cXinelibOsd(m_Device, Left, Top, Level);

  // sorted insert
  cXinelibOsd *it = cXinelibOsd::m_OsdStack.First();
  while(it) {
    if(it->m_Layer >= Level) {
      cXinelibOsd::m_OsdStack.Ins(m_OsdInstance, it);
      break;
    }
    it = cXinelibOsd::m_OsdStack.Next(it);
  }
  if(!it)
    cXinelibOsd::m_OsdStack.Add(m_OsdInstance);

  LOGOSD("New OSD: index %d, layer %d [now %d OSDs]",
         m_OsdInstance->Index(), Level, cXinelibOsd::m_OsdStack.Count());

  if(xc.osd_mixer == OSD_MIXER_NONE) {
    // hide all but top-most OSD
    LOGOSD(" OSD mixer off");
    it = cXinelibOsd::m_OsdStack.Last();
    while(cXinelibOsd::m_OsdStack.Prev(it)) {
      LOGOSD(" -> hide OSD %d", it->Index());
      it->Hide();
      it = cXinelibOsd::m_OsdStack.Prev(it);
    }

  } else /*if(xc.osd_mixer > OSD_MIXER_NONE)*/ {
    LOGOSD("OSD mixer on (%d)", xc.osd_mixer);
    it = cXinelibOsd::m_OsdStack.Last();
    while (cXinelibOsd::m_OsdStack.Prev(it)) {
      LOGOSD(" -> show OSD %d", it->Index());
      it->Show();
      it = cXinelibOsd::m_OsdStack.Prev(it);
    }
  }

  it->Show();

  return m_OsdInstance;
}

void cXinelibOsdProvider::RefreshOsd(void)
{
  TRACEF("cXinelibOsdProvider::RefreshOsd");

  cMutexLock ml(&cXinelibOsd::m_Lock);

  // bottom --> top (draw lower layer OSDs first)
  cXinelibOsd *it = cXinelibOsd::m_OsdStack.Last();
  while(it) {
    it->Refresh();
    it = cXinelibOsd::m_OsdStack.Prev(it);
  }
}



