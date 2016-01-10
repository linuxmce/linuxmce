/*
 * $RCSfile: x10_cmd.c,v $  $Revision: 1.1 $  $Name:  $
 * $Id: x10_cmd.c,v 1.1 2007/04/03 00:12:14 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/03 00:12:14 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2007)
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
 *    Functions to send X10 commands using the PLC.
 *
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "idrv.h"

extern int ibios_write_memory(ilib_t *iplc, unsigned short start_add,
		unsigned short length, unsigned char *data);
extern int ibios_mask(ilib_t *iplc, unsigned char or_mask,
		unsigned char and_mask);
extern int ibios_event(ilib_t *iplc, unsigned char event, unsigned char timer);


/*
 * send_x10
 *
 * This function fills the X10 xmit buffer and triggers the PLC
 * to send the command out.
 *
 * This is done by sending the X10 address and then the command.
 *
 * EX: A1 AON
 *     E5 EOFF
 */
void send_x10 (ilib_t *iplc, unsigned char x_addr, unsigned char x_cmd)
{

	int len;

	/* First write the Address to PLC's X10 transmit buffer */
	len = ibios_write_memory(iplc, 0x0165, 0x0001, &x_addr);
	usleep(2000);

	/*
	 * Set the X10 RTS flag (bit 7) and clear the X10 TXCOMMAND flag (bit 3)
	 * to send the X10 command.
	 */
	len = ibios_mask(iplc, 0x80, 0xf7);
	usleep(2000);

	/* First write the command to PLC's X10 transmit buffer */
	len = ibios_write_memory(iplc, 0x0165, 0x0001, &x_cmd);
	usleep(2000);

	/*
	 * Set the X10 RTS flag (bit 7) and set the X10 TXCOMMAND flag (bit 3)
	 * to send the X10 command.
	 */
	len = ibios_mask(iplc, 0x88, 0xff);
	usleep(2000);

	return;
}
