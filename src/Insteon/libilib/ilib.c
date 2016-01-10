/*
 * $RCSfile: ilib.c,v $  $Revision: 1.3 $  $Name:  $
 * $Id: ilib.c,v 1.3 2007/04/21 03:23:28 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/21 03:23:28 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2006)
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ----------------------------------------------------------------------------
 *
 *  Description:
 *
 *  This is the low level code that impliments a communication interface
 *  to the Insteon PLC.  It supports multiple physical interfaces via
 *  a interface dependent dispatch table. The specific physical interface
 *  to use will be determined at initialization time.
 *
 *  This file has all the interface independent routines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include "idrv.h"
#include "ikd.h"
#ifdef INCLUDE_LIBHID
#include "ihidlib.h"
#endif
#include "iusb.h"
#include "iser.h"
#include "ihid.h"


/*
 * ilib_open
 *
 * This function initializes the connection to the PLC. This must
 * be called before any other function since the handle returned is
 * passed as an argument to all other functions.
 */
ilib_t *ilib_open(int method, char *dev)
{
	iusb_t *iplc = NULL;


	/* Allocate interface handle */
	if ((iplc = (iusb_t *)malloc(sizeof(iusb_t))) == NULL) {
		fprintf(stderr, "Failed to allocate memory for interface handle.\n");
		return NULL;
	}
	memset(iplc, 0, sizeof(iusb_t));

	/* Allocate a ring buffer for this interface */
	iplc->ring_buffer = (unsigned char *)malloc(RING_SIZE);
	if (iplc->ring_buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory for ring buffer.\n");
		free(iplc);
		return NULL;
	}

	/* Clear the ring buffer */
	memset(iplc->ring_buffer, 0, RING_SIZE);

	/* Initialize the interface dependent function table */
	switch (method) {
		case USE_IPLC:
			iplc->init = ikd_init;
			iplc->close = ikd_close;
			iplc->read_byte = ikd_read_byte;
			iplc->cts = ikd_cts;
			iplc->send_pkt = ikd_send_pkt;
			iplc->send = ikd_send;
			break;
#ifdef INCLUDE_LIBHID
		case USE_LIBHID:
			iplc->init = ihidlib_init;
			iplc->close = ihidlib_close;
			iplc->read_byte = ihidlib_read;
			iplc->cts = ihidlib_cts;
			iplc->send_pkt = ihidlib_write_echo;
			iplc->send = ihidlib_write;
			break;
#endif
		case USE_LIBUSB:
			iplc->init = iusb_init;
			iplc->close = iusb_close;
			iplc->read_byte = iusb_read;
			iplc->cts = iusb_cts;
			iplc->send_pkt = iusb_write_echo;
			iplc->send = iusb_write;
			break;
		case USE_SERIAL:
			iplc->init = iser_init;
			iplc->close = iser_close;
			iplc->read_byte = iser_read;
			iplc->cts = iser_cts;
			iplc->send_pkt = iser_write_echo;
			iplc->send = iser_write;
			break;
		case USE_HIDDEV:
			iplc->init = ihid_init;
			iplc->close = ihid_close;
			iplc->read_byte = ihid_read;
			iplc->cts = ihid_cts;
			iplc->send_pkt = ihid_write_echo;
			iplc->send = ihid_write;
			break;
		default:
			fprintf(stderr, "Method %d not implimented yet.\n", method);
			free(iplc->ring_buffer);
			free(iplc);
			return NULL;
			break;
	}

	/* Call interface dependent initialization */
	iplc->vfd = (iplc->init)(dev);

	if (iplc->vfd == NULL) {
		fprintf(stderr, "Open failed.\n");
		free(iplc->ring_buffer);
		free(iplc);
		return NULL;
	}

	return (ilib_t *)iplc;
}


/*
 * ilib_close
 *
 * Release control of the PLC and shutdown the connection to it.
 */
void ilib_close(ilib_t *iil)
{
	iusb_t *iplc = (iusb_t *)iil;

	(iplc->close)(iplc);
	free(iplc->ring_buffer);
	free(iplc);
}
