	
#include "supersample.h"

// This is called from GameLoop when the current screen is csBlitRotatedStretchScreen
void supersample::BlitRotatedStretchScreenGameLoop(DisplayDevice* display) {
  const float  PI = 3.1415926535897932384626433832795f;
	static float rotation_angle		 = 0.0f;
	static int   stretch_amount		 = 0;
	static int   stretch_direction = 1;
	display->Clear(Color(0, 0, 0));
	Surface* stretch_surface = display->CreateSurface(imgWhitestar->GetWidth() + stretch_amount, imgWhitestar->GetHeight() + stretch_amount);
	display->BlitRotatedStretch( ((display->GetWidth()  - imgWhitestar->GetWidth())  / 2) + imgWhitestar->GetWidth()  / 2, 
		                           ((display->GetHeight() - imgWhitestar->GetHeight()) / 2) + imgWhitestar->GetHeight() / 2, 
															 rotation_angle, 
															 imgWhitestar, 
															 stretch_surface, 
															 NULL
														 );
	delete stretch_surface;
  rotation_angle += 0.05f;
  if (rotation_angle > 2.0f * PI) {
      rotation_angle -= 2.0f * PI;
	}
	switch (stretch_direction) {
		case 0:
			stretch_amount = stretch_amount - 5;
			if (stretch_amount < -100) {
				stretch_direction = 1;
			}
		break;
		case 1:
			stretch_amount = stretch_amount + 5;
			if (stretch_amount > 100) {
				stretch_direction = 0;
			}
		break;
	}
	display->DrawVGAText(VGAROMFont, TEXT("BlitRotatedStretch()"), DVT_LEFT, 0, 302,  Color(255, 255, 255));
}


// This is called when the stylus is put on the screen and the current screen is csBlitRotatedStretchScreen
void supersample::BlitRotatedStretchScreenStylusDown(Point stylus) {
}


// This is called when the stylus is moved on the screen and the current screen is csBlitRotatedStretchScreen
void supersample::BlitRotatedStretchScreenStylusMove(Point stylus) {
}


// This is called when the stylus is lifted off the screen and the current screen is csBlitRotatedStretchScreen
void supersample::BlitRotatedStretchScreenStylusUp(Point stylus) {
	Shutdown(); // Exit the application
}


// This is called when a hardware button is pressed and the current screen is csBlitRotatedStretchScreen
void supersample::BlitRotatedStretchScreenButtonDown(int button) {
	Shutdown(); // Exit the application
}


// This is called when a hardware button is depressed and the current screen is csBlitRotatedStretchScreen
void supersample::BlitRotatedStretchScreenButtonUp(int button) {
	Shutdown(); // Exit the application
}