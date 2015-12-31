
#include <PocketFrog.h>

using namespace Frog;


// ***************************************************************************
// Defines
// ***************************************************************************
#define csVGATextScreen						 1
#define csFilledPolysScreen				 2
#define csBlitRotatedStretchScreen 3


// ***************************************************************************
// Define our applications's class
// ***************************************************************************
class supersample : public Game {

	public:

    // Construction
    supersample();

    // Framework callbacks
    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    virtual void StylusDown(Point);
    virtual void StylusMove(Point);
    virtual void StylusUp(Point);
		virtual void ButtonDown(int);
		virtual void ButtonUp(int);

	private:

		// Your own members here

		uint32_t			 FPSTicksPerFrame;
		uint32_t			 FPSLastTime;
    int						 FPSCounter;
    uint32_t			 FPSTicks[16];
		int						 currentScreen;
		void					 VGATextScreenGameLoop(DisplayDevice*);
		void					 VGATextScreenStylusDown(Point);
		void					 VGATextScreenStylusMove(Point);
		void					 VGATextScreenStylusUp(Point);
		void					 VGATextScreenButtonDown(int);
		void					 VGATextScreenButtonUp(int);
		void					 FilledPolysScreenGameLoop(DisplayDevice*);
		void					 FilledPolysScreenStylusDown(Point);
		void					 FilledPolysScreenStylusMove(Point);
		void					 FilledPolysScreenStylusUp(Point);
		void					 FilledPolysScreenButtonDown(int);
		void					 FilledPolysScreenButtonUp(int);
		void					 BlitRotatedStretchScreenGameLoop(DisplayDevice*);
		void					 BlitRotatedStretchScreenStylusDown(Point);
		void					 BlitRotatedStretchScreenStylusMove(Point);
		void					 BlitRotatedStretchScreenStylusUp(Point);
		void					 BlitRotatedStretchScreenButtonDown(int);
		void					 BlitRotatedStretchScreenButtonUp(int);
		Surface*			 m_numbers;
		Surface*       imgWhitestar;
		unsigned char* WackyFont;
		unsigned char* VGAROMFont;

};