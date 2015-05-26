/*
 * frontend.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend.h,v 1.45 2012-03-19 11:57:18 phintuka Exp $
 *
 */

#ifndef __XINELIB_FRONTEND_H
#define __XINELIB_FRONTEND_H

#include <vdr/tools.h>
#include <vdr/thread.h>

#include "xine_input_vdr_net.h" // eStreamId

class cStatus;
class cXinelibDevice;
enum ePlayMode;

//----------------------------- cXinelibThread --------------------------------

class cXinelibThread : public cThread, public cListObject 
{
  private:
    cXinelibThread(cXinelibThread&); // no copy contructor

  public:
    cXinelibThread(cXinelibDevice *Dev, const char *Description = NULL);
    virtual ~cXinelibThread();

  //
  // Thread control
  //

  public:
    bool IsReady(void);

  //
  // Playback control
  //

  public:
    void PauseOutput(void)  { TrickSpeed(0); }
    void ResumeOutput(void) { TrickSpeed(1); }
    void TrickSpeed(int Speed) { TrickSpeed(Speed, false); }
    void SetVolume(int NewVolume);
    void SetLiveMode(bool);
    void SetStillMode(bool);
    void SetNoVideo(bool bVal);
    void AudioStreamChanged(bool ac3, int StreamId);
    void SetSubtitleTrack(eTrackType Track);

    virtual void TrickSpeed(int Speed, bool Backwards);

    // Sync(): wait until all pending control messages have been processed by the client
    virtual void Sync(void) { Xine_Control("SYNC"); };

    void Pip_Config(int Index, int X = -1, int Y = -1, int W = -1, int H = -1);
    void Pip_Close(int Index);

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
    virtual int     Poll(cPoller &Poller, int TimeoutMs);
    virtual bool    Flush(int TimeoutMs);
    virtual void    Clear(void);
    virtual int     Play(const uchar *buf, int len, eStreamId StreamId = sidVdr);
    virtual void    OsdCmd(void *cmd) = 0;
    virtual int64_t GetSTC(void) { return -1; }
    virtual void    SetHDMode(bool On) { (void)Xine_Control("HDMODE", m_bHDMode = On); };
    virtual void    SetHeader(const uchar *data, int length, bool reset = false) {};

    // Stream type conversions
    int     Play_Mpeg1_PES(const uchar *data, int len);
    bool    Play_Mpeg2_ES(const uchar *data, int len, int streamID, bool h264 = false);

    // Built-in still images
    bool BlankDisplay(void);
    bool QueueBlankDisplay(void);
    bool LogoDisplay(void);
    bool NoSignalDisplay(void);

    // Playback files
    virtual bool PlayFile(const char *FileName, int Position = 0, 
			  bool LoopPlay = false, ePlayMode PlayMode = pmAudioVideo,
			  int TimeoutMs = -1);
    virtual int  PlayFileCtrl(const char *Cmd, int TimeoutMs=-1) { return Xine_Control(Cmd); }
    virtual bool EndOfStreamReached(void);

    // Image grabbing
    virtual uchar *GrabImage(int &Size, bool Jpeg, int Quality, 
			     int SizeX, int SizeY) { return NULL; }

    // Control from frontend
    void KeypressHandler(const char *keymap, const char *key,
                         bool repeat, bool release);
    void InfoHandler(const char *info);

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
			       int brightness, int sharpness, int noise_reduction, int contrast,
			       int overscan, int vo_aspect_ratio);
    // Local frontend:
    virtual void ConfigureWindow(int fullscreen, int width, int height, 
				 int modeswitch, const char *modeline, 
				 int aspect, int scale_video) {};
    virtual void ConfigureDecoder(int pes_buffers) {};
    // Remote frontend server:
    virtual bool Listen(int port) { return false; }

    virtual int  SupportsTrueColorOSD(void) { return 0; } /* 0: no, 1: yes, -1: unknown */

  //
  // Data
  //

  protected:
    cXinelibDevice *m_Dev;

    bool m_bReady;
    bool m_bNoVideo;
    bool m_bLiveMode;
    bool m_bHDMode;
    int  m_TrickSpeed;
    bool m_bTrickSpeedBack;
    bool m_bEndOfStreamReached;
    bool m_bPlayingFile;
    int  m_Volume;
    cString  m_FileName;
    uint64_t m_StreamPos;
    uint64_t m_LastClearPos;
    uint32_t m_Frames;

    cStatus *m_StatusMonitor;
    bool     m_SpuLangAuto;
};


#endif // __XINELIB_FRONTEND_H
