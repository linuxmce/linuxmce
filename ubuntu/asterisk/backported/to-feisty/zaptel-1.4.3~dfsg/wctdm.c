/*
 * Wilcard TDM400P TDM FXS/FXO Interface Driver for Zapata Telephony interface
 *
 * Written by Mark Spencer <markster@digium.com>
 *            Matthew Fredrickson <creslin@digium.com>
 *
 * Copyright (C) 2001, Linux Support Services, Inc.
 *
 * All rights reserved.
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
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include "proslic.h"
#include "wctdm.h"
/*
 *  Define for audio vs. register based ring detection
 *  
 */
/* #define AUDIO_RINGCHECK  */

/*
  Experimental max loop current limit for the proslic
  Loop current limit is from 20 mA to 41 mA in steps of 3
  (according to datasheet)
  So set the value below to:
  0x00 : 20mA (default)
  0x01 : 23mA
  0x02 : 26mA
  0x03 : 29mA
  0x04 : 32mA
  0x05 : 35mA
  0x06 : 37mA
  0x07 : 41mA
*/
static int loopcurrent = 20;

static int reversepolarity = 0;

static alpha  indirect_regs[] =
{
{0,255,"DTMF_ROW_0_PEAK",0x55C2},
{1,255,"DTMF_ROW_1_PEAK",0x51E6},
{2,255,"DTMF_ROW2_PEAK",0x4B85},
{3,255,"DTMF_ROW3_PEAK",0x4937},
{4,255,"DTMF_COL1_PEAK",0x3333},
{5,255,"DTMF_FWD_TWIST",0x0202},
{6,255,"DTMF_RVS_TWIST",0x0202},
{7,255,"DTMF_ROW_RATIO_TRES",0x0198},
{8,255,"DTMF_COL_RATIO_TRES",0x0198},
{9,255,"DTMF_ROW_2ND_ARM",0x0611},
{10,255,"DTMF_COL_2ND_ARM",0x0202},
{11,255,"DTMF_PWR_MIN_TRES",0x00E5},
{12,255,"DTMF_OT_LIM_TRES",0x0A1C},
{13,0,"OSC1_COEF",0x7B30},
{14,1,"OSC1X",0x0063},
{15,2,"OSC1Y",0x0000},
{16,3,"OSC2_COEF",0x7870},
{17,4,"OSC2X",0x007D},
{18,5,"OSC2Y",0x0000},
{19,6,"RING_V_OFF",0x0000},
{20,7,"RING_OSC",0x7EF0},
{21,8,"RING_X",0x0160},
{22,9,"RING_Y",0x0000},
{23,255,"PULSE_ENVEL",0x2000},
{24,255,"PULSE_X",0x2000},
{25,255,"PULSE_Y",0x0000},
//{26,13,"RECV_DIGITAL_GAIN",0x4000},	// playback volume set lower
{26,13,"RECV_DIGITAL_GAIN",0x2000},	// playback volume set lower
{27,14,"XMIT_DIGITAL_GAIN",0x4000},
//{27,14,"XMIT_DIGITAL_GAIN",0x2000},
{28,15,"LOOP_CLOSE_TRES",0x1000},
{29,16,"RING_TRIP_TRES",0x3600},
{30,17,"COMMON_MIN_TRES",0x1000},
{31,18,"COMMON_MAX_TRES",0x0200},
{32,19,"PWR_ALARM_Q1Q2",0x07C0},
{33,20,"PWR_ALARM_Q3Q4",0x2600},
{34,21,"PWR_ALARM_Q5Q6",0x1B80},
{35,22,"LOOP_CLOSURE_FILTER",0x8000},
{36,23,"RING_TRIP_FILTER",0x0320},
{37,24,"TERM_LP_POLE_Q1Q2",0x008C},
{38,25,"TERM_LP_POLE_Q3Q4",0x0100},
{39,26,"TERM_LP_POLE_Q5Q6",0x0010},
{40,27,"CM_BIAS_RINGING",0x0C00},
{41,64,"DCDC_MIN_V",0x0C00},
{42,255,"DCDC_XTRA",0x1000},
{43,66,"LOOP_CLOSE_TRES_LOW",0x1000},
};

static struct fxo_mode {
	char *name;
	/* FXO */
	int ohs;
	int ohs2;
	int rz;
	int rt;
	int ilim;
	int dcv;
	int mini;
	int acim;
	int ring_osc;
	int ring_x;
} fxo_modes[] =
{
	{ "FCC", 0, 0, 0, 1, 0, 0x3, 0, 0, }, 	/* US, Canada */
	{ "TBR21", 0, 0, 0, 0, 1, 0x3, 0, 0x2, 0x7e6c, 0x023a, },
										/* Austria, Belgium, Denmark, Finland, France, Germany, 
										   Greece, Iceland, Ireland, Italy, Luxembourg, Netherlands,
										   Norway, Portugal, Spain, Sweden, Switzerland, and UK */
	{ "ARGENTINA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "AUSTRALIA", 1, 0, 0, 0, 0, 0, 0x3, 0x3, },
	{ "AUSTRIA", 0, 1, 0, 0, 1, 0x3, 0, 0x3, },
	{ "BAHRAIN", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "BELGIUM", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "BRAZIL", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "BULGARIA", 0, 0, 0, 0, 1, 0x3, 0x0, 0x3, },
	{ "CANADA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "CHILE", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "CHINA", 0, 0, 0, 0, 0, 0, 0x3, 0xf, },
	{ "COLUMBIA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "CROATIA", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "CYRPUS", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "CZECH", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "DENMARK", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "ECUADOR", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "EGYPT", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "ELSALVADOR", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "FINLAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "FRANCE", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "GERMANY", 0, 1, 0, 0, 1, 0x3, 0, 0x3, },
	{ "GREECE", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "GUAM", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "HONGKONG", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "HUNGARY", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "ICELAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "INDIA", 0, 0, 0, 0, 0, 0x3, 0, 0x4, },
	{ "INDONESIA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "IRELAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "ISRAEL", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "ITALY", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "JAPAN", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "JORDAN", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "KAZAKHSTAN", 0, 0, 0, 0, 0, 0x3, 0, },
	{ "KUWAIT", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "LATVIA", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "LEBANON", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "LUXEMBOURG", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "MACAO", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "MALAYSIA", 0, 0, 0, 0, 0, 0, 0x3, 0, },	/* Current loop >= 20ma */
	{ "MALTA", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "MEXICO", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "MOROCCO", 0, 0, 0, 0, 1, 0x3, 0, 0x2, },
	{ "NETHERLANDS", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "NEWZEALAND", 0, 0, 0, 0, 0, 0x3, 0, 0x4, },
	{ "NIGERIA", 0, 0, 0, 0, 0x1, 0x3, 0, 0x2, },
	{ "NORWAY", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "OMAN", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "PAKISTAN", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "PERU", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "PHILIPPINES", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "POLAND", 0, 0, 1, 1, 0, 0x3, 0, 0, },
	{ "PORTUGAL", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "ROMANIA", 0, 0, 0, 0, 0, 3, 0, 0, },
	{ "RUSSIA", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "SAUDIARABIA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "SINGAPORE", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "SLOVAKIA", 0, 0, 0, 0, 0, 0x3, 0, 0x3, },
	{ "SLOVENIA", 0, 0, 0, 0, 0, 0x3, 0, 0x2, },
	{ "SOUTHAFRICA", 1, 0, 1, 0, 0, 0x3, 0, 0x3, },
	{ "SOUTHKOREA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "SPAIN", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "SWEDEN", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "SWITZERLAND", 0, 1, 0, 0, 1, 0x3, 0, 0x2, },
	{ "SYRIA", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "TAIWAN", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "THAILAND", 0, 0, 0, 0, 0, 0, 0x3, 0, },
	{ "UAE", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "UK", 0, 1, 0, 0, 1, 0x3, 0, 0x5, },
	{ "USA", 0, 0, 0, 0, 0, 0x3, 0, 0, },
	{ "YEMEN", 0, 0, 0, 0, 0, 0x3, 0, 0, },
};

#ifdef STANDALONE_ZAPATA
#include "zaptel.h"
#else
#include <zaptel/zaptel.h>
#endif

#ifdef LINUX26
#include <linux/moduleparam.h>
#endif

#define NUM_FXO_REGS 60

#define WC_MAX_IFACES 128

#define WC_CNTL    	0x00
#define WC_OPER		0x01
#define WC_AUXC    	0x02
#define WC_AUXD    	0x03
#define WC_MASK0   	0x04
#define WC_MASK1   	0x05
#define WC_INTSTAT 	0x06
#define WC_AUXR		0x07

#define WC_DMAWS	0x08
#define WC_DMAWI	0x0c
#define WC_DMAWE	0x10
#define WC_DMARS	0x18
#define WC_DMARI	0x1c
#define WC_DMARE	0x20

#define WC_AUXFUNC	0x2b
#define WC_SERCTL	0x2d
#define WC_FSCDELAY	0x2f

#define WC_REGBASE	0xc0

#define WC_SYNC		0x0
#define WC_TEST		0x1
#define WC_CS		0x2
#define WC_VER		0x3

#define BIT_CS		(1 << 2)
#define BIT_SCLK	(1 << 3)
#define BIT_SDI		(1 << 4)
#define BIT_SDO		(1 << 5)

#define FLAG_EMPTY	0
#define FLAG_WRITE	1
#define FLAG_READ	2

/* the constants below control the 'debounce' periods enforced by the
   check_hook routines; these routines are called once every 4 interrupts
   (the interrupt cycles around the four modules), so the periods are
   specified in _4 millisecond_ increments
*/
#define RING_DEBOUNCE		4		/* Ringer Debounce (64 ms) */
#define DEFAULT_BATT_DEBOUNCE	4		/* Battery debounce (64 ms) */
#define POLARITY_DEBOUNCE 	4		/* Polarity debounce (64 ms) */
#define DEFAULT_BATT_THRESH	3		/* Anything under this is "no battery" */

#define OHT_TIMER		6000	/* How long after RING to retain OHT */

#define FLAG_3215	(1 << 0)

#define NUM_CARDS 4

#define MAX_ALARMS 10

#define MOD_TYPE_FXS	0
#define MOD_TYPE_FXO	1

#define MINPEGTIME	10 * 8		/* 30 ms peak to peak gets us no more than 100 Hz */
#define PEGTIME		50 * 8		/* 50ms peak to peak gets us rings of 10 Hz or more */
#define PEGCOUNT	5		/* 5 cycles of pegging means RING */

#define NUM_CAL_REGS 12

struct calregs {
	unsigned char vals[NUM_CAL_REGS];
};

enum proslic_power_warn {
	PROSLIC_POWER_UNKNOWN = 0,
	PROSLIC_POWER_ON,
	PROSLIC_POWER_WARNED,
};

struct wctdm {
	struct pci_dev *dev;
	char *variety;
	struct zt_span span;
	unsigned char ios;
	int usecount;
	unsigned int intcount;
	int dead;
	int pos;
	int flags[NUM_CARDS];
	int freeregion;
	int alt;
	int curcard;
	int cardflag;		/* Bit-map of present cards */
	enum proslic_power_warn proslic_power;
	spinlock_t lock;

	union {
		struct {
#ifdef AUDIO_RINGCHECK
			unsigned int pegtimer;
			int pegcount;
			int peg;
			int ring;
#else			
			int wasringing;
#endif			
			int ringdebounce;
			int offhook;
			int battdebounce;
			int nobatttimer;
			int battery;
		        int lastpol;
		        int polarity;
		        int polaritydebounce;
		} fxo;
		struct {
			int oldrxhook;
			int debouncehook;
			int lastrxhook;
			int debounce;
			int ohttimer;
			int idletxhookstate;		/* IDLE changing hook state */
			int lasttxhook;
			int palarms;
			struct calregs calregs;
		} fxs;
	} mod[NUM_CARDS];

	/* Receive hook state and debouncing */
	int modtype[NUM_CARDS];
	unsigned char reg0shadow[NUM_CARDS];
	unsigned char reg1shadow[NUM_CARDS];

	unsigned long ioaddr;
	dma_addr_t 	readdma;
	dma_addr_t	writedma;
	volatile unsigned int *writechunk;				/* Double-word aligned write memory */
	volatile unsigned int *readchunk;				/* Double-word aligned read memory */
	struct zt_chan chans[NUM_CARDS];
};


struct wctdm_desc {
	char *name;
	int flags;
};

static struct wctdm_desc wctdm = { "Wildcard S400P Prototype", 0 };
static struct wctdm_desc wctdme = { "Wildcard TDM400P REV E/F", 0 };
static struct wctdm_desc wctdmh = { "Wildcard TDM400P REV H", 0 };
static struct wctdm_desc wctdmi = { "Wildcard TDM400P REV I", 0 };
static int acim2tiss[16] = { 0x0, 0x1, 0x4, 0x5, 0x7, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0, 0x2, 0x0, 0x3 };

static struct wctdm *ifaces[WC_MAX_IFACES];

static void wctdm_release(struct wctdm *wc);

static int battdebounce = DEFAULT_BATT_DEBOUNCE;
static int battthresh = DEFAULT_BATT_THRESH;
static int debug = 0;
static int robust = 0;
static int timingonly = 0;
static int lowpower = 0;
static int boostringer = 0;
static int fastringer = 0;
static int _opermode = 0;
static char *opermode = "FCC";
static int fxshonormode = 0;
static int alawoverride = 0;
static int fastpickup = 0;
static int fxotxgain = 0;
static int fxorxgain = 0;
static int fxstxgain = 0;
static int fxsrxgain = 0;

static int wctdm_init_proslic(struct wctdm *wc, int card, int fast , int manual, int sane);

static inline void wctdm_transmitprep(struct wctdm *wc, unsigned char ints)
{
	volatile unsigned int *writechunk;
	int x;
	if (ints & 0x01) 
		/* Write is at interrupt address.  Start writing from normal offset */
		writechunk = wc->writechunk;
	else 
		writechunk = wc->writechunk + ZT_CHUNKSIZE;
	/* Calculate Transmission */
	zt_transmit(&wc->span);

	for (x=0;x<ZT_CHUNKSIZE;x++) {
		/* Send a sample, as a 32-bit word */
		writechunk[x] = 0;
#ifdef __BIG_ENDIAN
		if (wc->cardflag & (1 << 3))
			writechunk[x] |= (wc->chans[3].writechunk[x]);
		if (wc->cardflag & (1 << 2))
			writechunk[x] |= (wc->chans[2].writechunk[x] << 8);
		if (wc->cardflag & (1 << 1))
			writechunk[x] |= (wc->chans[1].writechunk[x] << 16);
		if (wc->cardflag & (1 << 0))
			writechunk[x] |= (wc->chans[0].writechunk[x] << 24);
#else
		if (wc->cardflag & (1 << 3))
			writechunk[x] |= (wc->chans[3].writechunk[x] << 24);
		if (wc->cardflag & (1 << 2))
			writechunk[x] |= (wc->chans[2].writechunk[x] << 16);
		if (wc->cardflag & (1 << 1))
			writechunk[x] |= (wc->chans[1].writechunk[x] << 8);
		if (wc->cardflag & (1 << 0))
			writechunk[x] |= (wc->chans[0].writechunk[x]);
#endif		
	}

}

#ifdef AUDIO_RINGCHECK
static inline void ring_check(struct wctdm *wc, int card)
{
	int x;
	short sample;
	if (wc->modtype[card] != MOD_TYPE_FXO)
		return;
	wc->mod[card].fxo.pegtimer += ZT_CHUNKSIZE;
	for (x=0;x<ZT_CHUNKSIZE;x++) {
		/* Look for pegging to indicate ringing */
		sample = ZT_XLAW(wc->chans[card].readchunk[x], (&(wc->chans[card])));
		if ((sample > 10000) && (wc->mod[card].fxo.peg != 1)) {
			if (debug > 1) printk("High peg!\n");
			if ((wc->mod[card].fxo.pegtimer < PEGTIME) && (wc->mod[card].fxo.pegtimer > MINPEGTIME))
				wc->mod[card].fxo.pegcount++;
			wc->mod[card].fxo.pegtimer = 0;
			wc->mod[card].fxo.peg = 1;
		} else if ((sample < -10000) && (wc->mod[card].fxo.peg != -1)) {
			if (debug > 1) printk("Low peg!\n");
			if ((wc->mod[card].fxo.pegtimer < (PEGTIME >> 2)) && (wc->mod[card].fxo.pegtimer > (MINPEGTIME >> 2)))
				wc->mod[card].fxo.pegcount++;
			wc->mod[card].fxo.pegtimer = 0;
			wc->mod[card].fxo.peg = -1;
		}
	}
	if (wc->mod[card].fxo.pegtimer > PEGTIME) {
		/* Reset pegcount if our timer expires */
		wc->mod[card].fxo.pegcount = 0;
	}
	/* Decrement debouncer if appropriate */
	if (wc->mod[card].fxo.ringdebounce)
		wc->mod[card].fxo.ringdebounce--;
	if (!wc->mod[card].fxo.offhook && !wc->mod[card].fxo.ringdebounce) {
		if (!wc->mod[card].fxo.ring && (wc->mod[card].fxo.pegcount > PEGCOUNT)) {
			/* It's ringing */
			if (debug)
				printk("RING on %d/%d!\n", wc->span.spanno, card + 1);
			if (!wc->mod[card].fxo.offhook)
				zt_hooksig(&wc->chans[card], ZT_RXSIG_RING);
			wc->mod[card].fxo.ring = 1;
		}
		if (wc->mod[card].fxo.ring && !wc->mod[card].fxo.pegcount) {
			/* No more ring */
			if (debug)
				printk("NO RING on %d/%d!\n", wc->span.spanno, card + 1);
			zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
			wc->mod[card].fxo.ring = 0;
		}
	}
}
#endif
static inline void wctdm_receiveprep(struct wctdm *wc, unsigned char ints)
{
	volatile unsigned int *readchunk;
	int x;

	if (ints & 0x08)
		readchunk = wc->readchunk + ZT_CHUNKSIZE;
	else
		/* Read is at interrupt address.  Valid data is available at normal offset */
		readchunk = wc->readchunk;
	for (x=0;x<ZT_CHUNKSIZE;x++) {
#ifdef __BIG_ENDIAN
		if (wc->cardflag & (1 << 3))
			wc->chans[3].readchunk[x] = (readchunk[x]) & 0xff;
		if (wc->cardflag & (1 << 2))
			wc->chans[2].readchunk[x] = (readchunk[x] >> 8) & 0xff;
		if (wc->cardflag & (1 << 1))
			wc->chans[1].readchunk[x] = (readchunk[x] >> 16) & 0xff;
		if (wc->cardflag & (1 << 0))
			wc->chans[0].readchunk[x] = (readchunk[x] >> 24) & 0xff;
#else
		if (wc->cardflag & (1 << 3))
			wc->chans[3].readchunk[x] = (readchunk[x] >> 24) & 0xff;
		if (wc->cardflag & (1 << 2))
			wc->chans[2].readchunk[x] = (readchunk[x] >> 16) & 0xff;
		if (wc->cardflag & (1 << 1))
			wc->chans[1].readchunk[x] = (readchunk[x] >> 8) & 0xff;
		if (wc->cardflag & (1 << 0))
			wc->chans[0].readchunk[x] = (readchunk[x]) & 0xff;
#endif
	}
#ifdef AUDIO_RINGCHECK
	for (x=0;x<wc->cards;x++)
		ring_check(wc, x);
#endif		
	/* XXX We're wasting 8 taps.  We should get closer :( */
	for (x = 0; x < NUM_CARDS; x++) {
		if (wc->cardflag & (1 << x))
			zt_ec_chunk(&wc->chans[x], wc->chans[x].readchunk, wc->chans[x].writechunk);
	}
	zt_receive(&wc->span);
}

static void wctdm_stop_dma(struct wctdm *wc);
static void wctdm_reset_tdm(struct wctdm *wc);
static void wctdm_restart_dma(struct wctdm *wc);

static inline void __write_8bits(struct wctdm *wc, unsigned char bits)
{
	/* Drop chip select */
	int x;
	wc->ios |= BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	wc->ios &= ~BIT_CS;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	for (x=0;x<8;x++) {
		/* Send out each bit, MSB first, drop SCLK as we do so */
		if (bits & 0x80)
			wc->ios |= BIT_SDI;
		else
			wc->ios &= ~BIT_SDI;
		wc->ios &= ~BIT_SCLK;
		outb(wc->ios, wc->ioaddr + WC_AUXD);
		/* Now raise SCLK high again and repeat */
		wc->ios |= BIT_SCLK;
		outb(wc->ios, wc->ioaddr + WC_AUXD);
		bits <<= 1;
	}
	/* Finally raise CS back high again */
	wc->ios |= BIT_CS;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	
}

static inline void __reset_spi(struct wctdm *wc)
{
	/* Drop chip select and clock once and raise and clock once */
	wc->ios |= BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	wc->ios &= ~BIT_CS;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	wc->ios |= BIT_SDI;
	wc->ios &= ~BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	/* Now raise SCLK high again and repeat */
	wc->ios |= BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	/* Finally raise CS back high again */
	wc->ios |= BIT_CS;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	/* Clock again */
	wc->ios &= ~BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	/* Now raise SCLK high again and repeat */
	wc->ios |= BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	
}

static inline unsigned char __read_8bits(struct wctdm *wc)
{
	unsigned char res=0, c;
	int x;
	wc->ios |= BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	/* Drop chip select */
	wc->ios &= ~BIT_CS;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	for (x=0;x<8;x++) {
		res <<= 1;
		/* Get SCLK */
		wc->ios &= ~BIT_SCLK;
		outb(wc->ios, wc->ioaddr + WC_AUXD);
		/* Read back the value */
		c = inb(wc->ioaddr + WC_AUXR);
		if (c & BIT_SDO)
			res |= 1;
		/* Now raise SCLK high again */
		wc->ios |= BIT_SCLK;
		outb(wc->ios, wc->ioaddr + WC_AUXD);
	}
	/* Finally raise CS back high again */
	wc->ios |= BIT_CS;
	outb(wc->ios, wc->ioaddr + WC_AUXD);
	wc->ios &= ~BIT_SCLK;
	outb(wc->ios, wc->ioaddr + WC_AUXD);

	/* And return our result */
	return res;
}

static void __wctdm_setcreg(struct wctdm *wc, unsigned char reg, unsigned char val)
{
	outb(val, wc->ioaddr + WC_REGBASE + ((reg & 0xf) << 2));
}

static unsigned char __wctdm_getcreg(struct wctdm *wc, unsigned char reg)
{
	return inb(wc->ioaddr + WC_REGBASE + ((reg & 0xf) << 2));
}

static inline void __wctdm_setcard(struct wctdm *wc, int card)
{
	if (wc->curcard != card) {
		__wctdm_setcreg(wc, WC_CS, (1 << card));
		wc->curcard = card;
	}
}

static void __wctdm_setreg(struct wctdm *wc, int card, unsigned char reg, unsigned char value)
{
	__wctdm_setcard(wc, card);
	if (wc->modtype[card] == MOD_TYPE_FXO) {
		__write_8bits(wc, 0x20);
		__write_8bits(wc, reg & 0x7f);
	} else {
		__write_8bits(wc, reg & 0x7f);
	}
	__write_8bits(wc, value);
}

static void wctdm_setreg(struct wctdm *wc, int card, unsigned char reg, unsigned char value)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->lock, flags);
	__wctdm_setreg(wc, card, reg, value);
	spin_unlock_irqrestore(&wc->lock, flags);
}

static unsigned char __wctdm_getreg(struct wctdm *wc, int card, unsigned char reg)
{
	__wctdm_setcard(wc, card);
	if (wc->modtype[card] == MOD_TYPE_FXO) {
		__write_8bits(wc, 0x60);
		__write_8bits(wc, reg & 0x7f);
	} else {
		__write_8bits(wc, reg | 0x80);
	}
	return __read_8bits(wc);
}

static inline void reset_spi(struct wctdm *wc, int card)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->lock, flags);
	__wctdm_setcard(wc, card);
	__reset_spi(wc);
	__reset_spi(wc);
	spin_unlock_irqrestore(&wc->lock, flags);
}

static unsigned char wctdm_getreg(struct wctdm *wc, int card, unsigned char reg)
{
	unsigned long flags;
	unsigned char res;
	spin_lock_irqsave(&wc->lock, flags);
	res = __wctdm_getreg(wc, card, reg);
	spin_unlock_irqrestore(&wc->lock, flags);
	return res;
}

static int __wait_access(struct wctdm *wc, int card)
{
    unsigned char data = 0;
    long origjiffies;
    int count = 0;

    #define MAX 6000 /* attempts */


    origjiffies = jiffies;
    /* Wait for indirect access */
    while (count++ < MAX)
	 {
		data = __wctdm_getreg(wc, card, I_STATUS);

		if (!data)
			return 0;

	 }

    if(count > (MAX-1)) printk(" ##### Loop error (%02x) #####\n", data);

	return 0;
}

static unsigned char translate_3215(unsigned char address)
{
	int x;
	for (x=0;x<sizeof(indirect_regs)/sizeof(indirect_regs[0]);x++) {
		if (indirect_regs[x].address == address) {
			address = indirect_regs[x].altaddr;
			break;
		}
	}
	return address;
}

static int wctdm_proslic_setreg_indirect(struct wctdm *wc, int card, unsigned char address, unsigned short data)
{
	unsigned long flags;
	int res = -1;
	/* Translate 3215 addresses */
	if (wc->flags[card] & FLAG_3215) {
		address = translate_3215(address);
		if (address == 255)
			return 0;
	}
	spin_lock_irqsave(&wc->lock, flags);
	if(!__wait_access(wc, card)) {
		__wctdm_setreg(wc, card, IDA_LO,(unsigned char)(data & 0xFF));
		__wctdm_setreg(wc, card, IDA_HI,(unsigned char)((data & 0xFF00)>>8));
		__wctdm_setreg(wc, card, IAA,address);
		res = 0;
	};
	spin_unlock_irqrestore(&wc->lock, flags);
	return res;
}

static int wctdm_proslic_getreg_indirect(struct wctdm *wc, int card, unsigned char address)
{ 
	unsigned long flags;
	int res = -1;
	char *p=NULL;
	/* Translate 3215 addresses */
	if (wc->flags[card] & FLAG_3215) {
		address = translate_3215(address);
		if (address == 255)
			return 0;
	}
	spin_lock_irqsave(&wc->lock, flags);
	if (!__wait_access(wc, card)) {
		__wctdm_setreg(wc, card, IAA, address);
		if (!__wait_access(wc, card)) {
			unsigned char data1, data2;
			data1 = __wctdm_getreg(wc, card, IDA_LO);
			data2 = __wctdm_getreg(wc, card, IDA_HI);
			res = data1 | (data2 << 8);
		} else
			p = "Failed to wait inside\n";
	} else
		p = "failed to wait\n";
	spin_unlock_irqrestore(&wc->lock, flags);
	if (p)
		printk(p);
	return res;
}

static int wctdm_proslic_init_indirect_regs(struct wctdm *wc, int card)
{
	unsigned char i;

	for (i=0; i<sizeof(indirect_regs) / sizeof(indirect_regs[0]); i++)
	{
		if(wctdm_proslic_setreg_indirect(wc, card, indirect_regs[i].address,indirect_regs[i].initial))
			return -1;
	}

	return 0;
}

static int wctdm_proslic_verify_indirect_regs(struct wctdm *wc, int card)
{ 
	int passed = 1;
	unsigned short i, initial;
	int j;

	for (i=0; i<sizeof(indirect_regs) / sizeof(indirect_regs[0]); i++) 
	{
		if((j = wctdm_proslic_getreg_indirect(wc, card, (unsigned char) indirect_regs[i].address)) < 0) {
			printk("Failed to read indirect register %d\n", i);
			return -1;
		}
		initial= indirect_regs[i].initial;

		if ( j != initial && (!(wc->flags[card] & FLAG_3215) || (indirect_regs[i].altaddr != 255)))
		{
			 printk("!!!!!!! %s  iREG %X = %X  should be %X\n",
				indirect_regs[i].name,indirect_regs[i].address,j,initial );
			 passed = 0;
		}	
	}

    if (passed) {
		if (debug)
			printk("Init Indirect Registers completed successfully.\n");
    } else {
		printk(" !!!!! Init Indirect Registers UNSUCCESSFULLY.\n");
		return -1;
    }
    return 0;
}

static inline void wctdm_proslic_recheck_sanity(struct wctdm *wc, int card)
{
	int res;
	/* Check loopback */
	res = wc->reg1shadow[card];
	if (!res && (res != wc->mod[card].fxs.lasttxhook)) {
		res = wctdm_getreg(wc, card, 8);
		if (res) {
			printk("Ouch, part reset, quickly restoring reality (%d)\n", card);
			wctdm_init_proslic(wc, card, 1, 0, 1);
		} else {
			if (wc->mod[card].fxs.palarms++ < MAX_ALARMS) {
				printk("Power alarm on module %d, resetting!\n", card + 1);
				if (wc->mod[card].fxs.lasttxhook == 4)
					wc->mod[card].fxs.lasttxhook = 1;
				wctdm_setreg(wc, card, 64, wc->mod[card].fxs.lasttxhook);
			} else {
				if (wc->mod[card].fxs.palarms == MAX_ALARMS)
					printk("Too many power alarms on card %d, NOT resetting!\n", card + 1);
			}
		}
	}
}

static inline void wctdm_voicedaa_check_hook(struct wctdm *wc, int card)
{
#ifndef AUDIO_RINGCHECK
	unsigned char res;
#endif	
	signed char b;
	int poopy = 0;
	/* Try to track issues that plague slot one FXO's */
	b = wc->reg0shadow[card];
	if ((b & 0x2) || !(b & 0x8)) {
		/* Not good -- don't look at anything else */
		if (debug)
			printk("Poopy (%02x) on card %d!\n", b, card + 1); 
		poopy++;
	}
	b &= 0x9b;
	if (wc->mod[card].fxo.offhook) {
		if (b != 0x9)
			wctdm_setreg(wc, card, 5, 0x9);
	} else {
		if (b != 0x8)
			wctdm_setreg(wc, card, 5, 0x8);
	}
	if (poopy)
		return;
#ifndef AUDIO_RINGCHECK
	if (!wc->mod[card].fxo.offhook) {
		res = wc->reg0shadow[card];
		if ((res & 0x60) && wc->mod[card].fxo.battery) {
			wc->mod[card].fxo.ringdebounce += (ZT_CHUNKSIZE * 16);
			if (wc->mod[card].fxo.ringdebounce >= ZT_CHUNKSIZE * 64) {
				if (!wc->mod[card].fxo.wasringing) {
					wc->mod[card].fxo.wasringing = 1;
					zt_hooksig(&wc->chans[card], ZT_RXSIG_RING);
					if (debug)
						printk("RING on %d/%d!\n", wc->span.spanno, card + 1);
				}
				wc->mod[card].fxo.ringdebounce = ZT_CHUNKSIZE * 64;
			}
		} else {
			wc->mod[card].fxo.ringdebounce -= ZT_CHUNKSIZE * 4;
			if (wc->mod[card].fxo.ringdebounce <= 0) {
				if (wc->mod[card].fxo.wasringing) {
					wc->mod[card].fxo.wasringing = 0;
					zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
					if (debug)
						printk("NO RING on %d/%d!\n", wc->span.spanno, card + 1);
				}
				wc->mod[card].fxo.ringdebounce = 0;
			}
				
		}
	}
#endif
	b = wc->reg1shadow[card];
#if 0 
	{
		static int count = 0;
		if (!(count++ % 100)) {
			printk("Card %d: Voltage: %d  Debounce %d\n", card + 1, 
			       b, wc->mod[card].fxo.battdebounce);
		}
	}
#endif	
	if (abs(b) < battthresh) {
		wc->mod[card].fxo.nobatttimer++;
#if 0
		if (wc->mod[card].fxo.battery)
			printk("Battery loss: %d (%d debounce)\n", b, wc->mod[card].fxo.battdebounce);
#endif
		if (wc->mod[card].fxo.battery && !wc->mod[card].fxo.battdebounce) {
			if (debug)
				printk("NO BATTERY on %d/%d!\n", wc->span.spanno, card + 1);
			wc->mod[card].fxo.battery =  0;
#ifdef	JAPAN
			if ((!wc->ohdebounce) && wc->offhook) {
				zt_hooksig(&wc->chans[card], ZT_RXSIG_ONHOOK);
				if (debug)
					printk("Signalled On Hook\n");
#ifdef	ZERO_BATT_RING
				wc->onhook++;
#endif
			}
#else
			zt_hooksig(&wc->chans[card], ZT_RXSIG_ONHOOK);
#endif
			wc->mod[card].fxo.battdebounce = battdebounce;
		} else if (!wc->mod[card].fxo.battery)
			wc->mod[card].fxo.battdebounce = battdebounce;
	} else if (abs(b) > battthresh) {
		if (!wc->mod[card].fxo.battery && !wc->mod[card].fxo.battdebounce) {
			if (debug)
				printk("BATTERY on %d/%d (%s)!\n", wc->span.spanno, card + 1, 
					(b < 0) ? "-" : "+");			    
#ifdef	ZERO_BATT_RING
			if (wc->onhook) {
				wc->onhook = 0;
				zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
				if (debug)
					printk("Signalled Off Hook\n");
			}
#else
			zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
#endif
			wc->mod[card].fxo.battery = 1;
			wc->mod[card].fxo.nobatttimer = 0;
			wc->mod[card].fxo.battdebounce = battdebounce;
		} else if (wc->mod[card].fxo.battery)
			wc->mod[card].fxo.battdebounce = battdebounce;

		if (wc->mod[card].fxo.lastpol >= 0) {
		    if (b < 0) {
			wc->mod[card].fxo.lastpol = -1;
			wc->mod[card].fxo.polaritydebounce = POLARITY_DEBOUNCE;
		    }
		} 
		if (wc->mod[card].fxo.lastpol <= 0) {
		    if (b > 0) {
			wc->mod[card].fxo.lastpol = 1;
			wc->mod[card].fxo.polaritydebounce = POLARITY_DEBOUNCE;
		    }
		}
	} else {
		/* It's something else... */
		wc->mod[card].fxo.battdebounce = battdebounce;
	}
	if (wc->mod[card].fxo.battdebounce)
		wc->mod[card].fxo.battdebounce--;
	if (wc->mod[card].fxo.polaritydebounce) {
	        wc->mod[card].fxo.polaritydebounce--;
		if (wc->mod[card].fxo.polaritydebounce < 1) {
		    if (wc->mod[card].fxo.lastpol != wc->mod[card].fxo.polarity) {
				if (debug)
					printk("%lu Polarity reversed (%d -> %d)\n", jiffies, 
				       wc->mod[card].fxo.polarity, 
				       wc->mod[card].fxo.lastpol);
				if (wc->mod[card].fxo.polarity)
				    zt_qevent_lock(&wc->chans[card], ZT_EVENT_POLARITY);
				wc->mod[card].fxo.polarity = wc->mod[card].fxo.lastpol;
		    }
		}
	}
}

static inline void wctdm_proslic_check_hook(struct wctdm *wc, int card)
{
	char res;
	int hook;

	/* For some reason we have to debounce the
	   hook detector.  */

	res = wc->reg0shadow[card];
	hook = (res & 1);
	if (hook != wc->mod[card].fxs.lastrxhook) {
		/* Reset the debounce (must be multiple of 4ms) */
		wc->mod[card].fxs.debounce = 8 * (4 * 8);
#if 0
		printk("Resetting debounce card %d hook %d, %d\n", card, hook, wc->mod[card].fxs.debounce);
#endif
	} else {
		if (wc->mod[card].fxs.debounce > 0) {
			wc->mod[card].fxs.debounce-= 16 * ZT_CHUNKSIZE;
#if 0
			printk("Sustaining hook %d, %d\n", hook, wc->mod[card].fxs.debounce);
#endif
			if (!wc->mod[card].fxs.debounce) {
#if 0
				printk("Counted down debounce, newhook: %d...\n", hook);
#endif
				wc->mod[card].fxs.debouncehook = hook;
			}
			if (!wc->mod[card].fxs.oldrxhook && wc->mod[card].fxs.debouncehook) {
				/* Off hook */
#if 1
				if (debug)
#endif				
					printk("wctdm: Card %d Going off hook\n", card);
				zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
				if (robust)
					wctdm_init_proslic(wc, card, 1, 0, 1);
				wc->mod[card].fxs.oldrxhook = 1;
			
			} else if (wc->mod[card].fxs.oldrxhook && !wc->mod[card].fxs.debouncehook) {
				/* On hook */
#if 1
				if (debug)
#endif				
					printk("wctdm: Card %d Going on hook\n", card);
				zt_hooksig(&wc->chans[card], ZT_RXSIG_ONHOOK);
				wc->mod[card].fxs.oldrxhook = 0;
			}
		}
	}
	wc->mod[card].fxs.lastrxhook = hook;
}

ZAP_IRQ_HANDLER(wctdm_interrupt)
{
	struct wctdm *wc = dev_id;
	unsigned char ints;
	int x;
	int mode;

	ints = inb(wc->ioaddr + WC_INTSTAT);

	if (!ints)
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		

	outb(ints, wc->ioaddr + WC_INTSTAT);

	if (ints & 0x10) {
		/* Stop DMA, wait for watchdog */
		printk("TDM PCI Master abort\n");
		wctdm_stop_dma(wc);
#ifdef LINUX26
		return IRQ_RETVAL(1);
#else
		return;
#endif		
	}
	
	if (ints & 0x20) {
		printk("PCI Target abort\n");
#ifdef LINUX26
		return IRQ_RETVAL(1);
#else
		return;
#endif		
	}

	for (x=0;x<4;x++) {
		if (wc->cardflag & (1 << x) &&
		    (wc->modtype[x] == MOD_TYPE_FXS)) {
			if (wc->mod[x].fxs.lasttxhook == 0x4) {
				/* RINGing, prepare for OHT */
				wc->mod[x].fxs.ohttimer = OHT_TIMER << 3;
				if (reversepolarity)
					wc->mod[x].fxs.idletxhookstate = 0x6;	/* OHT mode when idle */
				else
					wc->mod[x].fxs.idletxhookstate = 0x2; 
			} else {
				if (wc->mod[x].fxs.ohttimer) {
					wc->mod[x].fxs.ohttimer-= ZT_CHUNKSIZE;
					if (!wc->mod[x].fxs.ohttimer) {
						if (reversepolarity)
							wc->mod[x].fxs.idletxhookstate = 0x5;	/* Switch to active */
						else
							wc->mod[x].fxs.idletxhookstate = 0x1;
						if ((wc->mod[x].fxs.lasttxhook == 0x2) || (wc->mod[x].fxs.lasttxhook == 0x6)) {
							/* Apply the change if appropriate */
							if (reversepolarity) 
								wc->mod[x].fxs.lasttxhook = 0x5;
							else
								wc->mod[x].fxs.lasttxhook = 0x1;
							wctdm_setreg(wc, x, 64, wc->mod[x].fxs.lasttxhook);
						}
					}
				}
			}
		}
	}

	if (ints & 0x0f) {
		wc->intcount++;
		x = wc->intcount & 0x3;
		mode = wc->intcount & 0xc;
		if (wc->cardflag & (1 << x)) {
			switch(mode) {
			case 0:
				/* Rest */
				break;
			case 4:
				/* Read first shadow reg */
				if (wc->modtype[x] == MOD_TYPE_FXS)
					wc->reg0shadow[x] = wctdm_getreg(wc, x, 68);
				else if (wc->modtype[x] == MOD_TYPE_FXO)
					wc->reg0shadow[x] = wctdm_getreg(wc, x, 5);
				break;
			case 8:
				/* Read second shadow reg */
				if (wc->modtype[x] == MOD_TYPE_FXS)
					wc->reg1shadow[x] = wctdm_getreg(wc, x, 64);
				else if (wc->modtype[x] == MOD_TYPE_FXO)
					wc->reg1shadow[x] = wctdm_getreg(wc, x, 29);
				break;
			case 12:
				/* Perform processing */
				if (wc->modtype[x] == MOD_TYPE_FXS) {
					wctdm_proslic_check_hook(wc, x);
					if (!(wc->intcount & 0xf0))
						wctdm_proslic_recheck_sanity(wc, x);
					} else if (wc->modtype[x] == MOD_TYPE_FXO) {
					wctdm_voicedaa_check_hook(wc, x);
				}
				break;
			}
		}
		if (!(wc->intcount % 10000)) {
			/* Accept an alarm once per 10 seconds */
			for (x=0;x<4;x++) 
				if (wc->modtype[x] == MOD_TYPE_FXS) {
					if (wc->mod[x].fxs.palarms)
						wc->mod[x].fxs.palarms--;
				}
		}
		wctdm_receiveprep(wc, ints);
		wctdm_transmitprep(wc, ints);
	}
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
	
}

static int wctdm_voicedaa_insane(struct wctdm *wc, int card)
{
	int blah;
	blah = wctdm_getreg(wc, card, 2);
	if (blah != 0x3)
		return -2;
	blah = wctdm_getreg(wc, card, 11);
	if (debug)
		printk("VoiceDAA System: %02x\n", blah & 0xf);
	return 0;
}

static int wctdm_proslic_insane(struct wctdm *wc, int card)
{
	int blah,insane_report;
	insane_report=0;

	blah = wctdm_getreg(wc, card, 0);
	if (debug) 
		printk("ProSLIC on module %d, product %d, version %d\n", card, (blah & 0x30) >> 4, (blah & 0xf));

#if 0
	if ((blah & 0x30) >> 4) {
		printk("ProSLIC on module %d is not a 3210.\n", card);
		return -1;
	}
#endif
	if (((blah & 0xf) == 0) || ((blah & 0xf) == 0xf)) {
		/* SLIC not loaded */
		return -1;
	}
	if ((blah & 0xf) < 2) {
		printk("ProSLIC 3210 version %d is too old\n", blah & 0xf);
		return -1;
	}
	if (wctdm_getreg(wc, card, 1) & 0x80)
		/* ProSLIC 3215, not a 3210 */
		wc->flags[card] |= FLAG_3215;

	blah = wctdm_getreg(wc, card, 8);
	if (blah != 0x2) {
		printk("ProSLIC on module %d insane (1) %d should be 2\n", card, blah);
		return -1;
	} else if ( insane_report)
		printk("ProSLIC on module %d Reg 8 Reads %d Expected is 0x2\n",card,blah);

	blah = wctdm_getreg(wc, card, 64);
	if (blah != 0x0) {
		printk("ProSLIC on module %d insane (2)\n", card);
		return -1;
	} else if ( insane_report)
		printk("ProSLIC on module %d Reg 64 Reads %d Expected is 0x0\n",card,blah);

	blah = wctdm_getreg(wc, card, 11);
	if (blah != 0x33) {
		printk("ProSLIC on module %d insane (3)\n", card);
		return -1;
	} else if ( insane_report)
		printk("ProSLIC on module %d Reg 11 Reads %d Expected is 0x33\n",card,blah);

	/* Just be sure it's setup right. */
	wctdm_setreg(wc, card, 30, 0);

	if (debug) 
		printk("ProSLIC on module %d seems sane.\n", card);
	return 0;
}

static int wctdm_proslic_powerleak_test(struct wctdm *wc, int card)
{
	unsigned long origjiffies;
	unsigned char vbat;

	/* Turn off linefeed */
	wctdm_setreg(wc, card, 64, 0);

	/* Power down */
	wctdm_setreg(wc, card, 14, 0x10);

	/* Wait for one second */
	origjiffies = jiffies;

	while((vbat = wctdm_getreg(wc, card, 82)) > 0x6) {
		if ((jiffies - origjiffies) >= (HZ/2))
			break;;
	}

	if (vbat < 0x06) {
		printk("Excessive leakage detected on module %d: %d volts (%02x) after %d ms\n", card,
		       376 * vbat / 1000, vbat, (int)((jiffies - origjiffies) * 1000 / HZ));
		return -1;
	} else if (debug) {
		printk("Post-leakage voltage: %d volts\n", 376 * vbat / 1000);
	}
	return 0;
}

static int wctdm_powerup_proslic(struct wctdm *wc, int card, int fast)
{
	unsigned char vbat;
	unsigned long origjiffies;
	int lim;

	/* Set period of DC-DC converter to 1/64 khz */
	wctdm_setreg(wc, card, 92, 0xff /* was 0xff */);

	/* Wait for VBat to powerup */
	origjiffies = jiffies;

	/* Disable powerdown */
	wctdm_setreg(wc, card, 14, 0);

	/* If fast, don't bother checking anymore */
	if (fast)
		return 0;

	while((vbat = wctdm_getreg(wc, card, 82)) < 0xc0) {
		/* Wait no more than 500ms */
		if ((jiffies - origjiffies) > HZ/2) {
			break;
		}
	}

	if (vbat < 0xc0) {
		if (wc->proslic_power == PROSLIC_POWER_UNKNOWN)
				 printk("ProSLIC on module %d failed to powerup within %d ms (%d mV only)\n\n -- DID YOU REMEMBER TO PLUG IN THE HD POWER CABLE TO THE TDM400P??\n",
					card, (int)(((jiffies - origjiffies) * 1000 / HZ)),
					vbat * 375);
		wc->proslic_power = PROSLIC_POWER_WARNED;
		return -1;
	} else if (debug) {
		printk("ProSLIC on module %d powered up to -%d volts (%02x) in %d ms\n",
		       card, vbat * 376 / 1000, vbat, (int)(((jiffies - origjiffies) * 1000 / HZ)));
	}
	wc->proslic_power = PROSLIC_POWER_ON;

        /* Proslic max allowed loop current, reg 71 LOOP_I_LIMIT */
        /* If out of range, just set it to the default value     */
        lim = (loopcurrent - 20) / 3;
        if ( loopcurrent > 41 ) {
                lim = 0;
                if (debug)
                        printk("Loop current out of range! Setting to default 20mA!\n");
        }
        else if (debug)
                        printk("Loop current set to %dmA!\n",(lim*3)+20);
        wctdm_setreg(wc,card,LOOP_I_LIMIT,lim);

	/* Engage DC-DC converter */
	wctdm_setreg(wc, card, 93, 0x19 /* was 0x19 */);
#if 0
	origjiffies = jiffies;
	while(0x80 & wctdm_getreg(wc, card, 93)) {
		if ((jiffies - origjiffies) > 2 * HZ) {
			printk("Timeout waiting for DC-DC calibration on module %d\n", card);
			return -1;
		}
	}

#if 0
	/* Wait a full two seconds */
	while((jiffies - origjiffies) < 2 * HZ);

	/* Just check to be sure */
	vbat = wctdm_getreg(wc, card, 82);
	printk("ProSLIC on module %d powered up to -%d volts (%02x) in %d ms\n",
		       card, vbat * 376 / 1000, vbat, (int)(((jiffies - origjiffies) * 1000 / HZ)));
#endif
#endif
	return 0;

}

static int wctdm_proslic_manual_calibrate(struct wctdm *wc, int card){
	unsigned long origjiffies;
	unsigned char i;

	wctdm_setreg(wc, card, 21, 0);//(0)  Disable all interupts in DR21
	wctdm_setreg(wc, card, 22, 0);//(0)Disable all interupts in DR21
	wctdm_setreg(wc, card, 23, 0);//(0)Disable all interupts in DR21
	wctdm_setreg(wc, card, 64, 0);//(0)

	wctdm_setreg(wc, card, 97, 0x18); //(0x18)Calibrations without the ADC and DAC offset and without common mode calibration.
	wctdm_setreg(wc, card, 96, 0x47); //(0x47)	Calibrate common mode and differential DAC mode DAC + ILIM

	origjiffies=jiffies;
	while( wctdm_getreg(wc,card,96)!=0 ){
		if((jiffies-origjiffies)>80)
			return -1;
	}
//Initialized DR 98 and 99 to get consistant results.
// 98 and 99 are the results registers and the search should have same intial conditions.

/*******************************The following is the manual gain mismatch calibration****************************/
/*******************************This is also available as a function *******************************************/
	// Delay 10ms
	origjiffies=jiffies; 
	while((jiffies-origjiffies)<1);
	wctdm_proslic_setreg_indirect(wc, card, 88,0);
	wctdm_proslic_setreg_indirect(wc,card,89,0);
	wctdm_proslic_setreg_indirect(wc,card,90,0);
	wctdm_proslic_setreg_indirect(wc,card,91,0);
	wctdm_proslic_setreg_indirect(wc,card,92,0);
	wctdm_proslic_setreg_indirect(wc,card,93,0);

	wctdm_setreg(wc, card, 98,0x10); // This is necessary if the calibration occurs other than at reset time
	wctdm_setreg(wc, card, 99,0x10);

	for ( i=0x1f; i>0; i--)
	{
		wctdm_setreg(wc, card, 98,i);
		origjiffies=jiffies; 
		while((jiffies-origjiffies)<4);
		if((wctdm_getreg(wc,card,88)) == 0)
			break;
	} // for

	for ( i=0x1f; i>0; i--)
	{
		wctdm_setreg(wc, card, 99,i);
		origjiffies=jiffies; 
		while((jiffies-origjiffies)<4);
		if((wctdm_getreg(wc,card,89)) == 0)
			break;
	}//for

/*******************************The preceding is the manual gain mismatch calibration****************************/
/**********************************The following is the longitudinal Balance Cal***********************************/
	wctdm_setreg(wc,card,64,1);
	while((jiffies-origjiffies)<10); // Sleep 100?

	wctdm_setreg(wc, card, 64, 0);
	wctdm_setreg(wc, card, 23, 0x4);  // enable interrupt for the balance Cal
	wctdm_setreg(wc, card, 97, 0x1); // this is a singular calibration bit for longitudinal calibration
	wctdm_setreg(wc, card, 96,0x40);

	wctdm_getreg(wc,card,96); /* Read Reg 96 just cause */

	wctdm_setreg(wc, card, 21, 0xFF);
	wctdm_setreg(wc, card, 22, 0xFF);
	wctdm_setreg(wc, card, 23, 0xFF);

	/**The preceding is the longitudinal Balance Cal***/
	return(0);

}
#if 1
static int wctdm_proslic_calibrate(struct wctdm *wc, int card)
{
	unsigned long origjiffies;
	int x;
	/* Perform all calibrations */
	wctdm_setreg(wc, card, 97, 0x1f);
	
	/* Begin, no speedup */
	wctdm_setreg(wc, card, 96, 0x5f);

	/* Wait for it to finish */
	origjiffies = jiffies;
	while(wctdm_getreg(wc, card, 96)) {
		if ((jiffies - origjiffies) > 2 * HZ) {
			printk("Timeout waiting for calibration of module %d\n", card);
			return -1;
		}
	}
	
	if (debug) {
		/* Print calibration parameters */
		printk("Calibration Vector Regs 98 - 107: \n");
		for (x=98;x<108;x++) {
			printk("%d: %02x\n", x, wctdm_getreg(wc, card, x));
		}
	}
	return 0;
}
#endif

static void wait_just_a_bit(int foo)
{
	long newjiffies;
	newjiffies = jiffies + foo;
	while(jiffies < newjiffies);
}

static int wctdm_init_voicedaa(struct wctdm *wc, int card, int fast, int manual, int sane)
{
	unsigned char reg16=0, reg26=0, reg30=0, reg31=0;
	long newjiffies;
	wc->modtype[card] = MOD_TYPE_FXO;
	/* Sanity check the ProSLIC */
	reset_spi(wc, card);
	if (!sane && wctdm_voicedaa_insane(wc, card))
		return -2;

	/* Software reset */
	wctdm_setreg(wc, card, 1, 0x80);

	/* Wait just a bit */
	wait_just_a_bit(HZ/10);

	/* Enable PCM, ulaw */
	if (alawoverride)
		wctdm_setreg(wc, card, 33, 0x20);
	else
		wctdm_setreg(wc, card, 33, 0x28);

	/* Set On-hook speed, Ringer impedence, and ringer threshold */
	reg16 |= (fxo_modes[_opermode].ohs << 6);
	reg16 |= (fxo_modes[_opermode].rz << 1);
	reg16 |= (fxo_modes[_opermode].rt);
	wctdm_setreg(wc, card, 16, reg16);
	
	/* Set DC Termination:
	   Tip/Ring voltage adjust, minimum operational current, current limitation */
	reg26 |= (fxo_modes[_opermode].dcv << 6);
	reg26 |= (fxo_modes[_opermode].mini << 4);
	reg26 |= (fxo_modes[_opermode].ilim << 1);
	wctdm_setreg(wc, card, 26, reg26);

	/* Set AC Impedence */
	reg30 = (fxo_modes[_opermode].acim);
	wctdm_setreg(wc, card, 30, reg30);

	/* Misc. DAA parameters */
	if (fastpickup)
		reg31 = 0xb3;
	else
		reg31 = 0xa3;

	reg31 |= (fxo_modes[_opermode].ohs2 << 3);
	wctdm_setreg(wc, card, 31, reg31);

	/* Set Transmit/Receive timeslot */
	wctdm_setreg(wc, card, 34, (3-card) * 8);
	wctdm_setreg(wc, card, 35, 0x00);
	wctdm_setreg(wc, card, 36, (3-card) * 8);
	wctdm_setreg(wc, card, 37, 0x00);

	/* Enable ISO-Cap */
	wctdm_setreg(wc, card, 6, 0x00);

	if (fastpickup)
		wctdm_setreg(wc, card, 17, wctdm_getreg(wc, card, 17) | 0x20);

	/* Wait 1000ms for ISO-cap to come up */
	newjiffies = jiffies;
	newjiffies += 2 * HZ;
	while((jiffies < newjiffies) && !(wctdm_getreg(wc, card, 11) & 0xf0))
		wait_just_a_bit(HZ/10);

	if (!(wctdm_getreg(wc, card, 11) & 0xf0)) {
		printk("VoiceDAA did not bring up ISO link properly!\n");
		return -1;
	}
	if (debug)
		printk("ISO-Cap is now up, line side: %02x rev %02x\n", 
		       wctdm_getreg(wc, card, 11) >> 4,
		       (wctdm_getreg(wc, card, 13) >> 2) & 0xf);
	/* Enable on-hook line monitor */
	wctdm_setreg(wc, card, 5, 0x08);
	
	/* Take values for fxotxgain and fxorxgain and apply them to module */
	if (fxotxgain) {
		if (fxotxgain >=  -150 && fxotxgain < 0) {
			wctdm_setreg(wc, card, 38, 16 + (fxotxgain/-10));
			if(fxotxgain % 10) {
				wctdm_setreg(wc, card, 40, 16 + (-fxotxgain%10));
			}
		}
		else if (fxotxgain <= 120 && fxotxgain > 0) {
			wctdm_setreg(wc, card, 38, fxotxgain/10);
			if(fxotxgain%10) {
				wctdm_setreg(wc, card, 40, (fxotxgain%10));
			}
		}
	}
	if (fxorxgain) {
		if (fxorxgain >=  -150 && fxorxgain < 0) {
			wctdm_setreg(wc, card, 39, 16 + (fxorxgain/-10));
			if(fxotxgain%10) {
				wctdm_setreg(wc, card, 41, 16 + (-fxorxgain%10));
			}
		}
		else if (fxorxgain <= 120 && fxorxgain > 0) {
			wctdm_setreg(wc, card, 39, fxorxgain/10);
			if(fxorxgain % 10) {
				wctdm_setreg(wc, card, 41, (fxorxgain%10));
			}
		}
	}

	/* NZ -- crank the tx gain up by 7 dB */
	if (!strcmp(fxo_modes[_opermode].name, "NEWZEALAND")) {
		printk("Adjusting gain\n");
		wctdm_setreg(wc, card, 38, 0x7);
	}
	
	if(debug)
		printk("DEBUG fxotxgain:%i.%i fxorxgain:%i.%i\n", (wctdm_getreg(wc, card, 38)/16)?-(wctdm_getreg(wc, card, 38) - 16) : wctdm_getreg(wc, card, 38), (wctdm_getreg(wc, card, 40)/16)? -(wctdm_getreg(wc, card, 40) - 16):wctdm_getreg(wc, card, 40), (wctdm_getreg(wc, card, 39)/16)? -(wctdm_getreg(wc, card, 39) - 16) : wctdm_getreg(wc, card, 39),(wctdm_getreg(wc, card, 41)/16)?-(wctdm_getreg(wc, card, 41) - 16):wctdm_getreg(wc, card, 41));

	return 0;
		
}

static int wctdm_init_proslic(struct wctdm *wc, int card, int fast, int manual, int sane)
{

	unsigned short tmp[5];
	unsigned char r19,r9;
	int x;
	int fxsmode=0;

	/* By default, don't send on hook */
	if (reversepolarity)
		wc->mod[card].fxs.idletxhookstate = 5;
	else
		wc->mod[card].fxs.idletxhookstate = 1;

	/* Sanity check the ProSLIC */
	if (!sane && wctdm_proslic_insane(wc, card))
		return -2;
		
	if (sane) {
		/* Make sure we turn off the DC->DC converter to prevent anything from blowing up */
		wctdm_setreg(wc, card, 14, 0x10);
	}

	if (wctdm_proslic_init_indirect_regs(wc, card)) {
		printk(KERN_INFO "Indirect Registers failed to initialize on module %d.\n", card);
		return -1;
	}

	/* Clear scratch pad area */
	wctdm_proslic_setreg_indirect(wc, card, 97,0);

	/* Clear digital loopback */
	wctdm_setreg(wc, card, 8, 0);

	/* Revision C optimization */
	wctdm_setreg(wc, card, 108, 0xeb);

	/* Disable automatic VBat switching for safety to prevent
	   Q7 from accidently turning on and burning out. */
	wctdm_setreg(wc, card, 67, 0x17);

	/* Turn off Q7 */
	wctdm_setreg(wc, card, 66, 1);

	/* Flush ProSLIC digital filters by setting to clear, while
	   saving old values */
	for (x=0;x<5;x++) {
		tmp[x] = wctdm_proslic_getreg_indirect(wc, card, x + 35);
		wctdm_proslic_setreg_indirect(wc, card, x + 35, 0x8000);
	}

	/* Power up the DC-DC converter */
	if (wctdm_powerup_proslic(wc, card, fast)) {
		printk("Unable to do INITIAL ProSLIC powerup on module %d\n", card);
		return -1;
	}

	if (!fast) {

		/* Check for power leaks */
		if (wctdm_proslic_powerleak_test(wc, card)) {
			printk("ProSLIC module %d failed leakage test.  Check for short circuit\n", card);
		}
		/* Power up again */
		if (wctdm_powerup_proslic(wc, card, fast)) {
			printk("Unable to do FINAL ProSLIC powerup on module %d\n", card);
			return -1;
		}
#ifndef NO_CALIBRATION
		/* Perform calibration */
		if(manual) {
			if (wctdm_proslic_manual_calibrate(wc, card)) {
				//printk("Proslic failed on Manual Calibration\n");
				if (wctdm_proslic_manual_calibrate(wc, card)) {
					printk("Proslic Failed on Second Attempt to Calibrate Manually. (Try -DNO_CALIBRATION in Makefile)\n");
					return -1;
				}
				printk("Proslic Passed Manual Calibration on Second Attempt\n");
			}
		}
		else {
			if(wctdm_proslic_calibrate(wc, card))  {
				//printk("ProSlic died on Auto Calibration.\n");
				if (wctdm_proslic_calibrate(wc, card)) {
					printk("Proslic Failed on Second Attempt to Auto Calibrate\n");
					return -1;
				}
				printk("Proslic Passed Auto Calibration on Second Attempt\n");
			}
		}
		/* Perform DC-DC calibration */
		wctdm_setreg(wc, card, 93, 0x99);
		r19 = wctdm_getreg(wc, card, 107);
		if ((r19 < 0x2) || (r19 > 0xd)) {
			printk("DC-DC cal has a surprising direct 107 of 0x%02x!\n", r19);
			wctdm_setreg(wc, card, 107, 0x8);
		}

		/* Save calibration vectors */
		for (x=0;x<NUM_CAL_REGS;x++)
			wc->mod[card].fxs.calregs.vals[x] = wctdm_getreg(wc, card, 96 + x);
#endif

	} else {
		/* Restore calibration registers */
		for (x=0;x<NUM_CAL_REGS;x++)
			wctdm_setreg(wc, card, 96 + x, wc->mod[card].fxs.calregs.vals[x]);
	}
	/* Calibration complete, restore original values */
	for (x=0;x<5;x++) {
		wctdm_proslic_setreg_indirect(wc, card, x + 35, tmp[x]);
	}

	if (wctdm_proslic_verify_indirect_regs(wc, card)) {
		printk(KERN_INFO "Indirect Registers failed verification.\n");
		return -1;
	}


#if 0
    /* Disable Auto Power Alarm Detect and other "features" */
    wctdm_setreg(wc, card, 67, 0x0e);
    blah = wctdm_getreg(wc, card, 67);
#endif

#if 0
    if (wctdm_proslic_setreg_indirect(wc, card, 97, 0x0)) { // Stanley: for the bad recording fix
		 printk(KERN_INFO "ProSlic IndirectReg Died.\n");
		 return -1;
	}
#endif

    if (alawoverride)
    	wctdm_setreg(wc, card, 1, 0x20);
    else
    	wctdm_setreg(wc, card, 1, 0x28);
 	// U-Law 8-bit interface
    wctdm_setreg(wc, card, 2, (3-card) * 8);    // Tx Start count low byte  0
    wctdm_setreg(wc, card, 3, 0);    // Tx Start count high byte 0
    wctdm_setreg(wc, card, 4, (3-card) * 8);    // Rx Start count low byte  0
    wctdm_setreg(wc, card, 5, 0);    // Rx Start count high byte 0
    wctdm_setreg(wc, card, 18, 0xff);     // clear all interrupt
    wctdm_setreg(wc, card, 19, 0xff);
    wctdm_setreg(wc, card, 20, 0xff);
    wctdm_setreg(wc, card, 73, 0x04);
	if (fxshonormode) {
		fxsmode = acim2tiss[fxo_modes[_opermode].acim];
		wctdm_setreg(wc, card, 10, 0x08 | fxsmode);
		if (fxo_modes[_opermode].ring_osc)
			wctdm_proslic_setreg_indirect(wc, card, 20, fxo_modes[_opermode].ring_osc);
		if (fxo_modes[_opermode].ring_x)
			wctdm_proslic_setreg_indirect(wc, card, 21, fxo_modes[_opermode].ring_x);
	}
    if (lowpower)
    	wctdm_setreg(wc, card, 72, 0x10);

#if 0
    wctdm_setreg(wc, card, 21, 0x00); 	// enable interrupt
    wctdm_setreg(wc, card, 22, 0x02); 	// Loop detection interrupt
    wctdm_setreg(wc, card, 23, 0x01); 	// DTMF detection interrupt
#endif

#if 0
    /* Enable loopback */
    wctdm_setreg(wc, card, 8, 0x2);
    wctdm_setreg(wc, card, 14, 0x0);
    wctdm_setreg(wc, card, 64, 0x0);
    wctdm_setreg(wc, card, 1, 0x08);
#endif

	if (fastringer) {
		/* Speed up Ringer */
		wctdm_proslic_setreg_indirect(wc, card, 20, 0x7e6d);
		wctdm_proslic_setreg_indirect(wc, card, 21, 0x01b9);
		/* Beef up Ringing voltage to 89V */
		if (boostringer) {
			wctdm_setreg(wc, card, 74, 0x3f);
			if (wctdm_proslic_setreg_indirect(wc, card, 21, 0x247)) 
				return -1;
			printk("Boosting fast ringer on slot %d (89V peak)\n", card + 1);
		} else if (lowpower) {
			if (wctdm_proslic_setreg_indirect(wc, card, 21, 0x14b)) 
				return -1;
			printk("Reducing fast ring power on slot %d (50V peak)\n", card + 1);
		} else
			printk("Speeding up ringer on slot %d (25Hz)\n", card + 1);
	} else {
		/* Beef up Ringing voltage to 89V */
		if (boostringer) {
			wctdm_setreg(wc, card, 74, 0x3f);
			if (wctdm_proslic_setreg_indirect(wc, card, 21, 0x1d1)) 
				return -1;
			printk("Boosting ringer on slot %d (89V peak)\n", card + 1);
		} else if (lowpower) {
			if (wctdm_proslic_setreg_indirect(wc, card, 21, 0x108)) 
				return -1;
			printk("Reducing ring power on slot %d (50V peak)\n", card + 1);
		}
	}

	if(fxstxgain || fxsrxgain) {
		r9 = wctdm_getreg(wc, card, 9);
		switch (fxstxgain) {
		
			case 35:
				r9+=8;
				break;
			case -35:
				r9+=4;
				break;
			case 0: 
				break;
		}
	
		switch (fxsrxgain) {
			
			case 35:
				r9+=2;
				break;
			case -35:
				r9+=1;
				break;
			case 0:
				break;
		}
		wctdm_setreg(wc,card,9,r9);
	}

	if(debug)
			printk("DEBUG: fxstxgain:%s fxsrxgain:%s\n",((wctdm_getreg(wc, card, 9)/8) == 1)?"3.5":(((wctdm_getreg(wc,card,9)/4) == 1)?"-3.5":"0.0"),((wctdm_getreg(wc, card, 9)/2) == 1)?"3.5":((wctdm_getreg(wc,card,9)%2)?"-3.5":"0.0"));

	wctdm_setreg(wc, card, 64, 0x01);
	return 0;
}


static int wctdm_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	struct wctdm_stats stats;
	struct wctdm_regs regs;
	struct wctdm_regop regop;
	struct wctdm_echo_coefs echoregs;
	struct wctdm *wc = chan->pvt;
	int x;
	switch (cmd) {
	case ZT_ONHOOKTRANSFER:
		if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_FXS)
			return -EINVAL;
		if (get_user(x, (int *)data))
			return -EFAULT;
		wc->mod[chan->chanpos - 1].fxs.ohttimer = x << 3;
		if (reversepolarity)
			wc->mod[chan->chanpos - 1].fxs.idletxhookstate = 0x6;	/* OHT mode when idle */
		else
			wc->mod[chan->chanpos - 1].fxs.idletxhookstate = 0x2;
		if (wc->mod[chan->chanpos - 1].fxs.lasttxhook == 0x1) {
				/* Apply the change if appropriate */
				if (reversepolarity)
					wc->mod[chan->chanpos - 1].fxs.lasttxhook = 0x6;
				else
					wc->mod[chan->chanpos - 1].fxs.lasttxhook = 0x2;
				wctdm_setreg(wc, chan->chanpos - 1, 64, wc->mod[chan->chanpos - 1].fxs.lasttxhook);
		}
		break;
	case ZT_SETPOLARITY:
		if (get_user(x, (int *)data))
			return -EFAULT;
		if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_FXS)
			return -EINVAL;
		/* Can't change polarity while ringing or when open */
		if ((wc->mod[chan->chanpos -1 ].fxs.lasttxhook == 0x04) ||
		    (wc->mod[chan->chanpos -1 ].fxs.lasttxhook == 0x00))
			return -EINVAL;

		if ((x && !reversepolarity) || (!x && reversepolarity))
			wc->mod[chan->chanpos - 1].fxs.lasttxhook |= 0x04;
		else
			wc->mod[chan->chanpos - 1].fxs.lasttxhook &= ~0x04;
		wctdm_setreg(wc, chan->chanpos - 1, 64, wc->mod[chan->chanpos - 1].fxs.lasttxhook);
		break;
	case WCTDM_GET_STATS:
		if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS) {
			stats.tipvolt = wctdm_getreg(wc, chan->chanpos - 1, 80) * -376;
			stats.ringvolt = wctdm_getreg(wc, chan->chanpos - 1, 81) * -376;
			stats.batvolt = wctdm_getreg(wc, chan->chanpos - 1, 82) * -376;
		} else if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO) {
			stats.tipvolt = (signed char)wctdm_getreg(wc, chan->chanpos - 1, 29) * 1000;
			stats.ringvolt = (signed char)wctdm_getreg(wc, chan->chanpos - 1, 29) * 1000;
			stats.batvolt = (signed char)wctdm_getreg(wc, chan->chanpos - 1, 29) * 1000;
		} else 
			return -EINVAL;
		if (copy_to_user((struct wctdm_stats *)data, &stats, sizeof(stats)))
			return -EFAULT;
		break;
	case WCTDM_GET_REGS:
		if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS) {
			for (x=0;x<NUM_INDIRECT_REGS;x++)
				regs.indirect[x] = wctdm_proslic_getreg_indirect(wc, chan->chanpos -1, x);
			for (x=0;x<NUM_REGS;x++)
				regs.direct[x] = wctdm_getreg(wc, chan->chanpos - 1, x);
		} else {
			memset(&regs, 0, sizeof(regs));
			for (x=0;x<NUM_FXO_REGS;x++)
				regs.direct[x] = wctdm_getreg(wc, chan->chanpos - 1, x);
		}
		if (copy_to_user((struct wctdm_regs *)data, &regs, sizeof(regs)))
			return -EFAULT;
		break;
	case WCTDM_SET_REG:
		if (copy_from_user(&regop, (struct wctdm_regop *)data, sizeof(regop)))
			return -EFAULT;
		if (regop.indirect) {
			if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_FXS)
				return -EINVAL;
			printk("Setting indirect %d to 0x%04x on %d\n", regop.reg, regop.val, chan->chanpos);
			wctdm_proslic_setreg_indirect(wc, chan->chanpos - 1, regop.reg, regop.val);
		} else {
			regop.val &= 0xff;
			printk("Setting direct %d to %04x on %d\n", regop.reg, regop.val, chan->chanpos);
			wctdm_setreg(wc, chan->chanpos - 1, regop.reg, regop.val);
		}
		break;
	case WCTDM_SET_ECHOTUNE:
		printk("-- Setting echo registers: \n");
		if (copy_from_user(&echoregs, (struct wctdm_echo_coefs*)data, sizeof(echoregs)))
			return -EFAULT;

		if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO) {
			/* Set the ACIM register */
			wctdm_setreg(wc, chan->chanpos - 1, 30, echoregs.acim);

			/* Set the digital echo canceller registers */
			wctdm_setreg(wc, chan->chanpos - 1, 45, echoregs.coef1);
			wctdm_setreg(wc, chan->chanpos - 1, 46, echoregs.coef2);
			wctdm_setreg(wc, chan->chanpos - 1, 47, echoregs.coef3);
			wctdm_setreg(wc, chan->chanpos - 1, 48, echoregs.coef4);
			wctdm_setreg(wc, chan->chanpos - 1, 49, echoregs.coef5);
			wctdm_setreg(wc, chan->chanpos - 1, 50, echoregs.coef6);
			wctdm_setreg(wc, chan->chanpos - 1, 51, echoregs.coef7);
			wctdm_setreg(wc, chan->chanpos - 1, 52, echoregs.coef8);

			printk("-- Set echo registers successfully\n");

			break;
		} else {
			return -EINVAL;

		}
		break;
	default:
		return -ENOTTY;
	}
	return 0;

}

static int wctdm_open(struct zt_chan *chan)
{
	struct wctdm *wc = chan->pvt;
	if (!(wc->cardflag & (1 << (chan->chanpos - 1))))
		return -ENODEV;
	if (wc->dead)
		return -ENODEV;
	wc->usecount++;
#ifndef LINUX26
	MOD_INC_USE_COUNT;
#else
	try_module_get(THIS_MODULE);
#endif	
	return 0;
}

static int wctdm_watchdog(struct zt_span *span, int event)
{
	printk("TDM: Restarting DMA\n");
	wctdm_restart_dma(span->pvt);
	return 0;
}

static int wctdm_close(struct zt_chan *chan)
{
	struct wctdm *wc = chan->pvt;
	wc->usecount--;
#ifndef LINUX26
	MOD_DEC_USE_COUNT;
#else
	module_put(THIS_MODULE);
#endif
	if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXS) {
		if (reversepolarity)
			wc->mod[chan->chanpos - 1].fxs.idletxhookstate = 5;
		else
			wc->mod[chan->chanpos - 1].fxs.idletxhookstate = 1;
	}
	/* If we're dead, release us now */
	if (!wc->usecount && wc->dead) 
		wctdm_release(wc);
	return 0;
}

static int wctdm_hooksig(struct zt_chan *chan, zt_txsig_t txsig)
{
	struct wctdm *wc = chan->pvt;
	int reg=0;
	if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO) {
		/* XXX Enable hooksig for FXO XXX */
		switch(txsig) {
		case ZT_TXSIG_START:
		case ZT_TXSIG_OFFHOOK:
			wc->mod[chan->chanpos - 1].fxo.offhook = 1;
			wctdm_setreg(wc, chan->chanpos - 1, 5, 0x9);
			break;
		case ZT_TXSIG_ONHOOK:
			wc->mod[chan->chanpos - 1].fxo.offhook = 0;
			wctdm_setreg(wc, chan->chanpos - 1, 5, 0x8);
			break;
		default:
			printk("wcfxo: Can't set tx state to %d\n", txsig);
		}
	} else {
		switch(txsig) {
		case ZT_TXSIG_ONHOOK:
			switch(chan->sig) {
			case ZT_SIG_EM:
			case ZT_SIG_FXOKS:
			case ZT_SIG_FXOLS:
				wc->mod[chan->chanpos-1].fxs.lasttxhook = wc->mod[chan->chanpos-1].fxs.idletxhookstate;
				break;
			case ZT_SIG_FXOGS:
				wc->mod[chan->chanpos-1].fxs.lasttxhook = 3;
				break;
			}
			break;
		case ZT_TXSIG_OFFHOOK:
			switch(chan->sig) {
			case ZT_SIG_EM:
				wc->mod[chan->chanpos-1].fxs.lasttxhook = 5;
				break;
			default:
				wc->mod[chan->chanpos-1].fxs.lasttxhook = wc->mod[chan->chanpos-1].fxs.idletxhookstate;
				break;
			}
			break;
		case ZT_TXSIG_START:
			wc->mod[chan->chanpos-1].fxs.lasttxhook = 4;
			break;
		case ZT_TXSIG_KEWL:
			wc->mod[chan->chanpos-1].fxs.lasttxhook = 0;
			break;
		default:
			printk("wctdm: Can't set tx state to %d\n", txsig);
		}
		if (debug)
			printk("Setting FXS hook state to %d (%02x)\n", txsig, reg);

#if 1
		wctdm_setreg(wc, chan->chanpos - 1, 64, wc->mod[chan->chanpos-1].fxs.lasttxhook);
#endif
	}
	return 0;
}

static int wctdm_initialize(struct wctdm *wc)
{
	int x;

	/* Zapata stuff */
	sprintf(wc->span.name, "WCTDM/%d", wc->pos);
	sprintf(wc->span.desc, "%s Board %d", wc->variety, wc->pos + 1);
	if (alawoverride) {
		printk("ALAW override parameter detected.  Device will be operating in ALAW\n");
		wc->span.deflaw = ZT_LAW_ALAW;
	} else
		wc->span.deflaw = ZT_LAW_MULAW;
	for (x = 0; x < NUM_CARDS; x++) {
		sprintf(wc->chans[x].name, "WCTDM/%d/%d", wc->pos, x);
		wc->chans[x].sigcap = ZT_SIG_FXOKS | ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_SF | ZT_SIG_EM | ZT_SIG_CLEAR;
		wc->chans[x].sigcap |= ZT_SIG_FXSKS | ZT_SIG_FXSLS | ZT_SIG_SF | ZT_SIG_CLEAR;
		wc->chans[x].chanpos = x+1;
		wc->chans[x].pvt = wc;
	}
	wc->span.chans = wc->chans;
	wc->span.channels = NUM_CARDS;
	wc->span.hooksig = wctdm_hooksig;
	wc->span.open = wctdm_open;
	wc->span.close = wctdm_close;
	wc->span.flags = ZT_FLAG_RBS;
	wc->span.ioctl = wctdm_ioctl;
	wc->span.watchdog = wctdm_watchdog;
	init_waitqueue_head(&wc->span.maintq);

	wc->span.pvt = wc;
	if (zt_register(&wc->span, 0)) {
		printk("Unable to register span with zaptel\n");
		return -1;
	}
	return 0;
}

static void wctdm_post_initialize(struct wctdm *wc)
{
	int x;
	/* Finalize signalling  */
	for (x = 0; x < NUM_CARDS; x++) {
		if (wc->cardflag & (1 << x)) {
			if (wc->modtype[x] == MOD_TYPE_FXO)
				wc->chans[x].sigcap = ZT_SIG_FXSKS | ZT_SIG_FXSLS | ZT_SIG_SF | ZT_SIG_CLEAR;
			else
				wc->chans[x].sigcap = ZT_SIG_FXOKS | ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_SF | ZT_SIG_EM | ZT_SIG_CLEAR;
		}
	}
}

static int wctdm_hardware_init(struct wctdm *wc)
{
	/* Hardware stuff */
	unsigned char ver;
	unsigned char x,y;
	int failed;

	/* Signal Reset */
	outb(0x01, wc->ioaddr + WC_CNTL);

	/* Check Freshmaker chip */
	x=inb(wc->ioaddr + WC_CNTL);
	ver = __wctdm_getcreg(wc, WC_VER);
	failed = 0;
	if (ver != 0x59) {
		printk("Freshmaker version: %02x\n", ver);
		for (x=0;x<255;x++) {
			/* Test registers */
			if (ver >= 0x70) {
				__wctdm_setcreg(wc, WC_CS, x);
				y = __wctdm_getcreg(wc, WC_CS);
			} else {
				__wctdm_setcreg(wc, WC_TEST, x);
				y = __wctdm_getcreg(wc, WC_TEST);
			}
			if (x != y) {
				printk("%02x != %02x\n", x, y);
				failed++;
			}
		}
		if (!failed) {
			printk("Freshmaker passed register test\n");
		} else {
			printk("Freshmaker failed register test\n");
			return -1;
		}
		/* Go to half-duty FSYNC */
		__wctdm_setcreg(wc, WC_SYNC, 0x01);
		y = __wctdm_getcreg(wc, WC_SYNC);
	} else {
		printk("No freshmaker chip\n");
	}

	/* Reset PCI Interface chip and registers (and serial) */
	outb(0x06, wc->ioaddr + WC_CNTL);
	/* Setup our proper outputs for when we switch for our "serial" port */
	wc->ios = BIT_CS | BIT_SCLK | BIT_SDI;

	outb(wc->ios, wc->ioaddr + WC_AUXD);

	/* Set all to outputs except AUX 5, which is an input */
	outb(0xdf, wc->ioaddr + WC_AUXC);

	/* Select alternate function for AUX0 */
	outb(0x4, wc->ioaddr + WC_AUXFUNC);
	
	/* Wait 1/4 of a sec */
	wait_just_a_bit(HZ/4);

	/* Back to normal, with automatic DMA wrap around */
	outb(0x30 | 0x01, wc->ioaddr + WC_CNTL);
	
	/* Make sure serial port and DMA are out of reset */
	outb(inb(wc->ioaddr + WC_CNTL) & 0xf9, wc->ioaddr + WC_CNTL);
	
	/* Configure serial port for MSB->LSB operation */
	outb(0xc1, wc->ioaddr + WC_SERCTL);

	/* Delay FSC by 0 so it's properly aligned */
	outb(0x0, wc->ioaddr + WC_FSCDELAY);

	/* Setup DMA Addresses */
	outl(wc->writedma,                    wc->ioaddr + WC_DMAWS);		/* Write start */
	outl(wc->writedma + ZT_CHUNKSIZE * 4 - 4, wc->ioaddr + WC_DMAWI);		/* Middle (interrupt) */
	outl(wc->writedma + ZT_CHUNKSIZE * 8 - 4, wc->ioaddr + WC_DMAWE);			/* End */
	
	outl(wc->readdma,                    	 wc->ioaddr + WC_DMARS);	/* Read start */
	outl(wc->readdma + ZT_CHUNKSIZE * 4 - 4, 	 wc->ioaddr + WC_DMARI);	/* Middle (interrupt) */
	outl(wc->readdma + ZT_CHUNKSIZE * 8 - 4, wc->ioaddr + WC_DMARE);	/* End */
	
	/* Clear interrupts */
	outb(0xff, wc->ioaddr + WC_INTSTAT);

	/* Wait 1/4 of a second more */
	wait_just_a_bit(HZ/4);

	for (x = 0; x < NUM_CARDS; x++) {
		int sane=0,ret=0,readi=0;
#if 1
		/* Init with Auto Calibration */
		if (!(ret=wctdm_init_proslic(wc, x, 0, 0, sane))) {
			wc->cardflag |= (1 << x);
                        if (debug) {
                                readi = wctdm_getreg(wc,x,LOOP_I_LIMIT);
                                printk("Proslic module %d loop current is %dmA\n",x,
                                ((readi*3)+20));
                        }
			printk("Module %d: Installed -- AUTO FXS/DPO\n",x);
		} else {
			if(ret!=-2) {
				sane=1;
				/* Init with Manual Calibration */
				if (!wctdm_init_proslic(wc, x, 0, 1, sane)) {
					wc->cardflag |= (1 << x);
                                if (debug) {
                                        readi = wctdm_getreg(wc,x,LOOP_I_LIMIT);
                                        printk("Proslic module %d loop current is %dmA\n",x,
                                        ((readi*3)+20));
                                }
					printk("Module %d: Installed -- MANUAL FXS\n",x);
				} else {
					printk("Module %d: FAILED FXS (%s)\n", x, fxshonormode ? fxo_modes[_opermode].name : "FCC");
				} 
			} else if (!(ret = wctdm_init_voicedaa(wc, x, 0, 0, sane))) {
				wc->cardflag |= (1 << x);
				printk("Module %d: Installed -- AUTO FXO (%s mode)\n",x, fxo_modes[_opermode].name);
			} else
				printk("Module %d: Not installed\n", x);
		}
#endif
	}

	/* Return error if nothing initialized okay. */
	if (!wc->cardflag && !timingonly)
		return -1;
	__wctdm_setcreg(wc, WC_SYNC, (wc->cardflag << 1) | 0x1);
	return 0;
}

static void wctdm_enable_interrupts(struct wctdm *wc)
{
	/* Enable interrupts (we care about all of them) */
	outb(0x3f, wc->ioaddr + WC_MASK0);
	/* No external interrupts */
	outb(0x00, wc->ioaddr + WC_MASK1);
}

static void wctdm_restart_dma(struct wctdm *wc)
{
	/* Reset Master and TDM */
	outb(0x01, wc->ioaddr + WC_CNTL);
	outb(0x01, wc->ioaddr + WC_OPER);
}

static void wctdm_start_dma(struct wctdm *wc)
{
	/* Reset Master and TDM */
	outb(0x0f, wc->ioaddr + WC_CNTL);
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(1);
	outb(0x01, wc->ioaddr + WC_CNTL);
	outb(0x01, wc->ioaddr + WC_OPER);
}

static void wctdm_stop_dma(struct wctdm *wc)
{
	outb(0x00, wc->ioaddr + WC_OPER);
}

static void wctdm_reset_tdm(struct wctdm *wc)
{
	/* Reset TDM */
	outb(0x0f, wc->ioaddr + WC_CNTL);
}

static void wctdm_disable_interrupts(struct wctdm *wc)	
{
	outb(0x00, wc->ioaddr + WC_MASK0);
	outb(0x00, wc->ioaddr + WC_MASK1);
}

static int __devinit wctdm_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int res;
	struct wctdm *wc;
	struct wctdm_desc *d = (struct wctdm_desc *)ent->driver_data;
	int x;
	int y;
	static int initd_ifaces=0;
	
	if(initd_ifaces){
		memset((void *)ifaces,0,(sizeof(struct wctdm *))*WC_MAX_IFACES);
		initd_ifaces=1;
	}
	for (x=0;x<WC_MAX_IFACES;x++)
		if (!ifaces[x]) break;
	if (x >= WC_MAX_IFACES) {
		printk("Too many interfaces\n");
		return -EIO;
	}
	
	if (pci_enable_device(pdev)) {
		res = -EIO;
	} else {
		wc = kmalloc(sizeof(struct wctdm), GFP_KERNEL);
		if (wc) {
			int cardcount = 0;

			ifaces[x] = wc;
			memset(wc, 0, sizeof(struct wctdm));
			spin_lock_init(&wc->lock);
			wc->curcard = -1;
			wc->ioaddr = pci_resource_start(pdev, 0);
			wc->dev = pdev;
			wc->pos = x;
			wc->variety = d->name;
			for (y=0;y<NUM_CARDS;y++)
				wc->flags[y] = d->flags;
			/* Keep track of whether we need to free the region */
			if (request_region(wc->ioaddr, 0xff, "wctdm")) 
				wc->freeregion = 1;

			/* Allocate enough memory for two zt chunks, receive and transmit.  Each sample uses
			   32 bits.  Allocate an extra set just for control too */
			wc->writechunk = pci_alloc_consistent(pdev, ZT_MAX_CHUNKSIZE * 2 * 2 * 2 * 4, &wc->writedma);
			if (!wc->writechunk) {
				printk("wctdm: Unable to allocate DMA-able memory\n");
				if (wc->freeregion)
					release_region(wc->ioaddr, 0xff);
				return -ENOMEM;
			}

			wc->readchunk = wc->writechunk + ZT_MAX_CHUNKSIZE * 2;	/* in doublewords */
			wc->readdma = wc->writedma + ZT_MAX_CHUNKSIZE * 8;		/* in bytes */

			if (wctdm_initialize(wc)) {
				printk("wctdm: Unable to intialize FXS\n");
				/* Set Reset Low */
				x=inb(wc->ioaddr + WC_CNTL);
				outb((~0x1)&x, wc->ioaddr + WC_CNTL);
				/* Free Resources */
				free_irq(pdev->irq, wc);
				if (wc->freeregion)
					release_region(wc->ioaddr, 0xff);
				pci_free_consistent(pdev, ZT_MAX_CHUNKSIZE * 2 * 2 * 2 * 4, (void *)wc->writechunk, wc->writedma);
				kfree(wc);
				return -EIO;
			}

			/* Enable bus mastering */
			pci_set_master(pdev);

			/* Keep track of which device we are */
			pci_set_drvdata(pdev, wc);

			if (request_irq(pdev->irq, wctdm_interrupt, SA_SHIRQ, "wctdm", wc)) {
				printk("wctdm: Unable to request IRQ %d\n", pdev->irq);
				if (wc->freeregion)
					release_region(wc->ioaddr, 0xff);
				pci_free_consistent(pdev, ZT_MAX_CHUNKSIZE * 2 * 2 * 2 * 4, (void *)wc->writechunk, wc->writedma);
				pci_set_drvdata(pdev, NULL);
				kfree(wc);
				return -EIO;
			}


			if (wctdm_hardware_init(wc)) {
				unsigned char x;

				/* Set Reset Low */
				x=inb(wc->ioaddr + WC_CNTL);
				outb((~0x1)&x, wc->ioaddr + WC_CNTL);
				/* Free Resources */
				free_irq(pdev->irq, wc);
				if (wc->freeregion)
					release_region(wc->ioaddr, 0xff);
				pci_free_consistent(pdev, ZT_MAX_CHUNKSIZE * 2 * 2 * 2 * 4, (void *)wc->writechunk, wc->writedma);
				pci_set_drvdata(pdev, NULL);
				zt_unregister(&wc->span);
				kfree(wc);
				return -EIO;

			}

			wctdm_post_initialize(wc);

			/* Enable interrupts */
			wctdm_enable_interrupts(wc);
			/* Initialize Write/Buffers to all blank data */
			memset((void *)wc->writechunk,0,ZT_MAX_CHUNKSIZE * 2 * 2 * 4);

			/* Start DMA */
			wctdm_start_dma(wc);

			for (x = 0; x < NUM_CARDS; x++) {
				if (wc->cardflag & (1 << x))
					cardcount++;
			}

			printk("Found a Wildcard TDM: %s (%d modules)\n", wc->variety, cardcount);
			res = 0;
		} else
			res = -ENOMEM;
	}
	return res;
}

static void wctdm_release(struct wctdm *wc)
{
	zt_unregister(&wc->span);
	if (wc->freeregion)
		release_region(wc->ioaddr, 0xff);
	kfree(wc);
	printk("Freed a Wildcard\n");
}

static void __devexit wctdm_remove_one(struct pci_dev *pdev)
{
	struct wctdm *wc = pci_get_drvdata(pdev);
	if (wc) {

		/* Stop any DMA */
		wctdm_stop_dma(wc);
		wctdm_reset_tdm(wc);

		/* In case hardware is still there */
		wctdm_disable_interrupts(wc);
		
		/* Immediately free resources */
		pci_free_consistent(pdev, ZT_MAX_CHUNKSIZE * 2 * 2 * 2 * 4, (void *)wc->writechunk, wc->writedma);
		free_irq(pdev->irq, wc);

		/* Reset PCI chip and registers */
		outb(0x0e, wc->ioaddr + WC_CNTL);

		/* Release span, possibly delayed */
		if (!wc->usecount)
			wctdm_release(wc);
		else
			wc->dead = 1;
	}
}

static struct pci_device_id wctdm_pci_tbl[] = {
	{ 0xe159, 0x0001, 0xa159, PCI_ANY_ID, 0, 0, (unsigned long) &wctdm },
	{ 0xe159, 0x0001, 0xe159, PCI_ANY_ID, 0, 0, (unsigned long) &wctdm },
	{ 0xe159, 0x0001, 0xb100, PCI_ANY_ID, 0, 0, (unsigned long) &wctdme },
	{ 0xe159, 0x0001, 0xb1d9, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmi },
	{ 0xe159, 0x0001, 0xb118, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmi },
	{ 0xe159, 0x0001, 0xb119, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmi },
	{ 0xe159, 0x0001, 0xa9fd, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
	{ 0xe159, 0x0001, 0xa8fd, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
	{ 0xe159, 0x0001, 0xa800, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
	{ 0xe159, 0x0001, 0xa801, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
	{ 0xe159, 0x0001, 0xa908, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
	{ 0xe159, 0x0001, 0xa901, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
#ifdef TDM_REVH_MATCHALL
	{ 0xe159, 0x0001, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &wctdmh },
#endif
	{ 0 }
};

MODULE_DEVICE_TABLE(pci, wctdm_pci_tbl);

static struct pci_driver wctdm_driver = {
	name: 	"wctdm",
	probe: 	wctdm_init_one,
#ifdef LINUX26
	remove:	__devexit_p(wctdm_remove_one),
#else
	remove:	wctdm_remove_one,
#endif
	suspend: NULL,
	resume:	NULL,
	id_table: wctdm_pci_tbl,
};

static int __init wctdm_init(void)
{
	int res;
	int x;
	for (x=0;x<(sizeof(fxo_modes) / sizeof(fxo_modes[0])); x++) {
		if (!strcmp(fxo_modes[x].name, opermode))
			break;
	}
	if (x < sizeof(fxo_modes) / sizeof(fxo_modes[0])) {
		_opermode = x;
	} else {
		printk("Invalid/unknown operating mode '%s' specified.  Please choose one of:\n", opermode);
		for (x=0;x<sizeof(fxo_modes) / sizeof(fxo_modes[0]); x++)
			printk("  %s\n", fxo_modes[x].name);
		printk("Note this option is CASE SENSITIVE!\n");
		return -ENODEV;
	}
	if (!strcmp(fxo_modes[_opermode].name, "AUSTRALIA")) {
		boostringer=1;
		fxshonormode=1;
	}
	

	res = zap_pci_module(&wctdm_driver);
	if (res)
		return -ENODEV;
	return 0;
}

static void __exit wctdm_cleanup(void)
{
	pci_unregister_driver(&wctdm_driver);
}

#ifdef LINUX26
module_param(debug, int, 0600);
module_param(loopcurrent, int, 0600);
module_param(reversepolarity, int, 0600);
module_param(robust, int, 0600);
module_param(_opermode, int, 0600);
module_param(opermode, charp, 0600);
module_param(timingonly, int, 0600);
module_param(lowpower, int, 0600);
module_param(boostringer, int, 0600);
module_param(fastringer, int, 0600);
module_param(fxshonormode, int, 0600);
module_param(battdebounce, int, 0600);
module_param(battthresh, int, 0600);
module_param(alawoverride, int, 0600);
module_param(fastpickup, int, 0600);
module_param(fxotxgain, int, 0600);
module_param(fxorxgain, int, 0600);
module_param(fxstxgain, int, 0600);
module_param(fxsrxgain, int, 0600);
#else
MODULE_PARM(debug, "i");
MODULE_PARM(loopcurrent, "i");
MODULE_PARM(reversepolarity, "i");
MODULE_PARM(robust, "i");
MODULE_PARM(_opermode, "i");
MODULE_PARM(opermode, "s");
MODULE_PARM(timingonly, "i");
MODULE_PARM(lowpower, "i");
MODULE_PARM(boostringer, "i");
MODULE_PARM(fastringer, "i");
MODULE_PARM(fxshonormode, "i");
MODULE_PARM(battdebounce, "i");
MODULE_PARM(battthresh, "i");
MODULE_PARM(alawoverride, "i");
MODULE_PARM(fastpickup, "i");
MODULE_PARM(fxotxgain, "i");
MODULE_PARM(fxorxgain, "i");
MODULE_PARM(fxstxgain, "i");
MODULE_PARM(fxsrxgain, "i");
#endif
MODULE_DESCRIPTION("Wildcard TDM400P Zaptel Driver");
MODULE_AUTHOR("Mark Spencer <markster@digium.com>");
#if defined(MODULE_ALIAS)
MODULE_ALIAS("wcfxs");
#endif
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

module_init(wctdm_init);
module_exit(wctdm_cleanup);
