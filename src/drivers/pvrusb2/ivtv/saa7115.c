/* 
 * saa7115 - Philips SAA7115 video decoder driver version 0.0.1
 *
 * Copyright (C) 2002 Maxim Yevtyushkin <max@linuxmedialabs.com>
 *
 * Based on saa7111 driver by Dave Perks
 *
 * Copyright (C) 1998 Dave Perks <dperks@ibm.net>
 *
 * Slight changes for video timing and attachment output by
 * Wolfgang Scherr <scherr@net4you.net>
 *
 * Changes by Ronald Bultje <rbultje@ronald.bitfreak.net>
 *    - moved over to linux>=2.4.x i2c protocol (1/1/2003)
 *
 * Changes by Kevin Thayer <nufan_wfk at yahoo.com>
 *    - changed to saa7115. (2/17/2003)
 *
 * Changes by Hans Verkuil <hverkuil@xs4all.nl>
 *    - VBI additions (2004)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/version.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/init.h>

#include <linux/slab.h>

#include <linux/mm.h>
#include <linux/pci.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/types.h>

#include "compat.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#endif /* LINUX26 */

#include <linux/videodev.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/page.h>

MODULE_DESCRIPTION("Philips SAA7115 video decoder driver");
MODULE_AUTHOR("Kevin Thayer");
MODULE_LICENSE("GPL");

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#ifndef I2C_DRIVERID_SAA7115
// Using temporary hack for missing I2C driver-ID for saa7115
#define I2C_DRIVERID_SAA7115 I2C_DRIVERID_EXP1
#endif /* I2C_DRIVERID_SAA7115 */

#include <linux/video_decoder.h>

#define IVTV_INTERNAL
#include "ivtv.h"
#include "decoder.h"

static int debug = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(debug, int, 0644);
#else
MODULE_PARM(debug, "i");
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) */
MODULE_PARM_DESC(debug, "Debug level (0-1)");

#define SAA7115_DEBUG(fmt,arg...) \
	do { \
		if (debug) \
	                printk(KERN_INFO "%s debug %d-%04x: " fmt, client->driver->name, \
                               i2c_adapter_id(client->adapter), client->addr , ## arg); \
	} while (0)

#define SAA7115_ERR(fmt, arg...) do { \
	printk(KERN_ERR "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)
#define SAA7115_INFO(fmt, arg...) do { \
	printk(KERN_INFO "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)

#define   I2C_SAA7114        0x42
#define   I2C_SAA7114A       0x40

/* ----------------------------------------------------------------------- */

static inline int saa7115_write(struct i2c_client *client, u8 reg, u8 value)
{
	return i2c_smbus_write_byte_data(client, reg, value);
}

static int writeregs(struct i2c_client *client, const unsigned char *regs)
{
	unsigned char reg, data;

	while (*regs != 0x00) {
		reg = *(regs++);
		data = *(regs++);
		if (saa7115_write(client, reg, data) < 0)
			return -1;
	}
	return 0;
}

static inline int saa7115_read(struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}

/* ----------------------------------------------------------------------- */

// If a value differs from the Hauppauge driver values, then the comment start with
// 'was 0xXX' to denote the Hauppauge value. Otherwise the value is identical what the
// Hauppauge driver sets.

static const unsigned char init_saa7115_auto_input[] = {
	0x01, 0x48,		// was 0x48. 0x08: white peak control enabled, 0x48: white peak control disabled
	0x03, 0x20,		// was 0x30. 0x30: automatic gain control + long vertical blanking
	//           0x3c: user programmable gain + long vertical blanking
	0x04, 0x90,		// analog gain set to 0
	0x05, 0x90,		// analog gain set to 0
	0x06, 0xEB,		// horiz sync begin = -21
	0x07, 0xE0,		// horiz sync stop = -17
	0x0A, 0x80,		// was 0x88. decoder brightness, 0x80 is itu standard
	0x0B, 0x44,		// was 0x48. decoder contrast, 0x44 is itu standard
	0x0C, 0x40,		// was 0x47. decoder saturation, 0x40 is itu standard
	0x0D, 0x00,		// chrominance hue control
	0x0F, 0x00,		// chrominance gain control: use automicatic mode
	0x10, 0x06,		// chrominance/luminance control: active adaptive combfilter
	0x11, 0x00,		// delay control
	0x12, 0x9D,		// RTS0 output control: VGATE
	0x13, 0x80,		// X-port output control: ITU656 standard mode, RTCO output enable RTCE
	0x14, 0x00,		// analog/ADC/auto compatibility control
	0x18, 0x40,		// raw data gain 0x00 = nominal
	0x19, 0x80,		// raw data offset 0x80 = 0 LSB
	0x1A, 0x77,		// color killer level control 0x77 = recommended
	0x1B, 0x42,		// misc chroma control 0x42 = recommended
	0x1C, 0xA9,		// combfilter control 0xA9 = recommended
	0x1D, 0x01,		// combfilter control 0x01 = recommended
	0x88, 0xD0,		// reset device
	0x88, 0xF0,		// set device programmed, all in operational mode
	0x00, 0x00
};

static const unsigned char cfg_saa7115_reset_scaler[] = {
	0x87, 0x00,		// disable I-port output
	0x88, 0xD0,		// reset scaler
	0x88, 0xF0,		// activate scaler
	0x87, 0x01,		// enable I-port output
	0x00, 0x00
};

/* ============== SAA7715 VIDEO templates =============  */

static const unsigned char cfg_saa7115_NTSC_fullres_x[] = {
	0xCC, 0xD0,		// hsize low (output) //hor output window size = 0x2d0 = 720
	0xCD, 0x02,		// hsize hi (output)

	/*Why not in NTSC-Land, too? */
	0xD0, 0x01,		// down scale = 1
	0xD8, 0x00,		// hor lum scaling 0x0400 = 1
	0xD9, 0x04,
	0xDC, 0x00,		// hor chrom scaling 0x0200. must be hor lum scaling /2
	0xDD, 0x02,		// H-scaling incr chroma

	0x00, 0x00
};
static const unsigned char cfg_saa7115_NTSC_fullres_y[] = {
	0xCE, 0xF8,		// vsize low (output) ver output window size = 248 (but NTSC is 240?)
	0xCF, 0x00,		// vsize hi (output)

	/*Why not in NTSC-Land, too? */
	0xD5, 0x40,		// Lum contrast, nominal value = 0x40
	0xD6, 0x40,		// Chroma satur. nominal value = 0x80

	0xE0, 0x00,		// V-scaling incr luma low
	0xE1, 0x04,		// " hi
	0xE2, 0x00,		// V-scaling incr chroma low
	0xE3, 0x04,		// " hi

	0x00, 0x00
};

static const unsigned char cfg_saa7115_NTSC_video[] = {
	0x80, 0x00,		// reset tasks
	0x88, 0xD0,		// reset scaler

	0x15, 0x03,		// VGATE pulse start
	0x16, 0x11,		// VGATE pulse stop
	0x17, 0x9C,		// VGATE MSB and other values

	0x08, 0x68,		// 0xBO: auto detection, 0x68 = NTSC
	0x0E, 0x07,		// lots of different stuff... video autodetection is on

	0x5A, 0x06,		// Vertical offset, standard NTSC value for ITU656 line counting

	// Task A
	0x90, 0x80,		// Task Handling Control
	0x91, 0x48,		// X-port formats/config
	0x92, 0x40,		// Input Ref. signal Def.
	0x93, 0x84,		// I-port config
	0x94, 0x01,		// hoffset low (input), 0x0002 is minimum
	0x95, 0x00,		// hoffset hi (input)
	0x96, 0xD0,		// hsize low (input), 0x02d0 = 720
	0x97, 0x02,		// hsize hi (input)
	0x98, 0x05,		// voffset low (input)
	0x99, 0x00,		// voffset hi (input)
	0x9A, 0x0C,		// vsize low (input), 0x0c = 12
	0x9B, 0x00,		// vsize hi (input)
	0x9C, 0xA0,		// hsize low (output), 0x05a0 = 1440
	0x9D, 0x05,		// hsize hi (output)
	0x9E, 0x0C,		// vsize low (output), 0x0c = 12
	0x9F, 0x00,		// vsize hi (output)

	// Task B
	0xC0, 0x00,		// Task Handling Control
	0xC1, 0x08,		// X-port formats/config
	0xC2, 0x00,		// Input Ref. signal Def.
	0xC3, 0x80,		// I-port config
	0xC4, 0x02,		// hoffset low (input), 0x0002 is minimum
	0xC5, 0x00,		// hoffset hi (input)
	0xC6, 0xD0,		// hsize low (input), 0x02d0 = 720
	0xC7, 0x02,		// hsize hi (input)
	0xC8, 0x12,		// voffset low (input), 0x12 = 18
	0xC9, 0x00,		// voffset hi (input)
	0xCA, 0xF8,		// vsize low (input), 0xf8 = 248
	0xCB, 0x00,		// vsize hi (input)
	0xCC, 0xD0,		// hsize low (output), 0x02d0 = 720
	0xCD, 0x02,		// hsize hi (output)

	0xF0, 0xAD,		// Set PLL Register. NTSC 525 lines per frame, 27 MHz
	0xF1, 0x05,		// low bit with 0xF0
	0xF5, 0xAD,		// Set pulse generator register
	0xF6, 0x01,

	0x87, 0x00,		// Disable I-port output
	0x88, 0xD0,		// reset scaler
	0x80, 0x20,		//Activate only task "B", continuous mode (was 0xA0)
	0x88, 0xF0,		//activate scaler
	0x87, 0x01,		//Enable I-port output
	0x00, 0x00
};

static const unsigned char cfg_saa7115_PAL_fullres_x[] = {
	0xCC, 0xD0,		// hsize low (output), 720 same as NTSC
	0xCD, 0x02,		// hsize hi (output)

	0xD0, 0x01,		// down scale = 1
	0xD8, 0x00,		// hor lum scaling 0x0400 = 1
	0xD9, 0x04,
	0xDC, 0x00,		// hor chrom scaling 0x0200. must be hor lum scaling /2
	0xDD, 0x02,		// H-scaling incr chroma

	0x00, 0x00
};
static const unsigned char cfg_saa7115_PAL_fullres_y[] = {
	0xCE, 0x20,		// vsize low (output), 0x0120 = 288
	0xCF, 0x01,		// vsize hi (output)

	0xD5, 0x40,		// Lum contrast, nominal value = 0x40
	0xD6, 0x40,		// Chroma satur. nominal value = 0x80

	0xE0, 0x00,		// V-scaling incr luma low
	0xE1, 0x04,		// " hi
	0xE2, 0x00,		// V-scaling incr chroma low
	0xE3, 0x04,		// " hi

	0x00, 0x00
};

static const unsigned char cfg_saa7115_PAL_video[] = {
	0x80, 0x00,		// reset tasks
	0x88, 0xD0,		// reset scaler

	0x15, 0x37,		// VGATE start
	0x16, 0x16,		// VGATE stop
	0x17, 0x99,		// VGATE MSB and other values

	0x08, 0x28,		// 0x28 = PAL
	0x0E, 0x07,		// chrominance control 1

	0x5A, 0x03,		// Vertical offset, standard PAL value

	// Task A
	0x90, 0x81,		// Task Handling Control
	0x91, 0x48,		// X-port formats/config
	0x92, 0x40,		// Input Ref. signal Def.
	0x93, 0x84,		// I-port config
	// This is weird: the datasheet says that you should use 2 as the minimum value,
	// but Hauppauge uses 0, and changing that to 2 causes indeed problems (for PAL)
	0x94, 0x00,		// hoffset low (input), 0x0002 is minimum
	0x95, 0x00,		// hoffset hi (input)
	0x96, 0xD0,		// hsize low (input), 0x02d0 = 720
	0x97, 0x02,		// hsize hi (input)
	0x98, 0x03,		// voffset low (input)
	0x99, 0x00,		// voffset hi (input)
	0x9A, 0x12,		// vsize low (input), 0x12 = 18
	0x9B, 0x00,		// vsize hi (input)
	0x9C, 0xA0,		// hsize low (output), 0x05a0 = 1440
	0x9D, 0x05,		// hsize hi (output)
	0x9E, 0x12,		// vsize low (output), 0x12 = 18
	0x9F, 0x00,		// vsize hi (output)

	// Task B
	0xC0, 0x00,		// Task Handling Control
	0xC1, 0x08,		// X-port formats/config
	0xC2, 0x00,		// Input Ref. signal Def.
	0xC3, 0x80,		// I-port config
	0xC4, 0x00,		// hoffset low (input), 0x0002 is minimum. See comment at 0x94 above.
	0xC5, 0x00,		// hoffset hi (input)
	0xC6, 0xD0,		// hsize low (input), 0x02d0 = 720
	0xC7, 0x02,		// hsize hi (input)
	0xC8, 0x16,		// voffset low (input), 0x16 = 22
	0xC9, 0x00,		// voffset hi (input)
	0xCA, 0x20,		// vsize low (input), 0x0120 = 288
	0xCB, 0x01,		// vsize hi (input)                       
	0xCC, 0xD0,		// hsize low (output), 0x02d0 = 720
	0xCD, 0x02,		// hsize hi (output)
	0xCE, 0x20,		// vsize low (output), 0x0120 = 288
	0xCF, 0x01,		// vsize hi (output)

	0xF0, 0xB0,		// Set PLL Register. PAL 625 lines per frame, 27 MHz
	0xF1, 0x05,		// low bit with 0xF0, (was 0x05)
	0xF5, 0xB0,		// Set pulse generator register
	0xF6, 0x01,

	0x87, 0x00,		// Disable I-port output
	0x88, 0xD0,		// reset scaler (was 0xD0)
	0x80, 0x20,		// Activate only task "B"
	0x88, 0xF0,		// activate scaler
	0x87, 0x01,		// Enable I-port output
	0x00, 0x00
};

/* ============== SAA7715 VIDEO templates (end) =======  */

static const unsigned char cfg_saa7115_vbi_on[] = {
	0x80, 0x00,		// reset tasks
	0x88, 0xD0,		// reset scaler
	0x80, 0x30,		// Activate both tasks
	0x88, 0xF0,		// activate scaler
	0x87, 0x01,		// Enable I-port output
	0x00, 0x00
};

static const unsigned char cfg_saa7115_vbi_off[] = {
	0x80, 0x00,		// reset tasks
	0x88, 0xD0,		// reset scaler
	0x80, 0x20,		// Activate only task "B"
	0x88, 0xF0,		// activate scaler
	0x87, 0x01,		// Enable I-port output
	0x00, 0x00
};

static const unsigned char init_saa7115_misc[] = {
	0x38, 0x03,		// audio stuff
	0x39, 0x10,
	0x3A, 0x08,

	0x81, 0x01,		// reg 0x15,0x16 define blanking window
	0x82, 0x00,
	0x83, 0x01,		// I port settings
	0x84, 0x20,
	0x85, 0x21,
	0x86, 0xC5,
	0x87, 0x01,

	// Task A
	0xA0, 0x01,		// down scale = 1
	0xA1, 0x00,		// prescale accumulation length = 1
	0xA2, 0x00,		// dc gain and fir prefilter control
	0xA4, 0x80,		// Lum Brightness, nominal value = 0x80
	0xA5, 0x40,		// Lum contrast, nominal value = 0x40
	0xA6, 0x40,		// Chroma satur. nominal value = 0x80
	0xA8, 0x00,		// hor lum scaling 0x0200 = 2 zoom
	0xA9, 0x02,		// note: 2 x zoom ensures that VBI lines have same length as video lines.
	0xAA, 0x00,		// H-phase offset Luma = 0
	0xAC, 0x00,		// hor chrom scaling 0x0200. must be hor lum scaling /2
	0xAD, 0x01,		// H-scaling incr chroma 
	0xAE, 0x00,		// H-phase offset chroma. must be offset luma /2         

	0xB0, 0x00,		// V-scaling incr luma low
	0xB1, 0x04,		// " hi
	0xB2, 0x00,		// V-scaling incr chroma low
	0xB3, 0x04,		// " hi
	0xB4, 0x01,		// V-scaling mode control
	0xB8, 0x00,		// V-phase offset chroma 00
	0xB9, 0x00,		// V-phase offset chroma 01
	0xBA, 0x00,		// V-phase offset chroma 10
	0xBB, 0x00,		// V-phase offset chroma 11
	0xBC, 0x00,		// V-phase offset luma 00
	0xBD, 0x00,		// V-phase offset luma 01
	0xBE, 0x00,		// V-phase offset luma 10
	0xBF, 0x00,		// V-phase offset luma 11

	// Task B
	0xD0, 0x01,		// down scale = 1
	0xD1, 0x00,		// prescale accumulation length = 1
	0xD2, 0x00,		// dc gain and fir prefilter control
	0xD4, 0x80,		// Lum Brightness, nominal value = 0x80
	0xD5, 0x40,		// Lum contrast, nominal value = 0x40
	0xD6, 0x40,		// Chroma satur. nominal value = 0x80
	0xD8, 0x00,		// hor lum scaling 0x0400 = 1
	0xD9, 0x04,
	0xDA, 0x00,		// H-phase offset Luma = 0
	0xDC, 0x00,		// hor chrom scaling 0x0200. must be hor lum scaling /2
	0xDD, 0x02,		// H-scaling incr chroma
	0xDE, 0x00,		// H-phase offset chroma. must be offset luma /2

	0xE0, 0x00,		// V-scaling incr luma low
	0xE1, 0x04,		// " hi
	0xE2, 0x00,		// V-scaling incr chroma low
	0xE3, 0x04,		// " hi
	0xE4, 0x01,		// V-scaling mode control
	0xE8, 0x00,		// V-phase offset chroma 00
	0xE9, 0x00,		// V-phase offset chroma 01
	0xEA, 0x00,		// V-phase offset chroma 10
	0xEB, 0x00,		// V-phase offset chroma 11
	0xEC, 0x00,		// V-phase offset luma 00
	0xED, 0x00,		// V-phase offset luma 01
	0xEE, 0x00,		// V-phase offset luma 10
	0xEF, 0x00,		// V-phase offset luma 11

	0xF2, 0x50,		// crystal clock = 24.576 MHz, target = 27MHz
	0xF3, 0x46,
	0xF4, 0x00,
	0xF7, 0x4B,		// not the recommended settings!
	0xF8, 0x00,
	0xF9, 0x4B,
	0xFA, 0x00,
	0xFB, 0x4B,
	0xFF, 0x88,		// PLL2 lock detection settings: 71 lines 50% phase error

	/* Turn off VBI */
	0x40, 0x20,             // No framing code errors allowed.
	0x41, 0xFF,
	0x42, 0xFF,
	0x43, 0xFF,
	0x44, 0xFF,
	0x45, 0xFF,
	0x46, 0xFF,
	0x47, 0xFF,
	0x48, 0xFF,
	0x49, 0xFF,
	0x4A, 0xFF,
	0x4B, 0xFF,
	0x4C, 0xFF,
	0x4D, 0xFF,
	0x4E, 0xFF,
	0x4F, 0xFF,
	0x50, 0xFF,
	0x51, 0xFF,
	0x52, 0xFF,
	0x53, 0xFF,
	0x54, 0xFF,
	0x55, 0xFF,
	0x56, 0xFF,
	0x57, 0xFF,
	0x58, 0x40,
	0x59, 0x47,
	0x5B, 0x83,
	0x5D, 0xBD,
	0x5E, 0x35,

	0x02, 0x84,		// input tuner -> input 4, amplifier active
	0x09, 0x53,		// 0x53, was 0x56 for NTSC. luminance control

	0x80, 0x20,		// enable task B
	0x88, 0xD0,
	0x88, 0xF0,
	0x00, 0x00
};

/* ============== SAA7715 AUDIO settings =============  */
static const unsigned char cfg_saa7115_48_audio[] = {
	0x34, 0xCE,		// 48khz
	0x35, 0xFB,		// "
	0x36, 0x30,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_441_audio[] = {
	0x34, 0xF2,		// 44.1khz
	0x35, 0x00,		// "
	0x36, 0x2D,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_32_audio[] = {
	0x34, 0xDF,		// 32.0khz
	0x35, 0xA7,		// "
	0x36, 0x20,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_NTSC_48_audio[] = {
	0x30, 0xCD,		// 48.0khz NTSC
	0x31, 0x20,		// "
	0x32, 0x03,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_PAL_48_audio[] = {
	0x30, 0x00,		// 48.0khz PAL
	0x31, 0xC0,		// "
	0x32, 0x03,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_NTSC_441_audio[] = {
	0x30, 0xBC,		// 44.1khz NTSC
	0x31, 0xDF,		// "
	0x32, 0x02,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_PAL_441_audio[] = {
	0x30, 0x00,		// 44.1khz PAL
	0x31, 0x72,		// "
	0x32, 0x03,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_NTSC_32_audio[] = {
	0x30, 0xDE,		// 32.0khz NTSC
	0x31, 0x15,		// "
	0x32, 0x02,		// "
	0x00, 0x00
};

static const unsigned char cfg_saa7115_PAL_32_audio[] = {
	0x30, 0x00,		// 32.0khz PAL
	0x31, 0x80,		// "
	0x32, 0x02,		// "
	0x00, 0x00
};

/* If the VBI slicer does not detect any signal it will fill up
   the buffer with 0xa0 bytes. */
static const char vbi_no_data[] = {
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0
};

static int has_vbi_data(const u8 * vbi)
{
	// skip if this line contains no data
	if (!memcmp(vbi, vbi_no_data, 10)) {
		return 0;
	}
	return 1;
}

static int odd_parity(u8 c)
{
	c ^= (c >> 4);
	c ^= (c >> 2);
	c ^= (c >> 1);

	return c & 1;
}

static int decode_vps(u8 * dst, u8 * p)
{
	static u8 biphase_tbl[] = {
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96,
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2,
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94,
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5,
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1,
		0xc3, 0x4b, 0x43, 0xc3, 0x87, 0x0f, 0x07, 0x87,
		0x83, 0x0b, 0x03, 0x83, 0xc3, 0x4b, 0x43, 0xc3,
		0xc1, 0x49, 0x41, 0xc1, 0x85, 0x0d, 0x05, 0x85,
		0x81, 0x09, 0x01, 0x81, 0xc1, 0x49, 0x41, 0xc1,
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5,
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1,
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4,
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0,
		0xc2, 0x4a, 0x42, 0xc2, 0x86, 0x0e, 0x06, 0x86,
		0x82, 0x0a, 0x02, 0x82, 0xc2, 0x4a, 0x42, 0xc2,
		0xc0, 0x48, 0x40, 0xc0, 0x84, 0x0c, 0x04, 0x84,
		0x80, 0x08, 0x00, 0x80, 0xc0, 0x48, 0x40, 0xc0,
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4,
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96,
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2,
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94,
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
	};
	int i;
	u8 c, err = 0;

	for (i = 0; i < 2 * 13; i += 2) {
		err |= biphase_tbl[p[i]] | biphase_tbl[p[i + 1]];
		c = (biphase_tbl[p[i + 1]] & 0xf) | ((biphase_tbl[p[i]] & 0xf)
						     << 4);
		dst[i / 2] = c;
	}
	return err & 0xf0;
}

static int decode_wss(u8 * p)
{
	static const int wss_bits[8] = {
		0, 0, 0, 1, 0, 1, 1, 1
	};
	unsigned char parity;
	int wss = 0;
	int i;

	for (i = 0; i < 16; i++) {
		int b1 = wss_bits[p[i] & 7];
		int b2 = wss_bits[(p[i] >> 3) & 7];

		if (b1 == b2)
			return -1;
		wss |= b2 << i;
	}
	parity = wss & 15;
	parity ^= parity >> 2;
	parity ^= parity >> 1;

	if (!(parity & 1))
		return -1;

	return wss;
}

/* ============ SAA7715 AUDIO settings (end) ============= */

static int saa7115_command(struct i2c_client *client, unsigned int cmd,
			   void *arg)
{
	struct decoder_state *state = i2c_get_clientdata(client);

	switch (cmd) {

	case 0:
		//SAA7115_DEBUG("writing init\n");
		//saa7115_write_block(client, init, sizeof(init));
		break;
		/* ioctls to allow direct access to the saa7115 registers for testing */
	case DECODER_GET_REG:
	{
		struct decoder_reg *reg = arg;

		reg->val = saa7115_read(client, reg->reg & 0xff);
		break;
	}
	case DECODER_SET_REG:
	{
		struct decoder_reg *reg = arg;

		saa7115_write(client, reg->reg & 0xff, reg->val & 0xff);
		break;
	}

	case DECODER_SET_SIZE:
	{
		/* Used video_window because it has height/width and is
		 * already defined */
		struct video_window *wind = arg;
		int HPSC, HFSC;
		int VSCY, Vsrc;

		SAA7115_DEBUG("decoder set size\n");

		/* FIXME need better bounds checking here */
		if ((wind->width < 1) || (wind->width > 1440))
			return -EINVAL;
		if ((wind->height < 1) || (wind->height > 960))
			return -EINVAL;

		/* probably have a valid size, let's set it */
/* Set output width/height */
		/* width */
		saa7115_write(client, 0xCC, (u8) (wind->width & 0xFF));
		saa7115_write(client, 0xCD, (u8) ((wind->width >> 8) & 0xFF));
		/* height */
		saa7115_write(client, 0xCE, (u8) (wind->height & 0xFF));
		saa7115_write(client, 0xCF, (u8) ((wind->height >> 8) & 0xFF));

/* Scaling settings */
		/* Hprescaler is floor(inres/outres) */
		/* FIXME hardcoding input res */
		if (wind->width != 720) {
			HPSC = (int)(720 / wind->width);
                        // 0 is not allowed (div. by zero)
                        HPSC = HPSC ? HPSC : 1;
			HFSC = (int)((1024 * 720) / (HPSC * wind->width));

			SAA7115_DEBUG("Hpsc: 0x%05x, Hfsc: 0x%05x\n", HPSC, HFSC);
			/* FIXME hardcodes to "Task B" 
			 * write H prescaler integer */
			saa7115_write(client, 0xD0, (u8) (HPSC & 0x3F));

			/* write H fine-scaling (luminance) */
			saa7115_write(client, 0xD8, (u8) (HFSC & 0xFF));
			saa7115_write(client, 0xD9, (u8) ((HFSC >> 8) & 0xFF));
			/* write H fine-scaling (chrominance)
			 * must be lum/2, so i'll just bitshift :) */
			saa7115_write(client, 0xDC, (u8) ((HFSC >> 1) & 0xFF));
			saa7115_write(client, 0xDD, (u8) ((HFSC >> 9) & 0xFF));
		} else {
			if (state->norm != VIDEO_MODE_NTSC) {
				SAA7115_DEBUG("Setting full PAL width\n");
				writeregs(client, cfg_saa7115_PAL_fullres_x);
			} else {
				SAA7115_DEBUG("Setting full NTSC width\n");
				writeregs(client, cfg_saa7115_NTSC_fullres_x);
			}
		}

		Vsrc = 480;
		if (state->norm != VIDEO_MODE_NTSC)
			Vsrc = 576;

		if (wind->height != Vsrc) {
			VSCY = (int)((1024 * Vsrc) / wind->height);
			SAA7115_DEBUG("Vsrc: %d, Vscy: 0x%05x\n", Vsrc, VSCY);

			/* Correct Contrast and Luminance */
			saa7115_write(client, 0xD5, (u8) (64 * 1024 / VSCY));
			saa7115_write(client, 0xD6, (u8) (64 * 1024 / VSCY));

			/* write V fine-scaling (luminance) */
			saa7115_write(client, 0xE0, (u8) (VSCY & 0xFF));
			saa7115_write(client, 0xE1, (u8) ((VSCY >> 8) & 0xFF));
			/* write V fine-scaling (chrominance) */
			saa7115_write(client, 0xE2, (u8) (VSCY & 0xFF));
			saa7115_write(client, 0xE3, (u8) ((VSCY >> 8) & 0xFF));
		} else {
			if (state->norm != VIDEO_MODE_NTSC) {
				SAA7115_DEBUG("Setting full PAL height\n");
				writeregs(client, cfg_saa7115_PAL_fullres_y);
			} else {
				SAA7115_DEBUG("Setting full NTSC height\n");
				writeregs(client, cfg_saa7115_NTSC_fullres_y);
			}
		}

		writeregs(client, cfg_saa7115_reset_scaler);
		break;
	}
	case DECODER_DUMP:
	{
		int i;

		SAA7115_DEBUG("decoder dump\n");

		for (i = 0; i < 32; i += 16) {
			int j;

			SAA7115_DEBUG("%s: %03x", client->name, i);
			for (j = 0; j < 16; ++j) {
				printk(" %02x", saa7115_read(client, i + j));
			}
			printk("\n");
		}
	}
		break;

	case DECODER_GET_CAPABILITIES:
	{
		struct video_decoder_capability *cap = arg;

		SAA7115_DEBUG("decoder get capabilities\n");

		cap->flags = VIDEO_DECODER_PAL |
		    VIDEO_DECODER_NTSC |
		    VIDEO_DECODER_SECAM |
		    VIDEO_DECODER_AUTO | VIDEO_DECODER_CCIR;
		cap->inputs = 8;
		cap->outputs = 1;
	}
		break;

	case DECODER_SET_AUDIO:
	{
		int *iarg = arg;
		SAA7115_DEBUG("set audio: 0x%02x\n", *iarg);
		switch (*iarg) {
		case DECODER_AUDIO_32_KHZ:
			writeregs(client, cfg_saa7115_32_audio);
			if (state->norm == VIDEO_MODE_NTSC) {
				writeregs(client, cfg_saa7115_NTSC_32_audio);
			} else {
				writeregs(client, cfg_saa7115_PAL_32_audio);
			}
			break;
		case DECODER_AUDIO_441_KHZ:
			writeregs(client, cfg_saa7115_441_audio);
			if (state->norm == VIDEO_MODE_NTSC) {
				writeregs(client, cfg_saa7115_NTSC_441_audio);
			} else {
				writeregs(client, cfg_saa7115_PAL_441_audio);
			}
			break;
		case DECODER_AUDIO_48_KHZ:
			writeregs(client, cfg_saa7115_48_audio);
			if (state->norm == VIDEO_MODE_NTSC) {
				writeregs(client, cfg_saa7115_NTSC_48_audio);
			} else {
				writeregs(client, cfg_saa7115_PAL_48_audio);
			}
			break;
		default:
			SAA7115_DEBUG("invalid audio setting 0x%02x\n", *iarg);
		}

		state->audio = *iarg;

	}
		break;
	case DECODER_GET_STATUS:
	{
		int *iarg = arg;
		int status;
		int res;

		status = saa7115_read(client, 0x1f);

		SAA7115_DEBUG("status: 0x%02x\n", status);
		res = 0;
		if ((status & (1 << 6)) == 0) {
			res |= DECODER_STATUS_GOOD;
		}
		switch (state->norm) {
		case VIDEO_MODE_NTSC:
			res |= DECODER_STATUS_NTSC;
			break;
		case VIDEO_MODE_PAL:
			res |= DECODER_STATUS_PAL;
			break;
		case VIDEO_MODE_SECAM:
			res |= DECODER_STATUS_SECAM;
			break;
		default:
		case VIDEO_MODE_AUTO:
			if ((status & (1 << 5)) != 0) {
				res |= DECODER_STATUS_NTSC;
			} else {
				res |= DECODER_STATUS_PAL;
			}
			break;
		}
		if ((status & (1 << 0)) != 0) {
			res |= DECODER_STATUS_COLOR;
		}
		*iarg = res;
	}
		break;

        case DECODER_LOG_STATUS:
                /* TODO: print video status */
                break;

	case DECODER_SET_NORM:
	{
		int *iarg = arg;
		int taskb = saa7115_read(client, 0x80) & 0x10;

		switch (*iarg) {
		case VIDEO_MODE_NTSC:
			SAA7115_DEBUG("decoder set norm NTSC\n");
			writeregs(client, cfg_saa7115_NTSC_video);
			break;

		case VIDEO_MODE_PAL:
			SAA7115_DEBUG("decoder set norm PAL\n");
			writeregs(client, cfg_saa7115_PAL_video);
			break;

		case VIDEO_MODE_SECAM:
			SAA7115_DEBUG("decoder set norm SECAM\n");
			writeregs(client, cfg_saa7115_PAL_video);
			break;

		default:
			SAA7115_DEBUG("Unknown video mode!!!\n");
			return -EINVAL;
		}

		state->norm = *iarg;

		/* restart task B if needed */
		if (taskb) {
			writeregs(client, cfg_saa7115_vbi_on);
		}

		/* switch audio mode too! */
		saa7115_command(client, DECODER_SET_AUDIO, &state->audio);

	}
		break;

	case DECODER_SET_INPUT:
	{
		int *iarg = arg;

		SAA7115_DEBUG("decoder set input (%d)\n", *iarg);
		/* inputs from 0-9 are available */
		if (*iarg < 0 || *iarg > 9) {
			return -EINVAL;
		}

		if (state->input != *iarg) {
			SAA7115_DEBUG("now setting %s input\n",
				*iarg >= 6 ? "S-Video" : "Composite");
			state->input = *iarg;

			/* select mode */
			saa7115_write(client,
				      0x02,
				      (saa7115_read(client, 0x02) & 0xf0) |
				      state->input);

			/* bypass chrominance trap for modes 6..9 */
			saa7115_write(client, 0x09,
				      (saa7115_read(client, 0x09) & 0x7f) |
				      (state->input < 6 ? 0x0 : 0x80));
		}
	}
		break;

	case DECODER_SET_OUTPUT:
	{
		int *iarg = arg;

		SAA7115_DEBUG("decoder set output\n");

		/* not much choice of outputs */
		if (*iarg != 0) {
			return -EINVAL;
		}
	}
		break;

	case DECODER_ENABLE_OUTPUT:
	{
		int *iarg = arg;
		int enable = (*iarg != 0);

		SAA7115_DEBUG("decoder %s output\n",
			enable ? "enable" : "disable");

		state->playback = !enable;

		if (state->enable != enable) {
			state->enable = enable;

			if (state->enable) {
				saa7115_write(client, 0x87, 0x01);
			} else {
				saa7115_write(client, 0x87, 0x00);
			}
		}
	}
		break;

	case DECODER_GET_STATE:
	{
		struct decoder_state *pic = arg;

		pic->bright = state->bright;
		pic->contrast = state->contrast;
		pic->sat = state->sat;
		pic->hue = state->hue;
	}
		break;

	case DECODER_GET_WSS:
	{
		int *iarg = arg;
		u8 status, wss1, wss2;

		status = saa7115_read(client, 0x6b);
		if (status & 0xC0) {
			*iarg = -1;
			break;
		}
		wss1 = saa7115_read(client, 0x6c);
		wss2 = saa7115_read(client, 0x6d);
		*iarg = wss2 << 8 | wss1;
		break;
	}

	case DECODER_GET_CC_ODD:
	{
		int *iarg = arg;
		u8 status, cc1, cc2;

		status = saa7115_read(client, 0x66);
		if (status & 0xC0) {
			*iarg = -1;
			break;
		}
		cc1 = saa7115_read(client, 0x67);
		cc2 = saa7115_read(client, 0x68);
		*iarg = cc2 << 8 | cc1;
		break;
	}

	case DECODER_GET_CC_EVEN:
	{
		int *iarg = arg;
		u8 status, cc1, cc2;

		status = saa7115_read(client, 0x66);
		if (status & 0x30) {
			*iarg = -1;
			break;
		}
		cc1 = saa7115_read(client, 0x69);
		cc2 = saa7115_read(client, 0x6A);
		*iarg = cc2 << 8 | cc1;
		break;
	}

	case DECODER_GET_VBI:
	{
                static u8 lcr2vbi[] = {
                        0, IVTV_SLICED_TYPE_TELETEXT_B, 0, 0,     // 1 = Teletext
                        IVTV_SLICED_TYPE_CAPTION_525, IVTV_SLICED_TYPE_WSS_625, // 4 = CC, 5 = WSS
                        0, IVTV_SLICED_TYPE_VPS, // 7 = VPS
                        0, 0, 0, 0,
                        0, 0, 0, 0
                };
                struct decoder_lcr *lcr = (struct decoder_lcr *)arg;
                int i;

                lcr->raw = (saa7115_read(client, 0x80) & 0x10) ? 1 : 0;
                lcr->lcr[0] = lcr->lcr[1] = 0;
                for (i = 2; i <= 23; i++) {
                        u8 v = lcr->raw ? 0 : saa7115_read(client, i - 2 + 0x41);

                        lcr->lcr[i] = lcr2vbi[v >> 4] << 4 |
                                      lcr2vbi[v & 0xF];
                }
                break;
        }

	case DECODER_SET_VBI:
	{
                static u8 vbi2lcr[] = {
                        0xF, 1, 0, 0,     // 1 = Teletext
                        4,   5, 0, 7,     // 4 = CC, 5 = WSS, 7 = VPS
                        0,   0, 0, 0,
                        0,   0, 0, 0
                };
                struct decoder_lcr *declcr = (struct decoder_lcr *)arg;
		int is_pal = (state->norm != VIDEO_MODE_NTSC);
                u8 lcr[24];
		int i;

                for (i = 0; i <= 23; i++)
                        lcr[i] = 0xFF;
                if (declcr->raw) {
                        for (i = 0; i <= 23; i++)
                                declcr->lcr[i] = 0;
                        if (is_pal)
                                for (i = 6; i <= 23; i++)
                                        lcr[i] = 0xDD;
                        else
                                for (i = 10; i <= 21; i++)
                                        lcr[i] = 0xDD;
                }
                else {
                        if (is_pal) {
                                for (i = 0; i <= 5; i++)
                                        declcr->lcr[i] = 0;
                        }
                        else {
                                for (i = 0; i <= 9; i++)
                                        declcr->lcr[i] = 0;
                                for (i = 22; i <= 23; i++)
                                        declcr->lcr[i] = 0;
                        }
                        for (i = 6; i <= 23; i++)
                                lcr[i] = vbi2lcr[declcr->lcr[i] >> 4] << 4 |
                                         vbi2lcr[declcr->lcr[i] & 0xF];
                }
                for (i = 2; i <= 23; i++) {
                        saa7115_write(client, i - 2 + 0x41, lcr[i]);
                }
                writeregs(client, declcr->raw ? cfg_saa7115_vbi_on : cfg_saa7115_vbi_off);
                break;
	}

        case DECODER_DECODE_VBI_LINE: {
                struct decode_vbi_line *vbi = (struct decode_vbi_line *)arg;
                u8 *p = vbi->p;
                u32 wss;
                int id1, id2, l, err = 0;

                id1 = p[2];
                if (state->norm == VIDEO_MODE_NTSC)
                        id1 ^= 0x40;
                id2 = p[3];
                p += 4;
                l = (id1 & 0x3f) << 3;
                l |= (id2 & 0x70) >> 4;
                id2 &= 0xf;
                if (!has_vbi_data(p)) {
                        err = 1;
                }
                switch (id2) {
                case 1:
                        id2 = V4L2_SLICED_TELETEXT_B;
                        break;
                case 4:
                        id2 = V4L2_SLICED_CAPTION_525;
                        err = !odd_parity(p[0]) || !odd_parity(p[1]);
                        break;
                case 5:
                        id2 = V4L2_SLICED_WSS_625;
                        wss = decode_wss(p);
                        if (wss == -1) {
                                err = 1;
                        } else {
                                p[0] = wss & 0xff;
                                p[1] = wss >> 8;
                        }
                        break;
                case 7:
                        id2 = V4L2_SLICED_VPS;
                        if (decode_vps(p, p) != 0) {
                                err = 1;
                        }
                        break;
                default:
                        id2 = 0;
                        err = 1;
                        break;
                }
                vbi->type = err ? 0 : id2;
                vbi->line = err ? 0 : l;
                vbi->is_even_field = err ? 0 : ((id1 & 0x40) != 0);
                vbi->p = p;
                break;
	}

	case DECODER_SET_STATE:
	{
		struct decoder_state *pic = arg;

		SAA7115_DEBUG("decoder set picture bright=%d contrast=%d saturation=%d hue=%d\n",
			pic->bright, pic->contrast, pic->sat, pic->hue);

		if (state->bright != pic->bright) {
			/* We want 0 to 255 */
			if (pic->bright < 0 || pic->bright > 255) {
				SAA7115_ERR
				    ("invalid brightness setting %d\n",
				     pic->bright);
				return -EINVAL;
			}
			state->bright = pic->bright;
			saa7115_write(client, 0x0a, state->bright);
		}
		if (state->contrast != pic->contrast) {
			/* We want 0 to 127 */
			if (pic->contrast < 0 || pic->contrast > 127) {
				SAA7115_ERR("invalid contrast setting %d\n",
						 pic->contrast);
				return -EINVAL;
			}
			state->contrast = pic->contrast;
			saa7115_write(client, 0x0b, state->contrast);
		}
		if (state->sat != pic->sat) {
			/* We want 0 to 127 */
			if (pic->sat < 0 || pic->sat > 127) {
				SAA7115_ERR
				    ("invalid saturation setting %d\n", pic->sat);
				return -EINVAL;
			}
			state->sat = pic->sat;
			saa7115_write(client, 0x0c, state->sat);
		}
		if (state->hue != pic->hue) {
			/* We want -128 to 127 */
			if (pic->hue < -128 || pic->hue > 127) {
				SAA7115_ERR("invalid hue setting %d\n",
						 pic->hue);
				return -EINVAL;
			}
			state->hue = pic->hue;
			saa7115_write(client, 0x0d, state->hue);
		}
		break;
	}
	case DECODER_RESET:
	{
		SAA7115_DEBUG("decoder RESET\n");

		writeregs(client, cfg_saa7115_reset_scaler);
		break;
	}

	default:
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
#ifndef NEW_I2C
  /* pre i2c-2.8.0 */

static void saa7115_inc_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif /* MODULE */
}

static void saa7115_dec_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif /* MODULE */
}

#else
	/* i2c-2.8.0 and later */
#endif /* NEW_I2C */

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] =
    { I2C_SAA7114 >> 1, I2C_SAA7114A >> 1, I2C_CLIENT_END };
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) */
I2C_CLIENT_INSMOD;

struct i2c_driver i2c_driver_saa7115;

static int saa7115_detect_client(struct i2c_adapter *adapter, int address,
#ifndef LINUX26
				 unsigned short flags,
#endif /* LINUX26 */
				 int kind)
{
	struct i2c_client *client;
	struct decoder_state *state;

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return -ENOMEM;
	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver_saa7115;
	client->flags = I2C_CLIENT_ALLOW_USE;
	snprintf(client->name, sizeof(client->name) - 1, "saa7115");

	SAA7115_INFO("saa7115 found @ 0x%x (%s)\n", address << 1, adapter->name);

	state = kmalloc(sizeof(struct decoder_state), GFP_KERNEL);
	i2c_set_clientdata(client, state);
	if (state == NULL) {
		kfree(client);
		return -ENOMEM;
	}
	memset(state, 0, sizeof(struct decoder_state));
	state->norm = VIDEO_MODE_NTSC;
	state->input = -1;
	state->enable = 1;
	state->bright = 128;
	state->contrast = 64;
	state->hue = 0;
	state->sat = 64;
	state->playback = 0;	// initially capture mode used
	state->audio = DECODER_AUDIO_48_KHZ;

	SAA7115_DEBUG("writing init values\n");

	/* init to NTSC/48khz */
	writeregs(client, init_saa7115_auto_input);
	writeregs(client, init_saa7115_misc);
	writeregs(client, cfg_saa7115_NTSC_fullres_x);
	writeregs(client, cfg_saa7115_NTSC_fullres_y);
	writeregs(client, cfg_saa7115_NTSC_video);
	writeregs(client, cfg_saa7115_48_audio);
	writeregs(client, cfg_saa7115_NTSC_48_audio);
	writeregs(client, cfg_saa7115_reset_scaler);

	/*saa7115_sleep_timeout(2*HZ); */

	i2c_attach_client(client);

	SAA7115_DEBUG("status: (1E) 0x%02x, (1F) 0x%02x\n",
		saa7115_read(client, 0x1e), saa7115_read(client, 0x1f));

	return 0;
}

static int saa7115_attach_adapter(struct i2c_adapter *adapter)
{
	return i2c_probe(adapter, &addr_data, &saa7115_detect_client);
}

static int saa7115_detach_client(struct i2c_client *client)
{
	struct decoder_state *state = i2c_get_clientdata(client);
	int err;

	err = i2c_detach_client(client);
	if (err) {
		return err;
	}

	kfree(state);
	kfree(client);
	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
struct i2c_driver i2c_driver_saa7115 = {
	.name = "saa7115",

	.id = I2C_DRIVERID_SAA7115,
	.flags = I2C_DF_NOTIFY,

	.attach_adapter = saa7115_attach_adapter,
	.detach_client = saa7115_detach_client,
	.command = saa7115_command,
#ifndef NEW_I2C
/* pre i2c-2.8.0 */
	.inc_use = saa7115_inc_use,
	.dec_use = saa7115_dec_use,
#else
/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif /* NEW_I2C */
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif /* LINUX26 */

static int __init saa7115_init(void)
{
	return i2c_add_driver(&i2c_driver_saa7115);
}

static void __exit saa7115_exit(void)
{
	i2c_del_driver(&i2c_driver_saa7115);
}

module_init(saa7115_init);
module_exit(saa7115_exit);
