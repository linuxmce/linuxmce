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
#ifndef GenericBackEnd_H_
#define GenericBackEnd_H_

#include "WM_Event.h"
#include "ColorDesc.h"
#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
using namespace std;

class GenericBackEnd
{
protected:
	SDL_Event Event;
	bool IsEventWaiting;

	int ScreenWidth, ScreenHeight;


	TColorDesc* BackColor;

	/**
	*	Current used font for rendering text	
	*/
	TTF_Font *CurrentFont;

public:
	GenericBackEnd(void);
	virtual ~GenericBackEnd(void);

	int GetScreenWidth();

	int GetScreenHeight();

	virtual bool HasEventPending();

	virtual void TranslateEvent(WM_Event& WMEvent);

	virtual int StartVideoMode(int Width, int Height, bool FullScreen);

	virtual int TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode);

	//Drawing routines
	/**
	*	Fills a rectangle with a specified color
	*/
	virtual void FillRectangle(int Left, int Top, int Width, int Height, TColorDesc Color);
	/**
	*	Horizontal line
	*/
	virtual void HLine(int Left, int Width, int Top, TColorDesc Color);
	/**
	*	Vertical line
	*/
	virtual void VLine(int Left, int Top, int Height, TColorDesc Color);

	/**
	*	Draws a rectangle
	*/
	virtual void Rectangle(int Left, int Top, int Width, int Height, TColorDesc Desc);

	/**
	*	Says the width in pixels of rendering text
	*/
	virtual int TextOutWidth(std::string Text);

	/**
	*	Display the final result 
	*/
	virtual void Flip(int LeftBorder, int TopBorder, int Border);

	virtual void PaintBackground();

	virtual bool SetCurrentFont(std::string FontName, int FontHeight, int Style);

	virtual void Blit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect);

	/**
	*	Paints a surface in the background and will not be in the zoomable area
	*/
	virtual void BackBlit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect);
	virtual void BackFillColor(TColorDesc* Color);
};

#endif
