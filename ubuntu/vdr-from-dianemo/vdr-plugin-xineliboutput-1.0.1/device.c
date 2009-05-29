/*
 * device.c: xine-lib output device for the Video Disk Recorder
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: device.c,v 1.57 2008/04/14 19:58:50 phintuka Exp $
 *
 */

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS
#include <inttypes.h>

#include <vdr/config.h>
#include <vdr/thread.h>
#include <vdr/dvbspu.h>
#include <vdr/channels.h>
#include <vdr/skins.h>
#include <vdr/status.h>
#include <vdr/remote.h>

//#define XINELIBOUTPUT_DEBUG
//#define XINELIBOUTPUT_DEBUG_STDERR
//#define TRACK_EXEC_TIME
//#define FORWARD_DVD_SPUS
//#define DEBUG_SWITCHING_TIME
//#define LOG_TRICKSPEED
#if VDRVERSNUM >= 10510
# define DEVICE_SUPPORTS_IBP_TRICKSPEED
#endif

#include "logdefs.h"
#include "config.h"
#include "osd.h"

#include "tools/listiter.h"
#include "tools/pes.h"
#include "tools/functor.h"

#include "frontend_local.h"
#include "frontend_svr.h"

#include "device.h"

#define STILLPICTURE_REPEAT_COUNT 3
#define LOCAL_INIT_TIMEOUT        20  // seconds
#define SERVER_INIT_TIMEOUT       5   // seconds

#ifdef LOG_TRICKSPEED
#  define LOGTRICKSPEED(x...) LOGMSG("trs: " x)
#else
#  define LOGTRICKSPEED(x...)
#endif

//---------------------------- status monitor -------------------------------

class cXinelibStatusMonitor : public cStatus 
{
  private:
    cXinelibStatusMonitor(); 
    cXinelibStatusMonitor(cXinelibStatusMonitor&); 

  public:
    cXinelibStatusMonitor(cXinelibDevice& device, int cardIndex) :
        m_Device(device), m_cardIndex(cardIndex) 
    {
#ifdef DEBUG_SWITCHING_TIME
      switchtimeOff   = 0LL;
      switchtimeOn    = 0LL;
      switchingIframe = false;
#endif
    };

  protected:
    virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
    virtual void Replaying(const cControl *Control, const char *Name, 
			   const char *FileName, bool On);

    cXinelibDevice& m_Device;
    int m_cardIndex;

#ifdef DEBUG_SWITCHING_TIME
  public:
    int64_t switchtimeOff;
    int64_t switchtimeOn;
    bool    switchingIframe;

    void IFrame(void)  
    {
      if(!switchingIframe) {
	int64_t now = cTimeMs::Now();
	switchingIframe = true;
	LOGMSG("Channel switch: off -> on %" PRId64 " ms, "
	       "on -> 1. I-frame %" PRId64 " ms",
	       switchtimeOn-switchtimeOff, now-switchtimeOn);
      } else {
	int64_t now = cTimeMs::Now();
	LOGMSG("Channel switch: on -> 2. I-frame %" PRId64 " ms, "
	       "Total %" PRId64 " ms",
	       now-switchtimeOn, now-switchtimeOff);
	switchtimeOff = 0LL;
	switchtimeOn = 0LL;
	switchingIframe = false;
      }
    }
#endif
};

void cXinelibStatusMonitor::ChannelSwitch(const cDevice *Device, 
					  int ChannelNumber) 
{
  TRACEF("cXinelibStatusMonitor::ChannelSwitch");
  TRACK_TIME(200);

  if (ChannelNumber) {
    if (Device->CardIndex() == m_cardIndex) {
#ifdef DEBUG_SWITCHING_TIME
      switchtimeOn = cTimeMs::Now();
#endif
      m_Device.SetTvMode(Channels.GetByNumber(ChannelNumber));
      TRACE("cXinelibStatusMonitor: Set to TvMode");
    }
  } else {
    if (Device->CardIndex() == m_cardIndex) {
#ifdef DEBUG_SWITCHING_TIME
      switchtimeOff = cTimeMs::Now();
#endif
      m_Device.StopOutput();
      TRACE("cXinelibStatusMonitor: received stop");
    }
  }
}

void cXinelibStatusMonitor::Replaying(const cControl *Control, 
				      const char *Name,
				      const char *FileName, bool On)
{
  TRACEF("cXinelibStatusMonitor::Replaying");

  if (On /*&& Name != NULL*/) {
    TRACE("cXinelibStatusMonitor: Replaying " << Name << "(" << FileName << ")");
    m_Device.SetReplayMode();
  }
}

//----------------------------- device ----------------------------------------

//
// Singleton
//

cXinelibDevice* cXinelibDevice::m_pInstance = NULL;

cXinelibDevice& cXinelibDevice::Instance(void) 
{
  TRACEF("cXinelibDevice::Instance");
  if (!m_pInstance) {
    m_pInstance = new cXinelibDevice();
    TRACE("cXinelibDevice::Instance(): create, cardindex = " 
	  << m_pInstance->CardIndex());
  }
  
  return *m_pInstance;
}

void cXinelibDevice::Dispose(void) 
{
  TRACEF("cXinelibDevice::Dispose");
  delete m_pInstance;
  m_pInstance = NULL;
}

//
// init and shutdown
//

cXinelibDevice::cXinelibDevice()
{
  TRACEF("cXinelibDevice::cXinelibDevice");

  m_statusMonitor = NULL;
  m_spuDecoder    = NULL;

  m_local  = NULL;
  m_server = NULL;

  m_OriginalPrimaryDevice = 0;
  m_ForcePrimaryDeviceCnt = 0;

  if(*xc.local_frontend && strncmp(xc.local_frontend, "none", 4))
    m_clients.Add(m_local = new cXinelibLocal(xc.local_frontend));
  if(xc.remote_mode && xc.listen_port>0)
    m_clients.Add(m_server = new cXinelibServer(xc.listen_port));

  m_ac3Present  = false;
  m_spuPresent  = false;

#if VDRVERSNUM < 10515
  m_CurrentDvdSpuTrack = ttXSubtitleNone;
  m_ForcedDvdSpuTrack = false;
  ClrAvailableDvdSpuTracks();
#endif

  memset(m_MetaInfo, 0, sizeof(m_MetaInfo));
    
  m_PlayMode = pmNone;
  m_AudioChannel = 0;

  m_liveMode    = true;
  m_TrickSpeed  = -1;
  m_TrickSpeedPts = 0;
  m_TrickSpeedMode = 0;
  m_TrickSpeedDelay = 0;
  m_VDR_TrickSpeedIBP = 0;
  m_SkipAudio   = false;
  m_PlayingFile = pmNone;
  m_StreamStart = true;
  m_RadioStream = false;
  m_AudioCount  = 0;
  m_FreeBufs = 0;
  m_Cleared = true;
  m_h264 = false;
}

cXinelibDevice::~cXinelibDevice() 
{
  TRACEF("cXinelibDevice::~cXinelibDevice");

  StopDevice();

  m_pInstance = NULL;
}

bool cXinelibDevice::StartDevice()
{
  TRACEF("cXinelibDevice::StartDevice");

  if(m_local)
    m_local->Start();
  if(m_server)
    m_server->Start();

  // if(dynamic_cast<cXinelibLocal*>(it))
  if(m_local) {
    int timer = 0;
    while(!m_local->IsReady()) {
      cCondWait::SleepMs(100);
      if(m_local->IsFinished()) {
        LOGMSG("cXinelibDevice::Start(): Local frontend init failed");
        return false;
      }
      if(++timer >= LOCAL_INIT_TIMEOUT*10) {
        LOGMSG("cXinelibDevice::Start(): Local frontend init timeout");
        return false;
      }
    }
    if(xc.force_primary_device)
      ForcePrimaryDevice(true);
  }

  if(m_server) {
    int timer = 0;
    while(!m_server->IsReady()) {
      cCondWait::SleepMs(100);
      if(m_server->IsFinished()) {
        LOGMSG("cXinelibDevice::Start(): Server init failed");
        return false;
      }
      if(++timer >= SERVER_INIT_TIMEOUT*10) {
        LOGMSG("cXinelibDevice::Start(): Server init timeout");
        return false;
      }
    }
  }

  ASSERT(m_statusMonitor == NULL, false);
  m_statusMonitor = new cXinelibStatusMonitor(*this, CardIndex());

  LOGDBG("cXinelibDevice::StartDevice(): Device started");
  return true;
}

void cXinelibDevice::StopDevice(void) 
{
  TRACEF("cXinelibDevice::StopDevice");
  LOGDBG("cXinelibDevice::StopDevice(): Stopping device ...");

  if(m_statusMonitor) {
    delete m_statusMonitor;
    m_statusMonitor = NULL;
  }
  if (m_spuDecoder) {
    delete m_spuDecoder;
    m_spuDecoder = NULL;
  }

  cXinelibThread *server = m_server;
  cXinelibThread *local  = m_local;
  m_local = m_server = NULL;

  cControl::Shutdown();
  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  TrickSpeed(-1);

  if(local)  m_clients.Del(local,  false);
  if(server) m_clients.Del(server, false);

  if(server) {
    server->Stop();
    delete server;
  }
  if(local) {
    local->Stop();
    delete local;
  }
 
  m_clients.Clear();
}

//
// Primary device switching
//

void cXinelibDevice::MakePrimaryDevice(bool On) 
{
  TRACEF("cXinelibDevice::MakePrimaryDevice");

  if(On)
    new cXinelibOsdProvider(this);
}

void cXinelibDevice::ForcePrimaryDevice(bool On) 
{
  TRACEF("cXinelibDevice::ForcePrimaryDevice");

  m_MainThreadLock.Lock();
  m_MainThreadFunctors.Add(CreateFunctor(this, &cXinelibDevice::ForcePrimaryDeviceImpl, On));
  m_MainThreadLock.Unlock();
}

void cXinelibDevice::ForcePrimaryDeviceImpl(bool On) 
{
  TRACEF("cXinelibDevice::ForcePrimaryDeviceImpl");
  ASSERT(cThread::IsMainThread(), false);

  if(On) {
    m_ForcePrimaryDeviceCnt++;

    if(xc.force_primary_device) {
      if(cDevice::PrimaryDevice() && this != cDevice::PrimaryDevice()) {
	m_OriginalPrimaryDevice = cDevice::PrimaryDevice()->DeviceNumber() + 1;
	cControl::Shutdown();
	LOGMSG("Forcing primary device, original index = %d", m_OriginalPrimaryDevice);
	if(cOsd::IsOpen()) {
	  LOGMSG("Forcing primary device, old OSD still open !");
	  xc.main_menu_mode = CloseOsd;
	  cRemote::CallPlugin("xineliboutput");
	}
	SetPrimaryDevice(DeviceNumber() + 1);
      }
    }
  
  } else /* Off */ {
    m_ForcePrimaryDeviceCnt--;

    if(m_ForcePrimaryDeviceCnt < 0)
      LOGMSG("ForcePrimaryDevice: Internal error (ForcePrimaryDevice < 0)");
    else if(m_ForcePrimaryDeviceCnt == 0) {
      if(m_OriginalPrimaryDevice) {
	LOGMSG("Restoring original primary device %d", m_OriginalPrimaryDevice);
	cControl::Shutdown();
	if(cOsd::IsOpen()) {
	  LOGMSG("Restoring primary device, xineliboutput OSD still open !");
	  xc.main_menu_mode = CloseOsd; /* will be executed in future by vdr main thread */
	  cRemote::CallPlugin("xineliboutput");
	}
	cChannel *channel = Channels.GetByNumber(CurrentChannel());
	cDevice::SetPrimaryDevice(m_OriginalPrimaryDevice);
	PrimaryDevice()->SwitchChannel(channel, true);
	m_OriginalPrimaryDevice = 0;
      }
    }
  }
}

//
// Execute functors in main thread context
//

void cXinelibDevice::MainThreadHook(void)
{
  TRACEF("cXinelibDevice::MainThreadHook");

  if(m_MainThreadFunctors.First()) {
    cFunctor *f = NULL;
    do {
      m_MainThreadLock.Lock();
      if(f)
	m_MainThreadFunctors.Del(f);
      f = m_MainThreadFunctors.First();
      m_MainThreadLock.Unlock();
      
      if(f) {
	/*LOGDBG("cXinelibDevice::MainThreadHook: executing functor 0x%lx",(long)f);*/
	f->Execute();
      }

    } while(f);
  }
}

//
// Configuration
//

void cXinelibDevice::ConfigurePostprocessing(const char *deinterlace_method, 
					     int audio_delay, 
					     int audio_compression, 
					     const int *audio_equalizer, 
					     int audio_surround,
					     int speaker_type)
{
  TRACEF("cXinelibDevice::ConfigurePostprocessing");

  if(m_local)
    m_local->ConfigurePostprocessing(deinterlace_method, audio_delay, 
				     audio_compression, audio_equalizer,
				     audio_surround, speaker_type);
  if(m_server)
    m_server->ConfigurePostprocessing(deinterlace_method, audio_delay, 
				      audio_compression, audio_equalizer,
				      audio_surround, speaker_type);
}

void cXinelibDevice::ConfigurePostprocessing(const char *name, bool on, 
					     const char *args)
{
  TRACEF("cXinelibDevice::ConfigurePostprocessing");

  if(m_local)
    m_local->ConfigurePostprocessing(name, on, args);
  if(m_server)
    m_server->ConfigurePostprocessing(name, on, args);
}

void cXinelibDevice::ConfigureVideo(int hue, int saturation, int brightness, int contrast,
				    int overscan, int vo_aspect_ratio)
{
  TRACEF("cXinelibDevice::ConfigureVideo");

  if(m_local)
    m_local->ConfigureVideo(hue, saturation, brightness, contrast, overscan, vo_aspect_ratio);
  if(m_server)
    m_server->ConfigureVideo(hue, saturation, brightness, contrast, overscan, vo_aspect_ratio);
}

void cXinelibDevice::ConfigureDecoder(int pes_buffers)
{
  TRACEF("cXinelibDevice::ConfigureDecoder");

  if(m_local)
    m_local->ConfigureDecoder(pes_buffers);
  //if(m_server)
  //  m_server->ConfigureDecoder(pes_buffers);

  cXinelibOsdProvider::RefreshOsd();
}

void cXinelibDevice::ConfigureWindow(int fullscreen, int width, int height, 
				     int modeswitch, const char *modeline, 
				     int aspect, int scale_video, 
				     int field_order)
{
  TRACEF("cXinelibDevice::ConfigureWindow");

  if((!*xc.local_frontend || !strncmp(xc.local_frontend, "none", 4)) && m_local) {
    cXinelibThread *tmp = m_local;
    m_clients.Del(tmp, false);
    m_local = NULL;
    cCondWait::SleepMs(5);
    tmp->Stop();
    cCondWait::SleepMs(5);
    delete tmp;
    if(xc.force_primary_device)
      ForcePrimaryDevice(false);
  }

  if(m_local)
    m_local->ConfigureWindow(fullscreen, width, height, modeswitch, modeline, 
			     aspect, scale_video, field_order);

  else if(*xc.local_frontend && strncmp(xc.local_frontend, "none", 4)) {
    cXinelibThread *tmp = new cXinelibLocal(xc.local_frontend);
    tmp->Start();
    m_clients.Add(m_local = tmp);

    cCondWait::SleepMs(25);
    while(!m_local->IsReady() && !m_local->IsFinished())
      cCondWait::SleepMs(25);

    if(m_local->IsFinished()) {
      m_local = NULL;
      m_clients.Del(tmp, true);
      Skins.QueueMessage(mtError, tr("Frontend initialization failed"), 10);
    } else {
      if(xc.force_primary_device)
	ForcePrimaryDevice(true);
  
      m_local->ConfigureWindow(fullscreen, width, height, modeswitch, modeline,
			       aspect, scale_video, field_order);
    }
  }
}

void cXinelibDevice::Listen(bool activate, int port)
{
  TRACEF("cXinelibDevice::Listen");

  if(activate && port>0) {
    if(!m_server) {
      cXinelibThread *tmp = new cXinelibServer(port);
      tmp->Start();
      m_clients.Add(m_server = tmp);

      cCondWait::SleepMs(10);
      while(!m_server->IsReady() && !m_server->IsFinished())
	cCondWait::SleepMs(10);

      if(m_server->IsFinished()) {
	Skins.QueueMessage(mtError, tr("Server initialization failed"), 10);
	m_server = NULL;
	m_clients.Del(tmp, true);
      }

    } else {
      if(! m_server->Listen(port))
	Skins.QueueMessage(mtError, tr("Server initialization failed"), 10);
    }
  } else if( /*((!activate) || port<=0) && */ m_server) {
    cXinelibThread *tmp = m_server;
    m_clients.Del(tmp, false);
    m_server = NULL;
    cCondWait::SleepMs(5);
    tmp->Stop();
    cCondWait::SleepMs(5);
    delete tmp;
  }
}

//
// OSD
//

void cXinelibDevice::OsdCmd(void *cmd)
{
  TRACEF("cXinelibDevice::OsdCmd");
  TRACK_TIME(250);

  if(m_server)    // call first server, local frontend modifies contents of the message ...
    m_server->OsdCmd(cmd);
  if(m_local)
    m_local->OsdCmd(cmd);
}

//
// Play mode control
//
void cXinelibDevice::StopOutput(void)
{
  TRACEF("cXinelibDevice::StopOutput");
  TRACK_TIME(250);

  m_RadioStream = false;
  m_AudioCount  = 0;
  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  Clear();
  ForEach(m_clients, &cXinelibThread::QueueBlankDisplay);
  ForEach(m_clients, &cXinelibThread::SetNoVideo, false);
#if VDRVERSNUM < 10515
  ClrAvailableDvdSpuTracks();
  m_ForcedDvdSpuTrack = false;
#endif
}

void cXinelibDevice::SetTvMode(cChannel *Channel)
{
  TRACEF("cXinelibDevice::SetTvMode");
  TRACK_TIME(250);

  m_RadioStream = false;
  if (Channel && !Channel->Vpid() && (Channel->Apid(0) || Channel->Apid(1)))
    m_RadioStream = true;
  if(m_PlayMode == pmAudioOnlyBlack)
    m_RadioStream = true;
  TRACE("cXinelibDevice::SetTvMode - isRadio = "<<m_RadioStream);

  m_StreamStart = true;
  m_liveMode = true;
  m_TrickSpeed = -1;
  m_SkipAudio  = false;
  m_AudioCount = 0;
  m_spuPresent = false;

  Clear();
  ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
  ForEach(m_clients, &cXinelibThread::SetLiveMode, true);
  ForEach(m_clients, &cXinelibThread::ResumeOutput);
}

void cXinelibDevice::SetReplayMode(void)
{
  TRACEF("cXinelibDevice::SetReplayMode");

  m_RadioStream = true; // first seen replayed video packet resets this
  m_AudioCount  = 15;
  m_StreamStart = true;

  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  Clear();
  ForEach(m_clients, &cXinelibThread::SetNoVideo, false /*m_RadioStream*/);
  if(m_RadioStream && !m_liveMode)
    ForEach(m_clients, &cXinelibThread::BlankDisplay);
  ForEach(m_clients, &cXinelibThread::ResumeOutput);

  m_liveMode = false;
}

bool cXinelibDevice::SetPlayMode(ePlayMode PlayMode) 
{
  TRACEF("cXinelibDevice::SetPlayMode");

#ifdef XINELIBOUTPUT_DEBUG
  switch (PlayMode) {
    case pmNone:     
      TRACE("cXinelibDevice::SetPlayMode audio/video from decoder"); break;
    case pmAudioVideo: 
      TRACE("cXinelibDevice::SetPlayMode audio/video from player"); break;
    case pmVideoOnly:  
      TRACE("cXinelibDevice::SetPlayMode video from player, audio from decoder"); break;
    case pmAudioOnly:  
      TRACE("cXinelibDevice::SetPlayMode audio from player, video from decoder"); break;
    case pmAudioOnlyBlack: 
      TRACE("cXinelibDevice::SetPlayMode audio only from player, no video (black screen)"); break;
    case pmExtern_THIS_SHOULD_BE_AVOIDED: 
      TRACE("cXinelibDevice::SetPlayMode this should be avoided"); break;
  }
#endif

  m_ac3Present = false;
  m_spuPresent = false;

#if VDRVERSNUM < 10515
  ClrAvailableDvdSpuTracks();
#endif
  m_PlayMode = PlayMode;

  TrickSpeed(-1);
  if (m_PlayMode == pmAudioOnlyBlack) {
    TRACE("pmAudioOnlyBlack --> BlankDisplay, NoVideo");
    ForEach(m_clients, &cXinelibThread::BlankDisplay);
    ForEach(m_clients, &cXinelibThread::SetNoVideo, true);
  } else {
    if(m_liveMode)
      ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
    else
      ForEach(m_clients, &cXinelibThread::SetNoVideo, 
	      m_RadioStream && (m_AudioCount<1));
    Clear();
  }
  
  return true;
}

//
// Playback control
//

// m_TrickSpeedMode flags
#define trs_IPB_frames 0x01  // stream has all frame types
#define trs_I_frames   0x02  // stream has only I-frames
#define trs_PTS_recalc 0x04  // PTS must be re-calculated
#define trs_NoAudio    0x08  // no audio in trick speed mode
#define trs_PTS_check  0x80  // detect in PlayVideo if PTS must be recalculated

bool cXinelibDevice::HasIBPTrickSpeed(void)
{
  TRACEF("cXinelibDevice::HasIBPTrickSpeed");

#ifndef DEVICE_SUPPORTS_IBP_TRICKSPEED
#warning VDR has not been patched for smooth fast forward. Disabling smooth fast forward.
  return false;
#else
  m_VDR_TrickSpeedIBP = true;

  return xc.ibp_trickspeed;
#endif
}

bool cXinelibDevice::UseIBPTrickSpeed(void) 
{ 
  return m_VDR_TrickSpeedIBP && xc.ibp_trickspeed; 
}

void cXinelibDevice::TrickSpeed(int Speed) 
{
  TRACEF("cXinelibDevice::TrickSpeed");

  if(m_TrickSpeed != Speed) {
    int RealSpeed = abs(Speed);
    LOGTRICKSPEED("TrickSpeed changed from %d to %d [%d]", m_TrickSpeed, Speed, RealSpeed);

    m_TrickSpeedPts = 0;
    m_TrickSpeed = Speed;
    m_TrickSpeedDelay = 0;

    //  Possible transitions:
    //     fast <-> play
    //     play <-> pause
    //     pause <-> slow
    //     _and_ from any mode to normal play and pause

    if(Speed == 8 || Speed == 4 || Speed == 2) {
      LOGTRICKSPEED("    Slow forward (1/%d speed), IPB-frames", Speed);

      // do nothing - slow forward is just slow playback of complete stream
      m_TrickSpeedMode = trs_IPB_frames;

      // previous state was slow forward or pause --> no need for clear

      // change decoder and UDP/RTP scheduler clock rates
      ForEach(m_clients, &cXinelibThread::TrickSpeed, RealSpeed);
    }

    else if(Speed == 63 || Speed == 48 || Speed == 24) {
      RealSpeed = (Speed+11)/12;
      LOGTRICKSPEED("    Slow backward (1/%d speed), I-frames only", RealSpeed);    

      // previous state was slow backwards or pause --> clear if it was pause
      //
      //if(PrevSpeed == 0 && !(m_TrickSpeedMode & trs_PTS_recalc)) {
      //  LOGMSG("    -> Clear");
      //  ForEach(m_clients, &cXinelibThread::Clear);
      //}

      // only I-frames, backwards, pts must be re-generated
      m_TrickSpeedMode = trs_I_frames | trs_PTS_recalc | trs_NoAudio;

      // change decoder and UDP/RTP scheduler clock rates
      ForEach(m_clients, &cXinelibThread::TrickSpeed, RealSpeed);
    }

    else if(Speed == 6 || Speed == 3 || Speed == 1) {
      RealSpeed = 12/Speed;
      LOGTRICKSPEED("    Fast (%dx speed), direction unknown", RealSpeed);

      if(RealSpeed > xc.max_trickspeed) {
	RealSpeed = xc.max_trickspeed;
	LOGTRICKSPEED("    Trick speed limited to %dx speed", RealSpeed);
      }

      /* only I-frames, backwards, pts must be re-generated if playing backwards */
      m_TrickSpeedMode |= trs_PTS_check;

      /* backward/forward state is unknown until first PTS is seen 
	 so, clear() must be done in PlayVideo. */
      /* previous trick speed state is not overwritten yet ... ! */
      
      // change decoder and UDP/RTP scheduler clock rates
      ForEach(m_clients, &cXinelibThread::TrickSpeed, -RealSpeed);
    }

    else if(Speed==-1 || Speed == 0) {
      LOGTRICKSPEED("    Play/Pause");

      // change decoder and UDP/RTP scheduler clock rates
      ForEach(m_clients, &cXinelibThread::TrickSpeed, RealSpeed);

      // returning from backward mode needs Clear 
      //
      //if(Speed==-1 && (m_TrickSpeedMode & trs_PTS_recalc)) {
      //  LOGMSG("    -> Clear");
      //  ForEach(m_clients, &cXinelibThread::Clear);
      //  m_TrickSpeedMode = 0;
      //}
      // returning from fast forward mode needs Clear
      // because of DvbPlayer jumps few seconds back at mode change ...
      //
      //if(Speed==-1 && (m_TrickSpeedMode & trs_I_frames)) {
      //  LOGMSG("    -> Clear");
      //  ForEach(m_clients, &cXinelibThread::Clear);
      //}
      m_TrickSpeedMode = 0;
    } 

    else {
      LOGTRICKSPEED("    Unknown trickspeed %d !", Speed);
      m_TrickSpeedMode = 0;
      m_TrickSpeed = -1;
      ForEach(m_clients, &cXinelibThread::TrickSpeed, -1);
    }
  }
}

void cXinelibDevice::Clear(void) 
{
  TRACEF("cXinelibDevice::Clear");
  TRACK_TIME(100);

  if(m_Cleared && m_StreamStart && m_TrickSpeed == -1) {
    //LOGMSG("************ Double Clear ***************");
  } else {
    //LOGMSG("************ FIRST  Clear ***************");
    m_Cleared = true;
    m_StreamStart = true;
    m_h264 = false;
    m_FreeBufs = 0;
    TrickSpeed(-1);
    ForEach(m_clients, &cXinelibThread::Clear);
    ForEach(m_clients, &cXinelibThread::SetStillMode, false);
  }
}

void cXinelibDevice::Play(void) 
{
  TRACEF("cXinelibDevice::Play");

  m_SkipAudio  = false;
  
  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  TrickSpeed(-1);
}

void cXinelibDevice::Freeze(void) 
{
  TRACEF("cXinelibDevice::Freeze");

  TrickSpeed(0);
}

int64_t cXinelibDevice::GetSTC(void)
{
  TRACEF("cXinelibDevice::GetSTC");

  if(m_local)
    return m_local->GetSTC();
  if(m_server)
    return m_server->GetSTC();
  return cDevice::GetSTC();
}

bool cXinelibDevice::Flush(int TimeoutMs) 
{
  TRACEF("cXinelibDevice::Flush");
  TRACK_TIME(500);

  if(m_TrickSpeed == 0) {
    ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
    TrickSpeed(-1);
  }

  bool r = ForEach(m_clients, &cXinelibThread::Flush, TimeoutMs, 
		   &mand<bool>, true);

  return r;
}


//
// Playback of files and images
//

int cXinelibDevice::PlayFileCtrl(const char *Cmd)
{
  TRACEF("cXinelibDevice::PlayFile");
  int result = -1;

  if(m_PlayingFile != pmNone) {
    if(m_server)
      result = m_server->PlayFileCtrl(Cmd);
    if(m_local) 
      result = m_local->PlayFileCtrl(Cmd);
  }
  return result;
}

bool cXinelibDevice::EndOfStreamReached(void)
{
  if(m_local && !m_local->EndOfStreamReached())
    return false;
  if(m_server && !m_server->EndOfStreamReached())
    return false;
  return true;
}

bool cXinelibDevice::PlayFile(const char *FileName, int Position, 
			      bool LoopPlay, ePlayMode PlayMode)
{
  TRACEF("cXinelibDevice::PlayFile");
  TRACE("cXinelibDevice::PlayFile(\"" << FileName << "\")");

  bool result = true;

  if(FileName) {
    if(m_PlayingFile == pmNone) {
      m_PlayingFile = PlayMode;
      if (!xc.IsImageFile(FileName))
         StopOutput();
    }
    for(int i = 0; i < mi_Count; i++) 
      m_MetaInfo[i][0] = 0;
    if(m_server)
      result = m_server->PlayFile(FileName, Position, LoopPlay, PlayMode);
    if(m_local)
      result = m_local->PlayFile(FileName, Position, LoopPlay, PlayMode);
  } else if(/*!FileName &&*/m_PlayingFile != pmNone) {
    if(m_server) 
      result = m_server->PlayFile(NULL, 0);
    if(m_local)
      result = m_local->PlayFile(NULL, 0);
    if(!m_liveMode)
      SetReplayMode();
    else
      SetTvMode(Channels.GetByNumber(cDevice::CurrentChannel()));
    m_PlayingFile = pmNone;
  }

  return result;
}

//
// Data stream handling
//

int cXinelibDevice::PlayTrickSpeed(const uchar *buf, int length) 
{
  if(abs(m_TrickSpeed) > 1 && (m_TrickSpeedMode & trs_I_frames)) {
    uint8_t PictureType = pes_get_picture_type(buf, length);
#ifdef LOG_TRICKSPEED
      if(PictureType != NO_PICTURE && PES_HAS_PTS(buf)) {
	int64_t pts = pes_get_pts(buf, length);
	LOGMSG("    TrickSpeed: frame %s pts %"PRId64, picture_type_str[PictureType], pts);
      }
#endif

#if 1
    // limit I-frame rate
    if(PictureType == I_FRAME) {
      static int64_t t0 = 0;
      int64_t t1 = cTimeMs::Now();
      if((t1 - t0) < 1000) {
	int fdelay = 40*12; // = 480 ms, time of one GOP in normal speed
	switch(m_TrickSpeed) {
	  case  6: /*   2x ff  */ fdelay /= min( 2, xc.max_trickspeed); break;
	  case  3: /*   4x ff  */ fdelay /= min( 4, xc.max_trickspeed); break;
	  case  1: /*  12x ff  */ fdelay /= min(12, xc.max_trickspeed); break;
	  case 63: /* 1/6x rew */ fdelay *= 6; break;
	  case 48: /* 1/4x rew */ fdelay *= 4; break;
	  case 24: /* 1/2x rew */ fdelay *= 2; break;
	  default: break;
	}
	/* wait if data is coming in too fast */
	if(fdelay - (t1-t0) >= 40) {
	  m_TrickSpeedDelay = 40;
	  return -1;
	}
	
	t0 += fdelay;

	pes_change_pts((uchar*)buf, length, INT64_C(0));
      } else {
	t0 = t1;
      }
    }
#endif
  }

  //
  // detecting trick speed mode ?
  //
  if( m_TrickSpeed > 0 && (m_TrickSpeedMode & trs_PTS_check) && IS_VIDEO_PACKET(buf)) {
    int64_t pts;
    if (PES_HAS_PTS(buf) && (pts = pes_get_pts(buf, length)) > 0) {
      uint8_t PictureType = pes_get_picture_type(buf, length);
      if(PictureType != I_FRAME && PictureType != NO_PICTURE) {
	// --> must be fast worward with IBP frames.
	// --> PTS check does not work (frames are sent in decoder order) ! */
	m_TrickSpeedPts = pts - 1;
	LOGTRICKSPEED("    Detected fast forward mode, using IBP frames");
      }
      if(m_TrickSpeedPts == 0) {
	m_TrickSpeedMode |= trs_NoAudio;
	m_TrickSpeedPts = pts;
	LOGTRICKSPEED("    Seen video pts = %"PRId64, pts);
      } else {
	if(pts < m_TrickSpeedPts) {
	  /* -> playing fast backwards */
	  LOGTRICKSPEED("    Detected fast backward mode. last %"PRId64" now %"PRId64, 
			m_TrickSpeedPts, pts);
	  //if(!(m_TrickSpeedMode & trs_PTS_recalc))
	  //  ForEach(m_clients, &cXinelibThread::Clear);
	  m_TrickSpeedMode = trs_I_frames | trs_PTS_recalc | trs_NoAudio;	  
	} else {
	  LOGTRICKSPEED("    Detected fast forward mode");
          if(UseIBPTrickSpeed())
            m_TrickSpeedMode = trs_IPB_frames;
          else
            m_TrickSpeedMode = trs_I_frames;
	}
      }
    }
  }

  //
  // Trick speed mode with PTS re-calc
  //
  if( m_TrickSpeed > 0 && (m_TrickSpeedMode & trs_PTS_recalc) && 
      IS_VIDEO_PACKET(buf) && PES_HAS_PTS(buf)) {
    int64_t pts = pes_get_pts(buf, length);
    if (pts > 0) {
      
      /* m_TrickSpeedPts could be 0 in case of slow backwards */
      if(m_TrickSpeedPts == 0)
	m_TrickSpeedPts = pts;

      LOGTRICKSPEED("    pts %"PRId64" -> %"PRId64" (diff %"PRId64")  %"PRId64"", pts, 
		    m_TrickSpeedPts + 40*12*90, m_TrickSpeedPts + 40*12*90 - pts,
		    (m_TrickSpeedPts + 40*12*90)^0x80000000);
      pts = m_TrickSpeedPts = m_TrickSpeedPts + 40*12*90; /* 12 frames * 40ms -> pts units */
      pts ^= 0x80000000; /* discontinuity (when mode changes) forces re-syncing of all clocks */
      pes_change_pts((uchar*)buf, length, pts);
    }
  }

#if 1
  else if (m_TrickSpeedMode & trs_I_frames) {
    if (IS_VIDEO_PACKET(buf) && PES_HAS_PTS(buf)) {
      int64_t pts = pes_get_pts(buf, length);
      if (pts > 0) {
	pts ^= 0x80000000; /* discontinuity (when mode changes) forces re-syncing of all clocks */
	pes_change_pts((uchar*)buf, length, pts);
      }
    }
  }
#endif

  return 0;
}

int cXinelibDevice::PlayAny(const uchar *buf, int length) 
{
  TRACEF("cXinelibDevice::PlayAny");
  TRACK_TIME(100);

#if 0
  if(m_PlayingFile)
    return length;
#endif

  //
  // Need to be sure Poll has been called for every frame:
  //  - cDevice can feed multiple frames after each poll from player/transfer.
  //  - If only part of frames are consumed, rest are fed again after next Poll.
  //  - If there are multiple clients it is possible first client(s)
  //    can queue more frames than last client(s).
  //    -> frame(s) are either lost immediately (last client(s)) 
  //       or duplicated after next poll (first client(s))
  //
  if(m_FreeBufs < 1) {
    cPoller Poller;
    if(!Poll(Poller,0)) {
      errno = EAGAIN;
      return 0;
    }
  }

  int isMpeg1 = pes_is_mpeg1(buf);
  int len = pes_packet_len(buf, length);
  if(len>0 && len != length) 
    LOGMSG("cXinelibDevice::PlayAny: invalid data !");

  if(m_TrickSpeed > 0) {
    if(PlayTrickSpeed(buf, length) < 0)
      return 0; /* wait if data is coming in too fast */
  } else if(m_SkipAudio) {
    /* needed for still images when moving cutting marks */
    pes_change_pts((uchar*)buf, length, INT64_C(0));
  }
  m_Cleared = false;
  m_FreeBufs --;

  if(m_local) {
    length = (isMpeg1 ? m_local->Play_Mpeg1_PES(buf,length) : 
	                m_local->Play_PES(buf,length));
  } 
  if(m_server && length > 0) {
    int length2 = isMpeg1 ? m_server->Play_Mpeg1_PES(buf, length) : 
                            m_server->Play_PES(buf, length);   
    if(!m_local)
      return length2;
  }
  
  return length;
}

int cXinelibDevice::PlayVideo(const uchar *buf, int length) 
{
  TRACEF("cXinelibDevice::PlayVideo");
  TRACK_TIME(100);

  if(m_PlayMode == pmAudioOnlyBlack)
    return length;

  if(m_RadioStream) {
    m_RadioStream = false;
    m_AudioCount  = 0;
    ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
  }
  
  if(m_StreamStart) {

#ifdef START_IFRAME
    // Start with I-frame if stream has video
    // wait for first I-frame
    if (pes_get_picture_type(buf, length) == I_FRAME) {
      m_StreamStart = false;
    } else {
      return length;
    }
#endif

    if (!m_h264 && pes_is_frame_h264(buf, length)) {
      LOGMSG("cXinelibDevice::PlayVideo: Detected H.264 video");
      m_h264 = true;
    }

    video_size_t Size;
    if (pes_get_video_size(buf, length, &Size, m_h264 ? 1:0)) {
      m_StreamStart = false;
      /*if (m_h264)*/ LOGMSG("Detected video size %dx%d", Size.width, Size.height);
      //LOGDBG("Detected video size %dx%d", Width, Height);
      ForEach(m_clients, &cXinelibThread::SetHDMode, (Size.width > 800));
    }
  }

#ifdef DEBUG_SWITCHING_TIME
  if(m_statusMonitor->switchtimeOff && m_statusMonitor->switchtimeOn) {
    if (pes_get_picture_type(buf, length) == I_FRAME)
      m_statusMonitor->IFrame();
  }
#endif

  if(m_PlayingFile && (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmVideoOnly))
    return length;

  return PlayAny(buf, length);
}

void cXinelibDevice::StillPicture(const uchar *Data, int Length) 
{
  TRACEF("cXinelibDevice::StillPicture");

  // skip still images coming in too fast (ex. when moving cutting marks)
  if(cRemote::HasKeys()) {
    static int skipped = 0;
    static uint64_t lastshow = 0;
    uint64_t now = cTimeMs::Now();
    if(now - lastshow < 500) {
      skipped++;
      //LOGMSG("Skipping still image (coming in too fast)");
      return;
    }
    LOGDBG("Forcing still image - skipped %d images", skipped);
    lastshow = now;
    skipped = 0;
  }

  bool isPes   = (!Data[0] && !Data[1] && Data[2] == 0x01 && 
		  (Data[3] & 0xF0) == 0xE0);
  bool isMpeg1 = isPes && ((Data[6] & 0xC0) != 0x80);
  int i;

  if(m_PlayingFile && (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmVideoOnly))
    return;

  TRACE("cXinelibDevice::StillPicture: isPes = "<<isPes
	<<", isMpeg1 = "<<isMpeg1);

  ForEach(m_clients, &cXinelibThread::Clear);
  ForEach(m_clients, &cXinelibThread::SetNoVideo, false);
  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  ForEach(m_clients, &cXinelibThread::SetStillMode, true);
  ForEach(m_clients, &cXinelibThread::TrickSpeed, 1);

  m_TrickSpeed = -1; // to make Poll work ...
  m_SkipAudio = 1;   // enables audio and pts stripping

  for(i=0; i<STILLPICTURE_REPEAT_COUNT; i++)
    if(isMpeg1) {
      ForEach(m_clients, &cXinelibThread::Play_Mpeg1_PES, Data, Length, 
	      &mmin<int>, Length);
    } else if(isPes) {
      /*cDevice::*/PlayPes(Data, Length, m_SkipAudio);
    } else {
      ForEach(m_clients, &cXinelibThread::Play_Mpeg2_ES, 
	      Data, Length, VIDEO_STREAM,
	      &mand<bool>, true);
    }

  // creates empty video PES with pseudo-pts
  ForEach(m_clients, &cXinelibThread::Play_Mpeg2_ES,
	  Data, 0, VIDEO_STREAM,
	  &mand<bool>, true);

  ForEach(m_clients, &cXinelibThread::Flush, 60, 
	  &mand<bool>, true);

  m_TrickSpeed = 0;
  m_SkipAudio = 0;
}

int cXinelibDevice::PlayAudio(const uchar *buf, int length, uchar Id) 
{
  TRACEF("cXinelibDevice::PlayAudio");
  TRACK_TIME(100);

  if(!buf || length < 6)
    return length;

#ifdef SKIP_AC3_AUDIO
    // skip AC3 audio
  if(((unsigned char *)buf)[3] == PRIVATE_STREAM1) {
    TRACE("cXinelibDevice::PlayVideo: PRIVATE_STREAM1 discarded");
    return length;
  }
#endif

  // strip audio in trick speed modes and when displaying still images
  if(m_SkipAudio /*|| m_TrickSpeed > 0*/)
    return length;
  if(m_TrickSpeedMode & trs_NoAudio)
    return length;

  if(m_RadioStream) {
    if(m_AudioCount) {
      m_AudioCount--;
      if(!m_AudioCount) {
	LOGDBG("PlayAudio detected radio stream");
	ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
      }
    }
  }

  if(m_PlayingFile && (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmAudioOnly))
    return length;

  return PlayAny(buf, length);
}

#if VDRVERSNUM < 10510
int cXinelibDevice::PlaySpu(const uchar *buf, int length, uchar Id) 
{
  TRACEF("cXinelibDevice::PlaySpu");

  if(!buf || length < 6)
    return length;

  if(((unsigned char *)buf)[3] == PRIVATE_STREAM1) {

    if(!m_spuPresent) {
      TRACE("cXinelibDevice::PlaySpu first DVD SPU frame");
      Skins.QueueMessage(mtInfo,"DVD Subtitles");
      m_spuPresent = true;
      
      ForEach(m_clients, &cXinelibThread::SpuStreamChanged, (int)Id);
    }

    // Strip all but selected SPU track
    if(Id != m_CurrentDvdSpuTrack)
      return length;
  }

  return PlayAny(buf, length);
}
#endif

bool cXinelibDevice::Poll(cPoller &Poller, int TimeoutMs) 
{
  TRACEF("cXinelibDevice::Poll");
  TRACK_TIME(400);

  if(m_PlayingFile == pmAudioVideo)
    return true;

  if(m_TrickSpeed == 0) {
    cCondWait::SleepMs(min(TimeoutMs, 20));
    return Poller.Poll(0);
  }

  if(!m_local && !m_server) {
    /* nothing to do... why do I exist ... ? */
    //cCondWait::SleepMs(TimeoutMs);
    //return Poller.Poll(0);
    return true;
  }

  if(m_TrickSpeed > 1 && m_TrickSpeedDelay > 20) {
    LOGTRICKSPEED("    Poll: m_TrickSpeedDelay=%d.", m_TrickSpeedDelay);
    cCondWait::SleepMs(20);
    m_TrickSpeedDelay -= 20;
    return false;
  }

  if(m_FreeBufs < 1) {
    int result = DEFAULT_POLL_SIZE;

    if(m_local)
      result = min(result, m_local->Poll(Poller, TimeoutMs));
    if(m_server)
      result = min(result, m_server->Poll(Poller, TimeoutMs));

    m_FreeBufs = max(result, 0);
  }
  
  return m_FreeBufs > 0 /*|| Poller.Poll(0)*/;
}

//
// Audio facilities
//

void cXinelibDevice::SetVolumeDevice(int Volume) 
{
  TRACEF("cXinelibDevice::SetVolumeDevice");

  ForEach(m_clients, &cXinelibThread::SetVolume, Volume);  
}

void cXinelibDevice::SetAudioTrackDevice(eTrackType Type)
{
  TRACEF("cXinelibDevice::SetAudioTrackDevice");

  // track changes are autodetected at xine side
}

void cXinelibDevice::SetAudioChannelDevice(int AudioChannel)
{
  TRACEF("cXinelibDevice::SetAudioChannelDevice");

  if(m_AudioChannel != AudioChannel) {
    m_AudioChannel = AudioChannel;
    //LOGDBG("cXinelibDevice::SetAudioChannelDevice --> %d", AudioChannel);
#if 0
    switch(AudioChannel) {
      default:
    //case 0: ConfigurePostprocessing("upmix_mono", false, NULL);
      case 0: ConfigurePostprocessing("upmix_mono", true, "channel=-1");
              break;
      case 1: ConfigurePostprocessing("upmix_mono", true, "channel=0");
	      break;
      case 2: ConfigurePostprocessing("upmix_mono", true, "channel=1");
	      break;
    }
#else
    switch(AudioChannel) {
      default:
      case 0: ConfigurePostprocessing("audiochannel", false, NULL); 
              break;
      case 1: ConfigurePostprocessing("audiochannel", true, "channel=0");
	      break;
      case 2: ConfigurePostprocessing("audiochannel", true, "channel=1");
	      break;
    }
#endif
  }
}

void cXinelibDevice::SetDigitalAudioDevice(bool On)
{
  TRACEF("cXinelibDevice::SetDigitalAudioDevice");

  // track changes are autodetected at xine side
}

//
// Video format facilities
//

void cXinelibDevice::SetVideoFormat(bool VideoFormat16_9) 
{
  TRACEF("cXinelibDevice::SetVideoFormat");
  cDevice::SetVideoFormat(VideoFormat16_9);

#if 0
  //
  // TODO
  //
  if(xc.aspect != ASPECT_AUTO && 
     xc.aspect != ASPECT_DEFAULT) {
    if(VideoFormat16_9)
      xc.aspect = ASPECT_16_9;
    else if(xc.aspect == ASPECT_16_9)
      xc.aspect = ASPECT_4_3;
    ConfigureDecoder(,,,xc.aspect,,,);
  }
#endif
}

void cXinelibDevice::SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat)
{
  TRACEF("cXinelibDevice::SetVideoDisplayFormat");
  cDevice::SetVideoDisplayFormat(VideoDisplayFormat);

#if 0
  //
  // TODO
  //
  //  - set normal, pan&scan, letterbox (only for 4:3?)
  //
  if(xc.aspect != ASPECT_AUTO && 
     xc.aspect != ASPECT_DEFAULT)  {
    switch(VideoDisplayFormat) {
    case vdfPanAndScan:
      xc.aspect = ASPECT_PAN_SCAN;
      break;
    case vdfLetterBox:    
      xc.aspect = ASPECT_4_3; /* borders are added automatically if needed */
      break;
    case vdfCenterCutOut:
      xc.aspect = ASPECT_CENTER_CUT_OUT;
      break;
    }
    ConfigureDecoder(,,,xc.aspect,,,);
  }
#endif
}

eVideoSystem cXinelibDevice::GetVideoSystem(void)
{
  TRACEF("cXinelibDevice::GetVideoSystem");
  return cDevice::GetVideoSystem();
}


//
// SPU decoder
//

#ifdef FORWARD_DVD_SPUS
# include "spu_decoder.h"
#endif

cSpuDecoder *cXinelibDevice::GetSpuDecoder(void)
{
  TRACEF("cXinelibDevice::GetSpuDecoder");
  if (!m_spuDecoder && IsPrimaryDevice()) {
    //
    // TODO
    //
    //  - use own derived SpuDecoder with special cXinelibOsd 
    //    -> always visible
    //

#ifdef FORWARD_DVD_SPUS
    // forward DVD SPUs to xine without decoding
    m_spuDecoder = new cFwdSpuDecoder(this);
#else
    m_spuDecoder = new cDvbSpuDecoder();
#endif
  }
  return m_spuDecoder;
}

//
// Image Grabbing
//

uchar *cXinelibDevice::GrabImage(int &Size, bool Jpeg, 
				 int Quality, int SizeX, int SizeY)
{
  TRACEF("cXinelibDevice::GrabImage");

  if(m_local)
    return m_local->GrabImage(Size, Jpeg, Quality, SizeX, SizeY);
  if(m_server)
    return m_server->GrabImage(Size, Jpeg, Quality, SizeX, SizeY);

  return NULL;
}


//
// DVD SPU support in VDR recordings
//
//   - override cDevice::PlayPesPacket to get DVD SPUs
//

#if VDRVERSNUM < 10510
int cXinelibDevice::PlayPesPacket(const uchar *Data, int Length, 
				  bool VideoOnly)
{
  switch (Data[3]) {
    case 0xBD: { // private stream 1
      int PayloadOffset = Data[8] + 9;
      uchar SubStreamId = Data[PayloadOffset];
      uchar SubStreamType = SubStreamId & 0xF0;
      uchar SubStreamIndex = SubStreamId & 0x1F;
      switch (SubStreamType) {
        case 0x20: // SPU
        case 0x30: // SPU
	  SetAvailableDvdSpuTrack(SubStreamIndex);
	  return PlaySpu(Data, Length, SubStreamIndex);
	  break;
        default:
	  ;
      }
    }
    default:
      ;
  }
  return cDevice::PlayPesPacket(Data, Length, VideoOnly);
}
#endif

//
// Available DVD SPU tracks
//

#if VDRVERSNUM < 10515
bool cXinelibDevice::SetCurrentDvdSpuTrack(int Type, bool Force)
{
  if(Type == ttXSubtitleNone || 
     ( Type >= 0 && 
       Type < 64 &&
       m_DvdSpuTrack[Type].id != 0xffff)) {
    m_CurrentDvdSpuTrack = Type;
    ForEach(m_clients, &cXinelibThread::SpuStreamChanged, Type);
    if(Force)
      m_ForcedDvdSpuTrack = true;
    return true;
  }
  return false;
}
#endif

#if VDRVERSNUM >= 10515
void cXinelibDevice::SetSubtitleTrackDevice(eTrackType Type)
{
  if (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmVideoOnly)
    ForEach(m_clients, &cXinelibThread::SetSubtitleTrack, Type);
}
#endif

#if VDRVERSNUM < 10515
void cXinelibDevice::ClrAvailableDvdSpuTracks(bool NotifyFrontend)
{
  for(int i=0; i<64; i++)
    m_DvdSpuTrack[i].id = 0xffff;
  if(m_CurrentDvdSpuTrack >= 0 ) {
    m_CurrentDvdSpuTrack = ttXSubtitleNone;
    if(NotifyFrontend)
      ForEach(m_clients, &cXinelibThread::SpuStreamChanged, m_CurrentDvdSpuTrack);
  }
}
#endif

#if VDRVERSNUM < 10515
int cXinelibDevice::NumDvdSpuTracks(void) const
{ 
  int DvdSpuTracks = 0;
  for(int i=0; i<64; i++)
    if(m_DvdSpuTrack[i].id != 0xffff)
      DvdSpuTracks++;
  return DvdSpuTracks; 
}
#endif

#if VDRVERSNUM < 10515
const tTrackId *cXinelibDevice::GetDvdSpuTrack(int Type) const
{
  if(Type >= 0 && Type < 64 &&
     m_DvdSpuTrack[Type].id != 0xffff)
    return &m_DvdSpuTrack[Type];
  return NULL;
}
#endif

#if VDRVERSNUM < 10515
const char *cXinelibDevice::GetDvdSpuLang(int Type) const
{
  const tTrackId *track = GetDvdSpuTrack(Type);
  if(track)
    return track->language[0] ? track->language : NULL;
  return NULL;
}
#endif

#if VDRVERSNUM < 10515
bool cXinelibDevice::SetAvailableDvdSpuTrack(int Type, const char *lang, bool Current)
{
  if(Type >= 0 && Type < 64) {

    m_DvdSpuTrack[Type].id = Type;
    m_DvdSpuTrack[Type].language[0] = '\0';
    if(lang) 
      strn0cpy(m_DvdSpuTrack[Type].language, lang, MAXLANGCODE2);
    if(Current)
      m_CurrentDvdSpuTrack = Type;

    return true;
  }
  return false;
}
#endif

#if VDRVERSNUM < 10515
void cXinelibDevice::EnsureDvdSpuTrack(void)
{
  if(!m_ForcedDvdSpuTrack &&
     NumDvdSpuTracks() > 0 &&
     (m_DvdSpuTrack[0].id == 0xffff ||
      strcmp(m_DvdSpuTrack[0].language, "menu"))) {

    if(xc.spu_autoshow) {
      int pref, len, track;
      for(pref = 0; pref < 4; pref++)
	for(track = 0; track < 64; track++)
	  if(m_DvdSpuTrack[track].id != 0xffff)    
	    if((len=strlen(xc.spu_lang[pref])) > 0)
	      //if(!strncmp(m_DvdSpuTrack[track].language,
	      //            xc.spu_lang[pref], len)) {
	      if(!strcmp(m_DvdSpuTrack[track].language,
			 xc.spu_lang[pref])) {
		if(m_CurrentDvdSpuTrack != track) {
		  LOGMSG("Auto-selecting %d. SPU track \'%s\'  (%d. preferred language is \'%s\')", 
			 track, m_DvdSpuTrack[track].language, pref+1, xc.spu_lang[pref]);
		  cXinelibDevice::SetCurrentDvdSpuTrack(track);
		  cString msg = cString::sprintf("Subtitles: %s", m_DvdSpuTrack[track].language);
		  Skins.QueueMessage(mtInfo, msg);
		}
		m_spuPresent = true;
		track = 64;
		pref = 99;
	      }
      if(pref < 99) {
	for(track = 0; track < 64; track++)
	  if(m_DvdSpuTrack[track].id != 0xffff)
	    if(m_CurrentDvdSpuTrack != track) {
	      LOGMSG("Auto-selecting (non-matching) %d. SPU track \'%s\'",
		     track, m_DvdSpuTrack[track].language);
	      cXinelibDevice::SetCurrentDvdSpuTrack(track);
	    }
      }
    }

    if(!m_spuPresent) {
      Skins.QueueMessage(mtInfo, "Subtitles present");
      m_spuPresent = true;
    }
  }
}
#endif

//
// Metainfo
//

const char *cXinelibDevice::GetMetaInfo(eMetainfoType Type)
{
  if(Type >= 0 && Type < mi_Count) {
    if ((Type == miTitle) || 
        (Type == miTracknumber && xc.playlist_tracknumber == 1) ||
        (Type == miArtist      && xc.playlist_artist == 1) ||
        (Type == miAlbum       && xc.playlist_album == 1) ||
	(Type > miAlbum)) {
      return m_MetaInfo[Type];
    }
    return "";
  }

  LOGMSG("cXinelibDevice::GetMetaInfo: unknown metainfo type");
  return "";
}

void  cXinelibDevice::SetMetaInfo(eMetainfoType Type, const char *Value)
{
  if(Type >= 0 && Type < mi_Count) {
    /* set to 0 first, so if player is accessing string in middle of 
       copying it will always be 0-terminated (but truncated) */
    memset(m_MetaInfo[Type], 0, sizeof(m_MetaInfo[Type]));
    strn0cpy(m_MetaInfo[Type], Value, MAX_METAINFO_LEN);
  } else {
    LOGMSG("cXinelibDevice::SetMetaInfo: unknown metainfo type");
  }
}

