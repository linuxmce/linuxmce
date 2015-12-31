 //////////////////////////////////////////////////////////////////////////////
//
// quick shader demo by Damian Ancukiewicz... this is by no means canonical

#include "resource.h"

#include <PocketFrog.h>

using namespace Frog;

//////////////////////////////////////////////////////////////////////////////
//
// _Module : This is needed to use ATL Windows
//
//////////////////////////////////////////////////////////////////////////////

CComModule _Module;

int ShaderNumber = 0; // 0: Grayscale shader
	                      // 1: Fading shader
	                      // 2: 4-bit color shader
	                      // sorry... i'm uncreative in the pixel shading department

int FadeSetting = 0;  // setting for fade shader 
int FadeDir = 1; // direction in which it fades
// fade shader
struct FadeShader : public PixelShader 
{
	 Pixel operator() (Pixel src, Pixel dest) const 
	 {
		  int R = src & RED_MASK;    // Get the red part of the pixel
		  int G = src & GREEN_MASK;  // Green part
		  int B = src & BLUE_MASK;   // Blue part

		  R = R - (FadeSetting << RED_SHIFT);          // Shift red part down to the last 5 bits
		  G = G - (FadeSetting << (GREEN_SHIFT + 1));  // Same with green (remember, green
		                                               // has one more bit than the other colors)
		  B = B - (FadeSetting << BLUE_SHIFT);         // Ditto

		  // Don't want any color to be less than 0 or else it will wrap around
		  if (R < 0)
		  R = 0;
		  if (G < 0)
		  G = 0;
		  if (B < 0)
		  B = 0;

		  return (R | G | B);  // Return the colors OR'ed back together
	 }
};

FadeShader fadeShader; // Creates a fade shader

// the grayscale shader
struct GrayShader : public PixelShader {
	 Pixel operator() (Pixel src, Pixel dest) const {
		 register int R = src & RED_MASK;   // red
		 register int G = src & GREEN_MASK; // green
		 register int B = src & BLUE_MASK;  // blue


		 R = R >> RED_SHIFT;           // shift all colors down so they can be averaged
		 G = G >> (GREEN_SHIFT + 1);   // remember, green has 1 more bit in 565 color
		 B = B >> BLUE_SHIFT;

		 register int fillColor = (R + G + B) / 3;  // average all colors for a fill color

		 return (fillColor << RED_SHIFT | fillColor << (GREEN_SHIFT + 1) | 
			 fillColor << BLUE_SHIFT);  // fill every color with the fill color
	 }
};

GrayShader grayShader; // Create a grayscale shader

// the 8 bit shader, for some beautiful color
struct EightShader : public PixelShader {
	 Pixel operator() (Pixel src, Pixel dest) const {
		 register int R = src & RED_MASK;  
		 register int G = src & GREEN_MASK; 
		 register int B = src & BLUE_MASK;  


		 R = R >> (RED_SHIFT + 2);     // Lose the extra color bits so only 3 remain
		 G = G >> (GREEN_SHIFT + 3);   
		 B = B >> (BLUE_SHIFT + 3);    // 8-bit color is 332; only 2 blue bits 

		 

		 return (R << 13 | G << 8 | 
			 B << 3); // go back to "16" bit
	 }
};

EightShader eightShader; // create 8-bit shader

class Shader : public Game
{
private:

	Surface* Background; // Background surface

public:

    // Construction
    Shader()
	{
		m_config.szWindowName     = _T("Pixel Shader Demo");
		m_config.orientation      = ORIENTATION_WEST;
		m_config.splashScreenTime = 1000;
	}



    // Framework callbacks
	virtual bool GameInit() 
	{
		DisplayDevice* display = GetDisplay();

		Background = LoadImage(display, IDB_BACKGROUND);  // Load background image
		Background->SetColorMask(Color(0,0,0));

		return true;
	}

    virtual void GameEnd() 
	{
		delete Background;
	}

    virtual void GameLoop()
	{
		DisplayDevice* display = GetDisplay();

		FadeSetting += FadeDir;             // Cycle through the fade settings
		                                    // going up and down
		// In 555 color (extra bit was lost), each color has 32 values
		if (FadeSetting < 1)
			// reverse the direction
			FadeDir = 1;
		if (FadeSetting > 30)
			FadeDir = -1;

		display->Blit(0, 0, Background); // blit the background
		display->Update(); // update display
	}

    virtual void StylusDown( Point stylus )
	{
		DisplayDevice* display = GetDisplay();

		ShaderNumber++;  // Cycle through the shaders
		if (ShaderNumber > 2)
			ShaderNumber = 0;

		switch (ShaderNumber)
		{
		case 0:
			display->SetPixelShader(&fadeShader);  // set fade shader
			break;
		case 1:
			display->SetPixelShader(&grayShader);  // set grayscale shader
			break;
		case 2:
			display->SetPixelShader(&eightShader);  // set 8-bit shader
			break;
		}
	}
    virtual void StylusUp(Point stylus){}
	virtual void StylusMove(Point stylus){}
};


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
    _Module.Init( 0, hInstance );
    
    Shader game;
    
    game.Run();
    
    return 0;
}
