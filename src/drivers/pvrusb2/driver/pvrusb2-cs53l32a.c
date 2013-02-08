/*
 *
 *  $Id: pvrusb2-cs53l32a.c 2309 2009-06-17 03:35:16Z isely $
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
   v4l-dvb cs53l32a module.

*/

#include "pvrusb2-cs53l32a.h"

#ifdef PVR2_ENABLE_CS53L32A
#ifdef PVR2_ENABLE_OLD_I2COPS
#include "pvrusb2-i2c-cmd-v4l2.h"
#endif

#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include <linux/videodev2.h>
#ifdef PVR2_ENABLE_V4L2COMMON
#include <media/v4l2-common.h>
#endif
#include <linux/errno.h>
#include <linux/slab.h>
#include "compat.h"

struct routing_scheme {
	const int *def;
	unsigned int cnt;
};


static const int routing_scheme1[] = {
	[PVR2_CVAL_INPUT_TV] = 2,  /* 1 or 2 seems to work here */
	[PVR2_CVAL_INPUT_RADIO] = 2,
	[PVR2_CVAL_INPUT_COMPOSITE] = 0,
	[PVR2_CVAL_INPUT_SVIDEO] =  0,
};

static const struct routing_scheme routing_def1 = {
	.def = routing_scheme1,
	.cnt = ARRAY_SIZE(routing_scheme1),
};

static const struct routing_scheme *routing_schemes[] = {
	[PVR2_ROUTING_SCHEME_ONAIR] = &routing_def1,
};


#ifdef PVR2_ENABLE_OLD_I2COPS
struct pvr2_cs53l32a {
	struct pvr2_i2c_handler handler;
	struct pvr2_i2c_client *client;
	struct pvr2_hdw *hdw;
	unsigned long stale_mask;
};


static void set_input(struct pvr2_cs53l32a *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	struct v4l2_routing route;
	const struct routing_scheme *sp;
	unsigned int sid = hdw->hdw_desc->signal_routing_scheme;

	pvr2_trace(PVR2_TRACE_CHIPS,"i2c cs53l32a set_input(%d)",
		   hdw->input_val);

	sp = (sid < ARRAY_SIZE(routing_schemes)) ?
		routing_schemes[sid] : NULL;
	if ((sp == NULL) ||
	    (hdw->input_val < 0) ||
	    (hdw->input_val >= sp->cnt)) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "*** WARNING *** i2c cs53l32a set_input:"
			   " Invalid routing scheme (%u) and/or input (%d)",
			   sid,hdw->input_val);
		return;
	}
	memset(&route,0,sizeof(route));
	route.input = sp->def[hdw->input_val];
	pvr2_i2c_client_cmd(ctxt->client,VIDIOC_INT_S_AUDIO_ROUTING,&route);
}


static int check_input(struct pvr2_cs53l32a *ctxt)
{
	struct pvr2_hdw *hdw = ctxt->hdw;
	return hdw->input_dirty != 0;
}


struct pvr2_cs53l32a_ops {
	void (*update)(struct pvr2_cs53l32a *);
	int (*check)(struct pvr2_cs53l32a *);
};


static const struct pvr2_cs53l32a_ops cs53l32a_ops[] = {
	{ .update = set_input, .check = check_input},
};


static void cs53l32a_detach(struct pvr2_cs53l32a *ctxt)
{
	ctxt->client->handler = NULL;
	kfree(ctxt);
}


static int cs53l32a_check(struct pvr2_cs53l32a *ctxt)
{
	unsigned long msk;
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(cs53l32a_ops); idx++) {
		msk = 1 << idx;
		if (ctxt->stale_mask & msk) continue;
		if (cs53l32a_ops[idx].check(ctxt)) {
			ctxt->stale_mask |= msk;
		}
	}
	return ctxt->stale_mask != 0;
}


static void cs53l32a_update(struct pvr2_cs53l32a *ctxt)
{
	unsigned long msk;
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(cs53l32a_ops); idx++) {
		msk = 1 << idx;
		if (!(ctxt->stale_mask & msk)) continue;
		ctxt->stale_mask &= ~msk;
		cs53l32a_ops[idx].update(ctxt);
	}
}


static unsigned int cs53l32a_describe(struct pvr2_cs53l32a *ctxt,char *buf,unsigned int cnt)
{
	return scnprintf(buf,cnt,"handler: pvrusb2-cs53l32a");
}


static const struct pvr2_i2c_handler_functions hfuncs = {
	.detach = (void (*)(void *))cs53l32a_detach,
	.check = (int (*)(void *))cs53l32a_check,
	.update = (void (*)(void *))cs53l32a_update,
	.describe = (unsigned int (*)(void *,char *,unsigned int))cs53l32a_describe,
};


int pvr2_i2c_cs53l32a_setup(struct pvr2_hdw *hdw,struct pvr2_i2c_client *cp)
{
	struct pvr2_cs53l32a *ctxt;

	if (cp->handler) return 0;

	ctxt = kzalloc(sizeof(*ctxt),GFP_KERNEL);
	if (!ctxt) return 0;

	ctxt->handler.func_data = ctxt;
	ctxt->handler.func_table = &hfuncs;
	ctxt->client = cp;
	ctxt->hdw = hdw;
	ctxt->stale_mask = (1 << ARRAY_SIZE(cs53l32a_ops)) - 1;
	cp->handler = &ctxt->handler;
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c 0x%x cs53l32a V4L2 handler set up",
		   cp->client->addr);
	return !0;
}


#endif /* PVR2_ENABLE_OLD_I2COPS */
#ifdef PVR2_ENABLE_V4L2SUBDEV
void pvr2_cs53l32a_subdev_update(struct pvr2_hdw *hdw, struct v4l2_subdev *sd)
{
	if (hdw->input_dirty || hdw->force_dirty) {
		const struct routing_scheme *sp;
		unsigned int sid = hdw->hdw_desc->signal_routing_scheme;
		u32 input;
		pvr2_trace(PVR2_TRACE_CHIPS, "subdev v4l2 set_input(%d)",
			   hdw->input_val);
		sp = (sid < ARRAY_SIZE(routing_schemes)) ?
			routing_schemes[sid] : NULL;
		if ((sp == NULL) ||
		    (hdw->input_val < 0) ||
		    (hdw->input_val >= sp->cnt)) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "*** WARNING *** subdev v4l2 set_input:"
				   " Invalid routing scheme (%u)"
				   " and/or input (%d)",
				   sid, hdw->input_val);
			return;
		}
		input = sp->def[hdw->input_val];
#ifdef PVR2_ENABLE_V4L2SUBDEV_THRASH1
		sd->ops->audio->s_routing(sd, input, 0, 0);
#else
		{
			struct v4l2_routing route;
			memset(&route,0,sizeof(route));
			route.input = input;
			sd->ops->audio->s_routing(sd, &route);
		}
#endif
	}
}
#endif /* PVR2_ENABLE_V4L2SUBDEV */
#endif /* PVR2_ENABLE_CS53L32A */


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
