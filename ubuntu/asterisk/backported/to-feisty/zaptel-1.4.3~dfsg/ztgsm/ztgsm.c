/*
 * ztgsm.c - Zaptel driver for the uno/duo/quad GSM PCI cards
 *
 * Copyright (C) 2005, 2006 Junghanns.NET GmbH
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
#include "ztgsm.h"

#ifdef LINUX26
#include <linux/moduleparam.h>
#endif

#if CONFIG_PCI

static int debug=0;
static int pcm_xbar=0;
static struct ztgsm_card *ztgsm_dev_list = NULL;
static int ztgsm_dev_count = 0;
static int ztgsm_spans = 0;
static struct pci_dev *multi_gsm = NULL;
static spinlock_t registerlock = SPIN_LOCK_UNLOCKED;

void ztgsm_init_xbar(struct ztgsm_card *gsmtmp) {
    int i = 0;
    for (i=0; i <= 0x01FF; i++) {
	ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x00000182 | i);
    }
}

void ztgsm_switch_on(struct ztgsm_card *gsmtmp, int span) {
    unsigned int dtr_on_off = 0;
    unsigned int rts_o = 0;
    unsigned long flags;
    
    printk(KERN_INFO "ztgsm: Powering up span %d ...", span);
    spin_lock_irqsave(&(gsmtmp->lock),flags);
	gsmtmp->gsmspan[span].led = 0xC1;
	dtr_on_off = ztgsm_indw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF);
	dtr_on_off |= (1 << span) | (1 << (span+4));
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, dtr_on_off);
    
	rts_o = ztgsm_indw_io(gsmtmp, ztgsm_SER_RTS_O);
	rts_o |= (1 << span);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_RTS_O, rts_o);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((1000 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	dtr_on_off = ztgsm_indw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF);
        dtr_on_off &= ~(1 << span);
        dtr_on_off |= 1 << (span+4);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, dtr_on_off);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((800 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	dtr_on_off = ztgsm_indw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF);
	dtr_on_off |= (1 << span) | (1 << (span+4));
        ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, dtr_on_off);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((8000 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	dtr_on_off = ztgsm_indw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF);
	dtr_on_off &= ~(1 << (span+4));
        dtr_on_off |= (1 << span);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, dtr_on_off);

        rts_o = ztgsm_indw_io(gsmtmp, ztgsm_SER_RTS_O);
	rts_o &= ~(1 << span);
        ztgsm_outdw_io(gsmtmp, ztgsm_SER_RTS_O, 0x0);
	gsmtmp->power[span] = 1;
	gsmtmp->gsmspan[span].led = 0x81;
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    printk(" done.\n");
}

void ztgsm_switch_off(struct ztgsm_card *gsmtmp, int span) {
    unsigned int dtr_on_off = 0;
    unsigned long flags;

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	gsmtmp->gsmspan[span].led = 0xC0;
	dtr_on_off = ztgsm_indw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF);
	printk(KERN_INFO "ztgsm: Powering down span %d (SER_DTR_ON_OFF %x)...", span, dtr_on_off);
    
	dtr_on_off &= ~ (1 << span);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, dtr_on_off);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((5000 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	dtr_on_off = ztgsm_indw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF);
        dtr_on_off |= (1 << span);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, dtr_on_off);
	gsmtmp->power[span] = 0;
	gsmtmp->gsmspan[span].led = 0x80;
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);
    printk(" done.\n");
}

void ztgsm_switch_on_all(struct ztgsm_card *gsmtmp) {
    unsigned long flags;
    printk(KERN_INFO "ztgsm: Powering up all spans...");
    spin_lock_irqsave(&(gsmtmp->lock),flags);
	gsmtmp->gsmspan[0].led = 0xC1;
        gsmtmp->gsmspan[1].led = 0xC1;
	gsmtmp->gsmspan[2].led = 0xC1;
        gsmtmp->gsmspan[3].led = 0xC1;

	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0xff);
    
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_RTS_O, 0x0f);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((1000 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0xf0);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((800 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0xff);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((8000 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0xff);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_RTS_O, 0x0); /* 1 == -12 v */
    
/* new
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0x00);

    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout((1000 * HZ) / 1000);

    ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0x0f);
*/    

	gsmtmp->power[0] = 1;
	gsmtmp->power[1] = 1;
        gsmtmp->power[2] = 1;
	gsmtmp->power[3] = 1;

	gsmtmp->gsmspan[0].led = 0x81;
	gsmtmp->gsmspan[1].led = 0x81;
	gsmtmp->gsmspan[2].led = 0x81;
	gsmtmp->gsmspan[3].led = 0x81;
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

    printk(" done.\n");
}

void ztgsm_switch_off_all(struct ztgsm_card *gsmtmp) {
    unsigned long flags;

    if (gsmtmp->power[0] || gsmtmp->power[1] || gsmtmp->power[2] || gsmtmp->power[3]) { 
    spin_lock_irqsave(&(gsmtmp->lock),flags);
	gsmtmp->gsmspan[0].led = 0xC0;
        gsmtmp->gsmspan[1].led = 0xC0;
	gsmtmp->gsmspan[2].led = 0xC0;
        gsmtmp->gsmspan[3].led = 0xC0;
	
	printk(KERN_INFO "ztgsm: Powering down all spans...");
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0x0);
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);

	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout((5000 * HZ) / 1000);

    spin_lock_irqsave(&(gsmtmp->lock),flags);
	ztgsm_outdw_io(gsmtmp, ztgsm_SER_DTR_ON_OFF, 0xf);
	printk(" done.\n");

	gsmtmp->power[0] = 0;
	gsmtmp->power[1] = 0;
        gsmtmp->power[2] = 0;
	gsmtmp->power[3] = 0;

	gsmtmp->gsmspan[0].led = 0x80;
	gsmtmp->gsmspan[1].led = 0x80;
        gsmtmp->gsmspan[2].led = 0x80;
	gsmtmp->gsmspan[3].led = 0x80;
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);
    }
}


void ztgsm_shutdownCard(struct ztgsm_card *gsmtmp) {
    unsigned long flags;
    struct ztgsm_span *gsmspan = NULL;
    int i = 0;
    if (gsmtmp == NULL) {
	printk(KERN_INFO "ztgsm: shutting down NULL card!\n");
	return;
    }

    ztgsm_switch_off_all(gsmtmp);

    spin_lock_irqsave(&gsmtmp->lock,flags);

    gsmtmp->dead = 1;

    if ((!gsmtmp->pci_io) || (gsmtmp->ioport == 0)) {
	return;
    }
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_INT_MASK, 0x0);
    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_FC_TOG_BIT, 0x0);
    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_SAP_EN, 0x0);


/*    for (i=0; i < gsmtmp->gsmspans; i++) {
	ztgsm_switch_off(gsmtmp, i);
    }  */


    ztgsm_outdw_io(gsmtmp, ztgsm_LED_DUAL, 0xFF00);


    // softreset

    release_region(gsmtmp->ioport, 0x100);
    iounmap((void *) gsmtmp->pci_io);
    release_mem_region(gsmtmp->pci_io_phys, gsmtmp->iomem_size);

    gsmtmp->pci_io = 0;
    gsmtmp->ioport = 0;

    // turn off irqs

    free_irq(gsmtmp->irq,gsmtmp);

    pci_write_config_word(gsmtmp->pcidev, PCI_COMMAND, 0);	

    if (gsmtmp->pcidev != NULL) {
        pci_disable_device(gsmtmp->pcidev);
    }
    
    spin_unlock_irqrestore(&gsmtmp->lock,flags);

    for (i=0; i < gsmtmp->gsmspans; i++) {
	gsmspan = &gsmtmp->gsmspan[i];
        if(gsmspan->span.flags & ZT_FLAG_RUNNING) {
//	    ztgsm_shutdown(&gsmspan->span);
	    if (debug)
	        printk(KERN_INFO "ztgsm: shutdown card %d span %d.\n",gsmtmp->cardno,i);
	}
        if(gsmspan->span.flags & ZT_FLAG_REGISTERED) {
	    zt_unregister(&gsmspan->span);
	    if (debug)
	        printk(KERN_INFO "ztgsm: unregistered card %d span %d.\n",gsmtmp->cardno,i);
	}
    }

}

void ztgsm_register_card(struct ztgsm_card *gsmtmp) {
    spin_lock(&registerlock);
    if (gsmtmp != NULL) {
	gsmtmp->prev = NULL;
	gsmtmp->next = ztgsm_dev_list;
	if (ztgsm_dev_list) {
	    ztgsm_dev_list->prev = gsmtmp;
	}
	ztgsm_dev_list = gsmtmp;
	gsmtmp->cardno = ++ztgsm_dev_count;
    } else {
	printk(KERN_INFO "ztgsm: trying to register NULL card.\n");
    }
    spin_unlock(&registerlock);
}


void ztgsm_resetCard(struct ztgsm_card *gsmtmp) {
    unsigned long flags;
    spin_lock_irqsave(&(gsmtmp->lock),flags);
//    pci_write_config_word(gsmtmp->pcidev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY);
    
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_CLK_DIV, 0x12);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_CLK_PRE_DIV, 0x06);
//    ztgsm_outdw_io(gsmtmp, ztgsm_SER_CLK_PRE_DIV, 0x03);
//    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_CLK_PRE_DIV, 0x06);
    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_CLK_PRE_DIV, 0x0C);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_INT_MASK, 0x0);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_IDLE_VAL, 0x01);

    ztgsm_outdw_io(gsmtmp, ztgsm_SER_TX_EN, 0x0);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_RX_EN, 0x0);


    gsmtmp->ticks = 0;
    gsmtmp->clicks = 0;

    spin_unlock_irqrestore(&(gsmtmp->lock),flags);
}

void ztgsm_startCard(struct ztgsm_card *gsmtmp) {
    unsigned long flags;
    spin_lock_irqsave(&(gsmtmp->lock),flags);
    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_FC_TOG_BIT, 0x03);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_INT_MASK, 0x1FFFF);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_RX_WATERMARK, 0x0A);
    ztgsm_outdw_io(gsmtmp, ztgsm_SIM_SEL, 0x0);
    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_SAP_EN, 0x0);
    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_MASK_LAST, 0x001F0000 | 0x1F); /* PCM32 0x100 frames */
//    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_MASK_LAST, (0x001FC000 | 0x1F)); /* PCM32 0x40 frames */

    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_SAP_EN, 0x1F);
    switch (gsmtmp->gsmspans) {
	case 4:
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01010186); /* GSM_A (alaw) -> pci slot 0x01 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01840101); /* pci slot 0x01 -> GSM_A */

	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x0103018A); /* GSM_B (alaw) -> pci slot 0x03 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01880103); /* pci slot 0x03 -> GSM_B */

	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x0105018E); /* GSM_C (alaw) -> pci slot 0x05 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x018C0105); /* pci slot 0x05 -> GSM_C */

	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01070192); /* GSM_D (alaw) -> pci slot 0x07 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01900107); /* pci slot 0x07 -> GSM_D */
	    break;
	case 2:
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01010186); /* GSM_A (alaw) -> pci slot 0x01 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01840101); /* pci slot 0x01 -> GSM_A */

	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x0103018A); /* GSM_B (alaw) -> pci slot 0x03 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01880103); /* pci slot 0x03 -> GSM_B */
	    break;
	case 1:
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01010186); /* GSM_A (alaw) -> pci slot 0x01 */
	    ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG, 0x01840101); /* pci slot 0x01 -> GSM_A */
	    break;
    }

    ztgsm_outdw_io(gsmtmp, ztgsm_SER_RX_EN, 0x0F);
    ztgsm_outdw_io(gsmtmp, ztgsm_SER_TX_EN, 0x0F);
    if (debug)
	printk(KERN_INFO "ztgsm: SER_TX_EN %d SER_RX_EN %d \n", ztgsm_indw_io(gsmtmp, ztgsm_SER_TX_EN), ztgsm_indw_io(gsmtmp, ztgsm_SER_RX_EN));
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);
}

static void ztgsm_xbar(struct ztgsm_card *gsmtmp, int dst, int src, int slin) {
    unsigned long flags;
    int format = 0;
    
    if (!gsmtmp) return;

    if (slin) format = 0x2000;
    spin_lock_irqsave(&(gsmtmp->lock), flags);
	if (debug > 1)
	    printk(KERN_INFO "ztgsm: ztgsm_PCM_VECTOR_CFG %4x \n", (dst << 16) | src | format);
	ztgsm_outdw_io(gsmtmp, ztgsm_PCM_VECTOR_CFG,  (dst << 16) | src | format);
    spin_unlock_irqrestore(&(gsmtmp->lock), flags);
}

static int ztgsm_dacs(struct zt_chan *dst, struct zt_chan *src) {
    struct ztgsm_span *src_span;
    struct ztgsm_span *dst_span;
    
    dst_span = dst->pvt;
    if (dst->chanpos == 2) return -1;

    if (src) {
	if (src->chanpos == 2) return -1;
	src_span = src->pvt;
	if (debug > 1)
	    printk(KERN_INFO "linking channel %d span %d to channel %d span %d\n", src->chanpos, src_span->span.offset, dst->chanpos, dst_span->span.offset);
	/* ALAW */
	ztgsm_xbar(dst->span->pvt, 0x184 + (4 * dst_span->span.offset), 0x186 + (4 * src_span->span.offset), 0);

	/* SLIN */
/*	ztgsm_xbar(dst->span->pvt, 0x184 + (4 * dst_span->span.offset), 0x184 + (4 * src_span->span.offset), 1);
	ztgsm_xbar(dst->span->pvt, 0x185 + (4 * dst_span->span.offset), 0x185 + (4 * src_span->span.offset), 1); */

    } else {
	if (debug > 1)
	    printk(KERN_INFO "unlinking channel %d span %d\n", dst->chanpos, dst_span->span.offset);
	/* reassign pci source */
	ztgsm_xbar(dst->span->pvt, 0x184 + (4 * dst_span->span.offset), 0x100 | ((dst_span->span.offset * 2)+1), 0);
	/* reassign pci destination */
	ztgsm_xbar(dst->span->pvt, 0x100 | ((dst_span->span.offset * 2)+1), 0x186 + (4 * dst_span->span.offset), 0);
    }

    return 0;
}

static int ztgsm_ser_rx(struct ztgsm_card *gsmtmp, int span) {
    unsigned int rxcreg = 0;
    unsigned int rxdreg = 0;
    int rx_count = 0;
    int i = 0;
    unsigned char data = 0;
    unsigned int count_reg = 0;
    int rd_ptr = 0;
    int wr_ptr = 0;
    switch (span) {
	case 0:	rxcreg = ztgsm_SER_RX_COUNT_A;
		rxdreg = ztgsm_SER_RX_DATA_A;
		break;
	case 1:	rxcreg = ztgsm_SER_RX_COUNT_B;
		rxdreg = ztgsm_SER_RX_DATA_B;
		break;
	case 2:	rxcreg = ztgsm_SER_RX_COUNT_C;
		rxdreg = ztgsm_SER_RX_DATA_C;
		break;
	case 3:	rxcreg = ztgsm_SER_RX_COUNT_D;
		rxdreg = ztgsm_SER_RX_DATA_D;
		break;
    }
    count_reg = ztgsm_indw_io(gsmtmp, rxcreg);
    rx_count = count_reg & 0x1F;
    if (rx_count) {
	rd_ptr = (count_reg & 0x03E0) >> 5;
	wr_ptr = (count_reg & 0x7C00) >> 10;
//	if (debug) 
//	    printk(KERN_CRIT "ztgsm: SER_RX_COUNT_%d [wr_ptr %d rd_ptr %d count %d]\n", span, wr_ptr, rd_ptr, rx_count);
	if ((gsmtmp->ser_rx_idx[span] + rx_count) < ztgsm_SER_BUF_SIZE) {
	    if (debug) printk(KERN_CRIT "ztgsm: SER_RX span %d [", span);
	    for (i=0;i<rx_count; i++) {
		data = ztgsm_indw_io(gsmtmp, rxdreg);
		if (debug) printk(" %#x", data);
		if (gsmtmp->gsmspan[span].span.flags & ZT_FLAG_RUNNING)
		    gsmtmp->ser_rx_buf[span][gsmtmp->ser_rx_idx[span]++] = data;
	    }
    	    if (debug) printk("]\n");
	} else {
	    printk(KERN_INFO "ztgsm: RX buffer overflow on span %d\n", span);
	}
    }
    return rx_count;
}

static int ztgsm_zap_rx(struct ztgsm_card *gsmtmp, int span) {
    int i = 0;
        
    if (gsmtmp->ser_rx_idx[span]) {
	memcpy(gsmtmp->drxbuf[span], &gsmtmp->ser_rx_buf[span], gsmtmp->ser_rx_idx[span]);
	if (debug) {
	    printk(KERN_INFO "ztgsm: span %d RX [ \n", span);
	    for (i=0;i<gsmtmp->ser_rx_idx[span]; i++) {
		if (gsmtmp->ser_rx_buf[span][i] != 0x0d)
		    printk("%c", gsmtmp->ser_rx_buf[span][i]);
	    }
    	    printk("]\n");
	}
	gsmtmp->gsmspan[span].chans[1].eofrx = 1;
	gsmtmp->gsmspan[span].chans[1].bytes2receive = gsmtmp->ser_rx_idx[span];
	gsmtmp->ser_rx_idx[span] = 0;
    }
    return 0;
}

static int ztgsm_ser_tx(struct ztgsm_card *gsmtmp, int span) {
    unsigned int txcreg = 0;
    unsigned int txdreg = 0;
    unsigned int tx_wm_sen = 0;
    int left = 0;
    int i = 0;
    int count = 0;
    unsigned int count_reg = 0;
    int rd_ptr = 0;
    int wr_ptr = 0;
    struct ztgsm_span *gsmspan = NULL;

    switch (span) {
	case 0:	txcreg = ztgsm_SER_TX_COUNT_A;
		txdreg = ztgsm_SER_TX_DATA_A;
		break;
	case 1:	txcreg = ztgsm_SER_TX_COUNT_B;
		txdreg = ztgsm_SER_TX_DATA_B;
		break;
	case 2:	txcreg = ztgsm_SER_TX_COUNT_C;
		txdreg = ztgsm_SER_TX_DATA_C;
		break;
	case 3:	txcreg = ztgsm_SER_TX_COUNT_D;
		txdreg = ztgsm_SER_TX_DATA_D;
		break;
    }
    gsmspan = &gsmtmp->gsmspan[span];
    if (gsmspan) {
	count_reg = ztgsm_indw_io(gsmtmp, txcreg);
	left = ztgsm_FIFO_SIZE - (count_reg & 0x1F);
	if (left >= 1 ) {
	    rd_ptr = (count_reg & 0x03E0) >> 5;
	    wr_ptr = (count_reg & 0x7C00) >> 10;
	    if (debug) 
		printk(KERN_CRIT "ztgsm: SER_TX_COUNT_%d [wr_ptr %d rd_ptr %d free %d]\n", span, wr_ptr, rd_ptr, left);
	    if (gsmtmp->ser_tx_idx[span] < left) {
		count = gsmtmp->ser_tx_idx[span];
    	    } else {
		count = left;
	    }
	    if (debug)
		printk(KERN_INFO "ztgsm: span %d SER_TX [ ", span);
	    for (i=0;i<count; i++) {
		if (debug)
	    	    printk("%c", gsmtmp->ser_tx_buf[span][i]);
		ztgsm_outdw_io(gsmtmp, txdreg, gsmtmp->ser_tx_buf[span][i]);
	    }
	    if (debug)
	        printk("]\n");
	    gsmtmp->ser_tx_idx[span] -= count;
	    if (gsmtmp->ser_tx_idx[span] > 0) {
		memmove(&gsmtmp->ser_tx_buf[span][0], &gsmtmp->ser_tx_buf[span][i], gsmtmp->ser_tx_idx[span]);
	    }
	    tx_wm_sen = (ztgsm_indw_io(gsmtmp, ztgsm_SER_TX_WM_SEN) & 0xF) | (1 << span);
	    ztgsm_outdw_io(gsmtmp, ztgsm_SER_TX_WM_SEN, tx_wm_sen);
//	    printk(KERN_INFO "ztgsm: span %d TX_IDX %d count %d tx_wm_send %d\n", span, gsmtmp->ser_tx_idx[span], count, tx_wm_sen);
	}
    }
    return i;
}

static int ztgsm_zap_tx(struct ztgsm_card *gsmtmp, int span) {
    struct ztgsm_span *gsmspan = NULL;
    int i = 0;

    gsmspan = &gsmtmp->gsmspan[span];
    if (!gsmspan)
	 return -1;
	if (gsmspan->chans[1].bytes2transmit) {
	    if (debug) {
		printk(KERN_INFO "ztgsm: span %d TX [ ", span);
		for (i=0;i<gsmspan->chans[1].bytes2transmit; i++) {
		    printk("%c ", gsmtmp->dtxbuf[span][i]);
		}
	        printk("]\n");
	    }
	    if (gsmtmp->ser_tx_idx[span] + gsmspan->chans[1].bytes2transmit < ztgsm_SER_BUF_SIZE) {
		memcpy(&gsmtmp->ser_tx_buf[span][gsmtmp->ser_tx_idx[span]], gsmtmp->dtxbuf[span], gsmspan->chans[1].bytes2transmit);
		gsmtmp->ser_tx_idx[span] += gsmspan->chans[1].bytes2transmit;
		ztgsm_ser_tx(gsmtmp, span);
	    } else {
		printk(KERN_INFO "ztgsm: TX buffer overflow on span %d (TX_IDX %d BTT %d)\n", span, gsmtmp->ser_tx_idx[span] , gsmspan->chans[1].bytes2transmit);
	    }
	}
		
	gsmspan->chans[1].bytes2receive = 0;
	gsmspan->chans[1].bytes2transmit = 0;
	gsmspan->chans[1].eofrx = 0;
	gsmspan->chans[1].eoftx = 0;
    return 0;
}


static int ztgsm_span_rx(struct ztgsm_card *gsmtmp, int span) {
    struct ztgsm_span *gsmspan = NULL;
    unsigned int addr = 0;
    unsigned int framecnt = 0;
    unsigned int pcmframecnt = 0;
    unsigned int fraddr = 0;
    unsigned int data = 0;
    int start = 0;
    int len = 0;
    int i = 0;
    unsigned int slot = 0;
    
    switch(span) {
	case 0: 
	    slot = 1;
	    break;
	case 1: 
	    slot = 3;
	    break;
	case 2: 
	    slot = 5;
	    break;
	case 3: 
	    slot = 7;
	    break;
    }
//    slot = span;
    gsmspan = &gsmtmp->gsmspan[span];
    if (gsmspan) {
	framecnt = gsmtmp->framecnt;
	pcmframecnt = framecnt;
    	framecnt &= ztgsm_FRAMES - 1;
	start = framecnt - 16;
	if (start < 0) {
	    len = -start;
	    if (len > ZT_CHUNKSIZE) len = ZT_CHUNKSIZE;
	    fraddr = ztgsm_FRAMES + start;
	} else {
	    len = ZT_CHUNKSIZE;
	    fraddr = start;
	}

	if (fraddr & 3) {
	    if (debug > 0)
	        printk(KERN_INFO "ztgsm: RX span %d unaligned word address %#x (fraddr & 3 = %d)\n", span, fraddr, (fraddr & 3));
	    fraddr -= fraddr & 3; /* align */
	} 
	if (len == ZT_CHUNKSIZE) {
    	    addr = (slot << 8) | fraddr;
//    	    addr = ((span+1) << 8) | fraddr;
//    ztgsm_outdw_io(gsmtmp, ztgsm_LED_DUAL, addr & 0x4);
	    data = ztgsm_indw(gsmtmp, addr);
	    *((unsigned int *)&gsmtmp->rxbuf[span][0]) = data;


//    ztgsm_outdw_io(gsmtmp, ztgsm_LED_DUAL, addr & 0x4);
    	    addr = (slot << 8) | (fraddr + 4);
//    	    addr = ((span+1) << 8) | (fraddr + 4);
	    data = ztgsm_indw(gsmtmp, addr);
	    *((unsigned int *)&gsmtmp->rxbuf[span][4]) = data;
//    ztgsm_outdw_io(gsmtmp, ztgsm_LED_DUAL, 0xf);
	} else {
	    printk(KERN_INFO "ztgsm: dropped audio span %d fraddr %d addr %d\n", span, fraddr, addr);
	}
if (!(gsmtmp->ticks % 1000) && (debug > 4)) {
	printk(KERN_INFO "ztgsm: RX DATA:");
	for (i=0; i < ZT_CHUNKSIZE; i++) {
	    printk("%x", gsmtmp->rxbuf[span][i]);
	}
	printk("\n");
}

    }
    return 0;
}

static int ztgsm_span_tx(struct ztgsm_card *gsmtmp, int span) {
    struct ztgsm_span *gsmspan = NULL;
    unsigned int addr = 0;
    unsigned int framecnt = 0;
    unsigned int fraddr = 0;
    int start = 0;
    int len = 0;
    unsigned int slot = 0;
    
    switch(span) {
	case 0: 
	    slot = 1;
	    break;
	case 1: 
	    slot = 3;
	    break;
	case 2: 
	    slot = 5;
	    break;
	case 3: 
	    slot = 7;
	    break; 
    }
//    slot = span;

    gsmspan = &gsmtmp->gsmspan[span];
    if (gsmspan) {
	framecnt = gsmtmp->framecnt;
    	framecnt &= ztgsm_FRAMES - 1;
	start = framecnt + 16;

	if (start < 0x0) {
	    len = -start;
	    if (len > ZT_CHUNKSIZE) len = ZT_CHUNKSIZE;
	    fraddr = ztgsm_FRAMES + start;
	} else {
	    len = ZT_CHUNKSIZE;
	    fraddr = start;
	}
	fraddr -= fraddr & 3; /* align */

	if (fraddr & 3) {
//	    printk(KERN_EMERG "ztgsm: unaligned word address %#x\n", addr);
	} else if (len == ZT_CHUNKSIZE) {
	    addr =(slot << 8 ) | fraddr;
//	    addr =( (span+1) << 8 ) | fraddr;
	    ztgsm_outdw(gsmtmp, addr, *((unsigned int*)&(gsmtmp->txbuf[span][0])));
//    	    addr =( (span+1) << 8 ) | (fraddr + 4);
    	    addr =(slot << 8 ) | (fraddr + 4);
	    ztgsm_outdw(gsmtmp, addr, *((unsigned int*)&(gsmtmp->txbuf[span][4])));
	}

    }
    return 0;
}

static void ztgsm_leds(struct ztgsm_card *gsmtmp, int tick) {
    int i = 0;
    unsigned int leds = 0;
    unsigned int color = 0;
    for (i=0; i<gsmtmp->gsmspans; i++) {
//	printk(KERN_INFO "ztgsm: led = %x\n",gsmtmp->gsmspan[i].led);
	color = gsmtmp->gsmspan[i].led & 0x1;
	leds |= (color << i);
	if (!(gsmtmp->gsmspan[i].led & 0x80)) {
	    leds |= (1 << (i+8));
	}
	if ((gsmtmp->gsmspan[i].led & 0x40)) {
	    if (tick == 300)
		leds |= (1 << (i+8));
	}

    }
//    printk(KERN_INFO "ztgsm: leds = %d\n",leds);
    ztgsm_outdw_io(gsmtmp, ztgsm_LED_DUAL, leds);
}

static inline void ztgsm_run(struct ztgsm_card *gsmtmp) {
    int s=0;
    struct ztgsm_span *gsmspan = NULL;
    for (s=0;s<gsmtmp->gsmspans;s++) {
        gsmspan = &gsmtmp->gsmspan[s];
        if (gsmspan) {
	    if (!(gsmtmp->ticks % 1000)) {
	        gsmtmp->ticks = 0;
    	    }
	    if (gsmtmp->ticks % 300 == 0)
	        ztgsm_leds(gsmtmp, gsmtmp->ticks);
	    if (gsmspan->span.flags & ZT_FLAG_RUNNING) {
		/* oh zaptel! tell us what to transmit... */
    		zt_transmit(&gsmspan->span);
		if (debug && (gsmspan->chans[1].bytes2transmit > 0))
		    printk(KERN_CRIT "ztgsm: span %d bytes2transmit = %d\n", s, gsmspan->chans[1].bytes2transmit);
		ztgsm_span_tx(gsmtmp, s);
		ztgsm_zap_tx(gsmtmp, s);
	    }

	    if (gsmspan->span.flags & ZT_FLAG_RUNNING) {
	        ztgsm_zap_rx(gsmtmp, s);
	    	ztgsm_span_rx(gsmtmp, s);
		/* oh zaptel! thou shall receive! */
		zt_receive(&gsmspan->span);
	    }
	} 
    }
}



#ifdef LINUX26
static irqreturn_t ztgsm_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#else
static void ztgsm_interrupt(int irq, void *dev_id, struct pt_regs *regs) {
#endif
    struct ztgsm_card *gsmtmp = dev_id;
    unsigned int ser_status = 0;
    unsigned char mods = 0;
    int s = 0;
    int rx_count = 0;
    unsigned long flags;

    if (!gsmtmp || gsmtmp->dead) {
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }

    if ((!gsmtmp->pci_io) || (!gsmtmp->ioport)) {
	    printk(KERN_CRIT "ztgsm: no pci mem/io\n");
#ifdef LINUX26
		return IRQ_NONE;
#else
		return;
#endif		
    }
    
    spin_lock_irqsave(&(gsmtmp->lock),flags);
//	    printk(KERN_INFO "gsm: irq\n");
    gsmtmp->last_framecnt = gsmtmp->framecnt;
    gsmtmp->framecnt = ztgsm_indw_io(gsmtmp, ztgsm_PCM_FRAME_CNT);
    ser_status = ztgsm_indw_io(gsmtmp, ztgsm_SER_STATUS);
    if (ser_status) {
	if (ser_status & 0x10000) {
	    if (gsmtmp->framecnt - gsmtmp->last_framecnt != 8) {
//		printk(KERN_INFO "ztgsm: missed IRQ, framecnt %d last_framecnt %d (diff %d)\n", gsmtmp->framecnt, gsmtmp->last_framecnt, (gsmtmp->framecnt - gsmtmp->last_framecnt));
	    }
	    gsmtmp->ticks++;
	    if (!(gsmtmp->ticks % 1000)) {
//		printk(KERN_INFO "ztgsm: pcm framce counter  %d\n", ztgsm_indw_io(gsmtmp, ztgsm_PCM_FRAME_CNT) & 0x1f);
	    }
	    if (!(gsmtmp->ticks % 300)) {
		for (s=0; s<gsmtmp->gsmspans; s++) {
		    rx_count = ztgsm_ser_rx(gsmtmp, s);
		    if (debug && rx_count) 
			printk(KERN_INFO "ztgsm: RX %d bytes on span %d\n", rx_count, s);
		}
	    }
	    ztgsm_run(gsmtmp);
	} else if (ser_status & 0x0F) {
	    mods = (ser_status & 0x0F);
	    for (s=0; s<gsmtmp->gsmspans; s++) {
		if (mods & (1 << s)) {
		    rx_count = ztgsm_ser_rx(gsmtmp, s);
		    if (debug) 
			printk(KERN_INFO "ztgsm: TX fifo overrun on span %d\n", s);
		}
	    }
	} else if (ser_status & 0xF0) {
	    mods = (ser_status & 0xF0) >> 4;
		    if (debug) 
			printk(KERN_INFO "ztgsm: RX mods %d\n", mods);
	    for (s=0; s<gsmtmp->gsmspans; s++) {
		if (mods & (1 << s)) {
		    rx_count = ztgsm_ser_rx(gsmtmp, s);
		    if (debug) 
			printk(KERN_INFO "ztgsm: RX %d bytes on span %d\n", rx_count, s);
		}
	    }
	} else if (ser_status & 0xF00) {
	    mods = (ser_status & 0xF00) >> 8;
	    for (s=0; s<gsmtmp->gsmspans; s++) {
		if (mods & (1 << s)) {
		    rx_count = ztgsm_ser_rx(gsmtmp, s);
		    if (debug) 
			printk(KERN_INFO "ztgsm: RX fifo overrun on span %d\n", s);
		}
	    }
	} else if (ser_status & 0xF000) {
	    mods = (ser_status & 0xF000) >> 12;
	    for (s=0; s<gsmtmp->gsmspans; s++) {
		if (mods & (1 << s)) {
		    if (gsmtmp->ser_tx_idx[s]) {
			/* sumfin left to transmit */
			ztgsm_ser_tx(gsmtmp, s);
		    }
		    if (debug) 
			printk(KERN_INFO "ztgsm: TX low water status %#x\n", ser_status);
		}
	    }
	} else {
	    if (debug) {
		printk(KERN_INFO "ztgsm: SER_STATUS = %#x\n", ser_status);
	    }
	}
    }
    spin_unlock_irqrestore(&(gsmtmp->lock),flags);
#ifdef LINUX26
	return IRQ_RETVAL(1);
#endif		
}


static int ztgsm_open(struct zt_chan *chan) {
//    printk(KERN_INFO "ztgsm: channel %d opened.\n",chan->channo);
#ifndef LINUX26
    MOD_INC_USE_COUNT;
#else
    try_module_get(THIS_MODULE);
#endif
    return 0;
}

static int ztgsm_close(struct zt_chan *chan) {
//    printk(KERN_INFO "ztgsm: channel %d closed.\n",chan->channo);
#ifndef LINUX26
    MOD_DEC_USE_COUNT;
#else
    module_put(THIS_MODULE);
#endif
    return 0;
}

static int ztgsm_chanconfig(struct zt_chan *chan,int sigtype) {
//    printk(KERN_INFO "chan_config sigtype=%d\n",sigtype);
    return 0;
}

static int ztgsm_spanconfig(struct zt_span *span,struct zt_lineconfig *lc) {
//    span->lineconfig = lc->lineconfig;
    return 0;
}


static int ztgsm_startup(struct zt_span *span) {
    struct ztgsm_card *gsmtmp = span->pvt;
    int running;
    
    if (gsmtmp == NULL) {
	printk(KERN_INFO "ztgsm: no card for span at startup!\n");
    }
    
    running = span->flags & ZT_FLAG_RUNNING;
    span->chans[1].flags &= ~ZT_FLAG_HDLC;
    span->chans[1].flags |= ZT_FLAG_BRIDCHAN;

    if (!running) {
	if (!gsmtmp->power[span->offset]) 
	    ztgsm_switch_on(gsmtmp, span->offset);
	span->flags |= ZT_FLAG_RUNNING;

    } else {
	printk(KERN_CRIT "already running\n");
	return 0;
    }
    return 0;
}

static int ztgsm_shutdown(struct zt_span *span) {
    int running;
    struct ztgsm_card *gsmtmp = span->pvt;

    if (gsmtmp == NULL) {
	printk(KERN_INFO "ztgsm: no card for span at shutdown!\n");
    }

    running = span->flags & ZT_FLAG_RUNNING;

    if (running) {
	span->flags |= ZT_FLAG_RUNNING;
	if (gsmtmp->power[span->offset]) 
	    ztgsm_switch_off(gsmtmp, span->offset);
    }
    return 0;
}

static int ztgsm_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data) {
        switch(cmd) {
        default:
                return -ENOTTY;
        }
        return 0;
}


static int ztgsm_init(struct ztgsm_span *gsmspan, struct ztgsm_card *gsmtmp, int offset) {
    memset(&gsmspan->span,0,sizeof(struct zt_span)); // you never can tell...
    sprintf(gsmspan->span.name,"ztgsm/%d",gsmtmp->cardno);
    switch (gsmtmp->type) {
        case 0xb55d:
		sprintf(gsmspan->span.desc,"Junghanns.NET unoGSM PCI Card %d",gsmtmp->cardno);
	    break;
	case 0xb55e:
		sprintf(gsmspan->span.desc,"Junghanns.NET duoGSM PCI Card %d",gsmtmp->cardno);
	    break;
	case 0xb55f:
		sprintf(gsmspan->span.desc,"Junghanns.NET quadGSM PCI Card %d",gsmtmp->cardno);
	    break;
    }
    
    gsmspan->span.spanconfig = ztgsm_spanconfig;
    gsmspan->span.chanconfig = ztgsm_chanconfig;
    gsmspan->span.startup = ztgsm_startup;
    gsmspan->span.shutdown = ztgsm_shutdown;
    gsmspan->span.maint = NULL;
    gsmspan->span.rbsbits = NULL;
    gsmspan->span.open = ztgsm_open;
    gsmspan->span.close = ztgsm_close;
    gsmspan->span.ioctl = ztgsm_ioctl;

    if (pcm_xbar == 1) 
	gsmspan->span.dacs = ztgsm_dacs;

    gsmspan->span.chans = gsmspan->chans;
    gsmspan->span.channels = 2;
    gsmspan->span.deflaw = ZT_LAW_ALAW;
    gsmspan->span.linecompat = ZT_CONFIG_CCS | ZT_CONFIG_AMI; 
    init_waitqueue_head(&gsmspan->span.maintq);
    gsmspan->span.pvt = gsmtmp;
    gsmspan->span.offset = offset;

    memset(&(gsmspan->chans[0]),0x0,sizeof(struct zt_chan));
    sprintf(gsmspan->chans[0].name,"ztgsm/%d", 0);
    gsmspan->chans[0].pvt = gsmspan;
    gsmspan->chans[0].sigcap =  ZT_SIG_CLEAR;
    gsmspan->chans[0].chanpos = 1; 

    memset(&(gsmspan->chans[1]),0x0,sizeof(struct zt_chan));
    sprintf(gsmspan->chans[1].name,"ztgsm/%d", 1);
    gsmspan->chans[1].pvt = gsmspan;
    gsmspan->chans[1].sigcap =  ZT_SIG_CLEAR;
    gsmspan->chans[1].chanpos = 2; 

	
    if (zt_register(&gsmspan->span,0)) {
        printk(KERN_INFO "ztgm: unable to register zaptel span!\n");
        return -1;
    }

    /* setup B channel buffers (8 bytes each) */
    memset(gsmtmp->rxbuf[offset],0x0,sizeof(gsmtmp->rxbuf[offset]));
    gsmspan->span.chans[0].readchunk = gsmtmp->rxbuf[offset];
    memset(gsmtmp->txbuf[offset],0x0,sizeof(gsmtmp->txbuf[offset]));
    gsmspan->span.chans[0].writechunk = gsmtmp->txbuf[offset];

    /* setup D channel buffer */
    memset(gsmtmp->dtxbuf[offset],0x0,sizeof(gsmtmp->dtxbuf[offset]));
    gsmspan->span.chans[1].writechunk = gsmtmp->dtxbuf[offset];
    gsmspan->span.chans[1].maxbytes2transmit = sizeof(gsmtmp->dtxbuf[offset]);

    memset(gsmtmp->drxbuf[offset],0x0,sizeof(gsmtmp->drxbuf[offset]));
    gsmspan->span.chans[1].readchunk = gsmtmp->drxbuf[offset];

    return 0;
}


int ztgsm_findCards(unsigned int pcidid) {
    struct pci_dev *tmp;
    struct ztgsm_card *gsmtmp = NULL;
    struct ztgsm_span *gsmspan = NULL;
    unsigned int ioport_size = 0;
    int i=0;
    int cid=0;
    tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,NULL);
    while (tmp != NULL) {
	multi_gsm = tmp;

	if (pci_enable_device(tmp)) {
	    return -1;
	}

	gsmtmp = kmalloc(sizeof(struct ztgsm_card),GFP_KERNEL);
	if (!gsmtmp) {
	    printk(KERN_WARNING "ztgsm: unable to kmalloc!\n");
	    pci_disable_device(tmp);
	    return -ENOMEM;
	}
	memset(gsmtmp, 0x0, sizeof(struct ztgsm_card));
	
	spin_lock_init(&gsmtmp->lock);
	gsmtmp->pcidev = tmp;
	gsmtmp->pcibus = tmp->bus->number;
	gsmtmp->pcidevfn = tmp->devfn; 

	if (!tmp->irq) {
	    printk(KERN_WARNING "ztgsm: no irq!\n");
	} else {
	    gsmtmp->irq = tmp->irq;
	}

	gsmtmp->pci_io_phys = tmp->resource[1].start;
	if (!gsmtmp->pci_io_phys) {
	    printk(KERN_WARNING "ztgsm: no iomem!\n");
	    pci_disable_device(tmp);
	    return -EIO;
	}
	gsmtmp->iomem_size = (tmp->resource[1].end - tmp->resource[1].start + 1);
	printk(KERN_INFO "ztgsm: iomem at %lx size %ld\n", gsmtmp->pci_io_phys, gsmtmp->iomem_size);

	if (check_mem_region(gsmtmp->pci_io_phys, gsmtmp->iomem_size)) {
	    printk(KERN_INFO "ztgsm: iomem already in use!\n");;
	    pci_disable_device(tmp);
	    return -EBUSY;
	}
	
	request_mem_region(gsmtmp->pci_io_phys, gsmtmp->iomem_size, "ztgsm");

	gsmtmp->pci_io = ioremap(gsmtmp->pci_io_phys, gsmtmp->iomem_size); /* 8kb */

	gsmtmp->ioport = tmp->resource[0].start;
	if (!gsmtmp->ioport) {
	    printk(KERN_WARNING "ztgsm: no ioport!\n");
	    pci_disable_device(tmp);
	    return -EIO;
	}
	ioport_size = (tmp->resource[0].end - tmp->resource[0].start + 1);
	printk(KERN_INFO "ztgsm: ioport size %d\n", ioport_size);

	if (!request_region(gsmtmp->ioport, 0x100, "ztgsm")) {
	    printk(KERN_WARNING "ztgsm: couldnt request io range!\n");
	    release_mem_region(gsmtmp->pci_io_phys, gsmtmp->iomem_size);
	    pci_disable_device(tmp);
	    return -EIO;
	}
	
	if (request_irq(gsmtmp->irq, ztgsm_interrupt, SA_INTERRUPT | SA_SHIRQ, "ztgsm", gsmtmp)) {
	    printk(KERN_WARNING "ztgsm: unable to register irq\n");
	    release_region(gsmtmp->ioport, 0x100);
	    release_mem_region(gsmtmp->pci_io_phys, gsmtmp->iomem_size);
	    kfree(gsmtmp);
	    pci_disable_device(tmp);
	    return -EIO;
	}
	
	pci_write_config_word(gsmtmp->pcidev, PCI_COMMAND, PCI_COMMAND_IO | PCI_COMMAND_MEMORY);

	// disable ints

	gsmtmp->type = tmp->subsystem_device;

	switch (gsmtmp->type) {
	    case 0xb55d:
		    printk(KERN_INFO
    			"ztgsm: Junghanns.NET unoGSM card configured at io port %x IRQ %d io mem %lx HZ %d CardID %d\n",
        		 (u_int) gsmtmp->ioport, gsmtmp->irq, (u_long) gsmtmp->pci_io, HZ, cid);
		break;
	    case 0xb55e:
		    printk(KERN_INFO
    			"ztgsm: Junghanns.NET duoGSM card configured at io port %x IRQ %d io mem %lx HZ %d CardID %d\n",
        		 (u_int) gsmtmp->ioport, gsmtmp->irq, (u_long) gsmtmp->pci_io, HZ, cid);
		break;
	    case 0xb55f:
		    printk(KERN_INFO
    			"ztgsm: Junghanns.NET quadGSM card configured at io port %x IRQ %d io mem %lx HZ %d CardID %d\n",
        		 (u_int) gsmtmp->ioport, gsmtmp->irq, (u_long) gsmtmp->pci_io, HZ, cid);
		break;
	}

	ztgsm_resetCard(gsmtmp);
	ztgsm_init_xbar(gsmtmp);

	switch (ztgsm_indw_io(gsmtmp, ztgsm_SER_G20_ACTIVATED)) {
	    case 0xf:
		gsmtmp->gsmspans = 4;
		break;
	    case 0x3:
		gsmtmp->gsmspans = 2;
		break;
	    case 0x1:
		gsmtmp->gsmspans = 1;
		break;
	}
	ztgsm_spans += gsmtmp->gsmspans;
	
	for (i=0; i < gsmtmp->gsmspans; i++) {
	    gsmspan = &gsmtmp->gsmspan[i];
	    ztgsm_init(gsmspan, gsmtmp, i);
	    gsmspan->led = 0x80;
	}
        ztgsm_leds(gsmtmp, 0);


	gsmtmp->version = ztgsm_indw_io(gsmtmp, ztgsm_VERS_A);
	printk(KERN_INFO "ztgsm: VERSION %x\n", gsmtmp->version);
	if (debug) {
	    printk(KERN_INFO "ztgsm: G20_ACTIVATED %d\n", ztgsm_indw_io(gsmtmp, ztgsm_SER_G20_ACTIVATED));
	    printk(KERN_INFO "ztgsm: DIPS %#x\n", ztgsm_indw_io(gsmtmp, ztgsm_DIP_SWITCH));
	    printk(KERN_INFO "ztgsm: tx_wm_sen %d\n", ztgsm_indw_io(gsmtmp, ztgsm_SER_TX_WM_SEN));
	}
	
	ztgsm_register_card(gsmtmp);

	ztgsm_startCard(gsmtmp);
	ztgsm_switch_on_all(gsmtmp);
	
	tmp = pci_find_device(PCI_VENDOR_ID_CCD,pcidid,multi_gsm);
    }
    return 0;
}


int init_module(void) {
    ztgsm_findCards(0xf001);
    if (ztgsm_dev_count == 0) {
	printk(KERN_INFO "ztgsm: no multiGSM cards found.\n");
    } else {
	printk(KERN_INFO "ztgsm: %d multiGSM card(s) in this box, %d GSM spans total.\n",ztgsm_dev_count, ztgsm_spans);
    }
    return 0;
}

void cleanup_module(void) {
    struct ztgsm_card *tmpcard,*tmplist;
    int i=0;
    tmplist = ztgsm_dev_list;
    while (tmplist != NULL) {
	ztgsm_shutdownCard(tmplist);
	tmplist = tmplist->next;
    }
    tmplist = ztgsm_dev_list;
    spin_lock(&registerlock);
    while (tmplist != NULL) {
	tmpcard = tmplist->next;
	kfree(tmplist);
	i++;
	tmplist = tmpcard;
    }
    spin_unlock(&registerlock);
    printk(KERN_INFO "ztgsm: shutdown %d multiGSM cards.\n", i);
}
#endif

#ifdef LINUX26
module_param(debug, int, 0600);
module_param(pcm_xbar, int, 0600);
#else
MODULE_PARM(debug,"i");
MODULE_PARM(pcm_xbar,"i");
#endif

MODULE_DESCRIPTION("uno/duao/quad GSM zaptel driver");
MODULE_AUTHOR("Klaus-Peter Junghanns <kpj@junghanns.net>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif	
