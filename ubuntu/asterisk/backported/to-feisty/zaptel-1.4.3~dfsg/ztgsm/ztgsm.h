#define ztgsm_RX_MAX	1000
#define ztgsm_FIFO_SIZE	16
#define ztgsm_SPANS	4
#define ztgsm_SER_BUF_SIZE	1000
#define ztgsm_FRAMES	0x100

typedef struct ztgsm_span {
    int led;	/* 0 == RED 1 == GREEN  0x80 == on 0x40 == blink */
    int sim_led;
    /* zaptel resources */
    struct zt_span span;
    struct zt_chan chans[2];

    /* more zaptel stuff */
    unsigned int usecount;
    int spantype;
    int spanflags;
} ztgsm_span;

typedef struct ztgsm_card {
    spinlock_t lock;
    unsigned char power[ztgsm_SPANS];
    int cardID;
    int dead;
    unsigned char cardno;
    unsigned int irq;
    unsigned int iomem;
    void *pci_io;
    unsigned int framecnt;
    unsigned int last_framecnt;
    unsigned long pci_io_phys;
    unsigned int version;
//    unsigned char *pci_io;
//    unsigned char *pci_io_phys;
    unsigned long iomem_size;
    unsigned long ioport;
    struct ztgsm_span gsmspan[ztgsm_SPANS];
    unsigned int gsmspans;
    unsigned int pcibus;
    unsigned int pcidevfn;
    struct pci_dev *pcidev;
    unsigned char gsms;
    unsigned int ticks;
    unsigned int clicks;
    unsigned int type;
    unsigned char rxbuf[ztgsm_SPANS][ZT_CHUNKSIZE];
    unsigned char txbuf[ztgsm_SPANS][ZT_CHUNKSIZE];
    unsigned char drxbuf[ztgsm_SPANS][ztgsm_SER_BUF_SIZE];
    unsigned char dtxbuf[ztgsm_SPANS][ztgsm_FIFO_SIZE];
    unsigned short rxbufi[ztgsm_SPANS];

    unsigned char ser_rx_buf[ztgsm_SPANS][ztgsm_SER_BUF_SIZE];
    unsigned char ser_tx_buf[ztgsm_SPANS][ztgsm_SER_BUF_SIZE];
    unsigned short ser_rx_idx[ztgsm_SPANS];
    unsigned short ser_tx_idx[ztgsm_SPANS];
    unsigned char tx_wm_sen;
    struct ztgsm_card *next;
    struct ztgsm_card *prev;
} ztgsm_card;

#define ztgsm_outb_io(a,b,c) \
    outw((b), ((a)->ioport+4)); \
    outb((c), ((a)->ioport));

#define ztgsm_inb_io(a,b) ({ outw((b), (a)->ioport+4); inb((a)->ioport); })

#define ztgsm_outw_io(a,b,c) \
    outw((b), ((a)->ioport+4)); \
    outw((c), ((a)->ioport));

#define ztgsm_inw_io(a,b) ({ outw((b), (a)->ioport+4); inw((a)->ioport); })

#define _ztgsm_outdw_io(a,b,c) \
    outw((b), ((a)->ioport+4)); \
    outl((c), ((a)->ioport));

#define ztgsm_outdw_io(a,b,c) (outl((c), ((a)->ioport+b)))

#define ztgsm_indw_io(a,b) (inl((a)->ioport+b))

#define ztgsm_outb(a,b,c) (writeb((c),(a)->pci_io+(b)))
#define ztgsm_inb(a,b) (readb((a)->pci_io+(b)))

#define ztgsm_outw(a,b,c) (writew((c),(a)->pci_io+(b)))
#define ztgsm_inw(a,b) (readw((a)->pci_io+(b)))

#define ztgsm_outdw(a,b,c) (writel((c),(a)->pci_io+(b)))
#define ztgsm_indw(a,b) (readl((a)->pci_io+(b)))

#define ztgsm_IO_BASE		0x0
#define ztgsm_VERS_A		ztgsm_IO_BASE
#define ztgsm_SER_CLK_DIV	ztgsm_IO_BASE + 4
#define ztgsm_SER_CLK_PRE_DIV	ztgsm_IO_BASE + (4 * 0x02)
#define ztgsm_PCM_CLK_PRE_DIV	ztgsm_IO_BASE + (4 * 0x03)
#define ztgsm_SER_IDLE_VAL	ztgsm_IO_BASE + (4 * 0x04)
#define ztgsm_SER_RTS_O 	ztgsm_IO_BASE + (4 * 0x05)
#define ztgsm_SER_TX_EN		ztgsm_IO_BASE + (4 * 0x06)
#define ztgsm_SER_RX_EN		ztgsm_IO_BASE + (4 * 0x07)
#define ztgsm_SER_DTR_ON_OFF 	ztgsm_IO_BASE + (4 * 0x08)
#define ztgsm_DIP_SWITCH 	ztgsm_IO_BASE + (4 * 0x09)
#define ztgsm_LED_DUAL 		ztgsm_IO_BASE + (4 * 0x0A)
#define ztgsm_SER_G20_ACTIVATED	ztgsm_IO_BASE + (4 * 0x0B)
#define ztgsm_SIM_SEL 		ztgsm_IO_BASE + (4 * 0x0C)
#define ztgsm_PCM_DIR 		ztgsm_IO_BASE + (4 * 0x0D)

#define ztgsm_SER_TX_DATA_A	ztgsm_IO_BASE + (4 * 0x10)
#define ztgsm_SER_TX_COUNT_A	ztgsm_IO_BASE + (4 * 0x11)
#define ztgsm_SER_TX_DATA_B	ztgsm_IO_BASE + (4 * 0x12)
#define ztgsm_SER_TX_COUNT_B	ztgsm_IO_BASE + (4 * 0x13)
#define ztgsm_SER_TX_DATA_C	ztgsm_IO_BASE + (4 * 0x14)
#define ztgsm_SER_TX_COUNT_C	ztgsm_IO_BASE + (4 * 0x15)
#define ztgsm_SER_TX_DATA_D	ztgsm_IO_BASE + (4 * 0x16)
#define ztgsm_SER_TX_COUNT_D	ztgsm_IO_BASE + (4 * 0x17)
#define ztgsm_SER_RX_DATA_A	ztgsm_IO_BASE + (4 * 0x18)
#define ztgsm_SER_RX_COUNT_A	ztgsm_IO_BASE + (4 * 0x19)
#define ztgsm_SER_RX_DATA_B	ztgsm_IO_BASE + (4 * 0x1a)
#define ztgsm_SER_RX_COUNT_B	ztgsm_IO_BASE + (4 * 0x1b)
#define ztgsm_SER_RX_DATA_C	ztgsm_IO_BASE + (4 * 0x1c)
#define ztgsm_SER_RX_COUNT_C	ztgsm_IO_BASE + (4 * 0x1d)
#define ztgsm_SER_RX_DATA_D	ztgsm_IO_BASE + (4 * 0x1e)
#define ztgsm_SER_RX_COUNT_D	ztgsm_IO_BASE + (4 * 0x1f)
#define ztgsm_SER_STATUS	ztgsm_IO_BASE + (4 * 0x20)
#define ztgsm_SER_INT_MASK	ztgsm_IO_BASE + (4 * 0x21)
#define ztgsm_SER_RX_WATERMARK	ztgsm_IO_BASE + (4 * 0x22)
#define ztgsm_SER_TX_WATERMARK	ztgsm_IO_BASE + (4 * 0x23)
#define ztgsm_SER_TX_WM_SEN	ztgsm_IO_BASE + (4 * 0x24)

#define ztgsm_PCM_SAP_EN	ztgsm_IO_BASE + (4 * 0x30)
#define ztgsm_PCM_MASK_LAST	ztgsm_IO_BASE + (4 * 0x31)
#define ztgsm_PCM_FRAME_CNT	ztgsm_IO_BASE + (4 * 0x32)
#define ztgsm_PCM_FC_TOG_BIT	ztgsm_IO_BASE + (4 * 0x33)
#define ztgsm_PCM_SIGNAL_CFG	ztgsm_IO_BASE + (4 * 0x34)
#define ztgsm_PCM_VECTOR_CFG	ztgsm_IO_BASE + (4 * 0x35)


