
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
// SUPERSAMPLE written by Frank W. Zammetti - This sample application is an
// attempt to showcase every feature of PocketFrog.  It should NOT be taken
// as the correct way to write a PocketFrog application, it should not even
// be taken as a GOOD way to write one!  But if you need to see how a certain
// function is used, this should be a good place to look.
//////////////////////////////////////////////////////////////////////////////




// ***************************************************************************
// Includes
// ***************************************************************************
#include "resource.h"
#include "supersample.h"


// ***************************************************************************
// Namespace declaration
// ***************************************************************************
using namespace Frog;


// ***************************************************************************
// Globals
// ***************************************************************************
CComModule _Module; // Needed to use ATL Windows


// ***************************************************************************
// WinMain - Application entry point
// ***************************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {
  _Module.Init(0, hInstance);
  supersample game;
  game.Run();
	return 0;
}


// ***************************************************************************
// Now implement our class 
// ***************************************************************************

// Construction
supersample::supersample() {
	m_config.szWindowName     = _T("supersample");
  m_config.orientation      = ORIENTATION_NORTH;
  m_config.splashScreenTime = 0; // Even though it's nice, let's not show the PF splash screen
	m_config.desktopZoom      = 0; // No zoom on PC (240x320 in other words)
}


// Do whatever initialization the application requires here
bool supersample::GameInit() {
	// Set what is the current screen
	currentScreen			= csVGATextScreen;
	// Load our two font resources
	HRSRC hResInfo	  = NULL;
	HGLOBAL hResource = NULL;
	hResInfo					= FindResource(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_WACKY_FONT), TEXT("FONTS"));
	hResource					= LoadResource(_Module.GetModuleInstance(), hResInfo);
	WackyFont					= (unsigned char*)LockResource(hResource);
	hResInfo					= FindResource(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_VGAROM_FONT), TEXT("FONTS"));
	hResource					= LoadResource(_Module.GetModuleInstance(), hResInfo);
	VGAROMFont				= (unsigned char*)LockResource(hResource);
	imgWhitestar			= LoadImage(GetDisplay(), IDR_WHITESTAR);
	m_numbers					= LoadImage(GetDisplay(), IDB_NUMBERS);
	FPSTicksPerFrame  = 1000 / 30; // Limit app to 30 frames per second max., give or take an FPS or two
	FPSLastTime			  = PocketPC::GetTickCount();
  FPSCounter				= 0;
	return true;
}


// Do whatever cleanup that PF doesn't handle for you here
void supersample::GameEnd() {
	delete m_numbers;
	delete imgWhitestar;
}


// The main game loop ... Here we call the handler for the screen we're currently on
void supersample::GameLoop() {

	DisplayDevice* display = GetDisplay();

		// Call the handler for the screen we're currently on
		switch (currentScreen) {
			case csVGATextScreen: {
				VGATextScreenGameLoop(display);
			break; }
			case csFilledPolysScreen: {
				FilledPolysScreenGameLoop(display);
			break; }
			case csBlitRotatedStretchScreen: {
				BlitRotatedStretchScreenGameLoop(display);
			break; }
		}

		// Do frame rate limiting
		while (PocketPC::GetTickCount() - FPSLastTime < FPSTicksPerFrame) { } 
		FPSLastTime = PocketPC::GetTickCount();

		// Update FPS
		FPSTicks[FPSCounter & 15] = FPSLastTime;
		if (FPSCounter > 15) {
			uint32_t totalTime = FPSTicks[FPSCounter & 15] - FPSTicks[(FPSCounter+1) & 15];
				if (totalTime == 0) {
					totalTime = 1;
				}
        uint32_t fps   = 16000 / totalTime;
        uint32_t n100  = fps	 / 100; 
				fps						-= n100  * 100;
        uint32_t n10   = fps	 / 10;  
				fps						-= n10	 * 10;
        uint32_t n1   = fps;
        if (n100 > 9) { 
					n100 = 9; 
					n10 = 9; 
					n1 = 9; 
				}
        // Display fps
        display->Blit( 0, 0, m_numbers, &Rect(n100 * 8, 0, (n100 + 1) * 8, 11));
        display->Blit( 8, 0, m_numbers, &Rect(n10  * 8, 0, (n10 + 1)  * 8, 11));
        display->Blit(16, 0, m_numbers, &Rect(n1   * 8, 0, (n1 + 1)   * 8, 11));
    }
    ++FPSCounter;

  display->Update();

}


// Called to handle stylus down events.  Call the handler for the screen we're on
void supersample::StylusDown(Point stylus) {
	switch (currentScreen) {
		case csVGATextScreen: {
			VGATextScreenStylusDown(stylus);
		break; }
		case csFilledPolysScreen: {
			FilledPolysScreenStylusDown(stylus);
		break; }
		case csBlitRotatedStretchScreen: {
			BlitRotatedStretchScreenStylusDown(stylus);
		break; }
	}
}


// Called to handle stylus move events.  Call the handler for the screen we're on
void supersample::StylusMove(Point stylus) {
	switch (currentScreen) {
		case csVGATextScreen: {
			VGATextScreenStylusMove(stylus);
		break; }
		case csFilledPolysScreen: {
			FilledPolysScreenStylusMove(stylus);
		break; }
		case csBlitRotatedStretchScreen: {
			BlitRotatedStretchScreenStylusMove(stylus);
		break; }
	}
}


// Called to handle stylus up events.  Call the handler for the screen we're on
void supersample::StylusUp(Point stylus) {
	switch (currentScreen) {
		case csVGATextScreen: {
			VGATextScreenStylusUp(stylus);
		break; }
		case csFilledPolysScreen: {
			FilledPolysScreenStylusUp(stylus);
		break; }
		case csBlitRotatedStretchScreen: {
			BlitRotatedStretchScreenStylusUp(stylus);
		break; }
	}
}


// Called to handle button down events.  Call the handler for the screen we're on
void supersample::ButtonDown(int button) {
	switch (currentScreen) {
		case csVGATextScreen: {
			VGATextScreenButtonDown(button);
		break; }
		case csFilledPolysScreen: {
			FilledPolysScreenButtonDown(button);
		break; }
		case csBlitRotatedStretchScreen: {
			BlitRotatedStretchScreenButtonDown(button);
		break; }
	}
}


// Called to handle button up events.  Call the handler for the screen we're on
void supersample::ButtonUp(int button) {
	switch (currentScreen) {
		case csVGATextScreen: {
			VGATextScreenButtonUp(button);
		break; }
		case csFilledPolysScreen: {
			FilledPolysScreenButtonUp(button);
		break; }
		case csBlitRotatedStretchScreen: {
			BlitRotatedStretchScreenButtonUp(button);
		break; }
	}
}
