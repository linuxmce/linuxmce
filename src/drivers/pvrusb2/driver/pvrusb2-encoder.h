/*
 *
 *  $Id: pvrusb2-encoder.h 1736 2007-11-17 05:29:19Z isely $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *  Copyright (C) 2004 Aurelien Alleaume <slts@free.fr>
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

#ifndef __PVRUSB2_ENCODER_H
#define __PVRUSB2_ENCODER_H

struct pvr2_hdw;

#ifdef PVR2_ENABLE_CX2341XMOD
int pvr2_encoder_adjust(struct pvr2_hdw *);
#endif
int pvr2_encoder_configure(struct pvr2_hdw *);
int pvr2_encoder_start(struct pvr2_hdw *);
int pvr2_encoder_stop(struct pvr2_hdw *);

#endif /* __PVRUSB2_ENCODER_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
