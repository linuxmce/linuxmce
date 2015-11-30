/*
 * rtp.h: RFC1889: RTP - A Transport Protocol for Real-Time Applications
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: rtp.h,v 1.4 2010-07-19 13:20:22 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_RTP_H_
#define XINELIBOUTPUT_RTP_H_

#if defined(__APPLE__) || defined (__FreeBSD__)
# include <machine/endian.h>
#else
# include <endian.h>
#endif


#ifndef PACKED
#  define PACKED __attribute__((packed))
#endif

#if   __BYTE_ORDER == __BIG_ENDIAN
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#else
#  error __BYTE_ORDER not defined
#endif

#if defined __cplusplus
extern "C" {
#endif


/* Generic RTP header extension */
typedef struct stream_rtp_header_ext {

  union {
    uint8_t  raw[4];
    uint32_t rawd;

    struct {
      uint16_t type;
      uint16_t size; /* Size of ext_data field in DWORDS */
    } PACKED;
  } PACKED;

  uint8_t ext_data[0];

} PACKED stream_rtp_header_ext_t;


/* Common RTP data header */
typedef struct stream_rtp_header {

  union {
    uint8_t raw[12];

    struct {
#if __BYTE_ORDER == __BIG_ENDIAN
      unsigned int version:2;   /* protocol version */
      unsigned int padding:1;   /* padding flag */
      unsigned int ext:1;       /* header extension flag */
      unsigned int cc:4;        /* CSRC count */

      unsigned int marker:1;    /* marker bit */
      unsigned int paytype:7;   /* payload type */
#else
      unsigned int cc:4;        /* CSRC count */
      unsigned int ext:1;       /* header extension flag */
      unsigned int padding:1;   /* padding flag */
      unsigned int version:2;   /* protocol version */

      unsigned int paytype:7;   /* payload type */
      unsigned int marker:1;    /* marker bit */
#endif
      uint16_t seq;              /* sequence number */
      uint32_t ts;               /* timestamp */
      uint32_t ssrc;             /* synchronization source */

      /*uint32_t csrc[0];*/      /* optional CSRC list */
    } PACKED;
  } PACKED;


  union {
    stream_rtp_header_ext_t hdr_ext[0];  
    uint8_t                 payload[0];  
  } PACKED;


} PACKED stream_rtp_header_t;

#if defined __cplusplus
};
#endif

#endif /* XINELIBOUTPUT_RTP_H_ */

