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
//
// Blitting for monochrome displays (4 bpp)
//
//////////////////////////////////////////////////////////////////////////////

#include "graphicbuffer.h"

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// Blit4()
//
// Use the nearest intensity
//
//////////////////////////////////////////////////////////////////////////////

// todo:Another equation I've found in Graphic Gems 2: 
//   luminosity = .263 red + .655 green + .082 blue


// This function takes two color Pixel and return a single byte (2 monochrome pixels of 4 bits each)
//
// The NTSC formula for luminosity is: luminosity = .299 red + .587 green + .114 blue
// Thing is, I doubt that the LCD screens used on PocketPC use the same equation.
// Another thing is, we don't want to use floating point to compute the luminosity..
//
// Roughly, green is twice as important as red, which in turn is twice as important as blue.
//
// We get:  7 * luminosity = 2 * red + 4 * green + 1 * blue
// Or    :      luminosity = (2 * red + 4 * green + 1 * blue) / 7
//
// It would be nice to use a shift to the the division, but then we need 8 times the luminosity.
// A simple solution is to double the blue component, making it more visible.
//
// The equation becomes: luminosity = (4 * green + 2 * red + 2 * blue) / 8
//

static inline BYTE ConvertToMonochrome4( Pixel c1, Pixel c2 )
{
    c1 = ((c1 >> RED_SHIFT) + ((c1 & GREEN_MASK) >> GREEN_SHIFT) + (c1 & BLUE_MASK)) >> 3;
    c2 = ((c2 >> RED_SHIFT) + ((c2 & GREEN_MASK) >> GREEN_SHIFT) + (c2 & BLUE_MASK)) >> 3;
    
    // All monochrome displays I've heard of have the kfDirectInverted flag.
    // This make sense for a B&W LCD display, so I invert the result.
    return ~( (c1 << 4) | c2 );
}



//todo : use LUT
void Blit4( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    int nbRow    = pGAPIBuffer->GetHeight();
    int nbColumn = pGAPIBuffer->GetWidth() >> 1;  // 2 pixels/bytes
    int pitch    = pGAPIBuffer->GetPitch();
    
    BYTE* pDest       = (BYTE*)pGAPIBuffer->GetPixels();
    const Pixel* pSrc = pBackBuffer->GetPixels();
    
    for ( int row = nbRow; row; --row )
    {
        BYTE* pRow = pDest;
        for ( int column = nbColumn ; column; --column )
        {
            Pixel c1 = *pSrc++;
            Pixel c2 = *pSrc++;
            
            *pRow++ = ConvertToMonochrome4( c1, c2 );
        }
        
        pDest += pitch;
    }
}



//////////////////////////////////////////////////////////////////////////////
//
// Blit4DitherNice()
//
// Todo: implement error diffusion dithering
//
//////////////////////////////////////////////////////////////////////////////

void Blit4DitherNice( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    Blit4( pGAPIBuffer, pBackBuffer );
}



//////////////////////////////////////////////////////////////////////////////
//
// Blit4DitherFast()
//
// Todo: implement ordered dithering
//
//////////////////////////////////////////////////////////////////////////////

void Blit4DitherFast( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    Blit4( pGAPIBuffer, pBackBuffer );
}


        
} // end of namespace Internal
} // end of namespace Frog
