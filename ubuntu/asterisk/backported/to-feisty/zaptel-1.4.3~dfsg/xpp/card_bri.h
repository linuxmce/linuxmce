#ifndef	CARD_BRI_H
#define	CARD_BRI_H
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

#include "xpd.h"

enum bri_opcodes {
	XPROTO_NAME(BRI, REGISTER_REQUEST)		= 0x0F,
	XPROTO_NAME(BRI, REGISTER_REPLY)		= 0x10,
	XPROTO_NAME(BRI, SET_LED)			= 0x33,
};

DEF_RPACKET_DATA(BRI, REGISTER_REPLY,	/* Get status of a single register (for debugging) */
	reg_cmd_t	regcmd;
	);
DEF_RPACKET_DATA(BRI, REGISTER_REQUEST,
	reg_cmd_t	reg_cmd;
	);

#endif	/* CARD_BRI_H */
