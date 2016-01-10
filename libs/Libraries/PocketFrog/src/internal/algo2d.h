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

#ifndef POCKETFROG_ALGO2D_H
#define POCKETFROG_ALGO2D_H

#include <cassert>
#include <cmath>
#include <algorithm>
#include <functional>
#include "graphicbuffer.h"

namespace Frog {
namespace Internal {
namespace Algo2D {



//////////////////////////////////////////////////////////////////////////////
//
// transform_if : extension to the STL
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _STLPORT_VERSION
#define _STLP_INLINE_LOOP inline
#define _STLP_DEBUG_CHECK(x)
#endif

template <class _InputIter1, class _InputIter2, class _OutputIter, class _Predicate, class _BinaryOperation>
_STLP_INLINE_LOOP _OutputIter 
transform_if( _InputIter1 __first1, _InputIter1 __last1, 
              _InputIter2 __first2,
              _OutputIter __result,
              _Predicate __pred, _BinaryOperation __binary_op )
{
    _STLP_DEBUG_CHECK(__check_range(__first1, __last1))
    for ( ; __first1 != __last1; ++__first1, ++__first2, ++__result)
        if (__pred(*__first1)) *__result = __binary_op(*__first1, *__first2);
    return __result;
}



//////////////////////////////////////////////////////////////////////////////
//
// 2D Raster Algorithms
//
//////////////////////////////////////////////////////////////////////////////

template<typename PixelFunction> inline
void Blit( PixelFunction& F, GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect )
{
    assert( srcRect.GetWidth() == destRect.GetWidth() );
    assert( srcRect.GetHeight() == destRect.GetHeight() );

    const Pixel* pSrc  = source.GetPixels( srcRect.left, srcRect.top );
    Pixel*       pDest = buffer.GetPixels( destRect.left, destRect.top );
    int          width = srcRect.GetWidth();

    for ( int h = srcRect.GetHeight(); h; --h )
    {
        std::transform( pSrc, pSrc + width, pDest, pDest, F );
        
        pSrc  = (Pixel*)((uint8_t*)pSrc + source.GetPitch());
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void BlitMask( PixelFunction& F, GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel mask )
{
    const Pixel* pSrc  = source.GetPixels( srcRect.left, srcRect.top );
    Pixel*       pDest = buffer.GetPixels( destRect.left, destRect.top );
    int          width = srcRect.GetWidth();

    for ( int h = srcRect.GetHeight(); h; --h )
    {
        transform_if( pSrc, pSrc + width, pDest, pDest, std::bind2nd( std::not_equal_to<Pixel>(), mask ), F );
        
        pSrc  = (Pixel*)((uint8_t*)pSrc + source.GetPitch());
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void BlitTransform( PixelFunction& F, GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source )
{
    unsigned y1 = (destRect.top << 6) + (1<<5);
    unsigned y2 = (destRect.bottom << 6) + (1<<5);
    unsigned x1 = (destRect.left << 6) + (1<<5);
    unsigned x2 = (destRect.right << 6) + (1<<5);

    unsigned u0 = x1 * transform[0][0] + y1 * transform[1][0] + transform[2][0];
    unsigned v0 = x1 * transform[0][1] + y1 * transform[1][1] + transform[2][1];

    unsigned dudx = transform[0][0] << 6;
    unsigned dvdx = transform[0][1] << 6;
    unsigned dudy = transform[1][0] << 6;
    unsigned dvdy = transform[1][1] << 6;

    for (unsigned dy = y1; dy != y2; dy += (1<<6) )
    {
        Pixel* dest = buffer.GetPixels( x1 >> 6, dy >> 6 );

        unsigned u = u0;
        unsigned v = v0;

        for (unsigned dx = x1; dx != x2; dx += (1<<6) )
        {
            unsigned ix = u >> 16;
            unsigned iy = v >> 16;
            
            if (ix < (unsigned)source.GetWidth() && iy < (unsigned)source.GetHeight() )
            {
                *dest = F( *source.GetPixels(ix,iy), *dest );
            }

            u += dudx;
            v += dvdx;
            ++dest;
        }

        u0 += dudy;
        v0 += dvdy;
    }
}



template<typename PixelFunction> inline
void BlitTransformMask( PixelFunction& F, GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel mask )
{
    unsigned y1 = (destRect.top << 6) + (1<<5);
    unsigned y2 = (destRect.bottom << 6) + (1<<5);
    unsigned x1 = (destRect.left << 6) + (1<<5);
    unsigned x2 = (destRect.right << 6) + (1<<5);

    unsigned u0 = x1 * transform[0][0] + y1 * transform[1][0] + transform[2][0];
    unsigned v0 = x1 * transform[0][1] + y1 * transform[1][1] + transform[2][1];

    unsigned dudx = transform[0][0] << 6;
    unsigned dvdx = transform[0][1] << 6;
    unsigned dudy = transform[1][0] << 6;
    unsigned dvdy = transform[1][1] << 6;

    for (unsigned dy = y1; dy != y2; dy += (1<<6) )
    {
        Pixel* dest = buffer.GetPixels( x1 >> 6, dy >> 6 );

        unsigned u = u0;
        unsigned v = v0;

        for (unsigned dx = x1; dx != x2; dx += (1<<6) )
        {
            unsigned ix = u >> 16;
            unsigned iy = v >> 16;
            
            if (ix < (unsigned)source.GetWidth() && iy < (unsigned)source.GetHeight() )
            {
                const Pixel& src = *source.GetPixels(ix,iy);

                if (src != mask)
                {
                    *dest = F( src, *dest );
                }
            }

            u += dudx;
            v += dvdx;
            ++dest;
        }

        u0 += dudy;
        v0 += dvdy;
    }
}



template<typename PixelFunction> inline
void Clear( PixelFunction& F, GraphicBuffer& buffer, Pixel color )
{
    Pixel* pDest  = buffer.GetPixels();
    int    width  = buffer.GetWidth();
    
    for ( int y = buffer.GetHeight(); y; --y )
    {
        std::transform( pDest, pDest + width, pDest, std::bind1st( F, color ) );
        
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void FillRect( PixelFunction& F, GraphicBuffer& buffer, const Rect& r, Pixel color )
{
    Pixel* pDest = buffer.GetPixels( r.left, r.top );
    int    width = r.GetWidth();

    for ( int y = r.top; y != r.bottom; ++y )
    {
        std::transform( pDest, pDest + width, pDest, std::bind1st( F, color ) );

        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void HLine( PixelFunction& F, GraphicBuffer& buffer, const Point& P, int length, Pixel color )
{
   Pixel* pDest = buffer.GetPixels( P.x, P.y );
   
   std::transform( pDest, pDest + length, pDest, std::bind1st( F, color ) );
}



template<typename PixelFunction> inline
void Line( PixelFunction& F, GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color )
{
   // Determine increments
   int xincr = 1;
   int dx    = B.x - A.x;
   if (dx < 0)
   {
      dx    = -dx;
      xincr = -xincr;
   }

   int yincr = buffer.GetPitch() / 2;
   int dy    = B.y - A.y;
   if (dy < 0)
   {
      dy    = -dy;
      yincr = -yincr;
   }
   
   int xyincr = xincr + yincr;

   
   // Find the frame buffer address for each point
   Pixel* pA = buffer.GetPixels( A.x, A.y );
   Pixel* pB = buffer.GetPixels( B.x, B.y );

   
   // Draw the line
   if (dx < dy)
   {
      int dpr  = 2 * dx;
      int p    = -dy;
      int dpru = -2 * dy;
      dx = dy >> 1;

      for ( ; dx > 0; --dx )
      {
          *pA = F( color, *pA );
          *pB = F( color, *pB );

         p += dpr;
         if (p > 0)
         {
            p += dpru;
            pA += xyincr;
            pB -= xyincr;
         }
         else
         {
            pA += yincr;
            pB -= yincr;
         }
      }

      *pA = F( color, *pA );
      if (dy & 1)
         *pB = F( color, *pB );
   }
   else
   {
      int dpr  = 2 * dy;
      int p    = -dx;
      int dpru = -2 * dx;
      dy = dx >> 1;

      for ( ; dy > 0; --dy )
      {
          *pA = F( color, *pA );
          *pB = F( color, *pB );

         p += dpr;
         if (p > 0)
         {
            p += dpru;
            pA += xyincr;
            pB -= xyincr;
         }
         else
         {
            pA += xincr;
            pB -= xincr;
         }
      }

      *pA = F( color, *pA );
      if (dx & 1)
         *pB = F( color, *pB );
   }
}



template<typename PixelFunction> inline
void SetPixel( PixelFunction& F, GraphicBuffer& buffer, const Point& P, Pixel color )
{
    Pixel* pDest = buffer.GetPixels( P.x, P.y );

    *pDest = F( color, *pDest );
}



template<typename PixelFunction> inline
void SetPixelClip( PixelFunction& F, GraphicBuffer& buffer, const Point& P, Pixel color, const Rect& clipper )
{
    if (P.x >= clipper.left && P.y >= clipper.top && P.x < clipper.right && P.y < clipper.bottom)
    {
         SetPixel( F, buffer, P, color );
    }
}



// Function modified 5/9/2003 by Frank W. Zammett
// Changed the line
// while (length-- > 0)
// to
// while (length-- > -1)
// This fixed the problem of rectangles missing the bottom right-hand corner
// pixel, and also vertical lines being one pixel too short.
template<typename PixelFunction> inline
void VLine( PixelFunction& F, GraphicBuffer& buffer, const Point& P, int length, Pixel color )
{
   Pixel* pDest = buffer.GetPixels( P.x, P.y );

   while (length-- > -1)
   {
      *pDest = F( color, *pDest );
      pDest = (Pixel*)((BYTE*)pDest + buffer.GetPitch());
   }
}



template<typename PixelFunction> inline
void Circle( PixelFunction& F, GraphicBuffer& buffer, const Point& P, int radius, Pixel color )
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    for ( ; x < y; ++x)
    {
        SetPixel( F, buffer, Point( P.x - x, P.y - y ), color );
        SetPixel( F, buffer, Point( P.x + x, P.y - y ), color );
        SetPixel( F, buffer, Point( P.x - x, P.y + y ), color );
        SetPixel( F, buffer, Point( P.x + x, P.y + y ), color );
        SetPixel( F, buffer, Point( P.x - y, P.y - x ), color );
        SetPixel( F, buffer, Point( P.x + y, P.y - x ), color );
        SetPixel( F, buffer, Point( P.x - y, P.y + x ), color );
        SetPixel( F, buffer, Point( P.x + y, P.y + x ), color );
        
        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x-y) + 10;
            --y;
        }
    }
    
    if (x==y)
    {
        SetPixel( F, buffer, Point( P.x - x, P.y - y ), color );
        SetPixel( F, buffer, Point( P.x + x, P.y - y ), color );
        SetPixel( F, buffer, Point( P.x - x, P.y + y ), color );
        SetPixel( F, buffer, Point( P.x + x, P.y + y ), color );
    }
}


//fixme: it doesn't make sense to have two circle algorithms
template<typename PixelFunction> inline
void Circle( PixelFunction& F, GraphicBuffer& buffer, const Point& P, int radius, Pixel color, const Rect& clipper )
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    for ( ; x < y; ++x)
    {
        SetPixelClip( F, buffer, Point( P.x - x, P.y - y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x + x, P.y - y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x - x, P.y + y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x + x, P.y + y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x - y, P.y - x ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x + y, P.y - x ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x - y, P.y + x ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x + y, P.y + x ), color, clipper );
        
        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x-y) + 10;
            --y;
        }
    }
    
    if (x==y)
    {
        SetPixelClip( F, buffer, Point( P.x - x, P.y - y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x + x, P.y - y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x - x, P.y + y ), color, clipper );
        SetPixelClip( F, buffer, Point( P.x + x, P.y + y ), color, clipper );
    }
}



} // end of namespace Algo2D
} // end of namespace Internal
} // end of namespace Frog



#endif
