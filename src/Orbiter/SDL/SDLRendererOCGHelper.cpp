/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "SDLRendererOCGHelper.h"
#include "../RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/FileUtils.h"
#include "../SDL_Helpers/SDL_Defs.h"
#include "../SDL_Helpers/SDL_pixel.h"
#include "../SerializeClass/ShapesColors.h"
#include "../DCE/Logger.h"
using namespace DCE;

#include <SDL.h>
//----------------------------------------------------------------------------------------------------------------
#define RED_MASK_16		(0x1F << 11)
#define GREEN_MASK_16	(0x3F << 5)
#define BLUE_MASK_16	0x1F
//----------------------------------------------------------------------------------------------------------------
inline Uint16 make16bcolor( unsigned char red, unsigned char green, unsigned char blue )
{
	return (Uint16) (((red << 8) & RED_MASK_16) | ((green << 3) & GREEN_MASK_16) | (blue >> 3));
}
//----------------------------------------------------------------------------------------------------------------
SDL_Surface* SDL_LoadOCG(char *pOCGData, int iOCGDataSize)
{
	RendererOCG *pRendererOCG = new RendererOCG();
	pRendererOCG->SetOCGData(pOCGData, iOCGDataSize);
	SDL_Surface *pSurface = NULL;

	char *pPixelsData;
	int iPixelsDataSize;
	char *pPixelFormatData;
	int iPixelFormatDataSize;
	int iWidth;
	int iHeigth;

	if(pRendererOCG->GetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeigth))
	{
		pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, iWidth, iHeigth, 16, 31 << 0,  31 << 5, 31 << 10, 0);

		Uint16 Pixel = 0;
		for (int j = 0; j < iHeigth; j++)
		{
			for (int i = 0; i < iWidth; i++)
			{
				Pixel = ((Uint16 *)pPixelsData)[i + j * iWidth];
				putpixel(pSurface, i, j, Pixel);
			}
		}
	}

	PLUTO_SAFE_DELETE(pRendererOCG);
	return pSurface;
}
//----------------------------------------------------------------------------------------------------------------
SDL_Surface* SDL_LoadOCG(string sFilename)
{
	size_t iSize;
	char *pData = FileUtils::ReadFileIntoBuffer(sFilename, iSize);

	if(!pData)
		return NULL;

	SDL_Surface *pSurface = SDL_LoadOCG(pData, (int)iSize);

	PLUTO_SAFE_DELETE_ARRAY(pData);
	return pSurface;
}
//----------------------------------------------------------------------------------------------------------------
bool SDL_SaveOCG(SDL_Surface *pSurface, string sFilename, bool bPocket)
{
	int bpp = pSurface->format->BytesPerPixel;

	int iPixelFormatDataSize = sizeof(SDL_PixelFormat);
	char *pPixelFormatData = new char[iPixelFormatDataSize];
	memcpy(pPixelFormatData, pSurface->format, iPixelFormatDataSize);

	//we need even surfaces
	int iWidth = pSurface->w % 2 ? pSurface->w - 1 : pSurface->w;
	int iHeight = pSurface->h % 2 ? pSurface->h - 1 : pSurface->h;
	int iPixelsDataSize = iHeight * iWidth * pSurface->format->BytesPerPixel;

	if(bpp == 4 && bPocket)
		iPixelsDataSize /= 2;

	char *pPixelsData = new char[iPixelsDataSize];
	if(!bPocket || (bPocket && bpp == 2) )
	{
		memcpy(pPixelsData, pSurface->pixels, iPixelsDataSize);
	}
	else
	{
		char *pPixelsDataTemp = pPixelsData;

		for(int y = 0; y < iHeight; y++)
			for(int x = 0; x < iWidth; x++)
			{
				Uint8 * pixel = (Uint8 *) pSurface->pixels + y * pSurface->pitch + x * bpp;
				Uint32 color;
				Uint16 _16bit_color;

				if(bpp == 4)
				{
					color = * (Uint32 *) pixel;
					Uint8 *pColor = (Uint8 *)(&color);
					_16bit_color = make16bcolor(pColor[0], pColor[1], pColor[2]);
				}
				else if(bpp == 3)
				{
					if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
						color = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
					else
						color = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

					Uint8 *pColor = (Uint8 *)(&color);
					_16bit_color = make16bcolor(pColor[2], pColor[1], pColor[0]);
				}


				* (Uint16 *)pPixelsDataTemp = _16bit_color;
				pPixelsDataTemp += sizeof(Uint16);
			}
	}

	RendererOCG *pRendererOCG = new RendererOCG(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize,
		iWidth, iHeight);

	bool bResult = pRendererOCG->SaveOCG(sFilename);
	PLUTO_SAFE_DELETE(pRendererOCG);

	return bResult;
}
//----------------------------------------------------------------------------------------------------------------
