/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef COLORSPACE_UTILS_H
#define COLORSPACE_UTILS_H

#include <inttypes.h>
#include <malloc.h>

class Colorspace_Utils
{
public:
	static void yuy2toyv12 ( uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height );
	static uint8_t * yv12torgb ( uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height );
};

#endif
