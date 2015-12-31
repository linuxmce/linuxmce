/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2004-2006, Xorcom
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
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#  warning "This module is tested only with 2.6 kernels"
#endif

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/delay.h>	/* for udelay */
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/timex.h>
#include <linux/proc_fs.h>
#include <linux/usb.h>
#include "xpd.h"
#include "xproto.h"
#include "xbus-core.h"
#ifdef	DEBUG
#include "card_fxs.h"
#include "card_fxo.h"
#endif
#include "parport_debug.h"

static const char rcsid[] = "$Id: xpp_usb.c 2536 2007-05-17 21:47:40Z tzafrir $";

DEF_PARM(int, print_dbg, 0, 0644, "Print DBG statements");	/* must be before zap_debug.h */

#include "zap_debug.h"

/* FIXME: A flag that was deprecated at some point, and rather useless */
/* anyway. Only used in the code or-ed to other flags                  */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
#  define URB_ASYNC_UNLINK 0
#endif
/* Get a minor range for your devices from the usb maintainer */
#define USB_SKEL_MINOR_BASE	192

#ifdef CONFIG_PROC_FS
#define	PROC_USBXPP_SUMMARY	"xpp_usb"
#endif

#ifdef	DEBUG_PCM_TIMING
static cycles_t	stamp_last_pcm_read;
static cycles_t accumulate_diff;
#endif

struct xusb_model_info;

struct xusb_endpoint {
	int		ep_addr;
	int		max_size;
	usb_complete_t	callback;
};

static int xusb_xframe_send(xbus_t *xbus, xframe_t *xframe);

xbus_ops_t	xusb_ops = {
	.xframe_send = xusb_xframe_send,
	.xframe_new = NULL,			// Default allocator
	.xframe_free = NULL,			// Default deallocator
};

enum {
	XUSB_N_RX_FRAMES,
	XUSB_N_TX_FRAMES,
	XUSB_N_RX_ERRORS,
	XUSB_N_TX_ERRORS,
};

#define	XUSB_COUNTER(xusb, counter)	((xusb)->counters[XUSB_N_ ## counter])

#define	C_(x)	[ XUSB_N_ ## x ] = { #x }

static struct xusb_counters {
	char	*name;
} xusb_counters[] = {
	C_(RX_FRAMES),
	C_(TX_FRAMES),
	C_(RX_ERRORS),
	C_(TX_ERRORS),
};

#undef C_

#define	XUSB_COUNTER_MAX	ARRAY_SIZE(xusb_counters)

#define	MAX_PENDING_WRITES	100

enum xusb_dir {
	XUSB_RECV = 0,
	XUSB_SEND = 1,
};

/*
 * USB XPP Bus (a USB Device)
 */
typedef struct xpp_usb_bus {
	xbus_t			*xbus;
	struct usb_device	*udev;			/* save off the usb device pointer */
	struct usb_interface	*interface;		/* the interface for this device */
	unsigned char		minor;			/* the starting minor number for this device */

	struct xusb_model_info	*model_info;
	struct xusb_endpoint	endpoints[2];		/* RECV/SEND endpoints */

	struct urb		*read_urb;

	struct completion	write_finished;		/* wait for the write to finish */

	int			present;		/* if the device is not disconnected */
	int			reading;		/* is the read_urb reading (listening) */
	atomic_t		pending_writes;		/* submited but not out yet */
	struct semaphore	sem;			/* locks this structure */
	int		counters[XUSB_COUNTER_MAX];

	/* metrics */
	struct timeval		last_tx;
	unsigned int		max_tx_delay;
} xusb_t;

static	spinlock_t	xusb_lock = SPIN_LOCK_UNLOCKED;
static xusb_t *xusb_array[MAX_BUSES] = {};
static unsigned bus_count = 0;


/* prevent races between open() and disconnect() */
static DECLARE_MUTEX (disconnect_sem);

/*
 * Function Prototypes
 */
#if 0
static ssize_t xusb_read	(struct file *file, char *buffer, size_t count, loff_t *ppos);
static ssize_t xusb_write	(struct file *file, const char *buffer, size_t count, loff_t *ppos);
static int xusb_ioctl		(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int xusb_open		(struct inode *inode, struct file *file);
static int xusb_release		(struct inode *inode, struct file *file);
static void xusb_write_bulk_callback	(struct urb *urb, struct pt_regs *regs);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#define	USB_PASS_CB(u)	struct urb *u, struct pt_regs *regs
#else
#define	USB_PASS_CB(u)	struct urb *u
#endif

static void xpp_urb_delete(struct urb *urb);
static struct urb *xpp_urb_new(xusb_t *dev, enum xusb_dir dir, size_t size);
static void xpp_send_callback(USB_PASS_CB(urb));
static void xpp_receive_callback(USB_PASS_CB(urb));

static int xusb_probe		(struct usb_interface *interface, const struct usb_device_id *id);
static void xusb_disconnect	(struct usb_interface *interface);
static int xusb_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data);

/*------------------------------------------------------------------*/
#if 0
#ifndef DEBUG

#define	packet_debug(m, x, p)

#else

/* FIXME FIXME FIXME: should move code to card methods */
static void packet_debug(const char msg[], xusb_t *xusb, xpacket_t *pack)
{
	char	title[XBUS_DESCLEN];

	if(pack->opcode == XPROTO_NAME(GLOBAL,PCM_READ)) {
#ifdef	DEBUG_PCM_TIMING
		/*
		 * DEBUG: high-res timing of PCM_READ to PCM_WRITE
		 */
		stamp_last_pcm_read = get_cycles();
#endif
#if 0
		// fill_beep((u_char *)&PACKET_FIELD(pack, PCM_READS, pcm), 2);	// Debugging BEEP
		static	int rate_limit;
		if((rate_limit++ % 1000) < 10)
			dump_packet("USB RECEIVE PCM", pack, print_dbg);
#endif
		return;
	} else if(pack->opcode == XPROTO_NAME(GLOBAL,PCM_WRITE)) {
#ifdef	DEBUG_PCM_TIMING
		/*
		 * DEBUG: high-res timing of PCM_READ to PCM_WRITE
		 */
		cycles_t	diff = get_cycles() - stamp_last_pcm_read;
		accumulate_diff += diff;
#endif
#if 0
		static	int rate_limit;
		if((rate_limit++ % 1000) < 10)
			dump_packet("USB SEND PCM", pack, print_dbg);
#endif
		return;
	} else if(pack->opcode == XPROTO_NAME(GLOBAL, REGISTER_REQUEST)) {
		reg_cmd_t	*regcmd;

		regcmd = &RPACKET_FIELD(pack, GLOBAL, REGISTER_REQUEST, reg_cmd);
		if(REG_FIELD(regcmd, regnum) == 0x06)		/* ignore SLIC_QUERY */
			return;
		if(REG_FIELD(regcmd, regnum) == DAA_VBAT_REGISTER)	/* ignore DAA_QUERY */
			return;
		if(REG_FIELD(regcmd, regnum) == 0x30) {		/* ignore BRI query */
#if 0
			static	int rate_limit;
			if((rate_limit++ % 1000) < 10)
				dump_packet("BRI STATE REG", pack, print_dbg);
#endif
			return;
		}
#if 0
	} else if(pack->opcode == XPROTO_NAME(FXS, REGISTER_REPLY)) {
			return;
#endif
	}
	snprintf(title, XBUS_DESCLEN, "%s: %s", msg, xusb->xbus->busname);
	dump_packet(title, pack, print_dbg);
}

#endif
#endif

static int xusb_xframe_send(xbus_t *xbus, xframe_t *xframe)
{
	xusb_t			*xusb;
	struct urb		*urb;
	int			ret = 0;
	size_t			size;
	struct xusb_endpoint	*xusb_ep;

	BUG_ON(!xframe);
	BUG_ON(!xbus);
	xusb = xbus->priv;
	BUG_ON(!xusb);
	if(!xusb->present) {
		static	int rate_limit;

		if((rate_limit++ % 5000) == 0)
			ERR("%s: USB device not present. Dropping packets (#%d).\n",
					xbus->busname, rate_limit);
		ret = -ENODEV;
		goto freepack;
	}
	/*
	 * If something really bad happend, do not overflow the USB stack
	 */
	if(atomic_read(&xusb->pending_writes) > MAX_PENDING_WRITES) {
		static	int rate_limit;

		if((rate_limit++ % 5000) == 0)
			ERR("%s: USB device is totaly stuck. Dropping packets (#%d).\n",
					xbus->busname, rate_limit);
		ret = -ENODEV;
		goto freepack;
	}
	if (print_dbg)
		dump_xframe("USB_FRAME_SEND", xbus, xframe);
	size = XFRAME_LEN(xframe);
	xusb_ep = &xusb->endpoints[XUSB_SEND];
	urb = xpp_urb_new(xusb, XUSB_SEND, size);
	if (!urb) {
		ERR("No free urbs available\n");
		ret = -ENOMEM;
		goto freepack;
	}

	/* FIXME: FIXME: FIXME: we use copy+free until low-level drivers allocate memory themselves */
	
	memcpy(urb->transfer_buffer, xframe->packets, size);

	ret = usb_submit_urb(urb, GFP_ATOMIC);
	if(ret < 0) {
		static int rate_limit;

		if((rate_limit++ % 1000) < 5)
			ERR("%s: failed submit_urb: %d\n", __FUNCTION__, ret);
		xpp_urb_delete(urb);
		ret = -EBADF;
		goto freepack;
	}
	do_gettimeofday(&xusb->last_tx);
	atomic_inc(&xusb->pending_writes);
freepack:
	xbus->ops->xframe_free(xbus, xframe);	// FIXME: eventually will be done in the urb callback
	if(ret < 0)
		XUSB_COUNTER(xusb, TX_ERRORS)++;
	return ret;
}

static void xpp_urb_delete(struct urb *urb)
{
	BUG_ON(!urb);
	// DBG("%s: (%d) %p %X", __FUNCTION__, urb->transfer_buffer_length, urb->transfer_buffer, urb->transfer_dma);
	usb_buffer_free (urb->dev, urb->transfer_buffer_length,
			urb->transfer_buffer,
			urb->transfer_dma);
	usb_free_urb(urb);
}

static struct urb *xpp_urb_new(xusb_t *xusb, enum xusb_dir dir, size_t size)
{
	struct usb_device	*udev = xusb->udev;
	struct xusb_endpoint	*xusb_ep = &xusb->endpoints[dir];
	unsigned int	ep_addr = xusb_ep->ep_addr;
	usb_complete_t	urb_cb = xusb_ep->callback;
	struct urb	*urb;
	unsigned char	*buffer;	/* the buffer to send data */
	unsigned int	epnum = ep_addr & USB_ENDPOINT_NUMBER_MASK;
	int		pipe = usb_pipein(ep_addr)
					? usb_rcvbulkpipe(udev, epnum)
					: usb_sndbulkpipe(udev, epnum);
		 
	if(size > xusb_ep->max_size)
		return NULL;
	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb) {
		err("No free urbs available");
		return NULL;
	}

	/* on some platforms using this kind of buffer alloc
	 * call eliminates a dma "bounce buffer".
	 *
	 * NOTE: you'd normally want i/o buffers that hold
	 * more than one packet, so that i/o delays between
	 * packets don't hurt throughput. (Probably applies only to isochronous 
	 * transfers)
	 */
	urb->transfer_flags = (URB_NO_TRANSFER_DMA_MAP | URB_ASYNC_UNLINK);
	buffer = usb_buffer_alloc(udev, size, GFP_ATOMIC, &urb->transfer_dma);
	// DBG("(%d) %p / %x", size, buffer, urb->transfer_dma);
	if (!buffer) {
		err("Couldn't allocate buffer");
		usb_free_urb(urb);
		return NULL;
	}
	usb_fill_bulk_urb(urb, udev, pipe, buffer, size, urb_cb, xusb);
	return urb;
}

/*------------------------- XPP USB Bus Handling -------------------*/

#define	XUSB_MODEL(ep_in,ep_out,type,str)	\
	{					\
		.in = { .ep_addr = (ep_in) },	\
		.out = { .ep_addr = (ep_out) },	\
		.bus_type = (type),		\
		.desc = (str)			\
	}

static const struct xusb_model_info {
	const char		*desc;
	struct xusb_endpoint	in;
	struct xusb_endpoint	out;
	xbus_type_t		bus_type;
} model_table[] = {
	XUSB_MODEL(0x86, 0x02, FIRMWARE_XPP,		"FPGA_XPD"),
};

/* table of devices that work with this driver */
static const struct usb_device_id xusb_table [] = {
//	{ USB_DEVICE(0x04B4, 0x8613) }, // default of cypress
	{ USB_DEVICE(0xE4E4, 0x2211), .driver_info=(kernel_ulong_t)&model_table[0] }, // OLD FPGA
	{ USB_DEVICE(0xE4E4, 0x1132), .driver_info=(kernel_ulong_t)&model_table[0] }, // FPGA_FXS
	{ USB_DEVICE(0xE4E4, 0x1142), .driver_info=(kernel_ulong_t)&model_table[0] }, // FPGA_1141
	{ USB_DEVICE(0xE4E4, 0x1152), .driver_info=(kernel_ulong_t)&model_table[0] }, // FPGA_1151
	/* "Gadget Zero" firmware runs under Linux */
	//{ USB_DEVICE(0x0525, 0xa4a0) },
	{ }					/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, xusb_table);


/* usb specific object needed to register this driver with the usb subsystem */
static struct usb_driver xusb_driver = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
	.owner =	THIS_MODULE,
#endif
	.name =		"xpp_usb",
	.probe =	xusb_probe,
	.disconnect =	xusb_disconnect,
	.id_table =	xusb_table,
};

/*
 * File operations needed when we register this driver.
 * This assumes that this driver NEEDS file operations,
 * of course, which means that the driver is expected
 * to have a node in the /dev directory. If the USB
 * device were for a network interface then the driver
 * would use "struct net_driver" instead, and a serial
 * device would use "struct tty_driver".
 */
static struct file_operations xusb_fops = {
	/*
	 * The owner field is part of the module-locking
	 * mechanism. The idea is that the kernel knows
	 * which module to increment the use-counter of
	 * BEFORE it calls the device's open() function.
	 * This also means that the kernel can decrement
	 * the use-counter again before calling release()
	 * or should the open() function fail.
	 */
	.owner =	THIS_MODULE,
};

/* 
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
static struct usb_class_driver xusb_class = {
	.name =		"usb/xpp_usb%d",
	.fops =		&xusb_fops,
/* FIXME: The sysfs class interfase seems to have chaged around here */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)
	.mode =		S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH,
#endif
	.minor_base =	USB_SKEL_MINOR_BASE,
};

/*
 * set up the endpoint information
 * check out the endpoints
 * FIXME: Should be simplified (above 2.6.10) to use usb_dev->ep_in[0..16] and usb_dev->ep_out[0..16]
 */
static int set_endpoints(xusb_t *xusb, struct usb_interface *interface, struct xusb_model_info *model_info)
{
	struct usb_host_interface	*iface_desc;
	struct usb_endpoint_descriptor	*endpoint;
	struct xusb_endpoint		*xusb_ep;
	int				ep_addr;
	int i;

	iface_desc = &interface->altsetting[0];
	DBG("Found interface. Endpoints: %d\n", iface_desc->desc.bNumEndpoints);

#define	BULK_ENDPOINT(ep) (((ep)->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)

	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		ep_addr = endpoint->bEndpointAddress;
		
		if(!BULK_ENDPOINT(endpoint)) {
			DBG("endpoint 0x%x is not bulk: mbAttributes=0x%X\n",
					ep_addr, endpoint->bmAttributes);
			continue;
		}
		if(usb_pipein(ep_addr)) {	// Input
			if(ep_addr == model_info->in.ep_addr) {
				if(endpoint->wMaxPacketSize < sizeof(xframe_t)) {
					NOTICE("USB input endpoint 0x%X support only wMaxPacketSize=%d (need USB-2)\n", ep_addr, endpoint->wMaxPacketSize);
				}
				xusb_ep = &xusb->endpoints[XUSB_RECV];
				xusb_ep->ep_addr = ep_addr;
				xusb_ep->max_size = endpoint->wMaxPacketSize;
				xusb_ep->callback = xpp_receive_callback;
			}
		} else {			// Output
			if(ep_addr == model_info->out.ep_addr) {
				if(endpoint->wMaxPacketSize < sizeof(xframe_t)) {
					NOTICE("USB output endpoint 0x%X support only wMaxPacketSize=%d (need USB-2)\n", ep_addr, endpoint->wMaxPacketSize);
				}
				xusb_ep = &xusb->endpoints[XUSB_SEND];
				xusb_ep->ep_addr = ep_addr;
				xusb_ep->max_size = endpoint->wMaxPacketSize;
				xusb_ep->callback = xpp_send_callback;
			}
		}
	}
	if (!xusb->endpoints[XUSB_RECV].ep_addr || !xusb->endpoints[XUSB_SEND].ep_addr) {
		ERR("Couldn't find bulk-in or bulk-out endpoints\n");
		return 0;
	}
	DBG("in=0x%02X out=0x%02X\n", xusb->endpoints[XUSB_RECV].ep_addr, xusb->endpoints[XUSB_SEND].ep_addr);
	return 1;
}

/**
 *	xusb_probe
 *
 *	Called by the usb core when a new device is connected that it thinks
 *	this driver might be interested in.
 */
static int xusb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device	*udev = interface_to_usbdev(interface);
	xusb_t			*xusb = NULL;
	struct xusb_model_info	*model_info = (struct xusb_model_info*)id->driver_info;
	struct proc_dir_entry	*procsummary = NULL;
	xbus_t			*xbus = NULL;
	struct xusb_endpoint	*xusb_ep;
	unsigned long		flags;
	int			retval = -ENOMEM;
	int			i;
	
	INFO("New XUSB device MODEL=%s bus_type=%d\n", model_info->desc, model_info->bus_type);

	/* The USB stack before 2.6.10 seems to be a bit shoddy. It seems that when being called
	 * from the probe we may already have the lock to udev (the Usb DEVice). Thus we call
	 * the internal __usb_reset_device instead. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10)
	if((retval = __usb_reset_device(udev)) < 0) {
#else
	if((retval =   usb_reset_device(udev)) < 0) {
#endif
		ERR("usb_reset_device failed: %d\n", retval);
		goto probe_failed;
	}
	if (!model_info) {
		ERR("Missing endpoint setup for this device %d:%d\n", 
				udev->descriptor.idVendor,udev->descriptor.idProduct);
		retval = -ENODEV;
		goto probe_failed;
	}

	/* allocate memory for our device state and initialize it */
	xusb = kmalloc(sizeof(xusb_t), GFP_KERNEL);
	if (xusb == NULL) {
		ERR("xpp_usb: Unable to allocate new xpp usb bus\n");
		retval = -ENOMEM;
		goto probe_failed;
	}
	memset(xusb, 0, sizeof(xusb_t));

	init_MUTEX (&xusb->sem);
	atomic_set(&xusb->pending_writes, 0);
	xusb->udev = udev;
	xusb->interface = interface;
	xusb->model_info = model_info;

	if(!set_endpoints(xusb, interface, model_info)) {
		retval = -ENODEV;
		goto probe_failed;
	}
	xusb_ep = &xusb->endpoints[XUSB_RECV];
	xusb->read_urb = xpp_urb_new(xusb, XUSB_RECV, xusb_ep->max_size);
	if (!xusb->read_urb) {
		ERR("No free urbs available\n");
		retval = -ENOMEM;
		goto probe_failed;
	}

	/* allow device read, write and ioctl */
	xusb->present = 1;

	/* we can register the device now, as it is ready */
	usb_set_intfdata (interface, xusb);
	retval = usb_register_dev (interface, &xusb_class);
	if (retval) {
		/* something prevented us from registering this driver */
		ERR ("Not able to get a minor for this device.\n");
		goto probe_failed;
	}

	xusb->minor = interface->minor;

	/* let the user know what node this device is now attached to */
	INFO ("USB XPP device now attached to minor %d\n", xusb->minor);

	/* Allocate high level structures */
	xbus = xbus_new(&xusb_ops);
	if(!xbus) {
		retval = -ENOMEM;
		goto probe_failed;
	}
	xbus->bus_type = model_info->bus_type;
	xbus->max_packet_size = min(xusb->endpoints[XUSB_SEND].max_size , xusb->endpoints[XUSB_RECV].max_size);

	spin_lock_irqsave(&xusb_lock, flags);
	for(i = 0; i < MAX_BUSES; i++) {
		if(xusb_array[i] == NULL)
			break;
	}
	spin_unlock_irqrestore(&xusb_lock, flags);
	if(i >= MAX_BUSES) {
		ERR("xpp_usb: Too many XPP USB buses\n");
		retval = -ENOMEM;
		goto probe_failed;
	}
	{
		char	path[XBUS_DESCLEN];

		usb_make_path(udev, path, XBUS_DESCLEN);	// May trunacte... ignore
		snprintf(xbus->busdesc, XBUS_DESCLEN, "%s", path);
	}

	DBG("GOT XPP USB BUS #%d: %s (type=%d)\n", i, xbus->busdesc, xbus->bus_type);

	xusb_array[i] = xusb;


#ifdef CONFIG_PROC_FS
	DBG("Creating proc entry " PROC_USBXPP_SUMMARY " in bus proc dir.\n");
	procsummary = create_proc_read_entry(PROC_USBXPP_SUMMARY, 0444, xbus->proc_xbus_dir,
			xusb_read_proc, xusb);
	if (!procsummary) {
		ERR("Failed to create proc read entry for xbus %s\n", xbus->busname);
		// FIXME: better error handling
		retval = -EIO;
		goto probe_failed;
	}
	procsummary->owner = THIS_MODULE;
#endif
	bus_count++;
	retval = usb_submit_urb(xusb->read_urb, GFP_ATOMIC);
	if(retval < 0) {
		ERR("%s: Failed to submit the receive URB errno=%d\n", __FUNCTION__, retval);
	}
	xusb->xbus = xbus;
	xbus->priv = xusb;
	xbus_activate(xbus);
	return retval;
probe_failed:
	ERR("Failed to initialize xpp usb bus: %d\n", retval);
	usb_set_intfdata (interface, NULL);
	if(xusb) {
		if(xusb->read_urb)
			xpp_urb_delete(xusb->read_urb);
		if(xusb->minor)	// passed registration phase
			usb_deregister_dev(interface, &xusb_class);
		kfree(xusb);
	}
	if(xbus) {
		if(procsummary) {
			DBG("Remove proc_entry: " PROC_USBXPP_SUMMARY "\n");
			remove_proc_entry(PROC_USBXPP_SUMMARY, xbus->proc_xbus_dir);
			procsummary = NULL;
		}
		xbus_disconnect(xbus);		// Blocking until fully deactivated!
	}
	return retval;
}

/**
 *	xusb_disconnect
 *
 *	Called by the usb core when the device is removed from the system.
 *
 *	This routine guarantees that the driver will not submit any more urbs
 *	by clearing dev->udev.  It is also supposed to terminate any currently
 *	active urbs.  Unfortunately, usb_bulk_msg(), used in xusb_read(), does
 *	not provide any way to do this.  But at least we can cancel an active
 *	write.
 */
static void xusb_disconnect(struct usb_interface *interface)
{
	xusb_t			*xusb;
	xbus_t			*xbus;
	int			minor;
	int			i;

	DBG("CALLED\n");
	/* prevent races with open() */
	down (&disconnect_sem);

	xusb = usb_get_intfdata (interface);
	usb_set_intfdata (interface, NULL);
	xbus = xusb->xbus;

	/* find our xusb */
	for(i = 0; i < MAX_BUSES; i++) {
		if(xusb_array[i] == xusb)
			break;
	}
	BUG_ON(i >= MAX_BUSES);
	xusb_array[i] = NULL;

#ifdef CONFIG_PROC_FS
	if(xbus->proc_xbus_dir) {
		DBG("Remove proc_entry: " PROC_USBXPP_SUMMARY "\n");
		remove_proc_entry(PROC_USBXPP_SUMMARY, xbus->proc_xbus_dir);
	}
#endif
	xusb->present = 0;
	xbus_disconnect(xbus);		// Blocking until fully deactivated!

	down (&xusb->sem);

	minor = xusb->minor;

	/* give back our minor */
	usb_deregister_dev (interface, &xusb_class);

	/* terminate an ongoing read */
	/* terminate an ongoing write */
	// FIXME: Does it really kill pending URB's?

	if(xusb->read_urb)
		xpp_urb_delete(xusb->read_urb);

	up (&xusb->sem);
	DBG("Semaphore released\n");

	kfree(xusb);

	up (&disconnect_sem);
	INFO("XUSB #%d now disconnected\n", minor);
}

static void xpp_send_callback(USB_PASS_CB(urb))
{
	xusb_t			*xusb = (xusb_t *)urb->context;
	xbus_t			*xbus = xusb->xbus;
	struct timeval		now;
	long			usec_diff;

	BUG_ON(!xbus);
	//flip_parport_bit(6);
	atomic_dec(&xusb->pending_writes);
	do_gettimeofday(&now);
	usec_diff =
		(now.tv_sec - xusb->last_tx.tv_sec)*1000*1000 +
		(now.tv_usec - xusb->last_tx.tv_usec);
	if(usec_diff > xusb->max_tx_delay)
		xusb->max_tx_delay = usec_diff;
	if(unlikely(usec_diff > 500)) {
		static int rate_limit;

		if((rate_limit++ % 5003) == 0)
			NOTICE("%s: Slagish USB. %ld usec to transmit a frame\n",
				xbus->busname, usec_diff);
	}
	/* sync/async unlink faults aren't errors */
	if (urb->status && !(urb->status == -ENOENT || urb->status == -ECONNRESET)) {
		static	int rate_limit;
		if((rate_limit++ % 1000) < 10)
			DBG("nonzero read bulk status received: %d\n", urb->status);
		XUSB_COUNTER(xusb, TX_ERRORS)++;
	}
	xpp_urb_delete(urb);
	if(!xusb->present) {
		ERR("A packet from non-connected device?\n");
		return;
	}
	/* allow device read, write and ioctl */
	XUSB_COUNTER(xusb, TX_FRAMES)++;
}

static void xpp_receive_callback(USB_PASS_CB(urb))
{
	xusb_t		*xusb = (xusb_t *)urb->context;
	xbus_t		*xbus;
	xpacket_t	*pack;
	xframe_t	*xframe;
	size_t		size;
	int		retval;
	bool		do_resubmit = 1;
	bool		is_inuse = 0;

	BUG_ON(!xusb);
	xbus = xusb->xbus;
	if(!xbus) {
		NOTICE("spurious URB\n");
		return;
	}
	//flip_parport_bit(7);
	if (urb->status) {
		DBG("nonzero read bulk status received: %d\n", urb->status);
		XUSB_COUNTER(xusb, RX_ERRORS)++;
		/* Free old URB, allocate a fresh one */
		if(xusb->read_urb)
			xpp_urb_delete(xusb->read_urb);
		xusb->read_urb = xpp_urb_new(xusb, XUSB_RECV, xusb->endpoints[XUSB_RECV].max_size);
		if (!xusb->read_urb) {
			ERR("URB allocation failed\n");
			do_resubmit = 0;;
		}
		goto end;
	}
	if(!down_read_trylock(&xbus->in_use)) {
		ERR("%s: xbus is going down\n", __FUNCTION__);
		do_resubmit = 0;
		goto end;
	}
	is_inuse = 1;
	if(!xusb->present) {
		ERR("A packet from non-connected device?\n");
		do_resubmit = 0;
		goto end;
	}
	xframe = xbus->ops->xframe_new(xbus, GFP_ATOMIC);
	if(!xframe) {
		ERR("%s: Not enough memory for packets. Dropping\n", __FUNCTION__);
		goto end;
	}
	size = urb->actual_length;
	atomic_set(&xframe->frame_len, size);
	pack = (xpacket_t *)xframe->packets;
	memcpy(xframe->packets, urb->transfer_buffer, size);

	if (print_dbg)
		dump_xframe("USB_FRAME_RECEIVE", xbus, xframe);
	XUSB_COUNTER(xusb, RX_FRAMES)++;
	// Send UP
	xframe_receive(xbus, xframe);
end:
	if(is_inuse)
		up_read(&xbus->in_use);
	if(do_resubmit) {
		retval = usb_submit_urb(urb, GFP_ATOMIC);
		if (retval < 0) {
			ERR("failed re-submitting read urb, error %d\n", retval);
			return;
		}
	}
}


/*------------------------- Initialization -------------------------*/

int __init xpp_usb_init(void)
{
	int result;
	//xusb_t *xusb;

	INFO("%s revision %s\n", THIS_MODULE->name, XPP_VERSION);

	/* register this driver with the USB subsystem */
	result = usb_register(&xusb_driver);
	if (result) {
		ERR("usb_register failed. Error number %d\n", result);
		return result;
	}
	return 0;
}


void __exit xpp_usb_cleanup(void)
{
	DBG("\n");
	/* deregister this driver with the USB subsystem */
	usb_deregister(&xusb_driver);
}



#ifdef CONFIG_PROC_FS

static int xusb_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int 		len = 0;
	unsigned long	flags;
	int		i;
	//unsigned long stamp = jiffies;
	xusb_t		*xusb = data;

	if(!xusb)
		goto out;
	// TODO: probably needs a per-xusb lock:
	spin_lock_irqsave(&xusb_lock, flags);
	len += sprintf(page + len, "Device: %03d/%03d\n",
		xusb->udev->bus->busnum,
		xusb->udev->devnum
		);
	len += sprintf(page + len, "Minor: %d\n"
		"Model Info: Bus Type=%d (%s)\n",
		xusb->minor,
		xusb->model_info->bus_type,
		xusb->model_info->desc
		);
	len += sprintf(page + len, "Endpoints:\n"
		"\tIn:  0x%02X  - Size: %d)\n"
		"\tOut: 0x%02X  - Size: %d)\n",
		xusb->endpoints[XUSB_RECV].ep_addr,
		xusb->endpoints[XUSB_RECV].max_size,
		xusb->endpoints[XUSB_SEND].ep_addr,
		xusb->endpoints[XUSB_SEND].max_size
		);
	len += sprintf(page + len, "\npending_writes=%d\n", atomic_read(&xusb->pending_writes));
	len += sprintf(page + len, "max_tx_delay=%d\n", xusb->max_tx_delay);
	xusb->max_tx_delay = 0;
#ifdef	DEBUG_PCM_TIMING
	len += sprintf(page + len, "\nstamp_last_pcm_read=%lld accumulate_diff=%lld\n", stamp_last_pcm_read, accumulate_diff);
#endif
	len += sprintf(page + len, "\nCOUNTERS:\n");
	for(i = 0; i < XUSB_COUNTER_MAX; i++) {
		len += sprintf(page + len, "\t%-15s = %d\n", xusb_counters[i].name, xusb->counters[i]);
	}
#if 0
	len += sprintf(page + len, "<-- len=%d\n", len);
#endif
	spin_unlock_irqrestore(&xusb_lock, flags);
out:
	if (len <= off+count)
		*eof = 1;
	*start = page + off;
	len -= off;
	if (len > count)
		len = count;
	if (len < 0)
		len = 0;
	return len;

}

#endif



MODULE_DESCRIPTION("XPP USB Driver");
MODULE_AUTHOR("Oron Peled <oron@actcom.co.il>");
MODULE_LICENSE("GPL");
MODULE_VERSION(XPP_VERSION);

module_init(xpp_usb_init);
module_exit(xpp_usb_cleanup);
