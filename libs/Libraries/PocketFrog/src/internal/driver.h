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

#ifndef POCKETFROG_INTERNAL_DRIVER_H
#define POCKETFROG_INTERNAL_DRIVER_H

#include "../defs.h"

namespace Frog {
namespace Internal {


    
//////////////////////////////////////////////////////////////////////////////
//
// Driver interface
//
//////////////////////////////////////////////////////////////////////////////

class Driver
{
public:

    // Construction / destruction
    virtual ~Driver() {}

    // Initialization
    virtual bool Initialize( HWND hwnd ) = 0;
    
    // Shutdown
    virtual void Shutdown() = 0;
    
    // Application lost focus
    virtual void Suspend() = 0;
    
    // Application regained focus
    virtual void Resume() = 0;
};



} // end of namespace Internal
} // end of namespace Frog




#endif
