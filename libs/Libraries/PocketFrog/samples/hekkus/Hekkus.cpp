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
// Hekkus sample by Frank W. Zammetti, 6/10/2003
// This sample was added with PocketFrog v0.6.2 to show (a) how to use the
// new GameSuspend() and GameResume() methods and (b) to show how to hook
// into the Hekkus Sound System my Thomas Maniero (aka Gersen).  This is so
// that PocketFrog can continue to develop separately from Hekkus without
// Gersen needing to worry about putting out a new version of PocketFrogPlus.
// 
// IMPORTANT NOTES:
// In Project Settings, under 
// C/C++... Preprocessor... Additional Include Directories,
// Add the following, tailored properly for your system, 
// to the Additional Include Directories:
// C:\Program Files\Libraries\HekkusSoundSystem
// Then, under Links... Input,
// Add the following, again tailored properly for your system,
// to the Additional Library Path for the build your want to do:
// C:\Program Files\Libraries\HekkusSoundSystem\lib\xp\debug
// You also will need to add libcmtd.lib to the Ignore Libraries section to
// avoid a compiler warning (I'm going to ask Thomas about that)
//////////////////////////////////////////////////////////////////////////////


#include "Hekkus.h"
#include "resource.h"

using namespace Frog;


//////////////////////////////////////////////////////////////////////////////
//
// _Module : This is needed to use ATL Windows
//
//////////////////////////////////////////////////////////////////////////////
CComModule _Module;


//////////////////////////////////////////////////////////////////////////////
//
// HekkusSample
//
//////////////////////////////////////////////////////////////////////////////

// Application constructor, some basic initialization
HekkusSample::HekkusSample() {

	m_config.szWindowName     = _T("Hekkus Sample");
  m_config.orientation      = ORIENTATION_WEST;
  m_config.splashScreenTime = 2000;

}


// Called to initialize your application
bool HekkusSample::GameInit() {

	// Initialize random seed
  srand(PocketPC::GetTickCount());

  DisplayDevice* display = GetDisplay();

  m_images[0]  = LoadImage(display, IDB_TRUMPET);
  m_images[1]  = LoadImage(display, IDB_CHICKEN);
  m_images[2]  = LoadImage(display, IDB_DRUM);
  m_images[3]	 = LoadImage(display, IDB_PHONE);
  m_background = LoadImage(display, IDR_BACKGROUND);

  for (int i = 0; i < 4; ++i) {
		// Set color mask
    m_images[i]->SetColorMask(Color(255, 0, 255));
    // Set initial position
    m_position[i].Set(0, 0, m_images[i]->GetWidth(), m_images[i]->GetHeight());
		// New X/Y coordinates
    int x = rand() % (display->GetWidth()  - m_images[i]->GetWidth());
    int y = rand() % (display->GetHeight() - m_images[i]->GetHeight());
		// Move them
    m_position[i].Move(x, y);
  }

  m_numbers    = LoadImage(display, IDB_NUMBERS);
  m_FPSCounter = 0;
  m_dragImage  = -1; // Not dragging any image

	// Initialize the Hekkus Sound System
	m_sound.open(22050, 10, 10, true);
	
	// Load our sound FX
  m_sfx[0].load(_Module.GetModuleInstance(), IDR_TRUMPET);
  m_sfx[1].load(_Module.GetModuleInstance(), IDR_CHICKEN);
  m_sfx[2].load(_Module.GetModuleInstance(), IDR_DRUM);
  m_sfx[3].load(_Module.GetModuleInstance(), IDR_PHONE);

  // Load the MOD resource and start it playing
  m_module.load(_Module.GetModuleInstance(), IDR_MOD);
  m_module.loop(true);
  m_sound.playMod(&m_module);

  return true;

}


// Called when your application is shutting down.  Do your cleanup here.
void HekkusSample::GameEnd() {

	for (int i = 0; i < 4; ++i) {
		delete m_images[i];
  }

  delete m_background;
  delete m_numbers;

	// Shut down the Hekkus Sound System
	m_sound.close();

}


// Called when your application is being suspended, like when a low battery message appears.
void HekkusSample::GameSuspend() {

	// Suspend the Hekkus Sound System
	m_sound.suspend();

}


// Called when your application is being resumed, like after a low battery warning message has been dismissed
void HekkusSample::GameResume() {

	// Resume the Hekkus Sound System
	m_sound.resume();

}


// This is your main application loop.  Do all your real work here.
void HekkusSample::GameLoop() {

	DisplayDevice* display = GetDisplay();

  display->Blit(0, 0, m_background);

	#if defined(FROG_HPC)
			display->Hekkus(320, 0, m_background);
	#endif

  for (int i = 0; i < 4; ++i) {

		Rect& pos = m_position[i];

    // Display image
    display->Blit(pos.left, pos.top, m_images[i]);

		if (i != m_dragImage) {
			// Move image (horizontal)
      switch (rand() % 3) {
				case 0: 
				break;  // Don't move
        case 1: 
					if (pos.left > 0) {
						pos.Translate(-1, 0); 
					}
				break;
        case 2: 
					if (pos.right < display->GetWidth()) {
						pos.Translate(1, 0); 
					}
				break;
      }

      // Move image (vertical)
      switch (rand() % 3) {
				case 0: 
				break;  // Don't move
        case 1: 
					if (pos.top > 0) {
						pos.Translate(0, -1); 
				}
				break;
        case 2: 
					if (pos.bottom < display->GetHeight()) {
						pos.Translate(0, 1); 
					}
				break;
      }
    }
  }

  // Update FPS
  m_FPSTicks[ m_FPSCounter & 15 ] = PocketPC::GetTickCount();
  if (m_FPSCounter > 15) {
		uint32_t totalTime = m_FPSTicks[m_FPSCounter & 15] - m_FPSTicks[(m_FPSCounter+1) & 15];
    if (totalTime == 0) {
			totalTime = 1;
		}
    uint32_t fps  = 16000 / totalTime;
    uint32_t n100 = fps / 100; 
		fps					 -= n100 * 100;
    uint32_t n10  = fps / 10;  
		fps					 -= n10 * 10;
    uint32_t n1   = fps;

    if (n100 > 9) { 
			n100 = 9; 
			n10 = 9; 
			n1 = 9; 
		}

    // Display fps
    display->Blit(0,  0, m_numbers, &Rect(n100 * 8, 0, (n100 + 1) * 8, 11));
    display->Blit(8,  0, m_numbers, &Rect(n10  * 8, 0, (n10  + 1) * 8, 11));
    display->Blit(16, 0, m_numbers, &Rect(n1   * 8, 0, (n1   + 1) * 8, 11));
  }
  ++m_FPSCounter;

  // Update screen
  display->Update();

}


// Called to handle stylus press events
void HekkusSample::StylusDown(Point stylus) {

	// Check if the point is on one of the images
  for (int i = 3; i >= 0; --i) {
		if (m_position[i].Contains(stylus)) {
			m_dragImage  = i;
      m_dragOffset = stylus - Point(m_position[i].left, m_position[i].top);
			// Play the appropriate sound FX
			m_sound.playSFX(&m_sfx[i]);
      break;
    }
  }

}


// Called to stylus lift events
void HekkusSample::StylusUp(Point stylus) {

	m_dragImage = -1;

}


// Called to handle stylus movement events
void HekkusSample::StylusMove( Point stylus ) {

		if (m_dragImage == -1) { 
			return; 
		}

    Point newPosition(stylus - m_dragOffset);

    const Surface* image = m_images[m_dragImage];
    Rect limit(0, 0, GetDisplay()->GetWidth() - image->GetWidth(), GetDisplay()->GetHeight() - image->GetHeight());

    if (newPosition.x < limit.left) {
			newPosition.x = limit.left;
		} else if (newPosition.x > limit.right) {
			newPosition.x = limit.right;
		}

    if (newPosition.y < limit.top) {
			newPosition.y = limit.top;
		} else if (newPosition.y > limit.bottom) {
			newPosition.y = limit.bottom;
		}

    m_dragOffset = stylus - newPosition;

    m_position[m_dragImage].Move(newPosition.x, newPosition.y);

}


//////////////////////////////////////////////////////////////////////////////
//
// WinMain - Entry point
//
//////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {

	_Module.Init(0, hInstance);
  HekkusSample game;
  game.Run();
  return 0;

}
