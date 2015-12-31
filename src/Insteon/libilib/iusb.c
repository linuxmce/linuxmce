/*
 * $RCSfile: iusb.c,v $  $Revision: 1.1 $  $Name:  $
 * $Id: iusb.c,v 1.1 2006/10/04 03:34:16 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2006/10/04 03:34:16 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2006)
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ----------------------------------------------------------------------------
 *
 *  Description:
 *
 *  This is the interface dependent code that uses libusb to
 *  communicate with the Insteon PLC.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <usb.h>

#include "idrv.h"


void *iusb_init(char *dev);
void iusb_close(iusb_t *iplc);
int iusb_read(iusb_t *iplc);
int iusb_write_echo(iusb_t *iplc, unsigned char *pkt, int rt);
int iusb_write(iusb_t *iplc, unsigned char *pkt, int rt);
int iusb_cts(iusb_t *iplc, int cts);


/*
 * locate_iplc
 *
 * Scan the USB bus for a device matching the INSTEON USB controller
 */
usb_dev_handle *locate_iplc()
{
	struct usb_bus *busses, *bus;
	struct usb_device *dev;
	struct usb_dev_handle *dev_handle = NULL;

	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();
	for (bus = busses; bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (!dev_handle && (dev->descriptor.idVendor == IPLC_VENDOR) &&
					(dev->descriptor.idProduct == IPLC_PRODUCT)) {
				dev_handle = usb_open(dev);
			}
		}
	}
	return dev_handle;
}

/*
 * iusb_init
 *
 * This function initializes the USB connection to the PLC. This must
 * be called before any other function since the handle returned is
 * passed as an argument to all other functions.
 */
void *iusb_init(char *dev)
{
	struct usb_dev_handle *dev_handle = NULL;
	int claimed = 0;

	usb_set_debug(0);

	if ((dev_handle = locate_iplc()) != NULL) {
		if (usb_claim_interface(dev_handle, 0) != 0) {
			/* Try detaching the kernel driver? */
			if (usb_detach_kernel_driver_np(dev_handle, 0) != 0) {
				fprintf(stderr, "Error detaching kernel driver.\n");
			} else {
				/* Try claiming it again */
				if (usb_claim_interface(dev_handle, 0) != 0) {
					fprintf(stderr, "Error claiming interface.\n");
				} else {
					claimed = 1;
				}
			}
		} else {
			claimed = 1;
		}

		if (claimed) {
			usb_clear_halt(dev_handle, 1);
		} else {
			usb_close(dev_handle);
			return NULL;
		}
	}

	return (void *)dev_handle;
}


/*
 * iusb_close
 *
 * Release control of the PLC and shutdown the connection to it.
 */
void iusb_close(iusb_t *iplc)
{
	struct usb_dev_handle *dh = (struct usb_dev_handle *)iplc->vfd;

	usb_release_interface(dh, 0);
	usb_close(dh);
}

/*
 * Low level USB/PLC commands
 *
 * This section has the functions that work at a fairly low level when
 * controlling the USB plc. These functions should probably only be
 * called by the higher level functions and not by the top level
 * application.
 *
 * The functions are:
 *
 * iusb_read       -- Read packets from the USB interface and buffer.
 * iusb_write      -- Write a data packet to the USB interface
 * iusb_write_echo -- Write a data packet to the USB interface
 * iusb_cts        -- Wait for a clear-to-send from the USB interface.
 *
 */


/*
 * iusb_read
 *
 * This reads packets from the USB interface and returns 1 byte at
 * a time to the caller.
 */

#define URB_TIMEOUT 60 /* This is near the minimum, might have to increase it */
int iusb_read(iusb_t *iplc)
{
	struct usb_dev_handle *dh = (struct usb_dev_handle *)iplc->vfd;
	unsigned char packet_buf[8];
	int cnt = 0;
	int ret;
	int i;
	int timeout = 0;
	unsigned char rb;

	if (iplc->ring_head == RING_SIZE) {
		iplc->ring_head = 0;
	}

	if (iplc->ring_head == iplc->ring_tail) {  /* ring buffer empty */
		do {
			memset(packet_buf, 0, 8);
			/* ret = read(iplc->fd, packet_buf, 8); */
			ret = usb_interrupt_read(dh, 1, (char *)packet_buf, 8, 500);
			if (ret == 8) {
				cnt = packet_buf[0] & 0x0f;
				for (i = 0; i < cnt; i++) {
					iplc->ring_buffer[iplc->ring_tail++] = packet_buf[i+1];
					if (iplc->ring_tail == RING_SIZE) {
						iplc->ring_tail = 0;
					}
				}
				if (packet_buf[0] & 0x70) {
					printf("got something in high nybble 0x%02x\n",
							packet_buf[0]);
				}
			} else if (ret > 0) {
				printf("Got %d bytes instead of 8!\n", ret);
			}
			timeout++;
			usleep(100);
		} while ((cnt < 1) && (timeout < URB_TIMEOUT));
	}

	if (iplc->ring_head != iplc->ring_tail) {
		/* When poping a byte from the ring buffer, clear it */
		rb = iplc->ring_buffer[iplc->ring_head];
		iplc->ring_buffer[iplc->ring_head] = 0x00;
		iplc->ring_head++;
		return rb;
	} else {
		return -110;
	}
}

/*
 * iusb_cts
 *
 * Wait for the PCL to give us a CTS signal.  We could end up locking
 * up in this function.
 */
int iusb_cts(iusb_t *iplc, int cts)
{
	struct usb_dev_handle *dh = (struct usb_dev_handle *)iplc->vfd;
	unsigned char usbp[8];
	int i;
	int ret;
	int timeout = 0;

	do {
		memset(usbp, 0, 8);
		ret = usb_interrupt_read(dh, 1, (char *)usbp, 8, 500);
		if (ret == 8) {
			if (usbp[0] & 0x0f) { /* need to add to ring buffer */
				for (i = 0; i < (usbp[0] & 0x0f); i++) {
					iplc->ring_buffer[iplc->ring_tail++] = usbp[i+1];
					if (iplc->ring_tail == RING_SIZE) {
						iplc->ring_tail = 0;
					}
				}
			}
			if (usbp[0] & 0x70) {
				printf("CTS: got something in high nybble 0x%02x\n", usbp[0]);
			}
		} else if (ret > 0) {
			printf("Got %d bytes instead of 8!\n", ret);
		}
		timeout++;
		usleep(120);
	} while ((!(usbp[0] & 0x80)) && (timeout < 5500) && !cts);

	if (timeout >= 5500) {
		printf("Timeout in usb_cts (%d)\n", cts);
	}
	return 0;
}


/*
 * iusb_write_echo
 *
 * Send a two byte packet and wait for the ring buffer tail to reach rt.
 */
int iusb_write_echo(iusb_t *iplc, unsigned char *pkt, int len)
{
	struct usb_dev_handle *dh = (struct usb_dev_handle *)iplc->vfd;
	int ret;
	int rs;
	int to; /* number of loops before timing out */

	to = 35;
	rs = iplc->ring_tail;
	ret = usb_interrupt_write(dh, 1, (char *)pkt, 8, 10);
	iusb_cts(iplc, 0);

	/* Got a clear to send, but did the bytes get echoed back? */
	while (to) {
		if (rb_avail(iplc, rs, len) < 2) {
			iusb_cts(iplc, 1);
			to--;
		} else {
			print_ring(iplc);
			while (rb_avail(iplc, rs, len) >= 2) {
				if (echo_match(iplc, rs, &pkt[1], len)) {
					return ret;
				}
				if (rs == iplc->ring_head) {
					iplc->ring_buffer[iplc->ring_head++] = 0x00;
				}
				if (++rs == RING_SIZE) {
					rs = 0;
				}
			}
		}
	}

	printf("Timeout trying to get echo back from PLC\n");
	return ret;
}

/*
 * iusb_write
 *
 * Send a packet
 */
int iusb_write(iusb_t *iplc, unsigned char *pkt, int len)
{
	struct usb_dev_handle *dh = (struct usb_dev_handle *)iplc->vfd;
	int ret;

	ret = usb_interrupt_write(dh, 1, (char *)pkt, 8, 10);

	return ret;
}

