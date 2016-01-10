#ifndef	XPP_ZAP_H
#define	XPP_ZAP_H
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
#include "xproto.h"

void xpd_disconnect(xpd_t *xpd);
int xframe_send(xbus_t *xbus, xframe_t *xframe);
void card_detected(struct card_desc_struct *card_desc);
xpd_t *xpd_alloc(size_t privsize, xbus_t *xbus, int xpd_num, const xproto_table_t *proto_table, int channels, byte revision);
void xpd_remove(xpd_t *xpd);
void update_xpd_status(xpd_t *xpd, int alarm_flag);
void update_zap_ring(xpd_t *xpd, int pos, bool on);
void update_line_status(xpd_t *xpd, int pos, bool good);
void fill_beep(u_char *buf, int num, int duration);
void got_sync_from(xpd_t *xpd);
int xpp_open(struct zt_chan *chan);
int xpp_close(struct zt_chan *chan);
int xpp_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long arg);
int xpp_maint(struct zt_span *span, int cmd);

#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>

extern struct proc_dir_entry	*xpp_proc_toplevel;
#endif

#define	SPAN_REGISTERED(xpd)	atomic_read(&(xpd)->zt_registered)

#endif	/* XPP_ZAP_H */
