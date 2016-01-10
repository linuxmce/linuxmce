/*
 * Wilcard TDM2400P TDM FXS/FXO Interface Driver for Zapata Telephony interface
 *
 * Written by Mark Spencer <markster@digium.com>
 * Further modified to add support for the  TDM800P by Matthew Fredrickson <creslin@digium.com>
 *
 * Copyright (C) 2005,2006, Digium, Inc.
 * All rights reserved.
 *
 * Sections for QRV cards written by Jim Dixon <jim@lambdatel.com>
 * Copyright (C) 2006, Jim Dixon and QRV Communications
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
/* For QRV DRI cards, gain is signed short, expressed in hundredths of
db (in reference to 1v Peak @ 1000Hz) , as follows:

Rx Gain: -11.99 to 15.52 db
Tx Gain - No Pre-Emphasis: -35.99 to 12.00 db
Tx Gain - W/Pre-Emphasis: -23.99 to 0.00 db
*/

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include "proslic.h"
#include "wctdm.h"

/* Comment to disable VPM support */
#define VPM_SUPPORT

#ifdef VPM_SUPPORT

/* Define to get more attention-grabbing but slightly more CPU using echocan status */
#define FANCY_ECHOCAN

#endif

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
{33,20,"PWR_ALARM_Q3Q4", 0x4C00 /* 0x2600 */},
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

#ifdef FANCY_ECHOCAN
static char ectab[] = {
0, 0, 0, 1, 2, 3, 4, 6, 8, 9, 11, 13, 16, 18, 20, 22, 24, 25, 27, 28, 29, 30, 31, 31, 32, 
32, 32, 32, 32, 32, 32, 32, 32, 32, 32 ,32 ,32, 32,
32, 32, 32, 32, 32, 32, 32, 32, 32, 32 ,32 ,32, 32,
32, 32, 32, 32, 32, 32, 32, 32, 32, 32 ,32 ,32, 32,
31, 31, 30, 29, 28, 27, 25, 23, 22, 20, 18, 16, 13, 11, 9, 8, 6, 4, 3, 2, 1, 0, 0, 
};
static int ectrans[4] = { 0, 1, 3, 2 };
#define EC_SIZE (sizeof(ectab))
#define EC_SIZE_Q (sizeof(ectab) / 4)
#endif

/* Undefine to enable Power alarm / Transistor debug -- note: do not
   enable for normal operation! */
/* #define PAQ_DEBUG */

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

#define FLAG_EMPTY	0
#define FLAG_WRITE	1
#define FLAG_READ	2

#define RING_DEBOUNCE	128		/* Ringer Debounce (in ms) */
#define DEFAULT_BATT_DEBOUNCE	64		/* Battery debounce (in ms) */
#define POLARITY_DEBOUNCE 64           /* Polarity debounce (in ms) */
#define DEFAULT_BATT_THRESH	3		/* Anything under this is "no battery" */

#define OHT_TIMER		6000	/* How long after RING to retain OHT */

#define FLAG_3215	(1 << 0)

#define NUM_CARDS 24
#define NUM_EC	  4
#define NUM_SLOTS 6
#define MAX_TDM_CHAN 31

#define EFRAME_SIZE	108
#define ERING_SIZE 16		/* Maximum ring size */
#define EFRAME_GAP 20
#define SFRAME_SIZE ((EFRAME_SIZE * ZT_CHUNKSIZE) + (EFRAME_GAP * (ZT_CHUNKSIZE - 1)))

#define MAX_ALARMS 10

#define MOD_TYPE_NONE		0
#define MOD_TYPE_FXS		1
#define MOD_TYPE_FXO		2
#define MOD_TYPE_FXSINIT	3	
#define MOD_TYPE_VPM		4
#define MOD_TYPE_QRV		5

#define MINPEGTIME	10 * 8		/* 30 ms peak to peak gets us no more than 100 Hz */
#define PEGTIME		50 * 8		/* 50ms peak to peak gets us rings of 10 Hz or more */
#define PEGCOUNT	5		/* 5 cycles of pegging means RING */

#define SDI_CLK		(0x00010000)
#define SDI_DOUT	(0x00020000)
#define SDI_DREAD	(0x00040000)
#define SDI_DIN		(0x00080000)

#define NUM_CAL_REGS 12

#define PCI_WINDOW_SIZE ((2 * 2 * 2 * SFRAME_SIZE) + (2 * ERING_SIZE * 4))

#define USER_COMMANDS 8
#define ISR_COMMANDS  2
#define	QRV_DEBOUNCETIME 20

#define MAX_COMMANDS (USER_COMMANDS + ISR_COMMANDS)

#define __CMD_RD   (1 << 20)		/* Read Operation */
#define __CMD_WR   (1 << 21)		/* Write Operation */
#define __CMD_FIN  (1 << 22)		/* Has finished receive */
#define __CMD_TX   (1 << 23)		/* Has been transmitted */

#define CMD_WR(a,b) (((a) << 8) | (b) | __CMD_WR)
#define CMD_RD(a) (((a) << 8) | __CMD_RD)

#define CMD_BYTE(card,bit,altcs) (((((card) & 0x3) * 3 + (bit)) * 7) \
			+ ((card) >> 2) + (altcs) + ((altcs) ? -21 : 0))

struct calregs {
	unsigned char vals[NUM_CAL_REGS];
};

struct cmdq {
	unsigned int cmds[MAX_COMMANDS];
	unsigned char isrshadow[ISR_COMMANDS];
};

struct wctdm {
	struct pci_dev *dev;
	char *variety;
	struct zt_span span;
	unsigned char ios;
	unsigned int sdi;
	int usecount;
	unsigned int intcount;
	unsigned int rxints;
	unsigned int txints;
	unsigned int intmask;
	unsigned char txident;
	unsigned char rxident;
	int dead;
	int pos;
	int flags[NUM_CARDS];
	int freeregion;
	int alt;
	int rdbl;
	int tdbl;
	int curcard;
	unsigned char ctlreg;
	int cards;
	int cardflag;		/* Bit-map of present cards */
	int altcs[NUM_CARDS + NUM_EC];
	char qrvhook[NUM_CARDS];
	unsigned short qrvdebtime[NUM_CARDS];
	int radmode[NUM_CARDS];
#define	RADMODE_INVERTCOR 1
#define	RADMODE_IGNORECOR 2
#define	RADMODE_EXTTONE 4
#define	RADMODE_EXTINVERT 8
#define	RADMODE_IGNORECT 16
#define	RADMODE_PREEMP	32
#define	RADMODE_DEEMP 64
	unsigned short debouncetime[NUM_CARDS];
	signed short rxgain[NUM_CARDS];
	signed short txgain[NUM_CARDS];
	spinlock_t reglock;
	wait_queue_head_t regq;
	/* FXO Stuff */
	union {
		struct {
			int wasringing;
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
	} mods[NUM_CARDS];
	struct cmdq cmdq[NUM_CARDS + NUM_EC];
	/* Receive hook state and debouncing */
	int modtype[NUM_CARDS + NUM_EC];
	/* Set hook */
	int sethook[NUM_CARDS + NUM_EC];
	int dacssrc[NUM_CARDS];
	int type;

#ifdef VPM_SUPPORT
	int vpm;
	unsigned int dtmfactive;
	unsigned int dtmfmask;
	unsigned int dtmfmutemask;
	short dtmfenergy[NUM_CARDS];
	short dtmfdigit[NUM_CARDS];
#ifdef FANCY_ECHOCAN
	int echocanpos;
	int blinktimer;
#endif	
#endif
	unsigned long iobase;
	dma_addr_t 	readdma;
	dma_addr_t	writedma;
	dma_addr_t  descripdma;
	volatile unsigned int *writechunk;					/* Double-word aligned write memory */
	volatile unsigned int *readchunk;					/* Double-word aligned read memory */
	volatile unsigned int *descripchunk;					/* Descriptors */
	struct zt_chan chans[NUM_CARDS];
};


struct wctdm_desc {
	char *name;
	int flags;
	int ports;
};

static struct wctdm_desc wctdm2400 = { "Wildcard TDM2400P", 0, 24 };
static struct wctdm_desc wctdm800 = { "Wildcard TDM800P", 0, 8 };
static int acim2tiss[16] = { 0x0, 0x1, 0x4, 0x5, 0x7, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0, 0x2, 0x0, 0x3 };

static struct wctdm *ifaces[WC_MAX_IFACES];

static void wctdm_release(struct wctdm *wc);

static int battdebounce = DEFAULT_BATT_DEBOUNCE;
static int battthresh = DEFAULT_BATT_THRESH;
static int debug = 0;
static int robust = 0;
static int lowpower = 0;
static int boostringer = 0;
static int fastringer = 0;
static int _opermode = 0;
static char *opermode = "FCC";
static int fxshonormode = 0;
static int alawoverride = 0;
static int fxo_addrs[4] = { 0x00, 0x08, 0x04, 0x0c };
static int fxotxgain = 0;
static int fxorxgain = 0;
static int fxstxgain = 0;
static int fxsrxgain = 0;
static int nativebridge = 1;
#ifdef VPM_SUPPORT
static int vpmsupport = 1;
static int vpmdtmfsupport = 0;
#define VPM_DEFAULT_DTMFTHRESHOLD 1250
static int dtmfthreshold = VPM_DEFAULT_DTMFTHRESHOLD;
#endif

static int wctdm_init_proslic(struct wctdm *wc, int card, int fast , int manual, int sane);

/* sleep in user space until woken up. Equivilant of tsleep() in BSD */
static int schluffen(wait_queue_head_t *q)
{
	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(q, &wait);
	current->state = TASK_INTERRUPTIBLE;
	if (!signal_pending(current)) schedule();
	current->state = TASK_RUNNING;
	remove_wait_queue(q, &wait);
	if (signal_pending(current)) return -ERESTARTSYS;
	return(0);
}

static inline int empty_slot(struct wctdm *wc, int card)
{
	int x;
	for (x=0;x<USER_COMMANDS;x++) {
		if (!wc->cmdq[card].cmds[x])
			return x;
	}
	return -1;
}

static inline void cmd_dequeue(struct wctdm *wc, volatile unsigned char *writechunk, int card, int pos)
{
	unsigned long flags;
	unsigned int curcmd=0;
	int x;
	int subaddr = card & 0x3;
#ifdef FANCY_ECHOCAN
	int ecval;
	ecval = wc->echocanpos;
	ecval += EC_SIZE_Q * ectrans[(card & 0x3)];
	ecval = ecval % EC_SIZE;
#endif

	/* if a QRV card, map it to its first channel */  
	if ((wc->modtype[card] ==  MOD_TYPE_QRV) && (card & 3))
	{
		return;
	}
	if (wc->altcs[card])
		subaddr = 0;

	/* Skip audio */
	writechunk += 24;
	spin_lock_irqsave(&wc->reglock, flags);
	/* Search for something waiting to transmit */
	if (pos) {
		for (x=0;x<MAX_COMMANDS;x++) {
			if ((wc->cmdq[card].cmds[x] & (__CMD_RD | __CMD_WR)) && 
			   !(wc->cmdq[card].cmds[x] & (__CMD_TX | __CMD_FIN))) {
			   	curcmd = wc->cmdq[card].cmds[x];
#if 0
				printk("Transmitting command '%08x' in slot %d\n", wc->cmdq[card].cmds[x], wc->txident);
#endif			
				wc->cmdq[card].cmds[x] |= (wc->txident << 24) | __CMD_TX;
				break;
			}
		}
	}
	if (!curcmd) {
		/* If nothing else, use filler */
		if (wc->modtype[card] == MOD_TYPE_FXS)
			curcmd = CMD_RD(64);
		else if (wc->modtype[card] == MOD_TYPE_FXO)
			curcmd = CMD_RD(12);
		else if (wc->modtype[card] == MOD_TYPE_QRV)
			curcmd = CMD_RD(3);
		else if (wc->modtype[card] == MOD_TYPE_VPM) {
#ifdef FANCY_ECHOCAN
			if (wc->blinktimer >= 0xf) {
				curcmd = CMD_WR(0x1ab, 0x0f);
			} else if (wc->blinktimer == (ectab[ecval] >> 1)) {
				curcmd = CMD_WR(0x1ab, 0x00);
			} else
#endif
			curcmd = CMD_RD(0x1a0);
		}
	}
	if (wc->modtype[card] == MOD_TYPE_FXS) {
		writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = (1 << (subaddr));
		if (curcmd & __CMD_WR)
			writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = (curcmd >> 8) & 0x7f;
		else
			writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = 0x80 | ((curcmd >> 8) & 0x7f);
		writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = curcmd & 0xff;
	} else if (wc->modtype[card] == MOD_TYPE_FXO) {
		if (curcmd & __CMD_WR)
			writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = 0x20 | fxo_addrs[subaddr];
		else
			writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = 0x60 | fxo_addrs[subaddr];
		writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = (curcmd >> 8) & 0xff;
		writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = curcmd & 0xff;
	} else if (wc->modtype[card] == MOD_TYPE_FXSINIT) {
		/* Special case, we initialize the FXS's into the three-byte command mode then
		   switch to the regular mode.  To send it into thee byte mode, treat the path as
		   6 two-byte commands and in the last one we initialize register 0 to 0x80. All modules
		   read this as the command to switch to daisy chain mode and we're done.  */
		writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = 0x00;
		writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = 0x00;
		if ((card & 0x1) == 0x1) 
			writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = 0x80;
		else
			writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = 0x00;
#ifdef VPM_SUPPORT
	} else if (wc->modtype[card] == MOD_TYPE_VPM) {
		if (curcmd & __CMD_WR)
			writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = ((card & 0x3) << 4) | 0xc | ((curcmd >> 16) & 0x1);
		else
			writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = ((card & 0x3) << 4) | 0xa | ((curcmd >> 16) & 0x1);
		writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = (curcmd >> 8) & 0xff;
		writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = curcmd & 0xff;
#endif
	} else if (wc->modtype[card] == MOD_TYPE_QRV) {

		writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = 0x00;
		if (!curcmd)
		{
			writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = 0x00;
			writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = 0x00;
		}
		else
		{
			if (curcmd & __CMD_WR)
				writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = 0x40 | ((curcmd >> 8) & 0x3f);
			else
				writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = 0xc0 | ((curcmd >> 8) & 0x3f);
			writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = curcmd & 0xff;
		}
	} else if (wc->modtype[card] == MOD_TYPE_NONE) {
		writechunk[CMD_BYTE(card, 0, wc->altcs[card])] = 0x00;
		writechunk[CMD_BYTE(card, 1, wc->altcs[card])] = 0x00;
		writechunk[CMD_BYTE(card, 2, wc->altcs[card])] = 0x00;
	}
#if 0
	/* XXX */
	if (cmddesc < 40)
		printk("Pass %d, card = %d (modtype=%d), pos = %d, CMD_BYTES = %d,%d,%d, (%02x,%02x,%02x) curcmd = %08x\n", cmddesc, card, wc->modtype[card], pos, CMD_BYTE(card, 0), CMD_BYTE(card, 1), CMD_BYTE(card, 2), writechunk[CMD_BYTE(card, 0)], writechunk[CMD_BYTE(card, 1)], writechunk[CMD_BYTE(card, 2)], curcmd);
#endif
	spin_unlock_irqrestore(&wc->reglock, flags);
#if 0
	/* XXX */
	cmddesc++;
#endif	
}

static inline void cmd_decifer(struct wctdm *wc, volatile unsigned char *readchunk, int card)
{
	unsigned long flags;
	unsigned char ident;
	int x;

	/* if a QRV card, map it to its first channel */  
	if ((wc->modtype[card] ==  MOD_TYPE_QRV) && (card & 3))
	{
		return;
	}
	/* Skip audio */
	readchunk += 24;
	spin_lock_irqsave(&wc->reglock, flags);
	/* Search for any pending results */
	for (x=0;x<MAX_COMMANDS;x++) {
		if ((wc->cmdq[card].cmds[x] & (__CMD_RD | __CMD_WR)) && 
		    (wc->cmdq[card].cmds[x] & (__CMD_TX)) && 
		   !(wc->cmdq[card].cmds[x] & (__CMD_FIN))) {
		   	ident = (wc->cmdq[card].cmds[x] >> 24) & 0xff;
		   	if (ident == wc->rxident) {
				/* Store result */
				wc->cmdq[card].cmds[x] |= readchunk[CMD_BYTE(card, 2, wc->altcs[card])];
				wc->cmdq[card].cmds[x] |= __CMD_FIN;
				if (wc->cmdq[card].cmds[x] & __CMD_WR) {
					/* Go ahead and clear out writes since they need no acknowledgement */
					wc->cmdq[card].cmds[x] = 0x00000000;
				} else if (x >= USER_COMMANDS) {
					/* Clear out ISR reads */
					wc->cmdq[card].isrshadow[x - USER_COMMANDS] = wc->cmdq[card].cmds[x] & 0xff;
					wc->cmdq[card].cmds[x] = 0x00000000;
				}
				break;
			}
		}
	}
#if 0
	/* XXX */
	if (!pos && (cmddesc < 256))
		printk("Card %d: Command '%08x' => %02x\n",card,  wc->cmdq[card].lasttx[pos], wc->cmdq[card].lastrd[pos]);
#endif
	spin_unlock_irqrestore(&wc->reglock, flags);
}

static inline void cmd_checkisr(struct wctdm *wc, int card)
{
	if (!wc->cmdq[card].cmds[USER_COMMANDS + 0]) {
		if (wc->sethook[card]) {
			wc->cmdq[card].cmds[USER_COMMANDS + 0] = wc->sethook[card];
			wc->sethook[card] = 0;
		} else if (wc->modtype[card] == MOD_TYPE_FXS) {
			wc->cmdq[card].cmds[USER_COMMANDS + 0] = CMD_RD(68);	/* Hook state */
		} else if (wc->modtype[card] == MOD_TYPE_FXO) {
			wc->cmdq[card].cmds[USER_COMMANDS + 0] = CMD_RD(5);	/* Hook/Ring state */
		} else if (wc->modtype[card] == MOD_TYPE_QRV) {
			wc->cmdq[card & 0xfc].cmds[USER_COMMANDS + 0] = CMD_RD(3);	/* COR/CTCSS state */
#ifdef VPM_SUPPORT
		} else if (wc->modtype[card] == MOD_TYPE_VPM) {
			wc->cmdq[card].cmds[USER_COMMANDS + 0] = CMD_RD(0xb9); /* DTMF interrupt */
#endif
		}
	}
	if (!wc->cmdq[card].cmds[USER_COMMANDS + 1]) {
		if (wc->modtype[card] == MOD_TYPE_FXS) {
#ifdef PAQ_DEBUG
			wc->cmdq[card].cmds[USER_COMMANDS + 1] = CMD_RD(19);	/* Transistor interrupts */
#else
			wc->cmdq[card].cmds[USER_COMMANDS + 1] = CMD_RD(64);	/* Battery mode */
#endif
		} else if (wc->modtype[card] == MOD_TYPE_FXO) {
			wc->cmdq[card].cmds[USER_COMMANDS + 1] = CMD_RD(29);	/* Battery */
		} else if (wc->modtype[card] == MOD_TYPE_QRV) {
			wc->cmdq[card & 0xfc].cmds[USER_COMMANDS + 1] = CMD_RD(3);	/* Battery */
#ifdef VPM_SUPPORT
		} else if (wc->modtype[card] == MOD_TYPE_VPM) {
			wc->cmdq[card].cmds[USER_COMMANDS + 1] = CMD_RD(0xbd); /* DTMF interrupt */
#endif
		}
	}
}

static inline void wctdm_transmitprep(struct wctdm *wc, int dbl)
{
	volatile unsigned char *writechunk;
	int x,y;

	dbl = dbl % 2;

	writechunk = (volatile unsigned char *)(wc->writechunk);
	if (dbl) 
		/* Write is at interrupt address.  Start writing from normal offset */
		writechunk += SFRAME_SIZE;

	/* Calculate Transmission */
	zt_transmit(&wc->span);

	for (x=0;x<ZT_CHUNKSIZE;x++) {
		/* Send a sample, as a 32-bit word */
		for (y=0;y < wc->cards;y++) {
			if (!x)
				cmd_checkisr(wc, y);

			if (y < wc->type)
				writechunk[y] = wc->chans[y].writechunk[x];
			cmd_dequeue(wc, writechunk, y, x);
		}
#ifdef VPM_SUPPORT
		if (wc->vpm) {
			if (!x)
				wc->blinktimer++;
			for (y=24;y<28;y++) {
				if (!x)
					cmd_checkisr(wc, y);
				cmd_dequeue(wc, writechunk, y, x);
			}
#ifdef FANCY_ECHOCAN
			if (wc->blinktimer >= 0xf) {
				wc->blinktimer = -1;
				wc->echocanpos++;
			}
#endif			
		}
#endif		
#if 0
		if (cmddesc < 1024)
			printk("TC Result: %02x\n", wc->txident);
#endif			
		if (x < ZT_CHUNKSIZE - 1) {
			writechunk[EFRAME_SIZE] = wc->ctlreg;
			writechunk[EFRAME_SIZE + 1] = wc->txident++;
		}
		writechunk += (EFRAME_SIZE + EFRAME_GAP);
	}
}

static inline void __wctdm_setctl(struct wctdm *wc, unsigned int addr, unsigned int val)
{
	outl(val, wc->iobase + addr);
}

static inline unsigned int __wctdm_getctl(struct wctdm *wc, unsigned int addr)
{
	return inl(wc->iobase + addr);
}

static inline void wctdm_setctl(struct wctdm *wc, unsigned int addr, unsigned int val)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->reglock, flags);
	__wctdm_setctl(wc, addr, val);
	spin_unlock_irqrestore(&wc->reglock, flags);
}

static inline int wctdm_setreg_full(struct wctdm *wc, int card, int addr, int val, int inisr)
{
	unsigned long flags;
	int hit=0;
	int ret;

	/* if a QRV card, use only its first channel */  
	if (wc->modtype[card] ==  MOD_TYPE_QRV)
	{
		if (card & 3) return(0);
	}
	do {
		spin_lock_irqsave(&wc->reglock, flags);
		hit = empty_slot(wc, card);
		if (hit > -1) {
			wc->cmdq[card].cmds[hit] = CMD_WR(addr, val);
		}
		spin_unlock_irqrestore(&wc->reglock, flags);
		if (inisr)
			break;
		if (hit < 0) {
			if ((ret = schluffen(&wc->regq)))
				return ret;
		}
	} while (hit < 0);
	return (hit > -1) ? 0 : -1;
}

static inline int wctdm_setreg_intr(struct wctdm *wc, int card, int addr, int val)
{
	return wctdm_setreg_full(wc, card, addr, val, 1);
}
static inline int wctdm_setreg(struct wctdm *wc, int card, int addr, int val)
{
	return wctdm_setreg_full(wc, card, addr, val, 0);
}

static inline int wctdm_getreg(struct wctdm *wc, int card, int addr)
{
	unsigned long flags;
	int hit;
	int ret=0;

	/* if a QRV card, use only its first channel */  
	if (wc->modtype[card] ==  MOD_TYPE_QRV)
	{
		if (card & 3) return(0);
	}
	do {
		spin_lock_irqsave(&wc->reglock, flags);
		hit = empty_slot(wc, card);
		if (hit > -1) {
			wc->cmdq[card].cmds[hit] = CMD_RD(addr);
		}
		spin_unlock_irqrestore(&wc->reglock, flags);
		if (hit < 0) {
			if ((ret = schluffen(&wc->regq)))
				return ret;
		}
	} while (hit < 0);
	do {
		spin_lock_irqsave(&wc->reglock, flags);
		if (wc->cmdq[card].cmds[hit] & __CMD_FIN) {
			ret = wc->cmdq[card].cmds[hit] & 0xff;
			wc->cmdq[card].cmds[hit] = 0x00000000;
			hit = -1;
		}
		spin_unlock_irqrestore(&wc->reglock, flags);
		if (hit > -1) {
			if ((ret = schluffen(&wc->regq)))
				return ret;
		}
	} while (hit > -1);
	return ret;
}

static inline unsigned int wctdm_getctl(struct wctdm *wc, unsigned int addr)
{
	unsigned long flags;
	unsigned int val;
	spin_lock_irqsave(&wc->reglock, flags);
	val = __wctdm_getctl(wc, addr);
	spin_unlock_irqrestore(&wc->reglock, flags);
	return val;
}

static inline int __wctdm_sdi_clk(struct wctdm *wc)
{
	unsigned int ret;
	wc->sdi &= ~SDI_CLK;
	__wctdm_setctl(wc, 0x0048, wc->sdi);
	ret = __wctdm_getctl(wc, 0x0048);
	wc->sdi |= SDI_CLK;
	__wctdm_setctl(wc, 0x0048, wc->sdi);
	return ret & SDI_DIN;
}

static inline void __wctdm_sdi_sendbits(struct wctdm *wc, unsigned int bits, int count)
{
	wc->sdi &= ~SDI_DREAD;
	__wctdm_setctl(wc, 0x0048, wc->sdi);
	while(count--) {
		if (bits & (1 << count))
			wc->sdi |= SDI_DOUT;
		else
			wc->sdi &= ~SDI_DOUT;
		__wctdm_sdi_clk(wc);
	}
}

static inline unsigned int __wctdm_sdi_recvbits(struct wctdm *wc, int count)
{
	unsigned int bits=0;
	wc->sdi |= SDI_DREAD;
	__wctdm_setctl(wc, 0x0048, wc->sdi);
	while(count--) {
		bits <<= 1;
		if (__wctdm_sdi_clk(wc))
			bits |= 1;
		else
			bits &= ~1;
	}
	return bits;
}

static inline void __wctdm_setsdi(struct wctdm *wc, unsigned char addr, unsigned short value)
{
	unsigned int bits;
	/* Send preamble */
	bits = 0xffffffff;
	__wctdm_sdi_sendbits(wc, bits, 32);
	bits = (0x5 << 12) | (1 << 7) | (addr << 2) | 0x2;
	__wctdm_sdi_sendbits(wc, bits, 16);
	__wctdm_sdi_sendbits(wc, value, 16);
	
}

static inline unsigned short __wctdm_getsdi(struct wctdm *wc, unsigned char addr)
{
	unsigned int bits;
	/* Send preamble */
	bits = 0xffffffff;
	__wctdm_sdi_sendbits(wc, bits, 32);
	bits = (0x6 << 10) | (1 << 5) | (addr);
	__wctdm_sdi_sendbits(wc, bits, 14);
	return __wctdm_sdi_recvbits(wc, 18);
}

static inline void wctdm_setsdi(struct wctdm *wc, unsigned char addr, unsigned short value)
{
	unsigned long flags;
	spin_lock_irqsave(&wc->reglock, flags);
	__wctdm_setsdi(wc, addr, value);
	spin_unlock_irqrestore(&wc->reglock, flags);
}

static inline unsigned short wctdm_getsdi(struct wctdm *wc, unsigned char addr)
{
	unsigned long flags;
	unsigned short val;
	spin_lock_irqsave(&wc->reglock, flags);
	val = __wctdm_getsdi(wc, addr);
	spin_unlock_irqrestore(&wc->reglock, flags);
	return val;
}
#ifdef VPM_SUPPORT
static inline unsigned char wctdm_vpm_in(struct wctdm *wc, int unit, const unsigned int addr)
{
	return wctdm_getreg(wc, unit + NUM_CARDS, addr);
}

static inline void wctdm_vpm_out(struct wctdm *wc, int unit, const unsigned int addr, const unsigned char val)
{
	wctdm_setreg(wc, unit + NUM_CARDS, addr, val);
}
#endif

static inline void cmd_retransmit(struct wctdm *wc)
{
	int x,y;
	unsigned long flags;
	/* Force retransmissions */
	spin_lock_irqsave(&wc->reglock, flags);
	for (x=0;x<MAX_COMMANDS;x++) {
		for (y=0;y<wc->cards;y++) {
			if (!(wc->cmdq[y].cmds[x] & __CMD_FIN))
				wc->cmdq[y].cmds[x] &= ~(__CMD_TX | (0xff << 24));
		}
	}
	spin_unlock_irqrestore(&wc->reglock, flags);
}

static inline void wctdm_receiveprep(struct wctdm *wc, int dbl)
{
	volatile unsigned char *readchunk;
	int x,y;
	unsigned char expected;

	dbl = dbl % 2;

	readchunk = (volatile unsigned char *)wc->readchunk;
	if (dbl)
		readchunk += SFRAME_SIZE;
	for (x=0;x<ZT_CHUNKSIZE;x++) {
		if (x < ZT_CHUNKSIZE - 1) {
			expected = wc->rxident+1;
			wc->rxident = readchunk[EFRAME_SIZE + 1];
			if (wc->rxident != expected) {
				wc->span.irqmisses++;
				cmd_retransmit(wc);
			}
		}
		for (y=0;y < wc->cards;y++) {
			if (y < wc->type)
				wc->chans[y].readchunk[x] = readchunk[y];
			cmd_decifer(wc, readchunk, y);
		}
#ifdef VPM_SUPPORT
		if (wc->vpm) {
			for (y=NUM_CARDS;y < NUM_CARDS + NUM_EC; y++)
				cmd_decifer(wc, readchunk, y);
		}
#endif
#if 0
		if (cmddesc < 1024) {
			printk("RC Result: %02x\n", readchunk[EFRAME_SIZE+1]);
		}
#endif		
		readchunk += (EFRAME_SIZE + EFRAME_GAP);
	}
	/* XXX We're wasting 8 taps.  We should get closer :( */
	for (x=0;x<wc->type;x++) {
		if (wc->cardflag & (1 << x))
			zt_ec_chunk(&wc->chans[x], wc->chans[x].readchunk, wc->chans[x].writechunk);
	}
	zt_receive(&wc->span);
	/* Wake up anyone sleeping to read/write a new register */
	wake_up_interruptible(&wc->regq);
}

static void wctdm_stop_dma(struct wctdm *wc);
static void wctdm_restart_dma(struct wctdm *wc);

static int wait_access(struct wctdm *wc, int card)
{
    unsigned char data=0;
    long origjiffies;
    int count = 0;

    #define MAX 10 /* attempts */


    origjiffies = jiffies;
    /* Wait for indirect access */
    while (count++ < MAX)
	 {
		data = wctdm_getreg(wc, card, I_STATUS);

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
	int res = -1;
	/* Translate 3215 addresses */
	if (wc->flags[card] & FLAG_3215) {
		address = translate_3215(address);
		if (address == 255)
			return 0;
	}
	if(!wait_access(wc, card)) {
		wctdm_setreg(wc, card, IDA_LO,(unsigned char)(data & 0xFF));
		wctdm_setreg(wc, card, IDA_HI,(unsigned char)((data & 0xFF00)>>8));
		wctdm_setreg(wc, card, IAA,address);
		res = 0;
	};
	return res;
}

static int wctdm_proslic_getreg_indirect(struct wctdm *wc, int card, unsigned char address)
{ 
	int res = -1;
	char *p=NULL;
	/* Translate 3215 addresses */
	if (wc->flags[card] & FLAG_3215) {
		address = translate_3215(address);
		if (address == 255)
			return 0;
	}
	if (!wait_access(wc, card)) {
		wctdm_setreg(wc, card, IAA, address);
		if (!wait_access(wc, card)) {
			unsigned char data1, data2;
			data1 = wctdm_getreg(wc, card, IDA_LO);
			data2 = wctdm_getreg(wc, card, IDA_HI);
			res = data1 | (data2 << 8);
		} else
			p = "Failed to wait inside\n";
	} else
		p = "failed to wait\n";
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
#ifdef PAQ_DEBUG
	res = wc->cmdq[card].isrshadow[1];
	res &= ~0x3;
	if (res) {
		wc->cmdq[card].isrshadow[1]=0;
		wc->mods[card].fxs.palarms++;
		if (wc->mods[card].fxs.palarms < MAX_ALARMS) {
			printk("Power alarm (%02x) on module %d, resetting!\n", res, card + 1);
			if (wc->mods[card].fxs.lasttxhook == 4)
				wc->mods[card].fxs.lasttxhook = 1;
			wc->sethook[card] = CMD_WR(19, res);
#if 0
			wc->sethook[card] = CMD_WR(64, wc->mods[card].fxs.lasttxhook);
#endif

			/* wctdm_setreg_intr(wc, card, 64, wc->mods[card].fxs.lasttxhook); */
			/* Update shadow register to avoid extra power alarms until next read */
			wc->cmdq[card].isrshadow[1] = 0;
		} else {
			if (wc->mods[card].fxs.palarms == MAX_ALARMS)
				printk("Too many power alarms on card %d, NOT resetting!\n", card + 1);
		}
	}
#else
	res = wc->cmdq[card].isrshadow[1];
	if (!res && (res != wc->mods[card].fxs.lasttxhook)) {
		wc->mods[card].fxs.palarms++;
		if (wc->mods[card].fxs.palarms < MAX_ALARMS) {
			printk("Power alarm on module %d, resetting!\n", card + 1);
			if (wc->mods[card].fxs.lasttxhook == 4)
				wc->mods[card].fxs.lasttxhook = 1;
			wc->sethook[card] = CMD_WR(64, wc->mods[card].fxs.lasttxhook);

			/* wctdm_setreg_intr(wc, card, 64, wc->mods[card].fxs.lasttxhook); */
			/* Update shadow register to avoid extra power alarms until next read */
			wc->cmdq[card].isrshadow[1] = wc->mods[card].fxs.lasttxhook;
		} else {
			if (wc->mods[card].fxs.palarms == MAX_ALARMS)
				printk("Too many power alarms on card %d, NOT resetting!\n", card + 1);
		}
	}
#endif
}

static inline void wctdm_qrvdri_check_hook(struct wctdm *wc, int card)
{
	signed char b,b1;
	int qrvcard = card & 0xfc;

	
	if (wc->qrvdebtime[card] >= 2) wc->qrvdebtime[card]--;
	b = wc->cmdq[qrvcard].isrshadow[0];	/* Hook/Ring state */
	b &= 0xcc; /* use bits 3-4 and 6-7 only */

	if (wc->radmode[qrvcard] & RADMODE_IGNORECOR) b &= ~4;
	else if (!(wc->radmode[qrvcard] & RADMODE_INVERTCOR)) b ^= 4;
	if (wc->radmode[qrvcard + 1] | RADMODE_IGNORECOR) b &= ~0x40;
	else if (!(wc->radmode[qrvcard + 1] | RADMODE_INVERTCOR)) b ^= 0x40;

	if ((wc->radmode[qrvcard] & RADMODE_IGNORECT) || 
		(!(wc->radmode[qrvcard] & RADMODE_EXTTONE))) b &= ~8;
	else if (!(wc->radmode[qrvcard] & RADMODE_EXTINVERT)) b ^= 8;
	if ((wc->radmode[qrvcard + 1] & RADMODE_IGNORECT) || 
		(!(wc->radmode[qrvcard + 1] & RADMODE_EXTTONE))) b &= ~0x80;
	else if (!(wc->radmode[qrvcard + 1] & RADMODE_EXTINVERT)) b ^= 0x80;
	/* now b & MASK should be zero, if its active */
	/* check for change in chan 0 */
	if ((!(b & 0xc)) != wc->qrvhook[qrvcard + 2])
	{
		wc->qrvdebtime[qrvcard] = wc->debouncetime[qrvcard];
		wc->qrvhook[qrvcard + 2] = !(b & 0xc);
	} 
	/* if timed-out and ready */
	if (wc->qrvdebtime[qrvcard] == 1)
	{
		b1 = wc->qrvhook[qrvcard + 2];
if (debug) printk("QRV channel %d rx state changed to %d\n",qrvcard,wc->qrvhook[qrvcard + 2]);
		zt_hooksig(&wc->chans[qrvcard], 
			(b1) ? ZT_RXSIG_OFFHOOK : ZT_RXSIG_ONHOOK);
		wc->qrvdebtime[card] = 0;
	}
	/* check for change in chan 1 */
	if ((!(b & 0xc0)) != wc->qrvhook[qrvcard + 3])
	{
		wc->qrvdebtime[qrvcard + 1] = QRV_DEBOUNCETIME;
		wc->qrvhook[qrvcard + 3] = !(b & 0xc0);
	}
	if (wc->qrvdebtime[qrvcard + 1] == 1)
	{
		b1 = wc->qrvhook[qrvcard + 3];
if (debug) printk("QRV channel %d rx state changed to %d\n",qrvcard + 1,wc->qrvhook[qrvcard + 3]);
		zt_hooksig(&wc->chans[qrvcard + 1], 
			(b1) ? ZT_RXSIG_OFFHOOK : ZT_RXSIG_ONHOOK);
		wc->qrvdebtime[card] = 0;
	}
	return;
}

static inline void wctdm_voicedaa_check_hook(struct wctdm *wc, int card)
{
	unsigned char res;
	signed char b;
	/* Try to track issues that plague slot one FXO's */
	b = wc->cmdq[card].isrshadow[0];	/* Hook/Ring state */
	b &= 0x9b;
	if (wc->mods[card].fxo.offhook) {
		if (b != 0x9)
			wctdm_setreg_intr(wc, card, 5, 0x9);
	} else {
		if (b != 0x8)
			wctdm_setreg_intr(wc, card, 5, 0x8);
	}
	if (!wc->mods[card].fxo.offhook) {
		res = wc->cmdq[card].isrshadow[0];	/* Hook/Ring state */
		if ((res & 0x60) && wc->mods[card].fxo.battery) {
			wc->mods[card].fxo.ringdebounce += (ZT_CHUNKSIZE * 4);
			if (wc->mods[card].fxo.ringdebounce >= ZT_CHUNKSIZE * RING_DEBOUNCE) {
				if (!wc->mods[card].fxo.wasringing) {
					wc->mods[card].fxo.wasringing = 1;
					zt_hooksig(&wc->chans[card], ZT_RXSIG_RING);
					if (debug)
						printk("RING on %d/%d!\n", wc->span.spanno, card + 1);
				}
				wc->mods[card].fxo.ringdebounce = ZT_CHUNKSIZE * RING_DEBOUNCE;
			}
		} else {
			wc->mods[card].fxo.ringdebounce -= ZT_CHUNKSIZE;
			if (wc->mods[card].fxo.ringdebounce <= 0) {
				if (wc->mods[card].fxo.wasringing) {
					wc->mods[card].fxo.wasringing = 0;
					zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
					if (debug)
						printk("NO RING on %d/%d!\n", wc->span.spanno, card + 1);
				}
				wc->mods[card].fxo.ringdebounce = 0;
			}
				
		}
	}
	b = wc->cmdq[card].isrshadow[1]; /* Voltage */
	if (abs(b) < battthresh) {
		wc->mods[card].fxo.nobatttimer++;
#if 0
		if (wc->mods[card].fxo.battery)
			printk("Battery loss: %d (%d debounce)\n", b, wc->mods[card].fxo.battdebounce);
#endif
		if (wc->mods[card].fxo.battery && !wc->mods[card].fxo.battdebounce) {
			if (debug)
				printk("NO BATTERY on %d/%d!\n", wc->span.spanno, card + 1);
			wc->mods[card].fxo.battery =  0;
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
			wc->mods[card].fxo.battdebounce = battdebounce;
		} else if (!wc->mods[card].fxo.battery)
			wc->mods[card].fxo.battdebounce = battdebounce;
	} else if (abs(b) > battthresh) {
		if (!wc->mods[card].fxo.battery && !wc->mods[card].fxo.battdebounce) {
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
			wc->mods[card].fxo.battery = 1;
			wc->mods[card].fxo.nobatttimer = 0;
			wc->mods[card].fxo.battdebounce = battdebounce;
		} else if (wc->mods[card].fxo.battery)
			wc->mods[card].fxo.battdebounce = battdebounce;

		if (wc->mods[card].fxo.lastpol >= 0) {
		    if (b < 0) {
			wc->mods[card].fxo.lastpol = -1;
			wc->mods[card].fxo.polaritydebounce = POLARITY_DEBOUNCE;
		    }
		} 
		if (wc->mods[card].fxo.lastpol <= 0) {
		    if (b > 0) {
			wc->mods[card].fxo.lastpol = 1;
			wc->mods[card].fxo.polaritydebounce = POLARITY_DEBOUNCE;
		    }
		}
	} else {
		/* It's something else... */
		wc->mods[card].fxo.battdebounce = battdebounce;
	}
	if (wc->mods[card].fxo.battdebounce)
		wc->mods[card].fxo.battdebounce--;
	if (wc->mods[card].fxo.polaritydebounce) {
	        wc->mods[card].fxo.polaritydebounce--;
		if (wc->mods[card].fxo.polaritydebounce < 1) {
		    if (wc->mods[card].fxo.lastpol != wc->mods[card].fxo.polarity) {
			if (debug)
				printk("%lu Polarity reversed (%d -> %d)\n", jiffies, 
			       wc->mods[card].fxo.polarity, 
			       wc->mods[card].fxo.lastpol);
			if (wc->mods[card].fxo.polarity)
			    zt_qevent_lock(&wc->chans[card], ZT_EVENT_POLARITY);
			wc->mods[card].fxo.polarity = wc->mods[card].fxo.lastpol;
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

	res = wc->cmdq[card].isrshadow[0];	/* Hook state */
	hook = (res & 1);
		
	if (hook != wc->mods[card].fxs.lastrxhook) {
		/* Reset the debounce (must be multiple of 4ms) */
		wc->mods[card].fxs.debounce = 8 * (4 * 8);
#if 0
		printk("Resetting debounce card %d hook %d, %d\n", card, hook, wc->mods[card].fxs.debounce);
#endif
	} else {
		if (wc->mods[card].fxs.debounce > 0) {
			wc->mods[card].fxs.debounce-= 4 * ZT_CHUNKSIZE;
#if 0
			printk("Sustaining hook %d, %d\n", hook, wc->mods[card].fxs.debounce);
#endif
			if (!wc->mods[card].fxs.debounce) {
#if 0
				printk("Counted down debounce, newhook: %d...\n", hook);
#endif
				wc->mods[card].fxs.debouncehook = hook;
			}
			if (!wc->mods[card].fxs.oldrxhook && wc->mods[card].fxs.debouncehook) {
				/* Off hook */
				if (debug)
					printk("wctdm: Card %d Going off hook\n", card);
				zt_hooksig(&wc->chans[card], ZT_RXSIG_OFFHOOK);
				if (robust)
					wctdm_init_proslic(wc, card, 1, 0, 1);
				wc->mods[card].fxs.oldrxhook = 1;
			
			} else if (wc->mods[card].fxs.oldrxhook && !wc->mods[card].fxs.debouncehook) {
				/* On hook */
				if (debug)
					printk("wctdm: Card %d Going on hook\n", card);
				zt_hooksig(&wc->chans[card], ZT_RXSIG_ONHOOK);
				wc->mods[card].fxs.oldrxhook = 0;
			}
		}
	}
	wc->mods[card].fxs.lastrxhook = hook;
}


static inline void wctdm_reinit_descriptor(struct wctdm *wc, int tx, int dbl, char *s)
{
	int o2 = 0;
	o2 += dbl * 4;
	if (!tx)
		o2 += ERING_SIZE * 4;
	wc->descripchunk[o2] = 0x80000000;
}

#ifdef VPM_SUPPORT
static inline void wctdm_vpm_check(struct wctdm *wc, int x)
{
	if (wc->cmdq[x].isrshadow[0]) {
		if (debug)
			printk("VPM: Detected dtmf ON channel %02x on chip %d!\n", wc->cmdq[x].isrshadow[0], x - NUM_CARDS);
		wc->sethook[x] = CMD_WR(0xb9, wc->cmdq[x].isrshadow[0]);
		wc->cmdq[x].isrshadow[0] = 0;
		/* Cancel most recent lookup, if there is one */
		wc->cmdq[x].cmds[USER_COMMANDS+0] = 0x00000000; 
	} else if (wc->cmdq[x].isrshadow[1]) {
		if (debug)
			printk("VPM: Detected dtmf OFF channel %02x on chip %d!\n", wc->cmdq[x].isrshadow[1], x - NUM_CARDS);
		wc->sethook[x] = CMD_WR(0xbd, wc->cmdq[x].isrshadow[1]);
		wc->cmdq[x].isrshadow[1] = 0;
		/* Cancel most recent lookup, if there is one */
		wc->cmdq[x].cmds[USER_COMMANDS+1] = 0x00000000; 
	}
}

static int wctdm_echocan(struct zt_chan *chan, int eclen)
{
	struct wctdm *wc = chan->pvt;
	int channel;
	int unit;
	if (!wc->vpm)
		return -ENODEV;
	channel = (chan->chanpos - 1);
	unit = (chan->chanpos - 1) & 0x3;
	if (wc->vpm < 2)
		channel >>= 2;

	if(debug) 
		printk("echocan: Unit is %d, Channel is  %d length %d\n", 
			unit, channel, eclen);
	if (eclen)
		wctdm_vpm_out(wc,unit,channel,0x3e);
	else
		wctdm_vpm_out(wc,unit,channel,0x01);
	return 0;
}
#endif
static inline void wctdm_isr_misc(struct wctdm *wc)
{
	int x;

	for (x=0;x<wc->cards;x++) {
		if (wc->cardflag & (1 << x)) {
			if (wc->modtype[x] == MOD_TYPE_FXS) {
				if (!(wc->intcount % 10000)) {
					/* Accept an alarm once per 10 seconds */
					if (wc->mods[x].fxs.palarms)
						wc->mods[x].fxs.palarms--;
				}
				wctdm_proslic_check_hook(wc, x);
				if (!(wc->intcount & 0xfc))
					wctdm_proslic_recheck_sanity(wc, x);
				if (wc->mods[x].fxs.lasttxhook == 0x4) {
					/* RINGing, prepare for OHT */
					wc->mods[x].fxs.ohttimer = OHT_TIMER << 3;
					wc->mods[x].fxs.idletxhookstate = 0x2;	/* OHT mode when idle */
				} else {
					if (wc->mods[x].fxs.ohttimer) {
						wc->mods[x].fxs.ohttimer-= ZT_CHUNKSIZE;
						if (!wc->mods[x].fxs.ohttimer) {
							wc->mods[x].fxs.idletxhookstate = 0x1;	/* Switch to active */
							if (wc->mods[x].fxs.lasttxhook == 0x2) {
								/* Apply the change if appropriate */
								wc->mods[x].fxs.lasttxhook = 0x1;
								wc->sethook[x] = CMD_WR(64, wc->mods[x].fxs.lasttxhook);
								/* wctdm_setreg_intr(wc, x, 64, wc->mods[x].fxs.lasttxhook); */
							}
						}
					}
				}
			} else if (wc->modtype[x] == MOD_TYPE_FXO) {
				wctdm_voicedaa_check_hook(wc, x);
			} else if (wc->modtype[x] == MOD_TYPE_QRV) {
				wctdm_qrvdri_check_hook(wc, x);
			}
		}
	}
#ifdef VPM_SUPPORT
	if (wc->vpm > 0) {
		for (x=NUM_CARDS;x<NUM_CARDS+NUM_EC;x++) {
			wctdm_vpm_check(wc, x);
		}
	}
#endif
}

static inline int wctdm_check_descriptor(struct wctdm *wc, int tx)
{
	int o2 = 0;
	if (!tx) {
		o2 += ERING_SIZE * 4;
		o2 += wc->rdbl * 4;
	} else {
		o2 += wc->tdbl * 4;
	}
	if (!(wc->descripchunk[o2] & 0x80000000)) {
		if (tx) {
			wc->txints++;
			wctdm_transmitprep(wc, wc->tdbl);
			wctdm_reinit_descriptor(wc, tx, wc->tdbl, "txchk");
			wc->tdbl = (wc->tdbl + 1) % ERING_SIZE;
			wctdm_isr_misc(wc);
			wc->intcount++;
		} else {
			wc->rxints++;
			wctdm_receiveprep(wc, wc->rdbl);
			wctdm_reinit_descriptor(wc, tx, wc->rdbl, "rxchk");
			wc->rdbl = (wc->rdbl + 1) % ERING_SIZE;
		}
		return 1;
	}
	return 0;
}

static void wctdm_init_descriptors(struct wctdm *wc)
{
	volatile unsigned int *descrip;
	dma_addr_t descripdma;
	dma_addr_t writedma;
	dma_addr_t readdma;
	int x;
	
	descrip = wc->descripchunk;
	descripdma = wc->descripdma;
	writedma = wc->writedma;
	readdma = wc->readdma;

	for (x=0;x<ERING_SIZE;x++) {
		if (x < ERING_SIZE - 1)
			descripdma += 16;
		else
			descripdma = wc->descripdma;

		/* Transmit descriptor */
		descrip[0 ] = 0x80000000;
		descrip[1 ] = 0xe5800000 | (SFRAME_SIZE);
		if (x % 2)
			descrip[2 ] = writedma + SFRAME_SIZE;
		else
			descrip[2 ] = writedma;
		descrip[3 ] = descripdma;
	
		/* Receive descriptor */
		descrip[0 + ERING_SIZE * 4] = 0x80000000;
		descrip[1 + ERING_SIZE * 4] = 0x01000000 | (SFRAME_SIZE);
		if (x % 2)
			descrip[2 + ERING_SIZE * 4] = readdma + SFRAME_SIZE;
		else
			descrip[2 + ERING_SIZE * 4] = readdma;
		descrip[3 + ERING_SIZE * 4] = descripdma + ERING_SIZE * 16;
	
		/* Advance descriptor */
		descrip += 4;
	}	
}

ZAP_IRQ_HANDLER(wctdm_interrupt)
{
	struct wctdm *wc = dev_id;
	unsigned int ints;
	int res;

	/* Read and clear interrupts */
	ints = wctdm_getctl(wc, 0x0028);

	if (!ints)
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		

	wctdm_setctl(wc, 0x0028, ints);

	ints &= wc->intmask;
	if (ints & 0x00000041) {
		do {
			res = wctdm_check_descriptor(wc, 0);
			res |= wctdm_check_descriptor(wc, 1);
		} while(res);
#if 0 
		while(wctdm_check_descriptor(wc, 0));
		wctdm_setctl(wc, 0x0010, 0x00000000);
	}
	if (ints & 0x00000005) {
		while(wctdm_check_descriptor(wc, 1));
		wctdm_setctl(wc, 0x0008, 0x00000000);
#endif
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
	wctdm_setreg(wc, card, 92, 0xc0 /* was 0xff */);

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
		printk("ProSLIC on module %d failed to powerup within %d ms (%d mV only)\n\n -- DID YOU REMEMBER TO PLUG IN THE HD POWER CABLE TO THE TDM CARD??\n",
		       card, (int)(((jiffies - origjiffies) * 1000 / HZ)),
			vbat * 375);
		return -1;
	} else if (debug) {
		printk("ProSLIC on module %d powered up to -%d volts (%02x) in %d ms\n",
		       card, vbat * 376 / 1000, vbat, (int)(((jiffies - origjiffies) * 1000 / HZ)));
	}

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

static int wctdm_proslic_manual_calibrate(struct wctdm *wc, int card)
{
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

	if (wc->modtype[card & 0xfc] == MOD_TYPE_QRV) return -2;

	wc->modtype[card] = MOD_TYPE_NONE;
	/* Wait just a bit */
	wait_just_a_bit(HZ/10);

	wc->modtype[card] = MOD_TYPE_FXO;
	wait_just_a_bit(HZ/10);

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
	reg31 = 0xa3;
	reg31 |= (fxo_modes[_opermode].ohs2 << 3);
	wctdm_setreg(wc, card, 31, reg31);

	/* Set Transmit/Receive timeslot */
	wctdm_setreg(wc, card, 34, (card * 8) & 0xff);
	wctdm_setreg(wc, card, 35, (card * 8) >> 8);
	wctdm_setreg(wc, card, 36, (card * 8) & 0xff);
	wctdm_setreg(wc, card, 37, (card * 8) >> 8);

	/* Enable ISO-Cap */
	wctdm_setreg(wc, card, 6, 0x00);

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
	
	/* Apply negative Tx gain of 4.5db to DAA */
	wctdm_setreg(wc, card, 38, 0x14);	/* 4db */
	wctdm_setreg(wc, card, 40, 0x15);	/* 0.5db */

	/* Apply negative Rx gain of 4.5db to DAA */
	wctdm_setreg(wc, card, 39, 0x14);	/* 4db */
	wctdm_setreg(wc, card, 41, 0x15);	/* 0.5db */

	
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
			if(fxotxgain % 10) {
				wctdm_setreg(wc, card, 40, (fxotxgain%10));
			}
		}
	}
	if (fxorxgain) {
		if (fxorxgain >=  -150 && fxorxgain < 0) {
			wctdm_setreg(wc, card, 39, 16+ (fxorxgain/-10));
			if(fxotxgain % 10) {
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
	
	if(debug)
		printk("DEBUG fxotxgain:%i.%i fxorxgain:%i.%i\n", (wctdm_getreg(wc, card, 38)/16) ? -(wctdm_getreg(wc, card, 38) - 16) : wctdm_getreg(wc, card, 38), (wctdm_getreg(wc, card, 40)/16) ? -(wctdm_getreg(wc, card, 40) - 16) : wctdm_getreg(wc, card, 40), (wctdm_getreg(wc, card, 39)/16) ? -(wctdm_getreg(wc, card, 39) - 16): wctdm_getreg(wc, card, 39), (wctdm_getreg(wc, card, 41)/16)?-(wctdm_getreg(wc, card, 41) - 16) : wctdm_getreg(wc, card, 41));
	
	return 0;
		
}

static int wctdm_init_proslic(struct wctdm *wc, int card, int fast, int manual, int sane)
{

	unsigned short tmp[5];
	unsigned char r19,r9;
	int x;
	int fxsmode=0;

	if (wc->modtype[card & 0xfc] == MOD_TYPE_QRV) return -2;

	/* By default, don't send on hook */
	wc->mods[card].fxs.idletxhookstate = 1;

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
			wc->mods[card].fxs.calregs.vals[x] = wctdm_getreg(wc, card, 96 + x);
#endif

	} else {
		/* Restore calibration registers */
		for (x=0;x<NUM_CAL_REGS;x++)
			wctdm_setreg(wc, card, 96 + x, wc->mods[card].fxs.calregs.vals[x]);
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
    wctdm_setreg(wc, card, 2, (card * 8) & 0xff);    // Tx Start count low byte  0
    wctdm_setreg(wc, card, 3, (card * 8) >> 8);    // Tx Start count high byte 0
    wctdm_setreg(wc, card, 4, (card * 8) & 0xff);    // Rx Start count low byte  0
    wctdm_setreg(wc, card, 5, (card * 8) >> 8);    // Rx Start count high byte 0
    wctdm_setreg(wc, card, 18, 0xff);     // clear all interrupt
    wctdm_setreg(wc, card, 19, 0xff);
    wctdm_setreg(wc, card, 20, 0xff);
    wctdm_setreg(wc, card, 22, 0xff);
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
	wc->mods[card].fxs.lasttxhook = 1;
	return 0;
}

static int wctdm_init_qrvdri(struct wctdm *wc, int card)
{
unsigned char x,y;
unsigned long endjif;

	/* have to set this, at least for now */
	wc->modtype[card] = MOD_TYPE_QRV;
	if (!(card & 3)) /* if at base of card, reset and write it */
	{
		wctdm_setreg(wc,card,0,0x80); 
		wctdm_setreg(wc,card,0,0x55);
		wctdm_setreg(wc,card,1,0x69);
		wc->qrvhook[card] = wc->qrvhook[card + 1] = 0;
		wc->qrvhook[card + 2] = wc->qrvhook[card + 3] = 0xff;
		wc->debouncetime[card] = wc->debouncetime[card + 1] = QRV_DEBOUNCETIME;
		wc->qrvdebtime[card] = wc->qrvdebtime[card + 1] = 0;
		wc->radmode[card] = wc->radmode[card + 1] = 0;
		wc->txgain[card] = wc->txgain[card + 1] = 3599;
		wc->rxgain[card] = wc->rxgain[card + 1] = 1199;
	} else { /* channel is on same card as base, no need to test */
		if (wc->modtype[card & 0x7c] == MOD_TYPE_QRV) 
		{
			/* only lower 2 are valid */
			if (!(card & 2)) return 0;
		}
		wc->modtype[card] = MOD_TYPE_NONE;
		return 1;
	}
	x = wctdm_getreg(wc,card,0);
	y = wctdm_getreg(wc,card,1);
	/* if not a QRV card, return as such */
	if ((x != 0x55) || (y != 0x69))
	{
		wc->modtype[card] = MOD_TYPE_NONE;
		return 1;
	}
	for(x = 0; x < 0x30; x++)
	{
		if ((x >= 0x1c) && (x <= 0x1e)) wctdm_setreg(wc,card,x,0xff);
		else wctdm_setreg(wc,card,x,0);
	}
	wctdm_setreg(wc,card,0,0x80); 
	endjif = jiffies + (HZ/10);
	while(endjif > jiffies);
	wctdm_setreg(wc,card,0,0x10); 
	wctdm_setreg(wc,card,0,0x10); 
	endjif = jiffies + (HZ/10);
	while(endjif > jiffies);
	/* set up modes */
	wctdm_setreg(wc,card,0,0x1c); 
	/* set up I/O directions */
	wctdm_setreg(wc,card,1,0x33); 
	wctdm_setreg(wc,card,2,0x0f); 
	wctdm_setreg(wc,card,5,0x0f); 
	/* set up I/O to quiescent state */
	wctdm_setreg(wc,card,3,0x11);  /* D0-7 */
	wctdm_setreg(wc,card,4,0xa);  /* D8-11 */
	wctdm_setreg(wc,card,7,0);  /* CS outputs */
	/* set up timeslots */
	wctdm_setreg(wc,card,0x13,card + 0x80);  /* codec 2 tx, ts0 */
	wctdm_setreg(wc,card,0x17,card + 0x80);  /* codec 0 rx, ts0 */
	wctdm_setreg(wc,card,0x14,card + 0x81);  /* codec 1 tx, ts1 */
	wctdm_setreg(wc,card,0x18,card + 0x81);  /* codec 1 rx, ts1 */
	/* set up for max gains */
	wctdm_setreg(wc,card,0x26,0x24); 
	wctdm_setreg(wc,card,0x27,0x24); 
	wctdm_setreg(wc,card,0x0b,0x01);  /* "Transmit" gain codec 0 */
	wctdm_setreg(wc,card,0x0c,0x01);  /* "Transmit" gain codec 1 */
	wctdm_setreg(wc,card,0x0f,0xff);  /* "Receive" gain codec 0 */
	wctdm_setreg(wc,card,0x10,0xff);  /* "Receive" gain codec 1 */
	return 0;
}

static void qrv_dosetup(struct zt_chan *chan,struct wctdm *wc)
{
int qrvcard;
unsigned char r;
long l;

	/* actually do something with the values */
	qrvcard = (chan->chanpos - 1) & 0xfc;
	if (debug) printk("@@@@@ radmodes: %d,%d  rxgains: %d,%d   txgains: %d,%d\n",
	wc->radmode[qrvcard],wc->radmode[qrvcard + 1],
		wc->rxgain[qrvcard],wc->rxgain[qrvcard + 1],
			wc->txgain[qrvcard],wc->txgain[qrvcard + 1]);
	r = 0;
	if (wc->radmode[qrvcard] & RADMODE_DEEMP) r |= 4;		
	if (wc->radmode[qrvcard + 1] & RADMODE_DEEMP) r |= 8;		
	if (wc->rxgain[qrvcard] < 1200) r |= 1;
	if (wc->rxgain[qrvcard + 1] < 1200) r |= 2;
	wctdm_setreg(wc, qrvcard, 7, r);
	if (debug) printk("@@@@@ setting reg 7 to %02x hex\n",r);
	r = 0;
	if (wc->radmode[qrvcard] & RADMODE_PREEMP) r |= 3;
	else if (wc->txgain[qrvcard] >= 3600) r |= 1;
	else if (wc->txgain[qrvcard] >= 1200) r |= 2;
	if (wc->radmode[qrvcard + 1] & RADMODE_PREEMP) r |= 0xc;
	else if (wc->txgain[qrvcard + 1] >= 3600) r |= 4;
	else if (wc->txgain[qrvcard + 1] >= 1200) r |= 8;
	wctdm_setreg(wc, qrvcard, 4, r);
	if (debug) printk("@@@@@ setting reg 4 to %02x hex\n",r);
	r = 0;
	if (wc->rxgain[qrvcard] >= 2400) r |= 1; 
	if (wc->rxgain[qrvcard + 1] >= 2400) r |= 2; 
	wctdm_setreg(wc, qrvcard, 0x25, r);
	if (debug) printk("@@@@@ setting reg 0x25 to %02x hex\n",r);
	r = 0;
	if (wc->txgain[qrvcard] < 2400) r |= 1; else r |= 4;
	if (wc->txgain[qrvcard + 1] < 2400) r |= 8; else r |= 0x20;
	wctdm_setreg(wc, qrvcard, 0x26, r);
	if (debug) printk("@@@@@ setting reg 0x26 to %02x hex\n",r);
	l = ((long)(wc->rxgain[qrvcard] % 1200) * 10000) / 46875;
	if (l == 0) l = 1;
	if (wc->rxgain[qrvcard] >= 2400) l += 181;
	wctdm_setreg(wc, qrvcard, 0x0b, (unsigned char)l);
	if (debug) printk("@@@@@ setting reg 0x0b to %02x hex\n",(unsigned char)l);
	l = ((long)(wc->rxgain[qrvcard + 1] % 1200) * 10000) / 46875;
	if (l == 0) l = 1;
	if (wc->rxgain[qrvcard + 1] >= 2400) l += 181;
	wctdm_setreg(wc, qrvcard, 0x0c, (unsigned char)l);
	if (debug) printk("@@@@@ setting reg 0x0c to %02x hex\n",(unsigned char)l);
	l = ((long)(wc->txgain[qrvcard] % 1200) * 10000) / 46875;
	if (l == 0) l = 1;
	wctdm_setreg(wc, qrvcard, 0x0f, (unsigned char)l);
	if (debug) printk("@@@@@ setting reg 0x0f to %02x hex\n", (unsigned char)l);
	l = ((long)(wc->txgain[qrvcard + 1] % 1200) * 10000) / 46875;
	if (l == 0) l = 1;
	wctdm_setreg(wc, qrvcard, 0x10,(unsigned char)l);
	if (debug) printk("@@@@@ setting reg 0x10 to %02x hex\n",(unsigned char)l);
	return;
}

static int wctdm_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	struct wctdm_stats stats;
	struct wctdm_regs regs;
	struct wctdm_regop regop;
	struct wctdm_echo_coefs echoregs;
	struct wctdm *wc = chan->pvt;
	int x;
	union {
		struct zt_radio_stat s;
		struct zt_radio_param p;
	} stack;

#if 0
	/* XXX */
	printk("RxInts: %d, TxInts: %d\n", wc->rxints, wc->txints);
	printk("RxIdent: %d, TxIdent: %d\n", wc->rxident, wc->txident);
	for (x=0;x<wc->cards;x++)
		printk("Card %d isrshadow: %02x/%02x\n", x, wc->cmdq[x].isrshadow[0], wc->cmdq[x].isrshadow[1]);
	cmddesc = 0;
#endif	
#if 0 
	if (wc->vpm) {
		char tmp[80];
		for (x=0;x<0x200;x++) {
			switch (x & 0xf) {
			case 0:
				sprintf(tmp, "%03x: %02x ", x, wctdm_vpm_in(wc, 0, x));
				break;
			case 0xf:
				printk("%s%02x\n", tmp, wctdm_vpm_in(wc, 0, x));
				break;
			default:
				sprintf(tmp + strlen(tmp), "%02x ", wctdm_vpm_in(wc, 0, x));
				break;
			}
		}
	}

#endif
	switch (cmd) {
	case ZT_ONHOOKTRANSFER:
		if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_FXS)
			return -EINVAL;
		if (get_user(x, (int *)data))
			return -EFAULT;
		wc->mods[chan->chanpos - 1].fxs.ohttimer = x << 3;
		wc->mods[chan->chanpos - 1].fxs.idletxhookstate = 0x2;	/* OHT mode when idle */
		if (wc->mods[chan->chanpos - 1].fxs.lasttxhook == 0x1) {
			/* Apply the change if appropriate */
			wc->mods[chan->chanpos - 1].fxs.lasttxhook = 0x2;
			wc->sethook[chan->chanpos - 1] = CMD_WR(64, wc->mods[chan->chanpos - 1].fxs.lasttxhook);
			/* wctdm_setreg(wc, chan->chanpos - 1, 64, wc->mods[chan->chanpos - 1].fxs.lasttxhook); */
		}
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
		} else if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_QRV) {
			memset(&regs, 0, sizeof(regs));
			for (x=0;x<0x32;x++)
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
#ifdef VPM_SUPPORT
	case ZT_TONEDETECT:
		if (get_user(x, (int *) data))
			return -EFAULT;
		if (!wc->vpm)
			return -ENOSYS;
		if (x && !vpmdtmfsupport)
			return -ENOSYS;
		if (x & ZT_TONEDETECT_ON)
			wc->dtmfmask |= (1 << (chan->chanpos - 1));
		else
			wc->dtmfmask &= ~(1 << (chan->chanpos - 1));
		if (x & ZT_TONEDETECT_MUTE)
			wc->dtmfmutemask |= (1 << (chan->chanpos - 1));
		else
			wc->dtmfmutemask &= ~(1 << (chan->chanpos - 1));
		return 0;
#endif
	case ZT_RADIO_GETPARAM:
		if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_QRV) 
			return -ENOTTY;
		if (copy_from_user(&stack.p,(struct zt_radio_param *)data,sizeof(struct zt_radio_param))) return -EFAULT;
		stack.p.data = 0; /* start with 0 value in output */
		switch(stack.p.radpar) {
		case ZT_RADPAR_INVERTCOR:
			if (wc->radmode[chan->chanpos - 1] & RADMODE_INVERTCOR)
				stack.p.data = 1;
			break;
		case ZT_RADPAR_IGNORECOR:
			if (wc->radmode[chan->chanpos - 1] & RADMODE_IGNORECOR)
				stack.p.data = 1;
			break;
		case ZT_RADPAR_IGNORECT:
			if (wc->radmode[chan->chanpos - 1] & RADMODE_IGNORECT)
				stack.p.data = 1;
			break;
		case ZT_RADPAR_EXTRXTONE:
			stack.p.data = 0;
			if (wc->radmode[chan->chanpos - 1] & RADMODE_EXTTONE)
			{
				stack.p.data = 1;
				if (wc->radmode[chan->chanpos - 1] & RADMODE_EXTINVERT)
				{
					stack.p.data = 2;
				}
			}
			break;
		case ZT_RADPAR_DEBOUNCETIME:
			stack.p.data = wc->debouncetime[chan->chanpos - 1];
			break;
		case ZT_RADPAR_RXGAIN:
			stack.p.data = wc->rxgain[chan->chanpos - 1] - 1199;
			break;
		case ZT_RADPAR_TXGAIN:
			stack.p.data = wc->txgain[chan->chanpos - 1] - 3599;
			break;
		case ZT_RADPAR_DEEMP:
			stack.p.data = 0;
			if (wc->radmode[chan->chanpos - 1] & RADMODE_DEEMP)
			{
				stack.p.data = 1;
			}
			break;
		case ZT_RADPAR_PREEMP:
			stack.p.data = 0;
			if (wc->radmode[chan->chanpos - 1] & RADMODE_PREEMP)
			{
				stack.p.data = 1;
			}
			break;
		default:
			return -EINVAL;
		}
		if (copy_to_user((struct zt_radio_param *)data,&stack.p,sizeof(struct zt_radio_param))) return -EFAULT;
		break;
	case ZT_RADIO_SETPARAM:
		if (wc->modtype[chan->chanpos - 1] != MOD_TYPE_QRV) 
			return -ENOTTY;
		if (copy_from_user(&stack.p,(struct zt_radio_param *)data,sizeof(struct zt_radio_param))) return -EFAULT;
		switch(stack.p.radpar) {
		case ZT_RADPAR_INVERTCOR:
			if (stack.p.data)
				wc->radmode[chan->chanpos - 1] |= RADMODE_INVERTCOR;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_INVERTCOR;
			return 0;
		case ZT_RADPAR_IGNORECOR:
			if (stack.p.data)
				wc->radmode[chan->chanpos - 1] |= RADMODE_IGNORECOR;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_IGNORECOR;
			return 0;
		case ZT_RADPAR_IGNORECT:
			if (stack.p.data)
				wc->radmode[chan->chanpos - 1] |= RADMODE_IGNORECT;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_IGNORECT;
			return 0;
		case ZT_RADPAR_EXTRXTONE:
			if (stack.p.data)
				wc->radmode[chan->chanpos - 1] |= RADMODE_EXTTONE;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_EXTTONE;
			if (stack.p.data > 1)
				wc->radmode[chan->chanpos - 1] |= RADMODE_EXTINVERT;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_EXTINVERT;
			return 0;
		case ZT_RADPAR_DEBOUNCETIME:
			wc->debouncetime[chan->chanpos - 1] = stack.p.data;
			return 0;
		case ZT_RADPAR_RXGAIN:
			/* if out of range */
			if ((stack.p.data <= -1200) || (stack.p.data > 1552))
			{
				return -EINVAL;
			}
			wc->rxgain[chan->chanpos - 1] = stack.p.data + 1199;
			break;
		case ZT_RADPAR_TXGAIN:
			/* if out of range */
			if (wc->radmode[chan->chanpos -1] & RADMODE_PREEMP)
			{
				if ((stack.p.data <= -2400) || (stack.p.data > 0))
				{
					return -EINVAL;
				}
			}
			else
			{
				if ((stack.p.data <= -3600) || (stack.p.data > 1200))
				{
					return -EINVAL;
				}
			}
			wc->txgain[chan->chanpos - 1] = stack.p.data + 3599;
			break;
		case ZT_RADPAR_DEEMP:
			if (stack.p.data)
				wc->radmode[chan->chanpos - 1] |= RADMODE_DEEMP;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_DEEMP;
			wc->rxgain[chan->chanpos - 1] = 1199;
			break;
		case ZT_RADPAR_PREEMP:
			if (stack.p.data)
				wc->radmode[chan->chanpos - 1] |= RADMODE_PREEMP;
			else
				wc->radmode[chan->chanpos - 1] &= ~RADMODE_PREEMP;
			wc->txgain[chan->chanpos - 1] = 3599;
			break;
		default:
			return -EINVAL;
		}
		qrv_dosetup(chan,wc);
		return 0;				
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
	int x;
	signed char reg;
	wc->usecount--;
#ifndef LINUX26
	MOD_DEC_USE_COUNT;
#else
	module_put(THIS_MODULE);
#endif
	for (x=0;x<wc->cards;x++) {
		if (wc->modtype[x] == MOD_TYPE_FXS)
			wc->mods[x].fxs.idletxhookstate = 1;
		if (wc->modtype[x] == MOD_TYPE_QRV)
		{
			int qrvcard = x & 0xfc;

			wc->qrvhook[x] = 0;
			wc->qrvhook[x + 2] = 0xff;
			wc->debouncetime[x] = QRV_DEBOUNCETIME;
			wc->qrvdebtime[x] = 0;
			wc->radmode[x] = 0;
			wc->txgain[x] = 3599;
			wc->rxgain[x] = 1199;
			reg = 0;
			if (!wc->qrvhook[qrvcard]) reg |= 1;
			if (!wc->qrvhook[qrvcard + 1]) reg |= 0x10;
			wc->sethook[qrvcard] = CMD_WR(3, reg);
			qrv_dosetup(chan,wc);
		}
	}
	/* If we're dead, release us now */
	if (!wc->usecount && wc->dead) 
		wctdm_release(wc);
	return 0;
}

static int wctdm_hooksig(struct zt_chan *chan, zt_txsig_t txsig)
{
	struct wctdm *wc = chan->pvt;
	int reg=0,qrvcard;
	if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_QRV) {
		qrvcard = (chan->chanpos - 1) & 0xfc;
		switch(txsig) {
		case ZT_TXSIG_START:
		case ZT_TXSIG_OFFHOOK:
			wc->qrvhook[chan->chanpos - 1] = 1;
			break;
		case ZT_TXSIG_ONHOOK:
			wc->qrvhook[chan->chanpos - 1] = 0;
			break;
		default:
			printk("wctdm24xxp: Can't set tx state to %d\n", txsig);
		}
		reg = 0;
		if (!wc->qrvhook[qrvcard]) reg |= 1;
		if (!wc->qrvhook[qrvcard + 1]) reg |= 0x10;
		wc->sethook[qrvcard] = CMD_WR(3, reg);
		/* wctdm_setreg(wc, qrvcard, 3, reg); */
	} else if (wc->modtype[chan->chanpos - 1] == MOD_TYPE_FXO) {
		switch(txsig) {
		case ZT_TXSIG_START:
		case ZT_TXSIG_OFFHOOK:
			wc->mods[chan->chanpos - 1].fxo.offhook = 1;
			wc->sethook[chan->chanpos - 1] = CMD_WR(5, 0x9);
			/* wctdm_setreg(wc, chan->chanpos - 1, 5, 0x9); */
			break;
		case ZT_TXSIG_ONHOOK:
			wc->mods[chan->chanpos - 1].fxo.offhook = 0;
			wc->sethook[chan->chanpos - 1] = CMD_WR(5, 0x8);
			/* wctdm_setreg(wc, chan->chanpos - 1, 5, 0x8); */
			break;
		default:
			printk("wctdm24xxp: Can't set tx state to %d\n", txsig);
		}
	} else {
		switch(txsig) {
		case ZT_TXSIG_ONHOOK:
			switch(chan->sig) {
			case ZT_SIG_EM:
			case ZT_SIG_FXOKS:
			case ZT_SIG_FXOLS:
				wc->mods[chan->chanpos - 1].fxs.lasttxhook = wc->mods[chan->chanpos - 1].fxs.idletxhookstate;
				break;
			case ZT_SIG_FXOGS:
				wc->mods[chan->chanpos - 1].fxs.lasttxhook = 3;
				break;
			}
			break;
		case ZT_TXSIG_OFFHOOK:
			switch(chan->sig) {
			case ZT_SIG_EM:
				wc->mods[chan->chanpos - 1].fxs.lasttxhook = 5;
				break;
			default:
				wc->mods[chan->chanpos - 1].fxs.lasttxhook = wc->mods[chan->chanpos - 1].fxs.idletxhookstate;
				break;
			}
			break;
		case ZT_TXSIG_START:
			wc->mods[chan->chanpos - 1].fxs.lasttxhook = 4;
			break;
		case ZT_TXSIG_KEWL:
			wc->mods[chan->chanpos - 1].fxs.lasttxhook = 0;
			break;
		default:
			printk("wctdm24xxp: Can't set tx state to %d\n", txsig);
		}
		if (debug)
			printk("Setting FXS hook state to %d (%02x)\n", txsig, reg);

		
		wc->sethook[chan->chanpos - 1] = CMD_WR(64, wc->mods[chan->chanpos - 1].fxs.lasttxhook);
		/* wctdm_setreg(wc, chan->chanpos - 1, 64, wc->mods[chan->chanpos - 1].fxs.lasttxhook); */
	}
	return 0;
}

static void wctdm_dacs_connect(struct wctdm *wc, int srccard, int dstcard)
{

	if (wc->dacssrc[dstcard] > - 1) {
		printk("wctdm_dacs_connect: Can't have double sourcing yet!\n");
		return;
	}
	if (!((wc->modtype[srccard] == MOD_TYPE_FXS)||(wc->modtype[srccard] == MOD_TYPE_FXO))){
		printk("wctdm_dacs_connect: Unsupported modtype for card %d\n", srccard);
		return;
	}
	if (!((wc->modtype[dstcard] == MOD_TYPE_FXS)||(wc->modtype[dstcard] == MOD_TYPE_FXO))){
		printk("wctdm_dacs_connect: Unsupported modtype for card %d\n", dstcard);
		return;
	}
	if (debug)
		printk("connect %d => %d\n", srccard, dstcard);
	wc->dacssrc[dstcard] = srccard;

	/* make srccard transmit to srccard+24 on the TDM bus */
	if (wc->modtype[srccard] == MOD_TYPE_FXS) {
		/* proslic */
		wctdm_setreg(wc, srccard, PCM_XMIT_START_COUNT_LSB, ((srccard+24) * 8) & 0xff); 
		wctdm_setreg(wc, srccard, PCM_XMIT_START_COUNT_MSB, ((srccard+24) * 8) >> 8);
	} else if(wc->modtype[srccard] == MOD_TYPE_FXO) { 
		/* daa */
		wctdm_setreg(wc, srccard, 34, ((srccard+24) * 8) & 0xff); /* TX */
		wctdm_setreg(wc, srccard, 35, ((srccard+24) * 8) >> 8);   /* TX */
	}

	/* have dstcard receive from srccard+24 on the TDM bus */
	if (wc->modtype[dstcard] == MOD_TYPE_FXS) {
		/* proslic */
    	wctdm_setreg(wc, dstcard, PCM_RCV_START_COUNT_LSB,  ((srccard+24) * 8) & 0xff);
		wctdm_setreg(wc, dstcard, PCM_RCV_START_COUNT_MSB,  ((srccard+24) * 8) >> 8);
	} else if(wc->modtype[dstcard] == MOD_TYPE_FXO) {
		/* daa */
		wctdm_setreg(wc, dstcard, 36, ((srccard+24) * 8) & 0xff); /* RX */
		wctdm_setreg(wc, dstcard, 37, ((srccard+24) * 8) >> 8);   /* RX */
	}

}

static void wctdm_dacs_disconnect(struct wctdm *wc, int card)
{
	if (wc->dacssrc[card] > -1) {
		if (debug)
			printk("wctdm_dacs_disconnect: restoring TX for %d and RX for %d\n",wc->dacssrc[card], card);

		/* restore TX (source card) */
		if(wc->modtype[wc->dacssrc[card]] == MOD_TYPE_FXS){
			wctdm_setreg(wc, wc->dacssrc[card], PCM_XMIT_START_COUNT_LSB, (wc->dacssrc[card] * 8) & 0xff);
			wctdm_setreg(wc, wc->dacssrc[card], PCM_XMIT_START_COUNT_MSB, (wc->dacssrc[card] * 8) >> 8);
		} else if(wc->modtype[wc->dacssrc[card]] == MOD_TYPE_FXO){
			wctdm_setreg(wc, card, 34, (card * 8) & 0xff);
			wctdm_setreg(wc, card, 35, (card * 8) >> 8);
		} else {
			printk("WARNING: wctdm_dacs_disconnect() called on unsupported modtype\n");
		}

		/* restore RX (this card) */
		if(wc->modtype[card] == MOD_TYPE_FXS){
	   		wctdm_setreg(wc, card, PCM_RCV_START_COUNT_LSB, (card * 8) & 0xff);
	    	wctdm_setreg(wc, card, PCM_RCV_START_COUNT_MSB, (card * 8) >> 8);
		} else if(wc->modtype[card] == MOD_TYPE_FXO){
			wctdm_setreg(wc, card, 36, (card * 8) & 0xff);
			wctdm_setreg(wc, card, 37, (card * 8) >> 8);
		} else {
			printk("WARNING: wctdm_dacs_disconnect() called on unsupported modtype\n");
		}

		wc->dacssrc[card] = -1;
	}
}

static int wctdm_dacs(struct zt_chan *dst, struct zt_chan *src)
{
	struct wctdm *wc;

	if(!nativebridge)
		return 0; /* should this return -1 since unsuccessful? */

	wc = dst->pvt;

	if(src) {
		wctdm_dacs_connect(wc, src->chanpos - 1, dst->chanpos - 1);
		if (debug)
			printk("dacs connecct: %d -> %d!\n\n", src->chanpos, dst->chanpos);
	} else {
		wctdm_dacs_disconnect(wc, dst->chanpos - 1);
		if (debug)
			printk("dacs disconnect: %d!\n", dst->chanpos);
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
	for (x=0;x<wc->cards;x++) {
		sprintf(wc->chans[x].name, "WCTDM/%d/%d", wc->pos, x);
		wc->chans[x].sigcap = ZT_SIG_FXOKS | ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_SF | ZT_SIG_EM | ZT_SIG_CLEAR;
		wc->chans[x].sigcap |= ZT_SIG_FXSKS | ZT_SIG_FXSLS | ZT_SIG_SF | ZT_SIG_CLEAR;
		wc->chans[x].chanpos = x+1;
		wc->chans[x].pvt = wc;
	}
	wc->span.chans = wc->chans;
	wc->span.channels = wc->type;
	wc->span.hooksig = wctdm_hooksig;
	wc->span.open = wctdm_open;
	wc->span.close = wctdm_close;
	wc->span.flags = ZT_FLAG_RBS;
	wc->span.ioctl = wctdm_ioctl;
	wc->span.watchdog = wctdm_watchdog;
	wc->span.dacs= wctdm_dacs;
#ifdef VPM_SUPPORT
	wc->span.echocan = wctdm_echocan;
#endif	
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
	for (x=0;x<wc->cards;x++) {
		if (wc->cardflag & (1 << x)) {
			if (wc->modtype[x] == MOD_TYPE_FXO)
				wc->chans[x].sigcap = ZT_SIG_FXSKS | ZT_SIG_FXSLS | ZT_SIG_SF | ZT_SIG_CLEAR;
			else if (wc->modtype[x] == MOD_TYPE_FXS)
				wc->chans[x].sigcap = ZT_SIG_FXOKS | ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_SF | ZT_SIG_EM | ZT_SIG_CLEAR;
			else if (wc->modtype[x] == MOD_TYPE_QRV)
				wc->chans[x].sigcap = ZT_SIG_SF | ZT_SIG_EM | ZT_SIG_CLEAR;
		}
	}
}

static int wctdm_hardware_init(struct wctdm *wc)
{
	/* Hardware stuff */
	unsigned int reg;
	unsigned long newjiffies;

	/* Initialize descriptors */
	wctdm_init_descriptors(wc);
	
	/* Enable I/O Access */
	pci_read_config_dword(wc->dev, 0x0004, &reg);
	reg |= 0x00000007;
	pci_write_config_dword(wc->dev, 0x0004, reg);
	printk("PCI Config reg is %08x\n", reg);

	wctdm_setctl(wc, 0x0000, 0xfff88001);

	newjiffies = jiffies + HZ/10;
	while(((reg = wctdm_getctl(wc,0x0000)) & 0x00000001) && (newjiffies > jiffies));
	printk("%s: New Reg: %08x!\n", wc->variety, reg);

	
	/* Configure watchdogs, access, etc */
	wctdm_setctl(wc, 0x0030, 0x00080048);
	wctdm_setctl(wc, 0x0078, 0x00000013 /* | (1 << 28) */);

#if 0
	/* XXX Enable loopback XXX */
	reg = wctdm_getctl(wc, 0x0030);
	wctdm_setctl(wc, 0x0030, reg | 0x00000400);

#else
	reg = wctdm_getctl(wc, 0x00fc);
	wctdm_setctl(wc, 0x00fc, (reg & ~0x7) | 0x7);
	wctdm_setsdi(wc, 0x00, 0x0100);
	wctdm_setsdi(wc, 0x16, 0x2100);
	printk("Detected REG0: %08x\n", wctdm_getsdi(wc, 0x00));
	printk("Detected REG1: %08x\n", wctdm_getsdi(wc, 0x01));
	printk("Detected REG2: %08x\n", wctdm_getsdi(wc, 0x02));
	
	reg = wctdm_getctl(wc, 0x00fc);
	printk("(pre) Reg fc is %08x\n", reg);

	wctdm_setctl(wc, 0x00fc, (reg & ~0x7) | 0x4);
	wctdm_setsdi(wc, 0x00, 0x0100); 
	wctdm_setsdi(wc, 0x16, 0x2100);
	reg = wctdm_getctl(wc, 0x00fc);
	printk("(post) Reg fc is %08x\n", reg);
	printk("Detected REG2: %08x\n", wctdm_getsdi(wc, 0x02));
#endif
	printk("wctdm24xxp: reg is %08x\n", wctdm_getctl(wc, 0x0088));

	return 0;
}

static void wctdm_setintmask(struct wctdm *wc, unsigned int intmask)
{
	wc->intmask = intmask;
	wctdm_setctl(wc, 0x0038, intmask);
}

static void wctdm_enable_interrupts(struct wctdm *wc)
{
	/* Enable interrupts */ 
	wctdm_setintmask(wc, 0x00010041);
}

static void wctdm_restart_dma(struct wctdm *wc)
{
}

static void wctdm_start_dma(struct wctdm *wc)
{
	unsigned int reg;
	wmb();
	wctdm_setctl(wc, 0x0020, wc->descripdma);
	wctdm_setctl(wc, 0x0018, wc->descripdma + (16 * ERING_SIZE));
	/* Start receiver/transmitter */
	reg = wctdm_getctl(wc, 0x0030);
	wctdm_setctl(wc, 0x0030, reg | 0x00002002);
	wctdm_setctl(wc, 0x0008, 0x00000000);
	wctdm_setctl(wc, 0x0010, 0x00000000);
	reg = wctdm_getctl(wc, 0x0028);
	wctdm_setctl(wc, 0x0028, reg);

}

static void wctdm_stop_dma(struct wctdm *wc)
{
	/* Disable interrupts and reset */
	unsigned int reg;
	/* Disable interrupts */
	wctdm_setintmask(wc, 0x00000000);
	wctdm_setctl(wc, 0x0084, 0x00000000);
	wctdm_setctl(wc, 0x0048, 0x00000000);
	/* Reset the part to be on the safe side */
	reg = wctdm_getctl(wc, 0x0000);
	reg |= 0x00000001;
	wctdm_setctl(wc, 0x0000, reg);
}

static void wctdm_disable_interrupts(struct wctdm *wc)	
{
	/* Disable interrupts */
	wctdm_setintmask(wc, 0x00000000);
	wctdm_setctl(wc, 0x0084, 0x00000000);
}


#ifdef VPM_SUPPORT
static void wctdm_vpm_set_dtmf_threshold(struct wctdm *wc, unsigned int threshold)
{
	unsigned int x;

	for (x = 0; x < 4; x++) {
		wctdm_vpm_out(wc, x, 0xC4, (threshold >> 8) & 0xFF);
		wctdm_vpm_out(wc, x, 0xC5, (threshold & 0xFF));
	}
	printk("VPM: DTMF threshold set to %d\n", threshold);
}

static void wctdm_vpm_init(struct wctdm *wc)
{
	unsigned char reg;
	unsigned int mask;
	unsigned int ver;
	unsigned char vpmver=0;
	unsigned int i, x, y;

	if (!vpmsupport) {
		printk("VPM: Support Disabled\n");
		wc->vpm = 0;
		return;
	}

	for (x=0;x<NUM_EC;x++) {
		ver = wctdm_vpm_in(wc, x, 0x1a0); /* revision */
		if (debug)
			printk("VPM: Chip %d: ver %02x\n", x, ver);
		if (ver != 0x33) {
			printk("VPM: %s\n", x ? "Inoperable" : "Not Present");
			wc->vpm = 0;
			return;
		}	

		if (!x) {
			vpmver = wctdm_vpm_in(wc, x, 0x1a6) & 0xf;
			printk("VPM Revision: %02x\n", vpmver);
		}


		/* Setup GPIO's */
		for (y=0;y<4;y++) {
			wctdm_vpm_out(wc, x, 0x1a8 + y, 0x00); /* GPIO out */
			if (y == 3)
				wctdm_vpm_out(wc, x, 0x1ac + y, 0x00); /* GPIO dir */
			else
				wctdm_vpm_out(wc, x, 0x1ac + y, 0xff); /* GPIO dir */
			wctdm_vpm_out(wc, x, 0x1b0 + y, 0x00); /* GPIO sel */
		}

		/* Setup TDM path - sets fsync and tdm_clk as inputs */
		reg = wctdm_vpm_in(wc, x, 0x1a3); /* misc_con */
		wctdm_vpm_out(wc, x, 0x1a3, reg & ~2);

		/* Setup Echo length (256 taps) */
		wctdm_vpm_out(wc, x, 0x022, 0);

		/* Setup timeslots */
		if (vpmver == 0x01) {
			wctdm_vpm_out(wc, x, 0x02f, 0x00); 
			wctdm_vpm_out(wc, x, 0x023, 0xff);
			mask = 0x11111111 << x;
		} else {
			wctdm_vpm_out(wc, x, 0x02f, 0x20  | (x << 3)); 
			wctdm_vpm_out(wc, x, 0x023, 0x3f);
			mask = 0x0000003f;
		}

		/* Setup the tdm channel masks for all chips*/
		for (i = 0; i < 4; i++)
			wctdm_vpm_out(wc, x, 0x33 - i, (mask >> (i << 3)) & 0xff);

		/* Setup convergence rate */
		reg = wctdm_vpm_in(wc,x,0x20);
		reg &= 0xE0;
		if (alawoverride) {
			if (!x)
				printk("VPM: A-law mode\n");
			reg |= 0x01;
		} else {
			if (!x)
				printk("VPM: U-law mode\n");
			reg &= ~0x01;
		}
		wctdm_vpm_out(wc,x,0x20,(reg | 0x20));

		/* Initialize echo cans */
		for (i = 0 ; i < MAX_TDM_CHAN; i++) {
			if (mask & (0x00000001 << i))
				wctdm_vpm_out(wc,x,i,0x00);
		}

		for (i=0;i<30;i++) 
			schluffen(&wc->regq);

		/* Put in bypass mode */
		for (i = 0 ; i < MAX_TDM_CHAN ; i++) {
			if (mask & (0x00000001 << i)) {
				wctdm_vpm_out(wc,x,i,0x01);
			}
		}

		/* Enable bypass */
		for (i = 0 ; i < MAX_TDM_CHAN ; i++) {
			if (mask & (0x00000001 << i))
				wctdm_vpm_out(wc,x,0x78 + i,0x01);
		}
      
		/* Enable DTMF detectors (always DTMF detect all spans) */
		for (i = 0; i < 6; i++) {
			if (vpmver == 0x01) 
				wctdm_vpm_out(wc, x, 0x98 + i, 0x40 | (i << 2) | x);
			else
				wctdm_vpm_out(wc, x, 0x98 + i, 0x40 | i);
		}

		for (i = 0xB8; i < 0xC0; i++)
			wctdm_vpm_out(wc, x, i, 0xFF);
		for (i = 0xC0; i < 0xC4; i++)
			wctdm_vpm_out(wc, x, i, 0xff);

	} 
	/* set DTMF detection threshold */
	wctdm_vpm_set_dtmf_threshold(wc, dtmfthreshold);
	
	if (vpmver == 0x01)
		wc->vpm = 2;
	else
		wc->vpm = 1;
}

#endif

static void wctdm_locate_modules(struct wctdm *wc)
{
	int x;
	unsigned long flags;
	printk("Resetting the modules...\n");
	/* Initialize control register */
	wc->ctlreg = 0x00;
	/* Set Reset */
	wctdm_setctl(wc, 0x0048, 0x00000000);
	for (x=0;x<10;x++) 
		schluffen(&wc->regq);
	printk("During Resetting the modules...\n");
	/* Clear reset */
	wctdm_setctl(wc, 0x0048, 0x00010000);
	for (x=0;x<10;x++) 
		schluffen(&wc->regq);
	printk("After resetting the modules...\n");
	/* Switch to caring only about receive interrupts */
	wctdm_setintmask(wc, 0x00010040);
	
	/* Make sure all units go into daisy chain mode */
	spin_lock_irqsave(&wc->reglock, flags);
	wc->span.irqmisses = 0;
	for (x=0;x<wc->cards;x++) 
		wc->modtype[x] = MOD_TYPE_FXSINIT;
#ifdef VPM_SUPPORT
	wc->vpm = -1;
	for (x=wc->cards;x<wc->cards+NUM_EC;x++)
		wc->modtype[x] = MOD_TYPE_VPM;
#endif
	spin_unlock_irqrestore(&wc->reglock, flags);
	/* Wait just a bit */
	for (x=0;x<10;x++) 
		schluffen(&wc->regq);
	spin_lock_irqsave(&wc->reglock, flags);
	for (x=0;x<wc->cards;x++) 
		wc->modtype[x] = MOD_TYPE_FXS;
	spin_unlock_irqrestore(&wc->reglock, flags);

#if 0
	/* XXX */
	cmddesc = 0;
#endif	
	/* Now that all the cards have been reset, we can stop checking them all if there aren't as many */
	spin_lock_irqsave(&wc->reglock, flags);
	wc->cards = wc->type;
	spin_unlock_irqrestore(&wc->reglock, flags);

	/* Reset modules */
	for (x=0;x<wc->cards;x++) {
		int sane=0,ret=0,readi=0;
retry:
		/* Init with Auto Calibration */
		if (!(ret = wctdm_init_proslic(wc, x, 0, 0, sane))) {
			wc->cardflag |= (1 << x);
			if (debug) {
				readi = wctdm_getreg(wc,x,LOOP_I_LIMIT);
				printk("Proslic module %d loop current is %dmA\n",x,
					((readi*3)+20));
			}
			printk("Port %d: Installed -- AUTO FXS/DPO\n", x + 1);
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
					printk("Port %d: Installed -- MANUAL FXS\n",x + 1);
				} else {
					printk("Port %d: FAILED FXS (%s)\n", x + 1, fxshonormode ? fxo_modes[_opermode].name : "FCC");
				} 
			} else if (!(ret = wctdm_init_voicedaa(wc, x, 0, 0, sane))) {
				wc->cardflag |= (1 << x);
				printk("Port %d: Installed -- AUTO FXO (%s mode)\n",x + 1, fxo_modes[_opermode].name);
			} else if (!wctdm_init_qrvdri(wc,x)) {
				wc->cardflag |= 1 << x;
				printk("Port %d: Installed -- QRV DRI card\n",x + 1);
			} else {
				if ((wc->type == 8) && ((x & 0x3) == 1) && !wc->altcs[x]) {
					spin_lock_irqsave(&wc->reglock, flags);
					wc->modtype[x] = MOD_TYPE_FXSINIT;
					wc->altcs[x] = 2;
					spin_unlock_irqrestore(&wc->reglock, flags);
					schluffen(&wc->regq);
					spin_lock_irqsave(&wc->reglock, flags);
					wc->modtype[x] = MOD_TYPE_FXS;
					spin_unlock_irqrestore(&wc->reglock, flags);
					if (debug)
						printk("Trying port %d with alternate chip select\n", x + 1);
					goto retry;
				} else {
					printk("Port %d: Not installed\n", x + 1);
					wc->modtype[x] = MOD_TYPE_NONE;
					wc->cardflag |= (1 << x);
				}
			}
		}
	}
#ifdef VPM_SUPPORT
	wctdm_vpm_init(wc);
	if (wc->vpm) {
		printk("VPM: Present and operational (Rev %c)\n", 'A' + wc->vpm - 1);
		wc->ctlreg |= 0x10;
	}
#endif
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
			ifaces[x] = wc;
			memset(wc, 0, sizeof(struct wctdm));
			spin_lock_init(&wc->reglock);
			wc->curcard = -1;
			wc->cards = NUM_CARDS;
			wc->iobase = pci_resource_start(pdev, 0);
			wc->type = d->ports;
			wc->dev = pdev;
			wc->pos = x;
			wc->variety = d->name;
			for (y=0;y<NUM_CARDS;y++) {
				wc->flags[y] = d->flags;
				wc->dacssrc[y] = -1;
			}
			/* Keep track of whether we need to free the region */
			if (request_region(wc->iobase, 0xff, "wctdm24xxp")) 
				wc->freeregion = 1;

			/* Allocate enough memory for two zt chunks, receive and transmit.  Each sample uses
			   32 bits.  Allocate an extra set just for control too */
			wc->writechunk = pci_alloc_consistent(pdev, PCI_WINDOW_SIZE, &wc->writedma);
			if (!wc->writechunk) {
				printk("wctdm: Unable to allocate DMA-able memory\n");
				if (wc->freeregion)
					release_region(wc->iobase, 0xff);
				return -ENOMEM;
			}

			wc->readchunk = wc->writechunk + SFRAME_SIZE / 2;	/* in doublewords */
			wc->readdma = wc->writedma + SFRAME_SIZE * 2;		/* in bytes */

			wc->descripchunk = wc->readchunk + SFRAME_SIZE / 2;	/* in doublewords */
			wc->descripdma = wc->readdma + SFRAME_SIZE * 2;		/* in bytes */

			/* Initialize Write/Buffers to all blank data */
			memset((void *)wc->writechunk,0x00, SFRAME_SIZE * 2);
			memset((void *)wc->readchunk, 0x00, SFRAME_SIZE * 2);

			init_waitqueue_head(&wc->regq);

			if (wctdm_initialize(wc)) {
				printk("%s: Unable to register span with zaptel\n", wc->variety);
				/* Set Reset Low */
				wctdm_stop_dma(wc);
				/* Free Resources */
				free_irq(pdev->irq, wc);
				if (wc->freeregion)
					release_region(wc->iobase, 0xff);
				pci_free_consistent(pdev, PCI_WINDOW_SIZE, (void *)wc->writechunk, wc->writedma);
				kfree(wc);
				return -EIO;
			}

			/* Enable bus mastering */
			pci_set_master(pdev);

			/* Keep track of which device we are */
			pci_set_drvdata(pdev, wc);

			if (request_irq(pdev->irq, wctdm_interrupt, SA_SHIRQ, wc->variety, wc)) {
				printk("wctdm24xxp: Unable to request IRQ %d\n", pdev->irq);
				if (wc->freeregion)
					release_region(wc->iobase, 0xff);
				pci_free_consistent(pdev, PCI_WINDOW_SIZE, (void *)wc->writechunk, wc->writedma);
				pci_set_drvdata(pdev, NULL);
				kfree(wc);
				return -EIO;
			}


			if (wctdm_hardware_init(wc)) {
				/* Set Reset Low */
				wctdm_stop_dma(wc);
				/* Free Resources */
				free_irq(pdev->irq, wc);
				if (wc->freeregion)
					release_region(wc->iobase, 0xff);
				pci_free_consistent(pdev, PCI_WINDOW_SIZE, (void *)wc->writechunk, wc->writedma);
				pci_set_drvdata(pdev, NULL);
				zt_unregister(&wc->span);
				kfree(wc);
				return -EIO;

			}


			/* Enable interrupts */
			wctdm_enable_interrupts(wc);

			/* Start DMA */
			wctdm_start_dma(wc);
			
			/* Now track down what modules are installed */
			wctdm_locate_modules(wc);

			/* Final initialization */
			wctdm_post_initialize(wc);

			printk("Found a Wildcard TDM: %s (%d modules)\n", wc->variety, wc->type);
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
		release_region(wc->iobase, 0xff);
	kfree(wc);
	printk("Freed a Wildcard\n");
}

static void __devexit wctdm_remove_one(struct pci_dev *pdev)
{
	struct wctdm *wc = pci_get_drvdata(pdev);
	if (wc) {

		/* Stop any DMA */
		wctdm_stop_dma(wc);

		/* In case hardware is still there */
		wctdm_disable_interrupts(wc);
		
		/* Immediately free resources */
		pci_free_consistent(pdev, PCI_WINDOW_SIZE, (void *)wc->writechunk, wc->writedma);
		free_irq(pdev->irq, wc);

		/* Release span, possibly delayed */
		if (!wc->usecount)
			wctdm_release(wc);
		else
			wc->dead = 1;
	}
}

static struct pci_device_id wctdm_pci_tbl[] = {
	{ 0xd161, 0x2400, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &wctdm2400 },
	{ 0xd161, 0x0800, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long) &wctdm800 },
	{ 0 }
};

MODULE_DEVICE_TABLE(pci, wctdm_pci_tbl);

static struct pci_driver wctdm_driver = {
	name: 	"wctdm24xxp",
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

	res = pci_module_init(&wctdm_driver);
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
module_param(robust, int, 0600);
module_param(_opermode, int, 0600);
module_param(opermode, charp, 0600);
module_param(lowpower, int, 0600);
module_param(boostringer, int, 0600);
module_param(fastringer, int, 0600);
module_param(fxshonormode, int, 0600);
module_param(battdebounce, int, 0600);
module_param(battthresh, int, 0600);
module_param(alawoverride, int, 0600);
module_param(nativebridge, int, 0600);
module_param(fxotxgain, int, 0600);
module_param(fxorxgain, int, 0600);
module_param(fxstxgain, int, 0600);
module_param(fxsrxgain, int, 0600);
#ifdef VPM_SUPPORT
module_param(vpmsupport, int, 0600);
module_param(vpmdtmfsupport, int, 0600);
module_param(dtmfthreshold, int, 0600);
#endif
#else
MODULE_PARM(debug, "i");
MODULE_PARM(loopcurrent, "i");
MODULE_PARM(robust, "i");
MODULE_PARM(_opermode, "i");
MODULE_PARM(opermode, "s");
MODULE_PARM(lowpower, "i");
MODULE_PARM(boostringer, "i");
MODULE_PARM(fastringer, "i");
MODULE_PARM(fxshonormode, "i");
MODULE_PARM(battdebounce, "i");
MODULE_PARM(battthresh, "i");
MODULE_PARM(alawoverride, "i");
MODULE_PARM(nativebridge, "i");
MODULE_PARM(fxotxgain, "i");
MODULE_PARM(fxorxgain, "i");
MODULE_PARM(fxstxgain, "i");
MODULE_PARM(fxsrxgain, "i");
#ifdef VPM_SUPPORT
MODULE_PARM(vpmsupport, "i");
MODULE_PARM(vpmdtmfsupport, "i");
MODULE_PARM(dtmfthreshold, "i");
#endif
#endif
MODULE_DESCRIPTION("Wildcard TDM2400P/TDM800P Zaptel Driver");
MODULE_AUTHOR("Mark Spencer <markster@digium.com>");
#if defined(MODULE_ALIAS)
MODULE_ALIAS("wctdm8xxp");
#endif
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

module_init(wctdm_init);
module_exit(wctdm_cleanup);
