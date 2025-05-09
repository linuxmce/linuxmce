/*
$Id: ow_com_change.c,v 1.1 2011/01/03 00:18:52 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
	email: palfille@earthlink.net
	Released under the GPL
	See the header file: ow.h for full attribution
	1wire/iButton system from Dallas Semiconductor
*/

#include <config.h>
#include "owfs_config.h"
#include "ow.h"
#include "ow_connection.h"

#ifdef HAVE_LINUX_LIMITS_H
#include <linux/limits.h>
#endif

GOOD_OR_BAD COM_change( struct connection_in *connection)
{
	if ( connection == NO_CONNECTION ) {
		return gbBAD ;
	}

	// is connection thought to be open?
	RETURN_BAD_IF_BAD( COM_test(connection) ) ;

	switch ( SOC(connection)->type ) {
		case ct_i2c:
		case ct_usb:
			LEVEL_DEBUG("Unimplemented!!!");
			return gbBAD ;
		case ct_telnet:
			// set to change settings at next write
			if ( SOC(connection)->dev.telnet.telnet_negotiated == completed_negotiation ) {
				SOC(connection)->dev.telnet.telnet_negotiated = needs_negotiation ;
			}
			return gbGOOD ;
		case ct_tcp:
		case ct_netlink:
			LEVEL_DEBUG("Cannot change baud rate on %s",SAFESTRING(SOC(connection)->devicename)) ;
			return gbGOOD ;
		case ct_serial:
			return serial_change( connection ) ;
		case ct_unknown:
		case ct_none:
		default:
			LEVEL_DEBUG("ERROR!!! ----------- ERROR!");
			return gbBAD ;
	}
}
