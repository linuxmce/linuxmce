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

#ifndef POCKETFROG_IMAGE_H
#define POCKETFROG_IMAGE_H

#include "defs.h"

namespace Frog {

class DisplayDevice;
class Surface;




//////////////////////////////////////////////////////////////////////////////
//
// LoadImage()
//
// Load an image from a file or a resource.
//
// The PocketPC and the desktop PC versions of this function are implemented
// differently. On PocketPC, we use the "imgdecmp.dll" decompression library
// to load different file formats without having to deal with legal issues
// such as GIF decompression.
//
// On the desktop, we use the OleLoadPicture() function to load image files.
// This give us access to a similar set of image formats.
//
// The following formats are supported:
//
//    PocketPC: .BMP, .GIF, .JPG, .PNG and .XBM
//    Desktop:  .BMP, .GIF, .JPG, .WMF and .ICO
//
//////////////////////////////////////////////////////////////////////////////

// Load an image from a file
Surface* LoadImage( DisplayDevice* display, const TCHAR* filename );

// Load an image from a resource
Surface* LoadImage( DisplayDevice* display, unsigned resourceID, const TCHAR* resclass = _T("GIF"), HINSTANCE hModule = 0 );

// Load an image from a memory buffer
Surface* LoadImage( DisplayDevice* display, const uint8_t* pBegin, const uint8_t* pEnd );



//////////////////////////////////////////////////////////////////////////////
//
// SaveImage()
//
// Save the specified surface to a BMP file
//
//////////////////////////////////////////////////////////////////////////////

// Save an image to a .BMP file
bool SaveImage( const Surface* surface, const TCHAR* filename );



} // end of namespace Frog



#endif
