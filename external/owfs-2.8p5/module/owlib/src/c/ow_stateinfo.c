/*
$Id: ow_stateinfo.c,v 1.6 2010/05/25 20:57:38 alfille Exp $
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

struct stateinfo StateInfo = {
	.owlib_state = lib_state_pre,
	.start_time = 0,
	.dir_time = 0,
	.shutting_down = 0,
};
