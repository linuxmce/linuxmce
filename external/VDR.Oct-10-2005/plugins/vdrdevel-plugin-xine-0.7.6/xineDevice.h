
#ifndef __XINEDEVICE_H
#define __XINEDEVICE_H



#include "xineCommon.h"

#include <vdr/dvbspu.h>
#include <vdr/device.h>

#include "xineLib.h"



class cPlugin;

namespace PluginXine
{
  class cXineSettings;
  class cXineOsd;
  class cXineDevice;
  
  class cXineSpuDecoder : public cDvbSpuDecoder
  {
    cXineDevice *const m_xineDevice;

    void ptsAdjust(uint32_t &pts);
    
  public:
    cXineSpuDecoder(cXineDevice *const xineDevice)
      : cDvbSpuDecoder()
      , m_xineDevice(xineDevice)
    {
    }
    
    virtual int setTime(uint32_t pts);
  };
  
  class cXineDevice : public cDevice, public cXineLibEvents
  {
    cXineSettings &m_settings;

    cXineOsd *m_currentOsd;
    cXineSpuDecoder *m_spuDecoder;
    
    virtual bool HasDecoder(void) const;
    virtual cSpuDecoder *GetSpuDecoder(void);
    virtual bool CanReplay(void) const;
    virtual bool SetPlayMode(ePlayMode PlayMode);
    virtual void TrickSpeed(int Speed);
    virtual void Clear(void);
    virtual void Play(void);
    virtual void Freeze(void);
    virtual void Mute(void);
    virtual void StillPicture(const uchar *Data, int Length);
    virtual bool Poll(cPoller &Poller, int TimeoutMs = 0);
    virtual bool Flush(int TimeoutMs = 0);
    virtual int PlayVideo(const uchar *Data, int Length);
    int PlayVideo1(const uchar *Data, int Length, const bool stillImageData);
    int PlayVideo2(const uchar *Data, int Length, const bool stillImageData);
    int PlayVideo3(const uchar *Data, int Length, const bool stillImageData);
    int PlayAudio2(const uchar *Data, int Length);
    int PlayAudio3(const uchar *Data, int Length);
    int PlayCommon(const uchar *Data, int Length, const bool stillImageData);
    int PlayCommon2(const uchar *Data, int Length);
    int PlayCommon3(const uchar *Data, int Length);
    
    virtual bool GrabImage(const char *FileName, bool Jpeg = true, int Quality = -1, int SizeX = -1, int SizeY = -1);
    virtual void SetVideoFormat(bool VideoFormat16_9);
    virtual void SetVolumeDevice(int Volume);
    
#if VDRVERSNUM >= 10307
    virtual void MakePrimaryDevice(bool On);
#endif

    int m_audioChannel;

#if VDRVERSNUM >= 10318
    virtual int GetAudioChannelDevice(void);
    virtual void SetAudioChannelDevice(int AudioChannel);
    virtual void SetDigitalAudioDevice(bool On);
    virtual int PlayAudio(const uchar *Data, int Length);
#else
    virtual void PlayAudio(const uchar *Data, int Length);
#endif

    int PlayAudioCommon(const uchar *Data, int Length);
    
    bool open();
    void close();

    int PushOut();
    void initStream();
    void reshowCurrentOsd(const bool dontOptimize = true, const int frameWidth = -1, const int frameHeight = -1);
    virtual void OnClientConnect();
    virtual void OnClientDisconnect();
    virtual void ReshowCurrentOSD(const int frameWidth, const int frameHeight);
    
  public:
    virtual int64_t GetSTC(void);
    
#if VDRVERSNUM < 10307
    virtual cOsdBase *NewOsd(int x, int y);
#else    
    virtual cOsd *NewOsd(int x, int y);
#endif
    
    cXineDevice(cPlugin *const plugin, cXineSettings &settings, cXineRemote *remote);
    virtual ~cXineDevice();

#if VDRVERSNUM < 10307
    void OnFreeOsd(cOsdBase *const osd);
#else    
    void OnFreeOsd(cOsd *const osd);
#endif

    cXineLib m_xineLib;
    cMutex m_osdMutex;

    cMutex m_pmMutex;
    cCondVar m_pmCondVar;
    
    static bool Create(cPlugin *const plugin, cXineSettings &settings, cXineRemote *remote);
    static bool Open();
    static void Stop();
    static cXineDevice *GetDevice();

    bool hasNoSignalStream() const
    {
      return m_xineLib.hasNoSignalStream();
    }
  };

};



#endif //__XINEDEVICE_H
