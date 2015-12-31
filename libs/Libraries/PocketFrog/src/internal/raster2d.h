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

#include "../defs.h"

namespace Frog {

struct PixelShader;

namespace Internal {

class GraphicBuffer;



//////////////////////////////////////////////////////////////////////////////
//
// 2D Rasterizer Interface
//
//////////////////////////////////////////////////////////////////////////////

struct I2DRasterizer
{
    virtual ~I2DRasterizer() {}
    virtual void Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect ) = 0;
    virtual void BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask ) = 0;
    virtual void BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source ) = 0;
    virtual void BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask ) = 0;
    virtual void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color ) = 0;
    virtual void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper ) = 0;
    virtual void Clear( GraphicBuffer& buffer, Pixel color ) = 0;
    virtual void FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color ) = 0;
    virtual void HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color ) = 0;
    virtual void Line( GraphicBuffer& buffer, const Point& P, const Point& Q, Pixel color ) = 0;
    virtual void SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color ) = 0;
    virtual void VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color ) = 0;
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
    void Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect );
    void BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask );
    void BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source );
    void BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask );
    void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color );
    void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper );
    void Clear( GraphicBuffer& buffer, Pixel color );
    void FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color );
    void HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color );
    void Line( GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color );
    void SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color );
    void VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color );
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
    void Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect );
    void BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask );
    void BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source );
    void BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask );
    void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color );
    void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper );
    void Clear( GraphicBuffer& buffer, Pixel color );
    void FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color );
    void HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color );
    void Line( GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color );
    void SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color );
    void VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color );


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
    void Blit( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect );
    void BlitMask( GraphicBuffer& buffer, const Rect& destRect, const GraphicBuffer& source, const Rect& srcRect, Pixel colorMask );
    void BlitTransform( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source );
    void BlitTransformMask( GraphicBuffer& buffer, const Rect& destRect, const unsigned transform[3][2], const GraphicBuffer& source, Pixel colorMask );
    void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color );
    void Circle( GraphicBuffer& buffer, const Point& center, int radius, Pixel color, const Rect& clipper );
    void Clear( GraphicBuffer& buffer, Pixel color );
    void FillRect( GraphicBuffer& buffer, const Rect& rect, Pixel color );
    void HLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color );
    void Line( GraphicBuffer& buffer, const Point& A, const Point& B, Pixel color );
    void SetPixel( GraphicBuffer& buffer, const Point& P, Pixel color );
    void VLine( GraphicBuffer& buffer, const Point& P, int length, Pixel color );


private:

    PixelShader* m_shader;  // Pixel shader
};



} // end of namespace Internal
} // end of namespace Frog



#endif
