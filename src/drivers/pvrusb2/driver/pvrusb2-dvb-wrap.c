/*
 *  $Id: pvrusb2-dvb-wrap.c 1954 2008-04-20 02:43:07Z isely $
 *
 *
 *  Copyright (C) 2008 Mike Isely <isely@pobox.com>
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


/*

This is a silly wrapper around pvrusb2-dvb.c so that we can skip its
compilation is DVB is not enabled.  Due to the vagaries of the kbuild
build system, we must always include it in a standlone driver build,
so the decision about actually compiling the included code has to
happen *within* the source code.

 */

#include "pvrusb2-options.h"
#ifdef CONFIG_VIDEO_PVRUSB2_DVB
#include "pvrusb2-dvb.c"
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
