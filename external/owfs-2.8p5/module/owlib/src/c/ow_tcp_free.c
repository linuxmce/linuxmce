/*
$Id: ow_tcp_free.c,v 1.1 2010/12/22 02:27:10 alfille Exp $
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

/* ---------------------------------------------- */
/* raw COM port interface routines                */
/* ---------------------------------------------- */

//free serial port and restore attributes

void tcp_free(struct connection_in *connection)
{
	if ( SOC(connection)->state == cs_virgin ) {
		return ;
	}

	COM_close( connection ) ;
	FreeClientAddr( connection ) ;
}

