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
#include "GenericBackEnd.h"

#include <iostream>

#include "Wizard.h"

GenericBackEnd::GenericBackEnd(void)
	: IsEventWaiting (false)
{
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		Wizard::GetInstance()->SetExitWithCode(-1);
		return;
	} 
	CurrentFont = NULL;

}

GenericBackEnd::~GenericBackEnd(void)
{
	if(CurrentFont!= NULL)
		TTF_CloseFont(CurrentFont);
	TTF_Quit();
}

int GenericBackEnd::GetScreenWidth()
{
	return ScreenWidth;
}

int GenericBackEnd::GetScreenHeight()
{
	return ScreenHeight;
}

bool GenericBackEnd::HasEventPending()
{
	if (IsEventWaiting)
		return true;

	IsEventWaiting = SDL_PollEvent(&Event) != 0;

	return IsEventWaiting;
}

void GenericBackEnd::TranslateEvent(WM_Event& WMEvent)
{
	if(IsEventWaiting)
	{
		WMEvent.ConvertFromSDLEvent(Event);
		IsEventWaiting = false;
	}
	else
		WMEvent.Type = 0;
}

int GenericBackEnd::StartVideoMode(int Width, int Height, bool FullScreen)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
	return 0;
}

int GenericBackEnd::TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode)
{
	return this->TextOutWidth(Text);
}

//Drawing routines
/**
*	Fills a rectangle with a specified color
*/
void GenericBackEnd::FillRectangle(int Left, int Top, int Width, int Height, TColorDesc Color)
{
}
/**
*	Horizontal line
*/
void GenericBackEnd::HLine(int Left, int Width, int Top, TColorDesc Color)
{
	FillRectangle(Left, Top, Width, 1, Color);
}
/**
*	Vertical line
*/
void GenericBackEnd::VLine(int Left, int Top, int Height, TColorDesc Color)
{
	FillRectangle(Left, Top, 1, Height, Color);
}
/**
*	Draws a rectangle
*/
void GenericBackEnd::Rectangle(int Left, int Top, int Width, int Height, TColorDesc Color)
{
	HLine(Left, Width, Top, Color);
	HLine(Left, Width, Top+Height-1, Color);

	VLine(Left, Top, Height, Color);
	VLine(Left + Width - 1, Top, Height, Color);
}

/**
*	Says the width in pixels of rendering text
*/
int GenericBackEnd::TextOutWidth(std::string Text)
{
	SDL_Color SDL_color;
	SDL_color.r = 0;
	SDL_color.g = 0;
	SDL_color.b = 0;
	SDL_color.unused = 0;

	SDL_Surface * RenderedText = NULL;
	try
	{
		RenderedText = TTF_RenderText_Blended(CurrentFont, Text.c_str(), SDL_color);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		cout<<"SDLFrontEnd::TextOutWidth : TTF_RenderText_Blended crashed!"<<std::endl;
		return -1;
	}
	int Result = 0;
	if (RenderedText != NULL)
		Result = RenderedText->w;

	return Result;
}

/**
*	Display the final result 
*/
void GenericBackEnd::Flip(int LeftBorder, int TopBorder, int Border)
{

}

void GenericBackEnd::PaintBackground()
{
}

bool GenericBackEnd::SetCurrentFont(string FontName, int FontHeight, int Style)
{
#ifdef WIN32
	FontName = "C:\\WINDOWS\\Fonts\\verdana.ttf";
#endif

	TTF_Font *Font = TTF_OpenFont(FontName.c_str(), FontHeight);

	if(Font!= NULL)
	{
		if(CurrentFont!= NULL)
			TTF_CloseFont(CurrentFont);
		CurrentFont = Font;
	}
	else
		return false;

	int style = Style;

	TTF_SetFontStyle(CurrentFont, style);
	return true;

}

void GenericBackEnd::BackFillColor(TColorDesc* Color)
{

}

void GenericBackEnd::BackBlit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect)
{

}

void GenericBackEnd::Blit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect)
{
}
