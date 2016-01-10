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
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#  warning "This module is tested only with 2.6 kernels"
#endif

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <zaptel.h>
#include "zap_debug.h"

static const char rcsid[] = "$Id: zap_debug.c 2123 2007-02-08 00:05:17Z tzafrir $";

#define	P_(x)	[ x ] = { .value = x, .name = #x, }
static	struct {
	int value;
	char *name;
} poll_names[] = {
	P_(POLLIN),
	P_(POLLPRI),
	P_(POLLOUT),
	P_(POLLERR),
	P_(POLLHUP),
	P_(POLLNVAL),
	P_(POLLRDNORM),
	P_(POLLRDBAND),
	P_(POLLWRNORM),
	P_(POLLWRBAND),
	P_(POLLMSG),
	P_(POLLREMOVE)
};
#undef	P_

void dump_poll(int print_dbg, const char *msg, int poll)
{
	int	i;

	for(i = 0; i < ARRAY_SIZE(poll_names); i++) {
		if(poll & poll_names[i].value)
			DBG("%s: %s\n", msg, poll_names[i].name);
	}
}


EXPORT_SYMBOL(dump_poll);
