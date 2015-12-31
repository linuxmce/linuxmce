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

#ifndef POCKETFROG_SAMPLE_ROTATE_H
#define POCKETFROG_SAMPLE_ROTATE_H

#include <PocketFrog.h>

using namespace Frog;



//////////////////////////////////////////////////////////////////////////////
//
// RotateSample
//
//////////////////////////////////////////////////////////////////////////////

class RotateSample : public Game
{
public:

    // Construction
    RotateSample();

    // Framework callbacks
    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    virtual void StylusDown( Point stylus );
    virtual void StylusUp( Point stylus );
    virtual void StylusMove( Point stylus );


private:

    // Data
    Surface* m_image;           // Frog image
    float    m_angle;           // Current rotation angle

    Surface* m_numbers;         // Numbers to display FPS
    int      m_FPSCounter;      // FPS counter
    uint32_t m_FPSTicks[16];    // Ticks for last 16 frames
};



#endif
