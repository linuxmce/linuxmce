/*
 *
 *  $Id: pvrusb2-i2c-chips-v4l2.c 2249 2009-03-15 20:49:05Z isely $
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

#include "pvrusb2-options.h"
#include <linux/kernel.h>
#include "pvrusb2-i2c-track.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "pvrusb2-i2c-cmd-v4l2.h"
#include "pvrusb2-audio.h"
#include "pvrusb2-tuner.h"
#ifdef PVR2_ENABLE_TDA9887
#include "pvrusb2-demod.h"
#endif
#include "pvrusb2-video-v4l.h"
#include "pvrusb2-cx2584x-v4l.h"
#include "pvrusb2-wm8775.h"
#ifdef PVR2_ENABLE_CS53L32A
#include "pvrusb2-cs53l32a.h"
#endif
#include "compat.h"

#ifdef PVR2_ENABLE_OLD_I2COPS

#define trace_i2c(...) pvr2_trace(PVR2_TRACE_I2C,__VA_ARGS__)

#define OP_INIT 0 /* MUST come first so it is run first */
#define OP_STANDARD 1
#define OP_AUDIOMODE 2
#define OP_BCSH 3
#define OP_VOLUME 4
#define OP_FREQ 5
#define OP_AUDIORATE 6
#define OP_CROP 7
#define OP_SIZE 8
#define OP_LOG 9

static const struct pvr2_i2c_op * const ops[] = {
	[OP_INIT] = &pvr2_i2c_op_v4l2_init,
	[OP_STANDARD] = &pvr2_i2c_op_v4l2_standard,
	[OP_AUDIOMODE] = &pvr2_i2c_op_v4l2_audiomode,
	[OP_BCSH] = &pvr2_i2c_op_v4l2_bcsh,
	[OP_VOLUME] = &pvr2_i2c_op_v4l2_volume,
	[OP_FREQ] = &pvr2_i2c_op_v4l2_frequency,
	[OP_CROP] = &pvr2_i2c_op_v4l2_crop,
	[OP_SIZE] = &pvr2_i2c_op_v4l2_size,
	[OP_LOG] = &pvr2_i2c_op_v4l2_log,
};

void pvr2_i2c_probe(struct pvr2_hdw *hdw,struct pvr2_i2c_client *cp)
{
	int id;
	id = cp->client->driver->id;
	cp->ctl_mask = ((1 << OP_INIT) |
			(1 << OP_STANDARD) |
			(1 << OP_AUDIOMODE) |
			(1 << OP_BCSH) |
			(1 << OP_VOLUME) |
			(1 << OP_FREQ) |
			(1 << OP_CROP) |
			(1 << OP_SIZE) |
			(1 << OP_LOG));
	cp->status_poll = pvr2_v4l2_cmd_status_poll;

	if (id == I2C_DRIVERID_MSP3400) {
		if (pvr2_i2c_msp3400_setup(hdw,cp)) {
			return;
		}
	}
	if (id == I2C_DRIVERID_TUNER) {
		if (pvr2_i2c_tuner_setup(hdw,cp)) {
			return;
		}
	}
#ifdef PVR2_ENABLE_CX2584X_V4L
	if (id == I2C_DRIVERID_CX25840) {
		if (pvr2_i2c_cx2584x_v4l_setup(hdw,cp)) {
			return;
		}
	}
#endif
#ifdef PVR2_ENABLE_WM8775
	if (id == I2C_DRIVERID_WM8775) {
		if (pvr2_i2c_wm8775_setup(hdw,cp)) {
			return;
		}
	}
#endif
#ifdef PVR2_ENABLE_CS53L32A
#ifdef I2C_DRIVERID_CS53L32A
	if (id == I2C_DRIVERID_CS53L32A) {
		if (pvr2_i2c_cs53l32a_setup(hdw,cp)) {
			return;
		}
	}
#endif
#endif
#ifdef PVR2_ENABLE_SAA711X
	if (id == I2C_DRIVERID_SAA711X) {
		if (pvr2_i2c_decoder_v4l_setup(hdw,cp)) {
			return;
		}
	}
#endif
#ifdef PVR2_ENABLE_TDA9887
	if (id == I2C_DRIVERID_TDA9887) {
		if (pvr2_i2c_demod_setup(hdw,cp)) {
			return;
		}
	}
#endif
}


const struct pvr2_i2c_op *pvr2_i2c_get_op(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(ops))
		return NULL;
	return ops[idx];
}

#endif /* PVR2_ENABLE_OLD_I2COPS */


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
