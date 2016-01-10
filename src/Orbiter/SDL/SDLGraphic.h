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
#ifndef __SDL_GRAPHIC_H__
#define __SDL_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include "SDL.h"
#include "../PlutoGraphic.h"
//-------------------------------------------------------------------------------------------------------
class OrbiterRenderer;
//-------------------------------------------------------------------------------------------------------
class SDLGraphic : public PlutoGraphic
{
public:
	SDLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);
	SDLGraphic(struct SDL_Surface *pSDL_Surface);
	SDLGraphic(OrbiterRenderer *pOrbiterRenderer);
	~SDLGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtSDLGraphic; }
	bool IsEmpty() { return NULL == m_pSDL_Surface; }
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0);
	void Clear(); 
	virtual PlutoGraphic *GetHighlightedVersion();
	PlutoGraphic* Clone();
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);

	struct SDL_Surface *m_pSDL_Surface;

	//helper methods
	static Uint32 getpixel(struct SDL_Surface *pSDL_Surface,int x, int y);
#ifdef MAEMO_NOKIA770	
	static void putpixel(struct SDL_Surface *pSDL_Surface,int x, int y, Uint16 pixel_color);
#else
	static void putpixel(struct SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color);
#endif
};
//-------------------------------------------------------------------------------------------------------
#endif //__SDL_GRAPHIC_H__

