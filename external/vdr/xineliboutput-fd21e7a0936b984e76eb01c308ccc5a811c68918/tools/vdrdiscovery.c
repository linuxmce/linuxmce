/*
 * vdrdiscovery.c
 *
 * Simple broadcast protocol to search VDR with xineliboutput server 
 * from (local) network.
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vdrdiscovery.c,v 1.11 2011-02-28 13:17:52 phintuka Exp $
 *
 */

#include <stdlib.h>
#include <stdio.h>
#ifdef __FreeBSD__
#include <string.h>
#endif
#include <poll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define LOG_MODULENAME "[discovery] "
#include "../logdefs.h"

#include "vdrdiscovery.h"

/*
 * constants
 */

#ifndef DISCOVERY_PORT
#  define DISCOVERY_PORT 37890
#endif

/* discovery protocol strings (v1.0) */
#define DISCOVERY_1_0_HDR     "VDR xineliboutput DISCOVERY 1.0" "\r\n"
#define DISCOVERY_1_0_CLI     "Client: %s:%d"      "\r\n"
#define DISCOVERY_1_0_SVR     "Server port: %d"    "\r\n"
#define DISCOVERY_1_0_ADDR    "Server address: %s" "\r\n"
#define DISCOVERY_1_0_VERSION "Server version: " /*vdr-" VDRVERSION "\r\n\t"*/ \
                              "xineliboutput-" XINELIBOUTPUT_VERSION "\r\n"

/*
 *
 */

static inline int discovery_init(int port)
{
  int fd_discovery = -1;
  int iBroadcast = 1, iReuse = 1;
  struct sockaddr_in sin;

  if ((fd_discovery = socket(PF_INET, SOCK_DGRAM, 0/*IPPROTO_TCP*/)) < 0) {
    LOGERR("discovery_init: socket() failed");
    return -1;
  }

  if (setsockopt(fd_discovery, SOL_SOCKET, SO_BROADCAST, &iBroadcast, sizeof(int)) < 0)
    LOGERR("discovery_init: setsockopt(SO_BROADCAST) failed");

  if (setsockopt(fd_discovery, SOL_SOCKET, SO_REUSEADDR, &iReuse, sizeof(int)) < 0)
    LOGERR("discovery_init: setsockopt(SO_REUSEADDR) failed");

  sin.sin_family = AF_INET;
  sin.sin_port   = htons(port);
  sin.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  if (bind(fd_discovery, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    LOGERR("discovery_init: bind() failed");
    close(fd_discovery);
    return -1;
  }

  return fd_discovery;
}

int udp_discovery_init(void)
{
  return discovery_init(DISCOVERY_PORT);
}

static inline int udp_discovery_send(int fd_discovery, int port, char *msg)
{
  struct sockaddr_in sin;
  int len = strlen(msg);

  sin.sin_family = AF_INET;
  sin.sin_port   = htons(port);
  sin.sin_addr.s_addr = INADDR_BROADCAST;

  if(len != sendto(fd_discovery, msg, len, 0,
		   (struct sockaddr *)&sin, sizeof(sin))) {
    LOGERR("UDP broadcast send failed (discovery)");
    return -1;
  }
   
  //LOGDBG("UDP broadcast send succeed (discovery)");
  return 0;
}

int udp_discovery_broadcast(int fd_discovery, int server_port, const char *server_address)
{
  char *msg = NULL;
  int result;

  if(server_address && *server_address) {
    result = asprintf(&msg,
	     DISCOVERY_1_0_HDR     //"VDR xineliboutput DISCOVERY 1.0" "\r\n"
	     DISCOVERY_1_0_SVR     //"Server port: %d" "\r\n"
	     DISCOVERY_1_0_ADDR    //"Server Address: %d.%d.%d.%d \r\n"
	     DISCOVERY_1_0_VERSION //"Server version: xineliboutput-" XINELIBOUTPUT_VERSION "\r\n"
	     "\r\n",
	     server_port, server_address);
  } else {
    result = asprintf(&msg,
	     DISCOVERY_1_0_HDR     //"VDR xineliboutput DISCOVERY 1.0" "\r\n"
	     DISCOVERY_1_0_SVR     //"Server port: %d" "\r\n"
	     DISCOVERY_1_0_VERSION //"Server version: xineliboutput-" XINELIBOUTPUT_VERSION "\r\n"
	     "\r\n",
	     server_port);
  }

  if (result >= 0) {
    result = udp_discovery_send(fd_discovery, DISCOVERY_PORT, msg);
    free(msg);
  }

  return result;
}

static inline int udp_discovery_search(int fd_discovery, int port)
{
  char *msg = NULL;
  int result;

  result = asprintf(&msg, 
	   DISCOVERY_1_0_HDR  /* "VDR xineliboutput DISCOVERY 1.0" "\r\n" */
	   DISCOVERY_1_0_CLI  /* "Client: %s:%d" "\r\n" */
	   "\r\n",
	   "255.255.255.255",
	   port);

  if (result >= 0) {
    result = udp_discovery_send(fd_discovery, port, msg);
    free(msg);
  }

  return result;
}

int udp_discovery_recv(int fd_discovery, char *buf, int timeout,
		       struct sockaddr_in *source)
{
  socklen_t sourcelen = sizeof(struct sockaddr_in);
  struct pollfd pfd;
  int err;

  pfd.fd = fd_discovery;
  pfd.events = POLLIN;

  errno = 0;
  err = poll(&pfd, 1, timeout);
  if(err < 1) {
    if(err < 0)
      LOGERR("broadcast poll error");
    return err;
  }

  memset(source, 0, sourcelen);
  memset(buf, 0, DISCOVERY_MSG_MAXSIZE);

  err = recvfrom(fd_discovery, buf, DISCOVERY_MSG_MAXSIZE-1, 0,
		 (struct sockaddr *)source, &sourcelen);

  if(err <= 0)
    LOGDBG("fd_discovery recvfrom() error");

  return err;
}

int udp_discovery_is_valid_search(const char *buf)
{
  static const char id_string[] = { DISCOVERY_1_0_HDR "Client:" };

  if(!strncmp(id_string, buf, strlen(id_string))) {
    LOGMSG("Received valid discovery message %s", buf);
    return 1;
  }
   
  LOGDBG("BROADCAST: %s", buf);
  return 0;
}

int udp_discovery_find_server(int *port, char *address)
{
  static const char mystring[] = DISCOVERY_1_0_HDR "Server port: ";
  struct sockaddr_in from;
  char buf[DISCOVERY_MSG_MAXSIZE];
  int fd_discovery = -1;
  int trycount = 0;
  int err = 0;

  *port = DISCOVERY_PORT;
  strcpy(address, "vdr");

  if((fd_discovery = discovery_init(DISCOVERY_PORT)) < 0)
    return 0;

  while(err >= 0 && ++trycount < 4) {

    if((err = udp_discovery_search(fd_discovery, DISCOVERY_PORT) >= 0)) {
    
      errno = 0;
      while( (err = udp_discovery_recv(fd_discovery, buf, 500, &from)) > 0) {
	
	uint32_t tmp = ntohl(from.sin_addr.s_addr);

	buf[err] = 0;
	LOGDBG("Reveived broadcast: %d bytes from %d.%d.%d.%d \n%s", 
	       err,
	       ((tmp>>24)&0xff), ((tmp>>16)&0xff), 
	       ((tmp>>8)&0xff), ((tmp)&0xff),
	       buf);
	if(!strncmp(mystring, buf, strlen(mystring))) {
          char *iploc;
	  LOGDBG("Valid discovery message");
	  close(fd_discovery);

	  // default: use broadcast source address
	  sprintf(address, "%d.%d.%d.%d",
		  ((tmp>>24)&0xff), ((tmp>>16)&0xff), 
		  ((tmp>>8)&0xff), ((tmp)&0xff));

	  // Check if announce message includes alternative server address
	  iploc = strstr(buf + strlen(mystring), "Server address: ");
	  if(iploc) {
	    uint32_t svraddr;
	    iploc += strlen("Server address: ");
	    svraddr = inet_addr(iploc);
	    if(svraddr == INADDR_NONE || svraddr == INADDR_ANY) {
	      LOGMSG("Server provided invalid address !");
	    } else {
	      svraddr = ntohl(svraddr);
	      sprintf(address, "%d.%d.%d.%d",
		      ((svraddr>>24)&0xff), ((svraddr>>16)&0xff), 
		      ((svraddr>>8)&0xff), ((svraddr)&0xff));
	      LOGMSG("Replacing broadcast source address %d.%d.%d.%d "
		     "with server-given address %s",
		     ((tmp>>24)&0xff), ((tmp>>16)&0xff), 
		     ((tmp>>8)&0xff), ((tmp)&0xff),
		     address);
	    }
	  }

	  *port = -1;
	  if(1 == sscanf(buf + strlen(mystring), "%d", port) && 
	     *port >= 1000 && *port <= 0xffff)
	    return 1;
	  LOGMSG("Server-given port is invalid !");
	} else {
	  LOGDBG("NOT valid discovery message");
	}
      }
    }
  }

  /* failed */
  close(fd_discovery);
  return 0;
}


