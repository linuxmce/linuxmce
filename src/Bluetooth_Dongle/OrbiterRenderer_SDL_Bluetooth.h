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
#ifndef __ORBITER_RENDERER_SDL_RENDERER_H__
#define __ORBITER_RENDERER_SDL_RENDERER_H__

#include "../Orbiter/SDL/OrbiterRenderer_SDL.h"

class OrbiterRendererFactory;

namespace DCE
{
	class Orbiter;

	class OrbiterRenderer_SDL_Bluetooth : public OrbiterRenderer_SDL
	{
	private:
		friend class ::OrbiterRendererFactory;
		OrbiterRenderer_SDL_Bluetooth(Orbiter *pOrbiter);

		//private data
		std::list<int> m_listLocalRenderedScreens;

	public:

		~OrbiterRenderer_SDL_Bluetooth();

		void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);
		void BeginPaint();
		void EndPaint();
	};
}

#endif // __ORBITER_RENDERER_SDL_RENDERER_H__