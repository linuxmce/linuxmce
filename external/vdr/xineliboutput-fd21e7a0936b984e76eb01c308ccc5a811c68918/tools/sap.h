/*
 * sap.h: RFC2974 Session Announcement Protocol (SAP) version 2
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: sap.h,v 1.9 2010-07-19 13:20:22 phintuka Exp $
 *
 */

#ifndef XINELIBOUTPUT_SAP_H_
#define XINELIBOUTPUT_SAP_H_

#include <arpa/inet.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
# include <machine/endian.h>
#else
# include <endian.h>
#endif


#ifndef PACKED
#  define PACKED  __attribute__((packed))
#endif

/*#define LOG_SAP*/

/* SAP IPv4 multicast addresses */
#define SAP_IP_ADDRESS_GLOBAL "224.2.127.254"   /* SAPv1 IP4 global scope multicast address */
#define SAP_IP_ADDRESS_ORG    "239.195.255.255" /* organization-local */
#define SAP_IP_ADDRESS_LOCAL  "239.255.255.255" /* local */
#define SAP_IP_ADDRESS_LINK   "224.0.0.255"     /* link-local */

#define SAP_IP_TTL     255
#define SAP_UDP_PORT   9875


typedef struct {

  /* RFC2974: SAP (Session Announcement Protocol) version 2 PDU */

  union {
    uint8_t raw0;
    struct {
#if __BYTE_ORDER == __BIG_ENDIAN
      uint8_t version    : 3;
      uint8_t addr_type  : 1;
      uint8_t reserved   : 1;
      uint8_t msg_type   : 1;
      uint8_t encrypted  : 1;
      uint8_t compressed : 1;
#else
      uint8_t compressed : 1;
      uint8_t encrypted  : 1;
      uint8_t msg_type   : 1;
      uint8_t reserved   : 1;
      uint8_t addr_type  : 1;
      uint8_t version    : 3;
#endif
    } PACKED;
  } PACKED;
  
  uint8_t  auth_len;
  uint16_t msgid_hash;

  union {
    uint8_t  u8[4];
    uint32_t u32;
  } PACKED ip4_source;
  
  char payload[0];

} PACKED sap_pdu_t;


static inline sap_pdu_t *sap_create_pdu(uint32_t src_ip, 
					uint16_t msgid,
					int announce, 
					const char *payload_type, 
					const char *payload)
{
  sap_pdu_t *pdu;
  int length = sizeof(sap_pdu_t) + strlen(payload) + 3;

  if(payload_type)
    length += strlen(payload_type);

  if(! (pdu = (sap_pdu_t*)malloc(length)))
    return NULL;

  memset(pdu, 0, sizeof(sap_pdu_t));
  pdu->version    = 1;  /* SAP v1 / v2 */
  pdu->msg_type   = announce ? 0 : 1;
  pdu->msgid_hash = msgid;
  pdu->ip4_source.u32 = src_ip;

  if(payload_type) {
    char *tmp = &pdu->payload[0];
    strcpy(tmp, payload_type);
    tmp += strlen(tmp) + 1;
    strcpy(tmp, payload);
  } else {
    /* payload type defaults to application/sdp */
    sprintf(&pdu->payload[0], "%s%c%c", payload, 0, 0);
  }

  return pdu;
}

static inline int sap_compress_pdu(sap_pdu_t *pdu)
{
#ifdef HAVE_ZLIB_H

  /* zlib compression */

  Compress();

  /*pdu->compressed = 1;*/

#endif

  /* not implemented */

  pdu->compressed = 0;
  return -1;
}

static inline int sap_send_pdu(int *pfd, sap_pdu_t *pdu, uint32_t dst_ip)
{
  int len = 0, r;
  int iReuse = 1, iLoop = 1, iTtl = SAP_IP_TTL;
  int fd;

  if(!pfd || *pfd < 0) {
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd < 0) {
      LOGERR("socket() failed (UDP/SAP multicast)");
      return -1;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &iReuse, sizeof(int));
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &iTtl, sizeof(int));
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &iLoop, sizeof(int));

    // Connect to multicast address
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SAP_UDP_PORT);
    sin.sin_addr.s_addr = dst_ip ? dst_ip : inet_addr(SAP_IP_ADDRESS_GLOBAL);
  
    if(connect(fd, (struct sockaddr *)&sin, sizeof(sin))==-1) 
      LOGERR("UDP/SAP multicast connect() failed.");
    
    // Set to non-blocking mode
    fcntl (fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK);

    if(pfd)
      *pfd = fd;

  } else {
    fd = *pfd;
  }

  // size of PDU
  len += strlen(&pdu->payload[0]);
  if(!strstr(&pdu->payload[0], "\r\n")) {
    /* assume mime content type is present */
    len += 1;
    len += strlen(&pdu->payload[len]);
    len += sizeof(sap_pdu_t);
  }

  // network order
  pdu->msgid_hash = htons(pdu->msgid_hash);

  // send
  r = send(fd, pdu, len, 0);
  if(r < 0)
    LOGERR("UDP/SAP multicast send() failed.");

  if(!pfd)
    close(fd);

#ifdef LOG_SAP
  /* log PDU */
  for(int i=0; i<len;) {
    char x[4096]="", a[4096]="";
    for(int j=0; j<16 && i<len; i++, j++) {
      char t[8], ch = ((char*)pdu)[i];
      sprintf(t, "%02X ", ((unsigned int)ch)&0xff);
      strcat(x, t);
      sprintf(t, "%c", (ch>=32 && ch<127) ? ch : '.');
      strcat(a, t);
    }
    LOGMSG("SAP: 0x%02x: %-50s%-18s",  i/16-1, x, a);
  }
#endif

  // back to host order
  pdu->msgid_hash = ntohs(pdu->msgid_hash);

  return r == len ? len : -1;
}


#endif /* XINELIBOUTPUT_SAP_H_ */
