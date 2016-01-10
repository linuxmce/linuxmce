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
#include "PNGWrapper.h"
//-------------------------------------------------------------------------------------------------------
#include "src/internal/graphicbuffer.h"
#include "src/internal/raster2d.h"
using namespace Internal;
//-------------------------------------------------------------------------------------------------------
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>

#include "png.h"
#include "cexcept.h"
//-------------------------------------------------------------------------------------------------------
define_exception_type(const char *);
extern struct exception_context the_exception_context[1];
struct exception_context the_exception_context[1];
png_const_charp msg;
//-------------------------------------------------------------------------------------------------------
static OPENFILENAME ofn;
static png_structp png_ptr = NULL;
static png_infop info_ptr = NULL;
//-------------------------------------------------------------------------------------------------------
static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
static void png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
static void png_flush(png_structp png_ptr);
//-------------------------------------------------------------------------------------------------------
static void png_cexcept_error(png_structp png_ptr, png_const_charp msg);
BOOL PngLoadImage (PTSTR pstrFileName, png_byte **ppbImageData, int *piWidth, int *piHeight, int *piChannels, png_color *pBkgColor);
//-------------------------------------------------------------------------------------------------------
Surface *PocketFrog_LoadPNG(DisplayDevice* pDevice, string sFilename)
{
	BYTE              *pbImage = NULL;
	int                cxImgSize = 0, cyImgSize = 0;
	int                cImgChannels = 0;
	png_color          bkgColor = {127, 127, 127};

	if(!PngLoadImage ((PTSTR)sFilename.c_str(), &pbImage, &cxImgSize, &cyImgSize, &cImgChannels, &bkgColor))
		return NULL;

	Surface *pSurface = pDevice->CreateSurface(cxImgSize, cyImgSize);
	Rasterizer *pRasterizer = pDevice->CreateRasterizer(pSurface);

	for(int y = 0; y < cyImgSize; y++)
		for(int x = 0; x < cxImgSize; x++)
		{
			BYTE * pixel = (BYTE *) &(pbImage[cImgChannels * (x + y * cxImgSize)]);

			unsigned long color;
			Pixel _16bit_color;

			if(cImgChannels == 4)
			{
				color = * (unsigned long *) pixel;
				BYTE *pColor = (BYTE *)(&color);
				_16bit_color = Color(pColor[0], pColor[1], pColor[2]);
			}
			else if(cImgChannels == 3)
			{
				color = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
				BYTE *pColor = (BYTE *)(&color);
				_16bit_color = Color(pColor[2], pColor[1], pColor[0]);
			}
			pRasterizer->SetPixel(x, y, _16bit_color);
		}

	free(pbImage);
	pbImage = NULL;

    delete pRasterizer;
	return pSurface;
}
//-------------------------------------------------------------------------------------------------------
static void png_cexcept_error(png_structp png_ptr, png_const_charp msg)
{
	Throw msg;
}
//-------------------------------------------------------------------------------------------------------
BOOL PngLoadImage (PTSTR pstrFileName, png_byte **ppbImageData,
                   int *piWidth, int *piHeight, int *piChannels, png_color *pBkgColor)
{
    static FILE        *pfFile;
    png_byte            pbSig[8];
    int                 iBitDepth;
    int                 iColorType;
    double              dGamma;
    png_color_16       *pBackground;
    png_uint_32         ulChannels;
    png_uint_32         ulRowBytes;
    png_byte           *pbImageData = *ppbImageData;
    static png_byte   **ppbRowPointers = NULL;
    int                 i;

    // open the PNG input file

    if (!pstrFileName)
    {
        *ppbImageData = pbImageData = NULL;
        return FALSE;
    }

    if (!(pfFile = fopen(pstrFileName, "rb")))
    {
        *ppbImageData = pbImageData = NULL;
        return FALSE;
    }

    // first check the eight byte PNG signature

    fread(pbSig, 1, 8, pfFile);
    if (!png_check_sig(pbSig, 8))
    {
        *ppbImageData = pbImageData = NULL;
        return FALSE;
    }

    // create the two png(-info) structures

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
      (png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
    if (!png_ptr)
    {
        *ppbImageData = pbImageData = NULL;
        return FALSE;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        *ppbImageData = pbImageData = NULL;
        return FALSE;
    }

    Try
    {
        
        // initialize the png structure
        
	    png_set_read_fn(png_ptr, (png_voidp)pfFile, png_read_data);
        
        png_set_sig_bytes(png_ptr, 8);
        
        // read all PNG info up to image data
        
        png_read_info(png_ptr, info_ptr);
        
        // get width, height, bit-depth and color-type
        
        png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)piWidth, (png_uint_32 *)piHeight, &iBitDepth,
            &iColorType, NULL, NULL, NULL);
        
        // expand images of all color-type and bit-depth to 3x8 bit RGB images
        // let the library process things like alpha, transparency, background
        
        if (iBitDepth == 16)
            png_set_strip_16(png_ptr);
        if (iColorType == PNG_COLOR_TYPE_PALETTE)
            png_set_expand(png_ptr);
        if (iBitDepth < 8)
            png_set_expand(png_ptr);
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            png_set_expand(png_ptr);
        if (iColorType == PNG_COLOR_TYPE_GRAY ||
            iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png_ptr);
        
        // set the background color to draw transparent and alpha images over.
        if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
        {
            png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
            pBkgColor->red   = (byte) pBackground->red;
            pBkgColor->green = (byte) pBackground->green;
            pBkgColor->blue  = (byte) pBackground->blue;
        }
        else
        {
            pBkgColor = NULL;
        }
        
        // if required set gamma conversion
        if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
            png_set_gamma(png_ptr, (double) 2.2, dGamma);
        
        // after the transformations have been registered update info_ptr data
        
        png_read_update_info(png_ptr, info_ptr);
        
        // get again width, height and the new bit-depth and color-type
        
        png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)piWidth, (png_uint_32 *)piHeight, &iBitDepth,
            &iColorType, NULL, NULL, NULL);
        
        
        // row_bytes is the width x number of channels
        
        ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
        ulChannels = png_get_channels(png_ptr, info_ptr);
        
        *piChannels = ulChannels;
        
        // now we can allocate memory to store the image
        
        if (pbImageData)
        {
            free (pbImageData);
            pbImageData = NULL;
        }
        if ((pbImageData = (png_byte *) malloc(ulRowBytes * (*piHeight)
                            * sizeof(png_byte))) == NULL)
        {
            png_error(png_ptr, "Visual PNG: out of memory");
        }
        *ppbImageData = pbImageData;
        
        // and allocate memory for an array of row-pointers
        
        if ((ppbRowPointers = (png_bytepp) malloc((*piHeight)
                            * sizeof(png_bytep))) == NULL)
        {
            png_error(png_ptr, "Visual PNG: out of memory");
        }
        
        // set the individual row-pointers to point at the correct offsets
        
        for (i = 0; i < (*piHeight); i++)
            ppbRowPointers[i] = pbImageData + i * ulRowBytes;
        
        // now we can go ahead and just read the whole image
        
        png_read_image(png_ptr, ppbRowPointers);
        
        // read the additional chunks in the PNG file (not really needed)
        
        png_read_end(png_ptr, NULL);
        
        // and we're done
        
        free (ppbRowPointers);
        ppbRowPointers = NULL;
        
        // yepp, done
    }

    Catch (msg)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        *ppbImageData = pbImageData = NULL;
        
        if(ppbRowPointers)
            free (ppbRowPointers);

        fclose(pfFile);

        return FALSE;
    }

    fclose (pfFile);

    return TRUE;
}
//-------------------------------------------------------------------------------------------------------
static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	/* fread() returns 0 on error, so it is OK to store this in a png_size_t
	* instead of an int, which is what fread() actually returns.
	*/
	check = (png_size_t)fread(data, (png_size_t)1, length,
		(FILE *)png_ptr->io_ptr);

	if (check != length)
	{
		png_error(png_ptr, "Read Error");
	}
}
//-------------------------------------------------------------------------------------------------------
static void png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_uint_32 check;

	check = (png_uint_32)fwrite(data, 1, length, (FILE *)(png_ptr->io_ptr));
	if (check != length)
	{
		png_error(png_ptr, "Write Error");
	}
}
//-------------------------------------------------------------------------------------------------------
static void png_flush(png_structp png_ptr)
{
	FILE *io_ptr;
	io_ptr = (FILE *)CVT_PTR((png_ptr->io_ptr));
	if (io_ptr != NULL)
		fflush(io_ptr);
}
//-------------------------------------------------------------------------------------------------------