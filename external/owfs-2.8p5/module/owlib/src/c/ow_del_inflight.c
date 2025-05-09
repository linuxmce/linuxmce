/*
$Id: ow_del_inflight.c,v 1.7 2010/12/20 03:29:54 alfille Exp $
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

/* Can only be called by a separate (monitoring) thread  so the write-lock won't deadlock */
/* This allows removing Bus Masters while program is running
 * The master is usually only read-locked for normal operation
 * write lock is done in a separate thread when no requests are being processed */
void Del_InFlight( GOOD_OR_BAD (*nomatch)(struct connection_in * trial,struct connection_in * existing), struct connection_in * old_in )
{
	if ( old_in == NO_CONNECTION ) {
		return ;
	}

	LEVEL_DEBUG("Request master be removed: %s", SOC(old_in)->devicename);

	CONNIN_WLOCK ;
	if ( nomatch != NULL ) {
		struct connection_in * in = Inbound_Control.head ;
		while ( in != NO_CONNECTION ) {
			struct connection_in * next = in-> next ;
			if ( BAD( nomatch( old_in, in )) ) {
				LEVEL_DEBUG("Removing BUS index=%d %s",in->index,SAFESTRING(SOC(in)->devicename));
				RemoveIn(in) ;
			}
			in = next ;
		}
	}
	CONNIN_WUNLOCK ;
	RemoveIn(old_in);
}
