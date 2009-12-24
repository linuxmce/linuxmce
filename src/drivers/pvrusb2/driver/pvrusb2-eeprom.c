/*
 *
 *  $Id: pvrusb2-eeprom.c 1758 2007-11-18 01:24:46Z isely $
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
#include "pvrusb2-eeprom.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "compat.h"

#define trace_eeprom(...) pvr2_trace(PVR2_TRACE_EEPROM,__VA_ARGS__)

#ifdef PVR2_EEPROM_INDIRECT
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
/* BEGIN INDIRECT METHOD */

/*

  isely@pobox.com 16-Oct-2005 - There are two method by which we can
  theoretically retrieve information from the device's eeprom:

  Method #1: We expect tveeprom to attach to our I2C adapter as a
      client, in which case we send it a command to tell us what it
      knows about the device.  This is the "indirect" method.

  Method #2: We retrieve the eeprom contents ourselves and call into
      tveeprom_hauppauge_analog() to parse the data and tell us what
      it knows about the device.  This is the "direct" method.

  Unfortunately it isn't perfectly clear which method is the best.
  They each have pros and cons:

  #1 is simpler & more portable and has an API which is more stable.

  #1 doesn't provide as much information as #2 does.  For example, we
     can't retrieve the device's serial number with method #1.

  #1 requires that tveeprom.ko autonomously detect the eeprom chip on
     its own; we can't help it out here.  Worse still, it seems that
     the eeprom in some PVR USB2 devices (like mine) can't be detected
     correctly (I don't see an ack on a zero length write which is
     what the I2C core attempts).

  #2 uses an unstable API.  Current the ivtv implementation of #2 uses
     a completely different tveeprom struct than the v4l
     implementation of #2.  This causes a usability nightmare.

  Since I can't decide, both methods are implemented in this source.
  Method #2 (direct) is the default choice, but if you want to try
  method #1, then define PVR2_EEPROM_INDIRECT and cross your
  fingers...

  If you use method #1, please be aware that you won't have a serial
  number for the device and thus the sysfs interface may be a little
  different.  In addition, if tveeprom.ko fails to detect the eeprom
  you may have to force it using standard i2c module options (try
  force=-1,80).  FINALLY (and this may foreclose this option for you
  completely), the PVR USB2 eeprom seems to have valid data only in
  the upper 128 bytes - the lower 128 bytes causes tveeprom.ko to
  abort.  In method #2 we only read the upper 128 bytes...

*/


#error isely@pobox.com 2-jan-2006 Indirect eeprom method is currently broken - do not use

#include "pvrusb2-i2c.h"

/* Use I2C client interface to retrieve usable information from within
 * tveeprom.  This tends to be more stable than directly calling. */

int pvr2_eeprom_analyze(struct pvr2_hdw *hdw)
{
	u32 props[5];
	int stat;

	stat = pvr2_i2c_tveeprom_cmd(hdw,0,props);
	if (stat < 0) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Failed to retrieve eeprom data stat=%d",stat);
		return stat;
	}

	trace_eeprom("eeprom client query results:");
	trace_eeprom("tuner_type=%d",props[0]);
	trace_eeprom("tuner_formats=0x%x",props[1]);
	trace_eeprom("model=%d",props[2]);
	trace_eeprom("revision=%d",props[3]);
	trace_eeprom("has_radio=%d",props[4]);

	hdw->tuner_type = props[0];
	hdw->tuner_updated = !0;
	hdw->video_standards = props[1];

	return 0;
}

/* END INDIRECT METHOD */
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#endif /* PVR2_EEPROM_INDIRECT */

#ifndef PVR2_EEPROM_INDIRECT

/*

   Read and analyze data in the eeprom.  Use tveeprom to figure out
   the packet structure, since this is another Hauppauge device and
   internally it has a family resemblence to ivtv-type devices

*/

#include <media/tveeprom.h>

/* We seem to only be interested in the last 128 bytes of the EEPROM */
#define EEPROM_SIZE 128

/* Grab EEPROM contents, needed for direct method. */
static u8 *pvr2_eeprom_fetch(struct pvr2_hdw *hdw)
{
	struct i2c_msg msg[2];
	u8 *eeprom;
	u8 iadd[2];
	u8 addr;
	u16 eepromSize;
	unsigned int offs;
	int ret;
	int mode16 = 0;
	unsigned pcnt,tcnt;
	eeprom = kmalloc(EEPROM_SIZE,GFP_KERNEL);
	if (!eeprom) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Failed to allocate memory"
			   " required to read eeprom");
		return NULL;
	}

	trace_eeprom("Value for eeprom addr from controller was 0x%x",
		     hdw->eeprom_addr);
	addr = hdw->eeprom_addr;
	/* Seems that if the high bit is set, then the *real* eeprom
	   address is shifted right now bit position (noticed this in
	   newer PVR USB2 hardware) */
	if (addr & 0x80) addr >>= 1;

	/* FX2 documentation states that a 16bit-addressed eeprom is
	   expected if the I2C address is an odd number (yeah, this is
	   strange but it's what they do) */
	mode16 = (addr & 1);
	eepromSize = (mode16 ? 4096 : 256);
	trace_eeprom("Examining %d byte eeprom at location 0x%x"
		     " using %d bit addressing",eepromSize,addr,
		     mode16 ? 16 : 8);

	msg[0].addr = addr;
	msg[0].flags = 0;
	msg[0].len = mode16 ? 2 : 1;
	msg[0].buf = iadd;
	msg[1].addr = addr;
	msg[1].flags = I2C_M_RD;

	/* We have to do the actual eeprom data fetch ourselves, because
	   (1) we're only fetching part of the eeprom, and (2) if we were
	   getting the whole thing our I2C driver can't grab it in one
	   pass - which is what tveeprom is otherwise going to attempt */
	memset(eeprom,0,EEPROM_SIZE);
	for (tcnt = 0; tcnt < EEPROM_SIZE; tcnt += pcnt) {
		pcnt = 16;
		if (pcnt + tcnt > EEPROM_SIZE) pcnt = EEPROM_SIZE-tcnt;
		offs = tcnt + (eepromSize - EEPROM_SIZE);
		if (mode16) {
			iadd[0] = offs >> 8;
			iadd[1] = offs;
		} else {
			iadd[0] = offs;
		}
		msg[1].len = pcnt;
		msg[1].buf = eeprom+tcnt;
		if ((ret = i2c_transfer(&hdw->i2c_adap,
					msg,ARRAY_SIZE(msg))) != 2) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "eeprom fetch set offs err=%d",ret);
			kfree(eeprom);
			return NULL;
		}
	}
	return eeprom;
}


#ifndef PVR2_EEPROM_IVTV
/* Directly call eeprom analysis function within tveeprom. */
int pvr2_eeprom_analyze(struct pvr2_hdw *hdw)
{
	u8 *eeprom;
	struct tveeprom tvdata;

	memset(&tvdata,0,sizeof(tvdata));

	eeprom = pvr2_eeprom_fetch(hdw);
	if (!eeprom) return -EINVAL;

	{
		struct i2c_client fake_client;
		/* Newer version expects a useless client interface */
		fake_client.addr = hdw->eeprom_addr;
		fake_client.adapter = &hdw->i2c_adap;
		tveeprom_hauppauge_analog(&fake_client,&tvdata,eeprom);
	}

	trace_eeprom("eeprom assumed v4l tveeprom module");
	trace_eeprom("eeprom direct call results:");
	trace_eeprom("has_radio=%d",tvdata.has_radio);
	trace_eeprom("tuner_type=%d",tvdata.tuner_type);
	trace_eeprom("tuner_formats=0x%x",tvdata.tuner_formats);
	trace_eeprom("audio_processor=%d",tvdata.audio_processor);
	trace_eeprom("model=%d",tvdata.model);
	trace_eeprom("revision=%d",tvdata.revision);
	trace_eeprom("serial_number=%d",tvdata.serial_number);
	trace_eeprom("rev_str=%s",tvdata.rev_str);
	hdw->tuner_type = tvdata.tuner_type;
	hdw->tuner_updated = !0;
	hdw->serial_number = tvdata.serial_number;
	hdw->std_mask_eeprom = tvdata.tuner_formats;

	kfree(eeprom);

	return 0;
}
#endif /* PVR2_EEPROM_IVTV */
#ifdef PVR2_EEPROM_IVTV
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
/* BEGIN DIRECT METHOD, V4L OR IVTV */

/* Directly call eeprom analysis function within tveeprom.  This has
   portability issues because the internal API has been changing.  We
   have to do something positively gross here.  The two variants of
   tveeprom that we deal with (ivtv and v4l) use completely different
   definitions for the tveeprom structure.  To accomodate this, we'll
   allocate enough storage for the larger of the two, initialize it to
   bad but predictable data, and then call the analysis function.
   Upon return, we'll check how much data was changed and use that as
   a hint to determine exactly which tveeprom structure had been
   used.  Did I say this was ugly?  It's disgusting. */


/* This has to be an EXACT(!!) match with the tveeprom structure
   defined in our local copy of tveeprom.c. */
struct tveeprom_ivtv {
	u32 has_radio;

	u32 tuner_type;
	u32 tuner_formats;

	u32 digitizer;
	u32 digitizer_formats;

	u32 audio_processor;
	u32 decoder_processor;
	/* a_p_fmts? */

	u32 model;
	u32 revision;
	u32 serial_number;
	char rev_str[5];
};


int pvr2_eeprom_analyze(struct pvr2_hdw *hdw)
{
	u8 *eeprom;
	unsigned tcnt;
	int tp;
	union {
		struct tveeprom v4l;
		struct tveeprom_ivtv ivtv;
	} tvdata;

	memset(&tvdata,0x93,sizeof(tvdata));

	eeprom = pvr2_eeprom_fetch(hdw);
	if (!eeprom) return -EINVAL;

#ifdef PVR2_ENABLE_TVEEPROM_FAKECLIENT
	{
		struct i2c_client fake_client;
		/* Newer version expects a useless client interface */
		fake_client.addr = hdw->eeprom_addr;
		fake_client.adapter = &hdw->i2c_adap;
		tveeprom_hauppauge_analog(&fake_client,
					  (struct tveeprom *)&tvdata,eeprom);
	}
#else
	tveeprom_hauppauge_analog((struct tveeprom *)&tvdata,eeprom);
#endif

	/* Now figure out which structure was used */
	for (tcnt = 0; tcnt < sizeof(tvdata); tcnt++) {
		if (((unsigned char *)(&tvdata))[sizeof(tvdata) - (tcnt+1)] !=
		    0x93) {
			break;
		}
	}
	tcnt = sizeof(tvdata) - tcnt;

	if (sizeof(tvdata.ivtv) < sizeof(tvdata.v4l)) {
		tp = 0;
		if (tcnt > sizeof(tvdata.ivtv)) tp = 1;
	} else {
		tp = 1;
		if (tcnt > sizeof(tvdata.v4l)) tp = 0;
	}

	if (tp) {
		/* v4l */
		trace_eeprom("eeprom detected v4l tveeprom module");
		trace_eeprom("eeprom direct call results:");
		trace_eeprom("has_radio=%d",tvdata.v4l.has_radio);
		trace_eeprom("tuner_type=%d",tvdata.v4l.tuner_type);
		trace_eeprom("tuner_formats=0x%x",tvdata.v4l.tuner_formats);
		trace_eeprom("audio_processor=%d",tvdata.v4l.audio_processor);
		trace_eeprom("model=%d",tvdata.v4l.model);
		trace_eeprom("revision=%d",tvdata.v4l.revision);
		trace_eeprom("serial_number=%d",tvdata.v4l.serial_number);
		trace_eeprom("rev_str=%s",tvdata.v4l.rev_str);
		hdw->tuner_type = tvdata.v4l.tuner_type;
		hdw->tuner_updated = !0;
		hdw->serial_number = tvdata.v4l.serial_number;
		hdw->std_mask_eeprom = tvdata.v4l.tuner_formats;
	} else {
		/* ivtv */
		trace_eeprom("eeprom detected ivtv tveeprom module");
		trace_eeprom("eeprom direct call results:");
		trace_eeprom("has_radio=%d",tvdata.ivtv.has_radio);
		trace_eeprom("tuner_type=%d",tvdata.ivtv.tuner_type);
		trace_eeprom("tuner_formats=0x%x",tvdata.ivtv.tuner_formats);
		trace_eeprom("audio_processor=%d",tvdata.ivtv.audio_processor);
		trace_eeprom("model=%d",tvdata.ivtv.model);
		trace_eeprom("revision=%d",tvdata.ivtv.revision);
		trace_eeprom("serial_number=%d",tvdata.ivtv.serial_number);
		trace_eeprom("rev_str=%s",tvdata.ivtv.rev_str);
		hdw->tuner_type = tvdata.ivtv.tuner_type;
		hdw->tuner_updated = !0;
		hdw->serial_number = tvdata.ivtv.serial_number;
		hdw->std_mask_eeprom = tvdata.ivtv.tuner_formats;
	}

	kfree(eeprom);

	return 0;
}

/* END DIRECT METHOD, V4L OR IVTV */
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#endif /* PVR2_EEPROM_V4L */
#endif /* PVR2_EEPROM_INDIRECT */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
