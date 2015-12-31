/*
 * $RCSfile: ibios.c,v $  $Revision: 1.4 $  $Name:  $
 * $Id: ibios.c,v 1.4 2007/09/09 18:07:48 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/09/09 18:07:48 $
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
 *  This is the mid level code for communicating with an Insteon PLC. It
 *  impliments various IBIOS functions.
 *
 *  is_plc              - True if the device id is the connected PLC.
 *  iplc_add_id         - Write or modify a link entry
 *  iplc_get_entry      - Find the entry for the specified device id
 *  iplc_clear_links    - Clear the PLC link database
 *  iplc_clear_entry    - Clear a single device entry
 *  iplc_get_date       - Get PLC date/time
 *
 *  ibios_get_version   - Get the PLC ID and firmware version
 *  ibios_read_memory   - Read a block of memory from the PLC
 *  ibios_write_memory  - Write data to the PLC's memory
 *  ibios_mask          - Apply and and or masks to a single PLC byte.
 *  ibios_event         - Fire an event on the PLC.
 *  ibios_reset         - Reset the PLC.
 *  ibios_erase_timers  - Erase all timers, keep address intact.
 *  ibios_reset_timers  - Erase all timers, reset address tables.
 *  ibios_timer_refresh - Recalculate next timer.
 *  ibios_randomize     - Randominze the timer offset.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "idrv.h"
#include "ibios.h"

/*
 * Upper level PLC commands
 *
 * These functions provide an interface to the PLC link database.
 *
 * is_plc              - True if the device id is the connected PLC.
 * iplc_add_id         - Write or modify a link entry
 * iplc_get_entry      - Find the entry for the specified device id
 * iplc_clear_links    - Clear the PLC link database
 * iplc_clear_entry    - Clear a single device entry
 * iplc_get_date       - Get PLC date/time
 */

/*
 * is_plc
 *
 * Checks a device id to see if it is the PLC's id.
 */
int is_plc(iusb_t *iplc, unsigned char *id)
{
	if(memcmp(iplc->plc_id, id, 3) == 0) {
		return 1;
	}
	return 0;
}

/*
 * iplc_add_id
 *
 * Given a device ID and a group number. Add an entry to the PLC link
 * table.
 */
void iplc_add_id(iusb_t *iplc, unsigned char *id, unsigned char group,
		unsigned char class)
{
	unsigned int start;
	unsigned short link;
	unsigned short last;
	unsigned char link_buf[10];
	unsigned char link_ptr[2];
	unsigned char flag;
	unsigned char lsb;
	int match;

	/* Find the start address for this device id */
	start = 0x10000 - ((((id[2] & 0xfe)/2) * 8) + 8);
	match = 0;
	last = start;

	while (!match && start > 0) {
		ibios_read_memory(iplc, start, 8, link_buf);

		link = link_buf[0] * 256 + (link_buf[1] & 0xf8);
		flag = link_buf[1] & 0x07;
		lsb = link_buf[1] & 0x01;

		if (link != 0) {
			last = link;
		}

		/* printf("Read link entry at 0x%04x, link = 0x%04x\n", start, link); */
		match = (((id[2] | lsb) == id[2]) && (id[0] == link_buf[3]) &&
			(id[1] == link_buf[2]) && (link_buf[4] == group) &&
			(class == (flag & 0x6)));

		if ((flag & 0x06) == 0) { /* This record is empty so add it here */
			/* printf("Adding record at 0x%04x\n", start); */
			/*
			 * This could be a group master so we need to pass that info
			 * in too. For now, we only add slave records.
			 */
			link_buf[0] = 0;
			link_buf[1] = (id[2] & 0x01) | class;
			link_buf[2] = id[1];
			link_buf[3] = id[0];
			link_buf[4] = group;
			link_buf[5] = 0;
			link_buf[6] = 0;
			link_buf[7] = 0;
			ibios_write_memory(iplc, start, 8, link_buf);
			return;
		}
		/* printf("link = 0x%04x\n", link); */
		if (!match) {
			start = link;
		}
	}

	/*
	 * Two possibilities when we get here. Either an exact match was
	 * found or the end of the list was reached.
	 */
	if (match) {
		/* printf("Found an exact match, updating data bytes\n"); */
		link_buf[5] = 0;
		link_buf[6] = 0;
		link_buf[7] = 0;
		ibios_write_memory(iplc, start, 8, link_buf);
	} else {
		/* printf("Need to add a new link to the end of the list\n"); */

		/* read the next link pointer value */
		ibios_read_memory(iplc, 0xfbf8, 2, link_ptr);
		link = link_ptr[0] * 256 + (link_ptr[1] & 0xf8);

		/* plug in the next link pointer into last (13 msbits) */
		link_buf[0] = link_ptr[0];
		link_buf[1] |= link_ptr[1];
		ibios_write_memory(iplc, last, 2, link_buf);

		/* write record at pointer value */
		link_buf[0] = 0;
		link_buf[1] = (id[2] & 0x01) | class;
		link_buf[2] = id[1];
		link_buf[3] = id[0];
		link_buf[4] = group;
		link_buf[5] = 0;
		link_buf[6] = 0;
		link_buf[7] = 0;
		ibios_write_memory(iplc, link, 8, link_buf);

		/* update next link pointer value */
		link -= 8;
		link_ptr[0] = (unsigned char)(link / 256);
		link_ptr[1] = link - (link_ptr[0] * 256);
		ibios_write_memory(iplc, 0xfbf8, 2, link_ptr);
	}
}


/*
 * iplc_count_links
 *
 * Read all the entries from the PLC's link table. Also need a way to
 * return all the entries.  Don't assume we know all the ID's that
 * have been stored.
 *
 * Start at top of memory and work down, looking for valid entries.
 *
 * How do we return all the entries?  Pass in a pointer to a link
 * array and fill it out?
 */
int iplc_count_links(iusb_t *iplc)
{
	unsigned int start;
	unsigned int link;
	int i;
	unsigned char link_buf[10];
	char flag[4];
	unsigned char lsb;
	int count = 0;

	start = 0x10000;

	for (i = 0; i < 128; i++) {
		start -= 8;
		ibios_read_memory(iplc, start, 8, link_buf);
		link = link_buf[0] * 256 + (link_buf[1] & 0xf8);
		lsb = (link_buf[1] & 0x01) | (i * 2);

		if ((link_buf[1] & 0x6) == 0x6) {
			strcpy(flag, "OTH");
		} else if (link_buf[1] & 0x04) {
			strcpy(flag, "MST");
		} else if (link_buf[1] & 0x02) {
			strcpy(flag, "SLV");
		} else {
			strcpy(flag, "   ");
		}

		while (link_buf[0] != 0) {
			count++;
			/* Valid record, print it */
			printf("%s %02x.%02x.%02x %03d 0x%02x 0x%02x 0x%02x\n",
					flag,
					link_buf[3], link_buf[2], lsb,
					link_buf[4], link_buf[5], link_buf[6], link_buf[7]);

			/* Follow linked list */
			ibios_read_memory(iplc, link, 8, link_buf);
			link = link_buf[0] * 256 + (link_buf[1] & 0xf8);
			lsb = link_buf[1] & 0x01;

			if ((link_buf[1] & 0x6) == 0x6) {
				strcpy(flag, "OTH");
			} else if (link_buf[1] & 0x04) {
				strcpy(flag, "MST");
			} else if (link_buf[1] & 0x02) {
				strcpy(flag, "SLV");
			} else {
				strcpy(flag, "   ");
			}
		}
	}

	return count;
}


/*
 * iplc_get_entry
 *
 * Get a (all) database entry(s) for a specific ID. Since there can be
 * more than one entry (different group #'s, master/slave) for a specific
 * device id, how do we relay that info?
 *
 * Return the total number found and specify which (by number? we want.
 */
int iplc_get_entry(iusb_t *iplc, unsigned char *id, int entry,
		unsigned char *buf)
{
	unsigned int start;
	unsigned char link_buf[10];
	unsigned char flag;
	unsigned char lsb;
	int match;
	int count = 0;

	start = 0x10000 - ((((id[2] & 0xfe)/2) * 8) + 8);
	match = 0;

	while (start > 0) {
		ibios_read_memory(iplc, start, 8, link_buf);
		start = link_buf[0] * 256 + (link_buf[1] & 0xf8);
		flag = link_buf[1] & 0x07;
		lsb = link_buf[1] & 0x01;


		if ((flag & 0x06) == 0) {
			memset(buf, 0, 8);
			return 0;
		}

		match = (((id[2] | lsb) == id[2]) && (id[0] == link_buf[3]) &&
			(id[1] == link_buf[2]));

		/* This gets the correct entry */
		if (match) {
			count++;
			if (count == entry) {
				buf[0] = flag >> 1;
				buf[1] = link_buf[3];
				buf[2] = link_buf[2];
				buf[3] = id[2];
				buf[4] = link_buf[4];
				buf[5] = link_buf[5];
				buf[6] = link_buf[6];
				buf[7] = link_buf[7];
			}
		}
	}
	return count;
}


/*
 * iplc_clear_links
 *
 * Clear the PLC's link table.
 */
void iplc_clear_links(iusb_t *iplc)
{
	unsigned int start_addr = 0xfff8;
	int i;
	unsigned char buf[8];

	/* Mark all records as empty */
	memset(buf, 0, 8);
	for (i = 0; i < 128; i++) {
		ibios_write_memory(iplc, start_addr, 8, buf);
		start_addr -= 8;
	}

	/* reset overflow pointer */
	buf[0] = 0xf9;
	buf[1] = 0xf8;
	ibios_write_memory(iplc, 0xfbf8, 2, buf);
}


/*
 * iplc_clear_entry
 *
 * Clear a single entry in the PLC's link table.
 */
void iplc_clear_entry(iusb_t *iplc, unsigned char *id)
{
	unsigned int start_addr;
	unsigned char buf[8];

	start_addr = 0x10000 - ((((id[2] & 0xfe)/2) * 8) + 8);
	memset(buf, 0, 8);

	printf("Clearing entry at 0x%04x\n", start_addr);
	ibios_write_memory(iplc, start_addr, 8, buf);
}

/*
 * iplc_get_date
 *
 * Read the date and time from the PLC
 */
void iplc_get_date(iusb_t *iplc, unsigned char *date)
{
	unsigned int start_addr;

	start_addr = 0x0158;
	ibios_read_memory(iplc, start_addr, 9, date);
}


/*
 * Mid level PLC commands
 *
 * These functions implement most of the iBIOS PLC interface.
 *
 * ibios_get_version   - Get the PLC ID and firmware version
 * ibios_read_memory   - Read a block of memory from the PLC
 * ibios_write_memory  - Write data to the PLC's memory
 * ibios_mask          - Apply and and or masks to a single PLC byte.
 * ibios_event         - Fire an event on the PLC.
 * ibios_reset         - Reset the PLC.
 * ibios_erase_timers  - Erase all timers, keep address intact.
 * ibios_reset_timers  - Erase all timers, reset address tables.
 * ibios_timer_refresh - Recalculate next timer.
 * ibios_randomize     - Randominze the timer offset.
 */

/*
 * ibios_get_version
 *
 * Query the PLC for it's device ID and firmware version. The device
 * ID is 3 bytes long and the incoming character pointer must point to
 * a block large enough to hold it. The firmware version is a single byte.
 */
int ibios_get_version(iusb_t *iplc, unsigned char *fw,
		unsigned char *id)
{
	unsigned char buf[10];
	int len;

	send_cmd(iplc, 0x48, NULL, 0);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x48);
	if (len > 5) {
		*fw = buf[6];
		//sprintf((char *)id, "%02x.%02x.%02x", buf[1], buf[2], buf[3]);
		memcpy(id,&buf[1],3);
		memcpy(iplc->plc_id, &buf[1], 3);
		return 0;
	}
	return -1;
}


/*
 * ibios_read_memory
 *
 * Given a start address in the PLC's flat memory map and a length, read
 * the values there and return them. The returned data is stuffed in the
 * data argument, thus the data argument must be large enough to hold
 * all the data.
 */
int ibios_read_memory(iusb_t *iplc, unsigned short start_add,
		unsigned short length, unsigned char *data)
{
	unsigned char buf[10];
	unsigned char *tmp;
	int len;

	buf[0] = (start_add >> 8) & 0xff;
	buf[1] = start_add & 0xff;
	buf[2] = (length >> 8) & 0xff;
	buf[3] = length & 0xff;
	send_cmd(iplc, 0x42, buf, 4);
	tmp = malloc(length + 8);
	len = plc_read_response(iplc, tmp, (length + 8), 0x42);
	if (len > length) {
		/* should we check the returned values to make sure it's right? */
		memcpy(data, (tmp + 5), length);
		len = length;
	}
	free (tmp);
	return len;
}

/*
 * ibios_write_memory
 *
 * Given a start address in the PLC's flat memory map, a length, and a 
 * block of data, write the data to the PLC.
 *
 * This will check the return and if the PCL indicates a NAK, it will 
 * retry until the PLC responds with a ACK (good for an infinite loop!)
 */
int ibios_write_memory(iusb_t *iplc, unsigned short addr,
		unsigned short length, unsigned char *data)
{
	unsigned char buf[10];
	unsigned char *tmp;
	unsigned short csum;
	int len;
	int tout = 0;
	int timeout = 4;

	tmp = malloc (length + 8);
	memset (tmp, 0, (length + 8));

	tmp[0] = (addr >> 8) & 0xff;
	tmp[1] = addr & 0xff;
	tmp[2] = (length >> 8) & 0xff;
	tmp[3] = length & 0xff;

	memcpy ((tmp+6), data, length);
	csum = chksum(tmp, (length + 6));
	tmp[5] = csum & 0xff;
	tmp[4] = (csum >> 8) & 0xff;

	while ((send_cmd(iplc, 0x40, tmp, (int)(length + 6)) == -1) && (tout < 5)) {
		sleep(2);
		tout++;
	}
	if (tout == 5) {
		printf("Timed out trying to write data to PLC.\n");
		free(tmp);
		return -1;
	}

	len = plc_read_response(iplc, buf, sizeof(buf), 0x40);

	while ((len > 0) && (buf[len - 1] == 0x15)) { /* NAK, resend? */
		printf("Write failed (got NAK), retry\n");

		sleep(4); /* wait 2 seconds before trying again */
		send_cmd(iplc, 0x40, tmp, (int)(length + 6));
		len = plc_read_response(iplc, buf, sizeof(buf), 0x40);
		if (!timeout--) {
			printf("Too many NAK's, aborting\n");
			break;
		}
	}

#if 0
	/*
	 * Should the memory location be checksummed after the write to
	 * make sure it was transferred correctly?
	 */
	send_cmd(iplc, 0x44, tmp, 4);
	memset (tmp, 0, (length + 8));
	len = plc_read_response(iplc, tmp, (length + 8), 0x44);
	if ((tmp[6] != (csum & 0xff)) || (tmp[5] != ((csum >> 8) & 0xff))) {
		printf("Checksum mismatch! %02x %02x   %02x %02x\n", tmp[6],
				(csum & 0xff), tmp[5], ((csum >> 8) & 0xff));
		len = -1;
	}
#endif

	free(tmp);
	return len;
}


/*
 * ibios_mask
 *
 * A mask instruction will change selected bits in a single byte.
 *
 * One of the primary uses is to flip the tramsmit bit telling the PLC
 * that there is a command ready to be sent out over the powerline.
 *
 * The byte is anded with the and_mask and or'd with the or_mask.
 *
 * FIXME:
 *   This function has the byte address hard coded as 0x0142 which
 *   is used for initiating Insteon messages. This should be changed
 *   to be a passed in argument to make it more general purpose.
 */
int ibios_mask(iusb_t *iplc, unsigned char or_mask,
		unsigned char and_mask)
{
	unsigned char buf[10];
	int len;

	buf[0] = 0x01;
	buf[1] = 0x42;
	buf[2] = or_mask;
	buf[3] = and_mask;
	send_cmd(iplc, 0x46, buf, 4);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x46);
	return len;
}

/*
 * ibios_event
 *
 * Cause an IBIOS event to fire on the PLC.
 */
int ibios_event(iusb_t *iplc, unsigned char event, 
		unsigned char timer)
{
	unsigned char buf[10];
	int len;

	buf[0] = event;
	buf[1] = timer;
	send_cmd(iplc, 0x47, buf, 2);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x47);
	usleep(50000);
	return len;
}

/*
 * ibios_reset
 *
 * Force a power-on reset. Based on page 102, event note 1 in the
 * developers guide.
 */
int ibios_reset(iusb_t *iplc)
{
	unsigned char buf[10];
	int len;

	buf[0] = 0x01;
	buf[1] = 0x54;
	buf[2] = 0x80;
	buf[3] = 0xff;
	send_cmd(iplc, 0x46, buf, 4);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x46);
	sleep(5);
	return len;
}


int ibios_salad_status(iusb_t *iplc)
{
	unsigned char buf[10];
	int ret;

	ret = ibios_event(iplc, 0x55, 0x00);
	ret = plc_read_response(iplc, buf, sizeof(buf), 0x45);
	if (buf[1] == 0xff) {
		printf("SALed busy\n");
	}
	return buf[1];
}

/*
 * ibios_erase_timers
 *
 * Erase all timers, keeping address intact.
 */
int ibios_erase_timers(iusb_t *iplc)
{
	unsigned char buf[10];
	int len;

	buf[0] = 0x51;
	buf[1] = 0x00;
	send_cmd(iplc, 0x47, buf, 2);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x47);
	sleep(1);
	return len;
}

/*
 * ibios_reset_timers
 *
 * Erase all timers and reset the address tables to:
 *   0x1800
 *   0x3000
 *   0x4800
 */
int ibios_reset_timers(iusb_t *iplc)
{
	unsigned char buf[10];
	int len;

	buf[0] = 0x52;
	buf[1] = 0x00;
	send_cmd(iplc, 0x47, buf, 2);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x47);
	sleep(1);
	return len;
}

/*
 * ibios_timer_refresh
 *
 * Recalculate next timer. Call this after setting the clock, adding a timer
 * deleting a timer or changing location.
 */
int ibios_timer_refresh(iusb_t *iplc)
{
	unsigned char buf[10];
	int len;

	buf[0] = 0x53;
	buf[1] = 0x00;
	send_cmd(iplc, 0x47, buf, 2);

	memset(buf, 0, 10);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x47);
	sleep(1);
	return len;
}

/*
 * ibios_randomize
 *
 * Randomize the security value for the timers.
 */
int ibios_randomize(iusb_t *iplc)
{
	unsigned char buf[10];
	int len;

	buf[0] = 0x56;
	buf[1] = 0x00;
	send_cmd(iplc, 0x47, buf, 2);
	len = plc_read_response(iplc, buf, sizeof(buf), 0x47);
	sleep(1);
	return len;
}

