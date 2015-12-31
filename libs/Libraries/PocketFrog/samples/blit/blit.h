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

#ifndef POCKETFROG_SAMPLE_BLIT_H
#define POCKETFROG_SAMPLE_BLIT_H

#include <PocketFrog.h>

using namespace Frog;



//////////////////////////////////////////////////////////////////////////////
//
// BlitSample
//
//////////////////////////////////////////////////////////////////////////////

class BlitSample : public Game
{
public:

    // Construction
    BlitSample();

    // Framework callbacks
    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    virtual void StylusDown( Point stylus );
    virtual void StylusUp( Point stylus );
    virtual void StylusMove( Point stylus );


private:

		// Data
    Surface* m_images[4];       // Moving images
    Surface* m_background;      // Background image
    Surface* m_numbers;         // Numbers to display FPS
    Rect     m_position[4];     // Positions of images
    int      m_dragImage;       // # of image being dragged (-1 if none)
    Point    m_dragOffset;      // Offset of stylus to image being dragged

    int      m_FPSCounter;      // FPS counter
    uint32_t m_FPSTicks[16];    // Ticks for last 16 frames

};



#endif
