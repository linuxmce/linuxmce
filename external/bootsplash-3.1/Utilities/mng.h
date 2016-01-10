/*
 *	fbmngplay - fb console MNG player.
 *	(c) 2001-2002 by Stefan Reinauer, <stepan@suse.de>
 *   
 *	This program is based on mngplay, part of libmng, written and (C) by
 *	Ralph Giles <giles@ashlu.bc.ca>
 *
 *	This program my be redistributed under the terms of the
 *	GNU General Public Licence, version 2, or at your preference,
 *	any later version.
 */

#ifndef __HOOKS_H
#define __HOOKS_H

#include "fbmngplay.h"

extern mngstuff *mng;

mng_ptr mngalloc(mng_uint32 size);
void mngfree(mng_ptr p, mng_uint32 size);
mng_bool mngopenstream(mng_handle mng);
mng_bool mngclosestream(mng_handle mng);
mng_bool mngreadstream(mng_handle mng, mng_ptr buffer, mng_uint32 size,
		       mng_uint32 * bytesread);
mng_bool mngprocessheader(mng_handle mng, mng_uint32 width,
			  mng_uint32 height);
mng_ptr mnggetcanvasline(mng_handle mng, mng_uint32 line);
mng_uint32 mnggetticks(mng_handle mng);
mng_bool mngrefresh(mng_handle mng, mng_uint32 x, mng_uint32 y,
		    mng_uint32 w, mng_uint32 h);
mng_bool mngsettimer(mng_handle mng, mng_uint32 msecs);
mng_bool mngerror(mng_handle mng, mng_int32 code, mng_int8 severity,
		  mng_chunkid chunktype, mng_uint32 chunkseq,
		  mng_int32 extra1, mng_int32 extra2, mng_pchar text);
int mngquit(mng_handle mng);
void cleanup(void);
void restore_area(void);

#endif
