/*
 * qozap.c - Zaptel driver for the quadBRI PCI ISDN card
 * and the octoBRI PCI ISDN card!
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Junghanns.NET GmbH
 *
 * Klaus-Peter Junghanns <kpj@junghanns.net>
 *
 * This program is free software and may be modified and
 * distributed under the terms of the GNU Public License.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <zaptel.h>
#include "qozap.h"

#ifdef LINUX26
#include <linux/moduleparam.h>
#endif

#if CONFIG_PCI

static int doubleclock=0;
static int ports=-1; /* autodetect */
static int pcmslave=0;
static int bloop=0;
static int debug=0;
static int dacs=1;
static struct qoz_card *qoz_dev_list = NULL;
static int qoz_dev_count = 0;
static int totalBRIs = 0;
static struct pci_dev *multi_qoz = NULL;
static spinlock_t registerlock = SPIN_LOCK_UNLOCKED;

static int ztqoz_shutdown(struct zt_span *span);

int qoz_waitbusy(struct qoz_card *qoztmp) {
    int x=1000;
    while (x-- && (qoz_inb(qoztmp,qoz_R_STATUS) & 1));
    if (x < 0) {
	return -1;
    } else {
	return 0;
    }
}

void qoz_shutdownCard(struct qoz_card *qoztmp) {
    int s=0;
    unsigned long flags;
    
    int stports=0;
    if (qoztmp == NULL) {
	printk(KERN_INFO "qozap: shutting down NULL card!\n");
	return;
    }

    if ((qoztmp->pci_io == NULL) || (qoztmp->ioport == 0)) {
	return;
    }

    if (debug)
	printk(KERN_INFO "qozap: shutting down card %d (cardID %d) at io port %#x.\n",qoztmp->cardno,qoztmp->cardID,(u_int) qoztmp->ioport);


    spin_lock_irqsave(&qoztmp->lock,flags);

    // turn off irqs
    qoz_outb(qoztmp,qoz_R_IRQMSK_MISC, 0); 
    qoz_outb(qoztmp,qoz_R_SCI_MSK, 0); 
    qoz_outb(qoztmp,qoz_R_IRQ_CTRL, 0); 

    spin_unlock_irqrestore(&qoztmp->lock,flags);

    stports = qoztmp->stports;
    for (s=0; s < stports; s++) {
        if(qoztmp->spans[s].flags & ZT_FLAG_REGISTERED) {
    	    zt_unregister(&(qoztmp->spans[s]));
	    if (debug > 2)
	        printk(KERN_INFO "qozap: unregistered card %d span %d.\n",qoztmp->cardno,s+1);
	}
    }


    release_region(qoztmp->ioport, 8);
    iounmap((void *) qoztmp->pci_io);
    release_mem_region((unsigned long) qoztmp->pci_io_phys, 256);

    spin_lock_irqsave(&qoztmp->lock,flags);

    qoztmp->pci_io = NULL;
    qoztmp->ioport = 0;

    if (qoztmp->pcidev != NULL) {
        pci_disable_device(qoztmp->pcidev);
    }
    pci_write_config_word(qoztmp->pcidev, PCI_COMMAND, 0);	
    
    free_irq(qoztmp->irq,qoztmp);
    spin_unlock_irqrestore(&qoztmp->lock,flags);

    
}

void qoz_doLEDs(struct qoz_card *qoztmp) {
    unsigned char leds = 0x0;
    unsigned long flags;
    spin_lock_irqsave(&qoztmp->lock,flags);

    if ((qoztmp->type == 0xb520) && (qoztmp->stports == 4)){
//	if ((qoztmp->ticks > 0) && (qoztmp->ticks <= 300)) {
	    qoz_outb(qoztmp,qoz_R_GPIO_SEL,0x20 | 0x10);
    	    qoz_outb(qoztmp,qoz_R_GPIO_EN1,0xf);
	    qoz_outb(qoztmp,qoz_R_GPIO_OUT1,(qoztmp->leds[0] | (qoztmp->leds[1] << 1) | (qoztmp->leds[2] << 2) | (qoztmp->leds[3] << 3)));
/*	}
	if ((qoztmp->ticks > 300) && (qoztmp->ticks <= 600)) {
    	    qoz_outb(qoztmp,qoz_R_GPIO_EN1,0x0);
	} */
    } else if ((qoztmp->type == 0xb550) && (qoztmp->stports == 4)){
	    qoz_outb(qoztmp,qoz_R_GPIO_SEL,0x80 | 0x40 | 0x20 | 0x10);
    	    qoz_outb(qoztmp,qoz_R_GPIO_EN1,0xff);
	    if (qoztmp->leds[0] == 0) {
		leds |= 0x80;
	    } else {
		leds |= 0x40;
	    }
	    if (qoztmp->leds[1] == 0) {
		leds |= 0x10;
	    } else {
		leds |= 0x20;
	    }
	    if (qoztmp->leds[2] == 0) {
		leds |= 0x04;
	    } else {
		leds |= 0x08;
	    }
	    if (qoztmp->leds[3] == 0) {
		leds |= 0x02;
	    } else {
		leds |= 0x01;
	    }
    /* 	0x80 st1g
	0x40 st1r
	0x20 st2r
	0x10 st2g	
	0x08 st3r
	0x04 st3g
	0x02 st4g
	0x01 st4r
    */
	    qoz_outb(qoztmp,qoz_R_GPIO_OUT1, leds);
    }
    spin_unlock_irqrestore(&qoztmp->lock,flags);
}

void qoz_doWD(struct qoz_card *qoztmp) {
    if (!qoztmp->wdp) {
	return;
    }
    if (qoztmp->wdp == 1) {
	    qoz_outdw_io(qoztmp,0x4000, qoz_WD_P2);
	    qoztmp->wdp = 2;
    } else {
	    qoz_outdw_io(qoztmp,0x4000, qoz_WD_P1);
	    qoztmp->wdp = 1;
    }
    qoz_inb_io(qoztmp,qoz_R_CHIP_ID);    
}

void qoz_undoWD(struct qoz_card *qoztmp) {
    unsigned long flags;

    spin_lock_irqsave(&qoztmp->lock,flags);
    printk(KERN_INFO "qozap: Stopping hardware watchdog.\n");
    qoz_outdw_io(qoztmp,0x4000, qoz_WD_P0);
    qoztmp->wdp = 0;
    qoz_inb_io(qoztmp,qoz_R_CHIP_ID);    
    spin_unlock_irqrestore(&qoztmp->lock,flags);
}

void qoz_resetCard(struct qoz_card *qoztmp) {
    unsigned long flags;
    unsigned int i=0;
    spin_lock_irqsave(&(qoztmp->lock),flags);
    pci_write_config_word(qoztmp->pcidev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY);
    
    // soft reset
    qoz_outb(qoztmp,qoz_R_CIRM,0x8);
    qoz_outb(qoztmp,qoz_R_CIRM,0x0); 
    qoz_waitbusy(qoztmp);

    // fifo reset
    qoz_outb(qoztmp,qoz_R_CIRM,0x10);
    qoz_outb(qoztmp,qoz_R_CIRM,0x0); 
    qoz_waitbusy(qoztmp);

    // s/t reset
    qoz_outb(qoztmp,qoz_R_CIRM,0x40);
    qoz_outb(qoztmp,qoz_R_CIRM,0x0); 
    qoz_waitbusy(qoztmp);

    /* set S0 amplitude */
    qoz_outb(qoztmp,qoz_R_PWM_MD,0xa0);
    if (qoztmp->type == 0xb552) {
	qoz_outb(qoztmp,qoz_R_PWM0,0x19);
    } else {
	qoz_outb(qoztmp,qoz_R_PWM0,0x1E);
    }

    /* set up the timer */
    qoz_outb(qoztmp,qoz_R_TI_WD, 0x2); 
    qoz_outb(qoztmp,qoz_R_IRQMSK_MISC, 0x2); 

    if (pcmslave) {
	qoz_outb(qoztmp,qoz_R_PCM_MD0, 0x0);
    } else {
	qoz_outb(qoztmp,qoz_R_PCM_MD0, 0x1);
    }

    for (i=0; i<256; i++) {
	qoz_outb(qoztmp,qoz_R_SLOT, i);
	qoz_outb(qoztmp,qoz_A_SL_CFG, 0x0);
    }

    /* all state changes */
    qoz_outb(qoztmp,qoz_R_SCI_MSK, 0xff); 

    if (qoztmp->type == 0xb552) {
        qoz_outb(qoztmp,qoz_R_FIFO_MD,0x16);
    } else {
        qoz_outb(qoztmp,qoz_R_FIFO_MD,0x26);
    }

    // double clock
    if (doubleclock == 1) {
	// hopefully you have set CLK_MODE correctly!
	qoz_outb(qoztmp,qoz_R_BRG_PCM_CFG,0x20); 
    } else {
	if (qoztmp->type == 0x08b4) {
	    qoz_outb(qoztmp,qoz_R_BRG_PCM_CFG,0x0); 
	} else if (qoztmp->type == 0xb550) {
	    qoz_outb(qoztmp,qoz_R_BRG_PCM_CFG,0x23); 
	} else if (qoztmp->type == 0xb520) {
	    qoz_outb(qoztmp,qoz_R_BRG_PCM_CFG,0x20); 
	} else {
	    /* you are on your own here! */
	    qoz_outb(qoztmp,qoz_R_BRG_PCM_CFG,0x20); 
	}
    }
    qoz_outb(qoztmp,qoz_R_CTRL,0x0); 

    /* R0 G1 */
    qoztmp->leds[0] = 0x0;
    qoztmp->leds[1] = 0x0;
    qoztmp->leds[2] = 0x0;
    qoztmp->leds[3] = 0x0;
    qoztmp->leds[4] = 0x0;
    qoztmp->leds[5] = 0x0;
    qoztmp->leds[6] = 0x0;
    qoztmp->leds[7] = 0x0;

    /* Finally enable IRQ output */
//    qoz_outb(qoztmp,qoz_R_IRQ_CTRL, 0x8 | 0x1); 
    if (qoztmp->type == 0xb552) {
	qoztmp->stports = 8;
    } else {
	qoztmp->stports = 4;
    }
    qoztmp->ticks = 0;
    qoztmp->clicks = 0;
    if (qoztmp->type == 0xb550) {
	printk(KERN_INFO "qozap: Starting hardware watchdog.\n");
	qoztmp->wdp = 2;
    } else {
	qoztmp->wdp = 0;
    }

    qoz_outb(qoztmp,qoz_R_ST_SYNC,0x0);
    
    spin_unlock_irqrestore(&(qoztmp->lock),flags);
}

void qoz_registerCard(struct qoz_card *qozcard) {
    spin_lock(&registerlock);
    if (qozcard != NULL) {
	qozcard->prev = NULL;
	qozcard->next = qoz_dev_list;
	if (qoz_dev_list) {
	    qoz_dev_list->prev = qozcard;
	}
	qoz_dev_list = qozcard;
	qozcard->cardno = ++qoz_dev_count;
    } else {
	printk(KERN_INFO "qozap: trying to register NULL card.\n");
    }
    spin_unlock(&registerlock);
}

static int qoz_dfifo_tx(struct qoz_card *qoztmp, int stport) {
    int chan = 2;
    int x=0;
    char fifo = 0;
    char offset = 0;
    unsigned long flags;


    if (qoztmp->type == 0xb552) {
	offset = 24;
    } else {
	offset = 28;
    }

    fifo = stport + offset;

    if (qoztmp->chans[stport][chan].bytes2transmit < 1) {
	return 0;
    } else {
	spin_lock_irqsave(&qoztmp->lock,flags);
	/* select fifo */
	qoz_outb(qoztmp,qoz_R_FIFO,fifo << 1);    
	qoz_waitbusy(qoztmp);
    
	if (debug > 1)
	    printk(KERN_INFO "qozap: card %d stport %d TX [ ", qoztmp->cardno, stport + 1);
	/* copy frame to fifo */
    	for (x=0;x<qoztmp->chans[stport][chan].bytes2transmit;x++) {
	    if (debug > 1)
	        printk("%#x ",qoztmp->dtxbuf[stport][x]);
    	    qoz_outb(qoztmp,qoz_A_FIFO_DATA0,qoztmp->dtxbuf[stport][x]);
	}
	if (debug > 1)
	    printk("] %d bytes\n",qoztmp->chans[stport][chan].bytes2transmit);

	if (qoztmp->chans[stport][chan].eoftx == 1) {
	    /* transmit HDLC frame */
    	    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x1);    
    	    qoz_waitbusy(qoztmp);
	}
	spin_unlock_irqrestore(&qoztmp->lock,flags);
    }
    return 0;
}

static int qoz_fifo_tx(struct qoz_card *qoztmp, char fifo) {
    int stport = fifo / 2;
    int chan = fifo % 2;
    unsigned long flags;

    spin_lock_irqsave(&qoztmp->lock,flags);
    /* select fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,0x80 | (fifo << 1));    
    qoz_waitbusy(qoztmp);
    /* transmit 8 bytes of transparent data */
    qoz_outdw(qoztmp,qoz_A_FIFO_DATA0,*((unsigned int *) &qoztmp->txbuf[stport][chan][0]));
    qoz_outdw(qoztmp,qoz_A_FIFO_DATA0,*((unsigned int *) &qoztmp->txbuf[stport][chan][4]));
	    
    spin_unlock_irqrestore(&qoztmp->lock,flags);
    return 0;
}

static int qoz_dfifo_rx(struct qoz_card *qoztmp, int stport) {
    unsigned char f1=1,f2=1,data,stat;
    unsigned char of1=0,of2=0;
    int len,i;
    unsigned short z1=1,z2=1;
    unsigned short oz1=0,oz2=0;
    char fifo = 0;
    char offset = 0;
    unsigned long flags;

    if (qoztmp->type == 0xb552) {
	offset = 24;
    } else {
	offset = 28;
    }

    fifo = stport + offset;

    spin_lock_irqsave(&qoztmp->lock,flags);
    // select rx fifo
    qoz_outb(qoztmp,qoz_R_FIFO,(fifo << 1) | 1);    
    qoz_waitbusy(qoztmp);

    while ((of1 != f1) && (of2 != f2)) {
        of1 = f1;
        of2 = f2;
        f1 = qoz_inb(qoztmp,qoz_A_F1) & 0xf;
        f2 = qoz_inb(qoztmp,qoz_A_F2) & 0xf;
    }
    
    if (f1 == f2) {
	/* no frame */
	qoztmp->st[stport].drx--;
	qoztmp->chans[stport][2].bytes2receive = 0;
	spin_unlock_irqrestore(&qoztmp->lock,flags);
	return 0;
    }
    
    while ((oz1 != z1) && (oz2 != z2)) {
        oz1 = z1;
        oz2 = z2;
	if (qoztmp->type != 0xb552) {
    	    z1 = qoz_inw(qoztmp,qoz_A_Z1) & 0x7ff;
	    z2 = qoz_inw(qoztmp,qoz_A_Z2) & 0x7ff;
	} else {
    	    z1 = qoz_inw(qoztmp,qoz_A_Z1) & 0x3ff;
	    z2 = qoz_inw(qoztmp,qoz_A_Z2) & 0x3ff;
	}
    }
    
    if (qoztmp->type == 0xb552) {
	len = z1 - z2;
	if (len < 0) {
	    len += qoz_DFIFO_SIZE8;
	}
    } else {
	len = z1 - z2;
	if (len < 0) {
	    len += qoz_DFIFO_SIZE4;
	}
    }

    if (len > qoz_DFIFO_SIZE4) {
	printk(KERN_INFO "\nqozap: buffer overflow in D channel RX!\n");
	qoztmp->chans[stport][2].bytes2receive = 0;
	qoztmp->chans[stport][2].eofrx = 0;
    } else {
	if (debug > 1) printk(KERN_INFO "qozap: card %d span %d RX [ ", qoztmp->cardno, stport + 1);
	for (i=0; i<len; i++) {
    	    data = qoz_inb(qoztmp,qoz_A_FIFO_DATA0);
	    qoztmp->drxbuf[stport][i] = data;
	    if (debug > 1) printk("%#x ",data);
	}
	if (debug > 1) printk("] %d bytes\n", len);
	qoztmp->chans[stport][2].bytes2receive = i;
	qoztmp->chans[stport][2].eofrx = 1;
    }

    stat = qoz_inb(qoztmp,qoz_A_FIFO_DATA0);
    if (stat != 0x0) {
	// bad CRC, skip it
	printk(KERN_INFO "qozap: CRC error for HDLC frame on card %d (cardID %d) S/T port %d\n",qoztmp->cardno, qoztmp->cardID, stport+1);
	qoztmp->chans[stport][2].bytes2receive = 0;
	qoztmp->chans[stport][2].eofrx = 0;
//        zt_qevent_nolock(&qoztmp->chans[stport][2], ZT_EVENT_BADFCS);
    }
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x1);    
    qoz_waitbusy(qoztmp);

    /* frame recevived */
    if (qoztmp->st[stport].drx > 0) { 
	qoztmp->st[stport].drx--;
    } else {
	printk(KERN_INFO "qozap: trying to receive too much (card %d span %d drx %d)\n", qoztmp->cardno, stport+1, qoztmp->st[stport].drx);
	qoztmp->st[stport].drx = 0;
    }
    spin_unlock_irqrestore(&qoztmp->lock,flags);
    return 0;
}


static int qoz_fifo_rx(struct qoz_card *qoztmp, char fifo) {
    int stport = fifo / 2;
    int chan = fifo % 2;
    unsigned char data;
    int len,i;
    unsigned short z1=1,z2=1;
    unsigned short oz1=0,oz2=0;
    int mumbojumbo=0;
    unsigned long flags;

    spin_lock_irqsave(&qoztmp->lock,flags);

        /* select rx fifo */
	qoz_outb(qoztmp,qoz_R_FIFO,0x80 | (fifo << 1) | 1);    
        qoz_waitbusy(qoztmp);
    
	while ((oz1 != z1) && (oz2 != z2)) {
	    oz1 = z1;
	    oz2 = z2;
	    z1 = qoz_inw(qoztmp,qoz_A_Z1) & 0x7f;
    	    z2 = qoz_inw(qoztmp,qoz_A_Z2) & 0x7f;
	}
	len = z1 - z2;
	if (len < 0) {
	    len += qoz_FIFO_SIZE;
	}
	if (len > 2 * ZT_CHUNKSIZE) {
	    mumbojumbo = len - (2 * ZT_CHUNKSIZE);
	    len = ZT_CHUNKSIZE;
	    for (i=0;i<mumbojumbo;i++) {
    		data = qoz_inb(qoztmp,qoz_A_FIFO_DATA0);
	    }
	    qoztmp->clicks++;
	    if ((qoztmp->clicks > 50) && (debug > 0)) {
		printk(KERN_CRIT "qozap: dropped audio card %d cardid %d bytes %d z1 %d z2 %d\n", qoztmp->cardno, qoztmp->cardID, mumbojumbo, z1, z2);
		qoztmp->clicks = 0;
	    }
	}


	if (len < ZT_CHUNKSIZE) {
//	    printk(KERN_INFO "qozap: not enough to receive (%d bytes)\n",len);
	    spin_unlock_irqrestore(&qoztmp->lock,flags);
	    return 0;
	} else {
	    if (bloop) {
		*((unsigned int *) &qoztmp->txbuf[stport][chan][0]) = qoz_indw(qoztmp,qoz_A_FIFO_DATA0);
	        *((unsigned int *) &qoztmp->txbuf[stport][chan][4]) = qoz_indw(qoztmp,qoz_A_FIFO_DATA0);
	    } else {
		*((unsigned int *) &qoztmp->rxbuf[stport][chan][0]) = qoz_indw(qoztmp,qoz_A_FIFO_DATA0);
	        *((unsigned int *) &qoztmp->rxbuf[stport][chan][4]) = qoz_indw(qoztmp,qoz_A_FIFO_DATA0);
	    }
	}

    spin_unlock_irqrestore(&qoztmp->lock,flags);
	if (bloop == 0)
	    zt_ec_chunk(&qoztmp->spans[stport].chans[chan], qoztmp->spans[stport].chans[chan].readchunk, qoztmp->spans[stport].chans[chan].writechunk);

//    printk(KERN_INFO "s/t port %d, channel %d, dbufi=%d, f1=%d, f2=%d, z1=%d, z2=%d  => len = %d stat=%#x, hdlc=%d\n",stport,chan,qoztmp->st[stport].dbufi,f1,f2,z1,z2,len,stat,hdlc);    
    return 0;
}

static void qoz_assign(struct qoz_card *qoztmp, int src_span, int src_chan, int dst_span, int dst_chan) {
    unsigned long flags;
    int timeslot = src_span * 2 + (src_chan - 1);
    int dst_fifo = dst_span * 2 + (dst_chan - 1); 
    int src_fifo = src_span * 2 + (src_chan - 1); 
    int src_hfc_chan = src_span * 4 + (src_chan - 1);
    int dst_hfc_chan = dst_span * 4 + (dst_chan - 1);

    spin_lock_irqsave(&qoztmp->lock, flags);

    qoz_outb(qoztmp,qoz_R_FIFO,(src_fifo << 1) | 1); 
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CHANNEL,(src_hfc_chan << 1) | 1); 
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0xDE); // was c2

    qoz_outb(qoztmp,qoz_R_SLOT,timeslot << 1);
    qoz_outb(qoztmp,qoz_A_SL_CFG, (src_hfc_chan << 1) | 0 | 0x40);

    qoz_outb(qoztmp,qoz_R_FIFO, dst_fifo << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CHANNEL,dst_hfc_chan << 1);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0xDE); // was c2
    
    qoz_outb(qoztmp,qoz_R_SLOT,(timeslot << 1) | 1); 
    qoz_outb(qoztmp,qoz_A_SL_CFG, (dst_hfc_chan << 1) | 1 | 0x40);

    spin_unlock_irqrestore(&qoztmp->lock, flags);
}

static void qoz_unassign(struct qoz_card *qoztmp, int span, int chan) {
    unsigned long flags;

    int timeslot = span * 2 + (chan - 1);
    int fifo = span * 2 + (chan - 1);
    int hfc_chan = span * 4 + (chan - 1);

    spin_lock_irqsave(&qoztmp->lock, flags);

    qoz_outb(qoztmp,qoz_R_FIFO,(fifo << 1));
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x02);
    qoz_outb(qoztmp,qoz_A_CHANNEL,(hfc_chan << 1)); 
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    qoz_outb(qoztmp,qoz_R_FIFO,(fifo << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x02);
    qoz_outb(qoztmp,qoz_A_CHANNEL,(hfc_chan << 1) | 1); 
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    qoz_outb(qoztmp,qoz_R_SLOT,(timeslot << 1) | 1);
    qoz_outb(qoztmp,qoz_A_SL_CFG, 0x0);

    qoz_outb(qoztmp,qoz_R_SLOT,timeslot << 1);
    qoz_outb(qoztmp,qoz_A_SL_CFG, 0x0);

    spin_unlock_irqrestore(&qoztmp->lock, flags);
}


static int ztqoz_dacs(struct zt_chan *dst, struct zt_chan *src)
{
	struct qoz_card *qoztmp = NULL;
	if (!dacs) return 0;

	if (src) { 
	    qoztmp = src->pvt;
	}
	if (src && (src->pvt != dst->pvt)) {
		/* cross card bridging */
		if (debug)
		    printk("Not Assigning %d/%d -> %d/%d, different cards!\n", src->span->offset, src->chanpos, dst->span->offset, dst->chanpos);
		return -1;
	}

	if (src) {
		if ((src->chanpos == 3) || (dst->chanpos == 3)) {
		    if (debug)
			printk("Not Assigning D-channel %d/%d -> %d/%d!\n", src->span->offset, src->chanpos, dst->span->offset, dst->chanpos);
		} else {
		    if (debug)
			printk("Assigning channel %d/%d -> %d/%d!\n", src->span->offset, src->chanpos, dst->span->offset, dst->chanpos);
		    qoz_assign(qoztmp, src->span->offset, src->chanpos, dst->span->offset, dst->chanpos);
		}
	} else {
		qoztmp = dst->pvt;
		if (dst->chanpos == 3) {
		    if (debug)
			printk("Not Unassigning D-channel %d/%d!\n", dst->span->offset, dst->chanpos);
		} else {
		    qoz_unassign(qoztmp, dst->span->offset, dst->chanpos);
		    if (debug)
			printk("Unassigning channel %d/%d!\n", dst->span->offset, dst->chanpos);
		}
	}
	return 0;
}

static void qoz_resync(struct qoz_card *qoztmp) {
    int i=0;
    int best=99;
    int src=-1;

    for (i=0; i<qoztmp->stports; i++){
	if (qoztmp->st[i].l1up && (qoztmp->st_sync[i] > 0) && (qoztmp->st_sync[i] < best)) {
	    best = qoztmp->st_sync[i];
	    src = i;
	}
    }
    if (src == qoztmp->syncsrc)
	return;
    if (src >= 0) {
	qoztmp->syncsrc = src;
	qoz_outb(qoztmp, qoz_R_ST_SYNC, src | 0x08);
	if (debug > 2)
	    printk(KERN_INFO "qozap: Sync source changed to span %d\n", src + 1);
    }
}

static inline void qoz_run(struct qoz_card *qoztmp) {
    int s=0;
    unsigned long flags;

    for (s=0;s<qoztmp->stports;s++) {
	if (!bloop) {
	    if (qoztmp->spans[s].flags & ZT_FLAG_RUNNING) {
		/* oh zaptel! tell us what to transmit... */
    		zt_transmit(&qoztmp->spans[s]);
	        /* B1 xmit */
		qoz_fifo_tx(qoztmp, s * 2);
		/* B2 xmit */
		qoz_fifo_tx(qoztmp, (s * 2) + 1);

		if ((qoztmp->st[s].layer1state != 7) && (qoztmp->chans[s][2].bytes2transmit > 0) && (qoztmp->st[s].nt_mode != 1)) {
		    if (qoztmp->st[s].t3 == -1) {
			if (debug > 2)
			    printk(KERN_INFO "qozap: activating layer 1, span %d\n",s);
			qoztmp->st[s].t3 = 0;
			spin_lock_irqsave(&qoztmp->lock,flags);
			    qoz_outb(qoztmp,qoz_R_ST_SEL, s);
			    qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x60); 
			spin_unlock_irqrestore(&qoztmp->lock, flags);
		    } else {
		    }
		}
		
		/* D xmit */
		if (qoztmp->spans[s].alarms != ZT_ALARM_RED) {
		    qoz_dfifo_tx(qoztmp, s);
		} else {
		    if ((qoztmp->st[s].t3 == -1) && (qoztmp->st[s].t4 == -1) && (qoztmp->st[s].layer1state == 3) && (qoztmp->st[s].nt_mode != 1)) {
			/* clear alarms */
			if (debug > 2)
			    printk(KERN_INFO "qozap: clearing alarms on span %d\n",s);
			qoztmp->spans[s].alarms = ZT_ALARM_NONE;
			zt_alarm_notify_no_master_change(&qoztmp->spans[s]);
		    }
		}

		qoztmp->chans[s][2].bytes2receive = 0;
		qoztmp->chans[s][2].bytes2transmit = 0;
		qoztmp->chans[s][2].eofrx = 0;
		qoztmp->chans[s][2].eoftx = 0;

	    }

	    /* B1 receive */
	    qoz_fifo_rx(qoztmp,(s*2));
	    /* B2 receive */
	    qoz_fifo_rx(qoztmp,(s*2)+1);
	    /* d-chan data */
	    if (qoztmp->st[s].drx > 0) {
	        if (debug > 2)
		    printk(KERN_CRIT "qozap: card %d st[%d].drx = %d\n", qoztmp->cardno, s, qoztmp->st[s].drx);
		qoz_dfifo_rx(qoztmp, s);
	    }
	    if (qoztmp->spans[s].flags & ZT_FLAG_RUNNING) {
	        /* oh zaptel! thou shall receive! */
		zt_receive(&(qoztmp->spans[s]));
	    }
	} else {
	    // loop
	    /* B1 receive */
	    qoz_fifo_rx(qoztmp,(s*2));
	    /* B2 receive */
	    qoz_fifo_rx(qoztmp,(s*2)+1);
	    /* d-chan data */
/*	    if (qoztmp->st[s].drx > 0) {
	        if (debug > 2)
		    printk(KERN_CRIT "qozap: card %d st[%d].drx = %d\n", qoztmp->cardno, s, qoztmp->st[s].drx);
		qoz_dfifo_rx(qoztmp, s);
	    } 
	    if (qoztmp->spans[s].flags & ZT_FLAG_RUNNING) {
		zt_receive(&(qoztmp->spans[s]));
	    }
*/
	    if (qoztmp->spans[s].flags & ZT_FLAG_RUNNING) {
		/* oh zaptel! tell us what to transmit... */
    	//	zt_transmit(&qoztmp->spans[s]);
	        /* B1 xmit */
		qoz_fifo_tx(qoztmp, s * 2);
		/* B2 xmit */
		qoz_fifo_tx(qoztmp, (s * 2) + 1);
		/* D xmit */
//		qoz_dfifo_tx(qoztmp, s);

		qoztmp->chans[s][2].bytes2receive = 0;
		qoztmp->chans[s][2].bytes2transmit = 0;
		qoztmp->chans[s][2].eofrx = 0;
		qoztmp->chans[s][2].eoftx = 0;

	    }
	}
    } 
}

#ifdef LINUX26
static irqreturn_t qoz_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#else
static void qoz_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#endif
    struct qoz_card *qoztmp = dev_id;
    unsigned long flags;
    unsigned char irq_misc,irq_sci,status,l1state,irq_foview,fi;
    int st=0,i=0,offset=0;
    int j=0;

    if (!qoztmp) {
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }

    if ((!qoztmp->pci_io) || (!qoztmp->ioport)) {
	    printk(KERN_CRIT "qozap: no pci mem/io\n");
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }
    if (qoztmp->dead) {
#ifdef LINUX26
	return IRQ_RETVAL(1);
#else
	return;
#endif		
    }

    
    spin_lock_irqsave(&(qoztmp->lock), flags);
	status = qoz_inb(qoztmp,qoz_R_STATUS);
	irq_sci = qoz_inb(qoztmp,qoz_R_SCI);
    spin_unlock_irqrestore(&(qoztmp->lock), flags);

    if (!(status & 0x80) && !(status & 0x40) && (irq_sci == 0)) {
//	printk(KERN_CRIT "qozap: status %#x\n", status);
	// it's not us!
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }
    /* state machine irq */
    if (irq_sci != 0) {
	if (debug > 1) {
	    printk(KERN_INFO "R_BERT_STA = %#x\n", qoz_inb(qoztmp, qoz_R_BERT_STA) & 7);
	}
	if (qoztmp->type == 0xb552) {
	    offset = 24;
        } else {
	    offset = 28;
	}
	spin_lock_irqsave(&(qoztmp->lock), flags);
	for (st=0;st<qoztmp->stports;st++) {
	    if (irq_sci & (1 << st)) {
		qoz_outb(qoztmp,qoz_R_ST_SEL,st);
		l1state = qoz_inb(qoztmp,qoz_A_ST_RD_STA) & 0xf;
		if (debug > 1) {
		    printk(KERN_INFO "A_ST_RD_STA = %#x\n", qoz_inb(qoztmp, qoz_A_ST_RD_STA));
		}
		qoztmp->st[st].layer1state = l1state;
		if (qoztmp->st[st].nt_mode == 1) {
		    if (debug)
			printk(KERN_INFO "card %d span %d state G%d (A_ST_RD_STA = %#x)\n",qoztmp->cardno,st+1,l1state,qoz_inb(qoztmp,qoz_A_ST_RD_STA));
		    // NT state machine
		    if (l1state == 3) {
			qoztmp->st[st].l1up = 1;
			// keep layer1 up!
			if (qoztmp->stports == 8) {
			    sprintf(qoztmp->spans[st].desc,"octoBRI PCI ISDN Card %d Span %d [NT] Layer 1 ACTIVATED (G%d)",qoztmp->cardno ,st + 1, l1state);
			} else {
			    sprintf(qoztmp->spans[st].desc,"quadBRI PCI ISDN Card %d Span %d [NT] (cardID %d) Layer 1 ACTIVATED (G%d)",qoztmp->cardno ,st + 1,qoztmp->cardID, l1state);
			}
			qoz_outb(qoztmp,qoz_A_ST_WR_STA,3 | 0x10 );
			qoztmp->leds[st] = 1;
		    } else {
			qoztmp->st[st].l1up = 0;
			if (qoztmp->stports == 8) {
			    sprintf(qoztmp->spans[st].desc,"octoBRI PCI ISDN Card %d Span %d [NT] Layer 1 DEACTIVATED (G%d)",qoztmp->cardno ,st + 1, l1state);
			} else {
			    sprintf(qoztmp->spans[st].desc,"quadBRI PCI ISDN Card %d Span %d [NT] (cardID %d) Layer 1 DEACTIVATED (G%d)",qoztmp->cardno ,st + 1,qoztmp->cardID, l1state);
			}
			qoztmp->leds[st] = 0;
		    }
		} else {
		    if (debug)
			printk(KERN_INFO "card %d span %d state F%d (A_ST_RD_STA = %#x)\n",qoztmp->cardno,st+1,l1state,qoz_inb(qoztmp,qoz_A_ST_RD_STA));
		    // TE state machine
		    if (l1state == 3) {
			qoztmp->st[st].l1up = 0;
			if (qoztmp->st[st].t3 > -1)  {
			    /* keep layer1 up, if the span is started. */
			    if (qoztmp->spans[st].flags & ZT_FLAG_RUNNING) {
				if (debug > 2) 
				    printk("qozap: re-activating layer1 span %d\n", st);
    				qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x60);
			    }
			} else {
				if (debug > 2) 
				    printk("qozap: not re-activating layer1 span %d\n", st);
    				qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x40);
			    /* if we tried to activate layer 1 and it failed make this an alarm */
//			    qoztmp->spans[st].alarms = ZT_ALARM_RED;
//			    zt_alarm_notify(&qoztmp->spans[st]);
			    /* if the network shuts us down in idle mode dont make this an alarm */
			}
			qoztmp->leds[st] = 0;
			if (qoztmp->stports == 8) {
			    sprintf(qoztmp->spans[st].desc,"octoBRI PCI ISDN Card %d Span %d [TE] Layer 1 DEACTIVATED (F%d)",qoztmp->cardno ,st + 1, l1state);
			} else {
			    sprintf(qoztmp->spans[st].desc,"quadBRI PCI ISDN Card %d Span %d [TE] (cardID %d) Layer 1 DEACTIVATED (F%d)",qoztmp->cardno ,st + 1,qoztmp->cardID, l1state);
			}
		    } else if (l1state == 7) {
			qoztmp->st[st].l1up = 1;
			/* reset D RX fifo */
			qoz_outb(qoztmp,qoz_R_FIFO,((st + offset) << 1) | 1);
			qoz_waitbusy(qoztmp);
			qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
			qoz_waitbusy(qoztmp);
			/* activation complete, stop timer t3 */
			qoztmp->st[st].t3 = -1;
			qoztmp->spans[st].alarms = ZT_ALARM_NONE;
			zt_alarm_notify(&qoztmp->spans[st]);
			qoztmp->leds[st] = 1;
			if (qoztmp->stports == 8) {
			    sprintf(qoztmp->spans[st].desc,"octoBRI PCI ISDN Card %d Span %d [TE] Layer 1 ACTIVATED (F%d)",qoztmp->cardno ,st + 1, l1state);
			} else {
			    sprintf(qoztmp->spans[st].desc,"quadBRI PCI ISDN Card %d Span %d [TE] (cardID %d) Layer 1 ACTIVATED (F%d)",qoztmp->cardno ,st + 1,qoztmp->cardID, l1state);
			}
		    } else if (l1state == 8) {
			/* lost framing */
			if (debug > 2)
			    printk(KERN_INFO "qozap: starting t4 for span %d\n", st);
			qoztmp->st[st].t4 = 0;
		    } else {
			qoztmp->st[st].l1up = 0;
			if (qoztmp->stports == 8) {
			    sprintf(qoztmp->spans[st].desc,"octoBRI PCI ISDN Card %d Span %d [TE] Layer 1 DEACTIVATED (F%d)",qoztmp->cardno ,st + 1, l1state);
			} else {
			    sprintf(qoztmp->spans[st].desc,"quadBRI PCI ISDN Card %d Span %d [TE] (cardID %d) Layer 1 DEACTIVATED (F%d)",qoztmp->cardno ,st + 1,qoztmp->cardID, l1state);
			}    
		    }
		}
		
	    }
	}
	qoz_resync(qoztmp);
	spin_unlock_irqrestore(&(qoztmp->lock), flags);
    }


    // misc irq
    if (status & 0x40) {
	spin_lock_irqsave(&(qoztmp->lock), flags);
	    irq_misc = qoz_inb(qoztmp,qoz_R_IRQ_MISC);
	spin_unlock_irqrestore(&(qoztmp->lock), flags);
	if (irq_misc & 0x2)  {
	    // qozap timer
	    qoztmp->ticks++;
	    qoz_run(qoztmp);
	    if (qoztmp->ticks % 100) {
		qoz_doLEDs(qoztmp);
	    }
    	    if (qoztmp->ticks % 40) { 
		/* you thought that 42 was the answer.... */
		qoz_doWD(qoztmp);
	    }
	    if (qoztmp->ticks > 1000) {
		qoztmp->ticks = 0;
		for (j=0;j<qoztmp->stports;j++) {
		    /* t3 */
		    if (qoztmp->st[j].t3 >= 0) {
			qoztmp->st[j].t3++;
		    }
		    if (qoztmp->st[j].nt_mode != 1) {
			if ((qoztmp->st[j].t3 > qoz_T3) && (qoztmp->st[j].layer1state != 7)) {
			    /* deactivate layer 1 */
			    if (debug > 2)
				printk(KERN_INFO "qozap: t3 timer expired for span %d\n", j);
			    spin_lock_irqsave(&(qoztmp->lock), flags);
				qoz_outb(qoztmp,qoz_R_ST_SEL, j);
				qoz_outb(qoztmp,qoz_A_ST_WR_STA, 0x40 );
				qoz_waitbusy(qoztmp);
				qoztmp->st[j].t3 = -1;
				qoztmp->spans[j].alarms = ZT_ALARM_RED;
			    spin_unlock_irqrestore(&(qoztmp->lock), flags);
			    zt_alarm_notify_no_master_change(&qoztmp->spans[j]);
			}
		    }
		    /* t4 */
		    if (qoztmp->st[j].t4 >= 0) {
			qoztmp->st[j].t4++;
		    }
		    if (qoztmp->st[j].nt_mode != 1) {
			if ((qoztmp->st[j].t4 > qoz_T4) && (qoztmp->st[j].layer1state != 7)) {
			    /* deactivate layer 1 */
			    if (debug > 2)
				printk(KERN_INFO "qozap: t4 timer expired for span %d\n", j);
			    spin_lock_irqsave(&(qoztmp->lock), flags);
				qoz_outb(qoztmp,qoz_R_ST_SEL, j);
				qoz_outb(qoztmp,qoz_A_ST_WR_STA, 0x40 );
				qoztmp->st[j].t4 = -1;
				qoztmp->st[st].l1up = 0;
				qoztmp->spans[j].alarms = ZT_ALARM_RED;
				qoz_waitbusy(qoztmp);
			    spin_unlock_irqrestore(&(qoztmp->lock), flags);
			    zt_alarm_notify_no_master_change(&qoztmp->spans[j]);
			}
		    }
		}
	    }
	}
	if (irq_misc & 0x4) {
	//    printk(KERN_INFO "qozap proc/nonproc irq\n");
	}
    }

    if (status & 0x80) {
	/* fifo irq */
	spin_lock_irqsave(&(qoztmp->lock), flags);
	irq_foview = qoz_inb(qoztmp,qoz_R_IRQ_OVIEW);
        if (qoztmp->type == 0xb552) {
	    if (irq_foview & 0x60) {
		offset = 0;
		fi = qoz_inb(qoztmp,qoz_R_IRQ_FIFO_BL6);
		for (i=0; i < 8; i++) {
		    if (fi & (1 << i)) {
			st = offset + (i / 2);
			if (i % 2) {
			    if (debug > 2) 
		    		printk(KERN_CRIT "qozap: HDLC RX irq fifo %d span %d\n", i, st+1);
				qoztmp->st[st].drx += 1;			
			} else {
			    if (debug > 2) 
		    		printk(KERN_CRIT "qozap: HDLC TX irq fifo %d span %d\n", i, st+1);
			}
		    }
		}
	    }
	    if (irq_foview & 0x80) {
		offset = 4;
		fi = qoz_inb(qoztmp,qoz_R_IRQ_FIFO_BL7);
		for (i=0; i < 8; i++) {
		    if (fi & (1 << i)) {
			st = offset + (i / 2);
			if (i % 2) {
			    if (debug > 2) 
		    		printk(KERN_CRIT "qozap: HDLC RX irq fifo %d span %d\n", i, st+1);
				qoztmp->st[st].drx += 1;			
			} else {
			    if (debug > 2) 
		    		printk(KERN_CRIT "qozap: HDLC TX irq fifo %d span %d\n", i, st+1);
			}
		    }
		}
	    }
	} else {
	    if (irq_foview & 0x80) {
		fi = qoz_inb(qoztmp,qoz_R_IRQ_FIFO_BL7);
		for (i=0; i < 8; i++) {
		    if (fi & (1 << i)) {
			st = i / 2;
			if (i % 2) {
			    if (debug > 2) 
		    		printk(KERN_CRIT "qozap: HDLC RX irq fifo %d span %d\n", i, st+1);
				qoztmp->st[st].drx += 1;			
			} else {
			    if (debug > 2) 
		    		printk(KERN_CRIT "qozap: HDLC TX irq fifo %d span %d\n", i, st+1);
			}
		    }
		}
	    }
	}
	spin_unlock_irqrestore(&(qoztmp->lock), flags);
    }
    
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
}

static int ztqoz_open(struct zt_chan *chan) {
//    printk(KERN_INFO "qozap: channel %d opened.\n",chan->channo);
#ifndef LINUX26
    MOD_INC_USE_COUNT;
#else
    try_module_get(THIS_MODULE);
#endif
    return 0;
}

static int ztqoz_close(struct zt_chan *chan) {
//    printk(KERN_INFO "qozap: channel %d closed.\n",chan->channo);
#ifndef LINUX26
    MOD_DEC_USE_COUNT;
#else
    module_put(THIS_MODULE);
#endif
    return 0;
}

static int ztqoz_rbsbits(struct zt_chan *chan, int bits) {
    return 0;
}

static int ztqoz_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data) {
        switch(cmd) {
        default:
                return -ENOTTY;
        }
        return 0;
}

static int ztqoz_startup(struct zt_span *span) {
    struct qoz_card *qoztmp = span->pvt;
    unsigned long flags;
    int alreadyrunning;
    int i=0;
    int offset = 0;
    
    if (qoztmp == NULL) {
	printk(KERN_INFO "qozap: no card for span at startup!\n");
    }
    
    alreadyrunning = span->flags & ZT_FLAG_RUNNING;
//    printk(KERN_CRIT "already running %d flags %d\n", alreadyrunning, span->flags);

    if (!alreadyrunning) {
	span->chans[2].flags &= ~ZT_FLAG_HDLC;
	span->chans[2].flags |= ZT_FLAG_BRIDCHAN;
    
	/* setup B channel buffers (8 bytes each) */
	for (i=0; i<2 ; i++) {
	    memset(qoztmp->rxbuf[span->offset][i],0x0,sizeof(qoztmp->rxbuf[span->offset][i]));
    	    span->chans[i].readchunk = qoztmp->rxbuf[span->offset][i];
    	    memset(qoztmp->txbuf[span->offset][i],0x0,sizeof(qoztmp->txbuf[span->offset][i]));
	    span->chans[i].writechunk = qoztmp->txbuf[span->offset][i];
	}
	/* setup D channel buffer */
    	memset(qoztmp->dtxbuf[span->offset],0x0,sizeof(qoztmp->dtxbuf[span->offset]));
	span->chans[2].writechunk = qoztmp->dtxbuf[span->offset];
	qoztmp->chans[span->offset][2].maxbytes2transmit = sizeof(qoztmp->dtxbuf[span->offset]);

	memset(qoztmp->drxbuf[span->offset],0x0,sizeof(qoztmp->drxbuf[span->offset]));
    	span->chans[2].readchunk = qoztmp->drxbuf[span->offset];

	span->flags |= ZT_FLAG_RUNNING;
    } else {
//	printk(KERN_CRIT "already running\n");
	return 0;
    }

    spin_lock_irqsave(&qoztmp->lock,flags);
    // irqs off
    qoz_outb(qoztmp,qoz_R_IRQ_CTRL, 0); 

    if (qoztmp->type == 0xb552) {
	offset = 24;
    } else {
	offset = 28;
    }

    /* setup D-FIFO TX */
    qoz_outb(qoztmp,qoz_R_FIFO,(span->offset + offset) << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0xD);
    qoz_outb(qoztmp,qoz_A_SUBCH_CFG,0x2);
    qoz_outb(qoztmp,qoz_A_CHANNEL,((span->offset * 4) + 2) << 1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    /* setup D-FIFO RX */
    qoz_outb(qoztmp,qoz_R_FIFO,((span->offset + offset) << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0xD);
    qoz_outb(qoztmp,qoz_A_SUBCH_CFG,0x2);
    qoz_outb(qoztmp,qoz_A_CHANNEL,(((span->offset * 4) + 2) << 1) | 1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    /* setup B1-FIFO TX */
    qoz_outb(qoztmp,qoz_R_FIFO,(span->offset * 2) << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_CHANNEL,(span->offset * 4) << 1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    /* setup B1-FIFO RX */
    qoz_outb(qoztmp,qoz_R_FIFO,((span->offset * 2) << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_CHANNEL,((span->offset * 4) << 1) | 1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    /* setup B2-FIFO TX */
    qoz_outb(qoztmp,qoz_R_FIFO,((span->offset * 2) + 1) << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_CHANNEL,((span->offset * 4) + 1) << 1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    /* setup B2-FIFO RX */
    qoz_outb(qoztmp,qoz_R_FIFO,(((span->offset * 2) + 1) << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_R_INC_RES_FIFO,0x2);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_CHANNEL,((((span->offset) * 4) + 1) << 1) | 1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x1);

    if (debug)
        printk(KERN_INFO "qozap: starting card %d span %d/%d.\n",qoztmp->cardno,span->spanno,span->offset);
    
    /* activate layer 1 */
    qoz_outb(qoztmp,qoz_R_ST_SEL,span->offset);
    if (qoztmp->st[span->offset].nt_mode == 1) {
	// NT mode
	qoz_outb(qoztmp,qoz_A_ST_CTRL0,0x7);
	qoz_outb(qoztmp,qoz_A_ST_CTRL1,0x0);
	qoz_outb(qoztmp,qoz_A_ST_CTRL2,0x3);
	qoz_outb(qoztmp,qoz_A_ST_CLK_DLY,0x60 | CLKDEL_NT);
    } else {
	// TE mode
	qoz_outb(qoztmp,qoz_A_ST_CTRL0,0x3);
	qoz_outb(qoztmp,qoz_A_ST_CTRL1,0x0);
	qoz_outb(qoztmp,qoz_A_ST_CTRL2,0x3);
	if (qoztmp->type == 0xb550) {
	    qoz_outb(qoztmp,qoz_A_ST_CLK_DLY,CLKDEL_TE);
	} else {
	    qoz_outb(qoztmp,qoz_A_ST_CLK_DLY,CLKDEL_TE + 1);
	}
    }
    qoztmp->st[span->offset].t3 = 0;
    qoztmp->st[span->offset].t4 = -1;
    
    qoz_outb(qoztmp,qoz_R_ST_SEL,span->offset);
    if (qoztmp->st[span->offset].nt_mode == 1) {
	qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x80); 
    } else {
	qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x0); 
    }


    qoz_outb(qoztmp,qoz_R_ST_SEL,span->offset);
    if (qoztmp->st[span->offset].nt_mode == 1) {
	qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x60 | 0x80); // ACT, G2->G3 EN
    } else {
	qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x60); // start Activation
    }

    /* enable irqs */
    qoz_outb(qoztmp,qoz_R_IRQ_CTRL, 8 | 1); 

    spin_unlock_irqrestore(&qoztmp->lock,flags);
    return 0;
}

static int ztqoz_shutdown(struct zt_span *span) {
    unsigned long flags;
    struct qoz_card *qoztmp = span->pvt;
    int alreadyrunning;
    int offset = 0;
    
    if (qoztmp == NULL) {
	printk(KERN_CRIT "qozap: qoztmp == NULL!\n");
	return 0;
	
    }

    alreadyrunning = span->flags & ZT_FLAG_RUNNING;
    
    if (!alreadyrunning) {
	return 0;
    }

//    printk(KERN_CRIT "qozap: stopping card %d port %d.\n",qoztmp->cardno, span->offset + 1);

    spin_lock_irqsave(&qoztmp->lock,flags);
    // turn off irqs for all fifos
    if (qoztmp->type == 0xb552) {
	offset = 24;
    } else {
	offset = 28;
    }

    /* disable D TX fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,(span->offset + offset) << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x0);

    /* disable D RX fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,((span->offset + offset) << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x1);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x0);

    /* disable B1 TX fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,(span->offset * 2) << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x0);

    /* disable B1 RX fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,((span->offset * 2) << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x0);

    /* disable B2 TX fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,(((span->offset) * 2) + 1) << 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x0);

    /* disable B2 RX fifo */
    qoz_outb(qoztmp,qoz_R_FIFO,((((span->offset) * 2) + 1) << 1) | 1);
    qoz_waitbusy(qoztmp);
    qoz_outb(qoztmp,qoz_A_CON_HDLC,0x2);
    qoz_outb(qoztmp,qoz_A_IRQ_MSK,0x0); 

    span->flags &= ~ZT_FLAG_RUNNING;

    /* Deactivate Layer 1 */
    qoz_outb(qoztmp,qoz_R_ST_SEL,span->offset);
    if (qoztmp->st[span->offset].nt_mode == 1) {
	qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x40); 
    } else {
	qoz_outb(qoztmp,qoz_A_ST_WR_STA,0x40);
    }


    spin_unlock_irqrestore(&qoztmp->lock,flags);
//    printk(KERN_CRIT "qozap: card %d span %d/%d down.\n",qoztmp->cardno,span->spanno,span->offset);
    return 0;
}

static int ztqoz_maint(struct zt_span *span, int cmd) {
    return 0;
}

static int ztqoz_chanconfig(struct zt_chan *chan,int sigtype) {
//    printk(KERN_INFO "chan_config sigtype=%d\n",sigtype);
    return 0;
}

static int ztqoz_spanconfig(struct zt_span *span,struct zt_lineconfig *lc) {
    struct qoz_card *qoztmp = span->pvt;
    span->lineconfig = lc->lineconfig;
    qoztmp->st_sync[span->offset] = lc->sync;
    qoztmp->syncsrc = -1;
    return 0;
}

static int ztqoz_initialize(struct qoz_card *qoztmp) {
    int i=0,s=0;
    
    for (s=0; s < qoztmp->stports; s++) {
	memset(&qoztmp->spans[s],0,sizeof(struct zt_span));
	sprintf(qoztmp->spans[s].name,"ztqoz/%d/%d",qoz_dev_count + 1,s + 1);
	if (qoztmp->stports == 8) {
	    if (qoztmp->st[s].nt_mode == 1){
		sprintf(qoztmp->spans[s].desc,"octoBRI PCI ISDN Card %d Span %d [NT]",qoztmp->cardno,s + 1);
	    } else {
		sprintf(qoztmp->spans[s].desc,"octoBRI PCI ISDN Card %d Span %d [TE]",qoztmp->cardno,s + 1);
	    }
	} else {
	    if (qoztmp->cardID < 0xff) {
		if (qoztmp->st[s].nt_mode == 1){
		    sprintf(qoztmp->spans[s].desc,"quadBRI PCI ISDN Card %d Span %d [NT] (cardID %d)",qoztmp->cardno,s + 1,qoztmp->cardID);
		} else {
		    sprintf(qoztmp->spans[s].desc,"quadBRI PCI ISDN Card %d Span %d [TE] (cardID %d)",qoztmp->cardno,s + 1,qoztmp->cardID);
		}
	    } else {
		if (qoztmp->st[s].nt_mode == 1){
		    sprintf(qoztmp->spans[s].desc,"quadBRI PCI ISDN Card %d Span %d [NT]",qoztmp->cardno,s + 1);
		} else {
		    sprintf(qoztmp->spans[s].desc,"quadBRI PCI ISDN Card %d Span %d [TE]",qoztmp->cardno,s + 1);
		}
	    }
	}

        qoztmp->spans[s].spanconfig = ztqoz_spanconfig;
        qoztmp->spans[s].chanconfig = ztqoz_chanconfig;
        qoztmp->spans[s].startup = ztqoz_startup;
        qoztmp->spans[s].shutdown = ztqoz_shutdown;
        qoztmp->spans[s].maint = ztqoz_maint;
        qoztmp->spans[s].rbsbits = ztqoz_rbsbits;
        qoztmp->spans[s].open = ztqoz_open;
        qoztmp->spans[s].close = ztqoz_close;
        qoztmp->spans[s].ioctl = ztqoz_ioctl;
	qoztmp->spans[s].dacs = ztqoz_dacs;

        qoztmp->spans[s].chans = qoztmp->chans[s];
        qoztmp->spans[s].channels = 3;
        qoztmp->spans[s].deflaw = ZT_LAW_ALAW;
        qoztmp->spans[s].linecompat = ZT_CONFIG_AMI | ZT_CONFIG_CCS;
        init_waitqueue_head(&qoztmp->spans[s].maintq);
        qoztmp->spans[s].pvt = qoztmp;
        qoztmp->spans[s].offset = s;

	for (i=0; i < qoztmp->spans[s].channels; i++) {
	    memset(&(qoztmp->chans[s][i]),0x0,sizeof(struct zt_chan));
	    sprintf(qoztmp->chans[s][i].name,"ztqoz%d/%d/%d",qoz_dev_count + 1,s + 1,i + 1);
	    qoztmp->chans[s][i].pvt = qoztmp;
	    qoztmp->chans[s][i].sigcap =  ZT_SIG_EM | ZT_SIG_CLEAR | ZT_SIG_FXSLS | ZT_SIG_FXSGS | ZT_SIG_FXSKS | ZT_SIG_FXOLS | ZT_SIG_FXOGS | ZT_SIG_FXOKS | ZT_SIG_CAS | ZT_SIG_SF;
	    qoztmp->chans[s][i].chanpos = i + 1; 
	}

	if (zt_register(&qoztmp->spans[s],0)) {
	    printk(KERN_INFO "qozap: unable to register zaptel span %d!\n",s+1);
	    return -1;
	}
//	 printk(KERN_INFO "qozap: registered zaptel span %d.\n",s+1);
    }

    return 0;
}

int qoz_findCards(unsigned int pcidid) {
    struct pci_dev *tmp;
    struct qoz_card *qoztmp = NULL;
    int i=0;
    unsigned char dips=0;
    int cid=0;
    int modes=0;
    tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,multi_qoz);
    while (tmp != NULL) {
	multi_qoz = tmp;	// skip this next time.

	if (pci_enable_device(tmp)) {
	    multi_qoz = NULL;
	    return -1;
	}

	qoztmp = kmalloc(sizeof(struct qoz_card),GFP_KERNEL);
	if (!qoztmp) {
	    printk(KERN_WARNING "qozap: unable to kmalloc!\n");
	    pci_disable_device(tmp);
	    multi_qoz = NULL;
	    return -ENOMEM;
	}
	memset(qoztmp, 0x0, sizeof(struct qoz_card));
	
	spin_lock_init(&qoztmp->lock);
	qoztmp->pcidev = tmp;
	qoztmp->pcibus = tmp->bus->number;
	qoztmp->pcidevfn = tmp->devfn; 

	if (!tmp->irq) {
	    printk(KERN_WARNING "qozap: no irq!\n");
	} else {
	    qoztmp->irq = tmp->irq;
	}

	qoztmp->pci_io_phys = (char *) tmp->resource[1].start;
	if (!qoztmp->pci_io_phys) {
	    printk(KERN_WARNING "qozap: no iomem!\n");
	    pci_disable_device(tmp);
	    multi_qoz = NULL;
	    return -EIO;
	}

	qoztmp->ioport = tmp->resource[0].start;
	if (!qoztmp->ioport) {
	    printk(KERN_WARNING "qozap: no ioport!\n");
	    pci_disable_device(tmp);
	    multi_qoz = NULL;
	    return -EIO;
	}
	if (!request_region(qoztmp->ioport, 8, "qozap")) {
	    printk(KERN_WARNING "qozap: couldnt request io range!\n");
	    pci_disable_device(tmp);
	    multi_qoz = NULL;
	    return -EIO;
	}
	if (!request_mem_region((unsigned long) qoztmp->pci_io_phys, 256, "qozap")) {
	    printk(KERN_WARNING "qozap: couldnt request io mem range!\n");
	    release_region(qoztmp->ioport, 8);
	    pci_disable_device(tmp);
	    multi_qoz = NULL;
	    return -EIO;
	}
	
	if (request_irq(qoztmp->irq, qoz_interrupt, SA_INTERRUPT | SA_SHIRQ, "qozap", qoztmp)) {
	    printk(KERN_WARNING "qozap: unable to register irq\n");
	    kfree(qoztmp);
	    pci_disable_device(tmp);
	    multi_qoz = NULL;
	    return -EIO;
	}

	qoztmp->pci_io = ioremap((ulong) qoztmp->pci_io_phys, 256);
	
	pci_write_config_word(qoztmp->pcidev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY);

	// disable ints
	qoz_outb(qoztmp,qoz_R_IRQ_CTRL, 0); 

	if (pcidid == PCI_DEVICE_ID_CCD_M) {
	    qoztmp->stports = 8;
	} else {
	    qoztmp->stports = 4;
	}
	

        if ((tmp->subsystem_device==0xb520) && (pcidid == PCI_DEVICE_ID_CCD_M4)) {
	//    printk(KERN_INFO "MODES = %#x.\n",modes);
	    qoz_outb(qoztmp,qoz_R_GPIO_SEL,0x80 | 0x40);
	    dips = (qoz_inb(qoztmp,qoz_R_GPIO_IN1) >> 5);
	    cid = 7;
	    for (i=0;i<3;i++) {
	        if ((dips & (1 << i)) != 0) {
	    	cid -= (1 << (2-i));
	        }
	    }
	//	printk(KERN_INFO "DIPS = %#x CID= %#x\n",dips,cid);
        } else if ((tmp->subsystem_device==0xb550) && (pcidid == PCI_DEVICE_ID_CCD_M4)) {
	//    printk(KERN_INFO "MODES = %#x.\n",modes);
	    dips = ~(qoz_inb(qoztmp,qoz_R_GPI_IN3) & 7);
	    cid = 0;
	    for (i=0;i<3;i++) {
	        if ((dips & (1 << i)) != 0) {
	    	cid += (1 << i);
	        }
	    }
	printk(KERN_INFO "DIPS = %#x CID= %#x\n",dips,cid);
        } else {
	    cid = 0xff;
        }

	if (ports == -1) {
    	    if ((tmp->subsystem_device==0xb520) && (pcidid == PCI_DEVICE_ID_CCD_M4)) {
		modes = qoz_inb(qoztmp,qoz_R_GPI_IN3) >> 4;
    	    } else if ((tmp->subsystem_device==0xb550) && (pcidid == PCI_DEVICE_ID_CCD_M4)) {
		qoz_outb(qoztmp,qoz_R_GPIO_SEL,0xff);
		qoz_outb(qoztmp,qoz_R_GPIO_EN0,0x00);
		printk(KERN_CRIT "gpio_in0 %#x \n", qoz_inb(qoztmp,qoz_R_GPIO_IN0));
		printk(KERN_CRIT "gpio_in1 %#x \n", qoz_inb(qoztmp,qoz_R_GPIO_IN1));
		printk(KERN_CRIT "gpi_in1 %#x \n", qoz_inb(qoztmp,qoz_R_GPI_IN1));
		printk(KERN_CRIT "gpi_in2 %#x \n", qoz_inb(qoztmp,qoz_R_GPI_IN2));
		printk(KERN_CRIT "gpi_in3 %#x \n", qoz_inb(qoztmp,qoz_R_GPI_IN3));
		modes = qoz_inb(qoztmp,qoz_R_GPI_IN3) >> 4;	    
	    } else {
		modes = 0; // assume TE mode
	    }
	} else {
	    modes = ports >> totalBRIs;
	}

	if (pcidid == PCI_DEVICE_ID_CCD_M4) {
	    switch (tmp->subsystem_device) {
		case 0x08b4:
			if (ports == -1) ports = 0; /* assume TE mode if no ports param */
			printk(KERN_INFO
		        "qozap: CologneChip HFC-4S evaluation board configured at io port %#x IRQ %d HZ %d\n",
		          (u_int) qoztmp->ioport,
		        qoztmp->irq, HZ);
		    break;
		case 0xb520:
			printk(KERN_INFO
		        "qozap: Junghanns.NET quadBRI card configured at io port %#x IRQ %d HZ %d CardID %d\n",
		          (u_int) qoztmp->ioport,
		        qoztmp->irq, HZ, cid);
		    break;
		case 0xb550:
			printk(KERN_INFO
		        "qozap: Junghanns.NET quadBRI (Version 2.0) card configured at io port %#x IRQ %d HZ %d CardID %d\n",
		          (u_int) qoztmp->ioport,
		        qoztmp->irq, HZ, cid);
		    break;
	    } 
	    totalBRIs += 4;
	} else {
	    switch (tmp->subsystem_device) {
		case 0xb552:
		    printk(KERN_INFO
		       "qozap: Junghanns.NET octoBRI card configured at io port %#x IRQ %d HZ %d\n",
		       (u_int) qoztmp->ioport,
		       qoztmp->irq, HZ);
	        break;
		default:
		    printk(KERN_INFO
		       "qozap: wtf\n");
		    if (qoztmp->pcidev != NULL) {
    			pci_disable_device(qoztmp->pcidev);
		    }
		    pci_write_config_word(qoztmp->pcidev, PCI_COMMAND, 0);	
		    free_irq(qoztmp->irq,qoztmp);
		    kfree(qoztmp);
		    qoztmp = NULL;
		    tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,multi_qoz);
		    continue;
		break;		
	    } 
	    totalBRIs += 8;
	}

	qoztmp->cardID = cid;
	qoztmp->type = tmp->subsystem_device;

	printk(KERN_INFO "qozap: S/T ports: %d [",qoztmp->stports);
	for (i=0;i<qoztmp->stports;i++) {
	    qoztmp->st_sync[i] = 0;
	    if ((modes & (1 << i)) != 0) {
	        qoztmp->st[i].nt_mode = 1;
	        printk(" NT");
	    } else {
	        qoztmp->st[i].nt_mode = 0;
	        printk(" TE");
	    }
	}
	printk(" ]\n");
	
	qoz_registerCard(qoztmp);
	tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,multi_qoz);
    }
    return 0;
}


int qoz_sortCards(void) {
    int changed=0,tmpcardno;
    struct qoz_card *tmpcard,*tmpcard2;
    spin_lock(&registerlock);
    do {
	changed = 0;
	tmpcard = qoz_dev_list;
	while (tmpcard != NULL) {
	    if (tmpcard->prev) {
		if (tmpcard->prev->cardID > tmpcard->cardID) {
		    tmpcardno = tmpcard->prev->cardno;
		    tmpcard->prev->cardno = tmpcard->cardno; 
		    tmpcard->cardno = tmpcardno;
		
		    tmpcard2 = tmpcard->prev;
		    if (tmpcard2->prev) {
			tmpcard2->prev->next = tmpcard;
		    } else {
			qoz_dev_list = tmpcard;
		    }
		    if (tmpcard->next) {
			tmpcard->next->prev = tmpcard2;
		    } 
		    tmpcard2->next = tmpcard->next;
		    tmpcard->prev = tmpcard2->prev;
		    tmpcard->next = tmpcard2;
		    tmpcard2->prev = tmpcard;
		    changed = 1;
		    tmpcard = tmpcard2;
		}
	    }
	    tmpcard = tmpcard->next;
	}
    } while (changed == 1);
    spin_unlock(&registerlock);
    return 0;
}

int qoz_zapCards(void) {
    struct qoz_card *tmpcard;
    tmpcard = qoz_dev_list;
    while (tmpcard != NULL) {
	ztqoz_initialize(tmpcard);
	qoz_resetCard(tmpcard);
	tmpcard = tmpcard->next;
    }
    return 0;
}


int init_module(void) {
    multi_qoz = NULL;
    qoz_findCards(PCI_DEVICE_ID_CCD_M4);
    multi_qoz = NULL;
    qoz_findCards(PCI_DEVICE_ID_CCD_M);
    qoz_sortCards();
    qoz_zapCards();
    if (qoz_dev_count == 0) {
	printk(KERN_INFO "qozap: no multiBRI cards found.\n");
    } else {
	printk(KERN_INFO "qozap: %d multiBRI card(s) in this box, %d BRI ports total, bloop %d, pcmslave %d.\n",qoz_dev_count, totalBRIs, bloop, pcmslave);
    }
    return 0;
}

void cleanup_module(void) {
    struct qoz_card *tmpcard,*tmplist;
    int i=0;
    tmplist = qoz_dev_list;
    while (tmplist != NULL) {
	tmplist->dead = 1;
	qoz_undoWD(tmplist);
	qoz_shutdownCard(tmplist);
	tmplist = tmplist->next;
    }
    tmplist = qoz_dev_list;
    spin_lock(&registerlock);
    while (tmplist != NULL) {
	tmpcard = tmplist->next;
	kfree(tmplist);
	i++;
	tmplist = tmpcard;
    }
    spin_unlock(&registerlock);
    printk(KERN_INFO "qozap: shutdown %d multiBRI cards.\n", i);
}
#endif

#ifdef LINUX26
module_param(doubleclock, int, 0600);
module_param(ports, int, 0600);
module_param(pcmslave, int, 0600);
module_param(bloop, int, 0600);
module_param(debug, int, 0600);
module_param(dacs, int, 0600);
#else
MODULE_PARM(doubleclock,"i");
MODULE_PARM(ports,"i");
MODULE_PARM(pcmslave,"i");
MODULE_PARM(bloop,"i");
MODULE_PARM(debug,"i");
MODULE_PARM(dacs,"i");
#endif


MODULE_DESCRIPTION("quad/octo BRI zaptel driver");
MODULE_AUTHOR("Klaus-Peter Junghanns <kpj@junghanns.net>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif	
