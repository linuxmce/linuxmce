/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef SDL_PIXEL_H
#define SDL_PIXEL_H

#include <SDL.h>

Uint8 * getpixelpointer(SDL_Surface *pSurface, int x, int y);
Uint32 getpixel(SDL_Surface *pSurface, int x, int y);
void putpixel(SDL_Surface *pSurface, int x, int y, Uint32 pixel_color);
#endif /*SDL_PIXEL_H*/
