/*
 * $RCSfile: insteon_msg.h,v $  $Revision: 1.1 $  $Name:  $
 * $Id: insteon_msg.h,v 1.1 2006/10/04 03:34:16 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2006/10/04 03:34:16 $
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
 *    Header file for insteon message sending functions.
 * ----------------------------------------------------------------------------
 */
#ifndef _INSTEON_MSG_H
#define _INSTEON_MSG_H

typedef struct _msb_cache_t {
	time_t timestamp;
	unsigned char id[3];
	unsigned char msb;
} msb_cache_t;


int insteon_set_remote_link(ilib_t *iplc, unsigned char *target_id, 
		unsigned char *plc_id, unsigned char group, unsigned char on_level, 
		unsigned char ramp);
int insteon_delete_remote_link(ilib_t *iplc, unsigned char *target_id, 
		unsigned char *plc_id, unsigned char group);	
int insteon_read_bytes(ilib_t *iplc, unsigned char *id, unsigned short addr,
		int len, unsigned char *buffer);
int insteon_read_byte(ilib_t *iplc, unsigned char *id, unsigned short addr);
int insteon_write_byte(ilib_t *iplc, unsigned char *id, unsigned short addr,
		unsigned char data);
int insteon_send_msg (ilib_t *iplc, unsigned char i_cmd, unsigned char i_cmd2,
		unsigned char *to);
int insteon_send_group_msg (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to);
int insteon_send_extended_msg (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to, unsigned char *user_data);
int insteon_send_ping (ilib_t *iplc, unsigned char *to, unsigned char *resp,
		int cnt);
int write_insteon_msg (ilib_t *iplc, unsigned char i_cmd, unsigned char i_cmd2,
		unsigned char *to);
int insteon_detect_device (ilib_t *iplc, unsigned char *response);
char *command(unsigned char cmd);
char *flags(unsigned char flag);
char *device_type_name(unsigned char hi, unsigned char low);

int salad_send_message (ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to);
int salad_send_message_group(ilib_t *iplc, unsigned char i_cmd,
		unsigned char i_cmd2, unsigned char *to, unsigned char *from);


/*
 * HOPS
 *
 * When creating messages, there are two "hop" values that can be set. Based
 * on the developer's documentation it seems that these should always be
 * set to the same value.
 *
 *   0x00, 0x00 - Don't re-transmit this message at all
 *   0x04, 0x01 - Allow 1 re-transmition
 *   0x08, 0x02 - Allow 2 re-transmitions
 *   0x0c, 0x03 - Allow 3 re-transmitions
 *
 *   The more hops, the more messages that must be processed and the long
 *   it will take, however, it should be more reliable.  At some point
 *   this should be tuneable.
 */

/* Message flags */
#define MAX_HOPS        0x0c /* Currently setting to 3 hops */
#define HOPS_REMAIN     0x03 /* Hops remaining set to 3 */
#define MSG_STANDARD    0x00
#define MSG_EXTENDED    0x80
#define MSG_GROUP       0xc0
#define MSG_GROUP_CLEAN 0x40
#define MSG_DIRECT      0x00
#define MSG_BROADCAST   0x80
#define MSG_0_HOP       0x00
#define MSG_1_HOP       0x05
#define MSG_2_HOP       0x0A
#define MSG_3_HOP       0x0F


/* Insteon message defines */
#define RES1                0x00
#define ASSIGN_TO_GROUP     0x01   /* group number */
#define DELETE_FROM_GROUP   0x02   /* group number */
#define READ_CONNECTIONS    0x03   /* group number, 0 = all */
#define READ_DEVICE_DATA    0x04
#define FACTORY_RESET       0x05   /* Returns extended message */
#define DEVICE_RESET        0x06   /* security code */
#define TERMINATE_DOWNLOAD  0x07
#define ENTER_MASTER_PRG    0x09   /* group number */
#define ENTER_SLAVE_PRG     0x0a
#define PING                0x10   /* returns device identification broadcast */
#define ON                  0x11   /* on level (00 - ff) or group number */
#define FAST_ON             0x12   /* on level or group number */
#define OFF                 0x13   /* none or group number */
#define FAST_OFF            0x14   /* none or group number */
#define BRIGHT              0x15   /* none or group number */
#define DIM                 0x16   /* none or group number */
#define START_MANUAL_CHNG   0x17   /* 1 = up, 0 = down */
#define STOP_MANUAL_CHNG    0x18   /* 1 = up, 0 = down */
#define STATUS_REQUEST      0x19   /* return link delta and on level */
#define STATUS_REPORT       0x1a   /* new status */
#define READ_LAST_LEVEL     0x1b   /* level returned in cmd 2 */
#define SET_LAST_LEVEL      0x1c   /* on level */
#define READ_PRESET_LEVEL   0x1d   /* returned in cmd2 */
#define SET_PRESET_LEVEL    0x1e   /* on level */
#define READ_OPER_FLAGS     0x1f   /* returned in cmd2 */
#define SET_OPER_FLAGS      0x20   /* flags */
#define DELETE_GRP_X10      0x21   /* group number */
#define LOAD_OFF            0x22
#define LOAD_ON             0x23
#define DO_READ_EE          0x24   /* read init values from EEPROM */
#define LEVEL_POKE          0x25   /* on level */
#define RATE_POKE           0x26   /* ramp rate */
#define CURRENT_STATUS      0x27   /* on level (update companion device) */
#define SET_ADDR_MSB        0x28   /* high byte of 16 bit addr */
#define POKE                0x29   /* byte to write */
#define POKE_EXT            0x2a   /* poke 13 bytes (extended message) */
#define PEEK                0x2b   /* LSB of address to peek or poke */
#define PEEK_INTERNAL       0x2c   /* LSB of address to read from */
#define POKE_INTERNAL       0x2d   /* byte to write */
#define ASSIGN_TO_CPN_GRP   0x81   /*  */


#define ALDB_L_VALID		0x80	/*Mask for All Link Database (Linear) Record Valid Bit */
#define ALDB_L_HIGH_WATER	0x02	/*Mask for All Link Database (Linear) High Water Mark Bit */


#endif
