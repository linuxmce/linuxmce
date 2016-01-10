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
Handle the X10 CM11A constant definitions.
 */

#ifndef __CM11ACONSTS_H_
#define __CM11ACONSTS_H_

// genral consts
/** Define the ack byte */
#define CM11A_ACK				0x00

/** The interface ready response */
#define CM11A_INTERFACE_READY	0x55

/** The computer ready code */
#define CM11A_COMPUTER_READY	0xC3

/** Something about the interface */
#define CM11A_INTERFACE_CQ		0x5A

/** Request to send the clock information */
#define CM11A_CLOCK_REQ			0xA5

/** Packet is a a standard address packet */
#define CM11A_STANDARD_ADDRESS	0x04

/** Packet is an extended function */
#define CM11A_STANDARD_FUNCTION	0x06

// function consts (foo values, not tested yet)
#define CM11A_FUNC_ALL_U_OFF	0x00
#define CM11A_FUNC_ALL_L_ON 	0x01
#define CM11A_FUNC_ON			0x02
#define CM11A_FUNC_OFF			0x03
#define CM11A_FUNC_DIM			0x04
#define CM11A_FUNC_BRIGHT		0x05
#define CM11A_FUNC_ALL_L_OFF	0x06
#define CM11A_FUNC_EXT			0x07
#define CM11A_FUNC_HREQ			0x08
#define CM11A_FUNC_HACK			0x09
#define CM11A_FUNC_PSD1			0x0A
#define CM11A_FUNC_PSD2			0x0B
#define CM11A_FUNC_EXTDT		0x0C
#define CM11A_FUNC_STATON		0x0D
#define CM11A_FUNC_STATOFF		0x0E
#define CM11A_FUNC_STATREQ		0x0F

/** Used to set tine time on the CM11A */
#define CM11A_SET_TIME        	0x9B

#endif
