/*
 * frontend_svr.c: server for remote frontends
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: frontend_svr.c,v 1.109 2014-06-23 12:20:28 phintuka Exp $
 *
 */

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS
#include <inttypes.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/plugin.h>
#include <vdr/device.h>

#include "logdefs.h"
#include "config.h"

#include "xine_input_vdr_net.h"   // stream header(s)

#include "tools/osd_command.h"     // osd commands
#include "tools/cxsocket.h"
#include "tools/future.h"
#include "tools/backgroundwriter.h"
#include "tools/udp_pes_scheduler.h"
#include "tools/http.h"
#include "tools/vdrdiscovery.h"
#include "tools/sdp.h"
#include "tools/rle.h"
#include "tools/sys_cap.h"

#include "frontend_svr.h"
#include "device.h"
#include "osd.h"

#define ALLOWED_HOSTS_FILE "xineliboutput/allowed_hosts.conf"

#define MAX_OSD_TIMEOUTS  (25*5)      /* max. rate 25 updates/s -> at least 5 seconds */
#define LOG_OSD_BANDWIDTH (128*1024)  /* log messages if OSD bandwidth > 1 Mbit/s */

#define PLAYFILE_CTRL_TIMEOUT   300   /* ms */
#define PLAYFILE_TIMEOUT      20000   /* ms */
#define REMOTE_FLUSH_TIMEOUT   1000   /* ms */
#define REMOTE_SYNC_TIMEOUT    1000   /* ms */
#define REMOTE_STC_TIMEOUT      300   /* ms */

#undef  MIN
#define MIN(a,b) ( (a) < (b) ? (a) : (b))

typedef struct {
  int    Size;
  uchar *Data;
} grab_result_t;

class cStcFuture : public cFuture<int64_t> {};
class cReplyFuture : public cFuture<int>, public cListObject {};
class cGrabReplyFuture : public cFuture<grab_result_t>, public cListObject {};
class cCmdFutures : public cHash<cReplyFuture> {};

class cAllowedHosts : public cSVDRPhosts {
  public:
    cAllowedHosts(const cString& AllowedHostsFile)
    {
      if (!Load(AllowedHostsFile, true, true)) {
        LOGMSG("Invalid or missing %s. Adding 127.0.0.1 to list of allowed hosts.", *AllowedHostsFile);
        cSVDRPhost *localhost = new cSVDRPhost;
        if (localhost->Parse("127.0.0.1"))
          Add(localhost);
        else
          delete localhost;
      }
    }
};


//----------------------------- cXinelibServer --------------------------------

// (control stream) connection types
enum {
  ctDetecting = 0x00,
  ctControl   = 0x01,
  ctHttp      = 0x02,
  ctRtsp      = 0x04, // TCP/RTSP + UDP/RTP
  ctRtspMux   = 0x08  // TCP: multiplexed RTSP control + RTP/RTCP data/control
};

// (data) connection types
enum {
  dtPipe    = 0x01,
  dtTcp     = 0x02,
  dtUdp     = 0x04,
  dtRtp     = 0x08,
  dtHttp    = 0x10,
  dtRtspMux = 0x20,
};

// (data) connection properties
#define DATA_STREAM(dt)    ((dt) & (dtPipe | dtTcp | dtHttp | dtRtspMux))
#define DATA_DATAGRAM(dt)  ((dt) & (dtUdp | dtRtp))
#define DATA_NOPOLL(dt)    ((dt) & (dtHttp | dtRtspMux))
#define DATA_NOCONTROL(dt) ((dt) & (dtHttp | dtRtspMux))

cXinelibServer::cXinelibServer(cXinelibDevice *Dev, int listen_port) :
  cXinelibThread(Dev, "Remote decoder/display server (cXinelibServer)")
{
  int i;
  for(i=0; i<MAXCLIENTS; i++) {
    fd_data[i] = -1;
    m_OsdTimeouts[i] = 0;
    m_Writer[i] = NULL;
    m_State[i] = NULL;
    m_bMulticast[i] = 0;
    m_bConfigOk[i] = false;
    m_bArgbOSD[i] = false;
    m_bUdp[i] = 0;
    m_ConnType[i] = ctDetecting;
  }

  m_Port = listen_port;
  m_ServerId = time(NULL) ^ getpid();

  fd_listen    = -1;
  fd_discovery = -1;

  m_iMulticastMask = 0;
  m_MasterCli = -1;

  m_Scheduler  = new cUdpScheduler;
  m_StcFuture  = new cStcFuture;
  m_Futures    = new cCmdFutures;

  cString Base(cPlugin::ConfigDirectory());
  if(*Base) {
    m_PipesDir = cString::sprintf("%s/xineliboutput/pipes.%d", *Base, getpid());
    m_AllowedHostsFile = cString::sprintf("%s/" ALLOWED_HOSTS_FILE, *Base);
  } else {
    LOGMSG("cXinelibServer: cPlugin::ConfigDirectory() failed !");
    m_PipesDir = cString::sprintf("/tmp/xineliboutput/pipes.%d", getpid());
    m_AllowedHostsFile = cString::sprintf("/video/" ALLOWED_HOSTS_FILE);
  }

  m_Token = 1;

  m_Header       = NULL;
  m_HeaderLength = 0;
  m_HeaderSize   = 0;
}

cXinelibServer::~cXinelibServer()
{
  int i;

  Cancel(-1);

  CLOSESOCKET(fd_listen);
  CLOSESOCKET(fd_discovery);

  cHttpStreamer::CloseAll();

  for(i=0; i<MAXCLIENTS; i++)
    CloseConnection(i);

  Cancel(3);

  delete m_StcFuture;
  delete m_Futures;
  delete m_Scheduler;

  free(m_Header);
}

void cXinelibServer::Clear(void)
{
  TRACEF("cXinelibServer::Clear");

  LOCK_THREAD;

  SetHeader(NULL, 0, true);

  for(int i = 0; i < MAXCLIENTS; i++)
    if(fd_control[i].open() && m_Writer[i])
      m_Writer[i]->Clear();

  if(m_Scheduler)
    m_Scheduler->Clear();

  cXinelibThread::Clear();
}

void cXinelibServer::CloseDataConnection(int cli)
{
  if(m_bUdp[cli] && fd_data[cli]>=0)
    m_Scheduler->RemoveHandle(fd_data[cli]);

  CLOSESOCKET(fd_data[cli]);

  if(m_Writer[cli]) {
    delete m_Writer[cli];
    m_Writer[cli] = NULL;
  }

  m_bUdp[cli] = false;
  m_bMulticast[cli] = false;
  m_bConfigOk[cli] = false;
  m_bArgbOSD[cli] = false;

  m_iMulticastMask &= ~(1<<cli);

  if(!m_iMulticastMask && !xc.remote_rtp_always_on)
    m_Scheduler->RemoveRtp();

  if (cli == m_MasterCli)
    m_MasterCli = -1;
}

void cXinelibServer::CloseConnection(int cli)
{
  CloseDataConnection(cli);
  if(fd_control[cli].open()) {
    LOGMSG("Closing connection %d", cli);
    fd_control[cli].close();
    if(m_State[cli]) {
      delete m_State[cli];
      m_State[cli] = NULL;
    }
    m_Dev->ForcePrimaryDevice(false);
  }
}

static int write_osd_command(cxSocket& s, osd_command_t *cmd)
{
  cxPoller p(s, true);
  if(!p.Poll(100)) {
    LOGMSG("write_osd_command: poll failed, OSD send skipped");
    return 0;
  }

  ssize_t max  = s.tx_buffer_free();
  ssize_t size = (ssize_t)8 +
                 (ssize_t)(sizeof(osd_command_t)) +
                 (ssize_t)(sizeof(osd_clut_t) * ntohl(cmd->colors)) +
                 (ssize_t)(ntohl(cmd->datalen));

  if(max > 0 && max < MIN(size, 1024)) {
/* #warning TODO: buffer latest failed OSD and retry
                  -> skipped OSDs can be left out but
                  latest will be always delivered */
    LOGMSG("write_osd_command: socket buffer full, OSD send skipped (got %d ; need %d",
           (int)max, (int)size);
    return 0;
  }

  cmd->size = sizeof(osd_command_t);

  if(8 != s.write("OSDCMD\r\n", 8, 100)) {
    LOGDBG("write_osd_command: write (command) failed");
    return -1;
  }
  if((ssize_t)sizeof(osd_command_t) !=
     s.write(cmd, sizeof(osd_command_t), 100)) {
    LOGDBG("write_osd_command: write (data) failed");
    return -1;
  }
  if(cmd->palette && cmd->colors &&
     (ssize_t)(sizeof(osd_clut_t)*ntohl(cmd->colors)) !=
     s.write(cmd->palette, sizeof(osd_clut_t)*ntohl(cmd->colors), 100)) {
    LOGDBG("write_osd_command: write (palette) failed");
    return -1;
  }
  if(cmd->data && cmd->datalen &&
     (ssize_t)ntohl(cmd->datalen) != s.write(cmd->data, ntohl(cmd->datalen), 300)) {
    LOGDBG("write_osd_command: write (bitmap) failed");
    return -1;
  }
  return 1;
}

void cXinelibServer::OsdCmd(void *cmd_gen)
{
  TRACEF("cXinelibServer::OsdCmd");
  int i;

  LOCK_THREAD;

  // check if there are any clients
  if(!HasClients())
    return;

  if(cmd_gen) {
    void *compressed_data = NULL;
    osd_command_t *cmd = (osd_command_t*)cmd_gen;
    osd_command_t cmdnet;
    memcpy(&cmdnet, cmd, sizeof(osd_command_t));

    if (cmd->data) {
      // compress data
      if (cmd->cmd == OSD_Set_LUT8) {
        int num_rle = 0;
        cmdnet.datalen  = rle_compress_hdmv(&cmdnet.raw_data, cmd->raw_data, cmd->w, cmd->h, &num_rle);
        cmdnet.num_rle  = num_rle;
        cmdnet.cmd      = OSD_Set_HDMV;
        compressed_data = cmdnet.raw_data; // free it later

      } else if (cmd->cmd == OSD_Set_RLE) {
        cmdnet.raw_data = (uint8_t *)malloc(cmd->datalen);
        cmdnet.datalen  = rle_recompress_net(cmdnet.raw_data, cmd->data, cmd->num_rle);
        compressed_data = cmdnet.raw_data; // free it later
      }
    }

    // -> network byte order
    hton_osdcmd(cmdnet);

    for(i = 0; i < MAXCLIENTS; i++) {
      if(fd_control[i].open() && m_bConfigOk[i]) {
        int r = write_osd_command(fd_control[i], &cmdnet);
        if(r < 0) {
          LOGMSG("Send OSD command failed, closing connection");
          CloseConnection(i);
        } else if(r == 0) {
          if(m_OsdTimeouts[i]++ > MAX_OSD_TIMEOUTS) {
            LOGMSG("Too many OSD timeouts, dropping client");
            CloseConnection(i);
          }
        } else {
          m_OsdTimeouts[i] = 0;
        }
      }
    }

    free(compressed_data);

#ifdef LOG_OSD_BANDWIDTH
    {
      static int64_t timer = 0LL;
      static int bytes = 0;
      int64_t now = cTimeMs::Now();

      if(timer + 5000LL < now) {
        timer = now;
        bytes = 0;
      } else if(timer + 1000LL < now) {
        bytes = bytes / (((int)(now - timer)) / 1000);
        if(bytes > LOG_OSD_BANDWIDTH)
          LOGMSG("OSD bandwidth: %d bytes/s (%d kbit/s)", bytes, bytes*8/1024);
        timer = now;
        bytes = 0;
      }
      bytes += sizeof(osd_command_t) + ntohl(cmdnet.datalen);
    }
#endif
  }
}

int64_t cXinelibServer::GetSTC(void)
{
  Lock();

  // check if there are any clients
  if(!HasClients()) {
    Unlock();
    return INT64_C(-1);
  }

  // Query client(s)
  m_StcFuture->Reset();
  Xine_Control("GETSTC");

  Unlock();

  if (!m_StcFuture->Wait(REMOTE_STC_TIMEOUT)) {
    LOGMSG("cXinelibServer::GetSTC timeout");
    return INT64_C(-1);
  }

  //if(delay.Elapsed() > 0 && !is_Paused)
  //  LOGMSG("GetSTC: compensating network delay by %s ticks (ms)\n",
  //         delay.Elapsed()*90000/2, delay.Elapsed()/2);

  return m_StcFuture->Value() /*+ (delay.Elapsed()*90000/2*/;
}

void cXinelibServer::SetHeader(const uchar *Data, int Length, bool Reset)
{
  LOCK_THREAD; // Lock control thread out

  if (Reset)
    m_HeaderLength = 0;

  if (m_HeaderSize < m_HeaderLength + Length) {
    if (!m_Header) {
      m_HeaderSize = Length;
      m_Header     = (uchar*)malloc(m_HeaderSize);
    } else {
      m_HeaderSize = m_HeaderLength + Length;
      m_Header     = (uchar*)realloc(m_Header, m_HeaderSize);
    }
  }

  if (m_Header && Data) {
    memcpy(m_Header + m_HeaderLength, Data, Length);
    m_HeaderLength += Length;
  }
}

int cXinelibServer::Play(const uchar *data, int len, eStreamId StreamId)
{
  int TcpClients = 0, UdpClients = 0, RtpClients = 0;

  LOCK_THREAD; // Lock control thread out

  for(int i=0; i<MAXCLIENTS; i++) {
    if(fd_control[i].open()) {
      if((m_bConfigOk[i] && fd_data[i] >= 0) ||
         m_ConnType[i] & (ctHttp|ctRtsp)) {

        if(m_bUdp[i]) {

          UdpClients++;

        } else if(m_Writer[i]) {

          int result = m_Writer[i]->Put(StreamId, m_StreamPos, data, len);
          if(!result) {
            LOGMSG("cXinelibServer::Play Write/Queue error (TCP/PIPE)");
            CloseConnection(i);
          } else if(result<0) {
            LOGMSG("cXinelibServer::Play Buffer overflow (TCP/PIPE)");
            if(m_ConnType[i] == ctHttp)
              m_Writer[i]->Clear();
          }

          TcpClients++;
        }
      }
    }
  }

  RtpClients = (m_iMulticastMask || xc.remote_rtp_always_on);

  if(UdpClients || RtpClients)
    if(! m_Scheduler->Queue(StreamId, m_StreamPos, data, len))
      LOGMSG("cXinelibServer::Play Buffer overflow (UDP/RTP)");

  if(TcpClients || UdpClients || RtpClients)
    cXinelibThread::Play(data, len, StreamId);

  return len;
}

void cXinelibServer::SetHDMode(bool On)
{
  cXinelibThread::SetHDMode(On);
#if 0
  /*#warning TODO*/
  LOCK_THREAD;

  int i;
  for(i=0; i<MAXCLIENTS; i++)
    if(m_Writer[i])
      m_Writer[i]->SetBuffer(On ? 2048 : 512);
  m_Scheduler->SetWindow(On ? 512 : 128);
#endif
}

int cXinelibServer::Poll(cPoller &Poller, int TimeoutMs)
{
  // in live mode transponder clock is the master ...
  // in replay mode local frontend (if present) is master
  if(m_bLiveMode || (*xc.local_frontend && strncmp(xc.local_frontend, "none", 4))) {
    if(m_Scheduler->Clients())
      return m_Scheduler->Poll(TimeoutMs, false);
    return DEFAULT_POLL_SIZE;
  }

  // replay mode:
  do {
    Lock();
    int Free = 0xfffff, FreeHttp = 0xfffff, FreeUdp = 0;
    int Clients = 0, Http = 0, Udp = 0;
    for(int i=0; i<MAXCLIENTS; i++) {
      if(fd_control[i].open()) {
        if(m_bConfigOk[i]) {
          if(m_Writer[i])
            Free = min(Free, m_Writer[i]->Free());
          else if(m_bUdp[i])
            Udp++;
          Clients++;
        } else if(m_ConnType[i] & (ctHttp|ctRtspMux)) {
          if(m_Writer[i]) {
            FreeHttp = min(FreeHttp, m_Writer[i]->Free());
            Http++;
          }
        }
      }
    }
    if(m_iMulticastMask) {
      Clients++;
      Udp++;
    }

    /* select master timing source for replay mode */
    int master = -1;
    if(Clients && !Udp) {
      for(int i=0; i<MAXCLIENTS; i++)
        if(fd_control[i].open() && m_bConfigOk[i] && m_Writer[i]) {
          master = i;
          break;
        }
    }
    if(master != m_MasterCli) {
      if(m_MasterCli >= 0)
        Xine_Control("MASTER 0");
      if(master >= 0)
        fd_control[master].write_cmd("MASTER 1\r\n");
      m_MasterCli = master;
    }

    Unlock();

    if(!Clients && !Http) {
      // live mode runs even if there are no clients
      if(m_bLiveMode)
        return DEFAULT_POLL_SIZE;
      // replay is paused when no clients
      if(TimeoutMs>0)
        cCondWait::SleepMs(TimeoutMs);
      return 0;
    }

    // in replay mode cUdpScheduler is master timing source
    if( Free < 8128 ||
        ((FreeUdp = m_Scheduler->Poll(TimeoutMs, true)) < 1) ||
        (!Clients && FreeHttp < 8128)) {

      if(TimeoutMs > 0)
        cCondWait::SleepMs(min(TimeoutMs, 5));
      TimeoutMs -= 5;

    } else {
      Free = min(Free, FreeHttp) / 2070;
      Free = min(Free, FreeUdp);
      return max(0, Free);
    }

  } while(TimeoutMs > 0);

  return 0;
}

bool cXinelibServer::Flush(int TimeoutMs)
{
  if (!HasClients())
    return true;

  int  result = true;

  if(m_Scheduler)
    result = m_Scheduler->Flush(TimeoutMs) && result;

  for(int i=0; i<MAXCLIENTS; i++)
    if(fd_control[i].open() && fd_data[i]>=0 && m_Writer[i])
      result = m_Writer[i]->Flush(TimeoutMs) && result;

  if(TimeoutMs > 50)
    TimeoutMs  = 50;

  if(result) {
    cString tmp = cString::sprintf("FLUSH %d %" PRIu64 " %d",
                                   TimeoutMs, m_StreamPos, m_Frames);
    result = (Xine_Control_Result(tmp, REMOTE_FLUSH_TIMEOUT)) <= 0;
  }

  return result;
}

/*
 * Xine_Control()
 *
 * Post control message to client (async RPC)
 */
int cXinelibServer::Xine_Control(const char *cmd)
{
  TRACEF("cXinelibServer::Xine_Control");

  if(cmd && *cmd) {
    char buf[4096];
    int len = snprintf(buf, sizeof(buf), "%s\r\n", cmd);
    if(len >= (int)sizeof(buf)) {
      LOGMSG("Xine_Control: command truncated !");
      return 0;
    }

    LOCK_THREAD;

    for(int i=0; i<MAXCLIENTS; i++)
      if(fd_control[i].open() && (fd_data[i]>=0 || m_bMulticast[i]) && m_bConfigOk[i])
        if(len != fd_control[i].write(buf, len, 100)) {
          LOGMSG("Control send failed (%s), dropping client", cmd);
          CloseConnection(i);
        }
  }

  return 1;
}

/*
 * Xine_Control_Sync()
 *
 * Post control message to client (message is transported in data stream)
 */
int cXinelibServer::Xine_Control_Sync(const char *cmd)
{
  TRACEF("cXinelibServer::Xine_Control_Sync");

  if(cmd && *cmd) {
    int  i, len, UdpClients = 0, RtpClients = 0;
    char buf[256];

    len = snprintf(buf, sizeof(buf), "%s\r\n", cmd) + 1;
    if(len >= (int)sizeof(buf)) {
      LOGMSG("Xine_Control_Sync: command truncated ! (%s)", cmd);
      len = sizeof(buf);
    }

    LOCK_THREAD;

    for(i=0; i<MAXCLIENTS; i++)
      if(fd_control[i].open() && m_bConfigOk[i]) {
        if(fd_data[i] >= 0) {
          if(m_bUdp[i])
            UdpClients++;
          else if(m_Writer[i])
            m_Writer[i]->Put(sidControl, (-1ULL), (const uchar*)buf, len);
        }
      }

    RtpClients = (m_iMulticastMask || xc.remote_rtp_always_on);

    if(UdpClients || RtpClients)
      if(! m_Scheduler->Queue(sidControl, (uint64_t)(-1ULL), (const uchar*)buf, len))
        LOGMSG("cXinelibServer::Xine_Control_Sync overflow (UDP/RTP)");
  }

  return 1;
}

/*
 * Sync()
 *
 * Wait until all control messages have been processed by the client
 */
void cXinelibServer::Sync(void)
{
  Xine_Control_Result("SYNC", REMOTE_SYNC_TIMEOUT);
}

void cXinelibServer::TrickSpeed(int Speed, bool Backwards)
{
  if(Speed == 0) {
    m_Scheduler->Pause(true);
  } else {
    m_Scheduler->Pause(false);
    m_Scheduler->TrickSpeed(Speed == -1 ? 1 : Speed);
  }

  cXinelibThread::TrickSpeed(Speed, Backwards);
}

bool cXinelibServer::EndOfStreamReached(void)
{
  LOCK_THREAD;

  /* Check if there are any clients */
  if(!HasClients())
    return true;

  return cXinelibThread::EndOfStreamReached();
}

int cXinelibServer::AllocToken(void)
{
  LOCK_THREAD;

  m_Token = (m_Token+1) & 0xffff;

  cXinelibThread::Xine_Control((const char *)"TOKEN", m_Token);

  return m_Token;
}

/*
 * Xine_Control_Result()
 *
 * Post control message to client and wait for result (synchronous RPC).
 */
int cXinelibServer::Xine_Control_Result(const char *Cmd, uint TimeoutMs)
{
  if(!HasClients()) {
    return -1;
  }

  if (TimeoutMs > 20000) {
    LOGMSG("Xine_Control_Result(): very long tomeout (%d sec) !", TimeoutMs/1000);
    TimeoutMs = 20000;
  }

  Lock();

  /* Get token, send it to client and set future for it */
  int          token = AllocToken();
  cReplyFuture future;
  m_Futures->Add(&future, token);

  /* Send actual command */
  Xine_Control(Cmd);

  Unlock();

  /* When server thread get REPLY %d %d (first %d == token, second returned value)
   * it sets corresponding future (by token; if found) in list
   * and removes it from list.
   */

#ifdef XINELIBOUTPUT_DEBUG
  int64_t t = cTimeMs::Now();
#endif

  future.Wait(TimeoutMs);

  Lock();
  m_Futures->Del(&future, token);
  Unlock();

  if (!future.IsReady()) {
    LOGMSG("cXinelibServer::Xine_Control_Result: Timeout (%s, %d ms) %d", Cmd, TimeoutMs, token);
    return -1;
  }

  return future.Value();
}

bool cXinelibServer::HasClients(void)
{
  LOCK_THREAD;

  int i;
  for(i=0; i<MAXCLIENTS; i++)
    if(fd_control[i].open() && m_bConfigOk[i])
      return true;

  return false;
}

int cXinelibServer::SupportsTrueColorOSD(void)
{
  LOCK_THREAD;

  unsigned i, has_clients = 0;

  for (i = 0; i < MAXCLIENTS; i++)
    if (fd_control[i].open() && m_bConfigOk[i]) {
      if (!m_bArgbOSD[i])
        return 0;
      else
        has_clients++;
    }

  return has_clients ? 1 : -1;
}

int cXinelibServer::PlayFileCtrl(const char *Cmd, int TimeoutMs)
{
  /* Check if there are any clients */
  if(!HasClients()) {
    cHttpStreamer::CloseAll();
    return -1;
  }

  int  result;
  bool bPlayfile = false;
  if((!strncmp(Cmd, "PLAYFILE", 8) && (bPlayfile=true)) ||
     (!strncmp(Cmd, "GET", 3)                         )) {  // GETPOS, GETLENGTH, ...

    if(TimeoutMs < 0)
      TimeoutMs = bPlayfile ? PLAYFILE_TIMEOUT : PLAYFILE_CTRL_TIMEOUT;

    result = Xine_Control_Result(Cmd, TimeoutMs);

  } else {
    result = cXinelibThread::PlayFileCtrl(Cmd);
  }

  if(!*m_FileName)
    cHttpStreamer::CloseAll();

  return result;
}


bool cXinelibServer::Listen(int listen_port)
{
  LOCK_THREAD;

  bool result = false;
  TRACEF("cXinelibServer::Listen");

  if(listen_port <= 0 || listen_port > 0xffff) {
    CLOSESOCKET(fd_listen);
    CLOSESOCKET(fd_discovery);
    if(m_Scheduler)
      m_Scheduler->RemoveRtp();
    cHttpStreamer::CloseAll();
    LOGMSG("Not listening for remote connections");
    return false;
  }

  if(fd_listen<0 || listen_port != m_Port) {
    m_Port = listen_port;
    CLOSESOCKET(fd_listen);

    int iReuse = 1;
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(m_Port);

    if(xc.remote_local_ip[0]) {
      uint32_t ip = inet_addr(xc.remote_local_ip);
      if(ip != INADDR_NONE) {
        char txt[128];
        name.sin_addr.s_addr = ip;
        LOGDBG("Binding server to %s", cxSocket::ip2txt(name.sin_addr.s_addr, htons(m_Port), txt));
      } else {
        LOGERR("Local interface address %s is invalid !", xc.remote_local_ip);
      }
    }
    fd_listen = socket(PF_INET,SOCK_STREAM,0);
    setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &iReuse, sizeof(int));

    if (bind(fd_listen, (struct sockaddr *)&name, sizeof(name)) < 0) {
      LOGERR("cXinelibServer: bind error %s port %d: %s",
             xc.remote_local_ip[0] ? xc.remote_local_ip : "",
             m_Port, strerror(errno));
      CLOSESOCKET(fd_listen);
    } else if(listen(fd_listen, MAXCLIENTS)) {
      LOGERR("cXinelibServer: listen error (port %d): %s",
             m_Port, strerror(errno));
      CLOSESOCKET(fd_listen);
    } else {
      LOGMSG("Listening on port %d", m_Port);
      result = true;
    }
  } else {
    result = true;
  }

  // set listen for discovery messages
  CLOSESOCKET(fd_discovery);
  if(xc.remote_usebcast) {
    fd_discovery = udp_discovery_init();
    if(udp_discovery_broadcast(fd_discovery, m_Port, xc.remote_local_ip) < 0)
      CLOSESOCKET(fd_discovery);
    else
      LOGMSG("Listening for UDP broadcasts on port %d", m_Port);
  }

  // set up multicast sockets

  if(m_Scheduler)
    m_Scheduler->RemoveRtp();

  if(xc.remote_usertp) {
    if(xc.remote_rtp_always_on)
      LOGMSG("WARNING: RTP Configuration: transmission is always on !");
    if(xc.remote_rtp_always_on || m_iMulticastMask)
      m_Scheduler->AddRtp();
  }

  return result;
}

uchar *cXinelibServer::GrabImage(int &Size, bool Jpeg,
                                 int Quality, int SizeX, int SizeY)
{
  cGrabReplyFuture future;
  uchar *result = NULL;
  cString cmd;

  cmd = cString::sprintf("GRAB %s %d %d %d\r\n",
                         Jpeg ? "JPEG" : "PNM",
                         Quality, SizeX, SizeY);

  Lock();

  /* Check if there are any clients */
  if(!HasClients()) {
    Unlock();
    return NULL;
  }

  int token = AllocToken();
  m_Futures->Add(&future, token);

  // might be better to request image from one client only (?)
  Xine_Control(cmd);

  Unlock();

  if(future.Wait(5000)) {
    grab_result_t r = future.Value();
    if((Size = r.Size) > 0) {
      LOGDBG("cXinelibServer::GrabImage: image size is %d bytes", Size);
      result = r.Data;
    } else {
      LOGMSG("cXinelibServer::Grab: Grab failed (%d)", Size);
    }
  } else {
    LOGMSG("cXinelibServer::Grab: Timeout (5000 ms)");
  }

  Lock();
  m_Futures->Del(&future, token);
  Unlock();

  return result;
}

//
// (Client) Control message handling
//

void cXinelibServer::Handle_Control_PIPE(int cli, const char *arg)
{
  LOGDBG("Trying PIPE connection ...");

  CloseDataConnection(cli);

  //
  // TODO: client should create pipe; waiting here is not good thing ...
  //

  if(!xc.remote_usepipe) {
    LOGMSG("PIPE transport disabled in configuration");
    fd_control[cli].write_cmd("PIPE: Pipe transport disabled in config.\r\n");
    return;
  }

  MakeDirs(m_PipesDir, true);

  int i;
  cString pipeName;
  for(i=0; i<10; i++) {
    pipeName = cString::sprintf("%s/pipe.%d", *m_PipesDir, i);
    if(mknod(pipeName, 0644|S_IFIFO, 0) < 0) {
      unlink(pipeName);
      continue;
    }
    else
      break;
  }
  if(i>=10) {
    LOGERR("Pipe creation failed (%s)", *pipeName);
    RemoveFileOrDir(m_PipesDir, false);
    fd_control[cli].write_cmd("PIPE: Pipe creation failed.\r\n");
    return;
  }

  fd_control[cli].printf("PIPE %s\r\n", *pipeName);

  cxPoller poller(fd_control[cli]);
  poller.Poll(500); /* quite short time ... */

  int fd;
  if((fd = open(pipeName, O_WRONLY|O_NONBLOCK)) < 0) {
    LOGDBG("Pipe not opened by client");
    /*write_cmd(fd_control[cli], "PIPE NONE\r\n");*/
    unlink(pipeName);
    RemoveFileOrDir(m_PipesDir, false);
    return;
  }

  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);

  //LOGDBG("cXinelibServer::Handle_Control: pipe %s open", pipeName);

  unlink(pipeName); /* safe to remove now, both ends are open or closed. */
  RemoveFileOrDir(m_PipesDir, false);
  fd_control[cli].write_cmd("PIPE OK\r\n");

  if (m_Writer[cli])
    delete m_Writer[cli];
  m_Writer[cli] = new cTcpWriter(fd);

  if (m_Header)
    m_Writer[cli]->Put(sidVdr, 0, m_Header, m_HeaderLength);

  fd_data[cli] = fd;
}


void cXinelibServer::Handle_Control_DATA(int cli, const char *arg)
{
  int clientId = -1;
  unsigned int ipc, portc;

  LOGDBG("Data connection (TCP) requested");

  CloseDataConnection(cli);

  if(!xc.remote_usetcp) {
    LOGMSG("TCP transports disabled in configuration");
    fd_control[cli].write_cmd("TCP: TCP transport disabled in config.\r\n");
    CloseConnection(cli); /* actually closes the new data connection */
    return;
  }

  /* validate client ID */
  if(3 != sscanf(arg, "%d 0x%x:%d", &clientId, &ipc, &portc) ||
     clientId < 0 ||
     clientId >= MAXCLIENTS ||
     !fd_control[clientId].open()) {
    fd_control[cli].write_cmd("TCP: Error in request (ClientId).\r\n");
    LOGDBG("Invalid data connection (TCP) request");
    /* close only new data connection, no control connection */
    CloseConnection(cli);
    return;
  }

#if 0
  /* check client IP's */
  struct sockaddr_in sinc, sind;
  socklen_t len = sizeof(sinc);
  sinc.sin_addr.s_addr = 0;
  sind.sin_addr.s_addr = ~0;
  fd_control[cli].getpeername((struct sockaddr *)&sind, &len);
  fd_control[clientId].getpeername((struct sockaddr *)&sinc, &len);
  if(sinc.sin_addr.s_addr != sind.sin_addr.s_addr) {
    fd_control[cli].write_cmd("TCP: Error in request (IP does not match).\r\n");
    LOGMSG("Invalid data connection (TCP) request: IP does not match: ctrl %x, data %x",
           (unsigned int)sinc.sin_addr.s_addr, (unsigned int)sind.sin_addr.s_addr);
    CloseConnection(cli);
    return;
  }
  if(htonl(ipc) != sinc.sin_addr.s_addr || htons(portc) != sinc.sin_port) {
    fd_control[cli].write_cmd("TCP: Error in request (invalid IP:port).\r\n");
    LOGMSG("Invalid data connection (TCP) request: control IP:port does not match"
           "control: %x:%d client: %x:%d",
           (unsigned int)sinc.sin_addr.s_addr, (unsigned int)sinc.sin_port,
           (unsigned int)htonl(ipc), (unsigned int)htons(portc));
    CloseConnection(cli);
    return;
  }
#endif

  /* close old data connection */
  CloseDataConnection(clientId);

  /* change connection type */

  fd_control[cli].write_cmd("DATA\r\n");
  fd_data[clientId] = fd_control[cli].handle(true);

  cli = clientId;

  if (m_Writer[cli])
    delete m_Writer[cli];
  m_Writer[cli] = new cTcpWriter(fd_data[cli]);

  if (m_Header)
    m_Writer[cli]->Put(sidVdr, 0, m_Header, m_HeaderLength);

  /* not anymore control connection, so dec primary device reference counter */
  m_Dev->ForcePrimaryDevice(false);
}

void cXinelibServer::Handle_Control_RTP(int cli, const char *arg)
{
  LOGDBG("Trying RTP connection ...");

  CloseDataConnection(cli);

  if(!xc.remote_usertp) {
    fd_control[cli].write_cmd("RTP: RTP transport disabled in configuration.\r\n");
    LOGMSG("RTP transports disabled");
    return;
  }

  fd_control[cli].printf("RTP %s:%d\r\n", xc.remote_rtp_addr, xc.remote_rtp_port);

  if(!m_iMulticastMask && !xc.remote_rtp_always_on)
    m_Scheduler->AddRtp();

  m_bMulticast[cli] = true;
  m_iMulticastMask |= (1<<cli);

  // Send padding packet before header (PAT/PMT).
  // Client uses first received UDP/RTP packet to test connection.
  m_Scheduler->QueuePadding();
  if (m_Header)
    m_Scheduler->Queue(sidVdr, 0, m_Header, m_HeaderLength);
}

void cXinelibServer::Handle_Control_UDP(int cli, const char *arg)
{
  LOGDBG("Trying UDP connection ...");

  CloseDataConnection(cli);

  if(!xc.remote_useudp) {
    fd_control[cli].write_cmd("UDP: UDP transport disabled in configuration.\r\n");
    LOGMSG("UDP transport disabled in configuration");
    return;
  }

  int fd = sock_connect(fd_control[cli].handle(), atoi(arg), SOCK_DGRAM);
  if(fd < 0) {
    LOGERR("socket() for UDP failed");
    fd_control[cli].write_cmd("UDP: Socked failed.\r\n");
    return;
  }

  fd_control[cli].write_cmd("UDP OK\r\n");
  m_bUdp[cli] = true;
  fd_data[cli] = fd;
  m_Scheduler->AddHandle(fd);

  // Send padding packet before header (PAT/PMT).
  // Client uses first received UDP/RTP packet to test connection.
  m_Scheduler->QueuePadding();
  if (m_Header)
    m_Scheduler->Queue(sidVdr, 0, m_Header, m_HeaderLength);
}

void cXinelibServer::Handle_Control_KEY(int cli, const char *arg)
{
  TRACE("cXinelibServer received KEY " << buf);

  if(!xc.remote_keyboard) {
    LOGMSG("Handle_Control_KEY(%s): Remote keyboard disabled in config", arg);
    return;
  }

  char buf[256], *pt, *key;
  bool repeat = false, release = false;
  strn0cpy(buf, arg, sizeof(buf));

  size_t n = *buf ? strlen(buf)-1 : 0;
  while(n && buf[n]==' ') buf[n--]=0; /* trailing spaces */
  if(NULL != (key=strchr(buf, ' '))) {
    while(*key == ' ')
      *(key++) = 0;
    if(NULL != (pt = strchr(key, ' '))) {
      *(pt++) = 0;
      if(strstr(pt, "Repeat"))
        repeat = true;
      if(strstr(pt, "Release"))
        release = true;
    }
    cXinelibThread::KeypressHandler(buf, key, repeat, release);
  } else {
    cXinelibThread::KeypressHandler(NULL, buf, repeat, release);
  }
}

void cXinelibServer::Handle_Control_CONFIG(int cli)
{
  m_bConfigOk[cli] = true;

  fd_control[cli].set_nodelay(true);

  fd_control[cli].printf("NOVIDEO %d\r\nLIVE %d\r\n",
                         m_bNoVideo?1:0, m_bLiveMode?1:0);
  fd_control[cli].printf("TRICKSPEED %d%s\r\n",
                         m_TrickSpeed, m_bTrickSpeedBack ? " Backwards" : "");

  SetVolume(m_Volume);

  Configure();

  fd_control[cli].write_cmd("CLEAR\r\n");

  if(m_bPlayingFile && *m_FileName) {
    Unlock();
    int pos = m_Dev->PlayFileCtrl("GETPOS");
    Lock();
    if(m_bPlayingFile && *m_FileName) {
      fd_control[cli].printf("PLAYFILE %d %s %s\r\n",
                             (pos>0?pos/1000:0), xc.audio_visualization, *m_FileName);
    }
  }

  cXinelibOsdProvider::RefreshOsd();
}

void cXinelibServer::Handle_Control_UDP_RESEND(int cli, const char *arg)
{
  unsigned int seq1, seq2;
  uint64_t pos;

  if( (!fd_data[cli] || !m_bUdp[cli]) &&
      (!m_bMulticast[cli])) {
    LOGMSG("Got invalid re-send request: no udp/rtp in use");
    return;
  }

  if(3 == sscanf(arg, "%d-%d %" PRIu64, &seq1, &seq2, &pos)) {

    if(seq1 <= UDP_SEQ_MASK && seq2 <= UDP_SEQ_MASK && pos <= m_StreamPos) {

      if(fd_data[cli] >= 0)
        m_Scheduler->ReSend(fd_data[cli], pos, seq1, seq2);
      else
        m_Scheduler->ReSend(-1, pos, seq1, seq2);
    } else {
      LOGMSG("Invalid re-send request: %s (send pos=%" PRIu64 ")",
             arg, m_StreamPos);
    }
  } else {
    LOGMSG("Invalid re-send request: %s (send pos=%" PRIu64 ")",
           arg, m_StreamPos);
  }
}

void cXinelibServer::Handle_Control_GRAB(int cli, const char *arg)
{
  cGrabReplyFuture *f;
  int token = -1, size = 0;
  if(2 == sscanf(arg, "%d %d", &token, &size)) {
    if(size > 0 && size < 20480000) {
      uchar *result = (uchar*)malloc(size);
      Unlock(); /* may take a while ... */
      ssize_t n = fd_control[cli].read(result, size, 2000);
      Lock();
      if(n == size) {
        if(NULL != (f = (cGrabReplyFuture*)m_Futures->Get(token))) {
          grab_result_t r;
          r.Size = size;
          r.Data = result;
          m_Futures->Del(f, token);
          f->Set(r);
          result = NULL;
        } else {
          LOGMSG("cXinelibServer: Grab image discarded");
        }
      } else {
        LOGMSG("cXinelibServer: Grab result read() failed");
        CloseConnection(cli);
      }
      free(result);
    } else if(NULL != (f = (cGrabReplyFuture*)m_Futures->Get(token))) {
      grab_result_t r;
      r.Size = 0;
      r.Data = NULL;
      m_Futures->Del(f, token);
      f->Set(r);
    }
  }
}

void cXinelibServer::Handle_Control_CONTROL(int cli, const char *arg)
{
  fd_control[cli].printf("VDR-" VDRVERSION " "
                         "xineliboutput-" XINELIBOUTPUT_VERSION " "
                         "READY\r\nCLIENT-ID %d\r\n", cli);
  m_ConnType[cli] = ctControl;
}

static int strcmp_escaped(const char *s1, const char *s2)
{
  if(!strncmp(s1, "file:", 5))
    s1 += 5;

  while(*s1 && *s2) {
    int c1 = *s1;
    int c2 = *s2;
    if(c1 == '%' && s1[1] && s1[2] && 1 == sscanf(s1+1, "%02x", &c1)) s1 += 2;
    if(c2 == '%' && s2[1] && s2[2] && 1 == sscanf(s2+1, "%02x", &c2)) s2 += 2;
    if(c1 < c2) return -1;
    if(c1 > c2) return 1;
    s1++; s2++;
  }
  return *s1 ? -1 : *s2 ? 1 : 0;
}

void cXinelibServer::Handle_Control_HTTP(int cli, const char *arg)
{
  // Parse request
  if(m_ConnType[cli] == ctDetecting || !m_State[cli]) {
    LOGDBG("HTTP request: %s", arg);

    DELETENULL(m_Writer[cli]);
    DELETENULL(m_State[cli]);

    m_State[cli] = new cConnState;
    if( !m_State[cli]->SetCommand(arg) ||
        strncmp(m_State[cli]->Version(), "HTTP/1.", 7) ||
        strcmp(m_State[cli]->Name(), "GET")) {
      LOGMSG("invalid HTTP request: %s", arg);
      CloseConnection(cli);
      return;
    }
    m_ConnType[cli] = ctHttp;
    return;
  }

  // Handle request
  else if(m_ConnType[cli] == ctHttp) {
    LOGDBG("HTTP(%d): %s", cli, arg);

    // Collect headers
    if(*arg) {
      m_State[cli]->AddHeader(arg);
      return;
    }

    LOGMSG("HTTP Request complete");

    //
    // primary device output (PES)
    //
    if(!strcmp(m_State[cli]->Uri(), "/")) {

      if(!xc.remote_use_http) {
        LOGMSG("HTTP transport disabled in configuration");
        fd_control[cli].write_cmd(HTTP_REPLY_404);
        LOGDBG("HTTP Reply: HTTP/1.1 404 Not Found");
        CloseConnection(cli);
        return;
      }

      LOGMSG("HTTP streaming primary device feed");
      fd_control[cli].write_cmd(HTTP_REPLY_200_PRIMARY);
#if 0
      // pack header (scr 0, mux rate 0x6270)
      fd_control[cli].write(
            "\x00\x00\x01\xba"
            "\x44\x00\x04\x00" "\x04\x01\x01\x89" "\xc3\xf8", 14);
      // system header (streams C0, E0, BD, BF)
      fd_control[cli].write(
            "\x00\x00\x01\xbb" "\x00\x12"
            "\x80\xc4\xe1" "\x00\xe1" "\x7f"
            "\xb9\xe0\xe8" "\xb8\xc0\x20" "\xbd\xe0\x3a" "\xbf\xe0\x02", 24);
#endif
      m_Writer[cli] = new cRawWriter(fd_control[cli].handle(), KILOBYTE(1024));

      if (m_Header)
        m_Writer[cli]->Put(sidVdr, 0, m_Header, m_HeaderLength);

      DELETENULL(m_State[cli]);
      return;
    }

    //
    // currently playing media file
    //
    else if(!strncmp(m_State[cli]->Uri(), "/PLAYFILE", 9)) {

      if(!xc.remote_http_files) {
        LOGMSG("HTTP transport for media files disabled in configuration");
        fd_control[cli].write_cmd(HTTP_REPLY_404);
        LOGDBG("HTTP Reply: HTTP/1.1 404 Not Found");
        CloseConnection(cli);
        return;
      }

      if( *m_FileName && m_bPlayingFile) {
        cString file = m_FileName;
        const char *pos = strstr(m_FileName, "#subtitle:");
        if(pos)
          file.Truncate(pos - m_FileName);
        bool Allow = ( !strcmp_escaped(file, m_State[cli]->Uri() + 9)
                       || !strcmp_escaped(xc.audio_vis_image_mrl, m_State[cli]->Uri() + 9)
                       || (pos && !strcmp_escaped(pos + 10, m_State[cli]->Uri() + 9)));
        if(Allow) {
          LOGMSG("HTTP streaming media file");

          // detach socket
          new cHttpStreamer(fd_control[cli].handle(true), m_State[cli]->Uri() + 9, m_State[cli]);
          m_State[cli] = NULL;
          CloseConnection(cli);
          return;
        }
        LOGDBG("HTTP Unauthorized request: %s", *m_State[cli]->Uri());
      }
      else
        LOGDBG("No currently playing file");
    }

    //
    // nothing else will be served ...
    //
    LOGMSG("Rejected HTTP request for \'%s\'", *m_State[cli]->Uri());
    fd_control[cli].write_cmd(HTTP_REPLY_404);
    LOGDBG("HTTP Reply: HTTP/1.1 404 Not Found");
    CloseConnection(cli);
  }
}

#define RTSP_200_OK "RTSP/1.0 200 OK\r\n" \
                    "CSeq: %d\r\n"
#define RTSP_401    "RTSP/1.0 401 Unauthorized\r\n" \
                    "CSeq: %d\r\n" RTSP_FIN
#define RTSP_415    "RTSP/1.0 415 Unsupported media type\r\n" \
                    "CSeq: %d\r\n" RTSP_FIN
#define RTSP_461    "RTSP/1.0 461 Unsupported transport\r\n" \
                    "CSeq: %d\r\n" RTSP_FIN
#define RTSP_501    "RTSP/1.0 501 Not implemented\r\n" \
                    "CSeq: %d\r\n" RTSP_FIN
#define RTSP_FIN    "\r\n", CSeq

#define RTSPOUT(x...) do { fd_control[cli].printf(x); LOGMSG("RTSP TX:" x); } while(0)
//#define RTSPOUT(x...) fd_control[cli].printf_cmd(x)

void cXinelibServer::Handle_Control_RTSP(int cli, const char *arg)
{
  //
  // Minimal RTSP (RFC 2326) server implementation
  //


  //
  // collect request and headers
  //
  if(m_ConnType[cli] == ctDetecting || !m_State[cli]) {
    LOGDBG("RTSP request: %s", arg);

    DELETENULL(m_State[cli]);
    m_State[cli] = new cConnState;

    if( !m_State[cli]->SetCommand(arg) ||
        strcmp(m_State[cli]->Version(), "RTSP/1.0")) {
      LOGMSG("invalid RTSP request: %s", arg);
      CloseConnection(cli);
      return;
    }
    m_ConnType[cli] = ctRtsp;
    return;
  }

  //
  // Process complete request
  //
  else if(m_ConnType[cli] == ctRtsp) {
    LOGDBG("RTSP(%d): %s", cli, arg);

    if(*arg) {
      m_State[cli]->AddHeader(arg);
      return;
    }

    cHeader *cseq = m_State[cli]->Header("CSeq");
    int CSeq = cseq ? cseq->IntValue() : -1;
    LOGMSG("RTSP Request complete (cseq %d)", CSeq);

    if(!xc.remote_use_rtsp) {
      LOGMSG("RTSP transport disabled in configuration");
      fd_control[cli].write_cmd(RTSP_401);
      LOGDBG("HTTP Reply: HTTP/1.1 404 Not Found");
      CloseConnection(cli);
      return;
    }

    //
    // OPTIONS rtsp://127.0.0.1:37890 RTSP/1.0
    // CSeq: 1
    //
    if(!strcmp(m_State[cli]->Name(), "OPTIONS")) {
      RTSPOUT(RTSP_200_OK
              "Public: DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
              RTSP_FIN);
    } // OPTIONS

    //
    // DESCRIBE rtsp://127.0.0.1:37890 RTSP/1.0
    // CSeq: 2
    // Accept: application/sdp
    //
    else if(!strcmp(m_State[cli]->Name(), "DESCRIBE")) {
      cHeader *accept = m_State[cli]->Header("Accept");
      if(accept && strstr(accept->Value(), "application/sdp")) {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        char buf[64];
        uint32_t payload_type = VDRVERSNUM > 10702 ? SDP_PAYLOAD_MPEG_TS : SDP_PAYLOAD_MPEG_PES;
        fd_control[cli].getsockname((struct sockaddr *)&sin, &len);
        const char *sdp_descr = vdr_sdp_description(cxSocket::ip2txt(sin.sin_addr.s_addr,
                                                                     sin.sin_port, buf),
                                                    2001,
                                                    xc.listen_port,
                                                    xc.remote_rtp_addr,
                                                    payload_type,
                                                    /*m_ssrc*/0x4df73452,
                                                    xc.remote_rtp_port,
                                                    xc.remote_rtp_ttl);
        size_t sdplen = sdp_descr ? strlen(sdp_descr) : 0;
        RTSPOUT(RTSP_200_OK
                "Content-Type: application/sdp\r\n"
                "Content-Length: %lu\r\n"
                "\r\n",
                CSeq, (unsigned long)sdplen);
        fd_control[cli].write_cmd(sdp_descr, sdplen);
      } else {
        RTSPOUT(RTSP_415 /*UNSUPPORTED_MEDIATYPE*/);
      }
    } // DESCRIBE

    //
    // SETUP rtsp://127.0.0.1:37890/ RTSP/1.0
    // CSeq: 15
    // Transport: RTP/AVP;unicast;client_port=37890-37891
    // User-Agent: VLC media player (LIVE555 Streaming Media v2005.11.10)
    //
    else if(!strcmp(m_State[cli]->Name(), "SETUP")) {
      cHeader *transport = m_State[cli]->Header("Transport");
      int urtp=0, mrtp=0, tcp=0;
      if(transport &&
         ( (strstr(transport->Value(), "RTP/AVP;multicast") && (mrtp=1)) ||
           (strstr(transport->Value(), "RTP/AVP;unicast") && (urtp=1)) ||
           (strstr(transport->Value(), "RTP/AVP;interleaved") && (tcp=1)))) {
        //if(!mrtp)
        //  sprintf(buf, "RTSP/1.0 461 Unsupported transport\r\n" RTSP_H_CSEQ RTSP_OK_FIN);
        //else
        RTSPOUT(RTSP_200_OK
                "Session: %u\r\n"
                "Transport: RTP/AVP;multicast;destination=224.8.4.9;server_port=37890-37891\r\n"
                RTSP_FIN,
                cli);
      } else {
        RTSPOUT(RTSP_461 /*UNSUPPORTED_TRANSPORT*/ );
      }
    } // SETUP

    //
    // PLAY rtsp://127.0.0.1:37890 RTSP/1.0
    // CSeq: 13
    // Session: 0
    // Range: npt=0.000-
    // User-Agent: VLC media player (LIVE555 Streaming Media v2005.11.10)
    //
    else if(!strcmp(m_State[cli]->Name(), "PLAY")) {
      RTSPOUT(RTSP_200_OK
              RTSP_FIN);

      if(!m_iMulticastMask && !xc.remote_rtp_always_on)
        m_Scheduler->AddRtp();

      m_bMulticast[cli] = true;
      m_iMulticastMask |= (1<<cli);
#if 0
      //udp
      int fd = sock_connect(fd_control[cli], atoi(arg), SOCK_DGRAM);
      if(fd < 0) {
        LOGERR("socket() for UDP failed");
        write_cmd(fd_control[cli], "UDP: Socked failed.\r\n");
        return;
      }
      m_bUdp[cli] = true;
      fd_data[cli] = fd;
      m_Scheduler->AddHandle(fd);
#endif
    } // PLAY

    //
    // TEARDOWN rtsp://127.0.0.1:37890 RTSP/1.0
    // CSeq: 39
    // Session: 1
    // User-Agent: VLC media player (LIVE555 Streaming Media v2005.11.10)
    //
    else if(!strcmp(m_State[cli]->Name(), "TEARDOWN")) {
      RTSPOUT(RTSP_200_OK
              RTSP_FIN);
      CloseConnection(cli);
    } // TEARDOWN

    //
    // unknown/unsupported request
    //
    else {
      LOGMSG("Unsupported RTSP request: %s", *m_State[cli]->Name());
      RTSPOUT(RTSP_501 /*NOT_IMPLEMENTED*/);
    } // unsupported request


    // dispose buffer
    DELETENULL(m_State[cli]);

  } // ConnectionType == ctRtsp
}

void cXinelibServer::Handle_Control(int cli, const char *cmd)
{
  TRACEF("cXinelibServer::Handle_Control");

  LOGVERBOSE("Server received %s", cmd);

  /* Order of tests is significant !!!
     (example: UDP 2\r\n or UDP FULL 1\r\n) */

  if(!strncasecmp(cmd, "OPTIONS ", 8) ||
     !strncasecmp(cmd, "SETUP ", 6) ||
     !strncasecmp(cmd, "DESCRIBE ", 9) ||
     m_ConnType[cli] == ctRtsp) {

    Handle_Control_RTSP(cli, cmd);

  } else if(!strncasecmp(cmd, "GET ", 4) ||
            m_ConnType[cli] == ctHttp) {

    Handle_Control_HTTP(cli, cmd);

  } else if(!strncasecmp(cmd, "PIPE OPEN", 9)) {
    LOGDBG("Pipe open");

  } else if(!strncasecmp(cmd, "PIPE", 4)) {
    Handle_Control_PIPE(cli, cmd+4);

  } else if(!strncasecmp(cmd, "RTP", 3)) {
    Handle_Control_RTP(cli, cmd+4);

  } else if(!strncasecmp(cmd, "UDP FULL", 8)) {

  } else if(!strncasecmp(cmd, "UDP RESEND ", 11)) {
    Handle_Control_UDP_RESEND(cli, cmd+11);

  } else if(!strncasecmp(cmd, "UDP ", 4)) {
    Handle_Control_UDP(cli, cmd+4);

  } else if(!strncasecmp(cmd, "DATA ", 5)) {
    Handle_Control_DATA(cli, cmd+5);

  } else if(!strncasecmp(cmd, "KEY ", 4)) {
    Handle_Control_KEY(cli, cmd+4);

  } else if(!strncasecmp(cmd, "CONFIG", 6)) {
    Handle_Control_CONFIG(cli);

  } else if(!strncasecmp(cmd, "STC ", 4)) {
    int64_t pts = -1;
    if(1 == sscanf(cmd+4, "%" PRId64, &pts))
      m_StcFuture->Set(pts);

  } else if(!strncasecmp(cmd, "ENDOFSTREAM", 11)) {
    m_bEndOfStreamReached = true;

  } else if(!strncasecmp(cmd, "RESULT ", 7)) {
    int token = -1, result = -1;
    if(2 == sscanf(cmd+7, "%d %d", &token, &result)) {
      cReplyFuture *f = m_Futures->Get(token);
      if(f) {
        m_Futures->Del(f, token);
        f->Set(result);
      }
    }

  } else if(!strncmp(cmd, "INFO ", 5)) {

    if(!strncmp(cmd, "INFO ARGBOSD", 12))
      m_bArgbOSD[cli] = true;

    if(!*xc.local_frontend || !strncmp(xc.local_frontend, "none", 4))
      cXinelibThread::InfoHandler(cmd+5);

  } else if(!strncasecmp(cmd, "GRAB ", 5)) {
    Handle_Control_GRAB(cli, cmd+5);

  } else if(!strncasecmp(cmd, "CLOSE", 5)) {
    CloseConnection(cli);

  } else if(!strncasecmp(cmd, "CONTROL", 7)) {
    Handle_Control_CONTROL(cli, cmd);

  }
}

void cXinelibServer::Read_Control(int cli)
{
  int n;
  while((n = fd_control[cli].recv(&m_CtrlBuf[ cli ][ m_CtrlBufPos[cli] ], 1)) == 1) {

    ++m_CtrlBufPos[cli];

    if( m_CtrlBufPos[cli] > CTRL_BUF_SIZE-2) {
      LOGMSG("Received too long control message from client %d (%d bytes)",
             cli, m_CtrlBufPos[cli]);
      LOGMSG("%81s",m_CtrlBuf[cli]);
      CloseConnection(cli);
      return;
    }

    if( m_CtrlBufPos[cli] > 1 &&
        m_CtrlBuf[ cli ][ m_CtrlBufPos[cli] - 2 ] == '\r' &&
        m_CtrlBuf[ cli ][ m_CtrlBufPos[cli] - 1 ] == '\n') {

      m_CtrlBufPos[cli] -= 2;
      m_CtrlBuf[ cli ][ m_CtrlBufPos[cli] ] = 0;

      Handle_Control(cli, m_CtrlBuf[cli]);

      m_CtrlBufPos[cli] = 0;
    }
  }
  if (n == 0) {
    LOGMSG("Client connection %d closed", cli);
    CloseConnection(cli);
  }
}

void cXinelibServer::Handle_ClientConnected(int fd)
{
  char buf[64];
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  int cli;

  for(cli=0; cli<MAXCLIENTS; cli++)
    if(!fd_control[cli].open())
      break;

  if(getpeername(fd, (struct sockaddr *)&sin, &len)) {
    LOGERR("getpeername() failed, dropping new incoming connection %d", cli);
    CLOSESOCKET(fd);
    return;
  }

  LOGMSG("Client %d connected: %s", cli,
         cxSocket::ip2txt(sin.sin_addr.s_addr, sin.sin_port, buf));

  cAllowedHosts AllowedHosts(m_AllowedHostsFile);
  if (!AllowedHosts.Acceptable(sin.sin_addr.s_addr)) {
    const char *msg = "Access denied.\r\n";
    ssize_t len = strlen(msg);
    LOGMSG("Address not allowed to connect (%s)", *m_AllowedHostsFile);
    if(write(fd, msg, len) != len)
       LOGERR("Write failed.");
    CLOSESOCKET(fd);
    return;
  }

  if(cli>=xc.remote_max_clients || cli>=MAXCLIENTS) {
    const char *msg = "Server busy.\r\n";
    ssize_t len = strlen(msg);
    // too many clients
    LOGMSG("Too many clients, connection refused");
    if(write(fd, msg, len) != len)
      LOGERR("Write failed.");
    CLOSESOCKET(fd);
    return;
  }

  if (fcntl (fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK) == -1) {
    LOGERR("Error setting control socket to nonblocking mode");
    CLOSESOCKET(fd);
    return;
  }

  int alive = 1;
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &alive, sizeof(alive));

  CloseDataConnection(cli);

  m_OsdTimeouts[cli] = 0;
  m_CtrlBufPos[cli] = 0;
  m_CtrlBuf[cli][0] = 0;
  m_ConnType[cli] = ctDetecting;
  fd_control[cli].set_handle(fd);
  fd_control[cli].set_buffers(KILOBYTE(128), KILOBYTE(128));

  if (!m_Dev->ForcePrimaryDevice(true)) {
    const char *msg = "Not primary device.\r\n";
    ssize_t len = strlen(msg);
    LOGMSG("Dropping client: xineliboutput is not the primary device !");
    if(write(fd, msg, len) != len)
      LOGERR("Write failed.");
    CLOSESOCKET(fd);
    return;
  }
}

void cXinelibServer::Handle_Discovery_Broadcast(void)
{
  if(!xc.remote_usebcast) {
    LOGDBG("BROADCASTS disabled in configuration");
    CLOSESOCKET(fd_discovery);
    return;
  }

  char buf[DISCOVERY_MSG_MAXSIZE] = {0};
  struct sockaddr_in from;

  if(udp_discovery_recv(fd_discovery, buf, 0, &from) > 0)
    if(udp_discovery_is_valid_search(buf)) {

      // Reply only if we can accept one more client
      int clients = 0;
      for(int c=0; c<MAXCLIENTS; c++)
        if(fd_control[c].open())
          clients++;
      if(clients >= xc.remote_max_clients) {
        LOGMSG("Not replying to discovery broadcast (too many clients)");
        return;
      }

      udp_discovery_broadcast(fd_discovery, m_Port, xc.remote_local_ip);
    }
}

void cXinelibServer::Action(void)
{
  TRACEF("cXinelibServer::Action");

  int    i, fds=0;
  pollfd pfd[MAXCLIENTS];

  /* higher priority */
  if (have_cap_sys_nice())
    SetPriority(-1);

  sched_param temp;
  temp.sched_priority = 2;

  /* request real-time scheduling */
  if (!pthread_setschedparam(pthread_self(), SCHED_RR, &temp)) {
    LOGDBG("cXinelibServer priority set successful SCHED_RR %d [%d,%d]",
           temp.sched_priority,
           sched_get_priority_min(SCHED_RR),
           sched_get_priority_max(SCHED_RR));
  } else {
    LOGDBG("cXinelibServer: Can't set priority to SCHED_RR %d [%d,%d]",
           temp.sched_priority,
           sched_get_priority_min(SCHED_RR),
           sched_get_priority_max(SCHED_RR));
  }
  errno = 0;

  Lock();
  Listen(m_Port);
  m_bReady=true;

  if(fd_listen>=0)
    while (Running() && fds>=0) {

      fds = 0;
      if(fd_listen>=0) {
        pfd[fds].fd = fd_listen;
        pfd[fds++].events = POLLIN;
      }
      if(fd_discovery >= 0) {
        pfd[fds].fd = fd_discovery;
        pfd[fds++].events = POLLIN;
      }

      for(i=0; i<MAXCLIENTS; i++) {
        if(fd_control[i].open()) {
          pfd[fds].fd = fd_control[i].handle();
          pfd[fds++].events = POLLIN;
        }
        if(fd_data[i]>=0) {
          pfd[fds].fd = fd_data[i];
          pfd[fds++].events = 0; /* check for errors only */
        }
      }
      Unlock();

      int err = poll(pfd,fds,1000);

      if(err < 0) {
        LOGERR("cXinelibServer: poll failed");
        if(Running())
          cCondWait::SleepMs(100);

      } else if(err == 0) {
        // poll timeout

      } else {
        Lock();
        for(int f=0; f<fds; f++) {

          // Check errors (closed connections etc.)
          if(pfd[f].revents & (POLLERR|POLLHUP|POLLNVAL)) {

            if(pfd[f].fd == fd_listen) {
              LOGERR("cXinelibServer: listen socket error");
              CLOSESOCKET(fd_listen);
              cCondWait::SleepMs(100);
              Listen(m_Port);
            } /* fd_listen */

            else if(pfd[f].fd == fd_discovery) {
              LOGERR("cXinelibServer: discovery socket error");
              CLOSESOCKET(fd_discovery);
            } /* fd_discovery */

            else /* fd_data[] / fd_control[] */ {
              for(i=0; i<MAXCLIENTS; i++) {
                if(pfd[f].fd == fd_data[i] || pfd[f].fd == fd_control[i].handle()) {
                  LOGMSG("Client %d disconnected", i);
                  CloseConnection(i);
                }
              }
            } /* fd_data / fd_control */

          } /* Check ERRORS */

          // Check ready for reading
          else if(pfd[f].revents & POLLIN) {

            // New connection
            if(pfd[f].fd == fd_listen) {
              int fd = accept(fd_listen, 0, 0);
              if(fd>=0)
                Handle_ClientConnected(fd);
            } /* fd_listen */

            // VDR Discovery
            else if(pfd[f].fd == fd_discovery) {
              Handle_Discovery_Broadcast();
            } /* fd_discovery */

            // Control data
            else {
              for(i=0; i<MAXCLIENTS; i++) {
                if(pfd[f].fd == fd_control[i].handle()) {
                  Read_Control(i);
                  break;
                }
              }
            } /* fd_control */

          } /* Check ready for reading */

        } /* for(fds) */

        Unlock();
      } /* Check poll result */

      Lock();
    } /* while running */

  m_bReady = false;
  Unlock();
}
