/*
 *
 *  $Id: pvrusb2-i2c-chips-all.c 2249 2009-03-15 20:49:05Z isely $
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
#include "compat.h"
#include "pvrusb2-i2c-track.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "pvrusb2-i2c-cmd-v4l2.h"
#include "pvrusb2-i2c-cmd-v4l1.h"
#include "pvrusb2-audio-old.h"
#include "pvrusb2-audio.h"
#include "pvrusb2-tuner.h"
#ifdef PVR2_ENABLE_TDA9887
#include "pvrusb2-demod.h"
#endif
#include "pvrusb2-video-v4l.h"
#include "pvrusb2-video-ivtv.h"
#include "pvrusb2-cx2584x-old.h"
#include "pvrusb2-cx2584x-v4l.h"
#include "pvrusb2-wm8775.h"
#ifdef PVR2_ENABLE_CS53L32A
#include "pvrusb2-cs53l32a.h"
#endif

#ifdef PVR2_ENABLE_OLD_I2COPS

#define trace_i2c(...) pvr2_trace(PVR2_TRACE_I2C,__VA_ARGS__)

#define OP_V4L2_INIT 0 /* MUST come first so it is run first */
#define OP_V4L2_STANDARD 1
#define OP_V4L2_AUDIOMODE 2
#define OP_V4L2_BCSH 3
#define OP_V4L2_VOLUME 4
#define OP_V4L2_FREQ 5
#define OP_V4L2_AUDIORATE 6
#define OP_V4L2_SIZE 7
#define OP_V4L1_STANDARD 8
#define OP_V4L1_VOLUME 9
#define OP_V4L1_FREQ 10
#define OP_V4L2_LOG 11

static const struct pvr2_i2c_op * const ops[] = {
	[OP_V4L2_INIT] = &pvr2_i2c_op_v4l2_init,
	[OP_V4L2_STANDARD] = &pvr2_i2c_op_v4l2_standard,
	[OP_V4L2_AUDIOMODE] = &pvr2_i2c_op_v4l2_audiomode,
	[OP_V4L2_BCSH] = &pvr2_i2c_op_v4l2_bcsh,
	[OP_V4L2_VOLUME] = &pvr2_i2c_op_v4l2_volume,
	[OP_V4L2_FREQ] = &pvr2_i2c_op_v4l2_frequency,
	[OP_V4L2_SIZE] = &pvr2_i2c_op_v4l2_size,
	[OP_V4L2_LOG] = &pvr2_i2c_op_v4l2_log,
	[OP_V4L1_STANDARD] = &pvr2_i2c_op_v4l1_standard,
	[OP_V4L1_VOLUME] = &pvr2_i2c_op_v4l1_volume,
	[OP_V4L1_FREQ] = &pvr2_i2c_op_v4l1_frequency,
};

#define OP_V4L2_MASK_ALL ((1 << OP_V4L2_INIT) | \
			  (1 << OP_V4L2_STANDARD) | \
			  (1 << OP_V4L2_AUDIOMODE) | \
			  (1 << OP_V4L2_BCSH) | \
			  (1 << OP_V4L2_VOLUME) | \
			  (1 << OP_V4L2_FREQ) | \
			  (1 << OP_V4L2_SIZE) | \
			  (1 << OP_V4L2_LOG))

#define OP_V4L1_MASK_ALL ((1 << OP_V4L1_STANDARD) | \
			  (1 << OP_V4L1_VOLUME) | \
			  (1 << OP_V4L1_FREQ))

#if defined(I2C_DRIVERID_EXP1) && !defined(I2C_DRIVERID_SAA7115)
// Using temporary hack for missing I2C driver-ID for saa7115
#define I2C_DRIVERID_SAA7115 I2C_DRIVERID_EXP1
#endif

void pvr2_i2c_probe(struct pvr2_hdw *hdw,struct pvr2_i2c_client *cp)
{
	int id;
	id = cp->client->driver->id;

	/* Unless we figure out otherwise, we will treat every I2C client
	   as a V4L2 compatible module. */
	cp->ctl_mask = OP_V4L2_MASK_ALL;
	cp->status_poll = pvr2_v4l2_cmd_status_poll;

	if (id == I2C_DRIVERID_MSP3400) {
#ifdef ENABLE_PVRUSB2_AUDIO_OLD
		cp->ctl_mask = OP_V4L1_MASK_ALL;
		if (pvr2_i2c_msp3400_old_setup(hdw,cp)) {
			/* Talking to an older msp3400... */
			return;
		}
#endif
		cp->ctl_mask = OP_V4L2_MASK_ALL;
		if (pvr2_i2c_msp3400_setup(hdw,cp)) {
			/* Talking to a recent v4l2-aware msp3400... */
			return;
		}
	}
	if (id == I2C_DRIVERID_TUNER) {
		if (pvr2_i2c_tuner_setup(hdw,cp)) {
			return;
		}
	}
#ifdef I2C_DRIVERID_CX25840
#ifdef PVR2_ENABLE_CX2584X_V4L
	if (id == I2C_DRIVERID_CX25840) {
		if (pvr2_i2c_cx2584x_v4l_setup(hdw,cp)) {
			return;
		}
	}
#endif
#ifdef PVR2_ENABLE_CX2584X_OLD
	if (id == I2C_DRIVERID_CX25840) {
		if (pvr2_i2c_cx2584x_old_setup(hdw,cp)) {
			return;
		}
	}
#endif
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
#ifdef I2C_DRIVERID_SAA711X
	if (id == I2C_DRIVERID_SAA711X) {
#ifdef PVR2_ENABLE_SAA711X
		if (pvr2_i2c_decoder_v4l_setup(hdw,cp)) {
			return;
		}
#endif
#ifdef PVR2_ENABLE_SAA7115
		if (pvr2_i2c_decoder_ivtv_setup(hdw,cp)) {
			cp->ctl_mask &= ~OP_V4L2_MASK_ALL;
			return;
		}
#endif
	}
#endif
#ifdef I2C_DRIVERID_SAA7115
	if (id == I2C_DRIVERID_SAA7115) {
#ifdef PVR2_ENABLE_SAA7115
		if (pvr2_i2c_decoder_ivtv_setup(hdw,cp)) {
			cp->ctl_mask &= ~OP_V4L2_MASK_ALL;
			return;
		}
#endif
#ifdef PVR2_ENABLE_SAA711X
		if (pvr2_i2c_decoder_v4l_setup(hdw,cp)) {
			return;
		}
#endif
	}
#endif
#ifdef PVR2_ENABLE_TDA9887
#ifdef I2C_DRIVERID_TDA9887
	if (id == I2C_DRIVERID_TDA9887) {
		if (pvr2_i2c_demod_setup(hdw,cp)) {
			return;
		}
	}
#endif
	/* This is a really horrid way to identify the tda9887 driver,
	   however (1) its assigned id in kernels is usually -1 which is
	   completely useless to us, and (2) perhaps tda9887 is going to
	   disappear soon anyway.  Once it really goes away, then this
	   silly thing here will just become harmless.  Yes, we really are
	   looking at the name for the occurrence of "tda9887". */
	if (strstr(cp->client->name,"tda9887")) {
		if (pvr2_i2c_demod_setup(hdw,cp)) {
			return;
		}
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
	if (strstr(cp->client->driver->name,"tda9887")) {
		if (pvr2_i2c_demod_setup(hdw,cp)) {
			return;
		}
	}
#endif
#endif
}


const struct pvr2_i2c_op *pvr2_i2c_get_op(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(ops)) return NULL;
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
