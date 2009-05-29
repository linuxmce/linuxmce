/*
 * frontend.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend.h,v 1.22.2.1 2008/12/10 22:56:15 phintuka Exp $
 *
 */

#ifndef __XINELIB_FRONTEND_H
#define __XINELIB_FRONTEND_H

#include <vdr/tools.h>
#include <vdr/thread.h>
#include <vdr/device.h> // ePlayMode

class cStatus;

//----------------------------- cXinelibThread --------------------------------

class cXinelibThread : public cThread, public cListObject 
{
  private:
    cXinelibThread(cXinelibThread&); // no copy contructor

  public:
    cXinelibThread(const char *Description = NULL);
    virtual ~cXinelibThread();

  //
  // Thread control
  //

  public:
    virtual void Start(void);
    virtual void Stop(void);
    bool IsReady(void);
    bool IsFinished(void);

  protected:
    void SetStopSignal(void);
    bool GetStopSignal(void);

    virtual void Action(void) = 0;

  //
  // Playback control
  //

  public:
    void PauseOutput(void)  { TrickSpeed(0); }
    void ResumeOutput(void) { TrickSpeed(1); }
    virtual void TrickSpeed(int Speed);
    void SetVolume(int NewVolume);
    void SetLiveMode(bool);
    void SetStillMode(bool);
    void SetNoVideo(bool bVal);
    void AudioStreamChanged(bool ac3, int StreamId);
#if VDRVERSNUM < 10515
    void SpuStreamChanged(int StreamId);
#else
    void SetSubtitleTrack(eTrackType Track);
#endif

  protected:
    int  Xine_Control(const char *cmd, const char *p1);
    int  Xine_Control(const char *cmd, int p1);
    int  Xine_Control(const char *cmd, int64_t p1);
    virtual int  Xine_Control(const char *cmd) = 0;
    virtual int  Xine_Control_Sync(const char *cmd) { return Xine_Control(cmd); }

    void Configure(void);

  //
  // Data transfer
  //

  public:
    virtual int  Poll(cPoller &Poller, int TimeoutMs);
    virtual bool Flush(int TimeoutMs);
    virtual void Clear(void);
    virtual int  Play_PES(const uchar *buf, int len);
    virtual void OsdCmd(void *cmd) = 0;
    virtual int64_t GetSTC(void) { return -1; }
    virtual void SetHDMode(bool On) { (void)Xine_Control("HDMODE",On?1:0); };

    // Stream type conversions
    int  Play_Mpeg1_PES(const uchar *data, int len);
    bool Play_Mpeg2_ES(const uchar *data, int len, int streamID);

    // Built-in still images
    bool BlankDisplay(void);
    bool QueueBlankDisplay(void);
    bool LogoDisplay(void);
    bool NoSignalDisplay(void);

    // Playback files
    virtual bool PlayFile(const char *FileName, int Position, 
			  bool LoopPlay = false, ePlayMode PlayMode = pmAudioVideo);
    virtual int  PlayFileCtrl(const char *Cmd, int TimeoutMs = -1) { return Xine_Control(Cmd); }
    virtual bool EndOfStreamReached(void);

    // Image grabbing
    virtual uchar *GrabImage(int &Size, bool Jpeg, int Quality, 
			     int SizeX, int SizeY) { return NULL; }

    // Control from frontend
    static void KeypressHandler(const char *keymap, const char *key, 
				bool repeat, bool release);
    static void InfoHandler(const char *info);

  //
  // Configuration
  //

  public:
    virtual int ConfigurePostprocessing(const char *deinterlace_method, 
					int audio_delay, 
					int audio_compression, 
					const int *audio_equalizer,
					int audio_surround,
					int speaker_type);
    virtual int ConfigurePostprocessing(const char *name, bool on, const char *args);
    virtual int ConfigureVideo(int hue, int saturation, 
			       int brightness, int contrast,
			       int overscan, int vo_aspect_ratio);
    // Local frontend:
    virtual void ConfigureWindow(int fullscreen, int width, int height, 
				 int modeswitch, const char *modeline, 
				 int aspect, int scale_video, 
				 int field_order) {};
    virtual void ConfigureDecoder(int pes_buffers) {};
    // Remote frontend server:
    virtual bool Listen(int port) { return false; }

  //
  // Data
  //

  protected:
    bool m_bStopThread;
    bool m_bReady;
    bool m_bIsFinished;
    bool m_bNoVideo;
    bool m_bLiveMode;
    bool m_bEndOfStreamReached;
    bool m_bPlayingFile;
    int  m_Volume;
    cString  m_FileName;
    uint64_t m_StreamPos;
    uint32_t m_Frames;

    cStatus *m_StatusMonitor;
    bool     m_SpuLangAuto;
};


#endif // __XINELIB_FRONTEND_H
