#include <mutex> // mutex
#include <string> // to_string
#include <iostream> // cout
#include <fstream> // ifstream
#include <unistd.h> // execl/usleep
#include <sys/wait.h> // waitpid()
#include <fcntl.h> // fcntl (pipe redirection)

//#define USE_PIPES

const int RETRIES = 10;
const int DBUS_RETRIES = 50;

using namespace std;

#include "OMXPlayerInterface.h"
#include "KeyConfig.h"


// D-Bus interfaces proxies
OMXPlayerClient::OMXPlayerClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}

OMXPropsClient::OMXPropsClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}

OMXRootClient::OMXRootClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}


// omx client start
static const char *OMXPLAYER_SERVER_NAME = "org.mpris.MediaPlayer2.omxplayer";
static const char *OMXPLAYER_SERVER_PATH = "/org/mpris/MediaPlayer2";

static const string OMXPLAYER_DBUS_ADDR = "/tmp/omxplayerdbus";
#define OMXPLAYER_DBUS_PID /tmp/omxplayerdbus.pid

// constructor
OMXPlayerInterface::OMXPlayerInterface(string sAudioDevice, bool bPassthrough, string sGpuDeInt) {
  // callback fn's set NULL so we know it's not registered
  m_fpStateNotifier = NULL;

  m_bOMXIsRunning = false;

  g_player_client = NULL;
  g_props_client = NULL;
  g_root_client = NULL;
  g_player_conn = NULL;
  g_props_conn = NULL;
  g_root_conn = NULL;

  m_bFinished = false;
  m_bStopped = true;
  m_sAudioDevice = sAudioDevice;
  m_bPassthrough = bPassthrough;

  m_bRunPlayerOutputReader = false;
  m_bRunPlayerMonitor = false;
  m_tPlayerOutputReaderThread = 0;
  m_tPlayerMonitorThread = 0;

  m_iChildPID = 0;

  m_bStoppedByReader = false;
  m_bStoppedByMonitor = false;
  m_iErrCount = 0;

  m_bInPipe[0] = m_bInPipe[1] = false;
  m_bOutPipe[0] = m_bOutPipe[1] = false;

  Set_Stopped(true);

  DBus::_init_threading();
  DBus::default_dispatcher = &dispatcher;
}

// destructor
OMXPlayerInterface::~OMXPlayerInterface() {
    Log("~OMXPlayerInterface() - DESTRUCTOR! - Calling DeInit()");
    SetState(STATE::EXITING);

    DeInit();
}

void OMXPlayerInterface::DeInit() {
  if (!IsFinished()) {
    Log("DeInit() - Calling Stop()"); 
    Stop();
    while (!IsFinished()) usleep(1000);
  }
}

void OMXPlayerInterface::Init() {
	if ( ePlayerState != STATE::UNKNOWN ) {
		Log("OMXPlayerInterface::Init - ERROR - Init() has already been run.");
		return;
	}
	SetState(STATE::STOPPED);
}


void OMXPlayerInterface::Do_DecreaseVolume() {
    Log("Do_DecreaseVolume() - calling Send_Action(KeyConfig::ACTION_DECREASE_VOLUME)");
    Send_Action(KeyConfig::ACTION_DECREASE_VOLUME);
}

void OMXPlayerInterface::Do_IncreaseVolume() {
    Log("Do_IncreaseVolume() - calling Send_Action(KeyConfig::ACTION_INCREASE_VOLUME)");
    Send_Action(KeyConfig::ACTION_INCREASE_VOLUME);
}

void OMXPlayerInterface::Do_SeekBackSmall() {
    Log("Do_SeekBack() - calling Send_Action(KeyConfig::ACTION_SEEK_BACK_SMALL)");
    Send_Action(KeyConfig::ACTION_SEEK_BACK_SMALL);
}

void OMXPlayerInterface::Do_SeekBackLarge() {
    Log("Do_SeekBack() - calling Send_Action(KeyConfig::ACTION_SEEK_BACK_LARGE)");
    Send_Action(KeyConfig::ACTION_SEEK_BACK_LARGE);
}

void OMXPlayerInterface::Do_SeekForwardSmall() {
    // TODO: MUST!!!
    //  Add checks for seeking past end, omxplayer
    //  does not check this and idles if you seek
    //  past the end of the stream.
    Log("Do_SeekForward() - calling Send_Action(KeyConfig::ACTION_SEEK_FORWARD_SMALL)");
    Send_Action(KeyConfig::ACTION_SEEK_FORWARD_SMALL);
}

void OMXPlayerInterface::Do_SeekForwardLarge() {
    // TODO: MUST!!!
    //  Add checks for seeking past end, omxplayer
    //  does not check this and idles if you seek
    //  past the end of the stream.
    Log("Do_SeekForward() - calling Send_Action(KeyConfig::ACTION_SEEK_FORWARD_LARGE)");
    Send_Action(KeyConfig::ACTION_SEEK_FORWARD_LARGE);
}

void OMXPlayerInterface::Do_Rewind() {
    Log("Do_Rewind() - calling Send_Action(KeyConfig::ACTION_REWIND)");
    Send_Action(KeyConfig::ACTION_REWIND);
}

void OMXPlayerInterface::Do_FastForward() {
    Log("Do_Rewind() - calling Send_Action(KeyConfig::ACTION_FAST_FORWARD");
    Send_Action(KeyConfig::ACTION_FAST_FORWARD);
}

void OMXPlayerInterface::Send_Action(int Action) {
  Log("Send_Action() - sending Action(" + to_string(Action) + ")");
  try {
    g_player_client->Action(Action);
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Action() - D-Bus error - omxplayer has gone away?");
  }
}

void OMXPlayerInterface::Do_Pause() {
    Log("Do_Pause() - calling Send_Pause()");
    Send_Pause();
}

void OMXPlayerInterface::Send_Pause() {
  Log("Send_Pause() - sending Pause()");
  try {
    g_player_client->Pause();
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Pause() - D-Bus error - omxplayer has gone away?");
  }
}

void OMXPlayerInterface::Send_Stop() {
  Log("Send_Stop() - sending Stop()");
  try {
    g_player_client->Stop();
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Stop() - D-Bus error - omxplayer has gone away?");
  }
}


void OMXPlayerInterface::Stop() {
    m_mtxLocal.lock();
    if (!m_bOMXIsRunning && ePlayerState >= STATE::STOPPING ) {
      m_mtxLocal.unlock();

      Log("Stop() - m_bOMXIsRunning = false, exiting Stop()");
      return;
    }
    m_bOMXIsRunning = false;
    m_mtxLocal.unlock();

    SetState(STATE::STOPPING);

    Log("Stop() - called");

    if ( !IsStopped() ) Send_Stop();
//    Send_Quit();

    // exiting player monitor thread
    if (m_bRunPlayerMonitor && m_tPlayerMonitorThread) {
      Log("Stop() - joining m_tPlayerMonitorThread");
      m_bRunPlayerMonitor = false;
      pthread_join(m_tPlayerMonitorThread, NULL);
      m_tPlayerMonitorThread = 0;
      Log("Stop() - m_tPlayerMonitorThread joined");
    }

    // exiting output reader thread
    if (m_bRunPlayerOutputReader && m_tPlayerOutputReaderThread)
    {
      Log("Stop() - joining m_tPlayerOutputReaderThread");
      m_bRunPlayerOutputReader = false;
      pthread_join(m_tPlayerOutputReaderThread, NULL);
      m_tPlayerOutputReaderThread = 0;
      Log("Stop() - m_tPlayerOutputReaderThread joined");
    }

#ifdef USE_PIPES
    ClosePipes();
#endif

    Log("Stop() - cleaning up D-Bus interface proxies");
    // Clean up the client interfaces and connections;
    if (g_player_client != NULL)
      delete g_player_client;
    g_player_client = NULL;
    if (g_props_client != NULL)
      delete g_props_client;
    g_props_client = NULL;
    if (g_root_client != NULL)
      delete g_root_client;
    g_root_client = NULL;

    Log("Stop() - cleaning up D-Bus connection proxies");
    if (g_player_conn != NULL)
      delete g_player_conn;
    g_player_conn = NULL;
    if (g_props_conn != NULL)
      delete g_props_conn;
    g_props_conn = NULL;
    if (g_root_conn != NULL)
      delete g_root_conn;
    g_root_conn = NULL;

    Log("Stop() - finished");
    Set_Stopped(true);
    Set_Finished();
    SetState(STATE::STOPPED);
}

void OMXPlayerInterface::Send_Quit() {
  Log("Send_Quit() - sending Quit()");
  try {

    if (!g_props_client) {
      Log("Send_Quit() - FUBAR! (props) - Stop() pressed when not PLAYING?");
      return;
    }

    if (!g_props_client->CanQuit() ) {
      Log("Send_Quit() - player unable to quit");
      return; // false;
    }

    if (!g_root_client) {
      Log("Send_Quit() - FUBAR! (root) - Stop() pressed when not PLAYING?");
      return;
    }
    g_root_client->Quit();
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Quit() - can't send, has omxplayer gone away?");
  }
}

void OMXPlayerInterface::ClosePipes() {
  Log("ClosePipes() - Closing pipes");
  for (int i=0; i<2; i++) {
    if (m_bInPipe[i])
      close(m_iInPipe[i]);
    if (m_bOutPipe[i])
      close(m_iOutPipe[i]);
    m_bInPipe[i] = m_bOutPipe[i] = false;
  }
  Log("ClosePipes() - Pipes are closed");
}

void OMXPlayerInterface::OpenPipes() {
  Log("OpenPipes() - called");
  // setup pipes for the PlayerOutputReaderThread

  // initializing in pipe
  if ( pipe(m_iInPipe) == -1 ) {
    Log("pipe() failed for in pipe");
    return;
  }
  else {
    m_bInPipe[0] = m_bInPipe[1] = true;
  }

  // initializing out pipe
  if ( pipe(m_iOutPipe) == -1 ) {
    Log("pipe() failed for out pipe");
    return;
  }
  else {
    m_bOutPipe[0] = m_bOutPipe[1] = true;
  }
  Log("OpenPipes() - finished");
}


bool OMXPlayerInterface::Play(string sMediaURL) {
  if ( ePlayerState != STATE::STOPPED  ) {
	Log("OMXPlayerInterface::Play - Must be STOPPED to Play()");
	return false;
  }

  SetState(STATE::INITIALIZING);

  // TODO add error reporting

#ifdef USE_PIPES
  OpenPipes();
#endif

  // fork a child process to run omxplayer in
  pid_t pid = fork();
  if (pid < 0)                      // failed to fork
  {
    cerr << "Failed to fork" << endl;
    exit(127);
    // Throw exception
  }
  else if (pid == 0)                // child
  {
    // Code only executed by child process
    Log("(Child's) Play() - begins");

#ifdef USING_PIPES    // connecting child stdin to InPipe, stdout & stderr to OutPipe
    Log("(Child's) Play() - connecting to InPipe and OutPipe");
    if ( dup2(m_iInPipe[0], STDIN_FILENO)<0 || dup2(m_iOutPipe[1], STDOUT_FILENO)<0 ||
         dup2(m_iOutPipe[1], STDERR_FILENO)<0 ) {
      Log("dup2() failed in child");
//      printf("%d : %s\n", errno, strerror(errno));
      exit(127);
    }
#endif

    // build command line for launching omxplayer
    vector<char*> args;
    char *fullname = (char *)"/usr/bin/omxplayer";
    char *name = (char *)"omxplayer";
    args.push_back(name);

    // set the passthrough option (if enabled)
    if (m_bPassthrough) {
      char *passthru = (char *)"-p";
      args.push_back(passthru);
    }

    // check the audio output device
    string sopt;
    if (m_sAudioDevice == "hdmi") {
      sopt = "-ohdmi";
    }
    else if (m_sAudioDevice == "local") {
      sopt = "-olocal";
    }
    else if (m_sAudioDevice == "both") {
      sopt = "-oboth";
    }
    else {
      sopt = "-oboth";
    }
    args.push_back((char *)sopt.c_str());

    if (true) {
      char *refresh = (char *)"-r";
      args.push_back(refresh);
    }

    if (true) {
      char *nokeys = (char *)"--no-keys";
      args.push_back(nokeys);
    }

    // add the media url
    Log("(Child's) Play() - MediaURL: " + sMediaURL);
    args.push_back((char *)sMediaURL.c_str());

    string sTxt = "(Child's) Play() - cmdline: ";
    for(std::vector<char *>::iterator it = args.begin(); it != args.end(); ++it) {
      string sArg(*it);
      sTxt = sTxt + sArg + " ";
    }
    Log(sTxt);

    // need a \0 as the last item in args[]
    args.push_back((char *)NULL);

    // replace this proces with omxplayer
    execv(fullname, &args[0]);

    Log("child - omxplayer - THIS SHOULD NEVER BE SEEN!");
    exit(127);
  }
  else                         // parent
  {
    // Code only executed by parent process
    m_iChildPID = pid;
    m_bOMXIsRunning = true;

#ifdef USE_PIPES
    Log("Play() - closing child's end of pipes");
    // closing child's end of pipes
    close(m_iInPipe[0]);
    m_bInPipe[0] = false;
    close(m_iOutPipe[1]);
    m_bOutPipe[1] = false;
    fcntl(m_iOutPipe[0], F_SETFL, O_NONBLOCK);


    // setup the output reader thread to watch for 'Stopped at XX:XX:XX' and 'have a nice day ;)' signals from omxplayer
    m_bRunPlayerOutputReader = true;
    // TODO error reporting here
    Log("Play() - creating PlayerOutputReaderThread");
    pthread_create(&m_tPlayerOutputReaderThread, NULL, &PlayerOutputReader, this);
#endif

    int i=0;
    const char *dbus_addr;
    string line("");
    ifstream infile;
    bool bPrivate(true);

    Log("Play() - Opening file '" + OMXPLAYER_DBUS_ADDR + "' to get d-bus address");
    infile.open(OMXPLAYER_DBUS_ADDR.c_str());

    while (++i<=RETRIES && line == "") {
      while (!infile.is_open() && ++i<=RETRIES ) {
        usleep(100000);
        infile.open(OMXPLAYER_DBUS_ADDR.c_str());
      }
      if (!infile.is_open())  {
        Log("Play() - Could not open file " + OMXPLAYER_DBUS_ADDR + ", exiting... ");
        Stop();
        system("killall omxplayer.bin");
        system("killall omxplayer");
        return false;
      }
      else {
        getline(infile, line);
        infile.close();
      }
    }

    if ( line != "" ) {
      Log("Play() - Read line: " + line);
    }
    else {
      Log("Play() - Failed to read D-Bus address line from " + OMXPLAYER_DBUS_ADDR);
//      Stop();
      return false;
    }

    dbus_addr = line.c_str();

    // setup the dbus 'Player' connection and client
    if (g_player_conn == NULL) {
      Log("Play() - Creating connection to 'Player' bus");
      g_player_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_player_conn->connected()) {
        Log("Play() - g_player_conn not connected(), exiting.");
      }
      if (!g_player_conn->register_bus()) {
        Log("Play() - g_player_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_player_client == NULL) {
      Log("Play() - Creating Player_proxy client interface");
      g_player_client = new OMXPlayerClient(*g_player_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
    }

    // setup the dbus 'Properties' connection and client
    if (g_props_conn == NULL) {
      Log("Play() - Creating connection to 'Propertiess' bus");
      g_props_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_props_conn->connected()) {
        Log("Play() - g_props_conn not connected(), exiting.");
      }
      if (!g_props_conn->register_bus()) {
        Log("Play() - g_props_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_props_client == NULL) {
      Log("Play() - Creating Properties_proxy client interface");
      g_props_client = new OMXPropsClient(*g_props_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
    }

    // setup the dbus 'Root' connection and client
    if (g_root_conn == NULL) {
      Log("Play() - Creating connection to 'Root' bus");
      g_root_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_root_conn->connected()) {
        Log("Play() - g_root_conn not connected(), exiting.");
      }
      if (!g_root_conn->register_bus()) {
        Log("Play() - g_root_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_root_client == NULL) {
      Log("Play() - Creating Root_proxy client interface");
      g_root_client = new OMXRootClient(*g_root_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
    }

    // setup the monitoring thread to waitpid()
    m_bRunPlayerMonitor = true;
    // TODO error reporting here
    Log("Play() - Creating PlayerMonitorThread");
    pthread_create(&m_tPlayerMonitorThread, NULL, &PlayerMonitor, this);

    if (g_player_client != NULL && g_props_client != NULL) {
      int i = 0;
      string sIdentity;
      bool bIdentity = false;
      while (!bIdentity && ++i<DBUS_RETRIES) {
        try {
          sIdentity = g_props_client->Identity();
          bIdentity = true;
        }
        catch (DBus::Error &dbus_err) {
          //Log("OMXPlayerInterface::Play() - Waiting for Identity()...");
          usleep(100000);
        }
      }
      Log("Play() - Identity() [" + to_string(i) + "]: " + sIdentity);
    }
    else {
      Log("Play() - FUBAR!");
    }

    SetState(STATE::PLAYING);
    Set_Stopped(false);
    // at this point the player is accessible through dbus
    return true;
  }
  // Code executed by both parent and child.
  Log("OMXPlayerInterface::Play - End - this should never happen.");
  return false;
}


void OMXPlayerInterface::SetState(OMXPlayerInterface::STATE playerState) {
  Log("OMXPlayerInterface::SetState - State: " + to_string((int)playerState));

  ePlayerState = playerState;

  // if a c callback has been registered notify it
  if (m_fpStateNotifier)
	m_fpStateNotifier(playerState);

  // if a c++ class has inherited this notify it
  PlayerStateNotifier(playerState);
}

OMXPlayerInterface::STATE OMXPlayerInterface::Get_PlayerState(void) {
//  Log("OMXPlayerInterface::Get_PlayerState");
  return ePlayerState;
}

void OMXPlayerInterface::Inc_Error(void) {
  ++m_iErrCount;
  Log("[" + to_string(m_iErrCount) + "] - ");

  if ( m_iErrCount >= DBUS_RETRIES ) {
    Stop();
  }

}

void OMXPlayerInterface::Set_Stopped(bool val) {

  int i = 0;
  while (!m_mtxLocal.try_lock() && ++i<=RETRIES) {
    usleep(1000);
  }
  if (i >= RETRIES) {
    Log("Set_Stopped() - can't get lock");
    return;
  }

  //Log("Set_Stopped() - setting m_bStopped=" + string(val ? "true" : "false"));
  m_bStopped = val;
  m_mtxLocal.unlock();
}

bool OMXPlayerInterface::IsStopped(void) {
  bool ret;

  int i = 0;
  while (!m_mtxLocal.try_lock() && ++i<=RETRIES) {
    usleep(1000);
  }
  if (i >= RETRIES) {
    Log("IsStopped() - can't get lock");
    return false;
  }

  ret = m_bStopped;
  m_mtxLocal.unlock();
  return ret;
}

int OMXPlayerInterface::Get_ErrCount(void) {
  int ret;
  ret = m_iErrCount;
  return ret;
}

void OMXPlayerInterface::Set_ErrCount(int count) {
  m_iErrCount = count;
  if (m_iErrCount >= DBUS_RETRIES) {
    Log("Set_ErrCount() - count exceeds retries, calling Stop()");
    Stop();
  }
}

string OMXPlayerInterface::Get_PlaybackStatus(void) {
  string ret;

  if (!g_props_client)
      Log("Get_PlaybackStatus() - FUBAR! Please report this to the author!");

  try {
    ret = g_props_client->PlaybackStatus();
    Set_ErrCount(0);
  }
  catch (DBus::Error &dbus_err) {
    Inc_Error();
    ret = "";
  }
  return ret;
}

uint64_t OMXPlayerInterface::Get_Position(void) {
  uint64_t ret = -1;
  try {
    ret = g_props_client->Position();
    Set_ErrCount(0);
  }
  catch (DBus::Error &dbus_err) {
    Inc_Error();
  }
  return ret;
}

uint64_t OMXPlayerInterface::Get_Duration(void) {
  uint64_t ret = -1;
  try {
    ret = g_props_client->Duration();
    Set_ErrCount(0);
  }
  catch (DBus::Error &dbus_err) {
    Inc_Error();
  }
  return ret;
}

bool OMXPlayerInterface::Get_CanQuit(void) {
  bool ret;
  ret = g_props_client->CanQuit();
  return ret;
}

bool OMXPlayerInterface::IsFinished(void) {
  bool ret = false;

  int i = 0;
  while (!m_mtxLocal.try_lock() && ++i<=RETRIES) {
    usleep(1000);
  }
  if (i >= RETRIES) {
    Log("IsFinished() - can't get lock");
    return false;
  }

  ret = m_bFinished;
  m_mtxLocal.unlock();
  return ret;
}

void OMXPlayerInterface::Set_Finished() {
  m_mtxLocal.lock();
  m_bFinished = true;
  m_mtxLocal.unlock();
}

void OMXPlayerInterface::Log(string txt) {
  m_mtxLog.lock();
  cout << txt << endl;
  m_mtxLog.unlock();
}

void *PlayerOutputReader(void *pInstance) {
  OMXPlayerInterface* pThis = (OMXPlayerInterface*) pInstance;

  pThis->Log("[PlayerOutputReader] started");

  while (!pThis->IsStopped()) {
    usleep(10000);
  }

  pThis->Log("[PlayerOutputReader] exiting");
}


void *PlayerMonitor(void *pInstance) {

  OMXPlayerInterface* pThis = (OMXPlayerInterface*) pInstance;

  pThis->Log("[PlayerMonitor] started");

  int *stat_loc;
  int options = 0;
  pid_t watch_id = pThis->m_iChildPID;
  pid_t child = waitpid(watch_id, stat_loc, options);
  //if (child == watch_id)  {
    pThis->Log("[PlayerMonitor] calling Stop()");
    pThis->Stop();
  //}
  pThis->Log("[PlayerMonitor] exiting");
}
