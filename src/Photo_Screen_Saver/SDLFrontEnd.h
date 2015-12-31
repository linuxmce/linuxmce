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

/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef SDLFrontEnd_H_
#define SDLFrontEnd_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include "ColorDesc.h"

#include "WM_Event.h"
#include <string>

class SDLFrontEnd
{
	SDL_Event Event;
	bool IsEventWaiting;
	/**
	 *	Offscreen surface that does the drawing. At the end you should 
	 *	show the results using Flip() method
	 */
	SDL_Surface* Display;

	TColorDesc* BackColor;

	/**
	 *	Current used font for rendering text	
	 */
	TTF_Font *CurrentFont;
	/**
	 *	Mode = 0 -> defalt align to left
	 *	Mode = 1 -> center align 
	 *	Mode = 2 -> right align 
	 */
	int PaintFont(char* Text, int Top, int Left, 
		TColorDesc Color, int Mode);
	bool NeedUpdateScreen, NeedUpdateBack;

public:

	static bool Quitting;

	int GetScreenWidth();

	int GetScreenHeight();

	/**
	 *	Standard constructor for SDL frontend	
	 */
	SDLFrontEnd();
	virtual ~SDLFrontEnd();
	
	bool HasEventPending();

	SDL_Event& GetLastEvent() { return Event; }
	
	void TranslateEvent(WM_Event& WMEvent);
	
	int StartVideoMode(int Width, int Height, bool FullScreen);

	int TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode);

	//Drawing routines
	/**
	 *	Fills a rectangle with a specified color
	 */
	void FillRectangle(int Left, int Top, int Width, int Height, TColorDesc Color);
	/**
	 *	Horizontal line
	 */
	void HLine(int Left, int Width, int Top, TColorDesc Color);
	/**
	 *	Vertical line
	 */
	void VLine(int Left, int Top, int Height, TColorDesc Color);
	
	/**
	 *	Draws a rectangle
	 */
	void Rectangle(int Left, int Top, int Width, int Height, TColorDesc Desc);

	/**
	 *	Says the width in pixels of rendering text
	 */
	int TextOutWidth(std::string Text);

	/**
	 *	Display the final result 
	 */
	void Flip();

	void PaintBackground();

	bool SetCurrentFont(std::string FontName, int FontHeight, int Style);

	void Blit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect);

	/**
	 *	Paints a surface in the background and will not be in the zoomable area
	 */
	void BackBlit(SDL_Surface* Surface, SDL_Rect SrcRect, SDL_Rect DestRect);
	void BackFillColor(TColorDesc* Color);


};

#endif /*SDLFRONTEND_H_*/
