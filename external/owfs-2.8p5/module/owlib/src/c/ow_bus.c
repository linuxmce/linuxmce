/*
$Id: ow_bus.c,v 1.102 2010/09/23 03:17:16 alfille Exp $
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
#include "ow_connection.h"

GOOD_OR_BAD BUS_detect( struct connection_in * in )
{
	GOOD_OR_BAD (*detect) (struct connection_in * in) = in->iroutines.detect ;
	if ( detect != NO_DETECT_ROUTINE ) {
		return (detect)(in) ;
	}
	return gbBAD ;
}

void BUS_close( struct connection_in * in )
{
	void (*bus_close) (struct connection_in * in) = in->iroutines.close ;
	if ( bus_close != NO_CLOSE_ROUTINE ) {
		return (bus_close)(in);
	}
}

