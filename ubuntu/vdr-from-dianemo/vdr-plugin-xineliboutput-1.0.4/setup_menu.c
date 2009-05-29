/*
 * setup_menu.c: Setup Menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: setup_menu.c,v 1.58.2.1 2008/10/18 10:41:06 phintuka Exp $
 *
 */

#include <vdr/config.h>
#include <vdr/plugin.h>
#include <vdr/remote.h>

#include "setup_menu.h"
#include "device.h"
#include "menuitems.h"
#include "config.h"
#include "i18n.h"      // trVDR for vdr-1.4.x
#include "osd.h"       // cXinelibOsdProvider::RefreshOsd()


namespace XinelibOutputSetupMenu {

//#define INTEGER_CONFIG_VIDEO_CONTROLS
//#define LINEAR_VIDEO_CONTROLS

#define ISNUMBERKEY(k) (RAWKEY(k) >= k0 && RAWKEY(k) <= k9)

//--- Setup Menu -------------------------------------------------------------

const char *ModeLineChars = 
  " 0123456789+-hvsync.";
const char *DriverNameChars = 
  " abcdefghijklmnopqrstuvwxyz0123456789-.,#~:;";
const char *OptionsChars = 
  "=.,abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char *LangNameChars = 
  "abcdefghijklmnopqrstuvwxyz";

const char *controls[] =
  { "Off",
    "[|---------------]","[|---------------]",
    "[-|--------------]","[-|--------------]",
    "[--|-------------]","[--|-------------]",
    "[---|------------]","[---|------------]",
    "[----|-----------]","[----|-----------]",
    "[-----|----------]","[-----|----------]",
    "[------|---------]","[------|---------]",
    "[-------|--------]","[-------|--------]",
    "[--------|-------]","[--------|-------]",
    "[---------|------]","[---------|------]",
    "[----------|-----]","[----------|-----]",
    "[-----------|----]","[-----------|----]",
    "[------------|---]","[------------|---]",
    "[-------------|--]","[-------------|--]",
    "[--------------|-]","[--------------|-]",
    "[---------------|]","[---------------|]",
    NULL
  };

#ifdef LINEAR_VIDEO_CONTROLS
#  define CONTROL_TO_INDEX(val) ((val)>=0 ? ((val)>>11)+1 : 0)
#  define INDEX_TO_CONTROL(ind) ((ind)==0 ? -1 : ((ind)-1)<<11)
#else
const int ind2ctrl_tbl[33] = {
      -1,      0, 0x0001, 0x0002, 0x0003, 0x0004, 0x0007, 0x000a, 
  0x000f, 0x0014, 0x001f,     42, 0x003f,     80, 0x007f,    170, 
  0x00ff,    336, 0x01ff,    682, 0x03ff,   1630, 0x07ff,   2730,
  0x0fff,   5726, 0x1fff,  10858, 0x3fff,  22110, 0x7fff,  43224,
  0xffff  };
static int CONTROL_TO_INDEX(int val) 
{
  for(int i=0; i<33;i++)
    if(val<=ind2ctrl_tbl[i])
      return i;
  return 32;
}
static int INDEX_TO_CONTROL(int ind) 
{
  if(ind<0) ind=0;
  if(ind>32) ind=32;
  return ind2ctrl_tbl[ind];
}
#endif

static cOsdItem *NewTitle(const char *s)
{
  char str[128];
  cOsdItem *tmp;
  snprintf(str, sizeof(str), "----- %s -----", s);
  str[sizeof(str)-1] = 0;
  tmp = new cOsdItem(str);
  tmp->SetSelectable(false);
  return tmp;
}

//--- cMenuSetupAudio --------------------------------------------------------

class cMenuSetupAudio : public cMenuSetupPage 
{
  private:
    config_t newconfig;
    int visualization;
    int goom_width, goom_height, goom_fps;

    cOsdItem *audio_ctrl_speakers;
    cOsdItem *audio_ctrl_volume;
    cOsdItem *audio_ctrl_delay;
    cOsdItem *audio_ctrl_compression;
    cOsdItem *audio_ctrl_upmix;
    cOsdItem *audio_ctrl_surround;
    cOsdItem *audio_ctrl_headphone;
    cOsdItem *audio_ctrl_vis;
  
  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupAudio(void);
    ~cMenuSetupAudio(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupAudio::cMenuSetupAudio(void) 
{
  memcpy(&newconfig, &xc, sizeof(config_t));

  visualization = strstra(xc.audio_visualization, 
			  xc.s_audioVisualizations, 
			  0);
  goom_width  = 720;
  goom_height = 576;
  goom_fps    = 25;

  char *pt;
  if(NULL != (pt=strstr(xc.audio_vis_goom_opts, "width=")))
    goom_width = max(320, min(1920, atoi(pt+6)));
  if(NULL != (pt=strstr(xc.audio_vis_goom_opts, "height=")))
    goom_height = max(240, min(1280, atoi(pt+7)));
  if(NULL != (pt=strstr(xc.audio_vis_goom_opts, "fps=")))
    goom_fps = max(1, min(100, atoi(pt+4)));

  Set();
}

cMenuSetupAudio::~cMenuSetupAudio(void) 
{
  cXinelibDevice::Instance().ConfigurePostprocessing(
        xc.deinterlace_method, xc.audio_delay, xc.audio_compression, 
	xc.audio_equalizer, xc.audio_surround, xc.speaker_type);
  cXinelibDevice::Instance().ConfigurePostprocessing(
        "upmix", xc.audio_upmix ? true : false, NULL);
#ifdef ENABLE_TEST_POSTPLUGINS
  cXinelibDevice::Instance().ConfigurePostprocessing(
        "headphone", xc.headphone ? true : false, NULL);
#endif
}

void cMenuSetupAudio::Set(void)
{
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  int current = Current();
  Clear();

  Add(NewTitle(tr("Audio")));

  Add(audio_ctrl_speakers =
      new cMenuEditStraI18nItem(tr("Speakers"), &newconfig.speaker_type, 
				SPEAKERS_count, xc.s_speakerArrangements));

  Add(audio_ctrl_volume =
      new cMenuEditBoolItem(tr("Volume control"), 
			    &newconfig.sw_volume_control,
			    tr("Hardware"), tr("Software")));

  Add(audio_ctrl_delay = 
      new cMenuEditTypedIntItem(tr("Delay"), tr("ms"), &newconfig.audio_delay,
				-3000, 3000, tr("Off")));
  Add(audio_ctrl_compression = 
      new cMenuEditTypedIntItem(tr("Audio Compression"), "%", 
				&newconfig.audio_compression, 
				100, 500, NULL, tr("Off")));
  Add(audio_ctrl_upmix =
      new cMenuEditBoolItem(tr("Upmix stereo to 5.1"), 
			    &newconfig.audio_upmix));
  Add(audio_ctrl_surround =
      new cMenuEditBoolItem(tr("Downmix AC3 to surround"), 
			    &newconfig.audio_surround));
#ifdef ENABLE_TEST_POSTPLUGINS
  Add(audio_ctrl_headphone =
      new cMenuEditBoolItem(tr("Mix to headphones"), 
			    &newconfig.headphone));
#else
  audio_ctrl_headphone = NULL;
#endif
  Add(audio_ctrl_vis = 
      new cMenuEditStraI18nItem(tr("Visualization"), &visualization, 
				AUDIO_VIS_count, 
				xc.s_audioVisualizationNames));
  if(visualization == AUDIO_VIS_GOOM) {
    Add(new cMenuEditTypedIntItem(tr("  Width"), tr("px"), &goom_width,
				  320, 1920));
    Add(new cMenuEditTypedIntItem(tr("  Height"),tr("px"), &goom_height,
				  240, 1280));
    Add(new cMenuEditTypedIntItem(tr("  Speed"), tr("fps"), &goom_fps,
				  1, 100));
  }

  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupAudio::ProcessKey(eKeys Key)
{
  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  Key = NORMALKEY(Key);

  if(Key!=kLeft && Key!=kRight)
    return state;

  if(item == audio_ctrl_delay || item == audio_ctrl_compression) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
	  xc.deinterlace_method, newconfig.audio_delay, 
	  newconfig.audio_compression, newconfig.audio_equalizer,
	  newconfig.audio_surround, newconfig.speaker_type);
  }
  else if(item == audio_ctrl_vis) {
    Set();
  }
  else if(item == audio_ctrl_speakers) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
          xc.deinterlace_method, newconfig.audio_delay, 
	  newconfig.audio_compression, newconfig.audio_equalizer,
	  newconfig.audio_surround, newconfig.speaker_type);
    if(newconfig.speaker_type <= SPEAKERS_STEREO &&
       newconfig.audio_upmix) {
      newconfig.audio_upmix = false;
      Set();
    }
  }
  else if(item == audio_ctrl_surround) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
          xc.deinterlace_method, newconfig.audio_delay, 
	  newconfig.audio_compression, newconfig.audio_equalizer,
	  newconfig.audio_surround, newconfig.speaker_type);
    if(newconfig.audio_surround && newconfig.audio_upmix) {
      newconfig.audio_upmix = 0;
      Set();
    }
  }
  else if(item == audio_ctrl_volume) {
    // trigger volume control message by toggling mute
    cRemote::Put(kMute);
    cRemote::Put(kMute);
  }
  else if(item == audio_ctrl_upmix) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
	  "upmix", newconfig.audio_upmix ? true : false, NULL);
    if(newconfig.audio_upmix && newconfig.audio_surround) {
      newconfig.audio_surround = 0;
      Set();
    }
  }
#ifdef ENABLE_TEST_POSTPLUGINS
  else if(item == audio_ctrl_headphone) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
	  "headphone", newconfig.headphone ? true : false, NULL);
  }
#endif

  return state;
}


void cMenuSetupAudio::Store(void)
{
  memcpy(&xc, &newconfig, sizeof(config_t));

  strn0cpy(xc.audio_visualization, xc.s_audioVisualizations[visualization], 
	   sizeof(xc.audio_visualization));
  snprintf(xc.audio_vis_goom_opts, sizeof(xc.audio_vis_goom_opts),
	   "width=%d,height=%d,fps=%d",
	   goom_width, goom_height, goom_fps);
  xc.audio_vis_goom_opts[sizeof(xc.audio_vis_goom_opts)-1] = 0;

  SetupStore("Audio.Speakers", xc.s_speakerArrangements[xc.speaker_type]);
  SetupStore("Audio.Delay",    xc.audio_delay);
  SetupStore("Audio.Compression",  xc.audio_compression);
  SetupStore("Audio.Surround",     xc.audio_surround);
  SetupStore("Audio.Upmix",        xc.audio_upmix);
  SetupStore("Audio.Headphone",    xc.headphone);
  SetupStore("Audio.Visualization",xc.audio_visualization);
  SetupStore("Audio.Visualization.GoomOpts",xc.audio_vis_goom_opts);
  SetupStore("Audio.SoftwareVolumeControl", xc.sw_volume_control);
  Setup.Save();
}

//--- cMenuSetupAudioEq ------------------------------------------------------

class cMenuSetupAudioEq : public cMenuSetupPage 
{
  private:
    config_t newconfig;
  
  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupAudioEq(void);
    ~cMenuSetupAudioEq(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupAudioEq::cMenuSetupAudioEq(void) 
{
  memcpy(&newconfig, &xc, sizeof(config_t));
  Set();
}

cMenuSetupAudioEq::~cMenuSetupAudioEq(void) 
{
  cXinelibDevice::Instance().ConfigurePostprocessing(
        xc.deinterlace_method, xc.audio_delay, xc.audio_compression, 
	xc.audio_equalizer, xc.audio_surround, xc.speaker_type);
}

void cMenuSetupAudioEq::Set(void)
{
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  int current = Current();
  Clear();

  Add(NewTitle(tr("Audio Equalizer")));
  for(int i=0; i<AUDIO_EQ_count; i++)
    Add(new cMenuEditTypedIntItem(config_t::s_audioEqNames[i], "%", 
				  &newconfig.audio_equalizer[i],
				  -100, 100, tr("Off")));

  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupAudioEq::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  Key = NORMALKEY(Key);

  if(Key == kLeft || Key == kRight) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
	xc.deinterlace_method, xc.audio_delay, xc.audio_compression, 
	newconfig.audio_equalizer, xc.audio_surround, xc.speaker_type);
  }

  return state;
}

void cMenuSetupAudioEq::Store(void)
{
  memcpy(&xc, &newconfig, sizeof(config_t));

  char tmp[256];
  sprintf(tmp,"%d %d %d %d %d %d %d %d %d %d",
	  xc.audio_equalizer[0], xc.audio_equalizer[1],
	  xc.audio_equalizer[2], xc.audio_equalizer[3],
	  xc.audio_equalizer[4], xc.audio_equalizer[5],
	  xc.audio_equalizer[6], xc.audio_equalizer[7],
	  xc.audio_equalizer[8], xc.audio_equalizer[9]);
  SetupStore("Audio.Equalizer", tmp);
  Setup.Save();
}

//--- cMenuSetupVideo --------------------------------------------------------

static const char * const tvtime_method[] =
  { "use_vo_driver",
    "Linear",
    "LinearBlend",
    "Greedy",
    "Greedy2Frame",
    "Weave",
    "LineDoubler",
    "Vertical",
    "ScalerBob",
    "GreedyH",
    "TomsMoComp",
    NULL};

static const int tvtime_methods_count = (sizeof(tvtime_method)/sizeof(tvtime_method[0]) - 1);

static const char * const tvtime_method_name[] =
  {trNOOP("Use Video-Out Driver"),       // "use_vo_driver"
   "Linear Interpolation",               // "Linear",
   "Linear Blend (mplayer)",             // "LinearBlend",
   "Greedy - Low motion (DScaler)",      // "Greedy",
   "Greedy 2-frame (DScaler)",           // "Greedy2Frame",
   "Weave Last Field",                   // "Weave",
   "Line Doubler",                       // "LineDoubler",
   "Vertical Blend (ffmpeg)",            // "Vertical",
   "Scaler Bob",                         // "ScalerBob",
   "Greedy - High Motion (DScaler)",     // "GreedyH",
   "Tom's Motion Compensated (DScaler)", // "TomsMoComp",
   NULL};

static const char * const tvtime_pulldown[] = 
  { "none",
    "vector",
    NULL};

static const char * const tvtime_pulldown_name[] = 
  { trNOOP("none"),
    trNOOP("vector"),
    NULL};

static const char * const tvtime_framerate[] = 
  { "full",
    "half_top",
    "half_bottom",
    NULL};

static const char * const tvtime_framerate_name[] = 
  { trNOOP("full"),
    trNOOP("half (top)"),
    trNOOP("half (bottom)"),
    NULL};

struct tvtime_s {
  int method;
  int cheap_mode;        // on/off
  int pulldown;          // none, vector
  int framerate;         // full, half_top, half_bottom
  int judder_correction; // on/off
  int use_progressive_frame_flag; // on/off
  int chroma_filter;     // on/off

  void Parse(const char *str)
  {
    cheap_mode = strstr(str, "cheap_mode=1") ? 1 : 0;
    pulldown   = strstr(str, "pulldown=none") ? 0 :
                 strstr(str, "pulldown=0") ? 0 : 1;
    framerate  = strstr(str, "framerate_mode=half_top")    ? 1 : 
                 strstr(str, "framerate_mode=1")           ? 1 : 
                 strstr(str, "framerate_mode=half_bottom") ? 2 :
                 strstr(str, "framerate_mode=2")           ? 2 : 0;
    chroma_filter = strstr(str, "chroma_filter=1") ? 1 : 0;
    judder_correction = strstr(str, "judder_correction=0") ? 0 : 1;
    use_progressive_frame_flag = strstr(str, "use_progressive_frame_flag=0") ? 0 : 1;
    method=1;
    char *m = strstr(str, "method=");
    if(m) {
      char *tmp = strdup(m + 7);
      if(strchr(tmp, ','))
	*strchr(tmp, ',') = 0;
      method = strstra(tmp, tvtime_method, 1);
      free(tmp);
    }
  }

  const char *ToString(void)
  {
    static char buf[256];
    snprintf(buf, sizeof(buf),
	     "method=%s,cheap_mode=%d,pulldown=%s,framerate_mode=%s,"
	    "judder_correction=%d,use_progressive_frame_flag=%d,"
	    "chroma_filter=%d,enabled=1",
	    tvtime_method[method], cheap_mode, tvtime_pulldown[pulldown],
	    tvtime_framerate[framerate], judder_correction, 
	    use_progressive_frame_flag, chroma_filter);
    buf[sizeof(buf)-1] = 0;
    return buf;
  }
};

class cMenuSetupVideo : public cMenuSetupPage 
{
  private:
    config_t newconfig;

    cOsdItem *ctrl_autocrop;
    cOsdItem *ctrl_swscale;
    cOsdItem *ctrl_swscale_resize;
    cOsdItem *ctrl_swscale_aspect;
    cOsdItem *ctrl_swscale_width;
    cOsdItem *ctrl_swscale_height;
    cOsdItem *ctrl_hue;
    cOsdItem *ctrl_saturation;
    cOsdItem *ctrl_contrast;
    cOsdItem *ctrl_brightness;
    cOsdItem *ctrl_overscan;
    cOsdItem *ctrl_pp;
    cOsdItem *ctrl_deinterlace;
    cOsdItem *ctrl_tvtime_method;
    cOsdItem *ctrl_unsharp;
    cOsdItem *ctrl_denoise3d;
    cOsdItem *ctrl_vo_aspect_ratio;

    int deinterlace;
    struct tvtime_s tvtime;
  
  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupVideo(void);
    ~cMenuSetupVideo(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupVideo::cMenuSetupVideo(void)
{
  memcpy(&newconfig, &xc, sizeof(config_t));

  newconfig.hue        = CONTROL_TO_INDEX(newconfig.hue);
  newconfig.saturation = CONTROL_TO_INDEX(newconfig.saturation);
  newconfig.contrast   = CONTROL_TO_INDEX(newconfig.contrast);
  newconfig.brightness = CONTROL_TO_INDEX(newconfig.brightness);

  deinterlace = strstra(xc.deinterlace_method, xc.s_deinterlaceMethods, 0);

  tvtime.Parse(newconfig.deinterlace_opts);

  Set();
}

cMenuSetupVideo::~cMenuSetupVideo(void)
{
  cXinelibDevice::Instance().ConfigureVideo(xc.hue, xc.saturation, 
					    xc.brightness, xc.contrast,
					    xc.overscan, xc.vo_aspect_ratio);
  cXinelibDevice::Instance().ConfigurePostprocessing(
       "autocrop", xc.autocrop ? true : false, xc.AutocropOptions());
  cXinelibDevice::Instance().ConfigurePostprocessing(
       "swscale", xc.swscale ? true : false, xc.SwScaleOptions());
  cXinelibDevice::Instance().ConfigurePostprocessing(
       "pp", xc.ffmpeg_pp ? true : false, xc.FfmpegPpOptions());
  cXinelibDevice::Instance().ConfigurePostprocessing(
       "unsharp", xc.unsharp ? true : false, xc.UnsharpOptions());
  cXinelibDevice::Instance().ConfigurePostprocessing(
       "denoise3d", xc.denoise3d ? true : false, xc.Denoise3dOptions());
  cXinelibDevice::Instance().ConfigurePostprocessing(
        xc.deinterlace_method, xc.audio_delay, xc.audio_compression,
        xc.audio_equalizer, xc.audio_surround, xc.speaker_type);
}

void cMenuSetupVideo::Set(void)
{
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  int current = Current();
  Clear();

  Add(NewTitle(tr("Video")));

  Add(ctrl_vo_aspect_ratio =
      new cMenuEditStraI18nItem(tr("Aspect ratio"), &newconfig.vo_aspect_ratio,
                                  VO_ASPECT_count, xc.s_vo_aspects));

  Add(ctrl_autocrop = 
      new cMenuEditBoolItem(tr("Crop letterbox 4:3 to 16:9"), 
			    &newconfig.autocrop));
  if(newconfig.autocrop) {
    Add(new cMenuEditBoolItem(tr("  Autodetect letterbox"), 
			      &newconfig.autocrop_autodetect));
    Add(new cMenuEditBoolItem(tr("  Soft start"), 
			      &newconfig.autocrop_soft));
    Add(new cMenuEditBoolItem(tr("  Crop to"),
			      &newconfig.autocrop_fixedsize,
			      "4:3...20:9", "14:9/16:9"));
    Add(new cMenuEditBoolItem(tr("  Detect subtitles"),
			      &newconfig.autocrop_subs));
  }

  ctrl_swscale_resize = ctrl_swscale_aspect = ctrl_swscale_width = ctrl_swscale_height = NULL;
  Add(ctrl_swscale =
      new cMenuEditBoolItem(tr("Software scaling"), 
			    &newconfig.swscale));
  if(newconfig.swscale) {
    Add(ctrl_swscale_aspect =
	new cMenuEditBoolItem(tr("  Change aspect ratio"), 
			      &newconfig.swscale_change_aspect));
    Add(ctrl_swscale_resize = 
	new cMenuEditBoolItem(tr("  Change video size"), 
			      &newconfig.swscale_resize));
    if(newconfig.swscale_resize) {
      Add(ctrl_swscale_width =
	  new cMenuEditIntItem( tr("  Width"), 
				&newconfig.swscale_width, 360, 2000));
      Add(ctrl_swscale_height =
	  new cMenuEditIntItem( tr("  Height"),
				&newconfig.swscale_height, 288, 1200));
      Add(new cMenuEditBoolItem(tr("  Allow downscaling"),
				&newconfig.swscale_downscale));
    }
  }

  Add(ctrl_overscan =
      new cMenuEditTypedIntItem(tr("Overscan (crop image borders)"), "%", 
				&newconfig.overscan, 0, 10,
				tr("Off")));

  Add(ctrl_pp = new cMenuEditBoolItem(tr("Post processing (ffmpeg)"), 
				      &newconfig.ffmpeg_pp));
  if(newconfig.ffmpeg_pp) {
    Add(new cMenuEditIntItem( tr("  Quality"), 
			      &newconfig.ffmpeg_pp_quality, 0, 6));
    Add(new cMenuEditStrItem( tr("  Mode"), newconfig.ffmpeg_pp_mode, 
			      255, OptionsChars));
  }

  Add(ctrl_deinterlace = 
      new cMenuEditStraI18nItem(tr("Deinterlacing"), &deinterlace, 
				DEINTERLACE_count, 
				xc.s_deinterlaceMethodNames));

  ctrl_tvtime_method = NULL;
  if(deinterlace == DEINTERLACE_TVTIME) {
    Add(ctrl_tvtime_method =
	new cMenuEditStraI18nItem(tr("  Method"), &tvtime.method,
				  tvtime_methods_count, tvtime_method_name));
    Add(new cMenuEditBoolItem(tr("  Cheap mode"), &tvtime.cheap_mode));
    Add(new cMenuEditStraI18nItem(tr("  Pulldown"), &tvtime.pulldown,
				  2, tvtime_pulldown_name));
    Add(new cMenuEditStraI18nItem(tr("  Frame rate"), &tvtime.framerate,
				  3, tvtime_framerate_name));
    Add(new cMenuEditBoolItem(tr("  Judder Correction"), &tvtime.judder_correction));
    Add(new cMenuEditBoolItem(tr("  Use progressive frame flag"), 
			      &tvtime.use_progressive_frame_flag));
    Add(new cMenuEditBoolItem(tr("  Chroma Filter"), 
			      &tvtime.chroma_filter));
  }

  Add(ctrl_unsharp = new cMenuEditBoolItem(tr("Sharpen / Blur"),
                                      &newconfig.unsharp));
  if(newconfig.unsharp) {
    Add(new cMenuEditOddIntItem( tr("  Width of the luma matrix"),
                                 &newconfig.unsharp_luma_matrix_width, 3, 11));
    Add(new cMenuEditOddIntItem( tr("  Height of the luma matrix"),
                                 &newconfig.unsharp_luma_matrix_height, 3, 11));
    Add(new cMenuEditFpIntItem( tr("  Amount of luma sharpness/blur"),
                                &newconfig.unsharp_luma_amount, -20, 20, 1,
                                tr("Off")));
    Add(new cMenuEditOddIntItem( tr("  Width of the chroma matrix"),
                              &newconfig.unsharp_chroma_matrix_width, 3, 11));
    Add(new cMenuEditOddIntItem( tr("  Height of the chroma matrix"),
                              &newconfig.unsharp_chroma_matrix_height, 3, 11));
    Add(new cMenuEditFpIntItem( tr("  Amount of chroma sharpness/blur"),
                                &newconfig.unsharp_chroma_amount, -20, 20, 1,
                                tr("Off")));
  }

  Add(ctrl_denoise3d = new cMenuEditBoolItem(tr("3D Denoiser"),
                                      &newconfig.denoise3d));
  if(newconfig.denoise3d) {
    Add(new cMenuEditFpIntItem( tr("  Spatial luma strength"),
                                &newconfig.denoise3d_luma, 0, 100, 1));
    Add(new cMenuEditFpIntItem( tr("  Spatial chroma strength"),
                                &newconfig.denoise3d_chroma, 0, 100, 1));
    Add(new cMenuEditFpIntItem( tr("  Temporal strength"),
                                &newconfig.denoise3d_time, 0, 100, 1));
  }


#ifdef INTEGER_CONFIG_VIDEO_CONTROLS
  Add(new cMenuEditIntItem(tr("HUE"), &newconfig.hue, -1, 0xffff));
  Add(new cMenuEditIntItem(tr("Saturation"), &newconfig.saturation,-1,0xffff));
  Add(new cMenuEditIntItem(tr("Contrast"),   &newconfig.contrast, -1, 0xffff));
  Add(new cMenuEditIntItem(tr("Brightness"), &newconfig.brightness,-1,0xffff));
#else
  Add(ctrl_hue = new cMenuEditStraItem(tr("HUE"), &newconfig.hue, 33, 
				       controls));
  Add(ctrl_saturation = 
      new cMenuEditStraItem(tr("Saturation"), &newconfig.saturation, 33, 
			    controls));
  Add(ctrl_contrast = 
      new cMenuEditStraItem(tr("Contrast"), &newconfig.contrast, 33, 
			    controls));
  Add(ctrl_brightness = 
      new cMenuEditStraItem(tr("Brightness"), &newconfig.brightness, 33, 
			    controls));
#endif

#ifdef DEVICE_SUPPORTS_IBP_TRICKSPEED
  Add(new cMenuEditBoolItem(tr("Smooth fast forward"),
			    &newconfig.ibp_trickspeed));
#endif
  Add(new cMenuEditIntItem(tr("Fastest trick speed"),
			   &newconfig.max_trickspeed, 1, 12));

  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  //SetCurrent(Get(1));
  Display();
}

eOSState cMenuSetupVideo::ProcessKey(eKeys Key)
{
  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  Key = NORMALKEY(Key);

  if(Key!=kLeft && Key!=kRight)
    return state;

  if(item == ctrl_hue || item == ctrl_saturation || 
     item == ctrl_contrast || item == ctrl_brightness ||
     item == ctrl_overscan || item == ctrl_vo_aspect_ratio)
#ifdef INTEGER_CONFIG_VIDEO_CONTROLS
    cXinelibDevice::Instance().ConfigureVideo(newconfig.hue, 
					      newconfig.saturation,
					      newconfig.brightness, 
					      newconfig.contrast,
					      newconfig.overscan,
                                              newconfig.vo_aspect_ratio);
#else
    cXinelibDevice::Instance().ConfigureVideo(
       INDEX_TO_CONTROL(newconfig.hue), 
       INDEX_TO_CONTROL(newconfig.saturation),
       INDEX_TO_CONTROL(newconfig.brightness), 
       INDEX_TO_CONTROL(newconfig.contrast),
       newconfig.overscan, newconfig.vo_aspect_ratio);
#endif
  else if(item == ctrl_autocrop) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
	 "autocrop", newconfig.autocrop ? true : false, 
	 newconfig.AutocropOptions());
    Set();
  }
  else if(item == ctrl_swscale || 
	  item == ctrl_swscale_resize ||
	  item == ctrl_swscale_aspect ||
	  item == ctrl_swscale_width ||
	  item == ctrl_swscale_height) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
         "swscale", newconfig.swscale ? true : false, 
	 newconfig.SwScaleOptions());
    Set();
  }
  else if(item == ctrl_pp) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
	 "pp", newconfig.ffmpeg_pp ? true : false, 
	 newconfig.FfmpegPpOptions());
    Set();
  }
  else if(item == ctrl_unsharp) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
         "unsharp", newconfig.unsharp ? true : false, 
         newconfig.UnsharpOptions());
    Set();
  } 
  else if(item == ctrl_denoise3d) {
    cXinelibDevice::Instance().ConfigurePostprocessing(
         "denoise3d", newconfig.denoise3d ? true : false, 
         newconfig.Denoise3dOptions());
    Set();
  } 
  else if(item == ctrl_deinterlace) {
    if(deinterlace == DEINTERLACE_TVTIME && !ctrl_tvtime_method) {
      Set();
    } else if(deinterlace != DEINTERLACE_TVTIME && ctrl_tvtime_method) {
      Set();
    }
  }

  return state;
}

void cMenuSetupVideo::Store(void)
{
  memcpy(&xc, &newconfig, sizeof(config_t));

#ifdef INTEGER_CONFIG_VIDEO_CONTROLS
#else
  xc.hue        = INDEX_TO_CONTROL(xc.hue);
  xc.saturation = INDEX_TO_CONTROL(xc.saturation);
  xc.contrast   = INDEX_TO_CONTROL(xc.contrast);
  xc.brightness = INDEX_TO_CONTROL(xc.brightness);
#endif

  strn0cpy(xc.deinterlace_method, xc.s_deinterlaceMethods[deinterlace], sizeof(xc.deinterlace_method));
  strn0cpy(xc.deinterlace_opts, tvtime.ToString(), sizeof(xc.deinterlace_opts));
  SetupStore("Video.Deinterlace", xc.deinterlace_method);
  SetupStore("Video.DeinterlaceOptions", xc.deinterlace_opts);

  SetupStore("Video.AutoCrop",   xc.autocrop); 
  SetupStore("Video.AutoCrop.AutoDetect", xc.autocrop_autodetect);
  SetupStore("Video.AutoCrop.SoftStart",  xc.autocrop_soft);
  SetupStore("Video.AutoCrop.FixedSize",  xc.autocrop_fixedsize);
  SetupStore("Video.AutoCrop.DetectSubs", xc.autocrop_subs);
  SetupStore("Video.SwScale",           xc.swscale);
  SetupStore("Video.SwScale.Aspect",    xc.swscale_change_aspect);
  SetupStore("Video.SwScale.Resize",    xc.swscale_resize);
  SetupStore("Video.SwScale.Width",     xc.swscale_width);
  SetupStore("Video.SwScale.Height",    xc.swscale_height);
  SetupStore("Video.SwScale.Downscale", xc.swscale_downscale);
  SetupStore("Video.HUE",        xc.hue);
  SetupStore("Video.Saturation", xc.saturation);
  SetupStore("Video.Contrast",   xc.contrast);
  SetupStore("Video.Brightness", xc.brightness);
  SetupStore("Video.Overscan",   xc.overscan);
  SetupStore("Video.IBPTrickSpeed", xc.ibp_trickspeed);
  SetupStore("Video.MaxTrickSpeed", xc.max_trickspeed);
  SetupStore("Video.AspectRatio", xc.vo_aspect_ratio);
  SetupStore("Post.pp.Enable",   xc.ffmpeg_pp);
  SetupStore("Post.pp.Quality",  xc.ffmpeg_pp_quality);
  SetupStore("Post.pp.Mode",     xc.ffmpeg_pp_mode);
  SetupStore("Post.unsharp.Enable",               xc.unsharp);
  SetupStore("Post.unsharp.luma_matrix_width",    xc.unsharp_luma_matrix_width);
  SetupStore("Post.unsharp.luma_matrix_height",   xc.unsharp_luma_matrix_height);
  SetupStore("Post.unsharp.luma_amount",          xc.unsharp_luma_amount);
  SetupStore("Post.unsharp.chroma_matrix_width",  xc.unsharp_chroma_matrix_width);
  SetupStore("Post.unsharp.chroma_matrix_height", xc.unsharp_chroma_matrix_height);
  SetupStore("Post.unsharp.chroma_amount",        xc.unsharp_chroma_amount);
  SetupStore("Post.denoise3d.Enable",   xc.denoise3d);
  SetupStore("Post.denoise3d.luma",     xc.denoise3d_luma);
  SetupStore("Post.denoise3d.chroma",   xc.denoise3d_chroma);
  SetupStore("Post.denoise3d.time",     xc.denoise3d_time);
#if 1
  // delete old keys (<1.0.0)
  SetupStore("Video.AutoScale");
#endif
  Setup.Save();
}


//--- cMenuSetupOSD ----------------------------------------------------------

class cMenuSetupOSD : public cMenuSetupPage 
{
  private:
    config_t newconfig;

    int orig_alpha_correction;
    int orig_alpha_correction_abs;

    cOsdItem *ctrl_scaling;
    cOsdItem *ctrl_alpha;
    cOsdItem *ctrl_alpha_abs;
    cOsdItem *ctrl_blending;
    cOsdItem *ctrl_lowres;
#if VDRVERSNUM < 10515
    cOsdItem *ctrl_spulang0;
#endif

  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupOSD(void);
    ~cMenuSetupOSD();

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupOSD::cMenuSetupOSD(void)
{
  memcpy(&newconfig, &xc, sizeof(config_t));
  orig_alpha_correction     = xc.alpha_correction;
  orig_alpha_correction_abs = xc.alpha_correction_abs;
  newconfig.extsub_size++;

  Set();
}

cMenuSetupOSD::~cMenuSetupOSD()
{
  xc.alpha_correction = orig_alpha_correction;
  xc.alpha_correction_abs = orig_alpha_correction_abs;
}

void cMenuSetupOSD::Set(void)
{ 
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  int current = Current();
  Clear();

  ctrl_scaling = NULL;
  ctrl_blending = NULL;
  ctrl_lowres = NULL;
  ctrl_alpha = NULL;
  ctrl_alpha_abs = NULL;
#if VDRVERSNUM < 10515
  ctrl_spulang0 = NULL;
#endif

  Add(NewTitle(tr("On-Screen Display")));
  Add(new cMenuEditBoolItem(tr("Hide main menu"), 
			    &newconfig.hide_main_menu));

  Add(ctrl_blending =
      new cMenuEditBoolItem(tr("Blending method"),
			    &newconfig.osd_blending,
			    tr(xc.s_osdBlendingMethods[OSD_BLENDING_SOFTWARE]),
			    tr(xc.s_osdBlendingMethods[OSD_BLENDING_HARDWARE])));
  if(newconfig.osd_blending == OSD_BLENDING_SOFTWARE) {
    Add(ctrl_lowres =
	new cMenuEditBoolItem(tr("  Use hardware for low-res video"),
			      &newconfig.osd_blending_lowresvideo));
  }

  Add(ctrl_scaling =
      new cMenuEditStraI18nItem(tr("Scaling method"), &newconfig.osd_scaling,
	                        OSD_SCALING_count, xc.s_osdScalings));

#if VDRVERSNUM >= 10509
# if 0
  Add(new cMenuEditStraI18nItem(tr("Show all layers"), &newconfig.osd_mixer, 
			OSD_MIXER_count, xc.s_osdMixers));
# endif
#endif

  Add(ctrl_alpha =
      new cMenuEditTypedIntItem(tr("Dynamic transparency correction"), "%", 
				&newconfig.alpha_correction, -200, 200,
				tr("Off")));
  Add(ctrl_alpha_abs =
      new cMenuEditTypedIntItem(tr("Static transparency correction"), "", 
				&newconfig.alpha_correction_abs, -0xff, 0xff,
				tr("Off")));
  
#if VDRVERSNUM < 10515
  Add(NewTitle(tr("Subtitles")));
  Add(new cMenuEditBoolItem(trVDR("Setup.EPG$Preferred languages"), 
			    &newconfig.spu_autoshow));
  if(newconfig.spu_autoshow) {
    Add(ctrl_spulang0 =
	new cMenuEditStrItem(trVDR("Setup.EPG$Preferred language"),
			     newconfig.spu_lang[0], 4, LangNameChars));
    Add(new cMenuEditStrItem(trVDR("Setup.EPG$Preferred language"),
			     newconfig.spu_lang[1], 4, LangNameChars));
    Add(new cMenuEditStrItem(trVDR("Setup.EPG$Preferred language"),
			     newconfig.spu_lang[2], 4, LangNameChars));
    Add(new cMenuEditStrItem(trVDR("Setup.EPG$Preferred language"),
			     newconfig.spu_lang[3], 4, LangNameChars));
  }
#endif

  Add(new cMenuEditStraI18nItem(tr("External subtitle size"),
				&newconfig.extsub_size, SUBTITLESIZE_count, xc.s_subtitleSizes));

  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  //SetCurrent(Get(1));
  Display();
}

eOSState cMenuSetupOSD::ProcessKey(eKeys Key)
{
  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  Key = NORMALKEY(Key);

  if(Key!=kLeft && Key!=kRight)
    return state;

  if(item == ctrl_scaling)
    cXinelibOsdProvider::RefreshOsd();
  else if(item == ctrl_alpha)
    xc.alpha_correction = newconfig.alpha_correction;
  else if(item == ctrl_alpha_abs)
    xc.alpha_correction_abs = newconfig.alpha_correction_abs;

  if(newconfig.osd_blending==OSD_BLENDING_SOFTWARE && !ctrl_lowres)
    Set();
  if(newconfig.osd_blending!=OSD_BLENDING_SOFTWARE && ctrl_lowres)
    Set();
#if VDRVERSNUM < 10515
  if(newconfig.spu_autoshow && !ctrl_spulang0)
    Set();
  if(!newconfig.spu_autoshow && ctrl_spulang0)
    Set();
#endif
  return state;
}

void cMenuSetupOSD::Store(void)
{
  newconfig.extsub_size --;
  if(newconfig.extsub_size != xc.extsub_size) {
    cString tmp = cString::sprintf("EXTSUBSIZE %d", newconfig.extsub_size);
    cXinelibDevice::Instance().PlayFileCtrl(tmp);
  }

  memcpy(&xc, &newconfig, sizeof(config_t));
  orig_alpha_correction = xc.alpha_correction;
  orig_alpha_correction_abs = xc.alpha_correction_abs;

  SetupStore("OSD.Scaling",         xc.osd_scaling);
  SetupStore("OSD.HideMainMenu",    xc.hide_main_menu);
  SetupStore("OSD.LayersVisible",   xc.osd_mixer);
  SetupStore("OSD.Blending",        xc.osd_blending);
  SetupStore("OSD.BlendingLowRes",  xc.osd_blending_lowresvideo);
#if 1
  // Delete old keys (<=1.0.0)
  SetupStore("OSD.UnscaledAlways");
  SetupStore("OSD.UnscaledLowRes");
  SetupStore("OSD.UnscaledOpaque");
  SetupStore("OSD.Prescale");
  SetupStore("OSD.Downscale");
#endif
  SetupStore("OSD.AlphaCorrection", xc.alpha_correction);
  SetupStore("OSD.AlphaCorrectionAbs", xc.alpha_correction_abs);

  SetupStore("OSD.ExtSubSize", xc.extsub_size);
#if VDRVERSNUM < 10515
  SetupStore("OSD.SpuAutoSelect", xc.spu_autoshow);
  SetupStore("OSD.SpuLang0", xc.spu_lang[0]);
  SetupStore("OSD.SpuLang1", xc.spu_lang[1]);
  SetupStore("OSD.SpuLang2", xc.spu_lang[2]);
  SetupStore("OSD.SpuLang3", xc.spu_lang[3]);
#endif
  Setup.Save();
}


//--- cMenuSetupDecoder ------------------------------------------------------

class cMenuSetupDecoder : public cMenuSetupPage 
{
  private:
    config_t newconfig;

    int pes_buffers_ind;

    cOsdItem *ctrl_pes_buffers_ind;
    cOsdItem *ctrl_pes_buffers;

  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupDecoder(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupDecoder::cMenuSetupDecoder(void)
{
  int i;
  memcpy(&newconfig, &xc, sizeof(config_t));

  pes_buffers_ind = PES_BUFFERS_CUSTOM;
  for(i=0;xc.s_bufferSize[i];i++)
    if(xc.pes_buffers == xc.i_pesBufferSize[i])
      pes_buffers_ind = i;

  Set();
}

void cMenuSetupDecoder::Set(void)
{
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  int current = Current();
  Clear();

  Add(NewTitle(tr("Decoder")));
  Add(ctrl_pes_buffers_ind = 
      new cMenuEditStraI18nItem(tr("Buffer size"), &pes_buffers_ind, 
				PES_BUFFERS_count, xc.s_bufferSize));
  if(pes_buffers_ind == PES_BUFFERS_CUSTOM)
    Add(ctrl_pes_buffers = 
	new cMenuEditIntItem(tr("  Number of PES packets"), &newconfig.pes_buffers, 
			     10, 10000));
  else
    ctrl_pes_buffers = NULL;

  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupDecoder::ProcessKey(eKeys Key)
{
  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  Key = NORMALKEY(Key);

  if(Key!=kLeft && Key!=kRight)
    return state;

  if(item == ctrl_pes_buffers_ind) {
    if(pes_buffers_ind == PES_BUFFERS_CUSTOM && !ctrl_pes_buffers) {
      Set();
    } else if(pes_buffers_ind != PES_BUFFERS_CUSTOM && ctrl_pes_buffers) {
      Set();
    }
  }

  return state;
}

void cMenuSetupDecoder::Store(void)
{
  int old_buffers = xc.pes_buffers;

  //memcpy(&xc, &newconfig, sizeof(config_t));
  xc.pes_buffers = newconfig.pes_buffers;

  if(pes_buffers_ind != PES_BUFFERS_CUSTOM)
    xc.pes_buffers = xc.i_pesBufferSize[pes_buffers_ind];

  SetupStore("Decoder.PesBuffers",    xc.pes_buffers);
#if 1
  // delete old keys (<1.0.0)
  SetupStore("Decoder.Priority");
  SetupStore("Decoder.InactivityTimer");
#endif

  if(xc.pes_buffers != old_buffers)
    cXinelibDevice::Instance().ConfigureDecoder(xc.pes_buffers);
  Setup.Save();
}


//--- cMenuSetupLocal --------------------------------------------------------

class cMenuSetupLocal : public cMenuSetupPage 
{
  private:
    config_t newconfig;

    int local_frontend;
    int local_frontend_orig;
    int audio_driver;
    int audio_driver_orig;
    int video_driver;   
    int video_driver_orig;

    cOsdItem *ctrl_scale;
    cOsdItem *ctrl_local_fe;
    cOsdItem *ctrl_driver;
    cOsdItem *ctrl_fullscreen;
    cOsdItem *ctrl_window_width;
    cOsdItem *ctrl_window_height;
    cOsdItem *ctrl_interlace_order;  
    cOsdItem *ctrl_aspect;
    cOsdItem *ctrl_audio_driver;
    cOsdItem *ctrl_audio_port;

  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupLocal(void);
    ~cMenuSetupLocal(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupLocal::cMenuSetupLocal(void)
{
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));

  memcpy(&newconfig, &xc, sizeof(config_t));

  local_frontend_orig = local_frontend = strstra(xc.local_frontend, xc.s_frontends, 0);
  audio_driver_orig = audio_driver = strstra(xc.audio_driver, xc.s_audioDrivers, 0);

  video_driver = 0;
  if(local_frontend == FRONTEND_X11)
    video_driver = strstra(xc.video_driver, xc.s_videoDriversX11, 0);
  if(local_frontend == FRONTEND_FB)
    video_driver = strstra(xc.video_driver, xc.s_videoDriversFB, 0);
  video_driver_orig = video_driver;

  Set();
}

cMenuSetupLocal::~cMenuSetupLocal(void)
{
  cXinelibDevice::Instance().ConfigureWindow(
       xc.fullscreen, xc.width, xc.height, xc.modeswitch, xc.modeline, 
       xc.display_aspect, xc.scale_video, xc.field_order);
  cXinelibDevice::Instance().ConfigurePostprocessing(
       xc.deinterlace_method, xc.audio_delay, xc.audio_compression, 
       xc.audio_equalizer, xc.audio_surround, xc.speaker_type);
}

void cMenuSetupLocal::Set(void)
{
  int current = Current();
  Clear();

  ctrl_interlace_order = NULL;
  ctrl_fullscreen = NULL;
  ctrl_window_width  = NULL;
  ctrl_window_height = NULL;
  ctrl_driver = NULL;
  ctrl_aspect = NULL;
  ctrl_scale = NULL;
  ctrl_audio_driver = NULL;
  ctrl_audio_port = NULL;

  Add(NewTitle(tr("Local Frontend")));

  Add(ctrl_local_fe = 
      new cMenuEditStraI18nItem(tr("Local Display Frontend"), &local_frontend,
				FRONTEND_count, xc.s_frontendNames));

  if(local_frontend == FRONTEND_X11) {
    Add(new cMenuEditBoolItem(tr("Use keyboard"), 
			      &newconfig.use_x_keyboard));
  }

  if(local_frontend != FRONTEND_NONE) {
    cString tmp = cString::sprintf("%s >>", tr("Decoder"));
    Add(new cOsdItem(tmp, osUser1));
    Add(NewTitle(tr("Video")));
  }

  if(local_frontend == FRONTEND_X11) {
    Add(ctrl_driver =
	new cMenuEditStraI18nItem(tr("Driver"), &video_driver, 
				  X11_DRIVER_count, 
				  xc.s_videoDriverNamesX11));
    Add(new cMenuEditStrItem(tr("Display address"), newconfig.video_port, 
			     31, DriverNameChars));

  } else if(local_frontend == FRONTEND_FB) {
    Add(ctrl_driver =
	new cMenuEditStraI18nItem(tr("Driver"), &video_driver, 
				  FB_DRIVER_count, 
				  xc.s_videoDriverNamesFB));
    Add(new cMenuEditStrItem(tr("Framebuffer device"), newconfig.video_port, 31, 
			     DriverNameChars));
  }
#if 0
  if(local_frontend == FRONTEND_FB || !newconfig.fullscreen) {
    Add(new cMenuEditStrItem( "Modeline", newconfig.modeline, 31, 
			      ModeLineChars));
    Add(new cMenuEditBoolItem("Videomode switching", &xc.modeswitch));
  }
#endif

  if(local_frontend == FRONTEND_X11) {
    Add(ctrl_fullscreen = new cMenuEditBoolItem(tr("Fullscreen mode"), 
						&newconfig.fullscreen));
    if(!newconfig.fullscreen) {
      Add(ctrl_window_width = 
	  new cMenuEditTypedIntItem( tr("  Window width"), tr("px"), 
				     &newconfig.width, 1, 2048));
      Add(ctrl_window_height = 
	  new cMenuEditTypedIntItem( tr("  Window height"), tr("px"), 
				     &newconfig.height, 1, 2048));
    }
  }

  if(local_frontend != FRONTEND_NONE) {
    Add(ctrl_aspect = 
	new cMenuEditStraI18nItem(tr("Window aspect"), &newconfig.display_aspect,
				  ASPECT_count, xc.s_aspects));
    Add(ctrl_scale =
	new cMenuEditBoolItem(tr("Scale to window size"), &newconfig.scale_video));

#ifdef HAVE_XV_FIELD_ORDER
    Add(ctrl_interlace_order = 
	new cMenuEditStraI18nItem(tr("Interlaced Field Order"), 
				  &newconfig.field_order, FIELD_ORDER_count, 
				  xc.s_fieldOrder));
#endif

    Add(NewTitle(tr("Audio")));

    Add(ctrl_audio_driver = 
	new cMenuEditStraI18nItem(tr("Driver"), &audio_driver, 
				  AUDIO_DRIVER_count, xc.s_audioDriverNames));
    if(audio_driver != AUDIO_DRIVER_AUTO && audio_driver != AUDIO_DRIVER_NONE)
      Add(ctrl_audio_port = 
	  new cMenuEditStrItem(tr("Port"), newconfig.audio_port, 31, 
			       DriverNameChars));
  }
  
  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupLocal::ProcessKey(eKeys Key)
{
  int prev_frontend = local_frontend;
  int prev_audio_driver = audio_driver;

  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if(state == osUser1)
    return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupDecoder);

  Key = NORMALKEY(Key);

  if((Key!=kLeft && Key!=kRight) || !item)
    return state;

  if(item == ctrl_audio_driver) {
    if(prev_audio_driver != audio_driver) {
     
      if(audio_driver == audio_driver_orig)
        strcpy(newconfig.audio_port, xc.audio_port);
      else if(audio_driver == AUDIO_DRIVER_ALSA)
        strcpy(newconfig.audio_port, "default");
      else if(audio_driver == AUDIO_DRIVER_OSS)
        strcpy(newconfig.audio_port, "/dev/dsp");
      else
        strcpy(newconfig.audio_port, "");
      Set();
    }
    else if((audio_driver != AUDIO_DRIVER_AUTO && 
	     audio_driver != AUDIO_DRIVER_NONE) && 
	    !ctrl_audio_port)
      Set();
    else if((audio_driver == AUDIO_DRIVER_AUTO || 
	     audio_driver == AUDIO_DRIVER_NONE) && 
	    ctrl_audio_port)
      Set();
  }
  else if(item == ctrl_aspect || item == ctrl_scale || item == ctrl_interlace_order)
    cXinelibDevice::Instance().ConfigureWindow(
	xc.fullscreen, xc.width, xc.height, xc.modeswitch, xc.modeline, 
	newconfig.display_aspect, newconfig.scale_video, 
	newconfig.field_order);
  else if(item == ctrl_local_fe && local_frontend != prev_frontend) {

    if(local_frontend == local_frontend_orig) {
      video_driver = video_driver_orig;
      strcpy(newconfig.video_port, xc.video_port);
    }
    else if(local_frontend == FRONTEND_FB)
      strcpy(newconfig.video_port, "/dev/fb/0");
    else if(local_frontend == FRONTEND_X11)
      strcpy(newconfig.video_port, "0.0");

    Set();
  }
  else if(item == ctrl_fullscreen) {
    if(!newconfig.fullscreen && !ctrl_window_width) {
      Set();
    } else if(newconfig.fullscreen && ctrl_window_width) {
      Set();
    }
  }

  return state;
}

void cMenuSetupLocal::Store(void)
{
  int old_buffers = xc.pes_buffers;

  memcpy(&xc, &newconfig, sizeof(config_t));

  xc.pes_buffers = old_buffers;

  strn0cpy(xc.audio_driver, xc.s_audioDrivers[audio_driver], sizeof(xc.audio_driver));
  strn0cpy(xc.local_frontend, xc.s_frontends[local_frontend], sizeof(xc.local_frontend));
  if(local_frontend == FRONTEND_X11)
    strn0cpy(xc.video_driver, xc.s_videoDriversX11[video_driver], sizeof(xc.video_driver));
  if(local_frontend == FRONTEND_FB)
    strn0cpy(xc.video_driver, xc.s_videoDriversFB[video_driver], sizeof(xc.video_driver));

  SetupStore("Frontend",      xc.local_frontend);
  SetupStore("Audio.Driver",  xc.audio_driver);
  SetupStore("Audio.Port",    xc.audio_port);
  SetupStore("Video.Driver",  xc.video_driver);
  SetupStore("Video.Port",    xc.video_port);
#if 0
  SetupStore("Video.Port",    NULL); /* should delete entry ? */
  SetupStore("Video.Port.X11",xc.video_port_x11);
  SetupStore("Video.Port.FB", xc.video_port_fb);
#endif
  SetupStore("Video.Scale",   xc.scale_video);
  SetupStore("Video.FieldOrder", xc.field_order);
  SetupStore("Modeline",      xc.modeline);
  SetupStore("VideoModeSwitching", xc.modeswitch);
  SetupStore("Fullscreen",    xc.fullscreen);
  SetupStore("DisplayAspect", xc.s_aspects[xc.display_aspect]);
  SetupStore("X11.WindowWidth",  xc.width);
  SetupStore("X11.WindowHeight", xc.height);
  SetupStore("X11.UseKeyboard",  xc.use_x_keyboard);
  Setup.Save();
}

//--- cMenuSetupRemote -------------------------------------------------------

class cMenuSetupRemote : public cMenuSetupPage 
{
  private:
    config_t newconfig;

    cOsdItem *ctrl_remote_mode;
    cOsdItem *ctrl_usertp;
    cOsdItem *ctrl_rtp_addr;
    cOsdItem *ctrl_use_http;
    cOsdItem *ctrl_http_ctrl;
    cOsdItem *ctrl_use_rtsp;
    cOsdItem *ctrl_rtsp_ctrl;

  protected:
    virtual void Store(void);
    void Set(void);
  
  public:
    cMenuSetupRemote(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupRemote::cMenuSetupRemote(void)
{
  memcpy(&newconfig, &xc, sizeof(config_t));
  Set();
}

void cMenuSetupRemote::Set(void)
{
  int current = Current();
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  Clear();

  Add(NewTitle(tr("Remote Clients")));
  Add(ctrl_remote_mode = new cMenuEditBoolItem(tr("Allow remote clients"), 
					       &newconfig.remote_mode));
  ctrl_usertp = NULL;
  ctrl_rtp_addr = NULL;
  ctrl_use_http = NULL;
  ctrl_use_rtsp = NULL;
  ctrl_http_ctrl = NULL;
  ctrl_rtsp_ctrl = NULL;
  if(newconfig.remote_mode) {
    Add(new cMenuEditIntItem( tr("  Listen port (TCP and broadcast)"), 
			      &newconfig.listen_port,
			      0, 0xffff));
    Add(new cMenuEditStrItem( tr("  Listen address"), 
			      &newconfig.remote_local_ip[0], 16, "0123456789."));
    Add(new cMenuEditBoolItem(tr("  Remote keyboard"), 
			      &newconfig.remote_keyboard));

    Add(new cMenuEditBoolItem(tr("  PIPE transport"), 
			      &newconfig.remote_usepipe));
    Add(new cMenuEditBoolItem(tr("  TCP transport"), 
			      &newconfig.remote_usetcp));
    Add(new cMenuEditBoolItem(tr("  UDP transport"), 
			      &newconfig.remote_useudp));
    Add(ctrl_usertp = 
	new cMenuEditBoolItem(tr("  RTP (multicast) transport"), 
			      &newconfig.remote_usertp));
    if(newconfig.remote_usertp) {
      Add(ctrl_rtp_addr =
	  new cMenuEditStrItem( tr("    Address"), 
			        &newconfig.remote_rtp_addr[0], 16, "0123456789."));
      Add(new cMenuEditOddIntItem( tr("    Port"), 
				&newconfig.remote_rtp_port, 1000, 0xfffe));
      Add(new cMenuEditIntItem( tr("    TTL"), 
				&newconfig.remote_rtp_ttl, 1, 10));
      Add(new cMenuEditBoolItem(tr("    Transmit always on"), 
				&newconfig.remote_rtp_always_on));
      Add(new cMenuEditBoolItem(tr("    SAP announcements"), 
				&newconfig.remote_rtp_sap));
    }
    Add(new cMenuEditBoolItem(tr("  Server announce broadcasts"), 
			      &newconfig.remote_usebcast));

    Add(new cMenuEditBoolItem(tr("  HTTP transport for media files"), 
			      &newconfig.remote_http_files));

    Add(NewTitle(tr("Additional network services")));
    Add(ctrl_use_http =
	new cMenuEditBoolItem(tr("HTTP server"),
			      &newconfig.remote_use_http));
    if(newconfig.remote_use_http)
      Add(ctrl_http_ctrl = 
	  new cMenuEditBoolItem(tr("HTTP clients can control VDR"), 
				&newconfig.remote_use_http_ctrl));
    Add(ctrl_use_rtsp =
	new cMenuEditBoolItem(tr("RTSP server"),
			      &newconfig.remote_use_rtsp));
    if(newconfig.remote_use_rtsp)
      Add(ctrl_rtsp_ctrl = 
	  new cMenuEditBoolItem(tr("RTSP clients can control VDR"), 
				&newconfig.remote_use_rtsp_ctrl));
  }

  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupRemote::ProcessKey(eKeys Key)
{
  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  Key = NORMALKEY(Key);

  if(Key!=kLeft && Key!=kRight)
    return state;

  if(item == ctrl_remote_mode) {
    if(newconfig.remote_mode && !ctrl_usertp) {
      Set();
    } else if(!newconfig.remote_mode && ctrl_usertp) {
      Set();
    }
  }
  if(item == ctrl_usertp) {
    if(newconfig.remote_usertp && !ctrl_rtp_addr) {
      Set();
    } else if(!newconfig.remote_usertp && ctrl_rtp_addr) {
      Set();
    }
  }
  if(item == ctrl_use_http) {
    if(newconfig.remote_use_http && !ctrl_http_ctrl) {
      Set();
    } else if(!newconfig.remote_use_http && ctrl_http_ctrl) {
      Set();
    }
  }
  if(item == ctrl_use_rtsp) {
    if(newconfig.remote_use_rtsp && !ctrl_rtsp_ctrl) {
      Set();
    } else if(!newconfig.remote_use_rtsp && ctrl_rtsp_ctrl) {
      Set();
    }
  }

  return state;
}

void cMenuSetupRemote::Store(void)
{
  memcpy(&xc, &newconfig, sizeof(config_t));

  SetupStore("RemoteMode",        xc.remote_mode);
  SetupStore("Remote.ListenPort", xc.listen_port);
  SetupStore("Remote.Iface",      xc.remote_local_if);
  SetupStore("Remote.LocalIP",    xc.remote_local_ip);
  SetupStore("Remote.Keyboard",   xc.remote_keyboard);

  SetupStore("Remote.UsePipe",xc.remote_usepipe);
  SetupStore("Remote.UseTcp", xc.remote_usetcp);
  SetupStore("Remote.UseUdp", xc.remote_useudp);
  SetupStore("Remote.UseRtp", xc.remote_usertp);
  SetupStore("Remote.UseBroadcast", xc.remote_usebcast);

  SetupStore("Remote.UseHttp", xc.remote_http_files);

  SetupStore("Remote.Rtp.Address",  xc.remote_rtp_addr);
  SetupStore("Remote.Rtp.Port",     xc.remote_rtp_port);
  SetupStore("Remote.Rtp.TTL",      xc.remote_rtp_ttl);
  SetupStore("Remote.Rtp.AlwaysOn", xc.remote_rtp_always_on);
  SetupStore("Remote.Rtp.SapAnnouncements", xc.remote_rtp_sap);

  SetupStore("Remote.AllowRtsp", xc.remote_use_rtsp);
  SetupStore("Remote.AllowRtspCtrl", xc.remote_use_rtsp_ctrl);
  SetupStore("Remote.AllowHttp", xc.remote_use_http);
  SetupStore("Remote.AllowHttpCtrl", xc.remote_use_http_ctrl);

  cXinelibDevice::Instance().Listen(xc.remote_mode, xc.listen_port);
  Setup.Save();
}

//--- cMenuSetupMediaPlayer --------------------------------------------------------

class cMenuSetupMediaPlayer : public cMenuSetupPage
{
  private:
    config_t newconfig;

    cOsdItem *media_ctrl_playlist_tracknumber;
    cOsdItem *media_ctrl_playlist_artist;
    cOsdItem *media_ctrl_playlist_album;
    cOsdItem *media_ctrl_playlist_cache;
    cOsdItem *media_ctrl_playlist_id3scanner;

  protected:
    virtual void Store(void);
    void Set(void);

  public:
    cMenuSetupMediaPlayer(void);

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupMediaPlayer::cMenuSetupMediaPlayer(void)
{
  memcpy(&newconfig, &xc, sizeof(config_t));
  Set();
}

void cMenuSetupMediaPlayer::Set(void)
{
  SetPlugin(cPluginManager::GetPlugin(PLUGIN_NAME_I18N));
  int current = Current();
  Clear();

  Add(NewTitle(tr("Playlist settings")));

  Add(media_ctrl_playlist_tracknumber =
      new cMenuEditBoolItem(tr("Show the track number"),
                            &newconfig.playlist_tracknumber));

  Add(media_ctrl_playlist_artist =
      new cMenuEditBoolItem(tr("Show the name of the artist"),
                            &newconfig.playlist_artist));
  
  Add(media_ctrl_playlist_album =
      new cMenuEditBoolItem(tr("Show the name of the album"),
                            &newconfig.playlist_album));

  Add(media_ctrl_playlist_id3scanner =
      new cMenuEditBoolItem(tr("Scan for metainfo"),
                            &newconfig.enable_id3_scanner));

  Add(media_ctrl_playlist_cache =
      new cMenuEditBoolItem(tr("Cache metainfo"),
                            &newconfig.cache_implicit_playlists));

  
  if(current<1) current=1; /* first item is not selectable */
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupMediaPlayer::ProcessKey(eKeys Key)
{ 
  eOSState state = cMenuSetupPage::ProcessKey(Key);
  return state;
}

void cMenuSetupMediaPlayer::Store(void)
{ 
  memcpy(&xc, &newconfig, sizeof(config_t));
  
  SetupStore("Playlist.Tracknumber", xc.playlist_tracknumber);
  SetupStore("Playlist.Album", xc.playlist_album);
  SetupStore("Playlist.Artist", xc.playlist_artist);
  SetupStore("Media.CacheImplicitPlaylists", xc.cache_implicit_playlists);
  SetupStore("Media.EnableID3Scanner", xc.enable_id3_scanner);
  Setup.Save();
}

} // namespace


//--- cMenuTestImages ------------------------------------------------------

#include <vdr/osdbase.h>

#define OSD_W (720-2)
#define OSD_H (576-2)
#define OSD_X (1)
#define OSD_Y (1)

//
// cTestGrayscale
//

class cTestGrayscale : public cOsdObject 
{
  private:
    cOsd *m_Osd;

  public:
    cTestGrayscale() { m_Osd = NULL; }
    virtual ~cTestGrayscale() { delete m_Osd; }

    virtual void Show();
    virtual eOSState ProcessKey(eKeys Key);
};

void cTestGrayscale::Show()
{
  tArea areas [] = { {      0, 0, OSD_W/2 - 1, OSD_H - 1, 8},
                     {OSD_W/2, 0,   OSD_W - 1, OSD_H - 1, 8}};
  int i;

  if(!m_Osd)
#if VDRVERSNUM >= 10509
    m_Osd = cOsdProvider::NewOsd(OSD_X, OSD_Y, 0);
#else
    m_Osd = cOsdProvider::NewOsd(OSD_X, OSD_Y);
#endif

  if(m_Osd) {
    if (m_Osd->CanHandleAreas(areas, sizeof(areas) / sizeof(tArea) ) == oeOk) {
      m_Osd->SetAreas(areas, sizeof(areas) / sizeof(tArea));
      m_Osd->Flush();

      // border
      m_Osd->DrawRectangle(0, 0, OSD_W - 1, OSD_H - 1, 0xff000000);
      m_Osd->DrawRectangle(1, 1, OSD_W - 2, OSD_H - 2, 0xff000000);

      // background
      m_Osd->DrawRectangle(2, 2, 2+103, OSD_H - 3, 0xffffffff); 
      m_Osd->DrawRectangle(OSD_W-2-103, 2, OSD_W-2, OSD_H - 3, 0xff000000); 

      for(i=0; i<0xff; i++)
	m_Osd->DrawRectangle(2+103+2*i, 2, 2+103+2*(i+1), OSD_H - 3, 
			     0xff000000|(i*0x00010101)/*=(i<<16)|(i<<8)|(i)*/);
      // line
      m_Osd->DrawRectangle(1, OSD_H/2-20, OSD_W - 2, OSD_H/2, 0xffffffff);
      m_Osd->DrawRectangle(1, OSD_H/2+1, OSD_W - 2, OSD_H/2+21, 0xff000000);

      // Cross
      for(int x=0; x<OSD_W;x++) {
	m_Osd->DrawPixel(x, x*OSD_H/OSD_W, 0x00000000);
	m_Osd->DrawPixel(x, OSD_H - 1 - x*OSD_H/OSD_W, 0x00000000);
      }

      // commit
      m_Osd->Flush();
    }

  }
}

eOSState cTestGrayscale::ProcessKey(eKeys key)
{
  char s[32];
  static int br = xc.brightness;
  static int co = xc.contrast;
  eOSState state = cOsdObject::ProcessKey(key);
  if (state == osUnknown) {
    switch (key & ~k_Repeat) {
      case kOk:
      case kBack:
	return osEnd;
      case kRight:
	br += 0xffff/1024*2;
      case kLeft:
	br -= 0xffff/1024;
	sprintf(s, "b %d", br);
	m_Osd->DrawText(400, 100, s, 0xff000000, 0xffffffff, cFont::GetFont(fontSml));
	cXinelibDevice::Instance().ConfigureVideo(xc.hue, xc.saturation, br, co, xc.overscan, xc.vo_aspect_ratio);
	m_Osd->Flush();
	return osContinue;	
      case kUp:
	co += 0xffff/1024*2;
      case kDown:
	co -= 0xffff/1024;
	sprintf(s, "c %d", co);
	m_Osd->DrawText(400, 130, s, 0xff000000, 0xffffffff, cFont::GetFont(fontSml));
	cXinelibDevice::Instance().ConfigureVideo(xc.hue, xc.saturation, br, co, xc.overscan, xc.vo_aspect_ratio);
	m_Osd->Flush();
	return osContinue;
    }
  }
  return state;
}


//
// cTestBitmap
//

class cTestBitmap : public cOsdObject 
{
  private:
    cOsd *m_Osd;
    int bpp;

  public:
    cTestBitmap(int _bpp = 1) { 
      m_Osd = NULL; 
      if(_bpp<1) _bpp = 1; 
      if(_bpp>6) _bpp = 6; 
      bpp = 1<<_bpp; 
    }
    virtual ~cTestBitmap() { delete m_Osd; }

    virtual void Show();
    virtual eOSState ProcessKey(eKeys Key);
};

void cTestBitmap::Show()
{
  tArea areas [] = {{ 0, 0, OSD_W - 1, OSD_H - 1, 8}};
  int x, y, bit = 0;

  if(!m_Osd) {
#if VDRVERSNUM >= 10509
    m_Osd = cOsdProvider::NewOsd(OSD_X, OSD_Y, 0);
#else
    m_Osd = cOsdProvider::NewOsd(OSD_X, OSD_Y);
#endif

    if(m_Osd) {
      if (m_Osd->CanHandleAreas(areas, sizeof(areas) / sizeof(tArea) ) == oeOk) {
	m_Osd->SetAreas(areas, sizeof(areas) / sizeof(tArea));
	m_Osd->Flush();
      }
    }      
  }
   
  if(m_Osd) {
    for(x=0; x<OSD_W; x+=bpp) {
      bit = (x/bpp) & 1;
      for(y=0; y<OSD_H; y+=bpp) {
	m_Osd->DrawRectangle(x, y, x+bpp, y+bpp, bit?0xffffffff:0xff000000);
	bit = !bit;
      }
    }
    // commit
    m_Osd->Flush();
  }
}

eOSState cTestBitmap::ProcessKey(eKeys key)
{
  eOSState state = cOsdObject::ProcessKey(key);

  if (state == osUnknown) {
    switch (key & ~k_Repeat) {
      case kOk:
      case kBack:
	return osEnd;
      case kRight:
	bpp = (bpp<64) ? (bpp<<1) : 1;
	Show();
	return osContinue;
      case kLeft:
	bpp = (bpp>1) ? (bpp>>1) : 64;
	Show();
	return osContinue;	
      default:
	break;
    }
  }
  return state;
}

//
// cMenuTestImages
//

#include <vdr/remote.h>   // CallPlugin

extern cOsdObject *g_PendingMenuAction;

class cMenuTestImages : public cMenuSetupPage {

  protected:
    void Set(void);
    virtual void Store(void) {};
 
  public:
    cMenuTestImages();

    virtual eOSState ProcessKey(eKeys Key);
};

cMenuTestImages::cMenuTestImages() 
{ 
  Set();
}

void cMenuTestImages::Set(void)
{
  char buf[128];
  Clear();
  
  SetHasHotkeys();
  Add(new cOsdItem(tr("Grayscale"), osUser1));

  snprintf(buf, sizeof(buf), "%s 1bit", tr("Bitmap"));
  buf[sizeof(buf)-1] = 0;
  Add(new cOsdItem(buf, osUser2));

  snprintf(buf, sizeof(buf), "%s 4bit", tr("Bitmap"));
  buf[sizeof(buf)-1] = 0;
  Add(new cOsdItem(buf, osUser3));
      
  Display();
}

eOSState cMenuTestImages::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  switch (state) {
    case osUser1:
      if(cRemote::CallPlugin("xineliboutput"))
	g_PendingMenuAction = new cTestGrayscale();
      return osEnd;
    case osUser2:
      if(cRemote::CallPlugin("xineliboutput"))
	g_PendingMenuAction = new cTestBitmap(1);
      return osEnd;
    case osUser3:
      if(cRemote::CallPlugin("xineliboutput"))
	g_PendingMenuAction = new cTestBitmap(4);
      return osEnd;
    default: ;
  }

  return state;
}

//--- cMenuSetupXinelib ------------------------------------------------------

cMenuSetupXinelib::cMenuSetupXinelib(void)
{
  XinelibOutputSetupMenu::controls[0] = tr("Off");
  Set();
}

void cMenuSetupXinelib::Set(void)
{
  Clear();

  SetHasHotkeys();
  Add(new cOsdItem(hk(tr("Audio")),          osUser1));
  Add(new cOsdItem(hk(tr("Audio Equalizer")),osUser2));
  Add(new cOsdItem(hk(tr("Video")),          osUser3));
  Add(new cOsdItem(hk(tr("OSD")),            osUser4));
  //Add(new cOsdItem(hk(tr("Decoder")),        osUser5));
  Add(new cOsdItem(hk(tr("Media Player")),   osUser5));
  Add(new cOsdItem(hk(tr("Local Frontend")), osUser6));
  Add(new cOsdItem(hk(tr("Remote Clients")), osUser7));
  Add(new cOsdItem(hk(tr("Test Images")),    osUser8));

  Display();
}

eOSState cMenuSetupXinelib::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  switch (state) {
    case osUser1: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupAudio);
    case osUser2: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupAudioEq);
    case osUser3: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupVideo);
    case osUser4: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupOSD);
    case osUser5: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupMediaPlayer);
    case osUser6: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupLocal);
    case osUser7: 
      return AddSubMenu(new XinelibOutputSetupMenu::cMenuSetupRemote);
    case osUser8: 
      return AddSubMenu(new cMenuTestImages);

    default: ;
  }

  return state;
}


