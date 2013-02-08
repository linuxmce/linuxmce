/*
 *
 *  $Id: pvrusb2-sysfs-sel.c 1665 2007-10-14 04:17:22Z isely $
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

#include <linux/version.h>

/* This file is present PURELY to select between one of two possible sysfs
 * implementations, depending on kernel version.  For kernels 2.6.22 or
 * later we'll use the new version which does away with class_device.
 * (Yes, we could actually go all the way back to 2.6.19 with that but I
 * can't easily test kernels that old anymore and the old sysfs
 * implementation worked fine for those.) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
/* Use implementation which requires class_device. */
#include "pvrusb2-sysfs-cd.c"
#else
/* Use better version which assumes additional features of the Linux device
 * structure.  This version can't function at all for kernels older than
 * 2.6.18, due to required features not being implemented until that
 * point. */
#include "pvrusb2-sysfs.c"
#endif

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
