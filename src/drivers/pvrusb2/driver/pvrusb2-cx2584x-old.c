/*
 *
 *  $Id: pvrusb2-cx2584x-old.c 2302 2009-06-14 21:10:45Z isely $
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

/*

   This source file is specifically designed to interface with the
   cx25480 in kernels older than 2.6.16.

*/

#include "pvrusb2-cx2584x-old.h"
#include "pvrusb2-video-v4l.h"
#include "pvrusb2-i2c-cmd-v4l2.h"

#ifdef PVR2_ENABLE_OLD_I2COPS
#ifdef PVR2_ENABLE_CX2584X_OLD

#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include <media/audiochip.h>
#include <linux/videodev.h>
#ifdef PVR2_ENABLE_V4L2COMMON
#include <media/v4l2-common.h>
#endif
#include <linux/errno.h>
#include <linux/slab.h>

/* This is a horrid piece of crap to put here.  However the "official"
   definition of this enum is buried inside a private header within
   the kernel tree so we have no choice but to duplicate it here.
   Fortunately this is an "old" driver; more recent kernels dispense
   with this foolishness and obviously it is never going to change in
   an old kernel so we're relatively safe. */
enum cx25840_input {
	CX25840_TUNER,
	CX25840_COMPOSITE0,
	CX25840_COMPOSITE1,
	CX25840_SVIDEO0,
	CX25840_SVIDEO1
};

struct pvr2_v4l_cx2584x {
	struct pvr2_i2c_handler handler;
	struct pvr2_decoder_ctrl ctrl;
	struct pvr2_i2c_client *client;
	struct pvr2_hdw *hdw;
	unsigned long stale_mask;
};


struct routing_scheme_item {
	int vid;
	int aud;
};

struct routing_scheme {
	const struct routing_scheme_item *def;
	unsigned int cnt;
};

static const struct routing_scheme_item routing_scheme0[] = {
	[PVR2_CVAL_INPUT_TV] = {
		.vid = CX25840_TUNER,
		.aud = AUDIO_TUNER,
	},
	[PVR2_CVAL_INPUT_RADIO] = { /* wm8775 routes similar to composite */
		.vid = CX25840_COMPOSITE0,
		.aud = AUDIO_EXTERN_1,
	},
	[PVR2_CVAL_INPUT_COMPOSITE] = {
		.vid = CX25840_COMPOSITE0,
		.aud = AUDIO_EXTERN_1,
	},
	[PVR2_CVAL_INPUT_SVIDEO] = {
		.vid = CX25840_SVIDEO0,
		.aud = AUDIO_EXTERN_1,
	},
};

static const struct routing_scheme routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = {
		.def = routing_scheme0,
		.cnt = ARRAY_SIZE(routing_scheme0),
	},
};

static void set_input(struct pvr2_v4l_cx2584x *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	int vid_input,aud_input;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x set_input(%d)",
		   hdw->input_val);

	if ((sid < ARRAY_SIZE(routing_schemes)) &&
	    ((sp = routing_schemes + sid) != 0) &&
	    (hdw->input_val >= 0) &&
	    (hdw->input_val < sp->cnt)) {
		vid_input = sp->def[hdw->input_val].vid;
		aud_input = sp->def[hdw->input_val].aud;
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c cx2584x set_input (old):"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}

	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_INPUT,&vid_input);
	pvr2_i2c_client_cmd(ctxt->client,AUDC_SET_INPUT,&aud_input);
}


static void vbi_hack(struct pvr2_v4l_cx2584x *ctxt)
{
	/*
	  Mike Isely <isely@pobox.com> 19-Nov-2006 - This bit of
	  nuttiness for cx25840 causes that module to correctly set up
	  its video scaling.  This is really a problem in the cx25840
	  module itself, but we work around it here.  The problem has
	  not been seen in ivtv because there VBI is supported and set
	  up.  We don't do VBI here (at least not yet) and thus we
	  never attempted to even set it up.
	*/
	struct v4l2_format fmt;
	memset(&fmt,0,sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_SLICED_VBI_CAPTURE;
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_FMT,&fmt);
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx25840 activate vbi hack");
}


static int check_input(struct pvr2_v4l_cx2584x *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->input_dirty != 0;
}


static void set_audio(struct pvr2_v4l_cx2584x *ctxt)
{
	u32 val;
	struct pvr2_hdw *hdw = ctxt->hdw;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x set_audio %d",hdw->srate_val);
#ifdef PVR2_ENABLE_CX2341XMOD
	switch (hdw->srate_val) {
	default:
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_48000:
		val = V4L2_AUDCLK_48_KHZ;
		break;
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_44100:
		val = V4L2_AUDCLK_441_KHZ;
		break;
	}
#else
	switch (hdw->srate_val) {
	default:
	case PVR2_CVAL_SRATE_48:
		val = V4L2_AUDCLK_48_KHZ;
		break;
	case PVR2_CVAL_SRATE_44_1:
		val = V4L2_AUDCLK_441_KHZ;
		break;
	}
#endif
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_AUDIO_CLOCK_FREQ,&val);
}


static int check_audio(struct pvr2_v4l_cx2584x *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->srate_dirty != 0;
}


struct pvr2_v4l_cx2584x_ops {
	void (*update)(struct pvr2_v4l_cx2584x *);
	int (*check)(struct pvr2_v4l_cx2584x *);
};


static const struct pvr2_v4l_cx2584x_ops decoder_ops[] = {
	{ .update = set_input, .check = check_input},
	{ .update = set_audio, .check = check_audio},
};


static void decoder_detach(struct pvr2_v4l_cx2584x *ctxt)
{
	ctxt->client->handler = 0;
	pvr2_hdw_set_decoder(ctxt->hdw,NULL);
	kfree(ctxt);
}


static int decoder_check(struct pvr2_v4l_cx2584x *ctxt)
{
	unsigned long msk;
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(decoder_ops); idx++) {
		msk = 1 << idx;
		if (ctxt->stale_mask & msk) continue;
		if (decoder_ops[idx].check(ctxt)) {
			ctxt->stale_mask |= msk;
		}
	}
	return ctxt->stale_mask != 0;
}


static void decoder_update(struct pvr2_v4l_cx2584x *ctxt)
{
	unsigned long msk;
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(decoder_ops); idx++) {
		msk = 1 << idx;
		if (!(ctxt->stale_mask & msk)) continue;
		ctxt->stale_mask &= ~msk;
		decoder_ops[idx].update(ctxt);
	}
}


static void decoder_enable(struct pvr2_v4l_cx2584x *ctxt,int fl)
{
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx25840 decoder_enable(%d)",fl);
	pvr2_v4l2_cmd_stream(ctxt->client,fl);
}


static unsigned int decoder_describe(struct pvr2_v4l_cx2584x *ctxt,char *buf,unsigned int cnt)
{
	return scnprintf(buf,cnt,"handler: pvrusb2-cx2584x-old");
}


static void decoder_reset(struct pvr2_v4l_cx2584x *ctxt)
{
	int ret;
	ret = pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_RESET,NULL);
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx25840 decoder_reset (ret=%d)",ret);
	vbi_hack(ctxt);
}


const static struct pvr2_i2c_handler_functions hfuncs = {
	.detach = (void (*)(void *))decoder_detach,
	.check = (int (*)(void *))decoder_check,
	.update = (void (*)(void *))decoder_update,
	.describe = (unsigned int (*)(void *,char *,unsigned int))decoder_describe,
};


int pvr2_i2c_cx2584x_old_setup(struct pvr2_hdw *hdw,
			       struct pvr2_i2c_client *cp)
{
	struct pvr2_v4l_cx2584x *ctxt;

	if (hdw->decoder_ctrl) return 0;
	if (cp->handler) return 0;

	ctxt = kzalloc(sizeof(*ctxt),GFP_KERNEL);
	if (!ctxt) return 0;

	ctxt->handler.func_data = ctxt;
	ctxt->handler.func_table = &hfuncs;
	ctxt->ctrl.ctxt = ctxt;
	ctxt->ctrl.detach = (void (*)(void *))decoder_detach;
	ctxt->ctrl.enable = (void (*)(void *,int))decoder_enable;
	ctxt->ctrl.force_reset = (void (*)(void*))decoder_reset;
	ctxt->client = cp;
	ctxt->hdw = hdw;
	ctxt->stale_mask = (1 << ARRAY_SIZE(decoder_ops)) - 1;
	pvr2_hdw_set_decoder(hdw,&ctxt->ctrl);
	cp->handler = &ctxt->handler;
	vbi_hack(ctxt);
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c 0x%x cx2584x handler set up",
		   cp->client->addr);
	return !0;
}


#endif  /* PVR2_ENABLE_CX2584X_OLD */
#endif /* PVR2_ENABLE_OLD_I2COPS */


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
