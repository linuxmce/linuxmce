/*
     Copyright (C) 2010 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef PictureCanvas_h
#define PictureCanvas_h

using namespace std;

#include <string>
#include "SDL_image.h"
#include "../PlutoUtils/MultiThreadIncludes.h"

#include "Picture.h"

namespace DCE
{
  class PictureCanvas
	{
		// Private member variables
		pluto_pthread_mutex_t m_ScreenMutex;
	        SDL_Surface *screen;
		int m_iDisplayWidth, m_iDisplayHeight;
		int m_iScreenWidth, m_iScreenHeight;
		Picture *pThisPicture;

		pthread_t ThreadID;
		bool quit;
		// Private methods
public:
		// Public member variables

public:
		// Constructors/Destructor
	        PictureCanvas();
		virtual ~PictureCanvas();

		bool Setup(int w, int h, string sWindowName);
		bool SetScreenSize(int w, int h);
		void Shutdown();
		bool Quit();

		void PaintPicture(Picture *pPicture);
		void Paint(SDL_Surface *surface, SDL_Rect *source, SDL_Rect *dest);
		void Repaint();
		void Update();
		void Clear();

		int GetHeight();
		int GetWidth();
	};

}
void *PictureCanvasEventThread(void* PictureCanvas);
#endif
