/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "SDL_pixel.h"

Uint8 * getpixelpointer(SDL_Surface *pSurface, int x, int y)
{
	int bpp = pSurface->format->BytesPerPixel;

	Uint8 * pixel = (Uint8 *) pSurface->pixels + y * pSurface->pitch + x * bpp;
	return pixel;
}

Uint32 getpixel(SDL_Surface *pSurface, int x, int y)
{
	// all pixels outside the pSurface are black and transparent
	if (x < 0 || x >= pSurface->w || y < 0 || y >= pSurface->h)
		return SDL_MapRGBA(pSurface->format, 0, 0, 0, 0);

	Uint8 * pixel = getpixelpointer(pSurface, x, y);

	int bpp = pSurface->format->BytesPerPixel;
	switch(bpp)
	{
	case 1:
		return * pixel;

	case 2:
		return * (Uint16 *) pixel;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
		else
			return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

	case 4:
		return * (Uint32 *) pixel;

	default:
		return 0;	   /* shouldn't happen, but avoids warnings */
	}
}

void putpixel(SDL_Surface *pSurface, int x, int y, Uint32 pixel_color)
{
	// don't try to put a pixel outside the pSurface
	if (x < 0 || x >= pSurface->w || y < 0 || y >= pSurface->h)
		return;

	Uint8 * pixel = getpixelpointer(pSurface, x, y);

	int bpp = pSurface->format->BytesPerPixel;
	switch(bpp)
	{
	case 1:
		* pixel = pixel_color;
		break;

	case 2:
		* (Uint16 *) pixel = pixel_color;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			pixel[0] = (pixel_color >> 16) & 0xff;
			pixel[1] = (pixel_color >> 8) & 0xff;
			pixel[2] = pixel_color & 0xff;
		}
		else
		{
			pixel[0] = pixel_color & 0xff;
			pixel[1] = (pixel_color >> 8) & 0xff;
			pixel[2] = (pixel_color >> 16) & 0xff;
		}
		break;

	case 4:
		* (Uint32 *) pixel = pixel_color;
		break;
	}
}
