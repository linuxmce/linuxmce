#define cwain_FIFO_SIZE	128
#define cwain_DFIFO_SIZE	4096
#define cwain_FRAME_SIZE 16	/* has to be %4==0 */
#define cwain_FIFO_HW cwain_FRAME_SIZE * 2 + ZT_CHUNKSIZE

typedef struct zt_cwain {
    /* pci resources */
    unsigned int irq;
    unsigned int iomem;
    unsigned long ioport;
    unsigned char *pci_io;
    void *pci_io_phys;
    unsigned int pcibus;
    unsigned int pcidevfn;
    struct pci_dev *pcidev;
    unsigned int type;

    /* lock to protect the structure */
    spinlock_t lock;
    int cardID;
    unsigned char cardno;
    
    /* ticks and clicks, fish and chips */
    unsigned int ticks;
    unsigned int clicks;
    unsigned char nt_mode;
    unsigned char sync_sta;
    unsigned char sync;
    int syncmode;
    
    /* blinky blink */
    unsigned char leds[4];

    /* B chan buffers */
    unsigned char rxbuf[30][ZT_CHUNKSIZE];
    unsigned char txbuf[30][ZT_CHUNKSIZE];

    /* buffers */
    unsigned char frxbuf[30][cwain_FRAME_SIZE];
    unsigned char ftxbuf[30][cwain_FRAME_SIZE];
    
    /* number of RXed dchan frames */
    unsigned char drx;
    /* D chan buffer */
    unsigned char drxbuf[cwain_DFIFO_SIZE];
    unsigned char dtxbuf[cwain_DFIFO_SIZE];


    /* zaptel resources */
    struct zt_span span;
    struct zt_chan chans[31];

    /* more zaptel stuff */
    unsigned int usecount;
    int spantype;
    int spanflags;

    /* linked list */
    struct zt_cwain *next;
    struct zt_cwain *prev;

} zt_cwain;

typedef struct zt_cwain_card {
    /* lock to protect the structure */
    spinlock_t lock;

    unsigned int spans;
    unsigned int dead;
    int cardID;
    unsigned char cardno;
    unsigned int ticks;

    struct zt_cwain *span[2];
    int syncsrc;
    int syncs[2];
    int master[2];
    
    unsigned int irq;   
    unsigned int pcibus;

    /* linked list */
    struct zt_cwain_card *next;
    struct zt_cwain_card *prev;
} zt_cwain_card;

#define cwain_outb(a,b,c) (writeb((c),(a)->pci_io+(b)))
#define cwain_inb(a,b) (readb((a)->pci_io+(b)))

#define cwain_outw(a,b,c) (writew((c),(a)->pci_io+(b)))
#define cwain_inw(a,b) (readw((a)->pci_io+(b)))

#define cwain_outdw(a,b,c) (writel((c),(a)->pci_io+(b)))
#define cwain_indw(a,b) (readl((a)->pci_io+(b)))


/* Write only registers */
#define cwain_A_CH_MSK		0xF4
#define cwain_A_CHANNEL 	0xFC
#define cwain_A_CON_HDLC	0xFA
#define cwain_A_CONF		0xD1
#define cwain_A_FIFO_SEQ	0xFD
#define cwain_R_INC_RES_FIFO	0x0E
#define cwain_A_IRQ_MSK		0xFF
#define cwain_A_SL_CFG		0xD0
#define cwain_A_SUBCH_CFG	0xFB
#define cwain_R_BERT_WD_MD	0x1B
#define cwain_R_BRG_PCM_CFG	0x02
#define cwain_R_CIRM		0x00
#define cwain_R_CONF_EN		0x18
#define cwain_R_CTRL		0x01
#define cwain_R_DTMF		0x1C
#define cwain_R_DTMF_N		0x1D
#define cwain_R_E1_WR_STA	0x20
#define cwain_R_FIFO_MD		0x0D
#define cwain_R_FIFO		0x0F
#define cwain_R_FIRST_FIFO	0x0B
#define cwain_R_FSM_IDX		0x0F
#define cwain_R_GPIO_EN0	0x42
#define cwain_R_GPIO_EN1	0x43
#define cwain_R_GPIO_OUT0	0x40
#define cwain_R_GPIO_OUT1	0x41
#define cwain_R_GPIO_SEL	0x44
#define cwain_R_IRQ_CTRL	0x13
#define cwain_R_IRQMSK_MISC	0x11
#define cwain_R_JATT_CFG	0x2F
#define cwain_R_LOS0		0x22
#define cwain_R_LOS1		0x23
#define cwain_R_PCM_MD0		0x14
#define cwain_R_PCM_MD1		0x15
#define cwain_R_PCM_MD2		0x15
#define cwain_R_PWM_MD		0x46
#define cwain_R_PWM0		0x38
#define cwain_R_PWM1		0x39
#define cwain_R_RAM_ADDR0	0x08
#define cwain_R_RAM_ADDR1	0x09
#define cwain_R_RAM_ADDR2	0x0A
#define cwain_R_RAM_MISC	0x0C
#define cwain_R_RX_OFFS		0x30
#define cwain_R_RX_SL0_CFG0	0x25
#define cwain_R_RX_SL0_CFG1	0x26
#define cwain_R_RX0		0x24
#define cwain_R_SH0H		0x15
#define cwain_R_SH0L		0x15
#define cwain_R_SH1H		0x15
#define cwain_R_SH1L		0x15
#define cwain_R_SL_SEL0		0x15
#define cwain_R_SL_SEL1		0x15
#define cwain_R_SL_SEL2		0x15
#define cwain_R_SL_SEL3		0x15
#define cwain_R_SL_SEL4		0x15
#define cwain_R_SL_SEL5		0x15
#define cwain_R_SL_SEL6		0x15
#define cwain_R_SL_SEL7		0x15
#define cwain_R_SLOT		0x10
#define cwain_R_SYNC_CTRL	0x35
#define cwain_R_SYNC_OUT	0x31
#define cwain_R_TI_WD		0x1A
#define cwain_R_TX_OFFS		0x34
#define cwain_R_TX_SL0_CFG0	0x2C
#define cwain_R_TX_SL0_CFG1	0x2E
#define cwain_R_TX_SL0		0x2D
#define cwain_R_TX0		0x28
#define cwain_R_TX1		0x29

#define cwain_R_TX_FR0		0x2C
#define cwain_R_TX_FR1		0x2D
#define cwain_R_TX_FR2		0x2E

#define cwain_R_RX_FR0		0x25
#define cwain_R_RX_FR1		0x26

/* Read only registers */
#define cwain_A_F1		0x0C
#define cwain_A_F12		0x0C
#define cwain_A_F2		0x0D
#define cwain_A_Z1		0x04
#define cwain_A_Z12		0x04
#define cwain_A_Z1H		0x05
#define cwain_A_Z1L		0x04
#define cwain_A_Z2		0x06
#define cwain_A_Z2H		0x07
#define cwain_A_Z2L		0x06
#define cwain_R_BERT_ECH	0x1B
#define cwain_R_BERT_ECL	0x1A
#define cwain_R_BERT_STA	0x17
#define cwain_R_CHIP_ID		0x16
#define cwain_R_CHIP_RV		0x1F
#define cwain_R_CONF_OFLOW	0x14
#define cwain_R_CRC_ECH		0x35
#define cwain_R_CRC_ECL		0x34
#define cwain_R_E_ECH		0x37
#define cwain_R_E_ECL		0x36
#define cwain_R_E1_RD_STA	0x20
#define cwain_R_F0_CNTH		0x19
#define cwain_R_F0_CNTL		0x18
#define cwain_R_FAS_ECH		0x31
#define cwain_R_FAS_ECL		0x30
#define cwain_R_GPI_IN0		0x44
#define cwain_R_GPI_IN1		0x45
#define cwain_R_GPI_IN2		0x46
#define cwain_R_GPI_IN3		0x47
#define cwain_R_GPIO_IN0	0x40
#define cwain_R_GPIO_IN1	0x41
#define cwain_R_INT_DATA	0x88
#define cwain_R_IRQ_FIFO_BL0	0xC8
#define cwain_R_IRQ_FIFO_BL1	0xC9
#define cwain_R_IRQ_FIFO_BL2	0xCA
#define cwain_R_IRQ_FIFO_BL3	0xCB
#define cwain_R_IRQ_FIFO_BL4	0xCC
#define cwain_R_IRQ_FIFO_BL5	0xCD
#define cwain_R_IRQ_FIFO_BL6	0xCE
#define cwain_R_IRQ_FIFO_BL7	0xCF
#define cwain_R_IRQ_MISC	0x11
#define cwain_R_IRQ_OVIEW	0x10
#define cwain_R_JATT_STA	0x2B
#define cwain_R_RAM_USE		0x15
#define cwain_R_RX_SL0_0	0x25
#define cwain_R_RX_SL0_1	0x26
#define cwain_R_RX_SL0_2	0x27
#define cwain_R_SA6_VAL13_ECH	0x39
#define cwain_R_SA6_VAL13_ECL	0x38
#define cwain_R_SA6_VAL23_ECH	0x3B
#define cwain_R_SA6_VAL23_ECL	0x3A
#define cwain_R_SLIP		0x2C
#define cwain_R_STATUS		0x1C
#define cwain_R_SYNC_STA	0x24
#define cwain_R_VIO_ECH		0x33
#define cwain_R_VIO_ECL		0x32


/* Read/Write registers */
#define cwain_A_FIFO_DATA0_NOINC	0x84
#define cwain_A_FIFO_DATA0	0x80
#define cwain_A_FIFO_DATA1_NOINC	0x84
#define cwain_A_FIFO_DATA1	0x80
#define cwain_A_FIFO_DATA2_NOINC	0x84
#define cwain_A_FIFO_DATA2	0x80
#define cwain_R_RAM_DATA	0xC0

#define PCI_DEVICE_ID_CCD_E	0x30b1

#define CLKDEL_TE	0xe	/* CLKDEL in TE mode */
#define CLKDEL_NT	0xc	/* CLKDEL in NT mode */

