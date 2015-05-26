/*
 * xine_input_vdr_net.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_input_vdr_net.h,v 1.16 2010-07-19 13:39:24 phintuka Exp $
 *
 */

#ifndef __XINE_INPUT_VDR_NET_H_
#define __XINE_INPUT_VDR_NET_H_

#include <arpa/inet.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
# include <machine/endian.h>
#else
# include <endian.h>
#endif

#ifndef PACKED
#  define PACKED  __attribute__((packed))
#endif

#include "tools/rtp.h" /* generic RTP headers */


/*
 * Default port(s)
 */

#ifndef DEFAULT_VDR_PORT
#  define DEFAULT_VDR_PORT 37890
#endif

/*
 * Byte-order conversions
 */

#if __BYTE_ORDER == __BIG_ENDIAN
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#else
#  error __BYTE_ORDER not defined !
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#  define ntohll(val)  (val)
#  define htonll(val)  (val)
#  define ntohull(val) (val)
#  define htonull(val) (val)
#else
#  define ntohll(val) ((int64_t)ntohull((uint64_t)val))
#  define htonll(val) ((int64_t)htonull((uint64_t)val))
#  define ntohull(val) \
          ((uint64_t) ntohl((uint32_t)((val) >> 32)) |  \
           (uint64_t) ntohl((uint32_t)(val)) << 32)
#  define htonull(val) \
          ((uint64_t) htonl((uint32_t)((val) >> 32)) |  \
           (uint64_t) htonl((uint32_t)(val)) << 32)
#endif


/*
 * Substreams
 */

enum eStreamId {
  sidVdr      = 0,    /* VDR primary video/audio (MPEG-PES or MPEG-TS) */

  sidPipFirst = 1,    /* VDR PIP video, first (MPEG-TS PAT+PMT+video) */
  sidPipLast  = 17,

  sidPadding  = 0xfd, /* UDP/RTP padding */
  sidOsd      = 0xfe, /* OSD */
  sidControl  = 0xff, /* control messages */
};

/*
 * Network packet headers
 */

#if defined __cplusplus
extern "C" {
#endif

/*
 * TCP / PIPE
 */

typedef struct {
  uint64_t pos;  /* stream position of first byte */
  uint32_t len;  /* length of following PES packet */
  uint8_t  stream;

  uint8_t  payload[0];

} PACKED stream_tcp_header_t;

#define TCP_PAYLOAD(pkt) ((uint8_t*)(pkt)+sizeof(stream_tcp_header_t))


/*
 * UDP
 */

typedef struct {
  uint64_t pos; /* stream position of first byte */
                /* -1ULL and first bytes of frame != 00 00 01 */
                /* --> embedded control stream data */
  uint16_t seq; /* packet sequence number
                   (for re-ordering and detecting missing packets) */
  uint8_t  stream;

  uint8_t  payload[0];

} PACKED stream_udp_header_t;

#define UDP_SEQ_MASK 0xff

#define UDP_PAYLOAD(pkt) ((uint8_t*)(pkt)+sizeof(stream_udp_header_t))


/*
 * RTP
 */

/* xineliboutput RTP header extension */
typedef struct {

  stream_rtp_header_ext_t hdr;

  union {
    uint8_t  raw[12]; /* 3 DWORDs */
    uint32_t rawd[3];

    union {

      struct {
        uint8_t             padding0; /* must be padded to full DWORDs */
        stream_udp_header_t udphdr;
      } PACKED;

      struct {
        uint8_t  padding1;  /* must be padded to full DWORDs */

        uint64_t pos;
        uint16_t seq;
        uint8_t  stream;
      } PACKED;

    } PACKED;
  } PACKED;

  uint8_t payload[0];

} PACKED stream_rtp_header_ext_x_t;


/* xineliboutput RTP header */
typedef struct stream_rtp_header_impl {

  stream_rtp_header_t       rtp_hdr;
  stream_rtp_header_ext_x_t hdr_ext;

  uint8_t payload[0];

} PACKED stream_rtp_header_impl_t;


#define RTP_VERSION           2
#define RTP_MARKER_BIT        0x80
#define RTP_HDREXT_BIT        0x10
#define RTP_PAYLOAD_TYPE_PES  96     /* application */
#define RTP_PAYLOAD_TYPE_TS   33     /* MPEG-TS */

#define RTP_VERSION_BYTE        (RTP_VERSION<<6)
#define RTP_PAYLOAD_TYPE_PES_M  (RTP_PAYLOAD_TYPE_PES|RTP_MARKER_BIT)
#define RTP_PAYLOAD_TYPE_TS_M   (RTP_PAYLOAD_TYPE_TS |RTP_MARKER_BIT)

#define RTP_HEADER_EXT_X_SIZE  3      /* dwords, not counting stream_rtp_header_ext_t */
#define RTP_HEADER_EXT_X_TYPE  0x54d3

#define RTP_PAYLOAD(pkt)     ((uint8_t*)(pkt)+sizeof(stream_rtp_header_impl_t))
/* access UDP header inside RTP header extension */
#define RTP_UDP_PAYLOAD(pkt) (RTP_PAYLOAD(pkt)-sizeof(stream_udp_header_t))

#if defined __cplusplus
}
#endif


#endif /*__XINE_INPUT_VDR_NET_H_*/

