/*
 * rtcp.h: RFC1889: RTCP
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: rtcp.h,v 1.4 2010-07-19 13:20:22 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_RTCP_H_
#define XINELIBOUTPUT_RTCP_H_

#if defined(__APPLE__) || defined(__FreeBSD__)
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


/* RTCP packet types */
typedef enum {
  RTCP_SR   = 200,
  RTCP_RR   = 201,
  RTCP_SDES = 202,
  RTCP_BYE  = 203,
  RTCP_APP  = 204
} rtcp_type_t;

/* RTCP SDES types */
typedef enum {
  RTCP_SDES_END   = 0,
  RTCP_SDES_CNAME = 1,

  RTCP_SDES_NAME  = 2,
  RTCP_SDES_EMAIL = 3,
  RTCP_SDES_PHONE = 4,
  RTCP_SDES_LOC   = 5,
  RTCP_SDES_TOOL  = 6,
  RTCP_SDES_NOTE  = 7,
  RTCP_SDES_PRIV  = 8
} rtcp_sdes_type_t;

/* RTCP common header word */
typedef struct {
  union {
    uint8_t raw[4];
    struct {
#if __BYTE_ORDER == __BIG_ENDIAN
      unsigned int version:2;   /* protocol version */
      unsigned int padding:1;   /* padding flag */
      unsigned int count:5;     /* varies by packet type */
#else
      unsigned int count:5;     /* varies by packet type */
      unsigned int padding:1;   /* padding flag */
      unsigned int version:2;   /* protocol version */
#endif
      unsigned int ptype:8;     /* RTCP packet type */
      
      uint16_t length;          /* pkt len in words, w/o this word */
    } PACKED;
  } PACKED;
} PACKED rtcp_common_t;

/* RTCP RR (Reception report) */
typedef struct {
  uint32_t ssrc;            /* data source being reported */
  unsigned int fraction:8;  /* fraction lost since last SR/RR */
  int lost:24;              /* cumul. no. pkts lost (signed!) */
  uint32_t last_seq;        /* extended last seq. no. received */
  uint32_t jitter;          /* interarrival jitter */
  uint32_t lsr;             /* last SR packet from this source */
  uint32_t dlsr;            /* delay since last SR packet */
} PACKED rtcp_rr_t;

/* RTCP SR (Sender report) */
typedef struct {
  uint32_t ssrc;
  uint32_t ntp_sec;  /* NTP timestamp, most significant word / seconds */
  uint32_t ntp_frac;
  uint32_t rtp_ts;
  uint32_t psent;    /* packets sent */
  uint32_t osent;    /* octets sent */
  rtcp_rr_t rr[0];   /* variable-length list */
} PACKED rtcp_sr_t;

/* RTCP SDES item */
typedef struct {
  uint8_t type;             /* type of item (rtcp_sdes_type_t) */
  uint8_t length;           /* length of item (in octets) */
  char    data[0];          /* text, not null-terminated */
} PACKED rtcp_sdes_item_t;

/* RTCP packet */
typedef struct {
  rtcp_common_t hdr;
  union {
    rtcp_sr_t sr;
    struct {
      uint32_t ssrc;
      rtcp_rr_t rr[0];
    } PACKED rr;
    struct {
      uint32_t ssrc;      /* first SSRC/CSRC */
      rtcp_sdes_item_t item[0]; /* list of SDES items */
    } PACKED sdes;
    struct {
      uint32_t src[0];   /* list of sources */
      /* can't express trailing text for reason */
    } PACKED bye;
  } PACKED;
} PACKED rtcp_packet_t;


#if defined __cplusplus
};
#endif

#endif /* XINELIBOUTPUT_RTCP_H_ */
