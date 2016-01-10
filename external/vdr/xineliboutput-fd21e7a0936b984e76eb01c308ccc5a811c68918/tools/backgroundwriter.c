/*
 * backgroundwriter.h: Buffered socket/file writing thread
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: backgroundwriter.c,v 1.23 2010-07-19 13:20:21 phintuka Exp $
 *
 */

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS
#include <inttypes.h>

#include <stdint.h>
#include <unistd.h>
#ifdef __FreeBSD__
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <netinet/tcp.h> // CORK, NODELAY

#include <vdr/tools.h>
#include <vdr/config.h>  // VDRVERSNUM

#include "../logdefs.h"
#include "../xine_input_vdr_net.h" // stream_tcp_header_t
#include "ts.h"
#include "pes.h"

#include "backgroundwriter.h"


#define MAX_OVERFLOWS_BEFORE_DISCONNECT 1000 // ~ 1 second


//
// cBackgroundWriterI
//

cBackgroundWriterI::cBackgroundWriterI(int fd, int Size, int Margin)
  : m_RingBuffer(Size, Margin)
{
  m_fd = fd;
  m_RingBuffer.SetTimeouts(0, 100);

  m_PutPos = 0;
  m_DiscardStart = 0;
  m_DiscardEnd   = 0;

  m_BufferOverflows = 0;

#if defined(TCP_CORK)
  int iCork = 1;
  if (setsockopt(m_fd, IPPROTO_TCP, TCP_CORK, &iCork, sizeof(int))) {
    if (errno != ENOTSOCK)
      LOGERR("cBackgroundWriter: setsockopt(TCP_CORK) failed");
    m_IsSocket = false;
    errno = 0;
  } else {
    m_IsSocket = true;
  }
#elif defined(TCP_NOPUSH)
  int iCork = 1;
  if (setsockopt(m_fd, IPPROTO_TCP, TCP_NOPUSH, &iCork, sizeof(int))) {
    if (errno != ENOTSOCK)
      LOGERR("cBackgroundWriter: setsockopt(TCP_NOPUSH) failed");
    m_IsSocket = false;
    errno = 0;
  } else {
    m_IsSocket = true;
  }
#endif

  LOGDBG("cBackgroundWriterI initialized (buffer %d kb)", Size/1024);
}

cBackgroundWriterI::~cBackgroundWriterI()
{
  Cancel(3);
}

int cBackgroundWriterI::Free(void)
{
  return m_RingBuffer.Free();
}

void cBackgroundWriterI::Clear(void)
{
  // Can't just drop buffer contents or PES frames will be broken.
  // Serialize with Put
  LOCK_THREAD;
  m_DiscardEnd = m_PutPos;
}

void cBackgroundWriterI::Cork(void)
{
  if (m_IsSocket) {
#if defined(TCP_CORK)
    int i = 1;
    if (setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(int))) {
      LOGERR("cBackgroundWriter: setsockopt(TCP_NODELAY) failed");
      errno = 0;
    }
#elif defined(TCP_NOPUSH)
    int On = 1, Off = 0;
    if (setsockopt(m_fd, IPPROTO_TCP, TCP_NOPUSH, &Off, sizeof(int)) ||
        setsockopt(m_fd, IPPROTO_TCP, TCP_NOPUSH, &On, sizeof(int))) {
      LOGERR("cBackgroundWriter: setsockopt(TCP_NOPUSH) failed");
      errno = 0;
    }
#endif
  }
}

bool cBackgroundWriterI::Flush(int TimeoutMs)
{
  uint64_t WaitEnd = cTimeMs::Now();

  // wait for ring buffer to drain
  if (TimeoutMs > 0) {
    WaitEnd += (uint64_t)TimeoutMs;

    while (cTimeMs::Now() < WaitEnd &&
           Running() &&
           m_RingBuffer.Available() > 0)
      cCondWait::SleepMs(3);
  }

  int Available = m_RingBuffer.Available();
  if (m_IsSocket && Available <= 0) {
    // flush corked data too
    Cork();
  }

  return Available <= 0;
}


//
// cTcpWriter
//

cTcpWriter::cTcpWriter(int fd, int Size) :
     cBackgroundWriterI(fd, Size, sizeof(stream_tcp_header_t))
{
  LOGDBG("cTcpWriter initialized (buffer %d kb)", Size/1024);
  Start();
}

void cTcpWriter::Action(void)
{
  uint64_t NextHeaderPos = 0;
  uint64_t GetPos        = 0;
  cPoller  Poller (m_fd, true);
  bool     CorkReq = false;

  while (Running()) {

    if (!Poller.Poll(100))
      continue;

    if (CorkReq && m_RingBuffer.Available() <= 0) {
      // Force TCP packet to avoid delaying control messages
      Cork();
      CorkReq = false;
    }

    uint64_t StartPos;
    int      Count = 0;
    int      n;
    uchar   *Data = m_RingBuffer.Get(Count);

    if (!Data || Count <= 0)
      continue;

    Lock(); // uint64_t m_DiscardStart can not be read atomically (IA32)
    StartPos = m_DiscardEnd;
    Unlock();

    // Next frame ?
    if (NextHeaderPos == GetPos) {

      // Discard data ?
      if (StartPos > GetPos) {
        // we're at frame boundary
        // drop only data packets, not control messages
        stream_tcp_header_t *header = (stream_tcp_header_t*)Data;
        if (eStreamId(header->stream) == sidVdr) {
          Count = min(Count, (int)(StartPos - GetPos));

          // size of next (complete) packet.
          // drop only one packet at time.
          int pkt_len = ntohl(header->len) + sizeof(stream_tcp_header_t);
          if (Count >= pkt_len) {
            // drop only complete packets.
            // some packets are not dropped (packets overlapping end of ringbuffer)
            Count = pkt_len;

            m_RingBuffer.Del(Count);
            GetPos += Count;
            NextHeaderPos = GetPos;

            CorkReq = true; // force sending last frame

            continue;
          }
        }
      }

      // Next frame

      if (Count < (int)sizeof(stream_tcp_header_t))
        LOGMSG("cBackgroundWriter @NextHeaderPos: Count < header size !");

      // limit single write to size of next (complete) packet.
      // (we need to track packet boundaries)
      stream_tcp_header_t *header = (stream_tcp_header_t*)Data;
      int pkt_len = ntohl(header->len) + sizeof(stream_tcp_header_t);
      if (Count > pkt_len)
        Count = pkt_len;
      // next packet start position in stream
      NextHeaderPos = GetPos + pkt_len;

      // check for control message
      if (eStreamId(header->stream) == sidControl)
        CorkReq = true;

    } else {
      // end of prev frame
      Count = min(Count, (int)(NextHeaderPos-GetPos));
    }

    errno = 0;
    n = write(m_fd, Data, Count);

    if (n <= 0) {

      if (n == 0) {
        LOGERR("cBackgroundWriter: Client disconnected data stream ?");
        break;
      }

      if (errno == EINTR || errno == EWOULDBLOCK)
        continue;

      LOGERR("cBackgroundWriter: TCP write error");
      break;
    }

    GetPos += n;
    m_RingBuffer.Del(n);
  }

  m_RingBuffer.Clear();
}

int cTcpWriter::Put(eStreamId StreamId, uint64_t StreamPos,
                    const uchar *Data, int DataCount)
{
  stream_tcp_header_t header;
  header.pos = htonull(StreamPos);
  header.len = htonl(DataCount);
  header.stream = (uint8_t)StreamId;
  return Put((uchar*)&header, sizeof(header), Data, DataCount);
}

int cTcpWriter::Put(const uchar *Header, int HeaderCount,
                    const uchar *Data, int DataCount)
{
  if (Running()) {

    // Serialize Put access to keep Data and Header together
    LOCK_THREAD;

    if (m_RingBuffer.Free() < HeaderCount+DataCount) {
      //LOGMSG("cXinelibServer: TCP buffer overflow !");
      if (m_BufferOverflows++ > MAX_OVERFLOWS_BEFORE_DISCONNECT) {
        LOGMSG("cXinelibServer: Too many TCP buffer overflows, dropping client");
        m_RingBuffer.Clear();
        Cancel(-1);
        return 0;
      }
      return -HeaderCount-DataCount;
    }
    int n = m_RingBuffer.Put(Header, HeaderCount) +
            m_RingBuffer.Put(Data, DataCount);
    if (n == HeaderCount + DataCount) {
      m_BufferOverflows = 0;
      m_PutPos += n;
      return n;
    }

    LOGMSG("cXinelibServer: TCP buffer internal error ?!?");
    m_RingBuffer.Clear();
    Cancel(-1);
  }

  return 0;
}


//
// cRawWriter
//

cRawWriter::cRawWriter(int fd, int Size) :
     cBackgroundWriterI(fd, Size, 6)
{
  LOGDBG("cRawWriter initialized (buffer %d kb)", Size/1024);
  Start();
}

void cRawWriter::Action(void)
{
  uint64_t NextHeaderPos = 0ULL;
  uint64_t GetPos        = 0ULL;
  cPoller  Poller(m_fd, true);

  while (Running()) {

    if (!Poller.Poll(100))
      continue;

    uint64_t StartPos;
    int      Count = 0;
    int      n;
    uchar   *Data = m_RingBuffer.Get(Count);

    if (!Data || Count <= 0)
      continue;

    Lock(); // uint64_t m_DiscardStart can not be read atomically (IA32)
    StartPos = m_DiscardEnd;
    Unlock();

    // Next frame ?
    if (NextHeaderPos == GetPos) {

      // Discard data ?
      if (StartPos > GetPos) {
        // we're at frame boundary
        Count = min(Count, (int)(StartPos - GetPos));

        m_RingBuffer.Del(Count);
        GetPos += Count;
        NextHeaderPos = GetPos;
        continue;
      }

      // Next frame

      if (Count < 6)
        LOGMSG("cBackgroundWriter @NextHeaderPos: Count < header size !");

      int packlen = DATA_IS_TS(Data) ? TS_SIZE : pes_packet_len(Data, Count);

      if (Count < packlen)
        ;//LOGMSG("Count = %d < %d", Count,
      //   header->len + sizeof(stream_tcp_header_t));
      else
        Count = packlen;
      NextHeaderPos = GetPos + packlen;

    } else {
      // end of prev frame
      Count = min(Count, (int)(NextHeaderPos-GetPos));
    }

    errno = 0;
    n = write(m_fd, Data, Count);

    if (n <= 0) {
      if (n == 0) {
        LOGERR("cBackgroundWriter: Client disconnected data stream ?");
        break;
      }

      if (errno == EINTR || errno == EWOULDBLOCK)
        continue;

      LOGERR("cBackgroundWriter: TCP write error");
      break;
    }

    GetPos += n;
    m_RingBuffer.Del(n);
  }

  m_RingBuffer.Clear();
}

int cRawWriter::Put(eStreamId StreamId, uint64_t StreamPos,
                    const uchar *Data, int DataCount)
{
  if (Running() && StreamId == sidVdr) {

    // Serialize Put access to keep Data and Header together
    LOCK_THREAD;

    if (m_RingBuffer.Free() < DataCount) {
      if (m_BufferOverflows++ > MAX_OVERFLOWS_BEFORE_DISCONNECT) {
        LOGMSG("cXinelibServer: Too many TCP buffer overflows, dropping client");
        m_RingBuffer.Clear();
        Cancel(-1);
        return 0;
      }
      return -DataCount;
    }
    int n = m_RingBuffer.Put(Data, DataCount);
    if (n == DataCount) {
      m_BufferOverflows = 0;
      m_PutPos += n;
      return n;
    }

    LOGMSG("cXinelibServer: TCP buffer internal error ?!?");
    m_RingBuffer.Clear();
    Cancel(-1);
  }

  return 0;
}


//
// cTsWriter
//  - Demux PES stream to PS
//

cTsWriter::cTsWriter(int fd, int Size) :
     cBackgroundWriterI(fd, Size, 6)
{
  LOGDBG("cTsWriter initialized (buffer %d kb)", Size/1024);
  Start();
}


void cTsWriter::Action(void)
{
}

int cTsWriter::Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount)
{
  return 0;
}


//
// cRtspMuxWriter
//  - RTSP multiplexed control+data
//  - Each encapsulated PES frame is written atomically to socket buffer
//  - Atomic control data can be written directly to socket
//    from another thread to bypass buffer
//

cRtspMuxWriter::cRtspMuxWriter(int fd, int Size) :
     cBackgroundWriterI(fd, Size, 6)
{
  LOGDBG("cRtspMuxWriter initialized (buffer %d kb)", Size/1024);
  Start();
}

void cRtspMuxWriter::Action(void)
{
}

int cRtspMuxWriter::Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount)
{
  return 0;
}


//
// cRtspRemuxWriter
//  - RTSP multiplexed control+data
//  - Demux PES stream to independent ES streams
//  - encapsulate ES to RTP/AVP compatible frames
//  - Mux RTP/AVP ES streams to pipelined RTCP control connection
//  - Each encapsulated frame is written atomically to socket buffer
//  - Atomic control data can be written directly to socket
//    from another thread to bypass buffer
//

cRtspRemuxWriter::cRtspRemuxWriter(int fd, int Size) :
     cBackgroundWriterI(fd, Size, 6)
{
  LOGDBG("cRtspRemuxWriter initialized (buffer %d kb)", Size/1024);
  Start();
}

void cRtspRemuxWriter::Action(void)
{
}

int cRtspRemuxWriter::Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount)
{
  return 0;
}


