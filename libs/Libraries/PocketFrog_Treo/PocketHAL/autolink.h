//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2003 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    This file contains compiler directives to automatically link the
    appropriate PocketHAL library.
    @remark This is only supported for Visual C++ compilers.
*/

#ifndef POCKETHAL_AUTOLINK_H
#define POCKETHAL_AUTOLINK_H

#include <PocketHAL/config.h>


#if defined(PHAL_PLATFORM_WCE)

    // Windows CE
    #if defined(PHAL_CPU_ARM)
        #pragma comment( lib, "PocketHAL_ARM.lib" )
    #elif defined(PHAL_CPU_MIPS)
        #pragma comment( lib, "PocketHAL_MIPS.lib" )
    #elif defined(PHAL_CPU_SH3)
        #pragma comment( lib, "PocketHAL_SH3.lib" )
    #elif defined(PHAL_CPU_X86)
        #pragma comment( lib, "PocketHAL_EMU.lib" )
    #else
        #error Unknown processor
    #endif


#elif defined(PHAL_PLATFORM_WIN32)

    // Windows for PC
    #pragma comment( lib, "PocketHAL_x86.lib" )

#endif



#endif
