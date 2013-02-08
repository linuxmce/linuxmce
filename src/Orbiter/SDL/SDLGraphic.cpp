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
#include "SDLGraphic.h"
#include "Logger.h"

#include "SDL_rotozoom.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#include "SDLRendererOCGHelper.h"
#include "../../SDL_Helpers/SDL_Defs.h"

#include "../OrbiterRenderer.h"
#include "../Orbiter.h"
using namespace DCE;
//-------------------------------------------------------------------------------------------------------
SDLGraphic::SDLGraphic(string Filename, eGraphicManagement GraphicManagement,
					   OrbiterRenderer *pOrbiterRenderer)
					   : PlutoGraphic(Filename, GraphicManagement, pOrbiterRenderer)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
SDLGraphic::SDLGraphic(OrbiterRenderer *pOrbiterRenderer)
: PlutoGraphic(pOrbiterRenderer)
{
	Initialize();
}
//-------------------------------------------------------------------------------------------------------
SDLGraphic::SDLGraphic(struct SDL_Surface *pSDL_Surface)
{
	Initialize();

	m_pSDL_Surface = pSDL_Surface;
    if (m_pSDL_Surface == NULL)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SDLGraphic::SDLGraphic() : NULL m_pSDL_Surface");
        return;
    }
	m_nWidth = m_pSDL_Surface->w;
	m_nHeight = m_pSDL_Surface->h;
}
//-------------------------------------------------------------------------------------------------------
SDLGraphic::~SDLGraphic()
{
	Clear();
}
//-------------------------------------------------------------------------------------------------------
void SDLGraphic::Initialize()
{
	m_pSDL_Surface = NULL;
}
//-------------------------------------------------------------------------------------------------------
bool SDLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
	if(m_GraphicFormat == GR_OCG)
	{	
		m_pSDL_Surface = SDL_LoadOCG(pData, iSize);
	}
	else
	{
		SDL_RWops * rw = SDL_RWFromMem(pData, int(iSize));
		m_pSDL_Surface = IMG_Load_RW(rw, 1); // rw is freed here
	}

	if( !m_pSDL_Surface )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to read sdl graphic from data %p with size %d", pData, iSize);
		return false;
	}

    if(iRotation)
    {
        SDL_Surface *pSourceSurface = m_pSDL_Surface;
        m_pSDL_Surface = rotozoomSurface(pSourceSurface, iRotation, 1, SMOOTHING_ON);
        SDL_FreeSurface(pSourceSurface);
    }

	m_nWidth = m_pSDL_Surface->w;
	m_nHeight = m_pSDL_Surface->h;

	return true;
}
//-------------------------------------------------------------------------------------------------------
void SDLGraphic::Clear()
{
	if (m_pSDL_Surface)
	{
		SDL_FreeSurface(m_pSDL_Surface);
		m_pSDL_Surface = NULL;
	}
}
//-------------------------------------------------------------------------------------------------------

PlutoGraphic *SDLGraphic::GetHighlightedVersion()
{
    if (m_pSDL_Surface == NULL)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SDLGraphic::GetHighlightedVersion() : NULL m_pSDL_Surface");
        return NULL;
    }
	if( m_pSDL_Surface->format->BytesPerPixel!=4 )
		return NULL;

	SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
		m_nWidth, m_nHeight, BIT_PER_PIXEL, rmask, gmask, bmask, amask);

	SDL_Rect SourceRect;
	SourceRect.x = 0; SourceRect.y = 0;
	SourceRect.w = m_nWidth; SourceRect.h = m_nHeight;

	SDL_SetAlpha(m_pSDL_Surface, 0, 0);

	for(int w=0;w<m_nWidth;w++)
	{
		for(int h=0;h<m_nHeight;h++)
		{
		    unsigned char *pS = (unsigned char *) m_pSDL_Surface->pixels + h * m_pSDL_Surface->pitch + w * 4;
		    unsigned char *pD = (unsigned char *) pSDL_Surface->pixels + h * pSDL_Surface->pitch + w * 4;
			pD[0] = min(255,pS[0] + 30);
			pD[1] = min(255,pS[1] + 30);
			pD[2] = min(255,pS[2] + 30);
			pD[3] = 255;
		}
	}

	return new SDLGraphic(pSDL_Surface);
}

//-----------------------------------------------------------------------------------------------------
Uint32 SDLGraphic::getpixel(SDL_Surface *pSDL_Surface,int x, int y)
{
    // all pixels outside the pSDL_Surface are black
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return SDL_MapRGB(pSDL_Surface->format, 0, 0, 0);

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

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
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

//-----------------------------------------------------------------------------------------------------
#ifdef MAEMO_NOKIA770
void SDLGraphic::putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint16 pixel_color)
#else
void SDLGraphic::putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color)
#endif
{
    // don't try to put a pixel outside the pSDL_Surface
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return;

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
#ifdef MAEMO_NOKIA770		
		* (Uint16 *) pixel = pixel_color;
		
		/*
		Uint8 red, green, blue;

		SDL_PixelFormat * fmt = pSDL_Surface->format;	
		
		SDL_GetRGB((Uint32)pixel_color, pSDL_Surface->format, &red, &green, &blue);		
		
		unsigned char *Dest = (unsigned char *) &pixel_color;
		
		* (Uint16 *) pixel = (Dest[0]  << rshift) + (Dest[1] << gshift) + (Dest[2] << bshift);
		* (Uint16 *) pixel = ((red >> fmt->Rloss) << fmt->Rshift) +
			         			((green >> fmt->Gloss) << fmt->Gshift) +
			                 	((blue >> fmt->Bloss) << fmt->Bshift);
		*/						
   		
		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "pixel_color: 0x%x - %u %u %u - 0x%x\n", pixel_color, red, green, blue, pixel);
#else
		* (Uint16 *) pixel = pixel_color;
#endif 
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
        * (Uint32 *) pixel = pixel_color | 0xFF000000; //opaque
        break;
    }
}

PlutoGraphic* SDLGraphic::Clone()
{
	SDL_Surface* Surface = SDL_DisplayFormat(this->m_pSDL_Surface);
	return new SDLGraphic(Surface);
}

bool SDLGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{	
	//TODO : implement me!
    pRawBitmapData = NULL;	
	ulSize = 0;
	return false;
}

