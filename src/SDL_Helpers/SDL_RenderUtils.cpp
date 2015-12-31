/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "SDL_RenderUtils.h"
#include "SDL_Defs.h"
#include "SDL_CompositeAlphaChannel.h"

#include <SDL_rotozoom.h>
#include <SDL_image.h>
//---------------------------------------------------------------------------------------------------
RendererImage *CreateBlankCanvas(PlutoSize size, bool bFillIt/* = false*/)
{
	RendererImage *Canvas = new RendererImage;

	Canvas->m_sFilename = "(new surface)";
	Canvas->m_pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, size.Width, size.Height, 32, 
		rmask, gmask, bmask, amask);

	if (Canvas->m_pSurface == NULL)
	{
#ifndef WINCE
		cout << "Failed to create blank canvas "
			"width: " << size.Width << " height " << size.Height << " error: " << SDL_GetError() << endl;
#endif
		throw string("Failed to create blank canvas: ") + SDL_GetError();
		delete Canvas;
		Canvas = NULL;
	}

	if( bFillIt )
	{
		SDL_FillRect(Canvas->m_pSurface, NULL, 
			SDL_MapRGBA(Canvas->m_pSurface->format, 0, 0, 0, 255));
	}

	Canvas->NewSurface = true;
	return Canvas;
}
//---------------------------------------------------------------------------------------------------
void CompositeImage(RendererImage *pRenderImage_Parent, RendererImage * pRenderImage_Child, PlutoPoint pos)
{
	// with no destination, function always fails
	if (pRenderImage_Parent == NULL || pRenderImage_Parent->m_pSurface == NULL)
		throw "Composite image passed null parent";

	// with no source, function always succeeds (I assume that it's an empty image)
	if (pRenderImage_Child == NULL || pRenderImage_Child->m_pSurface == NULL)
		throw "Composite image passed null child";

	SDL_Rect dest_rect;
	dest_rect.x = pos.X;
	dest_rect.y = pos.Y;

	bool bCompositeAlpha = true;
	bool WasSrcAlpha = (pRenderImage_Child->m_pSurface->flags & SDL_SRCALPHA) != 0;
	Uint8 WasAlpha = pRenderImage_Child->m_pSurface->format->alpha;
	if (pRenderImage_Parent->NewSurface)
	{
		SDL_SetAlpha(pRenderImage_Child->m_pSurface, 0, 0);
		pRenderImage_Parent->NewSurface = false;
		bCompositeAlpha = false;
	}

	if (SDL_BlitSurface(pRenderImage_Child->m_pSurface, NULL, pRenderImage_Parent->m_pSurface, &dest_rect) < 0)
	{
		throw string("Failed composing image: ") + SDL_GetError();
	}
	if (bCompositeAlpha)
		CompositeAlpha(pRenderImage_Parent, pRenderImage_Child, pos);

	SDL_SetAlpha(pRenderImage_Child->m_pSurface, WasSrcAlpha, WasAlpha);
}
//---------------------------------------------------------------------------------------------------
// Composite Alpha channel after alpha-blended SDL_Blit
void CompositeAlpha(RendererImage * pRenderImage_Parent, RendererImage * pRenderImage_Child, PlutoPoint pos)
{
	// with no destination, function always fails
	if (pRenderImage_Parent == NULL || pRenderImage_Parent->m_pSurface == NULL)
		throw "Composite image passed null parent";
	// with no source, function always succeeds (I assume that it's an empty image)
	if (pRenderImage_Child == NULL || pRenderImage_Child->m_pSurface == NULL)
		throw "Composite image passed null child";

	int iChildHeight = pRenderImage_Child->m_pSurface->h;
	int iChildWidth = pRenderImage_Child->m_pSurface->w;

	SDL_Rect SDL_pos;
	SDL_pos.x = pos.X;
	SDL_pos.y = pos.Y;
	CompositeAlphaChannel(pRenderImage_Child->m_pSurface, pRenderImage_Parent->m_pSurface, &SDL_pos);
}
//---------------------------------------------------------------------------------------------------
int DoRenderToSurface(SDL_Surface *Surface, list<RendererImage *> &RI, int posX, int posY)
{
	list<RendererImage *>::iterator i;

	RendererImage * myRI;
	for (i = RI.begin(); i != RI.end(); i++)
	{
		SDL_Rect dest;
		myRI = (RendererImage *) * i;

		dest.x = posX; dest.y = posY;

		SDL_BlitSurface(myRI->m_pSurface, NULL, (SDL_Surface *) Surface, &dest);
		CompositeAlphaChannel(myRI->m_pSurface, Surface, &dest);

		posX += myRI->m_pSurface->w;
	}

	return myRI->m_pSurface->h;
}
//---------------------------------------------------------------------------------------------------
void SetTransparentColor(SDL_Surface *pSurface, int R, int G, int B)
{
	for(int w = 0; w < pSurface->w; w++)
	{
		for(int h = 0; h < pSurface->h; h++)
		{
			unsigned char *pD = (unsigned char *) pSurface->pixels + h * pSurface->pitch + w * 4;
			if(pD[0] == R && pD[1] == G && pD[2] == B)
			{
				pD[0] = 0xFF;
				pD[1] = 0xFF;
				pD[2] = 0xFF;
				pD[3] = 0x00;
			}
		}
	}
}
//---------------------------------------------------------------------------------------------------
void SetGeneralSurfaceOpacity(SDL_Surface *pSurface, int SDL_Opacity)
{
    for(int w = 0; w < pSurface->w; w++)
    {
        for(int h = 0; h < pSurface->h; h++)
        {
            unsigned char *pD = (unsigned char *) pSurface->pixels + h * pSurface->pitch + w * 4;
            pD[3] = SDL_Opacity;
        }
    }
}
//---------------------------------------------------------------------------------------------------
void ChangeSDLSurfaceFormatForPluto(SDL_Surface **pSurface)
{
	SDL_Surface * pOldSurface = (*pSurface);

	// check if anything needs to be done
	if (pOldSurface->format->Ashift == ashift && pOldSurface->format->Rshift == rshift && pOldSurface->format->Gshift == gshift && pOldSurface->format->Bshift == bshift)
		return;

	SDL_Surface * pNewSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, pOldSurface->w, pOldSurface->h, 32, rmask, gmask, bmask, amask);
	for (int h = 0; h < pOldSurface->h; h++)
	{
		for (int w = 0; w < pOldSurface->w; w++)
		{
			unsigned char *pOldPixel = (unsigned char *) pOldSurface->pixels + h * pOldSurface->pitch + w * 4;
			unsigned char *pNewPixel = (unsigned char *) pNewSurface->pixels + h * pNewSurface->pitch + w * 4;

			unsigned char R, G, B, A;
			R = (* (unsigned int *) pOldPixel >> pOldSurface->format->Rshift) & 0xff;
			G = (* (unsigned int *) pOldPixel >> pOldSurface->format->Gshift) & 0xff;
			B = (* (unsigned int *) pOldPixel >> pOldSurface->format->Bshift) & 0xff;
			A = (* (unsigned int *) pOldPixel >> pOldSurface->format->Ashift) & 0xff;

			* (unsigned int *) pNewPixel = (R << rshift) | (G << gshift) | (B << bshift) | (A << ashift);
		}
	}
	SDL_FreeSurface(pOldSurface);
	*pSurface = pNewSurface;
}
//---------------------------------------------------------------------------------------------------
RendererImage *CreateFromFile(FILE * File, float fScaleX, float fScaleY, PlutoSize size, 
	bool bPreserveAspectRatio, bool bCrop,char cScale,PlutoRectangle offset, bool bUseAntiAliasing)
{
	SDL_RWops * rw = SDL_RWFromFP(File, 0);
	return CreateFromRWops(rw, fScaleX, fScaleY, true, size, bPreserveAspectRatio, 
		bCrop, cScale, offset, bUseAntiAliasing);
}
//---------------------------------------------------------------------------------------------------
RendererImage *DuplicateImage(RendererImage * pRendererImage)
{
	PlutoSize Size;
	PlutoPoint Pos(0, 0);

	Size.Width = pRendererImage->m_pSurface->w;
	Size.Height = pRendererImage->m_pSurface->h;
	RendererImage * Result = CreateBlankCanvas(Size);
	CompositeImage(Result, pRendererImage, Pos);

	return Result;
}
//---------------------------------------------------------------------------------------------------
RendererImage *CreateFromRWops(SDL_RWops * rw, float fScaleX, float fScaleY, bool bFreeRWops, PlutoSize size, 
	bool bPreserveAspectRatio, bool bCrop, char cScale, PlutoRectangle offset, bool bUseAntiAliasing)
{
	SDL_Surface * SurfaceFromFile=NULL;
	SurfaceFromFile = IMG_Load_RW(rw, bFreeRWops); // rw is freed here of bFreeRWops is true (=1)

	if (SurfaceFromFile == NULL)
	{
#ifndef WINCE
		cout << "Can't create surface from FILE pointer: " << SDL_GetError() << endl;		
#endif

		//return a dummy surface instead
		return CreateBlankCanvas(size);
	}

	if( offset.X || offset.Y || offset.Width || offset.Height )
	{
		SDL_Rect src_rect,dest_rect;
		src_rect.x=src_rect.y=dest_rect.x=dest_rect.y=0;
		src_rect.w=SurfaceFromFile->w;
		src_rect.h=SurfaceFromFile->h;

		if( offset.X>0 )
		{
			src_rect.x = offset.X;
			src_rect.w = SurfaceFromFile->w - offset.X;
		}
		else
			dest_rect.x = offset.X;

		if( offset.Y>0 )
		{
			src_rect.y = offset.Y;
			src_rect.h = SurfaceFromFile->h - offset.Y;
		}
		else
			dest_rect.y = offset.Y;

		if( offset.Width>0 )
			src_rect.w = offset.Width - offset.X;
		if( offset.Height>0 )
			src_rect.h = offset.Height - offset.Y;

		SDL_Surface *pCropped_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, src_rect.w, src_rect.h, 32, rmask, gmask, bmask, amask);

		SDL_SetAlpha(SurfaceFromFile, 0, 0);
		SDL_BlitSurface(SurfaceFromFile, &src_rect, pCropped_Surface, &dest_rect);

		SDL_FreeSurface(SurfaceFromFile);
		SurfaceFromFile = pCropped_Surface;
	}

	int W = size.Width == 0 ? SurfaceFromFile->w : size.Width;
	int H = size.Height == 0 ? SurfaceFromFile->h : size.Height;
	PlutoSize new_size(W, H);

	RendererImage * RIFromFile = CreateBlankCanvas(new_size);
	if (RIFromFile == NULL)
	{
		delete SurfaceFromFile;
		throw string("Failed to create new image surface");
	}

	SDL_Surface * ScaledSurface;
	if (W == SurfaceFromFile->w && H == SurfaceFromFile->h)
	{
		// no scaling/stretching needed
#if defined(USE_SGE_TRANSFORM)
		ScaledSurface = sge_transform_surface(SurfaceFromFile, SDL_MapRGBA(SurfaceFromFile->format, 0, 0, 0, 0), 0, 1, 1, bUseAntiAliasing ? SGE_TSAFE | SGE_TAA : SGE_TSAFE);
#else
		ScaledSurface = zoomSurface(SurfaceFromFile, 1, 1, bUseAntiAliasing ? SMOOTHING_ON : SMOOTHING_OFF);
#endif
	}
	else
	{
		// image needs to be steched/scaled
		// I could use SDL_SoftStretch(), but the SDL developers strongly advise against it for the moment
		// I use the SGE extension library instead
		float scaleX,scaleY;

		if( cScale=='S' )
		{
			scaleX = fScaleX;
			scaleY = fScaleY;
		}
		else
		{
			scaleX = (float) RIFromFile->m_pSurface->w / SurfaceFromFile->w;
			scaleY = (float) RIFromFile->m_pSurface->h / SurfaceFromFile->h;

			if( cScale=='Y' )
				scaleX = scaleY;
			else if( cScale=='X' )
				scaleY = scaleX;
			else if( bPreserveAspectRatio && bCrop )
			{
				if( scaleY>scaleX )  // Take the greater scale, ie cropping the edges
					scaleX=scaleY;
				else
					scaleY=scaleX;
			}
			else if( bPreserveAspectRatio )
			{
				if( scaleY<scaleX ) // Take the lesser scale, ie shrinking to fit
				{
					if( scaleY/scaleX<0.94 )  // If we're this close, then it's probably just a rounding error during scaling.  A 6% distortion won't be noticeable, it's more important to fit the target size exactly
						scaleX=scaleY;
				}
				else
				{
					if( scaleX/scaleY<0.94 )
						scaleY=scaleX;
				}
			}
		}
		
		//starting with the mobile phone, we have 'distorted' images because we want to re-use buttons, but the aspect ratios are different
#if defined(USE_SGE_TRANSFORM)
		ScaledSurface = sge_transform_surface(SurfaceFromFile, SDL_MapRGBA(SurfaceFromFile->format, 0, 0, 0, 0), 0, scaleX, scaleY, bUseAntiAliasing ? SGE_TSAFE | SGE_TAA : SGE_TSAFE );
#else
		ScaledSurface = zoomSurface(SurfaceFromFile, scaleX, scaleY, bUseAntiAliasing ? SMOOTHING_ON : SMOOTHING_OFF);
#endif
	}

	SDL_SetAlpha(ScaledSurface, 0, 0);
	SDL_BlitSurface(ScaledSurface, NULL, RIFromFile->m_pSurface, NULL);

	SDL_FreeSurface(ScaledSurface);
	SDL_FreeSurface(SurfaceFromFile);

	return RIFromFile;
}
//---------------------------------------------------------------------------------------------------
RendererImage * CreateFromFile(string sFilename, float fScaleX, float fScaleY, PlutoSize size, 
	bool bPreserveAspectRatio, bool bCrop, char cScale, PlutoRectangle offset, bool bUseAntiAliasing)
{
	FILE * File = fopen(sFilename.c_str(), "rb");
	RendererImage * Result = CreateFromFile(File, fScaleX, fScaleY, size, bPreserveAspectRatio, 
		bCrop, cScale, offset,bUseAntiAliasing);

	if (Result == NULL)
		throw "Can't create surface from file: " + sFilename  + ": " + SDL_GetError();

	fclose(File);
	return Result;
}
//---------------------------------------------------------------------------------------------------

