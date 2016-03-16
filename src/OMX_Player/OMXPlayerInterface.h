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
  enum STATE { UNKNOWN, INITIALIZING, PLAYING, PAUSED, STOPPING, STOPPED, EXITING };

private:
  // private variables
  string m_sMediaURL;

  // D-Bus data members
  DBus::BusDispatcher dispatcher;
  DBus::Connection *g_player_conn;
  DBus::Connection *g_props_conn;
  DBus::Connection *g_root_conn;
  OMXPlayerClient *g_player_client;
  OMXPropsClient *g_props_client;
  OMXRootClient *g_root_client;
  std::string m_s_dbus_addr;

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

  // state
  STATE ePlayerState = STATE::UNKNOWN;

  // general
  bool	m_bOMXIsRunning;
  bool	m_bFinished;
  bool	m_bStopped;
  bool	m_bPassthrough;
  bool	m_bStoppedByReader;
  bool	m_bStoppedByMonitor;
  bool  m_bPaused;
  bool  m_bSubtitlesShowing = false;

  string m_sAudioDevice;

  int	m_iErrCount;

  // mutexes for data access
  mutex m_mtxLocal;
  mutex m_mtxPlayer;
  mutex m_mtxProps;
  mutex m_mtxRoot;
  mutex m_mtxLog;

  // private methods
  bool Connect_Player(void);
  void Disconnect_Player(void);
  bool Reconnect_Player(void);
  //
  bool Connect_Properties(void);
  void Disconnect_Properties(void);
  bool Reconnect_Properties(void);
  //
  bool Connect_Root(void);
  void Disconnect_Root(void);
  bool Reconnect_Root(void);

  // friend functions (threads)
  friend void* PlayerMonitor(void *pInstance);
  friend void* PlayerOutputReader(void *pInstance);

  // player command class methods
  void Send_Quit(void);
  void Send_Stop(void);
  void Send_Action(int);
  void Send_Pause(void);
  int64_t Send_SetPosition(std::string sMediaPosition, int64_t xPos);
  std::vector< std::string > ListAudio();
  std::vector< std::string > ListSubtitles();

  // general class methods
  void SetState(STATE PlayerState);
  bool Get_CanQuit(void);
  void Set_Stopped(bool val);
  void Set_Finished(void);
  int  Get_ErrCount(void);
  void Set_ErrCount(int count);
  void Inc_Error(void);
  virtual void Log(string txt);
  void OpenPipes(void);
  void ClosePipes(void);
  void QuitOnError(void);
  string ZeroPad(int num, int width);
  string TimeFromUSec(uint64_t usecs);
  int64_t USecFromTime(string sTime);

   /**
     * @brief breaks the Input string in tokens and putes them in the deque_strings parameter, or a vector
     */
  string Tokenize( string &sInput, string sTokens, string::size_type &CurPos, bool bMultiCharacterTokens=false);
//  void Tokenize(string &Input, string Tokens, deque<string> &deque_strings, bool bPushToFront);
//  void Tokenize(string &Input, string Tokens, vector<string> &vect_strings);



public:
  // public methods
  OMXPlayerInterface(string sAudioDevice, bool bPassthrough, string sGpuDeInt);
  virtual ~OMXPlayerInterface();

  std::vector< std::string > ListVideo();
  void HideSubtitles();
  void ShowSubtitles();
  bool SelectSubtitle(int track);
  bool SelectVideo(int track);
  bool SelectAudio(int track);
  int64_t Seek(int64_t);
  void Rewind(void);
  void FastForward(void);
  void Pause(void);
  void SeekBackSmall();
  void SeekForwardSmall();
  void SeekBackLarge();
  void SeekForwardLarge();
  void DecreaseVolume();
  void IncreaseVolume();
  void DecreaseSpeed();
  void IncreaseSpeed();
  void VolumeUp();
  void VolumeDown();
  void Mute();
  void UnMute();
  void SetPosition(std::string sMediaURL, int64_t xPos);
  std::vector< std::string > Get_ListAudio();
  std::vector< std::string > Get_ListVideo();
  std::vector< std::string > Get_ListSubtitles();

  int getCurrentVideo();
  int getMaxVideo();
  int getCurrentAudio();
  int getMaxAudio();
  int getCurrentSubtitle();
  int getMaxSubtitle();
  bool setVideo(int track);
  bool setAudio(int track);
  bool setSubtitle(int track);
  bool getSubtitlesShowing();


  virtual bool Play(string sMediaURL, string sMediaPosition);
  void Stop(void);
  void Init(void);
  void DeInit(void);

  bool IsStopped(void);
  bool IsFinished(void);

  STATE Get_PlayerState(void);

  string Get_Identity(void);
  string Get_PlaybackStatus(void);
  uint64_t Get_Position(void);
  uint64_t Get_Duration(void);
  int Get_Speed(void);
  int Get_Title(void);
  int Get_Chapter(void);
  int Get_Subtitle(void);
  int Get_Audio(void);
  string Get_CurrentFile(void);


  // *********
  // callbacks
  // *********

  // *********
  // c style callback - player state notifier
  public:
    // fn pointer to store notifier
    void (*m_fpStateNotifier)(OMXPlayerInterface::STATE playerState);

    // fn to register notifier with the class
    void RegisterStateNotifier( void (*Notifier)(OMXPlayerInterface::STATE playerState)) {
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
    virtual void PlayerStateNotifier(OMXPlayerInterface::STATE playerState) {
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
