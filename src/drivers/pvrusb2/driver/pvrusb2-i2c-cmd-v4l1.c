/*
 *
 *  $Id: pvrusb2-i2c-cmd-v4l1.c 2226 2009-03-07 05:17:32Z isely $
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

#include "pvrusb2-i2c-cmd-v4l1.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include <linux/videodev.h>
#ifdef PVR2_ENABLE_OLDAUDC
#include <media/audiochip.h>
#else
#include <media/v4l2-common.h>
#endif

#ifdef PVR2_ENABLE_OLD_I2COPS


static void set_standard(struct pvr2_hdw *hdw)
{
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c v4l1 set_standard");
	if (hdw->input_val == PVR2_CVAL_INPUT_RADIO) {
		pvr2_i2c_core_cmd(hdw,AUDC_SET_RADIO,NULL);
	} else {

		int cvstd = hdw->std_mask_cur;
		struct video_channel vc;
		memset(&vc,0,sizeof(vc));

		if (cvstd & (V4L2_STD_NTSC|V4L2_STD_PAL_M|
#ifdef V4L2_STD_NTSC_443
			     V4L2_STD_NTSC_443|
#endif
			     V4L2_STD_PAL_60)) {
			vc.norm = VIDEO_MODE_NTSC;
		} else if (cvstd & V4L2_STD_SECAM) {
			vc.norm = VIDEO_MODE_PAL;
		} else if (cvstd & V4L2_STD_PAL) {
			vc.norm = VIDEO_MODE_PAL;
		} else {
			// Take a wild guess
			vc.norm = VIDEO_MODE_PAL;
		}
		pvr2_i2c_core_cmd(hdw,VIDIOCSCHAN,&vc);
	}
	hdw->tuner_signal_stale = !0;
	hdw->cropcap_stale = !0;
}


static int check_standard(struct pvr2_hdw *hdw)
{
	return (hdw->input_dirty != 0) || (hdw->std_dirty != 0);
}


const struct pvr2_i2c_op pvr2_i2c_op_v4l1_standard = {
	.check = check_standard,
	.update = set_standard,
	.name = "v4l1_standard",
};


static void set_volume(struct pvr2_hdw *hdw)
{
	struct video_audio vt;
	pvr2_trace(PVR2_TRACE_CHIPS,
		   "i2c v4l1 set_volume"
		   "(vol=%d bal=%d bas=%d treb=%d mute=%d)",
		   hdw->volume_val,
		   hdw->balance_val,
		   hdw->bass_val,
		   hdw->treble_val,
		   hdw->mute_val);
	memset(&vt,0,sizeof(vt));
	vt.flags = (VIDEO_AUDIO_VOLUME |
		    VIDEO_AUDIO_BALANCE |
		    VIDEO_AUDIO_BASS |
		    VIDEO_AUDIO_TREBLE |
		    VIDEO_AUDIO_MUTABLE);
	if (hdw->mute_val) {
		vt.flags |= VIDEO_AUDIO_MUTE;
	}
	vt.volume = hdw->volume_val;
	vt.balance = hdw->balance_val;
	vt.bass = hdw->bass_val;
	vt.treble = hdw->treble_val;

	pvr2_i2c_core_cmd(hdw,VIDIOCSAUDIO,&vt);
}


static int check_volume(struct pvr2_hdw *hdw)
{
	return (hdw->volume_dirty ||
		hdw->balance_dirty ||
		hdw->bass_dirty ||
		hdw->treble_dirty ||
		hdw->mute_dirty);
}


const struct pvr2_i2c_op pvr2_i2c_op_v4l1_volume = {
	.check = check_volume,
	.update = set_volume,
	.name = "v4l1_volume",
};


static void set_frequency(struct pvr2_hdw *hdw)
{
	unsigned long fv;
	unsigned long freq;
	fv = pvr2_hdw_get_cur_freq(hdw);
	pvr2_trace(PVR2_TRACE_CHIPS,"i2c v4l1 set_freq(%lu)",fv);
	freq = fv / 62500;
	pvr2_i2c_core_cmd(hdw,VIDIOCSFREQ,&freq);
}


static int check_frequency(struct pvr2_hdw *hdw)
{
	return hdw->freqDirty != 0;
}


const struct pvr2_i2c_op pvr2_i2c_op_v4l1_frequency = {
	.check = check_frequency,
	.update = set_frequency,
	.name = "v4l1_freq",
};


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
