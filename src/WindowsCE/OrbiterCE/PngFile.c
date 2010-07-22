//-------------------------------------
// PNGFILE.C -- Image File Functions
//-------------------------------------

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>

#include "png.h"
#include "PngFile.h"
#include "CeGccSupplement.h"

// PNG image handler functions

struct PngStreamData
{
	png_byte *PngAddress;
	unsigned long int Length;
	unsigned long int Offset;
};

static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	struct PngStreamData *StreamData = (struct PngStreamData *) png_get_io_ptr(png_ptr);

	if (StreamData->Offset + length > StreamData->Length)
	{
		png_error(png_ptr, "Read Error");
	}
	else
	{
		memcpy(data, StreamData->PngAddress + StreamData->Offset, length);
		StreamData->Offset += length;
	}
}

HBITMAP GetDIB(long Width, long Height)
{
	BITMAPINFO BitmapInfo;
	HBITMAP Bitmap;
	DWORD *Pixels;

	memset(&BitmapInfo, 0, sizeof(BITMAPINFO));

	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = -Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	Bitmap = CreateDIBSection(NULL, &BitmapInfo, DIB_RGB_COLORS, &Pixels, NULL, 0);

	return Bitmap;
}

HBITMAP PngLoadImageFromRAM(png_byte *PngAddress, unsigned long int length)
{
	int iBitDepth;
	int iColorType;
	int iWidth, iHeight;
	double dGamma;
	png_color_16 *pBackground;
	png_uint_32 ulChannels;
	png_uint_32 ulRowBytes;
	png_byte **ppbRowPointers = NULL;
	int i;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;


	if (PngAddress == NULL)
		return NULL;

	// first check the eight byte PNG signature
	if (!png_check_sig(PngAddress, 8))
		return NULL;

	// create the two png(-info) structures
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		return NULL;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	// setjmp() must be called in every function that calls a PNG-reading libpng function
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}

	// initialize the png structure

	struct PngStreamData StreamData;
	StreamData.PngAddress = PngAddress;
	StreamData.Length = length;
	StreamData.Offset = 0;
	png_set_read_fn(png_ptr, (png_voidp) &StreamData, PngReadData);

	//png_set_sig_bytes(png_ptr, 8);

	// read all PNG info up to image data
	png_read_info(png_ptr, info_ptr);

	// get width, height, bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, &iWidth, &iHeight, &iBitDepth, &iColorType, NULL, NULL, NULL);

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
	if (iColorType == PNG_COLOR_TYPE_GRAY || iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);
	if (iColorType == PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

	// WinCE DIBs store colours in BGR(A) format, while PNGs store colours in RGB(A) format
	// Note: DIB pixels are always stored in DWORDs, even when there is no alpha channel
	if (iColorType == PNG_COLOR_TYPE_RGB || iColorType == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);

#if 0 // if this is enabled and the png has a bKGD chunk, the alpha channel isn't generated and all the pixel bytes are shifted
	// set the background color to draw transparent and alpha images over.
	if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
		png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
#endif

	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, (double) 2.2, dGamma);

	// after the transformations have been registered update info_ptr data
	png_read_update_info(png_ptr, info_ptr);

	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, &iWidth, &iHeight, &iBitDepth, &iColorType,
			NULL, NULL, NULL);

	// row_bytes is the width x number of channels
	ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
	ulChannels = png_get_channels(png_ptr, info_ptr);

	// and allocate memory for an array of row-pointers
	if ((ppbRowPointers = (png_bytepp) malloc((iHeight) * sizeof(png_bytep))) == NULL)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}

	HBITMAP Bitmap = GetDIB(iWidth, iHeight);
	DIBSECTION DibSection;
	if (GetObject(Bitmap, sizeof(DIBSECTION), &DibSection) == 0)
	{
		free(ppbRowPointers);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}

	// set the individual row-pointers to point at the correct offsets
	for (i = 0; i < iHeight; i++)
		ppbRowPointers[i] = DibSection.dsBm.bmBits + i * ulRowBytes;

	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);

	// read the additional chuncks in the PNG file (not really needed)
	png_read_end(png_ptr, NULL);

	// and we're done
	free(ppbRowPointers);
	ppbRowPointers = NULL;
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return Bitmap;
}

//-----------------
// end of source
//-----------------
