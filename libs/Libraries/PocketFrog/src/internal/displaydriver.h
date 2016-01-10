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

#ifndef POCKETFROG_INTERNAL_DISPLAYDRIVER_H
#define POCKETFROG_INTERNAL_DISPLAYDRIVER_H

#include "driver.h"

namespace Frog {
namespace Internal {


    
//////////////////////////////////////////////////////////////////////////////
//
// DisplayDriver
//
//////////////////////////////////////////////////////////////////////////////

class DisplayDriver : public Driver
{
public:
    
    // Scene management
    virtual Pixel* BeginScene() = 0;
    virtual void   EndScene( const Rect& rect ) = 0;

    // Accessors
    virtual int         GetWidth() const = 0;
    virtual int         GetHeight() const = 0;
    virtual int         GetPitch() const = 0;
    virtual int         GetPixelDepth() const = 0;
    virtual Orientation GetOrientation() const = 0;
    virtual bool        IsBuffered() const = 0;
};



} // end of namespace Internal
} // end of namespace Frog



#endif
