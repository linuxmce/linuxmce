/*
 * cwain.c - Zaptel driver for the Junghanns.NET E1 card
 *
 * c.w.a.i.n. == card without an interesting name
 *
 * single/double E1 board
 *
 * Copyright (C) 2004, 2005 Junghanns.NET GmbH
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
#include "cwain.h"

#if CONFIG_PCI

static int ports=-1; /* autodetect */
static int debug=0;
static struct zt_cwain *cwain_span_list = NULL;
static int cwain_span_count = 0;
static struct zt_cwain_card *cwain_card_list = NULL;
static int cwain_card_count = 0;
static struct pci_dev *multi_cwain = NULL;
static spinlock_t cwain_span_registerlock = SPIN_LOCK_UNLOCKED;
static spinlock_t cwain_card_registerlock = SPIN_LOCK_UNLOCKED;

static int ztcwain_shutdown(struct zt_span *span);

int cwain_waitbusy(struct zt_cwain *cwaintmp) {
    int x=1000;
    while (x-- && (cwain_inb(cwaintmp,cwain_R_STATUS) & 1));
    if (x < 0) {
	printk(KERN_CRIT "cwain: really busy waiting!\n");
	return -1;
    } else {
	if ((x < 990) && (cwaintmp->ticks > 500)) {
	    printk(KERN_CRIT "cwain: waited %d\n", 1000 - x);
	}
	return 0;
    }
}

void cwain_unregister_zap_span(struct zt_cwain *cwainspan) {
    if (!cwainspan) {
	printk(KERN_INFO "cwain: shutting down NULL span!\n");
	return;
    }
    if(cwainspan->span.flags & ZT_FLAG_RUNNING) {
        ztcwain_shutdown(&cwainspan->span);
        if (debug)
            printk(KERN_INFO "cwain: shutdown span %d.\n",cwainspan->cardno);
    }
    if(cwainspan->span.flags & ZT_FLAG_REGISTERED) {
        zt_unregister(&cwainspan->span);
        if (debug)
    	printk(KERN_INFO "cwain: unregistered span %d.\n",cwainspan->cardno);
    }
}

void cwain_shutdown_span(struct zt_cwain *cwainspan) {
    
    if (!cwainspan) {
	printk(KERN_INFO "cwain: shutting down NULL span!\n");
	return;
    }

    if (cwainspan->pci_io == NULL) {
	return;
    }

    if (debug)
	printk(KERN_INFO "cwain: shutting down span %d (cardID %d) at %p.\n",cwainspan->cardno,cwainspan->cardID,cwainspan->pci_io);

    // turn off irqs

    // softreset
    cwain_outb(cwainspan,cwain_R_CIRM,0x8);
    cwain_outb(cwainspan,cwain_R_CIRM,0x0); 
    cwain_waitbusy(cwainspan);

    cwain_outb(cwainspan,cwain_R_IRQMSK_MISC, 0); 
    cwain_outb(cwainspan,cwain_R_IRQ_CTRL, 0); 

    pci_write_config_word(cwainspan->pcidev, PCI_COMMAND, 0);	// disable memio

    if (cwainspan->pcidev != NULL) {
        pci_disable_device(cwainspan->pcidev);
	cwainspan->pcidev = NULL;
    }

//    iounmap((void *) cwainspan->pci_io);
//    cwainspan->pci_io = NULL;
}

void cwain_shutdown_card(struct zt_cwain_card *cwaintmp) {
    unsigned long flags = 0;
    int i = 0;
    
    if (!cwaintmp) {
	printk(KERN_INFO "cwain: shutting down NULL card!\n");
	return;
    }

    for (i=0;i<cwaintmp->spans;i++) {
	cwain_unregister_zap_span(cwaintmp->span[i]);
    }

    spin_lock_irqsave(&cwaintmp->lock,flags);

    // turn off irqs
    cwain_outb(cwaintmp->span[0],cwain_R_IRQ_CTRL, 0); 
    cwain_outb(cwaintmp->span[0],cwain_R_IRQMSK_MISC, 0); 

    for (i=0;i<cwaintmp->spans;i++) {
	cwain_shutdown_span(cwaintmp->span[i]);
    }
    spin_unlock_irqrestore(&cwaintmp->lock,flags);

    for (i=0;i<cwaintmp->spans;i++) {
	release_region(cwaintmp->span[i]->ioport, 8);
	cwaintmp->span[i]->ioport = 0;
	iounmap((void *) cwaintmp->span[i]->pci_io);
	cwaintmp->span[i]->pci_io = NULL;
	release_mem_region((unsigned long)cwaintmp->span[i]->pci_io_phys, 256);
    }

    if (cwaintmp->spans == 2) {
	free_irq(cwaintmp->span[1]->irq,cwaintmp);
    }
    free_irq(cwaintmp->irq,cwaintmp);
}

void cwain_doLEDs(struct zt_cwain *cwaintmp) {
    /*
     O1 O3 (red)
     O2 O4 (green)
    */
    if (!(cwaintmp->span.flags & ZT_FLAG_RUNNING)) {
	return;
    }
    if ((cwaintmp->type == 0xb553) || (cwaintmp->type == 0xb554)) {
	/* sync status */
	if (((cwaintmp->sync_sta & 0x07) == 0x07) && cwaintmp->sync) {
	    cwaintmp->leds[0] = 1;    
	    cwaintmp->leds[1] = 0;    
	} else {
	    cwaintmp->leds[0] = 0;    
	    cwaintmp->leds[1] = 1;    
	}
	/* multiframe alignment */
	if ((cwaintmp->sync_sta & 0x20) == 0x20) {
    	    cwaintmp->leds[2] = 1;    
	    cwaintmp->leds[3] = 0;    
	} else {
	    if ((cwaintmp->span.lineconfig & ZT_CONFIG_CRC4) && cwaintmp->sync) {
		/* CRC4 requested */
		cwaintmp->leds[2] = 0;    
		cwaintmp->leds[3] = 1;    
	    } else {
		/* no CRC4, disable 3 and 4 */
		cwaintmp->leds[2] = 1;    
		cwaintmp->leds[3] = 1;    
	    }
	}
	cwain_outb(cwaintmp,cwain_R_GPIO_OUT1,(cwaintmp->leds[0] | (cwaintmp->leds[1] << 1) | (cwaintmp->leds[2] << 2) | (cwaintmp->leds[3] << 3)));
    }
}

void cwain_reset_span(struct zt_cwain *cwaintmp) {
    int i = 0;
    pci_write_config_word(cwaintmp->pcidev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_IO);	// enable memio

    /* FIFO, HDLC reset */
    cwain_outb(cwaintmp,cwain_R_CIRM,0x10);
    cwain_outb(cwaintmp,cwain_R_CIRM,0x0); 
    cwain_waitbusy(cwaintmp);

    /* PCM reset */
    cwain_outb(cwaintmp,cwain_R_CIRM,0x20);
    cwain_outb(cwaintmp,cwain_R_CIRM,0x0); 
    cwain_waitbusy(cwaintmp);

    for (i=0; i<128; i++) {
	cwain_outb(cwaintmp,cwain_R_SLOT, i);
	cwain_outb(cwaintmp,cwain_A_SL_CFG, 0x0);
    }
    
    /* E1 reset */
    cwain_outb(cwaintmp,cwain_R_CIRM,0x40);
    cwain_outb(cwaintmp,cwain_R_CIRM,0x0); 
    cwain_waitbusy(cwaintmp);

    /* 128 byte B chans, 4096 byte D chans */
    cwain_outb(cwaintmp,cwain_R_FIFO_MD,0x36);
    cwain_outb(cwaintmp,cwain_R_BRG_PCM_CFG,0x0); 
    cwain_outb(cwaintmp,cwain_R_CTRL,0x0); 

    /* no blinky blink */
    cwain_outb(cwaintmp,cwain_R_GPIO_SEL,0x20 | 0x10);
    cwain_outb(cwaintmp,cwain_R_GPIO_EN1,0x0f);
    cwain_outb(cwaintmp,cwain_R_GPIO_OUT1,0x0f);

    /* IRQs off */
    cwain_outb(cwaintmp,cwain_R_IRQMSK_MISC,0x0); 
    cwain_outb(cwaintmp,cwain_R_IRQ_CTRL,0x0); 

    cwaintmp->leds[0] = 1;    
    cwaintmp->leds[1] = 1;    
    cwaintmp->leds[2] = 1;    
    cwaintmp->leds[3] = 1;    
    
    cwaintmp->ticks = 0;
    cwaintmp->clicks = 0;
}

struct zt_cwain_card *cwain_get_card(unsigned int pcibus) {
    struct zt_cwain_card *cwaintmp = cwain_card_list;
    spin_lock(&cwain_card_registerlock);
    while (cwaintmp) {
	if (cwaintmp->pcibus == pcibus) {
	    break;
	}
	cwaintmp = cwaintmp->next;
    }
    spin_unlock(&cwain_card_registerlock);
    return cwaintmp;
}


void cwain_register_card(struct zt_cwain_card *cwaincard) {
    spin_lock(&cwain_card_registerlock);
    if (cwaincard != NULL) {
	cwaincard->prev = NULL;
	cwaincard->next = cwain_card_list;
	if (cwain_card_list) {
	    cwain_card_list->prev = cwaincard;
	}
	cwain_card_list = cwaincard;
	cwain_card_count++;
    } else {
	printk(KERN_INFO "cwain: trying to register NULL card.\n");
    }
    spin_unlock(&cwain_card_registerlock);
}

int cwain_register_span(struct zt_cwain *cwainspan) {
    struct zt_cwain_card *cwaintmp;
    spin_lock(&cwain_span_registerlock);
    if (cwainspan != NULL) {
	cwainspan->prev = NULL;
	cwainspan->next = cwain_span_list;
	if (cwain_span_list) {
	    cwain_span_list->prev = cwainspan;
	}
	cwain_span_list = cwainspan;
	cwainspan->cardno = ++cwain_span_count;
    } else {
	printk(KERN_INFO "cwain: trying to register NULL span.\n");
    }
    spin_unlock(&cwain_span_registerlock);

    if (cwainspan->type == 0xb553) {
	cwaintmp = kmalloc(sizeof(struct zt_cwain_card),GFP_KERNEL);
	if (!cwaintmp) {
	    printk(KERN_WARNING "cwain: unable to kmalloc!\n");
	    return -1;
	}
	memset(cwaintmp, 0x0, sizeof(struct zt_cwain_card));
	
	spin_lock_init(&cwaintmp->lock);
	cwaintmp->pcibus = cwainspan->pcibus;
	cwaintmp->span[0] = cwainspan;
	cwaintmp->syncs[0] = -1;
	cwaintmp->spans = 1;
	cwaintmp->cardID = cwainspan->cardID;
	cwain_register_card(cwaintmp);
	printk(KERN_INFO
	        "cwain: Junghanns.NET singleE1 PCI ISDN card configured at mem %lx IRQ %d HZ %d CardID %d\n",
	          (unsigned long) cwainspan->pci_io,
	        cwaintmp->span[0]->irq, HZ, cwainspan->cardID);
    } else {
	cwaintmp = cwain_get_card(cwainspan->pcibus);
	if (!cwaintmp) {
	    cwaintmp = kmalloc(sizeof(struct zt_cwain_card),GFP_KERNEL);
	    if (!cwaintmp) {
		printk(KERN_WARNING "cwain: unable to kmalloc!\n");
	        return -1;
	    }
	    memset(cwaintmp, 0x0, sizeof(struct zt_cwain_card));
	
	    spin_lock_init(&cwaintmp->lock);
	    cwaintmp->pcibus = cwainspan->pcibus;
	    cwaintmp->spans = cwainspan->type - 46419;
	    cwaintmp->span[0] = cwainspan;
	    cwaintmp->cardID = cwainspan->cardID;
	    cwaintmp->syncs[0] = -1;
	    cwain_register_card(cwaintmp);
	} else {
	    cwaintmp->spans = cwainspan->type - 46418;
	    if (cwainspan->cardID < cwaintmp->cardID) {
		cwaintmp->cardID = cwainspan->cardID;
		cwaintmp->span[1] = cwaintmp->span[0];
		cwaintmp->syncs[1] = cwaintmp->syncs[0];
	        cwaintmp->span[0] = cwainspan;
	    } else {
	        cwaintmp->span[1] = cwainspan;
		cwaintmp->syncs[1] = -1;
	    }
	    printk(KERN_INFO
	        "cwain: Junghanns.NET doubleE1 PCI ISDN card configured at mem (%lx / %lx) IRQ %d HZ %d CardID (%d / %d) bus %#x\n",
	          (unsigned long) cwaintmp->span[0]->pci_io, (unsigned long) cwaintmp->span[1]->pci_io,
	        cwaintmp->span[0]->irq, HZ, cwaintmp->span[0]->cardID, cwaintmp->span[1]->cardID, cwaintmp->pcibus);
	}
    }
    return 0;
}

static int cwain_dfifo_tx(struct zt_cwain *cwaintmp) {
    int chan = 15;
    int x=0;
    char fifo = 0;

    fifo = 0x1F;

    if (cwaintmp->chans[chan].bytes2transmit < 1) {
	return 0;
    } else {
	/* select fifo */
	cwain_outb(cwaintmp,cwain_R_FIFO,fifo << 1);    
	cwain_waitbusy(cwaintmp);
    
	if (debug)
	    printk(KERN_INFO "cwain: card %d TX [ ", cwaintmp->cardno);
	/* copy frame to fifo */
    	for (x=0;x<cwaintmp->chans[chan].bytes2transmit;x++) {
	    if (debug)
	        printk("%#x ",cwaintmp->dtxbuf[x]);
    	    cwain_outb(cwaintmp,cwain_A_FIFO_DATA0,cwaintmp->dtxbuf[x]);
	}
	if (debug)
	    printk("]\n");
	if (debug)
    	    printk(KERN_INFO "ztx %d bytes\n",cwaintmp->chans[chan].bytes2transmit);

	if (cwaintmp->chans[chan].eoftx == 1) {
	    /* transmit HDLC frame */
    	    cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x1);    
    	    cwain_waitbusy(cwaintmp);
	}
    }
    return 0;
}

static int cwain_fifo_tx(struct zt_cwain *cwaintmp, char fifo) {
    int chan,f;
    if (fifo >= 15) {
	chan = fifo;
    } else {
	chan = fifo;
    }
    /* select fifo */
    cwain_outb(cwaintmp,cwain_R_FIFO,0x80 | (fifo << 1));    
    cwain_waitbusy(cwaintmp);
    
    for (f=0; f < (cwain_FRAME_SIZE/4); f++) {
	cwain_outdw(cwaintmp,cwain_A_FIFO_DATA0,*((unsigned int *) &cwaintmp->ftxbuf[chan][f * 4]));
    }
    return 0;
}

static int cwain_dfifo_rx(struct zt_cwain *cwaintmp) {
    int chan = 15;
    unsigned char f1=1,f2=1,data,stat;
    unsigned char of1=0,of2=0;
    int len,i;
    unsigned short z1=1,z2=1;
    unsigned short oz1=0,oz2=0;
    char fifo = 0;

    fifo = 0x1F;
    
    /* select fifo */
    cwain_outb(cwaintmp,cwain_R_FIFO,(fifo << 1) | 1);    
    cwain_waitbusy(cwaintmp);
    
    while ((oz1 != z1) && (oz2 != z2)) {
        oz1 = z1;
        oz2 = z2;
    	z1 = cwain_inw(cwaintmp,cwain_A_Z1) & 0xfff;
	z2 = cwain_inw(cwaintmp,cwain_A_Z2) & 0xfff;
    }
    
    len = z1-(z2 & 0xfff);
    if (len < 0) {
        len += cwain_DFIFO_SIZE;
    }

    while ((of1 != f1) && (of2 != f2)) {
        of1 = f1;
        of2 = f2;
        f1 = cwain_inb(cwaintmp,cwain_A_F1) & 0xf;
        f2 = cwain_inb(cwaintmp,cwain_A_F2) & 0xf;
    }

    if (len > cwain_DFIFO_SIZE) {
	printk(KERN_INFO "\ncwain: buffer overflow in D channel RX!\n");
	cwaintmp->chans[chan].bytes2receive = 0;
	cwaintmp->chans[chan].eofrx = 0;
    } else {
	if (debug) printk(KERN_INFO "cwain: card %d RX [ ", cwaintmp->cardno);
	for (i=0; i<len; i++) {
    	    data = cwain_inb(cwaintmp,cwain_A_FIFO_DATA0);
	    cwaintmp->drxbuf[i] = data;
	    if (debug) printk("%#x ",data);
	}
	if (debug) printk("] %d bytes\n", i);
	cwaintmp->chans[chan].bytes2receive = i;
	cwaintmp->chans[chan].eofrx = 1;
    }

    stat = cwain_inb(cwaintmp,cwain_A_FIFO_DATA0);
    if (stat != 0x0) {
	// bad CRC, skip it
	if (cwaintmp->sync) {
	    printk(KERN_INFO "cwain: BAD CRC for hdlc frame on card %d (cardID %d) stat %#x\n",cwaintmp->cardno, cwaintmp->cardID, stat);
	}
	cwaintmp->chans[chan].bytes2receive = 0;
	cwaintmp->chans[chan].eofrx = 0;
//	    zt_qevent_nolock(&cwaintmp->spans[stport].chans[chan], ZT_EVENT_BADFCS);
    }
    cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x1);    
    cwain_waitbusy(cwaintmp);

    /* frame received */
    cwaintmp->drx--;
    return 0;
}


static int cwain_fifo_rx(struct zt_cwain *cwaintmp, char fifo) {
    int chan;
    unsigned int data;
    int len,i,f,flen = 0;
    unsigned short z1=1,z2=1;
    unsigned short oz1=0,oz2=0;
    int mumbojumbo=0;
    int x = 1000;

    chan = fifo;

    // select rx fifo
    
	// no hdlc, transparent data
	cwain_outb(cwaintmp,cwain_R_FIFO,0x80 | (fifo << 1) | 1);    
        cwain_waitbusy(cwaintmp);
    
	while (x-- && ((oz1 != z1) && (oz2 != z2))) {
	    len = z1 - z2;
	    if (len < 0) {
		len += cwain_FIFO_SIZE;
	    }
	    flen = len;
	    if (len > cwain_FIFO_HW) {
		mumbojumbo = len - (cwain_FIFO_HW - cwain_FRAME_SIZE);
	        len = cwain_FRAME_SIZE;
	    }
	    oz1 = z1;
	    oz2 = z2;
	    z1 = cwain_inw(cwaintmp,cwain_A_Z1) & 0x7f;
    	    z2 = cwain_inw(cwaintmp,cwain_A_Z2) & 0x7f;
	}
	if (x < 500) {
	    printk(KERN_CRIT "cwain: prevented endless loop\n");
	}
    
	if (mumbojumbo > 0) {
	    for (i=0;i<(mumbojumbo/4);i++) {
    		data = cwain_indw(cwaintmp,cwain_A_FIFO_DATA0);
	    }
	    cwaintmp->clicks++;
	}
	if (len < cwain_FRAME_SIZE) {
	    /* dont get nervous here */
	    if ((cwaintmp->clicks > 600) && (cwaintmp->span.alarms == ZT_ALARM_NONE)) {
		printk(KERN_INFO "cwain: not enough to receive (%d bytes)\n",len);
	    }
	    return 0;
	} else {
	    for (f=0;f<(cwain_FRAME_SIZE / 4);f++) {
		*((unsigned int *) &cwaintmp->frxbuf[chan][f*4]) = cwain_indw(cwaintmp,cwain_A_FIFO_DATA0);
	    }
	}


	/* dont get nervous here */
	if ((cwaintmp->clicks > 500) && (cwaintmp->span.alarms == ZT_ALARM_NONE)) {
	    printk(KERN_INFO "cwain: span %d dropped audio fifo %d mj %d flen %d z1 %d z2 %d\n", cwaintmp->cardID, fifo, mumbojumbo, flen, z1, z2);
	    cwaintmp->clicks = 0;
	}
//    printk(KERN_INFO "s/t port %d, channel %d, dbufi=%d, f1=%d, f2=%d, z1=%d, z2=%d  => len = %d stat=%#x, hdlc=%d\n",stport,chan,cwaintmp->st[stport].dbufi,f1,f2,z1,z2,len,stat,hdlc);    
    return 0;
}

void cwain_set_master(struct zt_cwain_card *cwaintmp, int span) {
    int i=0;

    if (cwaintmp->syncsrc == span) return;
    
    for (i=0; i < cwaintmp->spans; i++) {
	if (i != span) {
	    if (cwaintmp->syncs[i] > 0) {
		/* enable PCM slave mode, PCM32, synced to E1 receive */
    		cwain_outb(cwaintmp->span[i],cwain_R_PCM_MD0, 0x90);
		cwain_outb(cwaintmp->span[i],cwain_R_PCM_MD1, 0x0);
		cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD0, 0xA0);
		cwain_outb(cwaintmp->span[i],cwain_R_PCM_MD2, 0x00);
	    } else {
		/* enable PCM slave mode, PCM32 */
    		cwain_outb(cwaintmp->span[i],cwain_R_PCM_MD0, 0x90);
		cwain_outb(cwaintmp->span[i],cwain_R_PCM_MD1, 0x0);
		cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD0, 0xA0);
		cwain_outb(cwaintmp->span[i],cwain_R_PCM_MD2, 0x00);
	    }
	    cwaintmp->master[i] = 0;
	    cwaintmp->span[i]->span.syncsrc = 0;
	}
    }

    if (cwaintmp->syncs[span] > 0) {
        /* enable PCM master mode, PCM32, synced to E1 receive */
	if (debug)
	    printk(KERN_INFO "cwain: cardID %d span %d, PCM master E1 sync\n", cwaintmp->cardID, span);
	cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD0, 0x91);
        cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD1, 0x0);
	cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD0, 0xA1);
        cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD2, 0x00);
	cwaintmp->span[span]->span.syncsrc = 1;
    } else {
        /* enable PCM master mode, PCM32, free running */
	if (debug)
	    printk(KERN_INFO "cwain: cardID %d span %d, PCM master\n", cwaintmp->cardID, span);
	cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD0, 0x91);
        cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD1, 0x0);
	cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD0, 0xA1);
        cwain_outb(cwaintmp->span[span],cwain_R_PCM_MD2, 0x04);
	cwaintmp->span[span]->span.syncsrc = 0;
    }
    
    cwaintmp->master[span] = 1;
    cwaintmp->syncsrc = span;
}

void cwain_check_timing(struct zt_cwain_card *cwaintmp) {
    int i=0;
    int bestsync = 42;
    int sync_ok = 0;

    for (i=0; i < cwaintmp->spans; i++) {
	if (cwaintmp->span[i]->span.lineconfig & ZT_CONFIG_CRC4) {
	    /* CRC4 requested */
	    sync_ok = 0x27;
	} else {
	    /* doubleframing requested */
	    sync_ok = 0x07;
	}    
        if ((cwaintmp->syncs[i] > 0) && ((cwaintmp->span[i]->sync_sta & sync_ok) == sync_ok)) {
    	    if (bestsync < cwaintmp->spans) {
		if (cwaintmp->syncs[i] < cwaintmp->syncs[bestsync]) {
	    	    bestsync = i;
		}
	    } else {
	        bestsync = i;
	    }
	}
    }

    if (cwaintmp->syncsrc >= 0) {
	if (debug > 3)
	    printk(KERN_INFO "cwain: bestsync %d cwaintmp->syncsrc %d\n", bestsync, cwaintmp->syncsrc);
	
	if (bestsync == cwaintmp->syncsrc) {
	    if (debug > 3)
		printk(KERN_INFO "cwain: already on best syncsrc %d\n", bestsync);
	    return;
	}

	/* if we have a better syncsrc */
	if (bestsync < cwaintmp->spans) {
	    if (debug)
		printk(KERN_INFO "cwain: found better syncsrc %d\n", bestsync);
	    cwain_set_master(cwaintmp, bestsync);
	    return;		    
	}
    }

    /* if reelection failed, find internal sync source */
    if (cwaintmp->syncsrc == -1) {
	/* no master yet */
	if (debug > 3)
	    printk(KERN_INFO "cwain: no clocksource found cardID %d\n", cwaintmp->cardID);
	for (i=0; i < cwaintmp->spans; i++) {
	    /* find the first internal source */
	    if (debug > 3)
	        printk(KERN_INFO "cwain: cwaintmp->syncs[%d] = %d\n", i, cwaintmp->syncs[i]);
	    if (cwaintmp->syncs[i] == 0) {
		if (debug)
		    printk(KERN_INFO "cwain: using internal clock of span %d\n", i);
		cwain_set_master(cwaintmp, i);
		return;
	    }
	}
    }

    /* if we have no internal sync source the only thing we can do is to enable any of the possible sync sources*/
    if (cwaintmp->syncsrc == -1) {
	/* find the first possible sync source with framing */
        for (i=0; i < cwaintmp->spans; i++) {
	    if (cwaintmp->syncs[i] > 0) {
		if (debug)
		    printk(KERN_INFO "cwain: desperately using clock of span %d\n", i);
		cwain_set_master(cwaintmp, i);
		break;
	    }
	}
    }
}

static inline void cwain_isr_run(struct zt_cwain *cwaintmp, int ticks) {
    int fifo=0;
    if (cwaintmp->span.flags & ZT_FLAG_RUNNING) {
        /* oh zaptel! tell us what to transmit... */
        zt_transmit(&cwaintmp->span);
	/* B chans 1-15 mapped to fifos 0-14 */
	/* B chans 17-31 mapped to fifos 15-29 */
	for (fifo=0; fifo < 30; fifo++) {
	    /* copy to fbuffer */
	    if ((ticks < 1) || (ticks > 8)) {
		printk(KERN_INFO "cwain: whicked ticks make whicked tricks (%d)\n",cwaintmp->ticks);
	    } else {
		memcpy(&cwaintmp->ftxbuf[fifo][(ticks-1)*8], cwaintmp->txbuf[fifo], ZT_CHUNKSIZE);
	    }

	}
	if (cwaintmp->sync) {
	    cwain_dfifo_tx(cwaintmp);
	}

        cwaintmp->chans[15].bytes2receive = 0;
	cwaintmp->chans[15].bytes2transmit = 0;
	cwaintmp->chans[15].eofrx = 0;
	cwaintmp->chans[15].eoftx = 0;

	for (fifo=0; fifo < 30; fifo++) {
	    /* copy from fbuffer */
	    memcpy(cwaintmp->rxbuf[fifo], &cwaintmp->frxbuf[fifo][(ticks-1)*8], ZT_CHUNKSIZE);
	    zt_ec_chunk(&cwaintmp->span.chans[fifo], cwaintmp->span.chans[fifo].readchunk, cwaintmp->span.chans[fifo].writechunk);
	}

	/* d-chan data */
	if ((cwaintmp->drx > 0) && cwaintmp->sync){
	    if (debug > 2)
	    	printk(KERN_CRIT "drx = %d\n", cwaintmp->drx);
	    cwain_dfifo_rx(cwaintmp);
	}
	/* oh zaptel! thou shall receive! */
	zt_receive(&(cwaintmp->span));
    }
}

static inline void cwain_isr_err(struct zt_cwain *cwaintmp) {
    unsigned short crc, vio, ebit, fas;

    crc = (cwain_inb(cwaintmp, cwain_R_CRC_ECH) << 8) | cwain_inb(cwaintmp, cwain_R_CRC_ECL);
    vio = (cwain_inb(cwaintmp, cwain_R_VIO_ECH) << 8) | cwain_inb(cwaintmp, cwain_R_VIO_ECL);
    ebit = (cwain_inb(cwaintmp, cwain_R_E_ECH) << 8) | cwain_inb(cwaintmp, cwain_R_E_ECL);
    fas = (cwain_inb(cwaintmp, cwain_R_FAS_ECH) << 8) | cwain_inb(cwaintmp, cwain_R_FAS_ECL);
			
    cwaintmp->span.crc4count += crc;
    cwaintmp->span.bpvcount += vio;
    cwaintmp->span.ebitcount += ebit;
    cwaintmp->span.fascount += fas;
			
    if (debug > 2)
        printk("cwain: CRC4 %d BPVIOL %d EBIT %d FAS %d\n", crc, vio, ebit, fas);
}
						    		    

static inline void cwain_audio_run(struct zt_cwain *cwaintmp) {
    int fifo=0;
	for (fifo=0; fifo < 30; fifo++) {
	    /* B xmit */
	    cwain_fifo_tx(cwaintmp, fifo);
	}

	for (fifo=0; fifo < 30; fifo++) {
	    /* B rx */
	    cwain_fifo_rx(cwaintmp, fifo);
	}
}

int cwain_isr_sync(struct zt_cwain *cwainspan) {
    unsigned char sync_sta;
    unsigned char sync_ok = 0;
    unsigned char jatt_sta = 0;
    int res = 0; /* assume no l1event */

    if (!cwainspan->span.flags & ZT_FLAG_RUNNING) {
	return res;
    }

    sync_sta = cwain_inb(cwainspan, cwain_R_SYNC_STA);

    if ((!cwainspan->sync) || (sync_sta != cwainspan->sync_sta)) {

	if (debug > 2)
	    printk(KERN_CRIT "cwain: cardID %d R_SYNC_STA =%#x\n", cwainspan->cardID, sync_sta);

	if (cwainspan->span.lineconfig & ZT_CONFIG_CRC4) {
	    if ((sync_sta & 0x80) == 0x80) {
		/* reset MFA detection */
		cwain_outb(cwainspan ,cwain_R_RX_SL0_CFG1,0x41);
	    } else if ((sync_sta & 0x27) == 0x27) {
		if ((cwainspan->sync_sta & 0x27) != 0x27) {
		    /* sync achieved, restart JATT */
		    if (debug)
			printk(KERN_INFO "cwain: %d starting jitter attenuator\n", cwainspan->cardID);
		    cwain_outb(cwainspan, cwain_R_JATT_CFG,0x9c);
		}
		sync_ok = 0x27;
	    } else {
		sync_ok = 0x00;
	    }
	} else {
	    if ((sync_sta & 0x07) == 0x07) {
		if ((cwainspan->sync_sta & 0x7) != 0x7) {
		    /* sync achieved, restart JATT */
		    if (debug)
			printk(KERN_INFO "cwain: %d starting jitter attenuator\n", cwainspan->cardID);
		    cwain_outb(cwainspan, cwain_R_JATT_CFG,0x9c);
		}
		sync_ok = 0x07;
	    } else {
		sync_ok = 0x00;
	    }
	}

	cwainspan->sync_sta = sync_sta;

	jatt_sta = cwain_inb(cwainspan, cwain_R_JATT_STA);
	if ((jatt_sta & 0x60) != 0x60) {
	    if (debug > 2)
		printk(KERN_INFO "cwain: %d jitter attenuator %#x\n", cwainspan->cardID, (jatt_sta & 0x60) >> 5);
	    sync_ok = 0x00;
	} else if (!cwainspan->sync && sync_ok) {
	    if (debug)
		printk(KERN_CRIT "cwain: %d jitter attenuator %#x ok!\n", cwainspan->cardID, (jatt_sta & 0x60) >> 5);
	}

	if (sync_ok && (!cwainspan->sync)) {
	    /* elastic buffer offsets */
	    cwain_outb(cwainspan,cwain_R_RX_OFFS,0x06);
	    cwain_outb(cwainspan,cwain_R_TX_OFFS,0x06);

	    if (debug > 2)
		printk(KERN_INFO "cwain: enabling D channel fifos\n");
	    cwain_outb(cwainspan,cwain_R_FIFO,0x1F << 1);
	    cwain_waitbusy(cwainspan);
	    cwain_outb(cwainspan,cwain_A_CON_HDLC,0xd);
	    cwain_outb(cwainspan,cwain_A_IRQ_MSK,0x1);
	
	    cwain_outb(cwainspan,cwain_R_FIFO,(0x1F << 1) | 1);
	    cwain_waitbusy(cwainspan);
	    cwain_outb(cwainspan,cwain_A_CON_HDLC,0xd);
	    cwain_outb(cwainspan,cwain_A_IRQ_MSK,0x1);

	    cwainspan->span.crc4count = 0;
    	    cwainspan->span.bpvcount = 0;
	    cwainspan->span.ebitcount = 0;
    	    cwainspan->span.fascount = 0;
	    cwainspan->span.alarms = ZT_ALARM_NONE;
	    zt_alarm_notify(&cwainspan->span);
	    res = 1;
	}
	if (!sync_ok && cwainspan->sync) {
	    if (debug > 2)
		printk(KERN_INFO "cwain: disabling D channel fifos\n");
	    cwain_outb(cwainspan,cwain_R_FIFO,0x1F << 1);
	    cwain_waitbusy(cwainspan);
	    cwain_outb(cwainspan,cwain_A_CON_HDLC,0x1);
	    cwain_outb(cwainspan,cwain_A_IRQ_MSK,0x0);
	    cwain_outb(cwainspan,cwain_R_FIFO,(0x1F << 1) | 1);
	    cwain_waitbusy(cwainspan);
	    cwain_outb(cwainspan,cwain_A_CON_HDLC,0x1);
	    cwain_outb(cwainspan,cwain_A_IRQ_MSK,0x0);
	    cwainspan->span.alarms = ZT_ALARM_RED;
	    zt_alarm_notify(&cwainspan->span);
	    res = 1;
	}

	cwainspan->sync = sync_ok;
	if (sync_ok) {
	    switch (cwainspan->type) {
		case 0xb553:
		    sprintf(cwainspan->span.desc,"Junghanns.NET singleE1 PCI ISDN Card %d (cardID %d) SYNCED",cwainspan->cardno,cwainspan->cardID);
	    	    break;
    		case 0xb554:
		    sprintf(cwainspan->span.desc,"Junghanns.NET doubleE1 PCI ISDN Card %d (cardID %d) (1 E1 port) SYNCED",cwainspan->cardno,cwainspan->cardID);
		    break;
	    }
	} else {
	    switch (cwainspan->type) {
		case 0xb553:
		    sprintf(cwainspan->span.desc,"Junghanns.NET singleE1 PCI ISDN Card %d (cardID %d) NO SYNC (sync_sta = %#x)",cwainspan->cardno,cwainspan->cardID, sync_sta);
		    break;
    		case 0xb554:
		    sprintf(cwainspan->span.desc,"Junghanns.NET doubleE1 PCI ISDN Card %d (cardID %d) (1 E1 port) NO SYNC (sync_sta = %#x)",cwainspan->cardno,cwainspan->cardID, sync_sta);
	    	    break;
	    }
	}
	cwain_doLEDs(cwainspan);
    }
    return res;
}

int cwain_isr_fifo(struct zt_cwain *cwainspan, unsigned char status) {
    unsigned char irq_foview,fi;

    if (status & 0x80) {
	/* fifo irq */
	irq_foview = cwain_inb(cwainspan,cwain_R_IRQ_OVIEW);
	if (irq_foview & 0x80) {
	    fi = cwain_inb(cwainspan,cwain_R_IRQ_FIFO_BL7);
	    if (fi & 0x80) {
		if (debug > 2)
		    printk(KERN_CRIT "cwain: fifo 31 RX irq for D channel cardID %d\n", cwainspan->cardID);
		cwainspan->drx += 1;		
	    }
	}
	return 1;
    }
    return 0;
}

#ifdef LINUX26
static irqreturn_t cwain_dummy_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#else
static void cwain_dummy_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#endif
    struct zt_cwain_card *cwaintmp = dev_id;
    if (!cwaintmp) {
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }
    if (debug > 3)
	printk(KERN_INFO "cwain: dummy irq\n");
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
}


#ifdef LINUX26
static irqreturn_t cwain_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#else
static void cwain_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#endif
    struct zt_cwain_card *cwaintmp = dev_id;
    unsigned char status, status2, status_tmp, irq_misc, irq_misc2 = 0;
#ifndef RELAXED_LOCKING    
    unsigned long flags;
#endif
    int i = 0;
    int l1event = 0;
    
    if (!cwaintmp || cwaintmp->dead) {
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }
    
#ifdef RELAXED_LOCKING    
    spin_lock(&(cwaintmp->lock));
#else
    spin_lock_irqsave(&(cwaintmp->lock),flags);
#endif
    status = cwain_inb(cwaintmp->span[0],cwain_R_STATUS);

    status2 = 0;

    for (i=0;i<cwaintmp->spans;i++) {
	if (i == 0) {
	    status_tmp = status;
	} else {
	    status_tmp = cwain_inb(cwaintmp->span[i],cwain_R_STATUS);
	    status2 = status_tmp;
	}
	cwain_isr_fifo(cwaintmp->span[i], status_tmp);
    }

    if (!(status & 0x80) && !(status & 0x40)) {
	// it's not us!
#ifdef RELAXED_LOCKING    
	spin_unlock(&(cwaintmp->lock));
#else 
	spin_unlock_irqrestore(&(cwaintmp->lock),flags);
#endif
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }

    // misc irq
    if (status & 0x40) {
	irq_misc = cwain_inb(cwaintmp->span[0],cwain_R_IRQ_MISC);
	if (irq_misc & 0x2)  {
	    /* cwain timer */
	    cwaintmp->ticks++;
	    if (cwaintmp->ticks == 1) {
		for (i=0;i<cwaintmp->spans;i++) {
		    cwain_audio_run(cwaintmp->span[i]);
		}
	    }
	    for (i=0;i<cwaintmp->spans;i++) {
		cwain_isr_run(cwaintmp->span[i], cwaintmp->ticks);
	    }
	    if (cwaintmp->ticks == (cwain_FRAME_SIZE / 8)) {
		cwaintmp->ticks = 0;
	    }
	} 
	if (irq_misc & 0x1) {
	    /* state machine */
	    if (debug > 4)
		printk(KERN_INFO "cwain: state machine irq\n");
	    l1event++;
	}
	if (irq_misc & 0x10) {
    	    if (l1event == 0) {
		/* just in case we missed it */
		for (i=0;i<cwaintmp->spans;i++) {
    	    	    l1event += cwain_isr_sync(cwaintmp->span[i]);
    		}
	    }
	}
    }

    // misc irq
    if (status2 & 0x40) {
	if (cwaintmp->spans == 2) {
	    irq_misc2 = cwain_inb(cwaintmp->span[1],cwain_R_IRQ_MISC);
	}
	if (irq_misc2 & 0x1) {
	    /* state machine 2 */
	    if (debug > 4)
		printk(KERN_INFO "cwain: state machine 2 irq\n");
	    l1event++;
	}
    }

    if (l1event > 0) {
//	printk(KERN_INFO "cwain: l1event %d\n", l1event);
    	for (i=0;i<cwaintmp->spans;i++) {
    	    cwain_isr_sync(cwaintmp->span[i]);
    	}
        if (cwaintmp->spans == 2) {
    	    cwain_check_timing(cwaintmp);
        }
    }

#ifdef RELAXED_LOCKING    
    spin_unlock(&(cwaintmp->lock));
#else
    spin_unlock_irqrestore(&(cwaintmp->lock),flags);
#endif
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
}

static int ztcwain_open(struct zt_chan *chan) {
//    printk(KERN_INFO "cwain: channel %d opened.\n",chan->channo);
#ifndef LINUX26
    MOD_INC_USE_COUNT;
#else
    try_module_get(THIS_MODULE);
#endif
    return 0;
}

static int ztcwain_close(struct zt_chan *chan) {
//    printk(KERN_INFO "cwain: channel %d closed.\n",chan->channo);
#ifndef LINUX26
    MOD_DEC_USE_COUNT;
#else
    module_put(THIS_MODULE);
#endif
    return 0;
}

static int ztcwain_rbsbits(struct zt_chan *chan, int bits) {
    return 0;
}

static int ztcwain_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data) {
        switch(cmd) {
        default:
                return -ENOTTY;
        }
        return 0;
}

static int ztcwain_startup(struct zt_span *span) {
    struct zt_cwain_card *cwaincard = span->pvt;
    struct zt_cwain *cwaintmp;
    unsigned long flags;
    int alreadyrunning;
    int i=0;

//    printk(KERN_INFO "cwain: startup spanno %d offset %d\n", span->spanno, span->offset);

    if (cwaincard == NULL) {
	printk(KERN_CRIT "cwain: cwaincard == NULL!\n");
	return 0;
    }

    cwaintmp = cwaincard->span[span->offset];
    if (cwaintmp == NULL) {
	printk(KERN_CRIT "cwain: cwaintmp == NULL!\n");
	return 0;
    }

    
    alreadyrunning = span->flags & ZT_FLAG_RUNNING;
//    printk(KERN_CRIT "already running %d flags %d\n", alreadyrunning, span->flags);

    if (!alreadyrunning) {
	span->chans[15].flags &= ~ZT_FLAG_HDLC;
	span->chans[15].flags |= ZT_FLAG_BRIDCHAN; /* yes! */
    
	/* setup B channel buffers (8 bytes each) */
	for (i=0; i<15 ; i++) {
	    memset(cwaintmp->rxbuf[i],0x0,sizeof(cwaintmp->rxbuf[i]));
    	    memset(cwaintmp->txbuf[i],0x0,sizeof(cwaintmp->txbuf[i]));

    	    span->chans[i].readchunk = cwaintmp->rxbuf[i];
	    span->chans[i].writechunk = cwaintmp->txbuf[i];
	}
	for (i=16; i<31 ; i++) {
	    memset(cwaintmp->rxbuf[i-1],0x0,sizeof(cwaintmp->rxbuf[i-1]));
    	    memset(cwaintmp->txbuf[i-1],0x0,sizeof(cwaintmp->txbuf[i-1]));
    	    span->chans[i].readchunk = cwaintmp->rxbuf[i-1];
	    span->chans[i].writechunk = cwaintmp->txbuf[i-1];
	}
	/* setup D channel buffer */
    	memset(cwaintmp->dtxbuf,0x0,sizeof(cwaintmp->dtxbuf));
	span->chans[15].writechunk = cwaintmp->dtxbuf;
	cwaintmp->chans[15].maxbytes2transmit = sizeof(cwaintmp->dtxbuf);

	memset(cwaintmp->drxbuf,0x0,sizeof(cwaintmp->drxbuf));
    	span->chans[15].readchunk = cwaintmp->drxbuf;

	span->flags |= ZT_FLAG_RUNNING;
    } else {
	printk(KERN_CRIT "already running\n");
	return 0;
    }

    spin_lock_irqsave(&cwaintmp->lock,flags);
    // irqs off
    cwain_outb(cwaintmp,cwain_R_IRQ_CTRL, 0); 

    /* setup D-FIFO TX */
    cwain_outb(cwaintmp,cwain_R_FIFO,0x1F << 1);
    cwain_waitbusy(cwaintmp);
    cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x2);
    cwain_waitbusy(cwaintmp);
    cwain_outb(cwaintmp,cwain_A_CON_HDLC,0x1);
    cwain_outb(cwaintmp,cwain_A_SUBCH_CFG,0x0);
    cwain_outb(cwaintmp,cwain_A_CHANNEL,0x10 << 1);
    cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x0);

    /* setup D-FIFO RX */
    cwain_outb(cwaintmp,cwain_R_FIFO,(0x1F << 1) | 1);
    cwain_waitbusy(cwaintmp);
    cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x2);
    cwain_waitbusy(cwaintmp);
    cwain_outb(cwaintmp,cwain_A_CON_HDLC,0x1);
    cwain_outb(cwaintmp,cwain_A_SUBCH_CFG,0x0);
    cwain_outb(cwaintmp,cwain_A_CHANNEL,(0x10 << 1) | 1);
    cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x0);

    /* setup B-FIFOs TX */
    /* map ts 1 to 15 to fifos 0 to 14 */
    for (i=1; i<16 ; i++) {
	cwain_outb(cwaintmp,cwain_R_FIFO,(i - 1) << 1);
	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x2);
	cwain_waitbusy(cwaintmp);
	cwain_outb(cwaintmp,cwain_A_CON_HDLC,0x2);
	cwain_outb(cwaintmp,cwain_A_CHANNEL,i << 1);
	cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x1);
    }
    /* map ts 17 to 31 to fifos 15 to 29 */
    for (i=17; i<32 ; i++) {
	cwain_outb(cwaintmp,cwain_R_FIFO,(i - 2) << 1);
	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x2);
	cwain_waitbusy(cwaintmp);
	cwain_outb(cwaintmp,cwain_A_CON_HDLC,0x2);
	cwain_outb(cwaintmp,cwain_A_CHANNEL,i << 1);
	cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x1);
    }

    /* setup B-FIFOs RX */
    /* map ts 1 to 15 to fifos 0 to 14 */
    for (i=1; i<16 ; i++) {
	cwain_outb(cwaintmp,cwain_R_FIFO,((i-1) << 1) | 1);
    	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x2);
    	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_A_CON_HDLC,0x2);
    	cwain_outb(cwaintmp,cwain_A_CHANNEL,(i << 1) | 1);
    	cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x1);
    }
    /* map ts 17 to 31 to fifos 15 to 29 */
    for (i=17; i<32 ; i++) {
	cwain_outb(cwaintmp,cwain_R_FIFO,((i-2) << 1) | 1);
    	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_R_INC_RES_FIFO,0x2);
    	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_A_CON_HDLC,0x2);
    	cwain_outb(cwaintmp,cwain_A_CHANNEL,(i << 1) | 1);
    	cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x1);
    }

    if (debug)
        printk(KERN_INFO "cwain: starting card %d span %d/%d.\n",cwaintmp->cardno,span->spanno,span->offset);

    if (cwaincard->spans == 1) {
	cwain_set_master(cwaincard, 0);
    }
    
    /* setup E1 amplitude */
    cwain_outb(cwaintmp,cwain_R_PWM_MD,0x20);
    cwain_outb(cwaintmp,cwain_R_PWM0,0x50);
    cwain_outb(cwaintmp,cwain_R_PWM1,0xff);

    /* setup E1 transceiver */
    cwain_outb(cwaintmp,cwain_R_TX_SL0,0xf8);  // R_TX_FR1
    cwain_outb(cwaintmp,cwain_R_TX_SL0_CFG0,0x00); /* semiautomatic mode */

    cwain_outb(cwaintmp,cwain_R_RX_SL0_CFG0,0x6); /* 0x26 */

    if (cwaintmp->span.lineconfig & ZT_CONFIG_AMI) {
	cwain_outb(cwaintmp,cwain_R_TX0,0x82);
	cwain_outb(cwaintmp,cwain_R_RX0,0x02);
    } else if (cwaintmp->span.lineconfig & ZT_CONFIG_HDB3) {
	cwain_outb(cwaintmp,cwain_R_TX0,0x81);
	cwain_outb(cwaintmp,cwain_R_RX0,0x01);
    }

    /* transmitter mode */
    cwain_outb(cwaintmp,cwain_R_TX1,0x60); 

    cwain_outb(cwaintmp,cwain_R_LOS0,0x10);
    cwain_outb(cwaintmp,cwain_R_LOS1,0x10);

    if (cwaintmp->span.lineconfig & ZT_CONFIG_CRC4) {
	/* crc4 multiframe */
	cwain_outb(cwaintmp,cwain_R_TX_SL0_CFG1,0x31);
//	cwain_outb(cwaintmp,cwain_R_RX_SL0_CFG1,0x41);
	cwain_outb(cwaintmp,cwain_R_RX_SL0_CFG1,0x03);
    } else {
	/* doubleframe */
	cwain_outb(cwaintmp,cwain_R_TX_SL0_CFG1,0x0);
//	cwain_outb(cwaintmp,cwain_R_RX_SL0_CFG1,0x40);
	cwain_outb(cwaintmp,cwain_R_RX_SL0_CFG1,0x02);
    }

    
    /* setup sync mode */    
    if (cwaincard->syncs[span->offset] > 0) {
	cwain_outb(cwaintmp,cwain_R_SYNC_CTRL,0x2);
	cwain_outb(cwaintmp,cwain_R_SYNC_OUT,0xe0);
	/* layer 1, here we go! */
	cwain_outb(cwaintmp,cwain_R_E1_WR_STA,0x00);
    } else {
	cwain_outb(cwaintmp,cwain_R_SYNC_CTRL,0x5);
	cwain_outb(cwaintmp,cwain_R_SYNC_OUT,0xe0);
	/* layer 1, up! */
	cwain_outb(cwaintmp,cwain_R_E1_WR_STA,0x11);
    }
    
    cwaintmp->sync = 0;
    cwaintmp->sync_sta = 0;
    
    /* enable irqs */
    if (!span->offset) {
	cwain_outb(cwaintmp,cwain_R_IRQ_CTRL, 8 | 1); 
    } else {
	cwain_outb(cwaintmp,cwain_R_IRQ_CTRL, 0x1); 
    }
    spin_unlock_irqrestore(&cwaintmp->lock,flags);
    return 0;
}

static int ztcwain_shutdown(struct zt_span *span) {
    struct zt_cwain_card *cwaincard = span->pvt;
    struct zt_cwain *cwaintmp;
    unsigned long flags;
    int alreadyrunning;
    int i=0;
    
    if (cwaincard == NULL) {
	printk(KERN_CRIT "cwain: cwaincard == NULL!\n");
	return 0;
    }

    cwaintmp = cwaincard->span[span->offset];
    if (cwaintmp == NULL) {
	printk(KERN_CRIT "cwain: cwaintmp == NULL!\n");
	return 0;
    }

    alreadyrunning = span->flags & ZT_FLAG_RUNNING;
    
    if (!alreadyrunning) {
	return 0;
    }
    spin_lock_irqsave(&cwaintmp->lock,flags);

//    printk(KERN_CRIT "cwain: stopping card %d span %d/%d.\n",cwaintmp->cardno,span->spanno,span->offset);

    // turn off irqs for all fifos

    /* disable FIFO TX */
    for (i=0; i<0x20 ; i++) {
	cwain_outb(cwaintmp,cwain_R_FIFO,i << 1);
	cwain_waitbusy(cwaintmp);
	cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x0);
    }

    /* disable FIFO RX */
    for (i=0; i<0x20 ; i++) {
	cwain_outb(cwaintmp,cwain_R_FIFO,(i << 1) | 1);
    	cwain_waitbusy(cwaintmp);
    	cwain_outb(cwaintmp,cwain_A_IRQ_MSK,0x0);
    }


    /* Deactivate Layer 1 */
    cwain_outb(cwaintmp,cwain_R_E1_WR_STA,0x10);

    cwain_outb(cwaintmp,cwain_R_IRQ_CTRL, 0); 
//    cwain_outb(cwaintmp,cwain_R_IRQMSK_MISC, 0); 
    cwain_inb(cwaintmp,cwain_R_STATUS);


    span->flags &= ~ZT_FLAG_RUNNING;


    spin_unlock_irqrestore(&cwaintmp->lock,flags);

//    printk(KERN_CRIT "cwain: card %d span %d/%d down.\n",cwaintmp->cardno,span->spanno,span->offset);
    return 0;
}

static int ztcwain_maint(struct zt_span *span, int cmd) {
    return 0;
}

static int ztcwain_chanconfig(struct zt_chan *chan,int sigtype) {
//    printk(KERN_INFO "chan_config sigtype=%d\n", sigtype);
    return 0;
}

static int ztcwain_spanconfig(struct zt_span *span,struct zt_lineconfig *lc) {
    struct zt_cwain_card *cwaincard = span->pvt;
    span->lineconfig = lc->lineconfig;
    span->syncsrc = lc->sync;

    cwaincard->syncs[span->offset] = lc->sync;
    cwaincard->syncsrc = -1;
//    printk(KERN_INFO "span_config %d lineconfig=%d syncsrc=%d\n", span->spanno, lc->lineconfig, lc->sync);
//    cwain_check_timing(cwaincard);
    return 0;
}

static int ztcwain_initialize(struct zt_cwain *cwaintmp, struct zt_cwain_card *cwaincard, int offset) {
    int i=0;
    
	memset(&cwaintmp->span,0,sizeof(struct zt_span)); // you never can tell...
	sprintf(cwaintmp->span.name,"cwain/%d",cwaintmp->cardno);
	switch (cwaintmp->type) {
	    case 0xb553:
		sprintf(cwaintmp->span.desc,"Junghanns.NET singleE1 PCI ISDN Card %d (cardID %d)",cwaintmp->cardno,cwaintmp->cardID);
		break;
	    case 0xb554:
		sprintf(cwaintmp->span.desc,"Junghanns.NET doubleE1 PCI ISDN Card %d (cardID %d) (1 E1 port)",cwaintmp->cardno,cwaintmp->cardID);
		break;
	    default:
		return -1;
	}
        cwaintmp->span.spanconfig = ztcwain_spanconfig;
        cwaintmp->span.chanconfig = ztcwain_chanconfig;
        cwaintmp->span.startup = ztcwain_startup;
        cwaintmp->span.shutdown = ztcwain_shutdown;
        cwaintmp->span.maint = ztcwain_maint;
        cwaintmp->span.rbsbits = ztcwain_rbsbits;
        cwaintmp->span.open = ztcwain_open;
        cwaintmp->span.close = ztcwain_close;
        cwaintmp->span.ioctl = ztcwain_ioctl;

        cwaintmp->span.chans = cwaintmp->chans;
        cwaintmp->span.channels = 31;
        cwaintmp->span.deflaw = ZT_LAW_ALAW;
        cwaintmp->span.linecompat = ZT_CONFIG_AMI | ZT_CONFIG_HDB3 | ZT_CONFIG_CCS; 
        init_waitqueue_head(&cwaintmp->span.maintq);
        cwaintmp->span.pvt = cwaincard;
        cwaintmp->span.offset = offset;

	for (i=0; i < cwaintmp->span.channels; i++) {
	    memset(&(cwaintmp->chans[i]),0x0,sizeof(struct zt_chan));
	    sprintf(cwaintmp->chans[i].name,"cwain%d/%d",cwain_span_count + 1,i + 1);
	    cwaintmp->chans[i].pvt = cwaintmp;
	    cwaintmp->chans[i].sigcap =  ZT_SIG_CLEAR;
	    cwaintmp->chans[i].chanpos = i + 1; 
	}

	if (zt_register(&cwaintmp->span,0)) {
	    printk(KERN_INFO "cwain: unable to register zaptel span!\n");
	    return -1;
	}
//	 printk(KERN_INFO "cwain: registered zaptel span %d.\n",s+1);

    return 0;
}

int cwain_reset_card(struct zt_cwain_card *cwaintmp) {
    unsigned long flags;
    int i = 0;

    cwaintmp->irq = cwaintmp->span[0]->irq;
    
    if (cwaintmp->spans == 2) {
	if (request_irq(cwaintmp->irq, cwain_interrupt, SA_INTERRUPT | SA_SHIRQ, "cwain2", cwaintmp)) {
    	    printk(KERN_WARNING "cwain: unable to register irq\n");
	    return -1;
	}
	if (request_irq(cwaintmp->span[1]->irq, cwain_dummy_interrupt, SA_INTERRUPT | SA_SHIRQ, "cwaindummy", cwaintmp)) {
    	    printk(KERN_WARNING "cwain: unable to register irq\n");
	    return -1;
	}
    } else {
	if (request_irq(cwaintmp->irq, cwain_interrupt, SA_INTERRUPT | SA_SHIRQ, "cwain", cwaintmp)) {
    	    printk(KERN_WARNING "cwain: unable to register irq\n");
	    return -1;
	}
    }

    spin_lock_irqsave(&(cwaintmp->lock),flags);

    for (i=0;i<cwaintmp->spans;i++) {
	cwain_reset_span(cwaintmp->span[i]);
    }

    /* no master yet, force reelection */
    cwaintmp->syncsrc = -1;
    
    /* set up the timer 1 khz, zaptel timing */
    cwain_outb(cwaintmp->span[0],cwain_R_TI_WD, 0x2);

    if (cwaintmp->spans == 2) {
//	cwain_outb(cwaintmp->span[1],cwain_R_IRQMSK_MISC, 0x1); 
	cwain_outb(cwaintmp->span[1],cwain_R_IRQMSK_MISC, 0x0); 
    }
    /* enable timer interrupts */
    cwain_outb(cwaintmp->span[0],cwain_R_IRQMSK_MISC, 0x13); 

    /* Finally enable IRQ output */
//    cwain_outb(cwaintmp->span[0],cwain_R_IRQ_CTRL, 0x8 | 0x1); 

    spin_unlock_irqrestore(&(cwaintmp->lock),flags);
    return 0;
}

int cwain_find_spans(unsigned int pcidid) {
    struct pci_dev *tmp;
    struct zt_cwain *cwaintmp = NULL;
    int i=0;
    unsigned char dips=0;
    int cid=0;
    int modes=0;
    tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,multi_cwain);
    while (tmp != NULL) {
	multi_cwain = tmp;	// skip this next time.

	if (pci_enable_device(tmp)) {
	    multi_cwain = NULL;
	    return -1;
	}

	cwaintmp = kmalloc(sizeof(struct zt_cwain),GFP_KERNEL);
	if (!cwaintmp) {
	    printk(KERN_WARNING "cwain: unable to kmalloc!\n");
	    pci_disable_device(tmp);
	    multi_cwain = NULL;
	    return -ENOMEM;
	}
	memset(cwaintmp, 0x0, sizeof(struct zt_cwain));
	
	spin_lock_init(&cwaintmp->lock);
	cwaintmp->pcidev = tmp;
	cwaintmp->pcibus = tmp->bus->number;
	cwaintmp->pcidevfn = tmp->devfn; 


	cwaintmp->pci_io_phys = (char *) tmp->resource[1].start;
	if (!cwaintmp->pci_io_phys) {
	    printk(KERN_WARNING "cwain: no iomem!\n");
	    pci_disable_device(tmp);
	    multi_cwain = NULL;
	    return -EIO;
	}

	if (!tmp->irq) {
	    printk(KERN_WARNING "cwain: PCI device has no irq!\n");
	    pci_disable_device(tmp);
	    multi_cwain = NULL;
	    return -EIO;
	}

	cwaintmp->ioport = tmp->resource[0].start;
	if (!cwaintmp->ioport) {
	    printk(KERN_WARNING "cwain: no ioport!\n");
	    pci_disable_device(tmp);
	    multi_cwain = NULL;
	    return -EIO;
	}
	if (!request_region(cwaintmp->ioport, 8, "cwain")) {
	    printk(KERN_WARNING "cwain: couldnt request io range!\n");
	    pci_disable_device(tmp);
	    multi_cwain = NULL;
	    return -EIO;
	}

	if (!request_mem_region((unsigned long) cwaintmp->pci_io_phys, 256, "cwain")) {
	    printk(KERN_WARNING "cwain: couldnt request io mem range!\n");
	    release_region(cwaintmp->ioport, 8);
	    pci_disable_device(tmp);
	    multi_cwain = NULL;
	    return -EIO;
	}

        cwaintmp->irq = tmp->irq;

	cwaintmp->pci_io = ioremap((ulong) cwaintmp->pci_io_phys, 256);
			       
	/* enable memio */
	pci_write_config_word(cwaintmp->pcidev, PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_IO);	

	/* disable interrupts */
	cwain_outb(cwaintmp,cwain_R_IRQ_CTRL, 0); 

        if (((tmp->subsystem_device==0xb553) || (tmp->subsystem_device==0xb554))&& (pcidid == PCI_DEVICE_ID_CCD_E)) {
	    dips = (cwain_inb(cwaintmp,cwain_R_GPI_IN0) >> 5);
	    cid = 7;
	    for (i=0;i<3;i++) {
	        if ((dips & (1 << i)) != 0) {
	    	cid -= (1 << (2-i));
	        }
	    }
//		printk(KERN_INFO "DIPS = %#x CID= %#x\n",dips,cid);
        } else {
	    cid = 0xff;
        }

	if (ports == -1) {
    	    if ((tmp->subsystem_device==0xb520) && (pcidid == PCI_DEVICE_ID_CCD_E)) {
		modes = (cwain_inb(cwaintmp,cwain_R_GPI_IN0) >> 4) & 0x01;
	    } else {
		modes = 0; // assume TE mode
	    }
	} else {
	    modes = ports >> cwain_span_count;
	}


	cwaintmp->cardID = cid;
	cwaintmp->type = tmp->subsystem_device;

	if ((modes & 1) != 0) {
	    cwaintmp->nt_mode = 1;
	} else {
	    cwaintmp->nt_mode = 0;
	}

	cwain_register_span(cwaintmp);

	tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,multi_cwain);
    }
    return 0;
}


int cwain_sort_cards(void) {
    int changed=0,tmpcardno;
    struct zt_cwain_card *tmpcard,*tmpcard2;
    spin_lock(&cwain_card_registerlock);
    do {
	changed = 0;
	tmpcard = cwain_card_list;
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
			cwain_card_list = tmpcard;
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
    spin_unlock(&cwain_card_registerlock);
    return 0;
}

int cwain_zap_cards(void) {
    struct zt_cwain_card *tmpcard = cwain_card_list;
    int i=0;
    int res=0;
    while (tmpcard != NULL) {
	for (i=0; i<tmpcard->spans; i++) {
	    ztcwain_initialize(tmpcard->span[i], tmpcard, i);
	}
	res = cwain_reset_card(tmpcard);
	tmpcard = tmpcard->next;
    }
    return res;
}


int init_module(void) {
    multi_cwain = NULL;
    cwain_find_spans(PCI_DEVICE_ID_CCD_E);
    cwain_sort_cards();
    cwain_zap_cards();
    if (cwain_card_count == 0) {
	printk(KERN_INFO "cwain: no cwain cards found.\n");
    } else {
	printk(KERN_INFO "cwain: %d cwain card(s) in this box, %d E1 ports total.\n", cwain_card_count, cwain_span_count);
    }
    return 0;
}

void cleanup_module(void) {
    struct zt_cwain_card *tmpcard,*tmplist;
    struct zt_cwain *tmpspan,*spanlist;
    int i=0;
    int j=0;
    
    tmplist = cwain_card_list;
    tmpcard = NULL;
    while (tmplist) {
	tmpcard = tmplist;
	tmplist = tmplist->next;

	tmpcard->dead = 1;
	cwain_shutdown_card(tmpcard);
	kfree(tmpcard);
	i++;
    }

    spanlist = cwain_span_list;
    tmpspan = NULL;
    while (spanlist) {
	tmpspan = spanlist;
	spanlist = spanlist->next;
	kfree(tmpspan);
	j++;
    }
    printk(KERN_INFO "cwain: shutdown %d spans, %d cwain cards.\n", j, i);
}
#endif

#ifdef LINUX26
module_param(ports, int, 0600);
module_param(debug, int, 0600);
#else
MODULE_PARM(ports,"i");
MODULE_PARM(debug,"i");
#endif

MODULE_DESCRIPTION("cwain zaptel driver");
MODULE_AUTHOR("Klaus-Peter Junghanns <kpj@junghanns.net>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif	
