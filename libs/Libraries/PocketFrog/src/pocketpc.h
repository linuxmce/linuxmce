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

#ifndef POCKETFROG_POCKETPC_H
#define POCKETFROG_POCKETPC_H

#include "defs.h"

namespace Frog {

class DisplayDevice;
class InputDevice;

void PocketPC_Cleanup();

namespace Internal
{
    class GAPILibrary;
}



//////////////////////////////////////////////////////////////////////////////
//
// PocketPC
//
//////////////////////////////////////////////////////////////////////////////

class PocketPC
{
public:

    // Known Pocket PC models
    enum Model
    {
        MODEL_NOT_DETECTED,
        MODEL_UNKNOWN,

        // Pocket PCs
        MODEL_CASIO_E115,
        MODEL_CASIO_E125,
        MODEL_CASIO_EM500,
        MODEL_JORNADA_540,
        MODEL_JORNADA_545,
        MODEL_JORNADA_568,
        MODEL_IPAQ_3600,
        MODEL_IPAQ_3800,
        MODEL_IPAQ_3900,
        MODEL_SYMBOL_PPT_2700,

        // Handheld PCs
        MODEL_JORNADA_680,
        MODEL_JORNADA_720
    };


    // Create the display device object
    static DisplayDevice* CreateDisplay( HWND hwnd, Orientation orientation = ORIENTATION_NORTH, unsigned int desktopZoom = 0 );
    
    // Create the input device object
    static InputDevice* CreateInput( HWND hwnd, Orientation orientation = ORIENTATION_NORTH );

    // Return the current tick count (in ms)
    static uint32_t GetTickCount();

    // Retrieve the full path and file name for the specified file
    // The specified buffer "pathname" must be at least MAX_PATH in length.
    // The value returned is the length of the string copied to "pathname",
    // not including the terminating NULL character.
    static size_t GetFullPathName( LPCTSTR filename, TCHAR pathname[MAX_PATH] );

    // Detect the Pocket PC model
    static Model GetModel();

    // Suspend and resume PocketFrog
    static void Suspend();
    static void Resume(); 


private:

    // There is no point in creating a PocketPC object, there is only static methods!
    PocketPC();
};



//////////////////////////////////////////////////////////////////////////////
// inline implementation
//////////////////////////////////////////////////////////////////////////////

// timeGetTime() has better precision then GetTickCount() on PC
inline uint32_t PocketPC::GetTickCount()
{
#if defined(_WIN32_WCE)
    return ::GetTickCount();
#else
    return ::timeGetTime();
#endif
}


} // end of namespace Frog



#endif
