/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __SDL_RENDER_UTILS_H__
#define __SDL_RENDER_UTILS_H__
//---------------------------------------------------------------------------------------------------
#include "RendererImage.h"
#include "../SerializeClass/ShapesColors.h"
//---------------------------------------------------------------------------------------------------
RendererImage *CreateBlankCanvas(PlutoSize size, bool bFillIt = false);
RendererImage *DuplicateImage(RendererImage * pRendererImage);

RendererImage *CreateFromFile(FILE * File, float fScaleX, float fScaleY, 
	PlutoSize size = PlutoSize(0,0), bool bPreserveAspectRatio = true,
	bool bCrop = false, char cScale = 0, PlutoRectangle offset = PlutoRectangle(0,0), 
	bool bUseAntiAliasing = true);

// If Crop is true and PreserveAspectRatio is true, then instead of shrinking to fit within the given space, it will
// fill the target space, and any excess will be cropped
RendererImage *CreateFromFile(string sFilename, float fScaleX, float fScaleY,
	PlutoSize size = PlutoSize(0,0), bool bPreserveAspectRatio = true, 
	bool bCrop = false, char cScale = 0,
	PlutoRectangle offset = PlutoRectangle(0,0), bool bUseAntiAliasing = true);

// if cUseAxis is 'X' or 'Y', then that axis will be used whether it's a crop or not
RendererImage *CreateFromRWops(SDL_RWops * rw, float fScaleX, float fScaleY,
	bool bFreeRWops = true, PlutoSize size = PlutoSize(0,0),
	bool bPreserveAspectRatio = true,bool bCrop = false,char cScale = 'F',
	PlutoRectangle offset = PlutoRectangle(0,0), bool bUseAntiAliasing = true);  

void CompositeImage(RendererImage *pRenderImage_Parent, RendererImage *pRenderImage_Child, PlutoPoint pos);
void CompositeAlpha(RendererImage *pRenderImage_Parent, RendererImage *pRenderImage_Child, PlutoPoint pos);
int DoRenderToSurface(SDL_Surface * Surface, list<RendererImage *> &RI, int posX, int posY);

void SetTransparentColor(SDL_Surface *pSurface, int R, int G, int B);
void SetGeneralSurfaceOpacity(SDL_Surface *pSurface, int SDL_Opacity);

void ChangeSDLSurfaceFormatForPluto(SDL_Surface **pSurface);
//---------------------------------------------------------------------------------------------------
#endif //__SDL_RENDER_UTILS_H__
