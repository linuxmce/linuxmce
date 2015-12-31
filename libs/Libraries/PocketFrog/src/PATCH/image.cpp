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
// 30 September 2005 - John Till - Removed dependency on imgdecmp.dll
//
//////////////////////////////////////////////////////////////////////////////

#include "image.h"
#include "display.h"
#include "surface.h"
#include "pocketpc.h"
#include "internal/memfile.h"
#include <cassert>

#ifndef _WIN32_WCE
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
// Desktop PC code
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
  TCHAR pathname[MAX_PATH];
  PocketPC::GetFullPathName( filename, pathname );
  HBITMAP hBmp = ::SHLoadImageFile( pathname );
  if(!hBmp)
    return 0;
  Surface *surface = CreateSurface(display, hBmp);
  ::DeleteObject(hBmp);
  return surface;
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
        
        HBITMAP hBmp = ::SHLoadImageResource( hModule, resourceID ); 
        if(!hBmp)
          return 0;
        Surface *surface = CreateSurface(display, hBmp); 
        ::DeleteObject( hBmp );
        return surface;
    }
}


Surface* LoadImage( DisplayDevice* display, const uint8_t* pBegin, const uint8_t* pEnd )
{
    return 0; // This breaks the splash screen image loading
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



} // end of namespace Frog
