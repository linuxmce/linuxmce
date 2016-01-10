/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "SDL_SavePNG.h"
#include "png.h"

bool IMG_SavePNG(SDL_Surface *pSurface, FILE * FileHandle, bool bPrependSignature)
{
	if (pSurface == NULL || FileHandle == NULL)
		return false;

	png_bytepp image_rows;
	int BitsPerColor;

	image_rows = new png_bytep[pSurface->h];
	for (int i = 0; i < pSurface->h; i++)
		image_rows[i] = (unsigned char *) pSurface->pixels + i * pSurface->pitch;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop png_info = png_create_info_struct(png_ptr);

	png_init_io(png_ptr, FileHandle);

	if (bPrependSignature == false)
		png_set_sig_bytes(png_ptr, 8);

	png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	BitsPerColor = pSurface->format->BitsPerPixel / pSurface->format->BytesPerPixel;
	png_set_IHDR(png_ptr, png_info, pSurface->w, pSurface->h, BitsPerColor, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//	png_set_bgr(png_ptr); // on x86, 24bit makes colors appear right; not for 32bit (RGBA) or it mixes the colors
	png_write_info(png_ptr, png_info);

	png_write_image(png_ptr, image_rows);
	png_write_end(png_ptr, png_info);

	delete [] image_rows;

	png_destroy_write_struct(&png_ptr, &png_info);

	return true;
}

bool IMG_SavePNG(SDL_Surface *pSurface, const string & sFileName)
{
	if (pSurface == NULL)
		return false;

	FILE * F = fopen(sFileName.c_str(), "wb");
	if (F == NULL)
		return false;

	bool bResult = IMG_SavePNG(pSurface, F);
	fclose(F);
	return bResult;
}
