/*
 * xine_sxfe_frontend.c: Simple front-end, X11 functions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_sxfe_frontend.c,v 1.41.2.10 2009/02/12 12:45:20 phintuka Exp $
 *
 */

/*#define HAVE_XF86VIDMODE*/

#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <poll.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#ifdef HAVE_XRENDER
#  include <X11/extensions/Xrender.h>
#endif
#ifdef HAVE_XF86VIDMODE
#  include <X11/extensions/xf86vmode.h>
#endif
#ifdef HAVE_XDPMS
#  include <X11/extensions/dpms.h>
#endif
#ifdef HAVE_XV_FIELD_ORDER
#  include <X11/extensions/Xvlib.h>
#endif

#ifdef boolean
#  define HAVE_BOOLEAN
#endif
#include <jpeglib.h>
#undef boolean

/* framegrab ports */
#define XINE_ENABLE_EXPERIMENTAL_FEATURES

#include <xine.h>
#ifndef XINE_ENGINE_INTERNAL
#  define XINE_ENGINE_INTERNAL
#  include <xine/xine_internal.h>
#  undef XINE_ENGINE_INTERNAL
#else
#  include <xine/xine_internal.h>
#endif
#include <xine/xineutils.h>
#include <xine/input_plugin.h>
#include <xine/plugin_catalog.h>

#include "xine_input_vdr.h"
#include "xine_osd_command.h"

#include "xine_frontend.h"
#include "xine/post.h"

#ifndef WIN_LAYER_NORMAL
  #define WIN_LAYER_NORMAL 4
#endif
#ifndef WIN_LAYER_ONTOP
  #define WIN_LAYER_ONTOP  6
#endif

#define MWM_HINTS_DECORATIONS       (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS     5
typedef struct _mwmhints {
  uint32_t                          flags;
  uint32_t                          functions;
  uint32_t                          decorations;
  int32_t                           input_mode;
  uint32_t                          status;
} MWMHints;

#ifdef HAVE_XRENDER
/* HUD Scaling */
typedef struct _xrender_surf
{
  int w, h;
  int depth;
  Visual *vis;
  Drawable draw;
  Picture pic;
  int allocated : 1;
} Xrender_Surf;
#endif /* HAVE_XRENDER */

/*
 * data
 */

typedef struct sxfe_s {

  /* function pointers */
  frontend_t              fe;
  void (*update_display_size)(frontend_t*);

  /* vdr */
  fe_keypress_f        keypress;

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
#ifdef HAVE_XDPMS
  BOOL     dpms_state;
#endif

  /* Atoms */
  Atom     atom_wm_delete_window;
  Atom     atom_sxfe_interrupt;
  Atom     atom_wm_hints, atom_win_layer;
  Atom     atom_state, atom_state_add, atom_state_del;
  Atom     atom_state_above, atom_state_fullscreen, atom_state_on_top;

  /* xine stuff */
  xine_t              *xine;
  xine_stream_t       *stream;
  xine_stream_t       *slave_stream;
  input_plugin_t      *input;
  xine_video_port_t   *video_port;
  xine_video_port_t   *video_port_none;
  xine_audio_port_t   *audio_port;
  xine_audio_port_t   *audio_port_none;
  xine_event_queue_t  *event_queue;

  post_plugins_t      *postplugins;

  x11_visual_t         vis;
  int                  xine_visual_type;

  uint16_t             pes_buffers;

  /* frontend */
  double    display_ratio;
  double    video_aspect;
  char     *aspect_controller;
  int       xpos, ypos;
  uint16_t  video_width, video_height;
  uint16_t  width, height;
  uint16_t  origxpos, origypos;
  uint16_t  origwidth, origheight;
  uint8_t   aspect;
  uint8_t   cropping;
  uint8_t   scale_video;
  uint8_t   overscan;
  uint8_t   playback_finished;
  uint8_t   slave_playback_finished;
  uint8_t   fullscreen;
  uint8_t   vmode_switch;
  uint8_t   field_order;
  uint8_t   fullscreen_state_forced;
  uint8_t   stay_above;
  uint8_t   no_border;
  uint8_t   check_move;

  /* strings */
  char    configfile[256];
  char    modeline[256];

  /* HUD stuff */
#ifdef HAVE_XRENDER
  uint8_t  hud;
  GC       gc;
  Window   hud_window;
  XImage  *hud_img;
  Visual  *hud_vis;
  XShmSegmentInfo hud_shminfo;
  Xrender_Surf *surf_win;
  Xrender_Surf *surf_img;
  int osd_width;
  int osd_height;
  uint32_t* hud_img_mem;
#endif /* HAVE_XRENDER */

} fe_t, sxfe_t;


/* Common (non-X11/FB) frontend functions */
#include "xine_frontend.c"

#define DOUBLECLICK_TIME   500  // ms

#define OSD_DEF_WIDTH      720
#define OSD_DEF_HEIGHT     576
#define HUD_MAX_WIDTH      1920
#define HUD_MAX_HEIGHT     1080

static void fe_dest_size_cb (void *data,
			     int video_width, int video_height, double video_pixel_aspect,
			     int *dest_width, int *dest_height, double *dest_pixel_aspect)  
{ 
  fe_t *this = (fe_t *)data;
  
  if (!this)
    return;

  *dest_width  = this->width;
  *dest_height = this->height;

  *dest_pixel_aspect = fe_dest_pixel_aspect(this, video_pixel_aspect,
					    video_width, video_height);
}

static void set_fullscreen_props(sxfe_t *this)
{
  XEvent ev;

  if(this->window_id > 0)
    return;

  if(this->atom_state == None) {
    this->atom_win_layer        = XInternAtom(this->display, "_WIN_LAYER", False);
    this->atom_state            = XInternAtom(this->display, "_NET_WM_STATE", False);
    this->atom_state_add        = XInternAtom(this->display, "_NET_WM_STATE_ADD", False);
    this->atom_state_del        = XInternAtom(this->display, "_NET_WM_STATE_DEL", False);
    this->atom_state_above      = XInternAtom(this->display, "_NET_WM_STATE_ABOVE", False);
    this->atom_state_fullscreen = XInternAtom(this->display, "_NET_WM_STATE_FULLSCREEN", False);
    this->atom_state_on_top     = XInternAtom(this->display, "_NET_WM_STATE_STAYS_ON_TOP", False);
  }

  memset(&ev, 0, sizeof(ev));
  ev.type                 = ClientMessage;
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = this->atom_state;
  ev.xclient.display      = this->display;
  ev.xclient.window       = this->window[1];
  ev.xclient.format       = 32;
  ev.xclient.data.l[0]    = 1; 
  /*ev.xclient.data.l[0]    = this->atom_state_add;*/

  /* _NET_WM_STATE_FULLSCREEN */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->atom_state_fullscreen;
  XSendEvent(this->display, DefaultRootWindow(this->display), False, 
	     SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_ABOVE */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->atom_state_above;
  XSendEvent(this->display, DefaultRootWindow(this->display), False, 
	     SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_ON_TOP */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->atom_state_on_top;
  XSendEvent(this->display, DefaultRootWindow(this->display), False, 
	     SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);
}

static void set_border(sxfe_t *this, int border)
{
  MWMHints   mwmhints;

  if(this->window_id > 0)
    return;

  this->no_border = border ? 0 : 1;

  /* Set/remove border */
  this->atom_wm_hints = XInternAtom(this->display, "_MOTIF_WM_HINTS", False);
  mwmhints.flags = MWM_HINTS_DECORATIONS;
  mwmhints.decorations = this->no_border ? 0 : 1;
  XChangeProperty(this->display, this->window[0], this->atom_wm_hints, this->atom_wm_hints, 32,
		  PropModeReplace, (unsigned char *) &mwmhints,
		  PROP_MWM_HINTS_ELEMENTS);
}

static void set_above(sxfe_t *this, int stay_above)
{
  XEvent ev;
  long propvalue[1];

  if(this->window_id > 0)
    return;

  if(this->stay_above != stay_above) {
    XLockDisplay(this->display);
#ifdef FE_STANDALONE
    char *name, *newname = NULL;
    if(XFetchName(this->display, this->window[0], &name) && name) {
      if(strstr(name, " (top)"))
	*strstr(name, " (top)") = 0;
      if(stay_above)
	if (asprintf(&newname, "%s (top)", name) < 0)
          newname = NULL;
      XStoreName(this->display, this->window[0], newname ?: name);
      XStoreName(this->display, this->window[1], newname ?: name);
      XFree(name);
      free(newname);
    } else {
      XStoreName(this->display, this->window[0], stay_above ? "VDR - (top)" : "VDR");
    }
#else
    XStoreName(this->display, this->window[0], stay_above ? "Local VDR (top)" : "Local VDR");
#endif
    this->stay_above = stay_above;
    XUnlockDisplay(this->display);
  }

  memset(&ev, 0, sizeof(ev));
  ev.type                 = ClientMessage;
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = this->atom_state;
  ev.xclient.display      = this->display;
  ev.xclient.window       = this->window[0];
  ev.xclient.format       = 32;
  ev.xclient.data.l[0]    = stay_above ? 1:0; /*this->atom_state_add : this->atom_state_del;*/

  /* _NET_WM_STATE_ABOVE */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->atom_state_above;
  XSendEvent(this->display, DefaultRootWindow(this->display), False, 
	     SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_ON_TOP */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = this->atom_state_on_top;
  XSendEvent(this->display, DefaultRootWindow(this->display), False, 
	     SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _NET_WM_STATE_STICKY */
  XLockDisplay(this->display);
  ev.xclient.data.l[1] = XInternAtom(this->display, "_NET_WM_STATE_STICKY", False);
  XSendEvent(this->display, DefaultRootWindow(this->display), False, 
	     SubstructureNotifyMask|SubstructureRedirectMask, &ev);
  XUnlockDisplay(this->display);

  /* _WIN_LAYER */
  propvalue[0] = stay_above ? WIN_LAYER_ONTOP : WIN_LAYER_NORMAL;
  XLockDisplay(this->display);
  XChangeProperty(this->display, this->window[0], XInternAtom(this->display, "_WIN_LAYER", False),
		  XA_CARDINAL, 32, PropModeReplace, (unsigned char *)propvalue,
		  1);
  XUnlockDisplay(this->display);

#if 0
  /* sticky */
  memset(&ev, 0, sizeof(ev));
  ev.xclient.type         = ClientMessage;
  ev.xclient.message_type = XInternAtom(this->display, "_WIN_STATE", False);
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
  ev.xclient.message_type = XInternAtom(this->display, "_WIN_LAYER", False);
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
  xev.message_type = this->atom_win_layer;
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

static void set_cursor(Display *dpy, Window win, const int enable)
{
  if(enable)
    XDefineCursor(dpy, win, None);
  else {
    /* no cursor */
    static char bm_no_data[] = { 0,0,0,0, 0,0,0,0 };
    Pixmap bm_no;
    Cursor no_ptr;
    XColor black, dummy;
    bm_no = XCreateBitmapFromData(dpy, win, bm_no_data, 8, 8);
    XAllocNamedColor(dpy, DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)), 
		     "black", &black, &dummy);
    no_ptr = XCreatePixmapCursor(dpy, bm_no, bm_no, &black, &black, 0, 0);
    XDefineCursor(dpy, win, None);
    XDefineCursor(dpy, win, no_ptr);
  }
}

#ifdef HAVE_XRENDER
Xrender_Surf * xrender_surf_new(Display *dpy, Drawable draw, Visual *vis, int w, int h, int alpha)
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

void xrender_surf_blend(Display *dpy, Xrender_Surf *src, Xrender_Surf *dst,
                        int x, int y, int w, int h, XDouble scale_x, XDouble scale_y, int smooth)
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
  x = (int)ceil((double)(x?x-1:0) * scale_x);
  y = (int)ceil((double)(y?y-1:0) * scale_y);
  w = (int)floor((double)(w+2) * scale_x);
  h = (int)floor((double)(h+2) * scale_y);
  XRenderComposite(dpy, PictOpSrc, src->pic, None, dst->pic, x, y, 0, 0, x, y, w, h);
}

Xrender_Surf * xrender_surf_adopt(Display *dpy, Drawable draw, Visual *vis, int w, int h)
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

void xrender_surf_free(Display *dpy, Xrender_Surf *rs)
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
			     (XDouble)this->width / (XDouble)this->osd_width,
			     (XDouble)this->height / (XDouble)this->osd_height,
			     (cmd->scaling & 2)); // Note: HUD_SCALING_BILINEAR=2
        }
      } else {
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
			     (XDouble)this->width / (XDouble)this->osd_width,
			     (XDouble)this->height / (XDouble)this->osd_height,
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
      XSetForeground(this->display, this->gc, 0x00000000);
      XFillRectangle(this->display, this->hud_window, this->gc,
		     0, 0, this->width, this->height);
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

static int hud_osd_open(frontend_t *this_gen)
{
  sxfe_t *this = (sxfe_t*)this_gen;
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
                                     this->xpos, this->ypos,
                                     this->width, this->height,
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

    if(this->completion_event != -1) {
      this->hud_img = XShmCreateImage(this->display, this->hud_vis, 32, ZPixmap, NULL, &(this->hud_shminfo),
                                      HUD_MAX_WIDTH, HUD_MAX_HEIGHT);

      this->hud_shminfo.shmid = shmget(IPC_PRIVATE, this->hud_img->bytes_per_line * this->hud_img->height,
                                       IPC_CREAT | 0777);

      this->hud_shminfo.shmaddr = this->hud_img->data = shmat(this->hud_shminfo.shmid, 0, 0);
      this->hud_shminfo.readOnly = True;

      XShmAttach(this->display, &(this->hud_shminfo));
    } else {
      /* Fall-back to traditional memory */
      LOGMSG("hud_osd_open: XShm not available, falling back to normal (slow) memory");
      this->hud_img_mem = malloc(4 * HUD_MAX_WIDTH * HUD_MAX_HEIGHT);
      this->hud_img = XCreateImage(this->display, this->hud_vis, 32, ZPixmap, 0, (char*)this->hud_img_mem,
                                   HUD_MAX_WIDTH, HUD_MAX_HEIGHT, 32, 0);
    }

    this->surf_win = xrender_surf_adopt(this->display, this->hud_window, this->hud_vis, HUD_MAX_WIDTH, HUD_MAX_HEIGHT);
    this->surf_img = xrender_surf_new(this->display, this->hud_window, this->hud_vis, HUD_MAX_WIDTH, HUD_MAX_HEIGHT, 1);

    XUnlockDisplay(this->display);

#ifndef FE_STANDALONE
    this->fe.xine_osd_command = hud_osd_command;
#endif
  }
  return 1;
}

static void hud_osd_close(frontend_t *this_gen)
{
  sxfe_t *this = (sxfe_t*)this_gen;
  if(this && this->hud) {
    XLockDisplay(this->display);
    LOGDBG("closing hud window...");

    if(this->completion_event != -1) {
      XShmDetach(this->display, &(this->hud_shminfo));
      XDestroyImage(this->hud_img);
      shmdt(this->hud_shminfo.shmaddr);
      shmctl(this->hud_shminfo.shmid, IPC_RMID, 0);
    } else
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
 * sxfe_display_open
 *
 * connect to X server, create windows
 */

static int sxfe_display_open(frontend_t *this_gen, int width, int height, int fullscreen, int hud,
			     int modeswitch, const char *modeline, int aspect,
			     fe_keypress_f keyfunc, const char *video_port,
			     int scale_video, int field_order) 
{
  sxfe_t    *this = (sxfe_t*)this_gen;

  MWMHints   mwmhints;
  XSizeHints hint;
  double     res_h, res_v, aspect_diff;

  if(this->display)
    this->fe.fe_display_close(this_gen);

  if(keyfunc) {
    this->keypress = keyfunc;
    this->keypress("XKeySym", ""); /* triggers learning mode */
  }

  LOGDBG("sxfe_display_open(width=%d, height=%d, fullscreen=%d, display=%s)",
	 width, height, fullscreen, video_port);

  if(hud) {
#ifdef HAVE_XRENDER
    LOGDBG("sxfe_display_open: Enabling HUD OSD");
    this->hud = hud;
    this->osd_width  = OSD_DEF_WIDTH;
    this->osd_height = OSD_DEF_HEIGHT;
#else
    LOGMSG("sxfe_display_open: Application was compiled without XRender support. HUD OSD disabled.");
#endif
  }

  this->xpos            = 0;
  this->ypos            = 0;
  this->origxpos        = 0;
  this->origypos        = 0;
  this->width           = width;
  this->height          = height;
  this->origwidth       = width>0 ? width : OSD_DEF_WIDTH;
  this->origheight      = height>0 ? height : OSD_DEF_HEIGHT;
  this->check_move      = 0;

  this->fullscreen      = fullscreen;
  this->vmode_switch    = modeswitch;
  this->aspect          = aspect;
  this->cropping        = 0;
  this->field_order     = field_order ? 1 : 0;
  this->scale_video     = scale_video;
  this->overscan        = 0;
  this->fullscreen_state_forced = 0;
  strn0cpy(this->modeline, modeline ? : "", sizeof(this->modeline));

  /*
   * init x11 stuff
   */

  if (!XInitThreads ()) {
    LOGERR("sxfe_display_open: XInitThreads failed");
    free(this);
    return 0;
  }

  if(video_port && strlen(video_port)>2) {
    if(!(this->display = XOpenDisplay(video_port)))
      LOGERR("sxfe_display_open: failed to connect to X server (%s)",
	     video_port);
  }
  if(!this->display) {
    if(NULL!=(video_port=getenv("DISPLAY")) && !(this->display = XOpenDisplay(video_port)))
      LOGERR("sxfe_display_open: failed to connect to X server (%s)",
	     video_port);
  }
  if(!this->display) {
    this->display = XOpenDisplay(NULL);
  }
  if(!this->display) {
    if(!(this->display = XOpenDisplay(":0.0")))
      LOGERR("sxfe_display_open: failed to connect to X server (:0.0)");
  }
  if(!this->display) {
    if(!(this->display = XOpenDisplay("127.0.0.1:0.0")))
      LOGERR("sxfe_display_open: failed to connect to X server (127.0.0.1:0.0");
  }
  if (!this->display) {
    LOGERR("sxfe_display_open: failed to connect to X server.");
    LOGMSG("If X server is running, try running \"xhost +\" in xterm window");
    return 0;
  }

  XLockDisplay (this->display);

  this->screen = DefaultScreen(this->display);

  /* #warning sxfe_display_open: TODO: switch vmode */

  /* completion event */
  if (XShmQueryExtension (this->display) == True) {
    this->completion_event = XShmGetEventBase (this->display) + ShmCompletion;
  } else {
    this->completion_event = -1;
  }

  if(fullscreen) {
    this->width = DisplayWidth(this->display, this->screen);
    this->height = DisplayHeight(this->display, this->screen);
  }
  
  if(this->window_id > 0) {
    LOGMSG("sxfe_diaplay_open(): Using X11 window %d for output", this->window_id);
    this->window[0] = this->window[1] = (Window)this->window_id;
    XUnmapWindow(this->display, this->window[0]);
  } else {
    /* create and display our video window */
    this->window[0] = XCreateSimpleWindow (this->display,
					   DefaultRootWindow(this->display),
					   this->xpos, this->ypos,
					   this->width, this->height,
					   1, 0, 0);
    this->window[1] = XCreateSimpleWindow(this->display, XDefaultRootWindow(this->display),
					  0, 0, (DisplayWidth(this->display, this->screen)),
					  (DisplayHeight(this->display, this->screen)), 0, 0, 0);
  }

  hint.flags  = USSize | USPosition | PPosition | PSize;
  hint.x      = 0;
  hint.y      = 0;
  hint.width  = DisplayWidth(this->display, this->screen);
  hint.height = DisplayHeight(this->display, this->screen);
  XSetNormalHints(this->display, this->window[1], &hint);

  if(this->window_id <= 0) {
    /* full-screen window */
    set_fullscreen_props(this);

    /* no border in fullscreen window */
    this->atom_wm_hints = XInternAtom(this->display, "_MOTIF_WM_HINTS", False);
    mwmhints.flags = MWM_HINTS_DECORATIONS;
    mwmhints.decorations = 0;
    XChangeProperty(this->display, this->window[1], this->atom_wm_hints, this->atom_wm_hints, 32,
		    PropModeReplace, (unsigned char *) &mwmhints,
		    PROP_MWM_HINTS_ELEMENTS);
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


  if(this->window_id <= 0) {

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
#ifdef FE_STANDALONE
    XStoreName(this->display, this->window[0], "VDR - ");
    XStoreName(this->display, this->window[1], "VDR - ");
#else
    XStoreName(this->display, this->window[0], "Local VDR");
    XStoreName(this->display, this->window[1], "Local VDR");
#endif

    /* Icon */
    set_icon(this);
  }

  /* Map current window */
  XMapRaised (this->display, this->window[this->fullscreen ? 1 : 0]);
  
  /* determine display aspect ratio */
  res_h = (DisplayWidth  (this->display, this->screen)*1000
	   / DisplayWidthMM (this->display, this->screen));
  res_v = (DisplayHeight (this->display, this->screen)*1000
	   / DisplayHeightMM (this->display, this->screen));
  this->display_ratio = res_v / res_h;
  aspect_diff = this->display_ratio - 1.0;
  if ((aspect_diff < 0.01) && (aspect_diff > -0.01)) {
    this->display_ratio   = 1.0;
  }
  LOGDBG("Display size : %d x %d mm", 
	 DisplayWidthMM (this->display, this->screen),
	 DisplayHeightMM (this->display, this->screen));
  LOGDBG("               %d x %d pixels", 
	 DisplayWidth (this->display, this->screen),
	 DisplayHeight (this->display, this->screen));
  LOGDBG("               %ddpi / %ddpi", 
	 (int)(res_v/1000*25.4), (int)(res_h/1000*25.4));
  LOGDBG("Display ratio: %f/%f = %f", res_v, res_h, this->display_ratio);

  /* we want to get notified if user closes the window */
  this->atom_wm_delete_window = XInternAtom(this->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(this->display, this->window[this->fullscreen ? 1 : 0], &(this->atom_wm_delete_window), 1);

  if(this->window_id <= 0)
    set_cursor(this->display, this->window[1], 0);

  /* No screen saver */
  /* #warning TODO: suspend --> activate blank screen saver / DPMS display off ? */
  XSetScreenSaver(this->display, 0, 0, DefaultBlanking, DefaultExposures);
#ifdef HAVE_XDPMS
  {
    int dpms_dummy;
    if (DPMSQueryExtension(this->display, &dpms_dummy, &dpms_dummy) && DPMSCapable(this->display)) {
      CARD16 dpms_level;
      DPMSInfo(this->display, &dpms_level, &this->dpms_state);
      DPMSDisable(this->display);
    } else {
      LOGMSG("sxfe_display_open: DPMS unavailable");
    }
  }
#endif

  this->xine_visual_type     = XINE_VISUAL_TYPE_X11;
  this->vis.display          = this->display;
  this->vis.screen           = this->screen;
  this->vis.d                = this->window[this->fullscreen ? 1 : 0];
  this->vis.dest_size_cb     = fe_dest_size_cb;
  this->vis.frame_output_cb  = fe_frame_output_cb;
  this->vis.user_data        = this;

  this->atom_sxfe_interrupt  = XInternAtom(this->display, "SXFE_INTERRUPT", False);

  set_fullscreen_props(this);

  XUnlockDisplay (this->display);

#ifdef HAVE_XRENDER
  return hud_osd_open(this_gen);
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
			       int width, int height, int fullscreen, 
			       int modeswitch, const char *modeline, 
			       int aspect, int scale_video, 
			       int field_order) 
{
  sxfe_t *this = (sxfe_t*)this_gen;
  
  if(this->fullscreen_state_forced)
    fullscreen = this->fullscreen ? 1 : 0;

  if(!fullscreen && (this->width != width || this->height != height)) {
    this->width      = width;
    this->height     = height;

    XLockDisplay(this->display);
    XResizeWindow(this->display, this->window[0], this->width, this->height);
    XUnlockDisplay(this->display);
    if(!fullscreen && !this->fullscreen)
      xine_port_send_gui_data(this->video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
			      (void*) this->window[0]);
  }

  if(fullscreen) {
    this->width = DisplayWidth(this->display, this->screen);
    this->height = DisplayHeight(this->display, this->screen);
  }
  
  if(fullscreen != this->fullscreen) {
    Window    tmp_win;
    XLockDisplay(this->display);
    XUnmapWindow(this->display, this->window[this->fullscreen ? 1 : 0]);
    this->fullscreen = fullscreen ? 1 : 0;
    if(fullscreen)
      set_fullscreen_props(this);
    else
      set_above(this, this->stay_above);
    XMapRaised(this->display, this->window[this->fullscreen ? 1 : 0]);
    if(!fullscreen) {
      XResizeWindow(this->display, this->window[0], this->width, this->height);
      XMoveWindow(this->display, this->window[0], this->xpos, this->ypos);
      LOGDBG("sxfe_display_config: XMoveWindow called with x=%d and y=%d",
	     this->xpos, this->ypos);
      this->check_move = 1;
      set_above(this, this->stay_above);
    } else {
      set_fullscreen_props(this);
    }
    XSync(this->display, False);
    XTranslateCoordinates(this->display, this->window[this->fullscreen ? 1 : 0],
			  DefaultRootWindow(this->display),
			  0, 0, &this->xpos, &this->ypos, &tmp_win);
    XUnlockDisplay(this->display);
    xine_port_send_gui_data(this->video_port, XINE_GUI_SEND_DRAWABLE_CHANGED,
			    (void*) this->window[this->fullscreen ? 1 : 0]);
  }

  if(!modeswitch && strcmp(modeline, this->modeline)) {
    strn0cpy(this->modeline, modeline, sizeof(this->modeline));
    /* #warning TODO - switch vmode */
  }

  this->vmode_switch = modeswitch;
  this->aspect = aspect;
  this->scale_video = scale_video;
#ifdef HAVE_XV_FIELD_ORDER
  if(this->field_order != field_order) {
    XLockDisplay (this->display);
    if(XInternAtom(this->display, "XV_SWAP_FIELDS", True) != None)
      XvSetPortAttribute (this->display, 53, 
			  XInternAtom (this->display, "XV_SWAP_FIELDS", False), 
			  field_order);
    XUnlockDisplay (this->display);
  }
#endif
  this->field_order = field_order ? 1 : 0;
  
  return 1;
}

static void sxfe_toggle_fullscreen(sxfe_t *this)
{
  int force = this->fullscreen_state_forced;
  this->fullscreen_state_forced = 0;

  if(!this->fullscreen) {
    this->origwidth  = this->width;
    this->origheight = this->height;
    this->origxpos = this->xpos;
    this->origypos = this->ypos;
  } else {
    this->xpos = this->origxpos;
    this->ypos = this->origypos;
  }

  this->fe.fe_display_config((frontend_t*)this, this->origwidth, this->origheight,
			     this->fullscreen ? 0 : 1, 
			     this->vmode_switch, this->modeline, 
			     this->aspect, this->scale_video, this->field_order);
  
  this->fullscreen_state_forced = !force;
}

/*
 *   X event loop
 */

static void sxfe_interrupt(frontend_t *this_gen) 
{
  sxfe_t *this = (sxfe_t*)this_gen;
  XClientMessageEvent ev2;

  ev2.type    = ClientMessage;
  ev2.display = this->display;
  ev2.window  = this->window[this->fullscreen ? 1 : 0];
  ev2.message_type = this->atom_sxfe_interrupt;
  ev2.format  = 32;

  XLockDisplay (this->display);

  if(!XSendEvent(ev2.display, ev2.window, TRUE, /*KeyPressMask*/0, (XEvent *)&ev2))
    LOGERR("sxfe_interrupt: XSendEvent(ClientMessage) FAILED\n");

  XFlush(this->display);

  XUnlockDisplay (this->display);
}

static int sxfe_run(frontend_t *this_gen) 
{
  sxfe_t *this = (sxfe_t*)this_gen;
  static int dragging = 0, drx = 0, dry = 0;

  int keep_going = 1;
  XEvent event;

  /* poll X server (connection socket). 
     (XNextEvent will block if no events are queued).
     We want to use timeout, blocking for long time usually causes vdr
     watchdog to emergency exit ... */
  if (! XPending(this->display)) {
    struct pollfd pfd[2];
    pfd[0].fd = ConnectionNumber(this->display);
    pfd[0].events = POLLIN;
    if(poll(pfd, 1, 50) < 1 || !(pfd[0].revents & POLLIN)) {
      return 1;
    }
  }

  while(keep_going && XPending(this->display) > 0) {

    XLockDisplay (this->display);
    XNextEvent (this->display, &event);
    XUnlockDisplay (this->display);

    switch (event.type) {
      case Expose:
	if (event.xexpose.count == 0)
	  xine_port_send_gui_data (this->video_port, XINE_GUI_SEND_EXPOSE_EVENT, &event);
	break;
	
      case ConfigureNotify:
      {
	XConfigureEvent *cev = (XConfigureEvent *) &event;
	Window tmp_win;
	
	/* Move and resize HUD along with main or fullscreen window */
#ifdef HAVE_XRENDER
	if(this->hud) {
	  if(cev->window == this->window[0]) {
	    int hud_x, hud_y;
	    XLockDisplay(cev->display);
	    XTranslateCoordinates(this->display, this->window[0],
				  DefaultRootWindow(this->display),
				  0, 0, &hud_x, &hud_y, &tmp_win);
	    XResizeWindow(this->display, this->hud_window, cev->width, cev->height);
	    XMoveWindow(this->display, this->hud_window, hud_x, hud_y);
	    set_cursor(this->display, this->hud_window, 1);
	    XUnlockDisplay(cev->display);
	  } else if(cev->window == this->window[1]) {
	    XLockDisplay(cev->display);
	    XResizeWindow(this->display, this->hud_window, cev->width, cev->height);
	    XMoveWindow(this->display, this->hud_window, 0, 0);
	    set_cursor(this->display, this->hud_window, 0);
	    XUnlockDisplay(cev->display);
	  }
	}
#endif
	this->width  = cev->width;
	this->height = cev->height;

        if(this->window[0] == cev->window && this->check_move) {
	  LOGDBG("ConfigureNotify reveived with x=%d, y=%d, check_move=%d", 
		 cev->x, cev->y, this->check_move);
	  this->check_move = 0;
	  if(this->xpos != cev->x && this->ypos != cev->y) {
	    XLockDisplay (this->display);
	    XMoveWindow(this->display, this->window[0], cev->x, cev->y);
	    XUnlockDisplay (this->display);
	  }
	}
	
	if ((cev->x == 0) && (cev->y == 0)) {
	  XLockDisplay(cev->display);
	  if(!this->fullscreen) 
	    XTranslateCoordinates(cev->display, cev->window,
				  DefaultRootWindow(cev->display),
				  0, 0, &this->xpos, &this->ypos, &tmp_win);
	  XUnlockDisplay(cev->display);
	} else {
	  if(!this->fullscreen) {
	    this->xpos = cev->x;
	    this->ypos = cev->y;
	  }
	}
	break;
      }

#ifdef HAVE_XRENDER
      case FocusIn:
      {
         if(this->hud) {
           XFocusChangeEvent *fev = (XFocusChangeEvent *) &event;
           /* Show HUD again if sxfe window receives focus */
           if(fev->window == this->window[0] || fev->window == this->window[1]) {
             XLockDisplay(fev->display);
             XMapWindow(this->display, this->hud_window);
             XUnlockDisplay(fev->display);
           }
        }
        break;
      }
      case FocusOut:
      {
	if(this->hud) {
	  XFocusChangeEvent *fev = (XFocusChangeEvent *) &event;
	  /* Dismiss HUD window if focusing away from frontend window */
	  if(fev->window == this->window[0] || fev->window == this->window[1]) {
            XLockDisplay(fev->display);
            XUnmapWindow(this->display, this->hud_window);
            XUnlockDisplay(fev->display);
          }
        }
        break;
      }
#endif /* HAVE_XRENDER */
      case ButtonRelease:
      {
	dragging = 0;
	break;
      }

      case MotionNotify:
      {
	if(dragging && !this->fullscreen) {
	  XMotionEvent *mev = (XMotionEvent *) &event;
	  Window tmp_win;
	  int xpos, ypos;

	  XLockDisplay(this->display);

	  while(XCheckMaskEvent(this->display, ButtonMotionMask, &event));

	  XTranslateCoordinates(this->display, this->window[0],
				DefaultRootWindow(this->display),
				0, 0, &xpos, &ypos, &tmp_win);

	  this->xpos = (xpos += mev->x_root - drx);
	  this->ypos = (ypos += mev->y_root - dry);
	  drx = mev->x_root;
	  dry = mev->y_root;

	  XMoveWindow(this->display, this->window[0], xpos, ypos);
          LOGDBG("MotionNotify: XMoveWindow called with x=%d and y=%d", xpos, ypos);

	  XUnlockDisplay(this->display);
	}
	break;
      }

      case ButtonPress:
      {
	XButtonEvent *bev = (XButtonEvent *) &event;
	if(bev->button == Button1) {
	  static Time prev_time = 0;
	  if(bev->time - prev_time < DOUBLECLICK_TIME) {
	    /* Toggle fullscreen */
	    sxfe_toggle_fullscreen(this);
	    prev_time = 0; /* don't react to third click ... */
	  } else {
	    prev_time = bev->time;
	    if(!this->fullscreen && this->no_border && !dragging) {
	      dragging = 1;
	      drx = bev->x_root;
	      dry = bev->y_root;
	    }
	  }
	} else if(bev->button == Button3) {
	  if(!this->fullscreen) {
	    if(!this->stay_above) {
	      set_above(this, 1);
	    } else if(!this->no_border) {
	      set_border(this, 0);
	    } else {
	      set_border(this, 1);
	      set_above(this, 0);
	    }
	  }
	}
	break;
      }

      case KeyPress:
      case KeyRelease:
      {
	XKeyEvent *kevent = (XKeyEvent *) &event;
	KeySym          ks;
	char            *ksname;
	char            buffer[20];
	int             buf_len = 20;
	XComposeStatus  status;

	if(kevent->keycode) {
	  XLockDisplay (this->display);
	  XLookupString(kevent, buffer, buf_len, &ks, &status);
	  XUnlockDisplay (this->display);
	  ksname = XKeysymToString(ks);
#if defined(XINELIBOUTPUT_FE_TOGGLE_FULLSCREEN) || defined(INTERPRET_LIRC_KEYS)
	  if(ks == XK_f || ks == XK_F) {
	    sxfe_toggle_fullscreen(this);
	  } else if(ks == XK_d || ks == XK_D) {
	    xine_set_param(this->stream, XINE_PARAM_VO_DEINTERLACE, 
			   xine_get_param(this->stream, XINE_PARAM_VO_DEINTERLACE) ? 0 : 1);
	  } else
#endif
#ifdef FE_STANDALONE
	  if(ks == XK_Escape) {
	    terminate_key_pressed = 1;
	    keep_going = 0;
	  } else if(this->input || find_input(this))
	    process_xine_keypress(this->input, "XKeySym",ksname, 0, 0);
#else
	  if(this->keypress) 
	    this->keypress("XKeySym",ksname);
#endif
	}
      }
      break;
      
      case ClientMessage:
      {
	XClientMessageEvent *cmessage = (XClientMessageEvent *) &event;	
	if ( cmessage->message_type == this->atom_sxfe_interrupt )
	  LOGDBG("ClientMessage: sxfe_interrupt");

	if ( cmessage->data.l[0] == this->atom_wm_delete_window )
	  /* we got a window deletion message from out window manager.*/
	  LOGDBG("ClientMessage: WM_DELETE_WINDOW");
	  keep_going=0;
      }
    }
    
    if (event.type == this->completion_event)
      xine_port_send_gui_data (this->video_port, XINE_GUI_SEND_COMPLETION_EVENT, &event);
  }

  return keep_going;
}

static void sxfe_display_close(frontend_t *this_gen) 
{
  sxfe_t *this = (sxfe_t*)this_gen;

#ifdef HAVE_XRENDER
  hud_osd_close(this_gen);
#endif

  if(this && this->display) {
    
    if(this->xine)
      this->fe.xine_exit(this_gen);
    
#ifdef HAVE_XDPMS
    if(this->dpms_state == TRUE)
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
}

static int sxfe_xine_play(frontend_t *this_gen)
{
  int r = fe_xine_play(this_gen);

#ifdef FE_STANDALONE
# ifdef HAVE_XRENDER
  sxfe_t *this = (sxfe_t*)this_gen;

  if(r && this->input && this->hud) {
    vdr_input_plugin_t *input_vdr = (vdr_input_plugin_t *)this->input;
    LOGDBG("sxfe_xine_play: Enabling HUD OSD");
    input_vdr->f.fe_handle  = this_gen;
    input_vdr->f.intercept_osd = hud_osd_command;
  }
# endif /* HAVE_XRENDER */
#endif /* FE_STANDALONE */

  return r;
}

static frontend_t *sxfe_get_frontend(void)
{
  sxfe_t *this = malloc(sizeof(sxfe_t));
  memset(this, 0, sizeof(sxfe_t));

  this->window_id = -1;
  
  this->fe.fe_display_open   = sxfe_display_open;
  this->fe.fe_display_config = sxfe_display_config;
  this->fe.fe_display_close  = sxfe_display_close;
  
  this->fe.xine_init  = fe_xine_init;
  this->fe.xine_open  = fe_xine_open;
  this->fe.xine_play  = sxfe_xine_play;
  this->fe.xine_stop  = fe_xine_stop;
  this->fe.xine_close = fe_xine_close;
  this->fe.xine_exit  = fe_xine_exit;
  this->fe.xine_is_finished = fe_is_finished;
  
  this->fe.fe_run  = sxfe_run;
  this->fe.fe_interrupt = sxfe_interrupt;
  this->fe.fe_free = fe_free;

  this->fe.grab                  = fe_grab;
#ifndef FE_STANDALONE
  this->fe.xine_osd_command      = xine_osd_command;
  this->fe.xine_control          = xine_control;

  this->fe.xine_queue_pes_packet = xine_queue_pes_packet;
#endif /*#ifndef FE_STANDALONE */

  return (frontend_t*)this;
}

/* ENTRY POINT */
const fe_creator_f fe_creator __attribute__((visibility("default"))) = sxfe_get_frontend;



