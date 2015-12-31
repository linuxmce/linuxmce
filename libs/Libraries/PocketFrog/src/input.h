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

#ifndef POCKETFROG_INPUT_H
#define POCKETFROG_INPUT_H

#include "defs.h"

namespace Frog {

namespace Internal
{
    class InputDriver;
}



//////////////////////////////////////////////////////////////////////////////
//
// InputDevice
//
//////////////////////////////////////////////////////////////////////////////

// List of available keys (through GAPI)
struct KeyList
{
    int16_t vkUp;       // D-Pad <up>
    int16_t vkDown;     // D-Pad <down>
    int16_t vkLeft;     // D-Pad <left>
    int16_t vkRight;    // D-Pad <right>
    int16_t vkA;        // Button <A>
    int16_t vkB;        // Button <B>
    int16_t vkC;        // Button <C>
    int16_t vkStart;    // Button <Start>
    int16_t vkAux1;     // Button <Aux1> (0 if not available)
    int16_t vkAux2;     // Button <Aux2> (0 if not available)
};

class InputDevice : noncopyable
{
public:

    // Destruction
    ~InputDevice();

    // Convert a button key from device to logical orientation
    void DeviceToLogical( int& button ) const;

    // Convert a coordinate from device to logical coordinates
    void DeviceToLogical( Point& P ) const;

    // Retrieve the list of button scan codes
    const KeyList& GetKeyList() const;


private:

    Internal::InputDriver* m_driver;
    Orientation            m_orientation;
    
    // You can't create a input directly, use PocketPC::CreateInput()
    friend class PocketPC;
    InputDevice( Internal::InputDriver* driver, Orientation orientation );
};



} // end of namespace Frog



#endif
