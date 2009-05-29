/*
 * config.h: Global configuration and user settings
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: config.h,v 1.46 2008/04/29 12:44:25 phintuka Exp $
 *
 */

#ifndef _XINELIB_CONFIG_H_
#define _XINELIB_CONFIG_H_

#include <string.h>
#include <stdint.h>

#include <vdr/config.h>

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

#define FIELD_ORDER_NORMAL       0
#define FIELD_ORDER_INVERTED     1
#define FIELD_ORDER_count        2

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
#define X11_DRIVER_XVMC          3
#define X11_DRIVER_XXMC          4
#define X11_DRIVER_VIDIX         5
#define X11_DRIVER_DIRECTFB      6
#define X11_DRIVER_OPENGL        7
#define X11_DRIVER_SDL           8
#define X11_DRIVER_NONE          9
#define X11_DRIVER_count         10

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
#define AUDIO_VIS_count  5

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

// OSD blending methods
#define OSD_BLENDING_SOFTWARE   0  // xine-lib "normal" osd
#define OSD_BLENDING_HARDWARE   1  // xine-lib "unscaled osd"
#define OSD_BLENDING_count      2

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

#define SUBTITLESIZE_count     7

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
    static const char * const s_bufferSize[];
    static const int          i_pesBufferSize[];
    static const char * const s_aspects[];
    static const char * const s_deinterlaceMethods[];
    static const char * const s_deinterlaceMethodNames[];
    static const char * const s_fieldOrder[];
    static const char * const s_audioDriverNames[];
    static const char * const s_audioDrivers[];
    static const char * const s_videoDriverNamesX11[];
    static const char * const s_videoDriversX11[];
    static const char * const s_videoDriverNamesFB[];
    static const char * const s_videoDriversFB[];
    static const char * const s_frontendNames[];
    static const char * const s_frontends[];
    static const char * const s_frontend_files[];
    static const char * const s_audioEqNames[];
    static const char * const s_audioVisualizations[];
    static const char * const s_audioVisualizationNames[];
    static const char * const s_speakerArrangements[];
    static const char * const s_subtitleSizes[];
    static const char * const s_subExts[];
    static const char * const s_osdBlendingMethods[];
    static const char * const s_osdMixers[];
    static const char * const s_osdScalings[];
    static const char * const s_vo_aspects[];

  public:

    // Force xineliboutput to be the primary device
    int  force_primary_device;

    // local frontend settings
    char local_frontend[64];
    char video_driver[32];
    char video_port[32];     // X11: DISPLAY=...
    char audio_driver[32];
    char audio_port[64];
    char *post_plugins;      // static post plugins from command line options
    int  pes_buffers;

    char modeline[64];
    int  fullscreen;
    int  modeswitch;
    int  width;
    int  height;
    int  display_aspect;
    int  scale_video;
    int  field_order;
    int  exit_on_close;    // Terminate VDR when local frontend is closed
    int  use_x_keyboard;   // Use X11 keyboard to control VDR (console kbd is handled by VDR)

    // Audio settings
    int  speaker_type;
    int  audio_delay;        // in ms
    int  audio_compression;  // 100%(=off)...500%
    int  audio_equalizer[AUDIO_EQ_count];
    int  audio_surround;    // downmix multichannel audio to stereo surround
    int  headphone;         // mix audio for headphones
    int  audio_upmix;       // upmix stereo to 5.1
    int  sw_volume_control; // software (xine-lib) or hardware (alsa) volume control and muting

    // Video settings
    int  ibp_trickspeed;
    int  max_trickspeed;
    int  overscan;            // %
    int  hue;                 // 0...0xffff, -1 == off
    int  saturation;          // 0...0xffff, -1 == off
    int  contrast;            // 0...0xffff, -1 == off
    int  brightness;          // 0...0xffff, -1 == off
    int  vo_aspect_ratio;

    // OSD settings 
    eMainMenuMode main_menu_mode;  // used internally to open right sub-menu
    int  hide_main_menu;
    int  osd_mixer;                // show multiple OSD layers
    int  osd_scaling;              // OSD scaling mode: off, nearest, bilinear
    int  hud_osd;                  // head up display OSD
    int  osd_blending;             // OSD blending method
    int  osd_blending_lowresvideo; // Use hardware blending for low-resolution video
    int  alpha_correction;
    int  alpha_correction_abs;
    int  extsub_size; /* size of separate subtitles ( -1 = xine default ; 0...6 = { tiny  small  normal  large  very large  huge } */

    // Media player
#if VDRVERSNUM < 10515
    int  spu_autoshow;    // Preferred SPU language(s) for media player
    char spu_lang[4][4];
#endif
    char browse_files_dir[4096];
    char browse_music_dir[4096];
    char browse_images_dir[4096];
    int  cache_implicit_playlists; // used in playlist.c
    int  enable_id3_scanner;       // used in playlist.c
    int  subtitle_vpos;            // used in media player. Not saved !
    int  playlist_tracknumber;
    int  playlist_artist;
    int  playlist_album;   

    // Audio visualization
    char audio_visualization[64];
    char audio_vis_goom_opts[256];

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
    int  autocrop_soft;
    int  autocrop_fixedsize;
    int  autocrop_subs;

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

    int  remote_usebcast;     // Use proadcasts to find servers automatically
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
    int scr_tunning;      /* Fine-tune xine egine SCR (to sync video to graphics output) */
    int scr_hz;           /* Current SCR speed (Hz), default is 90000 */

    config_t();

    bool SetupParse(const char *Name, const char *Value);
    bool ProcessArgs(int argc, char *argv[]);

    bool IsImageFile(const char *);
    bool IsAudioFile(const char *);
    bool IsVideoFile(const char *);
    bool IsPlaylistFile(const char *);

    cString AutocropOptions(void);
    cString SwScaleOptions(void);
    cString FfmpegPpOptions(void);
    cString UnsharpOptions(void);
    cString Denoise3dOptions(void);

    template<typename T> bool IsOptionHidden(T & option)
      { return hidden_options[(int)((long int)&option - (long int)this)];};
    template<typename T> bool IsOptionReadOnly(T & option)
      { return readonly_options[(int)((long int)&option - (long int)this)];};

  protected:
    bool ProcessArg(const char *Name, const char *Value);
    static cString m_ProcessedArgs;

    static uint8_t *hidden_options;
    static uint8_t *readonly_options;

    template<typename T> void HideOption(T & option)
      { hidden_options[(int)((long int)&option - (long int)this)] = 1;};
    template<typename T> void ReadOnlyOption(T & option)
      { readonly_options[(int)((long int)&option - (long int)this)] = 1;};
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

