#define qoz_SPANS 8	
#define qoz_FIFO_SIZE	128
#define qoz_DFIFO_SIZE4	2048
#define qoz_DFIFO_SIZE8	1024

typedef struct qoz_span {
    unsigned char nt_mode;
    unsigned char btx;
    unsigned char bswapped;
    unsigned char drx;
    int t3;
    int t4;
    int l1up;
    unsigned char layer1state;
} qoz_span;

typedef struct qoz_regs {
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
} qoz_regs;

typedef struct qoz_card {
    spinlock_t lock;
    int cardID;
    unsigned char dead;
    unsigned char leds[8];
    unsigned char cardno;
    unsigned int irq;
    unsigned int iomem;
    unsigned char *pci_io;
    void *pci_io_phys;
    unsigned long ioport;
    struct qoz_span st[qoz_SPANS];
    int st_sync[qoz_SPANS];
    int syncsrc;
    unsigned int pcibus;
    unsigned int pcidevfn;
    struct pci_dev *pcidev;
    struct zt_span spans[qoz_SPANS];
    struct zt_chan chans[qoz_SPANS][3];
    unsigned char rxbuf[qoz_SPANS][2][ZT_CHUNKSIZE];
    unsigned char txbuf[qoz_SPANS][2][ZT_CHUNKSIZE];
    unsigned char drxbuf[qoz_SPANS][qoz_DFIFO_SIZE4];
    unsigned char dtxbuf[qoz_SPANS][qoz_DFIFO_SIZE4];
    unsigned char stports;
    unsigned int ticks;
    unsigned int clicks;
    unsigned int type;
    unsigned int wdp;
    struct qoz_card *next;
    struct qoz_card *prev;
} qoz_card;


#define qoz_outb_io(a,b,c) \
    outw((b), ((a)->ioport+4)); \
    outb((c), ((a)->ioport));

#define qoz_inb_io(a,b) ({ outw((b), (a)->ioport+4); inb((a)->ioport); })

#define qoz_outw_io(a,b,c) \
    outw((b), ((a)->ioport+4)); \
    outw((c), ((a)->ioport));

#define qoz_inw_io(a,b) ({ outw((b), (a)->ioport+4); inw((a)->ioport); })

#define qoz_outdw_io(a,b,c) \
    outw((b), ((a)->ioport+4)); \
    outl((c), ((a)->ioport));

#define qoz_indw_io(a,b) ({ outw((b), (a)->ioport+4); inl((a)->ioport); })

#define qoz_outb(a,b,c) (writeb((c),(a)->pci_io+(b)))
#define qoz_inb(a,b) (readb((a)->pci_io+(b)))

#define qoz_outw(a,b,c) (writew((c),(a)->pci_io+(b)))
#define qoz_inw(a,b) (readw((a)->pci_io+(b)))

#define qoz_outdw(a,b,c) (writel((c),(a)->pci_io+(b)))
#define qoz_indw(a,b) (readl((a)->pci_io+(b)))


/* Write only registers */
#define qoz_A_CH_MSK	0xF4
#define qoz_A_CHANNEL 	0xFC
#define qoz_A_CON_HDLC	0xFA
#define qoz_A_CONF	0xD1
#define qoz_A_FIFO_SEQ	0xFD
#define qoz_R_INC_RES_FIFO	0x0E
#define qoz_A_IRQ_MSK	0xFF
#define qoz_A_SL_CFG	0xD0
#define qoz_A_ST_B1_TX	0x3C
#define qoz_A_ST_B2_TX	0x3D
#define qoz_A_ST_CLK_DLY	0x37
#define qoz_A_ST_CTRL0	0x31
#define qoz_A_ST_CTRL1	0x32
#define qoz_A_ST_CTRL2	0x33
#define qoz_A_ST_D_TX	0x3E
#define qoz_A_ST_SQ_WR	0x34
#define qoz_A_ST_WR_STA	0x30
#define qoz_A_SUBCH_CFG	0xFB
#define qoz_R_BERT_WD_MD	0x1B
#define qoz_R_BRG_CTRL	0x45
#define qoz_R_BRG_MD	0x47
#define qoz_R_BRG_PCM_CFG	0x02
#define qoz_R_BRG_TIM_SEL01	0x4C
#define qoz_R_BRG_TIM_SEL23	0x4D
#define qoz_R_BRG_TIM_SEL45	0x4E
#define qoz_R_BRG_TIM_SEL67	0x4F
#define qoz_R_BRG_TIM0	0x48
#define qoz_R_BRG_TIM1	0x49
#define qoz_R_BRG_TIM2	0x4A
#define qoz_R_BRG_TIM3	0x4B
#define qoz_R_CIRM	0x00
#define qoz_R_CONF_EN	0x18
#define qoz_R_CTRL	0x01
#define qoz_R_DTMF0	0x1C
#define qoz_R_DTMF1	0x1D
#define qoz_R_FIFO_MD	0x0D
#define qoz_R_FIFO	0x0F
#define qoz_R_FIRST_FIFO	0x0B
#define qoz_R_FSM_IDX	0x0F
#define qoz_R_GPIO_EN0	0x42
#define qoz_R_GPIO_EN1	0x43
#define qoz_R_GPIO_OUT0	0x40
#define qoz_R_GPIO_OUT1	0x41
#define qoz_R_GPIO_SEL	0x44
#define qoz_R_IRQ_CTRL	0x13
#define qoz_R_IRQMSK_MISC	0x11
#define qoz_R_PCM_MD0	0x14
#define qoz_R_PCM_MD1	0x15
#define qoz_R_PCM_MD2	0x15
#define qoz_R_PWM_MD	0x46
#define qoz_R_PWM0	0x38
#define qoz_R_PWM1	0x39
#define qoz_R_RAM_ADDR0	0x08
#define qoz_R_RAM_ADDR1	0x09
#define qoz_R_RAM_ADDR2	0x0A
#define qoz_R_RAM_MISC	0x0C
#define qoz_R_SCI_MSK	0x12
#define qoz_R_SH0H	0x15
#define qoz_R_SH0L	0x15
#define qoz_R_SH1H	0x15
#define qoz_R_SH1L	0x15
#define qoz_R_SL_SEL0	0x15
#define qoz_R_SL_SEL1	0x15
#define qoz_R_SL_SEL2	0x15
#define qoz_R_SL_SEL3	0x15
#define qoz_R_SL_SEL4	0x15
#define qoz_R_SL_SEL5	0x15
#define qoz_R_SL_SEL6	0x15
#define qoz_R_SL_SEL7	0x15
#define qoz_R_SLOT	0x10
#define qoz_R_ST_SEL	0x16
#define qoz_R_ST_SYNC	0x17
#define qoz_R_TI_WD	0x1A

/* Read only registers */
#define qoz_A_F1	0x0C
#define qoz_A_F12	0x0C
#define qoz_A_F2	0x0D
#define qoz_A_ST_B1_RX	0x3C
#define qoz_A_ST_B2_TX	0x3D
#define qoz_A_ST_D_RX	0x3E
#define qoz_A_ST_E_RX	0x3F
#define qoz_A_ST_RD_STA	0x30
#define qoz_A_ST_SQ_RD	0x34
#define qoz_A_Z1	0x04
#define qoz_A_Z12	0x04
#define qoz_A_Z1H	0x05
#define qoz_A_Z1L	0x04
#define qoz_A_Z2	0x06
#define qoz_A_Z2H	0x07
#define qoz_A_Z2L	0x06
#define qoz_R_BERT_ECH	0x1B
#define qoz_R_BERT_ECL	0x1A
#define qoz_R_BERT_STA	0x17
#define qoz_R_CHIP_ID	0x16
#define qoz_R_CHIP_RV	0x1F
#define qoz_R_CONF_OFLOW	0x14
#define qoz_R_F0_CNTH	0x19
#define qoz_R_F0_CNTL	0x18
#define qoz_R_GPI_IN0	0x44
#define qoz_R_GPI_IN1	0x45
#define qoz_R_GPI_IN2	0x46
#define qoz_R_GPI_IN3	0x47
#define qoz_R_GPIO_IN0	0x40
#define qoz_R_GPIO_IN1	0x41
#define qoz_R_INT_DATA	0x88
#define qoz_R_IRQ_FIFO_BL0	0xC8
#define qoz_R_IRQ_FIFO_BL1	0xC9
#define qoz_R_IRQ_FIFO_BL2	0xCA
#define qoz_R_IRQ_FIFO_BL3	0xCB
#define qoz_R_IRQ_FIFO_BL4	0xCC
#define qoz_R_IRQ_FIFO_BL5	0xCD
#define qoz_R_IRQ_FIFO_BL6	0xCE
#define qoz_R_IRQ_FIFO_BL7	0xCF
#define qoz_R_IRQ_MISC	0x11
#define qoz_R_IRQ_OVIEW	0x10
#define qoz_R_RAM_USE	0x15
#define qoz_R_SCI	0x12
#define qoz_R_STATUS	0x1C

/* Read/Write registers */
#define qoz_A_FIFO_DATA0_NOINC	0x84
#define qoz_A_FIFO_DATA0	0x80
#define qoz_A_FIFO_DATA1_NOINC	0x84
#define qoz_A_FIFO_DATA1	0x80
#define qoz_A_FIFO_DATA2_NOINC	0x84
#define qoz_A_FIFO_DATA2	0x80
#define qoz_R_RAM_DATA	0xC0

#define PCI_DEVICE_ID_CCD_M	0x16b8
#define PCI_DEVICE_ID_CCD_M4	0x08b4
#define CLKDEL_TE	0xe	/* CLKDEL in TE mode */
#define CLKDEL_NT	0xc	/* CLKDEL in NT mode */

#define HFC8S_CHIP_ID	0x80
#define HFC4S_CHIP_ID	0xC0

#define qoz_WD_P0	0x000000
#define qoz_WD_P1	0x808080
#define qoz_WD_P2	0x404040

#define qoz_T3		3
#define qoz_T4		1


