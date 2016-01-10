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
// This file has been adapted from a sample written by "Burning Sheep".
//
//////////////////////////////////////////////////////////////////////////////

#include "plasma.h"
#include "resource.h"
#include <math.h>



//////////////////////////////////////////////////////////////////////////////
//
// _Module : This is needed to use ATL Windows
//
//////////////////////////////////////////////////////////////////////////////

CComModule _Module;



//////////////////////////////////////////////////////////////////////////////
//
// Constants
//
//////////////////////////////////////////////////////////////////////////////

const float PI = 3.14159265f;

const int PLASMA_WIDTH  = 480;
const int PLASMA_HEIGHT = 640;




//////////////////////////////////////////////////////////////////////////////
//
// Helpers
//
//////////////////////////////////////////////////////////////////////////////

namespace
{
	double hyp( double a, double b )
    {
        return sqrt( a*a + b*b );
    }
}



//////////////////////////////////////////////////////////////////////////////
//
// Plasma
//
//////////////////////////////////////////////////////////////////////////////

Plasma::Plasma()
{
    m_config.szWindowName     = _T("Plasma Sample");
    m_config.orientation      = ORIENTATION_NORTH;
    m_config.splashScreenTime = 1;
}



bool Plasma::GameInit()
{
	// Allocate space for precalc tables
	plasma1      = (byte*) malloc(PLASMA_WIDTH * PLASMA_HEIGHT * sizeof(byte));
	plasma2      = (byte*) malloc(PLASMA_WIDTH * PLASMA_HEIGHT * sizeof(byte));
	m_background = (byte*) malloc(76800 * sizeof(byte));

	// Pre-calc sin/cos table to speed up initialization
    byte sin_table[256];
    byte cos_table[256];

	for ( int i = 0; i < 256; ++i )
    {
	    sin_table[i] = (byte) (128 + 127 * sin(i * 2 * PI / 255));
		cos_table[i] = (byte) (128 + 127 * cos(i * 2 * PI / 255));
	}

	// Pre-calc palette
	for( i = 0; i < 256; ++i )
    {
		byte red  = sin_table[cos_table[(i+32) % 256]];
		byte green = sin_table[cos_table[(i+11) % 256]];
		byte blue  = cos_table[sin_table[(i+55) % 256]];

		m_palette[i] = Color(red, green, blue);
	}

	// Pre-calc plasma tables
	int offset = 0;

	for( int j = 0; j < PLASMA_HEIGHT; j++)
	{
		for( i = 0; i < PLASMA_WIDTH; i++)
		{
			plasma1[offset] = (byte) sin_table[ (byte) hyp( (PLASMA_HEIGHT/2)-j, (PLASMA_WIDTH/2)-i ) ] >> 2;
			plasma2[offset] = (byte) sin_table[ (cos_table[(i + 76) % 256] + sin_table[(j + 42) % 256]) >> 1  ] >> 2;

			offset++;
		}
	}

	// Load image
    HRSRC hResource = ::FindResource( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_BINARY1), _T("binary") );
    if (!hResource) return false;
    DWORD    size    = ::SizeofResource( _Module.GetModuleInstance(), hResource );
    HGLOBAL  hMemory = ::LoadResource( _Module.GetModuleInstance(), hResource );
    uint8_t* data    = (uint8_t*)::LockResource( hMemory );

    memcpy( m_background, data, 76800 );

	return true;
}



void Plasma::GameLoop() 
{
    DisplayDevice* display = GetDisplay();

    Surface::LockInfo buffer;
	int offset = 0;

	display->GetBackBuffer()->Lock( buffer );

	// initialize vars
	int dy = buffer.pitch >> 1;

	int x1 = (int) (120 + 119 * sin(GetTickCount() / 452.0f));
	int x2 = (int) (120 + 119 * cos(GetTickCount() / 744.0f));
	int x3 = (int) (120 + 119 * cos(GetTickCount() / 421.0f));
	int y1 = (int) (120 + 119 * cos(GetTickCount() / 381.0f));
	int y2 = (int) (120 + 119 * sin(GetTickCount() / 827.0f));
	int y3 = (int) (120 + 119 * sin(GetTickCount() / 634.0f));

	long offs1 = PLASMA_WIDTH * y1 + x1;
	long offs2 = PLASMA_WIDTH * y2 + x2;
	long offs3 = PLASMA_WIDTH * y3 + x3;


    int width  = display->GetWidth();
    int height = display->GetHeight();

	for( int y = 0; y < height; ++y )
	{
		// set offset to beginning of the next line to draw
		offset = y * 240;

		// draw the current line
		for( int x = 0; x < width; ++x )
		{
			byte color = plasma1[offs1] + plasma2[offs2] + plasma2[offs3] + (m_background[offset] >> 2);

			// plot pixel
			buffer.pixels[y * dy + x] = m_palette[color];

			++offs1;
			++offs2;
			++offs3;
			++offset;
		}

		offs1 += 240;
		offs2 += 240;
		offs3 += 240;
	}


    display->GetBackBuffer()->Unlock();
    
    display->Update();
}



void Plasma::GameEnd()
{
	free(plasma1);
	free(plasma2);
	free(m_background);
}





//////////////////////////////////////////////////////////////////////////////
//
// WinMain - Entry point
//
//////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
    _Module.Init( 0, hInstance );
    
    Plasma game;
    
    game.Run();
    
    return 0;
}
