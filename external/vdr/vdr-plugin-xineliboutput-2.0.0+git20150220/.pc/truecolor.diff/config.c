/*
 * config.c: User settings
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: config.c,v 1.118 2013-10-28 15:31:45 rofafor Exp $
 *
 */

#include "features.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <vdr/config.h>
#include <vdr/videodir.h>
#include <vdr/device.h>
#include <vdr/i18n.h>

#include "logdefs.h"
#include "config.h"
#include "xine_frontend.h" // HUD_*

#define STRN0CPY(dst, src) \
  do { \
    strn0cpy(dst, src, sizeof(dst)); \
    if(strlen(src) >= sizeof(dst)) \
      LOGMSG("WARNING: Setting %s truncated to %s !", Name, dst); \
  } while(0)


#define DEFAULT_DEINTERLACE_OPTS "method=Linear,cheap_mode=1,pulldown=0,use_progressive_frame_flag=1"

const int config_t::i_pesBufferSize[ PES_BUFFERS_count+1 ] =  { 
  0, 50, 250, 500, 1000, 2000, 500
};

const char * const config_t::s_bufferSize[ PES_BUFFERS_count+1 ] = { 
  trNOOP("custom"),
  trNOOP("tiny"),
  trNOOP("small"),
  trNOOP("medium"),
  trNOOP("large"),
  trNOOP("huge"),
  NULL
};

const char * const config_t::s_aspects[ ASPECT_count+1 ] = {
  trNOOP("automatic"),
  trNOOP("default"),
  "4:3",
  "16:9",
  "16:10",
  trNOOP("Pan&Scan"),
  trNOOP("CenterCutOut"),
  NULL
};

const char * const config_t::s_vo_aspects[ VO_ASPECT_count+1 ] = {
  trNOOP("automatic"),
  trNOOP("square"),
  "4:3",
  trNOOP("anamorphic"),
  trNOOP("DVB"),
  NULL
};


const char * const config_t::s_deinterlaceMethods[ DEINTERLACE_count+1 ] = {
  "none",
  "bob",
  "weave",
  "greedy",
  "onefield",
  "onefield_xv",
  "linearblend",
  "tvtime",
  NULL
};

const char * const config_t::s_deinterlaceMethodNames[ DEINTERLACE_count+1 ] = {
  trNOOP("off"),
  "Bob",
  "Weave",
  "Greedy",
  "One Field",
  "One Field XV",
  "Linear Blend",
  "TvTime",
  NULL
};

const char * const config_t::s_audioDrivers[ AUDIO_DRIVER_count+1 ] = { 
  "auto", "alsa", "oss", "none", "esd", "jack",
  NULL
};

const char * const config_t::s_audioDriverNames[ AUDIO_DRIVER_count+1 ] = {
  trNOOP("automatic"),
  "Alsa",
  "OSS",
  trNOOP("no audio"),
  "ESD",
  "Jack",
  NULL
};

const char * const config_t::s_videoDriversX11[ X11_DRIVER_count+1 ] =  {
  "auto",
  "xshm",
  "xv",
  "vaapi",
  "vdpau",
  "opengl",
  "xvmc",
  "xxmc",
  "sdl",
  "XDirectFB",
  "vidix",
  "none",
  NULL
};

const char * const config_t::s_videoDriverNamesX11[ X11_DRIVER_count+1 ] =  {
  trNOOP("automatic"),
  "XShm",
  "Xv",
  "VAAPI",
  "VDPAU",
  "OpenGL",
  "XvMC",
  "XvMC+VLD",
  "SDL",
  "XDirectFB",
  "Vidix",
  trNOOP("no video"),
  NULL
};

const char * const config_t::s_videoDriversFB[ FB_DRIVER_count+1 ] = {
  "auto", "fb", "DirectFB", "sdl", "vidixfb", "aadxr3", "none",
  NULL 
};

const char * const config_t::s_videoDriverNamesFB [ FB_DRIVER_count+1 ] = {
  trNOOP("automatic"),
  "Framebuffer",
  "DirectFB",
  "SDL",
  "VidixFB",
  "DXR3",
  trNOOP("no video"),
  NULL
};

const char * const config_t::s_frontends[ FRONTEND_count+1 ] = {
  "sxfe", "fbfe", "none",
  NULL
};

const char * const config_t::s_frontendNames[ FRONTEND_count+1 ] = {
  "X11 (sxfe)",
  "Framebuffer (fbfe)",
  trNOOP("Off"),
  NULL 
};

const char * const config_t::s_frontend_files[ FRONTEND_count+1 ] = {
  "lib" PLUGIN_NAME_I18N "-sxfe.so." XINELIBOUTPUT_VERSION,
  "lib" PLUGIN_NAME_I18N "-fbfe.so." XINELIBOUTPUT_VERSION,
  // example: libxineliboutput-sxfe.so.0.4.0
  "",
  NULL
};

const char * const config_t::s_audioEqNames[ AUDIO_EQ_count+1 ] = {
  "30 Hz", "60 Hz", "125 Hz", "250 Hz", "500 Hz",
  "1 kHz", "2 kHz", "4 kHz", "8 kHz", "16 kHz",
  NULL
};

const char * const config_t::s_audioVisualizations[ AUDIO_VIS_count+1 ] = {
  "none", "goom", "oscope", "fftscope", "fftgraph", "image",
  NULL
};

const char * const config_t::s_audioVisualizationNames[ AUDIO_VIS_count+1 ] = {
  trNOOP("Off"),
  trNOOP("Goom"),
  trNOOP("Oscilloscope"),
  trNOOP("FFT Scope"),
  trNOOP("FFT Graph"),
  trNOOP("Image"),
  NULL
};

/* xine, audio_alsa_out.c */
const char * const config_t::s_speakerArrangements[ SPEAKERS_count+1 ] = {
  trNOOP("Mono 1.0"), trNOOP("Stereo 2.0"), trNOOP("Headphones 2.0"), trNOOP("Stereo 2.1"),
  trNOOP("Surround 3.0"), trNOOP("Surround 4.0"), trNOOP("Surround 4.1"),
  trNOOP("Surround 5.0"), trNOOP("Surround 5.1"), trNOOP("Surround 6.0"),
  trNOOP("Surround 6.1"), trNOOP("Surround 7.1"), trNOOP("Pass Through"),
  NULL
};

const char * const config_t::s_subtitleSizes[ SUBTITLESIZE_count+1 ] = { 
  trNOOP("default"),
  trNOOP("tiny"),
  trNOOP("small"),
  trNOOP("medium"),
  trNOOP("large"),
  trNOOP("very large"),
  trNOOP("huge"),
  NULL
};

const char * const config_t::s_subExts[] = {
  ".sub", ".srt", ".txt", ".ssa", "ass", "smi",
  ".SUB", ".SRT", ".TXT", ".SSA", "ASS", "SMI",
  NULL
};

const char * const config_t::s_osdBlendingMethods[] = {
  trNOOP("Software"),
  trNOOP("Hardware"),
  NULL
};

const char * const config_t::s_osdMixers[] = {
  trNOOP("no"),
  trNOOP("grayscale"),              // item [1]
  trNOOP("transparent"),            // item [2]
  trNOOP("transparent grayscale"),  // item [3] ([1 | 2])
  trNOOP("yes"),
  NULL
};

const char * const config_t::s_osdScalings[] = {
  trNOOP("no"),
  trNOOP("nearest"),              // item [1]
  trNOOP("bilinear"),             // item [2]
  NULL
};

const char * const config_t::s_osdColorDepths[] = {
  trNOOP("automatic"),
  trNOOP("LUT8"),
  trNOOP("TrueColor"),
  NULL
};

const char * const config_t::s_osdSizes[] = {
  trNOOP("automatic"),
  "720x576",
  "1280x720",
  "1920x1080",
  trNOOP("custom"),
  trNOOP("video stream"),
  NULL
};

const char * const config_t::s_decoders_MPEG2[] = {
  trNOOP("automatic"),
  "libmpeg2",
  "FFmpeg",
  NULL
};

const char * const config_t::s_decoders_H264[] = {
  trNOOP("automatic"),
  "FFmpeg",
  "CoreAVC",
  NULL
};

const char * const config_t::s_ff_skip_loop_filters[] = {
  trNOOP("automatic"),
  trNOOP("default"),
  trNOOP("none"),
  trNOOP("nonref"),
  trNOOP("bidir"),
  trNOOP("nonkey"),
  trNOOP("all"),
  NULL
};

const char * const config_t::s_ff_speed_over_accuracy[] = {
  trNOOP("automatic"),
  trNOOP("yes"),
  trNOOP("no"),
};

static const char exts_playlist[][4] = {
  "asx",
  "m3u",
  "pls",
  "ram",
};

static const char exts_audio[][8] = {
  "ac3",
  "asf",
  "au",
  "aud",
  "flac",
  "mpa",
  "mpega",
  "mp2",
  "mp3",
  "m4a",
  "ogg",
  "ogm",
  "ra",
  "spx",
  "wav",
  "wma",
};

static const char exts_video[][8] = {
  "asf",
  "avi",
  "dat",
  "divx",
  "dv",
  "fli",
  "flv",
  "iso", /* maybe dvd */
  "mkv",
  "mov",
  "mpeg",
  "mpg",
  "mpv",
  "mp4",
  "m2v",
  "m2t",
  "m2ts",
  "m4v",
  "mts",
  "pes",
  "rm",
  "ts",
  "vdr",
  "vob",
  "wmv",
  "xvid",
};

static const char exts_image[][8] = {
  "bmp",
  "gif",
  "jpeg",
  "jpg",
  "mng",
  "png",
  "tiff",
};

#define DEF_EXT_IS(TYPE) \
static bool ext_is_ ## TYPE(const char *ext) \
{ \
  for(unsigned int i=0; i<sizeof(exts_ ## TYPE)/sizeof(exts_ ## TYPE[0]); i++) \
    if(!strcasecmp(ext, exts_ ## TYPE[i])) \
      return true; \
  return false; \
} 
DEF_EXT_IS(playlist)
DEF_EXT_IS(audio)
DEF_EXT_IS(video)
DEF_EXT_IS(image)

static const char *get_extension(const char *fname)
{
  if(fname) {
    const char *pos = strrchr(fname, '.');
    if(pos)
      return pos+1;
  }
  return NULL;
}

static char *strcatrealloc(char *dest, const char *src)
{
  if (!src || !*src) 
    return dest;

  size_t l = (dest ? strlen(dest) : 0) + strlen(src) + 1;
  if(dest) {
    dest = (char *)realloc(dest, l);
    strcat(dest, src);
  } else {
    dest = (char*)malloc(l);
    strcpy(dest, src);
  } 
  return dest;
}


bool config_t::IsPlaylistFile(const char *fname)
{
  const char *ext = get_extension(fname);
  return ext && ext_is_playlist(ext);
}

bool config_t::IsAudioFile(const char *fname)
{
  const char *ext = get_extension(fname);
  return ext && (ext_is_audio(ext) || ext_is_playlist(ext));
}

bool config_t::IsVideoFile(const char *fname)
{
  const char *ext = get_extension(fname);
  return ext && (ext_is_video(ext) || ext_is_audio(ext) || ext_is_playlist(ext));
}

bool config_t::IsImageFile(const char *fname)
{
  const char *ext = get_extension(fname);
  return ext && (ext_is_image(ext) || ext_is_playlist(ext));
}

bool config_t::IsDvdImage(const char *fname)
{
  const char *ext = get_extension(fname);
  return (ext && !strcasecmp(ext, "iso")) ? true : false;
}

bool config_t::IsBluRayImage(const char *fname)
{
  if (IsDvdImage(fname)) {
    struct stat st;
    if (!stat(fname, &st)) {
      return st.st_size > 10*1024*1024*1024ll;
    }
  }
  return false;
}

bool config_t::IsDvdFolder(const char *fname)
{
  struct stat st;

  cString folder = cString::sprintf("%s/VIDEO_TS/", fname);
  if (stat(folder, &st) != 0) {
    folder = cString::sprintf("%s/video_ts/", fname);
    if (stat(folder, &st) != 0)
      return false;
  }

  if (stat(cString::sprintf("%s/video_ts.ifo", *folder), &st) == 0)
    return true;

  if (stat(cString::sprintf("%s/VIDEO_TS.IFO", *folder), &st) == 0)
    return true;

  return false;
}

bool config_t::IsBluRayFolder(const char *fname)
{
  struct stat st;

  cString folder = cString::sprintf("%s/BDMV/", fname);
  if (stat(folder, &st) != 0) {
    folder = cString::sprintf("%s/bdmv/", fname);
    if (stat(folder, &st) != 0)
      return false;
  }

  if (stat(cString::sprintf("%s/MovieObject.bdmv", *folder), &st) == 0)
    return true;

  if (stat(cString::sprintf("%s/movieobject.bdmv", *folder), &st) == 0)
    return true;

  if (stat(cString::sprintf("%s/MOVIEOBJECT.BDMV", *folder), &st) == 0)
    return true;

  return false;
}

cString config_t::AutocropOptions(void)
{
  if (!autocrop)
    return NULL;

  return cString::sprintf(
                  "enable_autodetect=%d,autodetect_rate=%d,"
                  "soft_start=%d,soft_start_step=%d,"
                  "stabilize=%d,stabilize_time=%d,"
                  "enable_subs_detect=%d,subs_detect_lifetime=%d,subs_detect_stabilize_time=%d,"
                  "logo_width=%d,overscan_compensate=%d,use_driver_crop=%d,"
                  "use_avards_analysis=%d,bar_tone_tolerance=%d",
			  autocrop_autodetect, autocrop_autodetect_rate,
			  autocrop_soft, autocrop_soft_start_step,
			  autocrop_fixedsize, autocrop_stabilize_time,
			  autocrop_subs, autocrop_subs_detect_lifetime, autocrop_subs_detect_stabilize_time,
			  autocrop_logo_width, autocrop_overscan_compensate, autocrop_use_driver_crop,
			  autocrop_use_avards_analysis, autocrop_bar_tone_tolerance);
}

cString config_t::SwScaleOptions(void)
{
  if (!swscale)
    return NULL;

  return cString::sprintf("output_aspect=%s,output_width=%d,output_height=%d,no_downscaling=%d",
			  swscale_change_aspect ? "auto" : "0.0",
			  swscale_resize ? swscale_width  : 0,
			  swscale_resize ? swscale_height : 0,
			  swscale_downscale ? 0 : 1 );
}

cString config_t::FfmpegPpOptions(void)
{
  if (!ffmpeg_pp)
    return NULL;

  if(*ffmpeg_pp_mode)
    return cString::sprintf("quality=%d,mode=%s", ffmpeg_pp_quality, ffmpeg_pp_mode);

  return cString::sprintf("quality=%d", ffmpeg_pp_quality);
}

cString config_t::UnsharpOptions(void)
{
  if (!unsharp)
    return NULL;

  return cString::sprintf("luma_matrix_width=%d,luma_matrix_height=%d,luma_amount=%1.1f,"
			  "chroma_matrix_width=%d,chroma_matrix_height=%d,chroma_amount=%1.1f",
			  unsharp_luma_matrix_width, unsharp_luma_matrix_height,
			  ((float)unsharp_luma_amount)/10.0,
			  unsharp_chroma_matrix_width, unsharp_chroma_matrix_height,
			  ((float)unsharp_chroma_amount)/10.0);
}

cString config_t::Denoise3dOptions(void)
{
  if (!denoise3d)
    return NULL;

  return cString::sprintf("luma=%1.1f,chroma=%1.1f,time=%1.1f",
			  ((float)denoise3d_luma)/10.0,
			  ((float)denoise3d_chroma)/10.0,
			  ((float)denoise3d_time)/10.0);
}

config_t::config_t() {
  memset(this, 0, sizeof(config_t));

  strn0cpy(local_frontend, s_frontends[FRONTEND_X11],        sizeof(local_frontend));
  strn0cpy(video_driver  , s_videoDriversX11[X11_DRIVER_XV], sizeof(video_driver));
  strn0cpy(video_port    , "0.0", sizeof(video_port));
  strn0cpy(modeline      , "",    sizeof(modeline));

  strn0cpy(audio_driver , s_audioDrivers[AUDIO_DRIVER_ALSA], sizeof(audio_driver));
  strn0cpy(audio_port   , "default", sizeof(audio_port));
  speaker_type = SPEAKERS_STEREO;

  post_plugins = NULL;
  config_file = NULL;

  audio_delay       = 0;
  audio_compression = 0;
  memset(audio_equalizer,0,sizeof(audio_equalizer));
  strn0cpy(audio_visualization, "goom", sizeof(audio_visualization));
  strn0cpy(audio_vis_goom_opts, "fps:25,width:720,height:576", sizeof(audio_vis_goom_opts));
  strn0cpy(audio_vis_image_mrl, "file:/usr/share/xine/visuals/default.avi", sizeof(audio_vis_image_mrl));

  headphone = 0;
  audio_upmix = 0;
  audio_surround = 0;
  sw_volume_control = 0;

  pes_buffers          = i_pesBufferSize[PES_BUFFERS_SMALL_250];
  strn0cpy(deinterlace_method, s_deinterlaceMethods[DEINTERLACE_NONE], sizeof(deinterlace_method));
  strn0cpy(deinterlace_opts, DEFAULT_DEINTERLACE_OPTS, sizeof(deinterlace_opts));
  ffmpeg_pp            = 0;
  ffmpeg_pp_quality    = 3;
  strn0cpy(ffmpeg_pp_mode, "de", sizeof(ffmpeg_pp_mode));
  subtitle_vpos = 0;

  unsharp = 0;
  unsharp_luma_matrix_width = 5;
  unsharp_luma_matrix_height = 5;
  unsharp_luma_amount = 0;
  unsharp_chroma_matrix_width = 3;
  unsharp_chroma_matrix_height = 3;
  unsharp_chroma_amount = 0;
  
  denoise3d = 0;
  denoise3d_luma = 40;
  denoise3d_chroma = 30;
  denoise3d_time = 60;

  display_aspect       = 0;     /* auto */

  hide_main_menu       = 0;
  osd_size             = OSD_SIZE_auto;
  osd_width            = 720;
  osd_height           = 576;
  osd_width_auto       = 0;
  osd_height_auto      = 0;
  osd_color_depth      = OSD_DEPTH_auto;
  osd_mixer            = OSD_MIXER_FULL;
  osd_scaling          = OSD_SCALING_NEAREST;
  osd_spu_scaling      = OSD_SCALING_NEAREST;
  hud_osd              = 0;
  opengl               = 0;

  osd_blending             = OSD_BLENDING_SOFTWARE;
  osd_blending_lowresvideo = OSD_BLENDING_HARDWARE;

  extsub_size = -1;
  dvb_subtitles = 0;

  alpha_correction     = 0;
  alpha_correction_abs = 0;

  fullscreen     = 0;
  modeswitch     = 0;
  width          = 720;
  height         = 576;
  scale_video    = 0;

  autocrop       = 0;
  autocrop_autodetect = 1;
  autocrop_autodetect_rate = 4;
  autocrop_soft  = 1;
  autocrop_soft_start_step  = 4;
  autocrop_fixedsize = 1;
  autocrop_stabilize_time = (5*25);
  autocrop_subs  = 1;
  autocrop_subs_detect_lifetime = (60*25);
  autocrop_subs_detect_stabilize_time = 12;
  autocrop_logo_width = 20;
  autocrop_use_driver_crop = 0;
  autocrop_use_avards_analysis = 0;
  autocrop_overscan_compensate = 0;
  autocrop_bar_tone_tolerance = 0;


  swscale               = 0;    // enable/disable
  swscale_change_aspect = 0;    // change video aspect ratio
  swscale_resize        = 0;    // change video size
  swscale_width         = 720;  //   output video width
  swscale_height        = 576;  //   output video height
  swscale_downscale     = 0;    //   allow downscaling

  remote_mode    = 0;
  listen_port    = LISTEN_PORT;
  remote_keyboard = 1;
  remote_max_clients = MAXCLIENTS;
  remote_usetcp   = 1;
  remote_useudp   = 1;
  remote_usertp   = 1;
  remote_usepipe  = 1;
  remote_usebcast = 1;
  remote_http_files    = 1; /* allow http streaming of media files to xineliboutput clients 
			     * (currently replayed media file from xineliboutput media player) 
			     *  - will be used if client can't access file directly (nfs etc.) */

  strn0cpy(remote_rtp_addr, "224.0.1.9", sizeof(remote_rtp_addr));
  remote_rtp_port = (LISTEN_PORT) & (0xfffe); /* even ports only */
  remote_rtp_ttl = 1;
  remote_rtp_always_on = 0;
  remote_rtp_sap = 1;

  remote_use_rtsp      = 1; // allow generic rtsp for primary device. needs enabled udp or rtp
  remote_use_rtsp_ctrl = 0; // allow rtsp to control primary device (play/pause/seek...)
  remote_use_http      = 1; // allow generic http streaming (primary device output)
  remote_use_http_ctrl = 0; // allow http to control primary device (play/pause/seek...)

  remote_local_if[0] = 0;   // use only this interface - undefined -> any/all
  remote_local_ip[0] = 0;   // bind locally to this IP - undefined -> any/all

  use_x_keyboard = 1;
  window_id      = WINDOW_ID_NONE;

  // video settings
  ibp_trickspeed = 1;
  max_trickspeed = 12;
  overscan       = 0;
  hue          = -1; 
  saturation   = -1; 
  contrast     = -1; 
  brightness   = -1; 
  sharpness    = -1;
  noise_reduction = -1;
  vo_aspect_ratio = 0;

  live_mode_sync = 1;      // Sync SCR to transponder clock in live mode
  scr_tuning     = 0;      // Fine-tune xine egine SCR (to sync video to graphics output)
  scr_hz         = 90000;  // Current SCR speed (Hz), default is 90000

  decoder_mpeg2  = DECODER_MPEG2_auto;
  decoder_h264   = DECODER_H264_auto;
  ff_h264_speed_over_accurancy = FF_H264_SPEED_OVER_ACCURACY_auto;
  ff_h264_skip_loop_filter     = FF_H264_SKIP_LOOPFILTER_auto;

  strn0cpy(media_root_dir,    "/",            sizeof(media_root_dir));
#if defined(APIVERSNUM) && (APIVERSNUM >= 20102)
  const char *VideoDirectory = cVideoDirectory::Name() ? cVideoDirectory::Name() : VIDEODIR;
#endif
  strn0cpy(browse_files_dir,  VideoDirectory, sizeof(browse_files_dir));
  strn0cpy(browse_music_dir,  VideoDirectory, sizeof(browse_music_dir));
  strn0cpy(browse_images_dir, VideoDirectory, sizeof(browse_images_dir));
  show_hidden_files = 0;
  cache_implicit_playlists = 1;
  enable_id3_scanner = 1;
  dvd_arrow_keys_control_playback = 1;
  media_menu_items = ~0;
  media_enable_delete = 0;
  media_enable_resume = 1;

  main_menu_mode = ShowMenu;
  last_hotkey = -1;//kNone;
  force_primary_device = 0;
};

#if 0
static uint8_t g_hidden_options[sizeof(config_t)] = {0};
static uint8_t g_readonly_options[sizeof(config_t)] = {0};
uint8_t *config_t::hidden_options   = &g_hidden_options[0];
uint8_t *config_t::readonly_options = &g_readonly_options[0];
#endif

cString config_t::m_ProcessedArgs;
bool config_t::ProcessArg(const char *Name, const char *Value)
{
  if(SetupParse(Name, Value)) {
    m_ProcessedArgs = cString::sprintf("%s%s ", *m_ProcessedArgs ? *m_ProcessedArgs : " ", Name);
    return true;
  }
  return false;
}

bool config_t::ProcessArgs(int argc, char *argv[])
{
  static const char short_options[] = "fDw:h:l:mr:A:V:d:P:C:pc";

  static const struct option long_options[] = {
      { "fullscreen",   no_argument,       NULL, 'f' },
      { "hud",          optional_argument, NULL, 'D' },
      { "opengl",       no_argument,       NULL, 'O' },
      { "width",        required_argument, NULL, 'w' },
      { "height",       required_argument, NULL, 'h' },
      { "geometry",     required_argument, NULL, 'g' },
      //{ "xkeyboard",    no_argument,       NULL, 'k' },
      //{ "noxkeyboard",  no_argument,       NULL, 'K' },
      { "local",        required_argument, NULL, 'l' },
      //{ "modeline",     required_argument, NULL, 'm' },
      { "remote",       required_argument, NULL, 'r' },
      { "audio",        required_argument, NULL, 'A' },
      { "video",        required_argument, NULL, 'V' },
      { "display",      required_argument, NULL, 'd' },
      { "window",       required_argument, NULL, 'W' },
      { "modeswitch",   no_argument,       NULL, 'm' },
      { "post",         required_argument, NULL, 'P' },
      { "config",       required_argument, NULL, 'C' },
      { "primary",      no_argument,       NULL, 'p' },
      { "exit-on-close",no_argument,       NULL, 'c' },
      { NULL,           no_argument,       NULL,  0  }
    };

  int c;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
    case 'd': ProcessArg("Video.Port", optarg);
              break;
    case 'W': ProcessArg("X11.WindowId", optarg);
              break;
    case 'm': ProcessArg("VideoModeSwitching", "1");
#ifndef HAVE_XRANDR
              LOGMSG("Video mode switching not supported");
#endif
              break;
    case 'f': ProcessArg("Fullscreen", "1");
              break;
    case 'D': ProcessArg("X11.HUDOSD", "1");
              if (optarg && strstr(optarg, "xshape")) {
                ProcessArg("XShapeHUDOSD", "1");
#ifndef HAVE_XSHAPE
                LOGMSG("XShape HUD OSD not supported\n");
#endif
              }
              if (optarg && strstr(optarg, "opengl")) {
                ProcessArg("OpenglHUDOSD", "1");
#ifndef HAVE_OPENGL
                LOGMSG("OpenGL HUD OSD not supported\n");
#endif
              }
#ifndef HAVE_XRENDER
              else
                LOGMSG("HUD OSD not supported\n");
#endif
              break;
    case 'O': ProcessArg("OpenglAlways", "1");
#ifndef HAVE_OPENGL
              LOGMSG("OpenGL not supported\n");
#endif
              break;
    case 'w': //ProcessArg("Fullscreen", "0");
              ProcessArg("X11.WindowWidth", optarg);
              break;
    case 'h': //ProcessArg("Fullscreen", "0");
              ProcessArg("X11.WindowHeight", optarg);
              break;
    case 'g': {
                int _width = width, _height = height, _xpos = 0, _ypos = 0;
                sscanf (optarg, "%dx%d+%d+%d", &_width, &_height, &_xpos, &_ypos);
                ProcessArg("X11.WindowWidth",  *cString::sprintf("%d", _width));
                ProcessArg("X11.WindowHeight", *cString::sprintf("%d", _height));
                ProcessArg("X11.XPos",         *cString::sprintf("%d", _xpos));
                ProcessArg("X11.YPos",         *cString::sprintf("%d", _ypos));
              }
              break;
    //case 'k': ProcessArg("X11.UseKeyboard", "1");
    //          break;
    //case 'K': ProcessArg("X11.UseKeyboard", "0");
    //          break;
    case 'l': ProcessArg("Frontend", optarg);
              break;
    //case 'm': ProcessArg("Modeline", optarg);
    //          break;
    case 'r': if(strcmp(optarg, "none")) {
                if(strchr(optarg, ':')) {
		  char *tmp = strdup(optarg);
		  char *pt = strchr(tmp,':');
		  *pt++ = 0;
		  ProcessArg("Remote.ListenPort", pt);
		  ProcessArg("RemoteMode", listen_port>0 ? "1" : "0");
		  ProcessArg("Remote.LocalIP", tmp);
		  free(tmp);
		  LOGMSG("Listening on address \'%s\' port %d", remote_local_ip, listen_port);
		} else {
		  ProcessArg("Remote.ListenPort", optarg);
		  ProcessArg("RemoteMode", listen_port>0 ? "1" : "0");
		}
              } else
                ProcessArg("RemoteMode", "0");
              break;
    case 'V': ProcessArg("Video.Driver", optarg);
              break;
    case 'A': if(strchr(optarg,':')) {
                char *tmp = strdup(optarg);
		char *pt = strchr(tmp,':');
		*pt = 0;
                ProcessArg("Audio.Driver", tmp);
                ProcessArg("Audio.Port", pt+1);
		free(tmp);
              } else
                ProcessArg("Audio.Driver", optarg);
              break;
    case 'P': if(post_plugins)
                post_plugins = strcatrealloc(post_plugins, ";");
              post_plugins = strcatrealloc(post_plugins, optarg);
              break;
    case 'C': config_file = strdup(optarg);
              break;
    case 'p': ProcessArg("ForcePrimaryDevice", "1");
              break;
    case 'c': exit_on_close = 1;
              break;

    default:  return false;
    }
  }
  return true;
}

bool config_t::SetupParse(const char *Name, const char *Value)
{
  const char *pt;
  if(*m_ProcessedArgs && NULL != (pt=strstr(m_ProcessedArgs+1, Name)) &&
     *(pt-1) == ' ' && *(pt+strlen(Name)) == ' ') {
    LOGDBG("Skipping configuration entry %s=%s (overridden in command line)", Name, Value);
    return true;
  }

       if (!strcasecmp(Name, "Frontend"))           STRN0CPY(local_frontend, Value);
  else if (!strcasecmp(Name, "Modeline"))           STRN0CPY(modeline, Value);
  else if (!strcasecmp(Name, "VideoModeSwitching")) modeswitch = atoi(Value);
  else if (!strcasecmp(Name, "Fullscreen"))         fullscreen = atoi(Value);
  else if (!strcasecmp(Name, "DisplayAspect"))      display_aspect = strstra(Value, s_aspects, 0);
  else if (!strcasecmp(Name, "ForcePrimaryDevice")) force_primary_device = atoi(Value);

  else if (!strcasecmp(Name, "X11.WindowId"))     window_id = (!strcmp(Value, "root")) ? WINDOW_ID_ROOT : atoi(Value);
  else if (!strcasecmp(Name, "X11.WindowWidth"))  width = atoi(Value);
  else if (!strcasecmp(Name, "X11.WindowHeight")) height = atoi(Value);
  else if (!strcasecmp(Name, "X11.XPos"))         xpos = atoi(Value);
  else if (!strcasecmp(Name, "X11.YPos"))         ypos = atoi(Value);
  else if (!strcasecmp(Name, "X11.UseKeyboard"))  use_x_keyboard = atoi(Value);
  else if (!strcasecmp(Name, "X11.HUDOSD"))       hud_osd |= (atoi(Value) ? HUD_COMPOSITE : 0);
  else if (!strcasecmp(Name, "X11.OpenglAlways")) opengl = atoi(Value);
  else if (!strcasecmp(Name, "X11.OpenglHUDOSD")) hud_osd |= (atoi(Value) ? HUD_OPENGL : 0);
  else if (!strcasecmp(Name, "X11.XShapeHUDOSD")) hud_osd |= (atoi(Value) ? HUD_XSHAPE : 0);

  else if (!strcasecmp(Name, "Audio.Driver")) STRN0CPY(audio_driver, Value);
  else if (!strcasecmp(Name, "Audio.Port"))   STRN0CPY(audio_port, Value);
  else if (!strcasecmp(Name, "Audio.Speakers")) speaker_type = strstra(Value, s_speakerArrangements, 
								       SPEAKERS_STEREO);
  else if (!strcasecmp(Name, "Audio.Delay"))  audio_delay = atoi(Value);
  else if (!strcasecmp(Name, "Audio.Compression")) audio_compression = atoi(Value);
  else if (!strcasecmp(Name, "Audio.Visualization.GoomOpts")) STRN0CPY(audio_vis_goom_opts, Value);
  else if (!strcasecmp(Name, "Audio.Visualization.ImageMRL")) STRN0CPY(audio_vis_image_mrl, Value);
  else if (!strcasecmp(Name, "Audio.Visualization")) STRN0CPY(audio_visualization, Value);
  else if (!strcasecmp(Name, "Audio.Surround"))  audio_surround = atoi(Value);
  else if (!strcasecmp(Name, "Audio.Upmix"))     audio_upmix = atoi(Value);
  else if (!strcasecmp(Name, "Audio.Headphone")) headphone = atoi(Value);
  else if (!strcasecmp(Name, "Audio.SoftwareVolumeControl")) sw_volume_control = atoi(Value);

  else if (!strcasecmp(Name, "OSD.HideMainMenu"))   hide_main_menu = atoi(Value);
  else if (!strcasecmp(Name, "OSD.Size"))           osd_size = strstra(Value, s_osdSizes, 0);
  else if (!strcasecmp(Name, "OSD.Width"))          osd_width = atoi(Value);
  else if (!strcasecmp(Name, "OSD.Height"))         osd_height = atoi(Value);
  else if (!strcasecmp(Name, "OSD.ColorDepth"))     osd_color_depth = strstra(Value, s_osdColorDepths, 0);
  else if (!strcasecmp(Name, "OSD.LayersVisible"))  osd_mixer = atoi(Value);
  else if (!strcasecmp(Name, "OSD.Scaling"))        osd_scaling = atoi(Value);
  else if (!strcasecmp(Name, "OSD.ScalingSPU"))     osd_spu_scaling = atoi(Value);
  else if (!strcasecmp(Name, "OSD.Blending"))       osd_blending = atoi(Value);
  else if (!strcasecmp(Name, "OSD.BlendingLowRes")) osd_blending_lowresvideo = atoi(Value);
#if 1
  // < 1.0.1
  else if (!strcasecmp(Name, "OSD.UnscaledAlways")) osd_blending = atoi(Value);
  else if (!strcasecmp(Name, "OSD.UnscaledLowRes")) osd_blending_lowresvideo = atoi(Value);
#endif
  else if (!strcasecmp(Name, "OSD.AlphaCorrection"))    alpha_correction = atoi(Value);
  else if (!strcasecmp(Name, "OSD.AlphaCorrectionAbs")) alpha_correction_abs = atoi(Value);

  else if (!strcasecmp(Name, "OSD.ExtSubSize"))    extsub_size = atoi(Value);
  else if (!strcasecmp(Name, "OSD.DvbSubtitles"))  dvb_subtitles = atoi(Value);

  else if (!strcasecmp(Name, "RemoteMode"))          remote_mode = atoi(Value);
  else if (!strcasecmp(Name, "Remote.ListenPort"))   listen_port = atoi(Value);
  else if (!strcasecmp(Name, "Remote.Keyboard"))     remote_keyboard = atoi(Value);
  else if (!strcasecmp(Name, "Remote.MaxClients"))   remote_max_clients = atoi(Value);
  else if (!strcasecmp(Name, "Remote.UseTcp"))       remote_usetcp = atoi(Value);
  else if (!strcasecmp(Name, "Remote.UseUdp"))       remote_useudp = atoi(Value);
  else if (!strcasecmp(Name, "Remote.UseRtp"))       remote_usertp = atoi(Value);
  else if (!strcasecmp(Name, "Remote.UsePipe"))      remote_usepipe= atoi(Value);
  else if (!strcasecmp(Name, "Remote.UseHttp"))      remote_http_files = atoi(Value);
  else if (!strcasecmp(Name, "Remote.UseBroadcast")) remote_usebcast = atoi(Value);

  else if (!strcasecmp(Name, "Remote.Rtp.Address"))  STRN0CPY(remote_rtp_addr, Value);
  else if (!strcasecmp(Name, "Remote.Rtp.Port"))     remote_rtp_port = (atoi(Value)) & (0xfffe);
  else if (!strcasecmp(Name, "Remote.Rtp.TTL"))      remote_rtp_ttl = atoi(Value);
  else if (!strcasecmp(Name, "Remote.Rtp.AlwaysOn")) remote_rtp_always_on = atoi(Value);
  else if (!strcasecmp(Name, "Remote.Rtp.SapAnnouncements")) remote_rtp_sap = atoi(Value);

  else if (!strcasecmp(Name, "Remote.AllowRtsp"))     remote_use_rtsp = atoi(Value);
  else if (!strcasecmp(Name, "Remote.AllowRtspCtrl")) remote_use_rtsp_ctrl = atoi(Value);
  else if (!strcasecmp(Name, "Remote.AllowHttp"))     remote_use_http = atoi(Value);
  else if (!strcasecmp(Name, "Remote.AllowHttpCtrl")) remote_use_http_ctrl = atoi(Value);

  else if (!strcasecmp(Name, "Remote.Iface"))     STRN0CPY(remote_local_if, Value);
  else if (!strcasecmp(Name, "Remote.LocalIP"))   STRN0CPY(remote_local_ip, Value);

  else if (!strcasecmp(Name, "Decoder.PesBuffers"))  pes_buffers=atoi(Value);

  else if (!strcasecmp(Name, "Video.Driver"))      STRN0CPY(video_driver, Value);
  else if (!strcasecmp(Name, "Video.Port"))        STRN0CPY(video_port, Value);

  else if (!strcasecmp(Name, "Video.Scale"))       scale_video = atoi(Value);
  else if (!strcasecmp(Name, "Video.DeinterlaceOptions")) STRN0CPY(deinterlace_opts, Value);
  else if (!strcasecmp(Name, "Video.Deinterlace")) STRN0CPY(deinterlace_method, Value);

  else if (!strcasecmp(Name, "Video.AutoCrop"))    autocrop = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.AutoDetect"))   autocrop_autodetect = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.AutoDetectRate"))   autocrop_autodetect_rate = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.SoftStart"))    autocrop_soft = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.SoftStartStep"))    autocrop_soft_start_step = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.FixedSize"))    autocrop_fixedsize = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.StabilizeTime"))    autocrop_stabilize_time = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.DetectSubs"))   autocrop_subs = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.SubsDetectLifetime"))   autocrop_subs_detect_lifetime = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.SubsDetectStabilizeTime"))   autocrop_subs_detect_stabilize_time = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.LogoWidth"))   autocrop_logo_width = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.UseDriverCrop"))   autocrop_use_driver_crop = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.UseAvardsAnalysis"))   autocrop_use_avards_analysis = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.OverscanCompensate"))   autocrop_overscan_compensate = atoi(Value);
  else if (!strcasecmp(Name, "Video.AutoCrop.BarToneTolerance"))   autocrop_bar_tone_tolerance = atoi(Value);

  else if (!strcasecmp(Name, "Video.SwScale"))           swscale = atoi(Value);
  else if (!strcasecmp(Name, "Video.SwScale.Aspect"))    swscale_change_aspect = atoi(Value);
  else if (!strcasecmp(Name, "Video.SwScale.Resize"))    swscale_resize = atoi(Value);
  else if (!strcasecmp(Name, "Video.SwScale.Downscale")) swscale_downscale = atoi(Value);
  else if (!strcasecmp(Name, "Video.SwScale.Width"))     swscale_width = atoi(Value);
  else if (!strcasecmp(Name, "Video.SwScale.Height"))    swscale_height = atoi(Value);

  else if (!strcasecmp(Name, "Video.HUE"))         hue = atoi(Value);
  else if (!strcasecmp(Name, "Video.Saturation"))  saturation = atoi(Value);
  else if (!strcasecmp(Name, "Video.Contrast"))    contrast = atoi(Value);
  else if (!strcasecmp(Name, "Video.Brightness"))  brightness = atoi(Value);
  else if (!strcasecmp(Name, "Video.Sharpness"))   sharpness = atoi(Value);
  else if (!strcasecmp(Name, "Video.NoiseReduction")) noise_reduction = atoi(Value);
  else if (!strcasecmp(Name, "Video.Overscan"))    overscan = atoi(Value);
  else if (!strcasecmp(Name, "Video.IBPTrickSpeed"))  ibp_trickspeed = atoi(Value);
  else if (!strcasecmp(Name, "Video.MaxTrickSpeed"))  max_trickspeed = atoi(Value);
  else if (!strcasecmp(Name, "Video.AspectRatio"))    vo_aspect_ratio = atoi(Value);

  else if (!strcasecmp(Name, "Video.Decoder.MPEG2"))  decoder_mpeg2 = strstra(Value, s_decoders_MPEG2, 0);
  else if (!strcasecmp(Name, "Video.Decoder.H264"))   decoder_h264  = strstra(Value, s_decoders_H264,  0);
  else if (!strcasecmp(Name, "Video.Decoder.H264.SpeedOverAccuracy")) ff_h264_speed_over_accurancy = strstra(Value, s_ff_speed_over_accuracy,  0);
  else if (!strcasecmp(Name, "Video.Decoder.H264.SkipLoopFilter"))    ff_h264_skip_loop_filter = strstra(Value, s_ff_skip_loop_filters,  0);

  else if (!strcasecmp(Name, "Post.pp.Enable"))    ffmpeg_pp = atoi(Value);
  else if (!strcasecmp(Name, "Post.pp.Quality"))   ffmpeg_pp_quality = atoi(Value);
  else if (!strcasecmp(Name, "Post.pp.Mode"))      STRN0CPY(ffmpeg_pp_mode, Value);

  else if (!strcasecmp(Name, "Post.unsharp.Enable"))               unsharp                      = atoi(Value);
  else if (!strcasecmp(Name, "Post.unsharp.luma_matrix_width"))    unsharp_luma_matrix_width    = atoi(Value);
  else if (!strcasecmp(Name, "Post.unsharp.luma_matrix_height"))   unsharp_luma_matrix_height   = atoi(Value);
  else if (!strcasecmp(Name, "Post.unsharp.luma_amount"))          unsharp_luma_amount          = atoi(Value);
  else if (!strcasecmp(Name, "Post.unsharp.chroma_matrix_width"))  unsharp_chroma_matrix_width  = atoi(Value);
  else if (!strcasecmp(Name, "Post.unsharp.chroma_matrix_height")) unsharp_chroma_matrix_height = atoi(Value);
  else if (!strcasecmp(Name, "Post.unsharp.chroma_amount"))        unsharp_chroma_amount        = atoi(Value);

  else if (!strcasecmp(Name, "Post.denoise3d.Enable"))  denoise3d        = atoi(Value);
  else if (!strcasecmp(Name, "Post.denoise3d.luma"))    denoise3d_luma   = atoi(Value);
  else if (!strcasecmp(Name, "Post.denoise3d.chroma"))  denoise3d_chroma = atoi(Value);
  else if (!strcasecmp(Name, "Post.denoise3d.time"))    denoise3d_time   = atoi(Value);

  else if (!strcasecmp(Name, "Media.RootDir"))           STRN0CPY(media_root_dir, Value);
  else if (!strcasecmp(Name, "Media.BrowseFilesDir"))    STRN0CPY(browse_files_dir, Value);
  else if (!strcasecmp(Name, "Media.BrowseMusicDir"))    STRN0CPY(browse_music_dir, Value);
  else if (!strcasecmp(Name, "Media.BrowseImagesDir"))   STRN0CPY(browse_images_dir, Value);
  else if (!strcasecmp(Name, "Media.ShowHiddenFiles"))   show_hidden_files = atoi(Value);
  else if (!strcasecmp(Name, "Media.CacheImplicitPlaylists")) cache_implicit_playlists = atoi(Value);
  else if (!strcasecmp(Name, "Media.EnableID3Scanner"))  enable_id3_scanner = atoi(Value);
  else if (!strcasecmp(Name, "Media.DVD.ArrowKeysControlPlayback")) dvd_arrow_keys_control_playback = atoi(Value);
  else if (!strcasecmp(Name, "Media.MenuItems"))         media_menu_items = atoi(Value);
  else if (!strcasecmp(Name, "Media.EnableDelete"))      media_enable_delete = atoi(Value);
  else if (!strcasecmp(Name, "Media.EnableResume"))      media_enable_resume = atoi(Value);

  else if (!strcasecmp(Name, "Playlist.Tracknumber")) playlist_tracknumber = atoi(Value);
  else if (!strcasecmp(Name, "Playlist.Artist"))      playlist_artist = atoi(Value);
  else if (!strcasecmp(Name, "Playlist.Album"))       playlist_album = atoi(Value);

  else if (!strcasecmp(Name, "Advanced.LiveModeSync")) live_mode_sync = atoi(Value);
  else if (!strcasecmp(Name, "Advanced.AdjustSCR"))    scr_tuning = atoi(Value);
  else if (!strcasecmp(Name, "Advanced.SCRSpeed"))     scr_hz = atoi(Value);

  else if (!strcasecmp(Name, "Audio.Equalizer")) 
    sscanf(Value,"%d %d %d %d %d %d %d %d %d %d",
	   audio_equalizer  ,audio_equalizer+1,
	   audio_equalizer+2,audio_equalizer+3,
	   audio_equalizer+4,audio_equalizer+5,
	   audio_equalizer+6,audio_equalizer+7,
	   audio_equalizer+8,audio_equalizer+9);

  else return false;

  return true;
}

/* Global instance */
config_t xc;


