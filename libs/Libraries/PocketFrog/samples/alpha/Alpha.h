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

#ifndef POCKETFROG_SAMPLE_alpha_H
#define POCKETFROG_SAMPLE_alpha_H

#include <PocketFrog.h>

using namespace Frog;


//////////////////////////////////////////////////////////////////////////////
//
// alphaSample
//
//////////////////////////////////////////////////////////////////////////////

class alphaSample : public Game {
public:
	// Construction
  alphaSample();
  // Framework callbacks
  virtual bool GameInit();
  virtual void GameEnd();
  virtual void GameLoop();
  virtual void StylusUp(Point stylus);
private:
  // Data
  Surface* m_picture;  // Our picture
	int fadeValue;			 // The current value for our fade
};

#endif
