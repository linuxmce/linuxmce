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

#ifndef POCKETFROG_RASTERIZER_H
#define POCKETFROG_RASTERIZER_H

#include "defs.h"



namespace Frog {

class Surface;

namespace Internal
{
    struct I2DRasterizer;
    class AlphaRaster2D;
    class GenericRaster2D;
}



//////////////////////////////////////////////////////////////////////////////
//
// PixelShader - Use to create your own raster operations
//
///////////////////////////////////////////////////////////////////////////////

struct PixelShader
{
    // Override this method to do your custom raster operations.
    // src is the source color color specified to the primitive)
    // dest is the content of the graphic buffer (dest color)
    virtual Pixel operator()( Pixel src, Pixel dest ) const = 0;
};



//////////////////////////////////////////////////////////////////////////////
//
// Rasterizer
//
//////////////////////////////////////////////////////////////////////////////

class Rasterizer : noncopyable
{
public:
    
    // Primitives
    void Blit( int destX, int destY, const Surface* source, const Rect* srcRect = 0 );
    void Blit( Point dest, const Surface* source, const Rect* srcRect = 0 );
    void Clear( Pixel color );
    void SetPixel( int x, int y, Pixel color );
		void SetPixel( Point point, Pixel color );
    void DrawCircle( int x, int y, int radius, Pixel color );
		void DrawCircle( Point point, int radius, Pixel color );
    void DrawLine( int x1, int y1, int x2, int y2, Pixel color );
		void DrawLine( Point point1, Point point2, Pixel color );
    void DrawHLine( int x, int y, int length, Pixel color );
		void DrawHLine( Point point, int length, Pixel color );
    void DrawVLine( int x, int y, int length, Pixel color );
		void DrawVLine( Point point, int length, Pixel color );
    void DrawRect( int x1, int y1, int x2, int y2, Pixel color );
		void DrawRect( const Rect& rect, Pixel color );
    void FillRect( int x1, int y1, int x2, int y2, Pixel color );
    void FillRect( const Rect& rect, Pixel color );
		void DrawPoly( Point points[], int pointCount, Pixel color );
		void FillPoly( Point points[], int pointCount, Pixel Color );
		void DrawVGAText( unsigned char* pFont, TCHAR* string, int align, int locX, int locY, Pixel color );

    // Blit with rotation
    void BlitRotated( int destX, int destY, float angle, const Surface* source );

    // Blit with stretching
    void BlitStretch( const Rect& dest, const Surface* source, const Rect* srcRect = 0 );

		// Blit with rotation and stretching (
		void BlitRotatedStretch( int destx, int desty, float angle, const Surface* source, Surface* stretch_surface, const Rect* src_rect);
		void BlitRotatedStretch( Point point, float angle, const Surface* source, Surface* stretch_surface, const Rect* src_rect);

    // Return the pixel at the specified location
    Pixel GetPixel( int x, int y );

    // Return the current clipping rectangle
    const Rect& GetClipping() const { return m_clipper; }

    // Set the blending alpha value (0 - transparent, 255 - opaque)
    void SetBlending( uint8_t alpha );

    // Set the clipping region
    void SetClipping( const Rect& rect );

    // Restore the default shader
    void SetDefaultShader();

    // Set a custom raster operation
    void SetPixelShader( PixelShader* shader );

    // Change the rendering target, returning the old target
    Surface* SetRenderTarget( Surface* target );

   

    
private:

    // Compute the outcode for a point (used for Cohen-Sutherland line clipping)
    int GetOutCode( const Point& p ) const;

    // Transform points and rectangles based on orientation
    void Transform( Point& p ) const;
    void Transform( Rect& r ) const;

		// Only needed by the FillPoly method, so let's make sure it's not externally accessible
		void PolyEdgeCalc( int x1, int y1, int x2, int y2, int* ymin, int* ymax, int window_height, int* edge_start_x, int* edge_end_x );

    // Data
    Orientation                             m_orientation;   // Orientation of target
    Surface*                                m_surface;       // Target
    Rect                                    m_clipper;       // Clipping rectangle
    Internal::I2DRasterizer*                m_raster;        // Current rasterizer
    scoped_ptr<Internal::AlphaRaster2D>     m_alphaRaster;   // Alpha blending rasterizer
    scoped_ptr<Internal::GenericRaster2D>   m_genericRaster; // Generic blending rasterizer
		unsigned char*													vga_font;        // Added 5/9/2003 by Frank W. Zammetti: Pointer to current font used by DrawVGAText


    // You can't create a Rasterizer directly, use DisplayDevice::CreateRasterizer()
    friend class DisplayDevice;
    void Init( Surface* target, Orientation orientation );
    Rasterizer();
};
    


} // end of namespace Frog



#endif
