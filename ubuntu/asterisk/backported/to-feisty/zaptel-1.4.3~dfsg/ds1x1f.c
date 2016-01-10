//=======================================================================
//            T1/E1 PCI card with failover  DS1x1F card Driver
//
//		     Ver: 2.14   Date: 17.Jan.2007
//
//                      based of work by:
//               Mark Spencer <markster@digium.com>
//            Matthew Fredrickson <creslin@digium.com>
//             William Meadows <wmeadows@digium.com>
//
//                  Copyright (c)2006,2007 TCDG Corp.
//                      All rights reserved.
//			
//			http://www.tc-dg.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
//
//=======================================================================

 
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#ifdef STANDALONE_ZAPATA
#include "zaptel.h"
#else
#include <linux/zaptel.h>
#endif


#define WC_MAX_CARDS		32
#define NUM_REGS	 	0xa9
#define NUM_PCI 		12

#define OFS_FALC		0x0000		// pci address space offset for FALC registers  256 * 32 bit words (only 8 LSB used) 
#define OFS_TDM_WR		0x0800		// pci address space offset for TDM media shift register buffers  8 * 32 bit words   
#define OFS_TDM_RD		0x0900		// pci address space offset for TDM media shift register buffers  8 * 32 bit words   
#define OFS_CTRL		0x1000		// pci address space offset for card control register             1 * 32 bit         
#define OFS_CLK			0x1004		// pci address space offset for card clock / h.100 register       1 * 32 bit         


#define FLAG_STARTED 		0x01
#define FLAG_NMF 		0x02
#define FLAG_SENDINGYELLOW 	0x04

#define MODE_SW			-1		// mode controlled by DIP switch
#define	MODE_T1			0x01		// T1 mode			bit.0 = T1
#define	MODE_E1			0x02		// E1 mode			bit.1 = E1
#define	MODE_UE1		0x82		// unchannelized E1		bit.7 = unchannelized
#define	MODE_J1			0x41		// J1 mode			bit.6 = j1mode


#define DS1_SET_REL		 7		// new MAINT commands for controlling the bypass relay and monitoring, hot-standby
#define DS1_CLR_REL		 8
#define DS1_SET_RXMON		 9
#define DS1_CLR_RXMON		10
#define DS1_SET_TXEN		11
#define DS1_CLR_TXEN		12



//======================================= Control register  bit definitions ================================

#define CTRL_LOFF               0x0000          // All LED off
#define CTRL_LGRN		0x0001		// Green LED control bit
#define CTRL_LRED		0x0002		// Red LED control bit
#define CTRL_REL		0x0004		// bypass relay  L = bypass  H = normal operation
#define CTRL_TEST		0x0008		// relay presence test bit  L = normal operation  H= presence test
#define CTRL_FRST		0x0010		// Falc reset bit  L = normal operation H = Falc Reset
#define CTRL_INTA		0x0100		// Interrupt acknowledge bit  writing a '1' resets IRQ and IRM
#define CTRL_IRQ		0x0100		// Interrupt request bit
#define CTRL_IRM		0x0400		// Interrupt missed bit  = previous interrupt has not yet been serviced when new one was triggered 
#define CTRL_RELI		0x0800		// relay presence bit  H = no relay  L = relay present  (valid only when CTRL_TEST = H)
#define CTRL_SW1		0x1000		// DIP switch #1
#define CTRL_SW2		0x2000		// DIP switch #2
#define CTRL_SW3		0x4000		// DIP switch #3
#define CTRL_SW4		0x8000		// DIP switch #4

//==================================== FALC-56 PEF-2256 register definitions ===============================

#define FR_XFIFO	0x00
#define FR_RFIFO	0x00
#define FR_CMDR		0x02
#define FR_MODE		0x03
#define FR_RAH1		0x04
#define FR_RAH2		0x05
#define FR_RAL1		0x06
#define FR_RAL2		0x07
#define FR_IPC 		0x08
#define FR_CCR1		0x09
#define FR_CCR2		0x0a
#define FR_RTR1		0x0c
#define FR_RTR2		0x0d
#define FR_RTR3		0x0e
#define FR_RTR4		0x0f
#define FR_TTR1		0x10
#define FR_TTR2		0x11
#define FR_TTR3		0x12
#define FR_TTR4		0x13
#define FR_IMR0		0x14
#define FR_IMR1		0x15
#define FR_IMR2		0x16
#define FR_IMR3		0x17
#define FR_IMR4		0x18
#define FR_IMR5		0x19
#define FR_IERR		0x1b
#define FR_FMR0		0x1c
#define FR_FMR1		0x1d
#define FR_FMR2		0x1e
#define FR_LOOP		0x1f
#define FR_FMR4		0x20
#define FR_FMR5		0x21
#define FR_XSW 		0x20
#define FR_XSP 		0x21
#define FR_XC0 		0x22
#define FR_XC1 		0x23
#define FR_RC0 		0x24
#define FR_RC1 		0x25
#define FR_XPM0		0x26
#define FR_XPM1		0x27
#define FR_XPM2		0x28
#define FR_TSWM		0x29
#define FR_IDLE		0x2b
#define FR_XSA4		0x2c
#define FR_XSA5		0x2d
#define FR_XSA6		0x2e
#define FR_XSA7		0x2f
#define FR_XSA8		0x30
#define FR_FMR3		0x31
#define FR_CCB1		0x2f
#define FR_CCB2		0x30
#define FR_CCB3		0x31
#define FR_ICB1		0x32
#define FR_ICB2		0x33
#define FR_ICB3		0x34
#define FR_ICB4		0x35
#define FR_LIM0		0x36
#define FR_LIM1		0x37
#define FR_PCD 		0x38
#define FR_PCR 		0x39
#define FR_LIM2		0x3a
#define FR_LCR1		0x3b
#define FR_LCR2		0x3c
#define FR_LCR3		0x3d
#define FR_SIC1		0x3e
#define FR_SIC2		0x3f
#define FR_SIC3		0x40
#define FR_CMR1		0x44
#define FR_CMR2		0x45
#define FR_GCR 		0x46
#define FR_ESM 		0x47
#define FR_RBD  	0x49
#define FR_VSTR 	0x4a
#define FR_RES  	0x4b
#define FR_FRS0 	0x4c
#define FR_FRS1 	0x4d
#define FR_RSW  	0x4e
#define FR_RSP  	0x4f
#define FR_FECL		0x50
#define FR_FECH		0x51
#define FR_CVCL		0x52
#define FR_CVCH		0x53
#define FR_CEC1L	0x54
#define FR_CEC1H	0x55
#define FR_EBCL 	0x56
#define FR_EBCH		0x57
#define FR_CEC2L	0x58
#define FR_CEC2H	0x59
#define FR_CEC3L	0x5a
#define FR_CEC3H	0x5b
#define FR_RSA4 	0x5c
#define FR_RSA5 	0x5d
#define FR_RSA6 	0x5e
#define FR_RSA7 	0x5f
#define FR_RSA8 	0x60
#define FR_DEC 		0x60
#define FR_RSA6S	0x61
#define FR_RSP1		0x62
#define FR_RSP2		0x63
#define FR_SIS 		0x64
#define FR_RSIS		0x65
#define FR_RBCL		0x66
#define FR_RBCH		0x67
#define FR_ISR0		0x68
#define FR_ISR1		0x69
#define FR_ISR2		0x6a
#define FR_ISR3		0x6b
#define FR_ISR4		0x6c
#define FR_ISR5		0x6d
#define FR_GIS 		0x6e
#define FR_XS1 		0x70
#define FR_XS2 		0x71
#define FR_XS3 		0x72
#define FR_XS4 		0x73
#define FR_XS5 		0x74
#define FR_XS6 		0x75
#define FR_XS7 		0x76
#define FR_XS8 		0x77
#define FR_XS9 		0x78
#define FR_XS10		0x79
#define FR_XS11		0x7a
#define FR_XS12		0x7b
#define FR_XS13		0x7c
#define FR_XS14		0x7d
#define FR_XS15		0x7e
#define FR_XS16		0x7f
#define FR_RS1 		0x70
#define FR_RS2 		0x71
#define FR_RS3 		0x72
#define FR_RS4 		0x73
#define FR_RS5 		0x74
#define FR_RS6 		0x75
#define FR_RS7 		0x76
#define FR_RS8 		0x77
#define FR_RS9 		0x78
#define FR_RS10		0x79
#define FR_RS11		0x7a
#define FR_RS12		0x7b
#define FR_RS13		0x7c
#define FR_RS14		0x7d
#define FR_RS15		0x7e
#define FR_RS16		0x7f
#define FR_PC1 		0x80
#define FR_PC2 		0x81
#define FR_PC3 		0x82
#define FR_PC4 		0x83
#define FR_PC5 		0x84
#define FR_GPC1		0x85
#define FR_PC6 		0x86
#define FR_CMDR2	0x87
#define FR_CMDR3	0x88
#define FR_CMDR4	0x89
#define FR_CCR3		0x8b
#define FR_CCR4		0x8c
#define FR_CCR5		0x8d
#define FR_MODE2	0x8e
#define FR_MODE3	0x8f
#define FR_RBC2		0x90
#define FR_RBC3		0x91
#define FR_GCM1		0x92
#define FR_GCM2		0x93
#define FR_GCM3		0x94
#define FR_GCM4		0x95
#define FR_GCM5		0x96
#define FR_GCM6  	0x97
#define FR_GCM7		0x98
#define FR_GCM8  	0x99
#define FR_SIS2		0x98
#define FR_RSIS2	0x99
#define FR_SIS3		0x9a
#define FR_RSIS3	0x9b
#define FR_XFIFO2	0x9c
#define FR_RFIFO2	0x9c
#define FR_XFIFO3	0x9e
#define FR_RFIFO3	0x9e
#define FR_TSEO 	0xa0
#define FR_TSBS1	0xa1
#define FR_TSBS2	0xa2
#define FR_TSBS3	0xa3
#define FR_TSS2 	0xa4
#define FR_TSS3 	0xa5
#define FR_TPC0 	0xa8
#define FR_WID 		0xec

//========================================================================================================

struct t4_regs {
        unsigned int  pci[NUM_PCI];
        unsigned char regs[NUM_REGS];
};


#define WCT4_GET_REGS   _IOW (ZT_CODE, 60, struct t4_regs)



static int clrtab[] = {0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};

static char name[]  = {"ds1x1f"};

struct t1 {
	struct pci_dev *dev;
	spinlock_t lock;
	int spantype;	
	int spanflags;						// Span flags
	unsigned char txsigs[16];  				// Copy of tx sig registers 
	int num;
	int alarmcount;						// How much red alarm we've seen
	int alarmdebounce;
	char *variety;

	unsigned int intcount;
	int          prescaler;					// interrupt divider

	int usecount;
	int blinktimer;
	int alarmtimer;
	int loopupcnt;
	int loopdowncnt;
	unsigned char     ledtestreg;
	
	void *            cnaddr;				// PCI and driver related parameters
	void *            ioaddr;
	unsigned long     iostart;
	unsigned long     iolen;
	unsigned long     cnstart;
	unsigned long     cnlen;
	struct resource * cres;
        struct resource * ires;

	unsigned char txchunkptr;				// pointer for rd/wr audiosamples
	unsigned char rxchunkptr;

        unsigned char ec_chunk1[32][ZT_CHUNKSIZE];		// echocanceller memory buffers
        unsigned char ec_chunk2[32][ZT_CHUNKSIZE];
	
	struct zt_span span;					// Span
	struct zt_chan chans[32];				// Channels 
};

								
								// module paramter variables
static int debug         =  0;   				// defines debug modes 0,1,2
static int cardmode      = -1;					// forces card into T1, E1, J1, UE1 mode , -1 = use DIP switches
static int alarmdebounce =  0;
static int loopback      =  0;
static int frames	 = -1;					// sets the size of the cards framebuffer 1-8 frames (preferred are 1,2,4,8)
static int extclk        = -1;					// controls the clock synchronization via H.100 bus connector
static int monitor       =  0;					// monitor = 3 puts the card in a Hi-Z mode and enabling it to monitor T1/E1 traffic

static struct t1 *cards[WC_MAX_CARDS];


static inline void start_alarm(struct t1 *wc)
{
	wc->blinktimer = 0;
}


static int ds1_open(struct zt_chan *chan)
{
	struct t1 *wc = chan->pvt;
	
	wc->usecount++;

	try_module_get(THIS_MODULE);

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int led_write_reg(struct t1 *wc, unsigned int val)		// write to the cards 32 bit control register
{
	unsigned int x;

	x = (ioread32(wc->ioaddr + OFS_CTRL) & 0xfc) | (val & 0x03);			// combine LED content with all other register data

	iowrite32(x, wc->ioaddr + OFS_CTRL);						// write back register content

	return x;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int ctrl_set_reg(struct t1 *wc, unsigned int val)		// set bits in control register
{
	unsigned x;

	x = (ioread32(wc->ioaddr + OFS_CTRL) & 0xff) | val;				// OR newcontent with all other register data

	iowrite32(x, wc->ioaddr + OFS_CTRL);						// write back register content

	return x;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int ctrl_clr_reg(struct t1 *wc, unsigned int val)		// clear bits in control register
{
	unsigned x;

	x = (ioread32(wc->ioaddr + OFS_CTRL) & 0xff) & (val ^ 0xff);			// OR newcontent with all other register data

	iowrite32(x, wc->ioaddr + OFS_CTRL);						// write back register content

	return x;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int ctrl_write_reg(struct t1 *wc, unsigned int val)		// write to the cards 32 bit control register
{
	iowrite32(val, wc->ioaddr + OFS_CTRL);						// write back register content

	return val;
}


//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int ctrl_read_reg(struct t1 *wc)					// read control register
{
	return (ioread32(wc->ioaddr + OFS_CTRL));					// return content of control register
}


//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int clk_write_reg(struct t1 *wc, unsigned int val)		// write to the cards 32 bit clock/ h.100 bus register
{
	iowrite32(val, wc->ioaddr + OFS_CLK);						// write back register content

	return val;
}


//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int clk_read_reg(struct t1 *wc)					// read clock / h.100 bus register
{
	return (ioread32(wc->ioaddr + OFS_CLK));					// return content of control register
}


//-------------------------------------------------------------------------------------------------------------------------------------------
static inline unsigned int __t1_framer_in(struct t1 *wc, const unsigned int reg)	// read a Falc framer register
{
	int loc = ((reg & 0x00ff) << 2) + OFS_FALC;					// FALC registers are on doubleword boundaries

	return ((ioread32(wc->ioaddr + loc)) & 0xff);					// return only the 8 lsb
}



static inline unsigned int t1_framer_in(struct t1 *wc, const unsigned int addr)		// spinlock wrapper
{
	unsigned long flags;
	unsigned int  ret;

	spin_lock_irqsave(&wc->lock, flags);

	ret = __t1_framer_in(wc, addr);

	spin_unlock_irqrestore(&wc->lock, flags);

	return ret;
}


static inline void __t1_framer_out(struct t1 *wc, const unsigned int reg, const unsigned int val)	// write to Falc framer register	
{
	int loc = ((reg & 0x00ff) << 2) + OFS_FALC;					// FALC registers are on doubleword boundaries
	int ret;

	iowrite32(val, wc->ioaddr + loc);						// write to register

	ret = (ioread32(wc->ioaddr + loc)) & 0xff;					// for debugging read back register content
	
	if (debug > 1)					
	{
		if (ret == val)								// check if write was successful
			printk("Wrote: %02x to Adr: %02x \n", val, reg);				// and print on the console
		else
			printk("Wrote: %02x to Adr: %02x but read back: %02x !!!\n", val, reg, ret);	// print results on the console
	}
}


static inline void t1_framer_out(struct t1 *wc, const unsigned int addr, const unsigned int value)	// spinlock wrapper
{
	unsigned long flags;

	spin_lock_irqsave(&wc->lock, flags);

	__t1_framer_out(wc, addr, value);

	spin_unlock_irqrestore(&wc->lock, flags);
}

//-------------------------------------------------------------------------------------------------------------------------------------------
static void ds1_release(struct t1 *wc)
{
	zt_unregister(&wc->span);

	kfree(wc);

	printk("Freed a DS1x1F card\n");
}


static int ds1_close(struct zt_chan *chan)
{
	struct t1 *wc = chan->pvt;
	wc->usecount--;
	
	module_put(THIS_MODULE);
	
	if (!wc->usecount) 						// If we're dead, release us now 
		ds1_release(wc);

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//                                                      HARDWARE INTERRUPT CONTROL Enable / Disable

static void ds1_enable_interrupts(struct t1 *wc)				// enable 8 kHz TDM interrupts from the pci card
{
	iowrite32(0x00000001, wc->cnaddr + 0x1ec);				// enable internal interrupt

	ctrl_set_reg(wc, CTRL_INTA);						// clear interrupt status bit

	if (debug) 
		printk("Enabled interrupts!\n");
}

static void ds1_disable_interrupts(struct t1 *wc)				// disable all interrupts from the card
{
	ctrl_set_reg(wc, CTRL_INTA);						// clear interrupt status bit

	iowrite32(0x00000000, wc->cnaddr + 0x1ec);				// disable internal interrupt

	if (debug) 
		printk("Disabled interrupts!\n");
}

//----------------------------------------------------------------------------------------------------------------------------------------
static void __ds1_set_clear(struct t1 *wc)					// SELECT CLEAR CHANNEL MODE - FOR T1 ONLY
{
	int i;
	unsigned short val;
	
	val = 0;
	for (i=0; i<8; i++) 							// go through channel 0-7
	{
		if (wc->span.chans[i   ].flags & ZT_FLAG_CLEAR)			// check if this channel is marked Clear channel 
			val |= clrtab[i];	
	}	
	__t1_framer_out(wc, FR_CCB1, val);					// write register CCB1
	
	val = 0;
	for (i=0; i<8; i++) 							// go through channel 8-15
	{
		if (wc->span.chans[i+ 8].flags & ZT_FLAG_CLEAR)			// check if this channel is marked Clear channel 
			val |= clrtab[i];	
	}	
	__t1_framer_out(wc, FR_CCB2, val);					// write register CCB2
	
	val = 0;
	for (i=0; i<8; i++) 							// go through channel 16-23
	{
		if (wc->span.chans[i+16].flags & ZT_FLAG_CLEAR)			// check if this channel is marked Clear channel 
			val |= clrtab[i];	
	}	
	__t1_framer_out(wc, FR_CCB3, val);					// write register CCB3 
	


}

//----------------------------------------------------------------------------------------------------------------------------------------
static int ds1_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	struct t4_regs regs;
	int x;
	struct t1 *wc;

	wc = chan->pvt;
	
	switch(cmd) 
	{
		case WCT4_GET_REGS:
			for (x=0; x<NUM_PCI; x++)
				regs.pci[x] = (ioread32(wc->ioaddr + (x << 2)));

			for (x=0; x<NUM_REGS; x++)
				regs.regs[x] = t1_framer_in(wc, x);

			if (copy_to_user((struct t4_regs *)data, &regs, sizeof(regs)))
				return -EFAULT;
			break;

		case DS1_SET_REL:								// Turn on failover relay - normal operation
			ctrl_set_reg (wc, CTRL_REL);
			break;

		case DS1_CLR_REL:								// turn off failover relay - bypass
			ctrl_clr_reg (wc, CTRL_REL);
			break;

		case DS1_SET_RXMON:								// Enable receiver Hi-Z monitor mode
			__t1_framer_out(wc, FR_LIM2,__t1_framer_in(wc, FR_LIM2) & 0xFD);	// LIM2: turn off AS1
			break;
			
		case DS1_CLR_RXMON:								// Set receiver to normal 100 Ohm impedance
			__t1_framer_out(wc, FR_LIM2,__t1_framer_in(wc, FR_LIM2) | 0x02);	// LIM2: turn on AS1
			break;

		case DS1_SET_TXEN:								// Enable transmitter - normal operation	
			__t1_framer_out(wc, FR_LIM2,__t1_framer_in(wc, FR_XPM2) & 0xBF);	// XPM2: turn on line drivers
			break;
			
		case DS1_CLR_TXEN:								// Disable transmitter - set to Hi-Z
			__t1_framer_out(wc, FR_LIM2,__t1_framer_in(wc, FR_XPM2) | 0x40);	// XPM2: turn off line drivers
			break;

			
			
		default:
			return -ENOTTY;
	}
	return 0;
}


static int ds1_maint(struct zt_span *span, int cmd)
{
	struct t1 *wc = span->pvt;

	if (wc->spantype == MODE_E1) 
	{
		switch(cmd) 
		{
			case ZT_MAINT_NONE:
				printk("XXX Turn off local and remote loops E1 XXX\n");
				break;

			case ZT_MAINT_LOCALLOOP:
				printk("XXX Turn on local loopback E1 XXX\n");
				break;

			case ZT_MAINT_REMOTELOOP:
				printk("XXX Turn on remote loopback E1 XXX\n");
				break;

			case ZT_MAINT_LOOPUP:
				printk("XXX Send loopup code E1 XXX\n");
				break;

			case ZT_MAINT_LOOPDOWN:
				printk("XXX Send loopdown code E1 XXX\n");
				break;

			case ZT_MAINT_LOOPSTOP:
				printk("XXX Stop sending loop codes E1 XXX\n");
				break;

			case DS1_SET_REL:
				ctrl_set_reg (wc, CTRL_REL);
				printk("XXX Turn off E1 bypass XXX\n");
				break;

			case DS1_CLR_REL:
				ctrl_clr_reg (wc, CTRL_REL);
				printk("XXX Turn on E1 bypass XXX\n");
				break;

			default:
				printk("DS1x1F: Unknown E1 maintainance command: %d\n", cmd);
				break;
		}
	} 
	else 
	{
		switch(cmd) 
		{
		    case ZT_MAINT_NONE:
				printk("XXX Turn off local and remote loops T1 XXX\n");
				break;

		    case ZT_MAINT_LOCALLOOP:
				printk("XXX Turn on local loop and no remote loop XXX\n");
				break;

		    case ZT_MAINT_REMOTELOOP:
				printk("XXX Turn on remote loopup XXX\n");
				break;

		    case ZT_MAINT_LOOPUP:
				t1_framer_out(wc, FR_FMR5, 0x50);	/* FMR5: Nothing but RBS mode */
				break;

		    case ZT_MAINT_LOOPDOWN:
				t1_framer_out(wc, FR_FMR5, 0x60);	/* FMR5: Nothing but RBS mode */
				break;

		    case ZT_MAINT_LOOPSTOP:
				t1_framer_out(wc, FR_FMR5, 0x40);	/* FMR5: Nothing but RBS mode */
				break;

		    case DS1_SET_REL:
				ctrl_set_reg (wc, CTRL_REL);
				printk("XXX Turn off T1 bypass XXX\n");
				break;

		    case DS1_CLR_REL:
				ctrl_clr_reg (wc, CTRL_REL);
				printk("XXX Turn on T1 bypass XXX\n");
				break;

		    default:
				printk("DS1x1F: Unknown T1 maintainance command: %d\n", cmd);
				break;
		   }
	}
	return 0;
}


static int ds1_rbsbits(struct zt_chan *chan, int bits)
{
	u_char m,c;
	int n,b;
	struct t1 *wc = chan->pvt;
	unsigned long flags;
	
	if(debug > 2) 
		printk("Setting CAS signalling bits to: %d for ch: %s\n", bits, chan->name);

	spin_lock_irqsave(&wc->lock, flags);	

	if (wc->spantype == MODE_E1) 				// E1 CAS (R2) signalling
	{ 							
		if (chan->chanpos == 16) 			// channel 16 is the signalling channel even for CAS	
		{
			spin_unlock_irqrestore(&wc->lock, flags);
			return 0;				// so leave it alone 	
		}

		n = chan->chanpos - 1;

		if (chan->chanpos > 15) 			// move everything above channel 16 one down
			n--;

		b  = (n % 15);
		c  = wc->txsigs[b];
		m  = (n / 15) << 2; 				// we have to write one nibble with new bits
		c &= (0xf << m); 				// while keeping the other nibble as is
		c |= (bits & 0xf) << (4 - m); 		

		wc->txsigs[b] = c;

		__t1_framer_out(wc,FR_XS2 + b,c); 		// In E1 mode XS1 is not used for CAS signalling

	} 
	else							// T1 or J1 CAS mode
	{
		if (wc->span.lineconfig & ZT_CONFIG_D4) 	// Old D4 framin selected	
		{
			n  = chan->chanpos - 1;
			b  = (n/4);
			c  = wc->txsigs[b];
			m  = ((3 - (n % 4)) << 1); 		// D4 only uses 2 CAS bits per channel
			c &= ~(0x3 << m); 			// do the nibble thing
			c |= ((bits >> 2) & 0x3) << m; 
			wc->txsigs[b] = c;
		  						// output them to the chip */
			__t1_framer_out(wc,FR_XS1 + b ,c); 	// since there are only 2 bits per channel
			__t1_framer_out(wc,FR_XS6 + b ,c); 	// write them into the CAS registers twice
		} 
		else
		{ 
			if (wc->span.lineconfig & ZT_CONFIG_ESF) // ESF = CAS 4 bits per channel
			{
				n  = chan->chanpos - 1;
				b  = (n/2);
				c  = wc->txsigs[b];
				m  = ((n % 2) << 2); 		// do the nibble thing
				c &= (0xf << m); 		
				c |= (bits & 0xf) << (4 - m); 
				wc->txsigs[b] = c;
								// output them to the chip 
				__t1_framer_out(wc,FR_XS1 + b,c); // write only once 
			} 
		}
	}

	spin_unlock_irqrestore(&wc->lock, flags);

	if (debug > 2)
		printk("Finished setting CAS signalling bits\n");

	return 0;
}


static void __t1_check_sigbits(struct t1 *wc)						// read the current CAS bits from framerchip
{
	int a,i,rxs;

	if (!(wc->span.flags & ZT_FLAG_RUNNING))
		return;

	if (wc->spantype == MODE_E1) 							// E1 CAS mode selected
	{
		for (i = 0; i < 15; i++) 						// do this in 16 steps
		{
			a = __t1_framer_in(wc, FR_RS2 + i);				// RS1 is not used for CAS bits
			rxs = (a & 0xf);					

			if (!(wc->span.chans[i+16].sig & ZT_SIG_CLEAR)) 		// get lower nibble = channel+16
			{
				if (wc->span.chans[i+16].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i+16], rxs);
			}

			rxs = (a >> 4) & 0xf;						// get upper nibble = channel +0

			if (!(wc->span.chans[i].sig & ZT_SIG_CLEAR)) 
			{
				if (wc->span.chans[i].rxsig != rxs)
					zt_rbsbits(&wc->span.chans[i], rxs);
			}
		}
	} 
	else 										// T1 or J1 mode selected
	{
		if (wc->span.lineconfig & ZT_CONFIG_D4) 				// D4 framing selected
		{									// D4 uses only 2 CAS bits per channel
			for (i = 0; i < 24; i+=4) 					// do it in 6 steps
			{
				a = __t1_framer_in(wc, FR_RS1 + (i>>2));
				
				rxs = (a & 0x3) << 2;					// get bits.0 and .1 = channel +3
				if (!(wc->span.chans[i+3].sig & ZT_SIG_CLEAR)) 
				{
					if (wc->span.chans[i+3].rxsig != rxs)
						zt_rbsbits(&wc->span.chans[i+3], rxs);
				}
	
				rxs = (a & 0xc);					// get bits.2 and .3 = channel +2	
				if (!(wc->span.chans[i+2].sig & ZT_SIG_CLEAR)) 
				{
					if (wc->span.chans[i+2].rxsig != rxs)
						zt_rbsbits(&wc->span.chans[i+2], rxs);
				}
	
				rxs = (a >> 2) & 0xc;					// get bits.4 and .5 = channel +1
				if (!(wc->span.chans[i+1].sig & ZT_SIG_CLEAR)) 
				{
					if (wc->span.chans[i+1].rxsig != rxs)
						zt_rbsbits(&wc->span.chans[i+1], rxs);
				}
	
				rxs = (a >> 4) & 0xc;					// get bits.6 and .7 = channel +0
				if (!(wc->span.chans[i].sig & ZT_SIG_CLEAR)) 
				{
					if (wc->span.chans[i].rxsig != rxs)
						zt_rbsbits(&wc->span.chans[i], rxs);
				}
			}
		}
		else 									// ESF framing uses 4 CAS bits per channel
		{
			for (i = 0; i < 24; i+=2) 					// do this in 12 steps 
			{
				a = __t1_framer_in(wc, FR_RS1 + (i>>1));	
		
				rxs = (a & 0xf);					// get lower nibble = channel +1 
				if (!(wc->span.chans[i+1].sig & ZT_SIG_CLEAR)) 
				{
					if (wc->span.chans[i+1].rxsig != rxs)
						zt_rbsbits(&wc->span.chans[i+1], rxs);
				}
		
				rxs = (a >> 4) & 0xf;					// get upper nibble = channel +0
				if (!(wc->span.chans[i].sig & ZT_SIG_CLEAR)) 
				{
					if (wc->span.chans[i].rxsig != rxs)
						zt_rbsbits(&wc->span.chans[i], rxs);
				}
			}
		}
	}	
}


static void t4_serial_setup(struct t1 *wc)
{
	unsigned int RC,XC;

	switch (wc->spantype)
	{
		case MODE_E1:			// E1 mode
        		printk("DS1x1F: Setting global parameters for E1 \n");
			break;

		case MODE_UE1:			// NEW - Unframed E1 mode
        		printk("DS1x1F: Setting global parameters for Unchannelized E1 \n");
			break;

		case MODE_J1:			// J1 mode
        		printk("DS1x1F: Setting global parameters for J1 \n");
			break;

		default:			// All others default to T1
        		printk("DS1x1F: Setting global parameters for T1 \n");
			break;
	}


	RC = 0x000;				// Rt = 4 x 125 ns cycle
	XC = 0x004;				// Xt = 0 x 125 nS cycles

	t1_framer_out(wc, FR_GPC1, 0xe0);	// GPC1: Multiplex mode enabled, FSC is output, active low, RCLK from channel 0
	t1_framer_out(wc, FR_IPC,  0x05);	// IPC: Interrupt push/pull active low 

	t1_framer_out(wc, FR_GCM1, 0x66);	
	t1_framer_out(wc, FR_GCM2, 0x0e);
	t1_framer_out(wc, FR_GCM3, 0x3f);
	t1_framer_out(wc, FR_GCM4, 0x0f);
	t1_framer_out(wc, FR_GCM5, 0x04);
	t1_framer_out(wc, FR_GCM6, 0x3c);
	t1_framer_out(wc, FR_GCM7, 0x9c);
	t1_framer_out(wc, FR_GCM8, 0x90);

	t1_framer_out(wc, FR_GCR,  0x40);		// GCR: Interrupt on Activation/Deactivation of AIX, LOS 
	t1_framer_out(wc, FR_SIC1, 0x82);		// SIC1: 2.048 Mhz clock/bus, double buffer receive / transmit, byte interleaved
	t1_framer_out(wc, FR_SIC2, 0x00); 		// SIC2: No FFS, no center receive eliastic buffer, phase 0
	t1_framer_out(wc, FR_SIC3, 0x08);		// SIC3: Edges for capture tx latches with rising edge / rx changes with falling edge
	t1_framer_out(wc, FR_CMR1, 0x30);		// CMR1: RCLK is at 8.192 Mhz dejittered
	t1_framer_out(wc, FR_CMR2, 0x25);   		// CMR2: sync and clock for tx and rx provided by FALC

	t1_framer_out(wc, FR_XC0,  0x00 | ((XC >> 8) & 0x07));		// XC0: Normal operation of Sa-bits
	t1_framer_out(wc, FR_XC1,  XC & 0xff);				// XC1: tx offset

	t1_framer_out(wc, FR_RC0,  0x00 | ((RC >> 8) & 0x07));		// RC0: rx offset
	t1_framer_out(wc, FR_RC1,  RC & 0xff);				// RC1:

							// ------------- Configure ports ---------------
	t1_framer_out(wc, FR_PC1,  0x10);		// PC1: FMR/SPYX output/input on RPA/XPA
	t1_framer_out(wc, FR_PC2,  0x65);		// PC2: unused
	t1_framer_out(wc, FR_PC3,  0x65);		// PC3: unused
	t1_framer_out(wc, FR_PC4,  0x35);		// PC4: unused
	t1_framer_out(wc, FR_PC5,  0x33);		// PC5: XMFS active low, SCLKR is input, RCLK is output
	t1_framer_out(wc, FR_PC6,  0x01);		// PC6: CLK1 is Tx Clock output, CLK2 is 2.048 Mhz from DCO-R

	t1_framer_out(wc, FR_LCR1, 0x00);		// Clear LCR1

	printk("DS1x1F: Successfully initialized card\n");
}


static void __t1_configure_t1(struct t1 *wc, int lineconfig, int txlevel)
{
	char *mode, *frame, *lcode;
	char as1;
	char xlt;
	
	__t1_framer_out(wc, FR_FMR1, 0xbc);		// FMR1: Mode 1, T1 mode, CRC on for ESF, 2.048 Mhz system data rate, no XAIS

	switch (monitor)				// module parameter monitor 
	{
		case 1:					
			as1 = 0x00;			// Tx = normal / Rx = Hi-Z
			xlt = 0x00;
			break;
			
		case 2:
			as1 = 0x02;			// Tx = Hi-Z  / Rx = 100 Ohm
			xlt = 0x40;
			break;				
			
		case 3:
			as1 = 0x00;			// Tx = Hi-Z  / Rx = Hi-Z
			xlt = 0x40;
			break;
			
		default:				// Tx = normal / Rx = 100 Ohm
			as1 = 0x02;
			xlt = 0x00;
			break;
	}
	
							/* Configure line interface */
	if (lineconfig & ZT_CONFIG_AMI) 
	{
		lcode = "AMI";
		__t1_framer_out(wc, FR_FMR0, 0xa0);
	} 
	else 
	{
		lcode = "B8ZS";
		__t1_framer_out(wc, FR_FMR0, 0xf0);
	}

 	if (!(lineconfig & ZT_CONFIG_D4) && !(lineconfig & ZT_CONFIG_ESF))	// support for F4  4 frame format
	{
		frame = "F4";
		if (loopback)
			__t1_framer_out(wc, FR_FMR2, 0x26);
		else
			__t1_framer_out(wc, FR_FMR2, 0x22);

		if (wc->spantype == MODE_J1)
			__t1_framer_out(wc, FR_FMR4, 0x1d);
		else
			__t1_framer_out(wc, FR_FMR4, 0x0d);
	} 

	if (!(lineconfig & ZT_CONFIG_D4) && (lineconfig & ZT_CONFIG_ESF))	// standard ESF 24 frame format 
	{
		frame  = "ESF";

		if (loopback)
			__t1_framer_out(wc, FR_FMR2, 0xe6);
		else
			__t1_framer_out(wc, FR_FMR2, 0xe2);
	
		if (wc->spantype == MODE_J1)
			__t1_framer_out(wc, FR_FMR4, 0x1e);
		else
			__t1_framer_out(wc, FR_FMR4, 0x0e);
	}

	if ((lineconfig & ZT_CONFIG_D4) && !(lineconfig & ZT_CONFIG_ESF))	// standard D4  12 frame format
	{
		frame = "D4";
		if (loopback)
			__t1_framer_out(wc, FR_FMR2, 0x26);
		else
			__t1_framer_out(wc, FR_FMR2, 0x22);

		if (wc->spantype == MODE_J1)
			__t1_framer_out(wc, FR_FMR4, 0x1c);
		else
			__t1_framer_out(wc, FR_FMR4, 0x0c);
	}

 	if ((lineconfig & ZT_CONFIG_D4) && (lineconfig & ZT_CONFIG_ESF))	// support for SLC96 framing mode used in TR-08 circuits
	{
		frame = "SLC96";
		if (loopback)
			__t1_framer_out(wc, FR_FMR2, 0x26);
		else
			__t1_framer_out(wc, FR_FMR2, 0x22);

		if (wc->spantype == MODE_J1)
			__t1_framer_out(wc, FR_FMR4, 0x1f);
		else
			__t1_framer_out(wc, FR_FMR4, 0x0f);
	} 


	__t1_framer_out(wc, FR_FMR5, 0x40);		// FMR5: Enable RBS mode */

	__t1_framer_out(wc, FR_LIM1, 0xf8);		// LIM1: Clear data in case of LOS, Set receiver threshold (0.5V), No remote loop, no DRS
	__t1_framer_out(wc, FR_LIM0, 0x08);		// LIM0: Enable auto long haul mode, no local loop (must be set after LIM1)

	__t1_framer_out(wc, FR_CMDR, 0x50);		// CMDR: Reset the receiver and transmitter line interface
	__t1_framer_out(wc, FR_CMDR, 0x00);		// CMDR: Reset the receiver and transmitter line interface 

	__t1_framer_out(wc, FR_PCD,  0x0a);		// PCD: LOS after 176 consecutive "zeros" 
	__t1_framer_out(wc, FR_PCR,  0x15);		// PCR: 22 "ones" clear LOS 

	if (wc->spantype == MODE_J1)
	{
		mode = "J1";
		__t1_framer_out(wc, FR_RC0, 0x80); 		// J1 overide
	}
	else
	{
		mode = "T1";
	}
		
	switch (txlevel) 					// Set Tx pulse mask + Rx line build out  
	{
		case 7:
			__t1_framer_out(wc, FR_LIM2, 0xe1 | as1);	// LIM2: LBO=3, RST=50%, LOS1=1
			__t1_framer_out(wc, FR_XPM0, 0x07);		// XPM0 
			__t1_framer_out(wc, FR_XPM1, 0x01);		// XPM1 
			__t1_framer_out(wc, FR_XPM2, 0x00 | xlt);	// XPM2 
			break;
	
		case 6:
			__t1_framer_out(wc, FR_LIM2, 0xa1 | as1);	// LIM2: LBO=2, RST=50%, LOS1=1      
			__t1_framer_out(wc, FR_XPM0, 0x8c);		// XPM0
			__t1_framer_out(wc, FR_XPM1, 0x11);		// XPM1 
			__t1_framer_out(wc, FR_XPM2, 0x01 | xlt);	// XPM2
			break;
	
		case 5:
			__t1_framer_out(wc, FR_LIM2, 0x61 | as1);	// LIM2: LBO=1, RST=50%, LOS1=1
			__t1_framer_out(wc, FR_XPM0, 0x8c);		// XPM0
			__t1_framer_out(wc, FR_XPM1, 0x01);		// XPM1
			__t1_framer_out(wc, FR_XPM2, 0x00 | xlt);	// XPM2
			break;
	
		default:
			__t1_framer_out(wc, FR_LIM2, 0x21 | as1);	// LIM2: LBO=0, RST=50%, LOS1=1
			__t1_framer_out(wc, FR_XPM0, 0xd7);		// XPM0
			__t1_framer_out(wc, FR_XPM1, 0x22);		// XPM1
			__t1_framer_out(wc, FR_XPM2, 0x01 | xlt);	// XPM2
			break;
	}


	printk("DS1x1F: Configured Mode: %s Framing: %s Linecode: %s\n", mode, frame, lcode);
}


static void __t1_configure_e1(struct t1 *wc, int lineconfig)
{
	unsigned int  fmr2, fmr1;
	unsigned int  cas = 0;
	char         *crc4 = "";
	char         *frame, *lcode;
	char	      as1;
	char          xlt;
	
	fmr1 = 0x44; 					/* FMR1: E1 mode, Automatic force resync, PCM30 mode, 2.048 Mhz backplane, no XAIS */
	fmr2 = 0x03; 					/* FMR2: Auto transmit remote alarm, auto loss of multiframe recovery, no payload loopback */

	switch (monitor)				// module parameter monitor 
	{
		case 1:					
			as1 = 0x00;			// Tx = normal / Rx = Hi-Z
			xlt = 0x00;
			break;
			
		case 2:
			as1 = 0x02;			// Tx = Hi-Z  / Rx = 100 Ohm
			xlt = 0x40;
			break;				
			
		case 3:
			as1 = 0x00;			// Tx = Hi-Z  / Rx = Hi-Z
			xlt = 0x40;
			break;
			
		default:				// Tx = normal / Rx = 100 Ohm
			as1 = 0x02;
			xlt = 0x00;
			break;
	}

	
	if (wc->spantype == MODE_UE1)
		fmr2 |= 0x30;

	if (loopback)
		fmr2 |= 0x4;

	if (lineconfig & ZT_CONFIG_CRC4) 
	{
		fmr1 |=   0x08;				/* CRC4 transmit */
		fmr2 |=   0xc0;				/* CRC4 receive */
		crc4  = "/CRC4";
	}

	__t1_framer_out(wc, FR_FMR1, fmr1);
	__t1_framer_out(wc, FR_FMR2, fmr2);


	if (lineconfig & ZT_CONFIG_AMI) 		/* Configure line interface */
	{
		lcode = "AMI";
		__t1_framer_out(wc, FR_FMR0, 0xa0);
	} 
	else 
	{
		lcode = "HDB3";
		__t1_framer_out(wc, FR_FMR0, 0xf0);
	}

	if (lineconfig & ZT_CONFIG_CCS) 
	{
		frame = "CCS";
	} 
	else 
	{
		frame = "CAS";
		cas   =  0x40;
	}

	if (wc->spantype == MODE_UE1)
		__t1_framer_out(wc, FR_LOOP, 0x40);

	__t1_framer_out(wc, FR_LIM1, 0xf0);			// LIM1: Clear data in case of LOS, Set receiver threshold (0.5V), No remote loop, no DRS
	__t1_framer_out(wc, FR_LIM0, 0x08);			// LIM0: Enable auto long haul mode, no local loop (must be after LIM1)

	__t1_framer_out(wc, FR_CMDR, 0x50);			// CMDR: Reset the receiver and transmitter line interface
	__t1_framer_out(wc, FR_CMDR, 0x00);			// CMDR: Reset the receiver and transmitter line interface

								// Condition receive line interface for E1 after reset
	__t1_framer_out(wc, 0xbb, 0x17);
	__t1_framer_out(wc, 0xbc, 0x55);
	__t1_framer_out(wc, 0xbb, 0x97);
	__t1_framer_out(wc, 0xbb, 0x11);
	__t1_framer_out(wc, 0xbc, 0xaa);
	__t1_framer_out(wc, 0xbb, 0x91);
	__t1_framer_out(wc, 0xbb, 0x12);
	__t1_framer_out(wc, 0xbc, 0x55);
	__t1_framer_out(wc, 0xbb, 0x92);
	__t1_framer_out(wc, 0xbb, 0x0c);
	__t1_framer_out(wc, 0xbb, 0x00);
	__t1_framer_out(wc, 0xbb, 0x8c);
	
	__t1_framer_out(wc, FR_LIM2, 0x20 | as1);		// LIM2: 50% peak amplitude is a "1"
	__t1_framer_out(wc, FR_PCD,  0x0a);			// PCD: LOS after 176 consecutive "zeros"
	__t1_framer_out(wc, FR_PCR,  0x15);			// PCR: 22 "ones" clear LOS
	
	__t1_framer_out(wc, FR_XSW,  0x9f);			// XSW: Spare bits all to 1 

	if (wc->spantype == MODE_UE1)
		__t1_framer_out(wc, FR_XSP, 0x3c      );
	else
		__t1_framer_out(wc, FR_XSP, 0x1c | cas);	// XSP: E-bit set when async. AXS auto, XSIF to 1
		
								// Generate pulse mask for E1
	__t1_framer_out(wc, FR_XPM0, 0x54);			// XPM0
	__t1_framer_out(wc, FR_XPM1, 0x02);			// XPM1
	__t1_framer_out(wc, FR_XPM2, 0x00 | xlt);		// XPM2

	if (wc->spantype == MODE_UE1)
		printk("DS1x1F: Configured Mode: E1-Unchannelized Framing: %s Linecode: %s\n", frame, lcode);
	else
		printk("DS1x1F: Configured Mode: E1%s Framing: %s Linecode: %s\n", crc4, frame, lcode);

}
//-----------------------------------------------------------------------------------------------------------------------------------------


static void ds1_framer_start(struct t1 *wc, struct zt_span *span)
{
	int alreadyrunning = wc->span.flags & ZT_FLAG_RUNNING;
	unsigned long flags;

	spin_lock_irqsave(&wc->lock, flags);

	if (wc->spantype & MODE_E1) 
	{ 						// configure in E1/ UE1 mode
		__t1_configure_e1(wc, span->lineconfig);
	} 
	else 
	{ 						// configure in T1 / J1 mode
		__t1_configure_t1(wc, span->lineconfig, span->txlevel);
		__ds1_set_clear(wc);
	}
	
	if (!alreadyrunning) 
		wc->span.flags |= ZT_FLAG_RUNNING;

	spin_unlock_irqrestore(&wc->lock, flags);
}


static int ds1_startup(struct zt_span *span)
{
	struct t1 *wc = span->pvt;

	int alreadyrunning = span->flags & ZT_FLAG_RUNNING;
							// initialize the start value for the entire chunk of last ec buffer 
							// Reset framer with proper parameters and start 
	ds1_framer_start(wc, span);
	printk("Calling startup (flags is %d)\n", span->flags);

 	ctrl_write_reg(wc, CTRL_REL  | CTRL_LRED);	// now enable the relay
 

	if (!alreadyrunning) 
	{						// Only if we're not already going
		ds1_enable_interrupts(wc);
		span->flags |= ZT_FLAG_RUNNING;
	}
	return 0;
}


static int ds1_shutdown(struct zt_span *span)
{
	struct t1 *wc = span->pvt;
	unsigned long flags;

	spin_lock_irqsave(&wc->lock, flags);

	__t1_framer_out(wc, FR_GCR, 0x41);		// GCR: Interrupt on Activation/Deactivation of AIX, LOS

	ds1_disable_interrupts(wc);

 	ctrl_write_reg(wc, CTRL_LRED);			// turn off relay and turn on RED LED only

	span->flags &= ~ZT_FLAG_RUNNING;
	spin_unlock_irqrestore(&wc->lock, flags);
	return 0;
}


static int ds1_chanconfig(struct zt_chan *chan, int sigtype)
{
	struct t1 *wc = chan->pvt;
	unsigned long flags;
	int alreadyrunning = chan->span->flags & ZT_FLAG_RUNNING;

	spin_lock_irqsave(&wc->lock, flags);

	if (alreadyrunning && (wc->spantype != MODE_E1))
		__ds1_set_clear(wc);

	spin_unlock_irqrestore(&wc->lock, flags);
	return 0;
}


static int ds1_spanconfig(struct zt_span *span, struct zt_lineconfig *lc)
{
	span->lineconfig = lc->lineconfig;
	span->txlevel    = lc->lbo;
	span->rxlevel    = 0;
							/* Do we want to SYNC on receive or not */
							/* If already running, apply changes immediately */
	if (span->flags & ZT_FLAG_RUNNING)
		return ds1_startup(span);

	return 0;
}


static int ds1_software_init(struct t1 *wc)
{
	int x;						/* Find position */

	for (x = 0; x < WC_MAX_CARDS; x++) 
	{
		if (!cards[x]) 
		{
			cards[x] = wc;
			break;
		}
	}

	if (x >= WC_MAX_CARDS)
		return -1;

	t4_serial_setup(wc);
	wc->num = x;

	sprintf(wc->span.name, "WCT1/%d"                , wc->num);
	sprintf(wc->span.desc, "%s Card %d", wc->variety, wc->num);

	wc->span.spanconfig = ds1_spanconfig;
	wc->span.chanconfig = ds1_chanconfig;
	wc->span.startup    = ds1_startup;
	wc->span.shutdown   = ds1_shutdown;
	wc->span.rbsbits    = ds1_rbsbits;
	wc->span.maint      = ds1_maint;
	wc->span.open       = ds1_open;
	wc->span.close      = ds1_close;

	switch (wc->spantype)
	{
		case MODE_UE1:
			wc->span.channels = 32;
			break;

		case MODE_E1:
			wc->span.channels = 31;
			break;

		default:
			wc->span.channels = 24;
			break;
	}

	wc->span.chans      = wc->chans;
	wc->span.flags      = ZT_FLAG_RBS;
	wc->span.linecompat = ZT_CONFIG_AMI | ZT_CONFIG_B8ZS | ZT_CONFIG_D4 | ZT_CONFIG_ESF;
	wc->span.ioctl      = ds1_ioctl;
	wc->span.pvt        = wc;

	if (wc->spantype & MODE_E1)
		wc->span.deflaw = ZT_LAW_ALAW;
	else
		wc->span.deflaw = ZT_LAW_MULAW;

	init_waitqueue_head(&wc->span.maintq);

	for (x=0;x<wc->span.channels;x++) 
        {
		sprintf(wc->chans[x].name, "WCT1/%d/%d", wc->num, x + 1);
		wc->chans[x].sigcap = ZT_SIG_EM | ZT_SIG_CLEAR | ZT_SIG_EM_E1 | 
				      ZT_SIG_FXSLS | ZT_SIG_FXSGS | 
				      ZT_SIG_FXSKS | ZT_SIG_FXOLS | ZT_SIG_DACS_RBS |
				      ZT_SIG_FXOGS | ZT_SIG_FXOKS | ZT_SIG_CAS | ZT_SIG_SF;
		wc->chans[x].pvt = wc;
		wc->chans[x].chanpos = x + 1;
	}

	if (zt_register(&wc->span, 0)) 
	{
		printk("Unable to register span with zaptel\n");
		return -1;
	}
	return 0;
}


static inline void __handle_leds(struct t1 *wc)
{
	int oldreg;

	wc->blinktimer++;

	if (wc->blinktimer >= 4000) 
		wc->blinktimer = 0;

	oldreg = wc->ledtestreg;

	if (wc->span.alarms & ZT_ALARM_RED)				// RED alarm active 
	{								// LOS  = loss of signal
		if (wc->blinktimer < 50) 
			wc->ledtestreg = CTRL_LRED;
		else
			wc->ledtestreg = CTRL_LOFF;
	
		if (wc->blinktimer >= 200) 				// very fast flickering
			wc->blinktimer = 0;
					
	} 
	else 
	{
		if (wc->span.alarms & ZT_ALARM_YELLOW) 			// YELLOW ALARM = RED LED flashing
		{							// other side can't see our signal
			if (wc->blinktimer < 250) 
				wc->ledtestreg = CTRL_LRED;
			else
				wc->ledtestreg = CTRL_LOFF;
	
			if (wc->blinktimer >= 500) 			// flashing
				wc->blinktimer = 0;
		} 
		else								
		{
			if (wc->span.alarms & ZT_ALARM_BLUE)		// BLUE ALARM 
			{						// T1 failure in front of a repeater down the line
				if (wc->blinktimer < 250) 
					wc->ledtestreg = CTRL_LGRN;	// GREEN LED flashing
				else
					wc->ledtestreg = CTRL_LOFF;
	
				if (wc->blinktimer >= 500) 		// flashing
					wc->blinktimer = 0;
			} 
			else	
			{						// NO ALARM
				if (wc->blinktimer < 150)		// slow winking green led heartbeat
					wc->ledtestreg = CTRL_LOFF;
				else
					wc->ledtestreg = CTRL_LGRN;	// GREEN LED on most of the time
			}
		}
	}

	if (oldreg != wc->ledtestreg)					// only when some bits have changed
		led_write_reg(wc, wc->ledtestreg);			// write LED bits 0,1
}


//====================================== SEND MEDIA DATA =====================================
static void ds1_media(struct t1 *wc)
{
        int x,y,ty,ry,shift;
        unsigned int tx,rx;
	char frm;

	frm = (clk_read_reg(wc) & 0x07) + 1;					 // see how many frames are to be read by card

        ty = wc->txchunkptr;
        ry = wc->rxchunkptr;


	for (y = 0; y < frm; y++)
	{
		int reg = y * 32;

	        if (ty >= ZT_CHUNKSIZE)
	        {
	                ty = 0;
	                zt_transmit(&wc->span);
	        }

	        rx = ioread32(wc->ioaddr + OFS_TDM_RD + 0 + reg);			// read in first 4 channels	
	        tx = 0xff;								// FALC-56 uses TS0 for sync purposes only

	        for (x=1; x<=wc->span.channels; x++)
	        {
	                if ((x % 4) == 0)
	                {
	                        iowrite32(tx, wc->ioaddr + OFS_TDM_WR + x - 4 + reg);   // write 4 bytes (channels)  of pcm data
	                        tx = 0;
	                        rx = ioread32(wc->ioaddr + OFS_TDM_RD + x     + reg);
        	        }

	                shift = ((x % 4) << 3);
	                tx |= (wc->chans[x-1].writechunk[ty] & 0xff) << shift;
	                       wc->chans[x-1].readchunk [ry] =  (rx  >> shift) & 0xff;
	        }

	        iowrite32(tx, wc->ioaddr + OFS_TDM_WR + (x & 0xfc) + reg);             // write 4 bytes (channels)  of pcm data

		ty++;
	        ry++;

	        if (ry >= ZT_CHUNKSIZE)
	        {
	                ry = 0;

		        for (x=0; x<wc->span.channels; x++)				// handle echocanceller memory fill
		        {
		              zt_ec_chunk(&wc->chans[x], wc->chans[x].readchunk,  wc->ec_chunk2[x]);

		              memcpy(wc->ec_chunk2[x],   wc->ec_chunk1[x]       , ZT_CHUNKSIZE);
		              memcpy(wc->ec_chunk1[x],   wc->chans[x].writechunk, ZT_CHUNKSIZE);
		        }

	                zt_receive(&wc->span);
	        }
	}
        
	wc->txchunkptr = ty;
	wc->rxchunkptr = ry;
}



//===============================================================================================================================================================================


static void __t1_check_alarms(struct t1 *wc)
{
	unsigned char c,d;
	int alarms;
	int x,j;

	if (!(wc->span.flags & ZT_FLAG_RUNNING))
		return;

	c = __t1_framer_in(wc, FR_FRS0);
	d = __t1_framer_in(wc, FR_FRS1);

	/* Assume no alarms */
	alarms = 0;

	/* And consider only carrier alarms */
	wc->span.alarms &= (ZT_ALARM_RED | ZT_ALARM_BLUE | ZT_ALARM_NOTOPEN);

	if (wc->spantype & MODE_E1) 
	{
		if (c & 0x04) 
		{				/* No multiframe found, force RAI high after 400ms only if we haven't found a multiframe since last loss of frame */
			if (!(wc->spanflags & FLAG_NMF)) 
			{
				__t1_framer_out(wc, FR_FMR4, 0x9f | 0x20);		/* FMR4: Force RAI High */
				wc->spanflags |= FLAG_NMF;
				printk("NMF workaround on!\n");
			}

			__t1_framer_out(wc, FR_FMR2, 0xc3);			/* Reset to CRC4 mode */
			__t1_framer_out(wc, FR_FMR0, 0xf2);			/* Force Resync */
			__t1_framer_out(wc, FR_FMR0, 0xf0);			/* Force Resync */
		} 
		else 
		{
			if (!(c & 0x02)) 
			{
				if ((wc->spanflags & FLAG_NMF)) 
				{
					__t1_framer_out(wc, FR_FMR4, 0x9f);	/* FMR4: Clear forced RAI */
					wc->spanflags &= ~FLAG_NMF;
					printk("NMF workaround off!\n");
				}
			}
		}
	} 
	else 
	{									/* Detect loopup code if we're not sending one */
		if ((!wc->span.mainttimer) && (d & 0x08)) 
		{
			if ((wc->loopupcnt++ > 80)  && (wc->span.maintstat != ZT_MAINT_REMOTELOOP)) 	// Loop-up code detected 
			{
				__t1_framer_out(wc, FR_LIM0, 0x08);					// LIM0: Disable any local loop
				__t1_framer_out(wc, FR_LIM1, 0xf6);					// LIM1: Enable remote loop

				wc->span.maintstat = ZT_MAINT_REMOTELOOP;				// maintainance status = REMOTELOOP
			}
		} 
		else
			wc->loopupcnt = 0;
		
										/* Same for loopdown code */
		if ((!wc->span.mainttimer) && (d & 0x10)) 
		{								/* Loop-down code detected */
			if ((wc->loopdowncnt++ > 80)  && (wc->span.maintstat == ZT_MAINT_REMOTELOOP)) 
			{
				__t1_framer_out(wc, FR_LIM0, 0x08);		/* LIM0: Disable any local loop */
				__t1_framer_out(wc, FR_LIM1, 0xf0);		/* LIM1: Disable remote loop */
				wc->span.maintstat = ZT_MAINT_NONE;
			}
		} 
		else
			wc->loopdowncnt = 0;
	}

	if (wc->span.lineconfig & ZT_CONFIG_NOTOPEN) 
	{
		for (x=0,j=0;x < wc->span.channels;x++)
		{
			if ((wc->span.chans[x].flags & ZT_FLAG_OPEN) || (wc->span.chans[x].flags & ZT_FLAG_NETDEV))
				j++;
		}

		if (!j)
			alarms |= ZT_ALARM_NOTOPEN;
	}

	if (c & 0xa0) 
	{
		if (wc->alarmcount >= alarmdebounce) 
		{
			if (!(wc->spantype & 0x80))
				alarms |= ZT_ALARM_RED;
		} 
		else
			wc->alarmcount++;
	} 
	else
		wc->alarmcount = 0;

	if (c & 0x4)
		alarms |= ZT_ALARM_BLUE;

										/* Keep track of recovering */
	if ((!alarms) && wc->span.alarms) 
		wc->alarmtimer = ZT_ALARMSETTLE_TIME;

	if (wc->alarmtimer)
		alarms |= ZT_ALARM_RECOVER;

										/* If receiving alarms, go into Yellow alarm state */
	if (alarms && !(wc->spanflags & FLAG_SENDINGYELLOW)) 
	{
		unsigned char fmr4;
		printk("DS1x1F: Setting yellow alarm\n");
										/* We manually do yellow alarm to handle RECOVER and NOTOPEN, otherwise it's auto anyway */
		fmr4 = __t1_framer_in(wc, FR_FMR4);
		__t1_framer_out(wc, FR_FMR4, fmr4 | 0x20);

		wc->spanflags |= FLAG_SENDINGYELLOW;
	} 
	else 
	{
		if ((!alarms) && (wc->spanflags & FLAG_SENDINGYELLOW)) 
		{
			unsigned char fmr4;
			printk("DS1x1F: Clearing yellow alarm\n");
										// We manually do yellow alarm to handle RECOVER 
			fmr4 = __t1_framer_in(wc, FR_FMR4);
			__t1_framer_out(wc, FR_FMR4, fmr4 & ~0x20);

			wc->spanflags &= ~FLAG_SENDINGYELLOW;
		}
	}
										// Re-check the timing source when we enter/leave alarm, not withstanding yellow alarm
	if ((c & 0x10) && !(wc->spantype & 0x80))
		alarms |= ZT_ALARM_YELLOW;

	if (wc->span.mainttimer || wc->span.maintstat) 
		alarms |= ZT_ALARM_LOOPBACK;

	wc->span.alarms = alarms;
	zt_alarm_notify(&wc->span);
}


static void __ds1_do_counters(struct t1 *wc)
{
	if (wc->alarmtimer) 
	{
		if (!--wc->alarmtimer) 
		{
			wc->span.alarms &= ~(ZT_ALARM_RECOVER);
			zt_alarm_notify(&wc->span);
		}
	}
}

///--------------------------------------------------------------------------------------------------------------------------------
static irqreturn_t ds1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct t1 *wc = dev_id;
	unsigned long flags;
	unsigned int x;
	int y;
	
	x = ctrl_read_reg(wc);								// read statusregister

	if (!(x & CTRL_IRQ))
		return IRQ_NONE;

	ctrl_set_reg(wc, CTRL_INTA);							// clear the interrupt flag

	if (!wc->intcount && debug)
		printk("DS1x1F: Got interrupt\n");

	if (x & CTRL_IRM)								// we missed at least one interrupt
		printk("DS1x1F: Missed an Interrupt !!!\n");
	
	ds1_media(wc);									// handle media / audio- buffers

	x = clk_read_reg(wc) & 0x07;							// get the current frame buffer setting

	spin_lock_irqsave(&wc->lock, flags);
	
	y = wc->prescaler - 1;

	if (y < 1)
	{
		y = 8 / (x + 1);

		__handle_leds(wc);
											// Count down timers
		__ds1_do_counters(wc);							

		wc->intcount++;

		x = wc->intcount & 0x0f;								// Do some things that we don't have to do very often

		switch(x) 
		{
			case 0:
			case 1:
				break;
			case 2:
				__t1_check_sigbits(wc);
				break;
			case 4:									// Check alarms 1/4 as frequently
				if (!(wc->intcount & 0x30))
					__t1_check_alarms(wc);
			break;
		}
	}
	wc->prescaler = y;
	
	spin_unlock_irqrestore(&wc->lock, flags);

	return IRQ_RETVAL(1);
}

static int memory_test(struct t1 *wc, int pattern)
{
	int ret = 0;
	int x, y;

	for (x=0; x<31; x++)							// clear out tx tdm memory to FF		
	{
		iowrite32 (pattern, wc->ioaddr + OFS_TDM_WR + (8 * x)    );
		
		y = ioread32(       wc->ioaddr + OFS_TDM_WR + (8 * x)    );

		if (y != pattern)
		{
			ret = 1;
			if (debug)
				printk("DS1x1F: TDM tx memory failure %08x / %08x @ %02x     LSB\n", pattern, y, x);
		}
		else
			if (debug)
				printk("DS1x1F: TDM tx memory OK      %08x / %08x @ %02x     LSB\n", pattern, y, x);
	
		iowrite32 (pattern, wc->ioaddr + OFS_TDM_RD + (8 * x));
		
		y = ioread32(       wc->ioaddr + OFS_TDM_RD + (8 * x));

		if (y != pattern)
		{
			ret = 1;
			if (debug)
				printk("DS1x1F: TDM rx memory failure %08x / %08x @ %02x MSB\n", pattern, y, x);
		}
		else
			if (debug)
				printk("DS1x1F: TDM rx memory OK      %08x / %08x @ %02x MSB\n", pattern, y, x);


	}
	return (ret);
}


//--------------------------------------------------------------------------------------------------------------------------------------
static int ds1_hardware_init(struct t1 *wc)
{
	unsigned int vstr, wid;
	unsigned int x;

	if (frames < 1)								// setting frames = 0 autoadjust to current ZT_CHUNKSIZE
		frames = ZT_CHUNKSIZE;

	if (frames > 8)								// maximum framebuffer size is 8 frames
		frames = 8;
	
	if (extclk == -1)							// extclk not specified by parameter
	{	
		x = ctrl_read_reg(wc);						// read DIP switch
		extclk = (x >> 14) & 0x03;					// SW 3-4 => clock selection	
	}	
	
	switch (extclk)								// external / H.100 bus clocking options
	{
		case 1:								// clock derived from H.100 bus if available
			x = 0x30 | (frames - 1);
			break;

		case 2:								// this card is bus clock master
			x = 0x20 | (frames - 1);
			break;

		default:							// all other cases - internally clocked / no clk output
			x =         frames - 1;		
			break;
	}

	clk_write_reg (wc, x);							// no loopback

	mdelay(10);								// wait for 10 msec. for FDET settle time

	x = clk_read_reg(wc);
	
	switch (x & 0x30)
	{
		case 0x20:
			printk("DS1x1F: Clocking: Busmaster  Framebuffer: %d frames\n",(x & 0x07) + 1);
			break;
	
		case 0x30:
			if (x & 0x00010000)
				printk("DS1x1F: Clocking:  External (   clock present on bus) Framebuffer: %d frames\n",(x & 0x07) + 1);
			else
				printk("DS1x1F: Clocking: (External) no clock present on bus  Framebuffer: %d frames\n",(x & 0x07) + 1);
			break;
	
		default:
			printk("DS1x1F: Clocking: Internal  Framebuffer: %d frames\n",(x & 0x07) + 1);
			break;
	}

	ctrl_write_reg(wc, CTRL_INTA | CTRL_TEST | CTRL_FRST);

	mdelay(10);								// wait for 10 msec.

	x = ctrl_read_reg(wc);							// read in the controlregister

	ctrl_write_reg(wc, CTRL_LRED);

	mdelay(1);								// wait for 1 msec.

	if (cardmode != -1) 
		wc->spantype = cardmode;					// read in spanmode paramter 
	else 
		switch (x & 0x3000)						// check if SW-1 and SW-2
		{
			case 0x0000:
				wc->spantype = MODE_T1;				// OFF - OFF = T1
				break;

			case 0x1000:
				wc->spantype = MODE_J1;				// OFF - ON  = J1
				break;

			case 0x2000:
				wc->spantype = MODE_UE1;			// ON - OFF  = unchannelized E1
				break;

			case 0x3000:
				wc->spantype = MODE_E1;				// ON - ON   =  E1
				break;				
		}

	if (x & CTRL_RELI)
		printk("DS1x1  Board Ver: %01x.%01x SW=%01x-%01x-%01x-%01x  [%x]\n",(x>>28),((x>>20)&0xf),((x>>15)&1),((x>>14)&1),((x>>13)&1),((x>>12)&1),x);	   
	else
		printk("DS1x1F Board Ver: %01x.%01x SW=%01x-%01x-%01x-%01x  [%x]\n",(x>>28),((x>>20)&0xf),((x>>15)&1),((x>>14)&1),((x>>13)&1),((x>>12)&1),x);	  

	vstr = t1_framer_in(wc ,FR_VSTR);					// identify FALC framer chip version
	wid  = t1_framer_in(wc ,FR_WID );

	if (vstr == 0x05)
	{
		if (wid & 0xc0)
			printk("FALC PEF-2256 Ver: 2.2 detected\n");
		else
			printk("FALC PEF-2256 Ver: 2.1 detected\n");
	}
	else
	{
		if ((wid & 0x03) == 0x03)
			printk("!!! WARNING !!! Old FALC PEB-2256 Ver: 1.2 detected\n");
		else
			printk("!!! WARNING !!! FALC Version unknown: VSTR: %02x WID: %02x\n", vstr, wid);
	}

	ds1_disable_interrupts(wc);

	x = 0;

	if (memory_test (wc, 0x5555aaaa)) x = 1;
	if (memory_test (wc, 0x01234567)) x = 1;
	if (memory_test (wc, 0xaaaa5555)) x = 1;
	if (memory_test (wc, 0x76543210)) x = 1;

	if (x)
		printk("DS1x1F: Card TDM memory test failed !!!\n");
	else
		printk("DS1x1F: Card TDM memory test completed.\n");

	ds1_enable_interrupts(wc);

	start_alarm(wc);
	return 0;

}


static int __devinit ds1_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct   t1   *wc;
	unsigned long end;

	
	if (pci_enable_device(pdev)) 
	{ 
		printk (KERN_ALERT "DS1x1F: PCI enable failed\n");
                pci_disable_device (pdev);
		return (-EIO);
	} 
	else 
	{
		wc = kmalloc(sizeof(struct t1), GFP_KERNEL);

		printk("Trying to init an DS1x1F\n");

		if (wc) 
		{
			memset(wc, 0x0, sizeof(struct t1));					// initialize structure with 0x00
			spin_lock_init(&wc->lock);

		        wc->cnstart = pci_resource_start(pdev, 0);                             // read the assigned memory resources for region 0
		        end   	    = pci_resource_end  (pdev, 0);
		        wc->cnlen   = end - wc->cnstart + 1;
		        wc->cres    = request_mem_region(wc->cnstart,wc->cnlen,name);
		        wc->cnaddr  = ioremap_nocache   (wc->cnstart,wc->cnlen     );
	
			if(debug)
				printk (KERN_INFO "Config resources = [0x%lx-0x%lx] (%ld) \n", wc->cnstart, end, wc->cnlen);

		        wc->iostart = pci_resource_start(pdev, 2);                             // read the assigned memory resources for region 2
		        end   	    = pci_resource_end  (pdev, 2);
		        wc->iolen   = end - wc->iostart + 1;
		        wc->ires    = request_mem_region(wc->iostart,wc->iolen,name);
		        wc->ioaddr  = ioremap_nocache   (wc->iostart,wc->iolen     );

			if(debug)
				printk (KERN_INFO "I/O resources    = [0x%lx-0x%lx] (%ld) \n", wc->iostart, end, wc->iolen);

			wc->dev    = pdev;
			
			pci_set_drvdata(pdev, wc);					 	// keep track of our device - i.e. for later removal

                        if (request_irq(pdev->irq, ds1_interrupt, SA_INTERRUPT | SA_SHIRQ, "ds1x1f", wc))
                        {
                                printk("DS1x1F: Unable to request IRQ %d\n", pdev->irq);
                                kfree(wc);
                                return -EIO;
                        }

			ds1_hardware_init(wc);							// Initialize hardware

			wc->variety = "DS1x1F T1/E1-card";					// We now know which version of card we have 
			ds1_software_init(wc);							// Misc. software stuff

			printk("Found an: %s\n", wc->variety);
			return (0);
		} 
		else
		{
			printk (KERN_ALERT "DS1x1F: failed to allocate memory\n");
			return (-ENOMEM);
		}
	}
	return (0);
}


static void __devexit ds1_remove(struct pci_dev *pdev)
{
	struct t1 *wc = pci_get_drvdata(pdev);

	if (wc) 
	{

		ds1_disable_interrupts(wc);			// In case hardware is still there 
		free_irq(pdev->irq, wc);			// release the interrupt resource
		mdelay(10);

		zt_unregister(&wc->span);

		if (debug) 
			printk (KERN_ALERT "DS1x1F: resetting relay / falc / red led on\n");

		ctrl_write_reg(wc, CTRL_FRST | CTRL_LRED);	// reset FALC, relay= off, Red LED = on

		if (wc->ioaddr  != NULL)			// unmap io space
                        iounmap (wc->ioaddr);

		if (wc->cnaddr  != NULL)			// unmap control space
                        iounmap (wc->cnaddr);

		release_mem_region(wc->iostart, wc->iolen);	// release io memory region
		release_mem_region(wc->cnstart, wc->cnlen);	// release control memory region

		kfree(wc);

		printk (KERN_ALERT "DS1x1F: disable pci device\n");
		pci_disable_device (pdev);
	}
	else
		printk (KERN_ALERT "DS1x1F: WARNING Removal Failed !!!\n");

}


static struct pci_device_id  ds1_ids[] = { { PCI_DEVICE( 0x2321, 0x011f) }, { 0, }, };


MODULE_DEVICE_TABLE(pci,ds1_ids);


static struct pci_driver ds1_driver = {
	name: 	  "ds1x1f",
	probe: 	  ds1_probe,
	remove:	  __devexit_p(ds1_remove),
	suspend:  NULL,
	resume:   NULL,
	id_table: ds1_ids,
};


static int __init ds1_init(void)
{
	int res;
	res = zap_pci_module(&ds1_driver);

	if (res)
		return -ENODEV;

	return 0;
}


static void __exit ds1_cleanup(void)
{
	pci_unregister_driver(&ds1_driver);
}



module_param(alarmdebounce,  int, 0600);
module_param(loopback,       int, 0600);
module_param(cardmode,       int, 0600);
module_param(frames,         int, 0600);
module_param(debug,          int, 0600);
module_param(extclk,         int, 0600);
module_param(monitor,        int, 0600);

MODULE_DESCRIPTION("TC-DG ds1x1f Zaptel Driver");
MODULE_AUTHOR("TCDG Corp. <tech@tc-dg.net");
MODULE_LICENSE("GPL");

module_init(ds1_init);
module_exit(ds1_cleanup);

