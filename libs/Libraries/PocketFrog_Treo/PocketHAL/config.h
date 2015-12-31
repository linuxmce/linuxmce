//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    This file is used to automatically configure PocketHAL for your compiler
    and target platform.
*/

#ifndef POCKETHAL_CONFIG_H
#define POCKETHAL_CONFIG_H


#if !defined(PHAL_COMPILER_CONFIG)
#include <PocketHAL/config/select_compiler.h>
#endif
#if defined(PHAL_COMPILER_CONFIG)
#include PHAL_COMPILER_CONFIG
#endif


#if !defined(PHAL_PLATFORM_CONFIG)
#include <PocketHAL/config/select_platform.h>
#endif
#if defined(PHAL_PLATFORM_CONFIG)
#include PHAL_PLATFORM_CONFIG
#endif



#endif
