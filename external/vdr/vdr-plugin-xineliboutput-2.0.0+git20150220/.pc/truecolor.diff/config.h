/*
 * config.h: Global configuration and user settings
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: config.h,v 1.91 2013-10-28 09:14:47 phintuka Exp $
 *
 */

#ifndef _XINELIB_CONFIG_H_
#define _XINELIB_CONFIG_H_

#include "features.h"

#include <string.h>
#include <stdint.h>

#include <vdr/tools.h>

class cOsdObject;

// Max number of remote clients
#define MAXCLIENTS     10

// Decoder buffer size
#define PES_BUFFERS_CUSTOM      0
#define PES_BUFFERS_TINY_50     1
#define PES_BUFFERS_SMALL_250   2
#define PES_BUFFERS_MEDIUM_500  3
#define PES_BUFFERS_LARGE_1000  4
#define PES_BUFFERS_HUGE_2000   5
#define PES_BUFFERS_count       6

// Output window aspect ratio
#define ASPECT_AUTO     0
#define ASPECT_DEFAULT  1
#define ASPECT_4_3      2
#define ASPECT_16_9     3
#define ASPECT_16_10    4
#define ASPECT_PAN_SCAN 5
#define ASPECT_CENTER_CUT_OUT 6
#define ASPECT_count          7

// VIDEO OUTPUT ASPECT RATIO
#define VO_ASPECT_AUTO                0
#define VO_ASPECT_SQUARE              1 /* 1:1    */
#define VO_ASPECT_4_3                 2 /* 4:3    */
#define VO_ASPECT_ANAMORPHIC          3 /* 16:9   */
#define VO_ASPECT_DVB                 4 /* 2.11:1 */
#define VO_ASPECT_count               5

// De-interlace method
#define DEINTERLACE_NONE         0
#define DEINTERLACE_BOB          1
#define DEINTERLACE_WEAVE        2
#define DEINTERLACE_GREEDY       3
#define DEINTERLACE_ONEFIELD     4
#define DEINTERLACE_ONEFIELD_XV  5
#define DEINTERLACE_LINEARLEND   6
#define DEINTERLACE_TVTIME       7
#define DEINTERLACE_count        8

// Audio driver
#define AUDIO_DRIVER_AUTO        0
#define AUDIO_DRIVER_ALSA        1
#define AUDIO_DRIVER_OSS         2
#define AUDIO_DRIVER_NONE        3
#define AUDIO_DRIVER_ESD         4
#define AUDIO_DRIVER_JACK        5
#define AUDIO_DRIVER_count       6

// Video driver
#define X11_DRIVER_AUTO          0
#define X11_DRIVER_XSHM          1
#define X11_DRIVER_XV            2
#define X11_DRIVER_VAAPI         3
#define X11_DRIVER_VDPAU         4
#define X11_DRIVER_OPENGL        5
#define X11_DRIVER_XVMC          6
#define X11_DRIVER_XXMC          7
#define X11_DRIVER_SDL           8
#define X11_DRIVER_DIRECTFB      9
#define X11_DRIVER_VIDIX         10
#define X11_DRIVER_NONE          11
#define X11_DRIVER_count         12

#define FB_DRIVER_AUTO           0
#define FB_DRIVER_FB             1
#define FB_DRIVER_DIRECTFB       2
#define FB_DRIVER_SDL            3
#define FB_DRIVER_VIDIXFB        4
#define FB_DRIVER_DXR3           5
#define FB_DRIVER_NONE           6
#define FB_DRIVER_count          7

// Local frontend
#define FRONTEND_X11             0
#define FRONTEND_FB              1
#define FRONTEND_NONE            2
#define FRONTEND_count           3
#define DEFAULT_FRONTEND         "sxfe"

#define LISTEN_PORT       37890
#define LISTEN_PORT_S    "37890"
#define DISCOVERY_PORT    37890

#define AUDIO_EQ_30HZ    0
#define AUDIO_EQ_60HZ    1
#define AUDIO_EQ_125HZ   2
#define AUDIO_EQ_250HZ   3
#define AUDIO_EQ_500HZ   4
#define AUDIO_EQ_1000HZ  5
#define AUDIO_EQ_2000HZ  6
#define AUDIO_EQ_4000HZ  7 
#define AUDIO_EQ_8000HZ  8
#define AUDIO_EQ_16000HZ 9
#define AUDIO_EQ_count   10

#define AUDIO_VIS_NONE   0
#define AUDIO_VIS_GOOM   1
#define AUDIO_VIS_IMAGE  5
#define AUDIO_VIS_count  6

/* speaker arrangements: xine, audio_out_alsa.c */
#define SPEAKERS_MONO          0
#define SPEAKERS_STEREO        1
#define SPEAKERS_HEADPHONES    2
#define SPEAKERS_SURROUND21    3
#define SPEAKERS_SURROUND3     4 
#define SPEAKERS_SURROUND4     5
#define SPEAKERS_SURROUND41    6
#define SPEAKERS_SURROUND5     7
#define SPEAKERS_SURROUND51    8
#define SPEAKERS_SURROUND6     9
#define SPEAKERS_SURROUND61    10
#define SPEAKERS_SURROUND71    11
#define SPEAKERS_A52_PASSTHRU  12 
#define SPEAKERS_count         13 

#define SUBTITLESIZE_count     7

// OSD blending methods
#define OSD_BLENDING_SOFTWARE   0  // xine-lib "normal" osd
#define OSD_BLENDING_HARDWARE   1  // xine-lib "unscaled osd"
#define OSD_BLENDING_count      2

// OSD depth
#define OSD_DEPTH_auto          0
#define OSD_DEPTH_LUT8          1
#define OSD_DEPTH_TRUECOLOR     2
#define OSD_DEPTH_count         3

// OSD layers mixing
#define OSD_MIXER_NONE          0
#define OSD_MIXER_GRAY          1
#define OSD_MIXER_ALPHA         2
#define OSD_MIXER_GRAYALPHA     3  // OSD_MIXER_GRAY | OSD_MIXER_ALPHA
#define OSD_MIXER_FULL          4
#define OSD_MIXER_count         5

// OSD scaling modes
#define OSD_SCALING_NONE        0
#define OSD_SCALING_NEAREST     1
#define OSD_SCALING_BILINEAR    2
#define OSD_SCALING_count       3

// OSD size
#define OSD_SIZE_auto           0  // frontend display resolution
#define OSD_SIZE_720x576        1
#define OSD_SIZE_1280x720       2
#define OSD_SIZE_1920x1080      3
#define OSD_SIZE_custom         4
#define OSD_SIZE_stream         5
#define OSD_SIZE_count          6

// Media player menu (bitmask)
#define MEDIA_MENU_FILES        (1<<0)
#define MEDIA_MENU_MUSIC        (1<<1)
#define MEDIA_MENU_IMAGES       (1<<2)
#define MEDIA_MENU_DVD          (1<<3)
#define MEDIA_MENU_CD           (1<<4)
#define MEDIA_MENU_BLURAY       (1<<5)
#define MEDIA_MENU_VIDEO_SETUP  (1<<6)
#define MEDIA_MENU_AUDIO_SETUP  (1<<7)

// Video decoder
#define DECODER_MPEG2_auto       0 /* use value from frontend config_xineliboutput */
#define DECODER_MPEG2_LIBMPEG2   1
#define DECODER_MPEG2_FFMPEG     2
#define DECODER_MPEG2_count      3

#define DECODER_H264_auto        0 /* use value from frontend config_xineliboutput */
#define DECODER_H264_FFMPEG      1
#define DECODER_H264_COREAVC     2
#define DECODER_H264_count       3

#define FF_H264_SKIP_LOOPFILTER_auto    0 /* use value from frontend config_xineliboutput */
#define FF_H264_SKIP_LOOPFILTER_DEFAULT 1
#define FF_H264_SKIP_LOOPFILTER_NONE    2
#define FF_H264_SKIP_LOOPFILTER_NONREF  3
#define FF_H264_SKIP_LOOPFILTER_BIDIR   4
#define FF_H264_SKIP_LOOPFILTER_NONKEY  5
#define FF_H264_SKIP_LOOPFILTER_ALL     6
#define FF_H264_SKIP_LOOPFILTER_count   7

#define FF_H264_SPEED_OVER_ACCURACY_auto  0  /* use value from frontend config_xineliboutput */
#define FF_H264_SPEED_OVER_ACCURACY_no    1
#define FF_H264_SPEED_OVER_ACCURACY_yes   2
#define FF_H264_SPEED_OVER_ACCURACY_count 3

#define HIDDEN_OPTION(opt) \
  (xc.IsOptionHidden(xc.opt))
#define READONLY_OPTION(opt) \
  (xc.IsOptionReadOnly(xc.opt))

#define DEFAULT_POLL_SIZE     16

typedef enum {
  ShowMenu   = 0,
  ShowEq     = 1,
  ShowFiles  = 2,
  ShowMusic  = 3,
  ShowImages = 4,
  CloseOsd   = 5
} eMainMenuMode;

class config_t {
  public:
    static const char * const s_bufferSize             [PES_BUFFERS_count   + 1];
    static const int          i_pesBufferSize          [PES_BUFFERS_count   + 1];
    static const char * const s_aspects                [ASPECT_count        + 1];
    static const char * const s_vo_aspects             [VO_ASPECT_count     + 1];
    static const char * const s_deinterlaceMethods     [DEINTERLACE_count   + 1];
    static const char * const s_deinterlaceMethodNames [DEINTERLACE_count   + 1];
    static const char * const s_audioDriverNames       [AUDIO_DRIVER_count  + 1];
    static const char * const s_audioDrivers           [AUDIO_DRIVER_count  + 1];
    static const char * const s_videoDriverNamesX11    [X11_DRIVER_count    + 1];
    static const char * const s_videoDriversX11        [X11_DRIVER_count    + 1];
    static const char * const s_videoDriverNamesFB     [FB_DRIVER_count     + 1];
    static const char * const s_videoDriversFB         [FB_DRIVER_count     + 1];
    static const char * const s_frontendNames          [FRONTEND_count      + 1];
    static const char * const s_frontends              [FRONTEND_count      + 1];
    static const char * const s_frontend_files         [FRONTEND_count      + 1];
    static const char * const s_audioEqNames           [AUDIO_EQ_count      + 1];
    static const char * const s_audioVisualizations    [AUDIO_VIS_count     + 1];
    static const char * const s_audioVisualizationNames[AUDIO_VIS_count     + 1];
    static const char * const s_speakerArrangements    [SPEAKERS_count      + 1];
    static const char * const s_subtitleSizes          [SUBTITLESIZE_count  + 1];
    static const char * const s_osdBlendingMethods     [OSD_BLENDING_count  + 1];
    static const char * const s_osdMixers              [OSD_MIXER_count     + 1];
    static const char * const s_osdScalings            [OSD_SCALING_count   + 1];
    static const char * const s_osdSizes               [OSD_SIZE_count      + 1];
    static const char * const s_osdColorDepths         [OSD_DEPTH_count     + 1];
    static const char * const s_decoders_MPEG2         [DECODER_MPEG2_count + 1];
    static const char * const s_decoders_H264          [DECODER_H264_count  + 1];
    static const char * const s_ff_skip_loop_filters   [FF_H264_SKIP_LOOPFILTER_count + 1];
    static const char * const s_ff_speed_over_accuracy [FF_H264_SPEED_OVER_ACCURACY_count + 1];

    static const char * const s_subExts[];

  public:

    // Force xineliboutput to be the primary device
    int  force_primary_device;

    // OSD state
    eMainMenuMode  main_menu_mode;      // used internally to open right sub-menu
    cOsdObject    *pending_menu_action; // used to replace current OSD with another type of OSD object
    time_t         last_hotkey_time;
    int/*eKeys*/   last_hotkey;

    // local frontend settings
    char local_frontend[64];
    char video_driver[32];
    char video_port[32];     // X11: DISPLAY=...
    char audio_driver[32];
    char audio_port[64];
    char *post_plugins;      // static post plugins from command line options
    char *config_file;       // config file from command line options
    int  pes_buffers;

    char modeline[64];
    int  fullscreen;
    int  modeswitch;
    int  width;
    int  height;
    int  xpos;
    int  ypos;
    int  display_aspect;
    int  scale_video;
    int  exit_on_close;    // Terminate VDR when local frontend is closed
    int  use_x_keyboard;   // Use X11 keyboard to control VDR (console kbd is handled by VDR)
    int  window_id;        // use existing X11 window
    int  hud_osd;          // head up display OSD
    int  opengl;           // use opengl acceleration for video and HUD OSD

    // Audio settings
    int  speaker_type;
    int  audio_delay;        // in ms
    int  audio_compression;  // 100%(=off)...500%
    int  audio_equalizer[AUDIO_EQ_count];
    int  audio_surround;    // downmix multichannel audio to stereo surround
    int  headphone;         // mix audio for headphones
    int  audio_upmix;       // upmix stereo to 5.1
    int  sw_volume_control; // software (xine-lib) or hardware (alsa) volume control and muting
    // Audio visualization
    char audio_visualization[64];
    char audio_vis_goom_opts[256];
    char audio_vis_image_mrl[4096];

    // Video settings
    int  ibp_trickspeed;
    int  max_trickspeed;
    int  overscan;            // %
    int  hue;                 // 0...0xffff, -1 == off
    int  saturation;          // 0...0xffff, -1 == off
    int  contrast;            // 0...0xffff, -1 == off
    int  brightness;          // 0...0xffff, -1 == off
    int  sharpness;           // 0...0xffff, -1 == off
    int  noise_reduction;     // 0...0xffff, -1 == off
    int  vo_aspect_ratio;

    // OSD settings
    int  hide_main_menu;
    int  osd_size;
    int  osd_width;
    int  osd_height;
    int  osd_width_auto;
    int  osd_height_auto;
    int  osd_color_depth;
    int  osd_mixer;                // show multiple OSD layers
    int  osd_scaling;              // OSD scaling mode: off, nearest, bilinear
    int  osd_spu_scaling;          // SPU OSD scaling mode: off, nearest, bilinear
    int  osd_blending;             // OSD blending method
    int  osd_blending_lowresvideo; // Use hardware blending for low-resolution video
    int  alpha_correction;
    int  alpha_correction_abs;
    int  extsub_size;              // size of separate subtitles ( -1 = xine default ; 0...6 = { tiny  small  normal  large  very large  huge }
    int  dvb_subtitles;            // send DVB subtitles in data stream (decode+display using xine-lib or external media player)

    // Media player
    char media_root_dir[4096];     // restrict file browser
    char browse_files_dir[4096];
    char browse_music_dir[4096];
    char browse_images_dir[4096];
    int  show_hidden_files;
    int  cache_implicit_playlists; // used in playlist.c
    int  enable_id3_scanner;       // used in playlist.c
    int  subtitle_vpos;            // used in media player. Not saved !
    int  playlist_tracknumber;
    int  playlist_artist;
    int  playlist_album;
    int  dvd_arrow_keys_control_playback;
    uint media_menu_items;         // enabled items in media player menu (bitmask)
    int  media_enable_delete;      // enable Delete in file browser
    int  media_enable_resume;

    // deinterlacing post plugin
    char deinterlace_method[32];
    char deinterlace_opts[256];

    // ffmpeg post processing
    int  ffmpeg_pp;           // enable / disable
    int  ffmpeg_pp_quality;   // 0...6
    char ffmpeg_pp_mode[256];

    // automatic 4:3 letterbox -> 16:9 cropping post plugin
    int  autocrop;            // enable / disable
    int  autocrop_autodetect;
    int  autocrop_autodetect_rate;
    int  autocrop_soft;
    int  autocrop_soft_start_step;
    int  autocrop_fixedsize;
    int  autocrop_stabilize_time;
    int  autocrop_subs;
    int  autocrop_subs_detect_lifetime;
    int  autocrop_subs_detect_stabilize_time;
    int  autocrop_logo_width;
    int  autocrop_use_driver_crop;
    int  autocrop_use_avards_analysis;
    int  autocrop_overscan_compensate;
    int  autocrop_bar_tone_tolerance;

    // (video) software scaling
    int  swscale;               // enable/disable
    int  swscale_change_aspect; // change video aspect ratio
    int  swscale_resize;        // change video size
    int  swscale_width;         //   output video width
    int  swscale_height;        //   output video height
    int  swscale_downscale;     //   allow downscaling

    // sharpen / soften post plugin
    int  unsharp;                      // enable / disable
    int  unsharp_luma_matrix_width;    //  3..11, should be an odd number
    int  unsharp_luma_matrix_height;   //  3..11, should be an odd number
    int  unsharp_luma_amount;          //  Actually a double between -2.0 and 2.0, but handled as a int between -20 and 20
    int  unsharp_chroma_matrix_width;  //  3..11, should be an odd number
    int  unsharp_chroma_matrix_height; //  3..11, should be an odd number
    int  unsharp_chroma_amount;        //  Actually a double between -2.0 and 2.0, but handled as a int between -20 and 20

    // 3D noise reduction post plugin
    int  denoise3d;        // enable / disable
    int  denoise3d_luma;   //  Actually a double between 0.0 and 10.0, but handled as a int between 0 and 100
    int  denoise3d_chroma; //  Actually a double between 0.0 and 10.0, but handled as a int between 0 and 100
    int  denoise3d_time;   //  Actually a double between 0.0 and 10.0, but handled as a int between 0 and 100

    int  volnorm;          // enable/disable volnorm post plugin (normalize audio volume)
    
    // Remote server settings
    int  remote_mode;         // Allow remote clients (vdr-sxfe, vdr-fbfe, ...)
    int  listen_port;         // Port of remote server
    char remote_local_if[32]; // Listen only on this interface
    char remote_local_ip[32]; // Bind locally to this IP
    int  remote_keyboard;     // Allow remote client to control VDR with keyboard, LIRC, etc.
    int  remote_max_clients;  // Max. number of clients

    int  remote_usebcast;     // Use broadcasts to find servers automatically
    int  remote_usepipe;      // enable local pipes for video transport
    int  remote_usertp;       // enable RTP multicast for video transport
    int  remote_useudp;       // enable UDP unicast for video transport
    int  remote_usetcp;       // enable TCP streams for video transport
    int  remote_http_files;   // allow http streaming of media files to xineliboutput clients 
                              // (=currently replayed media file from xineliboutput media player) 
			      // streaming is used only if client can't access file directly (nfs etc.)
    int  remote_use_rtsp;     // allow generic rtsp for primary device. needs enabled udp or rtp
    int  remote_use_rtsp_ctrl;// allow rtsp to control primary device (play/pause/seek...)
    int  remote_use_http;     // allow generic http streaming (primary device output)
    int  remote_use_http_ctrl;// allow http to control primary device (play/pause/seek...)

    // RTP parameters
    char remote_rtp_addr[32]; //xxx.xxx.xxx.xxx\0
    int  remote_rtp_port;
    int  remote_rtp_ttl;
    int  remote_rtp_always_on;
    int  remote_rtp_sap;

    // Advanced settings
    int live_mode_sync;   /* Sync SCR to transponder clock in live mode */
    int scr_tuning;       /* Fine-tune xine egine SCR (to sync video to graphics output) */
    int scr_hz;           /* Current SCR speed (Hz), default is 90000 */

    int decoder_mpeg2;    /* DECODER_MPEG2_... */
    int decoder_h264;     /* DECODER_H264_...  */
    int ff_h264_speed_over_accurancy;
    int ff_h264_skip_loop_filter; /* FF_H264_SKIP_LOOPFILTER_* */

    config_t();

    bool SetupParse(const char *Name, const char *Value);
    bool ProcessArgs(int argc, char *argv[]);

    static bool IsImageFile(const char *);
    static bool IsAudioFile(const char *);
    static bool IsVideoFile(const char *);
    static bool IsPlaylistFile(const char *);
    static bool IsDvdFolder(const char *);
    static bool IsBluRayFolder(const char *);
    static bool IsDvdImage(const char *);
    static bool IsBluRayImage(const char *);

    cString AutocropOptions(void);
    cString SwScaleOptions(void);
    cString FfmpegPpOptions(void);
    cString UnsharpOptions(void);
    cString Denoise3dOptions(void);

#if 0
    template<typename T> bool IsOptionHidden(T & option)
      { return hidden_options[(int)((long int)&option - (long int)this)];};
    template<typename T> bool IsOptionReadOnly(T & option)
      { return readonly_options[(int)((long int)&option - (long int)this)];};
#endif

  protected:
    bool ProcessArg(const char *Name, const char *Value);
    static cString m_ProcessedArgs;

#if 0
    static uint8_t *hidden_options;
    static uint8_t *readonly_options;

    template<typename T> void HideOption(T & option)
      { hidden_options[(int)((long int)&option - (long int)this)] = 1;};
    template<typename T> void ReadOnlyOption(T & option)
      { readonly_options[(int)((long int)&option - (long int)this)] = 1;};
#endif
};

// Global instance
extern config_t xc;

// Find index of string in array of strings
static inline int strstra(const char * const str, const char * const stra[], int def_index)
{
  if(str && stra) {
    int i;
    for(i=0; stra[i]; i++)
      if(!strcmp(str,stra[i]))
	return i;
  }
  return def_index;
}

#endif //_XINELIB_CONFIG_H_

