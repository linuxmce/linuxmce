//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

#ifndef POCKETHAL_VISUALC_H
#define POCKETHAL_VISUALC_H

#define PHAL_COMPILER_VISUALC

#pragma warning(disable: 4097 4100 4511 4512 4514)



//////////////////////////////////////////////////////////////////////////////
//
// CPU Detection
//
//////////////////////////////////////////////////////////////////////////////

#if defined(ARM)
#define PHAL_CPU_ARM

#elif defined(_MIPS_)
#define PHAL_CPU_MIPS

#elif defined(SHx)
#define PHAL_CPU_SH3

#elif defined(_M_IX86) || defined(_X86_) || defined(__WINS__) // WINS = Symbian emulator
#define PHAL_CPU_X86

#else
#error "Unknown CPU"

#endif



#endif
