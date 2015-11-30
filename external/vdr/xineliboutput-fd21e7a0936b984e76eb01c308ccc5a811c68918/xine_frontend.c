/*
 * xine_frontend.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_frontend.c,v 1.127 2013-10-28 07:45:50 phintuka Exp $
 *
 */

#include "features.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef HAVE_LIBJPEG
# ifdef boolean
#  define HAVE_BOOLEAN
# endif
# include <jpeglib.h>
# undef boolean
#endif

#define XINE_ENGINE_INTERNAL
#include <xine.h>
#include <xine/xine_internal.h>

#define LOG_MODULENAME "[vdr-fe]    "
#include "logdefs.h"

#include "xine_frontend_internal.h"
#include "xine/post.h"

#include "xine/vo_post.h"
#include "xine/vo_osdscaler.h"
#include "xine/vo_osdreorder.h"
#include "xine/vo_lastpts.h"
#include "xine/vo_frameoutput.h"

#undef  MIN
#define MIN(a,b) ( (a) < (b) ? (a) : (b))
#undef  MAX
#define MAX(a,b) ( (a) > (b) ? (a) : (b))


static void intercept_video_driver(xine_video_port_t *video_port)
{
  vo_driver_t *osdscaler = osdscaler_init();
  if (! wire_video_driver(video_port, osdscaler)) {
    LOGMSG("wire_video_driver() for osdscaler failed");
    osdscaler->dispose(osdscaler);
  }

  vo_driver_t *osdreorder = osd_reorder_init();
  if (! wire_video_driver(video_port, osdreorder)) {
    LOGMSG("wire_video_driver() for osdreorder failed");
    osdreorder->dispose(osdreorder);
  }

  vo_driver_t *lastpts = vo_lastpts_init();
  if (! wire_video_driver(video_port, lastpts)) {
    LOGMSG("wire_video_driver() for vo_lastpts failed");
    lastpts->dispose(lastpts);
  }
}


char *strn0cpy(char *dest, const char *src, int n) 
{
  char *s = dest;
  for ( ; --n && (*dest = *src) != 0; dest++, src++) ;
  *dest = 0;
  return s;
}

static int guess_cpu_count(void)
{
  static int cores = -1;
  FILE *f;

  if(cores >= 0)
    return cores;

  cores = 0;
  if(NULL != (f = fopen("/proc/cpuinfo", "r"))) {
    char buf[256];
    while (NULL != fgets(buf, 255, f))
      sscanf(buf, "processor : %d", &cores);
    fclose(f);
  }
  cores++;

  if(cores > 1) 
    LOGMSG("Detected %d CPUs", cores);
  else
    LOGDBG("Detected single CPU. Multithreaded decoding and post processing disabled.");

  return cores;
}

static void shutdown_system(char *cmd, int user_requested)
{
  const char *reason = user_requested ? "User requested system shutdown" : "Inactivity timer elapsed";

  if (cmd) {
    LOGMSG("%s. Executing '%s'", reason, cmd);
    if (-1 == system(cmd))
      LOGERR("Can't execute %s", cmd);
  } else {
    LOGMSG("%s, power off comand undefined!", reason);
  }
}

static void make_dirs(const char *file)
{
  struct stat st;
  char *s = strdup(file);
  char *p = s;

  if (*p == '/') {
    p++;
    while ((p = strchr(p, '/')) != NULL) {
      *p = 0;
      if (stat(s, &st) != 0 || !S_ISDIR(st.st_mode)) {
        if (mkdir(s, ACCESSPERMS) == -1) {
          LOGERR("Can't create %s", s);
          break;
        }
        LOGDBG("Created directory %s", s);
      }
      *p++ = '/';
    }
  }

  free(s);
}

/*
 * list available plugins
 */

static void list_plugins_type(xine_t *xine, const char *msg, typeof (xine_list_audio_output_plugins) list_func)
{
  static xine_t *tmp_xine = NULL;
  if(!xine) {
    if(!tmp_xine)
      xine_init(tmp_xine = xine_new());
    xine = tmp_xine;
  }
  const char *const *list = list_func(xine);

  printf("%s", msg);
  while(list && *list)
    printf(" %s", *list++);
  printf("\n");
}

void list_xine_plugins(frontend_t *fe, int verbose)
{
  fe_t *this = (fe_t *)fe;

  xine_t *tmp_xine = NULL;
  xine_t *xine = this ? this->xine : NULL;

  if (!xine)
    xine_init(xine = tmp_xine = xine_new());

  list_plugins_type (xine, "Available video drivers:", xine_list_video_output_plugins);
  list_plugins_type (xine, "Available audio drivers:", xine_list_audio_output_plugins); 
  if (verbose) {
    list_plugins_type (xine, "Available post plugins: ", xine_list_post_plugins); 
    list_plugins_type (xine, "Available input plugins:", xine_list_input_plugins);
    list_plugins_type (xine, "Available demux plugins:", xine_list_demuxer_plugins);
    list_plugins_type (xine, "Available audio decoder plugins:", xine_list_audio_decoder_plugins);
    list_plugins_type (xine, "Available video decoder plugins:", xine_list_video_decoder_plugins);
    list_plugins_type (xine, "Available SPU decoder plugins:  ", xine_list_spu_plugins);
  }

  if (tmp_xine)
    xine_exit(tmp_xine);
}

/*
 * detect input plugin 
 */

static int find_input_plugin(fe_t *this)
{
  if(!this->input_plugin) {
    if(!this->stream || !this->stream->input_plugin ||
       !this->stream->input_plugin->input_class || this->playback_finished) {
      LOGMSG("find_input_plugin: stream not initialized or playback finished !");
      usleep(100*1000);
      return 0;
    }
#if XINE_VERSION_CODE < 10190
    if(strcmp(this->stream->input_plugin->input_class->get_identifier(
	      this->stream->input_plugin->input_class),
              MRL_ID)) {
#else
    if(strcmp(this->stream->input_plugin->input_class->identifier,
              MRL_ID)) {
#endif
      LOGMSG("find_input_plugin: current xine input plugin is not " MRL_ID " !");
      return 0;
    }
    this->input_plugin = (vdr_input_plugin_if_t*)this->stream->input_plugin;
  }
  return 1;
}

static void *fe_control(frontend_t *this_gen, const char *cmd);

/*
 * xine callbacks
 */

static double fe_dest_pixel_aspect(const fe_t *this, double video_pixel_aspect,
				   int video_width, int video_height)
{
  /*int new_cropping = 0;*/
  double result = 1.0;

  if(!this->scale_video) {

    /*#warning what to do if scaling disabled ???*/

    /*return video_pixel_aspect;*/
  }

  switch(this->aspect) {
    /* Auto */
    default: {
               double correction = 
		 ((double)video_width/(double)video_height) /
		 ((double)this->width/(double)this->height);

               result = video_pixel_aspect * correction;
               if(result > (16.9/9.0 * (double)this->height/
			    (double)this->width))
	         result = (16.0/9.0 * (double)this->height/
			   (double)this->width);
	       break;
             }
    /* Default */
    case 1:  result = this->display_ratio; break;

    /* 4:3 */
    case 2:  result = (4.0/3.0 * (double)this->height/(double)this->width); break;
    /* 16:9 */
    case 3:  result = (16.0/9.0 * (double)this->height/(double)this->width); break;
    /* 16:10 */
    case 4:  result = (16.0/10.0 * (double)this->height/(double)this->width); break;
    /* Pan&Scan */
    case 5: {
      double aspect_diff /*= video_pixel_aspect - 1.0*/;
	      /* TODO */
	      /* does not work (?) */
aspect_diff=(video_pixel_aspect*(double)video_width/(double)video_height) - 4.0 / 3.0;
              if ((aspect_diff < 0.05) && (aspect_diff > -0.05)) {
		result = (4.0/3.0 * (double)this->height/(double)this->width);
		/*LOGDBG("diff: %f", aspect_diff);*/
		/*new_cropping = 1;*/
	      } else {
		result = (16.0/9.0 * (double)this->height/(double)this->width);
	      }
	      /*result = (4.0/3.0 * (double)this->height/(double)this->width);*/
	      break;
            }
    /* center cut-out */
    case 6: {
/*#warning center cut-out mode not implemented*/
      break;
    }

  }
#if 0
  if(this->cropping && !new_cropping) {
    LOGDBG("pan&scan CROP OFF");
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_LEFT, 0);
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_TOP, 72);
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_RIGHT, 0);
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_BOTTOM, 72);
    this->cropping = 0;
  }
  if(!this->cropping && new_cropping) {
    LOGDBG("pan&scan CROP ON");
    /*** Should set unscaled osd (or top & bottom will be cropped off) */
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_LEFT, 0);
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_TOP, 0);
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_RIGHT, 0);
    xine_set_param(this->stream, XINE_PARAM_VO_CROP_BOTTOM, 0);
    this->cropping = 1;
  }
#endif

  return result;
}

static void fe_frame_output_cb (void *data,
				int video_width, int video_height, 
				double video_pixel_aspect,
				int *dest_x, int *dest_y, 
				int *dest_width, int *dest_height, 
				double *dest_pixel_aspect,
				int *win_x, int *win_y) 
{
  fe_t *this = (fe_t *)data;

  if (!this)
    return;

  *dest_width  = this->width;
  *dest_height = this->height;
  *dest_x = 0;
  *dest_y = 0;

#if 1
  if(!this->scale_video) {
    if(video_height < this->height) {
      *dest_height = video_height;
      *dest_y = (this->height - video_height) / 2;
    }
    if(video_width < this->width) {
      *dest_width  = video_width;
      *dest_x = (this->width - video_width) / 2;
    }
  }
#endif

  *win_x = this->xpos;
  *win_y = this->ypos;  
  
  *dest_pixel_aspect = this->dest_pixel_aspect(this, video_pixel_aspect,
					       video_width, video_height);

#if 0
  if(this->cropping) {
    *dest_pixel_aspect = *dest_pixel_aspect * (16.0/9.0)/(4.0/3.0);
    *dest_y = *dest_y - 72;
    *dest_height = *dest_height + 144;
  }
#endif

#if 0
  /* video_out cropping works better */
  if(this->overscan) {
    int crop_x = this->overscan * this->width  / 100 / 2;
    int crop_y = this->overscan * this->height / 100 / 2;
    *dest_x -= crop_x;
    *dest_y -= crop_y;
    *dest_width  += crop_x;
    *dest_height += crop_y;
  }
#endif

  if(!this->stream)
    return;

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_VIDEO_RATIO, 
		     (int)(10000.0*video_pixel_aspect * 
			   ((double)video_width)/((double)video_height)));
  if(this->video_width  != video_width ||
     this->video_height != video_height) {
    xine_format_change_data_t framedata = {
      .width  = video_width,
      .height = video_height,
      .aspect = 0, /* TODO */
      .pan_scan = 0,
    };
    const xine_event_t event = {
      .type        = XINE_EVENT_FRAME_FORMAT_CHANGE,
      .stream      = this->stream,
      .data        = &framedata,
      .data_length = sizeof(framedata),
    };
    xine_event_send(this->stream, &event);
    this->video_width = video_width;
    this->video_height = video_height;
#if 0
    /* trigger forced redraw to make cropping changes effective */
    if(this->cropping)
      xine_set_param(this->stream, XINE_PARAM_VO_ZOOM_X, 100);      
#endif
  }

  if(this->aspect_controller) {
    double video_aspect = (video_pixel_aspect * (double)video_width / (double)video_height);
    double aspect_diff = video_aspect - this->video_aspect;
    if ((aspect_diff > 0.05) || (aspect_diff < -0.05)) {
      char cmd[4096];
      if(snprintf(cmd, sizeof(cmd), "%s %d", 
                  this->aspect_controller, (int)(video_aspect * 10000.0)) 
         < (int)sizeof(cmd)) {
        LOGDBG("Aspect ratio changed, executing %s", cmd);
        if(system(cmd) == -1)
	  LOGERR("Executing /bin/sh -c %s failed", cmd);
        this->video_aspect = video_aspect;
      }
    }
  }
}

static void xine_event_cb (void *user_data, const xine_event_t *event) 
{
  fe_t *this = (fe_t *)user_data;

  switch (event->type) {
    /* in local mode: vdr stream / slave stream ; in remote mode: vdr stream only */
    case XINE_EVENT_UI_PLAYBACK_FINISHED:
      LOGDBG("XINE_EVENT_UI_PLAYBACK_FINISHED");
      if(this) {
	if(event->stream == this->stream)
	  this->playback_finished = 1;
      } else {
	LOGMSG("xine_event_cb: NO USER DATA !");
      }
      break;
    default: break;
  }
}

/*
 * fe_xine_init
 *
 * initialize xine engine, initialize audio and video ports, setup stream
 */

#define MONO          0
#define STEREO        1
#define HEADPHONES    2
#define SURROUND21    3
#define SURROUND3     4
#define SURROUND4     5
#define SURROUND41    6
#define SURROUND5     7
#define SURROUND51    8
#define SURROUND6     9
#define SURROUND61    10
#define SURROUND71    11
#define A52_PASSTHRU  12 

#define x_reg_num(x...)  xine_config_register_num(this->xine, x)
#define x_reg_str(x...)  xine_config_register_string(this->xine, x)
#define x_reg_enum(x...) xine_config_register_enum(this->xine, x)
#define x_reg_bool(x...) xine_config_register_bool(this->xine, x)

#define x_upd_num(x...)  this->xine->config->update_num(this->xine->config, x)
#define x_upd_str(x...)  this->xine->config->update_string(this->xine->config, x)

static void configure_audio_out(const fe_t *this, const char *audio_driver, const char *audio_port)
{
  /*
   * alsa 
   */
  if(audio_driver && audio_port && !strcmp("alsa", audio_driver) && strlen(audio_port) > 0) {
    
    /* define possible speaker arrangements */
    /* From xine audio_alsa_out.c ; must be synchronized ! */
    static char *speaker_arrangement[] = 
      {"Mono 1.0", "Stereo 2.0", "Headphones 2.0", "Stereo 2.1",
       "Surround 3.0", "Surround 4.0", "Surround 4.1", 
       "Surround 5.0", "Surround 5.1", "Surround 6.0",
       "Surround 6.1", "Surround 7.1", "Pass Through", NULL};

    x_reg_enum("audio.output.speaker_arrangement",
	       STEREO,
	       speaker_arrangement,
	       _("speaker arrangement"),
	       _("Select how your speakers are arranged, "
		 "this determines which speakers xine uses for sound output. "
		 "The individual values are:\n\n"
		 "Mono 1.0: You have only one speaker.\n"
		 "Stereo 2.0: You have two speakers for left and right channel.\n"
		 "Headphones 2.0: You use headphones.\n"
		 "Stereo 2.1: You have two speakers for left and right channel, and one "
		 "subwoofer for the low frequencies.\n"
		 "Surround 3.0: You have three speakers for left, right and rear channel.\n"
		 "Surround 4.0: You have four speakers for front left and right and rear "
		 "left and right channels.\n"
		 "Surround 4.1: You have four speakers for front left and right and rear "
		 "left and right channels, and one subwoofer for the low frequencies.\n"
		 "Surround 5.0: You have five speakers for front left, center and right and "
		 "rear left and right channels.\n"
		 "Surround 5.1: You have five speakers for front left, center and right and "
		 "rear left and right channels, and one subwoofer for the low frequencies.\n"
		 "Surround 6.0: You have six speakers for front left, center and right and "
		 "rear left, center and right channels.\n"
		 "Surround 6.1: You have six speakers for front left, center and right and "
		 "rear left, center and right channels, and one subwoofer for the low frequencies.\n"
		 "Surround 7.1: You have seven speakers for front left, center and right, "
		 "left and right and rear left and right channels, and one subwoofer for the "
		 "low frequencies.\n"
		 "Pass Through: Your sound system will receive undecoded digital sound from xine. "
		 "You need to connect a digital surround decoder capable of decoding the "
		 "formats you want to play to your sound card's digital output."),
	       10, NULL, NULL);

    x_reg_str("audio.device.alsa_default_device",
	      "default",
	      _("device used for mono output"),
	      _("xine will use this alsa device "
		"to output mono sound.\n"
		"See the alsa documentation "
		"for information on alsa devices."),
	      10, NULL, NULL);
    x_reg_str("audio.device.alsa_front_device",
	      "plug:front:default",
	      _("device used for stereo output"),
	      _("xine will use this alsa device "
		"to output stereo sound.\n"
		"See the alsa documentation "
		"for information on alsa devices."),
	      10, NULL, NULL);
    x_reg_str("audio.device.alsa_surround51_device",
	      "plug:surround51:0",
	      _("device used for 5.1-channel output"),
	      _("xine will use this alsa device to output "
		"5 channel plus LFE (5.1) surround sound.\n"
		"See the alsa documentation for information "
		"on alsa devices."),
	      10,  NULL, NULL);
    x_reg_str("audio.device.alsa_passthrough_device",
	      "iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2",
	      _("device used for 5.1-channel output"),
	      _("xine will use this alsa device to output "
		"undecoded digital surround sound. This can "
		"be used be external surround decoders.\nSee the "
		"alsa documentation for information on alsa "
		"devices."),
	      10, NULL, NULL);

    x_upd_str("audio.device.alsa_front_device",      audio_port);
    x_upd_str("audio.device.alsa_default_device",    audio_port);
    x_upd_str("audio.device.alsa_surround51_device", audio_port);
    if(strstr(audio_port, "iec") ||
       strstr(audio_port, "spdif")) {
      x_upd_str("audio.device.alsa_passthrough_device",	audio_port);
      x_upd_num("audio.output.speaker_arrangement",     A52_PASSTHRU);
    } else {
#if 0
      x_upd_num("audio.output.speaker_arrangement",
		strstr(audio_port, "surround") ? SURROUND51 : STEREO);
#endif
    }
  }


  /* 
   * OSS 
   */
  if(audio_driver && !strcmp("oss", audio_driver) && audio_port) {
    int devnum = -2;
    if(!strcmp("default", audio_port))
      devnum = -1;
    if(!strncmp("/dev/dsp", audio_port, 8) &&
       sscanf(audio_port+8, "%d", &devnum) < 1)
      devnum = -1;
    if(!strncmp("/dev/sound/dsp", audio_port, 14) &&
       sscanf(audio_port+14, "%d", &devnum) < 1)
      devnum = -1;
    if(devnum > -2) {
      x_reg_num("audio.device.oss_device_number", -1,
		_("OSS audio device number, -1 for none"),
		_("The full audio device name is created by concatenating the "
		  "OSS device name and the audio device number.\n"
		  "If you do not need a number because you are happy with "
		  "your system's default audio device, set this to -1.\n"
		  "The range of this value is -1 or 0-15. This setting is "
		  "ignored, when the OSS audio device name is set to \"auto\"."),
		10, NULL, NULL);
      x_upd_num("audio.device.oss_device_num", devnum);
    }
  }
}

static int fe_xine_init(frontend_t *this_gen, const char *audio_driver, 
			const char *audio_port,
			const char *video_driver, 
			int pes_buffers,
			const char *static_post_plugins,
			const char *config_file)
{
  fe_t *this = (fe_t*)this_gen;
  post_plugins_t *posts = NULL;

  if(!this)
    return 0;

  /* check xine-lib version */
  if(!xine_check_version(1, 1, 0)) {
    LOGERR("xine-lib is too old, require at least xine library version 1.1.0\n");
    return FE_ERROR;
  }


  /*
   * init xine engine
   */

  if(this->xine)
    this->fe.xine_exit(this_gen);

  this->stream          = NULL;
  this->video_port      = NULL;
  this->audio_port      = NULL;
  this->input_plugin    = NULL;

  /* create a new xine and load config file */
  this->xine = xine_new();
  if(!this->xine)
    return 0;

  /* Set xine engine logging verbosity */
  this->xine->verbosity = (SysLogLevel>2);

  /*xine_register_log_cb(this->xine, xine_log_cb, this);*/

  free(this->configfile);
  this->configfile = NULL;
  if (config_file)
    this->configfile = strdup(config_file);
  else if(asprintf(&this->configfile, "%s%s", xine_get_homedir(), 
	      "/.xine/config_xineliboutput") < 0)
    return 0;

  LOGDBG("Using xine-lib config file %s", this->configfile);
  make_dirs(this->configfile);
  xine_config_load (this->xine, this->configfile);

  x_reg_num ("engine.buffers.video_num_buffers",
	     500,
	     _("number of video buffers"),
	     _("The number of video buffers "
	       "(each is 8k in size) "
	       "xine uses in its internal queue. "
	       "Higher values "
	       "mean smoother playback for unreliable "
	       "inputs, but "
	       "also increased latency and memory "
	       "consumption."),
	     20, NULL, NULL);
  x_reg_bool("gui.osd_use_unscaled",
	     0,
	     _("Use unscaled OSD"),
	     _("Use unscaled (full screen resolution) OSD if possible"),
	     10, NULL, NULL);

  xine_init (this->xine);

  x_upd_num("video.device.xv_double_buffer", 1);
  x_upd_num("engine.buffers.video_num_buffers", pes_buffers);

  if(this->video_port_name) {
    if(video_driver && !strcmp(video_driver, "fb"))
      x_upd_str("video.device.fb_device", this->video_port_name);
  }

  this->playback_finished = 0;

  /* create video port */
  if(video_driver && !strcmp(video_driver, "none")) 
    this->video_port = xine_open_video_driver(this->xine,
					      video_driver,
					      XINE_VISUAL_TYPE_NONE, 
					      NULL);
  else if(video_driver && !strcmp(video_driver, "dxr3"))
    this->video_port = xine_open_video_driver(this->xine,
					      video_driver,
					      XINE_VISUAL_TYPE_X11, 
					      NULL);
  else if(video_driver && !strcmp(video_driver, "aadxr3"))
    this->video_port = xine_open_video_driver(this->xine,
					      video_driver,
					      XINE_VISUAL_TYPE_AA, 
					      NULL);
  else
    this->video_port = xine_open_video_driver(this->xine,
					      video_driver,
					      this->xine_visual_type,
					      (void *) &(this->vis));
  if(!this->video_port) {
    LOGMSG("fe_xine_init: xine_open_video_driver(\"%s\") failed",
	   video_driver?video_driver:"(NULL)"); 
    this->fe.xine_exit(this_gen);
    return 0;
  }

  intercept_video_driver(this->video_port);
  if (this->frame_draw_cb) {
    vo_driver_t *frameoutput = vo_frameoutput_init(this, this->frame_draw_cb);
    if (! wire_video_driver(this->video_port, frameoutput)) {
      LOGMSG("wire_video_driver() for frame output handler failed");
      frameoutput->dispose(frameoutput);
    }
  }

  this->video_port_none = NULL;

  /* re-configure display size (DirectFB driver changes display mode in init) */
  if(this->update_display_size_cb)
    this->update_display_size_cb(this);

  /* create audio port */

  if(audio_driver && audio_port) 
    configure_audio_out(this, audio_driver, audio_port);

  if(audio_driver && !strcmp(audio_driver, "auto")) {
    this->audio_port = xine_open_audio_driver (this->xine, NULL, NULL);
#if XINE_VERSION_CODE < 10190
  } else if(audio_driver && !strcmp(audio_driver, "none")) {
    this->audio_port = _x_ao_new_port (this->xine, NULL, 1);
    this->audio_port->set_property(this->audio_port, AO_PROP_DISCARD_BUFFERS, 1);
#endif
  } else {
    this->audio_port = xine_open_audio_driver (this->xine, audio_driver, NULL);
  }

  if(!this->audio_port && (audio_driver && !!strcmp(audio_driver, "none"))) {
    LOGMSG("fe_xine_init: xine_open_audio_driver(\"%s%s%s\") failed",
	   audio_driver?audio_driver:"(NULL)", 
	   audio_port?":":"", audio_port?audio_port:""); 
  }

  this->audio_port_none = NULL;

  /* create stream */

  this->stream = xine_stream_new(this->xine, this->audio_port, this->video_port);
  this->slave_stream = NULL;

  if(!this->stream) {
    LOGMSG("fe_xine_init: xine_stream_new failed"); 
    this->fe.xine_exit(this_gen);
    return 0;
  }

  /* event handling */

  this->event_queue = xine_event_new_queue (this->stream);
  xine_event_create_listener_thread (this->event_queue, xine_event_cb, this);

  if(!this->event_queue)
    LOGMSG("fe_xine_init: xine_event_new_queue failed");

  /* misc. config */

  this->pes_buffers = pes_buffers;

  posts = this->postplugins = calloc(1, sizeof(post_plugins_t));
  posts->xine = this->xine;
  posts->audio_port = this->audio_port;
  posts->video_port = this->video_port;
  posts->video_source = posts->audio_source = this->stream;

  /* multithreaded decoding / post processing */

  if(guess_cpu_count() > 1) {
    if(!xine_check_version(1,1,9))
      LOGMSG("FFmpeg multithreaded video decoding is not supported in xine-lib < 1.1.9");
    else
      LOGMSG("Enabling FFmpeg multithreaded video decoding");

    /* try to enable anyway, maybe someone is using patched 1.1.8 ... */
    x_upd_num("video.processing.ffmpeg_thread_count", guess_cpu_count());
#if 0
    LOGMSG("Enabling multithreaded post processing");
    vpplugin_parse_and_store_post(posts, "thread");
#endif
  }

  /* static post plugins from command-line */

  if(static_post_plugins && *static_post_plugins) {
    int i;
    LOGDBG("static post plugins (from command line): %s", static_post_plugins);
    posts->static_post_plugins = strdup(static_post_plugins);
    vpplugin_parse_and_store_post(posts, posts->static_post_plugins);
    applugin_parse_and_store_post(posts, posts->static_post_plugins);

    for(i=0; i<posts->post_audio_elements_num; i++)
      if(posts->post_audio_elements[i])
	posts->post_audio_elements[i]->enable = 2;
    for(i=0; i<posts->post_video_elements_num; i++)
      if(posts->post_video_elements[i])
	posts->post_video_elements[i]->enable = 2;
    posts->post_video_enable = 1;
    posts->post_audio_enable = 1;
  }

  this->video_width = this->video_height = 0;

  return 1;
}

static void fe_shutdown_init(frontend_t *this_gen, const char *cmd, int timeout)
{
  fe_t *this = (fe_t*)this_gen;

  free(this->shutdown_cmd);

  this->shutdown_cmd     = cmd ? strdup(cmd) : NULL;
  this->shutdown_timeout = timeout;

  this->shutdown_time = (timeout <= 0) ? (time_t)-1 : time(NULL) + timeout;
}

/*
 * fe_xine_open
 *
 * open xine stream
 */

static int fe_xine_open(frontend_t *this_gen, const char *mrl)
{
  fe_t *this = (fe_t*)this_gen;
  int result = 0;
  char *url = NULL;

  if(!this)
    return 0;

  this->input_plugin      = NULL;
  this->playback_finished = 1;
  this->terminate_key_pressed = 0;

  if (asprintf(&url, "%s#nocache", mrl ? : MRL_ID "://") < 0)
    return 0;

  result = xine_open(this->stream, url);

  if(!result) {
    LOGMSG("fe_xine_open: xine_open(\"%s\") failed", url);
    free(url);
    return 0;
  }
  free(url);

#if 0
  this->xine->config->update_num(this->xine->config,
				 "video.output.xv_double_buffer",
				 1);
#endif
  x_upd_num("engine.buffers.video_num_buffers", this->pes_buffers);  

  return result;
}

/*
 * post plugin handling
 *
 */

#define POST_AUDIO_VIS  0
#define POST_AUDIO      1
#define POST_VIDEO      2
#define POST_VIDEO_PIP  3


static void init_dummy_ports(fe_t *this, int on)
{
  if(!on) {
    if(this->slave_stream)
      LOGMSG("ERROR: init_dummy_ports(false) called while port is still in use !");
    
    if(this->audio_port_none)
      xine_close_audio_driver(this->xine, this->audio_port_none);
    this->audio_port_none = NULL;
    if(this->video_port_none)
      xine_close_video_driver(this->xine, this->video_port_none);
    this->video_port_none = NULL;
  } else {
    if(! this->audio_port_none)
#if XINE_VERSION_CODE < 10190
      this->audio_port_none = _x_ao_new_port (this->xine, NULL, 1); 
#else
    this->audio_port_none = NULL;/*xine_new_framegrab_audio_port(this->xine);*/
#endif
    if(this->audio_port_none)
      this->audio_port_none->set_property(this->audio_port_none, AO_PROP_DISCARD_BUFFERS, 1);
    /*LOGMSG("initialized dummy audio port %x", this->audio_port_none);*/
#if 0
    if(! this->video_port_none)
      this->video_port_none =
	_x_vo_new_port(this->xine, 
		       _x_load_video_output_plugin(this->xine, "none", 
						   XINE_VISUAL_TYPE_NONE, NULL),
		       1);
    this->video_port_none->set_property(this->video_port_none, VO_PROP_DISCARD_FRAMES, 1);
#endif
  }
}

static void fe_post_unwire(fe_t *this)
{
  if (!this || !this->stream)
    return;

  xine_post_out_t  *vo_source = xine_get_video_source(this->stream);
  xine_post_out_t  *ao_source = xine_get_audio_source(this->stream);

  if(this->slave_stream &&
     this->slave_stream == this->postplugins->audio_source) {
    LOGDBG("unwiring slave stream audio post plugins");
    init_dummy_ports(this, 1);

    if(ao_source && this->audio_port_none)
      (void) xine_post_wire_audio_port(ao_source, this->audio_port_none);

    ao_source = xine_get_audio_source(this->slave_stream);
    if(ao_source && this->audio_port)
      (void) xine_post_wire_audio_port(ao_source, this->audio_port);

  } else {
    LOGDBG("unwiring audio post plugins");
    init_dummy_ports(this, 0);
    if(ao_source && this->audio_port)
      (void) xine_post_wire_audio_port(ao_source, this->audio_port);   
  }

  if(this->slave_stream &&
     this->slave_stream == this->postplugins->video_source) {
    LOGDBG("unwiring slave stream video post plugins");
    /*init_dummy_ports(this, 1);*/
    /*(void) xine_post_wire_video_port(vo_source, this->video_port_none);*/

    vo_source = xine_get_video_source(this->slave_stream);
    if(vo_source && this->video_port)
      (void) xine_post_wire_video_port(vo_source, this->video_port);

  } else {
    LOGDBG("unwiring video post plugins");
    /*init_dummy_ports(this, 0);*/
    if(vo_source && this->video_port)
      (void) xine_post_wire_video_port(vo_source, this->video_port);
  }
}

static void fe_post_rewire(const fe_t *this)
{
  LOGDBG("re-wiring post plugins");
  vpplugin_rewire_posts(this->postplugins);
  applugin_rewire_posts(this->postplugins);
}

static void fe_post_unload(const fe_t *this)
{
  if (this->postplugins) {
    LOGDBG("unloading post plugins");
    vpplugin_unload_post(this->postplugins, NULL);
    applugin_unload_post(this->postplugins, NULL);
  }
}

static void fe_post_close(const fe_t *this, const char *name, int which)
{
  post_plugins_t *posts = this->postplugins;

  if(!this)
    return;

  if(name && !strcmp(name, "AudioVisualization")) {
    name = NULL;
    which = POST_AUDIO_VIS;
  }
  if(name && !strcmp(name, "Pip")) {
    name = NULL;
    which = POST_VIDEO_PIP;
  }

  /* by name */
  if(name) {
    LOGDBG("closing post plugin: %s", name);
    if(applugin_unload_post(posts, name)) {
      /*LOGDBG("  * rewiring audio");*/
      applugin_rewire_posts(posts);
      return;
    }
    if(vpplugin_unload_post(posts, name)) {
      /*LOGDBG("  * rewiring video");*/
      vpplugin_rewire_posts(posts);
      return;
    }
    return;
  }

  /* by type */
  if(which == POST_AUDIO_VIS || which < 0) { /* audio visualization */
    if(posts->post_vis_elements_num && 
       posts->post_vis_elements &&   
       posts->post_vis_elements[0]) {
      LOGDBG("Closing audio visualization post plugins");
      if(applugin_unload_post(posts, posts->post_vis_elements[0]->name)) {
	/*LOGDBG("  * rewiring audio");*/
	applugin_rewire_posts(posts);
      }
    }
  }

  if(which == POST_AUDIO || which < 0) { /* audio effect(s) */
    LOGDBG("Closing audio post plugins");
    if(applugin_disable_post(posts, NULL)) {
      /*LOGDBG("  * rewiring audio");*/
      applugin_rewire_posts(posts);
    }
  }
  if(which == POST_VIDEO || which < 0) { /* video effect(s) */
    LOGDBG("Closing video post plugins");
    if(vpplugin_unload_post(posts, NULL)) {
      /*LOGDBG("  * rewiring video");*/
      vpplugin_rewire_posts(posts);
    }
  }

  if(which == POST_VIDEO_PIP || which < 0) { /* Picture-In-Picture */
    if(posts->post_pip_elements_num && 
       posts->post_pip_elements &&   
       posts->post_pip_elements[0]) {
      LOGDBG("Closing PIP (mosaico) post plugins");
      if(vpplugin_unload_post(posts, "mosaico")) {
	/*LOGDBG("  * rewiring video");*/
	vpplugin_rewire_posts(posts);
      }
    }
  }
}

static int get_opt_val(const char *s, const char *opt)
{
  int val = -1;
  const char *pt = strstr(s, opt);
  if(pt) 
    if(1 == sscanf(pt+strlen(opt)+1, "%d", &val))
      return val;
  return -1;
}

static void fe_post_open(const fe_t *this, const char *name, const char *args)
{
  post_plugins_t *posts = this->postplugins;
  char initstr[1024];
  int found = 0;

  if(!this || !this->xine || !this->stream || !name)
    return;

  /* pip */
  if(!strcmp(name, "Pip")) {
    posts->post_pip_enable = 1;
    name = "mosaico";
    if(!posts->post_pip_elements ||
       !posts->post_vis_elements[0] ||
       !posts->post_vis_elements[0]->enable)
      LOGMSG("enabling picture-in-picture (\"%s:%s\") post plugin", name, args);
  }

  if(args) {
    snprintf(initstr, sizeof(initstr), "%s:%s", name, args);
    initstr[sizeof(initstr)-1] = 0;
  } else
    strn0cpy(initstr, name, sizeof(initstr));

  /* swscale aspect ratio */
  if (!strcmp(name, "swscale")) {
    char *pt = strstr(initstr, "output_aspect=auto");
    if (pt) {
      char tmp[16];
      double r = 0.0;
      pt += 14;
      switch(this->aspect) {
      case 0:
      case 1: /*       */ r = this->display_ratio * (double)this->width / (double)this->height; break;
      case 2: /* 4:3   */ r = 4.0/3.0; break;
      case 3: /* 16:9  */ r = 16.0/9.0; break;
      case 4: /* 16:10 */ r = 16.0/10.0; break;
      default: LOGDBG("%s(%d): unknown aspect: %d", __FUNCTION__, __LINE__, this->aspect);
      }
      /* in finnish locale decimal separator is "," - same as post plugin parameter separator :( */
      sprintf(tmp, "%04d", (int)(r*1000.0));
      strncpy(pt, tmp, 4); 
    }
  }

  LOGDBG("opening post plugin: %s", initstr);

  /* close old audio visualization plugin */
  if(!strcmp(name,"goom") || 
     !strcmp(name,"oscope") || 
     !strcmp(name,"fftscope") || 
     !strcmp(name,"fftgraph") ||
     !strcmp(name,"fooviz")) {

    /* close if changed */
    if(posts->post_vis_elements_num && 
       posts->post_vis_elements &&   
       posts->post_vis_elements[0] &&
       strcmp(name, posts->post_vis_elements[0]->name)) {

      fe_post_close(this, NULL, POST_AUDIO_VIS);
    }

    if(!found && applugin_enable_post(posts, initstr, &found)) {
      posts->post_vis_enable = 1;
      applugin_rewire_posts(posts);

      // goom wants options thru config ...
      if(args && !strcmp(name,"goom")) {
	int val;
	if((val = get_opt_val(initstr, "fps")) > 0)
	  x_upd_num ("effects.goom.fps", val );
	if((val = get_opt_val(initstr, "width")) > 0)
	  x_upd_num ("effects.goom.width", val);  
	if((val = get_opt_val(initstr, "height")) > 0)
	  x_upd_num ("effects.goom.height", val);
	//if((val = get_opt_val(initstr, "csc_method"))>0)
	//  this->xine->config->update_enum (this->xine->config, "effects.goom.csc_method", val);
      }
    }

  } else {

    /* video filters */
    if(strcmp(name, "audiochannel") &&
       strcmp(name, "volnorm") && 
       strcmp(name, "stretch") && 
       strcmp(name, "upmix_mono") && 
       strcmp(name, "upmix") &&
       vpplugin_enable_post(posts, initstr, &found)) {

      posts->post_video_enable = 1;
      vpplugin_rewire_posts(posts);
    }

    /* audio filters */
    if(!found && applugin_enable_post(posts, initstr, &found)) {
      posts->post_audio_enable = 1;
      applugin_rewire_posts(posts);
    }
  }

  if(!found)
    LOGMSG("Can't load post plugin %s", name);
  else
    LOGDBG("Post plugin %s loaded and wired", name);
}

static void input_event_cb(frontend_t *this_gen, const char *keymap, const char *key)
{
  fe_t *this = (fe_t*)this_gen;

  if (this->fe.fe_message_cb) {
    this->fe.fe_message_cb(this->fe.fe_message_h, keymap, key);
  }
}

static int fe_xine_play(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;

  if(!this)
    return 0;

  fe_post_rewire(this);

  this->input_plugin      = NULL;
  this->playback_finished = xine_play(this->stream, 0, 0) ? 0 : 1;

  if(!find_input_plugin(this))
    return -1;

  this->input_plugin->f.xine_input_event = this->fe.fe_message_cb ? input_event_cb : NULL;
  this->input_plugin->f.fe_control = fe_control;
  this->input_plugin->f.fe_handle  = this_gen;

  if(this->playback_finished)
    LOGMSG("Error playing " MRL_ID ":// !");

  char str[128];
  snprintf(str, sizeof(str), "INFO WINDOW %dx%d", this->width, this->height);
  this->fe.send_event(&this->fe, str);

  return !this->playback_finished;
}

static int fe_xine_stop(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;

  if(!this)
    return 0;

  this->input_plugin      = NULL;
  this->playback_finished = 1;

  if (!this->stream)
    return 0;

  xine_stop(this->stream);

  fe_post_unwire(this);

  return 1;
}

static void fe_xine_close(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;

  if(!this)
    return;

  if (this && this->xine) {
    if(this->input_plugin) {
      this->input_plugin->f.xine_input_event = NULL;
      this->input_plugin->f.fe_control       = NULL;
    }

    if (!this->stream)
      return;

    fe_xine_stop(this_gen);

    xine_close(this->stream);
    if(this->postplugins->pip_stream) 
      xine_close(this->postplugins->pip_stream);
  }
}

static void fe_xine_exit(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;

  if (this && this->xine) {

    if(this->input_plugin || !this->playback_finished)
      fe_xine_close(this_gen);
    fe_post_unload(this);

    if(this->configfile) {
      xine_config_save (this->xine, this->configfile);
      free(this->configfile);
      this->configfile = NULL;
    }
    if(this->event_queue)
      xine_event_dispose_queue(this->event_queue);
    this->event_queue = NULL;

    if(this->stream)
      xine_dispose(this->stream);
    this->stream = NULL;

    if (this->postplugins) {
      if (this->postplugins->pip_stream)
        xine_dispose(this->postplugins->pip_stream);
      this->postplugins->pip_stream = NULL;

      free(this->postplugins->static_post_plugins);
    }
    free(this->postplugins);
    this->postplugins = NULL;

    if(this->slave_stream) 
      xine_dispose(this->slave_stream);
    this->slave_stream = NULL;

    if(this->audio_port)
      xine_close_audio_driver(this->xine, this->audio_port);
    this->audio_port = NULL;

    init_dummy_ports(this, 0);

    if(this->video_port)
      xine_close_video_driver(this->xine, this->video_port);
    this->video_port = NULL;

    xine_exit(this->xine);
    this->xine = NULL;
  }
}

static void fe_free(frontend_t *this_gen) 
{
  if (this_gen) {
    fe_t *this = (fe_t*)this_gen;
    this->fe.fe_display_close(this_gen);
    free(this->configfile);
    free(this->shutdown_cmd);
    free(this);
  }
}

static int fe_is_finished(frontend_t *this_gen, int slave_stream)
{
  fe_t *this = (fe_t*)this_gen;

  if (!this)
    return FE_XINE_ERROR;
  if (this->terminate_key_pressed)
    return FE_XINE_EXIT;
  if (this->playback_finished)
    return FE_XINE_ERROR;

  if (slave_stream) {
    if (!this->slave_stream || this->slave_playback_finished)
      return FE_XINE_EXIT;
  }

  /* check inactivity timer */
  if (this->shutdown_timeout > 0) {
    if (this->shutdown_time < time(NULL)) {
      shutdown_system(this->shutdown_cmd, 0);
      return FE_XINE_EXIT;
    }
  }

  return FE_XINE_RUNNING;
}

/************************** hooks to input plugin ****************************/

/*
 * data/control from VDR
 */

static int xine_control(frontend_t *this_gen, const char *cmd)
{
  fe_t *this = (fe_t*)this_gen;

  if(!find_input_plugin(this))
    return -1;

  return this->input_plugin->f.push_input_control(this->input_plugin, cmd);
}

static int xine_osd_command(frontend_t *this_gen, struct osd_command_s *cmd) {
  fe_t *this = (fe_t*)this_gen;

  if(!find_input_plugin(this))
    return -1;

  return this->input_plugin->f.push_input_osd(this->input_plugin, cmd);
}

static int xine_queue_pes_packet(frontend_t *this_gen, int stream, uint64_t pos, const char *data, int len)
{
  fe_t *this = (fe_t*)this_gen;

  if(!find_input_plugin(this))
    return 0/*-1*/;

  return this->input_plugin->f.push_input_write(this->input_plugin, stream, pos, data, len);
}

/*
 * control from frontend to xine/vdr
 */


static int fe_send_input_event(frontend_t *this_gen, const char *map, 
			       const char *key, int repeat, int release)
{
  fe_t *this = (fe_t*)this_gen;

  LOGDBG("Keypress: %s %s %s %s", 
	 map, key, repeat?"Repeat":"", release?"Release":"");

  /* reset inactivity timer */
  if (this->shutdown_timeout > 0)
    this->shutdown_time = time(NULL) + this->shutdown_timeout;

  /* local mode: --> vdr callback */
  if (this->fe.fe_message_cb) {
    this->fe.fe_message_cb(this->fe.fe_message_h, map, key);
    return FE_OK;
  }

  /* remote mode: --> input plugin --> vdr */
  if (find_input_plugin(this)) {
    if (this->input_plugin->f.post_vdr_event) {

      char *msg = NULL;
      if (map) {
        if (asprintf(&msg, "KEY %s %s %s %s\r\n", map, key,
                     repeat?"Repeat":"", release?"Release":"") < 0)
          msg = NULL;
      } else {
        if (asprintf(&msg, "KEY %s\r\n", key) < 0)
          msg = NULL;
      }

      if (msg) {
        int r = this->input_plugin->f.post_vdr_event(this->input_plugin, msg);
        free(msg);
        if (r > 0)
          return FE_OK;
      }
      LOGMSG("fe_send_input_event: message KEY %s lost", key);
      return FE_ERROR;
    }
  }

  LOGMSG("fe_send_input_event: handler not set, event lost !");
  return FE_ERROR;
}


static int fe_send_event(frontend_t *this_gen, const char *data)
{
  fe_t *this = (fe_t*)this_gen;

  if (!data)
    return FE_ERROR;

  if (!strcmp(data, "TOGGLE_FULLSCREEN")) {
    if(this->toggle_fullscreen_cb)
      this->toggle_fullscreen_cb(this, -1);

  } else if (!strncasecmp(data, "FULLSCREEN ", 11)) {
    if(this->toggle_fullscreen_cb)
      this->toggle_fullscreen_cb(this, atoi(data+11) ? 1: 0);

  } else if (!strcmp(data, "QUIT")) {
    this->terminate_key_pressed = 1;

  } else if(!strcmp(data, "TOGGLE_DEINTERLACE")) {
    xine_set_param(this->stream, XINE_PARAM_VO_DEINTERLACE,
		   xine_get_param(this->stream, XINE_PARAM_VO_DEINTERLACE) ? 0 : 1);

  } else if(!strncasecmp(data, "DEINTERLACE ", 12)) {
    xine_set_param(this->stream, XINE_PARAM_VO_DEINTERLACE, atoi(data+12) ? 1 : 0);

  } else if (!strcmp(data, "POWER_OFF")) {
    shutdown_system(this->shutdown_cmd, 1);

  } else {

    LOGDBG("Event: %s", data);

    /* local mode: --> vdr callback */
    if (this->fe.fe_message_cb) {
      this->fe.fe_message_cb(this->fe.fe_message_h, data, NULL);
      return FE_OK;
    }

    /* remote mode: --> input plugin --> vdr */
    if (find_input_plugin(this)) {
      if (this->input_plugin->f.post_vdr_event) {
        char *msg = NULL;
        if (asprintf(&msg, "%s\r\n", data) < 1)
          msg = NULL;
        if (msg) {
          int r = this->input_plugin->f.post_vdr_event(this->input_plugin, msg);
          free(msg);
          return (r > 0) ? FE_OK : FE_ERROR;
        }
        return FE_ERROR;
      }
    }
  }

  return FE_OK;
}

/*
 *  Control messages from input plugin
 */
static void *fe_control(frontend_t *this_gen, const char *cmd)
{
  fe_t *this = (fe_t*)this_gen;
  post_plugins_t *posts;

  /*LOGDBG("fe_control(\"%s\")", cmd);*/

  if(!cmd || !this) {
    LOGMSG("fe_control(0x%lx,0x%lx) : invalid argument", 
	   (unsigned long int)this_gen, (unsigned long int)cmd);
    return NULL;
  }

  posts = this->postplugins;

  if(!posts) {
    LOGMSG("fe_control : this->posts == NULL");
    return NULL;
  }

  if(!strncmp(cmd, "SLAVE CLOSED", 16)) {
    /*LOGMSG("fe_control : slave closed");*/
    if(this->slave_stream)
      fe_control(this_gen, "SLAVE 0x0\r\n");
    init_dummy_ports(this, 0);
    this->video_width = this->video_height = 0;

  } else if(!strncmp(cmd, "SLAVE 0x", 8)) {
    unsigned long pt;
    if(1 == sscanf(cmd+8, "%lx", &pt)) {
      xine_stream_t *slave_stream = (xine_stream_t*)pt;
      if(this->slave_stream != slave_stream) {

	fe_post_unwire(this);

	if(this->slave_stream) {
	  /*xine_post_out_t  *vo_source = xine_get_video_source(posts->slave_stream);*/
	  if(this->slave_stream == this->postplugins->audio_source) {
	    xine_post_out_t  *ao_source = xine_get_audio_source(this->slave_stream);
	    LOGMSG("unwiring slave stream from output");
	    /*(void) xine_post_wire_video_port(vo_source, this->video_port_none);*/
	    (void) xine_post_wire_audio_port(ao_source, this->audio_port_none);
	  }
	}

	this->slave_stream = slave_stream;

	this->postplugins->video_source = this->postplugins->audio_source = 
	  this->slave_stream ?: this->stream;
	if(strstr(cmd, "Video")) /* video only, audio from VDR */
	  this->postplugins->audio_source = this->stream;
	if(strstr(cmd, "Audio")) /* audio only, video from VDR */
	  this->postplugins->video_source = this->stream;

	if(this->slave_stream)
	  fe_post_unwire(this);

	fe_post_rewire(this);
      }
      this->slave_playback_finished = !slave_stream;
      this->video_width = this->video_height = 0;
    }

  } else if(!strncmp(cmd, "ENDOFSTREAM", 11)) {
    if(this->slave_stream)
      this->slave_playback_finished = 1;

  } else if(!strncmp(cmd, "SUBSTREAM ", 10)) {
    unsigned int pid;
    int x, y, w, h;
    if(5 == sscanf(cmd+10, "0x%x %d %d %d %d", &pid, &x, &y, &w, &h)) {
      char mrl[256];
      if(!posts->pip_stream)
	posts->pip_stream = xine_stream_new(this->xine, 
					    this->audio_port, 
					    this->video_port);
      LOGMSG("  PIP %d: %dx%d @ (%d,%d)", pid & 0x0f, w, h, x, y);
      LOGMSG("create pip stream done");
      sprintf(mrl, MRL_ID "+slave://0x%lx#nocache",
	      (unsigned long int)this);
      if(!xine_open(posts->pip_stream, mrl) ||
	 !xine_play(posts->pip_stream, 0, 0)) {
	LOGERR("  pip stream open/play failed");
      } else {
	char params[64];
	sprintf(params, "pip_num=1,x=%d,y=%d,w=%d,h=%d", x,y,w,h);
	fe_post_open(this, "Pip", params);
	return posts->pip_stream;
      }
    }
    fe_post_close(this, NULL, POST_VIDEO_PIP);
    if(posts->pip_stream) {
      xine_close(posts->pip_stream);
      xine_dispose(posts->pip_stream);
      posts->pip_stream = NULL;
    }
    return NULL;

  } else if(!strncmp(cmd, "POST ", 5)) {
    char *name = strdup(cmd+5), *args = name, *pt;

    if(NULL != (pt=strchr(name, '\r')))
      *pt = 0;
    if(NULL != (pt=strchr(name, '\n')))
      *pt = 0;

    while(*args && *args != ' ') /* skip name */
      args++;
    if(*args /*== ' '*/)
      *args++ = 0;

    while(*args && *args == ' ')  /* skip whitespace between name and args */
      args++;

    /*this->stream->xine->port_ticket->acquire(this->stream->xine->port_ticket, 0);*/
    /* - locks local frontend at startup */
    if(!strncmp(args, "On", 2)) {
      args += 2;
      while(*args == ' ') 
	args++;
      /*LOGDBG("  POST: %s On \"%s\"", name, args);*/
      fe_post_open(this, name, args);
    } else if(!strncmp(args, "Off", 3)) {
      /*LOGDBG("  POST: %s Off (name len=%d), name => int = %d", name, strlen(name), atoi(name));*/
      if(strlen(name) == 1)
	fe_post_close(this, NULL, atoi(name));
      else
	fe_post_close(this, name, -1);
    } else {
      LOGMSG("fe_control: POST: unknown command %s", cmd);
    }
    /*this->stream->xine->port_ticket->release(this->stream->xine->port_ticket, 0);*/
    /* - locks local frontend at startup */
    free(name);
    return NULL;

  } else if(!strncmp(cmd, "GRAB ", 5)) {
    int quality, width, height, jpeg, size=0;
    jpeg = !strncmp(cmd+5,"JPEG",4);
    if(3 == sscanf(cmd+5+4, "%d %d %d", &quality, &width, &height)) {
      grab_data_t *result = (grab_data_t*)malloc(sizeof(grab_data_t));   
      result->data = this->fe.grab((frontend_t*)this, &size, 
				   jpeg, quality, width, height);
      if(result->data && (result->size=size)>0)
	return result;
      free(result->data);
      free(result);
    }

  } else if(!strncmp(cmd, "OVERSCAN ", 9)) {
    int overscan;
    if(1 == sscanf(cmd+9, "%d", &overscan)) {
      int crop_x = overscan * this->width  / 100 / 2;
      int crop_y = overscan * this->height / 100 / 2;
      this->overscan = overscan;
      xine_set_param(this->stream, XINE_PARAM_VO_CROP_LEFT,   crop_x);
      xine_set_param(this->stream, XINE_PARAM_VO_CROP_TOP,    crop_y);
      xine_set_param(this->stream, XINE_PARAM_VO_CROP_RIGHT,  crop_x);
      xine_set_param(this->stream, XINE_PARAM_VO_CROP_BOTTOM, crop_y);
      /* trigger forced redraw to make changes effective */
      xine_set_param(this->stream, XINE_PARAM_VO_ZOOM_X, 100);      
    }
  }
  

  return NULL;
}


/*
 * --- RgbToJpeg -------------------------------------------------------------
 *
 *     source: vdr-1.3.42, tools.c
 *     modified to accept YUV data
 *
 *        - move to xine_input_vdr ?
 */

#ifdef HAVE_LIBJPEG

#define JPEGCOMPRESSMEM 500000

typedef struct tJpegCompressData_s {
  int size;
  unsigned char *mem;
} tJpegCompressData;

static void JpegCompressInitDestination(const j_compress_ptr cinfo)
{
  tJpegCompressData *jcd = (tJpegCompressData *)cinfo->client_data;
  if (jcd) {
     cinfo->dest->free_in_buffer = jcd->size = JPEGCOMPRESSMEM;
     cinfo->dest->next_output_byte = jcd->mem = 
       (unsigned char *)malloc(jcd->size);
     }
}

static boolean JpegCompressEmptyOutputBuffer(const j_compress_ptr cinfo)
{
  tJpegCompressData *jcd = (tJpegCompressData *)cinfo->client_data;
  if (jcd) {
     int Used = jcd->size;
     jcd->size += JPEGCOMPRESSMEM;
     jcd->mem = (unsigned char *)realloc(jcd->mem, jcd->size);
     if (jcd->mem) {
        cinfo->dest->next_output_byte = jcd->mem + Used;
        cinfo->dest->free_in_buffer = jcd->size - Used;
        return TRUE;
        }
     }
  return FALSE;
}

static void JpegCompressTermDestination(const j_compress_ptr cinfo)
{
  tJpegCompressData *jcd = (tJpegCompressData *)cinfo->client_data;
  if (jcd) {
     int Used = cinfo->dest->next_output_byte - jcd->mem;
     if (Used < jcd->size) {
        jcd->size = Used;
        jcd->mem = (unsigned char *)realloc(jcd->mem, jcd->size);
        }
     }
}

#ifndef HAVE_XINE_GRAB_VIDEO_FRAME

static vo_frame_t *yuy2_to_yv12_frame(xine_stream_t *stream, vo_frame_t *frame)
{
  /* convert yuy12 frames to yv12 */
  if (frame && frame->format == XINE_IMGFMT_YUY2) {
    stream->xine->port_ticket->acquire(stream->xine->port_ticket, 0);
    vo_frame_t *img = stream->video_out->get_frame (stream->video_out,
                                                    frame->width, frame->height,
                                                    frame->ratio, XINE_IMGFMT_YV12,
                                                    VO_BOTH_FIELDS);
    stream->xine->port_ticket->release(stream->xine->port_ticket, 0);

    if (!img) {
      LOGMSG("yuy2_to_yv12_frame: get_frame failed");
      frame->free(frame);
      return NULL;
    }

    init_yuv_conversion();
    yuy2_to_yv12(frame->base[0], frame->pitches[0],
                 img->base[0], img->pitches[0],
                 img->base[1], img->pitches[1],
                 img->base[2], img->pitches[2],
                 frame->width, frame->height);

    frame->free(frame);
    return img;
  }

  return frame;
}

static char *frame_compress_jpeg(fe_t *this, int *size, int quality, vo_frame_t *frame)
{
  struct jpeg_destination_mgr jdm;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  tJpegCompressData jcd;

  /* convert yuy2 frames to yv12 */
  frame = yuy2_to_yv12_frame(this->stream, frame);

  /* Compress JPEG */

  jdm.init_destination = JpegCompressInitDestination;
  jdm.empty_output_buffer = JpegCompressEmptyOutputBuffer;
  jdm.term_destination = JpegCompressTermDestination;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  cinfo.dest = &jdm;

  cinfo.client_data = &jcd;
  cinfo.image_width = frame->width;
  cinfo.image_height = frame->height;

  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr;

  switch (frame->format) {
    case XINE_IMGFMT_YV12: {
      JSAMPARRAY pp[3];
      JSAMPROW *rpY = (JSAMPROW*)malloc(sizeof(JSAMPROW) * frame->height);
      JSAMPROW *rpU = (JSAMPROW*)malloc(sizeof(JSAMPROW) * frame->height);
      JSAMPROW *rpV = (JSAMPROW*)malloc(sizeof(JSAMPROW) * frame->height);
      int k;

      jpeg_set_defaults(&cinfo);
      jpeg_set_quality(&cinfo, quality, TRUE);
      cinfo.raw_data_in = TRUE;
#if JPEG_LIB_VERSION >= 70
      cinfo.do_fancy_downsampling = FALSE;
#endif

      jpeg_set_colorspace(&cinfo, JCS_YCbCr);
      cinfo.comp_info[0].h_samp_factor =
      cinfo.comp_info[0].v_samp_factor = 2;
      cinfo.comp_info[1].h_samp_factor =
      cinfo.comp_info[1].v_samp_factor =
      cinfo.comp_info[2].h_samp_factor =
      cinfo.comp_info[2].v_samp_factor = 1;
      jpeg_start_compress(&cinfo, TRUE);

      for (k = 0; k < frame->height; k+=2) {
        rpY[k]   = frame->base[0] + k*frame->pitches[0];
        rpY[k+1] = frame->base[0] + (k+1)*frame->pitches[0];
        rpU[k/2] = frame->base[1] + (k/2)*frame->pitches[1];
        rpV[k/2] = frame->base[2] + (k/2)*frame->pitches[2];
      }
      for (k = 0; k < frame->height; k+=2*DCTSIZE) {
        pp[0] = &rpY[k];
        pp[1] = &rpU[k/2];
        pp[2] = &rpV[k/2];
        jpeg_write_raw_data(&cinfo, pp, 2*DCTSIZE);
      }
      free(rpY);
      free(rpU);
      free(rpV);
      break;
    }
#if 0
    case XINE_IMGFMT_RGB: {
      JSAMPROW rp[height];
      int rs, k;

      cinfo.in_color_space = JCS_RGB;

      jpeg_set_defaults(&cinfo);
      jpeg_set_quality(&cinfo, quality, TRUE);
      jpeg_start_compress(&cinfo, TRUE);

      rs = frame->pitches[0];
      for (k = 0; k < height; k++)
        rp[k] = frame->base[0] + k*rs;
      jpeg_write_scanlines(&cinfo, rp, height);
      break;
    }
#endif
    default:
      LOGMSG("fe_grab: grabbing failed (unsupported image format %d)",
             frame->format);
      break;
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  frame->free(frame);

  *size = jcd.size;
  return (char*) jcd.mem;
}
#endif /* HAVE_XINE_GRAB_VIDEO_FRAME */
#endif /* HAVE_LIBJPEG */

#ifndef HAVE_XINE_GRAB_VIDEO_FRAME
static vo_frame_t *yv12_to_yuy2_frame(xine_stream_t *stream, vo_frame_t *frame)
{
  /* convert yv12 frames to yuy2 */
  if (frame && frame->format == XINE_IMGFMT_YV12) {
    stream->xine->port_ticket->acquire(stream->xine->port_ticket, 0);
    vo_frame_t *img = stream->video_out->get_frame (stream->video_out,
                                                    frame->width, frame->height,
                                                    frame->ratio, XINE_IMGFMT_YUY2,
                                                    VO_BOTH_FIELDS);
    stream->xine->port_ticket->release(stream->xine->port_ticket, 0);

    if (!img) {
      LOGMSG("yv12_to_yuy2_frame: get_frame failed");
      frame->free(frame);
      return NULL;
    }

    init_yuv_conversion();
    yv12_to_yuy2(frame->base[0], frame->pitches[0],
                 frame->base[1], frame->pitches[1],
                 frame->base[2], frame->pitches[2],
                 img->base[0],   img->pitches[0],
                 frame->width, frame->height,
                 frame->progressive_frame);

    frame->free(frame);
    return img;
  }

  return frame;
}

#define YCBCR_TO_RGB( y, cb, cr, r, g, b ) \
  do { \
  int _y, _cb, _cr, _r, _g, _b; \
  _y  = ((y) - 16) * 76309; \
  _cb = (cb) - 128; \
  _cr = (cr) - 128; \
  _r = (_y                + _cr * 104597 + 0x8000) >> 16; \
  _g = (_y - _cb *  25675 - _cr *  53279 + 0x8000) >> 16; \
  _b = (_y + _cb * 132201                + 0x8000) >> 16; \
  (r) = (_r < 0) ? 0 : ((_r > 255) ? 255 : _r); \
  (g) = (_g < 0) ? 0 : ((_g > 255) ? 255 : _g); \
  (b) = (_b < 0) ? 0 : ((_b > 255) ? 255 : _b); \
} while (0)

static char *frame_compress_pnm(fe_t *this, int *size, vo_frame_t *frame)
{
  /* ensure yuy2 */
  if (!(frame = yv12_to_yuy2_frame(this->stream, frame)))
    return NULL;

  /* convert to PNM */

  /* allocate memory for result */
  size_t bytes = frame->width * frame->height * 3;
  uint8_t *pnm = malloc(bytes + 64);
  if (!pnm) {
    LOGMSG("fe_grab: malloc failed");
    return NULL;
  }

  /* PNM header */
  sprintf((char*)pnm, "P6\n%d\n%d\n255\n", frame->width, frame->height);
  int hdrlen = strlen((char*)pnm);
  uint8_t *p = pnm + hdrlen;
  uint8_t *s = frame->base[0];
  int x, y;

  *size = bytes + hdrlen;

  /* convert to RGB */
  for (y=0; y < frame->height; y++) {
    for (x=0; x < frame->width; x+=2, s+=4, p+=6) {
      YCBCR_TO_RGB(s[0], s[1], s[3], p[0], p[1], p[2]);
      YCBCR_TO_RGB(s[2], s[1], s[3], p[3], p[4], p[5]);
    }
    s = frame->base[0] + y*frame->pitches[0];
  }

  return (char*)pnm;
}
#endif /* HAVE_XINE_GRAB_VIDEO_FRAME */

#ifdef HAVE_XINE_GRAB_VIDEO_FRAME
static char *fe_compress_grab_frame(fe_t *this, int *size, int jpeg, int quality, int width, int height, xine_grab_video_frame_t *frame)
{
#ifdef HAVE_LIBJPEG
  if (jpeg) {
    /* Compress JPEG */
    struct jpeg_destination_mgr jdm;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    tJpegCompressData jcd;

    jdm.init_destination = JpegCompressInitDestination;
    jdm.empty_output_buffer = JpegCompressEmptyOutputBuffer;
    jdm.term_destination = JpegCompressTermDestination;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    cinfo.dest = &jdm;
    cinfo.client_data = &jcd;
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW rp[height];
    int rs = width * 3;
    int k;
    for (k = 0; k < height; k++)
      rp[k] = frame->img + k * rs;
    jpeg_write_scanlines(&cinfo, rp, height);

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    *size = jcd.size;
    return (char*) jcd.mem;
  }
#endif

  /* convert to PNM */

  /* allocate memory for result */
  size_t bytes = width * height * 3;
  uint8_t *pnm = malloc(bytes + 64);
  if (!pnm) {
    LOGMSG("fe_grab: malloc failed");
    return NULL;
  }

  /* PNM header */
  sprintf((char*)pnm, "P6\n%d\n%d\n255\n", width, height);
  int hdrlen = strlen((char*)pnm);

  /* copy image */
  xine_fast_memcpy(pnm + hdrlen, frame->img, bytes);

  *size = bytes + hdrlen;
  return (char*)pnm;
}
#endif /* HAVE_XINE_GRAB_VIDEO_FRAME */


static char *fe_grab(frontend_t *this_gen, int *size, int jpeg, 
		     int quality, int width, int height)
{
  fe_t *this = (fe_t*)this_gen;

#ifndef HAVE_LIBJPEG
  if(jpeg) {
    LOGMSG("fe_grab: JPEG grab support was not compiled in");
    return 0;
  }
#endif /* HAVE_LIBJPEG */

  if(!find_input_plugin(this))
    return 0;

  LOGDBG("fe_grab: grabbing %s %d %dx%d", 
	 jpeg ? "JPEG" : "PNM", quality, width, height);

  /* validate parameters */
  if ((quality < 0) || (quality > 100))
    quality = 100;
  width  = (MAX(16, MIN(width,  1920)) + 1) & ~1; /* 16...1920, even */
  height = (MAX(16, MIN(height, 1200)) + 1) & ~1; /* 16...1200, even */

  /* get last frame */
  this->stream->xine->port_ticket->acquire(this->stream->xine->port_ticket, 0);

#ifdef HAVE_XINE_GRAB_VIDEO_FRAME
  char *img = NULL;
  xine_grab_video_frame_t *grab_frame = xine_new_grab_video_frame(this->stream);
  if (grab_frame) {
    grab_frame->width = width;
    grab_frame->height = height;
    if (!grab_frame->grab(grab_frame))
      img = fe_compress_grab_frame(this, size, jpeg, quality, width, height, grab_frame);
    grab_frame->dispose(grab_frame);
  }
  this->stream->xine->port_ticket->release(this->stream->xine->port_ticket, 0);
  return img;
#else
  vo_frame_t *frame = this->stream->video_out->get_last_frame (this->stream->video_out);
#if XINE_VERSION_CODE < 10190
  if(frame)
    frame->lock(frame);
#endif
  this->stream->xine->port_ticket->release(this->stream->xine->port_ticket, 0);

  if(!frame) {
    LOGMSG("fe_grab: get_last_frame() failed");
    return NULL;
  }

  /* Scale image */
  if (frame->width != width || frame->height != height) {
    /* #warning TODO - scaling here */
    LOGMSG("fe_grab: scaling not implemented");
  }

  if (!jpeg)
    return frame_compress_pnm(this, size, frame);

#ifdef HAVE_LIBJPEG
  return frame_compress_jpeg(this, size, quality, frame);
#else /* HAVE_LIBJPEG */
  return NULL;
#endif
#endif /* HAVE_XINE_GRAB_VIDEO_FRAME */
}

/*
 * init_fe()
 *
 * initialize function pointers
 */
void init_fe(fe_t *fe)
{
  fe->fe.xine_init        = fe_xine_init;
  fe->fe.xine_open        = fe_xine_open;
  fe->fe.xine_play        = fe_xine_play;
  fe->fe.xine_stop        = fe_xine_stop;
  fe->fe.xine_close       = fe_xine_close;
  fe->fe.xine_exit        = fe_xine_exit;

  fe->fe.shutdown_init    = fe_shutdown_init;

  fe->fe.fe_free          = fe_free;

  fe->fe.xine_is_finished = fe_is_finished;

  fe->fe.xine_osd_command      = xine_osd_command;
  fe->fe.xine_control          = xine_control;
  fe->fe.xine_queue_pes_packet = xine_queue_pes_packet;

  fe->fe.grab             = fe_grab;

  fe->fe.send_event       = fe_send_event;
  fe->fe.send_input_event = fe_send_input_event;

  fe->dest_pixel_aspect    = fe_dest_pixel_aspect;
  fe->frame_output_handler = fe_frame_output_cb;
}

