//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

#ifndef POCKETHAL_SELECT_COMPILER_H
#define POCKETHAL_SELECT_COMPILER_H



#if defined(__GNUC__) || defined(__GCC32__)
// GCC
#define PHAL_COMPILER_CONFIG <PocketHAL/config/compiler/gcc.h>

#elif defined(__CW32__)
// Code Warrior
#define PHAL_COMPILER_CONFIG <PocketHAL/config/compiler/codewarrior.h>

#elif defined(_MSC_VER) || defined(__VC32__)
// Visual C
#define PHAL_COMPILER_CONFIG <PocketHAL/config/compiler/visualc.h>

#else

#error "Unknown compiler"

#endif



#endif
