/*
 * device.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: device.h,v 1.66 2014-01-07 16:03:54 rofafor Exp $
 *
 */

#ifndef __XINELIB_DEVICE_H
#define __XINELIB_DEVICE_H

#include <vdr/config.h>
#include <vdr/device.h>
#include <vdr/tools.h>
#include <vdr/thread.h>

class cXinelibStatusMonitor;
class cXinelibThread;
class cChannel;
class cFunctor;

struct video_size_s;
struct ts_state_s;

typedef enum {
  miTitle        = 0,
  miTracknumber  = 1,
  miArtist       = 2,
  miAlbum        = 3,
  miDvdTitleNo   = 4,
  miDvdButtons   = 5,
  mi_Count       = 6
} eMetainfoType;

#define ttXSubtitleNone  (-2)
#define ttXSubtitleAuto  (-1)

#define MAX_METAINFO_LEN 63
#define MAX_NUM_PIP      16

class cXinelibDevice : public cDevice
{

  // Singleton

  private:
    static cXinelibDevice* m_pInstance; // singleton
    cXinelibDevice();                   //
    cXinelibDevice(cXinelibDevice&);    // no copy constructor

  public:
    static cXinelibDevice& Instance(void); // singleton
    static void Dispose(void);

    virtual ~cXinelibDevice();

  // device start/stop (from cPlugin)

  public:
    bool InitDevice(void);
    bool StartDevice(void);
    void StopDevice(void);

  // function calls waiting to be executed in VDR main thread context

  private:
    cList<cFunctor> m_MainThreadFunctors;
    cMutex m_MainThreadLock;

  public:
    void MainThreadHook(void);

  // Primary device switching

  private:
    int m_OriginalPrimaryDevice;
    int m_ForcePrimaryDeviceCnt;

    void ForcePrimaryDeviceImpl(bool On);

  public:
    virtual void MakePrimaryDevice(bool On);
    bool ForcePrimaryDevice(bool On);

  // Device capabilities

  public:
    virtual bool HasDecoder(void) const { return true; };
    virtual bool CanReplay(void) const { return true; };
    virtual bool HasIBPTrickSpeed(void) { return xc.ibp_trickspeed; }
    virtual cRect CanScaleVideo(const cRect &Rect, int Alignment = taCenter);

    bool SupportsTrueColorOSD(void);

  // Playback control

  private:
    ePlayMode m_PlayMode;
    int       m_TrickSpeed;
    int       m_TrickSpeedMode;
    cRect     m_VideoWindow;

  public:
    virtual bool SetPlayMode(ePlayMode PlayMode);
    ePlayMode GetPlayMode(void) const { return m_PlayMode; };

    const cRect& GetVideoWindow(void) { return m_VideoWindow; }

  protected:
    virtual void    Clear(void);
    virtual void    Play(void);
    virtual void    TrickSpeed(int Speed, bool Forward) { TrickSpeed(Speed); }
    virtual void    TrickSpeed(int Speed);
    virtual void    Freeze(void);
    virtual bool    Flush(int TimeoutMs = 0);
    virtual int64_t GetSTC(void);
    virtual void    ScaleVideo(const cRect &Rect = cRect::Null);

  // Video format facilities

  public:
    virtual void SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat);
    virtual void SetVideoFormat(bool VideoFormat16_9);
    virtual eVideoSystem GetVideoSystem(void);

    struct video_size_s *m_VideoSize;
    struct ts_state_s   *m_tssVideoSize;
    virtual void GetVideoSize(int &Width, int &Height, double &VideoAspect);
    virtual void GetOsdSize(int &Width, int &Height, double &PixelAspect);

  // Track facilities

  protected:
    virtual void SetAudioTrackDevice(eTrackType Type);
  public:
    virtual void SetSubtitleTrackDevice(eTrackType Type);

  // Audio facilities

  private:
    int        m_AudioChannel;

  protected:
    virtual int  GetAudioChannelDevice(void) { return m_AudioChannel; }
    virtual void SetAudioChannelDevice(int AudioChannel);
    virtual void SetVolumeDevice(int Volume);
    virtual void SetDigitalAudioDevice(bool On);

  // Image grabbing

  public:
    virtual uchar *GrabImage(int &Size, bool Jpeg = true,
                             int Quality = -1, int SizeX = -1, int SizeY = -1);

  // SPU decoder

  private:
    cSpuDecoder *m_spuDecoder;

    friend class cXineSpuDecoder;

  public:
    virtual cSpuDecoder *GetSpuDecoder(void);

  // Messages from StatusMonitor:

  private:
    cXinelibStatusMonitor *m_statusMonitor;
    bool m_liveMode;

  public:
    void SetTvMode(cChannel *Channel);
    void SetReplayMode(void);
    void StopOutput(void);

  // Osd Commands (from cXinelibOsd)

  public:
    void OsdCmd(void *cmd);

  // Configuration

  private:
    cList<cXinelibThread>  m_clients;
    cXinelibThread        *m_server;
    cXinelibThread        *m_local;

  public:
    void ConfigurePostprocessing(const char *deinterlace_method,
                                 int audio_delay,
                                 int audio_compression,
                                 const int *audio_equalizer,
                                 int audio_surround,
                                 int speaker_type);
    void ConfigurePostprocessing(const char *name, bool on = true,
                                 const char *args = NULL);
    void ConfigureVideo(int hue, int saturation, int brightness, int sharpness,
                        int noise_reduction, int contrast, int overscan, int vo_aspect_ratio);
    // local mode:
    void ConfigureWindow(int fullscreen, int width, int height,
                         int modeswitch, const char *modeline,
                         int aspect, int scale_video);
    void ConfigureDecoder(int pes_buffers);
    // remote mode:
    void Listen(bool activate, int port);

  // File playback

  private:
    ePlayMode m_PlayingFile;

  public:
    bool PlayFile(const char *Filename, int Position = 0,
                  bool LoopPlay = false, ePlayMode PlayMode = pmAudioVideo,
                  int TimeoutMs = -1);
    int  PlayFileCtrl(const char *Cmd, int TimeoutMs = -1);
    bool EndOfStreamReached(void);

  // Metainfo cache

  private:
    char     m_MetaInfo[mi_Count][MAX_METAINFO_LEN+1];

  public:
    const char *GetMetaInfo(eMetainfoType Type);
    void        SetMetaInfo(eMetainfoType Type, const char *Value);

  // Stream data

  private:
    bool m_RadioStream;
    int  m_AudioCount;
    bool m_SkipAudio;
    bool m_StreamStart;
    int  m_FreeBufs;

    int PlayAny(const uchar *Data, int Length);

    bool AcceptVideoPacket(const uchar *Data, int Length);
    bool AcceptAudioPacket(const uchar *Data, int Length);

  protected:

    virtual bool Poll(cPoller &Poller, int TimeoutMs = 0);

    virtual void StillPicture(const uchar *Data, int Length);

    // MPEG-PES
    virtual int  PlayVideo(const uchar *Data, int Length);
    virtual int  PlayAudio(const uchar *Data, int Length, uchar Id);
    virtual int  PlaySubtitle(const uchar *Data, int Length);

    // join multiple TS packets to xineliboutput transport packet
    cMutex        m_TsBufLock;
    uint8_t       m_TsBuf[4096];
    uint          m_TsBufSize;

    int           TsBufferFlush(void);
    void          TsBufferClear(void);

    int           PlayTsAny(const uchar *Data, int Length);

    // MPEG-TS
    virtual int PlayTsVideo(const uchar *Data, int Length);
    virtual int PlayTsAudio(const uchar *Data, int Length);
    virtual int PlayTsSubtitle(const uchar *Data, int Length);
    virtual int PlayTs(const uchar *Data, int Length, bool VideoOnly = false);

  // Picture-In-Picture

  protected:
    uint16_t m_PipPid[MAX_NUM_PIP];

  public:
    int  Pip_Open   (void);
    void Pip_Config (int Index, int X, int Y, int W, int H);
    int  Pip_Play   (int Index, uint8_t *Data, int Length);
    void Pip_Close  (int Index);
};

#endif // __XINELIB_DEVICE_H
