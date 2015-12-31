//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Fast memory copy functions.
*/

#ifndef POCKETHAL_FASTCOPY_H
#define POCKETHAL_FASTCOPY_H

#include <PocketHAL/defs.h>



namespace PHAL
{

//////////////////////////////////////////////////////////////////////////////
//
/// Quickly copy a block of memory.
///
/// This is not intended as a replacement for memcpy(). FastCopy() should only
/// be used for big block of memory (>= 32 bytes). memcpy() is often an intrinsic
/// and can be optimized away by the compiler depending on the context. This is
/// not the case with FastCopy().
//.
//////////////////////////////////////////////////////////////////////////////

void FastCopy( void* destination, const void* source, unsigned count );




//////////////////////////////////////////////////////////////////////////////
//
// Implementation details
//
//////////////////////////////////////////////////////////////////////////////

#if defined(PHAL_CPU_ARM) && defined(PHAL_PLATFORM_WIN32)

    extern "C" void FastCopy_ARM( void* destination, const void* source, unsigned count );

    inline void FastCopy( void* destination, const void* source, unsigned count )
    {
        FastCopy_ARM( destination, source, count );
    }


#elif defined(PHAL_CPU_MIPS)


    extern "C" void FastCopy_MIPS( void* destination, const void* source, unsigned count );

    inline void FastCopy( void* destination, const void* source, unsigned count )
    {
        FastCopy_MIPS( destination, source, count );
    }


#else

    // Default implementation
    inline void FastCopy( void* destination, const void* source, unsigned count )
    {
        memcpy( destination, source, count );
    }

#endif



} // end of namespace PHAL



#endif
