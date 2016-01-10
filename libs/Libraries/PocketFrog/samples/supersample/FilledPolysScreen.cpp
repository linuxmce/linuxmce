	
#include "supersample.h"


// This is called from GameLoop when the current screen is csFilledPolysScreen
void supersample::FilledPolysScreenGameLoop(DisplayDevice* display) {
	static bool clear_done = false;
	if (!clear_done) {
		display->Clear(Color(0, 0, 0));
		clear_done = true;
	}
	int numPoints = (unsigned short int)(rand() * 10 / (RAND_MAX + 1)); 
	Point points[12];
	unsigned short int cR; 
	unsigned short int cG; 
	unsigned short int cB; 
	int i;
	cR = (unsigned short int)(rand() * 256 / (RAND_MAX + 1)); 
	cG = (unsigned short int)(rand() * 256 / (RAND_MAX + 1)); 
	cB = (unsigned short int)(rand() * 256 / (RAND_MAX + 1)); 
	for (i = 0; i < numPoints; i++) {
		points[i].x = (unsigned short int)(rand() * 240 / (RAND_MAX + 1));
		points[i].y = (unsigned short int)(rand() * 320 / (RAND_MAX + 1)); 
	}
	display->FillPoly(points, numPoints, Color(cR, cG, cB));
	display->DrawVGAText(VGAROMFont, TEXT("FillPoly()"), DVT_LEFT, 0, 302,  Color(255, 255, 255));
}


// This is called when the stylus is put on the screen and the current screen is csFilledPolysScreen
void supersample::FilledPolysScreenStylusDown(Point stylus) {
}


// This is called when the stylus is moved on the screen and the current screen is csFilledPolysScreen
void supersample::FilledPolysScreenStylusMove(Point stylus) {
}


// This is called when the stylus is lifted off the screen and the current screen is csFilledPolysScreen
void supersample::FilledPolysScreenStylusUp(Point stylus) {
	currentScreen = csBlitRotatedStretchScreen;
}


// This is called when a hardware button is pressed and the current screen is csFilledPolysScreen
void supersample::FilledPolysScreenButtonDown(int button) {
	Shutdown(); // Exit the application
}


// This is called when a hardware button is depressed and the current screen is csFilledPolysScreen
void supersample::FilledPolysScreenButtonUp(int button) {
	Shutdown(); // Exit the application
}