/*
 * xine_frontend.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_frontend.c,v 1.53.2.3 2009/02/12 13:03:50 phintuka Exp $
 *
 */

#ifndef XINE_VERSION_CODE
#  define XINE_VERSION_CODE (XINE_MAJOR_VERSION*10000 + \
                             XINE_MINOR_VERSION*100 + \
                             XINE_SUB_VERSION)
#endif

#define NEED_x_syslog
#define LOG_MODULENAME "[vdr-fe]    "
#include "logdefs.h"

#include "xine/post.h"

#ifdef FE_STANDALONE
  /* next two symbols are dynamically linked from input plugin */
  int SysLogLevel __attribute__((visibility("default"))) = 2; /* errors and info, no debug */
  int LogToSysLog __attribute__((visibility("default"))) = 0; /* log to syslog instead of console */

  static int verbose_xine_log = 0;
#else
  int LogToSysLog __attribute__((visibility("default"))) = 1; /* dynamically linked from input plugin */
#endif

/* from vdr_input_plugin: */
typedef struct {
  input_plugin_t           input_plugin;
  vdr_input_plugin_funcs_t f;
  /* ... */
} vdr_input_plugin_t;


static inline char *strn0cpy(char *dest, const char *src, int n) 
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

/*
 * detect input plugin 
 */

static int find_input(fe_t *this)
{
  if(!this->input) {
    if(!this->stream || !this->stream->input_plugin ||
       !this->stream->input_plugin->input_class || this->playback_finished) {
      LOGMSG("find_input: stream not initialized or playback finished !");
      usleep(100*1000);
      return 0;
    }
#if XINE_VERSION_CODE < 10190
    if(strcmp(this->stream->input_plugin->input_class->get_identifier(
	      this->stream->input_plugin->input_class),
              "xvdr")) {
#else
    if(strcmp(this->stream->input_plugin->input_class->identifier,
              "xvdr")) {
#endif
      LOGMSG("find_input: current xine input plugin is not xvdr !");
      return 0;
    }
    this->input = this->stream->input_plugin;
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
#ifndef HAVE_XV_FIELD_ORDER
  *dest_y = 0 + this->field_order;
#else
  *dest_y = 0;
#endif

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
  
  *dest_pixel_aspect = fe_dest_pixel_aspect(this, video_pixel_aspect,
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
    xine_event_t event;
    xine_format_change_data_t framedata;
    event.type = XINE_EVENT_FRAME_FORMAT_CHANGE;
    event.stream = this->stream;
    event.data = &framedata;
    event.data_length = sizeof(framedata);
    framedata.width  = video_width;
    framedata.height = video_height;
    framedata.aspect = 0; /* TODO */
    framedata.pan_scan = 0;
    xine_event_send(this->stream, &event);
    this->video_width = video_width;
    this->video_height = video_height;

    /* trigger forced redraw to make cropping changes effective */
    if(this->cropping)
      xine_set_param(this->stream, XINE_PARAM_VO_ZOOM_X, 100);      
  }

  if(this->aspect_controller) {
    double video_aspect = (video_pixel_aspect * (double)video_width / (double)video_height);
    double aspect_diff = video_aspect - this->video_aspect;
    if ((aspect_diff > 0.05) || (aspect_diff < -0.05)) {
      char cmd[4096];
      if(snprintf(cmd, sizeof(cmd), "%s %d", 
                  this->aspect_controller, (int)(video_aspect * 10000.0)) 
         < sizeof(cmd)) {
        LOGDBG("Aspect ratio changed, executing %s", cmd);
        if (system(cmd) == -1)
          LOGERR("%s failed", cmd);
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
      LOGMSG("xine_event_cb: XINE_EVENT_UI_PLAYBACK_FINISHED");
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
      x_upd_num("audio.output.speaker_arrangement",
		strstr(audio_port, "surround") ? SURROUND51 : STEREO);
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
			const char *static_post_plugins)
{
  fe_t *this = (fe_t*)this_gen;
  post_plugins_t *posts = NULL;

  if(!this)
    return 0;

  /*
   * init xine engine
   */

  if(this->xine)
    this->fe.xine_exit(this_gen);
  
  this->stream          = NULL;
  this->video_port      = NULL;
  this->audio_port      = NULL;
  this->input           = NULL;

  /* create a new xine and load config file */
  this->xine = xine_new();
  if(!this->xine)
    return 0;

#ifdef FE_STANDALONE
  this->xine->verbosity = verbose_xine_log;
#else
  this->xine->verbosity = (SysLogLevel>2);
#endif

  /*xine_register_log_cb(this->xine, xine_log_cb, this);*/

  /* TODO: use different config file ? (vdr conf.dir/xine/config_vdr ?) */
  snprintf(this->configfile, sizeof(this->configfile),
	   "%s%s", xine_get_homedir(), 
	  "/.xine/config_xineliboutput");
  this->configfile[sizeof(this->configfile)-1] = 0;
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

#ifdef IS_FBFE
  if(this->fb_dev) {
    if(video_driver && !strcmp(video_driver, "fb"))
      x_upd_str("video.device.fb_device", this->fb_dev);
    else if(video_driver && !strcmp(video_driver, "DirectFB")) { 
      /* DirectFBInit (NULL, NULL); */
      /* DirectFBSetOption ("fbdev", video_port); */
      char *orig = getenv("DFBARGS"), *head = NULL, *tail = NULL, *tmp = NULL;
      if(orig) {
	orig = strdup(orig);
	if(NULL != (head = strstr(orig,"fbdev="))) {
	  tail = strchr(head, ',');
	  if(head == orig)
	    head = NULL;
	  else
	    *head = 0;
	  if (asprintf(&tmp, "%sfbdev=%s%s",
                       head ? orig : "", this->fb_dev, tail ? tail : "") < 0)
            tmp = NULL;
	} else {
	  if (asprintf(&tmp, "fbdev=%s%s%s", this->fb_dev, orig?",":"", orig?orig:""))
            tmp = NULL;
	}
	LOGMSG("replacing environment variable DFBARGS with %s (original was %s)", 
	       tmp, getenv("DFBARGS"));
	free(orig);
      } else {
	if (asprintf(&tmp, "fbdev=%s", this->fb_dev) < 0)
          tmp = NULL;
	LOGMSG("setting environment variable DFBARGS to %s", tmp);
      }
      if (tmp)
        setenv("DFBARGS", tmp, 1);
      free(tmp);
      /*free(orig);*/
    }
  }
#endif

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
    xine_exit(this->xine);
    this->xine = NULL;
    return 0;
  }

  this->video_port_none = NULL;
  
  /* re-configure display size (DirectFB driver changes display mode in init) */
  if(this->update_display_size)
    this->update_display_size(this_gen);

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

    if(this->audio_port)
      xine_close_audio_driver(this->xine, this->audio_port);
    this->audio_port = NULL;
    xine_close_video_driver(this->xine, this->video_port);
    this->video_port = NULL;
    xine_exit(this->xine);
    this->xine = NULL;

    return 0;
  }

  /* event handling */

  this->event_queue = xine_event_new_queue (this->stream);
  xine_event_create_listener_thread (this->event_queue, xine_event_cb, this);

  if(!this->event_queue)
    LOGMSG("fe_xine_init: xine_event_new_queue failed");

  /* misc. config */

  this->pes_buffers = pes_buffers;

  posts = this->postplugins = malloc(sizeof(post_plugins_t));
  memset(posts, 0, sizeof(post_plugins_t));
  posts->xine = this->xine;
  posts->audio_port = this->audio_port;
  posts->video_port = this->video_port;
  posts->video_source = posts->audio_source = this->stream;

  /* multithreaded decoding / post processing */

  if(guess_cpu_count() > 1) {
    int xmajor, xminor, xsub;
    xine_get_version(&xmajor, &xminor, &xsub);
    if(xmajor*10000 + xminor*100 + xsub < 10109)
      LOGMSG("Multithreaded video decoding is not supported in xine-lib %d.%d.%d",
	     xmajor, xminor, xsub);
    else
      LOGMSG("Enabling multithreaded video decoding");

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

  this->input = NULL;
  this->playback_finished = 1;

  if (asprintf(&url, "%s#nocache;demux:mpeg_block", mrl ? : "xvdr://") < 0)
    url = strdup("xvdr://");

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

#if !defined(IS_FBFE) && defined(FE_STANDALONE)
  if(!strncmp(mrl, "xvdr", 4) && strstr(mrl, "//")) {
    char *name = NULL, *end;
    if (asprintf(&name, "VDR - %s", strstr(mrl, "//")+2) >= 0) {
      if(NULL != (end = strstr(name, ":37890"))) *end = 0; /* hide only default port */
      XStoreName(this->display, this->window[0], name);
      XStoreName(this->display, this->window[1], name);
      free(name);
    }
  }
#endif

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
  LOGDBG("unloading post plugins");
  vpplugin_unload_post(this->postplugins, NULL);
  applugin_unload_post(this->postplugins, NULL);
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
      LOGMSG("enabling picture-in-picture (\"%s\") post plugin", initstr);
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

static int fe_xine_play(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;
  vdr_input_plugin_t *input_vdr;

  if(!this)
    return 0;

  fe_post_rewire(this);

  this->input = NULL;
  this->playback_finished = xine_play(this->stream, 0, 0) ? 0 : 1;

  if(!this->input && !find_input(this))
    return -1;

  input_vdr = (vdr_input_plugin_t *)this->input;
  input_vdr->f.xine_input_event = this->keypress;
  input_vdr->f.fe_control = fe_control;
  input_vdr->f.fe_handle  = this_gen;

  if(this->playback_finished)
    LOGMSG("Error playing xvdr:// !");

  return !this->playback_finished;
}

static int fe_xine_stop(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;

  if(!this)
    return 0;

  this->input = NULL;
  this->playback_finished = 1;

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
#ifndef FE_STANDALONE
    if(this->input) {
      vdr_input_plugin_t *input_vdr;
      input_vdr = (vdr_input_plugin_t *)this->input;
      input_vdr->f.xine_input_event = NULL;
    }
#endif

    fe_xine_stop(this_gen);

    fe_post_unload(this);

    xine_close(this->stream);
    if(this->postplugins->pip_stream) 
      xine_close(this->postplugins->pip_stream);
  }
}

static void fe_xine_exit(frontend_t *this_gen) 
{
  fe_t *this = (fe_t*)this_gen;

  if (this && this->xine) {

    if(this->input || !this->playback_finished)
      fe_xine_close(this_gen);
    fe_post_unload(this);

    xine_config_save (this->xine, this->configfile);
    if(this->event_queue)
      xine_event_dispose_queue(this->event_queue);
    this->event_queue = NULL;

    if(this->stream)
      xine_dispose(this->stream);
    this->stream = NULL;

    if(this->postplugins->pip_stream) 
      xine_dispose(this->postplugins->pip_stream);
    this->postplugins->pip_stream = NULL;

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

    if(this->postplugins->static_post_plugins)
      free(this->postplugins->static_post_plugins);
    free(this->postplugins);
    this->postplugins = NULL;

    xine_exit(this->xine);
    this->xine = NULL;
  }
}

static void fe_free(frontend_t *this_gen) 
{
  if (this_gen) {
    fe_t *this = (fe_t*)this_gen;
    this->fe.fe_display_close(this_gen);
    free(this);
  }
}

static int fe_is_finished(frontend_t *this_gen, int slave_stream)
{
  fe_t *this = (fe_t*)this_gen;

  if(!this || this->playback_finished)
    return 1;
  
  if(slave_stream) {
    if(!this->slave_stream || this->slave_playback_finished)
      return 1;
  }
  
  return 0;
}

/************************** hooks to input plugin ****************************/

#ifndef FE_STANDALONE

static int xine_control(frontend_t *this_gen, const char *cmd)
{
  fe_t *this = (fe_t*)this_gen;
  vdr_input_plugin_t *input_vdr;

  if(!this->input && !find_input(this))
    return -1;

  input_vdr = (vdr_input_plugin_t *)this->input;
  return input_vdr->f.push_input_control(this->input, cmd);
}

static int xine_osd_command(frontend_t *this_gen, struct osd_command_s *cmd) {
  fe_t *this = (fe_t*)this_gen;
  vdr_input_plugin_t *input_vdr;

  if(!this->input && !find_input(this))
    return -1;

  input_vdr = (vdr_input_plugin_t *)this->input;
  return input_vdr->f.push_input_osd(this->input, cmd);
}

static int xine_queue_pes_packet(frontend_t *this_gen, const char *data, int len)
{
  fe_t *this = (fe_t*)this_gen;
  vdr_input_plugin_t *input_vdr;

  if(!this->input && !find_input(this))
    return 0/*-1*/;

#if 0
  if(len<6) {
    LOGMSG("xine_queue_pes_packet: len == %d, too short!", len);
    abort();
  }
  /* must contain single pes packet and nothing else */
  if(data[0] || data[1] || (data[2] != 1)) {
    static int counter=0;
    counter++;
    LOGMSG("xine_queue_pes_packet: packet not starting with 00 00 01 \n"
	   "    packet #%d, size=%d : %02x %02x %02x %02x %02x %02x\n",
	   counter, len,
	   data[0], data[1], data[2], data[3], data[4], data[5]);
    abort();
  }
#endif

  input_vdr = (vdr_input_plugin_t *)this->input;
  return input_vdr->f.push_input_write(this->input, data, len);
}

#else /* #ifndef FE_STANDALONE */

static void process_xine_keypress(input_plugin_t *input, 
				  const char *map, const char *key,
				  int repeat, int release)
{
  /* from UI --> input plugin --> vdr */
  LOGDBG("Keypress: %s %s %s %s", 
	 map, key, repeat?"Repeat":"", release?"Release":"");
  if(input) {
    vdr_input_plugin_t *input_vdr = (vdr_input_plugin_t *)input;
    if(input_vdr->f.input_control) {
      input_vdr->f.input_control(input, map, key, repeat, release);
    } else {
      LOGMSG("Keypress --- NO HANDLER SET");
    }
  } else {
    LOGMSG("Keypress --- NO PLUGIN FOUND");
  }
}

#endif /* #ifndef FE_STANDALONE */

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

  } else if(!strncmp(cmd, "SLAVE 0x", 8)) {
    unsigned long pt;
    if(1 == sscanf(cmd, "SLAVE 0x%lx", &pt)) {
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
    }

  } else if(!strncmp(cmd, "ENDOFSTREAM", 11)) {
    if(this->slave_stream)
      this->slave_playback_finished = 1;

  } else if(!strncmp(cmd, "SUBSTREAM ", 10)) {
    unsigned int pid;
    int x, y, w, h;
    if(5 == sscanf(cmd, "SUBSTREAM 0x%x %d %d %d %d", &pid, &x, &y, &w, &h)) {
      char mrl[256];
      if(!posts->pip_stream)
	posts->pip_stream = xine_stream_new(this->xine, 
					    this->audio_port, 
					    this->video_port);
      LOGMSG("  PIP %d: %dx%d @ (%d,%d)", pid & 0x0f, w, h, x, y);
      LOGMSG("create pip stream done");
      sprintf(mrl, "xvdr+slave://0x%lx#nocache;demux:mpeg_block",
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
  } else if(!strncmp(cmd, "VO_ASPECT ", 10)) {
    int vo_aspect_ratio;
    if(1 == sscanf(cmd+10, "%d", &vo_aspect_ratio)) {
      xine_set_param(this->stream, XINE_PARAM_VO_ASPECT_RATIO, vo_aspect_ratio);
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
 *  TODO: remote version: send to ctrl stream
 *        - move to xine_input_vdr ?
 */

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

static char *fe_grab(frontend_t *this_gen, int *size, int jpeg, 
		     int quality, int width, int height)
{
  struct jpeg_destination_mgr jdm;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  tJpegCompressData jcd;

  fe_t *this = (fe_t*)this_gen;
  vo_frame_t *frame, *img;

#ifndef PPM_SUPPORTED
  if(!jpeg) {
    LOGMSG("fe_grab: PPM grab not implemented");
    return 0;
  }
#else
  /* #warning TODO: convert to RGB PPM */
#endif

  if(!this->input && !find_input(this))
    return 0;

  LOGDBG("fe_grab: grabbing %s %d %dx%d", 
	 jpeg ? "JPEG" : "PNM", quality, width, height);

  if (quality < 0)
    quality = 0;
  else if(quality > 100)
    quality = 100;

  this->stream->xine->port_ticket->acquire(this->stream->xine->port_ticket, 0);
  frame = this->stream->video_out->get_last_frame (this->stream->video_out);
  if(frame)
    frame->lock(frame);
  this->stream->xine->port_ticket->release(this->stream->xine->port_ticket, 0);

  if(!frame)
    return NULL;

  // convert yuy2 frames to yv12
  if (frame->format == XINE_IMGFMT_YUY2) {
    this->stream->xine->port_ticket->acquire(this->stream->xine->port_ticket, 0);
    img = this->stream->video_out->get_frame (this->stream->video_out,
                                              frame->width, frame->height,
                                              frame->ratio, XINE_IMGFMT_YV12, 
                                              VO_BOTH_FIELDS);
    this->stream->xine->port_ticket->release(this->stream->xine->port_ticket, 0);

    if(!img) {
      LOGMSG("fe_grab: get_frame failed");
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
    frame = img;
  }

  /* #warning TODO: no scaling implemented */
  if(width != frame->width)
    width = frame->width;
  if(height != frame->height)
    height = frame->height;

  /* Compress JPEG */

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
  cinfo.in_color_space = JCS_YCbCr;

  switch (frame->format) {
    case XINE_IMGFMT_YV12: {
      JSAMPARRAY pp[3];
      JSAMPROW *rpY = (JSAMPROW*)malloc(sizeof(JSAMPROW) * height);
      JSAMPROW *rpU = (JSAMPROW*)malloc(sizeof(JSAMPROW) * height);
      JSAMPROW *rpV = (JSAMPROW*)malloc(sizeof(JSAMPROW) * height);
      int k;

      jpeg_set_defaults(&cinfo);
      jpeg_set_quality(&cinfo, quality, TRUE);
      cinfo.raw_data_in = TRUE;

      jpeg_set_colorspace(&cinfo, JCS_YCbCr);
      cinfo.comp_info[0].h_samp_factor = 
      cinfo.comp_info[0].v_samp_factor = 2;
      cinfo.comp_info[1].h_samp_factor =
      cinfo.comp_info[1].v_samp_factor =
      cinfo.comp_info[2].h_samp_factor =
      cinfo.comp_info[2].v_samp_factor = 1;
      jpeg_start_compress(&cinfo, TRUE);
      
      for (k = 0; k < height; k+=2) {
	rpY[k]   = frame->base[0] + k*frame->pitches[0];
	rpY[k+1] = frame->base[0] + (k+1)*frame->pitches[0];
	rpU[k/2] = frame->base[1] + (k/2)*frame->pitches[1];
	rpV[k/2] = frame->base[2] + (k/2)*frame->pitches[2];
      }
      for (k = 0; k < height; k+=2*DCTSIZE) {
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


#ifdef FE_STANDALONE

/* frontend main() */

#include "xine_frontend_main.c"

#endif /* #ifdef FE_STANDALONE */

