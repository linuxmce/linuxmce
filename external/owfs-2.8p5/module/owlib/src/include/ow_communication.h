/*
$Id: ow_communication.h,v 1.9 2011/01/02 04:25:05 alfille Exp $
    OW -- One-Wire filesystem
    version 0.4 7/2/2003

    Written 2003 Paul H Alfille
        Fuse code based on "fusexmp" {GPL} by Miklos Szeredi, mszeredi@inf.bme.hu
        Serial code based on "xt" {GPL} by David Querbach, www.realtime.bc.ca
        in turn based on "miniterm" by Sven Goldt, goldt@math.tu.berlin.de
    GPL license
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

 */

#ifndef OW_COMMUNICATION_H			/* tedious wrapper */
#define OW_COMMUNICATION_H

enum com_type {
	ct_unknown,
	ct_serial,
	ct_telnet,
	ct_tcp,
	ct_i2c,
	ct_usb,
	ct_netlink,
	ct_none,
} ;

enum com_state {
	cs_virgin,
	cs_deflowered,
} ;

struct com_serial {
	struct termios oldSerialTio;    /*old serial port settings */
} ;

struct com_tcp {
	char *host;
	char *service;
	struct addrinfo *ai;
	struct addrinfo *ai_ok;
	enum { pre_negotiation, needs_negotiation, completed_negotiation, } telnet_negotiated ; // have we attempted telnet negotiation -- reset at each OPEN
	int telnet_supported ; // server does telnet settings
} ;

struct communication {
	enum com_type type ;
	enum com_state state ;
	struct timeval timeout ; // for serial or tcp read
	FILE_DESCRIPTOR_OR_PERSISTENT file_descriptor;
	char * devicename ;
	union {
		struct com_serial serial ;
		struct com_tcp tcp ;
		struct com_tcp telnet ;
	} dev ;
	enum { flow_none, flow_soft, flow_hard, } flow ;
	speed_t baud; // baud rate in the form of B9600
	int bits;
	enum { parity_none, parity_odd, parity_even, parity_mark, } parity ;
	enum { stop_1, stop_2, stop_15, } stop;
	cc_t vmin ;
	cc_t vtime ;
} ;

#endif							/* OW_COMMUNICATION_H */
