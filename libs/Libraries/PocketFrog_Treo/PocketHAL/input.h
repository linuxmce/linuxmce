//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Input API
*/

#ifndef POCKETHAL_INPUT_H
#define POCKETHAL_INPUT_H

#include "stdint.h"
#include "defs.h"



namespace PHAL
{

//////////////////////////////////////////////////////////////////////////////
//
// KeyList
//
/// The KeyList structure is used to retrieve the key codes associated with
/// the hardware keys. It is meant to be used with the GetKeyList() function.
//
//////////////////////////////////////////////////////////////////////////////

struct KeyList
{
    int16_t vkUp;       ///< [UP] on the directional pad.
    int16_t vkDown;     ///< [DOWN] on the directional pad.
    int16_t vkLeft;     ///< [LEFT] on the directional pad.
    int16_t vkRight;    ///< [RIGHT] on the directional pad.

    int16_t vkStart;    ///< Button [Start]
    int16_t vkA;        ///< Button [A]
    int16_t vkB;        ///< Button [B]
    int16_t vkC;        ///< Button [C]

    int16_t vkAux1;     ///< Button [Aux1] (0 if not available)
    int16_t vkAux2;     ///< Button [Aux2] (0 if not available)
    int16_t vkAux3;     ///< Button [Aux3] (0 if not available)
    int16_t vkAux4;     ///< Button [Aux4] (0 if not available)

    int16_t vkAux5;     ///< Button [Aux5] (0 if not available)
    int16_t vkAux6;     ///< Button [Aux6] (0 if not available)
    int16_t vkAux7;     ///< Button [Aux7] (0 if not available)
    int16_t vkAux8;     ///< Button [Aux8] (0 if not available)
};



//////////////////////////////////////////////////////////////////////////////
//
// GetKeyList
//
/// Return the key list for the specified orientation.
//
//////////////////////////////////////////////////////////////////////////////

bool GetKeyList( KeyList* keylist, Orientation orientation = ORIENTATION_NORMAL );



} // end of namespace PHAL



#endif
