/*
 * backgroundwriter.h: Buffered socket/file writing thread
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: backgroundwriter.c,v 1.6 2008/02/04 23:54:06 phintuka Exp $
 *
 */

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS
#include <inttypes.h>

#include <stdint.h>
#include <unistd.h>
#include <netinet/tcp.h> // CORK, NODELAY

#include <vdr/tools.h>

#include "../logdefs.h"
#include "../xine_input_vdr_net.h" // stream_tcp_header_t

#include "backgroundwriter.h"

//#define DISABLE_DISCARD
//#define LOG_DISCARDS

#define MAX_OVERFLOWS_BEFORE_DISCONNECT 1000 // ~ 1 second


//
// cBackgroundWriterI
//

cBackgroundWriterI::cBackgroundWriterI(int fd, int Size, int Margin) 
  : m_RingBuffer(Size, Margin)
{ 
  m_fd = fd;
  m_RingBuffer.SetTimeouts(0, 100);
  m_Active = true;

  m_PutPos = 0;
  m_DiscardStart = 0;
  m_DiscardEnd   = 0;

  m_BufferOverflows = 0;

#if defined(TCP_CORK)
  int iCork = 1;
  if(setsockopt(m_fd, IPPROTO_TCP, TCP_CORK, &iCork, sizeof(int))) {
    if(errno != ENOTSOCK)
      LOGERR("cBackgroundWriter: setsockopt(TCP_CORK) failed");
    m_IsSocket = false;
    errno = 0;
  } else {
    m_IsSocket = true;
  }
#elif defined(TCP_NOPUSH)
  int iCork = 1;
  if(setsockopt(m_fd, IPPROTO_TCP, TCP_NOPUSH, &iCork, sizeof(int))) {
    if(errno != ENOTSOCK)
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
  m_Active = false;
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

bool cBackgroundWriterI::Flush(int TimeoutMs)
{
  uint64_t WaitEnd = cTimeMs::Now();

  // wait for ring buffer to drain
  if(TimeoutMs > 0) {
    WaitEnd += (uint64_t)TimeoutMs;

    while(cTimeMs::Now() < WaitEnd &&
	  m_Active &&
	  m_RingBuffer.Available() > 0)
      cCondWait::SleepMs(3);
  }

  if(m_IsSocket && m_RingBuffer.Available() <= 0) {
    // flush corked data too
#if defined(TCP_CORK)
    int i = 1;
    if(setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(int))) {
      LOGERR("cBackgroundWriter: setsockopt(TCP_NODELAY) failed");
      errno = 0;
    }
#elif defined(TCP_NOPUSH)
    int On = 1, Off = 0;
    if(setsockopt(m_fd, IPPROTO_TCP, TCP_NOPUSH, &Off, sizeof(int)) ||
       setsockopt(m_fd, IPPROTO_TCP, TCP_NOPUSH, &On, sizeof(int))) {
      LOGERR("cBackgroundWriter: setsockopt(TCP_NOPUSH) failed");
      errno = 0;
    }
#endif
  }
  
  return m_RingBuffer.Available() <= 0;
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
  uint64_t NextHeaderPos = 0ULL;
  uint64_t GetPos = 0ULL;
  cPoller Poller(m_fd, true);

  while(m_Active) {

    if(Poller.Poll(100)) {

      int Count = 0, n;
      uchar *Data = m_RingBuffer.Get(Count);

      if(Data && Count > 0) {

#ifndef DISABLE_DISCARD
	Lock(); // uint64_t m_DiscardStart can not be read atomically (IA32)
	if(m_DiscardEnd > GetPos) {

# ifdef LOG_DISCARDS
	  LOGMSG("TCP: queue: discard request: queue %d bytes, "
		 "next point %d bytes forward (Count=%d)",
		 m_RingBuffer.Available(),
		 NextHeaderPos - GetPos, 
		 Count);
# endif
	  if(NextHeaderPos == GetPos) {
	    // we're at frame boundary
# ifdef LOG_DISCARDS
	    uint8_t *pkt = TCP_PAYLOAD(Data);
	    if(pkt[0] || pkt[1] || pkt[2] != 1 || hdr->len > 2100) {
	      LOGMSG("  -> %x %x %x %x", pkt[0], pkt[1], pkt[2], pkt[3]);
	    }      
# endif
	    Count = min(Count, (int)(m_DiscardEnd - GetPos));
# ifdef LOG_DISCARDS
	    LOGMSG("Flushing %d bytes", Count);
#endif
	    Unlock();

	    m_RingBuffer.Del(Count);
	    GetPos += Count;
	    NextHeaderPos = GetPos;
# ifdef LOG_DISCARDS
	    LOGMSG("Queue now %d bytes", m_RingBuffer.Available());
	    pkt = TCP_PAYLOAD(Data);
	    if(pkt[0] || pkt[1] || pkt[2] != 1 || hdr->len > 2100) {
	      LOGMSG("  -> %x %x %x %x", pkt[0], pkt[1], pkt[2], pkt[3]);
# endif
	    continue;
	  }
	}
	Unlock();
#endif

#ifndef DISABLE_DISCARD
	if(GetPos == NextHeaderPos) {
	  if(Count < (int)sizeof(stream_tcp_header_t))
	    LOGMSG("cBackgroundWriter @NextHeaderPos: Count < header size !");

	  stream_tcp_header_t *header = (stream_tcp_header_t*)Data;
	  if(Count < (int)(ntohl(header->len) + sizeof(stream_tcp_header_t)))
	    ;//LOGMSG("Count = %d < %d", Count, 
	     //   header->len + sizeof(stream_tcp_header_t));
	  else
	    Count = ntohl(header->len) + sizeof(stream_tcp_header_t);
	  NextHeaderPos = GetPos + ntohl(header->len) + sizeof(stream_tcp_header_t);
	} else {
	  Count = min(Count, (int)(NextHeaderPos-GetPos));
	}
#endif

	errno = 0;
	n = write(m_fd, Data, Count);

	if(n == 0) {
	  LOGERR("cBackgroundWriter: Client disconnected data stream ?");
	  break;
	  
	} else if(n < 0) {
	  
	  if (errno == EINTR || errno == EWOULDBLOCK) {
	    TRACE("cBackgroundWriter: EINTR while writing to file handle "
		  <<m_fd<<" - retrying");
	    continue;

	  } else {
	    LOGERR("cBackgroundWriter: TCP write error");
	    break;
	  }
	}

	GetPos += n;
	m_RingBuffer.Del(n);
      }
    }
  }

  m_RingBuffer.Clear();
  m_Active = false;
}

int cTcpWriter::Put(uint64_t StreamPos, 
		    const uchar *Data, int DataCount)
{
  stream_tcp_header_t header;
  header.pos = htonull(StreamPos);
  header.len = htonl(DataCount);
  return Put((uchar*)&header, sizeof(header), Data, DataCount);
}

int cTcpWriter::Put(const uchar *Header, int HeaderCount, 
		    const uchar *Data, int DataCount) 
{
  if(m_Active) {

    // Serialize Put access to keep Data and Header together
    LOCK_THREAD;  
    
    if(m_RingBuffer.Free() < HeaderCount+DataCount) {
      //LOGMSG("cXinelibServer: TCP buffer overflow !");
      if(m_BufferOverflows++ > MAX_OVERFLOWS_BEFORE_DISCONNECT) {
	LOGMSG("cXinelibServer: Too many TCP buffer overflows, dropping client");
	m_RingBuffer.Clear();
	m_Active = false;
	return 0;
      }
      return -HeaderCount-DataCount;
    }
    int n = m_RingBuffer.Put(Header, HeaderCount) +
            m_RingBuffer.Put(Data, DataCount);
    if(n == HeaderCount+DataCount) {
      m_BufferOverflows = 0;
      m_PutPos += n;
      return n;
    }
    
    LOGMSG("cXinelibServer: TCP buffer internal error ?!?");
    m_RingBuffer.Clear();
    m_Active = false;
  }

  return 0;
}


//
// cRawWriter
//

#include "pes.h"

cRawWriter::cRawWriter(int fd, int Size) :
     cBackgroundWriterI(fd, Size, 6)
{ 
  LOGDBG("cRawWriter initialized (buffer %d kb)", Size/1024);
  Start();
}

void cRawWriter::Action(void)
{
  uint64_t NextHeaderPos = 0ULL;
  uint64_t GetPos = 0ULL;
  cPoller Poller(m_fd, true);

  while(m_Active) {

    if(Poller.Poll(100)) {

      int Count = 0, n;
      uchar *Data = m_RingBuffer.Get(Count);

      if(Data && Count > 0) {

#ifndef DISABLE_DISCARD
	Lock(); // uint64_t m_DiscardStart can not be read atomically (IA32)
	if(m_DiscardEnd > GetPos) {

	  if(NextHeaderPos == GetPos) {
	    // we're at frame boundary
	    Count = min(Count, (int)(m_DiscardEnd - GetPos));
	    Unlock();

	    m_RingBuffer.Del(Count);
	    GetPos += Count;
	    NextHeaderPos = GetPos;
	    continue;
	  }
	}
	Unlock();
#endif

#ifndef DISABLE_DISCARD
	if(GetPos == NextHeaderPos) {
	  if(Count < 6)
	    LOGMSG("cBackgroundWriter @NextHeaderPos: Count < header size !");
	  int packlen = pes_packet_len(Data, Count);
	  if(Count < packlen)
	    ;//LOGMSG("Count = %d < %d", Count, 
	     //   header->len + sizeof(stream_tcp_header_t));
	  else
	    Count = packlen;
	  NextHeaderPos = GetPos + packlen;
	} else {
	  Count = min(Count, (int)(NextHeaderPos-GetPos));
	}
#endif

	errno = 0;
	n = write(m_fd, Data, Count);

	if(n == 0) {
	  LOGERR("cBackgroundWriter: Client disconnected data stream ?");
	  break;
	  
	} else if(n < 0) {
	  
	  if (errno == EINTR || errno == EWOULDBLOCK) {
	    TRACE("cBackgroundWriter: EINTR while writing to file handle "
		  <<m_fd<<" - retrying");
	    continue;

	  } else {
	    LOGERR("cBackgroundWriter: TCP write error");
	    break;
	  }
	}

	GetPos += n;
	m_RingBuffer.Del(n);
      }
    }
  }

  m_RingBuffer.Clear();
  m_Active = false;
}

int cRawWriter::Put(uint64_t StreamPos, 
		    const uchar *Data, int DataCount)
{
  if(m_Active) {

    // Serialize Put access to keep Data and Header together
    LOCK_THREAD;  
    
    if(m_RingBuffer.Free() < DataCount) {
      if(m_BufferOverflows++ > MAX_OVERFLOWS_BEFORE_DISCONNECT) {
	LOGMSG("cXinelibServer: Too many TCP buffer overflows, dropping client");
	m_RingBuffer.Clear();
	m_Active = false;
	return 0;
      }
      return -DataCount;
    }
    int n = m_RingBuffer.Put(Data, DataCount);
    if(n == DataCount) {
      m_BufferOverflows = 0;
      m_PutPos += n;
      return n;
    }
    
    LOGMSG("cXinelibServer: TCP buffer internal error ?!?");
    m_RingBuffer.Clear();
    m_Active = false;
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

int cTsWriter::Put(uint64_t StreamPos, const uchar *Data, int DataCount)
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

int cRtspMuxWriter::Put(uint64_t StreamPos, const uchar *Data, int DataCount)
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

int cRtspRemuxWriter::Put(uint64_t StreamPos, const uchar *Data, int DataCount)
{
  return 0;
}


