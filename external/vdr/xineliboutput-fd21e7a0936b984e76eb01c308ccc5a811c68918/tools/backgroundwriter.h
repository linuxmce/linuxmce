/*
 * backgroundwriter.h: Buffered socket/file writing thread
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: backgroundwriter.h,v 1.10 2012-01-16 12:12:49 phintuka Exp $
 *
 */

#ifndef __BACKGROUNDWRITER_H
#define __BACKGROUNDWRITER_H

#include <stdint.h>

#include <vdr/thread.h>
#include <vdr/ringbuffer.h>

//
// cBackgroundWriterI
//  - generic interface for buffered output
//
class cBackgroundWriterI : public cThread 
{
  protected:
    cRingBufferLinear m_RingBuffer;

    int      m_fd;
    bool     m_IsSocket;

    uint64_t m_PutPos;
    uint64_t m_DiscardStart;
    uint64_t m_DiscardEnd;

    int      m_BufferOverflows;

  protected:
    virtual void Action(void) = 0;
    void Cork(void);

  public:
#if VDRVERSNUM >= 10708
    cBackgroundWriterI(int fd, int Size = KILOBYTE(2048), int Margin = 0);
#else
    cBackgroundWriterI(int fd, int Size = KILOBYTE(512), int Margin = 0);
#endif
    virtual ~cBackgroundWriterI();

    // Add PES frame to buffer
    //
    // Return value:
    // Success:     Count   (all bytes pushed to queue)
    // Error:       0       (write error ; socket disconnected)
    // Buffer full: -Count  (no bytes will be pushed to queue)
    //
    virtual int Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount) = 0;

    int  Free(void);           // Return largest possible Put size
    void Clear(void);          // Drop all data (only complete frames) from buffer
    bool Flush(int TimeoutMs); // Flush buffer (wait for data to be sent)
};


//
// cTcpWriter
//  - xineliboutput TCP data steam 
//  - stream_tcp_header_t encapsulated PES frames
//
class cTcpWriter : public cBackgroundWriterI 
{
  protected:
    virtual void Action(void);

    int Put(const uchar *Header, int HeaderCount, 
	    const uchar *Data,   int DataCount);

  public:
#if VDRVERSNUM >= 10708
    cTcpWriter(int fd, int Size = KILOBYTE(2048));
#else
    cTcpWriter(int fd, int Size = KILOBYTE(512));
#endif

    virtual ~cTcpWriter() {};

    virtual int Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount);
};


//
// cRawWriter
//  - Raw PES stream
//  - Used with HTTP
//
class cRawWriter : public cBackgroundWriterI 
{
  protected:
    virtual void Action(void);

  public:
    cRawWriter(int fd, int Size = KILOBYTE(512));
    virtual ~cRawWriter() {};

    virtual int Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount);
};


//
// cTsWriter
//  - Demux PES stream to PS
//
class cTsWriter : public cBackgroundWriterI 
{
  protected:
    virtual void Action(void);

  public:
    cTsWriter(int fd, int Size = KILOBYTE(512));
    virtual ~cTsWriter() {};

    virtual int Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount);
};


//
// cRtspMuxWriter
//  - RTSP multiplexed control+data
//  - Each encapsulated PES frame is written atomically to socket buffer
//  - Atomic control data can be written directly to socket 
//    from another thread to bypass buffer
//

class cRtspMuxWriter : public cBackgroundWriterI 
{
  protected:
    virtual void Action(void);

  public:
    cRtspMuxWriter(int fd, int Size = KILOBYTE(512));
    virtual ~cRtspMuxWriter() {};

    virtual int Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount);
};


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

class cRtspRemuxWriter : public cBackgroundWriterI 
{
  protected:
    virtual void Action(void);

  public:
    cRtspRemuxWriter(int fd, int Size = KILOBYTE(512));
    virtual ~cRtspRemuxWriter() {};

    virtual int Put(eStreamId StreamId, uint64_t StreamPos, const uchar *Data, int DataCount);
};


#endif
