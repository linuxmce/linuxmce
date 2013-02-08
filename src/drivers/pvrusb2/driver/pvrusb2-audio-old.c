/*
 *
 *  $Id: pvrusb2-audio-old.c 2226 2009-03-07 05:17:32Z isely $
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
#include "compat.h"

#ifdef PVR2_ENABLE_OLD_I2COPS
#ifdef ENABLE_PVRUSB2_AUDIO_OLD

#include "pvrusb2-audio-old.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "msp3400.h"
#include <linux/videodev.h>
#include <media/audiochip.h>
#ifdef PVR2_ENABLE_V4L2COMMON
#include <media/v4l2-common.h>
#endif

struct pvr2_msp3400_handler {
	struct pvr2_hdw *hdw;
	struct pvr2_i2c_client *client;
	struct pvr2_i2c_handler i2c_handler;
	unsigned long stale_mask;
};


/*

  MCI <isely@pobox.com> 10-Mar-2005 - Rather than operate the msp34xx
  directly, we rely on the msp3400.ko module to do it for us.  We
  really have to do this because that $##@!! module is going to attach
  itself to us anyway, so we really can't operate the chip ourselves.
  Unfortunately msp3400.ko is a real train wreck of a piece of code.
  Most of the code below tries to tickle that module in just the right
  way to get the results we need.  Yuck.  msp3400.c should be taken
  out back and shot.  Based on my reading of the actual chip datasheet
  it should in theory be possible to write a far cleaner and simpler
  driver than what is currently there right now.

*/

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
		.index = AUDIO_EXTERN,
		.input = SCART_IN1,
	},
	[PVR2_CVAL_INPUT_SVIDEO]    = {
		.index = AUDIO_EXTERN,
		.input = SCART_IN1,
	},
};

static const struct routing_scheme routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_HAUPPAUGE] = {
		.def = routing_scheme0,
		.cnt = ARRAY_SIZE(routing_scheme0),
	},
};


/* This function selects the correct audio input source */
static void set_stereo(struct pvr2_msp3400_handler *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	unsigned short sarg = 0;
	struct msp_matrix mspm;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c msp3400 set_stereo");

	if ((sid < ARRAY_SIZE(routing_schemes)) &&
	    ((sp = routing_schemes + sid) != 0) &&
	    (hdw->input_val >= 0) &&
	    (hdw->input_val < sp->cnt)) {
		sarg = sp->def[hdw->input_val].index;
		mspm.input = sp->def[hdw->input_val].input;
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c msp3400 set_stereo (old):"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}

	if (hdw->input_val == PVR2_CVAL_INPUT_TV) {
		struct v4l2_tuner vt;
		memset(&vt,0,sizeof(vt));
		vt.audmode = hdw->audiomode_val;
		pvr2_i2c_client_cmd(ctxt->client,VIDIOC_S_TUNER,&vt);
	}

	pvr2_i2c_client_cmd(ctxt->client,AUDC_SET_INPUT,&sarg);

	/* The above should have been enough to do the job, however
	   msp3400.ko does an incomplete job of handling the scart
	   routing.  Really.  It doesn't even bother to initialize the
	   SC1 output at all.  So we have to help it here...
	   Unfortunately this also means that we have include
	   msp3400.c's header file and it currently isn't in a public
	   place.  Damnit! */

	mspm.output = 1;
	pvr2_i2c_client_cmd(ctxt->client,MSP_SET_MATRIX,&mspm);
}


static int check_stereo(struct pvr2_msp3400_handler *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return (hdw->input_dirty ||
		hdw->audiomode_dirty);
}


/* This reads back the current volume parameters and signal type */
static void execute_query(struct pvr2_i2c_client *cp)
{
	struct video_audio vt;
	unsigned int msk = 0;

	memset(&vt,0,sizeof(vt));
	if (pvr2_i2c_client_cmd(cp,VIDIOCGAUDIO,&vt) < 0) return;

	if (vt.mode & VIDEO_SOUND_STEREO) {
		msk |= V4L2_TUNER_SUB_STEREO;
	}
	if (vt.mode & VIDEO_SOUND_MONO) {
		msk |= V4L2_TUNER_SUB_MONO;
	}
	if (vt.mode & VIDEO_SOUND_LANG1) {
		msk |= V4L2_TUNER_SUB_LANG1;
	}
	if (vt.mode & VIDEO_SOUND_LANG2) {
		msk |= V4L2_TUNER_SUB_LANG2;
	}
	cp->hdw->tuner_signal_info.rxsubchans = msk;
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
	ctxt->client->status_poll = NULL;
	kfree(ctxt);
}


static unsigned int pvr2_msp3400_describe(struct pvr2_msp3400_handler *ctxt,
					  char *buf,unsigned int cnt)
{
	return scnprintf(buf,cnt,"handler: pvrusb2-audio v4l1");
}


const static struct pvr2_i2c_handler_functions msp3400_funcs = {
	.detach = (void (*)(void *))pvr2_msp3400_detach,
	.check = (int (*)(void *))msp3400_check,
	.update = (void (*)(void *))msp3400_update,
	.describe = (unsigned int (*)(void *,char *,unsigned int))pvr2_msp3400_describe,
};


int pvr2_i2c_msp3400_old_setup(struct pvr2_hdw *hdw,struct pvr2_i2c_client *cp)
{
	struct v4l2_queryctrl qc;
	struct pvr2_msp3400_handler *ctxt;
	int stat;
	if (cp->handler) return 0;

	/* Figure out if this is an old msp3400.  This is ugly.  What
	   we need to do here is issue a command to it and examine the
	   answer.  Unfortunately we can't test for an unimplemented
	   command because old msp3400 module return success on
	   unrecognized commands.  Instead we'll issue a command that
	   requires data to be returned and see if the module actually
	   changed anything.  We're going to fetch the current volume
	   and see if we get back a name for it. */
	memset(&qc,0,sizeof(qc));
	qc.id = V4L2_CID_AUDIO_VOLUME;
	qc.name[0] = 0;
	stat = pvr2_i2c_client_cmd(cp,VIDIOC_QUERYCTRL,&qc);
	if (qc.name[0] != 0) return 0;  // It's newer than what we want.

	ctxt = kzalloc(sizeof(*ctxt),GFP_KERNEL);
	if (!ctxt) return 0;

	ctxt->i2c_handler.func_data = ctxt;
	ctxt->i2c_handler.func_table = &msp3400_funcs;
	ctxt->client = cp;
	ctxt->hdw = hdw;
	ctxt->stale_mask = (1 << ARRAY_SIZE(msp3400_ops)) - 1;
	cp->handler = &ctxt->i2c_handler;
	cp->status_poll = execute_query;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c 0x%x msp3400 V4L1 handler set up",
		   cp->client->addr);
	return !0;
}

#endif
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
