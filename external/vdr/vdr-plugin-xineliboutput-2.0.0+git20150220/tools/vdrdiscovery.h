/*
 * vdrdiscovery.h
 *
 * Simple broadcast protocol to search VDR with xineliboutput server 
 * from (local) network.
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vdrdiscovery.h,v 1.5 2009-02-10 15:26:11 phintuka Exp $
 *
 */

#ifndef _VDRDISCOVERY_H_
#define _VDRDISCOVERY_H_

#define DISCOVERY_MSG_MAXSIZE  1024

#ifdef __cplusplus
extern "C" {
#endif

struct sockaddr_in;

/*
 * Client interface
 */

/*
 * udp_discovery_find_server()
 *
 * Search for server. Return address and port.
 * Returns 0 on success.
 */
int udp_discovery_find_server(int *port, char *address);

/*
 * Server interface
 */

/*
 * udp_discovery_init()
 *
 * Initialize server socket. Return value is the socket file descriptor,
 * and can be used for polling.
 * Returns < 0 on error.
 */
int udp_discovery_init(void);

/*
 * udp_discovery_broadcast()
 *
 * Send server announcement.
 * Returns 0 on success.
 */
int udp_discovery_broadcast(int fd_discovery, int server_port, const char *server_address);

/*
 * udp_discovery_recv()
 *
 * Receive query or announcement.
 * Returns number of bytes received, <= 0 on error.
 * Result is null-terminated string, not more than DISCOVERY_MSG_MAXSIZE bytes.
 */
int udp_discovery_recv(int fd_discovery, char *buf, int timeout,
		       struct sockaddr_in *source);

/*
 * udp_discovery_is_valid_search()
 *
 * Check if string is valid search message.
 * Returns 1 for valid messages, 0 for invalid messages.
 */
int udp_discovery_is_valid_search(const char *buf);

#ifdef __cplusplus
};
#endif


#endif // _VDRDISCOVERY_H_
