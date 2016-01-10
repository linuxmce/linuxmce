/*
 * $RCSfile: ihid.h,v $  $Revision: 1.3 $  $Name:  $
 * $Id: ihid.h,v 1.3 2007/04/21 03:23:27 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/21 03:23:27 $
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
#ifndef _IHID_H
#define _IHID_H

#include "idrv.h"

extern void *ihid_init(char *dev);
extern void ihid_close(iusb_t *iplc);
extern int ihid_read(iusb_t *iplc);
extern int ihid_write(iusb_t *iplc, unsigned char *pkt, int len);
extern int ihid_write_echo(iusb_t *iplc, unsigned char *pkt, int len);
extern int ihid_cts(iusb_t *iplc, int cts);

#endif
