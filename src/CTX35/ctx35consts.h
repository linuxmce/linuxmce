/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file cm11aconsts.h
Handle the X10 CTX35 constant definitions.
 */

#ifndef __CTX35CONSTS_H_
#define __CTX35CONSTS_H_

// genral consts
/** Define the ack byte */
#define CTX35_ACK				0x00

/** The interface ready response */
#define CTX35_INTERFACE_READY	0x55

/** The computer ready code */
#define CTX35_COMPUTER_READY	0xC3

/** Something about the interface */
#define CTX35_INTERFACE_CQ		0x5A

/** Request to send the clock information */
#define CTX35_CLOCK_REQ			0xA5

/** Packet is a a standard address packet */
#define CTX35_STANDARD_ADDRESS	0x04

/** Packet is an extended function */
#define CTX35_STANDARD_FUNCTION	0x06

// function consts (foo values, not tested yet)
#define CTX35_FUNC_ALL_U_OFF	0x00
#define CTX35_FUNC_ALL_L_ON 	0x01
#define CTX35_FUNC_ON			0x02
#define CTX35_FUNC_OFF			0x03
#define CTX35_FUNC_DIM			0x04
#define CTX35_FUNC_BRIGHT		0x05
#define CTX35_FUNC_ALL_L_OFF	0x06
#define CTX35_FUNC_EXT			0x07
#define CTX35_FUNC_HREQ			0x08
#define CTX35_FUNC_HACK			0x09
#define CTX35_FUNC_PSD1			0x0A
#define CTX35_FUNC_PSD2			0x0B
#define CTX35_FUNC_EXTDT		0x0C
#define CTX35_FUNC_STATON		0x0D
#define CTX35_FUNC_STATOFF		0x0E
#define CTX35_FUNC_STATREQ		0x0F

/** Used to set tine time on the CTX35 */
#define CTX35_SET_TIME        	0x9B

#endif
