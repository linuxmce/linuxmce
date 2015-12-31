/*
 *
 *  $Id: pvrusb2-compat.c 1563 2007-02-28 03:30:33Z isely $
 *
 *  Copyright (C) 2007 Mike Isely <isely@pobox.com>
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

#include "pvrusb2-options.h"
#include "compat.h"
#include <linux/slab.h>

#ifdef PVR2_EMULATE_KZALLOC
void *kzalloc(unsigned int sz,unsigned int tp)
{
	void *ptr;
	ptr = kmalloc(sz,tp);
	if (ptr) memset(ptr,0,sz);
	return ptr;
}

#endif

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
