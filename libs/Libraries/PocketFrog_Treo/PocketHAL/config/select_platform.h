//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

#ifndef POCKETHAL_SELECT_PLATFORM_H
#define POCKETHAL_SELECT_PLATFORM_H



#if defined(__SYMBIAN32__)
// Symbian 32 bits
#define PHAL_PLATFORM_CONFIG <PocketHAL/config/platform/symbian.h>

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// Windows 32 bits
#define PHAL_PLATFORM_CONFIG <PocketHAL/config/platform/win32.h>

#else

#error "Unknown platform"

#endif



#endif
