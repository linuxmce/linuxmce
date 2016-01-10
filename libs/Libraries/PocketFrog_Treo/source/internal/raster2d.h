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

#ifndef POCKETFROG_RASTER2D_H
#define POCKETFROG_RASTER2D_H

#include <PocketHAL/surface.h>
#include "../defs.h"

namespace Frog {

struct PixelShader;

namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// 2D Rasterizer Interface
//
//////////////////////////////////////////////////////////////////////////////

struct I2DRasterizer
{
    virtual ~I2DRasterizer() {}
    virtual void Blit( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect ) = 0;
    virtual void BlitMask( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect, Pixel colorMask ) = 0;
    virtual void BlitTransform( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source ) = 0;
    virtual void BlitTransformMask( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source, Pixel colorMask ) = 0;
    virtual void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color ) = 0;
    virtual void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color, const Rect& clipper ) = 0;
    virtual void Clear( PHAL::Surface& buffer, Pixel color ) = 0;
    virtual void FillRect( PHAL::Surface& buffer, const Rect& rect, Pixel color ) = 0;
    virtual void HLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color ) = 0;
    virtual void Line( PHAL::Surface& buffer, const Point& P, const Point& Q, Pixel color ) = 0;
    virtual void SetPixel( PHAL::Surface& buffer, const Point& P, Pixel color ) = 0;
    virtual void VLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color ) = 0;
    //J Elmore 08/18/2004
    virtual void BlitAlpha(  PHAL::Surface& buffer, const Rect& destRect, const  PHAL::Surface& source, const  PHAL::Surface& alphasource, const Rect& srcRect ) = 0;
    
};



//////////////////////////////////////////////////////////////////////////////
//
// Raster2D: Normal 2D Rasterizer
//
//////////////////////////////////////////////////////////////////////////////

class Raster2D : public I2DRasterizer
{
public:

    Raster2D() {}

    // I2DRasterizer
    void Blit( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect );
    void BlitMask( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect, Pixel colorMask );
    void BlitTransform( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source );
    void BlitTransformMask( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source, Pixel colorMask );
    void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color );
    void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color, const Rect& clipper );
    void Clear( PHAL::Surface& buffer, Pixel color );
    void FillRect( PHAL::Surface& buffer, const Rect& rect, Pixel color );
    void HLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color );
    void Line( PHAL::Surface& buffer, const Point& A, const Point& B, Pixel color );
    void SetPixel( PHAL::Surface& buffer, const Point& P, Pixel color );
    void VLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color );
    //J Elmore 08/18/2004
    void BlitAlpha(  PHAL::Surface& buffer, const Rect& destRect, const  PHAL::Surface& source, const  PHAL::Surface& alphasource, const Rect& srcRect );
	    
};



//////////////////////////////////////////////////////////////////////////////
//
// AlphaRaster2D: Alpha Blending Rasterizer
//
//////////////////////////////////////////////////////////////////////////////

class AlphaRaster2D : public I2DRasterizer
{
public:

    AlphaRaster2D( int alpha ) : m_alpha(alpha) {}

    // Set the alpha value
    void SetAlpha( int alpha )  { m_alpha = alpha; }

    // I2DRasterizer
    void Blit( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect );
    void BlitMask( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect, Pixel colorMask );
    void BlitTransform( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source );
    void BlitTransformMask( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source, Pixel colorMask );
    void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color );
    void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color, const Rect& clipper );
    void Clear( PHAL::Surface& buffer, Pixel color );
    void FillRect( PHAL::Surface& buffer, const Rect& rect, Pixel color );
    void HLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color );
    void Line( PHAL::Surface& buffer, const Point& A, const Point& B, Pixel color );
    void SetPixel( PHAL::Surface& buffer, const Point& P, Pixel color );
    void VLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color );
    //J Elmore 08/18/2004
    void BlitAlpha( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const PHAL::Surface& alphasource, const Rect& srcRect );

private:

    int m_alpha;
};



//////////////////////////////////////////////////////////////////////////////
//
// GenericRaster2D: Generic rasterizer that works with PixelShader objects
//
//////////////////////////////////////////////////////////////////////////////

class GenericRaster2D : public I2DRasterizer
{
public:

    GenericRaster2D( PixelShader* shader ) : m_shader(shader) {}

    // Set the pixel shader
    void SetPixelShader( PixelShader* shader ) { m_shader = shader; }
    
    // I2DRasterizer
    void Blit( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect );
    void BlitMask( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const Rect& srcRect, Pixel colorMask );
    void BlitTransform( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source );
    void BlitTransformMask( PHAL::Surface& buffer, const Rect& destRect, const unsigned transform[3][2], const PHAL::Surface& source, Pixel colorMask );
    void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color );
    void Circle( PHAL::Surface& buffer, const Point& center, int radius, Pixel color, const Rect& clipper );
    void Clear( PHAL::Surface& buffer, Pixel color );
    void FillRect( PHAL::Surface& buffer, const Rect& rect, Pixel color );
    void HLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color );
    void Line( PHAL::Surface& buffer, const Point& A, const Point& B, Pixel color );
    void SetPixel( PHAL::Surface& buffer, const Point& P, Pixel color );
    void VLine( PHAL::Surface& buffer, const Point& P, int length, Pixel color );
    //J Elmore 08/18/2004
    void BlitAlpha( PHAL::Surface& buffer, const Rect& destRect, const PHAL::Surface& source, const PHAL::Surface& alphasource, const Rect& srcRect );


private:

    PixelShader* m_shader;  // Pixel shader
};



} // end of namespace Internal
} // end of namespace Frog



#endif
