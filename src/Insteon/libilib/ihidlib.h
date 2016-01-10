/*
 * $RCSfile: ihidlib.h,v $  $Revision: 1.1 $  $Name:  $
 * $Id: ihidlib.h,v 1.1 2007/04/21 03:23:28 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/21 03:23:28 $
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
 *  Function prototyes
 *
 * ----------------------------------------------------------------------------
 */
#ifndef _IHIDLIB_H
#define _IHIDLIB_H

#include "idrv.h"

#ifdef INCLUDE_LIBHID
extern void *ihidlib_init(char *dev);
extern void ihidlib_close(iusb_t *iplc);
extern int ihidlib_read(iusb_t *iplc);
extern int ihidlib_write(iusb_t *iplc, unsigned char *pkt, int len);
extern int ihidlib_write_echo(iusb_t *iplc, unsigned char *pkt, int len);
extern int ihidlib_cts(iusb_t *iplc, int cts);
#endif

#endif
