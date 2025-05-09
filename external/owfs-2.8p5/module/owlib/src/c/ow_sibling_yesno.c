/*
$Id: ow_sibling_yesno.c,v 1.2 2010/09/24 21:04:24 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
	email: palfille@earthlink.net
	Released under the GPL
	See the header file: ow.h for full attribution
	1wire/iButton system from Dallas Semiconductor
*/

/* General Device File format:
    This device file corresponds to a specific 1wire/iButton chip type
	( or a closely related family of chips )

	The connection to the larger program is through the "device" data structure,
	  which must be declared in the acompanying header file.

	The device structure holds the
	  family code,
	  name,
	  device type (chip, interface or pseudo)
	  number of properties,
	  list of property structures, called "filetype".

	Each filetype structure holds the
	  name,
	  estimated length (in bytes),
	  aggregate structure pointer,
	  data format,
	  read function,
	  write funtion,
	  generic data pointer

	The aggregate structure, is present for properties that several members
	(e.g. pages of memory or entries in a temperature log. It holds:
	  number of elements
	  whether the members are lettered or numbered
	  whether the elements are stored together and split, or separately and joined
*/

#include <config.h>
#include "owfs_config.h"
#include "ow.h"

ZERO_OR_ERROR FS_r_sibling_Y(INT *Y, const char * sibling, struct one_wire_query *owq)
{
	struct one_wire_query * owq_sibling  = OWQ_create_sibling( sibling, owq ) ;
	SIZE_OR_ERROR sib_status ;

	if ( owq_sibling == NO_ONE_WIRE_QUERY ) {
		return -EINVAL ;
	}
	sib_status = FS_read_local(owq_sibling) ;
	Y[0] = OWQ_Y(owq_sibling) ;
	OWQ_destroy(owq_sibling) ;
	return sib_status >= 0 ? 0 : -EINVAL ;
}

ZERO_OR_ERROR FS_w_sibling_Y(INT Y, const char * sibling, struct one_wire_query *owq)
{
	ZERO_OR_ERROR write_error;
	struct one_wire_query * owq_sibling  = OWQ_create_sibling( sibling, owq ) ;

	if ( owq_sibling == NO_ONE_WIRE_QUERY ) {
		return -EINVAL ;
	}
	OWQ_Y(owq_sibling) = Y ;
	write_error = FS_write_local(owq_sibling) ;
	OWQ_destroy(owq_sibling) ;
	return write_error ;
}
