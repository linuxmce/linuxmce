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
// Blitting for 16 bpp displays (555 and 565)
//
//////////////////////////////////////////////////////////////////////////////

#include "graphicbuffer.h"
#include <cassert>

namespace Frog {
namespace Internal {



void Blit16( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    assert( pGAPIBuffer->GetPitch() != pBackBuffer->GetPitch() );
    
    Blit( *pBackBuffer, *pGAPIBuffer );
}



void Blit16Fast( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer )
{
    assert( pGAPIBuffer->GetPitch() == pBackBuffer->GetPitch() );
    
    memcpy( pGAPIBuffer->GetPixels(), pBackBuffer->GetPixels(),
            pBackBuffer->GetHeight() * pBackBuffer->GetPitch() );
}



} // end of namespace Internal
} // end of namespace Frog
