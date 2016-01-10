/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "SDL_CompositeAlphaChannel.h"
#include "SDL_pixel.h"
#include "SDL_Defs.h"

void CompositeAlphaChannel(SDL_Surface *pSrc, SDL_Surface *pDest, SDL_Rect * pPos)
{
	int iSrcWidth = pSrc->w;
	int iSrcHeight = pSrc->h;

	for (int j = 0; j < iSrcHeight; j++)
	{
		for (int i = 0; i < iSrcWidth; i++)
		{
			int iParentX = pPos->x + i;
			int iParentY = pPos->y + j;
			Uint32 iPixel_Parent = getpixel(pDest, iParentX, iParentY);
			Uint32 iPixel_Child = getpixel(pSrc, i, j);
			Uint8 iAlpha_Parent = iPixel_Parent >> ashift & 0xff;
			Uint8 iAlpha_Child = iPixel_Child >> ashift & 0xff;

			// in SDL, the alpha value represents the opacity, not transparency (255 = opaque, 0 = transparent)
			// the most opaque one establishes resulting opacity
			//Uint8 iAlpha_Result = iAlpha_Parent > iAlpha_Child ? iAlpha_Parent : iAlpha_Child; // Formula 1: not correct
			//Uint8 iAlpha_Result = 255 - (Uint8) ((1 - iAlpha_Child / 255.0) * (255 - iAlpha_Parent)); // Formula 2: not fast
			// Formula 3: Formula 2 on steroids
			Uint32 Result = iAlpha_Child + iAlpha_Parent - iAlpha_Child * iAlpha_Parent / 256;
			Uint8 iAlpha_Result = Result >= 256 ? 255 : Result;

			Uint32 iPixel_Result = iPixel_Parent & ~amask | (iAlpha_Result << ashift);

			//printf("Pixel: P:%x(%x); C:%x(%x), R:%x(%x)\n", iPixel_Parent, iAlpha_Parent, iPixel_Child, iAlpha_Child, iPixel_Result, iAlpha_Result);
			putpixel(pDest, iParentX, iParentY, iPixel_Result);
		}
	}
}
