
#include "xineCommon.h"

#include "xineExternal.h"
#include "xineLib.h"



namespace PluginXine
{

  cXineExternal::cXineExternal()
    : cThread()
    , m_fdControl(-1)
    , m_fdResult(-1)
    , m_shutdown(false)
    , m_connected(false)
    , m_xineLib(0)
    , m_enabled(false)
  {
  }

  cXineExternal::~cXineExternal()
  {
    Stop();
  }
  
  void cXineExternal::Start()
  {
    cThread::Start();
  }

  void cXineExternal::Stop()
  {
    cMutexLock shutdownMutexLock(&m_shutdownMutex);
    m_shutdown = true;
    m_shutdownCondVar.Broadcast();

    disconnect();
  }

  bool cXineExternal::readCommand()
  {
    int len = 0;
    
    while (!m_shutdown
           && len < EXTERNAL_COMMAND_MAX_LEN)
    {
      cPoller poller(m_fdControl);
      if (poller.Poll(100))
      {
        void (* const sigPipeHandler)(int) = ::signal(SIGPIPE, SIG_IGN);
        
        int r = ::read(m_fdControl, &m_command[ len ], 1);
        int myErrno = errno;

        ::signal(SIGPIPE, sigPipeHandler);
        
        if (r < 0)
        {
          if (EAGAIN == myErrno)
            return false;

          if (m_connected)
          {
            errno = myErrno;
            ::perror("vdr-xine: reading external command failed");
          }
        }

        if (r <= 0)
        {
          disconnect();
          return false;
        }

        if ('\n' == m_command[ len ])
          break;
        
        len += r;
      } 
    }

    if (m_shutdown)
      return false;

    if (len >= EXTERNAL_COMMAND_MAX_LEN
        && m_command[ EXTERNAL_COMMAND_MAX_LEN - 1 ] != '\n')
    {
      ::fprintf(stderr, "vdr-xine: external command length exceeded!\n");
      disconnect();
      return false;
    }

    m_command[ len ] = '\0';

    return (len > 0);
  }

  bool cXineExternal::writeResult(const char *result)
  {
    int l = ::strlen(result);

    while (l > 0)
    {
      void (* const sigPipeHandler)(int) = ::signal(SIGPIPE, SIG_IGN);
      
      int r = ::write(m_fdResult, &result, l);
      int myErrno = errno;
      
      ::signal(SIGPIPE, sigPipeHandler);
      
      if (r < 0)
      {
        if (EAGAIN == myErrno)
          continue;
          
        errno = myErrno;
        ::perror("vdr-xine: writing external result failed");
      }
        
      if (r <= 0)
      {
        disconnect();
        return false;
      }

      result += r;
      l -= r;
    }

    return true;
  }
  
  void cXineExternal::cmdPlay(const char *const mrl)
  {
    if (m_xineLib)
    {
      m_xineLib->execFuncPlayExternal(mrl);
      m_xineLib->execFuncWait();
    }
  }

  void cXineExternal::enable(const bool enable)
  {
    m_enabled = enable;
  }

  void cXineExternal::externalStreamFinished()
  {
    ::fprintf(stderr, "vdr-xine: external stream finished\n");
        
    cmdPlay(0);

    writeResult("play finished\n");
  }
  
  void cXineExternal::Action(void)
  {
    while (!m_shutdown)
    {
      if (!isConnected())
      {
//        ::fprintf(stderr, "?");
        checkConnect();
      }
      else
      {
//        ::fprintf(stderr, "!");

        if (readCommand())
        {
          if (!m_enabled)
          {
            ::fprintf(stderr, "vdr-xine: external commands not allowed!\n");
            disconnect();
          }
          else
          {
            if (0 == strncmp(m_command, "play ", 5))
            {
              cmdPlay(m_command + 5);
            }
            else
            {
              ::fprintf(stderr, "vdr-xine: external command '%s' unknown!\n", m_command);
              disconnect();
            }
          }
        }          
      }

      if (!m_shutdown)
      {
        cMutexLock shutdownMutexLock(&m_shutdownMutex);
        if (!m_shutdown)
          m_shutdownCondVar.TimedWait(m_shutdownMutex, 100);
      }
    }
  }
  
  bool cXineExternal::isConnected()
  {
    return (-1 != m_fdControl);
  }

  bool cXineExternal::checkConnect()
  {
    m_fdResult  = ::open(m_xineLib->m_fifoNameExtResult.c_str() , O_WRONLY | O_NONBLOCK);
    if (-1 == m_fdResult)
      return false;
    
    ::fprintf(stderr, "vdr-xine: external connecting ...\n");
    
    m_fdControl = ::open(m_xineLib->m_fifoNameExtControl.c_str(), O_RDONLY);
    
    ::fcntl(m_fdResult, F_SETFL, ~O_NONBLOCK & ::fcntl(m_fdResult, F_GETFL, 0));
    
    if (isConnected())
    {
      ::fprintf(stderr, "vdr-xine: external connected!\n");

      m_connected = true;      
      return true;
    }
    
    ::fprintf(stderr, "vdr-xine: external connect failed!\n");
    
    return false;
  }

  void cXineExternal::disconnect()
  {
    m_connected = false;

    cmdPlay(0);
    
    if (-1 != m_fdControl)
    {
      ::close(m_fdControl);
      m_fdControl = -1;
    }

    if (-1 != m_fdResult)
    {
      ::close(m_fdResult);
      m_fdResult = -1;
    }
  }
  
};
