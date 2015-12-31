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
#include "GD-SDL.h"
#include "Orbiter/SDL/PlutoSDLDefs.h"

#include <SDL.h>
#include <gd.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
//------------------------------------------------------------------------------------------------------------------------
template <class T> inline void Swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}
//------------------------------------------------------------------------------------------------------------------------
bool GD_SavePNG(char *filename, gdImagePtr im)
{
    FILE *out;
    int size;
    char *data;
    out = fopen(filename, "wb");
    if (!out) 
        return false;

    data = (char *) gdImagePngPtr(im, &size);
    if (!data) 
        return false;

    fwrite(data, 1, size, out);
    fclose(out);
    gdFree(data);  
    return true;
}
//------------------------------------------------------------------------------------------------------------------------
gdImagePtr GD_LoadPNG(char *filename)
{
    FILE *in;
    struct stat stat_buf;
    gdImagePtr im;
    in = fopen(filename, "rb");
    if (!in) 
        return NULL;

    if (fstat(fileno(in), &stat_buf) != 0) 
        return NULL;

    // Read the entire thing into a buffer 
    //that we allocate 
    char *buffer = (char *)malloc(stat_buf.st_size);

    fread(buffer, 1, stat_buf.st_size, in);
 
    im = gdImageCreateFromPngPtr(stat_buf.st_size, buffer);
    // WE allocated the memory, WE free 
    //it with our normal free function 
    free(buffer);
    fclose(in);
    return im;
} 
//------------------------------------------------------------------------------------------------------------------------
static SDL_Surface* GDSurface2SDLSurface(gdImagePtr pGD_Surface)
{
    SDL_Surface *pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, pGD_Surface->sx, pGD_Surface->sy, 32, 
        rmask, gmask, bmask, amask);

    for(int n = 0; n < pGD_Surface->sy; n++)
    {
        memcpy((unsigned char *)pSurface->pixels + n * pSurface->pitch, pGD_Surface->tpixels[n], pSurface->pitch);
        for(int i = 0; i < pGD_Surface->sx; i++)
        {
            unsigned char *colors = (unsigned char *)pSurface->pixels + n * pSurface->pitch + 4 * i;
            Swap(colors[0], colors[2]);
            colors[3] = 0;
        }
    }

    return pSurface;
}
//------------------------------------------------------------------------------------------------------------------------
void DumpImage(gdImagePtr pImage, char *pFilename)
{
    FILE *file = fopen(pFilename, "wb");

    for(int n = 0; n < pImage->sy; n++)
    {
        for(int i = 0; i < pImage->sx; i++)
            fprintf(file, "%x\t", pImage->tpixels[n][i]);
        fprintf(file, "\r\n");
    }

    fclose(file);
}
//------------------------------------------------------------------------------------------------------------------------
static gdImagePtr SDLSurface2GDSurface(SDL_Surface* pSDL_Surface)
{
    SDL_Surface *pConvertedSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, pSDL_Surface->w, pSDL_Surface->h, 32, 
        rmask, gmask, bmask, amask);
    SDL_BlitSurface(pSDL_Surface, NULL, pConvertedSurface, NULL);

    gdImagePtr pImage = gdImageCreateTrueColor(pConvertedSurface->w, pConvertedSurface->h);
    for(int n = 0; n < pConvertedSurface->h; n++)
    {
        memcpy(pImage->tpixels[n], (unsigned char *) pConvertedSurface->pixels + n * pConvertedSurface->pitch, 
            pConvertedSurface->pitch);

        for(int i = 0; i < pConvertedSurface->w; i++)
        {
            unsigned char *colors = (unsigned char *)&pImage->tpixels[n][i];
            Swap(colors[0], colors[2]);
            colors[3] = 0;
        }
    }

    SDL_FreeSurface(pConvertedSurface);
    return pImage;
}
//------------------------------------------------------------------------------------------------------------------------
SDL_Surface* GD_ResizeSurface(SDL_Surface* pSurface, double ScaleX, double ScaleY, bool bResampled)
{
    gdImagePtr pSrcImage = SDLSurface2GDSurface(pSurface);
    gdImagePtr pResizedImage = gdImageCreateTrueColor(int(pSrcImage->sx * ScaleX), int(pSrcImage->sy * ScaleY));

    if(bResampled)
    {
        gdImageCopyResampled(pResizedImage, pSrcImage, 0, 0, 0, 0, pResizedImage->sx, pResizedImage->sy, 
            pSrcImage->sx, pSrcImage->sy);
        gdImageSharpen(pResizedImage, 70);
    }
    else
    {
        gdImageCopyResized(pResizedImage, pSrcImage, 0, 0, 0, 0, pResizedImage->sx, pResizedImage->sy, 
           pSrcImage->sx, pSrcImage->sy);
    }

    SDL_Surface *pDestSurface = GDSurface2SDLSurface(pResizedImage);

    gdImageDestroy(pSrcImage);
    gdImageDestroy(pResizedImage);

    return pDestSurface;
}
//------------------------------------------------------------------------------------------------------------------------
