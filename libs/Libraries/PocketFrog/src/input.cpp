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

#include "input.h"
#include "internal/GAPILibrary.h"
#include <algorithm>

namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// InputDevice
//
//////////////////////////////////////////////////////////////////////////////

InputDevice::InputDevice( Internal::InputDriver* driver, Orientation orientation )
:   m_driver( driver ),
    m_orientation( orientation )
{
    // If orientation is unspecified, use NORTH
    if (m_orientation == ORIENTATION_UNKNOWN)
        m_orientation = ORIENTATION_NORTH;
}



InputDevice::~InputDevice()
{
    m_driver->Shutdown();
}



void InputDevice::DeviceToLogical( int& button ) const
{
#if defined(_WIN32_WCE)

    const KeyList& keys = m_driver->GetKeyList();

    switch (m_orientation)
    {
    case ORIENTATION_WEST:
        if (button == keys.vkUp) button = keys.vkRight;
        else if (button == keys.vkLeft) button = keys.vkUp;
        else if (button == keys.vkDown) button = keys.vkLeft;
        else if (button == keys.vkRight) button = keys.vkDown;
        break;
        
    case ORIENTATION_EAST:
        if (button == keys.vkUp) button = keys.vkLeft;
        else if (button == keys.vkLeft) button = keys.vkDown;
        else if (button == keys.vkDown) button = keys.vkRight;
        else if (button == keys.vkRight) button = keys.vkUp;
        break;
        
    case ORIENTATION_SOUTH:
        if (button == keys.vkUp) button = keys.vkDown;
        else if (button == keys.vkLeft) button = keys.vkRight;
        else if (button == keys.vkDown) button = keys.vkUp;
        else if (button == keys.vkRight) button = keys.vkLeft;
        break;
    }

#endif
}



//fixme: don't use hard coded values for width and height
void InputDevice::DeviceToLogical( Point& P ) const
{
    switch (m_orientation)
    {
    case ORIENTATION_WEST:
        std::swap( P.x, P.y );
        P.x = 319 - P.x;
        break;
        
    case ORIENTATION_EAST:
        std::swap( P.x, P.y );
        P.y = 239 - P.y;
        break;
        
    case ORIENTATION_SOUTH:
        P.x = 239 - P.x;
        P.y = 319 - P.y;
        break;
    }
}



const KeyList& InputDevice::GetKeyList() const
{
    return m_driver->GetKeyList();
}




} // end of namespace Frog
