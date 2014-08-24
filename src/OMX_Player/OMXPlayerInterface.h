#ifndef __OMXPLAYER_CLIENT_H
#define __OMXPLAYER_CLIENT_H

#include <mutex> // for mutex
#include "KeyConfig.h" // for action definitions

#include <dbus-c++/dbus.h>  // for d-bus proxies
#include "omxplayer-dbus-glue.h" // d-bus proxy definitions

using namespace std;

// D-Bus access client proxy classes
class OMXPlayerClient
  : public org::mpris::MediaPlayer2::Player_proxy,
//  public DBus::IntrospectableProxy,
//  public DBus::PropertiesProxy,
    public DBus::ObjectProxy
{
  public:
    OMXPlayerClient(DBus::Connection &connection, const char *path, const char *name);

};

class OMXPropsClient
  : public org::freedesktop::DBus::Properties_proxy,
    public DBus::ObjectProxy
{
  public:
    OMXPropsClient(DBus::Connection &connection, const char *path, const char *name);

};

class OMXRootClient
  : public org::mpris::MediaPlayer2::Root_proxy,
    public DBus::ObjectProxy
{
  public:
    OMXRootClient(DBus::Connection &connection, const char *path, const char *name);

};


// client class
void* PlayerMonitor(void *pInstance);
void* PlayerOutputReader(void *pInstance);

class OMXPlayerInterface
{
public:
  // public variables
  enum State { UNKNOWN, INITIALIZING, PLAYING, PAUSED, STOPPING, STOPPED, EXITING };

private:
  // private variables

  // D-Bus data members
  DBus::BusDispatcher dispatcher;
  DBus::Connection *g_player_conn;
  DBus::Connection *g_props_conn;
  DBus::Connection *g_root_conn;
  OMXPlayerClient *g_player_client;
  OMXPropsClient *g_props_client;
  OMXRootClient *g_root_client;

  // child data and threads
  pid_t m_iChildPID;
  bool m_bRunPlayerMonitor;
  pthread_t m_tPlayerMonitorThread;
  bool m_bRunPlayerOutputReader;
  pthread_t m_tPlayerOutputReaderThread;

  // pipes for PlayerOutputReaderThread
  mutex m_mtxWritePipe;
  mutex m_mtxReadPipe;
  int m_iInPipe[2], m_iOutPipe[2];
  bool m_bInPipe[2], m_bOutPipe[2];

  // general
  bool	m_bOMXIsRunning;
  bool	m_bFinished;
  bool	m_bStopped;
  bool	m_bPassthrough;
  bool	m_bStoppedByReader;
  bool	m_bStoppedByMonitor;
  bool  m_bPaused;

  string m_sAudioDevice;

  int	m_iErrCount;

  // mutexes for data access
  mutex m_mtxLocal;
  mutex m_mtxPlayer;
  mutex m_mtxProps;
  mutex m_mtxRoot;
  mutex m_mtxLog;

  // private methods

  // friend functions (threads)
  friend void* PlayerMonitor(void *pInstance);
  friend void* PlayerOutputReader(void *pInstance);

  // player command class methods
  void Send_Quit(void);
  void Send_Stop(void);
  void Send_Action(int);
  void Send_Pause(void);

  // general class methods
  void SetState(State PlayerState);
  bool Get_CanQuit(void);
  void Set_Stopped(void);
  void Set_Finished(void);
  int  Get_ErrCount(void);
  void Set_ErrCount(int count);
  void Inc_Error(void);
  void Log(string txt);
  void OpenPipes(void);
  void ClosePipes(void);
  void Init(void);

public:
  // public methods
  OMXPlayerInterface(string sAudioDevice, bool bPassthrough, string sGpuDeInt);
  ~OMXPlayerInterface();

  void Do_Rewind(void);
  void Do_FastForward(void);
  void Do_Pause(void);
  void Do_SeekBackSmall();
  void Do_SeekForwardSmall();
  void Do_SeekBackLarge();
  void Do_SeekForwardLarge();
  void Do_DecreaseVolume();
  void Do_IncreaseVolume();

  bool Play(string sMediaURL);
  void Stop(void);
  void DeInit(void);

  bool IsStopped(void);
  bool IsFinished(void);

  string Get_Identity(void);
  string Get_PlaybackStatus(void);
  uint64_t Get_Position(void);
  uint64_t Get_Duration(void);


  // *********
  // callbacks
  // *********

  // *********
  // c style callback - player state notifier
  public:
    // fn pointer to store notifier
    void (*m_fpStateNotifier)(OMXPlayerInterface::State PlayerState);

    // fn to register notifier with the class
    void RegisterStateNotifier( void (*Notifier)(OMXPlayerInterface::State PlayerState)) {
      m_fpStateNotifier = Notifier;
    };
  /*
  // example c style callback implementation
  void PlayerStateNotifier(OMXPlayerInterface::State PlayerState) {
        cout << "[NOTIFIER] State: " << PlayerState;

        if (PlayerState == OMXPlayerInterface::State::PLAYER_STOPPED) {
                cout << " STOPPED - setting bStopped=true;" << endl;
                bStopped = true;
        }
  }
  */

  // *********
  // c++ style inheritance based callback implementation - player state notifier
  private:
    virtual void PlayerStateNotifier(OMXPlayerInterface::State PlayerState) {
    };
  /*
  //  example c++ stype inheritance implementation
  // a class derived from this can implement the following function to recieve state notifications
  private:
    void PlayerStateNotifier(OMXPlayerInterface::State PlayerState)
    {
      // do something with PlayerState
    }

  */

};

#endif//__OMXPLAYER_CLIENT_H
