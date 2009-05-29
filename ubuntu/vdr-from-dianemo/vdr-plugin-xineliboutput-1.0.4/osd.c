/*
 * osd.c: Xinelib On Screen Display control
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: osd.c,v 1.24.2.1 2008/09/26 13:17:43 phintuka Exp $
 *
 */

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/thread.h>

#include "logdefs.h"
#include "device.h"
#include "config.h"
#include "xine_osd_command.h"

#include "osd.h"

//#define LIMIT_OSD_REFRESH_RATE

#define LOGOSD(x...)

//
// tools
//

static inline int saturate(int x, int min, int max)
{
  return x < min ? min : (x > max ? max : x);
}

static inline void prepare_palette(xine_clut_t *clut, const unsigned int *palette, int colors, bool top, bool rgb)
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
	  clut[c].alpha = (clut[c].alpha >> 1) | 0x80;
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

static int rle_compress(xine_rle_elem_t **rle_data, const uint8_t *data, int w, int h)
{
  xine_rle_elem_t rle, *rle_p = 0, *rle_base;
  int x, y, num_rle = 0, rle_size = 8128;
  const uint8_t *c;

  rle_p = (xine_rle_elem_t*)malloc(4*rle_size);
  rle_base = rle_p;

  for( y = 0; y < h; y++ ) {
    rle.len = 0;
    rle.color = 0;
    c = data + y * w;
    for( x = 0; x < w; x++, c++ ) {
      if( rle.color != *c ) {
	if( rle.len ) {
	  if( (num_rle + h-y+1) > rle_size ) {
	    rle_size *= 2;
	    rle_base = (xine_rle_elem_t*)realloc( rle_base, 4*rle_size );
	    rle_p = rle_base + num_rle;
	  }
	  *rle_p++ = rle;
	  num_rle++;
	}
	rle.color = *c;
	rle.len = 1;
      } else {
	rle.len++;
      }
    }
    *rle_p++ = rle;
    num_rle++;
  }

  TRACE("xinelib_osd.c:CmdRle uncompressed="<< (w*h) <<", compressed=" << (4*num_rle));

  *rle_data = rle_base;
  return num_rle;
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
    void CmdRle(int Wnd, int X0, int Y0,
		int W, int H, unsigned char *Data,
		int Colors, unsigned int *Palette, 
		osd_rect_t *DirtyArea);
    void CmdClose(int Wnd);

  protected:
    static cMutex             m_Lock;
    static cList<cXinelibOsd> m_OsdStack;

    bool   m_IsVisible;
    bool   m_Refresh;
    uint   m_Layer;

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


void cXinelibOsd::CmdSize(int Width, int Height)
{
  TRACEF("cXinelibOsd::CmdSize");

  if(m_Device) {
    osd_command_t osdcmd;
    memset(&osdcmd,0,sizeof(osdcmd));

    osdcmd.cmd = OSD_Size;
    osdcmd.w = Width;
    osdcmd.h = Height;

    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::CmdClose(int Wnd)
{
  TRACEF("cXinelibOsd::CmdClose");

  if(m_Device) {
    osd_command_t osdcmd;
    memset(&osdcmd,0,sizeof(osdcmd));

    osdcmd.cmd = OSD_Close;
    osdcmd.wnd = Wnd;

    if(m_Refresh)
      osdcmd.flags |= OSDFLAG_REFRESH;
    
    m_Device->OsdCmd((void*)&osdcmd);
  }
}

void cXinelibOsd::CmdRle(int Wnd, int X0, int Y0, 
			 int W, int H, unsigned char *Data,
			 int Colors, unsigned int *Palette, 
			 osd_rect_t *DirtyArea)
{
  TRACEF("cXinelibOsd::CmdRle");

  if(m_Device) {

    osd_command_t osdcmd;
    xine_clut_t clut[256];

    memset(&osdcmd, 0, sizeof(osdcmd));
    osdcmd.cmd = OSD_Set_RLE;
    osdcmd.wnd = Wnd;
    osdcmd.x = X0;
    osdcmd.y = Y0;
    osdcmd.w = W;
    osdcmd.h = H;
    if(DirtyArea)
      memcpy(&osdcmd.dirty_area, DirtyArea, sizeof(osd_rect_t));
    if(m_Refresh)
      osdcmd.flags |= OSDFLAG_REFRESH;
    if(xc.osd_blending == OSD_BLENDING_HARDWARE)
      osdcmd.flags |= OSDFLAG_UNSCALED;
    if(xc.osd_blending_lowresvideo == OSD_BLENDING_HARDWARE)
      osdcmd.flags |= OSDFLAG_UNSCALED_LOWRES;

    prepare_palette(&clut[0], Palette, Colors, /*Top*/(Prev() == NULL), true);
    osdcmd.colors = Colors;
    osdcmd.palette = clut;

    osdcmd.num_rle = rle_compress(&osdcmd.data, Data, W, H);
    osdcmd.datalen = 4 * osdcmd.num_rle;

    osdcmd.scaling = xc.osd_scaling;
    
    m_Device->OsdCmd((void*)&osdcmd);

    if(osdcmd.data)
      free(osdcmd.data);
  }
}

cXinelibOsd::cXinelibOsd(cXinelibDevice *Device, int x, int y, uint Level)
#if VDRVERSNUM >= 10509
    : cOsd(x, y, Level)
#else
    : cOsd(x, y)
#endif
{
  TRACEF("cXinelibOsd::cXinelibOsd");

  m_Device = Device;
  m_Refresh = false;
  m_IsVisible = true;
  m_Layer = Level;
}

cXinelibOsd::~cXinelibOsd()
{
  TRACEF("cXinelibOsd::~cXinelibOsd");

  cMutexLock ml(&m_Lock);

  CloseWindows();

  m_OsdStack.Del(this,false);

  if(m_OsdStack.First())
    m_OsdStack.First()->Show();
}

eOsdError cXinelibOsd::SetAreas(const tArea *Areas, int NumAreas)
{
  TRACEF("cXinelibOsd::SetAreas");
  cMutexLock ml(&m_Lock);

  LOGOSD("cXinelibOsd::SetAreas");

  CloseWindows();

  eOsdError Result = cOsd::SetAreas(Areas, NumAreas);

  if(Left() + Width() > 720 || Top() + Height() > 576) {
    LOGDBG("Detected HD OSD, size > %dx%d, using setup values %dx%d", 
           2*Left() + Width(), 2*Top() + Height(),
           Setup.OSDWidth + (2*Setup.OSDLeft), Setup.OSDHeight + (2*Setup.OSDTop));
    CmdSize(Setup.OSDWidth + (2*Setup.OSDLeft), Setup.OSDHeight + (2*Setup.OSDTop));
  } else {
    CmdSize(720, 576);
  }

  return Result;
}

eOsdError cXinelibOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
{
  TRACEF("cXinelibOsd::CanHandleAreas");

  eOsdError Result = cOsd::CanHandleAreas(Areas, NumAreas);
  if (Result == oeOk) {
    if (NumAreas > MAX_OSD_OBJECT)
      return oeTooManyAreas;
    for (int i = 0; i < NumAreas; i++) {
      if (Areas[i].bpp != 1 && Areas[i].bpp != 2 && 
	  Areas[i].bpp != 4 && Areas[i].bpp != 8)
        return oeBppNotSupported;
    }
  }
  return Result;
}

void cXinelibOsd::Flush(void)
{
  TRACEF("cXinelibOsd::Flush");

  cMutexLock ml(&m_Lock);

  cBitmap *Bitmap;

  if(!m_IsVisible) 
    return;

  int SendDone = 0;
  for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) {
    int x1 = 0, y1 = 0, x2 = Bitmap->Width()-1, y2 = Bitmap->Height()-1;
    if (m_Refresh || Bitmap->Dirty(x1, y1, x2, y2)) {

      /* XXX what if only palette has been changed ? */
      int NumColors;
      const tColor *Colors = Bitmap->Colors(NumColors);
      if (Colors) {
	osd_rect_t DirtyArea = {x1:x1, y1:y1, x2:x2, y2:y2};
	CmdRle(i,
	       Left() + Bitmap->X0(), Top() + Bitmap->Y0(),
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

  if(m_IsVisible) {
    cBitmap *Bitmap;
    for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) {
      LOGOSD("Close OSD %d.%d", Index(), i);
      CmdClose(i);
    }
  }
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

cOsd *cXinelibOsdProvider::CreateOsd(int Left, int Top, uint Level)
{
  TRACEF("cXinelibOsdProvider::CreateOsd");

  cMutexLock ml(&cXinelibOsd::m_Lock);

#if VDRVERSNUM < 10509
  if(cXinelibOsd::m_OsdStack.First())
    LOGMSG("cXinelibOsdProvider::CreateOsd - OSD already open !");
#endif

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

  LOGOSD("New OSD: index %d, layer %d [now %d OSDs]", m_OsdInstance->Index(), Level, cXinelibOsd::m_OsdStack.Count());
  if(xc.osd_mixer == OSD_MIXER_NONE)
    LOGOSD(" OSD mixer off");

  // hide all but top-most OSD  
  it = cXinelibOsd::m_OsdStack.Last();
  while(cXinelibOsd::m_OsdStack.Prev(it)) {
    LOGOSD(" -> hide OSD %d", it->Index());
    it->Hide();
    it = cXinelibOsd::m_OsdStack.Prev(it);
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



