/*
 * Wilcard T400P FXS Interface Driver for Zapata Telephony interface
 *
 * Written by Mark Spencer <markster@linux-support.net>
 *
 * Copyright (C) 2001, Linux Support Services, Inc.
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

#include <linux/ioctl.h>

#define FRMR_TTR_BASE 0x10
#define FRMR_RTR_BASE 0x0c
#define FRMR_TSEO 0xa0
#define FRMR_TSBS1 0xa1
#define FRMR_CCR1 0x09
#define FRMR_CCR1_ITF 0x08
#define FRMR_CCR1_EITS 0x10
#define FRMR_CCR2 0x0a
#define FRMR_CCR2_RCRC 0x04
#define FRMR_CCR2_RADD 0x10
#define FRMR_MODE 0x03
#define FRMR_MODE_NO_ADDR_CMP 0x80
#define FRMR_MODE_SS7 0x20
#define FRMR_MODE_HRAC 0x08
#define FRMR_IMR0 0x14
#define FRMR_IMR0_RME 0x80
#define FRMR_IMR0_RPF 0x01
#define FRMR_IMR1 0x15
#define FRMR_IMR1_ALLS 0x20
#define FRMR_IMR1_XDU 0x10
#define FRMR_IMR1_XPR 0x01
#define FRMR_XC0 0x22
#define FRMR_XC1 0x23
#define FRMR_RC0 0x24
#define FRMR_RC1 0x25
#define FRMR_SIC1 0x3e
#define FRMR_SIC2 0x3f
#define FRMR_SIC3 0x40
#define FRMR_CMR1 0x44
#define FRMR_CMR2 0x45
#define FRMR_GCR 0x46
#define FRMR_ISR0 0x68
#define FRMR_ISR0_RME 0x80
#define FRMR_ISR0_RPF 0x01
#define FRMR_ISR1 0x69
#define FRMR_ISR1_ALLS 0x20
#define FRMR_ISR1_XDU 0x10
#define FRMR_ISR1_XPR 0x01
#define FRMR_ISR2 0x6a
#define FRMR_ISR3 0x6b
#define FRMR_ISR4 0x6c
#define FRMR_GIS  0x6e
#define FRMR_GIS_ISR0 0x01
#define FRMR_GIS_ISR1 0x02
#define FRMR_GIS_ISR2 0x04
#define FRMR_GIS_ISR3 0x08
#define FRMR_GIS_ISR4 0x10
#define FRMR_CIS 0x6f
#define FRMR_CIS_GIS1 0x01
#define FRMR_CIS_GIS2 0x02
#define FRMR_CIS_GIS3 0x04
#define FRMR_CIS_GIS4 0x08
#define FRMR_CMDR 0x02
#define FRMR_CMDR_SRES 0x01
#define FRMR_CMDR_XRES 0x10
#define FRMR_CMDR_RMC 0x80
#define FRMR_CMDR_XTF 0x04
#define FRMR_CMDR_XHF 0x08
#define FRMR_CMDR_XME 0x02
#define FRMR_RSIS 0x65
#define FRMR_RSIS_VFR 0x80
#define FRMR_RSIS_RDO 0x40
#define FRMR_RSIS_CRC16 0x20
#define FRMR_RSIS_RAB 0x10
#define FRMR_RBCL 0x66
#define FRMR_RBCL_MAX_SIZE 0x1f
#define FRMR_RBCH 0x67
#define FRMR_RXFIFO 0x00
#define FRMR_SIS 0x64
#define FRMR_SIS_XFW 0x40
#define FRMR_TXFIFO 0x00

#define NUM_REGS 0xa9
#define NUM_PCI 12

struct t4_regs {
	unsigned int pci[NUM_PCI];
	unsigned char regs[NUM_REGS];
};

#define WCT4_GET_REGS	_IOW (ZT_CODE, 60, struct t4_regs)

