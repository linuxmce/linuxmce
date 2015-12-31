/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __RENDERER_IMAGE_H__
#define __RENDERER_IMAGE_H__

#include <string>
using namespace std;

//hack: this should be a generic class!

#ifndef POCKETFROG
	#include <SDL.h>
#else
	#include <PocketFrog.h>
	using namespace Frog;
#endif

class RendererImage
{
	public:
		string m_sFilename;

#ifndef POCKETFROG
	SDL_Surface *m_pSurface;
#else
	Surface *m_pSurface;
#endif

		bool NewSurface;
		
		RendererImage() : m_sFilename(""), m_pSurface(NULL), NewSurface(true) {}
		~RendererImage() 
		{ 
			if (m_pSurface) 
			{
#ifndef POCKETFROG
				SDL_FreeSurface(m_pSurface);
#else
				delete m_pSurface;
#endif
			}
		}
};


#endif //__RENDERER_IMAGE_H__
