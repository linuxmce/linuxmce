/*
 *
 *  $Id: pvrusb2-audio.c 2341 2009-10-11 23:19:51Z isely $
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

#include "pvrusb2-options.h"
#include "pvrusb2-audio.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include <linux/videodev2.h>
#ifdef PVR2_ENABLE_NEW_ROUTING
#include <media/msp3400.h>
#else
#include "msp3400.h"
#include <media/audiochip.h>
#endif
#ifdef PVR2_ENABLE_V4L2COMMON
#include <media/v4l2-common.h>
#endif
#include "compat.h"

#ifdef PVR2_ENABLE_NEW_ROUTING

struct routing_scheme {
	const int *def;
	unsigned int cnt;
};

static const int routing_scheme0[] = {
	[PVR2_CVAL_INPUT_TV]        = MSP_INPUT_DEFAULT,
	[PVR2_CVAL_INPUT_RADIO]     = MSP_INPUT(MSP_IN_SCART2,
						MSP_IN_TUNER1,
						MSP_DSP_IN_SCART,
						MSP_DSP_IN_SCART),
	[PVR2_CVAL_INPUT_COMPOSITE] = MSP_INPUT(MSP_IN_SCART1,
						MSP_IN_TUNER1,
						MSP_DSP_IN_SCART,
						MSP_DSP_IN_SCART),
	[PVR2_CVAL_INPUT_SVIDEO]    = MSP_INPUT(MSP_IN_SCART1,
						MSP_IN_TUNER1,
						MSP_DSP_IN_SCART,
						MSP_DSP_IN_SCART),
};

static const struct routing_scheme routing_def0 = {
	.def = routing_scheme0,
	.cnt = ARRAY_SIZE(routing_scheme0),
};

static const struct routing_scheme *routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = &routing_def0,
};
#endif

#ifdef PVR2_ENABLE_OLD_I2COPS

struct pvr2_msp3400_handler {
	struct pvr2_hdw *hdw;
	struct pvr2_i2c_client *client;
	struct pvr2_i2c_handler i2c_handler;
	unsigned long stale_mask;
};


#ifdef PVR2_ENABLE_NEW_ROUTING


/* This function selects the correct audio input source */
static void set_stereo(struct pvr2_msp3400_handler *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct v4l2_routing route;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c msp3400 v4l2 set_stereo");

	memset(&route,0,sizeof(route));

	sp = (sid < ARRAY_SIZE(routing_schemes)) ? routing_schemes[sid] : NULL;
	if ((sp != NULL) &&
	    (hdw->input_val >= 0) &&
	    (hdw->input_val < sp->cnt)) {
		route.input = sp->def[hdw->input_val];
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c msp3400 v4l2 set_stereo:"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}
	route.output = MSP_OUTPUT(MSP_SC_IN_DSP_SCART1);
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_S_AUDIO_ROUTING,&route);
}
#else

struct routing_scheme_item {
	int index;
	int input;
};

struct routing_scheme {
	const struct routing_scheme_item *def;
	unsigned int cnt;
};

static const struct routing_scheme_item routing_scheme0[] = {
	[PVR2_CVAL_INPUT_TV]        = {
		.index = AUDIO_TUNER,
		.input = SCART_IN1_DA,
	},
	[PVR2_CVAL_INPUT_RADIO]     = {
		.index = AUDIO_RADIO,
		.input = SCART_IN2,
	},
	[PVR2_CVAL_INPUT_COMPOSITE] = {
		.index = AUDIO_EXTERN_1,
		.input = SCART_IN1,
	},
	[PVR2_CVAL_INPUT_SVIDEO]    = {
		.index = AUDIO_EXTERN_1,
		.input = SCART_IN1,
	},
};

static const struct routing_scheme routing_def0 = {
	.def = routing_scheme0,
	.cnt = ARRAY_SIZE(routing_scheme0),
};

static const struct routing_scheme *routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = &routing_def0,
};


/* This function selects the correct audio input source */
static void set_stereo(struct pvr2_msp3400_handler *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct v4l2_audio ac;
	struct msp_matrix mspm;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c msp3400 v4l2 set_stereo");

	memset(&ac,0,sizeof(ac));
	mspm.output = 1;

	sp = (sid < ARRAY_SIZE(routing_schemes)) ? routing_schemes[sid] : NULL;
	if ((sp != NULL) &&
	    (hdw->input_val >= 0) &&
	    (hdw->input_val < sp->cnt)) {
		ac.index = sp->def[hdw->input_val].index;
		mspm.input = sp->def[hdw->input_val].input;
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c msp3400 v4l2 set_stereo"
			   " (old routing):"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}

	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_AUDIO,&ac);
	pvr2_i2c_client_cmd(ctxt->client,MSP_SET_MATRIX,&mspm);
}
#endif


static int check_stereo(struct pvr2_msp3400_handler *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->input_dirty;
}


struct pvr2_msp3400_ops {
	void (*update)(struct pvr2_msp3400_handler *);
	int (*check)(struct pvr2_msp3400_handler *);
};


static const struct pvr2_msp3400_ops msp3400_ops[] = {
	{ .update = set_stereo, .check = check_stereo},
};


static int msp3400_check(struct pvr2_msp3400_handler *ctxt)
{
	unsigned long msk;
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(msp3400_ops); idx++) {
		msk = 1 << idx;
		if (ctxt->stale_mask & msk) continue;
		if (msp3400_ops[idx].check(ctxt)) {
			ctxt->stale_mask |= msk;
		}
	}
	return ctxt->stale_mask != 0;
}


static void msp3400_update(struct pvr2_msp3400_handler *ctxt)
{
	unsigned long msk;
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(msp3400_ops); idx++) {
		msk = 1 << idx;
		if (!(ctxt->stale_mask & msk)) continue;
		ctxt->stale_mask &= ~msk;
		msp3400_ops[idx].update(ctxt);
	}
}


static void pvr2_msp3400_detach(struct pvr2_msp3400_handler *ctxt)
{
	ctxt->client->handler = NULL;
	kfree(ctxt);
}


static unsigned int pvr2_msp3400_describe(struct pvr2_msp3400_handler *ctxt,
					  char *buf,unsigned int cnt)
{
#ifdef PVR2_ENABLE_NEW_ROUTING
	return scnprintf(buf,cnt,"handler: pvrusb2-audio v4l2");
#else
	return scnprintf(buf,cnt,"handler: pvrusb2-audio (old routing) v4l2");
#endif
}


static const struct pvr2_i2c_handler_functions msp3400_funcs = {
	.detach = (void (*)(void *))pvr2_msp3400_detach,
	.check = (int (*)(void *))msp3400_check,
	.update = (void (*)(void *))msp3400_update,
	.describe = (unsigned int (*)(void *,char *,unsigned int))pvr2_msp3400_describe,
};


int pvr2_i2c_msp3400_setup(struct pvr2_hdw *hdw,struct pvr2_i2c_client *cp)
{
	struct pvr2_msp3400_handler *ctxt;
	if (cp->handler) return 0;

	ctxt = kzalloc(sizeof(*ctxt),GFP_KERNEL);
	if (!ctxt) return 0;

	ctxt->i2c_handler.func_data = ctxt;
	ctxt->i2c_handler.func_table = &msp3400_funcs;
	ctxt->client = cp;
	ctxt->hdw = hdw;
	ctxt->stale_mask = (1 << ARRAY_SIZE(msp3400_ops)) - 1;
	cp->handler = &ctxt->i2c_handler;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c 0x%x msp3400 V4L2 handler set up",
		   cp->client->addr);
	return !0;
}

#endif /* PVR2_ENABLE_OLD_I2COPS */
#ifdef PVR2_ENABLE_V4L2SUBDEV
void pvr2_msp3400_subdev_update(struct pvr2_hdw *hdw, struct v4l2_subdev *sd)
{
	if (hdw->input_dirty || hdw->force_dirty) {
		const struct routing_scheme *sp;
		unsigned int sid = hdw->hdw_desc->signal_routing_scheme;
		u32 input;

		pvr2_trace(PVR2_TRACE_CHIPS, "subdev msp3400 v4l2 set_stereo");
		sp = (sid < ARRAY_SIZE(routing_schemes)) ?
			routing_schemes[sid] : NULL;

		if ((sp != NULL) &&
		    (hdw->input_val >= 0) &&
		    (hdw->input_val < sp->cnt)) {
			input = sp->def[hdw->input_val];
		} else {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "*** WARNING *** subdev msp3400 set_input:"
				   " Invalid routing scheme (%u)"
				   " and/or input (%d)",
				   sid, hdw->input_val);
			return;
		}
#ifdef PVR2_ENABLE_V4L2SUBDEV_THRASH1
		sd->ops->audio->s_routing(sd, input,
			MSP_OUTPUT(MSP_SC_IN_DSP_SCART1), 0);
#else
		{
			struct v4l2_routing route;
			memset(&route,0,sizeof(route));
			route.input = input;
			route.output = MSP_OUTPUT(MSP_SC_IN_DSP_SCART1);
			sd->ops->audio->s_routing(sd, &route);
		}
#endif
	}
}
#endif /* PVR2_ENABLE_V4L2SUBDEV */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
