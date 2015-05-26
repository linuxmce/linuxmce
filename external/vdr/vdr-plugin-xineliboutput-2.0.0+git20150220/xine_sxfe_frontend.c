/*
 * xine_sxfe_frontend.c: Simple front-end, X11 functions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_sxfe_frontend.c,v 1.209 2014-07-01 19:17:14 phintuka Exp $
 *
 */

/*#define HAVE_XF86VIDMODE*/
#include "features.h"

#include <poll.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <float.h>  /* DBL_MIN */

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
#ifdef HAVE_XSHAPE
#  include <X11/extensions/shape.h>
#endif
#ifdef HAVE_XDPMS
#  include <X11/extensions/dpms.h>
#endif
#ifdef HAVE_XINERAMA
#  include <X11/extensions/Xinerama.h>
#endif
#ifdef HAVE_XRANDR
#  include <X11/extensions/Xrandr.h>
#endif

#ifdef HAVE_OPENGL
#  include <GL/glx.h>
#  ifdef HAVE_DLFCN
#    include <dlfcn.h>
#  endif
#  ifdef HAVE_PTHREAD
#    include <pthread.h>
#  endif
#endif

#include <xine.h>

#define LOG_MODULENAME "[vdr-sxfe]  "
#include "logdefs.h"

#include "xine_frontend_internal.h"

#ifdef HAVE_DBUS_GLIB_1
#  include "tools/gnome_screensaver.h"
#endif

#include "tools/osd_command.h"
#include "tools/rle.h"
#include "tools/time_ms.h"

#ifndef WIN_LAYER_NORMAL
  #define WIN_LAYER_NORMAL 4
#endif
#ifndef WIN_LAYER_ONTOP
  #define WIN_LAYER_ONTOP  6
#endif

#define MWM_HINTS_DECORATIONS       (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS     5
typedef struct _mwmhints {
  unsigned long flags;
  unsigned long functions;
  unsigned long decorations;
  long          input_mode;
  unsigned long status;
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
  Window   root_window;
  Window   window[2];
  int      screen;
  int      window_id;        /* output to another window */
#ifdef HAVE_XSHM
  int      xshm_completion_event;
#endif
  Time     prev_click_time; /* time of previous mouse button click (grab double clicks) */
  int      mousecursor_timeout;
#ifdef HAVE_MCE_DBUS_NAMES
  int      mce_blank_prevent_timer;
#endif
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
  uint8_t  gui_hotkeys : 1;
  uint8_t  no_x_kbd : 1;
  uint8_t  touchscreen : 1;

  /* OSD Video Window */
  pthread_mutex_t video_win_mutex;
  uint8_t  video_win_active; /* activate video window? */
  uint8_t  video_win_changed;
  uint16_t video_win_x;      /* video window position, x */
  uint16_t video_win_y;      /* video window position, y */
  uint16_t video_win_w;      /* video window width */
  uint16_t video_win_h;      /* video window height */

  /* OSD */
#if defined(HAVE_XRENDER) || defined(HAVE_OPENGL)
  uint8_t       osd_visible;
  uint16_t      osd_width;
  uint16_t      osd_height;
#endif

  /* OpenGL */
#ifdef HAVE_OPENGL
  GLXDrawable     opengl_window;
  GLXContext      opengl_context;
  int             screen_width, screen_height;
  pthread_t       opengl_drawing_thread;
  pthread_mutex_t opengl_redraw_mutex;
  pthread_cond_t  opengl_redraw_cv;
  uint32_t        opengl_redraw_request_nr;
  uint32_t        opengl_redraw_served_nr;
  pthread_mutex_t opengl_redraw_finished_mutex;
  pthread_cond_t  opengl_redraw_finished_cv;
  pthread_mutex_t opengl_osd_texture_img_mutex;
  Pixmap          video_frame_pixmap;
  GC              video_frame_gc;
  GLuint          video_frame_texture;
  GLuint          osd_texture;
  uint32_t       *opengl_osd_texture_img;
  uint8_t         opengl_hud : 1;
  uint8_t         opengl_always : 1;
  uint8_t         opengl_osd_texture_img_updated : 1;
  uint8_t         opengl_deinit : 1;
#ifdef HAVE_XSHAPE
  uint8_t         opengl_xshape_available : 1;
#endif
#endif /* HAVE_OPENGL */

  /* HUD */
#ifdef HAVE_XRENDER
  uint8_t         hud;
  XImage         *hud_img;
  Visual         *hud_vis;
  Xrender_Surf   *surf_win;
  Xrender_Surf   *surf_img;
  uint32_t       *hud_img_mem;
  GC              gc;
  Window          hud_window;
# ifdef HAVE_XSHAPE
  uint8_t         xshape_hud : 1;
  Xrender_Surf   *surf_back_img;
  uint32_t       *shape_mask_mem;
  Pixmap          shape_mask_pixmap;
  GC              shape_mask_gc;
  Picture         shape_mask_picture;
# endif
# ifdef HAVE_XSHM
  XShmSegmentInfo hud_shminfo;
# endif
#endif /* HAVE_XRENDER */

} sxfe_t;


#ifdef HAVE_OPENGL
#if 0
typedef int (*GLXGetVideoSyncProc)  (unsigned int *count);
typedef int (*GLXWaitVideoSyncProc) (int          divisor,
                                     int          remainder,
                                     unsigned int *count);
#endif
typedef void    (*GLXBindTexImageProc)    (Display       *display,
                                           GLXDrawable   drawable,
                                           int           buffer,
                                           int           *attribList);
#if 0
GLXGetVideoSyncProc      getVideoSync;
GLXWaitVideoSyncProc     waitVideoSync;
#endif
GLXBindTexImageProc      bindTexImage;
#endif

#define DOUBLECLICK_TIME   500  // ms

#define OSD_DEF_WIDTH      720
#define OSD_DEF_HEIGHT     576
#define HUD_MAX_WIDTH      1920
#define HUD_MAX_HEIGHT     1200

#define MOUSECURSOR_TIMEOUT 2000  // 2 seconds

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

  if (!this->fe.fe_message_cb) { /* handler is set only in local mode */
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
  XLockDisplay(dpy);
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
  XUnlockDisplay(dpy);
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
  XLockDisplay (this->display);

  this->x.width  = DisplayWidth(this->display, this->screen);
  this->x.height = DisplayHeight(this->display, this->screen);

  XUnlockDisplay(this->display);

  update_xinerama_info(this);
}

/*
 * Generic OSD
 */
#if defined(HAVE_XRENDER) || defined(HAVE_OPENGL)

static void osd_fill_lut8(uint32_t* dst, int dst_pitch, int argb,
                          const struct osd_command_s *cmd)
{
  uint8_t *data = cmd->raw_data;
  unsigned x, y;
  uint32_t lut[256];

  if (argb) {
    rle_palette_to_argb(lut, cmd->palette, cmd->colors);
  } else {
    rle_palette_to_rgba(lut, cmd->palette, cmd->colors);
  }

  dst += cmd->y * dst_pitch + cmd->x;

  for (y = cmd->h; y; y--) {
    for (x = 0; x < cmd->w; x++) {
      dst[x] = lut[*data];
      ++data;
    }
    dst += dst_pitch;
  }
}

static void osd_set_video_window(sxfe_t *this, const struct osd_command_s *cmd)
{
  LOGDBG("unscaled video window: %d,%d %dx%d", cmd->x, cmd->y, cmd->w, cmd->h);

  // Compute the coordinates of the video window
  double scale_x = (double)this->x.width  / (double)this->osd_width;
  double scale_y = (double)this->x.height / (double)this->osd_height;

  int x = cmd->x;
  int y = cmd->y;
  int w = cmd->w;
  int h = cmd->h;

  x = (int)ceil((double)(x>0 ? x-1 : 0) * scale_x);
  y = (int)ceil((double)(y>0 ? y-1 : 0) * scale_y);
  w = (int)floor((double)(w+2) * scale_x);
  h = (int)floor((double)(h+2) * scale_y);

  pthread_mutex_lock(&this->video_win_mutex);

  if (x != this->video_win_x || y != this->video_win_y ||
      w != this->video_win_w || h != this->video_win_h)
    this->video_win_changed = 1;

  this->video_win_x = x;
  this->video_win_y = y;
  this->video_win_w = w;
  this->video_win_h = h;

  this->video_win_active = 1;

  pthread_mutex_unlock(&this->video_win_mutex);

  LOGDBG("scaled video window: %d,%d %dx%d", this->video_win_x, this->video_win_y, this->video_win_w, this->video_win_h);
}

static int osd_command(sxfe_t *this, struct osd_command_s *cmd)
{
  switch(cmd->cmd) {

    case OSD_Size: /* Set size of VDR OSD area */
      if (!(cmd->flags & OSDFLAG_TOP_LAYER))
        break;

      LOGVERBOSE("OSD Size %dx%d", cmd->w, cmd->h);
      this->osd_width  = (cmd->w > 0) ? cmd->w : OSD_DEF_WIDTH;
      this->osd_height = (cmd->h > 0) ? cmd->h : OSD_DEF_HEIGHT;
      break;

    case OSD_VideoWindow:
      LOGVERBOSE("OSD VideoWindow %dx%d@%d,%d", cmd->w, cmd->h, cmd->x, cmd->y);
      osd_set_video_window(this, cmd);
      break;

    default:
      break;
  }
  return 1;
}
#endif /* HAVE_XRENDER || HAVE_OPENGL */

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
                               XDouble scale_x, XDouble scale_y, int smooth,
                               int *new_x, int *new_y, int *new_w, int *new_h)
{
  XTransform xf;

  if (scale_x <= 2.0 * DBL_MIN)
    scale_x = 1;
  if (scale_y <= 2.0 * DBL_MIN)
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

  *new_x = x;
  *new_y = y;
  *new_w = w;
  *new_h = h;
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

static void hud_fill_argb(uint32_t* dst, int dst_pitch,
                          const struct osd_command_s *cmd)
{
  uint32_t *data = (uint32_t*)cmd->raw_data;
  unsigned y;

  dst += cmd->y * dst_pitch + cmd->x;

  for (y = cmd->h; y; y--) {
    memcpy(dst, data, cmd->w * sizeof(uint32_t));
    data += cmd->w;
    dst += dst_pitch;
  }
}

static void update_mask(uint32_t* dst, int dst_pitch,
                        uint32_t* mask, int mask_pitch,
                        int *mask_changed, const struct osd_command_s *cmd)
{
  uint x, y;
  *mask_changed = 0;

  dst  += dst_pitch  * cmd->y + cmd->x;
  mask += mask_pitch * cmd->y + cmd->x;

  for (y = 0; y < cmd->h; y++) {
    for (x = 0; x < cmd->w; x++) {
      if (mask[x] != dst[x]) {
        *mask_changed = 1;
        mask[x] = dst[x];
      }
    }
    dst  += dst_pitch;
    mask += mask_pitch;
  }
}

static void hud_fill_img_memory(uint32_t* dst, int dst_pitch,
                                uint32_t* mask, int mask_pitch,
                                int *mask_changed, const struct osd_command_s *cmd)
{
  if (cmd->cmd == OSD_Set_LUT8) {
    osd_fill_lut8(dst, dst_pitch, 1, cmd);

  } else if (cmd->cmd == OSD_Set_ARGB) {
    hud_fill_argb(dst, dst_pitch, cmd);

  } else if (cmd->cmd == OSD_Set_RLE) {
    rle_uncompress_argb(dst + cmd->y * dst_pitch + cmd->x,
                        cmd->w, cmd->h, dst_pitch,
                        cmd->data, cmd->num_rle,
                        cmd->palette, cmd->colors);

  } else {
    LOGMSG("hud_fill_img_memory(): unsupported format");
    return;
  }

  if (mask && mask_changed) {
    update_mask(dst, dst_pitch, mask, mask_pitch, mask_changed, cmd);
  }
}

static void hud_osd_draw(sxfe_t *this, const struct osd_command_s *cmd)
{
  int x = cmd->x + cmd->dirty_area.x1;
  int y = cmd->y + cmd->dirty_area.y1;
  int w = cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1;
  int h = cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1;

  XDouble scale_x  = (XDouble)this->x.width  / (XDouble)this->osd_width;
  XDouble scale_y  = (XDouble)this->x.height / (XDouble)this->osd_height;
  int     mask_changed;

#ifdef HAVE_XSHAPE
  Xrender_Surf *dst_surf = this->surf_back_img ? this->surf_back_img       : this->surf_win;
  Window        dst_win  = this->surf_back_img ? this->surf_back_img->draw : this->hud_window;
  uint32_t     *shape_mask_mem = this->shape_mask_mem;
#else
  Xrender_Surf *dst_surf = this->surf_win;
  Window        dst_win  = this->hud_window;
  uint32_t     *shape_mask_mem = NULL;
#endif

#ifdef HAVE_XSHM
  if (this->xshm_completion_event != -1) {
    hud_fill_img_memory((uint32_t*)(this->hud_img->data), HUD_MAX_WIDTH,
                        shape_mask_mem, HUD_MAX_WIDTH,
                        &mask_changed, cmd);
    if (!cmd->scaling) {
      /* Place image directly onto hud window */
      XShmPutImage(this->display, dst_win, this->gc, this->hud_img,
                   x, y, x, y, w, h,
                   False);
    } else {
      /* Place image onto Xrender surface which will be blended onto hud window */
      XShmPutImage(this->display, this->surf_img->draw, this->gc, this->hud_img,
                   x, y, x, y, w, h,
                   False);
      xrender_surf_blend(this->display, this->surf_img, dst_surf,
                         x, y, w, h,
                         scale_x, scale_y,
                         (cmd->scaling & 2),  // Note: HUD_SCALING_BILINEAR=2
                         &x, &y, &w, &h);
    }
  }
  else
#endif
    {
      hud_fill_img_memory(this->hud_img_mem, HUD_MAX_WIDTH,
                          shape_mask_mem, HUD_MAX_WIDTH,
                          &mask_changed, cmd);
      if (!cmd->scaling) {
        /* Place image directly onto hud window (always unscaled) */
        XPutImage(this->display, dst_win, this->gc, this->hud_img,
                  x, y, x, y, w, h);
      } else {
        /* Place image onto Xrender surface which will be blended onto hud window */
        XPutImage(this->display, this->surf_img->draw, this->gc, this->hud_img,
                  x, y, x, y, w, h);
        xrender_surf_blend(this->display, this->surf_img, dst_surf,
                           x, y, w, h,
                           scale_x, scale_y,
                           (cmd->scaling & 2),   // Note: HUD_SCALING_BILINEAR=2
                           &x, &y, &w, &h);
      }
    }

#ifdef HAVE_XSHAPE
  if (this->xshape_hud) {
    if (mask_changed) {
      LOGDBG("hud_osd_draw: Updating shape of window");
      XRenderComposite(this->display, PictOpSrc, this->surf_back_img->pic, None, this->shape_mask_picture,
                       x, y, 0, 0, x, y, w, h);
      XShapeCombineMask(this->display, this->hud_window, ShapeBounding, 0, 0, this->shape_mask_pixmap, ShapeSet);
    }
  }

  /* Put the image onto the hud window */
  if (this->surf_back_img)
    XRenderComposite(this->display, PictOpSrc, this->surf_back_img->pic, None, this->surf_win->pic,
                     x, y, 0, 0, x, y, w, h);
#endif

  XFlush(this->display);
}

static void hud_osd_hide(sxfe_t *this)
{
  if (!this->osd_visible)
    return;

  this->osd_visible = 0;
  this->video_win_active = 0;

#ifdef HAVE_XSHAPE
  if (this->xshape_hud) {
    XUnmapWindow(this->display, this->hud_window);
    XSetForeground(this->display, this->shape_mask_gc, 0);
    XFillRectangle(this->display, this->shape_mask_pixmap, this->shape_mask_gc, 0, 0, this->x.width, this->x.height);
    memset(this->shape_mask_mem, 0, sizeof(uint32_t) * HUD_MAX_WIDTH * HUD_MAX_HEIGHT);
    XShapeCombineMask(this->display, this->hud_window, ShapeBounding, 0, 0, this->shape_mask_pixmap, ShapeSet);
  }
#endif

  XSetForeground(this->display, this->gc, 0x00000000);
  XFillRectangle(this->display, this->hud_window, this->gc,
                 0, 0, this->x.width, this->x.height);
  XFillRectangle(this->display, this->surf_img->draw, this->gc,
                 0, 0, this->osd_width+2, this->osd_height+2);

  XFlush(this->display);
}

static void hud_osd_show(sxfe_t *this)
{
  if (this->osd_visible)
    return;

  this->osd_visible = 1;
  this->video_win_active = 0;

  XSetForeground(this->display, this->gc, 0x00000000);
  XFillRectangle(this->display, this->surf_img->draw, this->gc,
                 0, 0, this->osd_width+2, this->osd_height+2);

#ifdef HAVE_XSHAPE
  if (this->surf_back_img)
    XFillRectangle(this->display, this->surf_back_img->draw, this->gc, 0, 0, this->x.width, this->x.height);

  if (this->xshape_hud) {
    XSetForeground(this->display, this->shape_mask_gc, 0);
    XFillRectangle(this->display, this->shape_mask_pixmap, this->shape_mask_gc, 0, 0, this->x.width, this->x.height);
    memset(this->shape_mask_mem, 0, sizeof(uint32_t) * HUD_MAX_WIDTH * HUD_MAX_HEIGHT);
    XShapeCombineMask(this->display, this->hud_window, ShapeBounding, 0, 0, this->shape_mask_pixmap, ShapeSet);
    XRaiseWindow(this->display, this->hud_window);
    XMapWindow(this->display, this->hud_window);
  }
#endif

  XFlush(this->display);
}

static int hud_osd_command(frontend_t *this_gen, struct osd_command_s *cmd)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  if (this && cmd)
  if (this->hud) {

    osd_command(this, cmd);

    if (!(cmd->flags & OSDFLAG_TOP_LAYER))
      return 1;

    XLockDisplay(this->display);
    switch (cmd->cmd) {
    case OSD_Nop: /* Do nothing ; used to initialize delay_ms counter */
      LOGVERBOSE("HUD OSD NOP");
      break;

    case OSD_Size: /* Set size of VDR OSD area */
      LOGVERBOSE("HUD OSD Size");
      hud_osd_show(this);
      break;

    case OSD_Set_LUT8:
    case OSD_Set_ARGB:
    case OSD_Set_RLE: /* Create/update OSD window. Data is rle-compressed. */
      LOGVERBOSE("HUD OSD Set");
      hud_osd_draw(this, cmd);
      break;

    case OSD_SetPalette: /* Modify palette of already created OSD window */
      LOGDBG("HUD OSD SetPalette");
      break;

    case OSD_Move:       /* Change x/y position of already created OSD window */
      LOGDBG("HUD OSD Move");
      break;

    case OSD_Set_YUV:    /* Create/update OSD window. Data is in YUV420 format. */
      LOGDBG("HUD OSD Set YUV");
      break;

    case OSD_VideoWindow:
      break;

    case OSD_Close: /* Close OSD window */
      LOGVERBOSE("HUD OSD Close");
      hud_osd_hide(this);
      break;

    default:
      break;
    }
    XUnlockDisplay(this->display);
  }
  return 1;
}

static void hud_frame_output_cb (void *data,
                                  int video_width, int video_height,
                                  double video_pixel_aspect,
                                  int *dest_x, int *dest_y,
                                  int *dest_width, int *dest_height,
                                  double *dest_pixel_aspect,
                                  int *win_x, int *win_y)
{
  sxfe_t *this = (sxfe_t*)data;

  /* Call the original handler */
  this->x.frame_output_handler(data,
                               video_width, video_height,
                               video_pixel_aspect,
                               dest_x, dest_y,
                               dest_width, dest_height,
                               dest_pixel_aspect,
                               win_x, win_y);

  /* Set the desitination position if the video window is active */
  if (this->video_win_active) {

    pthread_mutex_lock(&this->video_win_mutex);

    /* Clear the window if the size has changed */
    if (this->video_win_changed) {
      Window win = this->window[!!this->fullscreen];
      GC     gc  = XCreateGC(this->display, win, 0, NULL);
      XSetForeground(this->display, gc, 0x00000000);
      XFillRectangle(this->display, win, gc, 0, 0, this->x.width-1, this->x.height-1);
      XFreeGC(this->display, gc);
    }

    *dest_x = this->video_win_x;
    *dest_y = this->video_win_y;
    *dest_width  = this->video_win_w;
    *dest_height = this->video_win_h;

    this->video_win_changed = 0;

    pthread_mutex_unlock(&this->video_win_mutex);
  }
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

    this->hud_vis = find_argb_visual(this->display, this->screen);
    if(!this->hud_vis) {
      LOGMSG("find_argb_visual() failed. HUD OSD disabled.");
      this->hud = 0;
      XUnlockDisplay(this->display);
      return 1;
    }

    Colormap hud_colormap = XCreateColormap(this->display,
                                            this->root_window,
                                            this->hud_vis, AllocNone);

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    attributes.background_pixel = 0x00000000;
    attributes.border_pixel = 0;
    attributes.colormap = hud_colormap;
    attributes.backing_store = Always;

    this->hud_window = XCreateWindow(this->display, this->root_window,
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
    if(this->xshm_completion_event != -1) {
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

#ifdef HAVE_XSHAPE
    if (this->xshape_hud) {

      // Check if extension is available
      if (XShapeQueryExtension(this->display, &dummy, &dummy)) {

        // Create the pixmap
        this->shape_mask_pixmap = XCreatePixmap(this->display, this->hud_window,
                                                DisplayWidth(this->display, this->screen),
                                                DisplayHeight(this->display, this->screen), 1);
        this->shape_mask_mem = calloc(sizeof(uint32_t), HUD_MAX_WIDTH * HUD_MAX_HEIGHT);
        this->shape_mask_gc = XCreateGC(this->display, this->shape_mask_pixmap, 0, NULL);
        XSetForeground(this->display, this->shape_mask_gc, 0);
        XFillRectangle(this->display, this->shape_mask_pixmap, this->shape_mask_gc, 0, 0,
                       DisplayWidth(this->display, this->screen), DisplayHeight(this->display, this->screen));
        this->shape_mask_picture = XRenderCreatePicture(this->display, this->shape_mask_pixmap,
                                                        XRenderFindStandardFormat(this->display, PictStandardA1),
                                                        0, NULL);
        XShapeCombineMask(this->display, this->hud_window, ShapeBounding, 0, 0, this->shape_mask_pixmap, ShapeSet);

     } else {
        LOGMSG("hud_osd_open(): XShape not available, composite window manager must be running to see OSD.");
        this->xshape_hud = 0;
     }
    }
#endif
    this->osd_visible = 0;

    this->surf_win = xrender_surf_adopt(this->display, this->hud_window, this->hud_vis, HUD_MAX_WIDTH, HUD_MAX_HEIGHT);
    this->surf_img = xrender_surf_new(this->display, this->hud_window, this->hud_vis, HUD_MAX_WIDTH, HUD_MAX_HEIGHT, 1);

#ifdef HAVE_XSHAPE
    if (this->xshape_hud)
      this->surf_back_img = xrender_surf_new(this->display, this->hud_window, this->hud_vis,
                                             DisplayWidth(this->display, this->screen),
                                             DisplayHeight(this->display, this->screen), 1);
#endif

    XUnlockDisplay(this->display);

    this->fe.xine_osd_command = hud_osd_command;

    if (this->x.scale_video) {
      // Enable this if you want to have video window support (e.g., for yaepghd)
      // However, vdpau does not support this. Works ok for xv
      this->x.vis_x11.frame_output_cb  = hud_frame_output_cb;
    }

    return 1;
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
                            this->root_window,
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

      if (fev->type == FocusIn) {
        /* Show HUD again if sxfe window receives focus */
        XMapWindow(this->display, this->hud_window);
      }

      else if (fev->type == FocusOut) {
        /* Dismiss HUD window if focusing away from frontend window */
        XUnmapWindow(this->display, this->hud_window);
      }

      XUnlockDisplay(this->display);
    }
}

static void hud_osd_close(sxfe_t *this)
{
  if(this && this->hud) {
    XLockDisplay(this->display);
    LOGDBG("closing hud window...");

#ifdef HAVE_XSHM
    if(this->xshm_completion_event != -1) {
      XShmDetach(this->display, &(this->hud_shminfo));
      XDestroyImage(this->hud_img);
      shmdt(this->hud_shminfo.shmaddr);
      shmctl(this->hud_shminfo.shmid, IPC_RMID, 0);
    }
    else
#endif
      XDestroyImage(this->hud_img);

#ifdef HAVE_XSHAPE
    if (this->shape_mask_picture)
      XRenderFreePicture(this->display, this->shape_mask_picture);
    if (this->shape_mask_pixmap)
      XFreePixmap(this->display, this->shape_mask_pixmap);
    free(this->shape_mask_mem);

    if (this->surf_back_img)
      xrender_surf_free(this->display, this->surf_back_img);
#endif

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
  if (video_port && *video_port) {
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
  uint      i;
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
  XSetWindowAttributes xswa;
  unsigned long        xswa_mask;

  xswa.background_pixel = 0x00000000;
  xswa.border_pixel     = 0;
  xswa.backing_store    = WhenMapped;

  //xswa_mask = CWBackPixel | CWBorderPixel | CWBackingStore;
  xswa_mask = CWBorderPixel | CWBackingStore;

  XLockDisplay(this->display);

  /* create and display our video window */

  this->window[0] = XCreateWindow (this->display, this->root_window,
                                   this->x.xpos, this->x.ypos,
                                   this->x.width, this->x.height, 1,
                                   CopyFromParent, InputOutput, CopyFromParent,
                                   xswa_mask, &xswa);
  this->window[1] = XCreateWindow (this->display, this->root_window,
                                   this->xinerama_x, this->xinerama_y,
                                   this->x.width, this->x.height, 0,
                                   CopyFromParent, InputOutput, CopyFromParent,
                                   xswa_mask, &xswa);

  /* full-screen window */
  set_fullscreen_props(this);

  /* Window hint */
  XClassHint *classHint = XAllocClassHint();
  if (classHint) {
    classHint->res_name = "VDR";
    classHint->res_class = "VDR";
    XSetClassHint(this->display, this->window[0], classHint);
    XSetClassHint(this->display, this->window[1], classHint);
    XFree(classHint);
  }

  /* Window name */
  const char *initial_title = (!this->fe.fe_message_cb) ? "Connecting to VDR ..." : "Local VDR";
  XStoreName(this->display, this->window[0], initial_title);
  XStoreName(this->display, this->window[1], initial_title);

  /* Icon */
  set_icon(this);
  XUnlockDisplay(this->display);
}

#ifdef HAVE_OPENGL

/*
 * OpenGL OSD
 */

static void opengl_fill_argb(uint32_t* dst, int dst_pitch,
                             const struct osd_command_s *cmd)
{
  int x0 = cmd->dirty_area.x1;
  int y0 = cmd->dirty_area.y1;
  int w = cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1;
  int h = cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1;
  int x, y;

  uint32_t *src = (uint32_t*)cmd->raw_data;

  dst += (cmd->y + y0) * dst_pitch + cmd->x + x0;
  src += y0 * cmd->w + x0;

  for (y = h; y; y--){
    for (x = 0; x < w; x++) {
      uint32_t value = src[x];
      dst[x] = (value<<8)|((value>>24)&0xFF);
    }
    src += cmd->w;
    dst += dst_pitch;
  }
}

static void opengl_osd_draw(sxfe_t *this, const struct osd_command_s *cmd)
{
  // Copy the image to the texture and inform the opengl thread
  pthread_mutex_lock(&this->opengl_osd_texture_img_mutex);

  uint32_t *dst = this->opengl_osd_texture_img;

  switch (cmd->cmd) {
    case OSD_Set_LUT8:
      osd_fill_lut8(dst, this->osd_width, 0, cmd);
      break;

    case OSD_Set_ARGB:
      opengl_fill_argb(dst, this->osd_width, cmd);
      break;

    case OSD_Set_RLE:
      rle_uncompress_rgba(dst + cmd->y * this->osd_width + cmd->x,
                          cmd->w, cmd->h, this->osd_width,
                          cmd->data, cmd->num_rle,
                          cmd->palette, cmd->colors);
      break;

    default:
      LOGMSG("opengl_fill_img_memory(): unsupported format");
      break;
  }

  this->opengl_osd_texture_img_updated = 1;
  pthread_mutex_unlock(&this->opengl_osd_texture_img_mutex);
}

static void opengl_osd_hide(sxfe_t *this)
{
  if (!this->osd_visible)
    return;

  this->osd_visible = 0;
  this->video_win_active = 0;
}

static void opengl_osd_show(sxfe_t *this)
{
  if (this->osd_visible)
    return;

  pthread_mutex_lock(&this->opengl_osd_texture_img_mutex);

  this->osd_visible = 1;
  this->video_win_active = 0;

  free(this->opengl_osd_texture_img);
  size_t size = sizeof(uint32_t) * this->osd_width * this->osd_height;
  this->opengl_osd_texture_img = malloc(size);
  memset(this->opengl_osd_texture_img, 0, size);
  this->opengl_osd_texture_img_updated = 1;

  pthread_mutex_unlock(&this->opengl_osd_texture_img_mutex);

  XFlush(this->display);
}

static int opengl_osd_command(frontend_t *this_gen, struct osd_command_s *cmd)
{
  sxfe_t *this = (sxfe_t*)this_gen;

  if (this && cmd)
  if (this->opengl_always || this->opengl_hud) {

    osd_command(this, cmd);

    if (!(cmd->flags & OSDFLAG_TOP_LAYER))
      return 1;

    XLockDisplay(this->display);
    switch (cmd->cmd) {
      case OSD_Size: /* Set size of VDR OSD area */
        LOGDBG("OpenGL OSD Size");
        opengl_osd_show(this);
        break;

      case OSD_Set_LUT8:
      case OSD_Set_ARGB:
      case OSD_Set_RLE: /* Create/update OSD window. Data is rle-compressed. */
        LOGDBG("OpenGL OSD Set");
        opengl_osd_draw(this, cmd);
        break;

      case OSD_Close: /* Close OSD window */
        LOGDBG("OpenGL OSD Close");
        opengl_osd_hide(this);
        break;

      default:
        break;
    }
    XUnlockDisplay(this->display);
  }
  return 1;
}

/*
 * OpenGL
 */

/*
 * Signals a change to the opengl drawing thread
 */
void opengl_trigger_drawing_thread(sxfe_t *this)
{
  pthread_mutex_lock(&this->opengl_redraw_mutex);
  this->opengl_redraw_request_nr++;
  pthread_cond_signal(&this->opengl_redraw_cv);
  pthread_mutex_unlock(&this->opengl_redraw_mutex);
}

/*
 * Wait until drawing is finished
 */
void opengl_wait_drawing_finished(sxfe_t *this)
{
  pthread_mutex_lock(&this->opengl_redraw_finished_mutex);
  if (this->opengl_redraw_request_nr != this->opengl_redraw_served_nr)
    pthread_cond_wait(&this->opengl_redraw_finished_cv, &this->opengl_redraw_finished_mutex);
  pthread_mutex_unlock(&this->opengl_redraw_finished_mutex);
}

static void opengl_frame_output_cb (void *data,
                                    int video_width, int video_height,
                                    double video_pixel_aspect,
                                    int *dest_x, int *dest_y,
                                    int *dest_width, int *dest_height,
                                    double *dest_pixel_aspect,
                                    int *win_x, int *win_y)
{
  sxfe_t *this = (sxfe_t*)data;

  /* Inform the opengl drawing thread */
  opengl_trigger_drawing_thread(this);

  /* Wait until the thrad is finished */
  opengl_wait_drawing_finished(this);

  /* Call the original handler */
  this->x.frame_output_handler(data,
                               video_width, video_height,
                               video_pixel_aspect,
                               dest_x, dest_y,
                               dest_width, dest_height,
                               dest_pixel_aspect,
                               win_x, win_y);
}

#if 0
static void time_measure_start(struct timeval *time_start)
{
    gettimeofday(time_start, NULL);
}

static int time_measure_end(struct timeval *time_start)
{
    struct timeval time_end;
    int diff;
    gettimeofday(&time_end, NULL);
    diff = time_end.tv_usec - time_start->tv_usec;
    if (time_end.tv_usec < time_start->tv_usec) {
      diff += 1000000;
    }
    return diff;
}
#endif

static int opengl_init_dl(sxfe_t *this)
{
  void *dlhand;

  // Get handles to important functions
  dlhand = dlopen (NULL, RTLD_LAZY);
  if (!dlhand) {
    LOGERR("opengl_init(): dlopen failed (%s)", dlerror());
    return -1;
  }

  void *(*glXGetProcAddressARB)(unsigned char *) = NULL;
  glXGetProcAddressARB = (void *(*)(unsigned char *))dlsym(dlhand, "glXGetProcAddressARB");

  if (glXGetProcAddressARB) {

#if 0	 
    if (!(waitVideoSync = (void *)glXGetProcAddressARB((unsigned char *)"glXWaitVideoSyncSGI"))) {
      LOGMSG("glXGetProcAddressARB(glXWaitVideoSyncSGI) failed");
      goto error;
    }
    if (!(getVideoSync = (void *)glXGetProcAddressARB((unsigned char *)"glXGetVideoSyncSGI"))) {
      LOGMSG("glXGetProcAddressARB(glXGetVideoSyncSGI) failed");
      goto error;
    }
#endif
    if (!(bindTexImage = (void *)glXGetProcAddressARB((unsigned char *)"glXBindTexImageEXT"))) {
      LOGMSG("glXGetProcAddressARB(glXBindTexImageEXT) failed");
      goto error;
    }

  } else {

    LOGMSG("glXGetProcAddressARB not found");

    dlerror();
    bindTexImage = dlsym(dlhand,"glXBindTexImageEXT");
    if (dlerror() != NULL) {
      LOGMSG("opengl_init(): can not get pointer to glXBindTexImageEXT");
      goto error;
    }
#if 0
    getVideoSync = dlsym(dlhand,"glXGetVideoSyncSGI");
    if (dlerror() != NULL) {
      LOGMSG("opengl_init(): can not get pointer to glXGetVideoSyncSGI");
      goto error;
    }
    waitVideoSync = dlsym(dlhand,"glXWaitVideoSyncSGI");
    if (dlerror() != NULL) {
      LOGMSG("opengl_init(): can not get pointer to glXWaitVideoSyncSGI");
      goto error;
    }
#endif
  }

  dlclose(dlhand);
  return 0;

 error:
  dlclose(dlhand);
  return -1;
}

static int opengl_init(sxfe_t *this)
{
  int glx_major, glx_minor;
  int n;
  GLXFBConfig *fbconfigs;
  GLXFBConfig fbcroot;
  XSetWindowAttributes attr;
  XVisualInfo *visinfo;
  const char* gl_version;
  int gl_major, gl_minor;
  static const int fbc_attr[] = {
    GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT | GLX_WINDOW_BIT,
    GLX_BIND_TO_TEXTURE_TARGETS_EXT, GLX_TEXTURE_2D_BIT_EXT,
    GLX_BIND_TO_TEXTURE_RGBA_EXT, True,
    GLX_DOUBLEBUFFER,  True,
    GLX_DEPTH_SIZE,    0,
    GLX_ALPHA_SIZE,    0,
    GLX_RED_SIZE,      1,
    GLX_GREEN_SIZE,    1,
    GLX_BLUE_SIZE,     1,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    None
  };
  int fbc_attr2[] = {
    GLX_RGBA,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_DOUBLEBUFFER, 1,
    None
  };
  static const int pixmapAttribs[] = {
    GLX_TEXTURE_TARGET_EXT,
    GLX_TEXTURE_2D_EXT,
    GLX_TEXTURE_FORMAT_EXT,
    GLX_TEXTURE_FORMAT_RGBA_EXT,
    None
  };
  GLXPixmap glxpixmap;
#if 0
  int frame_period_average = 0;
  int frame_period;
  const int frame_period_required_samples = 100;
  struct timeval t;
  int i;
  int unsigned sync;
#endif
  const char *glxExtensions;

  // Get handles to important functions
  if (opengl_init_dl(this) < 0) {
    return -1;
  }

  // Get the GLX version
  LOGVERBOSE("Get GLX version ...");
  if (!glXQueryVersion (this->display, &glx_major, &glx_minor)) {
    LOGMSG("no GLX support");
    return -1;
  }
  LOGVERBOSE("GLX %d.%d", glx_major, glx_minor);

  // Should have full GLX 1.3 for GL 1.2,
  if (!(glx_major > 1 || glx_minor > 2)) {
    const char* exts = glXQueryExtensionsString (this->display, this->screen);
    if (!exts || !strstr (exts, "GLX_SGIX_fbconfig")) {
      LOGMSG("no glx fbconfig support");
      return -1;
    }
  }
  LOGVERBOSE("Found GLX fbconfig support");

  if (!(glx_major > 1 || glx_minor > 2)) {
      LOGMSG("no glx 1.3 support");
      return -1;
  }

  // Check for important extension
  glxExtensions = glXQueryExtensionsString (this->display, this->screen);
  if (!strstr (glxExtensions, "GLX_EXT_texture_from_pixmap")) {
    LOGMSG("No texture from pixmap extension");
    return -1;
  }
  LOGVERBOSE("Found texture from pixmap extension");
#if 0
  if (!strstr (glxExtensions, "GLX_SGI_video_sync")) {
    LOGMSG("No sgi video sync extension");
    return -1;
  }
  LOGVERBOSE("Found sgi video sync extension");
#endif

  // Get handles to important functions
  if (opengl_init_dl(this) < 0) {
    return -1;
  }

  // Get properties of the root window
  this->screen_width = DisplayWidth (this->display, this->screen);
  this->screen_height = DisplayHeight (this->display, this->screen);

  // Create the opengl window
  if (!(fbconfigs = glXChooseFBConfig(this->display, this->screen, fbc_attr, &n))) {
    LOGMSG("No glx frame buffer");
    return -1;
  }

  fbcroot = fbconfigs[0];
  XFree (fbconfigs);
  visinfo = glXChooseVisual(this->display, this->screen, fbc_attr2);
  attr.colormap = XCreateColormap (this->display, this->root_window, visinfo->visual, AllocNone);
  attr.override_redirect = True;
  this->opengl_window = XCreateWindow (this->display, this->root_window,
                                       0, 0, this->screen_width, this->screen_height, 0,
                                       visinfo->depth, InputOutput,
                                       visinfo->visual, CWColormap | CWOverrideRedirect, &attr);
  XSelectInput(this->display, this->opengl_window,
               StructureNotifyMask |
               ExposureMask |
               KeyPressMask |
               ButtonPressMask |
               FocusChangeMask);
#ifdef HAVE_XSHAPE
  if (this->xshape_hud)
    XShapeCombineRectangles(this->display, this->opengl_window, ShapeInput, 0, 0, NULL, 0,
                            ShapeSet, Unsorted);
#endif
  this->opengl_context = glXCreateContext(this->display, visinfo, None, GL_TRUE);
  XFree (visinfo);
  if (this->opengl_context == NULL) {
    LOGERR("Can't create glx context");
    return -1;
  }

  glXMakeCurrent(this->display, this->opengl_window, this->opengl_context);
  if (!(gl_version = (const char*) glGetString (GL_VERSION))) {
    LOGERR("Failed to initialize GL");
    return -1;
  }
  sscanf (gl_version, "%d.%d", &gl_major, &gl_minor);
  if (!(gl_major > 1 || gl_minor > 1)) {
    LOGMSG("No GL 1.2 support on your platform");
    return -1;
  }

  if (!(gl_major > 1 || gl_minor > 2)) {
    LOGMSG("No GL 1.3 support on your platform");
    return -1;
  }

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0, this->screen_width-1, 0, this->screen_height-1, 0, 65535);
  glEnable(GL_TEXTURE_2D);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  // Hide cursor
  set_cursor(this->display, this->opengl_window, 0);

  // Associate video and hud with a texture
  this->video_frame_pixmap = XCreatePixmap(this->display, this->opengl_window,
                                           this->screen_width, this->screen_height,
                                           DefaultDepth(this->display, this->screen));
  this->video_frame_gc = XCreateGC(this->display, this->video_frame_pixmap, 0, NULL);
  glxpixmap = glXCreatePixmap (this->display, fbcroot, this->video_frame_pixmap, pixmapAttribs);
  glGenTextures (1, &this->video_frame_texture);
  glBindTexture (GL_TEXTURE_2D, this->video_frame_texture);
  bindTexImage (this->display, glxpixmap, GLX_FRONT_LEFT_EXT, NULL);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenTextures (1, &this->osd_texture);
  glBindTexture (GL_TEXTURE_2D, this->osd_texture);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if 0
  // Detect the frame rate
  for (i = 0; i < frame_period_required_samples + 2; i++) {
    if (i > 1) {
      frame_period = time_measure_end(&t);
      frame_period_average += frame_period;
    }
    time_measure_start(&t);
    (*getVideoSync) (&sync);
    (*waitVideoSync) (2, (sync + 1) % 2, &sync);
  }
  frame_period_average /= frame_period_required_samples;
  this->opengl_frame_period = frame_period_average;
  LOGMSG("opengl_init(): average frame period is %d us", this->opengl_frame_period);
#endif

  return 0;
}

// Frees all opengl related resources
static void opengl_deinit(sxfe_t *this)
{
  XFreePixmap(this->display, this->video_frame_pixmap);
  glXDestroyContext(this->display, this->opengl_context);
  XDestroyWindow(this->display, this->opengl_window);
}

// Handles video output
static void *opengl_draw_frame_thread(void *arg)
{
  sxfe_t *this=(sxfe_t *) arg;
  //int unsigned sync;
  int draw_frame = 0, window_mapped = 0, keep_osd_open = 0;
  int prev_osd_visible = 0;
  int16_t video_x0, video_y0, video_x1, video_y1;
  int16_t osd_x0 ,osd_y0, osd_x1, osd_y1;
  GLfloat osd_alpha = 0;
  GLfloat osd_alpha_step = 0.2;
  static int unsigned count = 0;
  int16_t win_width = -1, win_height = -1;
  int16_t win_x = -1, win_y = -1;
#ifdef HAVE_XSHAPE
  XRectangle xrect;
#endif
  GLfloat video_tex_width, video_tex_height;
  int first_frame = 1;
  int force_redirect = 0;
  //struct timeval t;

  XLockDisplay (this->display);
  if (opengl_init(this) < 0) {
    LOGMSG("OpenGL initialization failed");
    XUnlockDisplay (this->display);
    exit(1);
  }
  XUnlockDisplay (this->display);

  while (1) {

    // Wait for trigger
    pthread_mutex_lock(&this->opengl_redraw_mutex);
    if (this->opengl_redraw_served_nr == this->opengl_redraw_request_nr) {
    pthread_cond_wait(&this->opengl_redraw_cv, &this->opengl_redraw_mutex);
    }
    pthread_mutex_unlock(&this->opengl_redraw_mutex);
    //time_measure_start(&t);

    // Check if we should exit
    if (this->opengl_deinit)
      break;

    // Check if we need to change the shape of the window
    if (win_x != this->x.xpos || win_y != this->x.ypos ||
        win_width != this->x.width || win_height != this->x.height) {

      // Update sizes
      win_x = this->x.xpos;
      win_y = this->x.ypos;
      win_width = this->x.width;
      win_height = this->x.height;
      force_redirect = 1;

      // Set the shape of the opengl window
#ifdef HAVE_XSHAPE
      xrect.x = win_x;
      xrect.y = win_y;
      xrect.width = win_width;
      xrect.height = win_height;
      if (this->opengl_xshape_available)
        XShapeCombineRectangles(this->display, this->opengl_window, ShapeBounding, 0, 0, &xrect, 1, ShapeSet, 0);
#endif      
    }
    LOGVERBOSE("win_x=%d win_y=%d win_width=%d win_height=%d", win_x, win_y, win_width, win_height);
    // Update the global alpha value of the OSD
    keep_osd_open = 0;
    if (this->osd_visible) {
      if (osd_alpha < 1.0) {
        osd_alpha += osd_alpha_step;
        if (osd_alpha > 1.0)
	  osd_alpha = 1.0;
      }
    } else {
      this->video_win_active = 0;
      if (osd_alpha > 0.0) {
        osd_alpha -= osd_alpha_step;
        if (osd_alpha < 0.0)
	  osd_alpha = 0.0;
	keep_osd_open = 1;
      }
    }
    LOGVERBOSE("osd_alpha=%.2f keep_osd_open=%d", osd_alpha, keep_osd_open);

    // Decide if we need to do something
    draw_frame = (this->osd_visible || window_mapped || this->opengl_always);
    if ((this->opengl_hud && this->osd_visible && (!prev_osd_visible || force_redirect)) ||
        (this->opengl_always && (first_frame || force_redirect))) {
      LOGDBG("redirecting video to opengl frame texture");
      xine_port_send_gui_data(this->x.video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
                              (void*) this->video_frame_pixmap);
      draw_frame = 0; // first frame not yet available in pixmap
      count = 0;
      osd_alpha -= 2*osd_alpha_step; // delay the osd
    }
    if (this->opengl_hud && !this->osd_visible && prev_osd_visible && !keep_osd_open && !this->opengl_always) {
      LOGDBG("redirecting video to window");
      xine_port_send_gui_data(this->x.video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
                              (void*) this->window[this->fullscreen ? 1 : 0]);
      count = 0;
    }
    if (draw_frame) {
      LOGVERBOSE("drawing frame nr %d", count);
      XLockDisplay(this->display);
      video_tex_width = 1.0;
      video_tex_height = 1.0;
      if (this->video_win_active) {
        pthread_mutex_lock(&this->video_win_mutex);
        video_x0 = win_x + this->video_win_x;
	video_y0 = win_y + this->video_win_y;
	video_tex_width  = ((GLfloat)win_width)  / (GLfloat)this->screen_width;
	video_tex_height = ((GLfloat)win_height) / (GLfloat)this->screen_height;
	video_x1 = video_x0 + (this->video_win_w - 1);
	video_y1 = video_y0 + (this->video_win_h - 1);
        pthread_mutex_unlock(&this->video_win_mutex);
      } else {
        video_x0 = win_x;
	video_y0 = win_y;
	video_x1 = video_x0 + this->screen_width - 1;
	video_y1 = video_y0 + this->screen_height - 1;
      }
      osd_x0 = win_x;
      osd_y0 = win_y;
      osd_x1 = osd_x0 + win_width - 1;
      osd_y1 = osd_y0 + win_height - 1;
      video_y0 = (this->screen_height - 1) - video_y0;
      video_y1 = (this->screen_height - 1) - video_y1;
      osd_y0 = (this->screen_height - 1) - osd_y0;
      osd_y1 = (this->screen_height - 1) - osd_y1;
      LOGVERBOSE("video_x0=%d video_y0=%d video_x1=%d video_y1=%d", video_x0, video_y0, video_x1, video_y1);
      LOGVERBOSE("osd_x0=%d osd_y0=%d osd_x1=%d osd_y1=%d", osd_x0, osd_y0, osd_x1, osd_y1);
      glClear(GL_COLOR_BUFFER_BIT);
      glBindTexture (GL_TEXTURE_2D, this->video_frame_texture);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glBegin(GL_QUADS);
        glTexCoord2f(0.0, video_tex_height); glVertex3f(video_x0, video_y1 , 0.0);
        glTexCoord2f(video_tex_width, video_tex_height); glVertex3f(video_x1, video_y1, 0.0);
        glTexCoord2f(video_tex_width, 0.0); glVertex3f(video_x1, video_y0, 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3f(video_x0, video_y0, 0.0);
      glEnd();
      if (this->osd_visible || keep_osd_open) {

	// Check if we need to update the image
        glBindTexture (GL_TEXTURE_2D, this->osd_texture);
        pthread_mutex_lock(&this->opengl_osd_texture_img_mutex);
	if (this->opengl_osd_texture_img_updated) {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->osd_width, this->osd_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, this->opengl_osd_texture_img);
          GLenum error = glGetError();
          if (error != GL_NO_ERROR) {
            LOGERR("Can't update hud image texture");
          }
	  this->opengl_osd_texture_img_updated = 0;
	}
        pthread_mutex_unlock(&this->opengl_osd_texture_img_mutex);	

	// Draw the hud
        glColor4f(1.0f, 1.0f, 1.0f, osd_alpha);
        glBegin(GL_QUADS);
          glTexCoord2f(0.0, 1.0); glVertex3f(osd_x0, osd_y1 , 0.0);
          glTexCoord2f(1.0, 1.0); glVertex3f(osd_x1, osd_y1, 0.0);
          glTexCoord2f(1.0, 0.0); glVertex3f(osd_x1, osd_y0, 0.0);
          glTexCoord2f(0.0, 0.0); glVertex3f(osd_x0, osd_y0, 0.0);
        glEnd();
      }
      glXSwapBuffers(this->display, this->opengl_window);
      XUnlockDisplay(this->display);
      count++;
    }
    if ((this->osd_visible && count==2 && !window_mapped) || (first_frame && this->opengl_always) || force_redirect) {
      LOGDBG("mapping opengl window");
      XLockDisplay(this->display);
      XRaiseWindow (this->display, this->opengl_window);
      XMapWindow(this->display, this->opengl_window);
      XUnlockDisplay(this->display);
      window_mapped = 1;
    }
    if (!this->osd_visible && count==3 && window_mapped && !keep_osd_open && !this->opengl_always) {
      LOGDBG("unmapping opengl window");
      XLockDisplay(this->display);
      XLowerWindow (this->display, this->opengl_window);
      XUnmapWindow(this->display, this->opengl_window);
      XUnlockDisplay(this->display);
      window_mapped = 0;
    }
    if (!keep_osd_open) {
      prev_osd_visible = this->osd_visible;
    }
    first_frame = 0;
    force_redirect = 0;
    //LOGVERBOSE("drawing time = %d",time_measure_end(&t));

    // Drawing is finished
    pthread_mutex_lock(&this->opengl_redraw_finished_mutex);
    this->opengl_redraw_served_nr++;
    pthread_cond_signal(&this->opengl_redraw_finished_cv);
    pthread_mutex_unlock(&this->opengl_redraw_finished_mutex);

  }

  // Free resources
  opengl_deinit(this);
  return NULL;
}

static int opengl_start(sxfe_t *this)
{
  LOGDBG("sxfe_display_open: starting opengl drawing thread");
  pthread_mutex_init(&this->opengl_redraw_mutex, NULL);
  pthread_cond_init(&this->opengl_redraw_cv, NULL);
  this->opengl_redraw_request_nr = 0;
  this->opengl_redraw_served_nr = 0;
  pthread_mutex_init(&this->opengl_redraw_finished_mutex, NULL);
  pthread_cond_init(&this->opengl_redraw_finished_cv, NULL);
  pthread_mutex_init(&this->opengl_osd_texture_img_mutex, NULL);
  pthread_attr_t attr;
  struct sched_param param;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_getschedparam(&attr, &param);
  param.sched_priority = sched_get_priority_min(SCHED_OTHER);
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  this->x.vis_x11.frame_output_cb = opengl_frame_output_cb;     // ensure that the opengl drawing thread gets triggered
  this->fe.xine_osd_command = opengl_osd_command;               // opengl thread needs to get updated with the new hud image
  this->osd_width  = OSD_DEF_WIDTH;
  this->osd_height = OSD_DEF_HEIGHT;
  this->opengl_osd_texture_img = malloc(sizeof(uint32_t) * this->osd_width * this->osd_height);
  memset((void*)this->opengl_osd_texture_img,0,sizeof(uint32_t)*this->osd_width*this->osd_height);
  this->opengl_osd_texture_img_updated = 0;
#ifdef HAVE_XSHAPE
  int dummy;
  if (XShapeQueryExtension(this->display, &dummy, &dummy))
    this->opengl_xshape_available = 1;
  else
    this->opengl_xshape_available = 0;
#endif
  if (pthread_create(&this->opengl_drawing_thread, &attr, opengl_draw_frame_thread, (void *)this)) {
    pthread_attr_destroy(&attr);
    LOGERR("sxfe_display_open: can not start OpenGL drawing thread");
    this->opengl_always = this->opengl_hud = 0; /* avoid pthread_join segfault */
    return 0;
  }
  pthread_attr_destroy(&attr);

  return 1;
}

#endif

/*
 * sxfe_display_open
 *
 * connect to X server, create windows
 */
static int sxfe_display_open(frontend_t *this_gen,
                             int xpos, int ypos,
                             int width, int height, int fullscreen, int hud, int opengl,
                             int modeswitch, const char *modeline, int aspect,
                             int no_x_kbd, int gui_hotkeys, int touchscreen,
                             const char *video_port, int scale_video,
                             const char *aspect_controller, int window_id)
{
  sxfe_t    *this = (sxfe_t*)this_gen;

  if(this->display)
    this->fe.fe_display_close(this_gen);

  LOGDBG("sxfe_display_open(width=%d, height=%d, fullscreen=%d, display=%s)",
         width, height, fullscreen, video_port);

  pthread_mutex_init(&this->video_win_mutex, NULL);

  if(this->fe.fe_message_cb) {
    /* trigger key learning mode */
    this->fe.fe_message_cb(this->fe.fe_message_h, "XKeySym", "");
  }

#if defined(HAVE_XRENDER) || defined(HAVE_OPENGL)
  this->osd_width  = OSD_DEF_WIDTH;
  this->osd_height = OSD_DEF_HEIGHT;
#endif

  if (opengl) {
#ifdef HAVE_OPENGL
    LOGDBG("sxfe_display_open: Using opengl to draw video and HUD OSD");
    this->opengl_always = 1;
#else
    LOGMSG("sxfe_display_open: Application was compiled without OpenGL support.");
    return 0;
#endif
  }
  else if (hud & HUD_OPENGL) {
#ifdef HAVE_OPENGL
    LOGDBG("sxfe_display_open: Using opengl to draw HUD OSD only");
    this->opengl_hud = 1;
#else
    LOGMSG("sxfe_display_open: Application was compiled without OpenGL support.");
    return 0;
#endif
  } else if (hud) {
#ifdef HAVE_XRENDER
    LOGDBG("sxfe_display_open: Enabling HUD OSD");
    this->hud        = hud;
# ifdef HAVE_XSHAPE
    this->xshape_hud = !!(hud & HUD_XSHAPE);
# endif
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
  this->x.aspect_controller = aspect_controller ? strdup(aspect_controller) : NULL;

  this->video_win_active = 0;

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

  this->gui_hotkeys = !!gui_hotkeys;
  this->no_x_kbd    = !!no_x_kbd;
  this->touchscreen = !!touchscreen;

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
  this->root_window = DefaultRootWindow(this->display);
  /* #warning sxfe_display_open: TODO: switch vmode */

  /* completion event */
#ifdef HAVE_XSHM
  this->xshm_completion_event = -1;
  if (XShmQueryExtension (this->display) == True) {
    this->xshm_completion_event = XShmGetEventBase (this->display) + ShmCompletion;
  }
#endif

  init_atoms(this);

  if(fullscreen)
    update_screen_size(this);

  /* Output to existing window ? (embedded to another app) */

  if (this->window_id == WINDOW_ID_ROOT) {
    this->window_id = this->root_window;
  }
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
                FocusChangeMask |
                PointerMotionMask);
  XSelectInput (this->display, this->window[1],
                StructureNotifyMask |
                ExposureMask |
                KeyPressMask |
                ButtonPressMask |
                FocusChangeMask |
                PointerMotionMask);

  /* Get notified when root window size changes */
  XSelectInput (this->display, this->root_window,
		StructureNotifyMask);

  /* Map current window */
  XMapRaised (this->display, this->window[!!this->fullscreen]);
  XMoveWindow(this->display, this->window[0], this->x.xpos, this->x.ypos);

  /* determine display aspect ratio */
  this->x.display_ratio = detect_display_ratio(this->display, this->screen);

  /* we want to get notified if user closes the window */
  XSetWMProtocols(this->display, this->window[this->fullscreen ? 1 : 0], &(this->xa_WM_DELETE_WINDOW), 1);

  /* Hide cursor */
  if(this->window_id <= 0) {
    set_cursor(this->display, this->window[this->fullscreen ? 1 : 0], 0);
    this->mousecursor_timeout = 0;
  }

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


#ifdef HAVE_OPENGL
  // Shall opengl or xrender be used?
  if (this->opengl_always || this->opengl_hud) {

    // Start the drawing thread
    if (!opengl_start(this))
      return 0;
  }
#endif

#ifdef HAVE_XRENDER
  // Init the osd window
  if (!hud_osd_open(this))
    return 0;
#endif

  return 1;
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
                               int aspect, int scale_video)
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
                             this->root_window,
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

  return 1;
}

static void sxfe_toggle_fullscreen(fe_t *this_gen, int fullscreen)
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

  if (fullscreen < 0)
    fullscreen = !this->fullscreen;

  this->fe.fe_display_config((frontend_t*)this, -1, -1, this->origwidth, this->origheight,
                             fullscreen,
                             0/*this->vmode_switch*/, NULL/*this->modeline*/,
                             this->x.aspect, this->x.scale_video);

  this->fullscreen_state_forced = !force;
}

static unsigned char *sxfe_display_edid(frontend_t *this_gen, int *size)
{
  sxfe_t *this = (sxfe_t*)this_gen;
  unsigned char *edid = NULL;

#ifdef HAVE_XRANDR

  XLockDisplay(this->display);

  do {
    if (!this->display)
      break;

    int event_base, error_base;
    int major, minor;
    if (!XRRQueryExtension(this->display, &event_base, &error_base) ||
        !XRRQueryVersion(this->display, &major, &minor)) {
      LOGMSG("edid: RandR extension missing");
      break;
    }
    if (major < 1 || (major == 1 && minor < 2)) {
      LOGMSG("edid: RandR extension < 1.2");
      break;
    }

    XRRScreenResources *res;
    res = XRRGetScreenResourcesCurrent(this->display, this->root_window);
    if (!res) {
      LOGMSG("edid: failed getting screen resources");
      break;
    }

    int o;
    for (o = 0; o < res->noutput && !edid; o++) {
      XRROutputInfo *output_info = XRRGetOutputInfo(this->display, res, res->outputs[o]);
      if (!output_info) {
        LOGMSG("edid: failed getting output %d information", o);
        continue;
      }
      if (output_info->connection != RR_Connected) {
        LOGDBG("edid: output %s not connected", output_info->name);
        continue;
      }
      LOGDBG("edid: checking connected output %s", output_info->name);

      int nprop, j;
      Atom *props = XRRListOutputProperties(this->display, res->outputs[o], &nprop);
      for (j = 0; j < nprop && !edid; j++) {
        char *atom_name = XGetAtomName(this->display, props[j]);

        if (!strcmp(atom_name, "EDID")) {

          unsigned char *prop;
          int actual_format;
          unsigned long nitems, bytes_after;
          Atom actual_type;
          XRRGetOutputProperty(this->display, res->outputs[o], props[j],
                               0, 100, False, False,
                               AnyPropertyType,
                               &actual_type, &actual_format,
                               &nitems, &bytes_after, &prop);
          if (actual_format == 8 && actual_type == XA_INTEGER) {
            LOGDBG("edid: Found EDID, %ld bytes", nitems);
            *size = nitems;
            edid = malloc(*size);
            memcpy(edid, prop, *size);
            break;
          }
        }
      }
    }
  } while (0);

  XUnlockDisplay(this->display);

  if (!edid) {
    LOGMSG("no EDID found");
  }
#endif /* HAVE_XRANDR */

  return edid;
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
    const char    *ks_name;

    /* resolve key symbol */
    XLockDisplay (this->display);
    XLookupString(kev, buffer, sizeof(buffer), &ks, &status);
    ks_name = XKeysymToString(ks);
    XUnlockDisplay (this->display);

    /* hotkeys */

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
      case XK_p:
      case XK_P:
        if (this->gui_hotkeys)
          fe_event = "POWER_OFF";
        break;
      case XK_Escape:
        if (!this->fe.fe_message_cb) /* ESC exits only in remote mode */
          fe_event = "QUIT";
        break;
      default:;
    }

    if (fe_event) {
      this->x.fe.send_event((frontend_t*)this, fe_event);
      return;
    }

    /* send key event to VDR */

    if (!this->no_x_kbd && ks_name) {

      /* check for key modifiers (Alt/Ctrl) */
      char keyname[40] = "";
      if (kev->state & (Mod1Mask|ControlMask)) {
        if (kev->state & Mod1Mask) {
          strcat(keyname, "Alt+");
        }
        if (kev->state & ControlMask) {
          strcat(keyname, "Ctrl+");
        }
        strncat(keyname, ks_name, sizeof(keyname) - 11);
        ks_name = keyname;
      }

      this->x.fe.send_input_event((frontend_t*)this, "XKeySym", ks_name, 0, 0);
    }
  }
}

/*
 * XConfigureEvent handler
 *
 */
static void XConfigureEvent_handler(sxfe_t *this, XConfigureEvent *cev)
{
#ifdef HAVE_OPENGL
  if (cev->window == this->opengl_window)
    return;
#endif

  /* root window size changed ? */
  if (cev->window != (Window)this->window_id && cev->window == this->root_window) {
    if (this->fullscreen) {
      if (cev->width != this->x.width || cev->height != this->x.height) {
        LOGMSG("Root window size changed. Resizing video window from %dx%d to %dx%d",
               this->x.width, this->x.height, cev->width, cev->height);
        XLockDisplay(this->display);
        XResizeWindow(this->display, this->window[1], cev->width, cev->height);
        XMoveWindow(this->display, this->window[1], this->xinerama_x, this->xinerama_y);
        XUnlockDisplay(this->display);
      }
    }

    return;
  }

  /* Move and resize HUD along with main or fullscreen window */
#ifdef HAVE_XRENDER
  if(this->hud)
    hud_osd_resize(this, cev->window, cev->width, cev->height);
#endif

  /* update video window size */
  if (this->x.width != cev->width || this->x.height != cev->height) {

    if ( ( this->fullscreen && this->window[1] == cev->window) ||
         (!this->fullscreen && this->window[0] == cev->window)) {

      LOGDBG("Video window size changed from %dx%d to %dx%d", this->x.width, this->x.height, cev->width, cev->height);

      this->x.width  = cev->width;
      this->x.height = cev->height;

      /* inform VDR about new size */
      char str[128];
      snprintf(str, sizeof(str), "INFO WINDOW %dx%d", this->x.width, this->x.height);
      this->x.fe.send_event((frontend_t*)this, str);
    }
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

  if (!this->fullscreen) {
    if ((cev->x == 0) && (cev->y == 0)) {
      int tmp_x, tmp_y;
      Window tmp_win;
      XLockDisplay(this->display);
      if(XTranslateCoordinates(this->display, cev->window,
                               this->root_window,
                               0, 0, &tmp_x, &tmp_y, &tmp_win)) {
        this->x.xpos = tmp_x;
        this->x.ypos = tmp_y;
      }
      XUnlockDisplay(this->display);
    } else {
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
  // make mouse cursor visible
  if (this->mousecursor_timeout <= 0) {
    set_cursor(this->display, this->window[this->fullscreen ? 1 : 0], 1);
  }
  // start timeout
  this->mousecursor_timeout = MOUSECURSOR_TIMEOUT;

  if(this->dragging && !this->fullscreen) {
    Window tmp_win;
    int xpos, ypos;

    XLockDisplay(this->display);

    while(XCheckMaskEvent(this->display, ButtonMotionMask, (XEvent*)mev));

    XTranslateCoordinates(this->display, this->window[0],
                          this->root_window,
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

      if (this->touchscreen) {
        int x = bev->x * 4 / this->x.width;
        int y = bev->y * 3 / this->x.height;
        static const char * const map[3][4] = {
          {"Menu", "Up", "Back", "Ok"},
          {"Left", "Down", "Right", "Ok"},
          {"Red", "Green", "Yellow", "Blue"}};
        if (map[y][x]) {
          char tmp[128];
          sprintf(tmp, "KEY %s", map[y][x]);
          this->x.fe.send_event((frontend_t*)this, tmp);
        }
        return;
      }

      /* Double-click toggles between fullscreen and windowed mode */
      if(bev->time - this->prev_click_time < DOUBLECLICK_TIME) {
        /* Toggle fullscreen */
        this->x.toggle_fullscreen_cb(&this->x, -1);
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

    default:; // ignore other buttons.
  }
}

/*
 * sxfe_run
 *
 *  - main X event loop
 */

static void check_mouse_cursor_hide(sxfe_t *this, int64_t elapsed)
{
  if (elapsed > 0 && elapsed < 500) {
    this->mousecursor_timeout -= elapsed;
    if (this->mousecursor_timeout <= 0) {
      // hide Cursor
      set_cursor(this->display, this->window[this->fullscreen ? 1 : 0], 0);
    }
  }
}

static int sxfe_run(frontend_t *this_gen) 
{
  sxfe_t *this = (sxfe_t*)this_gen;

  /* poll X server (connection socket).
   * (XNextEvent will block until events are queued).
   * We want to use timeout, blocking for long time usually causes vdr
   * watchdog to emergency exit ...
   */
  if (! XPending(this->display)) {
    uint64_t poll_time = 0;
    const int poll_timeout = 50;
    struct pollfd pfd = {
      .fd = ConnectionNumber(this->display),
      .events = POLLIN,
    };

    if (this->mousecursor_timeout > 0) {
      poll_time = time_ms();
    }

#ifdef HAVE_MCE_DBUS_NAMES
# ifdef HAVE_DBUS_GLIB_1
    /* Disable MCE screensaver */
    if (++this->mce_blank_prevent_timer > 100) {
      gnome_screensaver_control(0);
      this->mce_blank_prevent_timer = 0;
    }
# endif
#endif

    if (poll(&pfd, 1, poll_timeout) < 1 || !(pfd.revents & POLLIN)) {

      if (this->mousecursor_timeout > 0) {
        check_mouse_cursor_hide(this, poll_timeout);
      }

      return !this->x.fe.xine_is_finished((frontend_t*)this, 0);
    }

    if (poll_time) {
      check_mouse_cursor_hide(this, elapsed(poll_time));
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

        if ( (Atom)cmessage->data.l[0] == this->xa_WM_DELETE_WINDOW ) {
          /* we got a window deletion message from out window manager.*/
          LOGDBG("ClientMessage: WM_DELETE_WINDOW");

          this->x.fe.send_event((frontend_t*)this, "QUIT");
        }
        break;
      }
      default:; // ignore other events.
    }

#ifdef HAVE_XSHM
    if (event.type == this->xshm_completion_event)
      xine_port_send_gui_data (this->x.video_port, XINE_GUI_SEND_COMPLETION_EVENT, &event);
#endif
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

#ifdef HAVE_OPENGL
    if (this->opengl_always || this->opengl_hud) {
      void *status;
      this->opengl_deinit = 1;
      opengl_trigger_drawing_thread(this);
      if (pthread_join(this->opengl_drawing_thread, &status)) {
        LOGERR("sxfe_display_close: can not join opengl drawing thread!");
      }
      free(this->opengl_osd_texture_img);
    }
#endif

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

    pthread_mutex_destroy(&this->video_win_mutex);
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

  if (result && this->x.input_plugin) {
#ifdef HAVE_XRENDER
    if (this->hud) {
      LOGDBG("sxfe_xine_play: Enabling HUD OSD");
      this->x.input_plugin->f.fe_handle     = this_gen;
      this->x.input_plugin->f.intercept_osd = hud_osd_command;

      this->x.fe.send_event((frontend_t*)this, "INFO ARGBOSD");
    }
#endif /* HAVE_XRENDER */
#ifdef HAVE_OPENGL
    if (this->opengl_always || this->opengl_hud) {
      LOGDBG("sxfe_xine_play: Enabling OpenGL OSD");
      this->x.input_plugin->f.fe_handle     = this_gen;
      this->x.input_plugin->f.intercept_osd = opengl_osd_command;

      this->x.fe.send_event((frontend_t*)this, "INFO ARGBOSD");
    }
#endif /* HAVE_OPENGL */
  }

  return result;
}

static frontend_t *sxfe_get_frontend(void)
{
  sxfe_t *this = calloc(1, sizeof(sxfe_t));

  init_fe((fe_t*)this);

  this->window_id = -1;

  this->fe.fe_display_open   = sxfe_display_open;
  this->fe.fe_display_config = sxfe_display_config;
  this->fe.fe_display_edid   = sxfe_display_edid;
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



