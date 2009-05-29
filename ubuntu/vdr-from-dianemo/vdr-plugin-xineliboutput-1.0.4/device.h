/*
 * device.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: device.h,v 1.34.2.2 2008/12/10 22:56:15 phintuka Exp $
 *
 */

#ifndef __XINELIB_DEVICE_H
#define __XINELIB_DEVICE_H

#include <vdr/config.h>
#include <vdr/device.h>
#include <vdr/tools.h>

class cXinelibStatusMonitor;
class cXinelibThread;
class cChannel;
class cFunctor;

typedef enum {
  miTitle  = 0,
  miTracknumber  = 1,
  miArtist = 2,
  miAlbum  = 3,
  miDvdTitleNo = 4,
  miDvdButtons = 5,
  mi_Count = 6
} eMetainfoType;

# define ttXSubtitleNone  (-2)
# define ttXSubtitleAuto  (-1)

#define MAX_METAINFO_LEN 63

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
    void ForcePrimaryDevice(bool On);


  // Device capabilities
  private:
    bool      m_VDR_TrickSpeedIBP;

  public:

    virtual bool HasDecoder(void) const { return true; };
    virtual bool CanReplay(void) const { return true; };

    virtual bool HasIBPTrickSpeed(void);

  // Playback control

  private:
    ePlayMode m_PlayMode;
    int       m_TrickSpeed;
    int64_t   m_TrickSpeedPts;
    int       m_TrickSpeedMode;
    int       m_TrickSpeedDelay;

  public:
    virtual bool SetPlayMode(ePlayMode PlayMode);
    ePlayMode GetPlayMode(void) const { return m_PlayMode; };

  protected:
    virtual void    Clear(void);
    virtual void    Play(void);
    virtual void    TrickSpeed(int Speed);
    virtual void    Freeze(void);
    virtual bool    Flush(int TimeoutMs = 0);
    virtual int64_t GetSTC(void);

    bool UseIBPTrickSpeed(void);

  // Video format facilities

  public:
    virtual void SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat);
    virtual void SetVideoFormat(bool VideoFormat16_9);
    virtual eVideoSystem GetVideoSystem(void);

  // Track facilities

  protected:
    virtual void SetAudioTrackDevice(eTrackType Type);

  private:
#if VDRVERSNUM < 10515
    // (DVD) SPU tracks
    tTrackId m_DvdSpuTrack[64];
    int      m_CurrentDvdSpuTrack;
    bool     m_ForcedDvdSpuTrack;
#endif
    char     m_MetaInfo[mi_Count][MAX_METAINFO_LEN+1];

  public:
#if VDRVERSNUM < 10515
    void ClrAvailableDvdSpuTracks(bool NotifyFrontend = true);
    bool SetAvailableDvdSpuTrack(int Type, const char *lang = NULL, bool Current = false);

    int   NumDvdSpuTracks(void) const;
    const tTrackId *GetDvdSpuTrack(int Type) const;
    const char *GetDvdSpuLang(int Type) const;

    int   GetCurrentDvdSpuTrack(void) const { return m_CurrentDvdSpuTrack; }
    bool  SetCurrentDvdSpuTrack(int Type, bool Force=false);
    void  EnsureDvdSpuTrack(void);
#else
    virtual void SetSubtitleTrackDevice(eTrackType Type);
#endif

    const char *GetMetaInfo(eMetainfoType Type);
    void        SetMetaInfo(eMetainfoType Type, const char *Value);

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
    void ConfigurePostprocessing(const char *name, bool on=true, 
				 const char *args=NULL);
    void ConfigureVideo(int hue, int saturation, int brightness, int contrast,
			int overscan, int vo_aspect_ratio);
    // local mode:
    void ConfigureWindow(int fullscreen, int width, int height, 
			 int modeswitch, const char *modeline, 
			 int aspect, int scale_video, int field_order);
    void ConfigureDecoder(int pes_buffers);
    // remote mode:
    void Listen(bool activate, int port);


  // File playback

  private:
    ePlayMode m_PlayingFile;

  public:
    bool PlayFile(const char *Filename, int Position=0, 
		  bool LoopPlay=false, ePlayMode PlayMode=pmAudioVideo);
    int  PlayFileCtrl(const char *Cmd, int TimeoutMs = -1);
    bool EndOfStreamReached(void);


  // Stream data

  private:
    bool m_ac3Present;
    bool m_spuPresent;
    bool m_RadioStream;
    int  m_AudioCount;
    bool m_SkipAudio;
    bool m_StreamStart;
    int  m_FreeBufs;
    bool m_Cleared;
    bool m_h264;

    int PlayAny(const uchar *Data, int Length);
    int PlayTrickSpeed(const uchar *buf, int length);

  protected:

    virtual bool Poll(cPoller &Poller, int TimeoutMs = 0);

    virtual void StillPicture(const uchar *Data, int Length);

    virtual int  PlayVideo(const uchar *Data, int Length);
    virtual int  PlayAudio(const uchar *Data, int Length, uchar Id);

#if VDRVERSNUM < 10510
    virtual int  PlaySpu(const uchar *Data, int Length, uchar Id);

    // conflicts with vdr-1.5.10+ DVB subtitle handling
    // override cDevice to get DVD SPUs
    virtual int PlayPesPacket(const uchar *Data, int Length,
			      bool VideoOnly = false);
#endif
};

#endif // __XINELIB_DEVICE_H
