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

#include <PocketFrog.h>
#include "resource.h"
#include  <string.h>
#include "blit.h"
#include <stdlib.h>
#include "sprite.h"

//////////////////////////////////////////////////////////////////////////////
//
// _Module : This is needed to use ATL Windows
//
//////////////////////////////////////////////////////////////////////////////

CComModule _Module;


//	int nWidth, nHeight; // current window size


// MyGame
//*****************************************************************************

using namespace Frog;


class MyGame : public Game 
{
public:

    MyGame()
    {
        PocketPC::GetModel();

        m_config.szWindowName     = _T("Sprites");
        m_config.orientation      = ORIENTATION_NORTH;//NORTH is portrait
        m_config.splashScreenTime = 200;
    }
// set up everything for game
//**********************************
    virtual bool GameInit()
    {
		srand( int(PocketPC::GetTickCount) );//seed random number generator

        DisplayDevice* display = GetDisplay();	
		
		cavegirl.SetDisplayDevice(display);		// Lucy
		cavegirl.LoadSpriteData(IDB_CAVEGIRL,4);// 4 frames
//InitSprite params are (xpos,ypos,xmove,ymove,xmax,xmin,speed,State)
// xmove & ymove can be + or -   speed is rate frames animate
		cavegirl.InitSprite(260,166,-2,0,260,-50,6,TRUE);// left x 2

		caveman.SetDisplayDevice(display);		// Rocky
		caveman.LoadSpriteData(IDB_CAVEMAN,4);// 4 frames
		caveman.InitSprite(60,162,0,0,0,0,15,TRUE);// no move

		ptera.SetDisplayDevice(display);		// Pteradactyl
		ptera.LoadSpriteData(IDB_PTERA,6);// 6 frames
		ptera.InitSprite(-120,50,4,0,280,-120,6,TRUE);// moves right x 3

		heart.SetDisplayDevice(display);		// love heart
		heart.LoadSpriteData(IDB_HEART,1);// only 1 frame
		heart.InitSprite(95,150,0,-2,0,0,8,FALSE);// not on 

// setups for the backgrounds
		speed_back = 4;
//		background_x = 0; //IF for background scroll
		xpos_back = 1;
		cnt_back = speed_back;
        m_background = LoadImage( display, IDB_BACKGROUND );//trees
        m_background->SetColorMask( Color( 255,0, 255) );//transparent color

		speed_sky = 10;
		xpos_sky = 1; //  first time round the -- makes it zero
		cnt_sky = speed_sky;
        m_sky = LoadImage( display, IDR_SKY );//Sky
        m_sky->SetColorMask( Color( 0, 0, 0) );//transparent color

		// Initialize random seed
		first_time = 0;	//IF for drawtext
		m_screen.Set(0,0,239,319);
		disp_text = FALSE;	//no message to display
        return true;
    }// end of GameInit
//**************************************************

    virtual void GameEnd()
    {
// Iknow I should tidy up here :)

    }

//**************************************************
    virtual void GameLoop()
    {
		CurTime = GetTickCount();// get current tick counter
		DisplayDevice* display = GetDisplay();
		RECT rcString;			// RECT for text display
		rcString.top = 100;
		rcString.left = 0;
		rcString.right = 240;
		rcString.bottom = 240;
		if(first_time == 0)// this stuff could be in gameinit
		{
		display->Clear(Color(10,1,129));
		display->DrawRect(5,5,233,313,Color(255,100,100));
	    HDC hDC = display->GetBackBuffer()->GetDC( false );//true overwrites
		if (hDC)
			{
//LOGFONT lf; // Initializes a LOGFONT structure
			memset(&lf, 0, sizeof(LOGFONT));		// Zero out the structure.
			lf.lfHeight = 32;						// Request a 12-pixel-height font.
			lstrcpy(lf.lfFaceName, _T("Tahoma"));   // Request a face name "Tahoma".
//HFONT hfont;							// create an 'HFONT'
			hfont = ::CreateFontIndirect(&lf);		//create our font
			SelectObject(hDC,hfont);				// make it the current font
			RECT rcString1; //bounding rect for text
			rcString1.top = 10;//just below keyboard
			rcString1.left = 0;
			rcString1.right = 240;
			rcString1.bottom = 300;
			SetTextColor(hDC,0x00FFFF);
			SetBkMode(hDC,TRANSPARENT);// for drawtext
			DrawText( hDC,TEXT("PocketFrog\rthe Game Toolkit\rfor PocketPC\rSide Scroller\rdemo with\rSprites and\rParallax scrolling"), -1, &rcString1, DT_CENTER|DT_WORDBREAK );//message on back buffer
		   }// end of if hDC
			display->GetBackBuffer()->ReleaseDC( hDC );// give control back to PocketFrog 
			display->Update();// draw to the screen
			Sleep(100);// a brief pause
// fade out the text
			int blend = 10;// start blend amount
			for (int x = 0; x < 16; ++x)
			{
				display->SetBlending(blend);//transparent rect over key pressed
				display->FillRect(m_screen,Color(10,1,129));
				display->Update();
				blend+=2;// change blend amount
				Sleep(70);// pause in between fades
			}
			display->SetBlending(0xFF);//backto no alpha blend
			display->Clear(Color(58,114,16));// clear screen to green
			SetTextColor(hDC,0x0000FF);

		   first_time++;// never 0 again so won;t come here
		}//end first_time

//
// Note: Sky & trees drawn in non 'OO' methods
// Draw the sky image to the background scrolling it to the left slowly
		cnt_sky--;//decrement movement counter
		if(cnt_sky == 0)//when it reaches zero we scroll the background
		{
			xpos_sky--;
			cnt_sky = speed_sky;
		}
		if(xpos_sky < -240)
		{
			xpos_sky = 0;
		}
		display->Blit(xpos_sky,0,m_sky);// drawing image twice for scroll [ | ][ | ]<---
		display->Blit(xpos_sky+240,0,m_sky);
// and now the scrolling trees, just like the sky but they have transparent bits (not Alpha)
		cnt_back--;//decrement movement counter
		if(cnt_back == 0)//when it reaches zero we scroll the background
		{
			xpos_back--;
			cnt_back = speed_back;
		}
		if(xpos_back < -240)
		{
			xpos_back = 0;
		}
		display->Blit(xpos_back,135,m_background);
		display->Blit(xpos_back+240,135,m_background);

// note: caveman & girl + pteradactyl drawn using a Sprite class, see sprite.h & sprite.cpp
		caveman.Draw();// Rocky our hero
		caveman.Move();// update position

		ptera.Draw();//Pteradactyl
		ptera.Move();//Lucy the cavegirl
// test for cavegirl meeting rocky
		if(	cavegirl.Hit(caveman) == true) //has she hit (ie: kissed) caveman
		{
			heart.On = TRUE;//turn on heart *
			heart.Alpha --;//fade
		}
		else
			{
			heart.Alpha = 0xFF;//no blending
			heart.On = FALSE;//turn off
			heart.Ypos = 150;//reset Ypos t start ( very crude way of doing this )
		}
		cavegirl.Draw();//test draw sprite myplayer
		cavegirl.Move();//update screen position

		heart.Draw();//only happens after 'kiss' *
		heart.Move();//update the x,y coords
// draw DGI text if buttons pressed
	if(disp_text == TRUE)
	{
		disp_mess();//draw text on backbuffer
		disp_text = FALSE;
	}
	display->Update();// all drawing finished, update the visible display
//	while( (GetTickCount() - CurTime) < 20 ) {}

    }//end gameloop
//*********************************************
// message to display using GDI 
	void disp_mess(){
		
		DisplayDevice* display = GetDisplay();

		display->FillRect( Rect( 0, 299, 240, 319 ), 0Xffff );//where we will draw text

	    HDC hDC = display->GetBackBuffer()->GetDC( false );//true overwrites

		LOGFONT lf; // Initializes a LOGFONT structure

		memset(&lf, 0, sizeof(LOGFONT));		// Zero out the structure.
		lf.lfHeight = 20;						// Request a 12-pixel-height font.
		lstrcpy(lf.lfFaceName, _T("Tahoma"));   // Request a face name "Tahoma".
		HFONT hfont;							// create an 'HFONT'
		hfont = ::CreateFontIndirect(&lf);		//create our font
		SelectObject(hDC,hfont);				// make it the current font

// Finished with the font, should now delete the font object?
//font.DeleteObject(); */


		RECT rcString1; //bounding rect for text
		rcString1.top = 300;//low down the screen
		rcString1.left = 4;
		rcString1.right = 239;
		rcString1.bottom = 319;
		SetBkMode(hDC,OPAQUE);// for drawtext
		DrawText( hDC, keystr, _tcslen(keystr), &rcString1, DT_LEFT|DT_WORDBREAK );//message on back buffer
		//_tcslen gets the length of the string
		//ExtTextOut(hDC, 5, 5, ETO_OPAQUE, NULL, _T("Hello"), 5,NULL);

		display->GetBackBuffer()->ReleaseDC( hDC ); 

	}//enddisp mess
//*********************************************

// ButtonDown() - A hard button was pressed, show it's location with a yellow box,

void ButtonDown( int button )
{

   if (button > 1 )// I don't know what this should actually be so don;t follow my lead
   {
// if you press buttons you will get text messages for thise set up below except D-Pad down
// which will exit the demo. Not all buttons have been picked up here
		const InputDevice::KeyList& keys = GetInput()->GetKeyList();
		if(button == keys.vkA)
		{
			LoadString(_Module.GetResourceInstance(),key_a,keystr,20);
			disp_text = TRUE;
		}
		if(button == keys.vkB)
		{
			LoadString(_Module.GetResourceInstance(),key_b,keystr,20);
			disp_text = TRUE;
		}
		if(button == keys.vkC)
		{
			LoadString(_Module.GetResourceInstance(),key_c,keystr,20);
			disp_text = TRUE;
		}
		if(button == keys.vkStart)
		{
			LoadString(_Module.GetResourceInstance(),key_action,keystr,20);
			disp_text = TRUE;
		}

		if(button == keys.vkLeft)
		{
			LoadString(_Module.GetResourceInstance(),key_butleft,keystr,20);

			disp_text = TRUE;
		}
		if(button == keys.vkRight)
		{
			LoadString(_Module.GetResourceInstance(),key_butright,keystr,20);
			disp_text = TRUE;
		}
		if(button == keys.vkUp)
		{
			LoadString(_Module.GetResourceInstance(),key_butup,keystr,20);
			disp_text = TRUE;
		}

		if(button == keys.vkDown)
		{
			Shutdown();//end the game
		}

   }//if button > 1 end
}//end button down

//***************************
// ButtonUp() - A button was released, empty the box using a black rectangle.

void ButtonUp( int button )
{
// no actions for button up in demo
}

//*********************************************
    virtual void StylusDown( Point stylus )
    {
// no stylus reading in this demo
    }//end stylusdown


 //*********************************************   
    virtual void StylusUp( Point stylus )
    {
// no stylus in this demo
    }


//***********************************************
    virtual void StylusMove( Point stylus )
  {
		return;
// no stylus move in demo
  }//end stylusmove



private:

//for TTF fonts

	LOGFONT lf; // Initializes a LOGFONT structure
	HFONT hfont;							// create an 'HFONT'

// variables
	bool disp_text;				// flag for whether to disp mess
	TCHAR keystr[200];			// string for text drawing
	int CurTime;				// used to time frames
	int first_time;				// TEST used in loop
	int speed_back;				// scroll speed
	int speed_sky;				// scroll speed
	int cnt_sky;				// speed counter
	int cnt_back;				//
	int xpos_sky;				//
	int xpos_back;				//
	int xpos_caveman;
	int ypos_caveman;
	int cnt_caveman;
	int framecnt_caveman;		// which frame to show
	int speed_caveman;
	int xpos_ptera;
	int ypos_ptera;
	int cnt_ptera;
	int framecnt_ptera;		// which frame to show
	int speed_ptera;



    Surface* m_background;      // Background trees image
	Surface* m_sky;				// Sky image
	Surface* m_keyglow;			// IF keyboard green button
    int      m_dragImage;       // # of image being dragged (-1 if none)
    Point    m_dragOffset;
	Rect	m_screen;
	Sprite caveman; // Rocky the hero
	Sprite cavegirl;// Lucy his girl
	Sprite ptera; // pteradactyl (bird)
	Sprite heart;// heart for when they kiss
};//end class MyGame

//////////////////////////////////////////////////////////////////////////////
//
// WinMain - Entry point
//
//////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
    _Module.Init( 0, hInstance );

    MyGame game;
    
    game.Run();

    return 0;
}