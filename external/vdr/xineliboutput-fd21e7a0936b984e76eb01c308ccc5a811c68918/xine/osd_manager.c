/*
 * osd_manager.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: osd_manager.c,v 1.30 2014-06-23 12:20:28 phintuka Exp $
 *
 */

#include <stdlib.h>
#include <pthread.h>

#define XINE_ENGINE_INTERNAL
#include <xine/xine_internal.h>
#include <xine/video_out.h>

#include "../xine_input_vdr.h"

#include "../tools/osd_command.h"
#include "../tools/rle.h"

#include "vo_props.h"
#include "osd_manager.h"

/*#define LOGOSD(x...) LOGMSG(x)*/
#define LOGOSD(x...) do {} while(0)

static const char log_module_input_osd[] = "[input_osd] ";
#define LOG_MODULENAME log_module_input_osd
#define SysLogLevel    iSysLogLevel

#include "../logdefs.h"


typedef struct {
  int           handle;     /* xine-lib overlay handle */
  osd_command_t cmd;        /* Full OSD data: last OSD_Set_RLE event */

  uint16_t      extent_width;  /* output size this OSD was designed for */
  uint16_t      extent_height;

  uint16_t      video_window_x;
  uint16_t      video_window_y;
  uint16_t      video_window_w;
  uint16_t      video_window_h;

  int64_t       last_changed_vpts;

#ifdef VO_CAP_ARGB_LAYER_OVERLAY
  argb_layer_t *argb_layer;
  uint32_t     *argb_buffer;
#endif
} osd_data_t;

typedef struct osd_manager_impl_s {
  osd_manager_t    mgr;

  pthread_mutex_t  lock;
  uint8_t          ticket_acquired;
  xine_stream_t   *stream;

  uint16_t         video_width;
  uint16_t         video_height;
  uint16_t         win_width;
  uint16_t         win_height;
  uint8_t          vo_scaling;

  osd_data_t       osd[MAX_OSD_OBJECT];

} osd_manager_impl_t;

/************************************* Tools ************************************/

/*
 * argb layer (originally copied from xine - why aren't these functions exported ?)
 */

#ifdef VO_CAP_ARGB_LAYER_OVERLAY
static argb_layer_t *argb_layer_create() {

  argb_layer_t *argb_layer = (argb_layer_t *)calloc(1, sizeof (argb_layer_t));

  pthread_mutex_init(&argb_layer->mutex, NULL);

  return argb_layer;
}

static void argb_layer_destroy(argb_layer_t *argb_layer) {

  pthread_mutex_destroy(&argb_layer->mutex);
  free(argb_layer);
}

static void set_argb_layer(argb_layer_t **dst, argb_layer_t *src) {

  if (src) {
    pthread_mutex_lock(&src->mutex);
    ++src->ref_count;
    pthread_mutex_unlock(&src->mutex);
  }

  if (*dst) {
    int free_argb_layer;

    pthread_mutex_lock(&(*dst)->mutex);
    free_argb_layer = (0 == --(*dst)->ref_count);
    pthread_mutex_unlock(&(*dst)->mutex);

    if (free_argb_layer)
      argb_layer_destroy(*dst);
  }

  *dst = src;
}
#endif /* VO_CAP_ARGB_LAYER_OVERLAY */


/*
 * acquire_ticket()
 */
static void acquire_ticket(osd_manager_impl_t *this)
{
  if (!this->ticket_acquired) {
    this->stream->xine->port_ticket->acquire(this->stream->xine->port_ticket, 1);
    this->ticket_acquired = 1;
  }
}

static void release_ticket(osd_manager_impl_t *this)
{
  if (this->ticket_acquired) {
    this->stream->xine->port_ticket->release(this->stream->xine->port_ticket, 1);
    this->ticket_acquired = 0;
  }
}

/*
 * get_overlay_manager()
 */
video_overlay_manager_t *get_ovl_manager(osd_manager_impl_t *this)
{
  /* Get overlay manager. We need ticket ... */
  acquire_ticket(this);
  video_overlay_manager_t *ovl_manager = this->stream->video_out->get_overlay_manager(this->stream->video_out);
  if (!ovl_manager) {
    LOGMSG("Stream has no overlay manager !");
    return NULL;
  }
  return ovl_manager;
}

/*
 * palette_argb_to_ayuv()
 */

#define saturate(x,min,max) ( (x)<(min) ? (min) : (x)>(max) ? (max) : (x))

static void palette_argb_to_ayuv(osd_clut_t *clut, int colors)
{
  if (clut && colors>0) {
    int c;
    for (c=0; c<colors; c++) {
      int R  = clut[c].r;
      int G  = clut[c].g;
      int B  = clut[c].b;
      int Y  = (( +  66*R + 129*G +  25*B + 0x80) >> 8) +  16;
      int CR = (( + 112*R -  94*G -  18*B + 0x80) >> 8) + 128;
      int CB = (( -  38*R -  74*G + 112*B + 0x80) >> 8) + 128;
      clut[c].y  = saturate( Y, 16, 235);
      clut[c].cb = saturate(CB, 16, 240);
      clut[c].cr = saturate(CR, 16, 240);
    }
  }
}

/*
 * clear_osdcmd()
 *
 *  - free allocated memory from osd_command_t
 */
static void clear_osdcmd(osd_command_t *cmd)
{
  free(cmd->data);
  cmd->data = NULL;
  free(cmd->palette);
  cmd->palette = NULL;
}

/*
 * osdcmd_to_overlay()
 *
 *  - fill xine-lib vo_overlay_t from osd_command_t
 */
static void osdcmd_to_overlay(vo_overlay_t *ovl, osd_command_t *cmd)
{
  uint i;

  ovl->rle       = (rle_elem_t*)cmd->data;
  ovl->data_size = cmd->datalen;
  ovl->num_rle   = cmd->datalen / 4;

  ovl->x      = cmd->x;
  ovl->y      = cmd->y;
  ovl->width  = cmd->w;
  ovl->height = cmd->h;

  /* palette */
  for (i=0; i<cmd->colors; i++) {
    ovl->color[i] = (*(uint32_t*)(cmd->palette + i)) & 0x00ffffff;
    ovl->trans[i] = (cmd->palette[i].alpha + 0x7)/0xf;
  }
  ovl->rgb_clut = cmd->flags & OSDFLAG_YUV_CLUT ? 0 : 1;

  ovl->unscaled = cmd->flags & OSDFLAG_UNSCALED ? 1 : 0;

  ovl->hili_top = ovl->hili_bottom = ovl->hili_left = ovl->hili_right = -1;
}

/*
 * osdcmd_scale()
 *
 * Scale OSD_Set_RLE data
 * - modified fields: x, y, w, h, (RLE) data and datalen
 * - old RLE data is stored to osd_data_t *osd
 */
static void osdcmd_scale(osd_manager_impl_t *this, osd_command_t *cmd,
                         osd_data_t *osd, int output_width, int output_height)
{
  LOGOSD("Size out of margins, rescaling rle image");

  /* new position and size */
  int new_x = cmd->x * this->video_width  / osd->extent_width;
  int new_y = cmd->y * this->video_height / osd->extent_height;
  int x2 = cmd->x + cmd->w + 1;
  int y2 = cmd->y + cmd->h + 1;
  x2  = ((x2+1) * this->video_width  - 1) / osd->extent_width;
  y2  = ((y2+1) * this->video_height - 1) / osd->extent_height;
  int new_w = x2 - new_x - 1;
  int new_h = y2 - new_y - 1;

  /* store original RLE data */
  osd->cmd.data    = cmd->data;
  osd->cmd.datalen = cmd->datalen;

  /* scale */
  int rle_elems = cmd->datalen / sizeof(osd_rle_elem_t);
  cmd->data = rle_scale_nearest(cmd->data, &rle_elems, cmd->w, cmd->h,
                                new_w, new_h);
  cmd->datalen = rle_elems * sizeof(osd_rle_elem_t);

  cmd->x = new_x;
  cmd->y = new_y;
  cmd->w = new_w;
  cmd->h = new_h;
}

/*
 * osd_exec_vpts()
 *
 *  - calculate execution time (vpts) for OSD update
 */
static int64_t osd_exec_vpts(osd_manager_impl_t *this, osd_command_t *cmd)
{
  int64_t vpts = 0; /* now */

  /* calculate exec time */
  if (cmd->pts || cmd->delay_ms) {
    int64_t now = xine_get_current_vpts(this->stream);

    if (cmd->pts)
      vpts = cmd->pts + this->stream->metronom->get_option(this->stream->metronom, METRONOM_VPTS_OFFSET);
    else
      vpts = this->osd[cmd->wnd].last_changed_vpts + cmd->delay_ms*90;

    /* execution time must be in future */
    if (vpts < now)
      vpts = 0;

    /* limit delay to 5 seconds */
    if (vpts > now + 5*90000)
      vpts = vpts + 5*90000;

    LOGOSD("OSD Command %d scheduled to +%dms", cmd->cmd, (int)(vpts>now ? vpts-now : 0)/90);
  }

  return vpts;
}

/***************************** OSD command handlers *****************************/

/*
 * exec_osd_size()
 *
 * - set the assumed full OSD size
 */
static int exec_osd_size(osd_manager_impl_t *this, osd_command_t *cmd)
{
  osd_data_t *osd = &this->osd[cmd->wnd];
  osd->extent_width  = cmd->w;
  osd->extent_height = cmd->h;

  osd->video_window_x = 0;
  osd->video_window_y = 0;
  osd->video_window_w = 0;
  osd->video_window_h = 0;

  acquire_ticket(this);

#ifdef VO_CAP_ARGB_LAYER_OVERLAY
  set_argb_layer(&osd->argb_layer, NULL);
  free(osd->argb_buffer);
  osd->argb_buffer = NULL;
#endif

  xine_video_port_t *video_out = this->stream->video_out;

  this->vo_scaling = !!(video_out->get_capabilities(video_out) & VO_XCAP_OSDSCALING);

  return CONTROL_OK;
}

/*
 * exec_osd_video_window()
 *
 * - set video window inside OSD
 */
static int exec_osd_video_window(osd_manager_impl_t *this, osd_command_t *cmd)
{
  osd_data_t *osd = &this->osd[cmd->wnd];

  osd->video_window_x = cmd->x;
  osd->video_window_y = cmd->y;
  osd->video_window_w = cmd->w;
  osd->video_window_h = cmd->h;

  return CONTROL_OK;
}

/*
 * exec_osd_nop()
 *
 * - update last changed time of an OSD window
 */
static int exec_osd_nop(osd_manager_impl_t *this, osd_command_t *cmd)
{
  this->osd[cmd->wnd].last_changed_vpts = xine_get_current_vpts(this->stream);
  return CONTROL_OK;
}

/*
 * exec_osd_flush()
 *
 * - commit all pending OSD events immediately
 */
static int exec_osd_flush(osd_manager_impl_t *this, osd_command_t *cmd)
{
  video_overlay_manager_t *ovl_manager = get_ovl_manager(this);
  if (!ovl_manager)
    return CONTROL_PARAM_ERROR;

  ovl_manager->flush_events(ovl_manager);

  return CONTROL_OK;
}

/*
 * exec_osd_close()
 *
 */
static int exec_osd_close(osd_manager_impl_t *this, osd_command_t *cmd)
{
  video_overlay_manager_t *ovl_manager = get_ovl_manager(this);
  osd_data_t              *osd         = &this->osd[cmd->wnd];
  int                      handle      = osd->handle;

  if (cmd->flags & OSDFLAG_REFRESH) {
    LOGDBG("Ignoring OSD_Close(OSDFLAG_REFRESH)");
    return CONTROL_OK;
  }

  if (handle < 0) {
    LOGMSG("OSD_Close(%d): non-existing OSD !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }

  if (!ovl_manager)
    return CONTROL_PARAM_ERROR;

  video_overlay_event_t ov_event = {0};
  ov_event.event_type    = OVERLAY_EVENT_FREE_HANDLE;
  ov_event.vpts          = osd_exec_vpts(this, cmd);
  ov_event.object.handle = handle;

  while (ovl_manager->add_event(ovl_manager, (void *)&ov_event) < 0) {
    LOGMSG("OSD_Close(%d): overlay manager queue full !", cmd->wnd);
    ovl_manager->flush_events(ovl_manager);
  }

  clear_osdcmd(&osd->cmd);
  osd->handle        = -1;
  osd->extent_width  = 720;
  osd->extent_height = 576;
  osd->last_changed_vpts = 0;

#ifdef VO_CAP_ARGB_LAYER_OVERLAY
  set_argb_layer(&osd->argb_layer, NULL);
  free(osd->argb_buffer);
  osd->argb_buffer = NULL;
#endif

  return CONTROL_OK;
}

/*
 * exec_osd_set_rle()
 *
 */
static int exec_osd_set_rle(osd_manager_impl_t *this, osd_command_t *cmd)
{
  video_overlay_manager_t *ovl_manager = get_ovl_manager(this);
  video_overlay_event_t    ov_event    = {0};
  vo_overlay_t             ov_overlay  = {0};
  osd_data_t              *osd         = &this->osd[cmd->wnd];
  int use_unscaled       = 0;
  int rle_scaled         = 0;
  int unscaled_supported;
  int handle             = osd->handle;

  if (!ovl_manager)
    return CONTROL_PARAM_ERROR;

  this->stream->video_out->enable_ovl(this->stream->video_out, 1);

  /* get / allocate OSD handle */
  if (handle < 0) {
    handle = ovl_manager->get_handle(ovl_manager,0);
    osd->handle            = handle;
    osd->extent_width      = osd->extent_width  ?: 720;
    osd->extent_height     = osd->extent_height ?: 576;
    osd->last_changed_vpts = 0;
  }

  /* fill SHOW event */
  ov_event.event_type         = OVERLAY_EVENT_SHOW;
  ov_event.vpts               = osd_exec_vpts(this, cmd);
  ov_event.object.handle      = handle;
  ov_event.object.overlay     = &ov_overlay;
  ov_event.object.object_type = 1; /* menu */

  /* check for unscaled OSD capability and request */
  xine_video_port_t *video_out = this->stream->video_out;
  unscaled_supported = !!(video_out->get_capabilities(video_out) & VO_CAP_UNSCALED_OVERLAY);
  if (unscaled_supported) {
    if (cmd->flags & OSDFLAG_UNSCALED)
      use_unscaled = 1;
    if (cmd->flags & (OSDFLAG_UNSCALED | OSDFLAG_UNSCALED_LOWRES)) {
      this->win_width  = video_out->get_property(video_out, VO_PROP_WINDOW_WIDTH);
      this->win_height = video_out->get_property(video_out, VO_PROP_WINDOW_HEIGHT);
    }
  }

  /* store osd for later rescaling (done if video size changes) */

  clear_osdcmd(&osd->cmd);

  memcpy(&osd->cmd, cmd, sizeof(osd_command_t));
  osd->cmd.data = NULL;
  if (cmd->palette) {
    /* RGB -> YUV */
    if(!(cmd->flags & OSDFLAG_YUV_CLUT))
      palette_argb_to_ayuv(cmd->palette, cmd->colors);
    cmd->flags |= OSDFLAG_YUV_CLUT;

    osd->cmd.palette = malloc(sizeof(osd_clut_t)*cmd->colors);
    memcpy(osd->cmd.palette, cmd->palette, 4*cmd->colors);
    osd->cmd.flags |= OSDFLAG_YUV_CLUT;
  }

  /* request OSD scaling from video_out layer */
  this->vo_scaling = !!(video_out->get_capabilities(video_out) & VO_XCAP_OSDSCALING);
  if (this->vo_scaling) {
    video_out->set_property(video_out, VO_PROP_OSD_SCALING, cmd->scaling ? 1 : 0);
  }

  /* if video size differs from expected (VDR osd is designed for 720x576),
     scale osd to video size or use unscaled (display resolution)
     blending */

  /* scale OSD ? */
  if (!this->vo_scaling && !use_unscaled) {
    int w_diff = (this->video_width  < ((osd->extent_width *242) >> 8) /*  95% */) ? -1 :
                 (this->video_width  > ((osd->extent_width *280) >> 8) /* 110% */) ?  1 : 0;
    int h_diff = (this->video_height < ((osd->extent_height*242) >> 8) /*  95% */) ? -1 :
                 (this->video_height > ((osd->extent_height*280) >> 8) /* 110% */) ?  1 : 0;

    if (w_diff || h_diff) {

      /* unscaled OSD instead of downscaling ? */
      if (w_diff < 0 || h_diff < 0)
        if (cmd->flags & OSDFLAG_UNSCALED_LOWRES)
          if (0 < (use_unscaled = unscaled_supported)) {
            LOGOSD("Size out of margins, using unscaled overlay");
            }
      if (!use_unscaled && cmd->scaling > 0) {
        osdcmd_scale(this, cmd, osd, this->video_width, this->video_height);
        rle_scaled = 1;
      }
    }
  }

  /* Scale unscaled OSD ? */
  if (!this->vo_scaling && use_unscaled && cmd->scaling > 0) {
    if (this->win_width >= 360 && this->win_height >= 288) {
      if (this->win_width != osd->extent_width || this->win_height != osd->extent_height) {
        osdcmd_scale(this, cmd, osd, this->win_width, this->win_height);
        rle_scaled = 1;
      }
    }
  }

  /* fill ov_overlay */
  osdcmd_to_overlay(&ov_overlay, cmd);
  ov_overlay.unscaled = use_unscaled;

  /* tag this overlay */
  ov_overlay.hili_rgb_clut = VDR_OSD_MAGIC;

  /* fill extra data */
  const vdr_osd_extradata_t extra_data = {
    extent_width:  osd->extent_width,
    extent_height: osd->extent_height,
    layer:         cmd->layer,
    scaling:       cmd->scaling
  };
  memcpy(ov_overlay.hili_color, &extra_data, sizeof(extra_data));

#ifdef VO_CAP_CUSTOM_EXTENT_OVERLAY
  if (cmd->scaling && !rle_scaled) {
    ov_overlay.extent_width   = osd->extent_width;
    ov_overlay.extent_height  = osd->extent_height;
  }
#endif
#ifdef VO_CAP_VIDEO_WINDOW_OVERLAY
  ov_overlay.video_window_x      = osd->video_window_x ?: -1;
  ov_overlay.video_window_y      = osd->video_window_y ?: -1;
  ov_overlay.video_window_width  = osd->video_window_w ?: -1;
  ov_overlay.video_window_height = osd->video_window_h ?: -1;
#endif

  /* if no scaling was required, we may still need to re-center OSD */
  if (!this->vo_scaling && !rle_scaled) {
    if (!use_unscaled) {
      if (this->video_width != osd->extent_width)
        ov_overlay.x += (this->video_width - osd->extent_width)/2;
      if (this->video_height != osd->extent_height)
        ov_overlay.y += (this->video_height - osd->extent_height)/2;
    } else {
      if (this->win_width >= 360 && this->win_height >= 288) {
        if (this->win_width != osd->extent_width)
          ov_overlay.x += (this->win_width - osd->extent_width)/2;
        if (this->win_height != osd->extent_height)
          ov_overlay.y += (this->win_height - osd->extent_height)/2;
      }
    }
  }

  /* store rle for later scaling (done if video size changes) */
  if (!rle_scaled /* if scaled, we already have a copy (original data) */ ) {
    osd->cmd.data = malloc(cmd->datalen);
    memcpy(osd->cmd.data, cmd->data, cmd->datalen);
  }
  cmd->data = NULL; /* we 'consume' data (ownership goes for xine-lib osd manager) */

  /* send event to overlay manager */
  while (ovl_manager->add_event(ovl_manager, (void *)&ov_event) < 0) {
    LOGMSG("OSD_Set_RLE(%d): overlay manager queue full !", cmd->wnd);
    ovl_manager->flush_events(ovl_manager);
  }

  osd->last_changed_vpts = ov_event.vpts ?: xine_get_current_vpts(this->stream);

  return CONTROL_OK;
}

static int exec_osd_set_lut8(osd_manager_impl_t *this, osd_command_t *cmd)
{
  osd_command_t cmdrle;
  memcpy(&cmdrle, cmd, sizeof(osd_command_t));
  if (cmd->data) {
    cmdrle.num_rle = rle_compress(&cmdrle.data, cmdrle.raw_data, cmdrle.w, cmdrle.h);
    cmdrle.datalen = 4 * cmdrle.num_rle;
    cmdrle.cmd = OSD_Set_RLE;
  }

  return exec_osd_set_rle(this, &cmdrle);
}

static int exec_osd_set_argb(osd_manager_impl_t *this, osd_command_t *cmd)
{
#ifdef VO_CAP_ARGB_LAYER_OVERLAY
  video_overlay_manager_t *ovl_manager = get_ovl_manager(this);
  video_overlay_event_t    ov_event    = {0};
  vo_overlay_t             ov_overlay  = {0};
  osd_data_t              *osd         = &this->osd[cmd->wnd];
  int                      handle      = osd->handle;

  if (!ovl_manager)
    return CONTROL_PARAM_ERROR;

  if (!this->mgr.argb_supported(this->stream)) {
    LOGMSG("ARGB overlay not supported by video driver");
    return CONTROL_PARAM_ERROR;
  }

  if (osd->extent_width < 32 || osd->extent_height < 32) {
    LOGMSG("ARGB overlay: incorrect extent");
    return CONTROL_PARAM_ERROR;
  }

  this->stream->video_out->enable_ovl(this->stream->video_out, 1);

  /* get / allocate OSD handle */
  if (handle < 0) {
    handle = ovl_manager->get_handle(ovl_manager,0);
    osd->handle            = handle;
    osd->extent_width      = osd->extent_width  ?: 720;
    osd->extent_height     = osd->extent_height ?: 576;
    osd->last_changed_vpts = 0;
  }

  /* fill SHOW event */
  ov_event.event_type         = OVERLAY_EVENT_SHOW;
  ov_event.vpts               = osd_exec_vpts(this, cmd);
  ov_event.object.handle      = handle;
  ov_event.object.overlay     = &ov_overlay;
  ov_event.object.object_type = 1; /* menu */

  /* ARGB overlays are not cached for re-scaling */
  clear_osdcmd(&osd->cmd);

  /* fill ov_overlay */
  ov_overlay.x      = 0;
  ov_overlay.y      = 0;
  ov_overlay.width  = osd->extent_width;
  ov_overlay.height = osd->extent_height;

  /* tag this overlay */
  ov_overlay.hili_rgb_clut = VDR_OSD_MAGIC;

  /* fill extra data */
  const vdr_osd_extradata_t extra_data = {
    extent_width:  osd->extent_width,
    extent_height: osd->extent_height,
    layer:         cmd->layer,
    scaling:       cmd->scaling
  };
  memcpy(ov_overlay.hili_color, &extra_data, sizeof(extra_data));

  /* xine-lib 1.2 extensions */
  ov_overlay.extent_width        = osd->extent_width;
  ov_overlay.extent_height       = osd->extent_height;
  ov_overlay.video_window_x      = osd->video_window_x ?: -1;
  ov_overlay.video_window_y      = osd->video_window_y ?: -1;
  ov_overlay.video_window_width  = osd->video_window_w ?: -1;
  ov_overlay.video_window_height = osd->video_window_h ?: -1;

  /* this should trigger blending at output resolution (after scaling video frame) ... */
  //ov_overlay.unscaled = 1;
  ov_overlay.unscaled = cmd->flags & OSDFLAG_UNSCALED ? 1 : 0;

  /* allocate buffer */
  if (!osd->argb_buffer) {
    osd->argb_buffer = calloc(sizeof(uint32_t), osd->extent_width * osd->extent_height);
  }
  if (!osd->argb_layer) {
    set_argb_layer(&osd->argb_layer, argb_layer_create());
    osd->argb_layer->buffer = osd->argb_buffer;
    osd->argb_layer->x1 = 0;
    osd->argb_layer->x2 = osd->extent_width - 1;
    osd->argb_layer->y1 = 0;
    osd->argb_layer->y2 = osd->extent_height - 1;
  }

  /* copy changed data to buffer */
  /* TODO: do we need double-buffering or locking to prevent tearing ? */

  uint32_t *src = (uint32_t*)cmd->raw_data;
  uint32_t* dst = osd->argb_buffer;
  int stride = cmd->w;
  int lines = cmd->h;

  /* clip */
  if (cmd->x + cmd->w > osd->extent_width) {
    stride = osd->extent_width - cmd->x;
    if (stride < 0) stride = 0;
    LOGMSG("ARGB overlay: incorrect extent, cropping right side");
  }
  if (cmd->y + cmd->h > osd->extent_height) {
    lines = osd->extent_height - cmd->y;
    LOGMSG("ARGB overlay: incorrect extent, cropping bottom");
  }

  /* blend */
  dst += cmd->y * osd->extent_width + cmd->x;
  for (; lines > 0; lines--) {
    memcpy(dst, src, stride * sizeof(uint32_t));
    src += cmd->w;
    dst += osd->extent_width;
  }

  /* set dirty area. not used in opengl2 driver ... */
#if 0
  /* this can't work... There's no way to know if driver has
     copied the dirty area to display. So dirty area can never be
     shrinked, just expanded ... */
  osd->argb_layer->x1 = cmd->x;
  osd->argb_layer->x2 = cmd->x + cmd->w - 1;
  osd->argb_layer->y1 = cmd->y;
  osd->argb_layer->y2 = cmd->y + cmd->h - 1;
#else
  /* argb layer update area accumulation */
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
  osd->argb_layer->x1 = MIN( osd->argb_layer->x1, cmd->x );
  osd->argb_layer->x2 = MAX( osd->argb_layer->x2, cmd->x + cmd->w - 1);
  osd->argb_layer->y1 = MIN( osd->argb_layer->y1, cmd->y );
  osd->argb_layer->y2 = MAX( osd->argb_layer->y2, cmd->y + cmd->h - 1);
#endif

  /* set buffer (ref-counted) */
  set_argb_layer(&ov_overlay.argb_layer, osd->argb_layer);

  /* send event to overlay manager */
  while (ovl_manager->add_event(ovl_manager, (void *)&ov_event) < 0) {
    LOGMSG("OSD_Set_ARGB(%d): overlay manager queue full !", cmd->wnd);
    ovl_manager->flush_events(ovl_manager);
  }

  /* release buffer (ref-counted) */
  set_argb_layer(&ov_overlay.argb_layer, NULL);

  osd->last_changed_vpts = ov_event.vpts ?: xine_get_current_vpts(this->stream);

  return CONTROL_OK;

#else /* VO_CAP_ARGB_LAYER_OVERLAY */
  LOGMSG("OSD_Set_ARGB: plugin was build without ARGB support");
  return CONTROL_PARAM_ERROR;
#endif
}

/*
 * exec_osd_set_palette()
 *
 * - replace palette of an already existing OSD
 */
static int exec_osd_set_palette(osd_manager_impl_t *this, osd_command_t *cmd)
{
  osd_data_t *osd = &this->osd[cmd->wnd];

  if (!osd->cmd.data) {
    LOGMSG("OSD_SetPalette(%d): old RLE data missing !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }
  if (!cmd->palette) {
    LOGMSG("OSD_SetPalette(%d): new palette missing !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }

  /* use cached event to re-create Set_RLE command with modified palette */
  osd_command_t tmp;
  /* steal the original command */
  memcpy(&tmp, &osd->cmd, sizeof(osd_command_t));
  memset(&osd->cmd, 0, sizeof(osd_command_t));

  /* replace palette */
  tmp.cmd      = OSD_Set_RLE;
  free(tmp.palette);
  tmp.palette  = malloc(cmd->colors * sizeof(osd_rle_elem_t));
  memcpy(tmp.palette, cmd->palette, cmd->colors * sizeof(osd_rle_elem_t));
  tmp.colors   = cmd->colors;
  tmp.pts      = cmd->pts;
  tmp.delay_ms = cmd->delay_ms;
  tmp.flags   |= cmd->flags & OSDFLAG_YUV_CLUT;

  /* redraw */
  int r = exec_osd_set_rle(this, &tmp);
  clear_osdcmd(&tmp);
  return r;
}

/*
 * exec_osd_move()
 *
 * - move existing OSD to new position
 */
static int exec_osd_move(osd_manager_impl_t *this, osd_command_t *cmd)
{
  osd_data_t *osd = &this->osd[cmd->wnd];

  if (!osd->cmd.data) {
    LOGMSG("OSD_Move(%d): old RLE data missing !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }
  if (!osd->cmd.palette) {
    LOGMSG("OSD_Move(%d): old palette missing !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }

  /* use cached event to re-create Set_RLE command with modified palette */
  osd_command_t tmp;
  /* steal the original command */
  memcpy(&tmp, &osd->cmd, sizeof(osd_command_t));
  memset(&osd->cmd, 0, sizeof(osd_command_t));

  /* replace position */
  tmp.cmd      = OSD_Set_RLE;
  tmp.x        = cmd->x;
  tmp.y        = cmd->y;
  tmp.pts      = cmd->pts;
  tmp.delay_ms = cmd->delay_ms;

  /* redraw */
  int r = exec_osd_set_rle(this, &tmp);
  clear_osdcmd(&tmp);
  return r;
}

/*
 * exec_osd_command_internal()
 */
static int exec_osd_command_internal(osd_manager_impl_t *this, struct osd_command_s *cmd)
{
  LOGOSD("exec_osd_command %d", cmd->cmd);

  switch (cmd->cmd) {
  case OSD_Nop:        return exec_osd_nop(this, cmd);
  case OSD_Size:       return exec_osd_size(this, cmd);
  case OSD_SetPalette: return exec_osd_set_palette(this, cmd);
  case OSD_Move:       return exec_osd_move(this, cmd);
  case OSD_Flush:      return exec_osd_flush(this, cmd);
  case OSD_Set_RLE:    return exec_osd_set_rle(this, cmd);
  case OSD_Set_LUT8:   return exec_osd_set_lut8(this, cmd);
  case OSD_Set_ARGB:   return exec_osd_set_argb(this, cmd);
  case OSD_Close:      return exec_osd_close(this, cmd);
  case OSD_VideoWindow:return exec_osd_video_window(this, cmd);
  case OSD_Commit:
    /* All OSD areas have been updated, commit changes to display */
    /* - not used with traditional xine-lib OSD */
    return CONTROL_OK;
  case OSD_Set_YUV:
    /* TODO */
    LOGMSG("OSD_Set_YUV not implemented !");
    return CONTROL_PARAM_ERROR;
  default:
    LOGMSG("Unknown OSD command %d", cmd->cmd);
    return CONTROL_PARAM_ERROR;
  }

  return CONTROL_PARAM_ERROR;
}

/****************************** Interface handlers ******************************/

/*
 * exec_osd_command()
 *
 * - handler for VDR-based osd_command_t events
 */
static int exec_osd_command(osd_manager_t *this_gen,
                            struct osd_command_s *cmd, xine_stream_t *stream)
{
  osd_manager_impl_t *this = (osd_manager_impl_t*)this_gen;
  int result;

  if (!cmd || !stream) {
    LOGMSG("exec_osd_command: Stream not initialized !");
    return CONTROL_DISCONNECTED;
  }
  if (cmd->wnd >= MAX_OSD_OBJECT) {
    LOGMSG("exec_osd_command: OSD window handle %d out of range !", cmd->wnd);
    return CONTROL_PARAM_ERROR;
  }

  if (pthread_mutex_lock(&this->lock)) {
    LOGERR("exec_osd_command: mutex lock failed");
    return CONTROL_DISCONNECTED;
  }

  this->stream = stream;
  result = exec_osd_command_internal(this, cmd);

  release_ticket(this);

  pthread_mutex_unlock(&this->lock);

  return result;
}

/*
 * video_size_changed()
 */
static void video_size_changed(osd_manager_t *this_gen, xine_stream_t *stream, int width, int height)
{
  osd_manager_impl_t *this = (osd_manager_impl_t*)this_gen;
  int i;

  if (!stream) {
    LOGMSG("video_size_changed: Stream not initialized !");
    return;
  }

  if (width < 1 || height < 1) {
    LOGMSG("video_size_changed: Invalid video size %dx%d", width, height);
    return;
  }

  if (pthread_mutex_lock(&this->lock)) {
    LOGERR("video_size_changed: mutex lock failed");
    return;
  }

  if (this->video_width == width && this->video_height == height) {
    pthread_mutex_unlock(&this->lock);
    return;
  }

  LOGOSD("New video size (%dx%d->%dx%d)", this->video_width, this->video_height, width, height);

  this->stream       = stream;
  this->video_width  = width;
  this->video_height = height;

  /* just call exec_osd_command for all stored osd's.
     scaling is done automatically if required. */
  if (!this->vo_scaling)
    for (i = 0; i < MAX_OSD_OBJECT; i++)
      if (this->osd[i].handle >= 0 &&
          this->osd[i].cmd.data &&
          this->osd[i].cmd.scaling > 0) {
        osd_command_t tmp;
        memcpy(&tmp, &this->osd[i].cmd, sizeof(osd_command_t));
        memset(&this->osd[i].cmd, 0, sizeof(osd_command_t));

        exec_osd_command_internal(this, &tmp);

        clear_osdcmd(&tmp);
      }

  release_ticket(this);
  pthread_mutex_unlock(&this->lock);
}

static int argb_supported(xine_stream_t *stream)
{
#ifdef VO_CAP_ARGB_LAYER_OVERLAY
  xine_video_port_t *video_out = stream->video_out;
  return !!(video_out->get_capabilities(video_out) & VO_CAP_ARGB_LAYER_OVERLAY);
#else
  return 0;
#endif
}

/*
 * osd_manager_dispose()
 */
static void osd_manager_dispose(osd_manager_t *this_gen, xine_stream_t *stream)
{
  osd_manager_impl_t *this = (osd_manager_impl_t*)this_gen;
  int i;

  while (pthread_mutex_destroy(&this->lock) == EBUSY) {
    LOGMSG("osd_manager_dispose: lock busy ...");
    pthread_mutex_lock(&this->lock);
    pthread_mutex_unlock(&this->lock);
  }

  /* close all */
  for (i=0; i<MAX_OSD_OBJECT; i++) {
    if (this->osd[i].handle >= 0) {
      osd_command_t cmd = {
        .cmd = OSD_Close,
        .wnd = i,
      };
      LOGOSD("Closing osd %d", i);
      exec_osd_close(this, &cmd);
    }
  }

  release_ticket(this);

  free(this);
}

/*
 * init_osd_manager()
 *
 * - exported
 */
osd_manager_t *init_osd_manager(void)
{
  osd_manager_impl_t *this = calloc(1, sizeof(osd_manager_impl_t));
  int i;

  this->mgr.command            = exec_osd_command;
  this->mgr.dispose            = osd_manager_dispose;
  this->mgr.video_size_changed = video_size_changed;
  this->mgr.argb_supported     = argb_supported;

  pthread_mutex_init(&this->lock, NULL);

  this->video_width  = 720;
  this->video_height = 576;

  for (i = 0; i < MAX_OSD_OBJECT; i++)
    this->osd[i].handle = -1;

  return &this->mgr;
}
