//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Surface API.
*/

#ifndef POCKETHAL_SURFACE_H
#define POCKETHAL_SURFACE_H

#include <PocketHAL/stdint.h>
#include <PocketHAL/utility.h>



namespace PHAL
{


//////////////////////////////////////////////////////////////////////////////
//
// Surface
//
//////////////////////////////////////////////////////////////////////////////

/// A Surface is a two-dimensional (2D) array of pixels. Each pixel is represented
/// as a 16-bit unsigned integer (uint16_t). The format of a pixel is R5G6B5 meaning
/// that the first 5 bits hold the red intensity (0-31), the next 6 bits hold the
/// green intensity (0-63) and the last 5 bits hold the blue intensity (0-31).
class Surface : NonCopyable
{
public:

    /// Static constructor: create a surface and allocate the needed memory.
    /// The surface will be padded to when needed to ensure maximum performances.
    /// Returns NULL if there is not enough memory.
    static Surface* Create( unsigned width, unsigned height );

    /// Static constructor: create a surface from an existing memory location.
    /// All the surface parameters must be specified. The specified memory won't be freed on destruction.
    /// Returns NULL if there is not enough memory.
    static Surface* Create( uint16_t* pixels, unsigned width, unsigned height, unsigned pitch );
    
    /// Destroy the surface, freeing any allocated memory
    virtual ~Surface();

    // Accessors
    unsigned        GetWidth() const                 { return m_width; }        ///< Return the width (in pixels) of the surface.
    unsigned        GetHeight() const                { return m_height; }       ///< Return the height (in pixels) of the surface.
    unsigned        GetPitch() const                 { return m_pitch; }        ///< Return the offset (in bytes) between pixel rows.
    uint16_t*       GetPixels()                      { return m_pixels; }       ///< Retrieve a pointer to the pixels.
    const uint16_t* GetPixels() const                { return m_pixels; }       ///< Retrieve a pointer to the pixels.
    uint16_t*       GetPixels( int x, int y )        { return (uint16_t*)((char*)(m_pixels + x) + y * m_pitch); }   ///< Retrieve a pointer to the pixels.
    const uint16_t* GetPixels( int x, int y ) const  { return (uint16_t*)((char*)(m_pixels + x) + y * m_pitch); }   ///< Retrieve a pointer to the pixels.



protected:

    /// Constructor - Used by derived classes only.
    Surface( unsigned width, unsigned height, unsigned pitch, uint16_t* pixels, bool bOwnMemory );

    unsigned  m_width;      ///< Width of the surface in pixels.
    unsigned  m_height;     ///< Height of the surface in pixels.
    unsigned  m_pitch;      ///< Offset between pixel rows (in bytes).
    uint16_t* m_pixels;     ///< Pointer to the actual pixels.
    bool      m_bOwnMemory; ///< Whether or not this object should free the pixel memory when deleted.
};



} // end of namespace PHAL



#endif
