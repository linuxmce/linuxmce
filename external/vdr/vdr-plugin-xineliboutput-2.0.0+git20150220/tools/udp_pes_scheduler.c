/*
 * udp_pes_scheduler.h: PES scheduler for UDP/RTP streams
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: udp_pes_scheduler.c,v 1.57 2013-10-25 17:22:38 rofafor Exp $
 *
 */

//#define LOG_UDP_RATE
//#define LOG_RESEND
//#define LOG_SCR

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS 
#include <inttypes.h>

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/videodir.h>

#include "../logdefs.h"            // logging
#include "../config.h"             // configuration data
#include "../xine_input_vdr_net.h" // frame headers and constants

#include "ts.h"
#include "pes.h"
#include "udp_buffer.h"
#include "time_pts.h"
#include "cxsocket.h"
#include "sap.h"  // SAP  - Session Announcement Protocol
#include "sdp.h"  // SDP  - Session Description Protocol
#include "rtcp.h" // RTCP
#include "sys_cap.h"

#include "udp_pes_scheduler.h"


#ifdef LOG_RESEND
#  define LOGRESEND LOGDBG
#else
#  define LOGRESEND(x...)
#endif

#ifdef LOG_SCR
#  define LOGSCR  LOGDBG
#else
#  define LOGSCR(x...)
#endif

/*
 * constants
 */

const uint    MAX_QUEUE_SIZE      = 64;       // ~ 65 ms with typical DVB stream
const uint    MAX_LIVE_QUEUE_SIZE = (64+60);  // ~ 100 ms with typical DVB stream
const uint    HARD_LIMIT          = (4*1024); // ~ 40 Mbit/s === 4 Mb/s

const uint    MAX_BURST_BYTES     = 32768;    // 32 kb
const uint    MAX_BURST_FRAMES    = 15;       // 15 UDP packets

const uint    RTCP_MIN_INTERVAL   = 45000;    // 500 ms (pts units)

// initial burst length after seek (500ms = ~13 video frames)
const int64_t INITIAL_BURST_TIME  = INT64_C(45000);  // pts units (90kHz)

// assume seek when when pts difference between two frames exceeds this (2,5 seconds)
const int64_t JUMP_LIMIT_TIME     = INT64_C(225000); // pts units (90kHz)

const uint    SCHEDULER_MIN_DELAY_MS = 3;
const uint    SCHEDULER_MAX_DELAY_MS = 20;

static inline int64_t abs64(int64_t val) { return val<0 ? -val : val; }

cUdpScheduler::cUdpScheduler()
{

  // Scheduler data

  m_ScrSource         = eScrDetect;
  m_CurrentAudioVtime = 0;
  m_CurrentVideoVtime = 0;
  m_CurrentPcr        = 0;

  m_MasterClock.Set(INT64_C(0));

  m_BurstBytes  = 0;
  m_BurstFrames = 0;

  m_Master = false;
  m_TrickSpeed = false;

  // RTP

  srandom(time(NULL) ^ getpid());

  m_ssrc = random();
  LOGDBG("RTP SSRC: 0x%08x", m_ssrc);
  m_LastRtcpTime = 0;
  m_Frames = 0;
  m_Octets = 0;
  m_RtpScr.Set((int64_t)random());

  m_fd_sap = -1;

  // Queuing

  int i;
  for(i=0; i<MAX_UDP_HANDLES; i++)
    m_Handles[i] = -1;

  m_BackLog = new cUdpBackLog;

  m_QueueNextSeq = 0;
  m_QueuePending = 0;

  // Thread

  Start();
}

cUdpScheduler::~cUdpScheduler()
{
  m_Lock.Lock();

  Cancel(-1);
  m_Cond.Broadcast();
  m_CondWait.Signal();
  m_Lock.Unlock();

  Cancel(3);

  if(m_fd_rtcp.open() || m_fd_rtp.open())
    Send_SAP(false);

  CLOSESOCKET(m_fd_sap);

  delete m_BackLog;
}

void cUdpScheduler::Scheduler_Sleep(int ms)
{
  m_CondWait.Wait(ms);
  m_BurstBytes = 0;
  m_BurstFrames = 0;
}

bool cUdpScheduler::AddRtp(void) 
{
  cMutexLock ml(&m_Lock);

  if(m_fd_rtcp.open()) {
    LOGERR("cUdpScheduler::AddHandle: RTCP socket already open !");
    Send_SAP(false);
    m_fd_rtcp.close();
  }

  /* need new ssrc */
  m_ssrc = random();
  LOGDBG("RTP SSRC: 0x%08x", m_ssrc);

  //
  // RTP
  //
  if(! m_fd_rtp.create(cxSocket::estDGRAM)) {
    LOGERR("socket() failed (UDP/RTP multicast)");
    return false;
  }

  // Set buffer sizes
  m_fd_rtp.set_buffers(KILOBYTE(256), 2048);

  // Set multicast socket options
  if(!m_fd_rtp.set_multicast(xc.remote_rtp_ttl)) {
    m_fd_rtp.close();
    return false;
  }

  if(xc.remote_local_ip[0]) {
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = inet_addr(xc.remote_local_ip);
    if(name.sin_addr.s_addr == INADDR_NONE)
      LOGERR("Local address %s is invalid", xc.remote_local_ip);
    name.sin_port = htons(xc.remote_rtp_port);
    if (bind(m_fd_rtp.handle(), (struct sockaddr *)&name, sizeof(name)) < 0)
      LOGERR("bind(%s:%d) failed for udp/rtp multicast", xc.remote_local_ip, xc.remote_rtp_port);
#if 0
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = inet_addr(xc.remote_rtp_addr);
    mreqn.imr_address.s_addr = inet_addr(xc.remote_local_ip);  /* IP address of local interface */
    //mreqn.imr_ifindex = ; /* interface index */
    if(setsockopt(m_fd_rtp.handle(), IPPROTO_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn)))
      LOGERR("setting multicast source address/interface failed");
#endif
  }

  // Connect to multicast address
  if(!m_fd_rtp.connect(xc.remote_rtp_addr, xc.remote_rtp_port) && 
     errno != EINPROGRESS) {
    LOGERR("connect(fd_rtp) failed. Address=%s, port=%d",
	   xc.remote_rtp_addr, xc.remote_rtp_port);
    m_fd_rtp.close();
    return false;
  }
  
  // Set to non-blocking mode
  m_fd_rtp.set_blocking(false);
  
  //
  // RTCP
  //
  if(! m_fd_rtcp.create(cxSocket::estDGRAM))
    LOGERR("socket() failed (RTCP multicast)");
    
  m_fd_rtcp.set_buffers(16384, 16384);
  if(!m_fd_rtcp.set_multicast(xc.remote_rtp_ttl))
    m_fd_rtcp.close();

  if(xc.remote_local_ip[0]) {
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = inet_addr(xc.remote_local_ip);
    name.sin_port = htons(xc.remote_rtp_port+1);
    if (bind(m_fd_rtcp.handle(), (struct sockaddr *)&name, sizeof(name)) < 0)
      LOGERR("bind(%s:%d) failed for udp/rtp multicast", xc.remote_local_ip, xc.remote_rtp_port);
#if 0
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = inet_addr(xc.remote_rtp_addr);
    mreqn.imr_address.s_addr = inet_addr(xc.remote_local_ip);  /* IP address of local interface */
    //mreqn.imr_ifindex = ; /* interface index */
    if(setsockopt(m_fd_rtp.handle(), IPPROTO_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn)))
      LOGERR("setting multicast source address/interface failed");
#endif
  }

  /* RTCP port (RFC 1889) */
  if(!m_fd_rtcp.connect(xc.remote_rtp_addr, xc.remote_rtp_port + 1) &&
     errno != EINPROGRESS) {
    LOGERR("connect(fd_rtcp) failed. Address=%s, port=%d",
	   xc.remote_rtp_addr, xc.remote_rtp_port +
	   (xc.remote_rtp_port&1)?-1:1);
    m_fd_rtcp.close();
  }

  // Set to non-blocking mode
  m_fd_rtcp.set_blocking(false);

  // Finished

  if(!AddHandle(m_fd_rtp))
    LOGERR("cUdpScheduler::AddHandle(fd_rtp) failed");
	
  Send_SAP(true);

  return true;
}

bool cUdpScheduler::AddHandle(int fd) 
{
  cMutexLock ml(&m_Lock);

  int i;

  for(i=0; i<MAX_UDP_HANDLES; i++)
    if(m_Handles[i] < 0 || m_Handles[i] == fd) {
      m_Handles[i] = fd;

      /* query socket send buffer size */
      m_wmem[i] = 0x10000; /* default to 64k */
      socklen_t l = sizeof(int);
      if(getsockopt(m_Handles[i], SOL_SOCKET, SO_SNDBUF, &m_wmem[i], &l))
	LOGERR("getsockopt(SO_SNDBUF) failed");
      m_wmem[i] /= 2; /* man 7 socket */

      m_Cond.Broadcast();

      return true;
    }

  return false;
}

void cUdpScheduler::RemoveRtp(void) 
{
  cMutexLock ml(&m_Lock);

  if(m_fd_rtp.open() || m_fd_rtcp.open()) {
    Send_SAP(false);

    RemoveHandle(m_fd_rtp);
    
    m_fd_rtp.close();
    m_fd_rtcp.close();
    CLOSESOCKET(m_fd_sap);
  }
}

void cUdpScheduler::RemoveHandle(int fd) 
{
  cMutexLock ml(&m_Lock);

  int i;
  for(i=0; i<MAX_UDP_HANDLES; i++)
    if(m_Handles[i] == fd)
      break;

  for(; i<MAX_UDP_HANDLES-1; i++)
    m_Handles[i] = m_Handles[i+1];

  m_Handles[MAX_UDP_HANDLES-1] = -1;

  if(m_Handles[0] < 0) {
    // No clients left ...

    // Flush all buffers
    m_QueueNextSeq = 0;
    m_QueuePending = 0;

    m_BackLogDeleteMutex.Lock();
    delete m_BackLog; 
    m_BackLog = new cUdpBackLog;
    m_BackLogDeleteMutex.Unlock();

    m_Frames = 0;
    m_Octets = 0;
  }
}

int cUdpScheduler::Poll(int TimeoutMs, bool Master)
{
  cMutexLock ml(&m_Lock);

  m_Master = Master;

  if(m_Handles[0] < 0) {
    // no clients, so we can eat all data we are given ...
    return DEFAULT_POLL_SIZE;
  }
  
  uint limit = m_Master ? MAX_QUEUE_SIZE : MAX_LIVE_QUEUE_SIZE;
  if(m_QueuePending >= limit) {
    uint64_t WaitEnd = cTimeMs::Now();
    if(TimeoutMs >= 0)
      WaitEnd += (uint64_t)TimeoutMs;

    while (cTimeMs::Now() < WaitEnd &&
           Running() &&
           m_QueuePending >= limit)
      m_Cond.TimedWait(m_Lock, 5);
  }

  return limit > m_QueuePending ? limit - m_QueuePending : 0;
}

bool cUdpScheduler::Flush(int TimeoutMs)
{
  cMutexLock ml(&m_Lock);

  if(m_Handles[0] < 0)
    return true;
  
  if(m_QueuePending > 0) {
    uint64_t WaitEnd = cTimeMs::Now();
    if(TimeoutMs >= 0)
      WaitEnd += (uint64_t)TimeoutMs;

    while (cTimeMs::Now() < WaitEnd &&
           Running() &&
           m_QueuePending > 0)
      m_Cond.TimedWait(m_Lock, 5);
  }
  return m_QueuePending == 0;
}

void cUdpScheduler::Clear(void)
{
  cMutexLock ml(&m_Lock);

  m_BackLog->Clear(m_QueuePending);

  m_QueuePending = 0;
  m_Cond.Broadcast();

  m_ScrSource = eScrDetect;
}

void cUdpScheduler::Pause(bool On)
{
  cMutexLock ml(&m_Lock);

  if(On)
    m_MasterClock.Pause();
  else
    m_MasterClock.Resume();

  m_TrickSpeed = false;
}

void cUdpScheduler::TrickSpeed(const int Multiplier)
{
  cMutexLock ml(&m_Lock);

#ifdef LOG_SCR
  if(Multiplier == 1 || Multiplier == -1) {
    LOGMSG("UDP clock --> normal");
  } else if(Multiplier < 0)
    LOGMSG("UDP clock --> %dx", -Multiplier);
  else
    LOGMSG("UDP clock --> 1/%d", Multiplier);
#endif

  m_MasterClock.TrickSpeed(Multiplier);

  m_TrickSpeed = (Multiplier==-1 || Multiplier==1) ? false : true;
}

void cUdpScheduler::SetScrSpeed(const int Speed)
{
  cMutexLock ml(&m_Lock);

  m_MasterClock.SetScrSpeed(Speed);
  m_RtpScr.SetScrSpeed(Speed);
}

bool cUdpScheduler::Queue(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int Length)
{
  cMutexLock ml(&m_Lock);

  if(m_Handles[0] < 0)
    return true;

  uint limit = m_Master ? MAX_QUEUE_SIZE : MAX_LIVE_QUEUE_SIZE;
  if(m_QueuePending >= limit)
    return false;

  m_BackLog->MakeFrame(StreamId, StreamPos, Data, Length);
  m_QueuePending++;

  m_Cond.Broadcast();

  return true;
}

void cUdpScheduler::QueuePadding(void)
{
  cMutexLock ml(&m_Lock);

  if (m_Handles[0] < 0)
    return;

  if (m_QueuePending > 2)
    return;

  QueuePaddingInternal();

  m_Cond.Broadcast();
}

void cUdpScheduler::QueuePaddingInternal(void)
{
  static unsigned const char Padding[] = {0x00,0x00,0x01,0xBE,0x00,0x02,0xff,0xff};

  int PrevSeq = (m_QueueNextSeq + UDP_BUFFER_SIZE - 1) & UDP_BUFFER_MASK;
  stream_rtp_header_impl_t *Frame = m_BackLog->Get(PrevSeq);
  if (Frame) {
    int      PrevLen = m_BackLog->PayloadSize(PrevSeq);
    uint64_t Pos     = ntohll(Frame->hdr_ext.pos) + PrevLen - 8;
    m_BackLog->MakeFrame(sidPadding, Pos, Padding, 8);
  } else
    m_BackLog->MakeFrame(sidPadding, 0, Padding, 8);

  m_QueuePending++;
}

int cUdpScheduler::CalcElapsedVtime(int64_t pts, ScrSource_t ScrSource)
{
  int64_t diff = 0;

  switch (ScrSource) {

  case eScrFromVideo:
    diff = pts - m_CurrentVideoVtime;
    if (diff > JUMP_LIMIT_TIME || (-diff) > JUMP_LIMIT_TIME) { // 1 s (must be > GOP)
      // RESET
      LOGSCR("cUdpScheduler SCR RESET (Video jump %lld->%lld)", m_CurrentVideoVtime, pts);
      m_CurrentVideoVtime = pts;

      // Use video pts for sync only in audioless trickspeeds
      // (audio has smaller, constant and increasing intervals)
      if (m_TrickSpeed)
	m_MasterClock.Set(m_CurrentVideoVtime + INITIAL_BURST_TIME);

      return -1;
    }
    if (diff < 0)  /* ignore small negative differences (B/P frames are sent out-of-order) */
      diff = 0;
    else
      m_CurrentVideoVtime = pts;
    break;

  case eScrFromAudio:
    diff = abs64(pts - m_CurrentAudioVtime);
    if (diff > JUMP_LIMIT_TIME) { // 1 sec
      // RESET
      LOGSCR("cUdpScheduler SCR RESET (Audio jump %lld->%lld)", m_CurrentAudioVtime, pts);
      m_CurrentAudioVtime = pts;
      m_MasterClock.Set(m_CurrentAudioVtime + INITIAL_BURST_TIME);
      return -1;
    }
    m_CurrentAudioVtime = pts;
    break;

  case eScrFromPcr:
    diff = pts - m_CurrentPcr;
    if (diff > JUMP_LIMIT_TIME || diff < 0) { // 1 sec
      // RESET
      LOGSCR("cUdpScheduler RESET (PCR jump %lld->%lld)", m_CurrentPcr, pts);
      m_CurrentPcr = pts;
      m_MasterClock.Set(m_CurrentPcr + INITIAL_BURST_TIME);
      return -1;
    }
    m_CurrentPcr = pts;
    break;

  default: break;
  }

  return (int) diff;
}

void cUdpScheduler::Send_RTCP(void)
{
  if(!m_fd_rtcp.open())
    return;

  uint64_t scr = m_RtpScr.Now();

  if(scr > (m_LastRtcpTime + RTCP_MIN_INTERVAL)) {
    uint8_t frame[2048], *content = frame;
    char hostname[64] = "";
    rtcp_packet_t  *msg = (rtcp_packet_t *)content;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    gethostname(hostname, sizeof(hostname)-1);
    hostname[sizeof(hostname)-1] = 0;

    // SR (Sender report)
    msg->hdr.raw[0] = 0x81;     // RTP version = 2, Report count = 1 */
    msg->hdr.ptype  = RTCP_SR;
    msg->hdr.length = htons(6); // length 6 dwords

    msg->sr.ssrc     = htonl(m_ssrc);
    msg->sr.ntp_sec  = htonl(tv.tv_sec + 0x83AA7E80);
    msg->sr.ntp_frac = htonl((uint32_t)((double)tv.tv_usec*(double)(1LL<<32)*1.0e-6));
    msg->sr.rtp_ts   = htonl((uint32_t)(scr    & 0xffffffff));
    msg->sr.psent    = htonl((uint32_t)(m_Frames & 0xffffffff));
    msg->sr.osent    = htonl((uint32_t)(m_Octets & 0xffffffff));

    content += sizeof(rtcp_common_t) + sizeof(rtcp_sr_t);
    msg = (rtcp_packet_t *)content;

    // SDES
    msg->hdr.raw[0] = 0x81;       // RTP version = 2, Report count = 1 */
    msg->hdr.ptype  = RTCP_SDES;  
    msg->hdr.count  = 1;

    msg->sdes.ssrc   = m_ssrc;
    msg->sdes.item[0].type   = RTCP_SDES_CNAME;
    sprintf(msg->sdes.item[0].data, "VDR@%s:%d%c%c%c", 
	    hostname[0] ? hostname : xc.remote_rtp_addr,
	    xc.remote_rtp_port, 0, 0, 0);
    msg->sdes.item[0].length = strlen(msg->sdes.item[0].data);
    msg->hdr.length = htons(1 + 1 + ((msg->sdes.item[0].length - 2) + 3) / 4); 
    
    content += sizeof(rtcp_common_t) + 4*ntohs(msg->hdr.length);
    msg = (rtcp_packet_t *)content;

    // Send
#ifndef LOG_RTCP
    (void) m_fd_rtcp.send(frame, content - frame);
#else
    LOGMSG("RTCP send (%d)", m_fd_rtcp.send(frame, content - frame));
    for(int i=0; i<content-frame; i+=16) 
      LOGMSG("%02X %02X %02X %02X %02X %02X %02X %02X  "
	     "%02X %02X %02X %02X %02X %02X %02X %02X  "
	     "  %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c",
	     frame[i+0],frame[i+1],frame[i+2],frame[i+3],
	     frame[i+4],frame[i+5],frame[i+6],frame[i+7],
	     frame[i+8],frame[i+9],frame[i+10],frame[i+11],
	     frame[i+12],frame[i+13],frame[i+14],frame[i+15],
	     frame[i+0],frame[i+1],frame[i+2],frame[i+3],
	     frame[i+4],frame[i+5],frame[i+6],frame[i+7],
	     frame[i+8],frame[i+9],frame[i+10],frame[i+11],
	     frame[i+12],frame[i+13],frame[i+14],frame[i+15]);
#endif

    m_LastRtcpTime = scr;
  }
}


void cUdpScheduler::Send_SAP(bool Announce)
{
  if(!xc.remote_rtp_sap || !m_fd_rtp.open())
    return;

  char ip[64] = "";
  uint32_t local_addr = m_fd_rtp.get_local_address(ip);

  if(!local_addr)
    return;

  uint32_t payload_type = VDRVERSNUM > 10702 ? SDP_PAYLOAD_MPEG_TS : SDP_PAYLOAD_MPEG_PES;
  const char *sdp_descr = vdr_sdp_description(ip,
                                              2001,
                                              xc.listen_port,
                                              xc.remote_rtp_addr,
                                              payload_type,
                                              m_ssrc,
                                              xc.remote_rtp_port,
                                              xc.remote_rtp_ttl);
  if(!sdp_descr)
    return;

#if 1
  /* store copy of SDP data */
  if(m_fd_sap < 0) {
    cString fname = AddDirectory(
#if defined(APIVERSNUM) && (APIVERSNUM < 20102)
				 VideoDirectory,
#else
				 cVideoDirectory::Name(),
#endif
				 cString::sprintf("xineliboutput@%s.sdp",
						  ip));
    FILE *fp = fopen(fname, "w");
    if(fp) {
      fprintf(fp, "%s", sdp_descr);
      fclose(fp);
    }
  }
#endif

  sap_pdu_t *pdu = sap_create_pdu(local_addr,
				  Announce, 
				  (m_ssrc >> 16 | m_ssrc) & 0xffff,
				  "application/sdp",
				  sdp_descr);
      
  if(!sap_send_pdu(&m_fd_sap, pdu, 0))
    LOGERR("SAP/SDP announce failed");
  free(pdu);

  if(!Announce)
    CLOSESOCKET(m_fd_sap);
}

#ifdef LOG_SCR
static const char ScrSourceName[][6] = {"???", "VIDEO", "PS1", "AUDIO", "PCR"};
#endif

void cUdpScheduler::Schedule(const uchar *Data, int Length)
{
  int64_t     pts       = NO_PTS;
  int         elapsed   = 0;
  ScrSource_t ScrSource = eScrDetect;

  // Get timestamp from data

  if (DATA_IS_TS(Data)) {
    if (ts_get_pcr_n(Data, Length/TS_SIZE, &pts)) {
      LOGSCR("UDP PCR: %"PRId64, pts);
      ScrSource = eScrFromPcr;
      elapsed   = CalcElapsedVtime(pts, eScrFromPcr);
    }
  } else /* if (DATA_IS_PES(Data)) */ {
    if (PES_HAS_PTS(Data)) {
      ScrSource = IS_VIDEO_PACKET(Data) ? eScrFromVideo : IS_AUDIO_PACKET(Data) ? eScrFromAudio : eScrDetect;
      pts       = pes_get_pts(Data, Length);
      elapsed   = CalcElapsedVtime(pts, ScrSource);
    }
  }

  if (m_ScrSource < ScrSource)
    m_ScrSource = ScrSource;

  if(elapsed > 0) {

    int64_t now      = m_MasterClock.Now();
    int64_t SendTime = ScrSource==eScrFromVideo ? m_CurrentVideoVtime :
                       ScrSource==eScrFromAudio ? m_CurrentAudioVtime :
                       ScrSource==eScrFromPcr   ? m_CurrentPcr        : now;

    LOGSCR("PTS: %lld  (%s) elapsed %d ms (PID %02x)", pts, ScrSourceStr[ScrSource],
           pts_to_ms(elapsed), DATA_IS_TS(Data) ? TS_PID(Data) : Data[3]);

    //
    // Detect discontinuity (against SCR)
    //
    if (now > SendTime && (now - SendTime) > JUMP_LIMIT_TIME) {
      LOGSCR("cUdpScheduler MasterClock init (was in past) %s", ScrSourceStr[ScrSource]);
      now = SendTime + INITIAL_BURST_TIME;
      m_MasterClock.Set(now);
    } else if (now < SendTime && (SendTime - now) > JUMP_LIMIT_TIME) {
      LOGSCR("cUdpScheduler MasterClock init (was in future) %s", ScrSourceStr[ScrSource]);
      now = SendTime + INITIAL_BURST_TIME;
      m_MasterClock.Set(now);
    }

    //
    // Delay
    //
    while (SendTime > now) {

      uint delay_ms = pts_to_ms(SendTime - now);
      if (delay_ms < SCHEDULER_MIN_DELAY_MS)
        break;

      LOGSCR("cUdpScheduler sleeping %d ms (time ref: %s, beat interval %d ms)",
             delay_ms, ScrSourceStr[ScrSource], pts_to_ms(elapsed));

      if (delay_ms > SCHEDULER_MAX_DELAY_MS)
        delay_ms = SCHEDULER_MAX_DELAY_MS;

      Scheduler_Sleep(delay_ms);

      now = m_MasterClock.Now();
    }
  }
}

void cUdpScheduler::Action(void)
{
  /* UDP Scheduler needs high priority */
  const int priority = -5;
  if (have_cap_sys_nice())
    SetPriority(priority);
  errno = 0;
  if ((nice(priority) == -1) && errno)
    LOGDBG("cUdpScheduler: Can't nice to value: %d", priority);

  m_Lock.Lock();

  while (Running()) {

    if(m_Handles[0] < 0) {
      m_Cond.TimedWait(m_Lock, 5000); 
      continue;
    }

    // Wait until we have outgoing data in queue
    if(m_QueuePending <= 0) {
      m_BurstFrames = m_BurstBytes = 0;
      m_Cond.TimedWait(m_Lock, 100); 
      if(m_QueuePending <= 0) {
	// Still nothing...
	// Send padding frame once in 100ms so clients can detect 
	// possible missing frames and server shutdown
        QueuePaddingInternal();
      }
      continue; // to check Running()
    }

    // Take next frame from queue
    stream_rtp_header_impl_t *frame = m_BackLog->Get(m_QueueNextSeq);
    int PayloadSize  = m_BackLog->PayloadSize(m_QueueNextSeq);
    int UdpPacketLen = PayloadSize + sizeof(stream_udp_header_t);
    int RtpPacketLen = PayloadSize + sizeof(stream_rtp_header_impl_t);

    m_QueueNextSeq = (m_QueueNextSeq + 1) & UDP_BUFFER_MASK;
    m_QueuePending--;

    m_Cond.Broadcast();
    m_BackLogDeleteMutex.Lock(); /* ensure frame will not be deleted from queue */
    m_Lock.Unlock();

    // Schedule frame
    if(m_Master && eStreamId(frame->hdr_ext.stream) == sidVdr)
      Schedule(frame->payload, PayloadSize);

    // Need some bandwidth limit for ex. sequence of still frames when
    // moving cutting marks very fast (no audio or PTS available).
    // Hard limit for used bandwidth:
    //   - ~1 frames/ms & 8kb/ms -> 8mb/s -> ~ 80 Mbit/s ( / client)
    //   - max burst 15 frames or 32 kb
    m_BurstFrames ++;
    m_BurstBytes += PayloadSize;
    if (m_BurstFrames >= MAX_BURST_FRAMES && m_BurstBytes >= MAX_BURST_BYTES) {
      Scheduler_Sleep(4);
#ifdef LOG_UDP_RATE
      static uint64_t dbg_timer = cTimeMs::Now();
      static int      dbg_bytes = 0;
      dbg_bytes += bytes;
      if (dbg_timer + 60000 <= cTimeMs::Now()) {
	LOGDBG("UDP rate: %4d Kbps (queue %d)", dbg_bytes/(60*1024/8), m_QueuePending);
	dbg_bytes = 0;
	dbg_timer = cTimeMs::Now();
      }
#endif
    }

    /* tag frame with ssrc and timestamp */
    frame->rtp_hdr.ts   = htonl((uint32_t)(m_RtpScr.Now() & 0xffffffff));
    frame->rtp_hdr.ssrc = htonl(m_ssrc);

    /* deliver to all active sockets */
    for(int i=0; i<MAX_UDP_HANDLES && m_Handles[i]>=0; i++) {

      //
      // use TIOCOUTQ ioctl instead of poll/select.
      // - poll/select for UDP/RTP may return true even when queue 
      //   is (almost) full
      // - kernel silently drops frames it cant send
      // -> poll() + send() just causes frames to be dropped
      //
      uint size = 0;
      if(!ioctl(m_Handles[i], TIOCOUTQ, &size)) {
	if(size >= (m_wmem[i] - 2*RtpPacketLen)) {
	  LOGMSG("cUdpScheduler: kernel transmit queue > ~%dkb (max %dkb) ! (master=%d)", 
		 (m_wmem[i] - 2*RtpPacketLen)/1024, m_wmem[i]/1024, m_Master);
	  Scheduler_Sleep(2);
	}
      }	else {
	if(m_QueuePending > (MAX_QUEUE_SIZE-5))
	  LOGDBG("cUdpScheduler: kernel transmit queue > ~30kb ! (master=%d ; Queue=%d)", 
		 m_Master, m_QueuePending);
	Scheduler_Sleep(2);
      }

      if(m_Handles[i] == m_fd_rtp.handle()) {
	if(send(m_Handles[i], frame, RtpPacketLen, 0) <= 0)
	  LOGERR("cUdpScheduler: UDP/RTP send() failed !");
      } else {
	/* UDP: send without rtp header */
	if (send(m_Handles[i], RTP_UDP_PAYLOAD(frame), UdpPacketLen, 0) <= 0)
	  LOGERR("cUdpScheduler: UDP send() failed !");
      }
    }

    m_BackLogDeleteMutex.Unlock(); /* release queue */
    m_Lock.Lock();

    m_Frames ++;
    m_Octets += PayloadSize;
    if(m_fd_rtcp.open() && (m_Frames & 0xff) == 1) { // every 256th frame
      Send_RTCP();
#if 0
      if((m_Frames & 0xff00) == 0) // every 65536th frame (~ 2 min)
	Send_SAP();
#else
      if((m_Frames & 0x0300) == 0) // every 1024th frame (~ 2...4 sec)
	Send_SAP();
#endif
    }
  }
  
  m_Lock.Unlock();
}

void cUdpScheduler::ReSend(int fd, uint64_t Pos, int Seq1, int Seq2) 
{
  if(fd < 0) /* no re-send for RTP */
    return;

  struct {
    stream_udp_header_t hdr;
    char                payload[64-sizeof(stream_udp_header_t)];
  } udp_ctrl = {{(uint64_t)INT64_C(-1), (uint16_t)-1, 0, {}}, {0}};

  // Handle buffer wrap
  if(Seq1 > Seq2)
    Seq2 += UDP_BUFFER_SIZE;

  cMutexLock ml(&m_Lock); // keeps also scheduler thread suspended ...

  if(Seq2-Seq1 > 64) {
    LOGDBG("cUdpScheduler::ReSend: requested range too large (%d-%d)", Seq1, Seq2);
    snprintf(udp_ctrl.payload, sizeof(udp_ctrl.payload),
             "UDP MISSING %d-%d %" PRIu64,
             Seq1, (Seq2 & UDP_BUFFER_MASK), Pos);
    send(fd, &udp_ctrl, sizeof(udp_ctrl), 0);
    return;
  }

  // re-send whole range
  for(; Seq1 <= Seq2; Seq1++) {

    // Wait if kernel queue is full
    int size = 0;
    if(!ioctl(fd, TIOCOUTQ, &size))
      if(size > ((0x10000)/2 - 2048)) { // assume 64k kernel buffer
	LOGDBG("cUdpScheduler::ReSend: kernel transmit queue > ~30kb !");
	Scheduler_Sleep(2);
      }

    stream_rtp_header_impl_t *frame = m_BackLog->Get(Seq1);

    if(frame) {
      if(ntohull(frame->hdr_ext.pos) - Pos < 100000) {
	send(fd,
	     RTP_UDP_PAYLOAD(frame),
	     m_BackLog->PayloadSize(Seq1) + sizeof(stream_udp_header_t),
	     0);
	LOGRESEND("cUdpScheduler::ReSend: %d (%d bytes) @%lld sent",
		  Seq1, m_BackLog->PayloadSize(Seq1), Pos);
	Pos = ntohull(frame->hdr_ext.pos) + m_BackLog->PayloadSize(Seq1);
	continue;
      } else {
	// buffer has been lost long time ago...
	LOGRESEND("cUdpScheduler::ReSend: Requested position does not match "
	       "(%lld ; has %lld)", Pos, ntohll(frame->hdr_ext.pos));
      }
    } else {
      LOGRESEND("cUdpScheduler::ReSend: %d @%lld missing", Seq1, Pos);
    }

    // buffer has been lost - send packet missing info

    LOGRESEND("cUdpScheduler::ReSend: missing %d-%d @%d (hdr 0x%llx 0x%x)",
              Seq1, Seq1, Pos, udp_ctrl.hdr.pos, udp_ctrl.hdr.seq);

    int Seq0 = Seq1;
    for(; Seq1 < Seq2; Seq1++) {
      stream_rtp_header_impl_t *frame = m_BackLog->Get(Seq1+1);
      if(frame && (ntohull(frame->hdr_ext.pos) - Pos < 100000))
	break;
    }

    snprintf(udp_ctrl.payload, sizeof(udp_ctrl.payload),
             "UDP MISSING %d-%d %" PRIu64,
             Seq0, (Seq1 & UDP_BUFFER_MASK), Pos);

    send(fd, &udp_ctrl, sizeof(udp_ctrl), 0);
  }
}
