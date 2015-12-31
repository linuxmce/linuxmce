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
#ifndef __PROXY_ORBITER_RENDERER_SDL_H__
#define __PROXY_ORBITER_RENDERER_SDL_H__

#include "../Orbiter/SDL/OrbiterRenderer_SDL.h"

class OrbiterRendererFactory;

namespace DCE
{
	class Orbiter;

	class Proxy_OrbiterRenderer_SDL : public OrbiterRenderer_SDL
	{
	private:
		friend class ::OrbiterRendererFactory;
		Proxy_OrbiterRenderer_SDL(Orbiter *pOrbiter);

		bool DisplayProgress(string sMessage, int nProgress);
		bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
		int PromptUser(string sPrompt,int iTimeoutSeconds=10, map<int,string> *p_mapPrompts = NULL);
		int PromptFor(string sToken);

	public:

		~Proxy_OrbiterRenderer_SDL();

		void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);
		void BeginPaint();
		void EndPaint();

		void RefreshScreen(void *data);
		void RenderScreen(bool bRenderGraphicsOnly);
	};
}

#endif // __PROXY_ORBITER_RENDERER_SDL_H__