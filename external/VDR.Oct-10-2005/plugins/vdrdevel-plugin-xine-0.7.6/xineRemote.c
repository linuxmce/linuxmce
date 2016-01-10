
#include "xineCommon.h"

#include "xineLib.h"



namespace PluginXine
{
  
  static eKeys KeyMap[] =
  {
    kNone,
    kUp,
    kDown,
    kMenu,
    kOk,
    kBack,
    kLeft,
    kRight,
    kRed,
    kGreen,
    kYellow,
    kBlue,
    k0, k1, k2, k3, k4, k5, k6, k7, k8, k9,
    kPlay,
    kPause,
    kStop,
    kRecord,
    kFastFwd,
    kFastRew,
    kPower,
    kChanUp,
    kChanDn,
    kVolUp,
    kVolDn,
    kMute,
    kSchedule,
    kChannels,
    kTimers,
    kRecordings,
    kSetup,
    kCommands,
    kUser1, kUser2, kUser3, kUser4, kUser5, kUser6, kUser7, kUser8, kUser9,
#if VDRVERSNUM >= 10318
    kAudio,
#endif
  };

  cXineRemote::cXineRemote(const bool remoteOn)
    : cRemote("XineRemote")
    , cThread()
    , m_remoteOn(remoteOn)
    , m_xineLib(0)
  {
#if VDRVERSNUM >= 10300
    SetDescription("XineRemote control");
#endif
    
    // ::fprintf( stderr, "XineRemote constructor\n");
    m_active = false;
    Start ();  // calls Action
  }
  
  cXineRemote::~cXineRemote()
  {
    {
      cMutexLock activeMutexLock(&m_activeMutex);
      m_active = false;
      m_activeCondVar.Broadcast();
    }
    
    // Close();
    Cancel(3);
  }
  
  void cXineRemote::setXineLib(cXineLib *const xineLib)
  {
    m_xineLib = xineLib;
  }

  bool cXineRemote::isConnected()
  {
    if (!m_xineLib)
      return false;

    return m_xineLib->isConnected();
  }
  
  void cXineRemote::Action (void)
  {
    dsyslog ("Entering cXineRemote thread\n");

    bool externalStreamFinished = false;
    bool osdReshowRequired = false;
    int frameLeft = 0;
    int frameTop = 0;
    int frameWidth = 0;
    int frameHeight = 0;
    
    m_active = true;
    
    while (m_active)
    {
      if (isConnected())
      {
        if (osdReshowRequired)
        {
          osdReshowRequired = false;

          if (m_xineLib)
            m_xineLib->ReshowCurrentOSD(frameWidth, frameHeight);
        }
        
        if (externalStreamFinished)
        {
          externalStreamFinished = false;

          if (m_xineLib)
            m_xineLib->ExternalStreamFinished();
        }
        
        cPoller Poller(m_xineLib->getRemoteFD());
        errno = 0;
        if (Poller.Poll(100)
            && 0 == errno)
        {
          event_union_t event_union;
          
          //printf( "fd_fifo read: %d\n", fd_fifo0);
          
          if (isConnected())
          {
            int n = m_xineLib->xread(m_xineLib->getRemoteFD(), (char *)&event_union.header, sizeof (event_union.header));
            if (n == sizeof(event_union.header))
            {
              // could check that we have header we expected
              //::fprintf(stderr, "got Key: %d\n", userInput.key);
              
              if (func_key == event_union.header.func)
              {
                event_key_t *event = &event_union.key;
                int n = m_xineLib->xread(m_xineLib->getRemoteFD(), (char *)event + sizeof (event->header), sizeof (*event) - sizeof (event->header));

                if (n == sizeof (*event) - sizeof (event->header))
                {
                  if (m_remoteOn)
                  {
                    cRemote::Put( KeyMap[ event->key ]);
                    //::fprintf(stderr, "Putting Key: 0x%08x\n",  KeyMap[ event->key]);
                  }
                }
                else
                {
                  m_xineLib->disconnect();
                }
              }
              else if (func_frame_size == event_union.header.func)
              {
                event_frame_size_t *event = &event_union.frame_size;
                int n = m_xineLib->xread(m_xineLib->getRemoteFD(), (char *)event + sizeof (event->header), sizeof (*event) - sizeof (event->header));

                if (n == sizeof (*event) - sizeof (event->header))
                {
                  osdReshowRequired = true;

                  frameLeft   = event->left;
                  frameTop    = event->top;
                  frameWidth  = event->width;
                  frameHeight = event->height;

                  fprintf(stderr, "osd: (%d, %d)-(%d, %d)\n", frameLeft, frameTop, frameWidth, frameHeight);
                }
                else
                {
                  m_xineLib->disconnect();
                }
              }
              else if (func_play_external == event_union.header.func)
              {
                event_play_external_t *event = &event_union.play_external;
                int n = m_xineLib->xread(m_xineLib->getRemoteFD(), (char *)event + sizeof (event->header), sizeof (*event) - sizeof (event->header));

                if (n == sizeof (*event) - sizeof (event->header))
                {
                  externalStreamFinished = true;
                }
                else
                {
                  m_xineLib->disconnect();
                }
              }
              else
              {
                ::fprintf(stderr, "vdr-xine: unknown event %d!\n", event_union.header.func);
                m_xineLib->disconnect();
              }
            }
            else
            {
              m_xineLib->disconnect();
            }
          }
        }
      }
      else
      {
        if (m_active)
        {
          cMutexLock activeMutexLock(&m_activeMutex);
          if (m_active)
            m_activeCondVar.TimedWait(m_activeMutex, 100);
        }
      }
    }
    
    dsyslog ("Leaving cXineRemote thread\n");
  }

}; // namepsace PluginXine


