/*
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __COMPAT_H
#define __COMPAT_H

#include "linux/version.h"
#include "linux/videodev.h"
#include "linux/videodev2.h"

/* MCI 15-Oct-2005 - This header is present purely for the needs of
   the modules pulled from ivtv.  Hopefully at some point we can stop
   grabbing those modules and then this won't be needed. */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#define LINUX26 1
#ifndef NEW_I2C
#define NEW_I2C
#endif
#endif

#ifndef I2C_NAME
#define I2C_NAME(s) (s)->name
#endif

#endif
