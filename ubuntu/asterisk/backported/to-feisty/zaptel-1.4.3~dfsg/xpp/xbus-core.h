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
#ifndef	XBUS_CORE_H
#define	XBUS_CORE_H

#include "xpd.h"

#define	MAX_BUSES		32
#define	XFRAME_DATASIZE		512

struct xframe {
	atomic_t	frame_len;
	byte		*packets;	/* max XFRAME_DATASIZE */
};

#define XFRAME_LEN(frame)	atomic_read(&(frame)->frame_len)

int xbus_core_init(void);		/* Initializer */
void xbus_core_shutdown(void);		/* Terminator */

#ifdef XPP_DEBUGFS
/* Debugfs handling */
int xbus_log(xbus_t *xbus, xpd_t *xpd, int direction, const void *buf, unsigned long len);
#endif

/* Frame handling */
xframe_t *xbus_frame_new(xbus_t *xbus, gfp_t flags);
void xbus_frame_free(xbus_t *xbus, xframe_t *p);
void dump_xframe(const char msg[], const xbus_t *xbus, const xframe_t *xframe);

/*
 * Return pointer to next packet slot in the frame
 * or NULL if the frame is full.
 */
xpacket_t *xframe_next_packet(xframe_t *xframe, int len);

/* XBUS handling */
xbus_t	*xbus_of(int xbus_num);
xpd_t	*xpd_of(const xbus_t *xbus, int xpd_num);
xbus_t	*xbus_new(xbus_ops_t *ops);
void xbus_remove(xbus_t *xbus);
void xbus_activate(xbus_t *xbus);
void xbus_disconnect(xbus_t *xbus);

void xbus_poller_notify(xbus_t *xbus, struct card_desc_struct *card_desc);
void xbus_reset_counters(xbus_t *xbus);

int xbus_register_xpd(xbus_t *xbus, xpd_t *xpd);
int xbus_unregister_xpd(xbus_t *xbus, xpd_t *xpd);

#endif	/* XBUS_CORE_H */

