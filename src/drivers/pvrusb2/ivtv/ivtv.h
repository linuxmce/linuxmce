/*
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __IVTV_H
#define __IVTV_H

/* MCI 13-Mar-2005 - This header is present purely for the needs of
   interfacing with saa7115.c, a module pulled unchanged from ivtv.
   Hopefully at some point saa7115.c will find its way into the main
   Linux kernel source tree and then this won't be needed. */

#define IVTV_SLICED_TYPE_TELETEXT_B     (1)
#define IVTV_SLICED_TYPE_CAPTION_525    (4)
#define IVTV_SLICED_TYPE_WSS_625        (5)
#define IVTV_SLICED_TYPE_VPS            (7)

#define VBI_TYPE_TELETEXT 	0x1
#define VBI_TYPE_CC 		0x4
#define VBI_TYPE_WSS 		0x5
#define VBI_TYPE_VPS 		0x7

/* V4L2 stuff needed by saa7115.c that appears not to be in the
   mainline kernel yet. */
#define V4L2_SLICED_TELETEXT_B          (0x0001)
#define V4L2_SLICED_VPS                 (0x0400)
#define V4L2_SLICED_CAPTION_525         (0x1000)
#define V4L2_SLICED_WSS_625             (0x4000)

#define V4L2_SLICED_VBI_525             (V4L2_SLICED_CAPTION_525)
#define V4L2_SLICED_VBI_625             (V4L2_SLICED_TELETEXT_B | V4L2_SLICED_VPS | V4L2_SLICED_WSS_625)


#endif
