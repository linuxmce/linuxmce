/*
 * $RCSfile: ibios.h,v $  $Revision: 1.2 $  $Name:  $
 * $Id: ibios.h,v 1.2 2007/04/03 00:12:13 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/03 00:12:13 $
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
 *   Function prototypes for the various mid level PLC functions.
 *
 * ----------------------------------------------------------------------------
 */
#ifndef _IBIOS_H
#define _IBIOS_H

int is_plc(iusb_t *iplc, unsigned char *id);
int is_plc_ready(iusb_t *iplc);
void iplc_add_id(iusb_t *iplc, unsigned char *id, unsigned char group,
		unsigned char class);
int iplc_get_entry(iusb_t *iplc, unsigned char *id, int entry,
		unsigned char *buf);
void iplc_clear_links(iusb_t *iplc);
void iplc_clear_entry(iusb_t *iplc, unsigned char *id);
int iplc_count_links(iusb_t *iplc);
int plc_read_response(iusb_t *iplc, unsigned char *buf, int size,
		unsigned char matach);
int ibios_get_version(iusb_t *iplc, unsigned char *fw, unsigned char *id);
int ibios_read_memory(iusb_t *iplc, unsigned short start_add,
		unsigned short length, unsigned char *data);
int ibios_write_memory(iusb_t *iplc, unsigned short start_add,
		unsigned short length, unsigned char *data);
int ibios_mask(iusb_t *iplc, unsigned char or_mask, unsigned char and_mask);
int ibios_event(iusb_t *iplc, unsigned char event, unsigned char timer);
int ibios_reset(iusb_t *iplc);
int ibios_salad_status(iusb_t *iplc);
int ibios_erase_timers(iusb_t *iplc);
int ibios_reset_timers(iusb_t *iplc);
int ibios_timer_refresh(iusb_t *iplc);
int ibios_randomize(iusb_t *iplc);

#endif
