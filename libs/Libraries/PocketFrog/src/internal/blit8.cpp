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
// Blitting for palettized displays (332 palette)
//
//////////////////////////////////////////////////////////////////////////////
//
// help: I made this code run as fast as possible on my desktop PC. I need
// someone to actually optimize them for real devices (Jornada 525)
//
//////////////////////////////////////////////////////////////////////////////

#include "graphicbuffer.h"

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// Look up tables (LUT) used by the different blit implementations
//
//////////////////////////////////////////////////////////////////////////////

static BYTE* l_closest;     // Closest color LUT
static int   l_2to8bpp[4];  // Convert a 2 bits intensity to it's corresponding 8 bits value
static int   l_3to8bpp[8];  // Convert a 3 bits intensity to it's corresponding 8 bits value
static int   l_5to8bpp[32]; // Convert a 5 bits intensity to it's corresponding 8 bits value
static int   l_6to8bpp[64]; // Convert a 6 bits intensity to it's corresponding 8 bits value
static int   l_error3[256]; // Find error from intensity (3 bits channel)
static int   l_error2[256]; // Find error from intensity (2 bits channel)



//////////////////////////////////////////////////////////////////////////////
//
// Initialize the 8-bits LUTs
//
//////////////////////////////////////////////////////////////////////////////

void InitializeLUTs8()
{
    // Initialize the bit depth conversion LUTs
    int i;
    
    for (i = 0; i < 4; ++i)
        l_2to8bpp[i] = (i << 6) | (i << 4) | (i << 2) | (i);
    
    for (i = 0; i < 8; ++i)
        l_3to8bpp[i] = (i << 5) | (i << 2) | (i >> 1);
    
    for (i = 0; i < 32; ++i)
        l_5to8bpp[i] = (i << 3) | (i >> 2);
    
    for (i = 0; i < 64; ++i)
        l_6to8bpp[i] = (i << 2) | (i >> 4);
    
    
    
    // Initialize the "closest color" LUT.
    //todo: This is a little slow...
    l_closest = new BYTE[65536];
    
    for (int c = 0; c < 65536; ++c)
    {
        int r = l_5to8bpp[ c >> RED_SHIFT ];
        int g = l_6to8bpp[ (c & GREEN_MASK) >> GREEN_SHIFT ];
        int b = l_5to8bpp[ c & BLUE_MASK ];
       
        int closest = 0;
        int bestDistance = 256*256*256;
        
        for (int i = 0; i < 256; ++i)
        {
            int tr = l_3to8bpp[ i >> 5 ];
            int tg = l_3to8bpp[ (i >> 2) & 7 ];
            int tb = l_2to8bpp[ i & 3 ];
            
            int distance = (r-tr)*(r-tr) + (g-tg)*(g-tg) + (b-tb)*(b-tb);
            
            if (distance < bestDistance)
            {
                closest = i;
                bestDistance = distance;
            }
        }
        
        l_closest[c] = closest;
    }
    
    
    // Initialize the error LUTs used for error-diffusion dithering. The error
    // is divided by two to save computations while dithering. We loose the
    // less signigicant bit of error. The quality is good enough and we get a
    // small speed boost.
    for (int color = 0; color < 256; ++color)
    {
        int bits        = color >> 5;
        int actual      = l_3to8bpp[ bits ];
        l_error3[color] = (actual - color) / 2;
        
        bits            = color >> 6;
        actual          = l_2to8bpp[ bits ];
        l_error2[color] = (actual - color) / 2;
    }
}




//////////////////////////////////////////////////////////////////////////////
//
// Blit8()
//
// Use the nearest color
//
// Desktop performance       : 1.21 ms
// Blit sample on Jornada 525: 20 fps
//
//////////////////////////////////////////////////////////////////////////////

void Blit8( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    int nbRow    = pGAPIBuffer->GetHeight();
    int nbColumn = pGAPIBuffer->GetWidth() >> 2;
    int pitch    = pGAPIBuffer->GetPitch();
    
    BYTE*        pDest = (BYTE*)pGAPIBuffer->GetPixels();
    const Pixel* pSrc  = pBackBuffer->GetPixels();
    
    for ( int row = nbRow; row; --row )
    {
        BYTE* pRow = pDest;
        for ( int column = nbColumn; column; --column )
        {
            *pRow++ = l_closest[ *pSrc++ ];
            *pRow++ = l_closest[ *pSrc++ ];
            *pRow++ = l_closest[ *pSrc++ ];
            *pRow++ = l_closest[ *pSrc++ ];
        }
        
        pDest += pitch;
    }
}





//////////////////////////////////////////////////////////////////////////////
//
// Blit8DitherNice
//
// Dithering is done using error diffusion (1/2 right, 1/2 down)
//
// Desktop performance       : 2.84 ms
// Blit sample on Jornada 525: 10 fps
//
// Note: Using a saturation table didn't help
//
//////////////////////////////////////////////////////////////////////////////

static inline BYTE DitherNice( Pixel c, int& errorR, int& errorG, int& errorB )
{
    // Find out the target color
    int targetR = l_5to8bpp[ c >> RED_SHIFT ];
    int targetG = l_6to8bpp[ (c & GREEN_MASK) >> GREEN_SHIFT ];
    int targetB = l_5to8bpp[ c & BLUE_MASK ];
    
    // Add the error component
    targetR -= errorR;
    targetG -= errorG;
    targetB -= errorB;
    
    // Check saturation
    if ( (targetR|targetG|targetB) & ~0xFF )
    {
        if (targetR & ~0xFF) targetR = ~(targetR >> 24);
        if (targetG & ~0xFF) targetG = ~(targetG >> 24);
        if (targetB & ~0xFF) targetB = ~(targetB >> 24);
    }
    
    // Find error (actually, 1/2 error)
    errorR = l_error3[ targetR ];
    errorG = l_error3[ targetG ];
    errorB = l_error2[ targetB ];
    
    //todo: use 3 LUTs to build pixel?
    BYTE result = (targetR & 0xE0) | ((targetG >> 3) & 0x1C) | (targetB >> 6);
    
    return result;
}



void Blit8DitherNice( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    int nbRow    = pGAPIBuffer->GetHeight();
    int nbColumn = pGAPIBuffer->GetWidth();
    int pitch    = pGAPIBuffer->GetPitch();
    
    BYTE*        pDest = (BYTE*)pGAPIBuffer->GetPixels();
    const Pixel* pSrc  = pBackBuffer->GetPixels();
    
    int rowErrors[321 * 3];
    memset( rowErrors, 0, sizeof(rowErrors) );
    
    for ( int row = nbRow; row; --row )
    {
        BYTE* pRow    = pDest;
        int*  pErrors = rowErrors;
        
        for ( int column = nbColumn ; column; --column )
        {
            // pErrors[0-2] hold the error adjustment for this pixel
            *pRow++ = DitherNice( *pSrc++, pErrors[0], pErrors[1], pErrors[2] );
            
            // DitherNice() will return 1/2 error in pErrors[0-2] that will be
            // used for next row. Copy this 1/2 error to the next pixel.
            pErrors[3] += pErrors[0];
            pErrors[4] += pErrors[1];
            pErrors[5] += pErrors[2];
            
            // Move to the next pixel
            pErrors += 3;
        }
        
        pDest += pitch;
    }
}




//////////////////////////////////////////////////////////////////////////////
//
// Blit8DitherFast()
//
// Desktop performance       : 2.43 ms
// Blit sample on Jornada 525: 14 fps
//
// This is not much faster then Blit8DitherNice() and the image seems a little
// too bright (and blueish). More work is needed here. Anyone?
//
//////////////////////////////////////////////////////////////////////////////

static inline BYTE DitherFast( Pixel c, int column, int row )
{
    static const int ditherMatrix[4][4] = 
    { 
        { -5,  3, -3,  5 },
        { 11,-13, 13,-11 },
        { -1,  7, -7,  1 },
        { 15, -9,  9,-15 }
    };
    
    int r = (c & RED_MASK)   >> (RED_SHIFT - (8 - RED_BITS));
    int g = (c & GREEN_MASK) >> (GREEN_SHIFT - (8 - GREEN_BITS));
    int b = (c & BLUE_MASK)  << (8 - BLUE_BITS);
    
    int offset = ditherMatrix[row & 3][column & 3];
    
    r += offset;
    g += offset;
    b += offset * 2;
    
    // Check saturation
    if ( (r|g|b) & ~0xFF )
    {
        if (r & ~0xFF) r = ~(r >> 24);
        if (g & ~0xFF) g = ~(g >> 24);
        if (b & ~0xFF) b = ~(b >> 24);
    }
    
    BYTE result = (r & 0xE0) | ((g >> 3) & 0x1C) | (b >> 6);
    
    return result;
}



void Blit8DitherFast( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    int nbRow    = pGAPIBuffer->GetHeight();
    int nbColumn = pGAPIBuffer->GetWidth();
    int pitch    = pGAPIBuffer->GetPitch();
    
    BYTE*        pDest = (BYTE*)pGAPIBuffer->GetPixels();
    const Pixel* pSrc  = pBackBuffer->GetPixels();
    
    for ( int row = nbRow; row; --row )
    {
        BYTE* pRow = pDest;
        for ( int column = nbColumn ; column; --column )
        {
            *pRow++ = DitherFast( *pSrc++, column, row );
        }
        
        pDest += pitch;
    }
}

        
        
        
} // end of namespace Internal
} // end of namespace Frog
