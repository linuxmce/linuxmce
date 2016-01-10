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
// This file has been adapted from a sample written by "Burning Sheep".
//
//////////////////////////////////////////////////////////////////////////////

#ifndef POCKETFROG_SAMPLE_PLASMA_H
#define POCKETFROG_SAMPLE_PLASMA_H

#include <PocketFrog.h>

#if defined(FROG_HPC)
#error This sample doesn't work on HPC for now.
#endif


using namespace Frog;



//////////////////////////////////////////////////////////////////////////////
//
// Plasma
//
//////////////////////////////////////////////////////////////////////////////

class Plasma : public Game  
{
public:
    
    // Construction
	Plasma();

    // Framework callbacks
	virtual bool GameInit();
	virtual void GameLoop();
	virtual void GameEnd();

    
private:
	
    // Data
    Pixel       m_palette[256];

	byte*       plasma1;
	byte*       plasma2;
	byte*       m_background;
};



#endif
