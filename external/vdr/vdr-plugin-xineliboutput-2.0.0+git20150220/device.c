/*
 * device.c: xine-lib output device for the Video Disk Recorder
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: device.c,v 1.127 2013-08-21 09:40:22 phintuka Exp $
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

#include "logdefs.h"
#include "config.h"
#include "osd.h"

#include "tools/listiter.h"
#include "tools/mpeg.h"
#include "tools/pes.h"
#include "tools/ts.h"
#include "tools/functor.h"
#include "tools/section_lock.h"

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
    virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber, bool LiveView);
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
					  int ChannelNumber, bool LiveView)
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

  memset(m_MetaInfo, 0, sizeof(m_MetaInfo));

  m_PlayMode = pmNone;
  m_AudioChannel = 0;

  m_liveMode    = true;
  m_TrickSpeed  = -1;
  m_TrickSpeedMode = 0;
  m_SkipAudio   = false;
  m_PlayingFile = pmNone;
  m_StreamStart = true;
  m_RadioStream = false;
  m_AudioCount  = 0;
  m_FreeBufs = 0;

  m_VideoSize = (video_size_t*)calloc(1, sizeof(video_size_t));
  m_tssVideoSize = NULL;

  TsBufferClear();
}

cXinelibDevice::~cXinelibDevice() 
{
  TRACEF("cXinelibDevice::~cXinelibDevice");

  StopDevice();

  m_pInstance = NULL;

  free (m_VideoSize);
  ts_state_dispose(m_tssVideoSize);
}

bool cXinelibDevice::InitDevice()
{
  TRACEF("cXinelibDevice::InitDevice");

  if (m_local || m_server) {
    LOGMSG("cXinelibDevice::InitDevice() called twice");
    return false;
  }

  if (*xc.local_frontend && strncmp(xc.local_frontend, "none", 4))
    m_clients.Add(m_local = new cXinelibLocal(this, xc.local_frontend));
  if (xc.remote_mode && xc.listen_port > 0)
    m_clients.Add(m_server = new cXinelibServer(this, xc.listen_port));

  return true;
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
    while (!m_local->IsReady()) {
      cCondWait::SleepMs(100);
      if (!m_local->Active()) {
        LOGMSG("cXinelibDevice::Start(): Local frontend init failed");
        return false;
      }
      if (++timer >= LOCAL_INIT_TIMEOUT*10) {
        LOGMSG("cXinelibDevice::Start(): Local frontend init timeout");
        return false;
      }
    }
    if(xc.force_primary_device)
      ForcePrimaryDevice(true);
    else if (cDevice::PrimaryDevice() && this != cDevice::PrimaryDevice()) {
      LOGMSG("WARNING: xineliboutput is not the primary device !");
    }
  }

  if(m_server) {
    int timer = 0;
    while(!m_server->IsReady()) {
      cCondWait::SleepMs(100);
      if (!m_server->Active()) {
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

  if(server)
    delete server;
  if(local)
    delete local;

  m_clients.Clear();
}

//
// Primary device switching
//

void cXinelibDevice::MakePrimaryDevice(bool On)
{
  TRACEF("cXinelibDevice::MakePrimaryDevice");

  cDevice::MakePrimaryDevice(On);

  if(On)
    new cXinelibOsdProvider(this);
}

bool cXinelibDevice::ForcePrimaryDevice(bool On)
{
  TRACEF("cXinelibDevice::ForcePrimaryDevice");

  m_MainThreadLock.Lock();
  m_MainThreadFunctors.Add(CreateFunctor(this, &cXinelibDevice::ForcePrimaryDeviceImpl, On));
  m_MainThreadLock.Unlock();

  return xc.force_primary_device ||
         (cDevice::PrimaryDevice() && this == cDevice::PrimaryDevice());
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

void cXinelibDevice::ConfigureVideo(int hue, int saturation, int brightness, int sharpness,
				    int noise_reduction, int contrast, int overscan, int vo_aspect_ratio)
{
  TRACEF("cXinelibDevice::ConfigureVideo");

  if(m_local)
    m_local->ConfigureVideo(hue, saturation, brightness, sharpness, noise_reduction, contrast, overscan, vo_aspect_ratio);
  if(m_server)
    m_server->ConfigureVideo(hue, saturation, brightness, sharpness, noise_reduction, contrast, overscan, vo_aspect_ratio);
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
				     int aspect, int scale_video)
{
  TRACEF("cXinelibDevice::ConfigureWindow");

  if((!*xc.local_frontend || !strncmp(xc.local_frontend, "none", 4)) && m_local) {
    cXinelibThread *tmp = m_local;
    m_clients.Del(tmp, false);
    m_local = NULL;
    cCondWait::SleepMs(5);
    delete tmp;
    if(xc.force_primary_device)
      ForcePrimaryDevice(false);
  }

  if(m_local)
    m_local->ConfigureWindow(fullscreen, width, height, modeswitch, modeline, 
			     aspect, scale_video);

  else if(*xc.local_frontend && strncmp(xc.local_frontend, "none", 4)) {
    cXinelibThread *tmp = new cXinelibLocal(this, xc.local_frontend);
    tmp->Start();
    m_clients.Add(m_local = tmp);

    cCondWait::SleepMs(25);
    while (!m_local->IsReady() && m_local->Active())
      cCondWait::SleepMs(25);

    if (!m_local->Active()) {
      m_local = NULL;
      m_clients.Del(tmp, true);
      Skins.QueueMessage(mtError, tr("Frontend initialization failed"), 10);
    } else {
      if(xc.force_primary_device)
	ForcePrimaryDevice(true);
  
      m_local->ConfigureWindow(fullscreen, width, height, modeswitch, modeline,
			       aspect, scale_video);
    }
  }
}

void cXinelibDevice::Listen(bool activate, int port)
{
  TRACEF("cXinelibDevice::Listen");

  if(activate && port>0) {
    if(!m_server) {
      cXinelibThread *tmp = new cXinelibServer(this, port);
      tmp->Start();
      m_clients.Add(m_server = tmp);

      cCondWait::SleepMs(10);
      while (!m_server->IsReady() && m_server->Active())
	cCondWait::SleepMs(10);

      if (!m_server->Active()) {
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
  m_TrickSpeed = -1;

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
#define trs_NoAudio    0x08  // no audio in trick speed mode
#define trs_Backward   0x40  // palying backwards -- same as regen pts ???

void cXinelibDevice::TrickSpeed(int Speed) 
{
  TRACEF("cXinelibDevice::TrickSpeed");

  if(m_TrickSpeed != Speed) {
    int RealSpeed = abs(Speed);
    LOGTRICKSPEED("TrickSpeed changed from %d to %d [%d]", m_TrickSpeed, Speed, RealSpeed);

    m_TrickSpeed = Speed;

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

      m_TrickSpeedMode = trs_I_frames | trs_Backward | trs_NoAudio;
      ForEach(m_clients, &cXinelibThread::TrickSpeed, RealSpeed, true);
    }

    else if(Speed == 6 || Speed == 3 || Speed == 1) {
      RealSpeed = 12/Speed;
      LOGTRICKSPEED("    Fast (%dx speed), direction unknown", RealSpeed);

      if (m_StreamStart) {
        LOGTRICKSPEED("    Fast (%dx speed), STREAM START --> BACKWARDS", RealSpeed);
      }

      if(RealSpeed > xc.max_trickspeed) {
	RealSpeed = xc.max_trickspeed;
	LOGTRICKSPEED("    Trick speed limited to %dx speed", RealSpeed);
      }

      if (m_StreamStart || (m_TrickSpeedMode & trs_Backward)) {
        m_TrickSpeedMode |= trs_I_frames | trs_Backward | trs_NoAudio;

        ForEach(m_clients, &cXinelibThread::TrickSpeed, -RealSpeed, true);

      } else {

        m_TrickSpeedMode |= trs_IPB_frames;

        ForEach(m_clients, &cXinelibThread::TrickSpeed, -RealSpeed);
      }
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

    ForEach(m_clients, &cXinelibThread::Sync);
  }
}

void cXinelibDevice::Clear(void) 
{
  TRACEF("cXinelibDevice::Clear");
  TRACK_TIME(100);

  TsBufferClear();

  m_StreamStart = true;
  m_FreeBufs = 0;
  TrickSpeed(-1);
  ForEach(m_clients, &cXinelibThread::Clear);
  ForEach(m_clients, &cXinelibThread::SetStillMode, false);
}

void cXinelibDevice::Play(void) 
{
  TRACEF("cXinelibDevice::Play");

  m_SkipAudio  = false;

  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  ForEach(m_clients, &cXinelibThread::SetStillMode, false);
  TrickSpeed(-1);
  cDevice::Play();
}

void cXinelibDevice::Freeze(void) 
{
  TRACEF("cXinelibDevice::Freeze");

  TsBufferFlush();
  TrickSpeed(0);
  cDevice::Freeze();
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

  TsBufferFlush();

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

int cXinelibDevice::PlayFileCtrl(const char *Cmd, int TimeoutMs)
{
  TRACEF("cXinelibDevice::PlayFileCtrl");
  int result = -1;

  /*if(m_PlayingFile != pmNone)*/ {
    if(m_server)
      result = m_server->PlayFileCtrl(Cmd, TimeoutMs);
    if(m_local) 
      result = m_local->PlayFileCtrl(Cmd, TimeoutMs);
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
			      bool LoopPlay, ePlayMode PlayMode,
			      int TimeoutMs)
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
      result = m_server->PlayFile(FileName, Position, LoopPlay, PlayMode, TimeoutMs);
    if(m_local)
      result = m_local->PlayFile(FileName, Position, LoopPlay, PlayMode, TimeoutMs);
  } else if(/*!FileName &&*/m_PlayingFile != pmNone) {
    if(m_server) 
      result = m_server->PlayFile(NULL, 0, 0, pmNone, TimeoutMs);
    if(m_local)
      result = m_local->PlayFile(NULL, 0, 0, pmNone, TimeoutMs);
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

int cXinelibDevice::PlayAny(const uchar *buf, int length) 
{
  TRACEF("cXinelibDevice::PlayAny");
  TRACK_TIME(100);

#if 0
  if(m_PlayingFile)
    return length;
#endif

  if (!buf || length <= 0)
    return length;

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

  bool isMpeg1 = false;
  if (DATA_IS_PES(buf)) {
    isMpeg1 = pes_is_mpeg1(buf);
    int len = pes_packet_len(buf, length);
    if (len>0 && len != length)
      LOGMSG("cXinelibDevice::PlayAny: invalid data !");
  }

  if(m_TrickSpeed > 0) {
  } else if(m_SkipAudio) {
    /* needed for still images when moving cutting marks */
    if (DATA_IS_PES(buf))
      pes_change_pts((uchar*)buf, length, INT64_C(0));
  }
  m_FreeBufs --;

  if(m_local) {
    length = (isMpeg1 ? m_local->Play_Mpeg1_PES(buf,length) :
                        m_local->Play(buf, length));
  }
  if(m_server && length > 0) {
    int length2 = isMpeg1 ? m_server->Play_Mpeg1_PES(buf, length) :
                            m_server->Play(buf, length);
    if(!m_local)
      return length2;
  }

  return length;
}

/*
 * hook to PlayTs() to get PAT and PMT
 */
int cXinelibDevice::PlayTs(const uchar *Data, int Length, bool VideoOnly)
{
  if (!Data || Length < TS_SIZE) {
    TsBufferFlush();
    return cDevice::PlayTs(Data, Length, VideoOnly);
  }

  /* Play single TS pack */
  int Result = cDevice::PlayTs(Data, TS_SIZE, VideoOnly);
  if (Result != TS_SIZE)
    return Result;

  /* Grab PAT and PMT */
  if (TsHasPayload(Data) && TsPayloadOffset(Data) < TS_SIZE) {

    int Pid = TsPid(Data);
    if (Pid == PATPID || PatPmtParser()->IsPmtPid(Pid)) {
      if (m_server)
        m_server->SetHeader(Data, Result, Pid == 0);

      if (PlayTsAny(Data, Result) != Result)
        LOGMSG("Lost PAT/PMT fragment !");

      TsBufferFlush();

      /* detect radio streams */
      int patv, pmtv;
      if (PatPmtParser()->GetVersions(patv, pmtv)) {
        if (!PatPmtParser()->Vpid() && PatPmtParser()->Apid(0)) {
          m_RadioStream = true;
          m_AudioCount  = 0;
          ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
        }
      }
    }
  }

  return Result;
}

/*
 * TS buffer
 */

#define TS_BUFFER_LOCK cSectionLock SectionLock(m_TsBufLock)

int cXinelibDevice::TsBufferFlush(void)
{
  TS_BUFFER_LOCK;

  if (m_TsBufSize) {
    int n;
    if ((n = PlayAny(m_TsBuf, m_TsBufSize)) == (int)m_TsBufSize) {
      m_TsBufSize = 0;
      return n;
    }
    if (n)
      LOGMSG("cXinelibDevice::TsBufferFlush: error: cache not flushed (%d %d)", n, m_TsBufSize);
    errno = EAGAIN;
  }
  return 0;
}

void cXinelibDevice::TsBufferClear(void)
{
  TS_BUFFER_LOCK;
  m_TsBufSize = 0;
}

int cXinelibDevice::PlayTsAny(const uchar *buf, int length)
{
  if (!DATA_IS_TS(buf))
    LOGMSG("PlayTsAny(): TS SYNC byte missing !");
  if (length != TS_SIZE)
    LOGMSG("PlayTsAny(): length == %d !", length);

  TS_BUFFER_LOCK;

  // cache full ? try to flush it
  if (m_TsBufSize >= 2048)
    if (!TsBufferFlush())
      return 0;

  // add packet to cache
  memcpy(m_TsBuf + m_TsBufSize, buf, length);
  m_TsBufSize += length;

  // time to flush ?
  if (m_TsBufSize >= 2048-TS_SIZE-1)
    TsBufferFlush();

  return length;
}

int cXinelibDevice::PlayTsSubtitle(const uchar *Data, int Length)
{
  if (!xc.dvb_subtitles)
    return cDevice::PlayTsSubtitle(Data, Length);

  return PlayTsAny(Data, Length);
}

int cXinelibDevice::PlayTsAudio(const uchar *Data, int Length)
{
  if (!AcceptAudioPacket(Data, Length))
    return Length;

  return PlayTsAny(Data, Length);
}

int cXinelibDevice::PlayTsVideo(const uchar *Data, int Length)
{
  if (ts_PID(Data) == PatPmtParser()->Vpid()) {

    if (!AcceptVideoPacket(Data, Length))
      return Length;

    if (m_StreamStart) {
      if (!m_tssVideoSize)
        m_tssVideoSize = ts_state_init(4096);

      if (ts_get_video_size(m_tssVideoSize, Data, m_VideoSize,
                            (PatPmtParser()->Vtype() == ISO_14496_PART10_VIDEO))) {

        m_StreamStart = false;
        LOGMSG("Detected video size %dx%d", m_VideoSize->width, m_VideoSize->height);
        ForEach(m_clients, &cXinelibThread::SetHDMode, (m_VideoSize->width > 800));

        ts_state_dispose(m_tssVideoSize);
        m_tssVideoSize = NULL;
      }
    }
  }

  return PlayTsAny(Data, Length);
}

bool cXinelibDevice::AcceptVideoPacket(const uchar *Data, int Length)
{
  if (!Data || Length < 6)
    return false;

  if (m_PlayMode == pmAudioOnlyBlack)
    return false;

  if (m_PlayingFile && (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmVideoOnly))
    return false;

  if (m_RadioStream) {
    m_RadioStream = false;
    m_AudioCount  = 0;
    ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
  }

  return true;
}

bool cXinelibDevice::AcceptAudioPacket(const uchar *Data, int Length)
{
  if (!Data || Length < 6)
    return false;

  if (m_PlayingFile && (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmAudioOnly))
    return false;

  // strip audio in trick speed modes and when displaying still images
  if (m_SkipAudio)
    return false;
  if (m_TrickSpeedMode & trs_NoAudio)
    return false;

  if (m_RadioStream) {
    if (m_AudioCount > 0) {
      m_AudioCount--;
      if (m_AudioCount <= 0) {
	LOGDBG("PlayAudio detected radio stream");
	ForEach(m_clients, &cXinelibThread::SetNoVideo, m_RadioStream);
      }
    }
  }

  return true;
}

int cXinelibDevice::PlayVideo(const uchar *buf, int length) 
{
  TRACEF("cXinelibDevice::PlayVideo");
  TRACK_TIME(100);

  if (!AcceptVideoPacket(buf, length))
    return length;

  if (!DATA_IS_PES(buf)) {
    LOGMSG("PlayVideo: data is not PES !");
    return length;
  }

  if(m_StreamStart) {
    bool h264 = pes_is_frame_h264(buf, length);
    if (h264) {
      LOGMSG("cXinelibDevice::PlayVideo: Detected H.264 video");
    }

    if (pes_get_video_size(buf, length, m_VideoSize, h264)) {
      m_StreamStart = false;
      LOGDBG("Detected video size %dx%d", m_VideoSize->width, m_VideoSize->height);
      ForEach(m_clients, &cXinelibThread::SetHDMode, (m_VideoSize->width > 800));
    }
  }

#ifdef DEBUG_SWITCHING_TIME
  if(m_statusMonitor->switchtimeOff && m_statusMonitor->switchtimeOn) {
    if (pes_get_picture_type(buf, length) == I_FRAME)
      m_statusMonitor->IFrame();
  }
#endif

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

  // Data type

  bool isPes   = DATA_IS_PES(Data) && ((Data[3] & 0xF0) == 0xE0);
  bool isMpeg1 = isPes && ((Data[6] & 0xC0) != 0x80);
  bool isH264  = isPes && pes_is_frame_h264(Data, Length);
  bool isTs    = DATA_IS_TS(Data);

  int i;

  if(m_PlayingFile && (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmVideoOnly))
    return;

  TsBufferFlush();

  ForEach(m_clients, &cXinelibThread::Clear);
  ForEach(m_clients, &cXinelibThread::SetNoVideo, false);
  ForEach(m_clients, &cXinelibThread::SetLiveMode, false);
  ForEach(m_clients, &cXinelibThread::ResumeOutput);
  ForEach(m_clients, &cXinelibThread::SetStillMode, true);
  ForEach(m_clients, &cXinelibThread::Sync);

  m_TrickSpeed = -1; // to make Poll work ...
  m_SkipAudio = 1;   // enables audio and pts stripping

  for (i = 0; i < STILLPICTURE_REPEAT_COUNT; i++) {
    if(isMpeg1) {
      ForEach(m_clients, &cXinelibThread::Play_Mpeg1_PES, Data, Length, 
	      &mmin<int>, Length);
    } else if(isPes) {
      /*cDevice::*/PlayPes(Data, Length, m_SkipAudio);
    } else if(isTs) {
      int written = 0, total = (Length/TS_SIZE)*TS_SIZE;
      while (written < total) {
        int n = PlayTs(Data+written, Length-written, m_SkipAudio);
        if (n > 0)
          written += n;
        else
          cCondWait::SleepMs(5);
      }
      TsBufferFlush();
    } else {
      ForEach(m_clients, &cXinelibThread::Play_Mpeg2_ES,
              Data, Length, VIDEO_STREAM, isH264,
              &mand<bool>, true);
    }
  }

  TsBufferFlush();

#if 0
  // creates empty video PES with pseudo-pts
  ForEach(m_clients, &cXinelibThread::Play_Mpeg2_ES,
          Data, 0, VIDEO_STREAM, isH264,
          &mand<bool>, true);
#endif

  ForEach(m_clients, &cXinelibThread::Flush, 60, &mand<bool>, true);

  m_TrickSpeed = 0;  // --> Play() triggers TrickSpeed change and resets still mode
  m_SkipAudio = 0;
}

int cXinelibDevice::PlayAudio(const uchar *buf, int length, uchar Id) 
{
  TRACEF("cXinelibDevice::PlayAudio");
  TRACK_TIME(100);

  if (!AcceptAudioPacket(buf, length))
    return length;

  return PlayAny(buf, length);
}

int cXinelibDevice::PlaySubtitle(const uchar *Data, int Length)
{
  if(!xc.dvb_subtitles)
    return cDevice::PlaySubtitle(Data, Length);
  return PlayAny(Data, Length);
}

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

void cXinelibDevice::GetVideoSize(int &Width, int &Height, double &VideoAspect)
{
  Width  = m_VideoSize->width;
  Height = m_VideoSize->height;
  VideoAspect = 1.0;
  if (m_VideoSize->pixel_aspect.den) {
    VideoAspect = (double)m_VideoSize->pixel_aspect.num / (double)m_VideoSize->pixel_aspect.den;
    VideoAspect *= (double)Width / (double)Height;
  }
}

void cXinelibDevice::GetOsdSize(int &Width, int &Height, double &PixelAspect)
{
  switch (xc.osd_size) {
    case OSD_SIZE_720x576:
      Width  = 720;
      Height = 576;
      break;
    case OSD_SIZE_1280x720:
      Width  = 1280;
      Height = 720;
      break;
    case OSD_SIZE_1920x1080:
      Width  = 1920;
      Height = 1080;
      break;
    case OSD_SIZE_stream:
      if (m_VideoSize->width > 0 && m_VideoSize->height > 0) {
        Width  = max<int>(m_VideoSize->width,  512);
        Height = max<int>(m_VideoSize->height, 480);
        break;
      }
    case OSD_SIZE_auto:
      if (xc.osd_width_auto > 0 && xc.osd_height_auto > 0) {
        Width  = xc.osd_width_auto;
        Height = xc.osd_height_auto;
        break;
      }
    case OSD_SIZE_custom:
    default:
      Width  = xc.osd_width;
      Height = xc.osd_height;
      break;
  }
  PixelAspect = 16.0 / 9.0 / (double)Width * (double)Height;
}

bool cXinelibDevice::SupportsTrueColorOSD(void)
{
  switch (xc.osd_color_depth) {
    default:
    case OSD_DEPTH_LUT8:      return false;
    case OSD_DEPTH_TRUECOLOR: return true;
    case OSD_DEPTH_auto:;
  }

  // Automatic mode:
  // Use TrueColor if all clients support it.
  // Special handling when no remote clients:
  //  - local frontend supports TrueColor --> use TrueColor
  //  - no local frontend --> use LUT8

  if (m_local) {
    if (!m_local->SupportsTrueColorOSD())
      return false;
    return !m_server || !!m_server->SupportsTrueColorOSD();
  }

  return m_server && (m_server->SupportsTrueColorOSD() == 1);
}

cRect cXinelibDevice::CanScaleVideo(const cRect &Rect, int Alignment)
{
  return Rect;
}

void cXinelibDevice::ScaleVideo(const cRect &Rect)
{
  if (m_VideoWindow != Rect) {
    m_VideoWindow = Rect;
    cXinelibOsdProvider::RefreshOsd();
  }
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
// Available DVD SPU tracks
//

void cXinelibDevice::SetSubtitleTrackDevice(eTrackType Type)
{
  if (m_PlayingFile == pmAudioVideo || m_PlayingFile == pmVideoOnly)
    ForEach(m_clients, &cXinelibThread::SetSubtitleTrack, Type);
}

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

//
// Picture-In-Picture
//

int cXinelibDevice::Pip_Open(void)
{
  for (int i = 0; i < MAX_NUM_PIP; i++)
    if (!m_PipPid[i]) {
      m_PipPid[i] = i;
      ForEach(m_clients, &cXinelibThread::Pip_Config, i, -1, -1, -1, -1);
      return i;
    }
  return -1;
}

void cXinelibDevice::Pip_Config(int Index, int X, int Y, int W, int H)
{
  if (Index >= 0 && Index < MAX_NUM_PIP) {
    if (m_PipPid[Index]) {
      ForEach(m_clients, &cXinelibThread::Pip_Config, Index, X, Y, W, H);
    }
  }
}

int  cXinelibDevice::Pip_Play(int Index, uint8_t *Data, int Length)
{
  if (Index >= 0 && Index < MAX_NUM_PIP) {
    if (m_PipPid[Index] && Data && Length > 0) {

      int len = Length;

      if (m_local)
        len = m_local->Play(Data, len, eStreamId(sidPipFirst + Index));

      if (m_server) {
        int len2 = m_server->Play(Data, len, eStreamId(sidPipFirst + Index));
        if (!m_local)
          return len2;
      }

      return len;
    }
  }
  return Length;
}

void cXinelibDevice::Pip_Close(int Index)
{
  if (Index >= 0 && Index < MAX_NUM_PIP) {
    if (m_PipPid[Index]) {
      ForEach(m_clients, &cXinelibThread::Pip_Close, Index);
      m_PipPid[Index] = 0;
    }
  }
}

