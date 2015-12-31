/*
 * programming the msp34* sound processor family
 *
 * (c) 1997-2001 Gerd Knorr <kraxel@bytesex.org>
 *
 * what works and what doesn't:
 *
 *  AM-Mono
 *      Support for Hauppauge cards added (decoding handled by tuner) added by
 *      Frederic Crozat <fcrozat@mail.dotcom.fr>
 *
 *  FM-Mono
 *      should work. The stereo modes are backward compatible to FM-mono,
 *      therefore FM-Mono should be allways available.
 *
 *  FM-Stereo (B/G, used in germany)
 *      should work, with autodetect
 *
 *  FM-Stereo (satellite)
 *      should work, no autodetect (i.e. default is mono, but you can
 *      switch to stereo -- untested)
 *
 *  NICAM (B/G, L , used in UK, Scandinavia, Spain and France)
 *      should work, with autodetect. Support for NICAM was added by
 *      Pekka Pietikainen <pp@netppl.fi>
 *
 *
 * TODO:
 *   - better SAT support
 *
 *
 * 980623  Thomas Sailer (sailer@ife.ee.ethz.ch)
 *         using soundcore instead of OSS
 *
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/smp_lock.h>

#include <asm/pgtable.h>
#include <asm/semaphore.h>

#include "compat.h"
#include "audiochip.h"
#include "msp3400.h"

/* Addresses to scan */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c[] = { I2C_CLIENT_END };
static unsigned short normal_i2c_range[] = { 0x40, 0x40, I2C_CLIENT_END };
#else
static unsigned short normal_i2c[] = { 
  0x40,
I2C_CLIENT_END 
};
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) */

I2C_CLIENT_INSMOD;

/* insmod parameters */
static int debug = 0;		/* debug output */
static int once = 0;		/* no continous stereo monitoring */
static int amsound = 0;		/* hard-wire AM sound at 6.5 Hz (france),
				   the autoscan seems work well only with FM... */
static int standard = 1;	/* Override auto detect of audio standard, if needed. */
static int simple = -1;		/* use short programming (>= msp3410 only) */
static int simpler = -1;	/* use shorter programming (>= msp34xxG) */
static int stereo_threshold = 0x190;	/* a2 threshold for stereo/bilingual (msp34xxg only) 0x00a0-0x03c0 */
static int dolby = 0;

#define DFP_COUNT 0x41
static const int bl_dfp[] = {
	0x00, 0x01, 0x02, 0x03, 0x06, 0x08, 0x09, 0x0a,
	0x0b, 0x0d, 0x0e, 0x10
};

#define IS_MSP34XX_G(msp) ((msp)->simpler)
struct msp3400c {
	int simple;
	int simpler;
	int source;		/* see msp34xxg_set_source */
	int nicam;
	int mode;
	int norm;
	int stereo;
	int nicam_on;
	int acb;
	int main, second;	/* sound carrier */
	int input;

	int muted;
	int left, right;	/* volume */
	int bass, treble;

	/* shadow register set */
	int dfp_regs[DFP_COUNT];

	/* thread */
	struct task_struct *thread;
	wait_queue_head_t wq;

	struct semaphore *notify;
	int active, restart, rmmod;

	int watch_stereo;
	struct timer_list wake_stereo;
};

#define MSP3400_MAX 4
static struct i2c_client *msps[MSP3400_MAX];

#define VIDEO_MODE_RADIO 16	/* norm magic for radio mode */

/* ---------------------------------------------------------------------- */

#define dprintk     if (debug>=1) printk
#define dprintk_trace     if (debug>=16) printk

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(once, int, 0644);
module_param(debug, int, 0644);
module_param(simple, int, 0644);
module_param(simpler, int, 0644);
module_param(stereo_threshold, int, 0644);
module_param(standard, int, 0644);
module_param(amsound, int, 0644);
module_param(dolby, int, 0644);
#else
MODULE_PARM(once, "i");
MODULE_PARM(debug, "i");
MODULE_PARM(simple, "i");
MODULE_PARM(simpler, "i");
MODULE_PARM(stereo_threshold, "i");
MODULE_PARM(standard, "i");
MODULE_PARM(amsound, "i");
MODULE_PARM(dolby, "i");
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) */

MODULE_DESCRIPTION("device driver for msp34xx TV sound processor");
MODULE_AUTHOR("Gerd Knorr");
MODULE_LICENSE("GPL");

/* ---------------------------------------------------------------------- */

#define I2C_MSP3400C       0x80
#define I2C_MSP3400C_DEM   0x10
#define I2C_MSP3400C_DFP   0x12

/* ----------------------------------------------------------------------- */
/* functions for talking to the MSP3400C Sound processor                   */

#ifndef I2C_M_IGNORE_NAK
# define I2C_M_IGNORE_NAK 0x1000
#endif /* I2C_M_IGNORE_NAK */

static int msp3400c_reset(struct i2c_client *client)
{
	/* reset and read revision code */
	static char reset_off[3] = { 0x00, 0x80, 0x00 };
	static char reset_on[3] = { 0x00, 0x00, 0x00 };
	static char write[3] = { I2C_MSP3400C_DFP + 1, 0x00, 0x1e };
	char read[2];
	struct i2c_msg reset[2] = {
		{client->addr, I2C_M_IGNORE_NAK, 3, reset_off},
		{client->addr, I2C_M_IGNORE_NAK, 3, reset_on},
	};
	struct i2c_msg test[2] = {
		{client->addr, 0, 3, write},
		{client->addr, I2C_M_RD, 2, read},
	};

	dprintk_trace("trace: msp3400c_reset\n");
	if ((1 != i2c_transfer(client->adapter, &reset[0], 1)) ||
	    (1 != i2c_transfer(client->adapter, &reset[1], 1)) ||
	    (2 != i2c_transfer(client->adapter, test, 2))) {
		printk(KERN_ERR "msp3400: chip reset failed\n");
		return -1;
	}
	return 0;
}

static int msp3400c_read(struct i2c_client *client, int dev, int addr)
{
	int err, retval;

	unsigned char write[3];
	unsigned char read[2];
	struct i2c_msg msgs[2] = {
		{client->addr, 0, 3, write},
		{client->addr, I2C_M_RD, 2, read}
	};
	write[0] = dev + 1;
	write[1] = addr >> 8;
	write[2] = addr & 0xff;

	for (err = 0; err < 3;) {
		if (2 == i2c_transfer(client->adapter, msgs, 2))
			break;
		err++;
		printk(KERN_WARNING
		       "msp34xx: I/O error #%d (read 0x%02x/0x%02x)\n", err,
		       dev, addr);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ / 10);
	}
	if (3 == err) {
		printk(KERN_WARNING
		       "msp34xx: giving up, reseting chip. Sound will go off, sorry folks :-|\n");
		msp3400c_reset(client);
		return -1;
	}
	retval = read[0] << 8 | read[1];
	dprintk_trace("trace: msp3400c_read(0x%x, 0x%x): 0x%x\n", dev, addr,
		      retval);
	return retval;
}

static int msp3400c_write(struct i2c_client *client, int dev, int addr, int val)
{
	int err;
	unsigned char buffer[5];

	buffer[0] = dev;
	buffer[1] = addr >> 8;
	buffer[2] = addr & 0xff;
	buffer[3] = val >> 8;
	buffer[4] = val & 0xff;

	dprintk_trace("trace: msp3400c_write(0x%x, 0x%x, 0x%x)\n", dev, addr,
		      val);
	for (err = 0; err < 3;) {
		if (5 == i2c_master_send(client, buffer, 5))
			break;
		err++;
		printk(KERN_WARNING
		       "msp34xx: I/O error #%d (write 0x%02x/0x%02x)\n", err,
		       dev, addr);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ / 10);
	}
	if (3 == err) {
		printk(KERN_WARNING
		       "msp34xx: giving up, reseting chip. Sound will go off, sorry folks :-|\n");
		msp3400c_reset(client);
		return -1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

/* This macro is allowed for *constants* only, gcc must calculate it
   at compile time.  Remember -- no floats in kernel mode */
#define MSP_CARRIER(freq) ((int)((float)(freq/18.432)*(1<<24)))

#define MSP_MODE_AM_DETECT   0
#define MSP_MODE_FM_RADIO    2
#define MSP_MODE_FM_TERRA    3
#define MSP_MODE_FM_SAT      4
#define MSP_MODE_FM_NICAM1   5
#define MSP_MODE_FM_NICAM2   6
#define MSP_MODE_AM_NICAM    7
#define MSP_MODE_BTSC        8
#define MSP_MODE_EXTERN      9

static struct MSP_INIT_DATA_DEM {
	int fir1[6];
	int fir2[6];
	int cdo1;
	int cdo2;
	int ad_cv;
	int mode_reg;
	int dfp_src;
	int dfp_matrix;
} msp_init_data[] = {
	/* AM (for carrier detect / msp3400) */
	{ {
	75, 19, 36, 35, 39, 40}, {
	75, 19, 36, 35, 39, 40},
	    MSP_CARRIER(5.5), MSP_CARRIER(5.5), 0x00d0, 0x0500, 0x0020, 0x3000},
	    /* AM (for carrier detect / msp3410) */
	{ {
	-1, -1, -8, 2, 59, 126}, {
	-1, -1, -8, 2, 59, 126},
	    MSP_CARRIER(5.5), MSP_CARRIER(5.5), 0x00d0, 0x0100, 0x0020, 0x3000},
	    /* FM Radio */
	{ {
	-8, -8, 4, 6, 78, 107}, {
	-8, -8, 4, 6, 78, 107},
	    MSP_CARRIER(10.7), MSP_CARRIER(10.7),
	    0x00d0, 0x0480, 0x0020, 0x3000},
	    /* Terrestial FM-mono + FM-stereo */
	{ {
	3, 18, 27, 48, 66, 72}, {
	3, 18, 27, 48, 66, 72},
	    MSP_CARRIER(5.5), MSP_CARRIER(5.5), 0x00d0, 0x0480, 0x0030, 0x3000},
	    /* Sat FM-mono */
	{ {
	1, 9, 14, 24, 33, 37}, {
	3, 18, 27, 48, 66, 72},
	    MSP_CARRIER(6.5), MSP_CARRIER(6.5), 0x00c6, 0x0480, 0x0000, 0x3000},
	    /* NICAM/FM --  B/G (5.5/5.85), D/K (6.5/5.85) */
	{ {
	-2, -8, -10, 10, 50, 86}, {
	3, 18, 27, 48, 66, 72},
	    MSP_CARRIER(5.5), MSP_CARRIER(5.5), 0x00d0, 0x0040, 0x0120, 0x3000},
	    /* NICAM/FM -- I (6.0/6.552) */
	{ {
	2, 4, -6, -4, 40, 94}, {
	3, 18, 27, 48, 66, 72},
	    MSP_CARRIER(6.0), MSP_CARRIER(6.0), 0x00d0, 0x0040, 0x0120, 0x3000},
	    /* NICAM/AM -- L (6.5/5.85) */
	{ {
	-2, -8, -10, 10, 50, 86}, {
-4, -12, -9, 23, 79, 126},
	    MSP_CARRIER(6.5), MSP_CARRIER(6.5),
	    0x00c6, 0x0140, 0x0120, 0x7c03},};

struct CARRIER_DETECT {
	int cdo;
	char *name;
};

static struct CARRIER_DETECT carrier_detect_main[] = {
	/* main carrier */
	{MSP_CARRIER(4.5), "4.5   NTSC"},
	{MSP_CARRIER(5.5), "5.5   PAL B/G"},
	{MSP_CARRIER(6.0), "6.0   PAL I"},
	{MSP_CARRIER(6.5), "6.5   PAL D/K + SAT + SECAM"}
};

static struct CARRIER_DETECT carrier_detect_55[] = {
	/* PAL B/G */
	{MSP_CARRIER(5.7421875), "5.742 PAL B/G FM-stereo"},
	{MSP_CARRIER(5.85), "5.85  PAL B/G NICAM"}
};

static struct CARRIER_DETECT carrier_detect_65[] = {
	/* PAL SAT / SECAM */
	{MSP_CARRIER(5.85), "5.85  PAL D/K + SECAM NICAM"},
	{MSP_CARRIER(6.2578125), "6.25  PAL D/K1 FM-stereo"},
	{MSP_CARRIER(6.7421875), "6.74  PAL D/K2 FM-stereo"},
	{MSP_CARRIER(7.02), "7.02  PAL SAT FM-stereo s/b"},
	{MSP_CARRIER(7.20), "7.20  PAL SAT FM-stereo s"},
	{MSP_CARRIER(7.38), "7.38  PAL SAT FM-stereo b"},
};

#define CARRIER_COUNT(x) (sizeof(x)/sizeof(struct CARRIER_DETECT))

/* ----------------------------------------------------------------------- */

static int scarts[3][9] = {
	/* MASK    IN1     IN2     IN1_DA  IN2_DA  IN3     IN4     MONO    MUTE   */
	{0x0320, 0x0000, 0x0200, -1, -1, 0x0300, 0x0020, 0x0100, 0x0320},
	{0x0c40, 0x0440, 0x0400, 0x0c00, 0x0040, 0x0000, 0x0840, 0x0800,
	 0x0c40},
	{0x3080, 0x1000, 0x1080, 0x0000, 0x0080, 0x2080, 0x3080, 0x2000,
	 0x3000},
};

static char *scart_names[] = {
	"mask", "in1", "in2", "in1 da", "in2 da", "in3", "in4", "mono", "mute"
};

static void msp3400c_set_scart(struct i2c_client *client, int in, int out)
{
	struct msp3400c *msp = i2c_get_clientdata(client);

	if (-1 == scarts[out][in])
		return;

	dprintk("msp34xx: scart switch: %s => %d\n", scart_names[in], out);
	msp->acb &= ~scarts[out][SCART_MASK];
	msp->acb |= scarts[out][in];
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0013, msp->acb);
}

/* ------------------------------------------------------------------------ */

static void msp3400c_setcarrier(struct i2c_client *client, int cdo1, int cdo2)
{
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0093, cdo1 & 0xfff);
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x009b, cdo1 >> 12);
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x00a3, cdo2 & 0xfff);
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x00ab, cdo2 >> 12);
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0056, 0);	/*LOAD_REG_1/2 */
}

static void msp3400c_setvolume(struct i2c_client *client,
			       int muted, int left, int right)
{
	int vol = 0, val = 0, balance = 0;

	if (!muted) {
		vol = (left > right) ? left : right;
		val = (vol * 0x7F / 65535) << 8;
	}
	if (vol > 0) {
		balance = ((right - left) * 127) / vol;
	}

	dprintk("msp34xx: setvolume: mute=%s %d:%d  v=0x%02x b=0x%02x\n",
		muted ? "on" : "off", left, right, val >> 8, balance);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0000, val);	/* loudspeaker */
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0006, val);	/* headphones  */
	// scart - on/off only - AEW why? undone NOTE values below
	// 40000 are mostly useless, 59343 is a good default (0x73)
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0007,
		       muted ? 0x1 : (val | 0x1));
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0001, balance << 8);
}

static void msp3400c_setbass(struct i2c_client *client, int bass)
{
	int val = ((bass - 32768) * 0x60 / 65535) << 8;

	dprintk("msp34xx: setbass: %d 0x%02x\n", bass, val >> 8);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0002, val);	/* loudspeaker */
}

static void msp3400c_settreble(struct i2c_client *client, int treble)
{
	int val = ((treble - 32768) * 0x60 / 65535) << 8;

	dprintk("msp34xx: settreble: %d 0x%02x\n", treble, val >> 8);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0003, val);	/* loudspeaker */
}

static void msp3400c_setmode(struct i2c_client *client, int type)
{
	struct msp3400c *msp = i2c_get_clientdata(client);
	int i;

	dprintk("msp3400: setmode: %d\n", type);
	msp->mode = type;
	msp->stereo = VIDEO_SOUND_MONO;

	msp3400c_write(client, I2C_MSP3400C_DEM, 0x00bb,	/* ad_cv */
		       msp_init_data[type].ad_cv);

	for (i = 5; i >= 0; i--)	/* fir 1 */
		msp3400c_write(client, I2C_MSP3400C_DEM, 0x0001,
			       msp_init_data[type].fir1[i]);

	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0005, 0x0004);	/* fir 2 */
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0005, 0x0040);
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0005, 0x0000);
	for (i = 5; i >= 0; i--)
		msp3400c_write(client, I2C_MSP3400C_DEM, 0x0005,
			       msp_init_data[type].fir2[i]);

	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0083,	/* MODE_REG */
		       msp_init_data[type].mode_reg);

	msp3400c_setcarrier(client, msp_init_data[type].cdo1,
			    msp_init_data[type].cdo2);

	msp3400c_write(client, I2C_MSP3400C_DEM, 0x0056, 0);	/*LOAD_REG_1/2 */

	if (dolby) {
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0008, 0x0520);	/* I2S1 */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0009, 0x0620);	/* I2S2 */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x000b,
			       msp_init_data[type].dfp_src);
	} else {
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0008,
			       msp_init_data[type].dfp_src);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0009,
			       msp_init_data[type].dfp_src);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x000b,
			       msp_init_data[type].dfp_src);
	}
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x000a,
		       msp_init_data[type].dfp_src);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x000e,
		       msp_init_data[type].dfp_matrix);

	if (msp->nicam) {
		/* nicam prescale */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0010, 0x5a00);	/* was: 0x3000 */
	}
}

// given a bitmask of VIDEO_SOUND_XXX returns the "best" in the bitmask
static int best_video_sound(int mode)
{
	int ret_cap = VIDEO_SOUND_MONO;
	if (mode & VIDEO_SOUND_STEREO) {
		ret_cap = VIDEO_SOUND_STEREO;
	} else if (mode & VIDEO_SOUND_LANG1) {
		ret_cap = VIDEO_SOUND_LANG1;
	} else if (mode & VIDEO_SOUND_LANG2) {
		ret_cap = VIDEO_SOUND_LANG2;
	}
	return ret_cap;
}

/* turn on/off nicam + stereo */
static void msp3400c_setstereo(struct i2c_client *client, int mode)
{
	static char *strmode[] = { "0", "mono", "stereo", "3",
		"lang1", "5", "6", "7", "lang2"
	};
	struct msp3400c *msp = i2c_get_clientdata(client);
	int nicam = 0;		/* channel source: FM/AM or nicam */
	int src = 0;

	if (IS_MSP34XX_G(msp)) {
		/* this method would break everything, let's make sure 
		 * it's never called
		 */
		dprintk
		    ("msp34xxg: DEBUG WARNING setstereo called with mode=%d instead of set_source (ignored)\n",
		     mode);
		return;
	}

	/* switch demodulator */
	switch (msp->mode) {
	case MSP_MODE_FM_TERRA:
		dprintk("msp3400: FM setstereo: %s\n", strmode[mode]);
		msp3400c_setcarrier(client, msp->second, msp->main);
		switch (mode) {
		case VIDEO_SOUND_STEREO:
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x000e,
				       0x3001);
			break;
		case VIDEO_SOUND_MONO:
		case VIDEO_SOUND_LANG1:
		case VIDEO_SOUND_LANG2:
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x000e,
				       0x3000);
			break;
		}
		break;
	case MSP_MODE_FM_SAT:
		dprintk("msp3400: SAT setstereo: %s\n", strmode[mode]);
		switch (mode) {
		case VIDEO_SOUND_MONO:
			msp3400c_setcarrier(client, MSP_CARRIER(6.5),
					    MSP_CARRIER(6.5));
			break;
		case VIDEO_SOUND_STEREO:
			msp3400c_setcarrier(client, MSP_CARRIER(7.2),
					    MSP_CARRIER(7.02));
			break;
		case VIDEO_SOUND_LANG1:
			msp3400c_setcarrier(client, MSP_CARRIER(7.38),
					    MSP_CARRIER(7.02));
			break;
		case VIDEO_SOUND_LANG2:
			msp3400c_setcarrier(client, MSP_CARRIER(7.38),
					    MSP_CARRIER(7.02));
			break;
		}
		break;
	case MSP_MODE_FM_NICAM1:
	case MSP_MODE_FM_NICAM2:
	case MSP_MODE_AM_NICAM:
		dprintk("msp3400: NICAM setstereo: %s\n", strmode[mode]);
		msp3400c_setcarrier(client, msp->second, msp->main);
		if (msp->nicam_on)
			nicam = 0x0100;
		break;
	case MSP_MODE_BTSC:
		dprintk("msp3400: BTSC setstereo: %s\n", strmode[mode]);
		nicam = 0x0300;
		break;
	case MSP_MODE_EXTERN:
		dprintk("msp3400: extern setstereo: %s\n", strmode[mode]);
		nicam = 0x0200;
		break;
	case MSP_MODE_FM_RADIO:
		dprintk("msp3400: FM-Radio setstereo: %s\n", strmode[mode]);
		break;
	default:
		dprintk("msp3400: mono setstereo\n");
		return;
	}

	/* switch audio */
	switch (best_video_sound(mode)) {
	case VIDEO_SOUND_STEREO:
		src = 0x0020 | nicam;
#if 0
		/* spatial effect */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0005, 0x4000);
#endif
		break;
	case VIDEO_SOUND_MONO:
		if (msp->mode == MSP_MODE_AM_NICAM) {
			dprintk("msp3400: switching to AM mono\n");
			/* AM mono decoding is handled by tuner, not MSP chip */
			/* SCART switching control register */
			msp3400c_set_scart(client, SCART_MONO, 0);
			src = 0x0200;
			break;
		}
	case VIDEO_SOUND_LANG1:
		src = 0x0000 | nicam;
		break;
	case VIDEO_SOUND_LANG2:
		src = 0x0010 | nicam;
		break;
	}
	dprintk("msp3400: setstereo final source/matrix = 0x%x\n", src);

	if (dolby) {
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0008, 0x0520);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0009, 0x0620);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x000a, src);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x000b, src);
	} else {
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0008, src);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0009, src);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x000a, src);
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x000b, src);
	}
}

static void msp3400c_print_mode(struct msp3400c *msp)
{
	if (msp->main == msp->second) {
		printk("msp3400: mono sound carrier: %d.%03d MHz\n",
		       msp->main / 910000, (msp->main / 910) % 1000);
	} else {
		printk("msp3400: main sound carrier: %d.%03d MHz\n",
		       msp->main / 910000, (msp->main / 910) % 1000);
	}
	if (msp->mode == MSP_MODE_FM_NICAM1 || msp->mode == MSP_MODE_FM_NICAM2)
		printk("msp3400: NICAM/FM carrier   : %d.%03d MHz\n",
		       msp->second / 910000, (msp->second / 910) % 1000);
	if (msp->mode == MSP_MODE_AM_NICAM)
		printk("msp3400: NICAM/AM carrier   : %d.%03d MHz\n",
		       msp->second / 910000, (msp->second / 910) % 1000);
	if (msp->mode == MSP_MODE_FM_TERRA && msp->main != msp->second) {
		printk("msp3400: FM-stereo carrier : %d.%03d MHz\n",
		       msp->second / 910000, (msp->second / 910) % 1000);
	}
}

static void msp3400c_restore_dfp(struct i2c_client *client)
{
	struct msp3400c *msp = i2c_get_clientdata(client);
	int i;

	for (i = 0; i < DFP_COUNT; i++) {
		if (-1 == msp->dfp_regs[i])
			continue;
		msp3400c_write(client, I2C_MSP3400C_DFP, i, msp->dfp_regs[i]);
	}
}

/* if the dfp_regs is set, set what's in there. Otherwise, set the default value */
static int
msp3400c_write_dfp_with_default(struct i2c_client *client, int addr,
				int default_value)
{
	struct msp3400c *msp = i2c_get_clientdata(client);
	int value = default_value;
	if (addr < DFP_COUNT && -1 != msp->dfp_regs[addr])
		value = msp->dfp_regs[addr];
	return msp3400c_write(client, I2C_MSP3400C_DFP, addr, value);
}

/* ----------------------------------------------------------------------- */

struct REGISTER_DUMP {
	int addr;
	char *name;
};

struct REGISTER_DUMP d1[] = {
	{0x007e, "autodetect"},
	{0x0023, "C_AD_BITS "},
	{0x0038, "ADD_BITS  "},
	{0x003e, "CIB_BITS  "},
	{0x0057, "ERROR_RATE"},
};

static int autodetect_stereo(struct i2c_client *client)
{
	struct msp3400c *msp = i2c_get_clientdata(client);
	int val;
	int newstereo = msp->stereo;
	int newnicam = msp->nicam_on;
	int update = 0;

	switch (msp->mode) {
	case MSP_MODE_FM_TERRA:
		val = msp3400c_read(client, I2C_MSP3400C_DFP, 0x18);
		if (val > 32767)
			val -= 65536;
		dprintk("msp34xx: stereo detect register: %d\n", val);

		if (val > 4096) {
			newstereo = VIDEO_SOUND_STEREO | VIDEO_SOUND_MONO;
		} else if (val < -4096) {
			newstereo = VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
		} else {
			newstereo = VIDEO_SOUND_MONO;
		}
		newnicam = 0;
		break;
	case MSP_MODE_FM_NICAM1:
	case MSP_MODE_FM_NICAM2:
	case MSP_MODE_AM_NICAM:
		val = msp3400c_read(client, I2C_MSP3400C_DEM, 0x23);
		dprintk("msp34xx: nicam sync=%d, mode=%d\n", val & 1,
			(val & 0x1e) >> 1);

		if (val & 1) {
			/* nicam synced */
			switch ((val & 0x1e) >> 1) {
			case 0:
			case 8:
				newstereo = VIDEO_SOUND_STEREO;
				break;
			case 1:
			case 9:
				newstereo = VIDEO_SOUND_MONO
				    | VIDEO_SOUND_LANG1;
				break;
			case 2:
			case 10:
				newstereo = VIDEO_SOUND_MONO
				    | VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
				break;
			default:
				newstereo = VIDEO_SOUND_MONO;
				break;
			}
			newnicam = 1;
		} else {
			newnicam = 0;
			newstereo = VIDEO_SOUND_MONO;
		}
		break;
	case MSP_MODE_BTSC:
		val = msp3400c_read(client, I2C_MSP3400C_DEM, 0x200);
		dprintk("msp3410: status=0x%x (pri=%s, sec=%s, %s%s%s)\n",
			val,
			(val & 0x0002) ? "no" : "yes",
			(val & 0x0004) ? "no" : "yes",
			(val & 0x0040) ? "stereo" : "mono",
			(val & 0x0080) ? ", nicam 2nd mono" : "",
			(val & 0x0100) ? ", bilingual/SAP" : "");
		newstereo = VIDEO_SOUND_MONO;
		if (val & 0x0040)
			newstereo |= VIDEO_SOUND_STEREO;
		if (val & 0x0100)
			newstereo |= VIDEO_SOUND_LANG1;
		break;
	}
	if (newstereo != msp->stereo) {
		update = 1;
		dprintk("msp34xx: watch: stereo %d => %d\n",
			msp->stereo, newstereo);
		msp->stereo = newstereo;
	}
	if (newnicam != msp->nicam_on) {
		update = 1;
		dprintk("msp34xx: watch: nicam %d => %d\n",
			msp->nicam_on, newnicam);
		msp->nicam_on = newnicam;
	}
	return update;
}

/*
 * A kernel thread for msp3400 control -- we don't want to block the
 * in the ioctl while doing the sound carrier & stereo detect
 */

static void msp3400c_stereo_wake(unsigned long data)
{
	struct msp3400c *msp = (struct msp3400c *)data;	/* XXX alpha ??? */

	wake_up_interruptible(&msp->wq);
}

/* stereo/multilang monitoring */
static void watch_stereo(struct i2c_client *client)
{
	struct msp3400c *msp = i2c_get_clientdata(client);

	if (autodetect_stereo(client)) {
		if (msp->stereo & VIDEO_SOUND_STEREO)
			msp3400c_setstereo(client, VIDEO_SOUND_STEREO);
		else if (msp->stereo & VIDEO_SOUND_LANG1)
			msp3400c_setstereo(client, VIDEO_SOUND_LANG1);
		else
			msp3400c_setstereo(client, VIDEO_SOUND_MONO);
	}
	if (once)
		msp->watch_stereo = 0;
	if (msp->watch_stereo)
		mod_timer(&msp->wake_stereo, jiffies + 5 * HZ);
}

static int msp3400c_thread(void *data)
{
	DEFINE_WAIT(wait);
	struct i2c_client *client = data;
	struct msp3400c *msp = i2c_get_clientdata(client);

	struct CARRIER_DETECT *cd;
	int count, max1, max2, val1, val2, val, this;

#ifndef LINUX26
	daemonize();
	exit_files(current);
	reparent_to_init();

	spin_lock_irq(SIGMASK_LOCK(current));
	sigfillset(&current->blocked);
	spin_unlock_irq(SIGMASK_LOCK(current));
	strcpy(current->comm, "msp3400");

	msp->thread = current;

#else				/* LINUX26 */
	daemonize("msp3400");
	msp->thread = current;
#endif				/* LINUX26 */

	printk("msp3400: daemon started\n");
	if (msp->notify != NULL)
		up(msp->notify);

	for (;;) {
		if (msp->rmmod)
			goto done;
		if (debug > 1)
			printk("msp3400: thread: sleep\n");
		prepare_to_wait(&msp->wq, &wait, TASK_INTERRUPTIBLE);
		schedule();
		finish_wait(&msp->wq, &wait);

		if (debug > 1)
			printk("msp3400: thread: wakeup\n");
		if (msp->rmmod || signal_pending(current))
			goto done;

		if (VIDEO_MODE_RADIO == msp->norm ||
		    MSP_MODE_EXTERN == msp->mode)
			continue;	/* nothing to do */

		msp->active = 1;

		if (msp->watch_stereo) {
			watch_stereo(client);
			msp->active = 0;
			continue;
		}

		/* some time for the tuner to sync */
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ / 5);
		if (signal_pending(current))
			goto done;

	      restart:
		if (VIDEO_MODE_RADIO == msp->norm ||
		    MSP_MODE_EXTERN == msp->mode)
			continue;	/* nothing to do */
		msp->restart = 0;
		msp3400c_setvolume(client, msp->muted, 0, 0);
		msp3400c_setmode(client, MSP_MODE_AM_DETECT /* +1 */ );
		val1 = val2 = 0;
		max1 = max2 = -1;
		del_timer(&msp->wake_stereo);
		msp->watch_stereo = 0;

		/* carrier detect pass #1 -- main carrier */
		cd = carrier_detect_main;
		count = CARRIER_COUNT(carrier_detect_main);

		if (amsound && (msp->norm == VIDEO_MODE_SECAM)) {
			/* autodetect doesn't work well with AM ... */
			max1 = 3;
			count = 0;
			dprintk("msp3400: AM sound override\n");
		}

		for (this = 0; this < count; this++) {
			msp3400c_setcarrier(client, cd[this].cdo, cd[this].cdo);

			current->state = TASK_INTERRUPTIBLE;
			schedule_timeout(HZ / 10);
			if (signal_pending(current))
				goto done;
			if (msp->restart)
				msp->restart = 0;

			val = msp3400c_read(client, I2C_MSP3400C_DFP, 0x1b);
			if (val > 32767)
				val -= 65536;
			if (val1 < val)
				val1 = val, max1 = this;
			dprintk("msp3400: carrier1 val: %5d / %s\n", val,
				cd[this].name);
		}

		/* carrier detect pass #2 -- second (stereo) carrier */
		switch (max1) {
		case 1:	/* 5.5 */
			cd = carrier_detect_55;
			count = CARRIER_COUNT(carrier_detect_55);
			break;
		case 3:	/* 6.5 */
			cd = carrier_detect_65;
			count = CARRIER_COUNT(carrier_detect_65);
			break;
		case 0:	/* 4.5 */
		case 2:	/* 6.0 */
		default:
			cd = NULL;
			count = 0;
			break;
		}

		if (amsound && (msp->norm == VIDEO_MODE_SECAM)) {
			/* autodetect doesn't work well with AM ... */
			cd = NULL;
			count = 0;
			max2 = 0;
		}
		for (this = 0; this < count; this++) {
			msp3400c_setcarrier(client, cd[this].cdo, cd[this].cdo);

			current->state = TASK_INTERRUPTIBLE;
			schedule_timeout(HZ / 10);
			if (signal_pending(current))
				goto done;
			if (msp->restart)
				goto restart;

			val = msp3400c_read(client, I2C_MSP3400C_DFP, 0x1b);
			if (val > 32767)
				val -= 65536;
			if (val2 < val)
				val2 = val, max2 = this;
			dprintk("msp3400: carrier2 val: %5d / %s\n", val,
				cd[this].name);
		}

		/* programm the msp3400 according to the results */
		msp->main = carrier_detect_main[max1].cdo;
		switch (max1) {
		case 1:	/* 5.5 */
			if (max2 == 0) {
				/* B/G FM-stereo */
				msp->second = carrier_detect_55[max2].cdo;
				msp3400c_setmode(client, MSP_MODE_FM_TERRA);
				msp->nicam_on = 0;
				msp3400c_setstereo(client, VIDEO_SOUND_MONO);
				msp->watch_stereo = 1;
			} else if (max2 == 1 && msp->nicam) {
				/* B/G NICAM */
				msp->second = carrier_detect_55[max2].cdo;
				msp3400c_setmode(client, MSP_MODE_FM_NICAM1);
				msp->nicam_on = 1;
				msp3400c_setcarrier(client, msp->second,
						    msp->main);
				msp->watch_stereo = 1;
			} else {
				goto no_second;
			}
			break;
		case 2:	/* 6.0 */
			/* PAL I NICAM */
			msp->second = MSP_CARRIER(6.552);
			msp3400c_setmode(client, MSP_MODE_FM_NICAM2);
			msp->nicam_on = 1;
			msp3400c_setcarrier(client, msp->second, msp->main);
			msp->watch_stereo = 1;
			break;
		case 3:	/* 6.5 */
			if (max2 == 1 || max2 == 2) {
				/* D/K FM-stereo */
				msp->second = carrier_detect_65[max2].cdo;
				msp3400c_setmode(client, MSP_MODE_FM_TERRA);
				msp->nicam_on = 0;
				msp3400c_setstereo(client, VIDEO_SOUND_MONO);
				msp->watch_stereo = 1;
			} else if (max2 == 0 && msp->norm == VIDEO_MODE_SECAM) {
				/* L NICAM or AM-mono */
				msp->second = carrier_detect_65[max2].cdo;
				msp3400c_setmode(client, MSP_MODE_AM_NICAM);
				msp->nicam_on = 0;
				msp3400c_setstereo(client, VIDEO_SOUND_MONO);
				msp3400c_setcarrier(client, msp->second,
						    msp->main);
				/* volume prescale for SCART (AM mono input) */
				msp3400c_write(client, I2C_MSP3400C_DFP, 0x000d,
					       0x1900);
				msp->watch_stereo = 1;
			} else if (max2 == 0 && msp->nicam) {
				/* D/K NICAM */
				msp->second = carrier_detect_65[max2].cdo;
				msp3400c_setmode(client, MSP_MODE_FM_NICAM1);
				msp->nicam_on = 1;
				msp3400c_setcarrier(client, msp->second,
						    msp->main);
				msp->watch_stereo = 1;
			} else {
				goto no_second;
			}
			break;
		case 0:	/* 4.5 */
		default:
		      no_second:
			msp->second = carrier_detect_main[max1].cdo;
			msp3400c_setmode(client, MSP_MODE_FM_TERRA);
			msp->nicam_on = 0;
			msp3400c_setcarrier(client, msp->second, msp->main);
			msp->stereo = VIDEO_SOUND_MONO;
			msp3400c_setstereo(client, VIDEO_SOUND_MONO);
			break;
		}

		/* unmute + restore dfp registers */
		msp3400c_setvolume(client, msp->muted, msp->left, msp->right);
		msp3400c_restore_dfp(client);

		if (msp->watch_stereo)
			mod_timer(&msp->wake_stereo, jiffies + 5 * HZ);

		if (debug)
			msp3400c_print_mode(msp);

		msp->active = 0;
	}

      done:
	dprintk("msp3400: thread: exit\n");
	msp->active = 0;
	msp->thread = NULL;

	if (msp->notify != NULL)
		up(msp->notify);
	return 0;
}

/* ----------------------------------------------------------------------- */
/* this one uses the automatic sound standard detection of newer           */
/* msp34xx chip versions                                                   */

static struct MODES {
	int retval;
	int main, second;
	char *name;
} modelist[] = {
	{
	0x0000, 0, 0, "ERROR"}, {
	0x0001, 0, 0, "autodetect start"}, {
	0x0002, MSP_CARRIER(4.5), MSP_CARRIER(4.72),
		    "4.5/4.72  M Dual FM-Stereo"}, {
	0x0003, MSP_CARRIER(5.5), MSP_CARRIER(5.7421875),
		    "5.5/5.74  B/G Dual FM-Stereo"}, {
	0x0004, MSP_CARRIER(6.5), MSP_CARRIER(6.2578125),
		    "6.5/6.25  D/K1 Dual FM-Stereo"}, {
	0x0005, MSP_CARRIER(6.5), MSP_CARRIER(6.7421875),
		    "6.5/6.74  D/K2 Dual FM-Stereo"}, {
	0x0006, MSP_CARRIER(6.5), MSP_CARRIER(6.5), "6.5  D/K FM-Mono (HDEV3)"},
	{
	0x0008, MSP_CARRIER(5.5), MSP_CARRIER(5.85), "5.5/5.85  B/G NICAM FM"},
	{
	0x0009, MSP_CARRIER(6.5), MSP_CARRIER(5.85), "6.5/5.85  L NICAM AM"},
	{
	0x000a, MSP_CARRIER(6.0), MSP_CARRIER(6.55), "6.0/6.55  I NICAM FM"},
	{
	0x000b, MSP_CARRIER(6.5), MSP_CARRIER(5.85), "6.5/5.85  D/K NICAM FM"},
	{
	0x000c, MSP_CARRIER(6.5), MSP_CARRIER(5.85),
		    "6.5/5.85  D/K NICAM FM (HDEV2)"}, {
	0x0020, MSP_CARRIER(4.5), MSP_CARRIER(4.5), "4.5  M BTSC-Stereo"}, {
	0x0021, MSP_CARRIER(4.5), MSP_CARRIER(4.5), "4.5  M BTSC-Mono + SAP"},
	{
	0x0030, MSP_CARRIER(4.5), MSP_CARRIER(4.5),
		    "4.5  M EIA-J Japan Stereo"}, {
	0x0040, MSP_CARRIER(10.7), MSP_CARRIER(10.7), "10.7  FM-Stereo Radio"},
	{
	0x0050, MSP_CARRIER(6.5), MSP_CARRIER(6.5), "6.5  SAT-Mono"}, {
	0x0051, MSP_CARRIER(7.02), MSP_CARRIER(7.20), "7.02/7.20  SAT-Stereo"},
	{
	0x0060, MSP_CARRIER(7.2), MSP_CARRIER(7.2), "7.2  SAT ADR"}, {
	-1, 0, 0, NULL},	/* EOF */
};

static inline const char *msp3400c_standard_mode_name(int mode)
{
	int i;
	for (i = 0; modelist[i].name != NULL; i++)
		if (modelist[i].retval == mode)
			return modelist[i].name;
	return "unknown";
}

static int msp3400c_modus(int norm)
{
	switch (norm) {
	case VIDEO_MODE_PAL:
		return 0x1003;
	case VIDEO_MODE_NTSC:	/* BTSC */
		return 0x2003;
	case VIDEO_MODE_SECAM:
		return 0x0003;
	case VIDEO_MODE_RADIO:
		return 0x0003;
	case VIDEO_MODE_AUTO:
		return 0x2003;
	default:
		return 0x0003;
	}
}

static int msp3410d_thread(void *data)
{
	DEFINE_WAIT(wait);
	struct i2c_client *client = data;
	struct msp3400c *msp = i2c_get_clientdata(client);
	int mode, val, i, std;

#ifndef LINUX26
	daemonize();
	exit_files(current);
	reparent_to_init();

	spin_lock_irq(SIGMASK_LOCK(current));
	sigfillset(&current->blocked);
	spin_unlock_irq(SIGMASK_LOCK(current));

	strcpy(current->comm, "msp3410 [auto]");

	msp->thread = current;

#else				/* LINUX26 */
	daemonize("msp3400 [auto]");
	msp->thread = current;
#endif				/* LINUX26 */

	printk("msp3410: daemon started\n");
	if (msp->notify != NULL)
		up(msp->notify);

	for (;;) {
		if (msp->rmmod)
			goto done;
		if (debug > 1)
			printk("msp3410: thread: sleep\n");
		prepare_to_wait(&msp->wq, &wait, TASK_INTERRUPTIBLE);
		schedule();
		finish_wait(&msp->wq, &wait);
		if (debug > 1)
			printk("msp3410: thread: wakeup\n");
		if (msp->rmmod || signal_pending(current))
			goto done;

		if (msp->mode == MSP_MODE_EXTERN)
			continue;

		msp->active = 1;

		if (msp->watch_stereo) {
			watch_stereo(client);
			msp->active = 0;
			continue;
		}

		/* some time for the tuner to sync */
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ / 5);
		if (signal_pending(current))
			goto done;

	      restart:
		if (msp->mode == MSP_MODE_EXTERN)
			continue;
		msp->restart = 0;
		del_timer(&msp->wake_stereo);
		msp->watch_stereo = 0;

		/* put into sane state (and mute) */
		msp3400c_reset(client);

		/* start autodetect */
		mode = msp3400c_modus(msp->norm);
		switch (msp->norm) {
		case VIDEO_MODE_PAL:
			std = standard;
			break;
		case VIDEO_MODE_NTSC:	/* BTSC */
			std = 0x0020;
			break;
		case VIDEO_MODE_SECAM:
			std = standard;
			break;
		case VIDEO_MODE_RADIO:
			std = 0x0040;
			break;
		case VIDEO_MODE_AUTO:
			std = standard;
			break;
		default:
			std = standard;
			break;
		}
		msp3400c_write(client, I2C_MSP3400C_DEM, 0x30, mode);
		msp3400c_write(client, I2C_MSP3400C_DEM, 0x20, std);

		if (debug) {
			printk("msp3410: setting mode: %s (0x%04x)\n",
			       msp3400c_standard_mode_name(std), std);
		}

		if (std != 1) {
			/* programmed some specific mode */
			val = std;
		} else {
			/* triggered autodetect */
			for (;;) {
				current->state = TASK_INTERRUPTIBLE;
				schedule_timeout(HZ / 10);
				if (signal_pending(current))
					goto done;
				if (msp->restart)
					goto restart;

				/* check results */
				val =
				    msp3400c_read(client, I2C_MSP3400C_DEM,
						  0x7e);
				if (val < 0x07ff)
					break;
				dprintk
				    ("msp3410: detection still in progress\n");
			}
		}
		for (i = 0; modelist[i].name != NULL; i++)
			if (modelist[i].retval == val)
				break;
		dprintk("msp3410: current mode: %s (0x%04x)\n",
			modelist[i].name ? modelist[i].name : "unknown", val);
		msp->main = modelist[i].main;
		msp->second = modelist[i].second;

		if (amsound && (msp->norm == VIDEO_MODE_SECAM)
		    && (val != 0x0009)) {
			/* autodetection has failed, let backup */
			dprintk
			    ("msp3410: autodetection failed, switching to backup mode: %s (0x%04x)\n",
			     modelist[8].name ? modelist[8].name : "unknown",
			     val);
			val = 0x0009;
			msp3400c_write(client, I2C_MSP3400C_DEM, 0x20, val);
		}

		/* set various prescales */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0d, 0x1900);	/* scart */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0e, 0x2403);	/* FM */
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x10, 0x5a00);	/* nicam */

		/* set stereo */
		switch (val) {
		case 0x0008:	/* B/G NICAM */
		case 0x000a:	/* I NICAM */
			if (val == 0x0008)
				msp->mode = MSP_MODE_FM_NICAM1;
			else
				msp->mode = MSP_MODE_FM_NICAM2;
			/* just turn on stereo */
			msp->stereo = VIDEO_SOUND_STEREO;
			msp->nicam_on = 1;
			msp->watch_stereo = 1;
			msp3400c_setstereo(client, VIDEO_SOUND_STEREO);
			break;
		case 0x0009:
			msp->mode = MSP_MODE_AM_NICAM;
			msp->stereo = VIDEO_SOUND_MONO;
			msp->nicam_on = 1;
			msp3400c_setstereo(client, VIDEO_SOUND_MONO);
			msp->watch_stereo = 1;
			break;
		case 0x0020:	/* BTSC */
			/* just turn on stereo */
			msp->mode = MSP_MODE_BTSC;
			msp->stereo = VIDEO_SOUND_STEREO;
			msp->nicam_on = 0;
			msp->watch_stereo = 1;
			msp3400c_setstereo(client, VIDEO_SOUND_STEREO);
			break;
		case 0x0040:	/* FM radio */
			msp->mode = MSP_MODE_FM_RADIO;
			msp->stereo = VIDEO_SOUND_STEREO;
			msp->nicam_on = 0;
			msp->watch_stereo = 0;
			/* scart routing */
			msp3400c_set_scart(client, SCART_IN2, 0);
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x08, 0x0220);
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x09, 0x0220);
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x0b, 0x0220);
			break;
		case 0x0003:
			msp->mode = MSP_MODE_FM_TERRA;
			msp->stereo = VIDEO_SOUND_MONO;
			msp->nicam_on = 0;
			msp->watch_stereo = 1;
			break;
		}

		// AEW a true reset has probably messed with our ACB register
		// we need to restore this.
		msp3400c_write(client, I2C_MSP3400C_DFP, 0x0013, msp->acb);

		/* unmute + restore dfp registers */
		msp3400c_setbass(client, msp->bass);
		msp3400c_settreble(client, msp->treble);
		msp3400c_setvolume(client, msp->muted, msp->left, msp->right);
		msp3400c_restore_dfp(client);

		if (msp->watch_stereo)
			mod_timer(&msp->wake_stereo, jiffies + HZ);

		msp->active = 0;
	}

      done:
	dprintk("msp3410: thread: exit\n");
	msp->active = 0;
	msp->thread = NULL;

	if (msp->notify != NULL)
		up(msp->notify);
	return 0;
}

/* ----------------------------------------------------------------------- */
/* msp34xxG + (simpler no-thread)                                          */
/* this one uses both automatic standard detection and automatic sound     */
/* select which are available in the newer G versions                      */
/* struct msp: only norm, acb and source are really used in this mode      */

static int msp34xxg_reset(struct i2c_client *client);
static void msp34xxg_channelchange(struct i2c_client *client);
static void msp34xxg_set_source(struct i2c_client *client, int source);
static int msp34xxg_get_v4l1_stereo(struct i2c_client *client);
static void msp34xxg_set_v4l1_stereo(struct i2c_client *client, int stereo);
static void msp34xxg_get_v4l2_stereo(struct i2c_client *client, int *rxsubchans,
				     int *audmode);
static void msp34xxg_set_v4l2_stereo(struct i2c_client *client, int audmode);

/* (re-)initialize the msp34xxg, according to the current norm in msp->norm 
 * return 0 if it worked, -1 if it failed
 */
static int msp34xxg_reset(struct i2c_client *client)
{
	struct msp3400c *msp = i2c_get_clientdata(client);
	int i;
	int modus;
	int std;

	if (msp3400c_reset(client))
		return -1;

	/* make sure that input/output is muted (paranoid mode) */
	if (msp3400c_write(client, I2C_MSP3400C_DFP, 0x13,	/* ACB */
			   0x0f20 /* mute DSP input, mute SCART 1 */ ))
		return -1;

	/* step-by-step initialisation, as described in the manual */
	modus = msp3400c_modus(msp->norm);
	modus &= ~0x03;		/* STATUS_CHANGE=0 */
	modus |= 0x01;		/* AUTOMATIC_SOUND_DETECTION=1 */
	if (msp3400c_write(client, I2C_MSP3400C_DEM, 0x30 /*MODUS*/, modus))
		return -1;

	/* write the dfps that may have an influence on standard/audio autodetection right now  */
	msp34xxg_set_source(client, msp->source);

	if (msp3400c_write_dfp_with_default(client, 0x0e,	/* AM/FM Prescale */
					    0x3000
					    /* default: [15:8] 75khz deviation */
	    ))
		return -1;

	if (msp3400c_write_dfp_with_default(client, 0x10,	/* NICAM Prescale */
					    0x5a00
					    /* default: 9db gain (as recommended) */
	    ))
		return -1;

	std = standard;
	if (msp3400c_write(client, I2C_MSP3400C_DEM, 0x20,	/* STANDARD SELECT  */
			   std	/* default: 0x01 for automatic standard select */
	    ))
		return -1;

	if (std == 0x01) {
		dprintk("msp34xxg: triggered autodetect, waiting for result\n");

		/* triggered autodetect */
		for (i = 0; i < 10; i++) {
			int val;
			current->state = TASK_INTERRUPTIBLE;
			schedule_timeout(HZ / 10);
			if (signal_pending(current))
				return -1;	/* failed */

			/* check results */
			val = msp3400c_read(client, I2C_MSP3400C_DEM, 0x7e);
			if (val < 0x07ff) {
				std = val;
				break;
			}
			dprintk("msp34xxg: detection still in progress\n");
		}
		if (i == 10) {
			dprintk
			    ("msp34xxg: detection still in progress after 10 tries. giving up.\n");
			return -1;
		}
	}
	dprintk("msp34xxg: current mode: %s (0x%04x)\n",
		msp3400c_standard_mode_name(std), std);

	/* unmute: dispatch sound to scart output, set scart volume */
	dprintk("msp34xxg: unmute\n");

	msp3400c_setbass(client, msp->bass);
	msp3400c_settreble(client, msp->treble);
	msp3400c_setvolume(client, msp->muted, msp->left, msp->right);

	/* restore other dfp's */
	msp3400c_restore_dfp(client);

	/* restore ACB */
	if (msp3400c_write(client, I2C_MSP3400C_DFP, 0x13,	/* ACB */
			   msp->acb))
		return -1;

	return 0 /*success */ ;
}

/* tell the msp34xxg that there's been a channel (frequency) change
 * and that stereo must be updated. norm stays the same.
 */
static void msp34xxg_channelchange(struct i2c_client *client)
{
	/* I'm pretty sure it's not necessary, but I feel safer this way (what
	 * if there are signals with different standards, or if autodetection
	 * failed initially because there was no signal at all ?)
	 */
	msp34xxg_reset(client);
}

/* set the same 'source' for the loudspeaker, scart and quasi-peak detector 
 * the value for source is the same as bit 15:8 of DFP registers 0x08,
 * 0x0a and 0x0c: 0=mono, 1=stereo or A|B, 2=SCART, 3=stereo or A, 4=stereo or B
 *
 * this function replaces msp3400c_setstereo
 */
static void msp34xxg_set_source(struct i2c_client *client, int source)
{
	struct msp3400c *msp = i2c_get_clientdata(client);

	/* fix matrix mode to stereo and let the msp choose what
	 * to output according to 'source', as recommended 
	 */
	int value = (source & 0x07) << 8 | (source == 0 ? 0x00 : 0x20);
	dprintk("msp34xxg: set source to %d (0x%x)\n", source, value);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x08,	/* Loudspeaker Output */
		       value);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0a,	/* SCART1 DA Output */
		       value);
	msp3400c_write(client, I2C_MSP3400C_DFP, 0x0c,	/* Quasi-peak detector */
		       value);
	/* 
	 * set identification threshold. Personally, I 
	 * I set it to a higher value that the default 
	 * of 0x190 to ignore noisy stereo signals.
	 * this needs tuning. (recommended range 0x00a0-0x03c0)
	 * 0x7f0 = forced mono mode
	 */
	msp3400c_write(client, I2C_MSP3400C_DEM, 0x22,	/* a2 threshold for stereo/bilingual */
		       source == 0 ? 0x7f0 : stereo_threshold);
	msp->source = source;
}

/* get the current stereo mode and return it as a V4L1 stereo value  (Use V4L2 calls whenever possible) */
static int msp34xxg_get_v4l1_stereo(struct i2c_client *client)
{
	/* This is not really clear: the API says that the mode should
	 * be the current mode, but the old driver returned what the
	 * mode *could* be (like rxsubchans in v4l2). I'll just
	 * follow what the API says... 
	 *
	 * The most important point is, I think, that if someone does a get
	 * and then a set with this value, nothing should have changed. It
	 * used not to be the case and that was extremely confusing: one would
	 * do a GET, change mute to 1, then a SET with the same pointer and the 
	 * stereo mode would have changed, the watcher thread killed... (see 
	 * handler for VIDIOCSAUDIO)
	 */
	struct msp3400c *msp = i2c_get_clientdata(client);
	switch (msp->source) {
	case 1:		/* stereo or A|B */
		return VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2 |
		    VIDEO_SOUND_STEREO;
	case 3:		/* stereo or A */
		return VIDEO_SOUND_LANG1 | VIDEO_SOUND_STEREO;
	case 4:		/* stereo or B */
		return VIDEO_SOUND_LANG1 | VIDEO_SOUND_STEREO;
	case 0:		/* mono */
		return VIDEO_SOUND_MONO;
	default:		/* scart input */
		return VIDEO_SOUND_STEREO;
	}
}

/* set the current stereo mode given a v4l1 mode (Use V4L2 calls whenever possible) */
static void msp34xxg_set_v4l1_stereo(struct i2c_client *client, int stereo)
{
	int source;
	if ((stereo & VIDEO_SOUND_LANG1) && (stereo & VIDEO_SOUND_LANG2))
		source = 1;	/* stereo or A|B */
	else if (stereo & VIDEO_SOUND_LANG1)
		source = 3;	/* stereo or A */
	else if (stereo & VIDEO_SOUND_LANG2)
		source = 4;	/* stereo or B */
	else if (stereo & VIDEO_SOUND_STEREO)
		source = 3;	/* stereo or A */
	else
		source = 0;	/* mono only */
	msp34xxg_set_source(client, source);
}

static void msp34xxg_get_v4l2_stereo(struct i2c_client *client, int *rxsubchans,
				     int *audmode)
{
	struct msp3400c *msp = i2c_get_clientdata(client);

	if (rxsubchans) {
		int status = msp3400c_read(client,
					   I2C_MSP3400C_DEM,
					   0x0200 /* STATUS */ );
		int is_bilingual = status & 0x100;
		int is_stereo = status & 0x40;
		int val = 0;
		if (is_stereo)
			val |= V4L2_TUNER_SUB_STEREO;
		else
			val |= V4L2_TUNER_SUB_MONO;
		if (is_bilingual) {
			val |= V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_LANG2;
			/* I'm supposed to check whether it's SAP or not
			 * and set only LANG2/SAP in this case. Yet, the MSP
			 * does a lot of work to hide this and handle everything
			 * the same way. I don't want to work around it so unless
			 * this is a problem, I'll handle SAP just like lang1/lang2.
			 */
		}
		dprintk
		    ("msp34xxg: status=0x%x, stereo=%d, bilingual=%d -> rxsubchans=%d\n",
		     status, is_stereo, is_bilingual, val);
		*rxsubchans = val;
	}
	if (audmode) {
		int val;
		switch (msp->source) {
		case 0:	/* mono only */
			val = V4L2_TUNER_MODE_MONO;
			break;

		case 1:	/* stereo or A|B */
		case 2:	/* scart input (stereo) */
			/* I'm surprised, but according to v4l2 spec, that
			 * is what V4L2_TUNER_MODE_STEREO *may* mean: 
			 * > When the tuner receives bilingual audio it may play 
			 * > different languages on the left and right channel or 
			 * > the primary language on both channels. 
			 * I chose the 2nd interpretation since there's no
			 * BILINGUAL mode. 
			 */
			val = V4L2_TUNER_MODE_STEREO;
			break;

		default:
		case 3:	/* stereo or A */
			val = V4L2_TUNER_MODE_LANG1;
			break;
		case 4:	/* stereo or B */
			val = V4L2_TUNER_MODE_LANG2;
			break;
		}
		dprintk("msp34xxg: source=%d -> audmode=%d\n",
			msp->source, val);
		*audmode = val;
	}
}

static void msp34xxg_set_v4l2_stereo(struct i2c_client *client, int audmode)
{
	int source = 0;
	switch (audmode) {
	case V4L2_TUNER_MODE_MONO:
		source = 0;	/* mono only */
		break;

	case V4L2_TUNER_MODE_STEREO:
		source = 1;	/* stereo or A|B, see comment in msp34xxg_get_v4l2_stereo() */
		/* problem: that could also mean 2 (scart input) */
		break;

	case V4L2_TUNER_MODE_LANG1:
		source = 3;	/* stereo or A */
		break;

	case V4L2_TUNER_MODE_LANG2:
		source = 4;	/* stereo or B */
		break;

	default:		/* doing nothing: a safe, sane default */
		return;
	}
	msp34xxg_set_source(client, source);
}

/* ----------------------------------------------------------------------- */

#ifndef LINUX26
static int msp_attach(struct i2c_adapter *adap, int addr,
		      unsigned short flags, int kind);
#else
static int msp_attach(struct i2c_adapter *adap, int addr, int kind);
#endif /* LINUX26 */
static int msp_detach(struct i2c_client *client);
static int msp_probe(struct i2c_adapter *adap);
static int msp_command(struct i2c_client *client, unsigned int cmd, void *arg);

static struct i2c_driver driver = {
	.name = "i2c msp3400 driver",
	.id = I2C_DRIVERID_MSP3400,
	.flags = I2C_DF_NOTIFY,
	.attach_adapter = msp_probe,
	.detach_client = msp_detach,
	.command = msp_command,
};

static struct i2c_client client_template = {
	.name = "(unset)",
	.flags = I2C_CLIENT_ALLOW_USE,
	.driver = &driver,
};

static int msp_attach(struct i2c_adapter *adap, int addr,
#ifndef LINUX26
		      unsigned short flags,
#endif /* LINUX26 */
		      int kind)
{
	DECLARE_MUTEX_LOCKED(sem);
	struct msp3400c *msp;
	struct i2c_client *c;
	int rev1, rev2, i;

	client_template.adapter = adap;
	client_template.addr = addr;

	if (-1 == msp3400c_reset(&client_template)) {
		dprintk("msp3400: no chip found\n");
		return -1;
	}

	if (NULL == (c = kmalloc(sizeof(struct i2c_client), GFP_KERNEL)))
		return -ENOMEM;
	memcpy(c, &client_template, sizeof(struct i2c_client));
	if (NULL == (msp = kmalloc(sizeof(struct msp3400c), GFP_KERNEL))) {
		kfree(c);
		return -ENOMEM;
	}

	memset(msp, 0, sizeof(struct msp3400c));
	msp->norm = VIDEO_MODE_NTSC;
	msp->left = 58880;	/* 0db gain */
	msp->right = 58880;	/* 0db gain */
	msp->bass = 32768;
	msp->treble = 32768;
	msp->input = -1;
	msp->muted = 0;
	for (i = 0; i < DFP_COUNT; i++)
		msp->dfp_regs[i] = -1;

	i2c_set_clientdata(c, msp);
	init_waitqueue_head(&msp->wq);

	if (-1 == msp3400c_reset(c)) {
		kfree(msp);
		kfree(c);
		dprintk("msp3400: no chip found\n");
		return -1;
	}

	rev1 = msp3400c_read(c, I2C_MSP3400C_DFP, 0x1e);
	if (-1 != rev1)
		rev2 = msp3400c_read(c, I2C_MSP3400C_DFP, 0x1f);
	if ((-1 == rev1) || (0 == rev1 && 0 == rev2)) {
		kfree(msp);
		kfree(c);
		printk("msp3400: error while reading chip version\n");
		return -1;
	}
#if 0
	/* this will turn on a 1kHz beep - might be useful for debugging... */
	msp3400c_write(c, I2C_MSP3400C_DFP, 0x0014, 0x1040);
#endif
	msp3400c_setvolume(c, msp->muted, msp->left, msp->right);

	sprintf(c->name, "MSP34%02d%c-%c%d",
		(rev2 >> 8) & 0xff, (rev1 & 0xff) + '@',
		((rev1 >> 8) & 0xff) + '@', rev2 & 0x1f);
	msp->nicam = (((rev2 >> 8) & 0xff) != 00) ? 1 : 0;

	if (simple == -1) {
		/* default mode */
		/* msp->simple = (((rev2>>8)&0xff) == 0) ? 0 : 1; */
		msp->simple = ((rev1 & 0xff) + '@' > 'C');
	} else {
		/* use insmod option */
		msp->simple = simple;
	}

	if (simpler == -1) {
		/* This still needs some research: I only have the documentation
		 * for the 'G' version, maybe it came earlier. Also, I have a 'W'
		 * version in my Hauppauge PVR350, which is strange because
		 * there's no mention of such a beast on macromedia's web site.
		 */
		msp->simpler = ((rev1 & 0xff) + '@' >= 'G');
	} else {
		msp->simpler = simpler;
	}

	/* hello world :-) */
	printk(KERN_INFO "msp34xx: ivtv version\n");
	printk(KERN_INFO "msp34xx: init: chip=%s", c->name);
	if (msp->nicam)
		printk(", has NICAM support");
	if (msp->simple)
		printk(", simple (D) mode");
	if (msp->simpler)
		printk(", simpler (G) no-thread mode");
	printk("\n");
	printk(KERN_INFO "msp34xx: $Id: msp3400.c 649 2005-11-09 02:22:02Z isely $ compiled on: " __DATE__ " " __TIME__
	       "\n");

	if (IS_MSP34XX_G(msp)) {
		msp->thread = NULL;
		msp->source = 3;	/* defaults to 'stereo or A', see msp34xxg_set_source */
		if (msp34xxg_reset(c) == -1) {
			dprintk
			    ("msp34xxg: initial automatic detection failed: will be re-done at next channel change\n");
		}
	}
#ifndef LINUX26
	MOD_INC_USE_COUNT;
#endif /* LINUX26 */
	if (!IS_MSP34XX_G(msp)) {
		/* timer for stereo checking */
		init_timer(&msp->wake_stereo);
		msp->wake_stereo.function = msp3400c_stereo_wake;
		msp->wake_stereo.data = (unsigned long)msp;

		/* startup control thread */
		msp->notify = &sem;
		kernel_thread(msp->simple ? msp3410d_thread : msp3400c_thread,
			      (void *)c, 0);
		down(&sem);
		msp->notify = NULL;
		wake_up_interruptible(&msp->wq);
	}

	/* update our own array */
	for (i = 0; i < MSP3400_MAX; i++) {
		if (NULL == msps[i]) {
			msps[i] = c;
			break;
		}
	}

	/* done */
	i2c_attach_client(c);
	return 0;
}

static int msp_detach(struct i2c_client *client)
{
	struct msp3400c *msp = (struct msp3400c *)i2c_get_clientdata(client);
	int i;

	if (!IS_MSP34XX_G(msp)) {
		DECLARE_MUTEX_LOCKED(sem);
		/* shutdown control thread */
		del_timer(&msp->wake_stereo);
		if (msp->thread) {
			msp->notify = &sem;
			msp->rmmod = 1;
			wake_up_interruptible(&msp->wq);
			down(&sem);
			msp->notify = NULL;
		}
	}

	msp3400c_reset(client);

	/* update our own array */
	for (i = 0; i < MSP3400_MAX; i++) {
		if (client == msps[i]) {
			msps[i] = NULL;
			break;
		}
	}

	i2c_detach_client(client);
	kfree(msp);
	kfree(client);

#ifndef LINUX26
	MOD_DEC_USE_COUNT;
#endif /* LINUX26 */
	return 0;
}

static int msp_probe(struct i2c_adapter *adap)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
	if (adap->id == I2C_HW_B_BT848)
#else
	if (adap->id == (I2C_ALGO_BIT | I2C_HW_B_BT848))
#endif
		return i2c_probe(adap, &addr_data, msp_attach);
	return 0;
}

static void msp_wake_thread(struct i2c_client *client)
{
	struct msp3400c *msp = (struct msp3400c *)i2c_get_clientdata(client);

	msp3400c_setvolume(client, msp->muted, 0, 0);
	msp->watch_stereo = 0;
	del_timer(&msp->wake_stereo);
	if (msp->active)
		msp->restart = 1;
	wake_up_interruptible(&msp->wq);
}

static int msp_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct msp3400c *msp = (struct msp3400c *)i2c_get_clientdata(client);
	__u16 *sarg = arg;
#if 0
	int *iarg = (int *)arg;
#endif

	switch (cmd) {

	case AUDC_SET_INPUT:
		/* scart switching
		   - IN1 is often used for external input
		   - Hauppauge uses IN2 for the radio */
		dprintk(KERN_DEBUG "msp34xx: AUDC_SET_INPUT(%d)\n", *sarg);
		if (*sarg == msp->input)
			break;
		msp->input = *sarg;
		switch (*sarg) {
		case AUDIO_RADIO:
			msp->mode = MSP_MODE_FM_RADIO;
			msp->stereo = VIDEO_SOUND_STEREO;
			msp3400c_set_scart(client, SCART_IN2, 0);
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x000d,
				       0x1900);
			if (!IS_MSP34XX_G(msp)) {
				msp3400c_setstereo(client, msp->stereo);
			}
			break;
		case AUDIO_EXTERN:
			msp->mode = MSP_MODE_EXTERN;
			msp->stereo = VIDEO_SOUND_STEREO;
			msp3400c_set_scart(client, SCART_IN1, 0);
			msp3400c_write(client, I2C_MSP3400C_DFP, 0x000d,
				       0x1900);
			if (!IS_MSP34XX_G(msp)) {
				msp3400c_setstereo(client, msp->stereo);
			}
			break;
		case AUDIO_TUNER:
			msp->mode = -1;
			if (!IS_MSP34XX_G(msp)) {
				msp_wake_thread(client);
			} else {
				/*msp3400c_set_scart(client, SCART_IN1_DA, 1); */
			}
			break;
		default:
			if (*sarg & AUDIO_MUTE)
				msp3400c_set_scart(client, SCART_MUTE, 0);
			break;
		}
		if (IS_MSP34XX_G(msp)) {
			msp34xxg_reset(client);
		} else if (msp->active)
			msp->restart = 1;
		break;

	case AUDC_SET_RADIO:
		dprintk(KERN_DEBUG "msp34xx: AUDC_SET_RADIO\n");
		msp->norm = VIDEO_MODE_RADIO;
		dprintk("msp34xx: switching to radio mode\n");
		if (IS_MSP34XX_G(msp)) {
			msp34xxg_reset(client);
			break;
		}
		msp->watch_stereo = 0;
		del_timer(&msp->wake_stereo);
		if (msp->simple) {
			/* the thread will do for us */
			msp_wake_thread(client);
		} else {
			/* set msp3400 to FM radio mode */
			msp3400c_setmode(client, MSP_MODE_FM_RADIO);
			msp3400c_setcarrier(client, MSP_CARRIER(10.7),
					    MSP_CARRIER(10.7));
			msp3400c_setvolume(client, msp->muted, msp->left,
					   msp->right);
		}
		if (msp->active)
			msp->restart = 1;
		break;

#if 1
		/* work-in-progress:  hook to control the DFP registers */
	case MSP_SET_DFPREG:
	{
		struct msp_dfpreg *r = arg;
		int i;

		if (r->reg < 0 || r->reg >= DFP_COUNT)
			return -EINVAL;
		for (i = 0; i < sizeof(bl_dfp) / sizeof(int); i++)
			if (r->reg == bl_dfp[i])
				return -EINVAL;
		msp->dfp_regs[r->reg] = r->value;
		msp3400c_write(client, I2C_MSP3400C_DFP, r->reg, r->value);
		return 0;
	}
	case MSP_GET_DFPREG:
	{
		struct msp_dfpreg *r = arg;

		if (r->reg < 0 || r->reg >= DFP_COUNT)
			return -EINVAL;
		r->value = msp3400c_read(client, I2C_MSP3400C_DFP, r->reg);
		return 0;
	}
#endif /* 1 */

		/* --- v4l ioctls --- */
		/* take care: bttv does userspace copying, we'll get a
		   kernel pointer here... */
	case VIDIOCGAUDIO:
	{
		struct video_audio *va = arg;

		dprintk(KERN_DEBUG "msp34xx: VIDIOCGAUDIO\n");
		va->flags = VIDEO_AUDIO_VOLUME |
		    VIDEO_AUDIO_BASS | VIDEO_AUDIO_TREBLE | VIDEO_AUDIO_MUTABLE;
		if (msp->muted)
			va->flags |= VIDEO_AUDIO_MUTE;
		va->volume = MAX(msp->left, msp->right);
		va->balance = (32768 * MIN(msp->left, msp->right)) /
		    (va->volume ? va->volume : 1);
		va->balance = (msp->left < msp->right) ?
		    (65535 - va->balance) : va->balance;
		if (0 == va->volume)
			va->balance = 32768;
		va->bass = msp->bass;
		va->treble = msp->treble;

		if (IS_MSP34XX_G(msp)) {
			va->mode = msp34xxg_get_v4l1_stereo(client);
		} else if (msp->norm != VIDEO_MODE_RADIO) {
			autodetect_stereo(client);
			va->mode = msp->stereo;
		}
		break;
	}
	case VIDIOCSAUDIO:
	{
		struct video_audio *va = arg;

		dprintk(KERN_DEBUG "msp34xx: VIDIOCSAUDIO\n");
		msp->muted = (va->flags & VIDEO_AUDIO_MUTE);
		msp->left = (MIN(65536 - va->balance, 32768) *
			     va->volume) / 32768;
		msp->right = (MIN(va->balance, 32768) * va->volume) / 32768;
		msp->bass = va->bass;
		msp->treble = va->treble;
		dprintk("msp34xx: VIDIOCSAUDIO setting va->volume to %d\n",
			va->volume);
		dprintk("msp34xx: VIDIOCSAUDIO setting va->balance to %d\n",
			va->balance);
		dprintk("msp34xx: VIDIOCSAUDIO setting va->flags to %d\n",
			va->flags);
		dprintk("msp34xx: VIDIOCSAUDIO setting msp->left to %d\n",
			msp->left);
		dprintk("msp34xx: VIDIOCSAUDIO setting msp->right to %d\n",
			msp->right);
		dprintk("msp34xx: VIDIOCSAUDIO setting msp->bass to %d\n",
			msp->bass);
		dprintk("msp34xx: VIDIOCSAUDIO setting msp->treble to %d\n",
			msp->treble);
		dprintk("msp34xx: VIDIOCSAUDIO setting msp->mode to %d\n",
			msp->mode);
		msp3400c_setvolume(client, msp->muted, msp->left, msp->right);
		msp3400c_setbass(client, msp->bass);
		msp3400c_settreble(client, msp->treble);

		if (va->mode != 0) {
			if (IS_MSP34XX_G(msp)) {
				msp34xxg_set_v4l1_stereo(client, va->mode);
			} else {
				msp->watch_stereo = 0;
				del_timer(&msp->wake_stereo);
				msp->stereo = va->mode;
				msp3400c_setstereo(client, va->mode);
			}
		}
		break;
	}
	case VIDIOC_G_TUNER:
	{
		struct v4l2_tuner *vt = (struct v4l2_tuner *)arg;
		vt->capability |=
		    V4L2_TUNER_CAP_STEREO | V4L2_TUNER_CAP_LANG1 |
		    V4L2_TUNER_CAP_LANG2;

		/* get rxsubchans and audmode */
		if (IS_MSP34XX_G(msp)) {
			msp34xxg_get_v4l2_stereo(client, &vt->rxsubchans,
						 &vt->audmode);
		}
		break;
	}
	case VIDIOC_S_TUNER:
	{
		struct v4l2_tuner *vt = (struct v4l2_tuner *)arg;
		/* only set audmode */
		if (vt->audmode != -1) {
			if (IS_MSP34XX_G(msp)) {
				msp34xxg_set_v4l2_stereo(client, vt->audmode);
			}
		}
		break;
	}

	case VIDIOCSCHAN:
	{
		struct video_channel *vc = arg;

		dprintk(KERN_DEBUG "msp34xx: VIDIOCSCHAN\n");
		dprintk("msp34xx: switching to TV mode\n");
		if (IS_MSP34XX_G(msp)) {
			if (msp->norm != vc->norm) {
				msp34xxg_channelchange(client);
			} else {
				msp->norm = vc->norm;
				msp34xxg_reset(client);
			}
			break;
		}
		msp->norm = vc->norm;
		msp_wake_thread(client);
		break;
	}
	case VIDIOCSFREQ:
	{
		/* new channel -- kick audio carrier scan */
		dprintk(KERN_DEBUG "msp34xx: VIDIOCSFREQ\n");
		if (IS_MSP34XX_G(msp)) {
			msp34xxg_channelchange(client);
			break;
		} else {
			msp_wake_thread(client);
		}
		break;
	}
	case MSP_SET_MATRIX:
	{
		struct msp_matrix *mspm = arg;

		dprintk(KERN_DEBUG "msp34xx: MSP_SET_MATRIX\n");
		msp3400c_set_scart(client, mspm->input, mspm->output);
		break;
	}
	default:
		/* nothing */
		break;
	}
	return 0;
}

/* ----------------------------------------------------------------------- */

static int msp3400_init_module(void)
{
	i2c_add_driver(&driver);
	return 0;
}

static void msp3400_cleanup_module(void)
{
	i2c_del_driver(&driver);
}

module_init(msp3400_init_module);
module_exit(msp3400_cleanup_module);

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
