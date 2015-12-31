/*
 *
 *  $Id: pvrusb2-cx2584x-v4l.c 2309 2009-06-17 03:35:16Z isely $
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
   cx2584x, in kernels 2.6.16 or newer.

*/

#include "pvrusb2-cx2584x-v4l.h"
#include "pvrusb2-video-v4l.h"
#ifdef PVR2_ENABLE_OLD_I2COPS
#include "pvrusb2-i2c-cmd-v4l2.h"
#endif

#ifdef PVR2_ENABLE_CX2584X_V4L

#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#ifdef PVR2_ENABLE_NEW_ROUTING
#include <media/cx25840.h>
#endif
#include <linux/videodev2.h>
#ifdef PVR2_ENABLE_V4L2COMMON
#include <media/v4l2-common.h>
#endif
#include <linux/errno.h>
#include <linux/slab.h>
#include "compat.h"

#ifdef PVR2_ENABLE_NEW_ROUTING

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
		.vid = CX25840_COMPOSITE7,
		.aud = CX25840_AUDIO8,
	},
	[PVR2_CVAL_INPUT_RADIO] = { /* Treat the same as composite */
		.vid = CX25840_COMPOSITE3,
		.aud = CX25840_AUDIO_SERIAL,
	},
	[PVR2_CVAL_INPUT_COMPOSITE] = {
		.vid = CX25840_COMPOSITE3,
		.aud = CX25840_AUDIO_SERIAL,
	},
	[PVR2_CVAL_INPUT_SVIDEO] = {
		.vid = CX25840_SVIDEO1,
		.aud = CX25840_AUDIO_SERIAL,
	},
};

static const struct routing_scheme routing_def0 = {
	.def = routing_scheme0,
	.cnt = ARRAY_SIZE(routing_scheme0),
};

/* Specific to gotview device */
static const struct routing_scheme_item routing_schemegv[] = {
	[PVR2_CVAL_INPUT_TV] = {
		.vid = CX25840_COMPOSITE2,
		.aud = CX25840_AUDIO5,
	},
	[PVR2_CVAL_INPUT_RADIO] = {
		/* line-in is used for radio and composite.  A GPIO is
		   used to switch between the two choices. */
		.vid = CX25840_COMPOSITE1,
		.aud = CX25840_AUDIO_SERIAL,
	},
	[PVR2_CVAL_INPUT_COMPOSITE] = {
		.vid = CX25840_COMPOSITE1,
		.aud = CX25840_AUDIO_SERIAL,
	},
	[PVR2_CVAL_INPUT_SVIDEO] = {
		.vid = (CX25840_SVIDEO_LUMA3|CX25840_SVIDEO_CHROMA4),
		.aud = CX25840_AUDIO_SERIAL,
	},
};

static const struct routing_scheme routing_defgv = {
	.def = routing_schemegv,
	.cnt = ARRAY_SIZE(routing_schemegv),
};
#ifdef PVR2_ENABLE_DEVICE_AV400

/* Specific to grabster av400 device */
static const struct routing_scheme_item routing_schemeav400[] = {
	[PVR2_CVAL_INPUT_COMPOSITE] = {
		.vid = CX25840_COMPOSITE1,
		.aud = CX25840_AUDIO_SERIAL,
	},
	[PVR2_CVAL_INPUT_SVIDEO] = {
		.vid = (CX25840_SVIDEO_LUMA3|CX25840_SVIDEO_CHROMA4),
		.aud = CX25840_AUDIO_SERIAL,
	},
};

static const struct routing_scheme routing_defav400 = {
	.def = routing_schemeav400,
	.cnt = ARRAY_SIZE(routing_schemeav400),
};
#endif

static const struct routing_scheme *routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = &routing_def0,
	[PVR2_ROUTING_SCHEME_GOTVIEW] = &routing_defgv,
#ifdef PVR2_ENABLE_DEVICE_AV400
	[PVR2_ROUTING_SCHEME_AV400] = &routing_defav400,
#endif
};
#endif

#ifdef PVR2_ENABLE_OLD_I2COPS
struct pvr2_v4l_cx2584x {
	struct pvr2_i2c_handler handler;
	struct pvr2_decoder_ctrl ctrl;
	struct pvr2_i2c_client *client;
	struct pvr2_hdw *hdw;
	unsigned long stale_mask;
};

#ifdef PVR2_ENABLE_NEW_ROUTING

static void set_input(struct pvr2_v4l_cx2584x *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct v4l2_routing route;
	enum cx25840_video_input vid_input;
	enum cx25840_audio_input aud_input;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	memset(&route,0,sizeof(route));

	if ((sid < ARRAY_SIZE(routing_schemes)) &&
	    ((sp = routing_schemes[sid]) != NULL) &&
	    (hdw->input_val >= 0) &&
	    (hdw->input_val < sp->cnt)) {
		vid_input = sp->def[hdw->input_val].vid;
		aud_input = sp->def[hdw->input_val].aud;
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c cx2584x set_input:"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x set_input vid=0x%x aud=0x%x",
		   vid_input,aud_input);
	route.input = (u32)vid_input;
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_S_VIDEO_ROUTING,&route);
	route.input = (u32)aud_input;
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_S_AUDIO_ROUTING,&route);
}
#else

struct routing_scheme {
	const int *def;
	unsigned int cnt;
};

static const int routing_scheme0[] = {
	[PVR2_CVAL_INPUT_TV] = ((8 << 16) | 7),
	[PVR2_CVAL_INPUT_RADIO] = ((0 << 16) | 3),
	[PVR2_CVAL_INPUT_COMPOSITE] = ((0 << 16) | 3),
	[PVR2_CVAL_INPUT_SVIDEO] = ((0 << 16) | 0x510),
};

static const struct routing_scheme routing_def0 = {
	.def = routing_scheme0,
	.cnt = ARRAY_SIZE(routing_scheme0),
};

static const struct routing_scheme *routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = &routing_def0,
};

static void set_input(struct pvr2_v4l_cx2584x *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct v4l2_audio inp;
	int msk = 0;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;
	int v;

	sp = (sid < ARRAY_SIZE(routing_schemes)) ?
		routing_schemes[sid] : NULL;
	if ((sp == NULL) ||
	    (hdw->input_val < 0) ||
	    (hdw->input_val >= sp->cnt)) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c cx2584x set_input"
			   " (old routing):"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}
	msk = sp->def[hdw->input_val];
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x set_input(val=%d msk=0x%x)",
		   hdw->input_val,msk);
	v = msk & 0x0000ffffu;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x S_INPUT val=0x%x",v);
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_INPUT,&v);
	v = (msk >> 16) & 0x0000ffffu;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x S_AUDIO val=0x%x",v);
	memset(&inp,0,sizeof(inp));
	inp.index = v;
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_AUDIO,&inp);
}
#endif


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

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x set_audio %d",
		   hdw->srate_val);
#ifdef PVR2_ENABLE_CX2341XMOD
	switch (hdw->srate_val) {
	default:
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_48000:
		val = 48000;
		break;
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_44100:
		val = 44100;
		break;
	case V4L2_MPEG_AUDIO_SAMPLING_FREQ_32000:
		val = 32000;
		break;
	}
#else
	switch (hdw->srate_val) {
	default:
	case PVR2_CVAL_SRATE_48:
		val = 48000;
		break;
	case PVR2_CVAL_SRATE_44_1:
		val = 44100;
		break;
	case PVR2_CVAL_SRATE_32:
		val = 32000;
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


static int check_vbi_hack(struct pvr2_v4l_cx2584x *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return (hdw->input_dirty != 0) || (hdw->std_dirty != 0);
}


static void set_vbi_hack(struct pvr2_v4l_cx2584x *ctxt)
{
	v4l2_std_id vs;
	struct pvr2_hdw *hdw = ctxt->hdw;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx2584x set_vbi_hack");

	if (hdw->input_val == PVR2_CVAL_INPUT_RADIO) return;

	/* Set the standard here first, even though we do it
	   automatically later.  Why?  This way the VBI configuration
	   will work with the correct standard in place at the
	   moment. */
	vs = hdw->std_mask_cur;
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_STD,&vs);

	vbi_hack(ctxt);
}


struct pvr2_v4l_cx2584x_ops {
	void (*update)(struct pvr2_v4l_cx2584x *);
	int (*check)(struct pvr2_v4l_cx2584x *);
};


static const struct pvr2_v4l_cx2584x_ops decoder_ops[] = {
	{ .update = set_input, .check = check_input},
	{ .update = set_audio, .check = check_audio},
	{ .update = set_vbi_hack, .check = check_vbi_hack},
};


static void decoder_detach(struct pvr2_v4l_cx2584x *ctxt)
{
	ctxt->client->handler = NULL;
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


static int decoder_detect(struct pvr2_i2c_client *cp)
{
	int ret;
	/* Attempt to query the decoder - let's see if it will answer */
	struct v4l2_queryctrl qc;

	memset(&qc,0,sizeof(qc));

	qc.id = V4L2_CID_BRIGHTNESS;

	ret = pvr2_i2c_client_cmd(cp,VIDIOC_QUERYCTRL,&qc);
	return ret == 0; /* Return true if it answered */
}


static unsigned int decoder_describe(struct pvr2_v4l_cx2584x *ctxt,
				     char *buf,unsigned int cnt)
{
	return scnprintf(buf,cnt,"handler: pvrusb2-cx2584x-v4l");
}


static void decoder_reset(struct pvr2_v4l_cx2584x *ctxt)
{
	int ret;
	ret = pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_RESET,NULL);
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cx25840 decoder_reset (ret=%d)",ret);
	vbi_hack(ctxt);
}


static const struct pvr2_i2c_handler_functions hfuncs = {
	.detach = (void (*)(void *))decoder_detach,
	.check = (int (*)(void *))decoder_check,
	.update = (void (*)(void *))decoder_update,
	.describe = (unsigned int (*)(void *,char *,unsigned int))decoder_describe,
};


int pvr2_i2c_cx2584x_v4l_setup(struct pvr2_hdw *hdw,
			       struct pvr2_i2c_client *cp)
{
	struct pvr2_v4l_cx2584x *ctxt;

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
	ctxt->ctrl.force_reset = (void (*)(void*))decoder_reset;
	ctxt->client = cp;
	ctxt->hdw = hdw;
	ctxt->stale_mask = (1 << ARRAY_SIZE(decoder_ops)) - 1;
	pvr2_hdw_set_decoder(hdw,&ctxt->ctrl);
	cp->handler = &ctxt->handler;
	vbi_hack(ctxt);
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c 0x%x cx2584x V4L2 handler set up",
		   cp->client->addr);
	return !0;
}


#endif /* PVR2_ENABLE_OLD_I2COPS */
#ifdef PVR2_ENABLE_V4L2SUBDEV
void pvr2_cx25840_subdev_update(struct pvr2_hdw *hdw, struct v4l2_subdev *sd)
{
	pvr2_trace(PVR2_TRACE_CHIPS, "subdev cx2584x update...");
	if (hdw->input_dirty || hdw->force_dirty) {
		enum cx25840_video_input vid_input;
		enum cx25840_audio_input aud_input;
		const struct routing_scheme *sp;
		unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

		sp = (sid < ARRAY_SIZE(routing_schemes)) ?
			routing_schemes[sid] : NULL;
		if ((sp == NULL) ||
		    (hdw->input_val < 0) ||
		    (hdw->input_val >= sp->cnt)) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "*** WARNING *** subdev cx2584x set_input:"
				   " Invalid routing scheme (%u)"
				   " and/or input (%d)",
				   sid, hdw->input_val);
			return;
		}
		vid_input = sp->def[hdw->input_val].vid;
		aud_input = sp->def[hdw->input_val].aud;
		pvr2_trace(PVR2_TRACE_CHIPS,
			   "subdev cx2584x set_input vid=0x%x aud=0x%x",
			   vid_input, aud_input);
#ifdef PVR2_ENABLE_V4L2SUBDEV_THRASH1
		sd->ops->video->s_routing(sd, (u32)vid_input, 0, 0);
		sd->ops->audio->s_routing(sd, (u32)aud_input, 0, 0);
#else
		{
			struct v4l2_routing route;
			memset(&route,0,sizeof(route));
			route.input = (u32)vid_input;
			sd->ops->video->s_routing(sd, &route);
			route.input = (u32)aud_input;
			sd->ops->audio->s_routing(sd, &route);
		}
#endif
	}
}
#endif /* PVR2_ENABLE_V4L2SUBDEV */
#endif  /* PVR2_ENABLE_CX2584X_V4L */



/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
