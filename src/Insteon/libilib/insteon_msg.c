/*
 * $RCSfile: insteon_msg.c,v $  $Revision: 1.1 $  $Name:  $
 * $Id: insteon_msg.c,v 1.1 2006/10/04 03:34:15 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2006/10/04 03:34:15 $
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
 *    Functions to send Insteon messages using the PLC.
 *
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "idrv.h"
#include "insteon_msg.h"

/*
 * SENDMSG defines the way messages are written to the PLC. There
 * seems to be two ways of doing this. One uses IBOIS and the other
 * uses SALAD.  This allows us to switch easily between the two for
 * testing.
 */
#define SENDMSG insteon_send_msg
/* #define SENDMSG salad_send_message */


static int send_message (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to, unsigned char flags);

extern int ibios_write_memory(ilib_t *iplc, unsigned short start_add,
		unsigned short length, unsigned char *data);
extern int ibios_mask(ilib_t *iplc, unsigned char or_mask,
		unsigned char and_mask);
extern int ibios_event(ilib_t *iplc, unsigned char event, unsigned char timer);

/*
 * The msb_cache holds info about the last SET_ADDR_MSB command so that
 * the command doesn't have to be re-sent for every byte read or written.
 *
 * It expires after 15 seconds.
 *
 * It only holds one device ID. This idea could be expanded to hold
 * multiple device id's in the future if the use model changes. But
 * for now, this is used mostly when reading/writing blocks of memory
 * so the overhead re-sending the SET_ADDR_MSB when switching to a
 * different device isn't too great.
 */
#define MSB_CACHE_EXPR 15
static msb_cache_t msb_cache = {0L, {0,0,0}, 0};

/*
 * insteon_read_byte
 *
 * Read a byte from a remote device with error checking.
 */
int insteon_read_byte(ilib_t *iplc, unsigned char *id, unsigned short addr)
{
	int ret;
	time_t now;
	int retrys = 10;

	do {
		ret = 0;

		/* check msb cache to see if we need to set the MSB */
		now = time(NULL);
		if (((msb_cache.timestamp - now) > MSB_CACHE_EXPR) ||
				(memcmp(msb_cache.id, id, 3) != 0) ||
				(msb_cache.msb != ((addr >> 8) & 0xff)) ) {
			ret = SENDMSG(iplc, SET_ADDR_MSB, (addr>>8) & 0xff, id);
		}
		if (ret != -1) {
			msb_cache.timestamp = now;
			memcpy(msb_cache.id, id, 3);
			msb_cache.msb = (addr >> 8) & 0xff;
			ret = SENDMSG(iplc, PEEK, (addr & 0xff), id);
		}
		retrys--;
		if (ret == -1) {
			/* printf("Read byte retrying...\n"); */
		}
	} while ((ret == -1) && (retrys >= 0));

	return ret;
}

/*
 * insteon_write_byte
 *
 * Write a byte to a remote device with error checking
 */
int insteon_write_byte(ilib_t *iplc, unsigned char *id, unsigned short addr,
		unsigned char data)
{
	int ret;
	time_t now;
	int retrys = 4;

	do {
		ret = 0;

		/* check msb cache to see if we need to set the MSB */
		now = time(NULL);
		if (((msb_cache.timestamp - now) > MSB_CACHE_EXPR) ||
				(memcmp(msb_cache.id, id, 3) != 0) ||
				(msb_cache.msb != ((addr >> 8) & 0xff)) ) {
			ret = SENDMSG(iplc, SET_ADDR_MSB, (addr>>8) & 0xff, id);
		}

		if (ret != -1) {
			msb_cache.timestamp = now;
			memcpy(msb_cache.id, id, 3);
			msb_cache.msb = (addr >> 8) & 0xff;

			ret = SENDMSG(iplc, PEEK, (addr & 0xff), id);
			if (ret != -1) {
				ret = SENDMSG(iplc, POKE, data, id);
			}
		}
		retrys--;
	} while ((ret == -1) && (retrys >= 0));

	return ret;
}

/*
 * insteon_send_msg
 *
 * This sends a standard insteon message. In most cases it should return
 * two bytes of data. A ping however, should look for a device identification
 * broadcast comming back to get device info (may not work).
 */
int insteon_send_msg (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to)
{
	unsigned char flags = MAX_HOPS | HOPS_REMAIN | MSG_STANDARD | MSG_DIRECT;

	return send_message(iplc, i_cmd, i_cmd2, to, flags);
}


int insteon_send_group_msg (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to)
{
	unsigned char flags = MAX_HOPS | HOPS_REMAIN | MSG_STANDARD | MSG_GROUP;

	return send_message(iplc, i_cmd, i_cmd2, to, flags);
}



/*
 * send_message
 *
 * This function fills the insteon message buffer and triggers the PLC
 * to send the message out.
 */
static int send_message (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to, unsigned char flags)
{

	unsigned char msg[6];
	unsigned char buffer[256];
	int len;
	int hop = 0;
	int result;
	int timeout = 0;
	int msg_cnt = 0;
	int msg_exp;

	msg[0] = to[0];  /* To address high */
	msg[1] = to[1];  /* To address mid */
	msg[2] = to[2];  /* To address low */
	msg[3] = flags;
	msg[4] = i_cmd;  /* Insteon command 1 */
	msg[5] = i_cmd2; /* Insteon command 2 */

	/* msg_exp = MAX_HOPS; */
	msg_exp = MAX_HOPS >> 1;

	/* First write the message to PLC's memory */
	len = ibios_write_memory(iplc, 0x01a4, 0x0006, msg);
	usleep(2000);

	/* Setting bit 4 with the mask command causes the message to get sent */
	len = ibios_mask(iplc, 0x10, 0xff);

	msg[0] = 0;
	msg[1] = 0;


	/* Check messages coming back from the PLC for an acknowlegement */
	do {
		len = plc_read_response(iplc, buffer, sizeof(buffer), 0x4f);
		if ((len == 11) || (len == 25)) {
			if ((buffer[0] == 0x4f) && (buffer[1] == 0x04)) {
				msg_cnt++;
				if (memcmp(&buffer[2], to, 3) == 0) {
					msg[0] = buffer[len-2];
					msg[1] = buffer[len-1];
					hop = 1;
				}
			} else if ((buffer[0] == 0x4f) && (buffer[1] == 0x03) &&
					(buffer[8] & 0x20)) {
				msg_cnt++;
				if ((hop != 1) && (memcmp(&buffer[2], to, 3) == 0)) {
					msg[0] = buffer[len-2];
					msg[1] = buffer[len-1];
					hop = 2;
				}
			}
		} else {
			if (timeout++ > 40) {
				break;
			}
		}
	} while (((hop == 0) || (len > 0)) && (msg_cnt < msg_exp));

	if (hop) {
		result = (msg[0] << 8) | msg[1];
	} else {
		result = -1;
	}
	return result;
}

/*
 * salad_send_message
 *
 * This function writes an Insteon message directly to the SALad coreapp
 * message buffer and then triggers the coreapp to send the message.
 *
 * Supposedly, this is faster and lets the coreapp handle more more of
 * the handshaking. Howeaver, we still have to wait for an ACK or NAK
 * message reply.
 *
 * This capability is not documented in the current developers guide, it
 * is baaed on a developers forum post.
 */
int salad_send_message(ilib_t *iplc, unsigned char i_cmd,
			unsigned char i_cmd2, unsigned char *to)
{
	unsigned char msg[9];
	unsigned char buffer[256];
	int len;
	int hop = 0;
	int result;
	int timeout = 0;
	int msg_cnt = 0;
	int msg_exp;
	iusb_t *ih = (iusb_t *)iplc;

	msg[0] = ih->plc_id[0];  /* From address high */
	msg[1] = ih->plc_id[1];  /* From address mid */
	msg[2] = ih->plc_id[2];  /* From address low */
	msg[3] = to[0];  /* To address high */
	msg[4] = to[1];  /* To address mid */
	msg[5] = to[2];  /* To address low */
	msg[6] = MAX_HOPS | HOPS_REMAIN | MSG_STANDARD | MSG_DIRECT;
	msg[7] = i_cmd;  /* Insteon command 1 */
	msg[8] = i_cmd2; /* Insteon command 2 */

	msg_exp = (MAX_HOPS >> 2) + HOPS_REMAIN + 1;

	/* First write the message to PLC's memory */
	len = ibios_write_memory(iplc, 0xfbb3, 0x0009, msg);
	/* Then trigger the core app so that it sends it */
	ibios_event(iplc, 0x31, 0x00);

	msg[0] = 0;
	msg[1] = 0;
	/*
	 * Check messages coming back from the PLC for an acknowlegement
	 *
	 * The number of messages expected varies with the MAX_HOPS/
	 * HOPS_REMAIN value. So start tracking them here:
	 *    0x00 0x00  =  1
	 *    0x04 0x00  =  2
	 *    0x08 0x00  =  3
	 *    0x0c 0x00  =  4
	 *    0x00 0x01  =  2
	 *    0x04 0x01  =  3
	 *    0x08 0x01  =  4
	 *    0x0c 0x01  =  5
	 *    0x00 0x02  =  3
	 *    0x04 0x02  =  4
	 *    0x08 0x02  =  5
	 *    0x0c 0x02  =  6
	 *    0x00 0x03  =  4
	 *    0x04 0x03  =  5
	 *    0x08 0x03  =  6
	 *    0x0c 0x03  =  7
	 *
	 *    looks like msg count == (max_hops >> 2) + hops_remain;
	 */

	do {
		len = plc_read_response(iplc, buffer, sizeof(buffer), 0x4f);
		if ((len == 11) || (len == 25)) {
			msg_cnt++;
			if ((buffer[0] == 0x4f) && (buffer[1] == 0x04)) {
				if (memcmp(&buffer[2], to, 3) == 0) {
					msg[0] = buffer[len-2];
					msg[1] = buffer[len-1];
					hop = 1;
				}
			} else if ((buffer[0] == 0x4f) && (buffer[1] == 0x03) &&
					(buffer[8] & 0x20)) {
				if ((hop != 1) && (memcmp(&buffer[2], to, 3) == 0)) {
					msg[0] = buffer[len-2];
					msg[1] = buffer[len-1];
					hop = 2;
				}
			}
		} else {
			if (timeout++ > 40) {
				break;
			}
		}
	} while (((hop == 0) || (len > 0)) && (msg_cnt < msg_exp));

	if (hop) {
		result = (msg[0] << 8) | msg[1];
	} else {
		result = -1;
	}
	return result;
}

/*
 * salad_send_message_group
 *
 * This function writes an Insteon message directly to the SALad coreapp
 * message buffer and then triggers the coreapp to send the message.
 *
 * Supposedly, this is faster and lets the coreapp handle more more of
 * the handshaking. Howeaver, we still have to wait for an ACK or NAK
 * message reply.
 *
 * This capability is not documented in the current developers guide, it
 * is baaed on a developers forum post.
 */
int salad_send_message_group(ilib_t *iplc, unsigned char i_cmd,
			unsigned char group, unsigned char *to, unsigned char *from)
{
	unsigned char msg[9];
	unsigned char buffer[256];
	int len;
	int hop = 0;
	int result;

	msg[0] = from[0];  /* From address high */
	msg[1] = from[1];  /* From address mid */
	msg[2] = from[2];  /* From address low */
	msg[3] = to[0];  /* To address high */
	msg[4] = to[1];  /* To address mid */
	msg[5] = to[2];  /* To address low */
	msg[6] = MAX_HOPS | HOPS_REMAIN | MSG_STANDARD | MSG_GROUP;
	msg[7] = i_cmd;  /* Insteon command 1 */
	msg[8] = group;  /* Insteon group */

	/* First write the message to PLC's memory */
	len = ibios_write_memory(iplc, 0xfbb3, 0x0009, msg);
	/* Then trigger the core app so that it sends it */
	ibios_event(iplc, 0x31, 0x00);

	msg[0] = 0;
	msg[1] = 0;
	/* Check messages coming back from the PLC for an acknowlegement */
	do {
		len = plc_read_response(iplc, buffer, sizeof(buffer), 0x4f);
		if ((len == 11) || (len == 25)) {
			if ((buffer[0] == 0x4f) && (buffer[1] == 0x04)) {
				msg[0] = buffer[len-2];
				msg[1] = buffer[len-1];
				hop = 1;
			} else if ((buffer[0] == 0x4f) && (buffer[1] == 0x03) &&
					(buffer[8] & 0x20)) {
				msg[0] = buffer[len-2];
				msg[1] = buffer[len-1];
				hop = 1;
			}
		}
	} while ((hop == 0) && (len > 0));

	if (hop == 1) {
		result = (msg[0] << 8) | msg[1];
	} else {
		result = -1;
	}
	return result;
}


/*
 * insteon_write_msg
 *
 * but don't send it!  This was for testing but might be useful to
 * write the message in the SALAD buffer before triggering an event
 * to send it.
 */
int insteon_write_msg (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to)
{

	unsigned char msg[6];
	int len;

	msg[0] = to[0];  /* To address high */
	msg[1] = to[1];  /* To address mid */
	msg[2] = to[2];  /* To address low */
	msg[3] = MAX_HOPS | HOPS_REMAIN | MSG_STANDARD | MSG_DIRECT;
	msg[4] = i_cmd;  /* Insteon command 1 */
	msg[5] = i_cmd2; /* Insteon command 2 */

	/* First write the message to PLC's memory */
	len = ibios_write_memory(iplc, 0x01a4, 0x0006, msg);

	return len;
}


/*
 * insteon_send_ping
 *
 * Send a ping message to an insteon device. It should respond with
 * a direct acknowledgement followed by a device identification broadcast,
 * however, not all devices seem to do the device identification
 * broadcast.
 */
int insteon_send_ping (ilib_t *iplc, unsigned char *to,
		unsigned char *response, int cnt)
{
	iusb_t *ih = (iusb_t *)iplc;
	unsigned char msg[9];
	unsigned char buffer[256];
	int len;
	int hop = 0;
	int result;
	int timeout = 0;
	int msg_cnt = 0;
	int msg_exp;
	unsigned char max_hops = 0x0c, hops_remain = 0x03;

	msg[0] = ih->plc_id[0];  /* From address high */
	msg[1] = ih->plc_id[1];  /* From address mid */
	msg[2] = ih->plc_id[2];  /* From address low */
	msg[3] = to[0];  /* To address high */
	msg[4] = to[1];  /* To address mid */
	msg[5] = to[2];  /* To address low */
	msg[6] = max_hops | hops_remain | MSG_STANDARD | MSG_DIRECT;
	msg[7] = 0x10;  /* Insteon command 1 */
	msg[8] = 0x00;  /* Insteon command 2 */

	/* Expect 1 more response for device identification broadcast */
	msg_exp = (max_hops >> 2) + hops_remain + 2;

	/* First write the message to PLC's memory */
#if 0
	len = ibios_write_memory(iplc, 0xfbb3, 0x0009, msg);
	/* Then trigger the core app so that it sends it */
	ibios_event(iplc, 0x31, 0x00);
#else
	len = ibios_write_memory(iplc, 0x01a4, 0x0006, &msg[3]);
	usleep(2000);
	/* Setting bit 4 with the mask command causes the message to get sent */
	len = ibios_mask(iplc, 0x10, 0xff);
#endif

	msg[0] = 0;
	msg[1] = 0;

	do {
		len = plc_read_response(iplc, buffer, sizeof(buffer), 0x4f);
		if ((len == 11) || (len == 25)) {
			msg_cnt++;
			if ((buffer[0] == 0x4f) && (buffer[1] == 0x02)) {
				if (memcmp(&buffer[2], to, 3) == 0) {
					memcpy(response, buffer, len);
					msg[0] = 0x10;
					msg[1] = 0x00;
					hop = 3;
				}
			}
		} else {
			if (timeout++ > 60) {
				break;
			}
		}
	} while (((hop == 0) || (len > 0)) && (msg_cnt < msg_exp));

	if (hop == 3) {
		result = (msg[0] << 8) | msg[1];
	} else {
		if (cnt) {
			/* Keep trying */
			usleep(20000);  /* wait a bit before retrying */
			result = insteon_send_ping (iplc, to, response, --cnt);
		} else {
			result = -1;
		}
	}
	return result;
}


/*
 * insteon_detect_device
 *
 * Send out a group enrollment message and wait for reply(s).  A device
 * with the set button pressed should respond with the "SET button pressed
 * Slave" message which should include the devices's address, type, and
 * firmware revision.
 */
int insteon_detect_device (ilib_t *iplc, unsigned char *response)
{
	unsigned char msg[6];
	unsigned char buffer[256];
	int len;
	int result;
	int i;
	time_t timeout;
	int ack;

	msg[0] = 0x00;   /* PowerLinc device type high */
	msg[1] = 0x01;   /* PowerLinc device type low */
	msg[2] = 0x2c;   /* Firmware Revison */
	msg[3] = MAX_HOPS | HOPS_REMAIN | MSG_BROADCAST;
	msg[4] = 0x03;   /* SET Button pressed Master */
	msg[5] = 0x00;   /* Not used (device attributes) */

	/* First write the message to PLC's memory */
	len = ibios_write_memory(iplc, 0x01a4, 0x0006, msg);
	usleep(2000);

	/* Setting bit 4 with the mask command causes the message to get sent */
	len = ibios_mask(iplc, 0x10, 0xff);

	/*
	 * Do we need to enable and disable linking around this block of code?
	 *
	 * i.e.
	 *    ibios_event(iplc, 0x01, 0x01);
	 *    ibios_event(iplc, 0x54, 0x00);
	 */

	/* Set enrollment timer to 4 minutes? */
	msg[0] = 240;
	ibios_write_memory(iplc, 0x016d, 1, msg);

	msg[0] = 0;
	msg[1] = 0;
	/*
	 * Look for responses back from the PLC. This might need to wait
	 * for up to 4 minutes so even it it times out once, it should
	 * continue waiting.
	 */
	timeout = time(NULL);
	ack = 0;
	do {
		while ((len = plc_read_response(iplc, buffer, sizeof(buffer), 0x4f))
				> 0) {
			if ((buffer[0] == 0x4f) && (buffer[1] == 0x02)) {
				for (i = 0; i < len; i++) {
					response[i] = buffer[i];
				}
				ack = 1;
			}
		}
	} while (!ack && (time(NULL) < (timeout + 240)));

	if (ack == 1) {
		result = buffer[len - 1];
	} else {
		result = -1;
	}

	/* Set enrollment timer off */
	msg[0] = 0;
	ibios_write_memory(iplc, 0x016d, 1, msg);

	return result;
}



/*
 * insteon_send_extended_msg
 *
 * This function creates an extended message and sends it.  It does not
 * work and probably never will. Given that extended message handling is
 * broken in all current Insteon devices.
 */
int insteon_send_extended_msg (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to, unsigned char *user_data)
{

	unsigned char msg[20];
	unsigned char buffer[256];
	int len;
	int hop = 0;
	int response_cnt = 0;
	int result;
	int i;

	msg[0] = to[0];  /* To address high */
	msg[1] = to[1];  /* To address mid */
	msg[2] = to[2];  /* To address low */
	msg[3] = MAX_HOPS | HOPS_REMAIN | MSG_EXTENDED | MSG_DIRECT;
	msg[4] = i_cmd;  /* Insteon command 1 */
	msg[5] = i_cmd2; /* Insteon command 2 */

	for (i = 6; i < 20; i++) {
		msg[i] = user_data[i-6];
	}

	/* First write the message to PLC's memory */
	len = ibios_write_memory(iplc, 0x01a4, 0x0014, msg);
	usleep(2000);

	/* Setting bit 4 with the mask command causes the message to get sent */
	len = ibios_mask(iplc, 0x10, 0xff);

	msg[0] = 0;
	msg[1] = 0;
	/* Check messages coming back from the PLC for an acknowlegement */
	while ((len = plc_read_response(iplc, buffer, sizeof(buffer), 0x4f)) > 0){
		if ((buffer[0] == 0x4f) && (buffer[1] == 0x04)) {
			for (i = 0; i < 24; i++) {
				printf("[0x%02x]", buffer[i]);
			}
			msg[0] = buffer[len-2];
			msg[1] = buffer[len-1];
			hop = 1;
		}
		if (!hop & (buffer[0] == 0x4f) && (buffer[1] == 0x03) && 
				(buffer[8] & 0x20)) {
			printf("len = %d: ", len);
			for (i = 0; i < 24; i++) {
				printf("[0x%02x]", buffer[i]);
			}
			printf("\n");
			msg[0] = buffer[len-2];
			msg[1] = buffer[len-1];
			hop = 1;
		}
		response_cnt++;
	}

	if (hop == 1) {
		result = (msg[0] << 8) | msg[1];
	} else {
		result = -1;
	}
	return result;
}


/*
 * insteon_read_bytes
 *
 * Read a block of memory from a remote device.
 */
int insteon_read_bytes(ilib_t *iplc, unsigned char *id, unsigned short addr,
		int len, unsigned char *buffer)
{
	int i;
	int ret;

	for (i = 0; i < len; i++) {
		if ((ret = insteon_read_byte(iplc, id, addr)) != -1) {
			buffer[i] = ret & 0xff;
		} else {
			return -1;
		}
		addr++;
	}
	return 0;
}



/*
 * 
 *	insteon_set_remnote_link 
 *
 */
int insteon_set_remote_link(ilib_t *iplc, unsigned char *target_id, unsigned char *plc_id, unsigned char group, 
		unsigned char on_level, unsigned char ramp)
{
	unsigned char new_record[8];
	unsigned char record[8];
	int i = 0;
	unsigned short record_address = 0xFFF8;
	unsigned short write_target;
	int last_record = 0;
	
	/*insteon_write_byte(iplc,target_id,0xFFF8,0x80);*/ 
	/*insteon_read_bytes(iplc,target_id,0x0FF8,8,record);*/
	
	/*Build Record to write to ALDB/L  */
	new_record[0] = 0x22 | ALDB_L_VALID;   //For some reason, bit 5 needs to be set...Saw this when SDM creates record
	new_record[1] = group;
	new_record[2] = plc_id[0];
	new_record[3] = plc_id[1];
	new_record[4] = plc_id[2];
	new_record[5] = on_level;
	new_record[6] = ramp;
	new_record[7] = 0x0;
	
	/*Find next Open Address */
	
	do {  
		insteon_read_bytes(iplc,target_id,record_address,1,record);
	
		if ((record[0] & ALDB_L_VALID) == 0)
		{
			/*We found a record that has been erased.  Write it here. */
			write_target = record_address;
			last_record = ((record[0] & ALDB_L_HIGH_WATER) == 0);					
			break;
		}
		
		insteon_read_bytes(iplc,target_id,record_address+1 ,7,(record + 1));
				
		if ((record[1] == group) && (record[2] == plc_id[0]) && (record[3] == plc_id[1]) 
				&& (record[4] == plc_id[2])) 
		{ 
			//We have found an existing record for this PLC and this group.  Go ahead and overwrite this record 
			write_target = record_address;
			break;
		}
		record_address-= 8;
	}			
	while (1);

	if (last_record == 1)  //Make sure we mark the next record with a high water mark if this is the last record
	{
		insteon_write_byte(iplc,target_id,write_target-8,0x00);
	}
	
 	for (i = 0; i < 8; i++)
	{
		insteon_write_byte(iplc,target_id,write_target,new_record[i]);
		++write_target;
	}
	return last_record;

}	

int insteon_delete_remote_link(ilib_t *iplc, unsigned char *target_id, unsigned char *plc_id, unsigned char group)
{
	unsigned char record[8];
	unsigned short record_address = 0xFFF8;
	unsigned char next_record_flags;

	do 
	{
		insteon_read_bytes(iplc,target_id,record_address,1,record);
		
		if ((record[0] & ALDB_L_VALID) != 0)
		{
			/*This is a valid record
			 * First get the rest of the record
			 */
			insteon_read_bytes(iplc,target_id,record_address+1,7,record + 1); 
			
			if ((record[1] == group) && (record[2] == plc_id[0]) && (record[3] == plc_id[1]) 
				&& (record[4] == plc_id[2])) 
			{ 
				/* We found the record in the ALDB */
				
				/* Check to see if the next record has the high water mark set
				 * If so, we need to set the high water mark for this record after we delete it
				 */
				insteon_read_bytes(iplc,target_id,record_address-8,1,&next_record_flags); 
				if ((next_record_flags & ALDB_L_HIGH_WATER) == 0)
				{
					record[0] &= ~ALDB_L_HIGH_WATER;
				}
				/* Now clear the valid bit */
				record[0] &= ~ALDB_L_VALID;
				/* Write the new flags byte */
				insteon_write_byte(iplc,target_id,record_address,record[0]);
				return 0;
			}
		}
		//Go to the next record
		record_address -= 8;
	}
	while((record[0] & ALDB_L_HIGH_WATER) != 0);
	return 1;
}
