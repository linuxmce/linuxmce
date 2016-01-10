/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "SDLFrontEnd.h"

#include <iostream>

#include "Wizard.h"

#include <SDL_rotozoom.h>

#define REDUCE_PERCENT 0.9

SDLFrontEnd::SDLFrontEnd()
: GenericBackEnd()
{
	Display = NULL;
	BackColor = NULL;

	Screen = NULL;
	BackSurface = NULL;
	ScaledScreen = NULL;
	ScaledBack = NULL;
}

SDLFrontEnd::~SDLFrontEnd()
{
	if (Screen)
		SDL_FreeSurface(Screen);
	if (BackSurface)
		SDL_FreeSurface(BackSurface);
	if (ScaledScreen)
		SDL_FreeSurface(ScaledScreen);
	if (ScaledBack)
		SDL_FreeSurface(ScaledBack);
}


int SDLFrontEnd::StartVideoMode(int Width, int Height, bool FullScreen)
{
	int Flags = 0;
	if(Display)
		SDL_Quit();
	//Test if SDL inits nicely
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)== -1)
	{
		std::cout<<"SDL library cannot init properly!"<<std::endl;
		Wizard::GetInstance()->SetExitWithCode(-1);
		return -1;
	}
		
	if (FullScreen)
		Flags = SDL_FULLSCREEN;
		
	Display = SDL_SetVideoMode(Width, Height, 0, Flags);
	if(!Display)
	{
		std::cout<<"Cannot init the video mode!"<<std::endl;
		Wizard::GetInstance()->SetExitWithCode(-1);
		return -1;
	}

	ScreenWidth = Width;
	ScreenHeight = Height;
	
	SDL_WM_SetCaption("AVWizard", "AVWizard");
 	// Hide the mouse cursor
	// SDL_ShowCursor(0);

	atexit(SDL_Quit);

	BackSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 24, 0, 0, 0, 0);
	Screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 24, 0, 0, 0, 0);

	return 0;
}

void SDLFrontEnd::Flip(int LeftBorder, int TopBorder, int Border)
{
 	int Width = Display->w;
	int Height = Display->h;

	int ReducePercent = static_cast<int>(2 * Border / static_cast<double>(Width) * 100);
	int OrbiterShiftX = LeftBorder - Border;
	int OrbiterShiftY = TopBorder - Border;

	cout << 
		"Width " << Width << endl <<
		"Height " << Height << endl <<
		"LeftBorder " << LeftBorder << endl <<
		"TopBorder " << TopBorder << endl <<
		"Border " << Border << endl <<
		"--------------------------" << endl <<
		"ReducePercent " << ReducePercent << endl <<
		"OrbiterShiftX  " << OrbiterShiftX << endl <<
		"OrbiterShiftY  " << OrbiterShiftY << endl;

	if (BackColor)
		SDL_FillRect(Display, NULL, SDL_MapRGBA(Screen->format, BackColor->GetRed(), BackColor->GetGreen(), 
			BackColor->GetBlue(), 255));

	double ZoomControlls = 1.0;

	SDL_Rect RectControlls, RectBackArrows;

	double ZoomBackArrows = (100 - ReducePercent) / 100.0;
	RectBackArrows.x = OrbiterShiftX + ReducePercent * Width / (2 * 100);
	RectBackArrows.y = OrbiterShiftY + ReducePercent * Height / (2 * 100);
	RectBackArrows.w = static_cast<int>(ZoomBackArrows * Width);
	RectBackArrows.h = static_cast<int>(ZoomBackArrows * Height);

	//scale to this resolution from 640x480 graphics
	double ScaleX = Width / 640.0;
	double ScaleY = Height / 480.0;

	if(Wizard::GetInstance()->m_PageSequence.CurrentPage() == "VideoAdjustSize")
	{
		ZoomControlls = REDUCE_PERCENT * ZoomBackArrows;
		RectControlls.x = RectBackArrows.x + static_cast<int>(((1.0 - REDUCE_PERCENT) / 2) * RectBackArrows.w);
		RectControlls.y = RectBackArrows.y + static_cast<int>(((1.0 - REDUCE_PERCENT) / 2) * RectBackArrows.h);
	}
	else
	{
		ZoomControlls = ZoomBackArrows;
		RectControlls = RectBackArrows;
	}

	if(NeedUpdateBack)
	{
		ScaledBack = zoomSurface(BackSurface, ZoomBackArrows * ScaleX, ZoomBackArrows * ScaleY, SMOOTHING_ON);
		NeedUpdateBack = false;
	}
	if(ScaledBack)
	{
		SDL_BlitSurface(ScaledBack, NULL, Display, &RectBackArrows);
	}

	if(NeedUpdateScreen)
	{
		ScaledScreen = zoomSurface(Screen, ZoomControlls * ScaleX, ZoomControlls * ScaleY, SMOOTHING_ON);
		NeedUpdateScreen = true;
	}
	if(ScaledScreen)
	{
		SDL_BlitSurface(ScaledScreen, NULL, Display, &RectControlls);
	}

	SDL_Flip(Display);	
}

void SDLFrontEnd::PaintBackground()
{
	SDL_FillRect(Display, NULL, 0xffffff);
}

int SDLFrontEnd::PaintFont(char* Text, 
						   int Top, int Left, 
						   TColorDesc Color,
						   int Mode)
{
	if(CurrentFont == NULL)
	{
		std::cout<<"Warning: there is no font setup for rendering!"<<std::endl;
		return -1;
	}
	SDL_Color SDL_color;

	SDL_color.r = Color.GetRed();
	SDL_color.g = Color.GetGreen();
	SDL_color.b = Color.GetBlue();
	SDL_color.unused = 0;


	SDL_Surface * RenderedText = NULL;
	try
	{
		RenderedText = TTF_RenderText_Blended(CurrentFont, Text, SDL_color);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		std::cout<<"Renderer::RealRenderText : TTF_RenderText_Blended crashed!"<<std::endl;
	}


	if (RenderedText == NULL)
	{	
		TTF_Quit();
		std::cout<<"Renderer::RealRenderText: Can't render text: "<<Text<<std::endl;
		return -1;
	}

	SDL_Rect SDL_rect;
	SDL_rect.x = Left;
	SDL_rect.y = Top;

	int Result = RenderedText->w;
	switch(Mode) {
	case 0:
		//Nothing to do, default aling is left one
		break;
	case 1:
		SDL_rect.x -= Result / 2;
		break;
	case 2:
		SDL_rect.x -= Result;
		break;
	}
	
	SDL_BlitSurface(RenderedText, NULL, Screen, &SDL_rect);
	try
	{
		SDL_FreeSurface(RenderedText);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		std::cout<<"Renderer::RealRenderText : SDL_FreeSurface crashed!"<<std::endl;
	}
	return Result;
}

int SDLFrontEnd::TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode)
{
	return PaintFont(Text, Top, Left, Color, Mode);
}


void SDLFrontEnd::Blit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect)
{
	NeedUpdateScreen = true;

	if(ScaledScreen)
	{
		SDL_FreeSurface(ScaledScreen);
		ScaledScreen = NULL;
	}

	SDL_BlitSurface(Surface, &SrcRect, Screen, &DestRect);
}

void SDLFrontEnd::BackBlit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect)
{
	if (Surface == NULL)
		return;
	NeedUpdateBack = true;
	if(ScaledBack)
	{
		SDL_FreeSurface(ScaledBack);
		ScaledBack = NULL;
	}

	SDL_BlitSurface(Surface, &SrcRect, BackSurface, &DestRect);	
}

void SDLFrontEnd::BackFillColor(TColorDesc* Color)
{
	NeedUpdateBack = true;
	if(BackColor == NULL)
		delete BackColor;
	this->BackColor = Color;
	if(ScaledBack)
	{
		SDL_FreeSurface(ScaledBack);
		ScaledBack = NULL;
	}	
	SDL_FillRect(BackSurface, NULL, SDL_MapRGBA(Screen->format, 
		Color->GetRed(), 
		Color->GetGreen(), 
		Color->GetBlue(), 
		255));
}

int SDLFrontEnd::GetScreenWidth()
{
	return Display->w;
}

int SDLFrontEnd::GetScreenHeight()
{
	return Display->h;
}

void SDLFrontEnd::FillRectangle(int Left, int Top, int Width, int Height, TColorDesc Color)
{
	SDL_Rect Rect;
	Rect.x = Left;
	Rect.y = Top;
	Rect.w = Width;
	Rect.h = Height;

	SDL_FillRect(Screen, &Rect, SDL_MapRGBA(Screen->format, Color.GetRed(),
		Color.GetGreen(),
		Color.GetBlue(),
		255));

}
