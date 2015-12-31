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
#include "DCE/Logger.h"
#ifndef WIN32
#include <GL/glx.h> /* for GLX_RGBA_BIT & GLX_WINDOW_BIT */
#endif
using namespace DCE;

#include <iostream>

#include <SDL_rotozoom.h>

#include <pthread.h>

#include "SDL_syswm.h"

bool SDLFrontEnd::Quitting = false;

void *SDLQuitWatchDogThread(void *)
{
	Sleep(5000);

	if( SDLFrontEnd::Quitting )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SDL_Quit hangs (nvidia drivers bug?). Restarting...");

#ifndef WIN32
		kill(getpid(), SIGKILL);
#endif
	}
	return NULL;
}


SDLFrontEnd::SDLFrontEnd()
	: IsEventWaiting (false)
{
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return;
	} 

	CurrentFont = NULL;

	Display = NULL;
}

SDLFrontEnd::~SDLFrontEnd()
{
	Quitting = true;

	//start watchdog thread
	pthread_t ThreadID = 0;
	pthread_create(&ThreadID, NULL, SDLQuitWatchDogThread, NULL);

	if(CurrentFont!= NULL)
		TTF_CloseFont(CurrentFont);

	TTF_Quit();
	SDL_Quit();

	Quitting = false;
}

bool SDLFrontEnd::HasEventPending()
{
	if (IsEventWaiting)
		return true;

	IsEventWaiting = SDL_PollEvent(&Event) != 0;
	
	return IsEventWaiting;
}

void SDLFrontEnd::TranslateEvent(WM_Event& WMEvent)
{
	if(IsEventWaiting)
	{
		WMEvent.ConvertFromSDLEvent(Event);
		IsEventWaiting = false;
	}
	else
		WMEvent.Type = 0;
}

int SDLFrontEnd::StartVideoMode(int Width, int Height, bool FullScreen)
{
	int Flags = 0;
	if(Display)
		SDL_Quit();

#ifndef WIN32
	//make sure we are not reusing any window
	//let sdl create its own window
	unsetenv("SDL_WINDOWID");
#endif

	//Test if SDL inits nicely
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)== -1)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SDL library cannot init properly!");
		return -1;
	}
		
	if (FullScreen)
		Flags = SDL_FULLSCREEN;

	Flags |=  SDL_OPENGL;
//	Flags |=  SDL_NOFRAME;

	Display = SDL_SetVideoMode(Width, Height, 0, Flags);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting video to w: %d h: %d",Width,Height);
	if(!Display)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot init the video mode! (%s)", SDL_GetError());
		return -1;
	}
	atexit(SDL_Quit);
	
	SDL_WM_SetCaption("PlutoGalleryViewer", "GalleryViewer");

#if !defined(WIN32)
	SDL_SysWMinfo info;

	SDL_VERSION(&info.version);
	if ( SDL_GetWMInfo(&info) ) 
	{
		SDL_bool set;
		Atom WM_HINTS;

		/* We haven't modified the window manager hints yet */
		set = SDL_FALSE;

		Window WMwindow = info.info.x11.wmwindow;

		/* First try to set MWM hints */
		WM_HINTS = XInternAtom(info.info.x11.display, "_MOTIF_WM_HINTS", True);
		if ( WM_HINTS != None ) {
			/* Hints used by Motif compliant window managers */
			struct {
				unsigned long flags;
				unsigned long functions;
				unsigned long decorations;
				long input_mode;
				unsigned long status;
			} MWMHints = { (1L << 1), 0, 0, 0, 0 };

			XChangeProperty(info.info.x11.display, WMwindow,
			                WM_HINTS, WM_HINTS, 32,
			                PropModeReplace,
					(unsigned char *)&MWMHints,
					sizeof(MWMHints)/sizeof(long));
			set = SDL_TRUE;
		}
		/* Now try to set KWM hints */
		WM_HINTS = XInternAtom(info.info.x11.display, "KWM_WIN_DECORATION", True);
		if ( WM_HINTS != None ) {
			long KWMHints = 0;

			XChangeProperty(info.info.x11.display, WMwindow,
			                WM_HINTS, WM_HINTS, 32,
			                PropModeReplace,
					(unsigned char *)&KWMHints,
					sizeof(KWMHints)/sizeof(long));
			set = SDL_TRUE;
		}
		/* Now try to set GNOME hints */
		WM_HINTS = XInternAtom(info.info.x11.display, "_WIN_HINTS", True);
		if ( WM_HINTS != None ) {
			long GNOMEHints = 0;

			XChangeProperty(info.info.x11.display, WMwindow,
			                WM_HINTS, WM_HINTS, 32,
			                PropModeReplace,
					(unsigned char *)&GNOMEHints,
					sizeof(GNOMEHints)/sizeof(long));
			set = SDL_TRUE;
		}
//		/* Finally set the transient hints if necessary */
//		if ( ! set ) {
//			XSetTransientForHint(info.info.x11.display, WMwindow, SDL_Root);
//		}
	} 
#endif
	SDL_ShowCursor(0);

	return 0;
}

void SDLFrontEnd::Flip()
{
	SDL_GL_SwapBuffers();
}

void SDLFrontEnd::PaintBackground()
{
	SDL_FillRect(Display, NULL, 0xffffff);
}

int SDLFrontEnd::TextOutWidth(std::string Text)
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
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SDLFrontEnd::TextOutWidth : TTF_RenderText_Blended crashed!");
		return -1;
	}
	int Result = 0;
	if (RenderedText != NULL)
		Result = RenderedText->w;

	return Result;
}

int SDLFrontEnd::PaintFont(char* Text, 
						   int Top, int Left, 
						   TColorDesc Color,
						   int Mode)
{
	if(CurrentFont == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Warning: there is no font setup for rendering!");
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
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Renderer::RealRenderText : TTF_RenderText_Blended crashed!");
	}


	if (RenderedText == NULL)
	{	
		TTF_Quit();
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Renderer::RealRenderText: Can't render text: %s", Text);
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
	
	//SDL_BlitSurface(RenderedText, NULL, Screen, &SDL_rect);
	try
	{
		SDL_FreeSurface(RenderedText);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Renderer::RealRenderText : SDL_FreeSurface crashed!");
	}
	return Result;
}

int SDLFrontEnd::TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode)
{
	return PaintFont(Text, Top, Left, Color, Mode);
}

bool SDLFrontEnd::SetCurrentFont(std::string FontName, int FontHeight, int Style)
{
	TTF_Font *Font;
	//set up the font
	Font = TTF_OpenFont(FontName.c_str(), FontHeight);

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

int SDLFrontEnd::GetScreenWidth()
{
	return Display->w;
}

int SDLFrontEnd::GetScreenHeight()
{
	return Display->h;
}

