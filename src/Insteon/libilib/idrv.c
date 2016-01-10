/*
 * $RCSfile: idrv.c,v $  $Revision: 1.4 $  $Name:  $
 * $Id: idrv.c,v 1.4 2007/09/09 18:08:33 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/09/09 18:08:33 $
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
 *  This is the low level code that impliments a communication interface
 *  to the Insteon PLC.  It supports multiple physical interfaces via
 *  a interface dependent dispatch table. The specific physical interface
 *  to use will be determined at initialization time.
 *
 *  This file has all the interface independent routines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include "idrv.h"

#define DEBUG 0

#if DEBUG
#define DBGMSG printf
#else
#define DBGMSG null_printf
#endif

int debug = 0;
char *version_str = VERSION;

/* DBGMSG is defined as null_printf when debugging is off */
static __inline void null_printf(const char *format, ...)
{
	return;
}

__inline void debug_printf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	fprintf(stderr, format, ap);
	va_end(ap);
}

int plc_read_response(iusb_t *iplc, unsigned char *buf, int size,
		unsigned char match);
int rb_avail(iusb_t *iplc, int index, int len);
void print_ring(iusb_t *iplc);
int echo_match (iusb_t *iplc, int st, unsigned char *pkt, int len);


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
 * chksum            -- Calculate a checksum on a block of data
 * send_cmd          -- Send an iBIOS command to the PCL
 * plc_read_response -- Get responses from the PLC
 *
 */

/*
 * chksum
 *
 * Calculate the checksum value for a data block. This is a 2's complement
 * of the sum of the data, limited to 16 bits.
 */
int chksum(unsigned char *data, int size)
{
	int total = 0;
	int i;

	for (i = 0; i < size; i++) {
		total += data[i];
	}

	total &= 0xffff;
	total = (~total + 1) & 0xffff;

	return total;
}

/*
 * send_cmd
 *
 * Command is one of the basic PowerLinc's commands
 *   0x40 - download
 *   0x42 - upload
 *   0x44 - get checksum
 *   0x46 - mask
 *   0x47 - simulated event
 *   0x48 - get version
 */
int send_cmd(iusb_t *iplc,
		unsigned char cmd,
		unsigned char *data,
		int len)
{
	int dptr = 0;
	int ret;
	int rt;
	unsigned char *packet;

	/* set up packet buffer */
	packet = (unsigned char *)malloc(9);
	if (packet == NULL) {
		printf("Memory allocation failure!\n");
		return -1;
	}
	memset(packet, 0, 8);

	/*
	 * For USB we send 8 byte packets, however, we may only be able to
	 * use the first 3 bytes.
	 *
	 * To send:
	 *   Send STX and command byte, wait for CTS & 2 bytes returned.
	 *   Send header bytes (2 at a time), wait for CTS & each 2 bytes returned.
	 *   Send data bytes (2 at a time).
	 *
	 *   Commands and header size
	 *   0x40  8 bytes
	 *   0x42  6 bytes
	 *   0x44  6 bytes
	 *   0x46  6? bytes
	 *   0x47  4 bytes
	 *   0x48  2 bytes
	 */

	/* read and throw away any data coming from the PLC */
	/*
	while ((ret = read(iplc->fd, packet, 8)) > 0);
	ring_head = ring_tail;
	*/
	rt = iplc->ring_tail;

	/* Construct the first packet */
	packet[0] = 0x02;  /* bytes used in packet */
	packet[1] = 0x02;  /* start of text */
	packet[2] = cmd;   /* command */

	rt += 2;
	if (rt > RING_SIZE) {
		rt -= RING_SIZE;
	}

	ret = (iplc->send_pkt)(iplc, packet, 2);
	if (ret == -1) {
		free(packet);
		return -1;
	}

	dptr = 0;
	if (cmd != 0x48) {
		packet[0] = 0x02;
		packet[1] = data[dptr++];
		packet[2] = data[dptr++];

		rt += 2;
		if (rt > RING_SIZE) {
			rt -= RING_SIZE;
		}
		ret = (iplc->send_pkt)(iplc, packet, 2);
		if (ret == -1) {
			free(packet);
			return -1;
		}

		if (cmd != 0x47) {
			packet[0] = 0x02;
			packet[1] = data[dptr++];
			packet[2] = data[dptr++];

			rt += 2;
			if (rt > RING_SIZE) {
				rt -= RING_SIZE;
			}
			ret = (iplc->send_pkt)(iplc, packet, 2);
			if (ret == -1) {
				free(packet);
				return -1;
			}

#if 0
			/* Apparently the checksum bytes are NOT part of the header */
			if (cmd == 0x40) {
				packet[0] = 0x02;
				packet[1] = data[dptr++];
				packet[2] = data[dptr++];
				printf("Command is 40 (upload) checksum = 0x%02x 0x%02x\n",
						packet[1], packet[2]);
				rt += 2;
				if (rt > RING_SIZE) {
					rt -= RING_SIZE;
				}
				ret = usb_send_pkt(iplc, packet, rt);
			}
#else
			if (cmd == 0x40) {
				packet[0] = 0x02;
				packet[1] = data[dptr++];
				packet[2] = data[dptr++];
				/*
				printf("Command is 40 (upload) checksum = 0x%02x 0x%02x\n",
						packet[1], packet[2]);
				*/
				ret = (iplc->send)(iplc, packet, 2);
				usleep(6000);
				(iplc->cts)(iplc, 0);
			}
#endif
		}
	}

	/* Send 1 byte at a time */
	while (dptr < len) {
		memset(packet, 0x00, 8);
		packet[0] = 0x01;
		packet[1] = data[dptr++];
		ret = (iplc->send)(iplc, packet, 1);
		usleep(6000);
		(iplc->cts)(iplc, 0);
	}

	free(packet);
	return 0;
}


void print_msg_buf(unsigned char *buf, int len)
{
	int i;

	if (debug) {
		printf("MSG: ");
		for (i = 0; i < len; i++) {
			printf("0x%02x ", buf[i]);
		}
		printf("\n");
	}
}


/*
 * plc_read_response
 *
 * Gets command responses from the PLC.
 *
 * This function needs to take a buffer (and it's size) as arguments and
 * should return the number of bytes being returned.
 *
 * Return -1 if timeout?
 *
 * NOTE that we have to read all the data from the PLC even if the passed
 * in buffer isn't large enough to hold it all. Any extra will have to get
 * thrown away.
 */
int plc_read_response(iusb_t *iplc, unsigned char *buf,
		int size, unsigned char match)
{
	int i;
	int len = 0;
	int data_len;
	int c;
	int state;
	int extended = 0;
	int som = 0;
	unsigned char cmd;
	unsigned char response[256];
	int d;

	/*
	 * Looks like we need to parse the response in order to know how
	 * much data we expect to receive here.  Response will look something
	 * like this:
	 *   0x02  (Start of Text)
	 *   0x4x  (command)
	 *
	 *   The rest is dependent on the command.  This is going to have to
	 *   be a nice little state machine!
	 */

	DBGMSG("ring head = %d, ring tail = %d [cmd == 0x%02x]\n", iplc->ring_head,
			iplc->ring_tail, match);
	state = 0;
	while (state != 4) {
		if (state == 0) { /* looking for STX (0x02) */
			do {
				if ((c = (iplc->read_byte)(iplc)) == -110) {
					return -1;
				}
				DBGMSG("... [0x%02x]", c);
			} while (c != 0x02);
			state = 1;
		} else if (state == 1) { /* looking for command */
			if ((c = (iplc->read_byte)(iplc)) == -110) {
				return -2;
			}
			DBGMSG("CMD[0x%02x]", c);
			cmd = (unsigned char)c;
			if (len < size) {
				buf[len++] = cmd;
			}
			if (cmd != 0x02) {
				state = 2;
			}
		} else if (state == 2) { /* looking for command data */
			switch (cmd) {
				case 0x40: /* download (data was sent to PLC) */
					/* Get start address and length */
					for (i = 0; i < 4; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len] = (unsigned char)c;
						}
						response[len++] = (unsigned char)c; /* for debug */
					}

					/* debug info */
					data_len = (response[3] << 8) | response[4];
					/*
					printf("Downloaded %d bytes starting at 0x%02x%02x\n",
							data_len, response[1], response[2]);
					*/
					state = 3;
					break;
				case 0x41: /* Fixed length message (should be less than 256) */
					if ((c = (iplc->read_byte)(iplc)) == -110) {
						return -1 ;
					}
					data_len = (unsigned char)c;
					DBGMSG("[0x%02x]", c);

					/* Read actual data */
					for (i = 0; i < data_len; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					state = 3;
					break;
				case 0x42: /* upload (reading data from PLC)*/
					/* Get start address and length */
					for (i = 0; i < 4; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -3;
						}
						DBGMSG("[0x%02x]", c);
						response[len] = (unsigned char)c;
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}

					data_len = (response[3] << 8) | response[4];

					/*
					printf("Uploading %d bytes starting at 0x%02x%02x\n",
							data_len, response[1], response[2]);
					*/

					/* Read actual data */
					for (i = 0; i < data_len; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -4;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}

					/* checksum bytes next */
					for (i = 0; i < 2; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -5;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					state = 3;
					break;
				case 0x43: /* Variable len message */
					state = 3;
					break;
				case 0x44: /* Get Checksum */
					/* Get start address, length, and checksum */
					for (i = 0; i < 6; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					state = 3;
					break;
				case 0x45: /* Event report */
					if ((c = (iplc->read_byte)(iplc)) == -110) {
						return -1;
					}

					DBGMSG("[0x%02x]", c);
					if (len < size) {
						buf[len++] = (unsigned char)c;
					}
					/*
					 * The documentation doesn't show this ending with
					 * a ACK. So for now, lets just exit our state engine.
					 */
					/* state = 3; */
					DBGMSG("\n");
					if (cmd != match) {
						state = 0;
						len = 0;
					} else {
						print_msg_buf(buf, len);
						return len;
					}
					break;
				case 0x46: /* Mask */
					/* Get address, OR mask, and AND mask */
					for (i = 0; i < 4; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					state = 3;
					break;
				case 0x47: /* Simulated Event */
					/* Get event handle and timer value */
					for (i = 0; i < 2; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					DBGMSG("\n");
					if (cmd != match) {
						state = 0;
						len = 0;
					} else {
						print_msg_buf(buf, len);
						return len;
					}
					state = 3;
					break;
				case 0x48: /* Firmware Version  (6 bytes) */
					for (i = 0; i < 6; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					state = 3;
					break;
				case 0x49: /* Debug report */
					state = 3;
					break;
				case 0x4a: /* X10 byte received */
					/* X10 address/command */
					for (i = 0; i < 2; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}
					}
					state = 3;
					break;
				case 0x4f: /* INSTEON Message */
					/*
					 * Need to start pushing these onto a queue of
					 * some sort so that we don't lose them.
					 */
					/* First byte is the event number */
					if ((c = (iplc->read_byte)(iplc)) == -110) {
						return -1;
					}

					DBGMSG("[0x%02x]", c);
					if (len < size) {
						buf[len++] = (unsigned char)c;
					}

					/* If event is EVINT_ITX_NACK (0x05) then len = 0 */
					if (c == 0x05) {
						print_msg_buf(buf, len);
						return len;
					}

					/* Message is either 9 or 23 bytes. So read 9 bytes */
					som = len;
					for (i = 0; i < 9; i++) {
						if ((c = (iplc->read_byte)(iplc)) == -110) {
							return -1;
						}
						DBGMSG("[0x%02x]", c);
						if (len < size) {
							buf[len++] = (unsigned char)c;
						}

						/* Check for extended message */
						if ((i == 6) && (c & 0x10)) {
							if ((c & 0xf0) != 0xf0) {
								extended = 1;
							} else {
								printf("NGCD: ");
								for (d = 0; d < len; d++) {
									printf("0x%02x ", buf[d]);
								}
								printf("\n");

								/*
								printf("Bogus message (Extened NAK of group "
										"cleanup direct message?\n");
								*/
							}
						}
					}

					/* if message byte 7 has bit 4 set, read 14 more bytes */
					if (extended) {
						printf("EXT : ");
						for (d = 0; d < len; d++) {
							printf("0x%02x ", buf[d]);
						}
						printf("\n");
						/*
						printf("Extended message? Flag = 0x%02x\n", buf[som+6]);
						*/
#if 0
						for (i = 0; i < 14; i++) {
							if ((c = (iplc->read_byte)(iplc)) == -110) {
								return -1;
							}
							DBGMSG("[0x%02x]", c);
							if (len < size) {
								buf[len++] = (unsigned char)c;
							}
						}
#endif
					}
					DBGMSG("\n");
					if (cmd != match) {
						len = 0;
						state = 0;
					} else {
						print_msg_buf(buf, len);
						return len;
					}
					break;
				default:
					printf("unknown command byte 0x%x\n", cmd);
					state = 3;
					break;
			}
		} else if (state == 3) { /* Look for ACK */
			if ((c = (iplc->read_byte)(iplc)) == -110) {
				DBGMSG(" TIMEOUT\n");
				return -6;
			}
			DBGMSG(" ACK[0x%02x]\n", c);
			if (len < size) {
				buf[len++] = (unsigned char)c;
			}
			if ((c == 0x06) || (c == 0x15)) { /* ACK or NAK */
				if (cmd == match) {
					state = 4;
				} else {
					state = 0;
					len = 0;
				}
			} else if (c == 0x02) { /* start of new command? */
				state = 1;
			}
		}
	}

	print_msg_buf(buf, len);
	return len;
}


/*
 * rb_avail
 *
 * Check to see if some number of bytes are available in the ring buffer.
 */
int rb_avail(iusb_t *iplc, int index, int len)
{
	int rb_size;

	if (index <= iplc->ring_tail) {
		rb_size = iplc->ring_tail - index;
	} else {
		rb_size = RING_SIZE - index + iplc->ring_tail;
	}

	return rb_size;
}



/*
 * echo_match
 *
 * See if the bytes in the packet match bytes in the ring buffer. Takes
 * into account wrapping in the ring buffer.
 */
int echo_match (iusb_t *iplc, int st, unsigned char *pkt, int len)
{
	int i;
	int match = 0;

	for (i = 0; i < len; i++) {
		if (pkt[i] == iplc->ring_buffer[st]) {
			match++;
		}
		st++;
		if (st == RING_SIZE) {
			st = 0;
		}
		if (st == iplc->ring_tail) {
			break;
		}
	}

	return (match == len) ? 1 : 0;
}

/*
 * print_ring
 *
 * Print out the ring buffer contents.
 */
void print_ring(iusb_t *iplc)
{
	int i;

	DBGMSG("head = %d, tail = %d\n", iplc->ring_head, iplc->ring_tail);
	DBGMSG("RB: ");
	if (iplc->ring_head <= iplc->ring_tail) {
		for (i = iplc->ring_head; i < iplc->ring_tail; i++) {
			DBGMSG("0x%02x ", iplc->ring_buffer[i]);
		}
	} else {
		for (i = iplc->ring_head; i < RING_SIZE; i++) {
			DBGMSG("0x%02x ", iplc->ring_buffer[i]);
		}
		for (i = 0; i < iplc->ring_tail; i++) {
			DBGMSG("0x%02x ", iplc->ring_buffer[i]);
		}
	}
	DBGMSG("\n");
}

