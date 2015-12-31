
#include "supersample.h"


// This is called from GameLoop when the current screen is csVGATextScreen
void supersample::VGATextScreenGameLoop(DisplayDevice* display) {
	int yLoc	 = 12;
	int incVal = 17;
	display->DrawVGAText(WackyFont, TEXT("Welcome to the PocketFrog"), DVT_CENTER, 0, yLoc,  Color(255, 0,   0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("SuperSample example app!"),  DVT_CENTER, 0, yLoc,  Color(255, 0,   0));
	yLoc = yLoc + incVal + 8;
	display->DrawVGAText(WackyFont, TEXT("This app attempts to"),      DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("showcase all features of"),  DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("PocketFrog and acts as an"), DVT_CENTER, 0, yLoc,  Color(0,	 255, 0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("example for PF developers"), DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
	yLoc = yLoc + incVal + 8;
	display->DrawVGAText(WackyFont, TEXT("This should NOT be taken"),  DVT_CENTER, 0, yLoc,  Color(100, 100, 255));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("to be the proper way to"),   DVT_CENTER, 0, yLoc,  Color(100, 100, 255));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("write a PF app, simply"),    DVT_CENTER, 0, yLoc,  Color(100, 100, 255));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("one possible way."),         DVT_CENTER, 0, yLoc,  Color(100, 100, 255));
	yLoc = yLoc + incVal + 8;
	display->DrawVGAText(WackyFont, TEXT("FPS is shown in the upper"), DVT_CENTER, 0, yLoc,  Color(255, 0,   0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("left-hand corner."),         DVT_CENTER, 0, yLoc,  Color(255, 0,   0));
	yLoc = yLoc + incVal + 8;
	display->DrawVGAText(WackyFont, TEXT("Tap the screen to move"),    DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("from screen to screen,"),    DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("press any hardware button"), DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
	yLoc = yLoc + incVal;
	display->DrawVGAText(WackyFont, TEXT("to exit the program."),      DVT_CENTER, 0, yLoc,  Color(0,   255, 0));
}


// This is called when the stylus is put on the screen and the current screen is csVGATextScreen
void supersample::VGATextScreenStylusDown(Point stylus) {
}


// This is called when the stylus is moved on the screen and the current screen is csVGATextScreen
void supersample::VGATextScreenStylusMove(Point stylus) {
}


// This is called when the stylus is lifted off the screen and the current screen is csVGATextScreen
void supersample::VGATextScreenStylusUp(Point stylus) {
	currentScreen = csFilledPolysScreen;
}


// This is called when a hardware button is pressed and the current screen is csVGATextScreen
void supersample::VGATextScreenButtonDown(int button) {
	Shutdown(); // Exit the application
}


// This is called when a hardware button is depressed and the current screen is csVGATextScreen
void supersample::VGATextScreenButtonUp(int button) {
	Shutdown(); // Exit the application
}