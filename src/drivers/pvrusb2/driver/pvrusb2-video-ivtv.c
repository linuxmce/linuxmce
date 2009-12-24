/*
 *
 *  $Id: pvrusb2-video-ivtv.c 2226 2009-03-07 05:17:32Z isely $
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
   saa7115 support that is available in ivtv, which is entirely
   different than that which is available in v4l.

*/

#include "pvrusb2-video-ivtv.h"

#ifdef PVR2_ENABLE_OLD_I2COPS
#ifdef PVR2_ENABLE_SAA7115

#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "decoder.h"
#include "ivtv.h"
#include <linux/videodev.h>
#include <linux/video_decoder.h>

#define MODE_UNKNOWN 0
#define MODE_OLD 1
#define MODE_NEW 2

struct pvr2_ivtv_decoder {
	struct pvr2_i2c_handler handler;
	struct pvr2_decoder_ctrl ctrl;
	struct pvr2_i2c_client *client;
	struct pvr2_hdw *hdw;
	int mode;
	unsigned long stale_mask;
};


struct routing_scheme {
	const int *def;
	unsigned int cnt;
};

static const int routing_scheme0[] = {
	[PVR2_CVAL_INPUT_TV] = 4,
	/* In radio mode, we mute the video, but point at one
	   spot just to stay consistent */
	[PVR2_CVAL_INPUT_RADIO] = 5,
	[PVR2_CVAL_INPUT_COMPOSITE] = 5,
	[PVR2_CVAL_INPUT_SVIDEO] =  8,
};

static const struct routing_scheme routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = {
		.def = routing_scheme0,
		.cnt = ARRAY_SIZE(routing_scheme0),
	},
};


static void set_input(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	int v = 0;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c ivtv set_input(%d)",hdw->input_val);
	if ((sid < ARRAY_SIZE(routing_schemes)) &&
	    ((sp = routing_schemes + sid) != 0) &&
	    (hdw->input_val >= 0) &&
	    (hdw->input_val < sp->cnt)) {
		v = sp->def[hdw->input_val];
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c ivtv set_input:"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}

	pvr2_i2c_client_cmd(ctxt->client,DECODER_SET_INPUT,&v);
}


static int check_input(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->input_dirty != 0;
}


static void set_bcsh(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct decoder_state st;
	int ret;
	pvr2_trace(PVR2_TRACE_CHIPS,
		   "i2c ivtv set_bcsh b=%d c=%d s=%d h=%d",
		   hdw->brightness_val,hdw->contrast_val,
		   hdw->saturation_val,hdw->hue_val);
	st.bright = hdw->brightness_val;
	st.contrast = hdw->contrast_val;
	st.sat = hdw->saturation_val;
	st.hue = hdw->hue_val;
	ret = -EINVAL;
	/* isely@pobox.com 15-Oct-2005 - Really ugly kludge to attempt to
	   operate with both older and newer types of saa7115 modules. */
	if ((ctxt->mode == MODE_UNKNOWN) || (ctxt->mode == MODE_NEW)) {
		ret = pvr2_i2c_client_cmd(ctxt->client,DECODER_SET_STATE,&st);
		if (!ret) {
			if (ctxt->mode == MODE_UNKNOWN) {
				pvr2_trace(PVR2_TRACE_INFO,
					   "newer saa7115 sensed");
				ctxt->mode = MODE_NEW;
			}
		}
		if (!ret || (ret != -EINVAL)) return;
	}
	if ((ctxt->mode == MODE_UNKNOWN) || (ctxt->mode == MODE_OLD)) {
		ret = pvr2_i2c_client_cmd(ctxt->client,
					  DECODER_SET_PICTURE,&st);
		if (!ret) {
			if (ctxt->mode == MODE_UNKNOWN) {
				pvr2_trace(PVR2_TRACE_INFO,
					   "old saa7115 sensed");
				ctxt->mode = MODE_OLD;
			}
		}
		if (!ret || (ret != -EINVAL)) return;
	}
}


static int check_bcsh(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return (hdw->brightness_dirty || hdw->contrast_dirty ||
		hdw->saturation_dirty || hdw->hue_dirty);
}


static void set_std(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	int val;
	int cvstd = hdw->std_mask_cur;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c ivtv set_std %d",cvstd);

 	if (cvstd & (V4L2_STD_NTSC|V4L2_STD_PAL_M|
#ifdef V4L2_STD_NTSC_443
		     V4L2_STD_NTSC_443|
#endif
		     V4L2_STD_PAL_60)) {
		val = VIDEO_MODE_NTSC;
	} else if (cvstd & V4L2_STD_SECAM) {
		val = VIDEO_MODE_PAL;
	} else if (cvstd & V4L2_STD_PAL) {
		val = VIDEO_MODE_PAL;
	} else {
		// Take a wild guess
		val = VIDEO_MODE_PAL;
	}

	pvr2_i2c_client_cmd(ctxt->client,DECODER_SET_NORM,&val);
}


static int check_std(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->std_dirty != 0;
}


static void execute_query(struct pvr2_i2c_client *cp)
{
	int status;
	struct v4l2_tuner *vtp = &cp->hdw->tuner_signal_info;
	if (pvr2_i2c_client_cmd(cp,DECODER_GET_STATUS,&status) < 0) return;
	vtp->signal = (status & DECODER_STATUS_GOOD) ? 65535 : 0;
}


static void set_audio(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	int val;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c ivtv set_audio %d",hdw->srate_val);
#ifdef PVR2_ENABLE_CX2341XMOD
	switch (hdw->srate_val) {
	default:
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_48000:
		val = DECODER_AUDIO_48_KHZ;
		break;
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_44100:
		val = DECODER_AUDIO_441_KHZ;
		break;
	}
#else
	switch (hdw->srate_val) {
	default:
	case PVR2_CVAL_SRATE_48:
		val = DECODER_AUDIO_48_KHZ;
		break;
	case PVR2_CVAL_SRATE_44_1:
		val = DECODER_AUDIO_441_KHZ;
		break;
	}
#endif
	pvr2_i2c_client_cmd(ctxt->client,DECODER_SET_AUDIO,&val);
}


static int check_audio(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->srate_dirty != 0;
}


static void set_size(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct video_window wind;
	wind.width = hdw->res_hor_val;
	wind.height = hdw->res_ver_val;
	pvr2_trace(PVR2_TRACE_CHIPS,"pvr2_decoder_set_size(%dx%d)",
		   wind.width,wind.height);
	pvr2_i2c_client_cmd(ctxt->client,DECODER_SET_SIZE,&wind);
}


static int check_size(struct pvr2_ivtv_decoder *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return (hdw->res_hor_dirty || hdw->res_ver_dirty);
}


struct pvr2_ivtv_decoder_ops {
	void (*update)(struct pvr2_ivtv_decoder *);
	int (*check)(struct pvr2_ivtv_decoder *);
};


static const struct pvr2_ivtv_decoder_ops decoder_ops[] = {
	{ .update = set_bcsh, .check = check_bcsh},
	{ .update = set_input, .check = check_input},
	{ .update = set_std, .check = check_std},
	{ .update = set_size, .check = check_size},
	{ .update = set_audio, .check = check_audio},
};


static void decoder_detach(struct pvr2_ivtv_decoder *ctxt)
{
	ctxt->client->handler = NULL;
	ctxt->client->status_poll = NULL;
	pvr2_hdw_set_decoder(ctxt->hdw,NULL);
	kfree(ctxt);
}


static int decoder_check(struct pvr2_ivtv_decoder *ctxt)
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


static void decoder_update(struct pvr2_ivtv_decoder *ctxt)
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


static int decoder_detect(struct pvr2_i2c_client *cp)
{
	/* Attempt to query the decoder - let's see if it will answer */
	int status;
	int ret;

	ret = pvr2_i2c_client_cmd(cp,DECODER_GET_STATUS,&status);
	return ret == 0; /* Return true if it answered */
}


static void decoder_enable(struct pvr2_ivtv_decoder *ctxt,int fl)
{
	int val = fl ? 1 : 0;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c ivtv decoder_enable(%d)",val);
	pvr2_i2c_client_cmd(ctxt->client,DECODER_ENABLE_OUTPUT,&val);
}


static unsigned int decoder_describe(struct pvr2_ivtv_decoder *ctxt,char *buf,unsigned int cnt)
{
	return scnprintf(buf,cnt,"handler: pvrusb2-video-ivtv");
}


const static struct pvr2_i2c_handler_functions hfuncs = {
	.detach = (void (*)(void *))decoder_detach,
	.check = (int (*)(void *))decoder_check,
	.update = (void (*)(void *))decoder_update,
	.describe = (unsigned int (*)(void *,char *,unsigned int))decoder_describe,
};


int pvr2_i2c_decoder_ivtv_setup(struct pvr2_hdw *hdw,
				struct pvr2_i2c_client *cp)
{
	struct pvr2_ivtv_decoder *ctxt;

	if (hdw->decoder_ctrl) return 0;
	if (cp->handler) return 0;
	if (!decoder_detect(cp)) return 0;

	ctxt = kzalloc(sizeof(*ctxt),GFP_KERNEL);
	if (!ctxt) return 0;

	ctxt->handler.func_data = ctxt;
	ctxt->handler.func_table = &hfuncs;
	ctxt->ctrl.ctxt = ctxt;
	ctxt->ctrl.detach = (void (*)(void *))decoder_detach;
	ctxt->ctrl.enable = (void (*)(void *,int))decoder_enable;
	ctxt->client = cp;
	ctxt->hdw = hdw;
	ctxt->mode = MODE_UNKNOWN;
	ctxt->stale_mask = (1 << ARRAY_SIZE(decoder_ops)) - 1;
	pvr2_hdw_set_decoder(hdw,&ctxt->ctrl);
	cp->handler = &ctxt->handler;
	cp->status_poll = execute_query;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c 0x%x saa7115 V4L1 handler set up",
		   cp->client->addr);
	return !0;
}


#endif /* PVR2_ENABLE_SAA7115 */
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
