//////////////////////////////////////////////////////////////////////////////
//
// PocketFrog - The Game Library for Pocket PC Devices
// Copyright 2002  Thierry Tremblay
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Thierry Tremblay makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
//////////////////////////////////////////////////////////////////////////////

#include "image.h"
#include "display.h"
#include "surface.h"
#include "pocketpc.h"
#include "internal/memfile.h"
#include <cassert>

#ifdef _WIN32_WCE
   #include "foreign/imgdecmp/imgdecmp.h"
	#ifdef ARMV4
		#pragma comment( lib, "imgdecmp.lib" )
	#endif
#else
   #include <olectl.h>
   #pragma comment( lib, "ole32.lib" )
   #pragma comment( lib, "olepro32.lib" )
   #define HIMETRIC_INCH 2540
#endif



namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// CreateSurface() - Create a surface from a bitmap
//
//////////////////////////////////////////////////////////////////////////////

static Surface* CreateSurface( DisplayDevice* display, HBITMAP hBitmap )
{
   // Retrieve bitmap info
   BITMAP bitmap;
   ::GetObject( hBitmap, sizeof(BITMAP), &bitmap );

   // Create the surface
   Surface* surface = display->CreateSurface( bitmap.bmWidth, bitmap.bmHeight );

   // Blit the bitmap on the surface, doing color conversion if needed
   HDC hDestDC = surface->GetDC( true );
   HDC hSrcDC  = ::CreateCompatibleDC( hDestDC );

   ::SelectObject( hSrcDC, hBitmap );
   ::BitBlt( hDestDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hSrcDC, 0, 0, SRCCOPY );

   ::DeleteDC( hSrcDC );
   surface->ReleaseDC( hDestDC );

   return surface;
}



#ifdef _WIN32_WCE

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// PocketPC code
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// Callbacks for DecompressImageIndirect()
//
//////////////////////////////////////////////////////////////////////////////

static DWORD CALLBACK LoadImage( char* buffer, DWORD dwBufferMax, LPARAM lparam )
{
   // We already have locked or memory mapped the image, simply return it's size.
   return dwBufferMax;
}



static void CALLBACK LoadImageProgress( IImageRender* pRender, BOOL bComplete, LPARAM lparam )
{
   // We don't care about the load progress...
}



//////////////////////////////////////////////////////////////////////////////
//
// DecompressImage() - Invoke imgdecmp.dll to decompress the image
//
//////////////////////////////////////////////////////////////////////////////

static Surface* DecompressImage( DisplayDevice* display, const uint8_t* pBegin, const uint8_t* pEnd )
{
   // This will be filled with a handle to the image
   HBITMAP hBitmap = 0;    

   // Prepare the decompression process
   // Here I use 24 bits output because if I select 16, imgdecmp.dll will
   // create a 555 DIB instead of a 565 (darn).
   DecompressImageInfo dii;

   dii.dwSize                = sizeof(dii);       // Size of this structure
   dii.pbBuffer              = (BYTE*)pBegin;     // Pointer to the buffer to use for data
   dii.dwBufferMax           = pEnd - pBegin;     // Size of the buffer
   dii.dwBufferCurrent       = 0;                 // The amount of data which is current in the buffer
   dii.phBM                  = &hBitmap;          // Pointer to the bitmap returned (can be NULL)
   dii.ppImageRender         = 0;                 // Pointer to an IImageRender object (can be NULL)
   dii.iBitDepth             = 24;                // Bit depth of the output image
   dii.lParam                = 0;                 // User parameter for callback functions
   dii.hdc                   = 0;                 // HDC to use for retrieving palettes
   dii.iScale                = 100;               // Scale factor (1 - 100)
   dii.iMaxWidth             = 8192;              // Maximum width of the output image
   dii.iMaxHeight            = 8192;              // Maxumum height of the output image
   dii.pfnGetData            = LoadImage;         // Callback function to get image data
   dii.pfnImageProgress      = LoadImageProgress; // Callback function to notify caller of progress decoding the image
   dii.crTransparentOverride = -1;                // If this color is not (UINT)-1, it will override the
                                                  // transparent color in the image with this color. (GIF ONLY)

   // Process and decompress the image data
   HRESULT hr = ::DecompressImageIndirect( &dii );
   if (FAILED(hr)) return 0;

   Surface* surface = CreateSurface( display, hBitmap );
   ::DeleteObject( hBitmap );

   return surface;
}



#else

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Desktop PC code
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// DecompressImage() - Invoke OleLoadPicture() to decompress the image
//
//////////////////////////////////////////////////////////////////////////////

static Surface* DecompressImage( DisplayDevice* display, const uint8_t* pBegin, const uint8_t* pEnd )
{
    // Copy the file to global memory so we create a stream on it
    int size = pEnd - pBegin;
    HGLOBAL hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, size );
    memcpy( ::GlobalLock( hGlobal ), pBegin, size );
    ::GlobalUnlock( hGlobal );
    
    // Get an IStream on the memory
    IStream* stream = 0;
    HRESULT hr = ::CreateStreamOnHGlobal( hGlobal, TRUE, &stream );
    if (FAILED(hr)) return 0;
    
    // Do the actual decompression
    IPicture* picture = 0;
    hr = ::OleLoadPicture( stream, size, FALSE, IID_IPicture, (void**)&picture );
    
    stream->Release();
    if (FAILED(hr)) return 0;
    
    // Find image size
    OLE_XSIZE_HIMETRIC hmWidth, hmHeight;
    picture->get_Width( &hmWidth );
    picture->get_Height( &hmHeight );
    
    HDC hDisplayDC = ::GetDC(0);
    int width  = ::MulDiv( hmWidth,  GetDeviceCaps( hDisplayDC, LOGPIXELSX ), HIMETRIC_INCH );
    int height = ::MulDiv( hmHeight, GetDeviceCaps( hDisplayDC, LOGPIXELSY ), HIMETRIC_INCH );
    ::ReleaseDC( 0, hDisplayDC );
    
    // Render the image on the surface
    Surface* surface = display->CreateSurface( width, height );
    HDC hdc = surface->GetDC( true );
    
    picture->Render( hdc, 0, 0, width, height, 0, hmHeight, hmWidth, -hmHeight, 0 );
    surface->ReleaseDC( hdc );
    
    // Cleanup
    picture->Release();
    
    return surface;
}



#endif



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Common code
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//
// LoadImage()
//
//////////////////////////////////////////////////////////////////////////////

Surface* LoadImage( DisplayDevice* display, const TCHAR* filename )
{
    Internal::MemoryFile file( filename );
    if (!file.IsOpen())
        return 0;
    
    return DecompressImage( display, file.begin(), file.end() );
}



Surface* LoadImage( DisplayDevice* display, unsigned resourceID, const TCHAR* resclass, HINSTANCE hModule )
{
    // If the module instance is not specified, get the application's instance
    if (!hModule) hModule = ::GetModuleHandle(0);
    
    // If the class is RT_BITMAP, imgdecmp.dll won't be able to decompress it as the file header
    // is not present (BITMAPFILEHEADER). In any case, it's better to use LoadBitmap()
    if (resclass == RT_BITMAP)
    {
        HBITMAP hBitmap = ::LoadBitmap( hModule, (LPCTSTR)resourceID );
        if (!hBitmap)
            return 0;
        
        Surface* surface = CreateSurface( display, hBitmap );
        ::DeleteObject( hBitmap );
        
        return surface;
    }
    else
    {
        // Lock the resource in memory
        HRSRC hResource = ::FindResource( hModule, (LPCTSTR)resourceID, resclass );
        if (!hResource)
        {
            // Try RT_BITMAP
            if (resclass != RT_BITMAP)
                return LoadImage( display, resourceID, RT_BITMAP, hModule );
            
            return 0;
        }
        
        DWORD    size    = ::SizeofResource( hModule, hResource );
        HGLOBAL  hMemory = ::LoadResource( hModule, hResource );
        uint8_t* data    = (uint8_t*)::LockResource( hMemory );
        
        // Decompress the image
        return DecompressImage( display, data, data + size );
    }
}



Surface* LoadImage( DisplayDevice* display, const uint8_t* pBegin, const uint8_t* pEnd )
{
    return DecompressImage( display, pBegin, pEnd );
}



bool SaveImage( const Surface* const_surface, const TCHAR* filename )
{
    Surface* surface = const_cast<Surface*>( const_surface );
    
    if (!surface || !filename)
        return false;

    TCHAR pathname[MAX_PATH];
    PocketPC::GetFullPathName( filename, pathname );
    _tcscat( pathname, _T(".bmp") );   

    // Lock the surface
    Surface::LockInfo lockinfo;
    if (!surface->Lock( lockinfo ))
        return false;

    // Lines must be dword aligned
    assert( ((surface->GetWidth() * sizeof(Pixel))&3)==0 );
    
    BITMAPINFOHEADER info;
    info.biSize          = sizeof(BITMAPINFOHEADER);
    info.biWidth         = surface->GetWidth();
    info.biHeight        = surface->GetHeight();
    info.biPlanes        = 1;
    info.biBitCount      = 16;
    info.biCompression   = BI_BITFIELDS;
    info.biSizeImage     = info.biWidth * info.biHeight;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed       = 0;
    info.biClrImportant  = 0; 
    
    uint32_t colors[3] = { RED_MASK, GREEN_MASK, BLUE_MASK };
    
    
    BITMAPFILEHEADER header;
    header.bfType      = 0x4d42;
    header.bfSize      = sizeof(BITMAPFILEHEADER) + info.biSize + sizeof(colors) + info.biSizeImage;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits   = sizeof(BITMAPFILEHEADER) + info.biSize + sizeof(colors);
    
    
    // Create the file
    HANDLE hFile = ::CreateFile( pathname, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        surface->Unlock( true );
        return false;
    }
    
    DWORD temp;
    
    // Write the header + bitmap info
    ::WriteFile( hFile, &header, sizeof(header), &temp, 0 );
    ::WriteFile( hFile, &info, sizeof(info), &temp, 0 );
    ::WriteFile( hFile, colors, sizeof(colors), &temp, 0 );
    
    // Write the image (must flip image vertically)
    const Pixel* pixels = lockinfo.pixels;
    pixels = (Pixel*)((uint8_t*)pixels + lockinfo.pitch * (surface->GetHeight()-1));
    
    for (int h = surface->GetHeight(); h; --h)
    {
        ::WriteFile( hFile, pixels, surface->GetWidth() * sizeof(Pixel), &temp, 0 );
        pixels = (Pixel*)((uint8_t*)pixels - lockinfo.pitch);
    }
    
    surface->Unlock( true );
    ::CloseHandle( hFile );
    
    return true;
}

bool SaveImage( const Surface* const_surface, char*& pRawBitmapData, size_t& ulSize )
{	
	Surface* surface = const_cast<Surface*>( const_surface );

	if (!surface)
		return false;

	// Lock the surface
	Surface::LockInfo lockinfo;
	if (!surface->Lock( lockinfo ))
		return false;

	//added 0-3 pixels to have width a multiple of 4
	int width = surface->GetWidth() + ((surface->GetWidth() * sizeof(Pixel)) & 3);

	BITMAPINFOHEADER info;
	info.biSize          = sizeof(BITMAPINFOHEADER);
	info.biWidth         = width;
	info.biHeight        = surface->GetHeight();
	info.biPlanes        = 1;
	info.biBitCount      = 16;
	info.biCompression   = BI_BITFIELDS;
	info.biSizeImage     = info.biWidth * info.biHeight;
	info.biXPelsPerMeter = 0;
	info.biYPelsPerMeter = 0;
	info.biClrUsed       = 0;
	info.biClrImportant  = 0; 

	uint32_t colors[3] = { RED_MASK, GREEN_MASK, BLUE_MASK };


	BITMAPFILEHEADER header;
	header.bfType      = 0x4d42;
	header.bfSize      = sizeof(BITMAPFILEHEADER) + info.biSize + sizeof(colors) + info.biSizeImage;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits   = sizeof(BITMAPFILEHEADER) + info.biSize + sizeof(colors);


	ulSize = sizeof(header) + sizeof(info) + sizeof(colors) + 
		(surface->GetHeight() + 1) * width * sizeof(Pixel);

	pRawBitmapData = new char[ulSize];
	if(NULL == pRawBitmapData)
	{
		//not enough memory ??
		surface->Unlock( true );
		ulSize = 0;
		return false;
	}
	
	char *pRawDataCursor = pRawBitmapData;
	memcpy(pRawDataCursor, &header, sizeof(header));
	pRawDataCursor += sizeof(header);
	memcpy(pRawDataCursor, &info, sizeof(info));
	pRawDataCursor += sizeof(info);
	memcpy(pRawDataCursor, colors, sizeof(colors));
	pRawDataCursor += sizeof(colors);
	
	// Write the image (must flip image vertically)
	const Pixel* pixels = lockinfo.pixels;
	pixels = (Pixel*)((uint8_t*)pixels + lockinfo.pitch * (surface->GetHeight()-1));

	for (int h = surface->GetHeight(); h; --h)
	{
		memcpy(pRawDataCursor, pixels, surface->GetWidth() * sizeof(Pixel));
		pRawDataCursor += surface->GetWidth() * sizeof(Pixel);

		//added eventually extra black pixels
		if(width > surface->GetWidth())
		{
			memset(pRawDataCursor, 0, (width - surface->GetWidth() + 1) * sizeof(Pixel));
			pRawDataCursor += (width - surface->GetWidth() + 1) * sizeof(Pixel);
		}

		pixels = (Pixel*)((uint8_t*)pixels - lockinfo.pitch);
	}

	surface->Unlock( true );
	return true;
}



} // end of namespace Frog

