/*
 * udp_pes_scheduler.h: PES scheduler for UDP/RTP streams
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: udp_pes_scheduler.h,v 1.23 2010-03-14 11:50:50 phintuka Exp $
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
    bool Queue(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int Length);
    void QueuePadding(void);
    void ReSend(int fd, uint64_t Pos, int Seq1, int Seq2);

    void Clear(void);
    bool Flush(int TimeoutMs);

    void Pause(bool On);
    void TrickSpeed(const int Multiplier);
    void SetScrSpeed(const int Speed = 90000);

  protected:

    // Signalling

    cCondVar  m_Cond;
    cMutex    m_Lock;

    // Clients

    int       m_Handles[MAX_UDP_HANDLES];
    uint      m_wmem[MAX_UDP_HANDLES];  /* kernel buffer size */

    cxSocket  m_fd_rtp;
    cxSocket  m_fd_rtcp;

    // Queue

    uint         m_QueueNextSeq;  /* next outgoing */
    uint         m_QueuePending;  /* outgoing queue size */
    cUdpBackLog *m_BackLog;       /* queue for incoming data (not yet send) and retransmissions */
    cMutex       m_BackLogDeleteMutex;

    // Scheduling

    typedef enum {
      /* from worst to best */
      eScrDetect     = 0,
      eScrFromVideo  = 1,
      eScrFromPS1    = 2,
      eScrFromAudio  = 3,
      eScrFromPcr    = 4,
    } ScrSource_t;

    cTimePts     m_MasterClock;   /* Current MPEG PTS (synchronized to current stream) */
    bool         m_TrickSpeed;    /* current (replay) speed */
    bool         m_Master;        /* if true, we are master metronom for playback */

    ScrSource_t  m_ScrSource;
    int64_t      m_CurrentAudioVtime;
    int64_t      m_CurrentVideoVtime;
    int64_t      m_CurrentPcr;

    uint         m_BurstBytes;   /* number of bytes sent without sleeps */
    uint         m_BurstFrames;  /* number of frames sent without sleeps */

    cCondWait    m_CondWait;

    int          CalcElapsedVtime(int64_t pts, ScrSource_t ScrSource);
    void         Schedule(const uchar *Data, int Length);
    void         Scheduler_Sleep(int ms);
    void         QueuePaddingInternal(void);

    // RTP

    uint32_t     m_ssrc;   /* RTP synchronization source id */
    cTimePts     m_RtpScr; /* 90 kHz monotonic time source for RTP timestamps */

    // RTCP

    uint64_t     m_LastRtcpTime;
    uint32_t     m_Frames;
    uint32_t     m_Octets;

    void         Send_RTCP(void);

    // SAP

    int          m_fd_sap;

    void         Send_SAP(bool Announce = true);

    // Thread

    virtual void Action(void);
};

#endif
