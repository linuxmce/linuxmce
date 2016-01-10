/*
     Copyright (C) 2010 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef Picture_h
#define Picture_h

using namespace std;

#include <string>
#include "SDL_image.h"

namespace DCE
{
  class Picture
	{
		// Private member variables
	        string m_sFile;
		SDL_Surface *picture;
		SDL_Surface *zoomPicture;
		float m_fViewZoom;
		int m_iCenterX; // center of the viewport, coordinates on zoomPicture
		int m_iCenterY;
		// Private methods
public:
		// Public member variables

public:
		// Constructors/Destructor
		Picture(string file);
		virtual ~Picture();

		bool Load();
		void ExifAdjust();
		int GetWidth();
		int GetHeight();
		SDL_Surface *GetSurface();
		void ZoomToFit(int sw, int sh);
		void Zoom(float zoom);
		void SetViewCenter(int x, int y);
		void GetViewRect(SDL_Rect *rect);
	};
}
#endif
