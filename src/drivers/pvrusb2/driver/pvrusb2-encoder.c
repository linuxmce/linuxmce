/*
 *
 *  $Id: pvrusb2-encoder.c 2333 2009-10-11 21:07:35Z isely $
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
#include <linux/device.h>   // for linux/firmware.h
#include <linux/firmware.h>
#include "pvrusb2-util.h"
#include "pvrusb2-encoder.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "pvrusb2-fx2-cmd.h"
#include "compat.h"

#ifndef PVR2_ENABLE_CX2341XMOD
static u32 pvr_tbl_emphasis [] = {
	[PVR2_CVAL_AUDIOEMPHASIS_NONE] = 0x0,
	[PVR2_CVAL_AUDIOEMPHASIS_50_15] = 0x1,
	[PVR2_CVAL_AUDIOEMPHASIS_CCITT] = 0x3,
};

static u32 pvr_tbl_srate[] = {
	[PVR2_CVAL_SRATE_48] =  0x01,
	[PVR2_CVAL_SRATE_44_1] = 0x00,
	[PVR2_CVAL_SRATE_32] =  0x02,
};

static u32 pvr_tbl_audiobitrate[] = {
	[PVR2_CVAL_AUDIOBITRATE_384] = 0xe,
	[PVR2_CVAL_AUDIOBITRATE_320] = 0xd,
	[PVR2_CVAL_AUDIOBITRATE_256] = 0xc,
	[PVR2_CVAL_AUDIOBITRATE_224] = 0xb,
	[PVR2_CVAL_AUDIOBITRATE_192] = 0xa,
	[PVR2_CVAL_AUDIOBITRATE_160] = 0x9,
	[PVR2_CVAL_AUDIOBITRATE_128] = 0x8,
	[PVR2_CVAL_AUDIOBITRATE_112] = 0x7,
	[PVR2_CVAL_AUDIOBITRATE_96]  = 0x6,
	[PVR2_CVAL_AUDIOBITRATE_80]  = 0x5,
	[PVR2_CVAL_AUDIOBITRATE_64]  = 0x4,
	[PVR2_CVAL_AUDIOBITRATE_56]  = 0x3,
	[PVR2_CVAL_AUDIOBITRATE_48]  = 0x2,
	[PVR2_CVAL_AUDIOBITRATE_32]  = 0x1,
	[PVR2_CVAL_AUDIOBITRATE_VBR] = 0x0,
};

static u32 pvr_tbl_audiomodeextension[] = {
	[PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_4] = 0,
	[PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_8] = 1,
	[PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_12] = 2,
	[PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_16] = 3,
};

static u32 pvr_tbl_mpegaudiomode[] = {
	[PVR2_CVAL_MPEGAUDIOMODE_STEREO] = 0,
	[PVR2_CVAL_MPEGAUDIOMODE_JOINT_STEREO] = 1,
	[PVR2_CVAL_MPEGAUDIOMODE_DUAL] = 2,
	[PVR2_CVAL_MPEGAUDIOMODE_MONO] = 3,
};

static u32 pvr_tbl_streamtype[] = {
	[PVR2_CVAL_STREAMTYPE_PS] = 0,
	[PVR2_CVAL_STREAMTYPE_TS] = 1,
	[PVR2_CVAL_STREAMTYPE_SS] = 2,
	[PVR2_CVAL_STREAMTYPE_DVD] = 10,
	[PVR2_CVAL_STREAMTYPE_VCD] = 11,
	[PVR2_CVAL_STREAMTYPE_SVCD] = 12,
	[PVR2_CVAL_STREAMTYPE_DVDS1] = 13,
	[PVR2_CVAL_STREAMTYPE_DVDS2] = 14,
	[PVR2_CVAL_STREAMTYPE_PESAV] = 3,
	[PVR2_CVAL_STREAMTYPE_PESV] = 5,
	[PVR2_CVAL_STREAMTYPE_PESA] = 7,
};

static u32 pvr_tbl_aspect[] = {
	[PVR2_CVAL_ASPECT_1X1] = 1,
	[PVR2_CVAL_ASPECT_4X3] = 2,
	[PVR2_CVAL_ASPECT_16X9] = 3,
	[PVR2_CVAL_ASPECT_221X1] = 4,
};
#endif


/* Firmware mailbox flags - definitions found from ivtv */
#define IVTV_MBOX_FIRMWARE_DONE 0x00000004
#define IVTV_MBOX_DRIVER_DONE 0x00000002
#define IVTV_MBOX_DRIVER_BUSY 0x00000001

#define MBOX_BASE 0x44


static int pvr2_encoder_write_words(struct pvr2_hdw *hdw,
				    unsigned int offs,
				    const u32 *data, unsigned int dlen)
{
	unsigned int idx,addr;
	unsigned int bAddr;
	int ret;
	unsigned int chunkCnt;

	/*

	Format: First byte must be 0x01.  Remaining 32 bit words are
	spread out into chunks of 7 bytes each, with the first 4 bytes
	being the data word (little endian), and the next 3 bytes
	being the address where that data word is to be written (big
	endian).  Repeat request for additional words, with offset
	adjusted accordingly.

	*/
	while (dlen) {
		chunkCnt = 8;
		if (chunkCnt > dlen) chunkCnt = dlen;
		memset(hdw->cmd_buffer,0,sizeof(hdw->cmd_buffer));
		bAddr = 0;
		hdw->cmd_buffer[bAddr++] = FX2CMD_MEM_WRITE_DWORD;
		for (idx = 0; idx < chunkCnt; idx++) {
			addr = idx + offs;
			hdw->cmd_buffer[bAddr+6] = (addr & 0xffu);
			hdw->cmd_buffer[bAddr+5] = ((addr>>8) & 0xffu);
			hdw->cmd_buffer[bAddr+4] = ((addr>>16) & 0xffu);
			PVR2_DECOMPOSE_LE(hdw->cmd_buffer, bAddr,data[idx]);
			bAddr += 7;
		}
		ret = pvr2_send_request(hdw,
					hdw->cmd_buffer,1+(chunkCnt*7),
					NULL,0);
		if (ret) return ret;
		data += chunkCnt;
		dlen -= chunkCnt;
		offs += chunkCnt;
	}

	return 0;
}


static int pvr2_encoder_read_words(struct pvr2_hdw *hdw,
				   unsigned int offs,
				   u32 *data, unsigned int dlen)
{
	unsigned int idx;
	int ret;
	unsigned int chunkCnt;

	/*

	Format: First byte must be 0x02 (status check) or 0x28 (read
	back block of 32 bit words).  Next 6 bytes must be zero,
	followed by a single byte of MBOX_BASE+offset for portion to
	be read.  Returned data is packed set of 32 bits words that
	were read.

	*/

	while (dlen) {
		chunkCnt = 16;
		if (chunkCnt > dlen) chunkCnt = dlen;
		if (chunkCnt < 16) chunkCnt = 1;
		hdw->cmd_buffer[0] =
			((chunkCnt == 1) ?
			 FX2CMD_MEM_READ_DWORD : FX2CMD_MEM_READ_64BYTES);
		hdw->cmd_buffer[1] = 0;
		hdw->cmd_buffer[2] = 0;
		hdw->cmd_buffer[3] = 0;
		hdw->cmd_buffer[4] = 0;
		hdw->cmd_buffer[5] = ((offs>>16) & 0xffu);
		hdw->cmd_buffer[6] = ((offs>>8) & 0xffu);
		hdw->cmd_buffer[7] = (offs & 0xffu);
		ret = pvr2_send_request(hdw,
					hdw->cmd_buffer,8,
					hdw->cmd_buffer,
					(chunkCnt == 1 ? 4 : 16 * 4));
		if (ret) return ret;

		for (idx = 0; idx < chunkCnt; idx++) {
			data[idx] = PVR2_COMPOSE_LE(hdw->cmd_buffer,idx*4);
		}
		data += chunkCnt;
		dlen -= chunkCnt;
		offs += chunkCnt;
	}

	return 0;
}


/* This prototype is set up to be compatible with the
   cx2341x_mbox_func prototype in cx2341x.h, which should be in
   kernels 2.6.18 or later.  We do this so that we can enable
   cx2341x.ko to write to our encoder (by handing it a pointer to this
   function).  For earlier kernels this doesn't really matter. */
static int pvr2_encoder_cmd(void *ctxt,
#ifdef PVR2_ENABLE_CX2341XOLD2
			    int cmd,
#else
			    u32 cmd,
#endif
			    int arg_cnt_send,
			    int arg_cnt_recv,
			    u32 *argp)
{
	unsigned int poll_count;
	unsigned int try_count = 0;
	int retry_flag;
	int ret = 0;
	unsigned int idx;
	/* These sizes look to be limited by the FX2 firmware implementation */
	u32 wrData[16];
	u32 rdData[16];
	struct pvr2_hdw *hdw = (struct pvr2_hdw *)ctxt;

#if 0
	{
		char buf[150];
		unsigned int ccnt,bcnt;

		bcnt = scnprintf(buf,sizeof(buf),"Encoder Cmd %02x (",cmd);
		for (idx = 0; idx < arg_cnt_send; idx++) {
			if (idx) {
				ccnt = scnprintf(buf+bcnt,
						 sizeof(buf)-bcnt," ");
				bcnt += ccnt;
			}
			ccnt = scnprintf(buf+bcnt,sizeof(buf)-bcnt,
					 "0x%x",argp[idx]);
			bcnt += ccnt;
		}
		ccnt = scnprintf(buf+bcnt,sizeof(buf)-bcnt,")");
		bcnt += ccnt;
		pvr2_trace(PVR2_TRACE_ENCODER,"%.*s",bcnt,buf);
	}
#endif

	/*

	The encoder seems to speak entirely using blocks 32 bit words.
	In ivtv driver terms, this is a mailbox at MBOX_BASE which we
	populate with data and watch what the hardware does with it.
	The first word is a set of flags used to control the
	transaction, the second word is the command to execute, the
	third byte is zero (ivtv driver suggests that this is some
	kind of return value), and the fourth byte is a specified
	timeout (windows driver always uses 0x00060000 except for one
	case when it is zero).  All successive words are the argument
	words for the command.

	First, write out the entire set of words, with the first word
	being zero.

	Next, write out just the first word again, but set it to
	IVTV_MBOX_DRIVER_DONE | IVTV_DRIVER_BUSY this time (which
	probably means "go").

	Next, read back the return count words.  Check the first word,
	which should have IVTV_MBOX_FIRMWARE_DONE set.  If however
	that bit is not set, then the command isn't done so repeat the
	read until it is set.

	Finally, write out just the first word again, but set it to
	0x0 this time (which probably means "idle").

	*/

	if (arg_cnt_send > (ARRAY_SIZE(wrData) - 4)) {
		pvr2_trace(
			PVR2_TRACE_ERROR_LEGS,
			"Failed to write cx23416 command"
			" - too many input arguments"
			" (was given %u limit %lu)",
			arg_cnt_send, (long unsigned) ARRAY_SIZE(wrData) - 4);
		return -EINVAL;
	}

	if (arg_cnt_recv > (ARRAY_SIZE(rdData) - 4)) {
		pvr2_trace(
			PVR2_TRACE_ERROR_LEGS,
			"Failed to write cx23416 command"
			" - too many return arguments"
			" (was given %u limit %lu)",
			arg_cnt_recv, (long unsigned) ARRAY_SIZE(rdData) - 4);
		return -EINVAL;
	}


	LOCK_TAKE(hdw->ctl_lock); do {

		if (!hdw->state_encoder_ok) {
			ret = -EIO;
			break;
		}

		retry_flag = 0;
		try_count++;
		ret = 0;
		wrData[0] = 0;
		wrData[1] = cmd;
		wrData[2] = 0;
		wrData[3] = 0x00060000;
		for (idx = 0; idx < arg_cnt_send; idx++) {
			wrData[idx+4] = argp[idx];
		}
		for (; idx < ARRAY_SIZE(wrData) - 4; idx++) {
			wrData[idx+4] = 0;
		}

		ret = pvr2_encoder_write_words(hdw,MBOX_BASE,wrData,idx);
		if (ret) break;
		wrData[0] = IVTV_MBOX_DRIVER_DONE|IVTV_MBOX_DRIVER_BUSY;
		ret = pvr2_encoder_write_words(hdw,MBOX_BASE,wrData,1);
		if (ret) break;
		poll_count = 0;
		while (1) {
			poll_count++;
			ret = pvr2_encoder_read_words(hdw,MBOX_BASE,rdData,
						      arg_cnt_recv+4);
			if (ret) {
				break;
			}
			if (rdData[0] & IVTV_MBOX_FIRMWARE_DONE) {
				break;
			}
			if (rdData[0] && (poll_count < 1000)) continue;
			if (!rdData[0]) {
				retry_flag = !0;
				pvr2_trace(
					PVR2_TRACE_ERROR_LEGS,
					"Encoder timed out waiting for us"
					"; arranging to retry");
			} else {
				pvr2_trace(
					PVR2_TRACE_ERROR_LEGS,
					"***WARNING*** device's encoder"
					" appears to be stuck"
					" (status=0x%08x)",rdData[0]);
			}
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"Encoder command: 0x%02x",cmd);
			for (idx = 4; idx < arg_cnt_send; idx++) {
				pvr2_trace(
					PVR2_TRACE_ERROR_LEGS,
					"Encoder arg%d: 0x%08x",
					idx-3,wrData[idx]);
			}
			ret = -EBUSY;
			break;
		}
		if (retry_flag) {
			if (try_count < 20) continue;
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"Too many retries...");
			ret = -EBUSY;
		}
		if (ret) {
			del_timer_sync(&hdw->encoder_run_timer);
			hdw->state_encoder_ok = 0;
			pvr2_trace(PVR2_TRACE_STBITS,
				   "State bit %s <-- %s",
				   "state_encoder_ok",
				   (hdw->state_encoder_ok ? "true" : "false"));
			if (hdw->state_encoder_runok) {
				hdw->state_encoder_runok = 0;
				pvr2_trace(PVR2_TRACE_STBITS,
				   "State bit %s <-- %s",
					   "state_encoder_runok",
					   (hdw->state_encoder_runok ?
					    "true" : "false"));
			}
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"Giving up on command."
				"  This is normally recovered via a firmware"
				" reload and re-initialization; concern"
				" is only warranted if this happens repeatedly"
				" and rapidly.");
			break;
		}
		wrData[0] = 0x7;
#if 0
		for (idx = 0; idx < args; idx++) {
			if (rdData[idx] != wrData[idx]) {
				pvr2_trace(
					PVR2_TRACE_DEBUG,
					"pvr2_encoder idx %02x mismatch exp:"
					" %08x got: %08x",
					idx,wrData[idx],rdData[idx]);
			}
		}
#endif
		for (idx = 0; idx < arg_cnt_recv; idx++) {
			argp[idx] = rdData[idx+4];
		}

		wrData[0] = 0x0;
		ret = pvr2_encoder_write_words(hdw,MBOX_BASE,wrData,1);
		if (ret) break;

	} while(0); LOCK_GIVE(hdw->ctl_lock);

	return ret;
}


static int pvr2_encoder_vcmd(struct pvr2_hdw *hdw, int cmd,
			     int args, ...)
{
	va_list vl;
	unsigned int idx;
	u32 data[12];

	if (args > ARRAY_SIZE(data)) {
		pvr2_trace(
			PVR2_TRACE_ERROR_LEGS,
			"Failed to write cx23416 command"
			" - too many arguments"
			" (was given %u limit %lu)",
			args, (long unsigned) ARRAY_SIZE(data));
		return -EINVAL;
	}

	va_start(vl, args);
	for (idx = 0; idx < args; idx++) {
		data[idx] = va_arg(vl, u32);
	}
	va_end(vl);

	return pvr2_encoder_cmd(hdw,cmd,args,0,data);
}


/* This implements some extra setup for the encoder that seems to be
   specific to the PVR USB2 hardware. */
static int pvr2_encoder_prep_config(struct pvr2_hdw *hdw)
{
	int ret = 0;
	int encMisc3Arg = 0;

#if 0 /* keep */
	/* This inexplicable bit happens in the Hauppauge windows
	   driver (for both 24xxx and 29xxx devices).  However I
	   currently see no difference in behavior with or without
	   this stuff.  Leave this here as a note of its existence,
	   but don't use it. */
	LOCK_TAKE(hdw->ctl_lock); do {
		u32 dat[1];
		dat[0] = 0x80000640;
		pvr2_encoder_write_words(hdw,0x01fe,dat,1);
		pvr2_encoder_write_words(hdw,0x023e,dat,1);
	} while(0); LOCK_GIVE(hdw->ctl_lock);
#endif

	/* Mike Isely <isely@pobox.com> 26-Jan-2006 The windows driver
	   sends the following list of ENC_MISC commands (for both
	   24xxx and 29xxx devices).  Meanings are not entirely clear,
	   however without the ENC_MISC(3,1) command then we risk
	   random perpetual video corruption whenever the video input
	   breaks up for a moment (like when switching channels). */


#if 0 /* keep */
	/* This ENC_MISC(5,0) command seems to hurt 29xxx sync
	   performance on channel changes, but is not a problem on
	   24xxx devices. */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC,4, 5,0,0,0);
#endif

	/* This ENC_MISC(3,encMisc3Arg) command is critical - without
	   it there will eventually be video corruption.  Also, the
	   saa7115 case is strange - the Windows driver is passing 1
	   regardless of device type but if we have 1 for saa7115
	   devices the video turns sluggish.  */
	if (hdw->hdw_desc->flag_has_cx25840) {
		encMisc3Arg = 1;
	} else {
		encMisc3Arg = 0;
	}
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC,4, 3,
				 encMisc3Arg,0,0);

	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC,4, 8,0,0,0);

#if 0 /* keep */
	/* This ENC_MISC(4,1) command is poisonous, so it is commented
	   out.  But I'm leaving it here anyway to document its
	   existence in the Windows driver.  The effect of this
	   command is that apps displaying the stream become sluggish
	   with stuttering video. */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC,4, 4,1,0,0);
#endif

	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC,4, 0,3,0,0);
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC,4,15,0,0,0);

	/* prevent the PTSs from slowly drifting away in the generated
	   MPEG stream */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_MISC, 2, 4, 1);

	return ret;
}

#ifdef PVR2_ENABLE_CX2341XMOD
int pvr2_encoder_adjust(struct pvr2_hdw *hdw)
{
	int ret;
	ret = cx2341x_update(hdw,pvr2_encoder_cmd,
			     (hdw->enc_cur_valid ? &hdw->enc_cur_state : NULL),
			     &hdw->enc_ctl_state);
	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Error from cx2341x module code=%d",ret);
	} else {
		memcpy(&hdw->enc_cur_state,&hdw->enc_ctl_state,
		       sizeof(struct cx2341x_mpeg_params));
		hdw->enc_cur_valid = !0;
	}
	return ret;
}


int pvr2_encoder_configure(struct pvr2_hdw *hdw)
{
	int ret;
	int val;
	pvr2_trace(PVR2_TRACE_ENCODER,"pvr2_encoder_configure"
		   " (cx2341x module)");
	hdw->enc_ctl_state.port = CX2341X_PORT_STREAMING;
	hdw->enc_ctl_state.width = hdw->res_hor_val;
	hdw->enc_ctl_state.height = hdw->res_ver_val;
	hdw->enc_ctl_state.is_50hz = ((hdw->std_mask_cur & V4L2_STD_525_60) ?
				      0 : 1);

	ret = 0;

	ret |= pvr2_encoder_prep_config(hdw);

	/* saa7115: 0xf0 */
	val = 0xf0;
	if (hdw->hdw_desc->flag_has_cx25840) {
		/* ivtv cx25840: 0x140 */
		val = 0x140;
	}

	if (!ret) ret = pvr2_encoder_vcmd(
		hdw,CX2341X_ENC_SET_NUM_VSYNC_LINES, 2,
		val, val);

	/* setup firmware to notify us about some events (don't know why...) */
	if (!ret) ret = pvr2_encoder_vcmd(
		hdw,CX2341X_ENC_SET_EVENT_NOTIFICATION, 4,
		0, 0, 0x10000000, 0xffffffff);

	if (!ret) ret = pvr2_encoder_vcmd(
		hdw,CX2341X_ENC_SET_VBI_LINE, 5,
		0xffffffff,0,0,0,0);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Failed to configure cx23416");
		return ret;
	}

	ret = pvr2_encoder_adjust(hdw);
	if (ret) return ret;

	ret = pvr2_encoder_vcmd(
		hdw, CX2341X_ENC_INITIALIZE_INPUT, 0);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Failed to initialize cx23416 video input");
		return ret;
	}

	return 0;
}
#endif

#ifndef PVR2_ENABLE_CX2341XMOD
int pvr2_encoder_configure(struct pvr2_hdw *hdw)
{
	int val;
	int ret = 0, audio, i;
	int mpeg1mode,is_30fps;

	ret = pvr2_encoder_prep_config(hdw);

	mpeg1mode = ((hdw->streamtype_val == PVR2_CVAL_STREAMTYPE_SS) ||
		     (hdw->streamtype_val == PVR2_CVAL_STREAMTYPE_VCD));
	is_30fps = (hdw->std_mask_cur & V4L2_STD_525_60) != 0;

	pvr2_trace(PVR2_TRACE_ENCODER,"pvr2_encoder_configure (native)");

	/* set stream output port.  Some notes here: The ivtv-derived
	   encoder documentation says that this command only gets a
	   single argument.  However the Windows driver for the model
	   29xxx series hardware has been sending 0x01 as a second
	   argument, while the Windows driver for the model 24xxx
	   series hardware has been sending 0x02 as a second argument.
	   Confusing matters further are the observations that 0x01
	   for that second argument simply won't work on the 24xxx
	   hardware, while 0x02 will work on the 29xxx - except that
	   when we use 0x02 then xawtv breaks due to a loss of
	   synchronization with the mpeg packet headers.  While xawtv
	   should be fixed to let it resync better (I did try to
	   contact Gerd about this but he has not answered), it has
	   also been determined that sending 0x00 as this mystery
	   second argument seems to work on both hardware models AND
	   xawtv works again.  So we're going to send 0x00. */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_OUTPUT_PORT, 2,
				 0x01, 0x00);

	/* set the Program Index Information. We want I,P,B frames (max 400) */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_PGM_INDEX_INFO, 2,
				 0x07, 0x0190);

	/* saa7115: 0xf0 */
	val = 0xf0;
	if (hdw->hdw_desc->flag_has_cx25840) {
		/* ivtv cx25840: 0x140 */
		val = 0x140;
	}
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_NUM_VSYNC_LINES, 2,
				 val, val);

	/* setup firmware to notify us about some events (don't know why...) */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_EVENT_NOTIFICATION, 4,
				 0, 0, 0x10000000, 0xffffffff);

	/* set fps to 25 or 30 (1 or 0)*/
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_FRAME_RATE, 1,
				 is_30fps ? 0 : 1);

	/* set encoding resolution */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_FRAME_SIZE, 2,
				 (mpeg1mode ?
				  hdw->res_ver_val / 2 : hdw->res_ver_val),
				 (mpeg1mode ?
				  hdw->res_hor_val / 2 : hdw->res_hor_val));
	/* set encoding aspect ratio */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_ASPECT_RATIO, 1,
				 pvr_tbl_aspect[hdw->aspect_val]);

	/* VBI */

	if (hdw->active_stream_type == pvr2_config_vbi) {
		int lines = 2 * (is_30fps ? 12 : 18);
		int size = (4*((lines*1443+3)/4)) / lines;
		ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_VBI_CONFIG, 7,
					 0xbd05, 1, 4,
					 0x25256262, 0x387f7f7f,
					 lines , size);
//                                     0x25256262, 0x13135454, lines , size);
		/* select vbi lines */
#define line_used(l)  (is_30fps ? (l >= 10 && l <= 21) : (l >= 6 && l <= 23))
		for (i = 2 ; i <= 24 ; i++){
			ret |= pvr2_encoder_vcmd(
				hdw,CX2341X_ENC_SET_VBI_LINE, 5,
				i-1,line_used(i), 0, 0, 0);
			ret |= pvr2_encoder_vcmd(
				hdw,CX2341X_ENC_SET_VBI_LINE, 5,
				(i-1) | (1 << 31),
				line_used(i), 0, 0, 0);
		}
	} else {
		ret |= pvr2_encoder_vcmd(
			hdw,CX2341X_ENC_SET_VBI_LINE, 5,
			0xffffffff,0,0,0,0);
	}

	/* set stream type */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_STREAM_TYPE, 1,
				 pvr_tbl_streamtype[hdw->streamtype_val]);

	/* set video bitrate */
	ret |= pvr2_encoder_vcmd(
		hdw, CX2341X_ENC_SET_BIT_RATE, 5,
		(hdw->vbr_val == PVR2_CVAL_VBR_CONSTANT ? 1 : 0),
		hdw->videobitrate_val,
		hdw->videopeak_val / 400,
		0,0);

	/* setup GOP structure (GOP size = 0f or 0c, 3-1 = 2 B-frames) */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_GOP_PROPERTIES, 2,
				 is_30fps ?  0x0f : 0x0c, 0x03);

	/* enable / disable 3:2 pulldown */
	ret |= pvr2_encoder_vcmd(hdw,CX2341X_ENC_SET_3_2_PULLDOWN,1,
				 hdw->pulldown_val ? 1 : 0);

	/* set GOP open/close property (open) */
	ret |= pvr2_encoder_vcmd(hdw,CX2341X_ENC_SET_GOP_CLOSURE,1,0);

	/* set audio stream properties 0x40b9? 0100 0000 1011 1001 */
	audio = (pvr_tbl_srate[hdw->srate_val] << 0 |
		 hdw->audiolayer_val << 2 |
		 pvr_tbl_audiobitrate[hdw->audiobitrate_val] << 4 |
		 pvr_tbl_mpegaudiomode[hdw->mpegaudiomode_val] << 8 |
		 pvr_tbl_audiomodeextension[hdw->audiomodeextension_val] << 10 |
		 pvr_tbl_emphasis[hdw->audioemphasis_val] << 12 |
		 (hdw->audiocrc_val ? 1 : 0) << 14);

	ret |= pvr2_encoder_vcmd(hdw,CX2341X_ENC_SET_AUDIO_PROPERTIES,1,
				 audio);

	/* set dynamic noise reduction filter to manual, Horiz/Vert */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_DNR_FILTER_MODE, 2,
				 0, 0x03);

	/* dynamic noise reduction filter param */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_DNR_FILTER_PROPS, 2
				 , 0, 0);

	/* dynamic noise reduction median filter */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_SET_CORING_LEVELS, 4,
				 0, 0xff, 0, 0xff);

	/* spacial prefiler parameter */
	ret |= pvr2_encoder_vcmd(hdw,
				 CX2341X_ENC_SET_SPATIAL_FILTER_TYPE, 2,
				 0x01, 0x01);

	/* initialize video input */
	ret |= pvr2_encoder_vcmd(hdw, CX2341X_ENC_INITIALIZE_INPUT, 0);

	return ret;
}
#endif

int pvr2_encoder_start(struct pvr2_hdw *hdw)
{
	int status;

	/* unmask some interrupts */
	pvr2_write_register(hdw, 0x0048, 0xbfffffff);

	pvr2_encoder_vcmd(hdw,CX2341X_ENC_MUTE_VIDEO,1,
			  hdw->input_val == PVR2_CVAL_INPUT_RADIO ? 1 : 0);

	switch (hdw->active_stream_type) {
	case pvr2_config_vbi:
		status = pvr2_encoder_vcmd(hdw,CX2341X_ENC_START_CAPTURE,2,
					   0x01,0x14);
		break;
	case pvr2_config_mpeg:
		status = pvr2_encoder_vcmd(hdw,CX2341X_ENC_START_CAPTURE,2,
					   0,0x13);
		break;
	default: /* Unhandled cases for now */
		status = pvr2_encoder_vcmd(hdw,CX2341X_ENC_START_CAPTURE,2,
					   0,0x13);
		break;
	}
	return status;
}

int pvr2_encoder_stop(struct pvr2_hdw *hdw)
{
	int status;

	/* mask all interrupts */
	pvr2_write_register(hdw, 0x0048, 0xffffffff);

	switch (hdw->active_stream_type) {
	case pvr2_config_vbi:
		status = pvr2_encoder_vcmd(hdw,CX2341X_ENC_STOP_CAPTURE,3,
					   0x01,0x01,0x14);
		break;
	case pvr2_config_mpeg:
		status = pvr2_encoder_vcmd(hdw,CX2341X_ENC_STOP_CAPTURE,3,
					   0x01,0,0x13);
		break;
	default: /* Unhandled cases for now */
		status = pvr2_encoder_vcmd(hdw,CX2341X_ENC_STOP_CAPTURE,3,
					   0x01,0,0x13);
		break;
	}

	return status;
}


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
