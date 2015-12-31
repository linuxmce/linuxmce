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

#include "alpha.h"
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
// alphaSample
//
//////////////////////////////////////////////////////////////////////////////

alphaSample::alphaSample() {
	m_config.szWindowName     = _T("Alpha Sample");
  m_config.orientation      = ORIENTATION_NORTH;
  m_config.splashScreenTime = 0;
}


bool alphaSample::GameInit() { 
	DisplayDevice* display = GetDisplay();
  m_picture = LoadImage(display, IDB_PICTURE); 
	this->fadeValue = 0;
  return true;
}


void alphaSample::GameEnd() {
	delete m_picture;
}


void alphaSample::GameLoop() {
	DisplayDevice* display = GetDisplay();
	display->SetBlending(this->fadeValue);
  display->Blit(0, 0, m_picture); 
#if defined(FROG_HPC)
    display->Blit(320, 0, m_picture);
#endif
	display->Update();
	Sleep(10);
	if (this->fadeValue < 255) { 
		this->fadeValue++;
	}
}


void alphaSample::StylusUp(Point stylus) {
	this->GameEnd();
}


//////////////////////////////////////////////////////////////////////////////
//
// WinMain - Entry point
//
//////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {
	_Module.Init(0, hInstance); 
  alphaSample game;
  game.Run();
  return 0;
}
