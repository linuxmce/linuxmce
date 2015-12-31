//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Basic compiler independent types.
*/

#ifndef POCKETHAL_STDINT_H
#define POCKETHAL_STDINT_H

#include <PocketHAL/config.h>


namespace PHAL
{

#if defined(PHAL_COMPILER_VISUALC)

typedef __int8           int8_t;
typedef __int16          int16_t;
typedef __int32          int32_t;
typedef __int64          int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

#else

typedef signed char        int8_t;      ///< Compiler independent signed 8-bit short integer
typedef short              int16_t;     ///< Compiler independent signed 16-bit short integer
typedef int                int32_t;     ///< Compiler independent signed 32-bit short integer
typedef long long          int64_t;     ///< Compiler independent signed 64-bit short integer
typedef unsigned char      uint8_t;     ///< Compiler independent unsigned 8-bit short integer
typedef unsigned short     uint16_t;    ///< Compiler independent unsigned 16-bit short integer
typedef unsigned int       uint32_t;    ///< Compiler independent unsigned 32-bit short integer
typedef unsigned long long uint64_t;    ///< Compiler independent unsigned 64-bit short integer

#endif



} // end of namespace PHAL



#endif
