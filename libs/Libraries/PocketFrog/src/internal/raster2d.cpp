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
// Memory hit is about 8K / rasterizer
//
//////////////////////////////////////////////////////////////////////////////

#include "raster2d.h"
#include "algo2d.h"
#include "../rasterizer.h"
#include <list>

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// Pixel Functions
//
//////////////////////////////////////////////////////////////////////////////

struct PixelSourceCopy : std::binary_function<Pixel,Pixel,Pixel>
{
    Pixel operator()( Pixel src, Pixel dest ) const
    {
        return src;
    }
};



struct PixelAlphaBlend : std::binary_function<Pixel,Pixel,Pixel>
{
    PixelAlphaBlend( int alpha ) : m_alpha(alpha) {}

    Pixel operator()( Pixel src, Pixel dest ) const
    {
        Pixel RB1 = dest & (RED_MASK | BLUE_MASK);
        Pixel G1  = dest & (GREEN_MASK );
        Pixel RB2 = src & (RED_MASK | BLUE_MASK);
        Pixel G2  = src & (GREEN_MASK );

        Pixel RB = RB1 + (((RB2-RB1) * (m_alpha>>3)) >> 5);
        Pixel G  = G1 + (((G2-G1)*(m_alpha>>2))>>6);
        RB &= (RED_MASK | BLUE_MASK);
        G  &= (GREEN_MASK);

        return RB | G;
    }

private:
    int m_alpha;
};



struct GenericShader : std::binary_function<Pixel,Pixel,Pixel>
{
    GenericShader( PixelShader* shader ) : m_shader(shader) {}

    Pixel operator()( Pixel src, Pixel dest ) const
    {
        return (*m_shader)( src, dest );
    }

private:
    PixelShader* m_shader;
};




//////////////////////////////////////////////////////////////////////////////
//
// 2D Rasterizer Interface
//
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//
// Default rasterizer: dest = src
//
//////////////////////////////////////////////////////////////////////////////

void Raster2D::Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect )
{
    assert( destRect.GetWidth() == srcRect.GetWidth() );
    assert( destRect.GetHeight() == srcRect.GetHeight() );
    
    //todo: optimize
    const Pixel* pSrc  = source.GetPixels( srcRect.left, srcRect.top );
    Pixel*       pDest = buffer.GetPixels( destRect.left, destRect.top );
    
    int nbCopy = srcRect.GetWidth() * 2;
    
    for ( int h = srcRect.GetHeight(); h; --h )
    {
        memcpy( pDest, pSrc, nbCopy );
        
        pSrc  = (Pixel*)((uint8_t*)pSrc + source.GetPitch());
        pDest = (Pixel*)((uint8_t*)pDest + buffer.GetPitch());
    }
}



void Raster2D::Clear( GraphicBuffer& buffer, Pixel color )
{
    // Check if we can use memset() to clear the surface
    if ( (color >> 8) == (color & 0xFF) )
    {
        memset( buffer.GetPixels(), color, buffer.GetHeight() * buffer.GetPitch() );
    }
    else
    {
        // Can we use the optimized version?
        if (!(buffer.GetWidth() & 7))
        {
            // Surface is a multiple of 8, use loop unrolling
            int cc = (color << 16) | color;
            unsigned* pDest = (unsigned*)buffer.GetPixels();
            
            for ( int y = buffer.GetHeight(); y; --y )
            {
                unsigned* pLine = pDest;
                
                for ( int x = buffer.GetWidth() >> 3; x; --x )
                {
                    *pLine++ = cc;
                    *pLine++ = cc;
                    *pLine++ = cc;
                    *pLine++ = cc;
                }
                
                pDest = (unsigned*)((uint8_t*)pDest + buffer.GetPitch());
            }
        }
        else
        {
            // Surface is not a multiple of 8, use template
            Algo2D::Clear( PixelSourceCopy(), buffer, color );
        }
    }
}



void Raster2D::BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask )
    { Algo2D::BlitMask( PixelSourceCopy(), buffer, destRect, source, srcRect, colorMask ); }

void Raster2D::BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source )
    { Algo2D::BlitTransform( PixelSourceCopy(), buffer, destRect, transform, source ); }

void Raster2D::BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask )
    { Algo2D::BlitTransformMask( PixelSourceCopy(), buffer, destRect, transform, source, colorMask ); }

void Raster2D::Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color )
    { Algo2D::Circle( PixelSourceCopy(), buffer, center, radius, color ); }

void Raster2D::Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper )
    { Algo2D::Circle( PixelSourceCopy(), buffer, center, radius, color, clipper ); }

void Raster2D::FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color )
    { Algo2D::FillRect( PixelSourceCopy(), buffer, rect, color ); }

void Raster2D::HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color )
    { Algo2D::HLine( PixelSourceCopy(), buffer, P, length, color ); }

void Raster2D::Line( GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color )
    { Algo2D::Line( PixelSourceCopy(), buffer, A, B, color ); }

void Raster2D::SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color )
    { Algo2D::SetPixel( PixelSourceCopy(), buffer, P, color ); }

void Raster2D::VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color )
    { Algo2D::VLine( PixelSourceCopy(), buffer, P, length, color ); }



//////////////////////////////////////////////////////////////////////////////
//
// Alpha blending rasterizer: dest = src * alpha + (1-alpha) * dest
//
//////////////////////////////////////////////////////////////////////////////

void AlphaRaster2D::Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect )
    { Algo2D::Blit( PixelAlphaBlend(m_alpha), buffer, destRect, source, srcRect ); }

void AlphaRaster2D::BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask )
    { Algo2D::BlitMask( PixelAlphaBlend(m_alpha), buffer, destRect, source, srcRect, colorMask ); }

void AlphaRaster2D::BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source )
    { Algo2D::BlitTransform( PixelAlphaBlend(m_alpha), buffer, destRect, transform, source ); }

void AlphaRaster2D::BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask )
    { Algo2D::BlitTransformMask( PixelAlphaBlend(m_alpha), buffer, destRect, transform, source, colorMask ); }

void AlphaRaster2D::Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color )
    { Algo2D::Circle( PixelAlphaBlend(m_alpha), buffer, center, radius, color ); }

void AlphaRaster2D::Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper )
    { Algo2D::Circle( PixelAlphaBlend(m_alpha), buffer, center, radius, color, clipper ); }

void AlphaRaster2D::Clear( GraphicBuffer& buffer, Pixel color )
    { Algo2D::Clear( PixelAlphaBlend(m_alpha), buffer, color ); }

void AlphaRaster2D::FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color )
    { Algo2D::FillRect( PixelAlphaBlend(m_alpha), buffer, rect, color ); }

void AlphaRaster2D::HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color )
    { Algo2D::HLine( PixelAlphaBlend(m_alpha), buffer, P, length, color ); }

void AlphaRaster2D::Line( GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color )
    { Algo2D::Line( PixelAlphaBlend(m_alpha), buffer, A, B, color ); }

void AlphaRaster2D::SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color )
    { Algo2D::SetPixel( PixelAlphaBlend(m_alpha), buffer, P, color ); }

void AlphaRaster2D::VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color )
    { Algo2D::VLine( PixelAlphaBlend(m_alpha), buffer, P, length, color ); }



//////////////////////////////////////////////////////////////////////////////
//
// GenericRaster2D: dest = (custom shader)
//
//////////////////////////////////////////////////////////////////////////////

void GenericRaster2D::Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect )
    { Algo2D::Blit( GenericShader(m_shader), buffer, destRect, source, srcRect ); }

void GenericRaster2D::BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask )
    { Algo2D::BlitMask( GenericShader(m_shader), buffer, destRect, source, srcRect, colorMask ); }

void GenericRaster2D::BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source )
    { Algo2D::BlitTransform( GenericShader(m_shader), buffer, destRect, transform, source ); }

void GenericRaster2D::BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask )
    { Algo2D::BlitTransformMask( GenericShader(m_shader), buffer, destRect, transform, source, colorMask ); }

void GenericRaster2D::Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color )
    { Algo2D::Circle( GenericShader(m_shader), buffer, center, radius, color ); }

void GenericRaster2D::Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper )
    { Algo2D::Circle( GenericShader(m_shader), buffer, center, radius, color, clipper ); }

void GenericRaster2D::Clear( GraphicBuffer& buffer, Pixel color )
    { Algo2D::Clear( GenericShader(m_shader), buffer, color ); }

void GenericRaster2D::FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color )
    { Algo2D::FillRect( GenericShader(m_shader), buffer, rect, color ); }

void GenericRaster2D::HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color )
    { Algo2D::HLine( GenericShader(m_shader), buffer, P, length, color ); }

void GenericRaster2D::Line( GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color )
    { Algo2D::Line( GenericShader(m_shader), buffer, A, B, color ); }

void GenericRaster2D::SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color )
    { Algo2D::SetPixel( GenericShader(m_shader), buffer, P, color ); }

void GenericRaster2D::VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color )
    { Algo2D::VLine( GenericShader(m_shader), buffer, P, length, color ); }



} // end of namespace Internal
} // end of namespace Frog
