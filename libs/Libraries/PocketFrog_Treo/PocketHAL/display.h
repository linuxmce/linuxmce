//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Display API
*/

#ifndef POCKETHAL_DISPLAY_H
#define POCKETHAL_DISPLAY_H

#include <PocketHAL/defs.h>
#include <PocketHAL/rect.h>
#include <PocketHAL/utility.h>



namespace PHAL
{

//////////////////////////////////////////////////////////////////////////////
//
// SwapEffect
//
//////////////////////////////////////////////////////////////////////////////

/// The SwapEffect is used to specify how the display is updated from the
/// backbuffer. 
enum SwapEffect
{
    SWAP_NONE,                  ///< Direct access (no buffering).
    SWAP_DISCARD,               ///< We don't care about the content of the backbuffer.
    SWAP_COPY                   ///< We want to keep the backbuffer content.
};



//////////////////////////////////////////////////////////////////////////////
//
// DisplayMode
//
//////////////////////////////////////////////////////////////////////////////

/// The DisplayMode contains information about a display mode.
struct DisplayMode
{
    unsigned    m_width;        ///< Width of display in pixels.
    unsigned    m_height;       ///< Height of display in pixels.
    Orientation m_orientation;  ///< Orientation.
};



//////////////////////////////////////////////////////////////////////////////
//
// DisplayParameters
//
//////////////////////////////////////////////////////////////////////////////

/// DisplayParameters is used to configure how the display will be initialized.
struct DisplayParameters
{
    unsigned    m_width;        ///< Width of backbuffer in pixels.
    unsigned    m_height;       ///< Height of backbuffer in pixels.
    Orientation m_orientation;  ///< Orientation.
    SwapEffect  m_swapEffect;   ///< Swap effect.

    /// Construction
    DisplayParameters( Orientation orientation = ORIENTATION_NORMAL,
                       SwapEffect  swapEffect  = SWAP_DISCARD )
        :   m_width(0),
            m_height(0),
            m_orientation( orientation ),
            m_swapEffect( swapEffect )
        {}
};



//////////////////////////////////////////////////////////////////////////////
//
// Flags for Display::Swap()
//
//////////////////////////////////////////////////////////////////////////////

/// Optional flags used when calling Display::Swap().
enum SwapFlags
{
    SWAP_VSYNC       = 1,   ///< Synchronize swap with VSYNC if possible, otherwise attempt to minimize tearing.
    SWAP_FULL_UPDATE = 2,   ///< Hint that all pixels need to be updated.
};



//////////////////////////////////////////////////////////////////////////////
//
// Display
//
//////////////////////////////////////////////////////////////////////////////

class Surface;

/// The Display class is used to access your device's screen.
/// You can initialize the display using different parameters and enumerate
/// available display modes.
class Display : NonCopyable
{
public:

    //////////////////////////////////////////////////////////////////////////
    // Static interface 
    //////////////////////////////////////////////////////////////////////////

    /// Static constructor: create a display object
    /// The display will be initialized with default parameters.
    /// Returns NULL if the display can't be initialized (not enough memory or invalid parameters).
    static Display* Create( WindowHandle hWindow );

    /// Static constructor: create a display object
    /// The display will be initialized with the specified parameters.
    /// Returns NULL if the display can't be initialized (not enough memory or invalid parameters).
    static Display* Create( WindowHandle hWindow, DisplayParameters& params );

    /// Return the number of display modes available
    static unsigned GetModeCount();

    /// Return a description of display mode 'index'
    static bool EnumMode( unsigned index, DisplayMode* mode );



    //////////////////////////////////////////////////////////////////////////
    // Class interface
    //////////////////////////////////////////////////////////////////////////

    /// Retrieve the display parameters in use
    const DisplayParameters& GetParameters() const;

    /// Destructor
    ~Display();

    /// Begin a scene. Must be called before rendering. This function will fail if your
    /// window doesn't have focus.
    /// @remark GetBackBuffer() will return 0 if BeginScene() fails or is not called first.
    bool BeginScene();

    /// Return the backbuffer.
    /// @remark GetBackBuffer() will return 0 if BeginScene() hasn't been called first.
    Surface* GetBackBuffer();

    /// Set the update viewport.
    void SetViewport( const Rect* viewport );

    /// Update the display with the content of the backbuffer (ie: flip).
    /// @remark Swap() will fail if BeginScene() wasn't called first.
    bool Swap( unsigned flags = 0 );



#ifdef PHAL_PLATFORM_SYMBIAN
    /// Callback for Symbian only. Your application must call this when it's foreground status changes
    void HandleForegroundEvent( TBool aForeground );
#endif


    // Implementation details
public:
    class Impl;

private:
    Display( Impl* );
    Impl* m_impl;
};



//////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////

inline Display* Display::Create( WindowHandle hWindow )
{
    DisplayParameters params;
    return Create( hWindow, params );
}



} // end of namespace PHAL



#endif
