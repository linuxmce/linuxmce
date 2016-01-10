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

#include "../defs.h"
#include <PocketHAL/surface.h>

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// todo: STL stuff needed until the new shaders are in place
//
//////////////////////////////////////////////////////////////////////////////

template <class _InputIter, class _OutputIter, class _UnaryOperation>
inline _OutputIter 
transform( _InputIter __first, _InputIter __last,
           _OutputIter __result,
           _UnaryOperation __unary_op )
{
    for ( ; __first != __last; ++__first, ++__result)
        *__result = __unary_op( *__first );
    return __result;
}

template <class _InputIter1, class _InputIter2, class _OutputIter, class _BinaryOperation>
inline _OutputIter 
transform( _InputIter1 __first1, _InputIter1 __last1, 
           _InputIter2 __first2,
           _OutputIter __result,
           _BinaryOperation __binary_op )
{
    for ( ; __first1 != __last1; ++__first1, ++__first2, ++__result)
        *__result = __binary_op(*__first1, *__first2);
    return __result;
}


template <class _InputIter1, class _InputIter2, class _OutputIter, class _Predicate, class _BinaryOperation>
inline _OutputIter 
transform_if( _InputIter1 __first1, _InputIter1 __last1, 
              _InputIter2 __first2,
              _OutputIter __result,
              _Predicate __pred, _BinaryOperation __binary_op )
{
    for ( ; __first1 != __last1; ++__first1, ++__first2, ++__result)
        if (__pred(*__first1)) *__result = __binary_op(*__first1, *__first2);
    return __result;
}

template<class _A, class _R>
struct unary_function
{
	typedef _A argument_type;
	typedef _R result_type;
};

template<class _A1, class _A2, class _R>
	struct binary_function {
	typedef _A1 first_argument_type;
	typedef _A2 second_argument_type;
	typedef _R result_type;
	};

template<class _Bfn>
class binder1st
:   public unary_function<typename _Bfn::second_argument_type, typename _Bfn::result_type>
{
public:
	binder1st(const _Bfn& _X, const typename _Bfn::first_argument_type& _Y)
		: op(_X), value(_Y) {}
	result_type operator()(const argument_type& _X) const
		{return (op(value, _X)); }
protected:
	_Bfn op;
	typename _Bfn::first_argument_type value;
};



template<class _Bfn>
	class binder2nd
	: public unary_function<typename _Bfn::first_argument_type,
		typename _Bfn::result_type> {
public:
	binder2nd(const _Bfn& _X,
		const typename _Bfn::second_argument_type& _Y)
		: op(_X), value(_Y) {}
	result_type operator()(const argument_type& _X) const
		{return (op(_X, value)); }
protected:
	_Bfn op;
	typename _Bfn::second_argument_type value;
	};


template<class _Bfn, class _Ty> inline
	binder2nd<_Bfn> bind2nd(const _Bfn& _X, const _Ty& _Y)
		{return (binder2nd<_Bfn>(_X,
			_Bfn::second_argument_type(_Y))); }


template<class _Bfn, class _Ty> inline
	binder1st<_Bfn> bind1st(const _Bfn& _X, const _Ty& _Y)
		{
            return (binder1st<_Bfn>(_X, _Bfn::first_argument_type(_Y)));
        }

template<class _Ty>
	struct not_equal_to : binary_function<_Ty, _Ty, bool> {
	bool operator()(const _Ty& _X, const _Ty& _Y) const
		{return (_X != _Y); }
	};


namespace Algo2D {

//////////////////////////////////////////////////////////////////////////////
//
// 2D Raster Algorithms
//
//////////////////////////////////////////////////////////////////////////////

template<typename PixelFunction> inline
void Blit( PixelFunction& F, PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect )
{

    const Pixel* pSrc  = source.GetPixels( srcRect.left, srcRect.top );
    Pixel*       pDest = buffer.GetPixels( destRect.left, destRect.top );
    int          width = srcRect.GetWidth();

    for ( int h = srcRect.GetHeight(); h; --h )
    {
        transform( pSrc, pSrc + width, pDest, pDest, F );
        
        pSrc  = (Pixel*)((uint8_t*)pSrc + source.GetPitch());
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void BlitMask( PixelFunction& F, PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect, Pixel mask )
{
    const Pixel* pSrc  = source.GetPixels( srcRect.left, srcRect.top );
    Pixel*       pDest = buffer.GetPixels( destRect.left, destRect.top );
    int          width = srcRect.GetWidth();

    for ( int h = srcRect.GetHeight(); h; --h )
    {
        transform_if( pSrc, pSrc + width, pDest, pDest, bind2nd( not_equal_to<Pixel>(), mask ), F );
        
        pSrc  = (Pixel*)((uint8_t*)pSrc + source.GetPitch());
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void BlitTransform( PixelFunction& F, PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source )
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
void BlitTransformMask( PixelFunction& F, PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source, Pixel mask )
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
void Clear( PixelFunction& F, PHAL::Surface& buffer, Pixel color )
{
    Pixel* pDest  = buffer.GetPixels();
    int    width  = buffer.GetWidth();
    
    for ( int y = buffer.GetHeight(); y; --y )
    {

        transform( pDest, pDest + width, pDest, bind1st( F, color ) );
        
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void FillRect( PixelFunction& F, PHAL::Surface& buffer, const Rect& r, Pixel color )
{
    Pixel* pDest = buffer.GetPixels( r.left, r.top );
    int    width = r.GetWidth();

    for ( int y = r.top; y != r.bottom; ++y )
    {
        transform( pDest, pDest + width, pDest, bind1st( F, color ) );

        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



template<typename PixelFunction> inline
void HLine( PixelFunction& F, PHAL::Surface& buffer, const Point& P, int length, Pixel color )
{
   Pixel* pDest = buffer.GetPixels( P.x, P.y );
   
   transform( pDest, pDest + length, pDest, bind1st( F, color ) );
}



template<typename PixelFunction> inline
void Line( PixelFunction& F, PHAL::Surface& buffer, const Point& A, const Point& B, Pixel color )
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
void SetPixel( PixelFunction& F, PHAL::Surface& buffer, const Point& P, Pixel color )
{
    Pixel* pDest = buffer.GetPixels( P.x, P.y );

    *pDest = F( color, *pDest );
}



template<typename PixelFunction> inline
void SetPixelClip( PixelFunction& F, PHAL::Surface& buffer, const Point& P, Pixel color, const Rect& clipper )
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
void VLine( PixelFunction& F, PHAL::Surface& buffer, const Point& P, int length, Pixel color )
{
   Pixel* pDest = buffer.GetPixels( P.x, P.y );

   while (--length > -1)/*while (length-- > -1)*/// fixed by Benjamin Kharitonov (19 June 2005)
   {
      *pDest = F( color, *pDest );
      pDest = (Pixel*)((BYTE*)pDest + buffer.GetPitch());
   }
}



template<typename PixelFunction> inline
void Circle( PixelFunction& F, PHAL::Surface& buffer, const Point& P, int radius, Pixel color )
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
void Circle( PixelFunction& F, PHAL::Surface& buffer, const Point& P, int radius, Pixel color, const Rect& clipper )
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
