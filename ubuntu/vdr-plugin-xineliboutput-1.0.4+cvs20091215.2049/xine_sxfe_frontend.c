/*
 * xine_sxfe_frontend.c: Simple front-end, X11 functions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_sxfe_frontend.c,v 1.116 2009/10/31 19:35:45 phintuka Exp $
 *
 */

/*#define HAVE_XF86VIDMODE*/
#include "features.h"

#include <poll.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>

/* X11 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#ifdef HAVE_XSHM
# include <X11/extensions/XShm.h>
#endif
#ifdef HAVE_XRENDER
#  include <X11/extensions/Xrender.h>
#endif
#ifdef HAVE_XF86VIDMODE
#  include <X11/extensions/xf86vmode.h>
#endif
#ifdef HAVE_XDPMS
#  include <X11/extensions/dpms.h>
#endif
#ifdef HAVE_XINERAMA
#  include <X11/extensions/Xinerama.h>
#endif

#include <xine.h>

#define LOG_MODULENAME "[vdr-sxfe]  "
#include "logdefs.h"

#include "xine_osd_command.h"
#include "xine_frontend_internal.h"

#ifdef HAVE_DBUS_GLIB_1
#  include "tools/gnome_screensaver.h"
#endif

#ifndef WIN_LAYER_NORMAL
  #define WIN_LAYER_NORMAL 4
#endif
#ifndef WIN_LAYER_ONTOP
  #define WIN_LAYER_ONTOP  6
#endif

#define MWM_HINTS_DECORATIONS       (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS     5
typedef struct _mwmhints {
  uint32_t     flags;
  uint32_t     functions;
  uint32_t     decorations;
  int32_t      input_mode;
  uint32_t     status;
} MWMHints;

#ifdef HAVE_XRENDER
/* HUD Scaling */
typedef struct _xrender_surf
{
  Visual   *vis;
  Drawable  draw;
  Picture   pic;
  uint16_t  w, h;
  uint8_t   depth;
  uint8_t   allocated : 1;
} Xrender_Surf;
#endif /* HAVE_XRENDER */

/*
 * data
 */

typedef struct sxfe_s {

  /* function pointers / base class */
  union {
    frontend_t fe;  /* generic frontend */
    fe_t       x;   /* xine frontend */
  };

  /* stored original handlers */
  int (*fe_xine_open)(frontend_t *this_gen, const char *mrl);
  int (*fe_xine_play)(frontend_t *this_gen);


  /* X11 */
  Display *display;
  Window   window[2];
  int      screen;
  int      window_id;        /* output to another window */
  int      completion_event;
#ifdef HAVE_XF86VIDMODE
  int      XF86_modelines_count;
  XF86VidModeModeInfo**  XF86_modelines;
#endif
  Time     prev_click_time; /* time of previous mouse button click (grab double clicks) */
#ifdef HAVE_XDPMS
  BOOL     dpms_state;
#endif

  /* Atoms */
  Atom     xa_SXFE_INTERRUPT;
  Atom     xa_WM_DELETE_WINDOW;
  Atom     xa_MOTIF_WM_HINTS;
  Atom     xa_WIN_LAYER;
  Atom     xa_WIN_STATE;
  Atom     xa_NET_ACTIVE_WINDOW;
  Atom     xa_NET_WM_STATE;
  Atom     xa_NET_WM_STATE_ADD;
  Atom     xa_NET_WM_STATE_DEL;
  Atom     xa_NET_WM_STATE_ABOVE;
  Atom     xa_NET_WM_STATE_STICKY;
  Atom     xa_NET_WM_STATE_FULLSCREEN;
  Atom     xa_NET_WM_STATE_STAYS_ON_TOP;

  int      xinerama_screen; /* current xinerama screen, -1 = auto */
  uint16_t xinerama_x;      /* left-top position of current xinerama screen */
  uint16_t xinerama_y;
  uint16_t origwidth;       /* saved window-mode window size */
  uint16_t origheight;
  uint16_t origxpos;        /* saved window-mode window position */
  uint16_t origypos;
  uint16_t dragging_x;
  uint16_t dragging_y;

  uint8_t  fullscreen : 1;
/*uint8_t  vmode_switch : 1;*/
  uint8_t  fullscreen_state_forced : 1;
  uint8_t  stay_above : 1;
  uint8_t  no_border : 1;
  uint8_t  check_move : 1;
  uint8_t  dragging : 1;
  uint8_t  hud : 1;
  uint8_t  gui_hotkeys : 1;
  uint8_t  no_x_kbd : 1;

  /* HUD stuff */
#ifdef HAVE_XRENDER
  XImage         *hud_img;
  Visual         *hud_vis;
  Xrender_Surf   *surf_win;
  Xrender_Surf   *surf_img;
  uint32_t       *hud_img_mem;
  GC              gc;
  Window          hud_window;
# ifdef HAVE_XSHM
  XShmSegmentInfo hud_shminfo;
# endif

  uint16_t      osd_width;
  uint16_t      osd_height;
#endif /* HAVE_XRENDER */

} sxfe_t;


#define DOUBLECLICK_TIME   500  // ms

#define OSD_DEF_WIDTH      720
#define OSD_DEF_HEIGHT     576
#define HUD_MAX_WIDTH      1920
#define HUD_MAX_HEIGHT     1080

static void sxfe_dest_size_cb (void *data,
                               int video_width, int video_height, double video_pixel_aspect,
                               int *dest_width, int *dest_height, double *dest_pixel_aspect)
{
  fe_t *this = (fe_t *)data;

  if (!this)
    return;

  *dest_width  = this->width;
  *dest_height = this->height;

  *dest_pixel_aspect = this->dest_pixel_aspect(this, video_pixel_aspect,
                                               video_width, video_height);
}

static void init_atoms(sxfe_t *this)
{
  if (this->xa_SXFE_INTERRUPT == None) {
    this->xa_SXFE_INTERRUPT     = XInternAtom(this->display, "SXFE_INTERRUPT", False);
    this->xa_WM_DELETE_WINDOW   = XInternAtom(this->display, "WM_DELETE_WINDOW", False);
    this->xa_MOTIF_WM_HINTS     = XInternAtom(this->display, "_MOTIF_WM_HINTS", False);
    this->xa_WIN_LAYER          = XInternAtom(this->display, "_WIN_LAYER", False);
    this->xa_WIN_STATE          = XInternAtom(this->display, "_WIN_STATE", False);
    this->xa_NET_ACTIVE_WINDOW  = XInternAtom(this->display, "_NET_ACTIVE_WINDOW", False);
    this->xa_NET_WM_STATE       = XInternAtom(this->display, "_NET_WM_STATE", False);
    this->xa_NET_WM_STATE_ADD   = XInternAtom(this->display, "_NET_WM_STATE_ADD", False);
    this->xa_NET_WM_STATE_DEL   = XInternAtom(this->display, "_NET_WM_STATE_DEL", False);
    this->xa_NET_WM_STATE_ABOVE = XInternAtom(this->display, "_NET_WM_STATE_ABOVE", False);
    this->xa_NET_WM_STATE_STICKY= XInternAtom(this->display, "_NET_WM_STATE_STICKY", False);
    this->xa_NET_WM_STATE_FULLSCREEN = XInternAtom(this->display, "_NET_WM_STATE_FULLSCREEN", False);
    this->xa_NET_WM_STATE_STAYS_ON_TOP = XInternAtom(this->display, "_NET_WM_STATE_STAYS_ON_TOP", False);
  }
}

static void set_fs_size_hint(sxfe_t *this)
{
  XSizeHints hint;
  hint.flags  = USSize | USPosition | PPosition | PSize;
  hint.x      = this->xinerama_x;
  hint.y      = this->xinerama_y;
  hint.width  = this->x.width;
  hint.height = this->x.height;
  XLockDisplay(this->display);
  XSetNormalHints(this->display, this->window[1], &hint);
  XUnlockDisplay(this->display);
}

/* set_border
 *
 * Set/remove window border
 *
 */
static void set_border(sxfe_t *this, Window window, int border)
{
  MWMHints mwmhints = {
    .flags       = MWM_HINTS_DECORATIONS,
    .decorations = border ? 1 : 0,
  };

  if(this->window_id > 0)
    return;

  XLockDisplay(this->display);
  XChangeProperty(this->display, window,
                  this->xa_MOTIF_WM_HINTS, this->xa_MOTIF_WM_HINTS, 32,
                  PropModeReplace, (unsigned char *) &mwmhints,
                  PROP_MWM_HINTS_ELEMENTS);
  XUnlockDisplay(this->display);
}

static void set_fullscreen_props(sxfe_t *this)
{
  XEvent ev;

  if(this->window_id > 0)
    return;

  set_fs_size_hint(this);

  /* no border in fullscreen window */
  set_border(this, this->window[1], 0);

  memset(&ev, 0, sizeof(ev));
  ev.type                 = ClientMessage;
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = this->xa_NET_WM_STATE;
  ev.xclient.display      = this->display;
  ev.xclient.window       = this->window[1];
  ev.xclient.format       = 32;
  ev.xclient.data.l[0]    = 1;
  /*ev.xclient.data.l[0]    = this->atom_state_add;*/

  /* _NET_WM_STATE_FULLSCREEN */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->xa_NET_WM_STATE_FULLSCREEN;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_ABOVE */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->xa_NET_WM_STATE_ABOVE;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_ON_TOP */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->xa_NET_WM_STATE_STAYS_ON_TOP;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_ACTIVE_WINDOW */
  XLockDisplay(this->display);
  ev.xclient.message_type = this->xa_NET_ACTIVE_WINDOW;
  ev.xclient.data.l[0] = 0;
  ev.xclient.data.l[1] = 0;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);
}

static void update_window_title(sxfe_t *this)
{
  XLockDisplay(this->display);

  if (!this->x.keypress) { /* handler is set only in local mode */
    char *name = NULL;
    if (XFetchName(this->display, this->window[0], &name) && name) {
      char *newname = NULL;
      if (strstr(name, " (top)"))
        *strstr(name, " (top)") = 0;
      if (this->stay_above)
        if (asprintf(&newname, "%s (top)", name) < 0)
          newname = NULL;
      XStoreName(this->display, this->window[0], newname ?: name);
      XStoreName(this->display, this->window[1], newname ?: name);
      XFree(name);
      free(newname);
    } else {
      XStoreName(this->display, this->window[0], this->stay_above ? "VDR - (top)" : "VDR");
      XStoreName(this->display, this->window[1], this->stay_above ? "VDR - (top)" : "VDR");
    }
  } else {
    XStoreName(this->display, this->window[0], this->stay_above ? "Local VDR (top)" : "Local VDR");
    XStoreName(this->display, this->window[1], this->stay_above ? "Local VDR (top)" : "Local VDR");
  }
  XUnlockDisplay(this->display);
}

static void set_above(sxfe_t *this, int stay_above)
{
  XEvent ev;
  long propvalue[1];

  if(this->window_id > 0)
    return;

  if(this->stay_above != stay_above) {
    this->stay_above = stay_above;
    /* Update window title */
    update_window_title(this);
  }

  memset(&ev, 0, sizeof(ev));
  ev.type                 = ClientMessage;
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = this->xa_NET_WM_STATE;
  ev.xclient.display      = this->display;
  ev.xclient.window       = this->window[0];
  ev.xclient.format       = 32;
  ev.xclient.data.l[0]    = stay_above ? 1:0; /*this->atom_state_add : this->atom_state_del;*/

  /* _NET_WM_STATE_ABOVE */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->xa_NET_WM_STATE_ABOVE;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_STAYS_ON_TOP */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->xa_NET_WM_STATE_STAYS_ON_TOP;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_STICKY */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->xa_NET_WM_STATE_STICKY;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _WIN_LAYER */
  propvalue[0] = stay_above ? WIN_LAYER_ONTOP : WIN_LAYER_NORMAL;
  XLockDisplay(this->display);
  XChangeProperty(this->display, this->window[0], this->xa_WIN_LAYER,
                  XA_CARDINAL, 32, PropModeReplace, (unsigned char *)propvalue,
                  1);
  XUnlockDisplay(this->display);

#if 0
  /* sticky */
  memset(&ev, 0, sizeof(ev));
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = this->xa_WIN_STATE;
  ev.xclient.display      = this->display;
  ev.xclient.window       = this->window[0];
  ev.xclient.format       = 32;
  ev.xclient.data.l[0] = (1<<0);
  ev.xclient.data.l[1] = (stay_above?(1<<0):0);
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
#endif
#if 0
  /* on top */
  memset(&ev, 0, sizeof(ev));
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = this->xa_WIN_LAYER;
  ev.xclient.display      = this->display;
  ev.xclient.window       = this->window[0];
  ev.xclient.format       = 32;
  ev.xclient.data.l[0] = (stay_above?10:6);
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask|SubstructureRedirectMask, &ev);
#endif
#if 0
  /* layer */
  XClientMessageEvent xev;

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.display = this->display;
  xev.window = this->window[0];
  xev.message_type = this->xa_WIN_LAYER;
  xev.format = 32;
  xev.data.l[0] = 10;
  xev.data.l[1] = CurrentTime;
  XSendEvent(this->display, DefaultRootWindow(this->display), False,
             SubstructureNotifyMask, (XEvent *) & xev);

  XMapRaised(this->display, this->window[0]);
#endif
#if 0
  xine_port_send_gui_data(this->video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
                          (void*) this->window[this->fullscreen ? 1 : 0]);
#endif
}

/*
 * set_cursor
 *
 * Disable mouse cursor in video window (set transparent cursor)
 */
static void set_cursor(Display *dpy, Window win, const int enable)
{
  if(enable)
    XDefineCursor(dpy, win, None);
  else {
    /* no cursor */
    const char bm_no_data[] = { 0,0,0,0, 0,0,0,0 };
    Cursor no_ptr;
    XColor black, dummy;
    Pixmap bm_no = XCreateBitmapFromData(dpy, win, bm_no_data, 8, 8);
    XAllocNamedColor(dpy, DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)),
                     "black", &black, &dummy);
    no_ptr = XCreatePixmapCursor(dpy, bm_no, bm_no, &black, &black, 0, 0);
    XDefineCursor(dpy, win, None);
    XDefineCursor(dpy, win, no_ptr);
  }
}

static void update_xinerama_info(sxfe_t *this)
{
#ifdef HAVE_XINERAMA
  int screen = this->xinerama_screen;
  this->xinerama_x = this->xinerama_y = 0;
  XLockDisplay(this->display);
  if(screen >= -1 && XineramaIsActive(this->display)) {
    XineramaScreenInfo *screens;
    int num_screens;
    screens = XineramaQueryScreens(this->display, &num_screens);
    if(screen >= num_screens)
      screen = num_screens - 1;
    if(screen == -1) {
      const int x = this->origxpos + this->origwidth / 2;
      const int y = this->origypos + this->origheight / 2;
      for(screen = num_screens - 1; screen > 0; screen--) {
        const int x0 = screens[screen].x_org;
        const int y0 = screens[screen].y_org;
        const int x1 = x0 + screens[screen].width;
        const int y1 = y0  + screens[screen].height;
        if(x0 <= x && x <= x1 && y0 <= y && y <= y1)
          break;
      }
    }
    if (screen < 0)
      screen = 0;
    this->xinerama_x = screens[screen].x_org;
    this->xinerama_y = screens[screen].y_org;
    this->x.width    = screens[screen].width;
    this->x.height   = screens[screen].height;

    XFree(screens);
  }
  XUnlockDisplay(this->display);
#endif
}

/*
 * update_screen_size
 *
 * Update screen size (= size of fullscreen window)
 */
static void update_screen_size(sxfe_t *this)
{
  this->x.width = DisplayWidth(this->display, this->screen);
  this->x.height = DisplayHeight(this->display, this->screen);
  update_xinerama_info(this);
}

/*
 * HUD OSD stuff
 */

#ifdef HAVE_XRENDER
static Xrender_Surf * xrender_surf_new(Display *dpy, Drawable draw, Visual *vis,
                                       int w, int h, int alpha)
{
  Xrender_Surf *rs;
  XRenderPictFormat *fmt;
  XRenderPictureAttributes att;

  rs = calloc(1, sizeof (Xrender_Surf));

  fmt = XRenderFindStandardFormat(dpy, alpha ? PictStandardARGB32 : PictStandardRGB24);
  rs->w = w;
  rs->h = h;
  rs->depth = fmt->depth;
  rs->vis = vis;
  rs->draw = XCreatePixmap(dpy, draw, w, h, fmt->depth);
  att.dither = 1;
  att.component_alpha = 1;
  att.repeat = 0;
  rs->pic = XRenderCreatePicture(dpy, rs->draw, fmt, CPRepeat | CPDither | CPComponentAlpha, &att);
  rs->allocated = 1;
  return rs;
}

static void xrender_surf_blend(Display *dpy, Xrender_Surf *src, Xrender_Surf *dst,
                               int x, int y, int w, int h,
                               XDouble scale_x, XDouble scale_y, int smooth)
{
  XTransform xf;

  if(!scale_x)
    scale_x = 1;
  if(!scale_y)
    scale_y = 1;

  xf.matrix[0][0] = XDoubleToFixed(1.0 / scale_x); xf.matrix[0][1] = 0; xf.matrix[0][2] = 0;
  xf.matrix[1][0] = 0; xf.matrix[1][1] = XDoubleToFixed(1.0 / scale_y); xf.matrix[1][2] = 0;
  xf.matrix[2][0] = 0; xf.matrix[2][1] = 0; xf.matrix[2][2] = XDoubleToFixed(1.0);
  XRenderSetPictureFilter(dpy, src->pic, smooth ? "bilinear" : "nearest", NULL, 0);
  XRenderSetPictureTransform(dpy, src->pic, &xf);
  x = (int)ceil((double)(x>0?x-1:0) * scale_x);
  y = (int)ceil((double)(y>0?y-1:0) * scale_y);
  w = (int)floor((double)(w+2) * scale_x);
  h = (int)floor((double)(h+2) * scale_y);
  XRenderComposite(dpy, PictOpSrc, src->pic, None, dst->pic, x, y, 0, 0, x, y, w, h);
}

static Xrender_Surf * xrender_surf_adopt(Display *dpy, Drawable draw, Visual *vis, int w, int h)
{
  Xrender_Surf *rs;
  XRenderPictFormat *fmt;
  XRenderPictureAttributes att;

  rs = calloc(1, sizeof(Xrender_Surf));

  fmt = XRenderFindVisualFormat(dpy, vis);
  rs->w = w;
  rs->h = h;
  rs->depth = fmt->depth;
  rs->vis = vis;
  rs->draw = draw;
  att.dither = 1;
  att.component_alpha = 1;
  att.repeat = 0;
  rs->pic = XRenderCreatePicture(dpy, rs->draw, fmt, CPRepeat | CPDither | CPComponentAlpha, &att);
  rs->allocated = 0;
  return rs;
}

static void xrender_surf_free(Display *dpy, Xrender_Surf *rs)
{
  if(rs->allocated)
    XFreePixmap(dpy, rs->draw);
  XRenderFreePicture(dpy, rs->pic);
  free(rs);
}

static Visual *find_argb_visual(Display *dpy, int scr)
{
  XVisualInfo *xvi, template;
  int nvi, i;
  XRenderPictFormat *format;
  Visual *visual;

  template.screen = scr;
  template.depth = 32;
  template.class = TrueColor;
  xvi = XGetVisualInfo(dpy, VisualScreenMask | VisualDepthMask |
                       VisualClassMask, &template, &nvi);

  if(!xvi) {
    LOGERR("find_argb_visual: XGetVisualInfo failed (no xvi)");
    return 0;
  }

  visual = 0;
  for(i = 0; i < nvi; i++) {
     LOGDBG("find_argb_visual: iteration %d of %d", i, nvi);
     format = XRenderFindVisualFormat(dpy, xvi[i].visual);
     if((format->type == PictTypeDirect) && format->direct.alphaMask) {
       visual = xvi[i].visual;
       break;
     }
  }

  XFree(xvi);

  if(!visual)
    LOGERR("find_argb_visual: No visual found");

  return visual;
}

static void hud_fill_img_memory(uint32_t* dst, const struct osd_command_s *cmd)
{
  int i, pixelcounter = 0;
  int idx = cmd->y * HUD_MAX_WIDTH + cmd->x;

  for(i = 0; i < cmd->num_rle; ++i) {
    const uint8_t alpha = (cmd->palette + (cmd->data + i)->color)->alpha;
    const uint8_t r = (cmd->palette + (cmd->data + i)->color)->r;
    const uint8_t g = (cmd->palette + (cmd->data + i)->color)->g;
    const uint8_t b = (cmd->palette + (cmd->data + i)->color)->b;
    int j, finalcolor = 0;
    finalcolor |= ((alpha << 24) & 0xFF000000);
    finalcolor |= ((r << 16) & 0x00FF0000);
    finalcolor |= ((g << 8) & 0x0000FF00);
    finalcolor |= (b & 0x000000FF);

    for(j = 0; j < (cmd->data + i)->len; ++j) {
      if(pixelcounter >= cmd->w) {
        idx += HUD_MAX_WIDTH - pixelcounter;
        pixelcounter = 0;
      }
      dst[idx++] = finalcolor;
      ++pixelcounter;
    }
  }
}

static int hud_osd_command(frontend_t *this_gen, struct osd_command_s *cmd)
{
  sxfe_t *this = (sxfe_t*)this_gen;
  if(this && this->hud && cmd) {
    XLockDisplay(this->display);
    switch(cmd->cmd) {
    case OSD_Nop: /* Do nothing ; used to initialize delay_ms counter */
      LOGDBG("HUD osd NOP");
      break;

    case OSD_Size: /* Set size of VDR OSD area */
      LOGDBG("HUD Set Size");
      this->osd_width = (cmd->w > 0) ? cmd->w : OSD_DEF_WIDTH;
      this->osd_height = (cmd->h > 0) ? cmd->h : OSD_DEF_HEIGHT;

      XSetForeground(this->display, this->gc, 0x00000000);
      XFillRectangle(this->display, this->surf_img->draw, this->gc,
                     0, 0, this->osd_width+2, this->osd_height+2);
      XFlush(this->display);
      break;

    case OSD_Set_RLE: /* Create/update OSD window. Data is rle-compressed. */
      LOGDBG("HUD Set RLE");
      if (!(cmd->flags & OSDFLAG_TOP_LAYER))
        break;
#ifdef HAVE_XSHM
      if(this->completion_event != -1) {
        hud_fill_img_memory((uint32_t*)(this->hud_img->data), cmd);
        if(!cmd->scaling) {
          /* Place image directly onto hud window */
          XShmPutImage(this->display, this->hud_window, this->gc, this->hud_img,
                       cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                       cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                       cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1,
                       cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1,
                       False);
        } else {
          /* Place image onto Xrender surface which will be blended onto hud window */
          XShmPutImage(this->display, this->surf_img->draw, this->gc, this->hud_img,
                       cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                       cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                       cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1,
                       cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1,
                       False);
          xrender_surf_blend(this->display, this->surf_img, this->surf_win,
                             cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                             cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1,
                             cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1,
                             (XDouble)this->x.width / (XDouble)this->osd_width,
                             (XDouble)this->x.height / (XDouble)this->osd_height,
                             (cmd->scaling & 2)); // Note: HUD_SCALING_BILINEAR=2
        }
      }
      else
#endif
      {
        hud_fill_img_memory(this->hud_img_mem, cmd);
        if(!cmd->scaling) {
          /* Place image directly onto hud window (always unscaled) */
          XPutImage(this->display, this->hud_window, this->gc, this->hud_img,
                    cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                    cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                    cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1,
                    cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1);
        } else {
          /* Place image onto Xrender surface which will be blended onto hud window */
          XPutImage(this->display, this->surf_img->draw, this->gc, this->hud_img,
                    cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                    cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                    cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1,
                    cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1);
          xrender_surf_blend(this->display, this->surf_img, this->surf_win,
                             cmd->x + cmd->dirty_area.x1, cmd->y + cmd->dirty_area.y1,
                             cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1,
                             cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1,
                             (XDouble)this->x.width / (XDouble)this->osd_width,
                             (XDouble)this->x.height / (XDouble)this->osd_height,
                             (cmd->scaling & 2)); // Note: HUD_SCALING_BILINEAR=2
        }
      }
      break;

    case OSD_SetPalette: /* Modify palette of already created OSD window */
      LOGDBG("HUD osd SetPalette");
      break;

    case OSD_Move:       /* Change x/y position of already created OSD window */
      LOGDBG("HUD osd Move");
      break;

    case OSD_Set_YUV:    /* Create/update OSD window. Data is in YUV420 format. */
      LOGDBG("HUD osd set YUV");
      break;

    case OSD_Close: /* Close OSD window */
      LOGDBG("HUD osd Close");
      if (!(cmd->flags & OSDFLAG_TOP_LAYER))
        break;
      XSetForeground(this->display, this->gc, 0x00000000);
      XFillRectangle(this->display, this->hud_window, this->gc,
                     0, 0, this->x.width, this->x.height);
      XFillRectangle(this->display, this->surf_img->draw, this->gc,
                     0, 0, this->osd_width+2, this->osd_height+2);
      XFlush(this->display);
      break;

    default:
      LOGDBG("hud_osd_command: unknown osd command");
      break;
    }
    XUnlockDisplay(this->display);
  }
  return 1;
}

static int hud_osd_open(sxfe_t *this)
{
  if(this && this->hud) {
    int dummy;

    XLockDisplay(this->display);

    LOGDBG("opening HUD OSD window...");

    if(!XRenderQueryExtension(this->display, &dummy, &dummy)) {
      LOGMSG("hud_osd_open: ERROR: XRender extension not available.");
      LOGMSG("XRender extension must be enabled in X configuration (xorg.conf etc.)");
      this->hud = 0;
      XUnlockDisplay(this->display);
      return 1;
    }

    this->hud_vis = find_argb_visual(this->display, DefaultScreen(this->display));
    if(!this->hud_vis) {
      LOGMSG("find_argb_visual() failed. HUD OSD disabled.");
      this->hud = 0;
      XUnlockDisplay(this->display);
      return 1;
    }

    Colormap hud_colormap = XCreateColormap(this->display,
                                            RootWindow(this->display, DefaultScreen(this->display)),
                                            this->hud_vis, AllocNone);

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    attributes.background_pixel = 0x00000000;
    attributes.border_pixel = 0;
    attributes.colormap = hud_colormap;
    attributes.backing_store = Always;

    this->hud_window = XCreateWindow(this->display, DefaultRootWindow(this->display),
                                     this->x.xpos, this->x.ypos,
                                     this->x.width, this->x.height,
                                     0, 32, InputOutput, this->hud_vis,
                                     CWBackPixel | CWBorderPixel |
                                     CWOverrideRedirect | CWColormap,
                                     &attributes);

    XSelectInput(this->display, this->hud_window,
                 StructureNotifyMask |
                 ExposureMask |
                 KeyPressMask |
                 ButtonPressMask |
                 FocusChangeMask);

    XStoreName(this->display, this->hud_window, "HUD");
    this->gc = XCreateGC(this->display, this->hud_window, 0, NULL);

#ifdef HAVE_XSHM
    if(this->completion_event != -1) {
      this->hud_img = XShmCreateImage(this->display, this->hud_vis, 32, ZPixmap, NULL, &(this->hud_shminfo),
                                      HUD_MAX_WIDTH, HUD_MAX_HEIGHT);

      this->hud_shminfo.shmid = shmget(IPC_PRIVATE, this->hud_img->bytes_per_line * this->hud_img->height,
                                       IPC_CREAT | 0777);

      this->hud_shminfo.shmaddr = this->hud_img->data = shmat(this->hud_shminfo.shmid, 0, 0);
      this->hud_shminfo.readOnly = True;

      XShmAttach(this->display, &(this->hud_shminfo));
    }
    else
#endif
    {
      /* Fall-back to traditional memory */
      LOGMSG("hud_osd_open: XShm not available, falling back to normal (slow) memory");
      this->hud_img_mem = malloc(4 * HUD_MAX_WIDTH * HUD_MAX_HEIGHT);
      this->hud_img = XCreateImage(this->display, this->hud_vis, 32, ZPixmap, 0, (char*)this->hud_img_mem,
                                   HUD_MAX_WIDTH, HUD_MAX_HEIGHT, 32, 0);
    }

    this->surf_win = xrender_surf_adopt(this->display, this->hud_window, this->hud_vis, HUD_MAX_WIDTH, HUD_MAX_HEIGHT);
    this->surf_img = xrender_surf_new(this->display, this->hud_window, this->hud_vis, HUD_MAX_WIDTH, HUD_MAX_HEIGHT, 1);

    XUnlockDisplay(this->display);

    this->fe.xine_osd_command = hud_osd_command;
  }
  return 1;
}

/*
 * hud_osd_resize
 *
 * - Move and resize HUD along with main or fullscreen window
 */
static void hud_osd_resize(sxfe_t *this, Window video_window, int width, int height)
{
  if(this->hud) {
    if(video_window == this->window[0]) {
      int    hud_x, hud_y;
      Window tmp_win;
      XLockDisplay(this->display);
      XTranslateCoordinates(this->display, this->window[0],
                            DefaultRootWindow(this->display),
                            0, 0, &hud_x, &hud_y, &tmp_win);
      XResizeWindow(this->display, this->hud_window, width, height);
      XMoveWindow(this->display, this->hud_window, hud_x, hud_y);
      set_cursor(this->display, this->hud_window, 1);
      XUnlockDisplay(this->display);
    } else if(video_window == this->window[1]) {
      XLockDisplay(this->display);
      XResizeWindow(this->display, this->hud_window, width, height);
      XMoveWindow(this->display, this->hud_window, 0, 0);
      set_cursor(this->display, this->hud_window, 0);
      XUnlockDisplay(this->display);
    }
  }
}

/*
 * hud_osd_focus
 *
 * - show / hide HUD OSD window
 */
static void hud_osd_focus(sxfe_t *this, XFocusChangeEvent *fev)
{
  if(this && this->hud)
    if(fev->window == this->window[0] || fev->window == this->window[1]) {

      XLockDisplay(this->display);

      if(fev->type == FocusIn)
        /* Show HUD again if sxfe window receives focus */
        XMapWindow(this->display, this->hud_window);

      else if(fev->type == FocusOut)
        /* Dismiss HUD window if focusing away from frontend window */
        XUnmapWindow(this->display, this->hud_window);

      XUnlockDisplay(this->display);
    }
}

static void hud_osd_close(sxfe_t *this)
{
  if(this && this->hud) {
    XLockDisplay(this->display);
    LOGDBG("closing hud window...");

#ifdef HAVE_XSHM
    if(this->completion_event != -1) {
      XShmDetach(this->display, &(this->hud_shminfo));
      XDestroyImage(this->hud_img);
      shmdt(this->hud_shminfo.shmaddr);
      shmctl(this->hud_shminfo.shmid, IPC_RMID, 0);
    }
    else
#endif
      XDestroyImage(this->hud_img);

    if(this->surf_img)
      xrender_surf_free(this->display, this->surf_img);
    if(this->surf_win)
      xrender_surf_free(this->display, this->surf_win);

    XDestroyWindow(this->display, this->hud_window);
    XUnlockDisplay(this->display);
  }
}
#endif /* HAVE_XRENDER */

/*
 * disable_DPMS
 */
static void disable_DPMS(sxfe_t *this)
{
#ifdef HAVE_XDPMS
  int dpms_dummy;
  XLockDisplay(this->display);
  if (DPMSQueryExtension(this->display, &dpms_dummy, &dpms_dummy) && DPMSCapable(this->display)) {
    CARD16 dpms_level;
    DPMSInfo(this->display, &dpms_level, &this->dpms_state);
    DPMSDisable(this->display);
  } else {
    LOGMSG("disable_DPMS: DPMS unavailable");
  }
  XUnlockDisplay(this->display);
#endif
}

/*
 * open_display
 *
 * Try to connect to X server, in order
 *   1) user-given display
 *   2) DISPLAY environment variable
 *   3) default display
 *   4) :0.0
 *   5) 127.0.0.1:0.0
 */
static int open_display(sxfe_t *this, const char *video_port)
{
  if (video_port && strlen(video_port)>2) {
    if (!(this->display = XOpenDisplay(video_port)))
      LOGERR("sxfe_display_open: failed to connect to X server (%s)",
             video_port);
  }

  if (!this->display) {
    if (NULL!=(video_port=getenv("DISPLAY")) && !(this->display = XOpenDisplay(video_port)))
      LOGERR("sxfe_display_open: failed to connect to X server (%s)",
             video_port);
  }

  if (!this->display) {
    this->display = XOpenDisplay(NULL);
  }

  if (!this->display) {
    if (!(this->display = XOpenDisplay(":0.0")))
      LOGERR("sxfe_display_open: failed to connect to X server (:0.0)");
  }

  if (!this->display) {
    if (!(this->display = XOpenDisplay("127.0.0.1:0.0")))
      LOGERR("sxfe_display_open: failed to connect to X server (127.0.0.1:0.0");
  }

  if (!this->display) {
    LOGERR("sxfe_display_open: failed to connect to X server.");
    LOGMSG("If X server is running, try running \"xhost +\" in xterm window");
    return 0;
  }

  return 1;
}

/*
 * set_icon
 */
static void set_icon(sxfe_t *this)
{
# include "vdrlogo_32x32.c"
  XLockDisplay(this->display);
#if defined(__WORDSIZE) && (__WORDSIZE == 32)
  /* Icon */
  XChangeProperty(this->display, this->window[0],
                  XInternAtom(this->display, "_NET_WM_ICON", False),
                  XA_CARDINAL, 32, PropModeReplace,
                  (unsigned char *) &vdrlogo_32x32,
                  2 + vdrlogo_32x32.width*vdrlogo_32x32.height);
#else
  long      q[2+32*32];
  uint32_t *p = (uint32_t*)&vdrlogo_32x32;
  int       i;
  for (i = 0; i < 2 + vdrlogo_32x32.width*vdrlogo_32x32.height; i++)
    q[i] = p[i];
  XChangeProperty(this->display, this->window[0],
                  XInternAtom(this->display, "_NET_WM_ICON", False),
                  XA_CARDINAL, 32, PropModeReplace,
                  (unsigned char *) q,
                  2 + vdrlogo_32x32.width*vdrlogo_32x32.height);
#endif
  XUnlockDisplay(this->display);
}

/* 
 * detect_display_ratio
 *
 * Calculate display aspect ratio
 */
static double detect_display_ratio(Display *dpy, int screen)
{
  double res_h = DisplayWidth  (dpy, screen) * 1000.0 / DisplayWidthMM  (dpy, screen);
  double res_v = DisplayHeight (dpy, screen) * 1000.0 / DisplayHeightMM (dpy, screen);

  double display_ratio = res_v / res_h;
  double diff          = display_ratio - 1.0;

  if ((diff < 0.01) && (diff > -0.01))
    display_ratio   = 1.0;

  LOGDBG("Display size : %d x %d mm",
         DisplayWidthMM  (dpy, screen),
         DisplayHeightMM (dpy, screen));
  LOGDBG("               %d x %d pixels",
         DisplayWidth  (dpy, screen),
         DisplayHeight (dpy, screen));
  LOGDBG("               %ddpi / %ddpi",
         (int)(res_v/1000*25.4), (int)(res_h/1000*25.4));
  LOGDBG("Display ratio: %f/%f = %f", res_v, res_h, display_ratio);

  return display_ratio;
}

/*
 * create_windows
 *
 * Create and initialize fullscreen and windowed mode X11 windows
 *  - Borderless fullscreen window
 *  - Set window title and icon
 */
static void create_windows(sxfe_t *this)
{
  XLockDisplay(this->display);
  /* create and display our video window */
  this->window[0] = XCreateSimpleWindow (this->display,
                                         DefaultRootWindow(this->display),
                                         this->x.xpos, this->x.ypos,
                                         this->x.width, this->x.height,
                                         1, 0, 0);
  this->window[1] = XCreateSimpleWindow(this->display, XDefaultRootWindow(this->display),
                                        this->xinerama_x, this->xinerama_y,
                                        this->x.width, this->x.height,
                                        0, 0, 0);

  /* full-screen window */
  set_fullscreen_props(this);

  /* Window hint */
  XClassHint *classHint = XAllocClassHint();
  if(classHint) {
    classHint->res_name = "VDR";
    classHint->res_class = "VDR";
    XSetClassHint(this->display, this->window[0], classHint);
    XSetClassHint(this->display, this->window[1], classHint);
    XFree(classHint);
  }

  /* Window name */
  const char *initial_title = (!this->x.keypress) ? "Connecting to VDR ..." : "Local VDR";
  XStoreName(this->display, this->window[0], initial_title);
  XStoreName(this->display, this->window[1], initial_title);

  /* Icon */
  set_icon(this);
  XUnlockDisplay(this->display);
}

/*
 * sxfe_display_open
 *
 * connect to X server, create windows
 */
static int sxfe_display_open(frontend_t *this_gen,
                             int xpos, int ypos,
                             int width, int height, int fullscreen, int hud,
                             int modeswitch, const char *modeline, int aspect,
                             fe_keypress_f keyfunc, int no_x_kbd, int gui_hotkeys,
                             const char *video_port, int scale_video, int field_order,
                             const char *aspect_controller, int window_id)
{
  sxfe_t    *this = (sxfe_t*)this_gen;

  if(this->display)
    this->fe.fe_display_close(this_gen);

  if(keyfunc) {
    this->x.keypress = keyfunc;
    this->x.keypress("XKeySym", ""); /* triggers learning mode */
  }

  LOGDBG("sxfe_display_open(width=%d, height=%d, fullscreen=%d, display=%s)",
         width, height, fullscreen, video_port);

  if(hud) {
#ifdef HAVE_XRENDER
    LOGDBG("sxfe_display_open: Enabling HUD OSD");
    this->hud        = hud;
    this->osd_width  = OSD_DEF_WIDTH;
    this->osd_height = OSD_DEF_HEIGHT;
#else
    LOGMSG("sxfe_display_open: Application was compiled without XRender support. HUD OSD disabled.");
#endif
  }

  this->x.xpos        = xpos;
  this->x.ypos        = ypos;
  this->x.width       = width;
  this->x.height      = height;
  this->x.aspect      = aspect;
/*this->x.cropping    = 0;*/
  this->x.overscan    = 0;
  this->x.scale_video = scale_video;
  this->x.field_order = field_order ? 1 : 0;
  this->x.aspect_controller = aspect_controller ? strdup(aspect_controller) : NULL;

  this->origxpos      = 0;
  this->origypos      = 0;
  this->origwidth     = width>0 ? width : OSD_DEF_WIDTH;
  this->origheight    = height>0 ? height : OSD_DEF_HEIGHT;

  this->check_move    = 0;
  this->dragging      = 0;
  this->dragging_x    = 0;
  this->dragging_y    = 0;

  this->fullscreen      = fullscreen;
/*this->vmode_switch    = modeswitch;*/
  this->fullscreen_state_forced = 0;
/*this->modeline = strdup(modeline ?: "");*/
  this->window_id = window_id;

  this->xinerama_screen = -1;

  this->gui_hotkeys = gui_hotkeys;
  this->no_x_kbd    = no_x_kbd ? 1 : 0;

  /*
   * init x11 stuff
   */

  if (!XInitThreads ()) {
    LOGERR("sxfe_display_open: XInitThreads failed");
    free(this);
    return 0;
  }

  if (!open_display(this, video_port))
    return 0;

  XLockDisplay (this->display);

  this->screen = DefaultScreen(this->display);

  /* #warning sxfe_display_open: TODO: switch vmode */

  /* completion event */
  this->completion_event = -1;
#ifdef HAVE_XSHM
  if (XShmQueryExtension (this->display) == True) {
    this->completion_event = XShmGetEventBase (this->display) + ShmCompletion;
  }
#endif

  init_atoms(this);

  if(fullscreen)
    update_screen_size(this);

  /* Output to existing window ? (embedded to another app) */
  if(this->window_id > 0) {
    LOGMSG("sxfe_display_open(): Using X11 window %d for output", this->window_id);
    this->window[0] = this->window[1] = (Window)this->window_id;
    XUnmapWindow(this->display, this->window[0]);
  } else {
    create_windows(this);
  }

  /* Select input */
  XSelectInput (this->display, this->window[0],
                StructureNotifyMask |
                ExposureMask |
                KeyPressMask |
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                FocusChangeMask);
  XSelectInput (this->display, this->window[1],
                StructureNotifyMask |
                ExposureMask |
                KeyPressMask |
                ButtonPressMask |
                FocusChangeMask);

  /* Map current window */
  XMapRaised (this->display, this->window[this->fullscreen ? 1 : 0]);
  XMoveWindow(this->display, this->window[0], this->x.xpos, this->x.ypos);

  /* determine display aspect ratio */
  this->x.display_ratio = detect_display_ratio(this->display, this->screen);

  /* we want to get notified if user closes the window */
  XSetWMProtocols(this->display, this->window[this->fullscreen ? 1 : 0], &(this->xa_WM_DELETE_WINDOW), 1);

  /* Hide cursor */
  if(this->window_id <= 0)
    set_cursor(this->display, this->window[1], 0);

  /* No screen saver */
  /* #warning TODO: suspend --> activate blank screen saver / DPMS display off ? */
  XSetScreenSaver(this->display, 0, 0, DefaultBlanking, DefaultExposures);

  /* Disable DPMS */
  disable_DPMS(this);

#ifdef HAVE_DBUS_GLIB_1
  /* Disable GNOME screensaver */
  gnome_screensaver_control(0);
#endif

  /* setup xine visual type */
  this->x.xine_visual_type         = XINE_VISUAL_TYPE_X11;
  this->x.vis_x11.display          = this->display;
  this->x.vis_x11.screen           = this->screen;
  this->x.vis_x11.d                = this->window[this->fullscreen ? 1 : 0];
  this->x.vis_x11.dest_size_cb     = sxfe_dest_size_cb;
  this->x.vis_x11.frame_output_cb  = this->x.frame_output_handler;
  this->x.vis_x11.user_data        = this;

  set_fullscreen_props(this);

  XUnlockDisplay (this->display);
#ifdef HAVE_XRENDER
  return hud_osd_open(this);
#else
  return 1;
#endif
}

/*
 * sxfe_display_config
 *
 * configure windows
 */
static int sxfe_display_config(frontend_t *this_gen,
                               int xpos, int ypos,
                               int width, int height, int fullscreen,
                               int modeswitch, const char *modeline,
                               int aspect, int scale_video,
                               int field_order)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  if(this->fullscreen_state_forced)
    fullscreen = this->fullscreen ? 1 : 0;

  if(!fullscreen && (this->x.width != width || this->x.height != height)) {
    this->x.width      = width;
    this->x.height     = height;

    XLockDisplay(this->display);
    XResizeWindow(this->display, this->window[0], this->x.width, this->x.height);
    XUnlockDisplay(this->display);
    if(!fullscreen && !this->fullscreen)
      xine_port_send_gui_data(this->x.video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
                              (void*) this->window[0]);
  }

  if(fullscreen)
    update_screen_size(this);

  if(fullscreen != this->fullscreen) {
    Window tmp_win;
    int    tmp_x, tmp_y;
    XLockDisplay(this->display);
    XUnmapWindow(this->display, this->window[this->fullscreen ? 1 : 0]);
    this->fullscreen = fullscreen ? 1 : 0;
    if(fullscreen)
      set_fullscreen_props(this);
    else
      set_above(this, this->stay_above);
    XMapRaised(this->display, this->window[this->fullscreen ? 1 : 0]);
    if(!fullscreen) {
      XResizeWindow(this->display, this->window[0], this->x.width, this->x.height);
      XMoveWindow(this->display, this->window[0], this->x.xpos, this->x.ypos);
      LOGDBG("sxfe_display_config: XMoveWindow called with x=%d and y=%d",
             this->x.xpos, this->x.ypos);
      this->check_move = 1;
      set_above(this, this->stay_above);
    } else {
      set_fullscreen_props(this);
      XResizeWindow(this->display, this->window[1], this->x.width, this->x.height);
      XMoveWindow(this->display, this->window[1], this->xinerama_x, this->xinerama_y);
    }
    XSync(this->display, False);
    if(XTranslateCoordinates(this->display, this->window[this->fullscreen ? 1 : 0],
                             DefaultRootWindow(this->display),
                             0, 0, &tmp_x, &tmp_y, &tmp_win)) {
      this->x.xpos = tmp_x;
      this->x.ypos = tmp_y;
    }
    XUnlockDisplay(this->display);
    xine_port_send_gui_data(this->x.video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
                            (void*) this->window[this->fullscreen ? 1 : 0]);
  }

#if 0
  if(!modeswitch && strcmp(modeline, this->modeline)) {
    free(this->modeline);
    this->modeline = strdup(modeline ?: "");
    /* #warning TODO - switch vmode */
  }
#endif

/*this->vmode_switch = modeswitch;*/
  this->x.aspect = aspect;
  this->x.scale_video = scale_video;
  this->x.field_order = field_order ? 1 : 0;

  return 1;
}

static void sxfe_toggle_fullscreen(fe_t *this_gen)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  int force = this->fullscreen_state_forced;
  this->fullscreen_state_forced = 0;

  if(!this->fullscreen) {
    this->origwidth  = this->x.width;
    this->origheight = this->x.height;
    this->origxpos = this->x.xpos;
    this->origypos = this->x.ypos;
  } else {
    this->x.xpos = this->origxpos;
    this->x.ypos = this->origypos;
  }

  this->fe.fe_display_config((frontend_t*)this, -1, -1, this->origwidth, this->origheight,
                             this->fullscreen ? 0 : 1,
                             0/*this->vmode_switch*/, NULL/*this->modeline*/,
                             this->x.aspect, this->x.scale_video, this->x.field_order);

  this->fullscreen_state_forced = !force;
}

/*
 *   X event loop
 */

/*
 * sxfe_interrupt
 *
 * - Interrupt X event loop (sxfe_run)
 *
 */
static void sxfe_interrupt(frontend_t *this_gen) 
{
  sxfe_t *this = (sxfe_t*)this_gen;

  XClientMessageEvent event = {
    .type         = ClientMessage,
    .display      = this->display,
    .window       = this->window[this->fullscreen ? 1 : 0],
    .message_type = this->xa_SXFE_INTERRUPT,
    .format       = 32,
  };
  XLockDisplay (this->display);
  if(!XSendEvent(event.display, event.window, 1, /*KeyPressMask*/0, (XEvent *)&event))
    LOGERR("sxfe_interrupt: XSendEvent(ClientMessage) FAILED\n");

  XFlush(this->display);
  XUnlockDisplay (this->display);
}

/*
 * XKeyEvent handler
 *
 */
static void XKeyEvent_handler(sxfe_t *this, XKeyEvent *kev)
{
  if(kev->keycode && kev->type == KeyPress) {
    KeySym         ks;
    char           buffer[20];
    XComposeStatus status;
    const char    *fe_event = NULL;

    XLockDisplay (this->display);
    XLookupString(kev, buffer, sizeof(buffer), &ks, &status);
    XUnlockDisplay (this->display);

    switch(ks) {
      case XK_f:
      case XK_F:
        if (this->gui_hotkeys)
          fe_event = "TOGGLE_FULLSCREEN";
        break;
      case XK_d:
      case XK_D:
        if (this->gui_hotkeys)
          fe_event = "TOGGLE_DEINTERLACE";
        break;
      case XK_Escape:
        if (!this->x.keypress) /* ESC exits only in remote mode */
          fe_event = "QUIT";
        break;
      default:;
    }
    if (fe_event)
      this->x.fe.send_event((frontend_t*)this, fe_event);
    else if (!this->no_x_kbd)
      this->x.fe.send_input_event((frontend_t*)this, "XKeySym", XKeysymToString(ks), 0, 0);
  }
}

/*
 * XConfigureEvent handler
 *
 */
static void XConfigureEvent_handler(sxfe_t *this, XConfigureEvent *cev)
{
  /* Move and resize HUD along with main or fullscreen window */
#ifdef HAVE_XRENDER
  if(this->hud)
    hud_osd_resize(this, cev->window, cev->width, cev->height);
#endif

  /* update video window size */
  if (this->x.width != cev->width || this->x.height != cev->height) {
    this->x.width  = cev->width;
    this->x.height = cev->height;

    /* inform VDR about new size */
    char str[128];
    snprintf(str, sizeof(str), "INFO WINDOW %dx%d", this->x.width, this->x.height);
    this->x.fe.send_event((frontend_t*)this, str);
  }

  if(this->window[0] == cev->window && this->check_move) {
    LOGDBG("ConfigureNotify reveived with x=%d, y=%d, check_move=%d",
           cev->x, cev->y, this->check_move);
    this->check_move = 0;
    if(this->x.xpos != cev->x && this->x.ypos != cev->y) {
      XLockDisplay (this->display);
      XMoveWindow(this->display, this->window[0], cev->x, cev->y);
      XUnlockDisplay (this->display);
    }
  }

  if ((cev->x == 0) && (cev->y == 0)) {
    if(!this->fullscreen) {
      int tmp_x, tmp_y;
      Window tmp_win;
      XLockDisplay(this->display);
      if(XTranslateCoordinates(this->display, cev->window,
                               DefaultRootWindow(this->display),
                               0, 0, &tmp_x, &tmp_y, &tmp_win)) {
        this->x.xpos = tmp_x;
        this->x.ypos = tmp_y;
      }
      XUnlockDisplay(this->display);
    }
  } else {
    if(!this->fullscreen) {
      /* update video window position */
      this->x.xpos = cev->x;
      this->x.ypos = cev->y;
    }
  }
}

/*
 * XMotionEvent handler
 *
 * Track mouse movement when Button1 is pressed down
 *   - enable window dragging: user can simply drag window around screen
 *   - useful when window is borderless (no title bar)
 */
static void XMotionEvent_handler(sxfe_t *this, XMotionEvent *mev)
{
  if(this->dragging && !this->fullscreen) {
    Window tmp_win;
    int xpos, ypos;

    XLockDisplay(this->display);

    while(XCheckMaskEvent(this->display, ButtonMotionMask, (XEvent*)mev));

    XTranslateCoordinates(this->display, this->window[0],
                          DefaultRootWindow(this->display),
                          0, 0, &xpos, &ypos, &tmp_win);

    this->x.xpos = (xpos += mev->x_root - this->dragging_x);
    this->x.ypos = (ypos += mev->y_root - this->dragging_y);
    this->dragging_x = mev->x_root;
    this->dragging_y = mev->y_root;

    XMoveWindow(this->display, this->window[0], xpos, ypos);
    LOGDBG("MotionNotify: XMoveWindow called with x=%d and y=%d", xpos, ypos);

    XUnlockDisplay(this->display);
  }
}

/*
 * XButtonEvent handler
 *
 *  - Double click switches between windowed and fullscreen mode
 *  - Window can be moved by dragging it
 *  - Right mouse button switches window state:
 *    normal window -> borderless window -> always on top -> ...
 */
static void XButtonEvent_handler(sxfe_t *this, XButtonEvent *bev)
{
  switch(bev->button) {
    case Button1:
      /* Double-click toggles between fullscreen and windowed mode */
      if(bev->time - this->prev_click_time < DOUBLECLICK_TIME) {
        /* Toggle fullscreen */
        sxfe_toggle_fullscreen((fe_t*)this);
        this->prev_click_time = 0; /* don't react to third click ... */
      } else {
        this->prev_click_time = bev->time;
        if(!this->fullscreen && this->no_border && !this->dragging) {
          /* start dragging window */
          this->dragging = 1;
          this->dragging_x = bev->x_root;
          this->dragging_y = bev->y_root;
        }
      }
      break;

    case Button3:
      /* Toggle border and stacking */
      if(!this->fullscreen) {
        if(!this->stay_above) {
          set_above(this, 1);
        } else if(!this->no_border) {
          set_border(this, this->window[0], 0);
          this->no_border = 1;
        } else {
          set_border(this, this->window[0], 1);
          this->no_border = 0;
          set_above(this, 0);
        }
      }
      break;
  }
}

/*
 * sxfe_run
 *
 *  - main X event loop
 */
static int sxfe_run(frontend_t *this_gen) 
{
  sxfe_t *this = (sxfe_t*)this_gen;

  /* poll X server (connection socket).
   * (XNextEvent will block until events are queued).
   * We want to use timeout, blocking for long time usually causes vdr
   * watchdog to emergency exit ...
   */
  if (! XPending(this->display)) {
    struct pollfd pfd = {
      .fd = ConnectionNumber(this->display),
      .events = POLLIN,
    };
    if (poll(&pfd, 1, 50) < 1 || !(pfd.revents & POLLIN)) {
      return !this->x.fe.xine_is_finished((frontend_t*)this, 0);
    }
  }

  while (XPending(this->display) > 0) {

    XEvent event;

    XLockDisplay (this->display);
    XNextEvent (this->display, &event);
    XUnlockDisplay (this->display);

    switch (event.type) {
      case Expose:
        if (event.xexpose.count == 0)
          xine_port_send_gui_data (this->x.video_port, XINE_GUI_SEND_EXPOSE_EVENT, &event);
        break;

      case ConfigureNotify:
        XConfigureEvent_handler(this, (XConfigureEvent *) &event);
        break;

#ifdef HAVE_XRENDER
      case FocusIn:
      case FocusOut:
        hud_osd_focus(this, (XFocusChangeEvent *) &event);
        break;
#endif

      case ButtonRelease:
        this->dragging = 0;
        break;

      case MotionNotify:
        XMotionEvent_handler(this, (XMotionEvent *) &event);
        break;

      case ButtonPress:
        XButtonEvent_handler(this, (XButtonEvent *) &event);
        break;

      case KeyPress:
      case KeyRelease:
        XKeyEvent_handler(this, (XKeyEvent *) &event);
        break;

      case ClientMessage:
      {
        XClientMessageEvent *cmessage = (XClientMessageEvent *) &event;
        if ( cmessage->message_type == this->xa_SXFE_INTERRUPT )
          LOGDBG("ClientMessage: sxfe_interrupt");

        if ( cmessage->data.l[0] == this->xa_WM_DELETE_WINDOW ) {
          /* we got a window deletion message from out window manager.*/
          LOGDBG("ClientMessage: WM_DELETE_WINDOW");

          this->x.fe.send_event((frontend_t*)this, "QUIT");
        }
        break;
      }
    }

    if (event.type == this->completion_event)
      xine_port_send_gui_data (this->x.video_port, XINE_GUI_SEND_COMPLETION_EVENT, &event);
  }

  return !this->x.fe.xine_is_finished((frontend_t*)this, 0);
}

static void sxfe_display_close(frontend_t *this_gen)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  if(!this)
    return;

  if(this->x.xine)
    this->fe.xine_exit(this_gen);

  if(this->display) {

#ifdef HAVE_XRENDER
    hud_osd_close(this);
#endif

#ifdef HAVE_DBUS_GLIB_1
    /* Restore GNOE screensaver */
    gnome_screensaver_control(1);
#endif

#ifdef HAVE_XDPMS
    if(this->dpms_state)
      DPMSEnable(this->display);
#endif
    if(this->window_id <= 0) {
      XLockDisplay(this->display);
      XUnmapWindow(this->display, this->window[this->fullscreen ? 1 : 0]);
      XDestroyWindow(this->display, this->window[0]);
      XDestroyWindow(this->display, this->window[1]);
      XUnlockDisplay(this->display);
    }
    XCloseDisplay (this->display);
    this->display = NULL;
  }

  free(this->x.aspect_controller);
  this->x.aspect_controller = NULL;
#if 0
  free(this->modeline);
  this->modeline = NULL;
#endif
}

/*
 * sxfe_xine_open
 *
 * Override fe_xine_open:
 *  - Set window name: append remote host address to title bar text
 */
static int sxfe_xine_open(frontend_t *this_gen, const char *mrl)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  int result = this->fe_xine_open(this_gen, mrl);

  if(result && mrl && !strncmp(mrl, MRL_ID, MRL_ID_LEN) && strstr(mrl, "//")) {
    char *name = NULL, *end;
    if (asprintf(&name, "VDR - %s", strstr(mrl, "//")+2) >= 0) {
      if (NULL != (end = strstr(name, ":37890")) || /* hide only default port */
          NULL != (end = strchr(name, '#')))        /* hide attributes */
        *end = 0;
      XStoreName(this->display, this->window[0], name);
      XStoreName(this->display, this->window[1], name);
      free(name);
    }
  }

  return result;
}

static int sxfe_xine_play(frontend_t *this_gen)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  int result = this->fe_xine_play(this_gen);

#ifdef HAVE_XRENDER
  if (result && this->x.input_plugin && this->hud) {
    LOGDBG("sxfe_xine_play: Enabling HUD OSD");
    this->x.input_plugin->f.fe_handle     = this_gen;
    this->x.input_plugin->f.intercept_osd = hud_osd_command;
  }
#endif /* HAVE_XRENDER */

  return result;
}

static frontend_t *sxfe_get_frontend(void)
{
  sxfe_t *this = calloc(1, sizeof(sxfe_t));

  init_fe((fe_t*)this);

  this->window_id = -1;

  this->fe.fe_display_open   = sxfe_display_open;
  this->fe.fe_display_config = sxfe_display_config;
  this->fe.fe_display_close  = sxfe_display_close;

  this->fe.fe_run       = sxfe_run;
  this->fe.fe_interrupt = sxfe_interrupt;

  this->x.toggle_fullscreen_cb = sxfe_toggle_fullscreen;

  /* override */

  this->fe_xine_open  = this->fe.xine_open;
  this->fe_xine_play  = this->fe.xine_play;

  this->fe.xine_open  = sxfe_xine_open;
  this->fe.xine_play  = sxfe_xine_play;

  return (frontend_t*)this;
}

/* ENTRY POINT */
const fe_creator_f fe_creator __attribute__((visibility("default"))) = sxfe_get_frontend;



