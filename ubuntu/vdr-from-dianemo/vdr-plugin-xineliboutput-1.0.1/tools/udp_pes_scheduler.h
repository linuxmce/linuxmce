/*
 * udp_pes_scheduler.h: PES scheduler for UDP/RTP streams
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: udp_pes_scheduler.h,v 1.13 2008/04/28 20:53:07 phintuka Exp $
 *
 */

#ifndef __UDP_PES_SCHEDULER_H
#define __UDP_PES_SCHEDULER_H

#include <stdint.h>

#include <vdr/tools.h>  // uchar
#include <vdr/thread.h>

#include "cxsocket.h"
#include "time_pts.h"

#define MAX_UDP_HANDLES 16

class cUdpBackLog;

class cUdpScheduler : public cThread
{
  public:

    cUdpScheduler();
    virtual ~cUdpScheduler();

    // fd should be binded & connected to IP:PORT (local+remote) pair !
    bool AddHandle(int fd);     /* UDP unicast */
    void RemoveHandle(int fd);  /* UDP unicast */ 
    bool AddRtp(void);          /* UDP/RTP multicast */
    void RemoveRtp(void);       /* UDP/RTP multicast */
    bool AddHandle(cxSocket& s) { return AddHandle(s.handle()); }
    void RemoveHandle(cxSocket& s) { RemoveHandle(s.handle()); }

    bool Clients(void) { return m_Handles[0] >= 0; }
    int  Poll(int TimeoutMs, bool Master);
    bool Queue(uint64_t StreamPos, const uchar *Data, int Length);
    void ReSend(int fd, uint64_t Pos, int Seq1, int Seq2);

    void Clear(void);
    bool Flush(int TimeoutMs);

    void Pause(bool On);
    void TrickSpeed(const int Multiplier);
    void SetScrSpeed(const int Speed = 90000);

  protected:

    // Data for payload handling & buffering

    // Signalling
    cCondVar  m_Cond;
    cMutex    m_Lock;

    // Clients
    int       m_Handles[MAX_UDP_HANDLES];
    int       m_wmem[MAX_UDP_HANDLES];  /* kernel buffer size */

    cxSocket  m_fd_rtp;
    cxSocket  m_fd_rtcp;

    // Queue
    int m_QueueNextSeq;      /* next outgoing */
    int m_QueuePending;      /* outgoing queue size */
    cUdpBackLog *m_BackLog;  /* queue for incoming data (not yet send) and retransmissions */
    cMutex m_BackLogDeleteMutex;

    // Data for scheduling algorithm
    cTimePts  MasterClock;   /* Current MPEG PTS (synchronized to current stream) */
    cCondWait CondWait;

    int64_t  current_audio_vtime;
    int64_t  current_video_vtime;

    // RTP
    uint32_t  m_ssrc;   /* RTP synchronization source id */
    cTimePts  RtpScr;   /* 90 kHz monotonic time source for RTP timestamps */

    // RTCP
    uint64_t  m_LastRtcpTime;
    uint32_t  m_Frames;
    uint32_t  m_Octets;

    // Scheduling 

    bool m_TrickSpeed;
    bool m_Master;     /* if true, we are master metronom for playback */
  
    int  calc_elapsed_vtime(int64_t pts, bool Audio);
    void Schedule(const uchar *Data, int Length);

    bool m_Running;
    virtual void Action(void);

    void Send_RTCP(void);

    int  m_fd_sap;

    void Send_SAP(bool Announce = true);
};

#endif
