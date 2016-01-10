#ifndef	CARD_GLOBAL_H
#define	CARD_GLOBAL_H
/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2004-2006, Xorcom
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "xdefs.h"

DEF_RPACKET_DATA(GLOBAL, NULL_REPLY);
DEF_RPACKET_DATA(GLOBAL, DESC_REQ);
DEF_RPACKET_DATA(GLOBAL, DEV_DESC,
	byte		rev;		/* Revision number */
	byte		type;		/* LSB: 1 - to_phone, 0 - to_line */
	xpp_line_t	line_status;	/* hook/ring status, depending on unit */
	);
DEF_RPACKET_DATA(GLOBAL, REGISTER_REQUEST,
	reg_cmd_t	reg_cmd;
	);
DEF_RPACKET_DATA(GLOBAL, PCM_WRITE,
	xpp_line_t	lines;
	byte		pcm[PCM_CHUNKSIZE];
	);
DEF_RPACKET_DATA(GLOBAL, PCM_READ,
	xpp_line_t	lines;
	byte		pcm[PCM_CHUNKSIZE];
	);
DEF_RPACKET_DATA(GLOBAL, SYNC_SOURCE,
	byte		sync_mode;
	byte		drift;
	);
DEF_RPACKET_DATA(GLOBAL, SYNC_REPLY,
	byte		sync_mode;
	byte		drift;
	);
DEF_RPACKET_DATA(GLOBAL, ERROR_CODE,
	byte		errorcode;
	union {
		reg_cmd_t	bad_spi_cmd;
	} info;
	);

enum sync_mode {
	SYNC_MODE_HOST	= 0x00,
	SYNC_MODE_AB	= 0x01,		/* Astribank sync */
	SYNC_MODE_PLL	= 0x03,		/* Adjust XPD's PLL according to HOST */
	SYNC_MODE_QUERY	= 0x80,
};

/* 0x04 */ DECLARE_CMD(GLOBAL, DESC_REQ, int xpd_num);
/* 0x19 */ DECLARE_CMD(GLOBAL, SYNC_SOURCE, enum sync_mode mode, int drift);
/* 0x11 */ DECLARE_CMD(GLOBAL, PCM_WRITE, xpp_line_t lines, volatile byte *buf);

extern xproto_table_t PROTO_TABLE(GLOBAL);
int run_initialize_registers(xpd_t *xpd);

#endif	/* CARD_GLOBAL_H */
