/*
 * udp_buffer.h: Ring buffer for UDP/RTP streams
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: udp_buffer.h,v 1.8 2010-06-04 10:40:21 rofafor Exp $
 *
 */

#ifndef __UDP_BUFFER_H
#define __UDP_BUFFER_H

#include <stdint.h>

#include <vdr/config.h>  // VDRVERSNUM

#include "../xine_input_vdr_net.h" // frame headers
#include "ts.h"

#define UDP_BUFFER_SIZE 0x100   // 2^n
#define UDP_BUFFER_MASK 0xff    // 2^n - 1

#if UDP_BUFFER_MASK != UDP_SEQ_MASK
#  error Buffer handling error !!! 
#endif


class cUdpBackLog 
{
    friend class cUdpScheduler;

  private:

    cUdpBackLog(cUdpBackLog&);

    stream_rtp_header_impl_t *m_UdpBuffer[UDP_BUFFER_SIZE];
    int m_UdpBufLen[UDP_BUFFER_SIZE];   /* size of allocated memory, not frame */
    int m_PayloadSize[UDP_BUFFER_SIZE]; /* size of frame */
    unsigned int m_SeqNo; /* next (outgoing) sequence number */
    unsigned int m_RtpSeqNo; /* next (outgoing) RTP sequence number */

  protected:

    cUdpBackLog()
    {
      memset(m_UdpBuffer, 0, sizeof(stream_rtp_header_impl_t *)*UDP_BUFFER_SIZE);
      memset(m_UdpBufLen, 0, sizeof(int) * UDP_BUFFER_SIZE);
      memset(m_PayloadSize, 0, sizeof(int) * UDP_BUFFER_SIZE); 
      m_SeqNo = 0;
      m_RtpSeqNo = random();
    }

    void Clear(int HowManyFrames)
    {
      // Clear n last frames from buffer.
      // (called to adjust sequence numbering when some
      // already allocated frames won't be sent)
      //
      // Note: Nothing is freed.
      //       To completely reset buffer it must be deleted and re-created.
      //
      m_SeqNo = (m_SeqNo + UDP_BUFFER_SIZE - HowManyFrames) & UDP_BUFFER_MASK;
    }

    virtual ~cUdpBackLog()
    {
      for(int i=0; i<UDP_BUFFER_SIZE; i++)
	if(m_UdpBuffer[i]) {
	  //m_UdpBufLen[i] = 0;
          delete[] m_UdpBuffer[i];
          m_UdpBuffer[i] = NULL;
	}
    }

    stream_rtp_header_impl_t *Get(int UdpSeqNo)
    {
      int BufIndex = UdpSeqNo & UDP_BUFFER_MASK;
      return m_UdpBuffer[BufIndex];
    }

    int PayloadSize(int UdpSeqNo)
    {
      int BufIndex = UdpSeqNo & UDP_BUFFER_MASK;
      return m_UdpBuffer[BufIndex] ? m_PayloadSize[BufIndex] : 0;
    }

    stream_rtp_header_impl_t *MakeFrame(eStreamId StreamId, uint64_t StreamPos,
                                        const uchar *Data, int DataLen)
    {
      int UdpPacketLen = DataLen + sizeof(stream_rtp_header_impl_t);
      int BufIndex = m_SeqNo & UDP_BUFFER_MASK;

      // old buffer too small ? free it
      if(m_UdpBuffer[BufIndex] && m_UdpBufLen[BufIndex] < UdpPacketLen) {
        delete[] m_UdpBuffer[BufIndex];
        m_UdpBuffer[BufIndex] = NULL;
      }

      // no buffer ? alloc it
      if(!m_UdpBuffer[BufIndex]) {
	m_UdpBuffer[BufIndex] = (stream_rtp_header_impl_t*)new uchar[UdpPacketLen];
	m_UdpBufLen[BufIndex] = UdpPacketLen;
      }
      m_PayloadSize[BufIndex] = DataLen;

      // Fill frame to buffer
      stream_rtp_header_impl_t *header = m_UdpBuffer[BufIndex];

      memcpy(header->payload, Data, DataLen);

      // RTP header
      header->rtp_hdr.raw[0] = RTP_VERSION_BYTE | RTP_HDREXT_BIT;
#if VDRVERSNUM >= 10701 || defined(TSPLAY_PATCH_VERSION)
      if (DATA_IS_TS(Data))
        header->rtp_hdr.raw[1] = RTP_PAYLOAD_TYPE_TS;
      else
        header->rtp_hdr.raw[1] = RTP_PAYLOAD_TYPE_PES;
#else
      header->rtp_hdr.raw[1] = RTP_PAYLOAD_TYPE_PES;
#endif
      header->rtp_hdr.seq  = htons(m_RtpSeqNo & 0xFFFF);
      /*header->rtp_hdr.ts   = htonl((uint32_t)(RtpScr.Now() & 0xffffffff));*/
      /*header->rtp_hdr.ssrc = htonl(m_ssrc);*/

      // RTP header extension
      header->hdr_ext.hdr.size = htons(RTP_HEADER_EXT_X_SIZE);
      header->hdr_ext.hdr.type = htons(RTP_HEADER_EXT_X_TYPE);

      // UDP header
      header->hdr_ext.pos = htonull(StreamPos);
      header->hdr_ext.seq = htons(m_SeqNo);
      header->hdr_ext.stream = (uint8_t)StreamId;

      header->hdr_ext.padding1 = 0;

      m_RtpSeqNo = (m_RtpSeqNo + 1) & 0xFFFF;
      m_SeqNo    = (m_SeqNo    + 1) & UDP_SEQ_MASK;

      return header;
    }
};


#endif
