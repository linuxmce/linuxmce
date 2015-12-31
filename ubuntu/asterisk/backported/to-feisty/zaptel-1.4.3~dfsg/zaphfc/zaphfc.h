/*
 * zaphfc.h - Zaptel driver for HFC-S PCI A based ISDN BRI cards
 *
 * kernel module based on HFC PCI ISDN4Linux and Zaptel drivers
 *
 * Copyright (C) 2002, 2003, 2004, 2005 Junghanns.NET GmbH
 *
 * Klaus-Peter Junghanns <kpj@junghanns.net>
 *
 * This program is free software and may be modified and
 * distributed under the terms of the GNU Public License.
 *
 */

/* HFC register addresses - accessed using memory mapped I/O */
/* For a list, see datasheet section 3.2.1 at page 21 */

#define hfc_outb(a,b,c) (writeb((c),(a)->pci_io+(b)))
#define hfc_inb(a,b) (readb((a)->pci_io+(b)))

/* GCI/IOM bus monitor registers */

#define hfc_C_I       0x08
#define hfc_TRxR      0x0C
#define hfc_MON1_D    0x28
#define hfc_MON2_D    0x2C


/* GCI/IOM bus timeslot registers */

#define hfc_B1_SSL    0x80
#define hfc_B2_SSL    0x84
#define hfc_AUX1_SSL  0x88
#define hfc_AUX2_SSL  0x8C
#define hfc_B1_RSL    0x90
#define hfc_B2_RSL    0x94
#define hfc_AUX1_RSL  0x98
#define hfc_AUX2_RSL  0x9C

/* GCI/IOM bus data registers */

#define hfc_B1_D      0xA0
#define hfc_B2_D      0xA4
#define hfc_AUX1_D    0xA8
#define hfc_AUX2_D    0xAC

/* GCI/IOM bus configuration registers */

#define hfc_MST_EMOD  0xB4
#define hfc_MST_MODE	 0xB8
#define hfc_CONNECT 	 0xBC


/* Interrupt and status registers */

#define hfc_FIFO_EN   0x44
#define hfc_TRM       0x48
#define hfc_B_MODE    0x4C
#define hfc_CHIP_ID   0x58
#define hfc_CIRM  	 0x60
#define hfc_CTMT	 0x64
#define hfc_INT_M1  	 0x68
#define hfc_INT_M2  	 0x6C
#define hfc_INT_S1  	 0x78
#define hfc_INT_S2  	 0x7C
#define hfc_STATUS  	 0x70

/* S/T section registers */

#define hfc_STATES  	 0xC0
#define hfc_SCTRL  	 0xC4
#define hfc_SCTRL_E   0xC8
#define hfc_SCTRL_R   0xCC
#define hfc_SQ  	 0xD0
#define hfc_CLKDEL  	 0xDC
#define hfc_B1_REC    0xF0
#define hfc_B1_SEND   0xF0
#define hfc_B2_REC    0xF4
#define hfc_B2_SEND   0xF4
#define hfc_D_REC     0xF8
#define hfc_D_SEND    0xF8
#define hfc_E_REC     0xFC

/* Bits and values in various HFC PCI registers */

/* bits in status register (READ) */
#define hfc_STATUS_PCI_PROC   0x02
#define hfc_STATUS_NBUSY	  0x04 
#define hfc_STATUS_TIMER_ELAP 0x10
#define hfc_STATUS_STATINT	  0x20
#define hfc_STATUS_FRAMEINT	  0x40
#define hfc_STATUS_ANYINT	  0x80

/* bits in CTMT (Write) */
#define hfc_CTMT_CLTIMER    0x80
#define hfc_CTMT_TIM3_125   0x04
#define hfc_CTMT_TIM25      0x10
#define hfc_CTMT_TIM50      0x14
#define hfc_CTMT_TIM400     0x18
#define hfc_CTMT_TIM800     0x1C
#define hfc_CTMT_AUTO_TIMER 0x20
#define hfc_CTMT_TRANSB2    0x02
#define hfc_CTMT_TRANSB1    0x01

/* bits in CIRM (Write) */
#define hfc_CIRM_AUX_MSK    0x07
#define hfc_CIRM_RESET  	  0x08
#define hfc_CIRM_B1_REV     0x40
#define hfc_CIRM_B2_REV     0x80

/* bits in INT_M1 and INT_S1 */
#define hfc_INTS_B1TRANS  0x01
#define hfc_INTS_B2TRANS  0x02
#define hfc_INTS_DTRANS   0x04
#define hfc_INTS_B1REC    0x08
#define hfc_INTS_B2REC    0x10
#define hfc_INTS_DREC     0x20
#define hfc_INTS_L1STATE  0x40
#define hfc_INTS_TIMER    0x80

/* bits in INT_M2 */
#define hfc_M2_PROC_TRANS    0x01
#define hfc_M2_GCI_I_CHG     0x02
#define hfc_M2_GCI_MON_REC   0x04
#define hfc_M2_IRQ_ENABLE    0x08
#define hfc_M2_PMESEL        0x80

/* bits in STATES */
#define hfc_STATES_STATE_MASK     0x0F
#define hfc_STATES_LOAD_STATE    0x10
#define hfc_STATES_ACTIVATE	     0x20
#define hfc_STATES_DO_ACTION     0x40
#define hfc_STATES_NT_G2_G3      0x80

/* bits in HFCD_MST_MODE */
#define hfc_MST_MODE_MASTER	     0x01
#define hfc_MST_MODE_SLAVE         0x00
/* remaining bits are for codecs control */

/* bits in HFCD_SCTRL */
#define hfc_SCTRL_B1_ENA	     0x01
#define hfc_SCTRL_B2_ENA	     0x02
#define hfc_SCTRL_MODE_TE        0x00
#define hfc_SCTRL_MODE_NT        0x04
#define hfc_SCTRL_LOW_PRIO	     0x08
#define hfc_SCTRL_SQ_ENA	     0x10
#define hfc_SCTRL_TEST	     0x20
#define hfc_SCTRL_NONE_CAP	     0x40
#define hfc_SCTRL_PWR_DOWN	     0x80

/* bits in SCTRL_E  */
#define hfc_SCTRL_E_AUTO_AWAKE    0x01
#define hfc_SCTRL_E_DBIT_1        0x04
#define hfc_SCTRL_E_IGNORE_COL    0x08
#define hfc_SCTRL_E_CHG_B1_B2     0x80

/* bits in FIFO_EN register */
#define hfc_FIFOEN_B1TX   0x01
#define hfc_FIFOEN_B1RX   0x02
#define hfc_FIFOEN_B2TX   0x04
#define hfc_FIFOEN_B2RX   0x08
#define hfc_FIFOEN_DTX    0x10
#define hfc_FIFOEN_DRX    0x20

#define hfc_FIFOEN_B1     (hfc_FIFOEN_B1TX|hfc_FIFOEN_B1RX)
#define hfc_FIFOEN_B2     (hfc_FIFOEN_B2TX|hfc_FIFOEN_B2RX)
#define hfc_FIFOEN_D      (hfc_FIFOEN_DTX|hfc_FIFOEN_DRX)

/* bits in the CONNECT register */
#define hfc_CONNECT_B1_shift	0
#define hfc_CONNECT_B2_shift	3

#define	hfc_CONNECT_HFC_from_ST		0x0
#define hfc_CONNECT_HFC_from_GCI	0x1
#define hfc_CONNECT_ST_from_HFC		0x0
#define hfc_CONNECT_ST_from_GCI		0x2
#define hfc_CONNECT_GCI_from_HFC	0x0
#define	hfc_CONNECT_GCI_from_ST		0x4

/* bits in the __SSL and __RSL registers */
#define	hfc_SRSL_STIO	0x40
#define hfc_SRSL_ENABLE	0x80
#define hfc_SRCL_SLOT_MASK	0x1f

/* FIFO memory definitions */

#define hfc_FMASK	0x000f
#define hfc_ZMASK	0x01ff
#define hfc_ZMASKB	0x1fff

#define hfc_D_FIFO_SIZE	0x0200
#define hfc_B_SUB_VAL	0x0200
#define hfc_B_FIFO_SIZE	0x1E00
#define hfc_MAX_DFRAMES	0x000f

#define hfc_FIFO_DTX_Z1	0x2080
#define hfc_FIFO_DTX_Z2 0x2082
#define hfc_FIFO_DTX_F1	0x20a0
#define hfc_FIFO_DTX_F2	0x20a1
#define hfc_FIFO_DTX	0x0000
#define hfc_FIFO_DTX_ZOFF	0x000

#define hfc_FIFO_DRX_Z1	0x6080
#define hfc_FIFO_DRX_Z2 0x6082
#define hfc_FIFO_DRX_F1	0x60a0
#define hfc_FIFO_DRX_F2	0x60a1
#define hfc_FIFO_DRX	0x4000
#define hfc_FIFO_DRX_ZOFF	0x4000

#define hfc_FIFO_B1TX_Z1	0x2000
#define hfc_FIFO_B1TX_Z2 	0x2002
#define hfc_FIFO_B1RX_Z1	0x6000
#define hfc_FIFO_B1RX_Z2 	0x6002

#define hfc_FIFO_B1TX_F1	0x2080
#define hfc_FIFO_B1TX_F2	0x2081
#define hfc_FIFO_B1RX_F1	0x6080
#define hfc_FIFO_B1RX_F2	0x6081

#define hfc_FIFO_B1RX_ZOFF	0x4000
#define hfc_FIFO_B1TX_ZOFF	0x0000

#define hfc_FIFO_B2TX_Z1	0x2100
#define hfc_FIFO_B2TX_Z2 	0x2102
#define hfc_FIFO_B2RX_Z1	0x6100
#define hfc_FIFO_B2RX_Z2 	0x6102

#define hfc_FIFO_B2TX_F1	0x2180
#define hfc_FIFO_B2TX_F2	0x2181
#define hfc_FIFO_B2RX_F1	0x6180
#define hfc_FIFO_B2RX_F2	0x6181

#define hfc_FIFO_B2RX_ZOFF	0x6000
#define hfc_FIFO_B2TX_ZOFF	0x2000

#define hfc_BTRANS_THRESHOLD 128
#define hfc_BTRANS_THRESMASK 0x00

/* Structures */

typedef struct hfc_regs {
    unsigned char fifo_en;
    unsigned char ctmt;
    unsigned char int_m1;
    unsigned char int_m2;
    unsigned char sctrl;
    unsigned char sctrl_e;
    unsigned char sctrl_r;
    unsigned char connect;
    unsigned char trm;
    unsigned char mst_mode;
    unsigned char bswapped;
    unsigned char nt_mode;
    unsigned char int_drec;
} hfc_regs;

typedef struct hfc_card {
    spinlock_t lock;
    unsigned int irq;
    unsigned int iomem;
    int ticks;		
    int clicks;		
    unsigned char *pci_io;
    void *fifomem;		// start of the shared mem
    volatile void *fifos;	// 32k aligned mem for the fifos
    struct hfc_regs regs;
    unsigned int pcibus;
    unsigned int pcidevfn;
    struct pci_dev *pcidev;
    struct zt_hfc *ztdev;
    int	drecinframe;
    unsigned char drecbuf[hfc_D_FIFO_SIZE];
    unsigned char dtransbuf[hfc_D_FIFO_SIZE];
    unsigned char brecbuf[2][ZT_CHUNKSIZE];
    unsigned char btransbuf[2][ZT_CHUNKSIZE];
    unsigned char cardno;
    struct hfc_card *next;
} hfc_card;

typedef struct zt_hfc {
    unsigned int usecount;
    struct zt_span span;
    struct zt_chan chans[3];
    struct hfc_card *card;
} zt_hfc;

/* tune this */
#define hfc_BCHAN_BUFFER	8
#define hfc_MAX_CARDS		8
