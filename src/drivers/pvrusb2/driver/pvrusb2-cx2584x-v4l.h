/*
 *
 *  $Id: pvrusb2-cx2584x-v4l.h 2226 2009-03-07 05:17:32Z isely $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *  Copyright (C) 2004 Aurelien Alleaume <slts@free.fr>
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

#ifndef __PVRUSB2_CX2584X_V4L_H
#define __PVRUSB2_CX2584X_V4L_H

/*

   This module connects the pvrusb2 driver to the I2C chip level
   driver which handles combined device audio & video processing.
   This interface is used internally by the driver; higher level code
   should only interact through the interface provided by
   pvrusb2-hdw.h.

*/

#include "pvrusb2-options.h"

#ifdef PVR2_ENABLE_CX2584X_V4L
#ifdef PVR2_ENABLE_OLD_I2COPS

#include "pvrusb2-i2c-track.h"

int pvr2_i2c_cx2584x_v4l_setup(struct pvr2_hdw *,struct pvr2_i2c_client *);

#endif /* PVR2_ENABLE_OLD_I2COPS */
#ifdef PVR2_ENABLE_V4L2SUBDEV

#include "pvrusb2-hdw-internal.h"

void pvr2_cx25840_subdev_update(struct pvr2_hdw *, struct v4l2_subdev *sd);

#endif /* PVR2_ENABLE_V4L2SUBDEV */
#endif /* PVR2_ENABLE_CX2584X_V4L */

#endif /* __PVRUSB2_CX2584X_V4L_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
