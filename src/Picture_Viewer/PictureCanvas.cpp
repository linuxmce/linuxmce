/*
     Copyright (C) 2010 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "PictureCanvas.h"
#include "DCE/Logger.h"

#include "SDL.h"

using namespace DCE;
using namespace std;

PictureCanvas::PictureCanvas() : m_ScreenMutex("screen mutex")
{
	m_ScreenMutex.Init(NULL);
        m_iScreenWidth = 0;
	m_iScreenHeight = 0;
	pThisPicture = NULL;
	quit = false;
}

PictureCanvas::~PictureCanvas()
{
        Shutdown();
	if (pThisPicture != NULL) {
	  delete pThisPicture;
	}
}

bool PictureCanvas::Setup(int width, int height, string sWindowName)
{

	unsetenv("SDL_VIDEO_X11_WMCLASS");

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
	        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PictureCanvas::Setup() Unable to set up SDL video : %s", SDL_GetError());
		return false;
	}
	// This is the size of the orbiter, i.e. the display size
	m_iDisplayWidth = width;
	m_iDisplayHeight = height;
	if (!SetScreenSize(width, height)) {
	        return false;
	}
	SDL_WM_SetCaption(sWindowName.c_str(), sWindowName.c_str());

	pthread_create(&ThreadID, NULL, PictureCanvasEventThread, this);

	SDL_WM_IconifyWindow();
	return true;
}

bool PictureCanvas::SetScreenSize(int width, int height)
{
        PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);
        m_iScreenWidth = width;
	m_iScreenHeight = height;
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "PictureCanvas::SetScreenSize() Changing to %d x %d", width, height);
	int flags = SDL_RESIZABLE | SDL_DOUBLEBUF;
	screen = SDL_SetVideoMode(m_iScreenWidth, m_iScreenHeight, 0, flags);
	if (screen == NULL) {
	        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PictureCanvas::SetScreenSize() Unable to set screen size : %s", SDL_GetError());
		return false;
	}

	return true;
}

void *PictureCanvasEventThread(void* pCanvas)
{
        SDL_Event event;
	bool quit = false;
	while (!quit && !((PictureCanvas*)pCanvas)->Quit()) {
	    while (SDL_WaitEvent(&event)) {
		switch (event.type)
		  {
		  case SDL_VIDEORESIZE:
		    ((PictureCanvas*)pCanvas)->SetScreenSize(event.resize.w, event.resize.h);
		    break;
		  case SDL_VIDEOEXPOSE:
		    ((PictureCanvas*)pCanvas)->Update();
		    break;
		  case SDL_QUIT:
		    quit = true;
		    break;
		  }
	    }
	}
	return 0;
}

void PictureCanvas::Shutdown()
{
	quit = true;
        SDL_Quit();
}

bool PictureCanvas::Quit() {
        return quit;
}

void PictureCanvas::Clear()
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "PictureCanvas::Clear() ");
        PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);
        SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = m_iScreenWidth;
	rect.h = m_iScreenHeight;
	SDL_FillRect(screen, &rect, 0);
}

void PictureCanvas::Paint(SDL_Surface *surface, SDL_Rect *source, SDL_Rect *dest)
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "PictureCanvas::Paint() ");
        PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);
        SDL_BlitSurface(surface, source, screen, dest);
}

void PictureCanvas::PaintPicture(Picture *pPicture)
{
	if (pThisPicture != NULL) {
	  delete pThisPicture;
	}
        pThisPicture = pPicture;
        SDL_Surface *picture = pPicture->GetSurface();
	if (!picture)
	        return;
	SDL_SetAlpha(picture, 0, SDL_ALPHA_OPAQUE);
	// get the zoomed picture and display it in the center of the screen
	SDL_Rect rect;
	rect.w = m_iScreenWidth;
	rect.h = m_iScreenHeight;
	pPicture->GetViewRect(&rect);
	
	SDL_Rect dest;
	dest.x = 0;
	dest.y = 0;
	Paint(picture, &rect, &dest);
}

void PictureCanvas::Repaint() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "PictureCanvas::Repaint() ");
	PaintPicture(pThisPicture);
}

void PictureCanvas::Update() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "PictureCanvas::Update() ");

        PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);
	SDL_Flip(screen);
}

int PictureCanvas::GetHeight() {
        return m_iScreenHeight;
}

int PictureCanvas::GetWidth() {
        return m_iScreenWidth;
}
