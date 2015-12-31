//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

#ifndef POCKETHAL_WIN32_H
#define POCKETHAL_WIN32_H

#define PHAL_PLATFORM_WIN32

#undef min
#undef max
#undef NOMINMAX
#define NOMINMAX


#if defined(_WIN32_WCE)

    #if (_WIN32_WCE < 300)
    #error Windows CE versions prior to 3.00 are not supported.
    #endif

    #define PHAL_PLATFORM_WCE

    #if defined(_WIN32_WCE_EMULATION)
    #define PHAL_EMULATOR
    #endif

    // Pocket PC
    #if defined(WIN32_PLATFORM_PSPC)
    #define PHAL_PLATFORM_PPC

    // Handheld PC (Windows CE 3.0)
    #elif defined(WIN32_PLATFORM_HPC2000)
    #define PHAL_PLATFORM_HPC

    // Smartphone
    #elif defined(WIN32_PLATFORM_WFSP)
    #define PHAL_PLATFORM_SMARTPHONE

    #endif

#endif


#endif
