/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#ifndef __ORBITER_RENDERER_LINUX_H__
#define __ORBITER_RENDERER_LINUX_H__

#ifdef ORBITER_OPENGL
	#include "../OpenGL/OrbiterRenderer_OpenGL.h"
#else
	#include "../SDL/OrbiterRenderer_SDL.h"
#endif

#ifndef DIRECTFB
#include <X11/X.h>
#include <X11/Xlib.h>
#include <SDL.h>
#endif

class OrbiterLinux;
class OrbiterRendererFactory;

namespace DCE
{
	class Orbiter;

#ifdef ORBITER_OPENGL
	class OrbiterRenderer_Linux : public OrbiterRenderer_OpenGL	
#else
	class OrbiterRenderer_Linux : public OrbiterRenderer_SDL
#endif
	{
	private:
		friend class ::OrbiterRendererFactory;
		OrbiterRenderer_Linux(Orbiter *pOrbiter);

		void ApplyMasks();

	public:

		~OrbiterRenderer_Linux();

		void RenderScreen(bool bRenderGraphicsOnly);

       	void EventLoop();

		void InitializeAfterSetVideoMode();
		void InitializeAfterRelatives();

		virtual bool DisplayProgress(string sMessage, int nProgress);
		virtual bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
		virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);

		void LockDisplay();
		void UnlockDisplay();

		bool HandleShowPopup(PlutoPopup* Popup, PlutoPoint Position, int EffectID);
		bool HandleHidePopup(PlutoPopup* Popup);

		void ObjectRendered(DesignObj_Orbiter *pObj_Screen, PlutoPoint point);

	private:

		//UI2 - non alpha blending with xshape
#ifndef DIRECTFB
		Pixmap m_screenMaskObjects; //"layer for object non-popup
		Pixmap m_screenMaskPopups;  //"layer" for popups
		Pixmap m_screenMaskCurrent; //the mask to apply
#endif
		bool m_bHasPopups;
		bool m_bScreenRendered;
	};
}
#endif //__ORBITER_RENDERER_SDL_LINUX_H__
